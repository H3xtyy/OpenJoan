#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Rename registrations (the name argument of JOAN_REPLACE* and module-local registration macros) by address.

Usage (from the repository root):
  rename_registrations.py --from-re2 [--confidence certain] --out re/names/<table>.csv   write a rename table
  rename_registrations.py --apply re/names/<table>.csv [--dry-run]                       apply one to src/**

A rename table is CSV: address, old_name, new_name, evidence (one row per registration to rename; '#' comments).
--from-re2 proposes one from re/re2_engine_matches.csv: the RE2 engine name for every registration of a matched
address whose name differs. Names stay unique: a new name that another registration already has (or that several
addresses would get) takes the original address as suffix (`CpuUsage::dump_45d330`), as the existing names do.
--apply edits exactly the registration line re/check_registrations.py reports for each (address, old name) and
fails when the old name is not found there. The registration's module and address never change; fn:: names and
counters are separate (re/gen_fn_names.py keeps an fn:: name per address).

Registration names are the IDs of the logs and of [Hooks] Disable= (the owner does not use the latter, 2026-11-07),
so no alias table is kept; the committed rename tables are the record.
"""
import argparse
import collections
import csv
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)


def registrations():
    """[(address, name, module, kind, extra, file, line, fn)] of src/** (re/check_registrations.py --dump)"""
    with tempfile.NamedTemporaryFile('r', suffix='.tsv') as t:
        subprocess.run([sys.executable, os.path.join(HERE, 'check_registrations.py'), '--dump', t.name], check=True,
                       cwd=ROOT, stdout=subprocess.DEVNULL)
        return [l.rstrip('\n').split('\t') for l in open(t.name) if l.strip()]


def read_table(path):
    return list(csv.DictReader(l for l in open(path) if not l.startswith('#')))


def from_re2(conf):
    regs = registrations()
    by_addr = collections.defaultdict(list)
    for r in regs:
        by_addr[r[0]].append(r)
    taken = collections.Counter(r[1] for r in regs)
    rows = []
    for m in read_table(os.path.join(HERE, 're2_engine_matches.csv')):
        if m['confidence'] not in conf:
            continue
        for r in by_addr.get(m['address'], []):
            if r[1] != m['re2_name']:
                rows.append([m['address'], r[1], m['re2_name'], 'RE2 engine symbols (%s, %s): %s' %
                             (m['confidence'], m['re2_file'], m['evidence'])])
    # uniqueness: names wanted by several addresses, or already held by a registration that keeps its name
    renamed = {(r[0], r[1]) for r in rows}
    kept = collections.Counter(r[1] for r in regs if (r[0], r[1]) not in renamed)
    want = collections.defaultdict(set)
    for r in rows:
        want[r[2]].add(r[0])
    for r in rows:
        if len(want[r[2]]) > 1 or kept[r[2]]:
            r[2] = '%s_%s' % (r[2], r[0][4:] if r[0].startswith('0x00') else r[0][2:])
    return rows


def apply(path, dry):
    rows = read_table(path)
    regs = registrations()
    where = collections.defaultdict(list)
    for r in regs:
        where[(r[0], r[1])].append((r[5], int(r[6])))
    edits = collections.defaultdict(list)  # file -> [(line, old, new)]
    for t in rows:
        locs = where.get((t['address'], t['old_name']))
        if not locs:
            sys.exit('not found: %s %s' % (t['address'], t['old_name']))
        for f, ln in locs:
            edits[f].append((ln, t['old_name'], t['new_name']))
    n = 0
    skipped = []
    for f, es in edits.items():
        p = os.path.join(ROOT, 'src', f)
        lines = open(p, encoding='utf-8', errors='surrogateescape').read().split('\n')
        for ln, old, new in es:
            i = ln - 1
            done = False
            # the name literal may sit on a continuation line of a multi-line macro call; module macros that prepend
            # the class (`#define MEM(name, ...) JOAN_REPLACE(..., "FileMem::" name, ...)`) hold only the member part
            cands = [('"%s"' % old, '"%s"' % new)]
            if '::' in old and '::' in new and old.rsplit('::', 1)[0] == new.rsplit('::', 1)[0]:
                cands.append(('"%s"' % old.rsplit('::', 1)[1], '"%s"' % new.rsplit('::', 1)[1]))
            for a, b in cands:
                for j in range(i, min(i + 8, len(lines))):  # (the dump's line can be a comment above the call)
                    if lines[j].count(a) == 1:
                        lines[j] = lines[j].replace(a, b)
                        n += 1
                        done = True
                        break
                if done:
                    break
            if not done:
                skipped.append('%s:%d %s -> %s' % (f, ln, old, new))
        if not dry:
            open(p, 'w', encoding='utf-8', errors='surrogateescape', newline='\n').write('\n'.join(lines))
    for x in skipped:
        print('skipped (the macro prepends another class):', x)
    print('%d registration names %s in %d files' % (n, 'to rename' if dry else 'renamed', len(edits)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--from-re2', action='store_true')
    ap.add_argument('--confidence', default='certain')
    ap.add_argument('--out')
    ap.add_argument('--apply')
    ap.add_argument('--dry-run', action='store_true')
    a = ap.parse_args()
    if a.from_re2:
        rows = from_re2(set(a.confidence.split(',')))
        with open(a.out, 'w', newline='') as o:
            o.write('# Registration renames from the Restaurant Empire 2 engine symbols (re/re2_engine_matches.csv, %s rows);\n'
                    '# applied by re/rename_registrations.py --apply.\n' % a.confidence)
            w = csv.writer(o, lineterminator='\n')
            w.writerow(['address', 'old_name', 'new_name', 'evidence'])
            w.writerows(rows)
        print('%d renames -> %s' % (len(rows), a.out))
    elif a.apply:
        apply(a.apply, a.dry_run)
    else:
        ap.error('--from-re2 or --apply')


if __name__ == '__main__':
    main()
