#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""View-class conflict scan (x64): one object read through two class layouts that agree on x86 and disagree on x64.

Inputs: layout_x86.json / layout_x64.json (flattened leaf fields of every joan::orig class, from both compilers).
For every function body in src/ (replacements and helpers alike) it collects, per object expression, the classes
whose FIELDS are read through it (`jo::self<jo::X>(E)->f`, `jo::as_x(E)->f`). `this` of a registered replacement also
gets the classes its callers read the object they pass through (`joan::orig::as_y(E)->m(...)` -> facade Y::m's address
-> the registered impl), and the bss typing of `obj(d::sym)` objects. Reported: a pair of classes on one object where
some offset read through either maps to different x64 offsets in the two layouts.
Also: views applied to an interior pointer (`jo::self<jo::X>(&...->f)`), checked against the owner's layout from f on.
Usage: scan.py OUT TREE (run through tools/x64_scan_view_classes.py).
"""
import json, re, pathlib, sys, collections

S = pathlib.Path(sys.argv[1])
ROOT = pathlib.Path(sys.argv[2])
L86 = json.load(open(S / "layout_x86.json"))
L64 = json.load(open(S / "layout_x64.json"))


def snake(n):
    s = re.sub(r"(?<=[a-z0-9])([A-Z])", r"_\1", n)
    s = re.sub(r"(?<=[A-Z])([A-Z][a-z])", r"_\1", s)
    return s.lower()


acc = {}
for m in re.finditer(r"inline (\w+)\* (as_\w+)\(P p\)", (ROOT / "src/original/gen/accessors.hpp").read_text()):
    acc[m.group(2)] = m.group(1)
for c in L86:
    acc.setdefault("as_" + snake(c), c)

# field -> (x86 offset, leaf index) of the first leaf of top-level member f
def member(cls, f):
    if cls not in L86 or cls not in L64: return None
    for i, (off, size, path, kind) in enumerate(L86[cls]["fields"]):
        p = path.split(".")[1] if path.startswith(".") else path
        p = re.sub(r"\[.*", "", p)
        if p == f: return off
    return None


def mapper(cls):
    a, b = L86[cls]["fields"], L64[cls]["fields"]
    if len(a) != len(b): return None
    pairs = [(x[0], x[1], y[0], x[2], x[3]) for x, y in zip(a, b)]
    def m(o):
        for (o86, sz, o64, path, kind) in pairs:
            if o86 <= o < o86 + max(sz, 1):
                if kind.startswith("P") and o != o86 and not kind.startswith("Pa"): return ("inptr", path)
                return (o64 + (o - o86), path)
        return None
    return m


MAPS = {}
def mp(c):
    if c not in MAPS: MAPS[c] = mapper(c) if c in L86 and c in L64 else None
    return MAPS[c]


def conflict(c1, offs1, c2, offs2):
    m1, m2 = mp(c1), mp(c2)
    if not m1 or not m2: return []
    out = []
    for o in sorted(set(offs1) | set(offs2)):
        a, b = m1(o), m2(o)
        if a is None or b is None: continue
        if a[0] != b[0]:
            out.append((o, a, b))
    return out


# ---- facade (class, method) -> addresses; registrations address -> (file, impl name)
facade = collections.defaultdict(set)
for f in (ROOT / "src/original/gen").glob("*_methods.inc"):
    t = f.read_text()
    for m in re.finditer(r"/// (0x[0-9a-f]{8}) (\w+)::(\w+)\n[^\n]*?\b(\w+)\(", t):
        facade[(m.group(2), m.group(4))].add(int(m.group(1), 16))
# the facade class is the struct of the header the .inc belongs to
cls_of_inc = {}
for h in (ROOT / "src/original/gen").glob("*.hpp"):
    m = re.search(r"^struct (\w+)\s*(?::[^;{]*)?\{", h.read_text(), re.M)
    if m: cls_of_inc[h.stem] = m.group(1)
facade2 = collections.defaultdict(set)
for f in (ROOT / "src/original/gen").glob("*_methods.inc"):
    cls = cls_of_inc.get(f.name[: -len("_methods.inc")])
    if not cls: continue
    t = f.read_text()
    for m in re.finditer(r"/// (0x[0-9a-f]{8}) [^\n]*\n[^\n]*?\b(\w+)\(", t):
        facade2[(cls, m.group(2))].add(int(m.group(1), 16))

reg_re = re.compile(r'"[^"]+",\s*(0x[0-9a-fA-F]+)u?,\s*&(?:H|[\w:]+)::(\w+)\)')
bss = {}
for m in re.finditer(r"::joan::orig::(\w+) (\w+);\s*// (0x[0-9a-f]+) obj:(\w+)", (ROOT / "src/data/bss.inc").read_text()):
    bss[int(m.group(3), 16)] = m.group(4)
dsym = {}
for m in re.finditer(r"inline constexpr img::Sym<[^>]*> (\w+)\{(0x[0-9a-f]+)u\}", (ROOT / "src/data_names.hpp").read_text()):
    dsym[m.group(1)] = int(m.group(2), 16)

VIEW = re.compile(r"jo::self<jo::(\w+)>\(|jo::(as_\w+)\(")
FAC = re.compile(r"joan::orig::(as_\w+)\(")


def args_at(t, i):
    """t[i] is just after '(' : returns (expr, index after ')')."""
    d = 1; j = i
    while j < len(t) and d:
        if t[j] == "(": d += 1
        elif t[j] == ")": d -= 1
        j += 1
    return t[i: j - 1].strip(), j


CONSTS = {}
def norm(e, consts=None):
    e = re.sub(r"\s+", "", e)
    if consts and e in consts: e = consts[e]
    e = re.sub(r"^(?:static_cast|reinterpret_cast)<[^>]*>\((.*)\)$", r"\1", e)
    e = re.sub(r"^jo::view<void\*>\((.*)\)$", r"\1", e)
    e = re.sub(r"^joan::orig::arg::mptr\((.*)\)$", r"\1", e)
    e = re.sub(r"^(?:img::|joan::img::)?obj\(", "obj(", e)
    e = re.sub(r"^(?:joan::hook::|hook::)?global<void\*>\(", "global<void*>(", e)
    e = re.sub(r"^global<(?:std::)?u?int32_t>\(d::", "global<void*>(d::", e)
    return e


def bodies(t):
    """(name, start, end) of function-like bodies: a line ending '{' after ')'."""
    out = []
    for m in re.finditer(r"\n([ \t]*)(?:[\w:<>,\*& ]+?)\b(\w+)\(([^;{}]*?)\)\s*(?:const\s*)?(?:noexcept\s*)?(?:->\s*[^{;]*)?\{", t):
        name = m.group(2)
        if name in ("if", "for", "while", "switch", "return", "catch"): continue
        i = m.end(); d = 1
        while i < len(t) and d:
            c = t[i]
            if c == "{": d += 1
            elif c == "}": d -= 1
            i += 1
        out.append((name, m.end(), i))
    return out


reads = collections.defaultdict(lambda: collections.defaultdict(set))  # (file, fn, obj) -> class -> offsets
lines = {}
impl_of = {}   # address -> (file, fn)
calls = []     # (file, fn, obj, class_of_facade, method)
interior = []
for f in sorted((ROOT / "src").rglob("*.cpp")):
    s = str(f.relative_to(ROOT))
    if "/original/gen/" in s or s.startswith("src/data/"): continue
    t = f.read_text(errors="replace")
    # file constants naming global objects; one file often defines the same name (kSprites, kMission) for different
    # objects in different namespace blocks, so each function sees the definitions made before it, the latest winning
    defs = []
    for m in re.finditer(r"(?:void\*|auto)\s*const\s+(\w+)\s*(?:=|\{)\s*((?:joan::)?(?:img::)?obj\(d::\w+\)|(?:joan::hook::)?global<void\*>\(d::\w+\))", t):
        defs.append((m.start(), m.group(1), norm(m.group(2))))
    for m in re.finditer(r"constexpr\s+(?:joan::)?img::Addr\s+(\w+)\s*(?:=|\{|\()\s*d::(\w+)", t):
        defs.append((m.start(), "obj(" + m.group(1) + ")", "obj(d::" + m.group(2) + ")"))
    defs.sort()
    for m in reg_re.finditer(t):
        impl_of.setdefault(int(m.group(1), 16), set()).add((s, m.group(2)))
    for (fn, b0, b1) in bodies(t):
        consts = {k: v for (pos, k, v) in defs if pos < b0}
        body = t[b0:b1]
        for m in VIEW.finditer(body):
            cls = m.group(1) or acc.get(m.group(2))
            if not cls: continue
            e, j = args_at(body, m.end())
            fm = re.match(r"\)?->(\w+)", body[j - 1: j + 60]) if False else re.match(r"->(\w+)", body[j: j + 60])
            if e.startswith("&"):
                im = re.match(r"&(?:jo::self<jo::(\w+)>\(|jo::(as_\w+)\()(.+)\)->(\w+)$", re.sub(r"\s+", "", e))
                if im:
                    oc = im.group(1) or acc.get(im.group(2))
                    interior.append((s, t[:b0].count("\n") + body[: m.start()].count("\n") + 1, fn, cls, oc, im.group(4), fm.group(1) if fm else "*"))
                continue
            if not fm: continue
            off = member(cls, fm.group(1))
            if off is None: continue
            key = (s, fn, norm(e, consts))
            reads[key][cls].add(off)
            lines.setdefault((key, cls), t[:b0].count("\n") + body[: m.start()].count("\n") + 1)
        for m in FAC.finditer(body):
            cls = acc.get(m.group(1))
            if not cls: continue
            e, j = args_at(body, m.end())
            mm = re.match(r"->(\w+)\(", body[j: j + 80])
            if mm: calls.append((s, fn, norm(e, consts), cls, mm.group(1), t[:b0].count("\n") + body[: m.start()].count("\n") + 1))

# objects typed by bss
def bss_class(e):
    m = re.match(r"obj\(d::(\w+)\)$", e)
    if m and m.group(1) in dsym: return bss.get(dsym[m.group(1)])
    return None

report = []
# 1. within one function, one object, two classes
for key, cls_offs in reads.items():
    cl = sorted(cls_offs)
    bc = bss_class(key[2])
    if bc and bc not in cls_offs and bc in L86:
        for c in cl:
            for x in conflict(c, cls_offs[c], bc, cls_offs[c])[:1]:
                report.append(("BSS", key[0], lines[(key, c)], key[1], key[2], c, bc, x))
    for i in range(len(cl)):
        for k in range(i + 1, len(cl)):
            x = conflict(cl[i], cls_offs[cl[i]], cl[k], cls_offs[cl[k]])
            if x: report.append(("SAMEFN", key[0], lines[(key, cl[i])], key[1], key[2], cl[i], cl[k], x[0]))

# 2a. fixpoint: evidence for each function's `this` = own reads + what callers passing an object know about it
evid_this = collections.defaultdict(lambda: collections.defaultdict(set))
for key, co in reads.items():
    if key[2] == "this":
        for c, o in co.items(): evid_this[(key[0], key[1])][c] |= o
glob = collections.defaultdict(lambda: collections.defaultdict(set))  # global object -> class -> offsets
glob_where = {}
for key, co in reads.items():
    if key[2].startswith(("obj(d::", "global<void*>(d::")):
        for c, o in co.items():
            glob[key[2]][c] |= o; glob_where.setdefault((key[2], c), "%s:%s" % (key[0], key[1]))
for _ in range(4):
    for (s, fn, e, fcls, meth, ln) in calls:
        if e == "this": src = evid_this.get((s, fn), {})
        elif e.startswith(("obj(d::", "global<void*>(d::")): src = glob.get(e, {})
        else: src = reads.get((s, fn, e), {})
        for addr in facade2.get((fcls, meth), ()):
            for (cs, cfn) in impl_of.get(addr, ()):
                own = this_reads_own = reads.get((cs, cfn, "this"), {})
                tgt = evid_this[(cs, cfn)]
                for c, o in src.items():
                    if c not in tgt: tgt[c] = set()
                if e.startswith(("obj(d::", "global<void*>(d::")):
                    for c, o in own.items():
                        glob[e][c] |= o; glob_where.setdefault((e, c), "%s:%s %#x" % (cs, cfn, addr))
# global objects: classes read anywhere through them or by the impls called on them
for g, co in glob.items():
    cl = sorted(co)
    bc = bss_class(g)
    for i in range(len(cl)):
        for k in range(i + 1, len(cl)):
            x = conflict(cl[i], co[cl[i]], cl[k], co[cl[k]])
            if x: report.append(("GLOBAL", "-", 0, "-", g, cl[i] + " @" + glob_where[(g, cl[i])], cl[k] + " @" + glob_where[(g, cl[k])], x[0]))
# functions whose `this` evidence (own + callers) conflicts with what they read
for (s, fn), ev in evid_this.items():
    own = reads.get((s, fn, "this"), {})
    for c2, o2 in own.items():
        for c1 in ev:
            if c1 == c2 or c1 in own: continue
            x = conflict(c1, o2, c2, o2)
            if x: report.append(("THIS", s, lines.get(((s, fn, "this"), c2), 0), fn, "this", c1 + " (from callers)", c2, x[0]))
# 2. caller's classes for the object -> callee's `this`
this_reads = {}
for key, co in reads.items():
    if key[2] == "this": this_reads[(key[0], key[1])] = co
for (s, fn, e, fcls, meth, ln) in calls:
    caller = reads.get((s, fn, e), {})
    evid = dict(caller)
    bc = bss_class(e)
    if bc: evid.setdefault(bc, set())
    if not evid: continue
    for addr in facade2.get((fcls, meth), ()):
        for (cs, cfn) in impl_of.get(addr, ()):
            callee = this_reads.get((cs, cfn))
            if not callee: continue
            for c1, o1 in evid.items():
                for c2, o2 in callee.items():
                    if c1 == c2: continue
                    x = conflict(c1, o1, c2, o2)
                    if x: report.append(("CALL", s, ln, fn, e, c1 + " (caller)", c2 + " in %s:%s %#x" % (cs, cfn, addr), x[0]))

# 3. interior views
for (s, ln, fn, cls, oc, f, rf) in interior:
    if not oc or cls not in L86 or oc not in L86: continue
    base = member(oc, f)
    if base is None: continue
    mo = mp(oc); mv = mp(cls)
    if not mo or not mv: continue
    offs = [member(cls, rf)] if rf != "*" else [x[0] for x in L86[cls]["fields"]]
    for o in offs:
        if o is None or base + o >= L86[oc]["size"]: continue
        a, b, c = mo(base + o), mo(base), mv(o)
        if a is None or b is None or c is None: continue
        if a[0] == "inptr" or b[0] == "inptr" or c[0] == "inptr" or (a[0] - b[0]) != c[0]:
            report.append(("INTERIOR", s, ln, fn, "&%s::%s" % (oc, f), cls + "->" + (rf if rf != "*" else "(view)"), oc, (o, c, a)))
            break

seen = set()
for r in sorted(report, key=lambda r: (r[0], r[1], r[2])):
    k = r[:7]
    if k in seen: continue
    seen.add(k)
    print("%-8s %s:%d %s  obj=%s  %s vs %s  x86 +%#x: %s / %s" % (r[0], r[1], r[2], r[3], r[4][:60], r[5], r[6], r[7][0], r[7][1], r[7][2]))
print(len(seen), "findings", file=sys.stderr)
