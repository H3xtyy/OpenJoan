#!/usr/bin/env python3
"""Data-image typing, step 1 (docs/X64.md image): what the original binary does with every object of the
data image (src/data/data_symbols.inc).

Scans joan.asm (MSVC 6 /Od, linear register tracking as in re/layout/asm_scan.py) for absolute operands inside the
image (0x9cf000..0xa70000) and records per symbol (+ offset inside it):
  * direct accesses: width, read / write, indexed (array);
  * `mov reg, [g]` followed by `[reg+off]` accesses -> the symbol holds a POINTER; what is read through it;
  * `mov ecx, [g]; call f` -> the pointee is the `this` of f (class label from re/layout/layouts.json);
  * `mov ecx, g` / `lea ecx, [g+k]; call f` -> the symbol (at +k) IS an object of f's class;
  * `push g` / `push [g]` before a call -> passed as an argument (address / value).

    re/image/gscan.py [--re ../re] [-o re/image/out/gfacts.json]
"""
import argparse
import bisect
import collections
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
IMG = (0x9cf000, 0xa70000)
INS = re.compile(r'^\s+([0-9a-f]+):\t(\S+)\s*(.*)$')
ABS = re.compile(r'(?:(BYTE|WORD|DWORD|QWORD|TBYTE) PTR )?ds:(0x[0-9a-f]+)|(?:(BYTE|WORD|DWORD|QWORD|TBYTE) PTR )?\[(?:([a-z]{3})\*(\d)\+)?(0x[0-9a-f]{6,})\]|(?:(BYTE|WORD|DWORD|QWORD|TBYTE) PTR )?\[([a-z]{3})(?:\*(\d))?\+(0x[0-9a-f]{6,})\]')
REGM = re.compile(r'(?:(BYTE|WORD|DWORD|QWORD|TBYTE) PTR )?\[([a-z]{3})(?:\+([a-z]{3})\*(\d))?(?:([+-])0x([0-9a-f]+))?\]')
WIDTH = {'BYTE': 1, 'WORD': 2, 'DWORD': 4, 'QWORD': 8, 'TBYTE': 10}
REGS = ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi')
SIZES = {'u8': 1, 'i8': 1, 'char': 1, 'u16': 2, 'i16': 2, 'u32': 4, 'i32': 4, 'float': 4, 'double': 8, 'Guid': 16, 'uptr': 4}


