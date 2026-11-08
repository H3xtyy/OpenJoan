#!/usr/bin/env python3
"""Generate joan.exe's data image as typed, named C++ (STANDALONE.md stage S6b; consumer: src/runtime/loader/data_image.cpp).

Usage (from the repository root):
  gen_data_curated.py <joan.exe> <joan.asm> <functions.json> <thunks.json> [hook dir]       generate src/data/
  gen_data_curated.py --check <joan.exe> <joan.asm> <functions.json> <thunks.json> [hook]  generated files up to date?
  gen_data_curated.py --verify <joan.exe> <image.bin> <dead.txt>                           compare a harness dump (the harness is gone, Phase 1: see tests/data_image_reference_test)

Regenerate after any change to src/** that names a new data address (like re/gen_code_slots.py for code): an object
nobody named when the files were generated may have been left out (zero in the image). `--check` exits 1 when the
committed files differ from what the current src/** gives.

The object map (boundaries, classes, readers, status) comes from re/data_classify.py - see its docstring for the
rules. Only objects with status `dead` (provably no reader since S5: EH / SEH / RTTI tables, data only the replaced
CRT / D3DX / libjpeg / zlib code read and our C++ does not reach; anything a game function read stays) are left out; zero padding between
them is left out as well (the image is committed zero-filled, so its bytes are unchanged). Everything else, including
every object whose deadness is uncertain, is emitted at its original offset.

Outputs (src/data/, all generated - do not edit):
  data_common.hpp   element types (u8 .. u64, Guid), JOAN_CODE_PTR / JOAN_DATA_PTR, the dead-range / section tables
  rdata.inc         .rdata 0x9cf000..0x9f3c86 \\ one packed struct per chunk (<= 4 KB / <= 512 members, contiguous
  data.inc          .data  0x9f4000..0xa34000 / emitted objects, split at left-out ranges), a member per object with
                    its value as default member initialiser, static_assert(offsetof) per object
  image.cpp         stage S7b: struct Image = the chunks at their offsets from 0x9cf000 + zero gaps + .bss + .idata
                    (0xa1000 bytes), the one constinit object joan_image_typed (= joan_image, src/runtime/image.hpp)
  dead.cpp          kDeadRanges {va, size, section} (left out: zero, or 0xCD with [Loader] PoisonDead=1), each with
                    its classes and the reason in a comment; kSections: per section its range, typed and dead bytes
                    (the loader checks every dead range lies in its section)
  data_symbols.inc  JOAN_DATA_SYMBOL(section, type, name, address) for every live object (also .bss) - names for S7
  data_relocs.inc   JOAN_DATA_RELOC(address, kind, value) for every pointer dword of the emitted image - S7 rebase
  layout.hpp        stage F1: the chunk structs and struct Image WITHOUT values (names, types, offsets; comments say
                    what an object is, never its value or text) - committed
  bss.inc          docs/X64.md image: struct Bss = .bss as typed members (objects of the game's classes, pointers,
                    scalars; types from re/image/image_types.json, class sizes from the compiler via re/image/probe.py)
  bss_classes.hpp   the class headers bss.inc needs
  image_xlate.inc   the x64 layout of struct Image: original address -> offset runs (re/image/gen_xlate.py, libclang
                    over the files just written for i686 and x86_64); src/runtime/image.hpp, loader/data_image.cpp
Committed files other than rdata.inc / data.inc carry no game text: dead.cpp and data_relocs.inc comments name objects
(member names), not their strings (stage F1).

Member types and values:
  strings      char name[n] = "text" (bytes >= 0x80 and control characters as \\x escapes: the build uses /utf-8)
  float/double exact decimal when a short decimal is the exact value (0.5f, 1080.0f), else a hex-float literal
               (exact by construction: no rounding involved) with the shortest round-trip decimal in a comment;
               infinity and denormals as std::bit_cast from the bit pattern; NaN as u32 / u64 bit pattern (payload
               and sign kept without any compiler float semantics)
  x87 TBYTE    u8[10] (MSVC long double is 64-bit)
  integers     typed by the original access width / the hook's global<T> type; signed in decimal, unsigned in hex
  vtables      u32[] of JOAN_CODE_PTR(thunk), one per line with the replacement's registry name
  pointers     uptr (std::uintptr_t: 4 bytes in this layout, 8 on x64) = JOAN_CODE_PTR / JOAN_DATA_PTR (their
               original values; data_relocs.inc lists them); a dword table is split into runs of u32 and uptr
Layout asserts (offsetof / sizeof against the original addresses) hold for x86 only; on x64 sizeof(Image) is checked
against image_xlate.inc. Needs libclang (pip libclang) and llvm-mingw (/opt/llvm-mingw) for the class sizes.
  GUIDs        Guid {d1, d2, d3, {d4}}
  padding      u8 pad[n] (zero, no initialiser)
  other        u8[] bytes
Names: the hook's own name for the address (kName / g_name / accessor), s_<text> for strings, vt_<Class> for vtables,
flt / dbl / guid / blob ..., always with the address as suffix (unique, searchable).
"""
import binascii
import collections
import decimal
import filecmp
import fractions
import json
import math
import os
import re
import shutil
import struct
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import data_classify as dc  # noqa: E402
from gen_data_image import resources  # noqa: E402

SECTIONS = [('rdata', '.rdata', dc.RDATA[0], dc.RDATA[1]), ('data', '.data', dc.DATA[0], dc.DATA[1])]
CHUNK_BYTES, CHUNK_MEMBERS = 4096, 512
OUT_FILES = ('data_common.hpp', 'bss.inc', 'bss_classes.hpp', 'rdata.inc', 'data.inc', 'image.cpp', 'dead.cpp', 'data_symbols.inc', 'data_relocs.inc',
             'layout.hpp', 'image_xlate.inc')
