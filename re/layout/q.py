#!/usr/bin/env python3
"""Query tool for the layout review (docs/X64.md). Reads re/layout/out/db.json (re/layout/db.py).

  q.py class <Label>          field table of a class label: per offset the widths (binary), the C++ types, readers
                              / writers; sizes from new+ctor; vtables; callers that pass this / a field / a sub-object
                              (base / member / pointee hints); facade casts
  q.py fn <0xaddr>            one function: name, label, files, binary + C++ views, calls, callers
  q.py asm <0xaddr>           the original disassembly of a function (joan.asm)
  q.py src <0xaddr>           the C++ replacement's source (file and line from the db)
  q.py labels [regex]         class labels (functions, sizes)
  q.py who <Label> <0xoff>    every function (any label) that touches offset off of an object of this label
"""
import bisect
import collections
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
RE = os.environ.get('JOAN_RE_DIR', os.path.join(os.path.dirname(ROOT), 're'))
DB = json.load(open(os.path.join(HERE, 'out', 'db.json')))
F = DB['functions']
C = DB['classes']


def label(addr):
    e = F.get(addr)
    return (e or {}).get('class') or '?'


def name(addr):
    e = F.get(addr)
    return (e or {}).get('name') or addr


def ptxt(p):
    return '/'.join(f'{x:#x}' for x in p) or 'this'


def cmd_class(lab):
    if lab not in C:
        sys.exit(f'no label {lab}; try: q.py labels {lab[:4]}')
    c = C[lab]
    print(f'# {lab}: {len(c["functions"])} functions; new+ctor sizes {[hex(s) for s in c["sizes"]]}; '
          f'vtables {c["vtables"]}')
    rows = collections.defaultdict(lambda: {'w': collections.Counter(), 'rw': collections.Counter(), 'cpp': collections.Counter(),
                                            'fns': set()})
    calls_out = collections.Counter()
    for a in c['functions']:
        e = F[a]
        for path, off, width, kind, n in e['asm']:
            key = (tuple(path), off)
            r = rows[key]
            r['w'][width] += n
            r['rw'][kind] += n
            r['fns'].add(a)
        for k, types in e['cpp'].items():
            p, off = k.split('+')
            path = tuple(int(x, 16) for x in p.split('/')) if p != 'this' else ()
            r = rows[(path, int(off, 16))]
            for t, n in types.items():
                r['cpp'][t] += n
        for path, add, t, n in e['calls']:
            calls_out[(tuple(path), add, label(f'{t:#x}'))] += n
    print('\n## fields (path+offset: binary widths, r/w, C++ types, functions)')
    for (path, off) in sorted(rows, key=lambda k: (len(k[0]), k[0], k[1])):
        r = rows[(path, off)]
        fl = sorted(r['fns'])
        print(f'{ptxt(path)}+{off:#x}: w{dict(r["w"])} {dict(r["rw"])} cpp{dict(r["cpp"])} '
              f'by {len(fl)}: {", ".join(name(x) for x in fl[:4])}{" ..." if len(fl) > 4 else ""}')
    print('\n## calls made by its functions with ecx = this / a field / a sub-object  (path, +add -> callee label)')
    for (path, add, lab2), n in sorted(calls_out.items(), key=lambda kv: (len(kv[0][0]), kv[0][0], kv[0][1]))[:120]:
        print(f'{ptxt(path)}+{add:#x} -> {lab2} x{n}')
    print('\n## callers passing an object as this (caller label, path, +add)')
    seen = collections.Counter()
    for a in c['functions']:
        for ca, path, add in F[a]['callers']:
            seen[(label(ca), ptxt(path), add)] += 1
    for (l2, p, add), n in seen.most_common(60):
        print(f'{l2} {p}+{add:#x} x{n}')
    casts = collections.Counter()
    for a in c['functions']:
        for where, as_ in F[a]['casts']:
            casts[(where, as_)] += 1
    if casts:
        print('\n## facade casts in its functions (where -> as)')
        for (w, as_), n in casts.most_common(40):
            print(f'{w} -> {as_} x{n}')
    print('\n## functions')
    for a in c['functions']:
        e = F[a]
        print(f'{a} {e["name"]} [{e["label_evidence"]}] {e["file"]}:{e["line"]} sizes={e["sizes"]} '
              f'{"" if e["thiscall"] else "(not thiscall)"}')


def cmd_fn(addr):
    addr = f'{int(addr, 16):#x}'
    e = F.get(addr)
    if not e:
        sys.exit('unknown function')
    print(json.dumps({k: v for k, v in e.items() if k not in ('asm', 'calls')}, indent=1))
    print('asm view:')
    for path, off, width, kind, n in e['asm']:
        print(f'  {ptxt(path)}+{off:#x} {width}B {kind} x{n}')
    print('calls with tracked ecx:')
    for path, add, t, n in e['calls']:
        print(f'  {ptxt(path)}+{add:#x} -> {t:#x} {name(f"{t:#x}")} [{label(f"{t:#x}")}] x{n}')


def cmd_asm(addr):
    a = int(addr, 16)
    fns = json.load(open(os.path.join(RE, 'functions.json')))
    end = next((int(f['end'], 16) for f in fns if int(f['addr'], 16) == a), a + 0x200)
    on = False
    for line in open(os.path.join(RE, 'joan.asm')):
        m = re.match(r'^\s+([0-9a-f]+):', line)
        if not m:
            continue
        x = int(m.group(1), 16)
        if x == a:
            on = True
        if on and x >= end:
            break
        if on:
            print(line.rstrip())


def cmd_src(addr):
    e = F.get(f'{int(addr, 16):#x}')
    if not e or not e['line']:
        sys.exit('no C++ source recorded')
    path = os.path.join(ROOT, 'src', e['file'])
    lines = open(path, encoding='utf-8', errors='replace').read().split('\n')
    i = e['line'] - 1
    depth, started = 0, False
    for j in range(i, min(len(lines), i + 400)):
        print(f'{j + 1}: {lines[j]}')
        depth += lines[j].count('{') - lines[j].count('}')
        started = started or '{' in lines[j]
        if started and depth <= 0:
            break


def cmd_labels(rx=''):
    for k, v in C.items():
        if re.search(rx, k):
            print(f'{k}: {len(v["functions"])} functions, sizes {[hex(s) for s in v["sizes"]]}, vtables {v["vtables"]}')


def cmd_who(lab, off):
    off = int(off, 16)
    for a in C.get(lab, {}).get('functions', []):
        for path, o, w, k, n in F[a]['asm']:
            if not path and o == off:
                print(f'{a} {name(a)} {w}B {k} x{n}')


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd, args = sys.argv[1], sys.argv[2:]
    {'class': cmd_class, 'fn': cmd_fn, 'asm': cmd_asm, 'src': cmd_src, 'labels': cmd_labels, 'who': cmd_who}[cmd](*args)


if __name__ == '__main__':
    main()
