#!/usr/bin/env python3
"""Struct recovery, step 2 (docs/X64.md): merges the per-file access records of extract.py into layouts.

Class of a function (`this`):
  1. a member of a typed record outside the per-block H (joan::orig::DynArrayB::linkin...) -> that record;
  2. its registration address in re/classes.csv (vtables / same-this calls / curated names);
  3. its registered name `Class::method` unless Class is a placeholder (Obj, X, ...);
  4. an unregistered member of an H whose registered members all have one class -> that class.
Layout keys:
  `Class`                    accesses through `this` (+ constant adjustments);
  `Class@0x1d`               the object a pointer field at 0x1d of Class points to (accesses through that field);
  `d::g_x`                   a data-image object (accesses through img::obj / global / gptr of d::g_x);
  `d::g_x@0x10`              ... and objects reached from it.
Facade casts (`as_file(f<void*>(0x1d))`) name the pointee: `Class@0x1d` is-a File.

    re/layout/aggregate.py <dir with *.jsonl> [-o re/layout/out]
Writes layouts.json (every key: fields by offset with types, widths, counts, writers), conflicts.txt and summary.md.
"""
import argparse
import collections
import csv
import glob
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.dirname(HERE))
PH = re.compile(r'^(Obj|X|Y|H|Rec|Record|Item|Entry|Node|Thing|Holder|Self|Base|Pair|Ptr|Ref|Cur|Iter|Unknown|Global|'
                r'Static\w*)$|^(Obj|X)[0-9A-Za-z_]+$')


def placeholder(c):
    if not c or PH.search(c):
        return True
    m = re.search(r'[0-9a-f]{3,}$', c)
    return bool(m and re.search(r'[0-9]', m.group(0)) and not re.search(r'[23][dD]$', c))


def class_of_name(name):
    if not name or '::' not in name:
        return None
    c = name.split('::')[0].split('<')[0]
    return None if placeholder(c) else c