OLD_FILES = ('rdata.cpp', 'data.cpp')          # before S7b-main (constinit const chunk objects + placement tables)
IMAGE_VA, IMAGE_IDATA, IMAGE_END = 0x9cf000, 0xa6d000, 0xa70000   # joan_image: .rdata start, .idata, end (src/runtime/image.hpp)
IDATA_STRUCT = '''// .idata: the import address table (IAT slots, filled by the loader: src/loader/imports); pointer-sized.
struct Idata {
    uptr slots[0x%x];
};''' % ((0xa70000 - 0xa6d000) // 4)
HEADER = '// Generated by re/gen_data_curated.py from joan.exe and src/** (STANDALONE.md S6b) - do not edit.\n'


# ---------------------------------------------------------------------------------------------------- literals
def comment_text(s):
    """ASCII-only text safe inside a // comment (no line splice, no trigraph)"""
    s = ''.join(c if 0x20 <= ord(c) < 0x7f else '?' for c in s)
    s = s.replace('??', '?.?').rstrip('\\ ')
    return s


def c_string(b, width=96):
    """one or more adjacent string literals for the bytes b (without the terminating NUL)"""
    toks = []
    prev_hex = False
    prev_q = False
    for c in b:
        ch = chr(c)
        if c == 0x5c:
            t = '\\\\'
        elif c == 0x22:
            t = '\\"'
        elif c == 0x0a:
            t = '\\n'
        elif c == 0x0d:
            t = '\\r'
        elif c == 0x09:
            t = '\\t'
        elif c == 0x3f and prev_q:
            t = '\\?'                               # no ?? sequence (trigraph warnings)
        elif 0x20 <= c < 0x7f:
            t = ch
            if prev_hex and ch in '0123456789abcdefABCDEF':
                t = '""' + ch                       # end the preceding \x escape
        else:
            toks.append(('\\x%02x' % c))
            prev_hex, prev_q = True, False
            continue
        toks.append(t)
        prev_hex, prev_q = False, c == 0x3f
    parts, cur = [], ''
    for t in toks:
        if len(cur) + len(t) > width and cur:
            if cur.endswith('""'):
                pass
            parts.append(cur)
            cur = ''
            if t.startswith('""'):
                t = t[2:]
        cur += t
    parts.append(cur)
    return ['"%s"' % p for p in parts]


def exact_decimal(x, sig):
    """shortest decimal (up to sig significant digits) whose value is exactly x, in plain notation, or None"""
    fx = fractions.Fraction(x)
    for p in range(1, sig + 1):
        s = '%.*g' % (p, x)
        if fractions.Fraction(s) == fx:
            d = decimal.Decimal(s)
            if d == d.to_integral_value() and abs(d) < 10 ** sig:
                return '%d.0' % d
            t = format(d, 'f')
            if '.' not in t:
                t += '.0'
            return t if len(t) <= sig + 6 else None
    return None


def shortest_decimal(x, sig, pack):
    for p in range(1, sig + 1):
        s = '%.*g' % (p, x)
        if pack(float(s)) == pack(x):
            return s
    return repr(x)


def pack_f32(v):
    return struct.pack('<f', v)


def pack_f64(v):
    return struct.pack('<d', v)


def hex_float(x, suffix):
    h = float.hex(x)                                # '-0x1.99999a0000000p-4'
    sign = '-' if h.startswith('-') else ''
    m, e = h.lstrip('-')[2:].split('p')
    if '.' in m:
        a, f = m.split('.')
        f = f.rstrip('0')
        m = a + ('.' + f if f else '')
    return '%s0x%sp%s%s' % (sign, m, e, suffix)


def f32_lit(raw):
    """(literal, comment or '') reproducing the float bit pattern raw exactly"""
    x = struct.unpack('<f', struct.pack('<I', raw))[0]
    if math.isnan(x) or math.isinf(x):
        what = 'NaN' if math.isnan(x) else ('-inf' if x < 0 else 'inf')
        return 'std::bit_cast<float>(0x%08xu)' % raw, what
    if raw == 0x80000000:
        return '-0.0f', ''
    if raw & 0x7f800000 == 0 and raw & 0x7fffff:
        return 'std::bit_cast<float>(0x%08xu)' % raw, 'denormal ' + shortest_decimal(x, 9, pack_f32)
    d = exact_decimal(x, 9)
    if d is not None:
        return d + 'f', ''
    return hex_float(x, 'f'), shortest_decimal(x, 9, pack_f32)


def f64_lit(raw):
    x = struct.unpack('<d', struct.pack('<Q', raw))[0]
    if math.isnan(x) or math.isinf(x):
        what = 'NaN' if math.isnan(x) else ('-inf' if x < 0 else 'inf')
        return 'std::bit_cast<double>(0x%016xull)' % raw, what
    if raw == 0x8000000000000000:
        return '-0.0', ''
    if raw & 0x7ff0000000000000 == 0 and raw & 0xfffffffffffff:
        return 'std::bit_cast<double>(0x%016xull)' % raw, 'denormal ' + shortest_decimal(x, 17, pack_f64)
    d = exact_decimal(x, 17)
    if d is not None:
        return d, ''
    return hex_float(x, ''), shortest_decimal(x, 17, pack_f64)


def int_lit(v, t):
    if t in ('u8', 'u16', 'u32', 'u64'):
        return '0x%x' % v + ('ull' if t == 'u64' else '')
    if t == 'i32' and v == -2147483648:
        return '(-2147483647 - 1)'
    return str(v)


ELEM = {'u8': (1, '<B'), 'i8': (1, '<b'), 'u16': (2, '<H'), 'i16': (2, '<h'), 'u32': (4, '<I'), 'i32': (4, '<i'),
        'u64': (8, '<Q'), 'float': (4, '<I'), 'double': (8, '<Q'),
        'uptr': (4, '<I')}  # uptr: a stored pointer (std::uintptr_t: 4 bytes in the original layout, 8 on x64)
HOOK_T = {'int16_t': 'i16', 'uint16_t': 'u16', 'int32_t': 'i32', 'uint32_t': 'u32', 'uint8_t': 'u8', 'int8_t': 'i8',
          'i16': 'i16', 'u16': 'u16', 'i32': 'i32', 'u32': 'u32', 'u8': 'u8', 'i8': 'i8', 'float': 'float',
          'double': 'double', 'int': 'i32', 'unsigned': 'u32', 'short': 'i16', 'char': 'i8', 'bool': 'u8',
          'unsigned char': 'u8', 'unsigned short': 'u16', 'unsigned int': 'u32', 'std::int16_t': 'i16',
          'std::uint16_t': 'u16', 'std::int32_t': 'i32', 'std::uint32_t': 'u32', 'std::uint8_t': 'u8',
          'std::int8_t': 'i8'}
ACC_T = {'int32': 'i32', 'int16': 'i16', 'int8': 'i8', 'uint16': 'u16', 'fnptr': 'u32', 'int64': 'u64',
         'mmx64': 'u64', 'float': 'float', 'double': 'double'}


def cident(s, n=28):
    s = re.sub(r'[^A-Za-z0-9]+', '_', s).strip('_')
    return s[:n].rstrip('_')


class Member:
    __slots__ = ('ctype', 'name', 'dim', 'init', 'comment', 'lines')

    def __init__(self, ctype, name, dim=None, init=None, comment='', lines=None):
        self.ctype, self.name, self.dim, self.init, self.comment, self.lines = ctype, name, dim, init, comment, lines

    def size(self):
        return ELEM.get(self.ctype, (16 if self.ctype == 'Guid' else 1,))[0] * (self.dim or 1) \
            if self.ctype != 'char' else self.dim

    def decl(self):
        return '%s %s%s' % (self.ctype, self.name, '[%d]' % self.dim if self.dim is not None else '')


# ---------------------------------------------------------------------------------------------------- generator
class Gen:
    def __init__(self, m):
        self.m = m
        self.names = set()
        self.name_of = {}         # object start -> name (one per object)
        self.relocs = []          # (address, kind, value, comment)
        self.first_member = {}    # object start -> name of its first member (symbols, offset asserts)
        self.first_type = {}      # object start -> C type of its first member
        self.uses_bit_cast = False
        self.layout_text = {}     # section key -> struct lines for layout.hpp

    # -- names
    def obj_name(self, o):
        if o['start'] in self.name_of:
            return self.name_of[o['start']]
        self.name_of[o['start']] = n = self._obj_name(o)
        return n

    def _obj_name(self, o):
        a, cls = o['start'], o['cls']
        if o.get('name') and re.match(r'^[A-Za-z_]\w*$', o['name']) and cls not in ('crt_init_table', 'padding'):
            base = o['name']
        elif cls in ('string', 'string_8bit') and o.get('_str'):
            base = 's_' + (cident(o['_str'][0].decode('latin1')) or 'empty')
        elif cls in ('vtable', 'fnptr_table'):
            c = (o.get('note') or '').split(' ')[0]
            base = ('vt_' if cls == 'vtable' else 'fns_') + (cident(c) if c and '::' not in c else 'anon')
        elif cls in ('float_const', 'global_float'):
            base = 'flt'
        elif cls in ('double_const', 'global_double'):
            base = 'dbl'
        elif cls in ('ld_const', 'global_long double'):
            base = 'ld'
        elif cls in ('mmx_const', 'global_mmx64'):
            base = 'mmx'
        elif cls == 'guid':
            base = 'guid'
        elif cls.startswith('eh_'):
            base = cls[3:]
        elif cls == 'rtti_type_descriptor':
            base = 'type_descriptor'
        elif cls == 'crt_init_table':
            base = 'xc_table'
        elif cls == 'padding':
            base = 'pad'
        elif cls == 'string_ptr_table':
            base = 'strs'
        elif cls in ('data_ptr_table', 'mixed_ptr_table'):
            base = 'ptrs'
        elif cls == 'blob':
            base = 'blob'
        else:
            base = {'.rdata': 'k', '.data': 'g', '.bss': 'b'}[o['sec']]
        n = '%s_%06x' % (base, a)
        assert n not in self.names, n
        self.names.add(n)
        return n

    def describe(self, o):
        bits = ['0x%06x' % o['start'], o['cls']]
        st = o['status']
        if st == 'live':
            r = self.m.readers(o)
            bits.append(('used by ' + r) if r else ('live: ' + o.get('why', '')))
        elif st != 'padding':
            bits.append('kept (%s): %s' % (st, o.get('why', '')))
        if o.get('note') and o['cls'] not in ('vtable', 'fnptr_table'):
            bits.append(o['note'])
        if o['cls'] in ('vtable', 'fnptr_table') and o.get('note'):
            bits.append('class ' + o['note'].split(' ')[0])
        return comment_text(' | '.join(bits))

    # -- members of one object
    def members(self, o):
        a, sz, cls = o['start'], o['size'], o['cls']
        name = self.obj_name(o)
        b = self.m.bytes_at(a, sz)
        cm = self.describe(o)
        for loc, v in o['ptrs']:
            kind = 'code' if dc.is_code(v) else 'data'
            self.relocs.append((loc, kind, v))
        if not any(b) and cls not in ('vtable',):
            if cls in ('float_const', 'global_float') and sz == 4:
                return [Member('float', name, None, '0.0f', cm)]
            if cls in ('double_const', 'global_double') and sz == 8:
                return [Member('double', name, None, '0.0', cm)]
            return [Member('u8', name, sz, None, cm)]
        if cls in ('string', 'string_8bit') and o.get('_str'):
            return self.string_members(o, name, b, cm)
        if cls in ('float_const', 'global_float') and sz >= 4:
            return self.scalar_members(name, b, 'float', cm)
        if cls in ('double_const', 'global_double') and sz >= 8:
            return self.scalar_members(name, b, 'double', cm)
        if cls in ('ld_const', 'global_long double'):
            return [Member('u8', name, sz, self.byte_list(b), cm + ' | x87 80-bit (MSVC long double is 64-bit)')]
        if cls == 'guid' and sz == 16:
            d1, d2, d3 = struct.unpack_from('<IHH', b)
            return [Member('Guid', name, None, '{0x%08x, 0x%04x, 0x%04x, {%s}}' % (
                d1, d2, d3, ', '.join('0x%02x' % c for c in b[8:])), cm)]
        if o['ptrs'] or cls in ('vtable', 'fnptr_table', 'crt_init_table', 'string_ptr_table', 'data_ptr_table',
                                'mixed_ptr_table', 'seh_scopetable') or cls.startswith('eh_') \
                or cls == 'rtti_type_descriptor':
            if all((loc - a) % 4 == 0 for loc, v in o['ptrs']):
                return self.dword_table(o, name, b, cm)
            return self.unaligned_ptrs(o, name, b, cm)
        t = HOOK_T.get((o.get('type') or '').strip())
        if t is None and o['acc']:
            top = sorted(o['acc'].items(), key=lambda kv: (-kv[1], kv[0]))[0][0]
            t = ACC_T.get(top)
        if t in ('float', 'double'):
            if sz >= ELEM[t][0]:
                return self.scalar_members(name, b, t, cm)
            return [Member('u8', name, sz, self.byte_list(b), cm)]
        if t and sz >= ELEM[t][0]:
            w, fmt = ELEM[t]
            n = sz // w
            vals = [int_lit(struct.unpack_from(fmt, b, w * i)[0], t) for i in range(n)]
            out = [Member(t, name, None if n == 1 else n, vals[0] if n == 1 else None, cm,
                          None if n == 1 else self.wrap(vals, 16 if w <= 2 else 8))]
            if sz - n * w:
                out.append(Member('u8', name + '_tail', sz - n * w, self.byte_list(b[n * w:]), 'rest'))
            return out
        return [Member('u8', name, sz, self.byte_list(b), cm)]

    def string_members(self, o, name, b, cm):
        out = []
        off = 0
        first = True
        for s in o['_str']:
            j = b.find(s + b'\0', off) if s else b.find(b'\0', off)
            if j != off:
                out.append(Member('u8', '%s_p%x' % (name, off), j - off, self.byte_list(b[off:j]), 'padding'))
                off = j
            nm = name if first else '%s_%x' % (name, off)
            # A brace list of character literals, not a string literal: MSVC (VS 2026) leaves a char array default
            # member initialised from a string literal ZERO in these constinit aggregates (found by the loader's CRC
            # check). The text is kept in the comment.
            text = '"%s"' % comment_text(s.decode('latin-1'))
            out.append(Member('char', nm, len(s) + 1, self.char_list(s + b'\0'),
                              (cm if first else 'string (same object)') + ' | ' + text))
            off += len(s) + 1
            first = False
        if off < len(b):
            rest = b[off:]
            out.append(Member('u8', name + '_pad', len(rest), self.byte_list(rest), 'padding'))
        return out

    def scalar_members(self, name, b, t, cm):
        w = ELEM[t][0]
        n = len(b) // w
        lits = []
        raws = [struct.unpack_from('<I' if w == 4 else '<Q', b, w * i)[0] for i in range(n)]
        for raw in raws:
            lit, note = (f32_lit if w == 4 else f64_lit)(raw)
            if lit.startswith('std::bit_cast'):
                self.uses_bit_cast = True
            lits.append((lit, note))
        if any(nt == 'NaN' for lit, nt in lits):
            # NaN: kept as its bit pattern in an integer member - no compiler float semantics involved (payload / sign)
            it = 'u32' if w == 4 else 'u64'
            vals = [int_lit(r, it) + ('  /* %s */' % nt if nt else '') for r, (lit, nt) in zip(raws, lits)]
            if n == 1:
                out = [Member(it, name, None, int_lit(raws[0], it), cm + ' | %s NaN (bit pattern)' % t)]
            else:
                out = [Member(it, name, n, None, cm + ' | %s values as bit patterns (NaN inside)' % t,
                              self.wrap(vals, 4))]
            if len(b) - n * w:
                out.append(Member('u8', name + '_tail', len(b) - n * w, self.byte_list(b[n * w:]), 'rest'))
            return out
        out = []
        if n == 1:
            lit, note = lits[0]
            out.append(Member(t, name, None, lit, cm + (' | ' + note if note else '')))
        else:
            vals = ['%s /* %s */' % (lit, nt) if nt else lit for lit, nt in lits]
            out.append(Member(t, name, n, None, cm, self.wrap(vals, 4 if any(nt for lit, nt in lits) else 8)))
        if len(b) - n * w:
            out.append(Member('u8', name + '_tail', len(b) - n * w, self.byte_list(b[n * w:]), 'rest'))
        return out

    def ptr_comment(self, v):
        if dc.is_code(v):
            return self.m.fn_name(v)
        t = self.m.obj_at(v) if dc.in_image(v) else None
        if t is not None and t.get('_str'):
            s = t['_str'][0][:40].decode('latin1')
            return '"%s"' % s if v == t['start'] else '"%s" + %d' % (s, v - t['start'])
        if t is not None:
            return '0x%06x + %d' % (t['start'], v - t['start']) if v != t['start'] else ''
        return ''

    def public_ptr_comment(self, v):
        """what a stored pointer points at, by name (committed files: no game text)"""
        if dc.is_code(v):
            return self.m.fn_name(v)
        t = self.m.obj_at(v) if dc.in_image(v) else None
        if t is None:
            return ''
        name = self.first_member.get(t['start'])
        if name is None:
            return '0x%06x + %d' % (t['start'], v - t['start']) if v != t['start'] else ''
        return name if v == t['start'] else '%s + %d' % (name, v - t['start'])

    def dword_table(self, o, name, b, cm):
        """dword tables / pointer-holding objects: runs of plain dwords (u32) and runs of stored pointers (uptr: 4
        bytes here, pointer-sized on x64, so the 64-bit layout widens exactly the pointers - docs/X64.md image)"""
        a, sz = o['start'], o['size']
        n = sz // 4
        ptrs = dict(o['ptrs'])
        ents = []
        for i in range(n):
            v = struct.unpack_from('<I', b, 4 * i)[0]
            if a + 4 * i in ptrs:
                ents.append((True, ('JOAN_CODE_PTR(0x%06x)' if dc.is_code(v) else 'JOAN_DATA_PTR(0x%06x)') % v,
                             comment_text(self.ptr_comment(v))))
            else:
                ents.append((False, '0x%x' % v, ''))
        out, i = [], 0
        while i < n:
            j = i
            while j < n and ents[j][0] == ents[i][0]:
                j += 1
            run = ents[i:j]
            is_ptr = run[0][0]
            nm = name if i == 0 else '%s_%x' % (name, 4 * i)
            c0 = cm if i == 0 else ('pointers' if is_ptr else 'dwords') + ' (same object)'
            ty = 'uptr' if is_ptr else 'u32'
            if len(run) == 1:
                out.append(Member(ty, nm, None, run[0][1], c0 + (' | -> ' + run[0][2] if run[0][2] else '')))
            elif is_ptr:
                out.append(Member(ty, nm, len(run), None, c0,
                                  ['%s,%s' % (v, ('  // ' + c) if c else '') for _, v, c in run]))
            else:
                out.append(Member(ty, nm, len(run), None, c0, self.wrap([v for _, v, _ in run], 8)))
            i = j
        if sz - 4 * n:
            out.append(Member('u8', name + '_tail', sz - 4 * n, self.byte_list(b[4 * n:]), 'rest'))
        return out

    def unaligned_ptrs(self, o, name, b, cm):
        """an object with pointers at unaligned offsets (packed records): byte runs and one uptr per pointer"""
        a, sz = o['start'], o['size']
        out, at = [], 0
        for loc, v in sorted(o['ptrs']):
            k = loc - a
            if k > at:
                out.append(Member('u8', name if at == 0 else '%s_%x' % (name, at), k - at, self.byte_list(b[at:k]),
                                  cm + ' | holds unaligned pointers' if at == 0 else 'bytes (same object)'))
            nm = name if k == 0 else '%s_%x' % (name, k)
            c = comment_text(self.ptr_comment(v))
            out.append(Member('uptr', nm, None, ('JOAN_CODE_PTR(0x%06x)' if dc.is_code(v) else 'JOAN_DATA_PTR(0x%06x)') % v,
                              (cm if k == 0 else 'pointer (same object)') + (' | -> ' + c if c else '')))
            at = k + 4
        if at < sz:
            out.append(Member('u8', '%s_%x' % (name, at), sz - at, self.byte_list(b[at:]), 'bytes (same object)'))
        return out

    @staticmethod
    def wrap(vals, per):
        return [', '.join(vals[i:i + per]) + ',' for i in range(0, len(vals), per)]

    def char_list(self, b):
        def lit(c):
            if c == 0x5c:
                return "'\\\\'"
            if c == 0x27:
                return "'\\''"
            if 0x20 <= c < 0x7f:
                return "'%s'" % chr(c)
            return "'\\x%02x'" % c
        toks = [lit(c) for c in b]
        if len(toks) <= 16:
            return '{' + ', '.join(toks) + '}'
        return ['{'] + self.wrap(toks, 16) + ['}']

    def byte_list(self, b):
        if not any(b):
            return None
        if len(b) <= 16:
            return '{' + ', '.join('0x%02x' % c for c in b) + '}'
        return ['{'] + self.wrap(['0x%02x' % c for c in b], 16) + ['}']

    # -- one section
    def chunks(self, sec):
        """[(chunk start, [objects])] - runs of emitted objects, split at left-out ranges and at the size limits"""
        objs = [o for o in self.m.objs if o['sec'] == sec]
        emit = []
        for i, o in enumerate(objs):
            if o['status'] == 'dead':
                emit.append(False)
            elif o['status'] == 'padding':
                p = i - 1
                while p >= 0 and objs[p]['status'] == 'padding':
                    p -= 1
                q = i + 1
                while q < len(objs) and objs[q]['status'] == 'padding':
                    q += 1
                emit.append(p >= 0 and q < len(objs) and objs[p]['status'] != 'dead' and
                            objs[q]['status'] != 'dead')
            else:
                emit.append(True)
        out, cur, cur_lo, nmem = [], [], None, 0
        for o, e in zip(objs, emit):
            if not e:
                if cur:
                    out.append((cur_lo, cur))
                cur, cur_lo, nmem = [], None, 0
                continue
            if cur and (o['end'] - cur_lo > CHUNK_BYTES or nmem >= CHUNK_MEMBERS):
                out.append((cur_lo, cur))
                cur, cur_lo, nmem = [], None, 0
            if not cur:
                cur_lo = o['start']
            cur.append(o)
            nmem += 1
        if cur:
            out.append((cur_lo, cur))
        left_out = [(o, e) for o, e in zip(objs, emit) if not e]
        return out, left_out

    def emit_member(self, w, mb, ind='    '):
        if mb.lines is not None and mb.ctype != 'char' and len(mb.lines) == 1 and '//' not in mb.lines[0] and \
                len(mb.lines[0]) <= 72:
            line = ind + '%s = {%s};' % (mb.decl(), mb.lines[0].rstrip(','))
            w(line + ('  // ' + mb.comment if mb.comment else ''))
            return
        if mb.lines is not None:
            if mb.comment:
                w(ind + '// ' + mb.comment)
            if mb.ctype == 'char':
                w(ind + mb.decl() + ' =')
                for i, ln in enumerate(mb.lines):
                    w(ind + '    ' + ln + (';' if i == len(mb.lines) - 1 else ''))
            else:
                w(ind + mb.decl() + ' = {')
                for ln in mb.lines:
                    w(ind + '    ' + ln)
                w(ind + '};')
            return
        init = mb.init
        if isinstance(init, list):
            if mb.comment:
                w(ind + '// ' + mb.comment)
            w(ind + mb.decl() + ' = {')
            for ln in init[1:-1]:
                w(ind + '    ' + ln)
            w(ind + '};')
            return
        line = ind + mb.decl() + ('' if init is None else ' = ' + init) + ';'
        if mb.comment:
            line += '  // ' + mb.comment
        w(line)

    def section(self, key, sec, lo, hi):
        """rdata.inc / data.inc: the chunk struct definitions of one section (image.cpp lays them out)"""
        chunks, left_out = self.chunks(sec)
        Sec = key.capitalize()
        L = []
        w = L.append
        body = []
        b = body.append
        lay = []                  # layout.hpp: the same structs without values (no game bytes, no game text)
        ly = lay.append
        emitted = 0
        nmembers = 0
        for clo, objs in chunks:
            chi = objs[-1]['end']
            sname = '%s_%06x' % (Sec, clo)
            b('')
            b('// ---------------------------------------------------------------- 0x%06x..0x%06x (%d bytes)' %
              (clo, chi, chi - clo))
            b('struct %s {' % sname)
            ly('')
            ly('// ---------------------------------------------------------------- 0x%06x..0x%06x (%d bytes)' %
               (clo, chi, chi - clo))
            ly('struct %s {' % sname)
            asserts = []
            for o in objs:
                ms = self.members(o)
                self.first_member[o['start']] = ms[0].name
                self.first_type[o['start']] = ms[0].ctype if ms[0].dim is None else '%s[%d]' % (ms[0].ctype, ms[0].dim)
                off = 0
                for i, mb in enumerate(ms):
                    self.emit_member(b, mb)
                    self.emit_layout_member(ly, mb, self.describe(o) if i == 0 else mb.comment.split(' | ')[0])
                    off += mb.size()
                    nmembers += 1
                assert off == o['size'], (hex(o['start']), off, o['size'])
                asserts.append('static_assert(offsetof(%s, %s) == 0x%x);' % (sname, ms[0].name, o['start'] - clo))
            G = '#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout'
            b('};')
            b(G)
            b('static_assert(sizeof(%s) == 0x%x);' % (sname, chi - clo))
            body.extend(asserts)
            b('#endif')
            ly('};')
            ly(G)
            ly('static_assert(sizeof(%s) == 0x%x);' % (sname, chi - clo))
            lay.extend(asserts)
            ly('#endif')
            emitted += chi - clo
        dead = sum(o['size'] for o, e in left_out if o['status'] == 'dead')
        pad = sum(o['size'] for o, e in left_out if o['status'] == 'padding')
        w(HEADER.rstrip('\n'))
        w('// joan.exe %s 0x%06x..0x%06x (%d bytes): %d chunks, %d members, %d bytes emitted; left out: %d bytes of dead' %
          (sec, lo, hi, hi - lo, len(chunks), nmembers, emitted, dead))
        w('// objects (src/data/dead.cpp) and %d bytes of zero padding around them. Each chunk is a packed struct whose' % pad)
        w('// members sit at their original offsets; src/data/image.cpp places chunk 0x%06x.. at offset 0x%06x - 0x%06x' %
          (lo, lo, IMAGE_VA))
        w('// of joan_image. Included by image.cpp only (inside namespace joan::data::layout, #pragma pack(1)).')
        L.extend(body)
        self.layout_text[key] = lay
        return '\n'.join(L) + '\n', chunks, left_out, emitted, dead, nmembers

    def pin_data(self, out_dir, emitted, dead):
        """.data is pinned: the replay hashes it with the types of the layout the recordings were made with, so its member
        boundaries, names and types are the committed ones (data.inc, layout.hpp), never re-derived from votes over src/**
        (which drift with every refactor). Only the `used by` lists are refreshed. Returns (data.inc, chunk bounds, members)."""
        inc = open(os.path.join(out_dir, 'data.inc'), encoding='utf-8').read()
        hpp = open(os.path.join(out_dir, 'layout.hpp'), encoding='utf-8').read()
        head = re.search(r'\.data 0x[0-9a-f]+\.\.0x[0-9a-f]+ \(\d+ bytes\): \d+ chunks, (\d+) members, (\d+) bytes emitted; left out: '
                         r'(\d+) bytes', inc)
        if not head or (int(head.group(2)), int(head.group(3))) != (emitted, dead):
            sys.exit('.data no longer matches the pinned layout (re/replay/frozen_data_dead.txt): %d emitted, %d dead' %
                     (emitted, dead))
        i = hpp.index('\n// ' + '-' * 64 + ' 0x%06x..' % dc.DATA[0])
        j = hpp.index('\n\n#include "bss.inc"')
        pinned = hpp[i:j]
        starts = {o['start']: o for o in self.m.objs if o['sec'] == '.data'}

        def readers(m):
            o = starts.get(int(m.group(2), 16))
            r = self.m.readers(o) if o is not None and o['status'] == 'live' else None
            return m.group(1) + comment_text('used by ' + r) + m.group(3) if r else m.group(0)
        pat = re.compile(r'(// (0x[0-9a-f]{6}) \| \w+ \| )used by [^|\n]*?( \||$)', re.M)
        self.layout_text['data'] = pat.sub(readers, pinned).split('\n')
        bounds = [(int(a, 16), int(b, 16)) for a, b in re.findall(r'^// -{20,} (0x[0-9a-f]+)\.\.(0x[0-9a-f]+) \(', pinned, re.M)]
        # object starts: the offset asserts of the pinned chunks, for pointer comments and data_symbols.inc
        syms = open(os.path.join(out_dir, 'data_symbols.inc'), encoding='utf-8').read()
        self.pinned_symbols = re.findall(r'^JOAN_DATA_SYMBOL\(data, .*$', syms, re.M)
        self.pinned_bss_symbols = re.findall(r'^JOAN_DATA_SYMBOL\(bss, .*$', syms, re.M)
        for m in re.finditer(r'^JOAN_DATA_SYMBOL\(data, (.*), (\w+), 0x([0-9a-f]+)u\)$', syms, re.M):
            self.first_member[int(m.group(3), 16)] = m.group(2)
            self.first_type[int(m.group(3), 16)] = m.group(1)
        return pat.sub(readers, inc), bounds, int(head.group(1))

    @staticmethod
    def emit_layout_member(w, mb, comment):
        """layout.hpp: declaration only; the comment says what the object is, never its value or text"""
        assert '"' not in comment and "'" not in comment, comment
        w('    %s;%s' % (mb.decl(), ('  // ' + comment) if comment else ''))

    @staticmethod
    def image_struct(layout):
        """struct Image (lines) and its offset asserts: the chunks at their offsets, zero gaps, .bss, .idata"""
        L, asserts = ['struct Image {'], []
        at = IMAGE_VA
        for name, typ, lo, hi in layout:
            if lo > at:
                L.append('    u8 gap_%06x[0x%x];' % (at, lo - at))
                asserts.append('static_assert(offsetof(Image, gap_%06x) == 0x%x);' % (at, at - IMAGE_VA))
            assert lo >= at, (hex(lo), hex(at))
            if typ.startswith('u8['):
                L.append('    u8 %s%s;' % (name, typ[2:]))
            else:
                L.append('    %s %s;' % (typ.replace('zero:', ''), name))
            asserts.append('static_assert(offsetof(Image, %s) == 0x%x);' % (name, lo - IMAGE_VA))
            at = hi
        assert at == IMAGE_END, hex(at)
        L.append('};')
        L.append('#pragma pack(pop)')
        # the original layout (x86); on x64 the pointers widen and src/data/image_xlate.inc maps the addresses
        return L + ['#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout',
                    'static_assert(sizeof(Image) == 0x%x);' % (IMAGE_END - IMAGE_VA)] + asserts + [
                    '#elif !defined(JOAN_IMAGE_LAYOUT_PROBE)  // x64: the layout src/data/image_xlate.inc was made from',
                    '#define JOAN_IMAGE_X64_SIZE(size) static_assert(sizeof(Image) == size, '
                    '"src/data/image_xlate.inc is stale (re/gen_data_curated.py)");',
                    '#include "image_xlate.inc"',
                    '#endif']

    def image_file(self, layout):
        """image.cpp: the one object joan_image (.rdata | .data | .bss | .idata at their original offsets)"""
        L = [HEADER.rstrip('\n')]
        w = L.append
        w('// joan.exe\'s data image as ONE object of openjoan.exe (docs/RUNTIME.md, stage S7b): .rdata, .data, .bss')
        w('// and .idata contiguous at their original offsets from 0x%06x (img::kVa). Each typed chunk of rdata.inc /' % IMAGE_VA)
        w('// data.inc is a member at its offset; the ranges between them (dead objects of src/data/dead.cpp, padding) are')
        w('// zero members; .bss and .idata are zero (the loader fills the IAT slots). constinit: the whole object is')
        w('// constant-initialised by the compiler (no dynamic initialiser, nothing runs before the data relocation of')
        w('// src/runtime/loader/data_image.cpp).')
        w('//')
        w('// Other translation units see it as `extern "C" unsigned char joan_image[]` (src/runtime/image.hpp): /alternatename')
        w('// resolves that name to joan_image_typed, so there is one definition and no type clash. Stored pointers keep')
        w('// their original values here (JOAN_DATA_PTR / JOAN_CODE_PTR); data pointers are relocated at startup')
        w('// (data_relocs.inc).')
        w('//')
        w('// The values are the committed typed source rdata.inc / data.inc (src/data/README.md): the')
        w('// only data path; joan.exe is never read.')
        w('#include "data_common.hpp"')
        w('#include "bss_classes.hpp"')
        w('')
        w('namespace joan::data::layout {')
        w('#pragma pack(push, 1)')
        w('#include "rdata.inc"')
        w('#include "data.inc"')
        w('#include "bss.inc"')
        w(IDATA_STRUCT)
        w('')
        L.extend(self.image_struct(layout))
        w('')
        w('}  // namespace joan::data::layout')
        w('')
        w('extern "C" {')
        w('alignas(4096) constinit joan::data::layout::Image joan_image_typed{};')
        w('}')
        w('// src/runtime/image.hpp\'s `extern "C" unsigned char joan_image[]` is this object: MSVC (x86:')
        w('// C names with a leading underscore; x64 without) through /alternatename, GCC / Clang (MinGW, Linux) through an alias.')
        w('#if defined(_MSC_VER) && defined(_M_IX86)')
        w('#pragma comment(linker, "/alternatename:_joan_image=_joan_image_typed")')
        w('#elif defined(_MSC_VER)')
        w('#pragma comment(linker, "/alternatename:joan_image=joan_image_typed")')
        w('#else')
        w('extern "C" unsigned char joan_image[sizeof(joan::data::layout::Image)] __attribute__((alias("joan_image_typed")));')
        w('#endif')
        return '\n'.join(L) + '\n'

    def layout_file(self, layout, stats):
        """layout.hpp: names, types, offsets and sizes of the image (no values) - the typed view of the committed
        rdata.inc / data.inc (ADR-0001)"""
        L = [HEADER.rstrip('\n')]
        w = L.append
        w('// The LAYOUT of joan.exe\'s data image (stage F1, docs/RUNTIME.md): every object of the emitted .rdata / .data')
        w('// as a named, typed member of a packed chunk struct at its original offset, and struct Image (the chunks at')
        w('// their offsets from 0x%06x, zero gaps for dead objects / padding, .bss, .idata; 0x%x bytes). No values: the' %
          (IMAGE_VA, IMAGE_END - IMAGE_VA))
        w('// values are the committed rdata.inc / data.inc, which image.cpp includes into the object joan_image_typed (the')
        w('// same chunk structs with their initialisers; ADR-0001). Comments name what an object is and who reads it - never')
        w('// its value or text. Member names of strings are derived from their text (s_<text>_<address>), like the names')
        w('// in src/data_names.hpp: short identifiers, kept so code and debugger can name the objects.')
        for key, (nch, nmem, emitted, dead, lo, hi) in stats.items():
            w('// %-6s 0x%06x..0x%06x: %d chunks, %d members, %d bytes emitted, %d bytes dead (left out)' %
              ('.' + key, lo, hi, nch, nmem, emitted, dead))
        w('#pragma once')
        w('')
        w('#include <cstddef>')
        w('')
        w('#include "data_common.hpp"')
        w('#include "bss_classes.hpp"')
        w('')
        w('namespace joan::data::layout {')
        w('#pragma pack(push, 1)')
        for key in ('rdata', 'data'):
            L.extend(self.layout_text[key])
        w('')
        w('#include "bss.inc"')
        w(IDATA_STRUCT)
        w('')
        L.extend(self.image_struct(layout))
        w('')
        w('}  // namespace joan::data::layout')
        return '\n'.join(L) + '\n'

    # -- .bss typed (docs/X64.md image): objects of their classes, pointers, scalars (re/image/image_types.json)
    def bss_struct(self):
        """bss.inc lines (struct Bss: .bss 0xa34000..IMAGE_IDATA as typed members), the class headers it needs,
        and a report. Types: re/image/image_types.json (the image review, keyed by address); class sizes from the
        compiler (re/image/probe.py, i686). The original layout is asserted member by member (x86 only)."""
        sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'image'))
        import probe  # libclang (re/image/probe.py)
        types = {int(k, 16): v for k, v in json.load(open(os.path.join(
            os.path.dirname(os.path.abspath(__file__)), 'image', 'image_types.json'))).items()}
        objs = [o for o in self.m.objs if o['sec'] == '.bss']
        classes = sorted({v['type'][4:] for v in types.values() if v.get('type', '')[:4] in ('obj:', 'ptr:')} -
                         {'void', 'char', 'fn'})
        hdr = probe.class_headers(classes)
        headers = sorted({h for h in hdr.values() if h})
        size86 = probe.sizes(classes, headers, 'x86')
        scalar = {'u8': 'u8', 'i8': 'i8', 'char': 'char', 'u16': 'u16', 'i16': 'i16', 'u32': 'u32', 'i32': 'i32',
                  'f32': 'float', 'f64': 'double'}
        ssize = {'u8': 1, 'i8': 1, 'char': 1, 'u16': 2, 'i16': 2, 'u32': 4, 'i32': 4, 'f32': 4, 'f64': 8}
        lo, hi = dc.BSS[0], IMAGE_IDATA
        L, asserts, rep = ['struct Bss {'], [], collections.Counter()
        notes = []
        at, i = lo, 0
        used_headers = set()

        def pad(a, b, name):
            if b > a:
                L.append('    u8 %s[0x%x];' % (name, b - a))

        while i < len(objs):
            o = objs[i]
            a = o['start']
            name = self.obj_name(o)
            v = types.get(a, {})
            ty = v.get('type', '')
            cnt = int(v.get('count', 1) or 1)
            decl, size = None, 0
            if 'part_of' in v and a > at - 1 and not ty:
                ty = ''
            if ty.startswith('obj:') and size86.get(ty[4:]):
                size = size86[ty[4:]] * cnt
                decl = '::joan::orig::%s %s%s' % (ty[4:], name, '[%d]' % cnt if cnt > 1 else '')
                used_headers.add(hdr[ty[4:]])
                rep['class objects'] += 1
            elif ty.startswith('ptr:'):
                c = ty[4:]
                pt = {'void': 'void*', 'char': 'char*', 'fn': 'uptr'}.get(c)
                if pt is None:
                    pt = '::joan::orig::%s*' % c
                    used_headers.add(hdr[c])
                size = 4 * cnt
                decl = '%s %s%s' % (pt, name, '[%d]' % cnt if cnt > 1 else '')
                rep['pointers'] += 1
            elif ty in scalar:
                size = ssize[ty] * cnt
                decl = '%s %s%s' % (scalar[ty], name, '[%d]' % cnt if cnt > 1 else '')
                rep['scalars'] += 1
            # the members it covers: following objects inside [a, a + size)
            j = i + 1
            if decl:
                while j < len(objs) and objs[j]['start'] < a + size:
                    w = types.get(objs[j]['start'], {})
                    if 'part_of' not in w and w.get('type'):
                        notes.append('%s %s x%d (0x%x bytes) runs into the typed symbol at 0x%06x: left as bytes' %
                                     (name, ty, cnt, size, objs[j]['start']))
                        decl = None
                        break
                    j += 1
                if decl and objs[j - 1]['end'] < a + size:
                    end_bss = objs[-1]['end']
                    if a + size > end_bss:
                        notes.append('%s %s x%d runs past .bss: left as bytes' % (name, ty, cnt))
                        decl = None
            if not decl:
                if ty and ty != 'bytes':
                    rep['typed in the review but not placeable'] += 1
                elif ty == 'bytes':
                    rep['raw bytes (review)'] += 1
                elif 'part_of' in v:
                    rep['part_of outside its owner'] += 1
                else:
                    rep['not reviewed (bytes)'] += 1
                L.append('    u8 %s[0x%x];  // 0x%06x %s' % (name, o['size'], a, o['cls']))
                asserts.append('static_assert(offsetof(Bss, %s) == 0x%x);' % (name, a - lo))
                at = o['end']
                i += 1
                continue
            L.append('    %s;  // 0x%06x %s%s' % (decl, a, ty, (' x%d' % cnt) if cnt > 1 else ''))
            asserts.append('static_assert(offsetof(Bss, %s) == 0x%x);' % (name, a - lo))
            end = max(objs[j - 1]['end'], a + size)
            pad(a + size, end, '%s_tail' % name)
            at = end
            i = j
        pad(at, hi, 'bss_end_%06x' % at)
        L.append('};')
        L.append('#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout')
        L.append('static_assert(sizeof(Bss) == 0x%x);' % (hi - lo))
        L.extend(asserts)
        L.append('#endif')
        return L, sorted(h for h in used_headers if h), rep, notes

    # -- everything
    def run(self, out_dir):
        files = {}
        stats = {}
        dead_rows = []
        layout = []
        for key, sec, lo, hi in SECTIONS:
            text, chunks, left_out, emitted, dead, nmem = self.section(key, sec, lo, hi)
            bounds = [(clo, objs[-1]['end']) for clo, objs in chunks]
            if key == 'data':
                text, bounds, nmem = self.pin_data(out_dir, emitted, dead)
            files['%s.inc' % key] = text
            for clo, chi in bounds:
                layout.append(('%s_%06x' % (key, clo), '%s_%06x' % (key.capitalize(), clo), clo, chi))
            stats[key] = (len(chunks), nmem, emitted, dead, lo, hi)
            # merge adjacent dead objects (padding between them stays zero, not poisoned)
            cur = None
            for o, e in left_out:
                if o['status'] != 'dead':
                    continue
                if cur and cur['end'] == o['start']:
                    cur['end'] = o['end']
                    cur['objs'].append(o)
                else:
                    cur = {'start': o['start'], 'end': o['end'], 'sec': sec, 'objs': [o]}
                    dead_rows.append(cur)
        layout.append(('bss', 'zero:Bss', dc.DATA[1], IMAGE_IDATA))
        layout.append(('idata', 'zero:Idata', IMAGE_IDATA, IMAGE_END))
        bss_lines, bss_headers, self.bss_report, self.bss_notes = self.bss_struct()
        # .bss is pinned like .data (pin_data): the replay hashes it
        files['bss.inc'] = open(os.path.join(out_dir, 'bss.inc'), encoding='utf-8').read()
        files['bss_classes.hpp'] = self.bss_classes_file(bss_headers)
        files['image.cpp'] = self.image_file(layout)
        files['layout.hpp'] = self.layout_file(layout, stats)
        files['dead.cpp'] = self.dead_file(dead_rows, stats)
        files['data_common.hpp'] = self.common()
        files['data_symbols.inc'] = self.symbols()
        files['data_relocs.inc'] = self.relocs_file()
        os.makedirs(out_dir, exist_ok=True)
        for name in OLD_FILES:
            if os.path.exists(os.path.join(out_dir, name)):
                os.remove(os.path.join(out_dir, name))
        for name, text in files.items():
            with open(os.path.join(out_dir, name), 'w', newline='\n') as f:
                f.write(text)
        # the x64 layout of the image, as the compiler lays out the files just written (re/image/gen_xlate.py)
        import gen_xlate
        s86, s64, runs, nleaves, problems = gen_xlate.build(out_dir)
        if problems:
            sys.exit('image_xlate: ' + '\n  '.join(problems[:30]))
        with open(os.path.join(out_dir, 'image_xlate.inc'), 'w', newline='\n') as f:
            f.write(gen_xlate.text(s86, s64, runs, nleaves))
        return stats, dead_rows

    @staticmethod
    def bss_file(lines):
        return HEADER + '''// .bss of joan.exe's data image as typed members (docs/X64.md image): the objects of the game's classes
// (src/original/*), pointers as pointers, scalars, at their original offsets in the x86 layout; on x64 the class
// layouts and pointers widen and src/data/image_xlate.inc maps the original addresses. Types: the image review
// (re/image/spec, re/image/image_types.json); symbols nobody reviewed are bytes. Zero (no initial value). Included by
// layout.hpp and image.cpp inside namespace joan::data::layout under #pragma pack(1).
''' + '\n'.join(lines) + '\n'

    @staticmethod
    def bss_classes_file(headers):
        return HEADER + '''// The class headers struct Bss (bss.inc) needs.
#pragma once
#include <cstdint>

''' + ''.join('#include "../%s"\n' % h for h in headers)

    def dead_file(self, rows, stats):
        L = [HEADER.rstrip('\n')]
        w = L.append
        tot = sum(r['end'] - r['start'] for r in rows)
        nz = sum(sum(1 for c in self.m.bytes_at(r['start'], r['end'] - r['start']) if c) for r in rows)
        nobj = sum(len(r['objs']) for r in rows)
        w('// Ranges of joan.exe\'s .rdata / .data that are NOT emitted: %d ranges, %d objects, %d bytes (%d of them nonzero' %
          (len(rows), nobj, tot, nz))
        w('// in joan.exe). They stay zero in the built image, or 0xCD with [Loader] PoisonDead=1 (runtime proof that nothing')
        w('// reads them). Why an object is dead: re/data_classify.py (status `dead`). Per range: classes, original readers.')
        cls_tot = collections.Counter()
        why_tot = collections.Counter()
        for r in rows:
            for o in r['objs']:
                cls_tot[o['cls']] += o['size']
                why_tot[o['why'].split(' (')[0]] += o['size']
        w('//')
        w('// By reason:')
        for k, v in sorted(why_tot.items(), key=lambda kv: (-kv[1], kv[0])):
            w('//   %7d bytes  %s' % (v, comment_text(k)))
        w('// By class:')
        for k, v in sorted(cls_tot.items(), key=lambda kv: (-kv[1], kv[0])):
            w('//   %7d bytes  %s' % (v, k))
        w('#include "data_common.hpp"')
        w('')
        w('namespace joan::data {')
        w('')
        w('const DeadRange kDeadRanges[] = {')
        for r in rows:
            cls = collections.Counter(o['cls'] for o in r['objs'])
            owners = sorted({o['owner'] or '-' for o in r['objs']})
            desc = ', '.join('%s%s' % (k, (' x%d' % v) if v > 1 else '') for k, v in
                             sorted(cls.items(), key=lambda kv: (-kv[1], kv[0])))
            extra = ''                                  # committed file: no game text (stage F1)
            w('    {0x%06xu, %du},  // %s: %s | read by %s%s' % (
                r['start'], r['end'] - r['start'], r['sec'], comment_text(desc), '/'.join(owners),
                comment_text(extra)))
        w('};')
        w('const std::size_t kDeadRangeCount = sizeof kDeadRanges / sizeof kDeadRanges[0];')
        w('')
        w('// The initialised sections of the image: name, original range, bytes of typed members, bytes of dead ranges left')
        w('// out (the loader checks that every dead range lies inside its section and logs these figures).')
        w('const Section kSections[] = {')
        for key, sec, lo, hi in SECTIONS:
            emitted, dead = stats[key][2], stats[key][3]
            w('    {"%s", 0x%06xu, 0x%xu, %du, %du},' % (sec, lo, hi - lo, emitted, dead))
        w('};')
        w('const std::size_t kSectionCount = sizeof kSections / sizeof kSections[0];')
        w('')
        w('}  // namespace joan::data')
        return '\n'.join(L) + '\n'

    def common(self):
        return HEADER + r'''// Shared declarations of the typed data image (src/data/image.cpp, dead.cpp; consumer src/runtime/loader/data_image.cpp).
// No Windows headers: tests/data_image_reference_test.cpp compiles the same files without the game.
#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>

namespace joan::data {

using u8 = std::uint8_t;
using i8 = std::int8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
// A stored pointer of the image (data_relocs.inc): 4 bytes in the original (x86) layout, pointer-sized on x64.
using uptr = std::uintptr_t;
static_assert(sizeof(uptr) == sizeof(void*));

struct Guid {
    u32 d1;
    u16 d2, d3;
    u8 d4[8];
};
static_assert(sizeof(Guid) == 16);
static_assert(sizeof(float) == 4 && sizeof(double) == 8);

/// A range left out of the image (dead objects): zero, or 0xCD with [Loader] PoisonDead=1.
struct DeadRange {
    u32 va;
    u32 size;
};
/// A section of the emitted image: name, original range, bytes of typed members, bytes of the dead ranges left out.
struct Section {
    const char* name;
    u32 va;
    u32 size;
    u32 emitted_bytes;
    u32 dead_bytes;
};

extern const DeadRange kDeadRanges[];
extern const std::size_t kDeadRangeCount;
extern const Section kSections[];
extern const std::size_t kSectionCount;

}  // namespace joan::data

// Stored pointers are written with their original values: a code pointer is the original thunk / function address
// (S7b: the generated code range still leads it to our C++), a data pointer an original address inside the image,
// which src/runtime/loader/data_image.cpp relocates at startup to where joan_image is (data_relocs.inc).
#define JOAN_CODE_PTR(addr) (addr)
#define JOAN_DATA_PTR(addr) (addr)
'''

    def symbols(self):
        L = [HEADER.rstrip('\n'),
             '// Every live object of joan.exe\'s data image with its C type, name and address (stage S7: names instead of',
             '// numbers). X-macro: define JOAN_DATA_SYMBOL(section, type, name, address) before including; `type` may be an',
             '// array type. .rdata / .data names are the member names in rdata.inc / data.inc; .bss has no initial value.',
             '#ifndef JOAN_DATA_SYMBOL',
             '#error define JOAN_DATA_SYMBOL(section, type, name, address) before including data_symbols.inc',
             '#endif']
        L.extend(self.rdata_symbols())
        L.extend(self.pinned_symbols)  # .data and .bss: the committed objects (pin_data)
        L.extend(self.pinned_bss_symbols)
        return '\n'.join(L) + '\n'

    def rdata_symbols(self):
        return ['JOAN_DATA_SYMBOL(rdata, %s, %s, 0x%06xu)' % (self.first_type[o['start']], self.first_member[o['start']], o['start'])
                for o in self.m.objs if o['sec'] == '.rdata' and o['status'] == 'live']

    def relocs_file(self):
        rows = sorted(set(self.relocs))
        L = [HEADER.rstrip('\n'),
             '// Every pointer dword of the emitted .rdata / .data (relocated in joan.exe): address, kind (code: thunk / function',
             '// address, data: address inside the image), original value. Stage S7 rebases them. X-macro: define',
             '// JOAN_DATA_RELOC(address, kind, value) before including; kind is the token code or data.',
             '#ifndef JOAN_DATA_RELOC',
             '#error define JOAN_DATA_RELOC(address, kind, value) before including data_relocs.inc',
             '#endif']
        for a, k, v in rows:
            c = self.public_ptr_comment(v)
            L.append('JOAN_DATA_RELOC(0x%06xu, %s, 0x%06xu)%s' % (a, k, v, ('  // ' + comment_text(c)) if c else ''))
        return '\n'.join(L) + '\n'


