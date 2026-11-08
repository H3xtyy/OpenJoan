#!/usr/bin/env python3
"""The typed table of the replay's image hash (src/runtime/replay/hash_types.inc): which bytes of joan.exe's .data / .bss
(0x9f4000..0xa6d000, the range src/runtime/replay/replay.cpp hashes) hold a pointer, which hold typed non-pointer data and
which are untyped bytes - asked from the compiler (libclang over src/data/layout.hpp for i686, re/image/probe.py,
the same walk as re/image/gen_xlate.py).

    JOAN_HASH_PTR(va, path)            a pointer of the original layout (4 bytes at original address va, any alignment):
                                       a pointer / reference / uptr member, an element of an array of pointers, a
                                       vtable slot (`void* vtable`), a pointer inside a nested object or an array of them
    JOAN_HASH_UNTYPED(va, size, path)  untyped bytes (u8 arrays whose content nobody typed: the gaps of the game's
                                       classes, the tails after a typed object, unreviewed .bss blocks, .data blobs)

`path` is the member designator below joan::data::layout::Image (offsetof(Image, path) == va - 0x9cf000; checked by
re/replay/test_hash_types.cpp). Every other byte is typed non-pointer data (integers, floats, chars, typed u8 arrays,
reviewed byte buffers): the hash compares it as it is. Which u8 arrays are untyped:
  - in a .data chunk (Data_xxxxxx): every u8 array (unknown content of joan.exe's data: blobs, tables, padding);
  - a member of Bss: `*_tail` and the `bss_game` blocks (src/data/bss.inc), except the symbols the image review typed
    `bytes` (re/image/image_types.json: buffers such as the DirectInput buffers, compared as they are);
  - inside an object of a game class: `gap_*`, `*_tail`, `*pad*` members;
  - the gaps of struct Image (gap_*).

    re/replay/gen_hash_types.py [-o src/runtime/replay/hash_types.inc] [--stats]
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, 're', 'image'))
import probe  # noqa: E402
import clang.cindex as ci  # noqa: E402

KVA = 0x9cf000
BEGIN, END = 0x9f4000, 0xa6d000  # .data + .bss (replay.cpp kDataVa / kDataEnd)
BYTE_KINDS = (ci.TypeKind.UCHAR, ci.TypeKind.CHAR_U)
PTR_KINDS = (ci.TypeKind.POINTER, ci.TypeKind.MEMBERPOINTER, ci.TypeKind.LVALUEREFERENCE)


def strip(t):
    while t.kind in (ci.TypeKind.ELABORATED, ci.TypeKind.TYPEDEF):
        t = t.get_declaration().underlying_typedef_type if t.kind == ci.TypeKind.TYPEDEF else t.get_named_type()
    return t


def is_uptr(t):
    s = t.spelling.replace('const ', '')
    return s.endswith('uptr') or s.endswith('uintptr_t')


def bss_untyped_members(data_dir):
    """names of the Bss members that are untyped bytes"""
    reviewed_bytes = set()
    p = os.path.join(ROOT, 're', 'image', 'image_types.json')
    for v in json.load(open(p)).values():
        if v.get('type') == 'bytes':
            reviewed_bytes.add(v['sym'])
    out = set()
    for line in open(os.path.join(data_dir, 'bss.inc')):
        m = re.match(r'\s+u8 (\w+)\[[^\]]+\];(?:\s*//\s*(.*))?$', line)
        if not m:
            continue
        name, comment = m.group(1), m.group(2) or ''
        if name.endswith('_tail') or (comment.endswith('bss_game') and name not in reviewed_bytes):
            out.add(name)
    return out


class Walker:
    def __init__(self, bss_untyped, typed_too=False):
        self.bss_untyped = bss_untyped
        self.typed_too = typed_too  # also list the typed non-pointer members ('T': --classify)
        self.cache = {}  # canonical record spelling -> [(off, 'P'|'U', size, subpath)] inside a game class object

    def class_entries(self, t):
        """entries of an object of a game class (or of an array element), relative to it"""
        key = t.get_canonical().spelling
        if key not in self.cache:
            out = []
            self.member(t, 0, '', 'class', out)
            self.cache[key] = out
        return self.cache[key]

    def member(self, t, base, path, ctx, out):
        """ctx: 'image' (a gap of struct Image), 'data' (a .data chunk), 'bss', 'class' (inside an object)"""
        k = t.get_canonical().kind
        if is_uptr(t) or k in PTR_KINDS:
            if t.get_size() != 4:
                sys.exit(f'gen_hash_types: pointer {path} has {t.get_size()} bytes in the x86 layout')
            out.append((base, 'P', 4, path))
            return
        if k == ci.TypeKind.CONSTANTARRAY:
            at = strip(t)
            e = at.element_type
            name = path.split('.')[-1]
            if e.get_canonical().kind in BYTE_KINDS:
                if self.untyped_bytes(name, ctx):
                    out.append((base, 'U', t.get_size(), path))
                elif self.typed_too:
                    out.append((base, 'T', t.get_size(), path))
                return
            es = e.get_size()
            ek = e.get_canonical().kind
            if ek == ci.TypeKind.RECORD:
                sub = self.class_entries(e)
            elif ek == ci.TypeKind.CONSTANTARRAY or is_uptr(e) or ek in PTR_KINDS:
                sub = []
                self.member(e, 0, '', ctx, sub)
            else:
                if self.typed_too:
                    out.append((base, 'T', t.get_size(), path))
                return  # scalars
            for i in range(at.element_count):
                out.extend((base + i * es + o, kind, size, f'{path}[{i}]{sp}') for o, kind, size, sp in sub)
            return
        if k == ci.TypeKind.RECORD:
            ct = t.get_canonical()
            decl = ct.get_declaration()
            if decl.kind == ci.CursorKind.UNION_DECL:  # (none today) a union is untyped
                out.append((base, 'U', t.get_size(), path))
                return
            if ctx in ('bss', 'data') and path.count('.') >= 1:  # an object in a chunk / in Bss: a game class
                out.extend((base + o, kind, size, path + sp) for o, kind, size, sp in self.class_entries(t))
                return
            own = {f.spelling for f in ct.get_fields()}
            for c in decl.get_children():
                if c.kind == ci.CursorKind.CXX_BASE_SPECIFIER:
                    sub = []
                    self.member(c.type, base, path, ctx, sub)  # bases at offset 0 (single inheritance)
                    # a base member hidden by a member of the same name here (Sprite2D::scene hides Camera::scene):
                    # named through its base ("object|Base|member")
                    bt = c.type.get_canonical().spelling
                    for o, kind, size, p in sub:
                        rest = p[len(path):]
                        if rest.startswith('.') and re.split(r'[.\[]', rest[1:])[0] in own:
                            if '|' in rest:
                                sys.exit(f'gen_hash_types: {p}: a hidden member inside a hidden member')
                            p = f'{path}|{bt}|{rest[1:]}'
                        out.append((o, kind, size, p))
            for f in ct.get_fields():
                if f.is_bitfield():
                    continue
                sub_ctx = ctx
                if ctx == 'image':
                    sub_ctx = 'bss' if f.spelling == 'bss' else 'data' if f.spelling.startswith(('data_', 'rdata_', 'idata')) else 'image'
                self.member(f.type, base + f.get_field_offsetof() // 8,
                            f'{path}.{f.spelling}' if path or ctx == 'class' else f.spelling, sub_ctx, out)

        elif self.typed_too:
            out.append((base, 'T', t.get_size(), path))

    def untyped_bytes(self, name, ctx):
        if ctx == 'image' or ctx == 'data':
            return True
        if ctx == 'bss':
            return name in self.bss_untyped
        return name.startswith('gap_') or name.endswith('_tail') or 'pad' in name


def build(data_dir, typed_too=False):
    text = '#include "%s"\n' % os.path.abspath(os.path.join(data_dir, 'layout.hpp'))
    tu, errs = probe.parse(text, 'x86')
    if errs:
        sys.exit('gen_hash_types: ' + '\n'.join(errs[:10]))
    image = probe.find_records(tu, ['joan::data::layout::Image'])['joan::data::layout::Image']
    if image.get_size() != 0xa1000:
        sys.exit(f'gen_hash_types: sizeof(Image) {image.get_size():#x}, expected 0xa1000')
    w = Walker(bss_untyped_members(data_dir), typed_too)
    # only the top-level members that overlap the hashed range are walked
    entries = []
    for f in image.get_fields():
        off = f.get_field_offsetof() // 8
        if KVA + off + f.type.get_size() <= BEGIN or KVA + off >= END:
            continue
        ctx = 'bss' if f.spelling == 'bss' else 'data' if f.spelling.startswith('data_') else 'image'
        w.member(f.type, off, f.spelling, ctx, entries)
    entries = [(KVA + o, k, s, p) for o, k, s, p in entries if KVA + o >= BEGIN and KVA + o + s <= END]
    entries.sort(key=lambda e: e[0])
    problems = []
    # cross-checks: the pointers are exactly the pointer leaves gen_xlate.py widens on x64 (probe.leaves, a walk of
    # its own), and every pointer of joan.exe's data (data_relocs.inc) in the range is one of them
    ptrs = {va for va, k, _, _ in entries if k == 'P'}
    leaves = {KVA + o for o, _ in probe.leaves(image) if BEGIN <= KVA + o < END}
    for va in sorted(ptrs ^ leaves)[:20]:
        problems.append(f'{va:#x}: pointer here {va in ptrs}, pointer leaf of probe.leaves {va in leaves}')
    for line in open(os.path.join(data_dir, 'data_relocs.inc')):
        m = re.match(r'JOAN_DATA_RELOC\((0x[0-9a-f]+)u', line)
        if m and BEGIN <= int(m.group(1), 16) < END and int(m.group(1), 16) not in ptrs:
            problems.append(f'reloc {m.group(1)} is not a pointer of the table')
    last = BEGIN
    for va, k, s, p in entries:
        if k == 'T':
            continue
        if va < last:
            problems.append(f'{p} at {va:#x} overlaps the entry before')
        last = va + s
    return entries, problems


def text(entries):
    nptr = sum(1 for e in entries if e[1] == 'P')
    unt = [e for e in entries if e[1] == 'U']
    ubytes = sum(e[2] for e in unt)
    L = ['// Generated by re/replay/gen_hash_types.py from src/data/layout.hpp (the compiler\'s x86 layout of the data',
         '// image) - do not edit. The typed image hash of the replay (src/runtime/replay/replay.cpp, typed_hash.hpp) over',
         '// .data + .bss 0x%06x..0x%06x (0x%x bytes): %d pointers (%d bytes), %d untyped ranges (%d bytes), the other' % (
             BEGIN, END, END - BEGIN, nptr, 4 * nptr, len(unt), ubytes),
         '// %d bytes typed non-pointer data. X-macros: define' % (END - BEGIN - 4 * nptr - ubytes),
         '//   JOAN_HASH_PTR(va, path)            a 4-byte pointer at original address va (any alignment)',
         '//   JOAN_HASH_UNTYPED(va, size, path)  untyped bytes [va, va + size)',
         '//   JOAN_HASH_PTR_BASE(va, object, base, member), JOAN_HASH_UNTYPED_BASE(va, size, object, base, member)',
         '//                                      the same for a member of a base class hidden by a member of the same',
         '//                                      name (default: JOAN_HASH_PTR(va, object) / JOAN_HASH_UNTYPED(...))',
         '// before including (path, object: member designators below joan::data::layout::Image; member: below `base`).',
         '// Sorted by va, no overlaps.',
         '#ifndef JOAN_HASH_PTR',
         '#define JOAN_HASH_PTR(va, path)',
         '#define JOAN_HASH_PTR_UNDEF',
         '#endif',
         '#ifndef JOAN_HASH_UNTYPED',
         '#define JOAN_HASH_UNTYPED(va, size, path)',
         '#define JOAN_HASH_UNTYPED_UNDEF',
         '#endif',
         '#ifndef JOAN_HASH_PTR_BASE',
         '#define JOAN_HASH_PTR_BASE(va, object, base, member) JOAN_HASH_PTR(va, object)',
         '#define JOAN_HASH_PTR_BASE_UNDEF',
         '#endif',
         '#ifndef JOAN_HASH_UNTYPED_BASE',
         '#define JOAN_HASH_UNTYPED_BASE(va, size, object, base, member) JOAN_HASH_UNTYPED(va, size, object)',
         '#define JOAN_HASH_UNTYPED_BASE_UNDEF',
         '#endif']
    for va, k, s, p in entries:
        hidden = p.split('|')
        if k == 'P':
            L.append('JOAN_HASH_PTR(0x%06xu, %s)' % (va, p) if len(hidden) == 1 else
                     'JOAN_HASH_PTR_BASE(0x%06xu, %s, ::%s, %s)' % (va, *hidden))
        else:
            L.append('JOAN_HASH_UNTYPED(0x%06xu, 0x%xu, %s)' % (va, s, p) if len(hidden) == 1 else
                     'JOAN_HASH_UNTYPED_BASE(0x%06xu, 0x%xu, %s, ::%s, %s)' % (va, s, *hidden))
    L += ['#ifdef JOAN_HASH_PTR_BASE_UNDEF', '#undef JOAN_HASH_PTR_BASE', '#undef JOAN_HASH_PTR_BASE_UNDEF', '#endif',
          '#ifdef JOAN_HASH_UNTYPED_BASE_UNDEF', '#undef JOAN_HASH_UNTYPED_BASE', '#undef JOAN_HASH_UNTYPED_BASE_UNDEF',
          '#endif',
          '#ifdef JOAN_HASH_PTR_UNDEF', '#undef JOAN_HASH_PTR', '#undef JOAN_HASH_PTR_UNDEF', '#endif',
          '#ifdef JOAN_HASH_UNTYPED_UNDEF', '#undef JOAN_HASH_UNTYPED', '#undef JOAN_HASH_UNTYPED_UNDEF', '#endif']
    return '\n'.join(L) + '\n'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--data', default=os.path.join(probe.SRC, 'data'))
    ap.add_argument('-o')
    ap.add_argument('--classify', nargs='*', help='ranges va+size: print their bytes by class (kDefaultSkip review)')
    a = ap.parse_args()
    entries, problems = build(a.data, a.classify is not None)
    if problems:
        sys.exit('gen_hash_types: ' + '\n  '.join(problems[:30]))
    if a.classify is not None:
        for r in a.classify:
            b, _, n = r.partition('+')
            b = int(b, 16)
            n = int(n, 16) if n else 4
            hit = [e for e in entries if e[0] < b + n and e[0] + e[2] > b]
            typ = sum(min(b + n, va + s) - max(b, va) for va, k, s, _ in hit if k == 'T')
            ptr = sum(min(b + n, va + s) - max(b, va) for va, k, s, _ in hit if k == 'P')
            unt = sum(min(b + n, va + s) - max(b, va) for va, k, s, _ in hit if k == 'U')
            names = [f'{p}@{va:#x}' for va, _, _, p in hit][:6]
            print(f'{b:#010x}+{n:#x}: pointer {ptr}, untyped {unt}, typed {typ}, none {n - ptr - unt - typ}  '
                  f'{" ".join(names)}')
        return
    out = a.o or os.path.join(ROOT, 'src', 'replay', 'hash_types.inc')
    open(out, 'w', newline='\n').write(text(entries))
    nptr = sum(1 for e in entries if e[1] == 'P')
    unt = [e for e in entries if e[1] == 'U']
    print(f'{nptr} pointers, {len(unt)} untyped ranges ({sum(e[2] for e in unt)} bytes) -> {out}')


if __name__ == '__main__':
    main()
