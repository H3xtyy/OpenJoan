"""Build a function database for joan.exe from objdump output + PE metadata.

Outputs functions.json: list of {addr, end, size, file, strings, calls, callers, imports}
"""
import json
import re
import sys
from collections import defaultdict

from pe import PE

exe = sys.argv[1] if len(sys.argv) > 1 else 'joan.exe'
asm = sys.argv[2] if len(sys.argv) > 2 else 'joan.asm'
pe = PE(exe)
imps = pe.imports()
text = pe.section('.text')
TLO, THI = pe.base + text[1], pe.base + text[1] + text[2]


def in_text(v):
    return TLO <= v < THI


line_re = re.compile(r'^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$')
hexnum = re.compile(r'0x([0-9a-f]+)')

insns = []  # (addr, mnem, ops)
for ln in open(asm):
    m = line_re.match(ln)
    if not m:
        continue
    insns.append((int(m.group(1), 16), m.group(2), m.group(3)))
print('instructions', len(insns))

# Incremental-linking thunk table (/INCREMENTAL): 'jmp target' stubs in 0x401000..0x414000
THUNK_END = 0x414000
thunks = {}
for a, mn, ops in insns:
    if a >= THUNK_END:
        break
    if mn == 'jmp':
        m = re.match(r'^0x([0-9a-f]+)', ops)
        if m:
            thunks[a] = int(m.group(1), 16)


def res(v):
    return thunks.get(v, v)

starts = {pe.entry}
call_edges = []
for a, mn, ops in insns:
    if mn == 'call':
        m = re.match(r'^0x([0-9a-f]+)', ops) or re.match(r'^([0-9a-f]+) <', ops)
        if m:
            t = res(int(m.group(1), 16))
            if in_text(t):
                starts.add(t)
                call_edges.append((a, t))

# pointers into .text that live in data sections (vtables, callbacks, jump tables)
relocs = pe.relocs()
data_ptrs = 0
code_ptr_in_code = set()
for r in relocs:
    v = res(pe.u32(r))
    if in_text(v):
        if not in_text(r):
            starts.add(v)
            data_ptrs += 1
        else:
            code_ptr_in_code.add(v)  # push offset func / jump tables

# addresses used as `push offset func` / mov reg, func (callbacks)
for a, mn, ops in insns:
    if mn in ('push', 'mov'):
        for h in hexnum.findall(ops):
            v = int(h, 16)
            if v in thunks and mn in ('push', 'mov'):
                starts.add(thunks[v])
            if v in code_ptr_in_code and mn == 'push':
                starts.add(v)

# Prologue heuristic after padding (int3/nop runs between functions)
prev_pad = False
for a, mn, ops in insns:
    if prev_pad and mn not in ('int3', 'nop', '(bad)'):
        starts.add(a)
    prev_pad = mn in ('int3',)

starts = sorted(s for s in starts if in_text(s) and s >= THUNK_END)
# validate starts are instruction boundaries
addrset = set(a for a, _, _ in insns)
starts = [s for s in starts if s in addrset]
print('function starts', len(starts), 'from data ptrs', data_ptrs)

import bisect
idx_of = {}


def func_of(a):
    i = bisect.bisect_right(starts, a) - 1
    return starts[i] if i >= 0 else None


funcs = {s: dict(addr=s, strings=[], calls=set(), callers=set(), imports=set(), ninsn=0)
         for s in starts}
ends = {}
for i, s in enumerate(starts):
    ends[s] = starts[i + 1] if i + 1 < len(starts) else THI

rdata = pe.section('.rdata'); data = pe.section('.data')
DLO = pe.base + rdata[1]
DHI = pe.base + data[1] + data[2]

src_re = re.compile(rb'^[a-zA-Z]:\\ww\\.*\.(cpp|h|c)$', re.I)
for a, mn, ops in insns:
    f = func_of(a)
    if f is None:
        continue
    F = funcs[f]
    F['ninsn'] += 1
    for h in hexnum.findall(ops):
        v = int(h, 16)
        if v in imps:
            F['imports'].add(imps[v])
        elif DLO <= v < DHI:
            s = pe.cstr(v, 300)
            if s and len(s) >= 4 and all(32 <= c < 127 or c in (9, 10, 13) for c in s):
                F['strings'].append(s.decode('latin1'))
    if mn == 'call':
        m = re.match(r'^0x([0-9a-f]+)', ops) or re.match(r'^([0-9a-f]+) <', ops)
        if m:
            t = res(int(m.group(1), 16))
            if t in funcs:
                F['calls'].add(t)
                funcs[t]['callers'].add(f)

# Assign source files: direct __FILE__ refs, then propagate along call graph neighbourhood
for F in funcs.values():
    files = [s for s in F['strings'] if src_re.match(s.encode('latin1'))]
    F['file'] = files[0].lower().replace('d:\\ww\\', '').replace('\\', '/') if files else None
    F['file_src'] = 'assert' if files else None

# Propagation: a function whose neighbours by address (between two functions of same file) inherits it.
ordered = [funcs[s] for s in starts]
known = [i for i, F in enumerate(ordered) if F['file']]
for k in range(len(known) - 1):
    i, j = known[k], known[k + 1]
    if ordered[i]['file'] == ordered[j]['file']:
        for t in range(i + 1, j):
            if not ordered[t]['file']:
                ordered[t]['file'] = ordered[i]['file']
                ordered[t]['file_src'] = 'between'

out = []
for s in starts:
    F = funcs[s]
    out.append(dict(addr='%08x' % s, end='%08x' % ends[s], size=ends[s] - s, ninsn=F['ninsn'],
                    file=F['file'], file_src=F['file_src'],
                    strings=sorted(set(F['strings']))[:20],
                    calls=['%08x' % c for c in sorted(F['calls'])],
                    ncallers=len(F['callers']),
                    imports=sorted(F['imports'])))
json.dump(out, open('functions.json', 'w'), indent=0)
json.dump({'%08x' % k: v for k, v in imps.items()}, open('imports.json', 'w'), indent=0)
json.dump({'%08x' % k: '%08x' % v for k, v in thunks.items()}, open('thunks.json', 'w'), indent=0)
print('written functions.json')
