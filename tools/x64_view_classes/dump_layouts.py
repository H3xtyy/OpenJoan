# SPDX-License-Identifier: GPL-3.0-or-later
# gdb -batch -x dump_layouts.py OBJECT  (env CLASSES, OUT): the flattened leaf fields of joan::orig classes as JSON
# {Class: {size, fields: [[offset, size, path, kind]]}}; kind P pointer, F float, I other, suffix aN for arrays of N-byte elements.
import gdb, json, os
names = [l.strip() for l in open(os.environ["CLASSES"]) if l.strip()]
out = {}
def leaves(t, base, prefix, acc, depth=0):
    t = t.strip_typedefs()
    if depth > 6: return
    if t.code == gdb.TYPE_CODE_STRUCT or t.code == gdb.TYPE_CODE_UNION:
        for f in t.fields():
            if f.is_base_class:
                leaves(f.type, base + f.bitpos // 8, prefix, acc, depth + 1); continue
            if not hasattr(f, "bitpos") or f.bitpos is None: continue  # static
            leaves(f.type, base + f.bitpos // 8, prefix + "." + (f.name or "?"), acc, depth + 1)
        return
    if t.code == gdb.TYPE_CODE_ARRAY:
        el = t.target().strip_typedefs()
        n = t.sizeof // el.sizeof if el.sizeof else 0
        if el.code in (gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION) and n <= 64:
            for i in range(n): leaves(el, base + i * el.sizeof, prefix + "[%d]" % i, acc, depth + 1)
        else:
            kind = "P" if el.code == gdb.TYPE_CODE_PTR else ("F" if el.code == gdb.TYPE_CODE_FLT else "I")
            acc.append([base, t.sizeof, prefix, kind + "a%d" % el.sizeof])
        return
    kind = "P" if t.code == gdb.TYPE_CODE_PTR else ("F" if t.code == gdb.TYPE_CODE_FLT else "I")
    acc.append([base, t.sizeof, prefix, kind])
for n in names:
    try:
        t = gdb.lookup_type("joan::orig::" + n)
    except Exception:
        continue
    if t.strip_typedefs().code != gdb.TYPE_CODE_STRUCT: continue
    acc = []
    try:
        leaves(t, 0, "", acc)
    except Exception as e:
        continue
    out[n] = {"size": t.sizeof, "fields": acc}
json.dump(out, open(os.environ["OUT"], "w"))
print("classes", len(out))
