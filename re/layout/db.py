#!/usr/bin/env python3
"""Struct recovery, step 3 (docs/X64.md): one database of everything known per original function and per class
label, for the layout review (re/layout/q.py reads it).

Inputs: the extract.py JSON lines (C++ view: accessed types, facade casts, registrations), re/layout/out/asm_facts.json
(binary view: widths, pointer chains, same-this calls, vtable stores, new+ctor sizes), re/classes.csv (labels),
re/attribution.csv (original source file).

    re/layout/db.py <dir with *.jsonl> [-o re/layout/out/db.json]
"""
import argparse
import collections
import csv
import glob
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
PH = re.compile(r'^(Obj|X|Y|H|Rec|Record|Item|Entry|Node|Thing|Holder|Self|Base|Pair|Ptr|Ref|Cur|Iter|Unknown|Global|'
                r'Static\w*)$|^(Obj|X)[0-9A-Za-z_]+$')


def placeholder(c):
    if not c or PH.search(c):
        return True
    m = re.search(r'[0-9a-f]{3,}$', c)
    return bool(m and re.search(r'[0-9]', m.group(0)) and not re.search(r'[23][dD]$', c))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dir')
    ap.add_argument('-o', default=os.path.join(HERE, 'out', 'db.json'))
    a = ap.parse_args()
    recs = [json.loads(line) for p in sorted(glob.glob(os.path.join(a.dir, '*.jsonl'))) for line in open(p)]
    facts = {int(k, 16): v for k, v in json.load(open(os.path.join(HERE, 'out', 'asm_facts.json'))).items()}
    labels = {}
    for r in csv.DictReader(open(os.path.join(ROOT, 're', 'classes.csv'))):
        labels[int(r['address'], 16)] = (r['class'], r['evidence'], r['name'])
    attrib = {int(r['address'], 16): r for r in csv.DictReader(open(os.path.join(ROOT, 're', 'attribution.csv')))}

    fns = {r['usr']: r for r in recs if r['k'] == 'fn'}
    by_fn_acc = collections.defaultdict(list)
    accs = {}
    for r in recs:
        if r['k'] == 'acc':
            by_fn_acc[r['fn']].append(r)
            accs[r['id']] = r
    casts = collections.defaultdict(list)
    for r in recs:
        if r['k'] == 'cast':
            casts[r['fn']].append(r)

    def cpp_path(acc, depth=0):
        """Asm-style path + offset of a C++ access (this-relative), or None."""
        if acc['off'] is None or depth > 6:
            return None
        b = acc['base']
        add = b.get('add', 0)
        if b['b'] == 'this':
            return ((), acc['off'] + add)
        if b['b'] == 'acc' and b['id'] in accs:
            p = cpp_path(accs[b['id']], depth + 1)
            if p is not None:
                return (p[0] + (p[1],), acc['off'] + add)
        return None

    db = {'functions': {}, 'classes': collections.defaultdict(lambda: {'functions': [], 'sizes': set(), 'vtables': set()})}
    reg_by_addr = {}
    for r in recs:
        if r['k'] == 'reg' and r.get('addr') is not None:
            reg_by_addr.setdefault(r['addr'], r)
    all_addrs = set(reg_by_addr) | set(facts)
    callers = collections.defaultdict(list)
    for f, v in facts.items():
        for path, add, t, n in v['calls']:
            callers[t].append((f, path, add))
    sizes = collections.defaultdict(set)
    for f, v in facts.items():
        for sz, ctor in v['new']:
            sizes[ctor].add(sz)

    for addr in sorted(all_addrs):
        reg = reg_by_addr.get(addr)
        lab = labels.get(addr, ('', '', ''))
        name = reg['name'] if reg else lab[2]
        cls = lab[0] or (None if placeholder(name.split('::')[0]) or '::' not in name else name.split('::')[0])
        fn = fns.get(reg['fn']) if reg and reg.get('fn') else None
        cpp = collections.defaultdict(collections.Counter)
        cast_list = []
        if fn:
            for acc in by_fn_acc.get(fn['usr'], []):
                p = cpp_path(acc)
                if p is not None:
                    cpp[f"{'/'.join(hex(x) for x in p[0]) or 'this'}+{p[1]:#x}"][acc['type'] + (' w' if acc['w'] else '')] += 1
            for c in casts.get(fn['usr'], []):
                b = c['base']
                if b['b'] == 'this':
                    cast_list.append(['this', c['as']])
                elif b['b'] == 'acc' and b['id'] in accs:
                    p = cpp_path(accs[b['id']])
                    if p is not None:
                        cast_list.append([f"{'/'.join(hex(x) for x in p[0] + (p[1],))}", c['as']])
                elif b['b'] == 'param':
                    cast_list.append([f"param{b['i']}", c['as']])
        fa = facts.get(addr, {})
        entry = {
            'name': name, 'class': cls, 'label_evidence': lab[1],
            'file': (fn or {}).get('file') or (attrib.get(addr) or {}).get('current_file', ''),
            'line': (fn or {}).get('line'), 'original_file': (attrib.get(addr) or {}).get('original_file', ''),
            'thiscall': fa.get('thiscall', False),
            'asm': fa.get('acc', []),
            'calls': fa.get('calls', []),
            'vt': fa.get('vt', []),
            'new': fa.get('new', []),
            'callers': [[f'{c:#x}', list(p), ad] for c, p, ad in callers.get(addr, [])][:40],
            'sizes': sorted(sizes.get(addr, ())),
            'cpp': {k: dict(v) for k, v in cpp.items()},
            'casts': cast_list,
        }
        db['functions'][f'{addr:#x}'] = entry
        if cls:
            c = db['classes'][cls]
            c['functions'].append(f'{addr:#x}')
            c['sizes'] |= set(entry['sizes'])
            for p, ad, v in entry['vt']:
                if not p and ad == 0:
                    c['vtables'].add(f'{v:#x}')
    db['classes'] = {k: {'functions': v['functions'], 'sizes': sorted(v['sizes']), 'vtables': sorted(v['vtables'])}
                     for k, v in sorted(db['classes'].items())}
    json.dump(db, open(a.o, 'w'))
    print(f"{len(db['functions'])} functions, {len(db['classes'])} class labels")


if __name__ == '__main__':
    main()
