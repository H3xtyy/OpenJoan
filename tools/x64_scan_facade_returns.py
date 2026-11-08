#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): generated facades (src/original/gen/*_methods.inc, free_functions.hpp) that
return a 32-bit integer for an original address whose registered replacement returns a pointer (or std::uintptr_t).
The facade's `thiscall<std::uint32_t>` cuts the pointer: harmless while every address of the process fits 32 bits,
a crash once the heap lies above 4 GB (Cell::get_9c 0x0054f890, found when the deterministic heap moved high).

    python3 tools/x64_scan_facade_returns.py

Output: one line per facade: address, facade, its return type, the replacement's return type and where it is
defined. Advisory: check every hit against the code (a replacement may return a pointer the callers never use).
"""
import pathlib
import re
import sys

SRC = pathlib.Path(__file__).resolve().parent.parent / 'src'

# facades: "/// 0xADDR Name" followed by a declaration "RET name(...) { return ...<RET>(::joan::fn::X, ..."
facade_rx = re.compile(r'^/// (0x[0-9a-f]{8}) (\S+)\n([^\n]*?)\b(\w+)\(([^\n]*?)\)\s*(?:const\s*)?\{[^\n]*?::joan::fn::(\w+)', re.M)
facades = {}
for f in sorted((SRC / 'original' / 'gen').glob('*')):
    if f.suffix not in ('.inc', '.hpp'):
        continue
    for m in facade_rx.finditer(f.read_text(errors='replace')):
        ret = m.group(3).strip()
        for kw in ('static ', 'inline ', 'auto '):
            ret = ret.replace(kw, '')
        facades.setdefault(int(m.group(1), 16), []).append((f.name, m.group(4), ret.strip()))

# registrations: MACRO("Name", 0xADDR, &H::method) or JOAN_REPLACE*(module, "Name", 0xADDR, &fn)
reg_rx = re.compile(r'\b[A-Z][A-Z0-9_]*\(\s*(?:"[^"]*",\s*)?"([^"]+)",\s*(0x[0-9a-fA-F]+)u?,\s*&\s*([\w:]+)\s*\)')
impls = {}
for f in sorted(SRC.rglob('*.cpp')):
    s = str(f)
    if '/original/gen/' in s or '/data/' in s:
        continue
    text = f.read_text(errors='replace')
    for m in reg_rx.finditer(text):
        addr = int(m.group(2), 16)
        name = m.group(3).split('::')[-1]
        # the definition: "<ret> name(" at the start of a line (members of H or free functions)
        d = re.search(r'^[ \t]*((?:static\s+|inline\s+|constexpr\s+)*[\w:<>,\s\*&]+?)\s+(?:JOAN_\w+\s+)?' + re.escape(name) + r'\s*\(',
                      text, re.M)
        if d:
            impls[addr] = (str(f.relative_to(SRC.parent)), name, ' '.join(d.group(1).split()))

ptr_ret = re.compile(r'\*\s*$|uintptr_t$')
int32 = re.compile(r'^(?:std::)?(?:u?int32_t|u32|i32|unsigned|int|unsigned int|long|unsigned long)$')
hits = 0
for addr in sorted(facades):
    if addr not in impls:
        continue
    file, name, iret = impls[addr]
    iret_clean = re.sub(r'\b(?:static|inline|constexpr|const)\b', '', iret).strip()
    if not ptr_ret.search(iret_clean):
        continue
    for gf, fname, fret in facades[addr]:
        if int32.match(fret.replace('const', '').strip()):
            print(f'0x{addr:08x} {gf}:{fname} returns {fret}; replacement {file}:{name} returns {iret_clean}')
            hits += 1
print(f'{hits} facade(s) cut a pointer result to 32 bits', file=sys.stderr)
