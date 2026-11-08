#!/usr/bin/env python3
"""Object map of joan.exe's data image (.rdata 0x9cf000..0x9f3c86, .data 0x9f4000..0xa34000, .bss ..0xa6cd44) for the
typed data generator re/gen_data_curated.py (STANDALONE.md stage S6b). Library module plus a small CLI.

What it does
1. References into the image:
   - original code: every relocated operand of .text that points into the image, with the instruction text
     (joan.asm) and the containing function (functions.json);
   - data: every relocated dword stored in .rdata / .data / .idata / .rsrc (pointers held by the image);
   - our C++: every 6..8 digit hex constant in src/** (comments and string literals stripped) with the helper that
     wraps it (`global<T>(`, `str(`, `obj(` ...) and that helper's template argument. Generated tables are skipped
     (code_slots.inc, import_table.inc, image_layout.inc, fn_names.hpp, data_names.hpp, and
     src/data/: the generator's own output; src/runtime/image.hpp: the image range itself). A name of src/data_names.hpp
     (`d::Name`, re/gen_data_names.py) or src/fn_names.hpp (`fn::Name`) counts exactly like the literal it stands for
     (the source is scanned with the names expanded). The S7b-prep forms read like the casts they replaced:
     `&img::global<T>(a)` / `img::obj(a)` / `img::addr(a)` are qualified helpers, never the scalar `global<T>(a)`.
2. Objects: a new object starts at every referenced address, at the structures found by the EH / RTTI parsers
   (FuncInfo 0x19930520, unwind / try-block / handler maps, CatchableType(Array), ThrowInfo, type descriptors), at
   the CRT initialiser table, and at the natural end of an obviously shorter item (a string, a scalar FP constant, a
   run of code pointers, a zero run). Not a start of its own (merged into the object it belongs to):
   - an address the original only reaches as a later piece of an inline copy: one function reading .rdata / .data
     with consecutive absolute operands (mov eax,[a]; mov ecx,[a+4]; mov dx,[a+8]: inlined strcpy / memcpy / struct
     copy) - the run [a, a+10) is one object;
   - an address inside a string whose first piece has no NUL: the string runs to its terminator (+ zero padding).
   Addresses our C++ names, pointer targets and EH / RTTI structures always stay starts.
3. Class per object (string, float_const, vtable, eh_funcinfo, blob, global_init, padding, ...) and the region of
   its original readers (game / crt / d3dx / lib / eh_funclet; joan-re/re/functions.csv).
4. Status per object, since stage S5 only our C++ runs:
   live      src/** names its start, or a kept object holds a relocated pointer to it (closure)
   tail      not named, but directly after a live object that the original indexed or that our C++ reads opaquely
             (obj(), memcpy, pointer arithmetic, array types ...): may be part of it. The run of tails ends at the
             next live object, at an object read by other original code (another compiland's variable), and in
             .rdata at a string / vtable / GUID / padding or a class change
   head      directly before a live object that the original indexed or our C++ reads below (base[-1], a - n):
             a negative index / bias may reach it
   unproven  not provably dead (kept): anything with an original GAME-code reader, anything our C++ names an
             address inside of, anything within NEAR bytes after an address our C++ computes with, pointer targets
             of every kept object, and runs of otherwise dead objects wedged between two kept objects with no padding
             gap (the same compiland's data)
   dead      only these: (a) EH / SEH / RTTI tables (only original frames and funclets use them; none exist since
             S5), (b) objects that only the replaced library code (CRT / D3DX / libjpeg / libpng / zlib, 0x763d00..
             0x7dd410) reads, incl. MMX constants, (c) objects only pointed to by such dead objects - and in every
             case not named / not reached by our C++ (see unproven)
   Only `dead` objects are left out of the generated image.

CLI: data_classify.py <joan.exe> <joan.asm> <functions.json> <thunks.json> [hook dir] [--tsv out.tsv]
     prints the totals per class / status and optionally writes one row per object.
"""
import bisect
import collections
import csv
import glob
import json
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pe import PE  # noqa: E402
import fn_scan  # noqa: E402

RE_DIR = os.path.dirname(os.path.abspath(__file__))
RDATA = (0x9cf000, 0x9f3c86)
DATA = (0x9f4000, 0xa34000)
BSS = (0xa34000, 0xa6cd44)
SECS = [('.rdata',) + RDATA, ('.data',) + DATA, ('.bss',) + BSS]
LO, HI = RDATA[0], BSS[1]
THUNK0, NTHUNK = 0x401005, 8015
CODE_HI = 0x7e50a0                     # end of real code (game, libraries, EH funclets); above: dead data in .text
CRT_XC = (0x9f4000, 0x9f4b88)          # __xc_a..__xc_z (crt/startup walks it by address)
SKIP_FILES = {'code_map.inc', 'code_slots.inc', 'census_table.inc', 'data_image.inc', 'import_table.inc', 'image_layout.inc',
              'fn_names.hpp', 'data_names.hpp',
              # the replay's typed-hash table: ~4,000 addresses of the image, each a description of the layout, not a use
              'hash_types.inc'}
SKIP_PATHS = {'runtime/image.hpp', 's7_probe.cpp', 'runtime/replay/x86_view.hpp', 'runtime/replay/hash_types_check.cpp',
              'runtime/loader/data_image.hpp', 'runtime/loader/data_image.cpp'}  # the image range itself (kVa / kEnd); the S7b build probe
SKIP_DIRS = ('data' + os.sep,)         # src/data/: generated by gen_data_curated.py
LIB_OWNERS = {'crt', 'd3dx', 'lib', 'eh', 'eh_funclet'}
KEPT = ('live', 'tail', 'head', 'unproven')
NEAR = 0x100                           # bytes after a base address our C++ computes with that it may reach
LEAF_CLASSES = {'string', 'string_8bit', 'float_const', 'double_const', 'ld_const', 'mmx_const', 'int_const', 'guid',
                'wstring'}
EH_CLASSES = {'eh_funcinfo', 'eh_unwindmap', 'eh_tryblockmap', 'eh_handlertype', 'eh_catchable_type',
              'eh_catchable_type_array', 'eh_throwinfo', 'rtti_type_descriptor', 'seh_scopetable'}

