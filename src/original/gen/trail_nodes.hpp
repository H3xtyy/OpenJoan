// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TrailNodes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TrailNode;

#pragma pack(push, 1)
struct TrailNodes {
#include "trail_nodes_fields.inc"
#include "trail_nodes_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TrailNodes) == 0xc);
static_assert(offsetof(TrailNodes, newest) == 0x0);
static_assert(offsetof(TrailNodes, oldest) == 0x4);
static_assert(offsetof(TrailNodes, count_) == 0x8);
#endif

}  // namespace joan::orig
