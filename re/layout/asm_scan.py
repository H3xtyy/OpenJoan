#!/usr/bin/env python3
"""Struct recovery from the original binary (docs/X64.md): joan.exe is an MSVC 6 /Od build, so every member
function saves `this` (ecx) to [ebp-4] and reloads it before each use. A linear register tracker over joan.asm gives,
per function, ground-truth field accesses with their widths, pointer fields (a field loaded and then dereferenced),
sub-objects (`lea ecx,[this+k]; call f`), same-`this` calls, vtable stores and the size passed to operator new before
a constructor call.

    re/layout/asm_scan.py [--re ../re] [-o re/layout/out/asm_facts.json]

Paths: () = this; (o1,) = the object the pointer at this+o1 points to; (o1, o2) = one level further; a sub-object at
this+k is the same path with the offset added (accesses are relative to the path's object, `add` folded in).
"""
import argparse
import bisect
import collections
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
INS = re.compile(r'^\s+([0-9a-f]+):\t(\S+)\s*(.*)$')
MEM = re.compile(r'(?:(BYTE|WORD|DWORD|QWORD|TBYTE|XMMWORD) PTR )?(?:[a-z]s:)?\[([a-z]{3})(?:\+([a-z]{3})\*(\d))?(?:([+-])0x([0-9a-f]+))?\]')
REGS32 = ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi')
SUB = {'al': 'eax', 'ah': 'eax', 'ax': 'eax', 'bl': 'ebx', 'bh': 'ebx', 'bx': 'ebx', 'cl': 'ecx', 'ch': 'ecx',
       'cx': 'ecx', 'dl': 'edx', 'dh': 'edx', 'dx': 'edx', 'si': 'esi', 'di': 'edi'}
WIDTH = {'BYTE': 1, 'WORD': 2, 'DWORD': 4, 'QWORD': 8, 'TBYTE': 10, 'XMMWORD': 16}
RDATA = (0x9cf000, 0x9f3c86)
OP_NEW = 0x7b1d35
NOWRITE = ('cmp', 'test', 'push', 'fld', 'fild', 'fcom', 'fcomp', 'fadd', 'fsub', 'fmul', 'fdiv', 'fsubr', 'fdivr',
           'fiadd', 'fisub', 'fimul', 'fidiv', 'ficom', 'ficomp', 'movsx', 'movzx', 'lea', 'call', 'jmp')
STORE_SRC = ('fstp', 'fst', 'fistp', 'fist', 'fnstsw', 'fnstcw')


class Desc:
    """What a register / stack slot holds: an object path plus a byte adjustment."""
    __slots__ = ('path', 'add')

    def __init__(self, path, add=0):
        self.path, self.add = path, add

    def key(self):
        return (self.path, self.add)