STRIP = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])\'', re.S)
HEX = re.compile(r'\b0x([0-9a-fA-F]{6,8})(?:[uUlL]{0,3})\b')
HELPER = re.compile(r'([A-Za-z_][\w:]*)\s*(<(?:[^<>;]|<[^<>;]*>)*>)?\s*\(\s*$')
REG_RE = re.compile(r'\b[A-Za-z_]\w*\s*\(\s*(?:[A-Za-z_]\w*\s*,\s*)?(?:"([^"]*)"\s*,\s*)?"([^"]*)"\s*,\s*'
                    r'(?:[A-Za-z_][\w:]*\s*,\s*)?'
                    r'0x([0-9a-fA-F]{6,8})')
MOD_RE = re.compile(r'JOAN_REPLACE\w*\s*\(\s*"([^"]+)"')
WRAP_RE = re.compile(r'\bW2?\s*\(\s*0x([0-9a-fA-F]{6,8})\s*,\s*0x([0-9a-fA-F]{6,8})\s*[,)]')
ASM_SCALAR = re.compile(r'\b(byte|word|dword|qword|tbyte)\s+ptr\s+(?:ds:)?\[\s*0x[0-9a-fA-F]+\s*\]', re.I)
SCALAR_T = re.compile(r'^(const\s+)?(u?int\d+_t|std::u?int\d+_t|[iu]\d+|float|double|bool|char|int|unsigned|short|'
                      r'long|std::uintptr_t|uintptr_t|void\s*\*|[\w:]+\s*\*)$')
SCALAR_HELPERS = {'str', 'import_call', 'stdcall_import', 'name', 'ini_int', 'ini_float', 'menu'}


def in_image(a):
    return LO <= a < HI


def sec_of(a):
    for n, lo, hi in SECS:
        if lo <= a < hi:
            return n
    return None


def hook_sources(hook):
    for path in sorted(glob.glob(os.path.join(hook, '**', '*'), recursive=True)):
        if not re.search(r'\.(cpp|hpp|h|inc|c)$', path):
            continue
        rel = os.path.relpath(path, hook)
        if os.path.basename(path) in SKIP_FILES or rel.startswith(SKIP_DIRS) or rel.replace(os.sep, '/') in SKIP_PATHS:
            continue
        yield path, rel.replace(os.sep, '/')


def load_frozen():
    """[(start, end)] of re/replay/frozen_data_dead.txt, sorted"""
    p = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'replay', 'frozen_data_dead.txt')
    res = []
    for ln in open(p):
        if ln.strip() and not ln.startswith('#'):
            a, n = ln.split()
            res.append((int(a, 16), int(a, 16) + int(n)))
    return sorted(res)


def in_frozen(frozen, o):
    i = bisect.bisect_right(frozen, (o['start'], 1 << 62)) - 1
    return i >= 0 and frozen[i][0] <= o['start'] and o['end'] <= frozen[i][1]


# .rdata objects whose only reader is a ported library that now holds its own copy (ADR-0001 item 4)
LIBRARY_OWN_COPY = {0x9e1c68: 'lib/jpeg/jpeg_crt.cpp'}


