#!/usr/bin/env python3
"""Generate hook/code_map.inc: the code joan_re.exe generates in place of joan.exe's .text ([Loader] Code=generated,
STANDALONE.md stage S5; consumer: hook/codegen.cpp).

Usage (from the repository root):
  gen_code_map.py <joan.exe> <joan.asm> <functions.json> <thunks.json> <hook dir> <out code_map.inc> [extra entries]

[extra entries] defaults to re/extra_entries.txt next to this script (when present): real function entries that
functions.json misses (merged into the function before them - code right after a jump table, hand-written asm
routines without padding, library methods only stored as `mov [x], offset fn`). First token of each non-# line is
the address; they are listed as FUNC.

joan.asm is `objdump -d -M intel --no-show-raw-insn` of joan.exe (only used to check that a constant of our C++ in the
library / funclet range is an instruction start).

Two tables:
- JOAN_CODE_THUNKS: the target of every incremental-link thunk, 0x00401005 + 5 * i (8015 `jmp rel32`, back to back).
  The loader writes them as `jmp target`, exactly what joan.exe has there; registry::install_all then retargets them
  to the replacements (redirect_function) and codegen::finalize sends every thunk that still leads into the
  generated range to a trap.
- JOAN_CODE_ENTRIES: every address inside 0x00414000..0x007d5000 / 0x007dd410..0x007e50a0 (game + statically linked
  libraries, EH funclets) where code may still arrive, with the room up to the next entry and why it is listed:
    FUNC   function start (functions.json, re/extra_entries.txt, or a registered replacement address)
    THUNK  a thunk jumps to it
    DATA   a relocated dword of .rdata / .data / .idata / .rsrc points at it (vtables, function pointer tables, CRT
           init tables, SEH scope tables, FuncInfo maps, ...)
    HOOK   a hex constant of our C++ (hook/**) other than a registration address, in a file that does not register
           that address (calls by real address, kc_ constants, SEH handlers pushed by our frames, callbacks); in
           KEY_FILES only direct calls count (the rest are lookup keys / range bounds)
    SELF   a direct call of that address (thiscall / cdecl_call / original<> / ORIG ...) in the file that registers it:
           the fallback / verify call of the "untouched original body" by the replacement's own module. Any other use
           in that file (callbacks for atexit / qsort / bsearch, kc_ constants, template arguments) counts as HOOK
    REG    registered replacement address (JOAN_REPLACE* or a module macro of the form X("name", 0x...))
    ENTRY  joan.exe's entry point (CRT startup, called by the loader)
  The loader writes a 5-byte `jmp` to a trap stub at each entry with room >= 5; codegen::finalize points the entries of
  installed replacements at the replacement (see codegen.cpp for the rule). Entries with room < 5 stay int3 (the
  vectored handler reports any execution there).
Names of hook/fn_names.hpp (re/gen_fn_names.py, `fn::Name` / `joan::fn::Name`) and hook/data_names.hpp
(re/gen_data_names.py, `d::Name`) count exactly like the literal they stand for: each source line is scanned with the
names expanded to their values (the two headers themselves are not scanned).
Constants of our C++ in the game range that are not function starts are byte-patch sites of canvas_hooks.cpp /
display.cpp (operands inside replaced functions), never control flow: they are listed in the header, not entries.
"""
import bisect
import collections
import glob
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pe import PE  # noqa: E402
import fn_scan  # noqa: E402

TEXT_LO, TEXT_HI = 0x00401000, 0x009ce700
CODE_RANGES = [(0x00414000, 0x007d5000), (0x007dd410, 0x007e50a0)]
GAME_HI = 0x00763d00
THUNK_FIRST, THUNK_COUNT = 0x00401005, 8015
# Range bounds used as plain numbers by census.cpp / patch.cpp / gl/census.cpp / codegen (not code references).
BOUNDS = {0x00401000, 0x009ce700, 0x00414000, 0x00763d00, 0x00765000, 0x007d5000, 0x007dd410, 0x007e50a0}
# Files whose code addresses are lookup keys / range bounds, not control flow: canvas_hooks.cpp (screen table keys
# for installed_replacement / redirect_function, size-read function ranges), display.cpp (Patch::function keys),
# fixes.cpp (redirect_function key). Only a direct call (CALL_RE) there is a reference; anything else would make the
# entry jump to the replacement, and the replacement's own fallback call of that address would then recurse.
KEY_FILES = {'canvas_hooks.cpp', 'display.cpp', 'fixes.cpp'}

