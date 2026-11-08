#!/usr/bin/env python3
"""List / compare every function registration of src/** (JOAN_REPLACE* invocations, through module-local macros).

Usage (from the repository root):
  check_registrations.py [--hook hook] --dump out.tsv        write one line per registration (sorted)
  check_registrations.py [--hook hook] --compare before.tsv  compare the current tree with an earlier dump

A registration is (address, name, module key, kind, extra), kind = the JOAN_REPLACE* macro (plain, YIELDING, IMPORT; the OPT_IN names of older trees count as plain),
extra = the yielding module / import slot variable. The dump also records the file and the function expression;
--compare requires the multiset of (address, name, module, kind, extra) to be identical (moves between files are
listed, not errors) and reports every address registered more than once whose first registration in path order
changed (re/fn_scan.py registry_names: the first one names the address in src/fn_names.hpp).

The macros are expanded textually (function-like #define / #undef of the file in order, `#` stringizing, `##`
pasting, nested module macros), which covers every registration form src/** uses.
"""
import argparse
import collections
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fn_scan as F  # noqa: E402

JOAN = {'JOAN_REPLACE': 'plain', 'JOAN_REPLACE_YIELDING': 'yielding', 'JOAN_REPLACE_IMPORT': 'import',
        'JOAN_REPLACE_OPT_IN': 'plain', 'JOAN_REPLACE_OPT_IN_YIELDING': 'yielding', 'JOAN_REPLACE_IMPORT_OPT_IN': 'import'}
DEF_RE = re.compile(r'^[ \t]*#[ \t]*(define|undef)[ \t]+([A-Za-z_]\w*)(\(([^)]*)\))?((?:[^\n]*\\\n)*[^\n]*)', re.M)
IDENT_RE = re.compile(r'\b([A-Za-z_]\w*)\s*\(')
STR_RE = re.compile(r'"((?:\\.|[^"\\])*)"')


def substitute(body, params, args):
    """body of a function-like macro with the arguments put in (# and ## as the preprocessor does it)"""
    amap = dict(zip(params, args))
    toks = re.findall(r'"(?:\\.|[^"\\])*"|##|#|[A-Za-z_]\w*|\s+|.', body)
    out = []
    i = 0
    while i < len(toks):
        t = toks[i]
        if t == '#' and i + 1 < len(toks):
            j = i + 1
            while j < len(toks) and toks[j].isspace():
                j += 1
            if j < len(toks) and toks[j] in amap:
                out.append('"' + amap[toks[j]].strip().replace('\\', '\\\\').replace('"', '\\"') + '"')
                i = j + 1
                continue
        if t == '##':
            while out and out[-1].isspace():
                out.pop()
            j = i + 1
            while j < len(toks) and toks[j].isspace():
                j += 1
            if j < len(toks):
                nxt = amap[toks[j]].strip() if toks[j] in amap else toks[j]
                out.append(('' if not out else out.pop()) + nxt)
                i = j + 1
                continue
        out.append(amap[t].strip() if t in amap else t)
        i += 1
    return ''.join(out)


def objlike(text, macros):
    """text with object-like macros of the file replaced (one level is all the modules use)"""
    return re.sub(r'\b[A-Za-z_]\w*\b', lambda m: macros[m.group(0)][1].strip()
                  if m.group(0) in macros and macros[m.group(0)][0] is None else m.group(0), text)


def str_value(text):
    """concatenated value of the adjacent string literals in text (None if text has none)"""
    parts = STR_RE.findall(text)
    if not parts:
        return None
    return ''.join(parts)


def expand(text, macros, depth, out, ctx):
    """find registration macro invocations in text (stripped of comments), expanding module macros"""
    code = F.strip(text, strings=False)
    # blank string literal contents for the identifier search only
    blank = F.strip(text, strings=True)
    pos = 0
    for m in IDENT_RE.finditer(blank):
        if m.start() < pos:
            continue
        name = m.group(1)
        if name not in JOAN and (name not in macros or macros[name][0] is None):
            continue
        spans, close = F.split_args(code, m.end() - 1)
        args = [code[a:b] for a, b in spans]
        pos = close + 1
        if name in JOAN:
            kind = JOAN[name]
            if len(args) < 4:
                continue
            args = [objlike(a, macros) for a in args]
            mod, nm, key, fn = (args[0].strip(), args[1].strip(), args[2].strip(), args[3].strip())
            extra = ','.join(a.strip() for a in args[4:])
            if kind == 'yielding':
                extra = str_value(extra) or extra
            mv, nv = str_value(mod), str_value(nm)
            if mv is None or nv is None:
                ctx['unparsed'].append((ctx['file'], ctx['line'], ' '.join((name + '(' + ', '.join(args) + ')').split())))
                continue
            try:
                addr = '0x%08x' % int(key.rstrip('uUlL'), 0)
            except ValueError:
                addr = key  # import slot variable
            out.append({'address': addr, 'name': nv, 'module': mv, 'kind': kind, 'extra': extra,
                        'file': ctx['file'], 'line': ctx['line'], 'fn': ' '.join(fn.split())})
        else:
            params, body = macros[name]
            if depth > 8 or (params is not None and len(params) != len(args) and not params[-1:] == ['...']):
                continue
            if params is None:
                continue
            expand(substitute(body, params, args), macros, depth + 1, out, ctx)


