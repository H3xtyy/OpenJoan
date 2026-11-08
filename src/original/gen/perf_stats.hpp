// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PerfStats class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "profiler.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PerfStats : Profiler {
#include "perf_stats_fields.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
