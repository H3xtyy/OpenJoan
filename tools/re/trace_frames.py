#!/usr/bin/env python3
"""Extract reference frames of the ORIGINAL game from a Direct3D 8 apitrace capture.

d3dretrace's --snapshot reads a stale surface for joan_steam.exe under Wine (every snapshot shows the menu
background video), but its state dump (`-D CALL`) contains the real render target. This script finds the
Present call of each requested frame, runs `d3dretrace -D <present-1>` under Wine and writes the framebuffer
PNG. Usage (env JOA_WORK_DIR, WINEPREFIX as in docs/re/ENVIRONMENT.md):

    tools/re/trace_frames.py TRACE OUTDIR FRAME [FRAME ...]

Each frame replays the trace from the start, so frames late in a long trace take minutes each.
"""
import base64
import os
import re
import subprocess
import sys


def wine_path(p: str) -> str:
    return "Z:" + os.path.abspath(p).replace("/", "\\")


def present_calls(trace: str, frames: list[int]) -> dict[int, int]:
    want = set(frames)
    out: dict[int, int] = {}
    n = 0
    proc = subprocess.Popen(["apitrace", "dump", trace], stdout=subprocess.PIPE, text=True,
                            errors="replace")
    assert proc.stdout is not None
    for line in proc.stdout:
        if "IDirect3DDevice8::Present(" in line:
            n += 1
            if n in want:
                out[n] = int(line.split(" ", 1)[0])
                if len(out) == len(want):
                    break
    proc.kill()
    return out


def main() -> int:
    if len(sys.argv) < 4:
        print(__doc__)
        return 2
    trace, outdir = sys.argv[1], sys.argv[2]
    frames = sorted(int(f) for f in sys.argv[3:])
    os.makedirs(outdir, exist_ok=True)
    work = os.environ["JOA_WORK_DIR"]
    retrace = os.path.join(work, "tools/apitrace-14.0-win32/bin/d3dretrace.exe")
    env = dict(os.environ, WINEDEBUG="-all")
    env.setdefault("WINEPREFIX", os.path.join(work, "wine32"))
    calls = present_calls(trace, frames)
    for frame in frames:
        call = calls.get(frame)
        if call is None:
            print(f"frame {frame}: not in trace")
            continue
        res = subprocess.run(["wine", retrace, "-D", str(call - 1), wine_path(trace)], env=env,
                             capture_output=True)
        text = res.stdout.decode("latin-1")
        m = re.search(r'"__data__": "([^"]*)"', text)
        if not m:
            print(f"frame {frame}: no framebuffer in the state dump")
            continue
        png = os.path.join(outdir, f"frame_{frame:06d}.png")
        with open(png, "wb") as f:
            f.write(base64.b64decode(re.sub(r"\s", "", m.group(1))))
        print(f"frame {frame} (call {call}): {png}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
