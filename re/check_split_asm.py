#!/usr/bin/env python3
"""Check that functions moved between hook files (re/split_modules.py) compile to the same code.

Usage (from anywhere):
  check_split_asm.py --before <repo root> --after <repo root> --shim DIR [--shim DIR] [--jobs 2] [-v]

For every registration whose file differs between the two trees (re/check_registrations.py), both files are compiled
with clang++ (MSVC target, the syntax checker's flags, -O0 -S) and the assembly of the registered function is
compared after normalising the names: the anonymous-namespace hash, the per-module block namespace of the target file
(`namespace game_b102_12`) and local label numbers are removed. Every other function of the same block (helpers,
template instantiations, members the moved function calls) that exists under the same normalised name in the old
file is compared as well, so a helper copied with different text, a dropped attribute (alignas) or a missing
destructor shows up. Exit status 1 on any difference.
"""
import argparse
import collections
import concurrent.futures
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check_registrations as CR  # noqa: E402

FUNC_RE = re.compile(r'^("?)(\?[^\s":]+)\1:\s*(?:#.*)?$', re.M)
ANON_RE = re.compile(r'\?A0x[0-9A-Fa-f]+@')
LABEL_RE = re.compile(r'(\.?L(?:BB|tmp|CPI|func_end|JTI)[0-9_]+|LBB[0-9_]+|\.?L_?[A-Za-z_]+\.\d+|__unnamed_\d+)')


def flags(shims):
    return ['-std=c++20', '--target=i686-pc-windows-msvc', '-fms-extensions', '-fms-compatibility', '-nostdinc++'] + \
        ['-I' + s for s in shims] + ['-I.', '-DWIN32_LEAN_AND_MEAN', '-DNOMINMAX', '-include', 'registry.hpp',
                                     '-include', 'data_names.hpp', '-include', 'fn_names.hpp', '-O0', '-S',
                                     '-w', '-fno-asynchronous-unwind-tables']


def compile_asm(root, rel, shims, out_dir):
    out = os.path.join(out_dir, re.sub(r'[^\w.]', '_', root) + '__' + rel.replace('/', '__') + '.s')
    p = subprocess.run(['clang++'] + flags(shims) + [rel, '-o', out], cwd=os.path.join(root, 'src'),
                       capture_output=True, text=True)
    if p.returncode != 0:
        return None, p.stderr[-2000:]
    return open(out).read(), None


def functions(asm):
    """{mangled name: [body lines]} of an assembly listing"""
    res = {}
    ms = list(FUNC_RE.finditer(asm))
    for i, m in enumerate(ms):
        name = m.group(2)
        end = ms[i + 1].start() if i + 1 < len(ms) else len(asm)
        body = asm[m.end():end]
        if not body.lstrip().startswith('# %bb.0') or name.startswith(('??__E', '??__F')):
            continue  # data, or a dynamic initializer (registrations are named after their line)
        m2 = re.search(r'^\s*\.?Lfunc_end\d+:|# -- End function', body, re.M)
        if m2:
            body = body[:m2.start()]
        res[name] = body
    return res


SYM_RE = re.compile(r'"(\?[^"]+)"|(\?[^\s,()"]+)')
_demangled = {}


def demangle_all(names):
    todo = sorted(n for n in set(names) if n not in _demangled)
    if not todo:
        return
    # llvm-undname memorises the anonymous namespace (?A0x<hash>@) as a back-reference, clang's mangler does
    # not: drop it before demangling so later back-references resolve to the right names
    fixed = {n: re.sub(r'\?A0x[0-9A-Fa-f]{1,8}@', '', n) for n in todo}
    p = subprocess.run(['llvm-undname'], input='\n'.join(fixed[n] for n in todo) + '\n', capture_output=True,
                       text=True)
    out = p.stdout.split('\n')
    # output per input line: the input, its demangling (or an error), a blank line
    pos = 0
    for n in todo:
        while pos < len(out) and out[pos].strip() != fixed[n]:
            pos += 1
        if pos + 1 < len(out) and not out[pos + 1].startswith('error'):
            _demangled[n] = out[pos + 1].strip()
            pos += 2
        else:
            _demangled[n] = n


