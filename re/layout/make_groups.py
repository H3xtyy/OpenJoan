#!/usr/bin/env python3
"""Splits the class labels (and unlabelled thiscall functions) of re/layout/out/db.json into review groups by
source file (docs/X64.md): re/layout/groups/gNN.json = {"classes": [...], "unlabelled": [...], "files": [...]}."""
import collections, json, os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
N = int(sys.argv[1]) if len(sys.argv) > 1 else 18
d = json.load(open(os.path.join(HERE, 'out', 'db.json'))); F, C = d['functions'], d['classes']
def weight(fns): return sum(len(F[a]['asm']) + len(F[a]['cpp']) for a in fns) + len(fns)
by_file = collections.defaultdict(lambda: {'classes': [], 'unlabelled': [], 'w': 0})
for c, v in C.items():
    if not any(F[a]['asm'] or F[a]['cpp'] for a in v['functions']):
        continue
    files = collections.Counter(F[a]['file'] or '?' for a in v['functions'])
    f = files.most_common(1)[0][0]
    if f.startswith('lib/'):
        continue  # CRT / D3DX / jpeg: replaced in A5, not typed
    by_file[f]['classes'].append(c); by_file[f]['w'] += weight(v['functions'])
for a, e in F.items():
    if not e['class'] and e['thiscall'] and e['asm'] and not (e['file'] or '').startswith('lib/'):
        f = e['file'] or '?'
        by_file[f]['unlabelled'].append(a); by_file[f]['w'] += weight([a])
files = sorted(by_file)  # path order keeps subsystems and neighbouring files together
total = sum(by_file[f]['w'] for f in files); target = total / N
groups, cur, w = [], {'classes': [], 'unlabelled': [], 'files': []}, 0
for f in files:
    if w >= target and len(groups) < N - 1:
        groups.append(cur); cur, w = {'classes': [], 'unlabelled': [], 'files': []}, 0
    cur['classes'] += by_file[f]['classes']; cur['unlabelled'] += by_file[f]['unlabelled']; cur['files'].append(f)
    w += by_file[f]['w']
groups.append(cur)
os.makedirs(os.path.join(HERE, 'groups'), exist_ok=True)
for i, g in enumerate(groups, 1):
    g['weight'] = sum(weight(C[c]['functions']) for c in g['classes']) + weight(g['unlabelled'])
    json.dump(g, open(os.path.join(HERE, 'groups', f'g{i:02d}.json'), 'w'), indent=1)
    print(f'g{i:02d}: {len(g["classes"])} classes, {len(g["unlabelled"])} unlabelled, weight {g["weight"]}, files {g["files"][0]} .. {g["files"][-1]} ({len(g["files"])})')
