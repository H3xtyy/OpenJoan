// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VertexFormat class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VertexFormat {
#include "vertex_format_fields.inc"
#include "vertex_format_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VertexFormat) == 0x10);
static_assert(offsetof(VertexFormat, type) == 0x0);
static_assert(offsetof(VertexFormat, tex_coords) == 0x1);
static_assert(offsetof(VertexFormat, prelit) == 0x3);
static_assert(offsetof(VertexFormat, stride) == 0x4);
static_assert(offsetof(VertexFormat, normal_offset) == 0x8);
static_assert(offsetof(VertexFormat, fvf) == 0xc);
#endif

}  // namespace joan::orig
