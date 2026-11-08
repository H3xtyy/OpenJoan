#!/usr/bin/env python3
"""Untyped pointer fields (docs/X64.md): for each class, the fields that the binary dereferences but whose spec
type is not `ptr:<Class>` (ptr:void, u8*, an integer, or not declared), with what is read through them, the labels of
functions called with ecx = that field, and candidate classes whose layout covers every offset read.

    re/layout/pointees.py [--chunk A|B|C|D] [--class Name] [--json out.json]
"""
import argparse
import collections
import glob
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import check_spec as K  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--chunk')
    ap.add_argument('--class', dest='cls')
    ap.add_argument('--json')
    a = ap.parse_args()
    classes, owner, fn_map, _ = K.load_specs(sorted(glob.glob(os.path.join(HERE, 'spec', '*.json'))))
    L = K.Layout(classes)
    mine = None
    if a.chunk:
        mine = set(json.load(open(os.path.join(HERE, 'groups', 'round3.json')))[a.chunk])
    members = collections.defaultdict(list)
    for addr, e in K.F.items():
        c = fn_map.get(addr, e['class']) if addr in fn_map else e['class']
        if c:
            members[c].append(addr)
    bytemaps = {n: L.bytes_of(n) for n in classes}
    out = {}
    for name in sorted(classes):
        if (mine is not None and name not in mine) or (a.cls and name != a.cls):
            continue
        reads = collections.defaultdict(set)   # field off -> {(off, width)}
        calls = collections.defaultdict(collections.Counter)
        for addr in members.get(name, []):
            e = K.F[addr]
            for path, off, width, kind, n in e['asm']:
                if len(path) == 1 and width:
                    reads[path[0]].add((off, width))
            for path, add, t, n in e['calls']:
                if len(path) == 1:
                    lab = K.F.get(f'{t:#x}', {}).get('class')
                    lab = fn_map.get(f'{t:#x}', lab)
                    calls[path[0]][(add, lab)] += n
        rows = []
        for fo in sorted(set(reads) | set(calls)):
            t = L.field_at(name, fo)
            if t and t.startswith('ptr:') and t[4:] not in ('void', 'fn', ''):
                continue
            rs = sorted(reads.get(fo, ()))
            cands = []
            if rs:
                for cn, bm in bytemaps.items():
                    if bm and all(all(b in bm for b in range(o, o + w)) for o, w in rs):
                        cands.append((len(bm), cn))
                cands = [c for _, c in sorted(cands)[:6]]
            rows.append({'field': f'{fo:#x}', 'declared': t, 'reads': [f'{o:#x}/{w}' for o, w in rs][:24],
                         'calls': [f'+{ad:#x}->{lab}' for (ad, lab), n in calls[fo].most_common(6)],
                         'covering_classes': cands})
        if rows:
            out[name] = rows
    if a.json:
        json.dump(out, open(a.json, 'w'), indent=1)
    for name, rows in out.items():
        print(f'## {name} ({owner.get(name)})')
        for r in rows:
            print(f"  +{r['field']} declared={r['declared']} reads={' '.join(r['reads'])} calls={r['calls']} "
                  f"covering={r['covering_classes']}")
    print(f'\n{sum(len(r) for r in out.values())} untyped dereferenced pointer fields in {len(out)} classes')


if __name__ == '__main__':
    main()
