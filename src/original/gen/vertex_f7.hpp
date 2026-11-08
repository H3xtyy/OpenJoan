// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VertexF7 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VertexF7 {
#include "vertex_f7_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VertexF7) == 0x48);
static_assert(offsetof(VertexF7, influence_pos) == 0x0);
static_assert(offsetof(VertexF7, influence_normal) == 0x18);
static_assert(offsetof(VertexF7, bone) == 0x30);
static_assert(offsetof(VertexF7, weight) == 0x34);
static_assert(offsetof(VertexF7, tail) == 0x3c);
#endif

}  // namespace joan::orig
