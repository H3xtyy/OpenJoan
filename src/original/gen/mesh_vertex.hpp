// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MeshVertex class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PfdPoint;

#pragma pack(push, 1)
struct MeshVertex {
#include "mesh_vertex_fields.inc"
#include "mesh_vertex_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MeshVertex) == 0x14);
static_assert(offsetof(MeshVertex, point) == 0x0);
static_assert(offsetof(MeshVertex, next) == 0x4);
static_assert(offsetof(MeshVertex, prev) == 0x8);
static_assert(offsetof(MeshVertex, assist_next) == 0xc);
static_assert(offsetof(MeshVertex, assist_prev) == 0x10);
#endif

}  // namespace joan::orig
