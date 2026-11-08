#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): raw hex offsets applied to byte pointers - `p[0x124]`, `p + 0x50`,
`at<T>(p, 0x..)` style reads of a game object through a `u8*` / `char*` variable - the form the hazard meter's
regexes do not count and that hid Hud::level_c3 (hud.cpp p[0x124]) and Plane::copy_extras (plane.cpp o[0x50]).
A hit is a hazard when the pointer is a game object (its layout widens on x64); it is noise when the pointer is a
file / record buffer read by value. Usage: python3 tools/x64_scan_raw_index.py [files...]  (default: all of src/).

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, sys, pathlib
files = [pathlib.Path(a) for a in sys.argv[1:]] or [p for p in pathlib.Path('src').rglob('*.cpp') if '/gen/' not in str(p) and '/data/' not in str(p)]
ptr_decl = re.compile(r'\b(?:const\s+)?(?:u8|std::uint8_t|unsigned char|char|i8|std::int8_t)\s*\*\s*(?:const\s+)?([A-Za-z_]\w*)\b')
index_use = re.compile(r'(?<![\w\]\)])([A-Za-z_]\w*)\[(0x[0-9a-fA-F]{2,4})\]')
arith_use = re.compile(r'(?<![\w\]\)])([A-Za-z_]\w*)\s*\+\s*(0x[0-9a-fA-F]{2,4})\b')
cast_use = re.compile(r'reinterpret_cast<(?:const\s+)?(?:std::uint8_t|std::int8_t|u8|i8|char|unsigned char)\s*\*>\(([^()]*)\)\s*(\[(0x[0-9a-fA-F]{2,4})\]|\+\s*(0x[0-9a-fA-F]{2,4}))')
rows = []
for f in files:
    t = f.read_text(errors='replace')
    ptrs = set(ptr_decl.findall(t))
    for i, line in enumerate(t.split('\n'), 1):
        code = line.split('//')[0]
        if re.search(r'\b(?:u8|char|std::uint8_t|std::int8_t|i8)\s+[A-Za-z_]\w*\[0x[0-9a-fA-F]+\]', code): continue  # an array declaration, not an offset
        for m in index_use.finditer(code):
            if m.group(1) in ptrs: rows.append((str(f), i, f'{m.group(1)}[{m.group(2)}]', code.strip()[:110]))
        for m in cast_use.finditer(code):
            rows.append((str(f), i, f'reinterpret_cast<u8*>({m.group(1)}) {m.group(2)}', code.strip()[:110]))
        for m in arith_use.finditer(code):
            if m.group(1) in ptrs and int(m.group(2), 16) >= 0x10: rows.append((str(f), i, f'{m.group(1)} + {m.group(2)}', code.strip()[:110]))
for r in rows: print('%s:%d  %-18s %s' % r)
print(len(rows), 'raw offsets on byte pointers', file=sys.stderr)
