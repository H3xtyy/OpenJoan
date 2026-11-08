#!/usr/bin/env python3
"""x64 meter for the data image (docs/X64.md image): `global<T>(d::X [+ k])` accesses whose bytes touch a stored
pointer of the image (a pointer member / uptr of src/data/layout.hpp, which is 8 bytes on x64) with a type T that is
not pointer-sized - on x64 they read or write half a pointer. Since the image layout is per architecture and
img::Addr maps original addresses (src/runtime/image.hpp), `d::X + k` itself is right on x64; only the access width matters.

Counts per directory: ok (no pointer touched, or T is a pointer / uintptr_t covering exactly the slot), hazard
(a 1/2/4/8-byte non-pointer access over a slot), dynamic (offset not a literal: not decided here).
    re/image/img_width.py [-v]
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
SRC = os.path.join(ROOT, 'src')
sys.path.insert(0, HERE)
import probe  # noqa: E402

KVA = 0x9cf000
SIZES = {'i8': 1, 'u8': 1, 'char': 1, 'bool': 1, 'std::int8_t': 1, 'std::uint8_t': 1, 'const char': 1,
         'i16': 2, 'u16': 2, 'std::int16_t': 2, 'std::uint16_t': 2, 'short': 2,
         'i32': 4, 'u32': 4, 'std::int32_t': 4, 'std::uint32_t': 4, 'int': 4, 'unsigned': 4, 'float': 4, 'f32': 4,
         'double': 8, 'f64': 8, 'u64': 8, 'std::uint64_t': 8, 'std::int64_t': 8}
PTR_TYPES = {'std::uintptr_t', 'uptr', 'uintptr_t', 'HWND', 'HANDLE', 'HINSTANCE', 'HMODULE', 'HDC', 'HBITMAP', 'HFONT',
             'HICON', 'HCURSOR', 'HMENU', 'HGLOBAL', 'LPVOID', 'PVOID', 'FARPROC'}  # pointer-sized
RX = re.compile(r'global<([^<>]+(?:<[^<>]*>)?)>\(d::(\w+)(?:\s*\+\s*([^()]+?))?\)')
# at<T>(obj(d::X), k): a raw byte offset from the object's CURRENT address - not mapped on x64: always a hazard when
# it crosses a pointer slot, and a hazard of its own kind (write global<T>(d::X + k) instead, which is mapped)
RX_AT = re.compile(r'at<([^<>]+(?:<[^<>]*>)?)>\((?:img::)?obj\(d::(\w+)\),\s*([^()]+?)\)')


def main():
    verbose = '-v' in sys.argv
    names = {}
    for line in open(os.path.join(SRC, 'data_names.hpp')):
        m = re.search(r'Sym<.*?> (\w+)\{(0x[0-9a-f]+)u\}', line)
        if m:
            names[m.group(1)] = int(m.group(2), 16)
    _, leaves = probe.walk('#include "%s"\n' % os.path.join(SRC, 'data', 'layout.hpp'), 'joan::data::layout::Image',
                           'x86')
    slot = set()
    starts = set()
    for o, _ in leaves:
        starts.add(KVA + o)
        slot.update(range(KVA + o, KVA + o + 4))
    stats = collections.defaultdict(collections.Counter)
    rows = []
    for dp, _, fs in os.walk(SRC):
        if os.path.relpath(dp, SRC).startswith('data'):
            continue
        for f in fs:
            if not f.endswith(('.cpp', '.hpp', '.inc')):
                continue
            p = os.path.join(dp, f)
            top = os.path.relpath(p, SRC).split(os.sep)[0] if os.sep in os.path.relpath(p, SRC) else '(src)'
            for n, line in enumerate(open(p, encoding='utf-8', errors='replace'), 1):
                for m in RX_AT.finditer(line):
                    stats[top]['at(obj)'] += 1
                    rows.append(f'{os.path.relpath(p, ROOT)}:{n}: at<{m.group(1)}>(obj(d::{m.group(2)}), {m.group(3)})  [raw offset]')
                for m in RX.finditer(line):
                    t, name, off = m.group(1).strip(), m.group(2), m.group(3)
                    if name not in names:
                        continue
                    k = 0
                    if off:
                        try:
                            k = int(off.strip().rstrip('u'), 0)
                        except ValueError:
                            stats[top]['dynamic'] += 1
                            if any(a in slot for a in range(names[name], names[name] + 0x400)):
                                rows.append(f'{os.path.relpath(p, ROOT)}:{n}: global<{t}>(d::{name} + {off})  [dynamic, pointers nearby]')
                            continue
                    va = names[name] + k
                    ptr = t.endswith('*') or t in PTR_TYPES
                    size = 4 if ptr else SIZES.get(t.replace('const ', ''), 0) or SIZES.get(t, 0)
                    touched = any(a in slot for a in range(va, va + max(size, 1)))
                    if not touched:
                        stats[top]['ok'] += 1
                    elif ptr and va in starts:
                        stats[top]['ok (pointer)'] += 1
                    else:
                        stats[top]['hazard'] += 1
                        rows.append(f'{os.path.relpath(p, ROOT)}:{n}: global<{t}>(d::{name}{" + " + off if off else ""})')
    cols = ['ok', 'ok (pointer)', 'hazard', 'dynamic', 'at(obj)']
    print(f"{'dir':<12}" + ''.join(f'{c:>14}' for c in cols))
    tot = collections.Counter()
    for d in sorted(stats, key=lambda d: -stats[d]['hazard']):
        print(f'{d:<12}' + ''.join(f'{stats[d][c]:>14}' for c in cols))
        tot.update(stats[d])
    print(f"{'TOTAL':<12}" + ''.join(f'{tot[c]:>14}' for c in cols))
    if verbose:
        print('\n'.join(rows))


if __name__ == '__main__':
    main()
