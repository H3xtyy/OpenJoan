// SPDX-License-Identifier: GPL-3.0-or-later
// The emulated x87 precision control (overview: fp/x87.hpp), shared by the x87 models (fp/x87.hpp, fp/x87_64.hpp).
#pragma once

namespace joan::fp {

// The x87 precision control the original's arithmetic runs at, kept in software (no x87 control word is read, so
// x86 and x64 / other systems agree): like the x87 control word it is per thread and starts at 53 bits (the
// Windows default; the C runtime's _setdefaultprecision asks for the same); the renderer's device switches its thread
// to 24 bits as Direct3D 8 does when the game creates its device without D3DCREATE_FPU_PRESERVE (gl/device.cpp
// set_precision). Rounding is round-to-nearest throughout: neither the game nor our code changes the rounding mode,
// so it is not asked here (fegetround is a C runtime call that reads the x87 and SSE control words - asked per
// emulated operation it cost ~6% of a frame). Unsupported is kept for callers that test for it.
enum class Precision { Single, Double, Unsupported };
inline thread_local Precision t_precision = Precision::Double;
/// What Direct3D 8's device creation does to the calling thread's x87 control word (and the C runtime's
/// _controlfp(_PC_53)): the precision this thread's emulated x87 arithmetic runs at from now on.
inline void set_precision(Precision p) { t_precision = p; }
inline Precision current_precision() { return t_precision; }

}  // namespace joan::fp