def clean(text, block_ns):
    """a demangled name without the anonymous namespace (and its hash) and the block namespace"""
    text = text.replace("`anonymous namespace'::", '')
    text = re.sub(r'\b0x[0-9A-F]{1,8}::', '', text)
    text = re.sub(r'\b0x[0-9A-F]{1,8}\b', '<anon>', text)
    for ns in block_ns:
        text = re.sub(r'\b' + re.escape(ns) + '::', '', text)
    text = re.sub(r'<lambda_\d+>', '<lambda>', text)
    # llvm-undname resolves back-references inside template arguments inconsistently (enum call::fn::Code vs
    # enum joan::fn::Code for the same symbol): compare unqualified names. Callees are still compared by body.
    head, sep, rest = text.partition('(')
    head = re.sub(r'\b(struct|class|enum|union) (?:[\w<>`\']+::)+', r'\1 ', head)
    text = head + sep + re.sub(r'(?:[\w<>`\']+::)+', '', rest)
    return text


def norm_name(name, block_ns):
    demangle_all([name])
    return clean(_demangled.get(name, name), block_ns)


def norm_body(body, block_ns):
    lines = []
    labels = {}
    syms = [a or b for a, b in SYM_RE.findall(body)]
    demangle_all(syms)

    def lab(m):
        return labels.setdefault(m.group(0), 'L%d' % len(labels))

    def sym(m):
        n = m.group(1) or m.group(2)
        return '<' + clean(_demangled.get(n, n), block_ns) + '>'
    for ln in body.split('\n'):
        s = ln.split('#', 1)[0].strip() if not ln.strip().startswith('.') else ln.strip()
        if not s or s.startswith(('.p2align', '.def', '.scl', '.type', '.endef', '.section', '.globl', '.linkonce',
                                  '.file', '.loc', '.cv_', '# %bb')):
            continue
        s = SYM_RE.sub(sym, s)
        s = LABEL_RE.sub(lab, s)
        lines.append(s)
    return lines


def block_ns_name(rel):
    """namespace of a moved block: the batch module's name (modules/game_b98_03.cpp -> game_b98_03), other files
    with their directory (ilib/file.cpp -> ilib_file)"""
    stem = os.path.splitext(rel)[0]
    if stem.startswith('modules/'):
        stem = stem[len('modules/'):]
    return re.sub(r'\W', '_', stem)


_ns_cache = {}


