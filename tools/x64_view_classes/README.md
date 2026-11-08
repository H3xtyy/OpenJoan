# tools/x64_view_classes

Helpers of `tools/x64_scan_view_classes.py`, the advisory scan for one object read through two class layouts that agree
on x86 and differ on x64 (see `docs/X64.md` and `tools/README.md`). Run from the repository root; the driver writes its
output to `build/x64-view-scan/`.

| File | Does |
|---|---|
| `dump_layouts.py` | gdb script (`gdb -batch -x dump_layouts.py OBJECT`, env `CLASSES` and `OUT`): writes the flattened leaf fields of the `joan::orig` classes as JSON `{Class: {size, fields: [[offset, size, path, kind]]}}`; kind P pointer, F float, I other |
| `scan.py` | the scan itself: reads `layout_x86.json` and `layout_x64.json`, collects per object expression the classes whose fields are read through it in `src/`, and reports offsets that map differently in the two layouts |
| `q.py` | query: `q.py OUT Class [field\|0xOFF ...]` prints a class's fields with their x86 and x64 offsets |

Needs the `build/linux-x86` and `build/linux-x64` trees configured and gdb.
