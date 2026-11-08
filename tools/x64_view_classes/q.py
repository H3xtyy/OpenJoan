#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""q.py OUT Class [field|0xOFF ...]: a class's fields with x86 and x64 offsets (layouts written by
tools/x64_scan_view_classes.py into OUT). No argument after the class: the whole flattened layout. A field name: its
leaves. 0xOFF: the leaf holding that x86 offset."""
import json, sys, os
D = sys.argv.pop(1)
a = json.load(open(os.path.join(D, "layout_x86.json"))); b = json.load(open(os.path.join(D, "layout_x64.json")))
c = sys.argv[1]
if c not in a: sys.exit("unknown class " + c)
A, B = a[c]["fields"], b[c]["fields"]
print("%s: size x86 %#x, x64 %#x" % (c, a[c]["size"], b[c]["size"]))
want = sys.argv[2:]
for x, y in zip(A, B):
    show = not want
    for w in want:
        if w.startswith("0x"):
            o = int(w, 16); show |= x[0] <= o < x[0] + max(x[1], 1)
        else:
            show |= x[2].lstrip(".").split(".")[0].split("[")[0] == w
    if show: print("  x86 +%#06x (%d)  x64 +%#06x (%d)  %s %s" % (x[0], x[1], y[0], y[1], x[2], x[3]))
