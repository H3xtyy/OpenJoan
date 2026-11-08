# src/fp

`joan::fp`: bit-exact models of the x87 arithmetic of joan.exe in portable C++ (SSE2 / x64 doubles; no x87 instructions,
no inline assembly). The original computed on the x87 FPU with the precision control Direct3D 8 sets (24-bit
significands), so every float result of the game depends on the rounding of each step. Float results are part of the
behaviour contract (`docs/CONTRIBUTING.md` section 5): never reorder float expressions in ported code, never
`-ffast-math`.

| File | Model |
|---|---|
| `x87.hpp` | the overview (included through `src/x87.hpp`, whose `joan::hook::x87` names forward here): the precision control in force, `Fpu` (one double operation rounded to the current precision), `ftol` (MSVC 6 `_ftol`), the `pc24_*` operations |
| `precision.hpp` | the emulated precision control, shared by the models |
| `bits.hpp` | bit patterns of IEEE values, `pow2`, the x87's signalling-NaN quietening on `fld` / `fstp` dword |
| `exact.hpp` | error-free transformations, double-double arithmetic, 128-bit integers (the kernels of the others) |
| `fma.hpp` | fused multiply-add used for the error terms |
| `pc24.hpp` | the PC24 register model of the statically linked D3DX8 maths (`lib/d3dx8/math_*.cpp`, `objects.cpp`) |
| `pc53.hpp` | the CRT's `atan` / `tan` / `log` at 53-bit precision (`lib/crt/string.cpp`, `string_format.cpp`, `xtoa_misc.cpp`) |
| `x87_64.hpp` | a software x87 value (64-bit significand, wide exponent) with models of `fsin` / `fcos` / `fpatan` / `fyl2x` / `f2xm1`, used by the CRT maths engine `lib/crt/x87_math.cpp` |

The models are distinct on purpose: each is the proven model of its callers and they differ in corner cases (exponent
range, NaN rules, rounding of transcendental results), so a caller keeps the one it was proven with. At 64-bit
precision control or a non-nearest rounding mode doubles cannot reproduce the x87 (`fp/x87.hpp` comment).

Compiler flags are part of the behaviour: `-fno-strict-aliasing -fwrapv -ffp-contract=off` on GCC/Clang, x86
`-msse2 -mfpmath=sse`, MSVC `/arch:SSE2 /fp:precise` (`docs/ARCHITECTURE.md` section 6; `CMakeLists.txt`).

Verification: the record/replay run (state hash per frame, [`src/runtime/replay/`](../runtime/replay/README.md)) and the
x86-vs-x64 state check ([`docs/X64_STATE_CHECK.md`](../../docs/X64_STATE_CHECK.md)). The headers cite differential
harnesses under `re/p1/` that are not in this repository: TODO(docs): where do the harnesses of `fp/*.hpp` live now?

See also: [`docs/ARCHITECTURE.md`](../../docs/ARCHITECTURE.md) section 6, [`src/lib/crt/README.md`](../lib/crt/README.md),
[`src/lib/d3dx8/README.md`](../lib/d3dx8/README.md).