class DataMap:
    """objs: list of dicts sorted by address (start, end, size, sec, cls, note, status, owner, name, type, ...)"""

    def __init__(self, exe, asm, functions_json, thunks_json, hook):
        self.pe = PE(exe)
        self.hook = hook
        # src/data_names.hpp (re/gen_data_names.py): `d::Name` counts exactly like the literal it stands for
        self.data_names = fn_scan.load_data_names(os.path.join(hook, 'data_names.hpp'))
        # src/fn_names.hpp: `fn::Name` too (the static initialiser wrappers W(wrapper, body) name their body)
        self.fn_names = fn_scan.load_fn_names(os.path.join(hook, 'fn_names.hpp'))
        self.img = bytearray(HI - LO)
        for name, lo, hi in SECS[:2]:
            o = self.pe.va2off(lo)
            self.img[lo - LO:hi - LO] = self.pe.data[o:o + (hi - lo)]
        self.thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(thunks_json)).items()}
        funcs = json.load(open(functions_json))
        self.fstarts = sorted(int(f['addr'], 16) for f in funcs)
        self.region = {}
        for row in csv.DictReader(open(os.path.join(RE_DIR, 'functions.csv'))):
            self.region[int(row['address'], 16)] = row['region']
        self._refs(asm)
        self._registry()
        self._objects()
        self._evidence()
        self._classify_all()
        self._status()
        self._names()

    # ------------------------------------------------------------------ bytes
    def source(self, path):
        """a src/** file's text with the data and function names expanded to their literals"""
        return fn_scan.expand_names(open(path, errors='replace').read(), self.fn_names, self.data_names)

    def bytes_at(self, a, n):
        return bytes(self.img[a - LO:a - LO + n])

    def u32(self, a):
        return struct.unpack_from('<I', self.img, a - LO)[0] if LO <= a <= HI - 4 else None

    # ------------------------------------------------------------------ 1. references
    def _refs(self, asm):
        relocs = sorted(self._relocs())
        starts, texts = [], {}
        rx = re.compile(r'^\s*([0-9a-f]+):\t(.*)$')
        hexr = re.compile(r'0x([0-9a-f]{6})\b')
        with open(asm) as fh:
            for ln in fh:
                m = rx.match(ln)
                if not m:
                    continue
                a = int(m.group(1), 16)
                if a >= RDATA[0]:
                    break
                starts.append(a)
                t = m.group(2)
                for h in hexr.findall(t):
                    if in_image(int(h, 16)):
                        texts[a] = t
                        break
        self.code_refs = collections.defaultdict(list)    # target -> [(insn, func, text)]
        self.data_ptr_at = {}                               # loc (in image) -> target
        self.data_ptr_to = collections.defaultdict(list)    # target (in image) -> [loc]
        for loc in relocs:
            v = self.pe.u32(loc)
            if 0x401000 <= loc < RDATA[0]:
                if in_image(v) and 0x414000 <= loc < CODE_HI:   # skip the dead data blob inside .text
                    i = bisect.bisect_right(starts, loc) - 1
                    ia = starts[i]
                    self.code_refs[v].append((ia, self.func_of(ia), texts.get(ia, '?')))
            else:
                if in_image(loc) and sec_of(loc):
                    self.data_ptr_at[loc] = v
                if in_image(v) and sec_of(v):
                    self.data_ptr_to[v].append(loc)
        self.hook_refs = collections.defaultdict(list)    # addr -> [(file, line, helper, ttype, raw line)]
        for path, rel in hook_sources(self.hook):
            src = self.source(path)
            code = STRIP.sub(lambda m: re.sub(r'[^\n]', ' ', m.group(0)), src).split('\n')
            raw = src.split('\n')
            for n, cl in enumerate(code, 1):
                for hm in HEX.finditer(cl):
                    v = int(hm.group(1), 16)
                    if not (in_image(v) and sec_of(v)):
                        continue
                    m = HELPER.search(cl[:hm.start()])
                    helper = m.group(1) if m else ''
                    ttype = (m.group(2) or '')[1:-1].strip() if m else ''
                    self.hook_refs[v].append((rel, n, helper, ttype, cl, hm.start(), hm.end(), raw[n - 1].strip()))

    def _relocs(self):
        p = self.pe
        rva, size = p.dirs[5]
        o = p.rva2off(rva)
        end = o + size
        out = []
        while o < end:
            page, bsz = struct.unpack_from('<II', p.data, o)
            if bsz == 0:
                break
            for k in range((bsz - 8) // 2):
                e = struct.unpack_from('<H', p.data, o + 8 + 2 * k)[0]
                if e >> 12 == 3:
                    out.append(p.base + page + (e & 0xfff))
            o += bsz
        return out

    def func_of(self, a):
        i = bisect.bisect_right(self.fstarts, a) - 1
        return self.fstarts[i] if i >= 0 else None

    def code_region(self, a):
        if THUNK0 <= a < THUNK0 + 5 * NTHUNK:
            a = self.thunks.get(a, a)
        if 0x7dd410 <= a < CODE_HI:
            return 'eh_funclet'
        if CODE_HI <= a < RDATA[0]:
            return 'text_data'
        f = self.func_of(a)
        r = self.region.get(f if f is not None else a)
        if r:
            return {'msvcrt': 'crt', 'd3dx8': 'd3dx', 'libpng/zlib': 'lib', 'game': 'game'}.get(r, r)
        return 'game' if a < 0x763d00 else 'lib'

    # ------------------------------------------------------------------ registry names (code pointer comments)
    def _registry(self):
        self.regname = {}
        wraps = []
        for path in sorted(glob.glob(os.path.join(self.hook, '**', '*.cpp'), recursive=True)):
            rel = os.path.relpath(path, self.hook)
            if rel.startswith(SKIP_DIRS):
                continue
            src = self.source(path)
            m = MOD_RE.search(src)
            module = m.group(1) if m else rel
            for m in REG_RE.finditer(src):
                a = int(m.group(3), 16)
                name = m.group(2)
                if not m.group(1) and '/' in name and not module:
                    continue
                if 0x401000 <= a < CODE_HI and a not in self.regname:
                    self.regname[a] = name
            for m in WRAP_RE.finditer(src):
                wraps.append((int(m.group(1), 16), int(m.group(2), 16)))
        # static initialiser wrappers of the CRT __xc table (game_b105_01 W(wrapper, body))
        for a, body in wraps:
            self.regname.setdefault(a, 'init -> ' + self.regname.get(body, 'sub_%06x' % body))

    def fn_name(self, v):
        t = self.thunks.get(v, v)
        return self.regname.get(t) or self.regname.get(v) or ('sub_%06x' % t)

    # ------------------------------------------------------------------ 2. objects
    def _objects(self):
        img, u32 = self.img, self.u32
        starts = set(self.code_refs) | set(self.data_ptr_to) | set(self.hook_refs)
        self.struct_tag = {}

        def mark(a, cls, name, size):
            starts.add(a)
            self.struct_tag[a] = (cls, name, size)
            if size:
                starts.add(a + size)

        def indata(a):
            return a is not None and in_image(a) and sec_of(a) is not None

        # RTTI type descriptors (.data): {vftable, spare, ".?A..."}
        self.type_infos = {}
        for a in range(DATA[0], DATA[1] - 12, 4):
            tag = bytes(img[a + 8 - LO:a + 10 - LO])
            if tag in (b'.?', b'.P', b'.H'):
                vft = u32(a)
                if vft and RDATA[0] <= vft < RDATA[1] and u32(a + 4) == 0:
                    e = img.index(b'\0', a + 8 - LO) + LO
                    name = img[a + 8 - LO:e - LO].decode('latin1')
                    if re.match(r'^\.[\?PHDKMNOJIFAE][\w@?$.]*$', name):
                        self.type_infos[a] = name
                        mark(a, 'rtti_type_descriptor', name, (e + 1 - a + 3) & ~3)
        # C++ EH tables (.rdata): FuncInfo 0x19930520 -> unwind map, try-block map -> handler types
        for a in range(RDATA[0], RDATA[1] - 28, 4):
            if u32(a) != 0x19930520:
                continue
            maxstate, punw, ntry, ptry = u32(a + 4), u32(a + 8), u32(a + 12), u32(a + 16)
            if maxstate > 10000 or ntry > 1000:
                continue
            mark(a, 'eh_funcinfo', 'FuncInfo', 20 if u32(a + 20) is None else
                 28 if (u32(a + 20) == 0 and u32(a + 24) == 0) else 20)
            if punw and indata(punw) and maxstate:
                mark(punw, 'eh_unwindmap', 'UnwindMapEntry[%d]' % maxstate, 8 * maxstate)
            if ptry and indata(ptry) and ntry:
                mark(ptry, 'eh_tryblockmap', 'TryBlockMapEntry[%d]' % ntry, 20 * ntry)
                for k in range(ntry):
                    ncat, ph = u32(ptry + 20 * k + 12), u32(ptry + 20 * k + 16)
                    if ph and indata(ph) and 0 < ncat < 100:
                        mark(ph, 'eh_handlertype', 'HandlerType[%d]' % ncat, 16 * ncat)
        # throw side: CatchableType (28), CatchableTypeArray, ThrowInfo (16)
        catchable, cta = set(), set()
        for a in range(RDATA[0], RDATA[1] - 28, 4):
            p = u32(a + 4)
            if p in self.type_infos and u32(a) in (0, 1, 2, 4, 8) and u32(a + 8) == 0 and u32(a + 12) == 0xffffffff:
                catchable.add(a)
                mark(a, 'eh_catchable_type', 'CatchableType ' + self.type_infos[p], 28)
        for a in range(RDATA[0], RDATA[1] - 8, 4):
            n = u32(a)
            if 0 < n < 16 and all(u32(a + 4 + 4 * i) in catchable for i in range(n)):
                cta.add(a)
                mark(a, 'eh_catchable_type_array', 'CatchableTypeArray[%d]' % n, 4 + 4 * n)
        for a in range(RDATA[0], RDATA[1] - 16, 4):
            if u32(a + 12) in cta and u32(a + 8) == 0 and u32(a) in (0, 1, 2, 3):
                mark(a, 'eh_throwinfo', 'ThrowInfo ' + self.type_infos.get(u32(u32(u32(a + 12) + 4) + 4), '?'), 16)
        for n, lo, hi in SECS:
            starts.add(lo)
            starts.add(hi)
        mark(CRT_XC[0], 'crt_init_table', '__xc_a..__xc_z (C++ dynamic initialisers)', CRT_XC[1] - CRT_XC[0])
        # boundaries that only exist because the original read a run piecewise (inline strcpy / memcpy / struct copy)
        # or because a string's tail was read separately: such a start is no object of its own
        hard = set(self.hook_refs) | set(self.data_ptr_to) | set(self.struct_tag) | \
            {x for n, lo, hi in SECS for x in (lo, hi)} | {a + t[2] for a, t in self.struct_tag.items() if t[2]}
        self.copy_runs = self._copy_runs()
        inner = {}                         # start -> start of the run / string it belongs to
        for a, e in self.copy_runs:
            for x in range(a + 1, e):
                inner.setdefault(x, a)
        starts -= {x for x in inner if x not in hard}
        self.merged = {}                   # removed start -> object start it was merged into (report)
        for _ in range(8):
            ss = sorted(s for s in starts if LO <= s <= HI)
            new = set()
            for i, s in enumerate(ss[:-1]):
                n = self._natural_size(s, ss[i + 1])
                if n:
                    new.add(s + n)
            new -= starts
            new -= {x for x in new if x in inner}
            starts |= new
            gone = self._string_spans(sorted(s for s in starts if LO <= s <= HI), hard)
            for x, a in gone.items():
                inner.setdefault(x, a)
            starts -= set(gone)
            if not new and not gone:
                break
        starts = sorted(s for s in starts if LO <= s <= HI)
        for x in inner:
            if x in self.code_refs and x not in hard:
                self.merged[x] = starts[bisect.bisect_right(starts, x) - 1]
        self.objs = []
        for i, s in enumerate(starts[:-1]):
            sec = sec_of(s)
            if sec is None:
                continue
            secend = [hi for n, lo, hi in SECS if n == sec][0]
            self.objs.append({'start': s, 'end': min(starts[i + 1], secend), 'sec': sec})
        self.by_start = {o['start']: o for o in self.objs}
        self._ostarts = [o['start'] for o in self.objs]

    def _copy_runs(self):
        """[(start, end)] of runs of .rdata / .data that one function reads piecewise with consecutive absolute operands
        (mov reg,[a]; mov reg,[a+4]; mov reg,[a+8]...: an inlined strcpy / memcpy / struct copy). One object each."""
        acc = collections.defaultdict(list)            # function -> [(insn, target, width)]
        for v, refs in self.code_refs.items():
            for ia, f, t in refs:
                m = COPY_RE.match(t)
                if not m or not m.group('dst') or m.group('src') or not (RDATA[0] <= v < DATA[1]):
                    continue               # reads of initialised data only (.bss is never emitted)
                w = {'BYTE': 1, 'WORD': 2, 'DWORD': 4}[m.group('sz')] if m.group('sz') else REG_W.get(m.group('dst'))
                if w and int(m.group('a'), 16) == v:
                    acc[f].append((ia, v, w))
        runs = []
        for f, rs in acc.items():
            rs.sort()
            # chains: the next piece (target == previous end) read within 0x30 bytes of code
            used = set()
            for i, (ia, v, w) in enumerate(rs):
                if i in used:
                    continue
                end, last = v + w, ia
                for j in range(i + 1, len(rs)):
                    ja, jv, jw = rs[j]
                    if ja - last > 0x30:
                        break
                    if jv == end:
                        used.add(j)
                        end, last = jv + jw, ja
                if end - v > w:
                    runs.append((v, end))
        runs.sort()
        out = []
        for a, e in runs:
            if out and a < out[-1][1]:
                out[-1] = (out[-1][0], max(out[-1][1], e))
            else:
                out.append((a, e))
        return out

    def _string_spans(self, ss, hard):
        """starts inside a string whose first piece holds no NUL (its tail was read on its own): {start: string start}"""
        gone = {}
        for i, s in enumerate(ss[:-1]):
            if not (RDATA[0] <= s < DATA[1]) or s in self.struct_tag or s in gone:
                continue
            e = ss[i + 1]
            b = self.img[s - LO:e - LO]
            if not b or 0 in b or sec_of(s) != sec_of(e - 1):
                continue
            j = self.img.find(b'\0', s - LO) + LO
            t = self.img[s - LO:j - LO]
            if j <= e or j - s > 4096 or len(t) < 2 or sec_of(j) != sec_of(s) or \
                    any(c not in STRPRINT for c in t) or sum(1 for c in t if 0x20 <= c < 0x7f) < 0.8 * len(t):
                continue
            k = i + 1
            inside = []
            while k < len(ss) and ss[k] <= j:
                inside.append(ss[k])
                k += 1
            if any(x in hard for x in inside):
                continue
            for x in inside:
                gone[x] = s
        return gone

    def obj_at(self, a):
        i = bisect.bisect_right(self._ostarts, a) - 1
        return self.objs[i] if i >= 0 and self.objs[i]['start'] <= a < self.objs[i]['end'] else None

    def _natural_size(self, s, e):
        """size of the item at s when it is obviously shorter than [s, e) (string, scalar, pointer run, zero run)"""
        if s in self.struct_tag or not (RDATA[0] <= s < DATA[1]):
            return None
        b = self.img[s - LO:e - LO]
        if not b:
            return None
        crefs = self.code_refs.get(s, [])
        hrefs = self.hook_refs.get(s, [])
        if any('*' in t or re.search(r'\[e\w\w\+0x', t) for ia, f, t in crefs):
            return None                    # indexed by the original: an array, keep whole
        if any(re.search(r'\[\d*\]', h[3] or '') or self._arith(h) for h in hrefs):
            return None                    # array type / pointer arithmetic in our C++
        if DATA[0] <= s < DATA[1] and (crefs or hrefs):
            if any(SIZE_RE0.search(t) for ia, f, t in crefs) or any(h[2] == 'global' for h in hrefs):
                return None                # .data globals: only string literals are split
        if b[0] == 0:
            k = 0
            while k < len(b) and b[k] == 0:
                k += 1
            k &= ~3
            return k if 0 < k < len(b) and not crefs and not hrefs and not self.data_ptr_to.get(s) else None
        k = 0
        while s + k + 4 <= e and s + k in self.data_ptr_at and is_code(self.data_ptr_at[s + k]):
            k += 4
        if k:
            return k if k < len(b) else None
        if s + 4 <= e and s in self.data_ptr_at:
            k = 0
            while s + k + 4 <= e and s + k in self.data_ptr_at:
                k += 4
            return k if k < len(b) else None
        j = b.find(b'\0')
        if j > 0 and all(c in STRPRINT for c in b[:j]) and sum(1 for c in b[:j] if 0x20 <= c < 0x7f) >= 0.6 * j:
            n = (j + 1 + 3) & ~3
            if n > j + 1 and any(b[j + 1:n]):
                n = j + 1
            return n if n < len(b) else None
        acc = collections.Counter()
        for ia, f, t in crefs:
            m = SIZE_RE0.search(t)
            if m and re.match(r'^f', t):
                acc[{'DWORD': 'float', 'QWORD': 'double', 'TBYTE': 'long double'}.get(m.group(1), '')] += 1
            elif t.split()[0] in MMX_OPS:
                acc['mmx64'] += 1
        if acc:
            n = {'float': 4, 'double': 8, 'long double': 10, 'mmx64': 8}.get(acc.most_common(1)[0][0])
            if n and n < len(b) and RDATA[0] <= s < RDATA[1]:
                return n
        return None

    @staticmethod
    def _minus(h):
        """our C++ reads below this constant (0x... - n, base[-1])"""
        rel, n, helper, ttype, cl, s, e, raw = h
        return cl[e:].lstrip()[:1] == '-' or cl[:s].rstrip().endswith('-') or '[-' in cl.replace(' ', '')

    @staticmethod
    def _arith(h):
        """our C++ computes an address from this constant (0x... + i, base[-1], &global<T>(a) + 1 ...)"""
        rel, n, helper, ttype, cl, s, e, raw = h
        before, after = cl[:s].rstrip(), cl[e:].lstrip()
        if after[:1] in '+-[' and after[:2] not in ('+=', '-=', '++', '--', '->'):
            return True
        if before.endswith(('+', '-')) and not before.endswith(('++', '--')):
            return True
        if helper:
            # the helper's result used as an address: str(a) + 1, obj(a)[i], &global<T>(a) + 1
            depth, i = 0, e
            while i < len(cl):
                if cl[i] == '(':
                    depth += 1
                elif cl[i] == ')':
                    if depth == 0:
                        break
                    depth -= 1
                i += 1
            rest = cl[i + 1:].lstrip()
            if rest[:1] in '+[' and rest[:2] not in ('+=', '++'):
                return True
            hb = cl[:s]
            k = hb.rfind(helper)
            if k > 0 and hb[:k].rstrip().endswith('&') and rest[:1] in '+-[':
                return True
        return False

    # ------------------------------------------------------------------ 3. evidence + classes
    def _evidence(self):
        for o in self.objs:
            s, e = o['start'], o['end']
            o['size'] = e - s
            o['code'] = self.code_refs.get(s, [])
            o['hook'] = self.hook_refs.get(s, [])
            o['ptr_in'] = self.data_ptr_to.get(s, [])
            o['ptrs'] = [(loc, self.data_ptr_at[loc]) for loc in range(s, e) if loc in self.data_ptr_at]
            acc = collections.Counter()
            idx = False
            for ia, f, t in o['code']:
                mn = t.split()[0] if t != '?' else '?'
                sm = SIZE_RE.search(t)
                sz = sm.group(1) if sm else ''
                if '*' in t or re.search(r'\[e\w\w\+0x', t):
                    idx = True
                if FPU.match(mn):
                    acc[{'DWORD': 'float', 'QWORD': 'double', 'TBYTE': 'long double'}.get(sz, 'fpu?')] += 1
                elif FPUI.match(mn):
                    acc[{'WORD': 'int16', 'DWORD': 'int32', 'QWORD': 'int64'}.get(sz, 'int?')] += 1
                elif mn in MMX_OPS:
                    acc['mmx64'] += 1
                elif mn == 'fldcw':
                    acc['uint16'] += 1
                elif mn in ('call', 'jmp'):
                    acc['fnptr'] += 1
                elif (re.match(r'^(push|mov\s+\w+,0x|mov\s+DWORD PTR \[[^\]]*\],0x)', t) and not sz) or \
                        (mn == 'mov' and re.search(r',0x[0-9a-f]{6}$', t) and not re.search(r'PTR ds:0x[0-9a-f]+,', t)) \
                        or mn == 'lea':
                    acc['addr'] += 1
                elif sz:
                    acc[{'BYTE': 'int8', 'WORD': 'int16', 'DWORD': 'int32', 'QWORD': 'int64'}.get(sz, sz)] += 1
                else:
                    acc['?'] += 1
            o['acc'] = acc
            o['indexed'] = idx
            o['fn_regions'] = collections.Counter(self.code_region(f) for ia, f, t in o['code'] if f)

    def _classify_all(self):
        for o in self.objs:
            if o['sec'] != '.bss' and o['start'] not in self.struct_tag and not o['ptrs']:
                st = strings_in(self.bytes_at(o['start'], o['size']))
                if st:
                    o['_str'] = st
        for o in self.objs:
            o['cls'], o['note'] = self._classify(o)
            regs = o['fn_regions']
            o['owner'] = 'game' if regs.get('game') else '/'.join(sorted(regs)) if regs else ''
            if o['cls'] in EH_CLASSES:
                o['owner'] = o['owner'] or 'eh'
            if o['cls'] in ('vtable', 'fnptr_table'):
                cls = collections.Counter()
                o['slots'] = []
                for loc, v in o['ptrs']:
                    nm = self.fn_name(v)
                    o['slots'].append(nm)
                    if '::' in nm:
                        cls[nm.split('::')[0]] += 1
                if cls:
                    o['note'] = (cls.most_common(1)[0][0] + ' ' + o['note']).strip()

    def _classify(self, o):
        s, e, sec = o['start'], o['end'], o['sec']
        b = self.bytes_at(s, o['size'])
        acc = o['acc']
        if s in self.struct_tag:
            return self.struct_tag[s][0], self.struct_tag[s][1]
        regs = o['fn_regions']
        owner = '/'.join(sorted(r for r in regs if r != 'game')) if regs and not regs.get('game') else ''
        if sec == '.bss':
            return 'bss_' + (owner or 'game'), ''
        if not any(b):
            if not o['code'] and not o['hook'] and not o['ptr_in']:
                return 'padding', ''
            return 'zero_init_' + (owner or 'game'), ''
        ptrs = o['ptrs']
        if ptrs and len(ptrs) * 4 >= (e - s) - 3 - (e - s) // 8:
            tg = [v for loc, v in ptrs]
            if all(is_code(v) for v in tg):
                if any(r[2].startswith('mov') and 'PTR [' in r[2] and ',0x' in r[2] for r in o['code']) or \
                        any('vtable' in h[7].lower() for h in o['hook']):
                    return 'vtable', ''
                if all(THUNK0 <= v < THUNK0 + 5 * NTHUNK for v in tg):
                    return ('vtable' if sec == '.rdata' else 'fnptr_table'), ''
                return 'fnptr_table', ''
            if all(in_image(v) and sec_of(v) for v in tg):
                tgt = [self.obj_at(v) for v in tg]
                if all(t.get('_str') for t in tgt if t):
                    return 'string_ptr_table', ''
                return 'data_ptr_table', ''
            return 'mixed_ptr_table', ''
        if (e - s) % 12 == 0 and e - s >= 12 and ptrs:
            good = True
            for k in range(0, e - s, 12):
                prev, filt, hand = struct.unpack_from('<III', b, k)
                if not (prev == 0xffffffff or prev < 64) or not is_code(hand) or not (filt == 0 or is_code(filt)):
                    good = False
                    break
            if good:
                return 'seh_scopetable', ''
        if e - s == 16 and b[8:16] == b'\xbf\xc7\x44\x45\x53\x54\x00\x00':
            return 'guid', 'DirectInput GUID'
        top = acc.most_common(1)[0][0] if acc else None
        if top in ('float', 'double', 'long double', 'mmx64'):
            if sec == '.rdata':
                return {'float': 'float_const', 'double': 'double_const', 'long double': 'ld_const',
                        'mmx64': 'mmx_const'}[top], ''
            return 'global_' + top, ''
        st = strings_in(b)
        if st and (sec == '.rdata' or (acc.get('addr') or not acc)):
            o['_str'] = st
            return ('string_8bit' if any(c >= 0x80 for x in st for c in x) else 'string'), ''
        if sec == '.data' and top in ('int8', 'int16', 'int32', 'int64', 'uint16', 'fnptr', 'int?', 'fpu?'):
            return 'global_init', top
        if sec == '.data' and any(h[2] == 'global' for h in o['hook']):
            return 'global_init', ''
        if len(b) >= 4 and len(b) % 2 == 0 and all(b[k + 1] == 0 for k in range(0, len(b) - 2, 2)) and \
                b[0] and all(0x20 <= b[k] < 0x7f or b[k] == 0 for k in range(0, len(b), 2)):
            return 'wstring', ''
        if sec == '.rdata':
            if top in ('int8', 'int16', 'int32', 'int64', 'uint16', 'int?'):
                return ('int_table' if (e - s) > 8 else 'int_const'), ''
            return 'blob', ''
        if top in ('int8', 'int16', 'int32', 'int64', 'uint16', 'fnptr', 'addr', 'int?', 'fpu?', '?'):
            return 'global_init', top
        if not o['code'] and not o['hook'] and not o['ptr_in']:
            return 'unref_init', ''
        return 'global_init', ''

    # ------------------------------------------------------------------ 4. status
    def opaque(self, h):
        """our C++ may read beyond the start of the object through this reference"""
        rel, n, helper, ttype, cl, s, e, raw = h
        if self._arith(h):
            return True
        if helper == 'global' and SCALAR_T.match((ttype or '').strip()):
            return False
        if helper in SCALAR_HELPERS:
            return False
        if not helper and ASM_SCALAR.search(cl):
            return False                   # naked asm operand of explicit size, no index register
        return True

    def _status(self):
        objs = self.objs
        for o in objs:
            o['status'] = ''
            o['opaque'] = o['indexed'] or any(self.opaque(h) for h in o['hook'])
        for o in objs:
            if o['hook']:
                o['status'] = 'live'
                o['why'] = 'hook'
        # .bss objects: reserved space, the status only documents usage (they are never emitted)
        for _ in range(64):
            changed = False
            kept = [o for o in objs if o['status']]
            # pointer closure over everything kept
            work = list(kept)
            while work:
                o = work.pop()
                for loc, v in o['ptrs']:
                    t = self.obj_at(v)
                    if t is not None and not t['status']:
                        t['status'] = 'live'
                        t['why'] = 'pointer from 0x%06x' % o['start']
                        work.append(t)
                        changed = True
            # tails / heads of live objects read as arrays / opaquely
            for i, o in enumerate(objs):
                if o['status'] != 'live' or not o['opaque']:
                    continue
                j = i + 1
                while j < len(objs) and not objs[j]['status'] and objs[j]['sec'] == o['sec'] and \
                        objs[j]['cls'] not in EH_CLASSES:
                    t = objs[j]
                    if o['owner'] and t['owner'] and not (set(o['owner'].split('/')) & set(t['owner'].split('/'))):
                        break              # read by other code (another compiland's variable)
                    if o['sec'] == '.rdata' and (t['cls'] in ('string', 'string_8bit', 'padding', 'vtable', 'guid')
                                                 or (not o['indexed'] and t['cls'] != o['cls'])):
                        break
                    t['status'] = 'tail'
                    t['why'] = 'after 0x%06x (read as an array / opaquely)' % o['start']
                    changed = True
                    j += 1
                if not (o['indexed'] or any(self._minus(h) for h in o['hook'])):
                    continue
                j = i - 1
                while j >= 0 and objs[j]['cls'] == 'padding' and objs[j]['sec'] == o['sec']:
                    j -= 1
                if j >= 0 and not objs[j]['status'] and objs[j]['sec'] == o['sec'] and \
                        objs[j]['cls'] not in EH_CLASSES and objs[j]['end'] + 16 >= o['start']:
                    objs[j]['status'] = 'head'
                    objs[j]['why'] = 'before 0x%06x (read as an array / opaquely)' % o['start']
                    changed = True
            if not changed:
                break
        hook_at = sorted(self.hook_refs)
        hook_base = sorted(a for a, hs in self.hook_refs.items() if any(self.opaque(h) for h in hs))

        def hook_inside(o):
            i = bisect.bisect_right(hook_at, o['start'])
            return i < len(hook_at) and hook_at[i] < o['end']

        def hook_near(o):
            """an address our C++ computes with (base + offset, opaque read) lies at most NEAR bytes below"""
            i = bisect.bisect_left(hook_base, o['start']) - 1
            return i >= 0 and o['start'] - hook_base[i] <= NEAR and sec_of(hook_base[i]) == o['sec']

        for o in objs:
            if o['status']:
                continue
            if o['cls'] == 'padding':
                o['status'] = 'padding'
                o['why'] = 'zero, unreferenced'
                continue
            if o['sec'] == '.bss':
                o['status'] = 'dead'
                o['why'] = 'unnamed .bss'
                continue
            o['status'] = 'unproven'
            libonly = bool(o['owner']) and set(o['owner'].split('/')) <= LIB_OWNERS
            if hook_inside(o):
                o['why'] = 'our C++ names an address inside it (%s, original readers: %s)' % (
                    o['cls'], o['owner'] or 'none')
            elif o['cls'] in EH_CLASSES:
                o['status'] = 'dead'
                o['why'] = 'C++ EH / SEH / RTTI table of original frames (no original frame exists since S5)'
            elif hook_near(o) and (libonly or not o['code']):
                o['why'] = 'within %d bytes after an address our C++ computes with (%s, original readers: %s)' % (
                    NEAR, o['cls'], o['owner'] or 'none')
            elif libonly:
                o['status'] = 'dead'
                o['why'] = ('MMX constant read only by replaced library code (%s)' if o['cls'] == 'mmx_const' else
                            'read only by replaced library code (%s)') % o['owner']
            else:
                o['why'] = 'not named by our C++, but not provably unread (%s, original readers: %s)' % (
                    o['cls'], o['owner'] or 'none')
        # library data the ported library now carries itself (ADR-0001 item 4); its strings follow below
        for o in objs:
            if o['start'] in LIBRARY_OWN_COPY:
                o['status'] = 'dead'
                o['why'] = 'the ported library has its own copy (%s)' % LIBRARY_OWN_COPY[o['start']]
        # the hashed range: an object may be dead only inside the frozen ranges (re/replay/frozen_data_dead.txt)
        frozen = load_frozen()
        def thaw(o):
            return o['sec'] == '.data' and o['status'] == 'dead' and not in_frozen(frozen, o)
        for o in objs:
            if thaw(o):
                o['status'] = 'unproven'
                o['why'] = 'kept: inside the replay-hashed range (re/replay/frozen_data_dead.txt)'
        # targets of dead tables only (D3DX error strings, EH type names ...): dead as well
        while True:
            changed = False
            for o in objs:
                if o['status'] != 'unproven' or o['code'] or o['hook'] or not o['ptr_in'] or hook_inside(o) or \
                        hook_near(o):
                    continue
                src = [self.obj_at(loc) for loc in o['ptr_in']]
                if all(s is not None and s['status'] == 'dead' for s in src):
                    o['status'] = 'dead'
                    o['why'] = 'only pointed to by dead objects (%s)' % ', '.join(
                        '0x%06x' % a for a in sorted({s['start'] for s in src})[:4])
                    changed = True
            if not changed:
                break
        # keep (unproven = keep): pointer targets of everything kept, and dead runs wedged between kept objects
        for _ in range(64):
            changed = False
            for o in objs:
                if o['status'] in ('dead', 'padding') or o['sec'] == '.bss':
                    continue
                for loc, v in o['ptrs']:
                    t = self.obj_at(v)
                    if t is not None and t['status'] == 'dead' and t['sec'] != '.bss':
                        t['status'] = 'unproven'
                        t['why'] = 'pointer from kept 0x%06x (%s)' % (o['start'], t['cls'])
                        changed = True
            i = 0
            while i < len(objs):
                o = objs[i]
                if o['status'] != 'dead' or o['sec'] == '.bss' or o['cls'] in EH_CLASSES:
                    i += 1
                    continue
                k = i
                while k + 1 < len(objs) and objs[k + 1]['status'] == 'dead' and objs[k + 1]['sec'] == o['sec'] and \
                        objs[k + 1]['cls'] not in EH_CLASSES:
                    k += 1
                p, q = (objs[i - 1] if i else None), (objs[k + 1] if k + 1 < len(objs) else None)
                if p and q and p['sec'] == q['sec'] == o['sec'] and p['end'] == o['start'] and \
                        q['start'] == objs[k]['end'] and p['status'] in KEPT and q['status'] in KEPT:
                    for t in objs[i:k + 1]:
                        t['status'] = 'unproven'
                        t['why'] = 'between kept 0x%06x and 0x%06x, no padding gap (%s, original readers: %s)' % (
                            p['start'], q['start'], t['cls'], t['owner'] or 'none')
                    changed = True
                i = k + 1
            if not changed:
                break

    # ------------------------------------------------------------------ names / C types used by our C++
    def _names(self):
        accessor = re.compile(r'(\w+)\s*\(\s*\)\s*(?:const\s*)?\{\s*return\s+(?:hook::)?global\s*<\s*([^>]+)>\s*'
                              r'\(\s*0x([0-9a-fA-F]+)')
        kname = re.compile(r'\b(k[A-Z]\w*|g_\w+|s_\w+)\s*(?:=|\{)\s*0x([0-9a-fA-F]+)')
        names = collections.defaultdict(collections.Counter)
        types = collections.defaultdict(collections.Counter)
        for path, rel in hook_sources(self.hook):
            src = self.source(path)
            for m in accessor.finditer(src):
                a = int(m.group(3), 16)
                names[a][m.group(1)] += 3
                types[a][m.group(2).strip()] += 3
            for m in kname.finditer(src):
                a = int(m.group(2), 16)
                if in_image(a):
                    names[a][m.group(1)] += 1
        for a, uses in self.hook_refs.items():
            for h in uses:
                if h[2] == 'global' and h[3]:
                    types[a][h[3]] += 1
        for o in self.objs:
            n = names.get(o['start'])
            # most used; ties broken alphabetically (deterministic)
            o['name'] = sorted(n.items(), key=lambda kv: (-kv[1], kv[0]))[0][0] if n else ''
            t = types.get(o['start'])
            o['type'] = sorted(t.items(), key=lambda kv: (-kv[1], kv[0]))[0][0] if t else ''

    # ------------------------------------------------------------------ report
    def readers(self, o, limit=3):
        files = collections.Counter(h[0].replace('modules/', '') for h in o['hook'])
        fs = sorted(files, key=lambda f: (-files[f], f))
        r = ', '.join(fs[:limit])
        if len(fs) > limit:
            r += ' +%d' % (len(fs) - limit)
        return r

    def write_tsv(self, path):
        with open(path, 'w', newline='\n') as f:
            f.write('start\tend\tsize\tsection\tclass\tstatus\towner\tname\tnotes\treaders\twhy\n')
            for o in self.objs:
                note = o['note']
                if o.get('_str'):
                    note = (note + ' ' + repr(b'|'.join(o['_str'])[:60].decode('latin1'))).strip()
                f.write('0x%06x\t0x%06x\t%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' % (
                    o['start'], o['end'], o['size'], o['sec'], o['cls'], o['status'], o['owner'],
                    o['name'] or o['type'], note.replace('\t', ' '), self.readers(o, 4), o.get('why', '')))


_R = r'(?:e?[a-d]x|e?[sd]i|[a-d][lh]|ebp|bp)'
COPY_RE = re.compile(r'^mov\s+(?:(?P<dst>' + _R + r'),)?(?:(?P<sz>BYTE|WORD|DWORD) PTR )?ds:0x(?P<a>[0-9a-f]+)'
                     r'(?:,(?P<src>' + _R + r'))?$')
REG_W = {r: 4 for r in ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp')}
REG_W.update({r: 2 for r in ('ax', 'bx', 'cx', 'dx', 'si', 'di', 'bp')})
REG_W.update({r: 1 for r in ('al', 'ah', 'bl', 'bh', 'cl', 'ch', 'dl', 'dh')})
FPU = re.compile(r'^(fld|fst|fstp|fadd|fsub|fsubr|fmul|fdiv|fdivr|fcom|fcomp|fcompp)\b')
FPUI = re.compile(r'^(fild|fist|fistp|fiadd|fisub|fisubr|fimul|fidiv|fidivr|ficom|ficomp)\b')
SIZE_RE = re.compile(r'(BYTE|WORD|DWORD|QWORD|TBYTE|XMMWORD|MMWORD) PTR')
SIZE_RE0 = re.compile(r'(BYTE|WORD|DWORD|QWORD|TBYTE) PTR')
MMX_OPS = {'movq', 'pmaddwd', 'paddd', 'pxor', 'psubsw', 'punpcklwd', 'punpckhwd', 'pand', 'por', 'paddw', 'psubw',
           'pmullw', 'pmulhw', 'psrlq', 'psllq'}
STRPRINT = set(range(0x20, 0x7f)) | {9, 10, 13} | set(range(0x80, 0x100))


def is_code(v):
    return v is not None and THUNK0 <= v < CODE_HI


def strings_in(b):
    """b as NUL-terminated strings + zero padding, or None"""
    out = []
    i, n = 0, len(b)
    while i < n:
        if b[i] == 0:
            j = i
            while j < n and b[j] == 0:
                j += 1
            if j == n:
                break
            if out and j - i < 4:
                i = j
                continue
            return None
        j = b.find(b'\0', i)
        if j < 0:
            return None
        s = b[i:j]
        if any(c not in STRPRINT for c in s) or sum(1 for c in s if 0x20 <= c < 0x7f) < 0.6 * len(s):
            return None
        out.append(s)
        i = j + 1
    return out or None


def main():
    args = [a for a in sys.argv[1:]]
    tsv = None
    if '--tsv' in args:
        k = args.index('--tsv')
        tsv = args[k + 1]
        del args[k:k + 2]
    if len(args) < 4:
        sys.exit(__doc__)
    hook = args[4] if len(args) > 4 else os.path.join(RE_DIR, '..', 'src')
    m = DataMap(args[0], args[1], args[2], args[3], hook)
    tot = collections.Counter()
    cnt = collections.Counter()
    for o in m.objs:
        tot[(o['sec'], o['status'])] += o['size']
        cnt[(o['sec'], o['status'])] += 1
    for k in sorted(tot):
        print('%-7s %-9s %6d objects %7d bytes' % (k[0], k[1], cnt[k], tot[k]))
    if tsv:
        m.write_tsv(tsv)


if __name__ == '__main__':
    main()
