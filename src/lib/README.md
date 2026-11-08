# src/lib

Libraries the original executable linked statically, and the imports it took from DLLs, ported to C++ (and, for OpenAL, to
OpenAL Soft). They are part of the game's behaviour: their results are bit-compared with the original, which is why they
are ported rather than replaced by system libraries.

| Directory | What |
|---|---|
| [`crt/`](crt/README.md) | the MSVC 6 C runtime: heap, stdio / lowio, printf and scanf engines, locale, multibyte strings, time, start-up and exit, x87 maths |
| [`d3dx8/`](d3dx8/README.md) | the statically linked D3DX8 maths and texture loaders |
| [`jpeg/`](jpeg/README.md) | the IJG libjpeg 6b compressor (and D3DX8's `jcomapi` / `jerror`) |
| [`openal/`](openal/README.md) | ALut and the game's OpenAL calls on OpenAL Soft |
| [`v3d/`](v3d/README.md) | the 23 vector-maths exports of `v3d_87.dll` |
| [`imports/`](imports/README.md) | small Win32 / CRT imports implemented on the platform layer |

Library-owned static data (JPEG tables, CRT strings, D3DX vtables) is mostly not in the data image any more
(`src/data/README.md`); the libraries carry their own copies, compared with the original's at the
2026-11-08 review. Floating point goes through [`src/fp`](../fp/README.md).

Verification: objcmp for moves, the replay (every float result of the game feeds the state hash), a manual play test.
