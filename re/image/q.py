#!/usr/bin/env python3
"""Query for the data-image typing review (docs/X64.md image).
  q.py sym <name>        symbol: declared type/section/address, neighbours, binary facts (re/image/out/gfacts.json),
                         C++ uses (src/**, `d::<name>`), layouts.json keys naming it
  q.py list <group>      the symbols of a review group (re/image/groups.json)
"""
import json, os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.dirname(os.path.dirname(HERE))
G = json.load(open(os.path.join(HERE, 'out', 'gfacts.json')))
SYMS = []
for line in open(os.path.join(ROOT, 'src', 'data', 'data_symbols.inc')):
    m = re.match(r'JOAN_DATA_SYMBOL\((\w+), ([^,]+), (\w+), (0x[0-9a-f]+)u\)', line)
    if m: SYMS.append((int(m.group(4), 16), m.group(3), m.group(2), m.group(1)))
SYMS.sort(); IDX = {s[1]: i for i, s in enumerate(SYMS)}
DNAME = {}  # symbol (data_symbols name) -> d:: names used by the C++ (src/data_names.hpp)
for line in open(os.path.join(ROOT, 'src', 'data_names.hpp')):
    m = re.search(r'Sym<.*?> (\w+)\{0x([0-9a-f]+)u\};\s*//.*? (\w+)$', line.strip())
    if m: DNAME.setdefault(m.group(3), []).append(m.group(1))
def sym(name):
    i = IDX.get(name)
    if i is None: sys.exit('unknown symbol')
    a, n, t, sec = SYMS[i]
    print(f'{n}: {sec} {t} at {a:#x}; next symbol {SYMS[i+1][1]} at {SYMS[i+1][0]:#x} (gap {SYMS[i+1][0]-a:#x})' if i + 1 < len(SYMS) else '')
    print('previous:', ', '.join(f'{s[1]}@{s[0]:#x}' for s in SYMS[max(0, i-3):i]))
    f = G.get(n)
    if f:
        for k in ('direct', 'deref', 'this_of', 'ptr_this_of'):
            if f[k]: print(f'{k}:', ', '.join(f'{x} x{c}' for x, c in sorted(f[k].items(), key=lambda kv: -kv[1])[:40]))
        print('pushed as address', f['pushed_addr'], 'pushed as value', f['pushed_val'])
    names = DNAME.get(n, [n])
    print('C++ names:', ', '.join('d::' + x for x in names))
    out = subprocess.run(['grep', '-rnE', '--include=*.cpp', '--include=*.hpp', r'd::(' + '|'.join(names) + r')\b', os.path.join(ROOT, 'src')], capture_output=True, text=True).stdout.split('\n')
    out = [l for l in out if l and '/src/data/' not in l and 'data_names.hpp' not in l]
    print(f'C++ uses: {len(out)}')
    for l in out[:25]: print('  ' + os.path.relpath(l.split(':')[0], ROOT) + ':' + ':'.join(l.split(':')[1:])[:200].strip())
def lst(g):
    for n in json.load(open(os.path.join(HERE, 'groups.json')))[g]: print(n)
{'sym': sym, 'list': lst}[sys.argv[1]](*sys.argv[2:])
