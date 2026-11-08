// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SkinVertex class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "skin_influence.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SkinVertex {
#include "skin_vertex_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SkinVertex) == 0x84);
static_assert(offsetof(SkinVertex, count) == 0x0);
static_assert(offsetof(SkinVertex, bone0) == 0x4);
static_assert(offsetof(SkinVertex, bone1) == 0x24);
static_assert(offsetof(SkinVertex, bone2) == 0x44);
static_assert(offsetof(SkinVertex, bone3) == 0x64);
#endif

}  // namespace joan::orig
