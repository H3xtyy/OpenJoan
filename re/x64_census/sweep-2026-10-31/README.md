# x64 sweep, 2026-10-31

Triggered by the owner's x64 mission switches (crashes entering missions 4, 5 and 7, all one bug class: an object read
through a layout that matches it at x86 but not at x64). Scans, then six `x64-auditor` agents over disjoint file groups, then review.

## Scanners (what each one sees)

1. `dump.py` (gdb Python): the flattened layout of every class of `re/layout/layouts.json` from the DWARF of a build:
   `gdb -batch -ex "python NAMES='names.txt'; OUT='x86.json'" -x dump.py build/linux-x86/src/openjoan` (and x64).
   Exact offsets of both builds, including bases and embedded objects.
2. `views.py`: size-less layouts (views) used in hand code whose named members coincide (x86 offset, size, pointer
   kind) with >= 60% of another class's, and whose shared members sit elsewhere on x64. Finds RecruitFacility-like
   views. Does not see a sized class applied to the wrong object (Anim on TrackNode) unless it is size-less.
3. `bss.py`: `as_X(obj(d::sym))` (and `k = obj(d::sym); as_X(k)`) where `src/data/bss.inc` types `sym` as another
   class R whose members drift against X's on x64. Sees only image objects, not heap objects.
4. Greps (in the sweep's candidate list): literal sizes in memset / memcpy / new / ctor helpers; raw hex and decimal
   byte offsets on object pointers (`tools/x64_scan_raw_index.py` plus `node + 8`-style decimal offsets).

532 candidate lines in 130 files; ~520 harmless (pointer-free records, file / string buffers, generic containers at
+0, facades whose replacement reads the real class).