def scan_file(rel, path):
    src = open(path, errors='replace').read()
    code = F.strip(src, strings=False)
    macros = {}
    out = []
    ctx = {'file': rel, 'line': 0, 'unparsed': []}
    events = []
    for m in DEF_RE.finditer(code):
        events.append((m.start(), m.end(), m))
    pos = 0
    for start, end, m in events + [(len(code), len(code), None)]:
        chunk = code[pos:start]
        if chunk.strip():
            # line of each registration: expand piecewise per statement for line numbers
            for sm in re.finditer(r'[^;]*;|[^;]+$', chunk):
                ctx['line'] = code.count('\n', 0, pos + sm.start() + (len(sm.group(0)) - len(sm.group(0).lstrip()))) + 1
                expand(sm.group(0), macros, 0, out, ctx)
        if m is None:
            break
        pos = end
        if m.group(1) == 'undef':
            macros.pop(m.group(2), None)
        elif m.group(3) is not None:
            params = [p.strip() for p in m.group(4).split(',')] if m.group(4).strip() else []
            macros[m.group(2)] = (params, m.group(5).replace('\\\n', ' '))
        else:
            macros[m.group(2)] = (None, m.group(5).replace('\\\n', ' '))
    return out, ctx['unparsed']


def scan(hook_dir):
    regs, unparsed = [], []
    for rel, path in F.hook_files(hook_dir):
        if not rel.endswith(('.cpp', '.inc')):
            continue
        r, u = scan_file(rel, path)
        regs += r
        unparsed += u
    return regs, unparsed


def key(r):
    return (r['address'], r['name'], r['module'], r['kind'], r['extra'])


def dump(regs, path):
    with open(path, 'w') as f:
        for r in sorted(regs, key=lambda r: key(r) + (r['file'], r['line'])):
            f.write('\t'.join(key(r) + (r['file'], str(r['line']), r['fn'])) + '\n')


def load(path):
    regs = []
    for line in open(path):
        p = line.rstrip('\n').split('\t')
        regs.append({'address': p[0], 'name': p[1], 'module': p[2], 'kind': p[3], 'extra': p[4], 'file': p[5],
                     'line': int(p[6]), 'fn': p[7]})
    return regs


def first_names(regs):
    """address -> (file, name) of the first registration in path order (fn_scan.registry_names)"""
    first = {}
    for r in sorted(regs, key=lambda r: (r['file'], r['line'])):
        first.setdefault(r['address'], (r['file'], r['name']))
    return first


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--hook', default='src')
    ap.add_argument('--dump')
    ap.add_argument('--compare')
    a = ap.parse_args()
    regs, unparsed = scan(a.hook)
    for u in unparsed:
        print('unparsed: %s:%d: %s' % u)
    print('%d registrations in %d files' % (len(regs), len({r['file'] for r in regs})))
    if a.dump:
        dump(regs, a.dump)
    if a.compare:
        old = load(a.compare)
        co, cn = collections.Counter(map(key, old)), collections.Counter(map(key, regs))
        bad = 0
        for k in sorted(set(co) | set(cn)):
            if co[k] != cn[k]:
                print('MISMATCH %s: before %d, after %d' % ('\t'.join(k), co[k], cn[k]))
                bad += 1
        of, nf = {key(r): r['file'] for r in old}, {key(r): r['file'] for r in regs}
        moved = collections.Counter((of[k], nf[k]) for k in nf if k in of and of[k] != nf[k])
        for (o, n), c in sorted(moved.items()):
            print('moved %3d  %s -> %s' % (c, o, n))
        fo, fn = first_names(old), first_names(regs)
        multi = {k for k, c in collections.Counter(r['address'] for r in regs).items() if c > 1}
        for addr in sorted(multi):
            if fo.get(addr, (None, None))[1] != fn.get(addr, (None, None))[1]:
                print('FIRST NAME CHANGED %s: %s -> %s' % (addr, fo.get(addr), fn.get(addr)))
                bad += 1
        print('%d registrations compared, %d moved, %d problems' % (len(regs), sum(moved.values()), bad))
        sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
