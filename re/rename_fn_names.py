#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Rename fn:: code-slot names after their registrations were renamed (re/rename_registrations.py).

Usage (from the repository root):
  rename_fn_names.py <rename table.csv> [--dry-run]

For every row of the table (address, old_name, new_name, ... : registration renames, e.g. re/names/re2_certain.csv)
each src/fn_names.hpp entry whose real address is that address and whose comment names the old registration is
renamed the way re/gen_fn_names.py would name it now: sanitise(new registration name) plus the entry's own suffix
(`_t<thunk>`, `_body`, `_<address>`). A name that would clash takes the real address as suffix. The slot index stays
(`inline constexpr Code Name{index}`), so every call compiles to the same code; the entry's comment, the slot's name
string in src/runtime/code_slots.inc and every qualified use (fn::Name) in src/** change with it. Entries whose name does not
derive from the old registration name (hand-chosen names) are left alone and listed.

Why not regenerate: re/gen_fn_names.py keeps an existing name per address, and a full regeneration on today's tree
also drops names that are still referenced (drift since joan-re); this tool renames only what was asked.
"""
import collections
import csv
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from gen_fn_names import sanitise  # noqa: E402

LINE = re.compile(r'^(\s*inline constexpr Code )(\w+)(\{\d+\};\s*//\s*0x[0-9a-f]{8}(?: -> (0x[0-9a-f]{8}))?)(\s+)(.*)$')


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    dry = '--dry-run' in sys.argv
    table = list(csv.DictReader(l for l in open(args[0]) if not l.startswith('#')))
    want = {(t['address'], t['old_name']): t['new_name'] for t in table}
    hpp = os.path.join(ROOT, 'src', 'fn_names.hpp')
    lines = open(hpp).read().split('\n')
    names = set()
    for ln in lines:
        m = LINE.match(ln)
        if m:
            names.add(m.group(2))
    ren = {}  # old identifier -> new identifier
    left = []
    for i, ln in enumerate(lines):
        m = LINE.match(ln)
        if not m:
            continue
        name, rest, real, ws, reg = m.group(2), m.group(3), m.group(4), m.group(5), m.group(6)
        if real is None:
            real = re.search(r'0x[0-9a-f]{8}', rest).group(0)
        real = '0x%08x' % int(real, 16)
        new_reg = want.get((real, reg))
        if new_reg is None:
            continue
        old_base = sanitise(reg)
        if not name.startswith(old_base):
            left.append('%s (%s): name not derived from %s' % (name, real, reg))
            continue
        suffix = name[len(old_base):]
        new = sanitise(new_reg) + suffix
        if new != name and (new in names or new in ren.values()):
            tail = '_%06x' % int(real, 16)
            if not new.endswith(tail):
                new = sanitise(new_reg) + tail + (suffix if suffix.startswith(('_t', '_body')) else '')
        if new in names and new != name or new in ren.values():
            left.append('%s (%s): %s already taken' % (name, real, new))
            continue
        ren[name] = new
        lines[i] = m.group(1) + new + rest + ws + new_reg
    print('%d fn:: names to rename, %d left as they are' % (len(ren), len(left)))
    for x in left:
        print('  left:', x)
    if dry or not ren:
        return
    open(hpp, 'w', newline='\n').write('\n'.join(lines))
    # only qualified uses (fn::Name, joan::fn::Name): an unqualified identifier of the same spelling is another symbol
    # (a free function of the same name, a layout field); the code-slot name strings of src/runtime/code_slots.inc by their quotes
    pat = re.compile(r'\b(fn::)(' + '|'.join(sorted(map(re.escape, ren), key=len, reverse=True)) + r')\b')
    spat = re.compile(r'"(' + '|'.join(sorted(map(re.escape, ren), key=len, reverse=True)) + r')"')
    changed = collections.Counter()
    for dirpath, _, files in os.walk(os.path.join(ROOT, 'src')):
        for f in files:
            if not f.endswith(('.cpp', '.hpp', '.inc', '.h')):
                continue
            p = os.path.join(dirpath, f)
            if p == hpp:
                continue
            s = open(p, encoding='utf-8', errors='surrogateescape').read()
            t = pat.sub(lambda m: m.group(1) + ren[m.group(2)], s)
            if f == 'code_slots.inc':
                t = spat.sub(lambda m: '"%s"' % ren[m.group(1)], t)
            if t != s:
                open(p, 'w', encoding='utf-8', errors='surrogateescape', newline='').write(t)
                changed[os.path.relpath(p, ROOT)] += 1
    print('uses rewritten in %d files' % len(changed))


if __name__ == '__main__':
    main()
