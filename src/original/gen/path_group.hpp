// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathGroup class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "triple.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PathGroup {
#include "path_group_fields.inc"
#include "path_group_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PathGroup) == 0x34);
static_assert(offsetof(PathGroup, count) == 0x0);
static_assert(offsetof(PathGroup, entries) == 0x4);
#endif

}  // namespace joan::orig
