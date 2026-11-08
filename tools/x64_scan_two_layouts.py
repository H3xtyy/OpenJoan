#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): advisory: replacements whose body views `this` through a layout other than the class whose facade declares the
address. The facade class labels are often wrong, so this lists many false positives; it was the starting point of the
2026-10-30 view-class sweep (re/x64_census/reports/2026-10-30-views-*.md). Usage: python3 tools/x64_scan_two_layouts.py

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, pathlib, json, collections, sys
gen = pathlib.Path('src/original/gen')
addr_class = {}
for f in gen.glob('*_methods.inc'):
    cls = None
    for hpp in (gen / (f.name[:-len('_methods.inc')] + '.hpp'),):
        if hpp.exists():
            m = re.search(r'^struct (\w+)', hpp.read_text(), re.M); cls = m.group(1) if m else None
    if not cls: continue
    for m in re.finditer(r'/// (0x[0-9a-f]{8}) ', f.read_text()):
        addr_class.setdefault(int(m.group(1), 16), set()).add(cls)
L = json.load(open('re/layout/layouts.json'))
items = L['classes'] if isinstance(L, dict) and 'classes' in L else L
items = list(items.values()) if isinstance(items, dict) else items
base = {c['class']: c.get('base') for c in items}
def chain(c):
    out = [c]
    while base.get(c): c = base[c]; out.append(c)
    return out
def related(a, b): return a in chain(b) or b in chain(a)
reg_re = re.compile(r'^\s*[A-Z][A-Za-z0-9_]*\("([^"]+)",\s*(0x[0-9a-fA-F]+)u?,\s*&(?:H|\w+)::(\w+)\)', re.M)
hits = collections.Counter(); rows = []
for f in sorted(pathlib.Path('src').rglob('*.cpp')):
    s = str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    t = f.read_text(errors='replace')
    for m in reg_re.finditer(t):
        name, addr, fn = m.group(1), int(m.group(2), 16), m.group(3)
        # method body: from the definition of fn to the next member definition / struct end (approximate)
        d = re.search(r'\n    [^\n]*\b' + fn + r'\(([^)]*)\)\s*(?:const\s*)?(?:->\s*[^{]*)?\{', t)
        if not d: continue
        body = t[d.end(): d.end() + 20000]
        end = re.search(r'\n    \S[^\n]*\)\s*(?:const\s*)?\{|\n\};', body)
        if end: body = body[:end.start()]
        views = set(re.findall(r'jo::self<jo::(\w+)>\(this\)', body))
        facade = addr_class.get(addr)
        if not views or not facade: continue
        for v in views:
            if not any(related(v, fc) for fc in facade):
                rows.append((s, t[:m.start()].count('\n') + 1, name, f'{addr:#010x}', v, '/'.join(sorted(facade))))
for r in rows: print('%s:%d %s %s  this viewed as %s, facade class %s' % r)
print(len(rows), 'mismatches', file=sys.stderr)
