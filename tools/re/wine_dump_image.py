#!/usr/bin/env python3
"""wine_dump_image.py — launch a 32-bit PE under Wine, wait, then dump its mapped image from /proc/<pid>/mem.

The game process is spawned as a direct child so that Yama ptrace_scope=1 still allows reading its memory.
Writes <out_dir>/<name>.image.bin (one contiguous dump of ImageBase..ImageBase+SizeOfImage, zero-filled where
unmapped) and prints a per-section comparison against the on-disk file (bytes equal / differing).

Usage: wine_dump_image.py EXE --cwd GAME_DIR --wait SECONDS --out OUT_DIR [--wineprefix PREFIX]
"""
from __future__ import annotations
import argparse, os, subprocess, sys, time
from pathlib import Path
import pefile

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe"); ap.add_argument("--cwd", required=True); ap.add_argument("--wait", type=float, default=15)
    ap.add_argument("--out", required=True); ap.add_argument("--wineprefix")
    a = ap.parse_args()
    env = dict(os.environ, WINEDEBUG="-all")
    if a.wineprefix: env["WINEPREFIX"] = a.wineprefix
    pe = pefile.PE(a.exe, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase; size = pe.OPTIONAL_HEADER.SizeOfImage
    proc = subprocess.Popen(["wine", a.exe], cwd=a.cwd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(a.wait)
    # `wine` may fork/exec helper processes; pick the process that maps an executable page at the image base
    # (needs ptrace_scope 0 for non-children; with scope 1 only the direct child is readable).
    pid = None
    for d in Path("/proc").iterdir():
        if not d.name.isdigit():
            continue
        try:
            cmd = (d / "cmdline").read_bytes()
            if Path(a.exe).name.encode().lower() not in cmd.lower():
                continue
            for line in (d / "maps").read_text().splitlines():
                rng, perms, *_ = line.split()
                lo, hi = (int(x, 16) for x in rng.split("-"))
                if lo <= base + 0x1000 < hi and "x" in perms:
                    pid = int(d.name); break
        except OSError:
            continue
        if pid:
            break
    if pid is None:
        print("no process maps an executable page at the image base; candidates:")
        for d in Path("/proc").iterdir():
            if d.name.isdigit():
                try:
                    cmd = (d / "cmdline").read_bytes()
                    if Path(a.exe).name.encode().lower() in cmd.lower():
                        print(" ", d.name, cmd.replace(b"\0", b" ")[:100])
                except OSError:
                    pass
        proc.kill(); sys.exit(1)
    maps = Path(f"/proc/{pid}/maps").read_text().splitlines()
    print(f"pid {pid} (spawned {proc.pid}), {len(maps)} mappings")
    image = bytearray(size)
    mapped = 0
    with open(f"/proc/{pid}/mem", "rb", 0) as mem:
        for line in maps:
            rng, perms, *_ = line.split()
            lo, hi = (int(x, 16) for x in rng.split("-"))
            if hi <= base or lo >= base + size or "r" not in perms:
                continue
            lo2, hi2 = max(lo, base), min(hi, base + size)
            try:
                mem.seek(lo2); data = mem.read(hi2 - lo2)
            except OSError as e:
                print(f"  {rng} {perms}: read failed {e}"); continue
            image[lo2 - base: lo2 - base + len(data)] = data; mapped += len(data)
            print(f"  {rng} {perms} -> {len(data)} bytes")
    out = Path(a.out); out.mkdir(parents=True, exist_ok=True)
    dump = out / (Path(a.exe).name + ".image.bin"); dump.write_bytes(image)
    print(f"dumped {mapped} bytes to {dump}")
    raw = Path(a.exe).read_bytes()
    for s in pe.sections:
        name = s.Name.rstrip(b"\0").decode(); va = s.VirtualAddress; vs = s.Misc_VirtualSize
        on_disk = raw[s.PointerToRawData: s.PointerToRawData + s.SizeOfRawData]
        on_disk = on_disk[:vs].ljust(vs, b"\0")
        in_mem = bytes(image[va: va + vs])
        diff = sum(1 for x, y in zip(on_disk, in_mem) if x != y)
        first = next((i for i, (x, y) in enumerate(zip(on_disk, in_mem)) if x != y), None)
        print(f"section {name:8s} va {base+va:#x} vsize {vs:#8x}: differing bytes {diff:8d}" + (f" first at {base+va+first:#x}" if first is not None else ""))
    subprocess.run(["kill", "-9", str(pid)], check=False)
    proc.kill()
    try: proc.wait(5)
    except Exception: pass

if __name__ == "__main__":
    main()
