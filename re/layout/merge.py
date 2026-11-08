#!/usr/bin/env python3
"""Consolidates the layout specs (docs/X64.md) into one file for the header generator (A2):
re/layout/layouts.json = {"classes": {Name: spec (overrides applied)}, "functions": {"0xaddr": Class | null},
"stats": {...}}. Function classes: db.json labels, then every spec file's `functions` in load order (g*, r2_*, r3_*,
zz_reconcile last).

    re/layout/merge.py [-o re/layout/layouts.json]
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
    ap.add_argument('-o', default=os.path.join(HERE, 'layouts.json'))
    a = ap.parse_args()
    paths = sorted(glob.glob(os.path.join(HERE, 'spec', '*.json')))
    classes, owner, fn_map, problems = K.load_specs(paths)
    if problems:
        sys.exit('\n'.join(problems))
    functions = {}
    for addr, e in K.F.items():
        c = fn_map[addr] if addr in fn_map else e['class']
        functions[addr] = c if c in classes else (None if c is None else c)
    report, stats, members = K.check(classes, fn_map)
    # renames (re/layout/renames.json): spec names that clash with names the sources already use in joan::orig
    ren = json.load(open(os.path.join(HERE, 'renames.json'))) if os.path.exists(os.path.join(HERE, 'renames.json')) else {}

    def rn(c):
        return ren.get(c, c) if isinstance(c, str) else c
    for n in list(classes):
        c = dict(classes[n])
        c['class'] = rn(c['class'])
        c['base'] = rn(c.get('base'))
        c['members'] = [dict(m, **{'class': rn(m['class'])}) for m in c.get('members', [])]
        c['fields'] = [dict(f, type=('ptr:' + rn(f['type'][4:])) if f['type'].startswith('ptr:') else rn(f['type']))
                       for f in c.get('fields', [])]
        del classes[n]
        classes[rn(n)] = c
        owner[rn(n)] = owner.get(n)
    functions = {a: rn(c) for a, c in functions.items()}
    unknown = sorted({c for c in functions.values() if c and c not in classes})
    conf = collections.Counter(c.get('confidence', '?') for c in classes.values())
    aliases = sum(1 for c in classes.values() if c.get('base') and not c.get('fields') and not c.get('members'))
    out = {
        'classes': {n: dict(c, defined_in=owner[n]) for n, c in sorted(classes.items())},
        'functions': dict(sorted(functions.items())),
        'stats': {'classes': len(classes), 'aliases': aliases, 'confidence': dict(conf),
                  'fields': sum(len(c.get('fields', [])) for c in classes.values()),
                  'pointer_fields': sum(1 for c in classes.values() for f in c.get('fields', [])
                                        if f['type'].startswith('ptr:') or f['type'].endswith('*')),
                  'functions_with_class': sum(1 for c in functions.values() if c in classes),
                  'functions_class_without_spec': unknown[:50], 'check': dict(stats),
                  'classes_with_problems': sorted(n for n in report if report[n])},
    }
    json.dump(out, open(a.o, 'w'), indent=1)
    print(json.dumps(out['stats'], indent=1))


if __name__ == '__main__':
    main()
