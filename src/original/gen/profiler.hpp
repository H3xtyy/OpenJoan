// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Profiler class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ArrU32;
struct ArrU64;
struct ProfFunc;
struct ProfNode;

#pragma pack(push, 1)
struct Profiler {
#include "profiler_fields.inc"
#include "profiler_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Profiler, enabled) == 0x0);
static_assert(offsetof(Profiler, func_count) == 0x4);
static_assert(offsetof(Profiler, frames) == 0x8);
static_assert(offsetof(Profiler, frame_slot) == 0xc);
static_assert(offsetof(Profiler, time_accum) == 0x10);
static_assert(offsetof(Profiler, frame_avg) == 0x18);
static_assert(offsetof(Profiler, clock) == 0x20);
static_assert(offsetof(Profiler, last) == 0x28);
static_assert(offsetof(Profiler, total) == 0x2c);
static_assert(offsetof(Profiler, start) == 0x30);
static_assert(offsetof(Profiler, depth) == 0x34);
static_assert(offsetof(Profiler, average) == 0x38);
static_assert(offsetof(Profiler, history) == 0x3c);
static_assert(offsetof(Profiler, history_hits) == 0x40);
static_assert(offsetof(Profiler, cpu_hz) == 0x44);
static_assert(offsetof(Profiler, funcs) == 0x4c);
static_assert(offsetof(Profiler, root) == 0x50);
static_assert(offsetof(Profiler, current) == 0x54);
static_assert(offsetof(Profiler, last_report) == 0x58);
static_assert(offsetof(Profiler, u32_5c) == 0x5c);
#endif

}  // namespace joan::orig
