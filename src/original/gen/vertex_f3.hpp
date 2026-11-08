// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VertexF3 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VertexF3 {
#include "vertex_f3_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VertexF3) == 0x1c);
static_assert(offsetof(VertexF3, pos) == 0x0);
static_assert(offsetof(VertexF3, attr) == 0xc);
#endif

}  // namespace joan::orig
