#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Re-sync the bridge definitions (re/bind/gen_bridges.py output at the end of the replacement sources) with the
facade declarations after the declarations were retyped (docs/X64.md round F):

    auto joan::orig::C::f(<params of the declaration> a0, ...) -> <result of the declaration> {
        [return joan::bind::conv<<result>>(] joan::bind::member<&...>(this, a0, ...) [)]; }  // 0xaddr

    re/bind/sync_bridges.py [--dry-run]
"""
import argparse
import glob
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
SRC = os.path.join(ROOT, 'src')
GEN = os.path.join(SRC, 'original', 'gen')
DECL = re.compile(r'^\s*(?:inline\s+|static\s+)*(.+?)\s+(\w+)\((.*?)\)\s*(const\s*)?(?:noexcept\s*)?[;{]')
BRIDGE = re.compile(r'^auto joan::orig::(?:([\w:]+)::)?(\w+)\((.*?)\)( const)? -> (.+?) \{ (.*)\}  // (0x[0-9a-f]+)\s*$')


def split_params(s):
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch in '<(':
            depth += 1
        elif ch in '>)':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return [re.sub(r'\s+a\d+$', '', p) for p in out]


def declarations():
    decl = {}
    files = glob.glob(os.path.join(GEN, '*_methods.inc')) + [os.path.join(GEN, 'free_functions.hpp')]
    for p in files:
        lines = open(p).read().split('\n')
        for i, l in enumerate(lines):
            m = re.match(r'^\s*/// 0x([0-9a-f]+)\s', l)
            if not m or i + 1 >= len(lines):
                continue
            d = DECL.match(lines[i + 1])
            if not d:
                continue
            ret, name, params, const = d.groups()
            decl[(int(m.group(1), 16), name)] = (ret.strip(), split_params(params), bool(const))
    return decl


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--dry-run', action='store_true')
    a = ap.parse_args()
    decl = declarations()
    changed_files, n_changed, missing = 0, 0, 0
    for path in glob.glob(os.path.join(SRC, '**', '*.cpp'), recursive=True):
        text = open(path, encoding='utf-8').read()
        if 'joan::bind::' not in text:
            continue
        lines = text.split('\n')
        dirty = False
        for i, l in enumerate(lines):
            m = BRIDGE.match(l)
            if not m:
                continue
            scope, name, params, _, ret, body, addr = m.groups()
            key = (int(addr, 16), name)
            if key not in decl:
                missing += 1
                continue
            dret, dparams, const = decl[key]
            if dparams == split_params(params) and dret == ret:
                continue
            plist = ', '.join(f'{t} a{k}' for k, t in enumerate(dparams))
            m2 = re.match(r'return joan::bind::conv<(.+?)>\((.*)\); $', body)
            if dret == 'void':
                call = m2.group(2) if m2 else body.strip().rstrip(';').strip()
                nbody = f'{call}; '
            else:
                call = m2.group(2) if m2 else body.strip().rstrip(';').strip()
                nbody = f'return joan::bind::conv<{dret}>({call}); '
            lines[i] = f'auto joan::orig::{scope + "::" if scope else ""}{name}({plist}){" const" if const else ""} -> {dret} {{ {nbody}}}  // {addr}'
            dirty = True
            n_changed += 1
        if dirty:
            changed_files += 1
            if not a.dry_run:
                open(path, 'w', encoding='utf-8').write('\n'.join(lines))
    print(f'{n_changed} bridge definitions re-synced in {changed_files} files; {missing} bridges without a declaration')


if __name__ == '__main__':
    main()
