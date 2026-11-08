#!/usr/bin/env python3
"""Address-ordered listing of the functions of one src/ file (an _unplaced stretch), for segmenting it into the
original objects: size, registered name, class label (re/classes.csv), strings, callees and callers by name.
Usage: re/stretch_view.py src/game/_unplaced/after_camres.cpp [--re ../re]"""
import argparse, collections, csv, json, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('file')
    ap.add_argument('--re', default=os.path.join(os.path.dirname(ROOT), 're'))
    a = ap.parse_args()
    fns = {int(f['addr'], 16): f for f in json.load(open(os.path.join(a.re, 'functions.json')))}
    thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(os.path.join(a.re, 'thunks.json'))).items()}
    cls = {int(r['address'], 16): r for r in csv.DictReader(open(os.path.join(ROOT, 're', 'classes.csv')))}
    att = {int(r['address'], 16): r for r in csv.DictReader(open(os.path.join(ROOT, 're', 'attribution.csv')))}
    callers = collections.defaultdict(set)
    for x, f in fns.items():
        for c in f['calls']:
            t = int(c, 16)
            callers[thunks.get(t, t)].add(x)
    want = os.path.relpath(os.path.abspath(a.file), ROOT)
    mine = sorted(x for x, r in att.items() if r['current_file'] == want)
    if not mine:
        raise SystemExit('no registrations of %s in re/attribution.csv' % want)
    lo, hi = mine[0], mine[-1]
    name = lambda x: (cls.get(x) or {}).get('name') or 'sub_%x' % x
    print('# %s: %d registered functions, 0x%x..0x%x (unregistered functions in the range marked with -)' %
          (want, len(mine), lo, hi))
    for x in sorted(f for f in fns if lo <= f <= hi):
        f = fns[x]
        reg = '*' if x in mine else ('=' if x in att else '-')
        c = (cls.get(x) or {}).get('class', '')
        s = ' '.join('"%s"' % t[:40].replace('\n', '\\n') for t in f['strings'][:3])
        ce = ', '.join(name(thunks.get(int(t, 16), int(t, 16))) for t in f['calls'][:5])
        cr = ', '.join(name(y) for y in sorted(callers[x])[:4])
        other = '' if reg != '=' else ' [in %s]' % att[x]['current_file']
        print('%s 0x%08x %5d %-40s {%s}%s %s | -> %s | <- %s' % (reg, x, f['size'], name(x)[:40], c, other, s, ce, cr))


if __name__ == '__main__':
    main()
