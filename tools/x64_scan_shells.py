#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): x86-sized byte shells used as objects of typed classes: `struct X { std::uint8_t b[0x1a0]; }` passed to an `as_<class>()`
accessor whose class is larger on x64 (sizes from the DWARF of the two builds given as arguments). Usage:
  python3 tools/x64_scan_shells.py <x64 binary> <x86 binary>   (-v lists every use)
Found the FileTxt / Material shells of 2026-10-30 (7100c48).

A scan is advisory: every hit is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).
"""
import re, subprocess, sys, pathlib, collections
root = pathlib.Path('src')
acc = {}
for line in (root/'original'/'direct.hpp').read_text().splitlines():
    m = re.search(r'inline (\w+)\* (as_\w+)\(P p\)', line)
    if m: acc[m.group(2)] = m.group(1)
shell_re = re.compile(r'struct\s+(?:alignas\(\d+\)\s+)?(\w+)\s*\{[^{}]*?\b(?:std::uint8_t|u8|char|unsigned char|std::byte)\s+\w+\[(0x[0-9a-fA-F]+|\d+)\];\s*(?://[^\n]*)?\n\s*\};', re.S)
arr_re = re.compile(r'(?:alignas\(\d+\)\s+)?(?:std::uint8_t|u8|char|unsigned char|std::byte)\s+(\w+)\[(0x[0-9a-fA-F]+|\d+)\];')
findings = []
classes = set()
for f in sorted(root.rglob('*.cpp')) + sorted(root.rglob('*.hpp')):
    s = str(f)
    if '/original/gen/' in s or '/data/' in s: continue
    text = f.read_text(errors='replace')
    shells = {}
    for m in shell_re.finditer(text):
        shells.setdefault(m.group(1), []).append(int(m.group(2), 0))
    # locals of shell types: Name var; / Name var{...};
    for name, sizes in shells.items():
        for m in re.finditer(r'\b' + name + r'\s+(\w+)\s*(?:;|\{)', text):
            var = m.group(1)
            if var in ('const',): continue
            for u in re.finditer(r'\b(as_\w+)\((?:static_cast<(?:const )?void\*>\()?&' + var + r'\b\)?\)', text):
                cls = acc.get(u.group(1))
                if cls:
                    classes.add(cls); findings.append((s, text[:u.start()].count('\n')+1, name, min(sizes), var, cls))
    # bare local arrays
    for m in arr_re.finditer(text):
        var, size = m.group(1), int(m.group(2), 0)
        for u in re.finditer(r'\b(as_\w+)\((?:static_cast<(?:const )?void\*>\()?&?' + var + r'\b\)?\)', text):
            cls = acc.get(u.group(1))
            if cls:
                classes.add(cls); findings.append((s, text[:u.start()].count('\n')+1, var+'[]', size, var, cls))
def sizes(binary):
    args = ['gdb', '-batch', '-q', '-ex', 'set debuginfod enabled off']
    cl = sorted(classes)
    for c in cl: args += ['-ex', f'p sizeof(joan::orig::{c})']
    args.append(binary)
    out = subprocess.run(args, capture_output=True, text=True).stdout
    res = {}
    i = 0
    for line in out.splitlines():
        m = re.match(r'\$(\d+) = (\d+)', line)
        if m: res[cl[int(m.group(1))-1]] = int(m.group(2))
    return res
x64 = sizes(sys.argv[1]); x86 = sizes(sys.argv[2])
seen = set()
for s, ln, shell, size, var, cls in sorted(findings):
    key = (s, shell, var, cls)
    if key in seen: continue
    seen.add(key)
    s64, s86 = x64.get(cls), x86.get(cls)
    flag = 'OVERRUN' if s64 is not None and s64 > size else ('?' if s64 is None else 'ok')
    if flag != 'ok' or '-v' in sys.argv:
        print(f'{flag:7} {s}:{ln} {shell} {size:#x} <- {cls} x64 {s64} x86 {s86} ({var})')
print(f'{len(findings)} uses, {len(classes)} classes', file=sys.stderr)
