#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Lists the fields reached as `byte pointer + constant` in the game code (x64 hazard: the constant is an x86 offset,
and the field moves on x64 when a pointer precedes it). The hazard meter's `raw_add` column counts the same form
more roughly. Each site needs a look: file records, strings, local buffers and pointer-free classes are fine; a class
with pointers before the offset is a bug (use the typed member; `tools/x64_view_classes/q.py` shows both offsets).

    python3 tools/x64_raw_offsets.py [--by-file]

A byte pointer is a variable declared `u8*` / `std::uint8_t*` / `char*` (const or not) in the 300 lines above; also
`add(this, N)` / `self(this, N)` / `self() + 0xN`. Offsets below 4 are skipped.
"""
import argparse
import collections
import glob
import re
import sys

ap = argparse.ArgumentParser()
ap.add_argument("--by-file", action="store_true")
args = ap.parse_args()
BYTEPTR = re.compile(r"^(const\s+)?(u8|std::uint8_t|char|unsigned char|std::int8_t|i8)\s*\*\s*(const\s*)?$")
out = []
dirs = ["src/game", "src/en3d", "src/en2d", "src/ilib", "src/elib", "src/interfac", "src/uti3d"]
for d in dirs:
    for f in glob.glob(d + "/**/*.cpp", recursive=True):
        src = open(f, errors="ignore").read()
        lines = src.split("\n")
        for i, l in enumerate(lines):
            code = l.split("//")[0]
            for m in re.finditer(r"\b([a-z_][a-z0-9_]*)\s*\+\s*(0x[0-9a-fA-F]+|[0-9]+)\b(?!\s*\*)", code):
                ident, off = m.group(1), int(m.group(2), 0)
                if off < 4: continue
                # declared type: nearest preceding declaration "TYPE ident" within the function (300 lines)
                t = None
                for j in range(i, max(-1, i - 300), -1):
                    mm = re.search(r"((?:const\s+)?[\w:]+\s*\*\s*(?:const\s*)?)\s*\b%s\b\s*(=|;|,|\))" % re.escape(ident), lines[j].split("//")[0])
                    if mm: t = mm.group(1).strip(); break
                if t is None or not BYTEPTR.match(t): continue
                out.append((f, i + 1, ident, hex(off), l.strip()[:150]))
            for m in re.finditer(r"\b(add|self)\s*\(\s*this\s*,\s*(0x[0-9a-fA-F]+|[0-9]+)|self\(\)\s*\+\s*(0x[0-9a-fA-F]+)", code):
                out.append((f, i + 1, "this", m.group(2) or m.group(3), l.strip()[:150]))
if args.by_file:
    for f, n in collections.Counter(o[0] for o in out).most_common(): print(n, f)
else:
    for o in out: print("%s:%d\t%s\t%s\t%s" % o)
print("%d sites" % len(out), file=sys.stderr)

