#!/usr/bin/env python3
"""x64 hazard meter (docs/X64.md): counts the 32-bit assumptions the compiler does not report, per
top-level directory of src/ (a rough subsystem split). Every route-A round must only lower these numbers.

    python3 re/x64_hazards.py [--by-file] [--baseline re/x64_census/hazards_baseline.txt]
"""
import argparse
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(HERE, '..', 'src')

# name -> regex (counted per match). Order = report column order.
HAZARDS = [
    ('raw_ptr', r'\b(?:at|f)<[^<>;()]*\*\s*>\('),            # at<T*>(p, off) / f<T*>(off): 8 bytes at an x86 offset
    ('raw_u32', r'\b(?:at|f)<(?:u32|std::uint32_t)>\('),      # may hold a pointer
    ('raw_any', r'\b(?:at|f)<[^<>;()]+>\('),                 # every raw field access (the A2/A3 target)
    ('vtbl4', r'void\*\*\*|(?:offset|off) / 4\]'),           # vtable reads assuming 4-byte entries
    ('slot_call', r'\b(?:thiscall|cdecl_call|stdcall_call|com_call|virtual_call|vcall|win_call)<'),
    ('orig_fwd', r'\borig::(?!arg::)[a-z_0-9:]+\('),          # facade forwards through slots (not the arg:: conversion helpers)
    ('img_use', r'\bd::[A-Za-z_][A-Za-z_0-9]*'),             # data-image symbols
    ('ptr_u32', r'static_cast<(?:u32|std::uint32_t)>\(reinterpret_cast<(?:std::)?uintptr_t>'),
    ('naked', r'__declspec\(naked\)'),
    ('asm', r'\b__asm\b'),
    ('x87emu', r'\bx87::(?:Fpu|ftol)\b'),
    # a field reached as `pointer + constant` (an x86 offset that moves on x64 when a pointer precedes the field):
    # cast or wrapped `p + N`, `add(this, N)`, `self() + N`. Also counts array arithmetic on byte buffers (records,
    # strings): a tracker that must only go down, not a list of bugs (tools/x64_raw_offsets.py lists the sites).
    ('raw_add', r'(?:(?:static_cast|reinterpret_cast)<[^<>;]*>|\b(?:ptr|mptr|str))\(\s*[a-z_][a-z_0-9]*\s*\+\s*(?:0x[0-9a-fA-F]+|[1-9][0-9]*)\s*\)'
                r'|\b(?:add|self)\(\s*this\s*,\s*(?:0x[0-9a-fA-F]+|[1-9][0-9]*)|\bself\(\)\s*\+\s*0x'),
    # a pointer-sized value cut to 32 bits on its way into a call or a 32-bit integer
    ('narrow64', r'\barg::(?:u32|i32)\(\s*(?:fn::code_value|reinterpret_cast<(?:std::)?u?intptr_t>)'
                 r'|static_cast<(?:i32|std::int32_t)>\(reinterpret_cast<(?:std::)?u?intptr_t>\([^()]*(?:\([^()]*\)[^()]*)*\)\)'),  # not a difference
]
RX = [(n, re.compile(p)) for n, p in HAZARDS]
EXT = ('.cpp', '.hpp', '.inc', '.h')
SKIP_DIRS = {'data'}  # generated image tables (counted as img_use where they are used, not where they are defined)


def scan():
    per_dir = collections.defaultdict(collections.Counter)
    per_file = collections.defaultdict(collections.Counter)
    for root, dirs, files in os.walk(SRC):
        rel_root = os.path.relpath(root, SRC)
        top = rel_root.split(os.sep)[0] if rel_root != '.' else '(src)'
        if top in SKIP_DIRS:
            dirs[:] = []
            continue
        for fn in files:
            if not fn.endswith(EXT):
                continue
            path = os.path.join(root, fn)
            text = open(path, encoding='utf-8', errors='replace').read()
            rel = os.path.relpath(path, SRC)
            for name, rx in RX:
                n = len(rx.findall(text))
                if n:
                    per_dir[top][name] += n
                    per_file[rel][name] += n
    return per_dir, per_file


def table(rows, title):
    names = [n for n, _ in HAZARDS]
    out = [f'{title:<34}' + ''.join(f'{n:>10}' for n in names)]
    total = collections.Counter()
    for key in sorted(rows, key=lambda k: -rows[k]['raw_any']):
        c = rows[key]
        total.update(c)
        out.append(f'{key:<34}' + ''.join(f'{c[n]:>10}' for n in names))
    out.append(f'{"TOTAL":<34}' + ''.join(f'{total[n]:>10}' for n in names))
    return out, total


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--by-file', action='store_true')
    ap.add_argument('--baseline', help='compare the totals with a previous output of this script')
    a = ap.parse_args()
    per_dir, per_file = scan()
    lines, total = table(per_file if a.by_file else per_dir, 'file' if a.by_file else 'directory')
    print('\n'.join(lines))
    if a.baseline:
        base = None
        for line in open(a.baseline):
            if line.startswith('TOTAL'):
                base = [int(x) for x in line.split()[1:]]
        if base is None:
            sys.exit('no TOTAL line in the baseline')
        names = [n for n, _ in HAZARDS]
        print('\nchange against the baseline:')
        print(''.join(f'{n:>10}' for n in names))
        print(''.join(f'{total[n] - b:>+10}' for n, b in zip(names, base)))


if __name__ == '__main__':
    main()
