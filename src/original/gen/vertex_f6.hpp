// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VertexF6 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VertexF6 {
#include "vertex_f6_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VertexF6) == 0x14);
static_assert(offsetof(VertexF6, pos) == 0x0);
static_assert(offsetof(VertexF6, attr) == 0xc);
#endif

}  // namespace joan::orig
