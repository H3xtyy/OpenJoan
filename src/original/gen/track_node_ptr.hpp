// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TrackNodePtr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TrackNode;

#pragma pack(push, 1)
struct TrackNodePtr {
#include "track_node_ptr_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TrackNodePtr) == 0x4);
static_assert(offsetof(TrackNodePtr, p) == 0x0);
#endif

}  // namespace joan::orig
