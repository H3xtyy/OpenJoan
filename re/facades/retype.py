#!/usr/bin/env python3
"""Facade declaration retypes (rounds P8, X64B): re/facades/retype_p8.json + retype_x64b.json -> the declaration lines of
src/original/gen/*_methods.inc. Each entry names the .inc, the original address (the `/// 0xaddr` comment above the
declaration), the parameter index or "ret", the type it has now ("from") and the new type ("to"); an entry already
applied is skipped, a "from" that does not match is an error. Inline facades (a body after the declaration) get the
same parameter list. Then run re/bind/sync_bridges.py (bridge definitions follow the declarations).

    re/facades/retype.py [--check]
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
GEN = os.path.join(ROOT, 'src', 'original', 'gen')
SPECS = ('retype_p8.json', 'retype_x64b.json', 'retype_phase2.json')  # rounds P8, X64B (re/a4/x64a_requests), Phase 2 (re/x64_census/reports)
DECL = re.compile(r'^(\s*(?:template <[^>]*>\s*)?)(.+?)\s+(\w+)\((.*?)\)(\s*(?:const\s*)?(?:noexcept\s*)?[;{].*)$')


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
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--check', action='store_true')
    a = ap.parse_args()
    spec = [r for f in SPECS for r in json.load(open(os.path.join(HERE, f)))['retypes']]
    files, done, errors = {}, 0, []
    for r in spec:
        p = os.path.join(GEN, r['inc'])
        lines = files.setdefault(p, open(p).read().split('\n'))
        addr = int(r['addr'], 16)
        idx = next((i + 1 for i, l in enumerate(lines) if re.match(rf'^\s*/// 0x{addr:08x}\s', l)), None)
        if idx is None:
            errors.append(f"{r['inc']} {r['addr']}: no declaration")
            continue
        # a template facade has its `template <...>` line between the comment and the declaration
        if lines[idx].startswith('template'):
            idx += 1
        m = DECL.match(lines[idx])
        if not m:
            errors.append(f"{r['inc']} {r['addr']}: declaration not understood: {lines[idx]}")
            continue
        pre, ret, name, params, post = m.groups()
        ps = split_params(params)
        if r['param'] == 'ret':
            cur = ret.strip()
            # an inline facade's call names the result type too (::joan::call::thiscall<R>(...))
            body = post.replace(f"<{r['from']}>(", f"<{r['to']}>(")
            if cur == r['to'] and body == post:
                continue
            if cur not in (r['from'], r['to']):
                errors.append(f"{r['inc']} {name}: result is {cur}, expected {r['from']}")
                continue
            ret, post = r['to'], body
        else:
            k = int(r['param'])
            t, _, nm = ps[k].rpartition(' ')
            if t.strip() == r['to']:
                continue
            if t.strip() != r['from']:
                errors.append(f"{r['inc']} {name}: a{k} is {t}, expected {r['from']}")
                continue
            ps[k] = f"{r['to']} {nm}"
        lines[idx] = f"{pre}{ret} {name}({', '.join(ps)}){post}"
        done += 1
    if errors:
        sys.exit('\n'.join(errors))
    if not a.check:
        for p, lines in files.items():
            open(p, 'w').write('\n'.join(lines))
    print(f'{done} retypes applied' + (' (check only)' if a.check else ''))


if __name__ == '__main__':
    main()
