#!/usr/bin/env python3
"""Checks layout specs (re/layout/spec/*.json, docs/X64.md) against the binary's field accesses.

Spec file: {"group": "...", "classes": [ClassSpec...], "functions": {"0xaddr": "Class" | null}, "notes": "..."}
ClassSpec:
  {"class": "DynArray", "size": "0x29" | null, "base": "Name" | null, "vtable": "0x9d...." | null,
   "fields": [{"off": "0x1d", "name": "body_buf", "type": "u8*" | "ptr:Class" | "i32" | "char[13]" | ..., "size": 4,
               "count": 1}],
   "members": [{"off": "0x39", "name": "items", "class": "DynArray"}],
   "evidence": "...", "confidence": "high" | "medium" | "low", "notes": "..."}
A class may be restated by a later file (round 2, re/layout/spec/r2_*.json) with "override": true: the restatement
replaces the earlier spec entirely (at most one override per class per round; later rounds win).
`functions` reassigns functions to classes (default: the label in db.json; null = not a member function of a game
class, e.g. a free helper or a CRT routine).

Checks per function of each class (own accesses through `this`): every byte it reads or writes lies in a field of
the class, its base chain or an embedded member (recursively); an access must not straddle two fields; fields must
not overlap; the size (if given) covers the fields. Accesses through a pointer field `ptr:X` are checked against X.

    re/layout/check_spec.py [spec files...]       (default: all of re/layout/spec/*.json)
    re/layout/check_spec.py --class Name          report one class only
"""
import argparse
import collections
import glob
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DB = json.load(open(os.path.join(HERE, 'out', 'db.json')))
F = DB['functions']


def num(x):
    if x is None:
        return None
    return int(x, 16) if isinstance(x, str) else int(x)


def load_specs(paths):
    classes, owner, fn_map, overridden = {}, {}, {}, {}
    problems = []
    for p in paths:
        try:
            s = json.load(open(p))
        except Exception as e:
            problems.append(f'{p}: not valid JSON: {e}')
            continue
        for c in s.get('classes', []):
            n = c['class']
            if n in classes:
                rnd = os.path.basename(p).split('_')[0]  # r2, r3, ...: one override per class per round
                if c.get('override') and (n, rnd) not in overridden:
                    overridden[(n, rnd)] = os.path.basename(p)
                elif c.get('override'):
                    problems.append(f'{p}: class {n} overridden again in round {rnd} (first by {overridden[(n, rnd)]})')
                    continue
                else:
                    problems.append(f'{p}: class {n} also defined in {owner[n]}')
            classes[n] = c
            owner[n] = os.path.basename(p)
        for a, c in s.get('functions', {}).items():
            fn_map[f'{num(a):#x}'] = c
    return classes, owner, fn_map, problems


class Layout:
    def __init__(self, classes):
        self.classes = classes
        self.memo = {}

    def bytes_of(self, name, depth=0):
        """offset -> (field description, field start, field size) for every byte of class `name` incl. bases / members."""
        if name in self.memo:
            return self.memo[name]
        self.memo[name] = {}
        c = self.classes.get(name)
        out = {}
        if c is None or depth > 20:
            return out
        if c.get('base'):
            out.update(self.bytes_of(c['base'], depth + 1))
        for m in c.get('members', []):
            o = num(m['off'])
            sub = self.bytes_of(m['class'], depth + 1)
            msz = (max(sub) + 1) if sub else 0
            sc = self.classes.get(m['class'], {})
            if sc.get('size'):
                msz = max(msz, num(sc['size']))
            for k in range(int(m.get('count', 1) or 1)):
                for b, (d, st, sz) in sub.items():
                    out[o + k * msz + b] = (f"{m['name']}[{k}].{d}", o + k * msz + st, sz)
        for f in c.get('fields', []):
            o, sz = num(f['off']), int(f.get('size', 0)) * int(f.get('count', 1) or 1)
            for b in range(o, o + max(sz, 1)):
                out[b] = (f['name'], o, sz)
        self.memo[name] = out
        return out

    def field_at(self, name, off):
        """The pointer field declared at `off` of class `name` (searching bases and members): its type or None."""
        c = self.classes.get(name)
        if c is None:
            return None
        for f in c.get('fields', []):
            if num(f['off']) == off:
                return f['type']
        for m in c.get('members', []):
            mo = num(m['off'])
            t = self.field_at(m['class'], off - mo) if off >= mo else None
            if t:
                return t
        return self.field_at(c['base'], off) if c.get('base') else None