def load(dirname):
    recs = []
    for p in sorted(glob.glob(os.path.join(dirname, '*.jsonl'))):
        for line in open(p):
            recs.append(json.loads(line))
    return recs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dir')
    ap.add_argument('-o', default=os.path.join(HERE, 'out'))
    a = ap.parse_args()
    os.makedirs(a.o, exist_ok=True)
    recs = load(a.dir)

    classes_csv = {}
    for r in csv.DictReader(open(os.path.join(ROOT, 're', 'classes.csv'))):
        if r['class']:
            classes_csv[int(r['address'], 16)] = r['class']

    regs = collections.defaultdict(list)  # fn usr -> registrations
    fns = {}
    accs = {}
    casts = []
    tus = []
    for r in recs:
        k = r['k']
        if k == 'reg' and r.get('fn'):
            regs[r['fn']].append(r)
        elif k == 'fn':
            fns.setdefault(r['usr'], r)
        elif k == 'acc':
            accs[r['id']] = r
        elif k == 'cast':
            casts.append(r)
        elif k == 'tu':
            tus.append(r)

    # --- class of each function -----------------------------------------------------------------------------------
    fn_class, fn_why = {}, {}
    for usr, f in fns.items():
        rec = f.get('record') or ''
        tail = rec.split('::')[-1] if rec else ''
        if rec and tail not in ('H', '(anon)') and not placeholder(tail) and not f.get('static'):
            fn_class[usr], fn_why[usr] = tail, 'record'
            continue
        for g in regs.get(usr, []):
            c = classes_csv.get(g['addr'])
            if c:
                fn_class[usr], fn_why[usr] = c, 'classes.csv'
                break
            c = class_of_name(g['name'])
            if c:
                fn_class[usr], fn_why[usr] = c, 'name'
                break
    by_record = collections.defaultdict(set)
    for usr, f in fns.items():
        if f.get('record'):
            by_record[(f['file'], f['record'])].add(usr)
    for key, members in by_record.items():
        cs = {fn_class[u] for u in members if u in fn_class and fn_why[u] != 'record'}
        if len(cs) == 1:
            c = next(iter(cs))
            for u in members:
                if u not in fn_class and not fns[u].get('static'):
                    fn_class[u], fn_why[u] = c, 'same H'

    # parameters named by a facade cast in the same function: as_file(dst) -> param 0 is a File
    param_class = {}
    for c in casts:
        b = c['base']
        if b['b'] == 'param' and not b.get('add'):
            param_class.setdefault((c['fn'], b['i']), collections.Counter())[c['as']] += 1
    param_class = {k: v.most_common(1)[0][0] for k, v in param_class.items() if len(v) == 1}

    # --- layout key of each access --------------------------------------------------------------------------------
    key_memo = {}

    def key_of(acc_id, depth=0):
        """(layout key, offset) of the object an access reads, or None."""
        if acc_id in key_memo:
            return key_memo[acc_id]
        key_memo[acc_id] = None
        acc = accs.get(acc_id)
        if acc is None or depth > 12 or acc['off'] is None:
            return None
        b = acc['base']
        add = b.get('add', 0)
        res = None
        if b['b'] == 'this':
            c = fn_class.get(acc['fn'])
            if c:
                res = (c, acc['off'] + add)
        elif b['b'] == 'acc':
            parent = key_of(b['id'], depth + 1)
            if parent is not None and accs[b['id']]['ptr'] or (parent is not None and accs[b['id']]['size'] == 4):
                res = (f'{parent[0]}@{parent[1]:#x}', acc['off'] + add)
        elif b['b'] == 'param':
            c = param_class.get((acc['fn'], b['i']))
            if c:
                res = (c, acc['off'] + add)
        elif b['b'] == 'global':
            res = (b['sym'] if b.get('helper') in ('gptr', 'ptr_of', 'deref') else f"&{b['sym']}", acc['off'] + add)
        key_memo[acc_id] = res
        return res

    layouts = collections.defaultdict(lambda: collections.defaultdict(lambda: {
        'types': collections.Counter(), 'sizes': collections.Counter(), 'ptr': 0, 'n': 0, 'w': 0, 'fns': set(),
        'files': set()}))
    unattributed = collections.Counter()
    total = 0
    for acc in accs.values():
        total += 1
        k = key_of(acc['id'])
        if k is None:
            b = acc['base']['b']
            why = 'variable offset' if acc['off'] is None else (
                'this of unknown class' if b == 'this' else
                'via unknown object' if b == 'acc' else b)
            unattributed[why] += 1
            continue
        cls, off = k
        e = layouts[cls][off]
        e['types'][acc['canon']] += 1
        e['sizes'][acc['size']] += 1
        e['ptr'] += 1 if acc['ptr'] else 0
        e['n'] += 1
        e['w'] += 1 if acc['w'] else 0
        e['fns'].add(fns[acc['fn']]['qual'] if acc['fn'] in fns else acc['fn'])
        e['files'].add(acc['file'])

    # facade names for pointee keys
    pointee_names = collections.defaultdict(collections.Counter)
    for c in casts:
        b = c['base']
        if b['b'] == 'acc':
            k = key_of(b['id'])
            if k:
                pointee_names[f'{k[0]}@{k[1]:#x}'][c['as']] += 1
        elif b['b'] == 'this' and c['fn'] in fn_class:
            pointee_names[fn_class[c['fn']]]['is-a ' + c['as']] += 1

    # --- outputs --------------------------------------------------------------------------------------------------
    out = {}
    conflicts = []
    for cls in sorted(layouts):
        fields = layouts[cls]
        rows = []
        for off in sorted(fields):
            e = fields[off]
            rows.append({'off': off, 'n': e['n'], 'w': e['w'], 'ptr': e['ptr'], 'sizes': dict(e['sizes']),
                         'types': dict(e['types'].most_common()), 'fns': sorted(e['fns'])[:12],
                         'files': sorted(e['files'])})
        # overlaps: a field that starts inside another one's widest access
        for i, r in enumerate(rows):
            w = max(int(s) for s in r['sizes'])
            for r2 in rows[i + 1:]:
                if r2['off'] >= r['off'] + w:
                    break
                conflicts.append(f"{cls}: {r['off']:#x} ({'/'.join(r['types'])}, {w} bytes) overlaps "
                                 f"{r2['off']:#x} ({'/'.join(r2['types'])})")
            if len(r['sizes']) > 1:
                conflicts.append(f"{cls}: {r['off']:#x} read with widths {sorted(r['sizes'])}: {'/'.join(r['types'])}")
            if r['ptr'] and r['ptr'] != r['n']:
                conflicts.append(f"{cls}: {r['off']:#x} pointer in {r['ptr']} of {r['n']} accesses: {'/'.join(r['types'])}")
        out[cls] = {'fields': rows, 'names': dict(pointee_names.get(cls, {})),
                    'extent': max((r['off'] + max(int(s) for s in r['sizes']) for r in rows), default=0)}
    json.dump(out, open(os.path.join(a.o, 'layouts.json'), 'w'), indent=1)
    open(os.path.join(a.o, 'conflicts.txt'), 'w').write('\n'.join(conflicts) + '\n')

    # summary
    attributed = total - sum(unattributed.values())
    named = [c for c in out if '@' not in c and not c.startswith(('d::', '&d::'))]
    lines = ['# Struct recovery A1 summary', '',
             f'files: {len(tus)} ({sum(1 for t in tus if t["fatal"])} with fatal errors)',
             f'functions: {len(fns)}, with a class: {len(fn_class)} '
             f'({collections.Counter(fn_why.values()).most_common()})',
             f'raw accesses: {total}; attributed to a layout: {attributed} ({100.0 * attributed / max(total, 1):.1f} %)',
             'not attributed: ' + ', '.join(f'{k} {v}' for k, v in unattributed.most_common()),
             f'layout keys: {len(out)} (classes {len(named)}, pointees {sum(1 for c in out if "@" in c)}, '
             f'globals {sum(1 for c in out if c.startswith(("d::", "&d::")) and "@" not in c)})',
             f'fields: {sum(len(v["fields"]) for v in out.values())}; pointer fields: '
             f'{sum(1 for v in out.values() for r in v["fields"] if r["ptr"])}; conflicts: {len(conflicts)}', '',
             '## Largest classes', '', '| class | fields | extent | accesses | pointer fields | names |', '|---|--:|--:|--:|--:|---|']
    for cls in sorted(named, key=lambda c: -sum(r['n'] for r in out[c]['fields']))[:60]:
        v = out[cls]
        lines.append(f"| {cls} | {len(v['fields'])} | {v['extent']:#x} | {sum(r['n'] for r in v['fields'])} | "
                     f"{sum(1 for r in v['fields'] if r['ptr'])} | {', '.join(list(v['names'])[:3])} |")
    open(os.path.join(a.o, 'summary.md'), 'w').write('\n'.join(lines) + '\n')
    print('\n'.join(lines[:9]))


if __name__ == '__main__':
    main()
