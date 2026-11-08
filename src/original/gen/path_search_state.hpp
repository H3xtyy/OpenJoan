// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathSearchState class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../point.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PathSearchState {
#include "path_search_state_fields.inc"
#include "path_search_state_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PathSearchState) == 0x34);
static_assert(offsetof(PathSearchState, buffer) == 0x8);
static_assert(offsetof(PathSearchState, pos) == 0x10);
#endif

}  // namespace joan::orig