def load_symbols():
    syms = []
    for line in open(os.path.join(ROOT, 'src', 'data', 'data_symbols.inc')):
        m = re.match(r'JOAN_DATA_SYMBOL\((\w+), ([^,]+), (\w+), (0x[0-9a-f]+)u\)', line)
        if not m:
            continue
        sec, typ, name, addr = m.groups()
        a = re.match(r'(\w+)(?:\[(\d+)\])?', typ)
        size = SIZES.get(a.group(1), 0) * (int(a.group(2)) if a.group(2) else 1)
        syms.append((int(addr, 16), size, name, typ, sec))
    syms.sort()
    return syms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--re', default=os.path.join(os.path.dirname(ROOT), 're'))
    ap.add_argument('-o', default=os.path.join(HERE, 'out', 'gfacts.json'))
    a = ap.parse_args()
    syms = load_symbols()
    starts = [s[0] for s in syms]

    def sym_of(addr):
        i = bisect.bisect_right(starts, addr) - 1
        if i < 0:
            return None, 0
        s = syms[i]
        nxt = syms[i + 1][0] if i + 1 < len(syms) else IMG[1]
        end = s[0] + s[1] if s[1] else nxt
        if addr < max(end, s[0] + 1) or (s[1] == 0 and addr < nxt):
            return s[2], addr - s[0]
        return None, 0

    thunks = {int(k, 16): int(v, 16) for k, v in json.load(open(os.path.join(a.re, 'thunks.json'))).items()}
    layouts = json.load(open(os.path.join(ROOT, 're', 'layout', 'layouts.json')))
    fn_class = layouts['functions']
    facts = collections.defaultdict(lambda: {'direct': collections.Counter(), 'deref': collections.Counter(),
                                             'this_of': collections.Counter(), 'ptr_this_of': collections.Counter(),
                                             'pushed_addr': 0, 'pushed_val': 0, 'unknown_addr': 0})
    slot = {}
    reg = {}          # reg -> ('ptr', sym, off) a pointer loaded from sym+off | ('addr', sym, off) address of sym+off
    last_push = []
    for line in open(os.path.join(a.re, 'joan.asm')):
        m = INS.match(line)
        if not m:
            continue
        mn, ops = m.group(2), m.group(3).split('  #')[0].strip()
        parts = split_ops(ops)
        dst = parts[0] if parts else ''
        src = parts[1] if len(parts) > 1 else ''
        if mn in ('ret', 'jmp') or mn.startswith('j'):
            if mn in ('ret', 'jmp'):
                reg.clear()
                if mn == 'ret':
                    slot.clear()
        set_now = False
        sm = re.fullmatch(r'DWORD PTR \[ebp-(0x[0-9a-f]+)\]', dst)
        if mn == 'mov' and sm and src in REGS:
            if src in reg:
                slot[sm.group(1)] = reg[src]
            else:
                slot.pop(sm.group(1), None)
            continue
        sm = re.fullmatch(r'DWORD PTR \[ebp-(0x[0-9a-f]+)\]', src)
        if mn == 'mov' and sm and dst in REGS:
            if sm.group(1) in slot:
                reg[dst] = slot[sm.group(1)]
            else:
                reg.pop(dst, None)
            continue
        # absolute operands
        for i, op in enumerate(parts):
            mm = re.search(r'(?:(BYTE|WORD|DWORD|QWORD|TBYTE) PTR )?(?:ds:|\[)(?:([a-z]{3})(?:\*(\d))?\+)?(0x[0-9a-f]{6,})\]?', op)
            if not mm:
                continue
            addr = int(mm.group(4), 16)
            if not IMG[0] <= addr < IMG[1]:
                continue
            name, off = sym_of(addr)
            if name is None:
                continue
            width = WIDTH.get(mm.group(1) or '', 4 if 'ds:' in op else 0)
            kind = ('w' if i == 0 and mn not in ('cmp', 'test', 'push', 'fld', 'fild', 'fcom', 'fcomp') else 'r') + ('[]' if mm.group(2) else '')
            if mn.startswith('fst') or mn.startswith('fist'):
                kind = 'w' + ('[]' if mm.group(2) else '')
            if mn == 'lea':
                kind = 'lea'
            facts[name]['direct'][f'{off:#x}/{width}/{kind}'] += 1
            if mn == 'mov' and i == 1 and dst in REGS and width == 4 and not mm.group(2):
                reg[dst] = ('ptr', name, off)
                set_now = True
                continue
            if mn == 'lea' and dst in REGS:
                reg[dst] = ('addr', name, off)
                set_now = True
                continue
            if mn == 'push':
                facts[name]['pushed_val'] += 1
        # immediate addresses
        if mn == 'mov' and dst in REGS and re.fullmatch(r'0x[0-9a-f]{6,}', src):
            v = int(src, 16)
            if IMG[0] <= v < IMG[1]:
                name, off = sym_of(v)
                if name:
                    reg[dst] = ('addr', name, off)
                    set_now = True
        if mn == 'push' and re.fullmatch(r'0x[0-9a-f]{6,}', ops):
            v = int(ops, 16)
            if IMG[0] <= v < IMG[1]:
                name, off = sym_of(v)
                if name:
                    facts[name]['pushed_addr'] += 1
        # accesses through registers holding a loaded pointer
        for i, op in enumerate(parts):
            if set_now:
                break
            rm = REGM.search(op)
            if not rm or rm.group(2) not in reg:
                continue
            k, name, off = reg[rm.group(2)]
            d = int(rm.group(6), 16) if rm.group(6) else 0
            d = -d if rm.group(5) == '-' else d
            if k == 'ptr' and mn != 'lea':
                kind = ('w' if i == 0 and mn not in ('cmp', 'test', 'push', 'fld', 'fild') else 'r') + ('[]' if rm.group(3) else '')
                facts[name]['deref'][f'{off:#x}->{d:#x}/{WIDTH.get(rm.group(1) or "", 0)}/{kind}'] += 1
        if mn == 'call':
            t = re.fullmatch(r'0x([0-9a-f]+)', ops)
            if t and 'ecx' in reg:
                tv = int(t.group(1), 16)
                tv = thunks.get(tv, tv)
                cls = fn_class.get(f'{tv:#x}') or '?'
                k, name, off = reg['ecx']
                facts[name]['ptr_this_of' if k == 'ptr' else 'this_of'][f'{off:#x}:{cls}'] += 1
            for r in ('eax', 'ecx', 'edx'):
                reg.pop(r, None)
            continue
        # anything else that writes a register forgets it
        if set_now:
            continue
        if dst in REGS and mn not in ('cmp', 'test', 'push'):
            if not (mn == 'mov' and src in REGS and src in reg):
                reg.pop(dst, None)
            else:
                reg[dst] = reg[src]
    out = {}
    for name, f in facts.items():
        out[name] = {k: (dict(v) if isinstance(v, collections.Counter) else v) for k, v in f.items()}
    os.makedirs(os.path.dirname(a.o), exist_ok=True)
    json.dump(out, open(a.o, 'w'), indent=0)
    ptr = sum(1 for f in out.values() if f['deref'] or f['ptr_this_of'])
    obj = sum(1 for f in out.values() if f['this_of'])
    print(f'{len(out)} symbols used; {ptr} hold pointers (dereferenced / used as this), {obj} are objects (address used as this)')


def split_ops(ops):
    out, depth, cur = [], 0, ''
    for ch in ops:
        if ch == '[':
            depth += 1
        elif ch == ']':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur:
        out.append(cur.strip())
    return out


if __name__ == '__main__':
    main()