def scan(redir):
    fns = json.load(open(os.path.join(redir, 'functions.json')))
    thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(os.path.join(redir, 'thunks.json'))).items()}
    starts = sorted(int(f['addr'], 16) for f in fns)
    ends = {int(f['addr'], 16): int(f['end'], 16) for f in fns}

    def fn_of(a):
        i = bisect.bisect_right(starts, a) - 1
        return starts[i] if i >= 0 and a < ends[starts[i]] else None

    facts = {}
    cur = None
    st = None

    def new_state(f):
        return {'f': f, 'this_slot': False, 'reg': {}, 'slot': {}, 'acc': collections.Counter(),
                'sub': collections.Counter(), 'calls': collections.Counter(), 'vt': set(), 'new': [],
                'pend_new': None, 'n': 0}

    def flush(s):
        if s is None or not (s['this_slot'] or s['new']):
            return
        facts[s['f']] = {
            'thiscall': s['this_slot'],
            'acc': [[list(p), o, w, k, n] for (p, o, w, k), n in sorted(s['acc'].items())],
            'calls': [[list(p), a, t, n] for (p, a, t), n in sorted(s['calls'].items())],
            'vt': [[list(p), a, v] for (p, a, v) in sorted(s['vt'])],
            'new': s['new'],
        }

    for line in open(os.path.join(redir, 'joan.asm')):
        m = INS.match(line)
        if not m:
            continue
        a = int(m.group(1), 16)
        mn, ops = m.group(2), m.group(3).split('  #')[0].strip()
        f = fn_of(a)
        if f != cur:
            flush(st)
            cur = f
            st = new_state(f) if f is not None else None
        if st is None:
            continue
        st['n'] += 1
        reg, slot = st['reg'], st['slot']
        if mn == 'mov' and ops == 'DWORD PTR [ebp-0x4],ecx' and st['n'] < 16:
            st['this_slot'] = True
            slot[-4] = Desc(())
            continue
        parts = [p.strip() for p in split_ops(ops)]
        dst = parts[0] if parts else ''
        src = parts[1] if len(parts) > 1 else ''

        def mem_desc(op):
            """(Desc of the base object, offset, width, index?) for a memory operand on a tracked register."""
            mm = MEM.search(op)
            if not mm:
                return None
            size, base, idx, scale, sign, off = mm.groups()
            if base == 'ebp':
                return ('stack', -int(off, 16) if sign == '-' else int(off or '0', 16), WIDTH.get(size, 0))
            d = reg.get(base)
            if d is None:
                return None
            o = int(off, 16) if off else 0
            o = -o if sign == '-' else o
            return (d, d.add + o, WIDTH.get(size, 0), bool(idx))

        def record(md, kind):
            d, o, w, indexed = md
            k = kind + ('[]' if indexed else '')
            st['acc'][(d.path, o, w, k)] += 1

        # ---- operator new / constructor pairing
        if mn == 'push' and re.fullmatch(r'0x[0-9a-f]+', ops):
            st['last_push'] = int(ops, 16)
        if mn == 'call':
            t = target(ops, thunks)
            if t == OP_NEW and st.get('last_push') is not None:
                st['pend_new'] = st['last_push']
            this_d = reg.get('ecx')
            if this_d is not None and t is not None:
                st['calls'][(this_d.path, this_d.add, t)] += 1
            elif t is not None and st['pend_new'] is not None and reg.get('ecx_new'):
                st['new'].append([st['pend_new'], t])
                st['pend_new'] = None
            for r in ('eax', 'ecx', 'edx', 'ecx_new'):
                reg.pop(r, None)
            st['last_push'] = None
            continue
        # after `call operator new`, eax holds the new object; `mov ecx,eax` (directly or via a stack slot) + call ctor
        if st['pend_new'] is not None and mn == 'mov':
            if dst == 'ecx' and src == 'eax':
                reg['ecx_new'] = True
            elif src == 'eax' and dst.startswith('DWORD PTR [ebp-'):
                st.setdefault('new_slots', set()).add(dst)
            elif dst == 'ecx' and src in st.get('new_slots', ()):
                reg['ecx_new'] = True

        # ---- memory operands
        for i, op in enumerate(parts):
            if '[' not in op:
                continue
            md = mem_desc(op)
            if md is None or md[0] == 'stack':
                continue
            if mn == 'lea':
                continue
            is_dst = (i == 0) and mn not in NOWRITE
            if mn in STORE_SRC:
                is_dst = True
            record(md, 'w' if is_dst else 'r')
            if i == 0 and mn == 'mov' and re.fullmatch(r'0x[0-9a-f]+', src) and md[1] == md[0].add and \
                    RDATA[0] <= int(src, 16) < RDATA[1]:
                st['vt'].add((md[0].path, md[0].add, int(src, 16)))

        # ---- register / slot tracking
        if mn == 'mov' and dst in REGS32:
            md = mem_desc(src) if '[' in src else None
            if md is not None and md[0] == 'stack':
                d = slot.get(md[1])
                reg[dst] = Desc(d.path, d.add) if d is not None and md[2] == 4 else None
            elif md is not None:
                d, o, w, indexed = md
                # a dword loaded from a tracked object: maybe a pointer; it becomes the object at that field
                reg[dst] = Desc(d.path + (o,)) if (w == 4 and not indexed and len(d.path) < 4) else None
            elif src in REGS32:
                d = reg.get(src)
                reg[dst] = Desc(d.path, d.add) if d is not None else None
            else:
                reg[dst] = None
            if reg.get(dst) is None:
                reg.pop(dst, None)
            continue
        if mn == 'mov' and '[' in dst:
            md = mem_desc(dst)
            if md is not None and md[0] == 'stack' and src in REGS32:
                d = reg.get(src)
                if d is not None:
                    slot[md[1]] = Desc(d.path, d.add)
                else:
                    slot.pop(md[1], None)
            continue
        if mn == 'lea' and dst in REGS32:
            md = mem_desc(src)
            if md is not None and md[0] != 'stack' and not md[3]:
                reg[dst] = Desc(md[0].path, md[1])
            else:
                reg.pop(dst, None)
            continue
        if mn in ('add', 'sub') and dst in REGS32 and re.fullmatch(r'0x[0-9a-f]+', src):
            d = reg.get(dst)
            if d is not None:
                k = int(src, 16)
                reg[dst] = Desc(d.path, d.add + (k if mn == 'add' else -k))
            continue
        # anything else that writes a register forgets it
        if dst in REGS32 or dst in SUB:
            r = SUB.get(dst, dst)
            if mn not in ('cmp', 'test', 'push'):
                reg.pop(r, None)
        if mn in ('cdq', 'imul', 'mul', 'div', 'idiv', 'movs', 'rep', 'stos', 'lods', 'xchg', 'pop'):
            for r in ('eax', 'edx', 'ecx', 'esi', 'edi'):
                if mn != 'pop' or r == dst:
                    reg.pop(r, None)
    flush(st)
    return facts


def split_ops(ops):
    out, depth, cur = [], 0, ''
    for ch in ops:
        if ch == '[':
            depth += 1
        elif ch == ']':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    if cur:
        out.append(cur)
    return out


def target(ops, thunks):
    m = re.fullmatch(r'0x([0-9a-f]+)', ops.strip())
    if not m:
        return None
    t = int(m.group(1), 16)
    return thunks.get(t, t)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--re', default=os.path.join(os.path.dirname(ROOT), 're'))
    ap.add_argument('-o', default=os.path.join(HERE, 'out', 'asm_facts.json'))
    a = ap.parse_args()
    facts = scan(a.re)
    os.makedirs(os.path.dirname(a.o), exist_ok=True)
    json.dump({f'{k:#010x}': v for k, v in sorted(facts.items())}, open(a.o, 'w'))
    n_acc = sum(len(v['acc']) for v in facts.values())
    print(f'{len(facts)} thiscall functions, {n_acc} distinct field accesses, '
          f'{sum(len(v["vt"]) for v in facts.values())} vtable stores, {sum(len(v["new"]) for v in facts.values())} new+ctor pairs')


if __name__ == '__main__':
    main()