def generate(exe, asm, fj, tj, hook, out_dir):
    m = dc.DataMap(exe, asm, fj, tj, hook)
    g = Gen(m)
    stats, dead_rows = g.run(out_dir)
    for f in OUT_FILES:
        assert os.path.exists(os.path.join(out_dir, f)), f
    return m, g, stats, dead_rows


def report(m, stats, dead_rows):
    for key, (nch, nmem, emitted, dead, lo, hi) in stats.items():
        print('%-6s 0x%06x..0x%06x %6d bytes: %4d chunks, %5d members, %6d bytes emitted, %5d bytes dead (left out)' %
              (key, lo, hi, hi - lo, nch, nmem, emitted, dead))
    tot = sum(r['end'] - r['start'] for r in dead_rows)
    nobj = sum(len(r['objs']) for r in dead_rows)
    nz = sum(sum(1 for c in m.bytes_at(r['start'], r['end'] - r['start']) if c) for r in dead_rows)
    print('dead: %d ranges, %d objects, %d bytes (%d nonzero in joan.exe)' % (len(dead_rows), nobj, tot, nz))
    st = collections.Counter()
    for o in m.objs:
        if o['sec'] != '.bss':
            st[o['status']] += o['size']
    print('status bytes (.rdata + .data): ' + ', '.join('%s %d' % kv for kv in sorted(st.items())))