F_FUNC, F_THUNK, F_DATA, F_HOOK, F_SELF, F_REG, F_ENTRY = 1, 2, 4, 8, 16, 32, 64
FLAG_NAMES = [(F_FUNC, 'FUNC'), (F_THUNK, 'THUNK'), (F_DATA, 'DATA'), (F_HOOK, 'HOOK'), (F_SELF, 'SELF'),
              (F_REG, 'REG'), (F_ENTRY, 'ENTRY')]

STRIP = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])\'', re.S)
# A registration: NAME("a", "b", 0x...) / NAME("b", 0x...) / NAME("a", "b", Class::x, 0x...)
REG_RE = re.compile(r'\b[A-Za-z_]\w*\s*\(\s*(?:"[^"]*"\s*,\s*){1,2}(?:[A-Za-z_][\w:]*\s*,\s*)?0x([0-9a-fA-F]{6,8})(?:[uUlL]{0,3})\b')
HEX_RE = re.compile(r'\b0x([0-9a-fA-F]{6,8})(?:[uUlL]{0,3})\b')
# The constant is the address argument of a direct call: thiscall / cdecl_call / stdcall / fastcall helpers,
# original<Fn>(a), ORIG / JOAN_ORIG(a, ...), reinterpret_cast<R (__cdecl*)(...)>(a)(...), and in naked asm
# `mov reg, a` (then jmp / call reg), `jmp a`, `call a`. A `push a` is a callback / SEH handler, never a call.
CALL_RE = re.compile(r'(?:\b(?:JOAN_)?ORIG\s*\(|\boriginal\s*<[^;]*>\s*\(|\b[A-Za-z_]\w*call\w*\s*(?:<[^;()]*>)?\s*\('
                     r'|\*\s*\)\s*\([^;]*\)\s*>\s*\(|^\s*(?:mov\s+\w+\s*,|jmp|call))\s*$')


def in_code(a):
    return any(lo <= a < hi for lo, hi in CODE_RANGES)


def load_extra(path):
    """Addresses of re/extra_entries.txt (first token per line, '#' comments)."""
    if not path or not os.path.exists(path):
        return []
    res = []
    for ln in open(path):
        ln = ln.strip()
        if ln and not ln.startswith('#'):
            res.append(int(ln.split()[0], 16))
    return res


