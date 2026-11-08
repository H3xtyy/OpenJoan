#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""x64 scan (Phase 2, skill oj-x64-port): one object read through two class layouts that agree on x86 and not on x64.

The bug class behind fecf572, a953942, 55330ab, 5a6e267 and d3c8d7a: `jo::self<jo::X>(p)->f` on an object whose real
class is Y. On x86 X and Y often put f at the same offset; on x64 the pointers before it are 8 bytes and the offsets
drift apart. The hazard meter and x64_scan_two_layouts.py do not see it when the wrong view is the only view in the
function (the object's class is decided by the caller, a global, or a pointer into the middle of another object).

Steps (all in OUT, default build/x64-view-scan/):
 1. all_types.cpp names every class of src/original (gen + hand-written) and is compiled with the exact flags of
    build/linux-x86 and build/linux-x64 (compile_commands.json), so the layouts come from both compilers.
 2. gdb reads the flattened leaf fields of every class from both objects (x64_view_classes/dump_layouts.py).
 3. x64_view_classes/scan.py collects, per function and object expression, the classes whose FIELDS are read through
    it; follows `this` through `as_x(E)->method()` into the registered replacement of method's address; pools global
    objects (`obj(d::sym)`, `global<void*>(d::sym)`, file constants); applies the bss typing; checks views applied to
    `&owner->field` field by field. A pair of classes on one object is reported when an offset read through either
    maps to different x64 offsets in the two layouts (SAMEFN / CALL / THIS / GLOBAL / BSS / INTERIOR).
    `x64_view_classes/q.py OUT Class [field|0xOFF]` prints a class's x86 and x64 offsets for triage.

Advisory and wide: it matches functions and locals by NAME, so same-named methods of different classes in one file and
reused locals (`p`, `e`, `r`) produce most of the false positives (2026-11-03 sweep: 428 findings, 13 real; file constants
are resolved per position since, which leaves 232 on that tree). Every hit
is checked against the code; a fix is proven with objcmp / the replay gate (CLAUDE.md 5).

Usage: python3 tools/x64_scan_view_classes.py [--out DIR] [--root TREE] [--reuse]
  --root: scan another checkout (layouts still come from this tree's builds); --reuse: skip steps 1-2.
"""
import argparse, json, os, pathlib, re, shlex, subprocess, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent


def class_names():
    names = set()
    for h in (ROOT / "src/original/gen").glob("*.hpp"):
        names.update(re.findall(r"^struct (\w+)\b", h.read_text(), re.M))
    for h in (ROOT / "src/original").glob("*.hpp"):
        names.update(re.findall(r"^struct (\w+)\s*[:{]", h.read_text(), re.M))
    return names


def compile_command(arch, src, obj):
    cc = json.load(open(ROOT / f"build/linux-{arch}/compile_commands.json"))
    e = next(e for e in cc if e["file"].endswith("src/game/_unplaced/rts.cpp"))
    args = shlex.split(e["command"]) if "command" in e else e["arguments"]
    out, skip = [], False
    for x in args:
        if skip:
            skip = False
            continue
        if x in ("-o", "-c", "-MF", "-MT", "-MQ", "-include"):
            skip = True
            continue
        if x.endswith("rts.cpp") or x.startswith("-MD") or x == "-Winvalid-pch" or x.startswith("-Werror"):
            continue
        out.append(x)
    return e["directory"], out + ["-g", "-fno-eliminate-unused-debug-types", "-c", str(src), "-o", str(obj)]


def build_layouts(out):
    rts = (ROOT / "src/game/_unplaced/rts.cpp").read_text().splitlines()[:40]
    pre = "".join(l.replace('"../../', '"' + str(ROOT / "src") + "/") + "\n" for l in rts if l.startswith("#include"))
    names, bad = class_names(), set()
    src = out / "all_types.cpp"
    for _ in range(40):  # drop names that are not complete types visible from game code
        ok = sorted(names - bad)
        body = "".join(f"joan::orig::{n}* p_{n};\nunsigned long s_{n} = sizeof(joan::orig::{n});\n" for n in ok)
        src.write_text(pre + "namespace x64_view_scan {\n" + body + "}\n")
        d, cmd = compile_command("x86", src, out / "all_types_x86.o")
        r = subprocess.run(cmd, cwd=d, capture_output=True, text=True)
        if r.returncode == 0:
            break
        lines = src.read_text().splitlines()
        new = {m.group(1) for ln in set(re.findall(r"all_types\.cpp:(\d+):\d+: error", r.stderr))
               for m in [re.search(r"joan::orig::(\w+)", lines[int(ln) - 1])] if m}
        if not new:
            sys.exit(r.stderr[:3000])
        bad |= new
    (out / "classes.txt").write_text("\n".join(sorted(names - bad)) + "\n")
    d, cmd = compile_command("x64", src, out / "all_types_x64.o")
    subprocess.run(cmd, cwd=d, check=True)
    for arch in ("x86", "x64"):
        env = dict(os.environ, CLASSES=str(out / "classes.txt"), OUT=str(out / f"layout_{arch}.json"))
        subprocess.run(["gdb", "-q", "-batch", "-x", str(HERE / "x64_view_classes/dump_layouts.py"),
                        str(out / f"all_types_{arch}.o")], env=env, check=True, capture_output=True)
    print(f"layouts: {len(names - bad)} classes ({len(bad)} skipped: {', '.join(sorted(bad))})", file=sys.stderr)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=str(ROOT / "build/x64-view-scan"))
    ap.add_argument("--root", default=str(ROOT))
    ap.add_argument("--reuse", action="store_true")
    a = ap.parse_args()
    out = pathlib.Path(a.out)
    out.mkdir(parents=True, exist_ok=True)
    if not a.reuse or not (out / "layout_x64.json").exists():
        build_layouts(out)
    sys.exit(subprocess.run([sys.executable, str(HERE / "x64_view_classes/scan.py"), str(out), a.root]).returncode)


if __name__ == "__main__":
    main()
