#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): output arguments of facade calls that are 4-byte locals while the replacement
writes a pointer through them - `u32 extra = 0; ...->select_mesh(cam, 1, &plane, &extra)` where the replacement takes
`void** out_extra`: the 8-byte store on x64 clobbers the neighbouring local (Actor::draw lost every static plane on
2026-10-30). Usage: python3 tools/x64_scan_out_params.py   (run from the repository root).

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, pathlib, sys
gen = pathlib.Path('src/original/gen')
facades = {}   # method name -> [(class, addr, nparams)]
for f in list(gen.glob('*_methods.inc')) + [gen / 'free_functions.hpp']:
    cls = f.name[:-len('_methods.inc')] if f.name.endswith('_methods.inc') else 'free'
    lines = f.read_text().splitlines()
    for i, l in enumerate(lines):
        m = re.match(r'/// (0x[0-9a-f]{8}) ', l)
        if not m: continue
        j = i + 1
        while j < len(lines) and lines[j].startswith('template'): j += 1
        if j >= len(lines): continue
        d = re.match(r'\s*(?:inline\s+)?(?:.+?)\s+(\w+)\(([^)]*)\)', lines[j])
        if not d: continue
        facades.setdefault(d.group(1), []).append((cls, int(m.group(1), 16)))
reg_re = re.compile(r'^\s*[A-Z][A-Za-z0-9_]*\((?:"[^"]*",\s*)?"([^"]+)",\s*(0x[0-9a-fA-F]+)u?,\s*&(?:(\w+)::)?(\w+)\)', re.M)  # B("name", addr, &fn) and JOAN_REPLACE("module", "name", addr, &fn)
impl = {}   # addr -> [param types]
files = {}
for f in sorted(pathlib.Path('src').rglob('*.cpp')):
    s = str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    t = f.read_text(errors='replace'); files[s] = t
    for m in reg_re.finditer(t):
        addr, fn = int(m.group(2), 16), m.group(4)
        d = re.search(r'\n\s*(?:static\s+)?[\w:<>\*&\s]+?\s+(?:JOAN_\w+\s+)?' + re.escape(fn) + r'\(([^)]*)\)\s*(?:const\s*)?(?:->[^{]*)?\{', t)
        if d: impl[addr] = [p.strip() for p in d.group(1).split(',') if p.strip()]
small = re.compile(r'\b(?:u32|i32|std::uint32_t|std::int32_t|int|unsigned|DWORD|LONG|i16|u16|std::int16_t|std::uint16_t)\s+(?:const\s+)?([A-Za-z_]\w*)\s*(?:=\s*[^,;]+)?(?:,\s*([A-Za-z_]\w*)\s*(?:=\s*[^,;]+)?)*\s*;')
addr_arg = re.compile(r'&\s*([A-Za-z_]\w*)\b')
def split_args(text, start):
    depth, args, cur, i = 0, [], '', start
    while i < len(text):
        c = text[i]
        if c == '-' and text[i + 1:i + 2] == '>':  # a member arrow is not a template close
            cur += '->'; i += 2; continue
        if c in '([{<': depth += 1
        elif c in ')]}>':
            if depth == 0: args.append(cur); return args
            depth -= 1
        if c == ',' and depth == 0: args.append(cur); cur = ''
        else: cur += c
        i += 1
    return args
rows = set()
call_re = re.compile(r'->(\w+)\(')
for s, t in files.items():
    # small integer locals of the file (name -> set of line numbers of declarations)
    decls = {}
    for dm in re.finditer(r'\b(?:u32|i32|std::uint32_t|std::int32_t|int|unsigned|i16|u16|std::int16_t|std::uint16_t)\s+(?:const\s+)?([A-Za-z_]\w*(?:\s*=\s*[^,;()]+)?(?:\s*,\s*[A-Za-z_]\w*(?:\s*=\s*[^,;()]+)?)*)\s*;', t):
        for name in re.findall(r'([A-Za-z_]\w*)\s*(?:=|,|$)', dm.group(1) + ','):
            decls.setdefault(name, set()).add(t[:dm.start()].count('\n') + 1)
    for m in call_re.finditer(t):
        cands = facades.get(m.group(1))
        if not cands: continue
        args = split_args(t, m.end())
        line = t[:m.start()].count('\n') + 1
        for pos, a in enumerate(args):
            am = addr_arg.search(a)
            if not am or am.group(1) not in decls: continue
            if not any(0 <= line - dl <= 40 for dl in decls[am.group(1)]): continue
            for cls, addr in cands:
                params = impl.get(addr)
                if not params or pos >= len(params): continue
                ptype = re.sub(r'\s*\w+$', '', params[pos]).replace('const ', '').strip()
                if ptype.endswith('**') or re.search(r'\bvoid\s*\*\s*\*', ptype) or ptype.endswith('*&'):
                    rows.add((s, line, f'{cls}::{m.group(1)}', f'{addr:#x}', am.group(1), ptype))
for r in sorted(rows): print('%s:%d %s %s  &%s (a 4-byte local) -> impl param %s' % r)
print(len(rows), 'hits', file=sys.stderr)
