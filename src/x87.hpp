// Bit-exact emulation of x87 arithmetic on SSE2 doubles, for reimplementing floating-point code of joan.exe.
//
// The implementation is joan::fp (src/fp/x87.hpp: precision control, Fpu, ftol, the pc24_* operations; the other
// models in src/fp/). This header keeps the names the game code uses: joan::hook::x87::Fpu / ftol /
// current_precision / Precision / round_to_float.
#pragma once

#include "fp/x87.hpp"

namespace joan::hook::x87 {

using joan::fp::current_precision;
using joan::fp::Fpu;
using joan::fp::ftol;
using joan::fp::Precision;
using joan::fp::round_to_float;

}  // namespace joan::hook::x87