def verify(exe, image_bin, dead_txt):
    """image.bin = the harness's image (0x00400000..), dead.txt = 'va size' lines it printed from kDeadRanges"""
    from pe import PE
    p = PE(exe)
    got = open(image_bin, 'rb').read()
    dead = []
    for ln in open(dead_txt):
        if ln.strip():
            a, n = ln.split()
            dead.append((int(a, 16), int(n)))
    ok = True
    base = 0x400000
    in_dead = bytearray(len(got))
    for a, n in dead:
        in_dead[a - base:a - base + n] = b'\1' * n
    for key, sec, lo, hi in SECTIONS:
        o = p.va2off(lo)
        ref = p.data[o:o + (hi - lo)]
        diff = 0
        nzdead = 0
        omitted = 0
        poison = 0
        for i in range(hi - lo):
            g = got[lo - base + i]
            if in_dead[lo - base + i]:
                omitted += 1
                nzdead += ref[i] != 0
                if g not in (0, 0xcd):
                    diff += 1
                poison += g == 0xcd
            elif g != ref[i]:
                diff += 1
        print('%-7s 0x%06x..0x%06x: %d bytes outside dead ranges %s joan.exe; dead ranges %d bytes (%d nonzero in '
              'joan.exe) are %s' % (sec, lo, hi, hi - lo - omitted, 'IDENTICAL to' if diff == 0 else
                                    'DIFFER (%d) from' % diff, omitted, nzdead,
                                    'poisoned 0xCD' if poison == omitted and omitted else 'zero'))
        ok &= diff == 0
    print('RESULT: %s' % ('OK' if ok else 'MISMATCH'))
    return ok


