// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Plane class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "resource.hpp"

namespace joan::orig {

struct Actor;
struct ArrI32;
struct ArrU16;
struct ArrU32;
struct Bone;
struct IDirect3DIndexBuffer8;
struct IDirect3DVertexBuffer8;
struct Material;
struct MaterialSpan;
struct Matrix44;
struct Tri16;
struct Vec3f;
struct VertexF1;
struct VertexF2;
struct VertexF3;
struct VertexF4;
struct VertexF5;
struct VertexF6;
struct VertexF7;
struct VertexF8;

#pragma pack(push, 1)
struct Plane {
#include "plane_fields.inc"
#include "plane_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Plane) == 0x11c);
static_assert(offsetof(Plane, bones) == 0x0);
static_assert(offsetof(Plane, vertex_format) == 0x4);
static_assert(offsetof(Plane, buffer_mode) == 0x5);
static_assert(offsetof(Plane, bone_count) == 0x6);
static_assert(offsetof(Plane, owns_bones) == 0x7);
static_assert(offsetof(Plane, prim_type) == 0x8);
static_assert(offsetof(Plane, skip_draw) == 0x9);
static_assert(offsetof(Plane, load_state) == 0xa);
static_assert(offsetof(Plane, data_pending) == 0xb);
static_assert(offsetof(Plane, source) == 0xc);
static_assert(offsetof(Plane, vertex_count) == 0x10);
static_assert(offsetof(Plane, vertex_capacity) == 0x14);
static_assert(offsetof(Plane, transformed) == 0x18);
static_assert(offsetof(Plane, vertices_f1) == 0x1c);
static_assert(offsetof(Plane, vertices_f2) == 0x20);
static_assert(offsetof(Plane, vertices_f3) == 0x24);
static_assert(offsetof(Plane, vertices_f4) == 0x28);
static_assert(offsetof(Plane, vertices_f5) == 0x2c);
static_assert(offsetof(Plane, vertices_f6) == 0x30);
static_assert(offsetof(Plane, vertices_f7) == 0x34);
static_assert(offsetof(Plane, vertices_f8) == 0x38);
static_assert(offsetof(Plane, colour_) == 0x3c);
static_assert(offsetof(Plane, triangle_count) == 0x40);
static_assert(offsetof(Plane, triangle_capacity) == 0x44);
static_assert(offsetof(Plane, triangles) == 0x48);
static_assert(offsetof(Plane, triangle_materials) == 0x4c);
static_assert(offsetof(Plane, use_strips) == 0x50);
static_assert(offsetof(Plane, has_strips) == 0x51);
static_assert(offsetof(Plane, strip_total) == 0x52);
static_assert(offsetof(Plane, strip_lengths) == 0x54);
static_assert(offsetof(Plane, strip_capacity) == 0x58);
static_assert(offsetof(Plane, strip_index_count) == 0x5c);
static_assert(offsetof(Plane, strip_indices) == 0x60);
static_assert(offsetof(Plane, material_count) == 0x64);
static_assert(offsetof(Plane, material_capacity) == 0x68);
static_assert(offsetof(Plane, materials) == 0x6c);
static_assert(offsetof(Plane, list_kind) == 0x70);
static_assert(offsetof(Plane, owner) == 0x74);
static_assert(offsetof(Plane, origin) == 0x78);
static_assert(offsetof(Plane, material_matrices) == 0x84);
static_assert(offsetof(Plane, material_ids) == 0x88);
static_assert(offsetof(Plane, dirty) == 0x8c);
static_assert(offsetof(Plane, vertex_buffer) == 0x90);
static_assert(offsetof(Plane, index_buffer) == 0x94);
static_assert(offsetof(Plane, resource) == 0x98);
static_assert(offsetof(Plane, material_first) == 0xb0);
static_assert(offsetof(Plane, material_ranges) == 0xb4);
static_assert(offsetof(Plane, list_slots_ready) == 0xb8);
static_assert(offsetof(Plane, list_slots_inline) == 0xbc);
static_assert(offsetof(Plane, list_slots) == 0xcc);
static_assert(offsetof(Plane, children) == 0xd0);
static_assert(offsetof(Plane, lod_distance) == 0xe0);
static_assert(offsetof(Plane, lod_count) == 0xe4);
static_assert(offsetof(Plane, u8_e7) == 0xe7);
static_assert(offsetof(Plane, parent) == 0xe8);
static_assert(offsetof(Plane, next) == 0xec);
static_assert(offsetof(Plane, linked) == 0xf0);
static_assert(offsetof(Plane, back_link) == 0xf4);
static_assert(offsetof(Plane, normals_pending_) == 0xf8);
static_assert(offsetof(Plane, u32_fc) == 0xfc);
static_assert(offsetof(Plane, flags_100) == 0x100);
static_assert(offsetof(Plane, flags_104) == 0x104);
static_assert(offsetof(Plane, shadow_edges) == 0x108);
static_assert(offsetof(Plane, stencil) == 0x10c);
static_assert(offsetof(Plane, mode) == 0x110);
static_assert(offsetof(Plane, face_normals) == 0x114);
static_assert(offsetof(Plane, vertex_normals) == 0x118);
#endif

}  // namespace joan::orig
