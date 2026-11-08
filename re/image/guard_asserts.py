#!/usr/bin/env python3
"""Wrap the x86-only static_asserts of hand-written headers in `#if UINTPTR_MAX == 0xffffffffu` (docs/X64.md).
Input: lines `path:line` of static_asserts that fail on x86_64; every contiguous run of static_assert lines around
such a line is wrapped (the same text in re/layout/hand_orig/ when present, so gen_types keeps it).
    re/image/guard_asserts.py <lines.txt>
"""
import collections, os, re, sys
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
GUARD = '#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout'
by = collections.defaultdict(set)
for l in open(sys.argv[1]):
    p, n = l.strip().rsplit(':', 1)
    by[os.path.normpath(os.path.join(ROOT, 'src', p))].add(int(n) - 1)
def wrap(lines, idx):
    runs = set()
    for i in idx:
        a = b = i
        while a > 0 and lines[a - 1].lstrip().startswith('static_assert'):
            a -= 1
        while b + 1 < len(lines) and lines[b + 1].lstrip().startswith('static_assert'):
            b += 1
        if a > 0 and lines[a - 1].startswith('#if UINTPTR_MAX'):
            continue
        runs.add((a, b))
    for a, b in sorted(runs, reverse=True):
        lines[b + 1:b + 1] = ['#endif']
        lines[a:a] = [GUARD]
    return lines, len(runs)
for path, idx in by.items():
    lines = open(path).read().split('\n')
    texts = [lines[i] for i in idx]
    lines, n = wrap(lines, idx)
    if not any('<cstdint>' in l for l in lines):
        k = max(i for i, l in enumerate(lines) if l.startswith('#include'))
        lines.insert(k + 1, '#include <cstdint>')
    open(path, 'w').write('\n'.join(lines))
    ho = os.path.join(ROOT, 're', 'layout', 'hand_orig', os.path.basename(path))
    if os.path.exists(ho):
        hl = open(ho).read().split('\n')
        hidx = [i for i, l in enumerate(hl) if l in texts]
        hl, _ = wrap(hl, hidx)
        if not any('<cstdint>' in l for l in hl):
            k = max(i for i, l in enumerate(hl) if l.startswith('#include'))
            hl.insert(k + 1, '#include <cstdint>')
        open(ho, 'w').write('\n'.join(hl))
    print(os.path.relpath(path, ROOT), n, 'runs')