def main():
    exe, asm, fjson, tjson, hook_dir, out = sys.argv[1:7]
    extra_path = sys.argv[7] if len(sys.argv) > 7 else os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                                                     'extra_entries.txt')
    pe = PE(exe)

    # ---- thunks: contiguous jmp rel32 table, checked against the bytes of joan.exe
    thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(tjson)).items()}
    assert sorted(thunks) == [THUNK_FIRST + 5 * i for i in range(THUNK_COUNT)], 'thunk table not contiguous'
    targets = []
    for i in range(THUNK_COUNT):
        at = THUNK_FIRST + 5 * i
        o = pe.va2off(at)
        assert pe.data[o] == 0xE9, hex(at)
        rel = int.from_bytes(pe.data[o + 1:o + 5], 'little', signed=True)
        t = (at + 5 + rel) & 0xFFFFFFFF
        assert t == thunks[at], (hex(at), hex(t), hex(thunks[at]))
        assert in_code(t), (hex(at), hex(t))
        targets.append(t)

    # ---- instruction starts (library / funclet constants must be one)
    insn = set()
    rx = re.compile(r'^\s*([0-9a-f]+):')
    for ln in open(asm):
        m = rx.match(ln)
        if m:
            a = int(m.group(1), 16)
            if a >= CODE_RANGES[-1][1]:
                break
            if a >= CODE_RANGES[0][0]:
                insn.add(a)

    flags = collections.defaultdict(int)
    funcs = json.load(open(fjson))
    for f in funcs:
        a = int(f['addr'], 16)
        if in_code(a):
            flags[a] |= F_FUNC
    extra = [a for a in load_extra(extra_path) if in_code(a)]
    for a in extra:
        flags[a] |= F_FUNC
    starts = set(flags)
    for t in targets:
        flags[t] |= F_THUNK | F_FUNC
    flags[pe.entry] |= F_ENTRY | F_FUNC

    # ---- data references (relocated dwords outside .text)
    data_refs = 0
    for loc in pe.relocs():
        if TEXT_LO <= loc < TEXT_HI:
            continue
        v = pe.u32(loc)
        if TEXT_LO <= v < TEXT_HI and not (THUNK_FIRST <= v < THUNK_FIRST + 5 * THUNK_COUNT):
            if in_code(v):
                flags[v] |= F_DATA
                data_refs += 1

    # ---- our C++
    hook_refs = collections.defaultdict(list)     # address -> (file, direct call) of every non-registration use
    registered = collections.defaultdict(set)     # address -> files that register it
    patch_sites = collections.defaultdict(list)
    # hook/data/ (re/gen_data_curated.py: joan.exe's typed .rdata / .data) and image_layout.inc / import_table.inc
    # (re/gen_data_image.py) are joan.exe's own data, layout and imports: their code pointers are the DATA references
    # above, the rest are not references at all.
    # fn_names.hpp (re/gen_fn_names.py) and data_names.hpp (re/gen_data_names.py) only define names: their uses are
    # expanded below.
    skip = {'code_map.inc', 'census_table.inc', 'image_layout.inc', 'import_table.inc', 'fn_names.hpp',
            'data_names.hpp'}
    fn_names = fn_scan.load_fn_names(os.path.join(hook_dir, 'fn_names.hpp'))
    data_names = fn_scan.load_data_names(os.path.join(hook_dir, 'data_names.hpp'))
    for path in sorted(glob.glob(os.path.join(hook_dir, '**', '*'), recursive=True)):
        if not re.search(r'\.(cpp|hpp|h|inc|c)$', path) or os.path.basename(path) in skip:
            continue
        rel = os.path.relpath(path, hook_dir).replace(os.sep, '/')
        if rel.startswith('data/') or rel in ('image.hpp', 's7_probe.cpp'):
            continue
        src = fn_scan.expand_names(open(path, errors='replace').read(), fn_names, data_names)
        code = STRIP.sub(lambda m: re.sub(r'[^\n]', ' ', m.group(0)), src).split('\n')
        for n, (cl, rl) in enumerate(zip(code, src.split('\n')), 1):
            regs = {int(h, 16) for h in REG_RE.findall(rl)} if '"' in rl else set()
            for a in regs:
                if in_code(a):
                    registered[a].add(rel)
            for hm in HEX_RE.finditer(cl):
                v = int(hm.group(1), 16)
                call = CALL_RE.search(cl[:hm.start()]) is not None
                if v in regs or v in BOUNDS or not (TEXT_LO <= v < TEXT_HI):
                    continue
                if THUNK_FIRST <= v < THUNK_FIRST + 5 * THUNK_COUNT:
                    continue  # a thunk: always generated
                if not in_code(v):
                    patch_sites[v].append('%s:%d (outside the code ranges)' % (rel, n))
                    continue
                if v < GAME_HI and v not in starts and v not in registered:
                    patch_sites[v].append('%s:%d' % (rel, n))
                    continue
                if v >= GAME_HI and v not in insn:
                    patch_sites[v].append('%s:%d (not an instruction start)' % (rel, n))
                    continue
                if rel in KEY_FILES and not call:
                    continue  # a table key / range bound
                hook_refs[v].append((rel, call))
    for a, files in registered.items():
        flags[a] |= F_REG | F_FUNC
    # A game-range constant registered somewhere (but found before its registration was seen) is a reference too.
    for v in list(patch_sites):
        if v in registered and v < GAME_HI:
            hook_refs[v].extend((p.split(':')[0], False) for p in patch_sites.pop(v) if p.split(':')[0] not in KEY_FILES)
    for a, uses in hook_refs.items():
        for rel, call in uses:
            flags[a] |= F_SELF if call and rel in registered.get(a, set()) else F_HOOK

    # ---- entries and their room
    addrs = sorted(flags)
    rows = []
    stats = collections.Counter()
    for k, a in enumerate(addrs):
        nxt = addrs[k + 1] if k + 1 < len(addrs) else None
        hi = next(h for lo, h in CODE_RANGES if lo <= a < h)
        room = min(nxt if nxt is not None and nxt < hi else hi, hi) - a
        room = min(room, 255)
        rows.append((a, room, flags[a]))
        stats['entries'] += 1
        if room < 5:
            stats['room<5'] += 1
        for bit, nm in FLAG_NAMES:
            if flags[a] & bit:
                stats[nm] += 1
    tight = [(a, r, f) for a, r, f in rows if r < 5]

    def fl(f):
        return '|'.join(nm for bit, nm in FLAG_NAMES if f & bit)

    with open(out, 'w', newline='\n') as o:
        o.write('// Generated by re/gen_code_map.py - do not edit. Code map for hook/codegen.cpp ([Loader] Code=generated,\n'
                '// STANDALONE.md S5). Regenerate (from the repository root):\n'
                '//   python3 re/gen_code_map.py <joan.exe> <joan.asm> <functions.json> <thunks.json> hook hook/code_map.inc\n')
        o.write('// %d thunks; %d entries (%s); %d data references into the code ranges.\n' % (
            THUNK_COUNT, len(rows), ', '.join('%s %d' % (nm, stats[nm]) for _, nm in FLAG_NAMES), data_refs))
        o.write('// %d entries have less than 5 bytes before the next one (left int3):\n' % len(tight))
        for a, r, f in tight:
            o.write('//   0x%08x room %d %s\n' % (a, r, fl(f)))
        o.write('// Constants of our C++ inside .text that are not code references (byte-patch sites, operands): %d\n'
                % len(patch_sites))
        for v in sorted(patch_sites):
            o.write('//   0x%08x %s\n' % (v, ', '.join(sorted(set(patch_sites[v]))[:3])))
        o.write('#define JOAN_CODE_THUNK_FIRST 0x%08x\n#define JOAN_CODE_THUNK_COUNT %d\n' % (THUNK_FIRST, THUNK_COUNT))
        o.write('#ifdef JOAN_CODE_THUNKS\n')
        for i in range(0, THUNK_COUNT, 8):
            o.write(' '.join('0x%08x,' % t for t in targets[i:i + 8]) + '\n')
        o.write('#endif\n#ifdef JOAN_CODE_ENTRIES\n')
        for a, r, f in rows:
            o.write('{0x%08x, %d, 0x%02x},\n' % (a, r, f))
        o.write('#endif\n')
    print('%d thunks, %d entries -> %s (%d extra entries from %s)' % (THUNK_COUNT, len(rows), out, len(extra),
                                                                       extra_path))
    for _, nm in FLAG_NAMES:
        print('  %-6s %d' % (nm, stats[nm]))
    print('  room<5 %d, data refs %d, patch-site constants %d' % (len(tight), data_refs, len(patch_sites)))
    for a, r, f in tight:
        print('  tight 0x%08x room %d %s' % (a, r, fl(f)))
    sel = sorted(a for a, f in flags.items() if f & F_SELF and not f & (F_HOOK | F_DATA))
    print('  SELF-only references: %d' % len(sel))


if __name__ == '__main__':
    main()
