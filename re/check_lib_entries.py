#!/usr/bin/env python3
"""Check that every library address (0x763d00..0x7d5000) registered in src/** can take a 5-byte entry jmp.

Usage: check_lib_entries.py <joan.asm (objdump -d of joan.exe)> <functions.json> <hook dir>
(any directory works: every *.cpp below it is scanned, so src/lib alone checks only the library modules)
Unsafe = the next function starts less than 5 bytes later, or code outside the function jumps / calls into
entry+1..entry+4. src/runtime/patch.cpp patch_entry relies on this check for library-range entries.
"""
import bisect, glob, json, re, sys

asm, fjson, mods = sys.argv[1:4]
targets, starts = {}, {int(f['addr'], 16) for f in json.load(open(fjson))}
rx = re.compile(r'^\s*([0-9a-f]+):\s.*?\t(j\w+|call|loop\w*)\s+0x([0-9a-f]+)')
for line in open(asm):
    m = rx.match(line)
    if m:
        src, dst = int(m.group(1), 16), int(m.group(3), 16)
        targets.setdefault(dst, []).append(src)
        if m.group(2) == 'call':
            starts.add(dst)
regs = set()
for f in glob.glob(mods + '/**/*.cpp', recursive=True):
    for line in open(f, errors='replace'):
        s = line.strip()
        if s.startswith(('//', '#')):
            continue
        m = re.match(r'[A-Z_0-9]+\(\s*"[^"]*"\s*,\s*(?:"[^"]*"\s*,\s*)?0x([0-9a-fA-F]+)', s)
        if m and 0x763d00 <= int(m.group(1), 16) < 0x7d5000:
            regs.add(int(m.group(1), 16))
order, bad = sorted(starts), []
for a in sorted(regs):
    i = bisect.bisect_right(order, a)
    nxt = order[i] if i < len(order) else 1 << 32
    ext = [(t, s) for t in range(a + 1, a + 5) for s in targets.get(t, []) if not a <= s < nxt]
    if nxt - a < 5 or ext:
        bad.append((hex(a), nxt - a, [(hex(t), hex(s)) for t, s in ext]))
print(f'{len(regs)} library entries registered, {len(bad)} unsafe')
for b in bad:
    print('UNSAFE', *b)
sys.exit(1 if bad else 0)
