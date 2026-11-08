// The two builds of openjoan.exe (CMakePresets.json):
//
//   debug   (presets linux-x64, windows-hook-x64, vs2026-hook-x64): every openjoan.ini setting is read - logs, the developer menu,
//           freecam, profiler, record / replay, consistency checks, shadow maps, [Fixes] switches (src/openjoan.ini).
//   release (presets linux-x64-release, vs2026-hook-x64-release; CMake -DJOAN_RELEASE=ON): the player build.
//           Only the settings of src/openjoan_release.ini are read; every other one is a constant of the build
//           (Settings in registry.hpp: JOAN_DEBUG_SETTING), so its checks compile away: no logs, no call counters,
//           no developer menu / freecam / profiler / replay, no consistency checks, the original stencil shadows
//           only (no shadow map), every [Fixes] correction on, HRTF off, near sounds centred.
//
// Both are optimized the same way; only what can be configured differs.
#pragma once

#ifndef JOAN_RELEASE
#define JOAN_RELEASE 0
#endif

namespace joan {
inline constexpr bool kReleaseBuild = JOAN_RELEASE != 0;
}  // namespace joan