def enclosing_ns(root, rel, line):
    """the innermost `namespace X {` block (closed by `}  // namespace X`) around a line of a split file"""
    key = (root, rel)
    if key not in _ns_cache:
        spans = []
        src = open(os.path.join(root, 'src', rel)).read().split('\n')
        opened = []
        for i, ln in enumerate(src, 1):
            m = re.match(r'^namespace (\w+) \{', ln)
            if m:
                opened.append((m.group(1), i))
            m = re.match(r'^\}  // namespace (\w+)\s*$', ln)
            if m:
                for j in range(len(opened) - 1, -1, -1):
                    if opened[j][0] == m.group(1):
                        spans.append((opened[j][1], i, m.group(1)))
                        del opened[j]
                        break
        _ns_cache[key] = spans
    best = None
    for b, e, n in _ns_cache[key]:
        if b <= line <= e and (best is None or b > best[0]):
            best = (b, e, n)
    return best[2] if best else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--before', required=True)
    ap.add_argument('--after', required=True)
    ap.add_argument('--shim', action='append', default=[])
    ap.add_argument('--jobs', type=int, default=2)
    ap.add_argument('-v', '--verbose', action='store_true')
    ap.add_argument('--only', help='regex: only moved registrations whose old or new file matches')
    a = ap.parse_args()
    shims = [os.path.abspath(s) for s in a.shim]
    before, _ = CR.scan(os.path.join(a.before, 'src'))
    after, _ = CR.scan(os.path.join(a.after, 'src'))
    bmap = {CR.key(r): r for r in before}
    moved = [(bmap[CR.key(r)], r) for r in after if CR.key(r) in bmap and bmap[CR.key(r)]['file'] != r['file']]
    if a.only:
        moved = [(o, n) for o, n in moved if re.search(a.only, o['file']) or re.search(a.only, n['file'])]
    print('%d moved registrations' % len(moved))
    jobs = set()
    for old, new in moved:
        jobs.add((a.before, old['file']))
        jobs.add((a.after, new['file']))
    tmp = tempfile.mkdtemp(prefix='split_asm_')
    asm = {}
    with concurrent.futures.ThreadPoolExecutor(a.jobs) as ex:
        futs = {ex.submit(compile_asm, root, rel, shims, tmp): (root, rel) for root, rel in jobs}
        for f in concurrent.futures.as_completed(futs):
            asm[futs[f]] = f.result()
    bad = 0
    for k, (text, err) in asm.items():
        if text is None:
            print('COMPILE FAILED %s/%s: %s' % (k[0], k[1], err.strip().split('\n')[-1]))
            bad += 1
    # pairs (old file, new file) and the block namespaces of the new file
    pairs = collections.defaultdict(list)
    for old, new in moved:
        # from one split file to another: the block namespaces around the registrations (old -> new)
        ons = None if old['file'].startswith('modules/') else enclosing_ns(a.before, old['file'], old['line'])
        nns = enclosing_ns(a.after, new['file'], new['line']) if ons else None
        pairs[(old['file'], new['file'], ons, nns)].append((old, new))
    checked_fns = 0
    for (of, nf, ons, nns), regs in sorted(pairs.items(), key=lambda kv: tuple(str(x) for x in kv[0])):
        ot, nt = asm[(a.before, of)][0], asm[(a.after, nf)][0]
        if ot is None or nt is None:
            continue
        if ons:
            block_ns, old_ns = [nns] if nns else [], [ons]
        else:
            stem = block_ns_name(of)
            nsre = re.compile(r'^namespace (%s(?:_\d+)?) \{' % re.escape(stem), re.M)
            src = open(os.path.join(a.after, 'src', nf)).read()
            block_ns = sorted(set(nsre.findall(src)), key=len, reverse=True)
            old_ns = []
        ofn, nfn = functions(ot), functions(nt)
        demangle_all(list(ofn) + list(nfn))
        syms = []
        for body in list(ofn.values()) + list(nfn.values()):
            syms += [x or y for x, y in SYM_RE.findall(body)]
        demangle_all(syms)
        onorm = collections.defaultdict(list)
        for n, body in ofn.items():
            if old_ns and not any('@' + x + '@' in n for x in old_ns) and any(
                    re.search(r'@game_|@en3d_|@lib_|@game\w*_\d', n) for _ in [0]):
                continue  # another block of the old split file
            onorm[norm_name(n, old_ns)].append(body)
        # roots: the moved registered functions (found by name in the new file); then everything they reference
        # that the new file defines (helpers, instantiations, members), compared with the old file's function of
        # the same normalised name
        roots = []
        for old, new in regs:
            m = re.match(r'&\s*(?:(\w+)::)?(\w+)$', new['fn'])
            if not m:
                continue
            pat = re.compile(r'(?<![\w:])' + (re.escape(m.group(1)) + '::' if m.group(1) else '') +
                             re.escape(m.group(2)) + r'\(')
            hit = [n for n in nfn if (any('@' + ns + '@' in n for ns in block_ns) or not block_ns) and
                   pat.search(norm_name(n, block_ns))]
            ohit = [n for n in ofn if pat.search(norm_name(n, old_ns))]
            if not hit and ohit:
                print('NOT FOUND %s (%s) in %s' % (new['name'], new['fn'], nf))
                bad += 1
            roots += hit
        seen, stack = set(), list(roots)
        while stack:
            n = stack.pop()
            if n in seen:
                continue
            seen.add(n)
            for x, y in SYM_RE.findall(nfn[n]):
                ref = x or y
                if ref in nfn and ref not in seen:
                    stack.append(ref)
        for n in sorted(seen):
            nn = norm_name(n, block_ns)
            if nn not in onorm and block_ns and not any('@' + ns + '@' in n for ns in block_ns):
                # a definition the target file already had (e.g. ilib/file.cpp's own File::file_read) that the
                # old file only declared: the same external function, resolved at link time
                if a.verbose:
                    print('EXT %s -> %s: %s (defined by the target file itself)' % (of, nf, nn))
                continue
            if nn not in onorm:
                print('NEW %s -> %s: %s (no function of that name in the old file)' % (of, nf, nn))
                bad += 1
                continue
            nb = norm_body(nfn[n], block_ns)
            if not any(norm_body(ob, old_ns) == nb for ob in onorm[nn]):
                print('DIFF %s -> %s: %s' % (of, nf, nn))
                if a.verbose:
                    import difflib
                    d = list(difflib.unified_diff(norm_body(onorm[nn][0], old_ns), nb, lineterm='', n=0))
                    print('\n'.join('    ' + x for x in d[2:14]))
                bad += 1
            checked_fns += 1
        mine = seen
        if a.verbose:
            print('%s -> %s: %d registrations, %d functions compared' % (of, nf, len(regs), len(mine)))
    print('%d functions compared, %d problems' % (checked_fns, bad))
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
