#!/usr/bin/env python3
"""Class labels for joan.exe functions (re/classes.csv): who `this` is.

Evidence (joan.exe is a /Od build, so the patterns are regular):
  - vtables: runs of code addresses in .rdata whose start is stored into [reg] by a function (`mov dword [reg], vtable`
    = the constructor / destructor); the constructor, destructor and the entries owned by one vtable get its label;
  - same-`this` calls: `mov ecx, [ebp-4]` (the saved this) right before `call f` in a thiscall function: f is a method of
    the caller's class (or of a base). A callee is labelled only when all its labelled callers agree;
  - seeds: the registered / fn_names.hpp names `Class::method` whose class is not a placeholder (Obj, X, Obj6b, ...).
Usage (repository root, needs ../re/joan.asm, functions.json, thunks.json, joan.exe):
  re/classes.py [--re ../re] [--out re/classes.csv]
"""
import argparse, bisect, collections, csv, json, os, re, struct

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PH = re.compile(r'^(Obj|X|Y|H|Rec|Record|Item|Entry|Node|Thing|Holder|Self|Base|Pair|Ptr|Ref|Cur|Iter|Unknown|Global|'
                r'Static\w*)$|^(Obj|X)[0-9A-Za-z_]+$')
INS = re.compile(r'^\s+([0-9a-f]+):\t(.*)$')
RDATA = (0x9cf000, 0x9f3c86, 0x5cf000)  # va begin, va end, file offset


def placeholder(c):
    if PH.search(c):
        return True
    m = re.search(r'[0-9a-f]{3,}$', c)
    return bool(m and re.search(r'[0-9]', m.group(0)) and not re.search(r'[23][dD]$', c))


def class_of(name):
    if '::' not in name:
        return None
    c = name.split('::')[0].split('<')[0]
    return None if placeholder(c) else c


def load_names():
    names = {}
    for line in open(os.path.join(ROOT, 'src', 'fn_names.hpp')):
        m = re.search(r'// 0x[0-9a-f]+ -> 0x([0-9a-f]+)  (\S+)', line) or re.search(r'// 0x([0-9a-f]+)  (\S+)', line)
        if m:
            names[int(m.group(1), 16)] = m.group(2)
    att = os.path.join(ROOT, 're', 'attribution.csv')
    if os.path.exists(att):
        for r in csv.DictReader(open(att)):
            names.setdefault(int(r['address'], 16), r['registered_name'])
    return names


def scan(redir):
    fns = json.load(open(os.path.join(redir, 'functions.json')))
    thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(os.path.join(redir, 'thunks.json'))).items()}
    starts = sorted(int(f['addr'], 16) for f in fns)
    ends = {int(f['addr'], 16): int(f['end'], 16) for f in fns}

    def fn_of(a):
        i = bisect.bisect_right(starts, a) - 1
        return starts[i] if i >= 0 and a < ends[starts[i]] else None
    same, vstore, thisin = collections.defaultdict(set), collections.defaultdict(set), set()
    cur, prev = None, []
    for line in open(os.path.join(redir, 'joan.asm')):
        m = INS.match(line)
        if not m:
            continue
        a, ins = int(m.group(1), 16), m.group(2)
        f = fn_of(a)
        if f != cur:
            cur, prev = f, []
        if f is None:
            continue
        if ins.startswith('mov    DWORD PTR [ebp-0x4],ecx') and len(prev) < 12:
            thisin.add(f)
        mm = re.match(r'call\s+0x([0-9a-f]+)$', ins)
        if mm and prev and prev[-1] == 'mov    ecx,DWORD PTR [ebp-0x4]' and f in thisin:
            t = int(mm.group(1), 16)
            same[f].add(thunks.get(t, t))
        mv = re.match(r'mov\s+DWORD PTR \[(e[a-z]x|esi|edi)\],0x([0-9a-f]+)$', ins)
        if mv and RDATA[0] <= int(mv.group(2), 16) < RDATA[1]:
            vstore[f].add(int(mv.group(2), 16))
        prev.append(ins)
    exe = open(os.path.join(redir, 'joan.exe'), 'rb').read()
    fset = set(starts)

    def rd(va):
        return struct.unpack_from('<I', exe, va - RDATA[0] + RDATA[2])[0]

    def code(v):
        return v in thunks or v in fset
    vstarts = sorted({v for vs in vstore.values() for v in vs if code(rd(v))})
    vt = {}
    for i, v in enumerate(vstarts):
        lim = vstarts[i + 1] if i + 1 < len(vstarts) else RDATA[1]
        ents, a = [], v
        while a < lim and code(rd(a)):
            ents.append(thunks.get(rd(a), rd(a)))
            a += 4
        vt[v] = ents
    ctors = collections.defaultdict(set)
    for f, vs in vstore.items():
        for v in vs:
            if v in vt:
                ctors[v].add(f)
    return same, vt, ctors


def label(names, same, vt, ctors):
    seed = {a: class_of(n) for a, n in names.items() if class_of(n)}
    lab, how = dict(seed), {a: 'name' for a in seed}
    callers = collections.defaultdict(set)
    for f, cs in same.items():
        for c in cs:
            if c != f:
                callers[c].add(f)
    vtg = collections.defaultdict(set)
    for v, ents in vt.items():
        for x in set(ents) | ctors.get(v, set()):
            vtg[x].add(v)
    for _ in range(5):
        vlab = {}
        for v, ents in vt.items():
            mem = [x for x in set(ents) | ctors.get(v, set()) if len(vtg[x]) == 1]
            c = collections.Counter(lab[x] for x in mem if x in lab)
            if c:
                vlab[v] = c.most_common(1)[0][0]
        for x, vs in vtg.items():
            if x in seed or len(vs) != 1:
                continue
            v = next(iter(vs))
            if v in vlab and lab.get(x) != vlab[v]:
                lab[x], how[x] = vlab[v], 'vtable 0x%x' % v
        for _ in range(10):
            ch = 0
            for c, fs in callers.items():
                if c in seed or how.get(c, '').startswith('vtable'):
                    continue
                ls = {lab[f] for f in fs if f in lab}
                if len(ls) == 1:
                    lv = ls.pop()
                    if lab.get(c) != lv:
                        lab[c], how[c] = lv, 'this-call from %d caller(s)' % len(fs)
                        ch += 1
                elif len(ls) > 1 and c in lab:
                    del lab[c]
                    how.pop(c, None)
                    ch += 1
            if not ch:
                break
    return lab, how


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--re', default=os.path.join(os.path.dirname(ROOT), 're'))
    ap.add_argument('--out', default=os.path.join(ROOT, 're', 'classes.csv'))
    a = ap.parse_args()
    names = load_names()
    same, vt, ctors = scan(a.re)
    lab, how = label(names, same, vt, ctors)
    with open(a.out, 'w', newline='') as f:
        w = csv.writer(f)
        w.writerow(['address', 'name', 'class', 'evidence'])
        for x in sorted(set(lab) | set(names)):
            w.writerow(['0x%08x' % x, names.get(x, ''), lab.get(x, ''), how.get(x, '')])
    print('%d vtables, %d labelled functions -> %s' % (len(vt), len(lab), a.out))


if __name__ == '__main__':
    main()
