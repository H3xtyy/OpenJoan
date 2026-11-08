#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): a call site that turns the 32-bit result of a generated facade into a
pointer, `arg::as<T*>(joan::orig::as_<class>(p)->method(...))` (also `as<void*>` / `as<std::uintptr_t>` / `as<u8*>`),
or a free function `joan::orig::name(...)`, where the facade declares a 32-bit integer result. The facade's
`thiscall<std::uint32_t>` has already cut the pointer: fine while every address fits 32 bits, wrong above 4 GB
(Cell::get_9c 0x0054f890). Fix: retype the facade result (re/facades/retype_phase2.json, "param": "ret").

    python3 tools/x64_scan_u32_to_ptr.py [paths...]     (default: src without src/original/gen and src/data)
"""
import pathlib
import re
import sys

SRC = pathlib.Path(__file__).resolve().parent.parent / 'src'
GEN = SRC / 'original' / 'gen'

# facade return types per (accessor class file stem, method) and for free functions
decl = re.compile(r'^/// (0x[0-9a-f]{8}) \S+\n\s*(?:static\s+|inline\s+)*([\w:<>\* ]+?)\s+(\w+)\(', re.M)
methods = {}  # (stem, name) -> [(ret, addr)]
for f in GEN.glob('*_methods.inc'):
    stem = f.name[:-len('_methods.inc')]
    for m in decl.finditer(f.read_text(errors='replace')):
        methods.setdefault((stem, m.group(3)), []).append((m.group(2).strip(), m.group(1)))
free = {}
for f in (GEN / 'free_functions.hpp',):
    if f.exists():
        for m in decl.finditer(f.read_text(errors='replace')):
            free.setdefault(m.group(3), []).append((m.group(2).strip(), m.group(1)))

int32 = re.compile(r'^(?:std::)?(?:u?int32_t|u32|i32)$')
site = re.compile(r'arg::as<\s*(?:const\s+)?(?:[\w:]+\s*\*+|(?:std::)?uintptr_t)\s*>\(\s*(?:joan::orig::|::joan::orig::)'
                  r'(?:as_(\w+)\([^;]*?\)->(\w+)\(|(\w+)\()')
roots = [pathlib.Path(p) for p in sys.argv[1:]] or [SRC]
hits = 0
for root in roots:
    for f in sorted(root.rglob('*.cpp')):
        s = str(f)
        if '/original/gen/' in s or '/data/' in s:
            continue
        for i, line in enumerate(f.read_text(errors='replace').split('\n'), 1):
            for m in site.finditer(line):
                if m.group(1):
                    cands = methods.get((m.group(1), m.group(2)), [])
                    what = f'{m.group(1)}::{m.group(2)}'
                else:
                    if m.group(3) in ('arg',):
                        continue
                    cands = free.get(m.group(3), [])
                    what = m.group(3)
                bad = [(r, a) for r, a in cands if int32.match(r)]
                if bad:
                    print(f'{s}:{i}: {what} ({", ".join(a for _, a in bad)}) returns {bad[0][0]} | {line.strip()[:110]}')
                    hits += 1
print(f'{hits} site(s)', file=sys.stderr)
