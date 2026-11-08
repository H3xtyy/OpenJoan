#!/usr/bin/env python3
"""Generate src/runtime/code_slots.inc: the code slot table of joan_re.exe (docs/RUNTIME.md; consumer:
src/runtime/code_table.cpp). Replaces re/gen_code_map.py (stage S5's generated code range at 0x00401000).

Usage (from the repository root):
  gen_code_slots.py <joan.exe> <joan.asm> <functions.json> <thunks.json> <hook dir> <out code_slots.inc> [extra entries]

[extra entries] defaults to re/extra_entries.txt next to this script (when present): real function entries that
functions.json misses (code right after a jump table, hand-written asm routines without padding, library methods only
stored as `mov [x], offset fn`). First token of each non-# line is the address; they are listed as FUNC.

joan.exe's code no longer exists at run time: every original code address our C++ or joan.exe's data can still use is
a SLOT, and the slot holds what a call of that address reaches (src/runtime/code_table.cpp: joan_code[slot], filled by
registry::install_all / fixes / canvas exactly where stage S5 retargeted thunks and wrote entry jumps). The slots:
- 0 .. JOAN_CODE_NAMED_SLOTS - 1: the names of src/fn_names.hpp (re/gen_fn_names.py), in their order: `fn::Name` IS
  the slot index (fn::Code). This includes every incremental-link thunk (0x00401005 + 5 * i, 8015).
- then every other code ENTRY of the S5 code map, by address: every address inside 0x00414000..0x007d5000 /
  0x007dd410..0x007e50a0 (game + statically linked libraries, EH funclets) where code may still arrive. The code
  relocations of the data image (src/data/data_relocs.inc: vtables, __xc initialisers, callbacks) are thunks or such
  entries; joan_re resolves each of them through its slot after the replacements are installed.
Per slot: original address, the thunk's original target (thunk slots; 0 otherwise) and flags. The flags of a real
address are the S5 code map's, so the S5 entry rule (codegen_core.hpp entry_jumps_to_replacement, now
code_slots_core.hpp) carries over unchanged:
    FUNC   function start (functions.json, re/extra_entries.txt, or a registered replacement address)
    THUNK  a thunk jumps to it
    DATA   a relocated dword of .rdata / .data / .idata / .rsrc points at it (vtables, function pointer tables, CRT
           init tables, SEH scope tables, FuncInfo maps, ...)
    HOOK   a hex constant / fn:: name of our C++ (src/**) other than a registration address, in a file that does not
           register that address (calls by real address, kc_ constants, callbacks); in KEY_FILES only direct calls
           count (the rest are lookup keys / range bounds)
    SELF   a direct call of that address (thiscall / cdecl_call / original<> / ORIG ...) in the file that registers it:
           the fallback / verify call of the "untouched original body" by the replacement's own module. Any other use
           in that file (callbacks for atexit / qsort / bsearch, kc_ constants, template arguments) counts as HOOK
    REG    registered replacement address (JOAN_REPLACE* or a module macro of the form X("name", 0x...))
    ENTRY  joan.exe's entry point (CRT startup, called by the loader)
    TIGHT  (0x100) S5 left this entry int3: less than 5 bytes to the next one. Informational (a slot needs no room).
    SLOT_THUNK (0x80) the slot is an incremental-link thunk (its target field is valid)
Alias groups: code values our C++ compares (`v == fn::code_value(fn::X)`, re/S7_DESIGN.md 4.2) must stay distinct
after resolution; each group lists the slots whose values can meet in one comparison (the compared names, the names
stored to the same data address by our C++, the data image's initial value there). code_table.cpp checks them at
seal().
Names of src/fn_names.hpp (`fn::Name` / `joan::fn::Name`) and src/data_names.hpp (`d::Name`) count exactly like the
literal they stand for: each source line is scanned with the names expanded to their values (the two headers
themselves are not scanned).
Constants of our C++ in the game range that are not function starts are IDs (canvas size-read instructions, display
immediates inside replaced functions), never control flow: they are listed in the header, not slots.
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
# Range bounds used as plain numbers by patch.cpp / code_table.cpp (not code references).
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


F_TIGHT, F_SLOT_THUNK = 0x100, 0x80
CMP_RE = re.compile(r'\b([A-Za-z_]\w*)\s*(?:==|!=)\s*(?:joan::)?fn::code_value\s*\(\s*(?:joan::)?fn::([A-Za-z_]\w*)\s*\)')
LOAD_RE = re.compile(r'\b([A-Za-z_]\w*)\s*=\s*(?:hook::|img::|joan::img::)?global\s*<\s*(?:u32|std::uint32_t|std::uintptr_t)\s*>'
                     r'\s*\(\s*d::([A-Za-z_]\w*)\s*\)')
# {d::X, fn::code_value(fn::Y)} (a value table) or {d::X, 0, fn::Y} (game_b103_08 Store: the code column)
STORE_RE = re.compile(r'\{\s*d::([A-Za-z_]\w*)\s*,\s*(?:(?:joan::)?fn::code_value\s*\(\s*(?:joan::)?fn::([A-Za-z_]\w*)\s*\)'
                      r'|0\s*,\s*(?:joan::)?fn::([A-Za-z_]\w*))\s*\}')


def code_map(exe, asm, fjson, tjson, hook_dir, extra_path):
    """The S5 code map: thunk targets, [(address, room, flags)] of every entry, stats, patch-site constants, data
    reference count"""
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
    # src/data/ (re/gen_data_curated.py: joan.exe's typed .rdata / .data) and image_layout.inc / import_table.inc
    # (re/gen_data_image.py) are joan.exe's own data, layout and imports: their code pointers are the DATA references
    # above, the rest are not references at all.
    # fn_names.hpp (re/gen_fn_names.py) and data_names.hpp (re/gen_data_names.py) only define names: their uses are
    # expanded below.
    skip = {'code_map.inc', 'code_slots.inc', 'census_table.inc', 'image_layout.inc', 'import_table.inc',
            'fn_names.hpp', 'data_names.hpp'}
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
                    continue  # a thunk: always a slot
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
    return targets, rows, stats, patch_sites, data_refs, len(extra)


def alias_groups(hook_dir, slot_of_name, slot_of_va, data_names, data_relocs):
    """[(data name, [slot, ...])]: the code values that can meet in one comparison of our C++ (see the module
    docstring)"""
    loads = {}       # variable -> data name it was loaded from
    compared = collections.defaultdict(set)   # data name -> fn names
    stored = collections.defaultdict(set)
    for rel, path in fn_scan.hook_files(hook_dir):
        src = fn_scan.strip(open(path, errors='replace').read())
        file_loads = {m.group(1): m.group(2) for m in LOAD_RE.finditer(src)}
        for m in CMP_RE.finditer(src):
            d = file_loads.get(m.group(1))
            key = d if d is not None else '%s:%s' % (rel, m.group(1))
            compared[key].add(m.group(2))
        for m in STORE_RE.finditer(src):
            stored[m.group(1)].add(m.group(2) or m.group(3))
        loads.update(file_loads)
    groups = []
    for key in sorted(compared):
        names = set(compared[key]) | stored.get(key, set())
        slots = {slot_of_name[n] for n in names}
        va = data_names.get(key)
        if va is not None and va in data_relocs:
            slots.add(slot_of_va[data_relocs[va]])
        groups.append((key, sorted(slots)))
    return groups


def main():
    exe, asm, fjson, tjson, hook_dir, out = sys.argv[1:7]
    extra_path = sys.argv[7] if len(sys.argv) > 7 else os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                                                     'extra_entries.txt')
    targets, rows, stats, patch_sites, data_refs, n_extra = code_map(exe, asm, fjson, tjson, hook_dir, extra_path)
    entry_flags = {a: f | (F_TIGHT if r < 5 else 0) for a, r, f in rows}
    thunk_target = {THUNK_FIRST + 5 * i: t for i, t in enumerate(targets)}

    # ---- slots: the fn:: names first (their index is the name's value), then every other entry by address
    named = fn_scan.load_fn_slots(os.path.join(hook_dir, 'fn_names.hpp'))
    assert named, 'src/fn_names.hpp is not in the S7c-main form (re/gen_fn_names.py)'
    n_named = max(s for s, _ in named.values()) + 1
    slots = [None] * n_named      # (va, name)
    for name, (s, va) in named.items():
        assert slots[s] is None, ('slot used twice', s, name)
        slots[s] = (va, name)
    for s, e in enumerate(slots):
        if e is None:
            slots[s] = (0, '')   # free index (a name that is gone): never used, resolves to a trap
    named_vas = {va for va, _ in slots if va}
    for a, r, f in rows:
        if a not in named_vas:
            slots.append((a, ''))
    slot_of_va = {}
    for s, (va, _) in enumerate(slots):
        if va:
            slot_of_va.setdefault(va, s)
    for va, name in slots:
        if va and not fn_scan.is_thunk(va):
            assert va in entry_flags, ('fn:: name of an address that is not a code entry', name, hex(va))

    # ---- data image code relocations: every value needs a slot
    data_relocs = {}
    rel_re = re.compile(r'JOAN_DATA_RELOC\(0x([0-9a-f]+)u, code, 0x([0-9a-f]+)u\)')
    for m in rel_re.finditer(open(os.path.join(hook_dir, 'data', 'data_relocs.inc')).read()):
        loc, v = int(m.group(1), 16), int(m.group(2), 16)
        assert v in slot_of_va, ('code relocation without a slot', hex(loc), hex(v))
        data_relocs[loc] = v

    data_names = fn_scan.load_data_names(os.path.join(hook_dir, 'data_names.hpp'))
    groups = alias_groups(hook_dir, {n: s for n, (s, _) in named.items()}, slot_of_va, data_names, data_relocs)

    by_va = sorted((va, s) for s, (va, _) in enumerate(slots) if va)
    tight = [(a, r, f) for a, r, f in rows if r < 5]

    def fl(f):
        return '|'.join(nm for bit, nm in FLAG_NAMES if f & bit)

    n_thunk = sum(1 for va, _ in slots if fn_scan.is_thunk(va))
    with open(out, 'w', newline='\n') as o:
        o.write('// Generated by re/gen_code_slots.py - do not edit. Code slots of src/runtime/code_table.cpp (docs/RUNTIME.md\n'
                '// stage S7c). Regenerate (from the repository root) after module changes:\n'
                '//   python3 re/gen_code_slots.py <joan.exe> <joan.asm> <functions.json> <thunks.json> hook src/runtime/code_slots.inc\n')
        o.write('// %d slots: %d named (src/fn_names.hpp; %d thunks), %d other code entries. S5 code map: %d entries (%s);\n'
                '// %d data references into the code ranges; %d code relocations of the data image.\n' % (
                    len(slots), n_named, n_thunk, len(slots) - n_named, len(rows),
                    ', '.join('%s %d' % (nm, stats[nm]) for _, nm in FLAG_NAMES), data_refs, len(data_relocs)))
        o.write('// %d entries had less than 5 bytes before the next one (S5: int3; flag TIGHT):\n' % len(tight))
        for a, r, f in tight:
            o.write('//   0x%08x room %d %s\n' % (a, r, fl(f)))
        o.write('// Constants of our C++ inside .text that are not code references (IDs: size-read instructions, operands): %d\n'
                % len(patch_sites))
        for v in sorted(patch_sites):
            o.write('//   0x%08x %s\n' % (v, ', '.join(sorted(set(patch_sites[v]))[:3])))
        o.write('// Alias groups (code values compared by our C++; data address: slots):\n')
        for key, ss in groups:
            o.write('//   %s: %s\n' % (key, ' '.join('%d=0x%08x' % (s, slots[s][0]) for s in ss)))
        o.write('#define JOAN_CODE_SLOT_COUNT %d\n#define JOAN_CODE_NAMED_SLOTS %d\n' % (len(slots), n_named))
        o.write('#define JOAN_CODE_THUNK_FIRST 0x%08x\n#define JOAN_CODE_THUNK_COUNT %d\n' % (THUNK_FIRST, THUNK_COUNT))
        o.write('#ifdef JOAN_CODE_SLOTS\n// {original address, thunk target, flags}\n')
        for s, (va, name) in enumerate(slots):
            if fn_scan.is_thunk(va):
                f = F_SLOT_THUNK
                t = thunk_target[va]
            else:
                f = entry_flags.get(va, 0)
                t = 0
            o.write('{0x%08xu, 0x%08xu, 0x%04x},\n' % (va, t, f))
        o.write('#endif\n#ifdef JOAN_CODE_SLOT_NAMES\n')
        for s in range(n_named):
            o.write('"%s",\n' % slots[s][1])
        o.write('#endif\n#ifdef JOAN_CODE_SLOTS_BY_VA\n')
        for i in range(0, len(by_va), 12):
            o.write(' '.join('%d,' % s for _, s in by_va[i:i + 12]) + '\n')
        o.write('#endif\n#ifdef JOAN_CODE_ALIAS_GROUPS\n// slots of one group, then 0xFFFFFFFF\n')
        for key, ss in groups:
            o.write(' '.join('%du,' % s for s in ss) + ' 0xFFFFFFFFu,  // %s\n' % key)
        o.write('#endif\n')
    print('%d slots (%d named, %d thunks, %d other entries) -> %s (%d extra entries from %s)' % (
        len(slots), n_named, n_thunk, len(slots) - n_named, out, n_extra, extra_path))
    for _, nm in FLAG_NAMES:
        print('  %-6s %d' % (nm, stats[nm]))
    print('  room<5 %d, data refs %d, code relocs %d, patch-site constants %d, alias groups %d' % (
        len(tight), data_refs, len(data_relocs), len(patch_sites), len(groups)))
    for key, ss in groups:
        print('  alias group %s: %d slots' % (key, len(ss)))


if __name__ == '__main__':
    main()