def main():
    a = sys.argv[1:]
    if a and a[0] == '--verify':
        sys.exit(0 if verify(a[1], a[2], a[3]) else 1)
    check = bool(a) and a[0] == '--check'
    if check:
        a = a[1:]
    if len(a) < 4:
        sys.exit(__doc__)
    hook = a[4] if len(a) > 4 else os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'src')
    out_dir = os.path.join(hook, 'data')
    if check:
        tmp = tempfile.mkdtemp(prefix='joan_data_')
        try:
            generate(a[0], a[1], a[2], a[3], hook, tmp)
            stale = [f for f in OUT_FILES if not os.path.exists(os.path.join(out_dir, f)) or
                     not filecmp.cmp(os.path.join(tmp, f), os.path.join(out_dir, f), shallow=False)]
            stale += [f + ' (obsolete)' for f in OLD_FILES if os.path.exists(os.path.join(out_dir, f))]
        finally:
            shutil.rmtree(tmp)
        print('src/data: %s' % ('up to date' if not stale else 'STALE: ' + ', '.join(stale)))
        sys.exit(1 if stale else 0)
    m, g, stats, dead_rows = generate(a[0], a[1], a[2], a[3], hook, out_dir)
    report(m, stats, dead_rows)
    print('relocs %d, bit_cast literals: %s' % (len(set(g.relocs)), g.uses_bit_cast))


if __name__ == '__main__':
    main()
