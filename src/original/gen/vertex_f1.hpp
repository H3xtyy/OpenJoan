// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VertexF1 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VertexF1 {
#include "vertex_f1_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VertexF1) == 0x24);
static_assert(offsetof(VertexF1, pos) == 0x0);
static_assert(offsetof(VertexF1, attr) == 0xc);
#endif

}  // namespace joan::orig
