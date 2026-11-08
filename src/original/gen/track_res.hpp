// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TrackRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PathGraph;

#pragma pack(push, 1)
struct TrackRes {
#include "track_res_fields.inc"
#include "track_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TrackRes) == 0x9);
static_assert(offsetof(TrackRes, paths) == 0x0);
static_assert(offsetof(TrackRes, path_count) == 0x4);
static_assert(offsetof(TrackRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