def check(classes, fn_map, only=None):
    L = Layout(classes)
    members = collections.defaultdict(list)
    for a, e in F.items():
        c = fn_map.get(a, e['class']) if a in fn_map else e['class']
        if c:
            members[c].append(a)
    report = collections.defaultdict(list)
    stats = collections.Counter()
    for name, c in classes.items():
        if only and name != only:
            continue
        fields = sorted(c.get('fields', []), key=lambda f: num(f['off']))
        for f1, f2 in zip(fields, fields[1:]):
            if num(f1['off']) + int(f1.get('size', 0)) * int(f1.get('count', 1) or 1) > num(f2['off']):
                report[name].append(f"fields {f1['name']}@{f1['off']} and {f2['name']}@{f2['off']} overlap")
        bmap = L.bytes_of(name)
        size = num(c.get('size'))
        if size is not None and bmap and max(bmap) >= size:
            report[name].append(f'size {size:#x} but bytes up to {max(bmap):#x} are declared')
        for a in members.get(name, []):
            for path, off, width, kind, n in F[a]['asm']:
                if width == 0:
                    continue
                target, toff = name, off
                if path:
                    # follow pointer fields: path[0] is a pointer field of this class, ...
                    t, ok = name, True
                    for p in path:
                        ft = L.field_at(t, p)
                        if not ft or not ft.startswith('ptr:') or ft[4:] in ('void', 'fn', ''):
                            ok = False
                            break
                        t = ft[4:]
                    if not ok or t not in classes:
                        stats['unchecked pointee'] += 1
                        continue
                    target, toff = t, off
                    bm = L.bytes_of(target)
                else:
                    bm = bmap
                stats['checked'] += 1
                if toff < 0 and toff >= -8:
                    # new[] count cookie before an object, or a 1-based array walk (base[i-1]): not a field
                    stats['negative'] += 1
                    continue
                if '[]' in kind:
                    if toff not in bm:
                        report[name].append(f'{a} {F[a]["name"]}: indexed {width}B access at '
                                            f'{"/".join(hex(x) for x in path) or "this"}+{toff:#x} ({target}) not declared')
                        stats['missing'] += 1
                    continue
                got = [bm.get(b) for b in range(toff, toff + width)]
                if any(g is None for g in got):
                    report[name].append(f'{a} {F[a]["name"]}: {width}B {kind} at '
                                        f'{"/".join(hex(x) for x in path) or "this"}+{toff:#x} ({target}) not covered')
                    stats['missing'] += 1
                elif len({g[1] for g in got}) > 1:
                    report[name].append(f'{a} {F[a]["name"]}: {width}B {kind} at +{toff:#x} straddles '
                                        f'{sorted({g[0] for g in got})}')
                    stats['straddle'] += 1
    return report, stats, members


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('specs', nargs='*')
    ap.add_argument('--class', dest='cls')
    a = ap.parse_args()
    paths = a.specs or sorted(glob.glob(os.path.join(HERE, 'spec', '*.json')))
    # bases / members from other groups resolve through every spec file
    paths = [os.path.realpath(p) for p in paths]
    all_paths = sorted(set(paths) | {os.path.realpath(p) for p in glob.glob(os.path.join(HERE, 'spec', '*.json'))})
    classes, owner, fn_map, problems = load_specs(all_paths)
    mine = set()
    for p in paths:
        try:
            mine |= {c['class'] for c in json.load(open(p)).get('classes', [])}
        except Exception:
            pass
    report, stats, members = check(classes, fn_map, a.cls)
    for p in problems:
        print('SPEC:', p)
    for name in sorted(report):
        if name not in mine:
            continue
        print(f'## {name} ({owner.get(name)})')
        for line in report[name][:60]:
            print('  ' + line)
        if len(report[name]) > 60:
            print(f'  ... {len(report[name]) - 60} more')
    print(f'\nclasses checked: {len(mine)}; accesses checked {stats["checked"]}, not covered {stats["missing"]}, '
          f'straddling {stats["straddle"]}, through unknown pointees {stats["unchecked pointee"]}, '
          f'negative offsets (new[] cookies / 1-based walks) {stats["negative"]}; '
          f'classes with problems {sum(1 for n in report if n in mine)}')
    sys.exit(1 if any(report[n] for n in report if n in mine) or problems else 0)


if __name__ == '__main__':
    main()
