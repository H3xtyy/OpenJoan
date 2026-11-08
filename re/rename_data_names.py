#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Rename d:: data names (src/data_names.hpp) by address from a committed table.

Usage (from the repository root):
  rename_data_names.py <table.csv> [--dry-run]

A rename table is CSV: address, old_name, new_name, evidence ('#' comment lines allowed). For every row the
src/data_names.hpp entry `inline constexpr img::Sym<T> old{0x00XXXXXXu};` with that address and name is renamed (the
value, the type and the trailing comment stay), and every qualified use `d::old` / `joan::d::old` in src/** (not the
generated src/data/) follows. A new name that already exists, or is not a C++ identifier, stops the tool.

re/gen_data_names.py keeps an existing name per address on regeneration, so the renamed name survives it.
"""
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
LINE = re.compile(r'^(\s*inline constexpr img::Sym<.*?> )(\w+)(\{0x([0-9a-f]{8})u\};.*)$')


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    dry = '--dry-run' in sys.argv
    table = list(csv.DictReader(l for l in open(args[0]) if not l.startswith('#')))
    hpp = os.path.join(ROOT, 'src', 'data_names.hpp')
    lines = open(hpp).read().split('\n')
    names = {}
    for ln in lines:
        m = LINE.match(ln)
        if m:
            names[m.group(2)] = '0x' + m.group(4)
    ren = {}
    for t in table:
        old, new, addr = t['old_name'], t['new_name'], '0x%08x' % int(t['address'], 16)
        if names.get(old) != addr:
            sys.exit('%s is not the name of %s in src/data_names.hpp' % (old, addr))
        if not re.fullmatch(r'[A-Za-z_]\w*', new):
            sys.exit('not an identifier: %s' % new)
        if new in names or new in ren.values():
            sys.exit('name already taken: %s (%s)' % (new, addr))
        ren[old] = new
    for i, ln in enumerate(lines):
        m = LINE.match(ln)
        if m and m.group(2) in ren:
            lines[i] = m.group(1) + ren[m.group(2)] + m.group(3)
    print('%d d:: names to rename' % len(ren))
    if dry or not ren:
        return
    open(hpp, 'w', newline='\n').write('\n'.join(lines))
    pat = re.compile(r'\b(d::)(' + '|'.join(sorted(map(re.escape, ren), key=len, reverse=True)) + r')\b')
    n = 0
    for dirpath, _, files in os.walk(os.path.join(ROOT, 'src')):
        if os.path.relpath(dirpath, ROOT).startswith('src/data'):
            continue
        for f in files:
            if not f.endswith(('.cpp', '.hpp', '.inc', '.h')):
                continue
            p = os.path.join(dirpath, f)
            if p == hpp:
                continue
            s = open(p, encoding='utf-8', errors='surrogateescape').read()
            t = pat.sub(lambda m: m.group(1) + ren[m.group(2)], s)
            if t != s:
                open(p, 'w', encoding='utf-8', errors='surrogateescape', newline='').write(t)
                n += 1
    print('uses rewritten in %d files' % n)


if __name__ == '__main__':
    main()
