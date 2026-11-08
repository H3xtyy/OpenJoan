# tools/re

Helpers for running and inspecting the original game under Wine on Linux. Optional:
OpenJoan itself needs neither Wine nor these. They still use other environment names (`JOA_WORK_DIR`,
`joan_steam.exe`, `WINEPREFIX`); TODO(docs): adapt them to `JOAN_WORK_DIR` and the reference `joan.exe` before use.
Never run them against `JOAN_GAME_DIR`; work on a copy under `JOAN_WORK_DIR`.

| File | Does |
|---|---|
| `dump_data.py` | prints what lives at virtual addresses of a PE file: C strings and dword / float tables (`EXE VA[:N] ...`; needs `pefile`) |
| `sample_dump.py` | hex plus `u32` / float views of the heads of files, written as Markdown (`OUT.md [--bytes N] FILE...`) |
| `wine_dump_image.py` | starts a 32-bit PE under Wine, waits, dumps its mapped image from `/proc/<pid>/mem` and compares each section with the file on disk |
| `apitrace_capture.sh` | captures a Direct3D 8 trace of the original game under Wine with apitrace's Windows build |
| `trace_frames.py` | extracts reference frames (PNG) from such a trace by replaying it with `d3dretrace` under Wine; slow for late frames |

The script headers hold the usage lines and prerequisites.
