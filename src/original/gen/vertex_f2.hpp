// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VertexF2 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VertexF2 {
#include "vertex_f2_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VertexF2) == 0x3c);
static_assert(offsetof(VertexF2, pos) == 0x0);
static_assert(offsetof(VertexF2, attr) == 0xc);
#endif

}  // namespace joan::orig
