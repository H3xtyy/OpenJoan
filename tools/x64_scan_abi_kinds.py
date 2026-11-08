#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): facade declarations vs replacement signatures: a float/double parameter or return on one side and an integer on the
other (XMM vs integer register on x64), or a pointer (P: `T*`, uintptr_t) on one side and a 32-bit integer on the other (the
pointer is truncated above 4 GB: Unit::set_group took the World* as int32_t and the live x64 game crashed at mission start
while the replay, whose deterministic heap lies below 4 GB, passed). ARITY rows are mostly the scan matching another overload (a true arity
mismatch does not compile through the bridges); the KIND rows are the finds. Usage: python3 tools/x64_scan_abi_kinds.py
Found the four float arguments of 6670134.

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, pathlib, sys, collections
gen = pathlib.Path('src/original/gen')
facade = {}
def kinds(params):
    out = []
    params = re.sub(r'/\*.*?\*/', '', params)  # `u32 /*unused*/` is an integer, not a pointer
    for p in [x.strip() for x in params.split(',') if x.strip()]:
        if re.match(r'^\.\.\.$', p): out.append('V'); continue
        ty = re.sub(r'\b[a-zA-Z_]\w*$', '', p).strip()     # drop the name
        if ty == '': ty = p
        if re.search(r'\bT\d+\b', ty): out.append('T'); continue  # by-value struct template: skip
        if '*' in ty or re.search(r'\b(u?intptr_t|size_t|ptrdiff_t|\w+_ptr|\w+Handler|JSAMP\w+|HANDLE|LP\w+)\b', ty): out.append('P'); continue  # pointer-sized (incl. the CRT/JPEG typedefs)
        out.append('F' if re.search(r'^(const )?(float|double)$', ty.strip()) else 'I')
    return out
def rkind(ret):
    return 'F' if re.search(r'^(const )?(float|double)$', ret.strip()) else 'I'
for f in list(gen.glob('*_methods.inc')) + [gen / 'free_functions.hpp']:
    lines = f.read_text().splitlines()
    for i, l in enumerate(lines):
        m = re.match(r'/// (0x[0-9a-f]{8}) ', l)
        if not m: continue
        j = i + 1
        while j < len(lines) and (lines[j].startswith('template') or lines[j].strip() == ''): j += 1
        if j >= len(lines): continue
        d = re.match(r'\s*(?:inline\s+)?(.+?)\s+(\w+)\(([^)]*)\)', lines[j])
        if not d: continue
        facade.setdefault(int(m.group(1), 16), []).append((rkind(d.group(1)), kinds(d.group(3)), f.name))
reg_re = re.compile(r'^\s*[A-Z][A-Za-z0-9_]*\("([^"]+)",\s*(0x[0-9a-fA-F]+)u?,\s*&(?:(\w+)::)?(\w+)\)', re.M)
rows = []
for f in sorted(pathlib.Path('src').rglob('*.cpp')):
    s = str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    t = f.read_text(errors='replace')
    for m in reg_re.finditer(t):
        name, addr, fn = m.group(1), int(m.group(2), 16), m.group(4)
        d = re.search(r'\n\s*(?:static\s+)?([\w:<>\*&\s]+?)\s+(?:JOAN_\w+\s+)?' + re.escape(fn) + r'\(([^)]*)\)\s*(?:const\s*)?(?:->\s*([^{]*))?\{', t)
        if not d: continue
        ret = d.group(3) or d.group(1)
        impl = (rkind(ret), kinds(d.group(2)))
        for fr, fk, src in facade.get(addr, []):
            ik = impl[1]
            if 'T' in fk or 'V' in fk or 'V' in ik: continue
            # member facades have no `this`; implementations in struct H neither. Free functions: same list.
            if len(fk) != len(ik):
                rows.append((s, t[:m.start()].count('\n') + 1, name, f'{addr:#x}', 'ARITY', ''.join(fk), ''.join(ik), src)); continue
            if fk != ik or (fr != impl[0]):
                rows.append((s, t[:m.start()].count('\n') + 1, name, f'{addr:#x}', 'KIND', fr + ':' + ''.join(fk), impl[0] + ':' + ''.join(ik), src))
for r in rows: print('%s:%d %s %s %s facade %s impl %s (%s)' % r)
print(len(rows), 'mismatches', file=sys.stderr)
