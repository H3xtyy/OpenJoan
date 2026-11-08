#!/usr/bin/env python3
"""re/image/spec/i*.json (keyed by data_symbols.inc names) -> re/image/image_types.json keyed by original address,
the input of re/gen_data_curated.py's typed .bss / data objects (names may change when the generator runs again;
addresses do not).  {"0xa39e20": {"type": "obj:NewsItem", "count": 1, "sym": "b_a39e20"},
                     "0xa39e21": {"part_of": "0xa39e20", "type": "u8", "sym": "b_a39e21"}, ...}
    re/image/normalize.py
"""
import json, os, re
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
syms = {}
for line in open(os.path.join(ROOT, 'src', 'data', 'data_symbols.inc')):
    m = re.match(r'JOAN_DATA_SYMBOL\((\w+), ([^,]+), (\w+), (0x[0-9a-f]+)u\)', line)
    if m:
        syms[m.group(3)] = int(m.group(4), 16)


def va(n):
    """address of a spec name: data_symbols.inc names end with their address (b_a39e20, kIob_a314a0); the spec was
    written against the names of the time, the generator may have renamed some since"""
    m = re.search(r'_([0-9a-f]{6})$', n)
    if m:
        return int(m.group(1), 16)
    return syms[n]


out = {}
for g in ('i1', 'i2', 'i3', 'i4'):
    for n, v in json.load(open(os.path.join(HERE, 'spec', f'{g}.json')))['symbols'].items():
        e = {'sym': n, 'group': g}
        for k in ('type', 'count', 'confidence'):
            if k in v:
                e[k] = v[k]
        if 'part_of' in v:
            e['part_of'] = f'{va(v["part_of"]):#x}'
        out[f'{va(n):#x}'] = e
json.dump(dict(sorted(out.items(), key=lambda kv: int(kv[0], 16))), open(os.path.join(HERE, 'image_types.json'), 'w'), indent=0)
print(len(out), 'symbols')
