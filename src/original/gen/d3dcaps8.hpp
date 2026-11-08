// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's D3DCaps8 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct D3DCaps8 {
#include "d3dcaps8_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(D3DCaps8) == 0xd4);
static_assert(offsetof(D3DCaps8, device_type) == 0x0);
static_assert(offsetof(D3DCaps8, adapter_ordinal) == 0x4);
static_assert(offsetof(D3DCaps8, caps) == 0x8);
static_assert(offsetof(D3DCaps8, caps2) == 0xc);
static_assert(offsetof(D3DCaps8, caps3) == 0x10);
static_assert(offsetof(D3DCaps8, presentation_intervals) == 0x14);
static_assert(offsetof(D3DCaps8, cursor_caps) == 0x18);
static_assert(offsetof(D3DCaps8, dev_caps) == 0x1c);
static_assert(offsetof(D3DCaps8, primitive_misc_caps) == 0x20);
static_assert(offsetof(D3DCaps8, raster_caps) == 0x24);
static_assert(offsetof(D3DCaps8, z_cmp_caps) == 0x28);
static_assert(offsetof(D3DCaps8, src_blend_caps) == 0x2c);
static_assert(offsetof(D3DCaps8, dest_blend_caps) == 0x30);
static_assert(offsetof(D3DCaps8, alpha_cmp_caps) == 0x34);
static_assert(offsetof(D3DCaps8, shade_caps) == 0x38);
static_assert(offsetof(D3DCaps8, texture_caps) == 0x3c);
static_assert(offsetof(D3DCaps8, texture_filter_caps) == 0x40);
static_assert(offsetof(D3DCaps8, cube_texture_filter_caps) == 0x44);
static_assert(offsetof(D3DCaps8, volume_texture_filter_caps) == 0x48);
static_assert(offsetof(D3DCaps8, texture_address_caps) == 0x4c);
static_assert(offsetof(D3DCaps8, volume_texture_address_caps) == 0x50);
static_assert(offsetof(D3DCaps8, line_caps) == 0x54);
static_assert(offsetof(D3DCaps8, max_texture_width) == 0x58);
static_assert(offsetof(D3DCaps8, max_texture_height) == 0x5c);
static_assert(offsetof(D3DCaps8, max_volume_extent) == 0x60);
static_assert(offsetof(D3DCaps8, max_texture_repeat) == 0x64);
static_assert(offsetof(D3DCaps8, max_texture_aspect_ratio) == 0x68);
static_assert(offsetof(D3DCaps8, max_anisotropy) == 0x6c);
static_assert(offsetof(D3DCaps8, max_vertex_w) == 0x70);
static_assert(offsetof(D3DCaps8, guard_band_left) == 0x74);
static_assert(offsetof(D3DCaps8, guard_band_top) == 0x78);
static_assert(offsetof(D3DCaps8, guard_band_right) == 0x7c);
static_assert(offsetof(D3DCaps8, guard_band_bottom) == 0x80);
static_assert(offsetof(D3DCaps8, extents_adjust) == 0x84);
static_assert(offsetof(D3DCaps8, stencil_caps) == 0x88);
static_assert(offsetof(D3DCaps8, fvf_caps) == 0x8c);
static_assert(offsetof(D3DCaps8, texture_op_caps) == 0x90);
static_assert(offsetof(D3DCaps8, max_texture_blend_stages) == 0x94);
static_assert(offsetof(D3DCaps8, max_simultaneous_textures) == 0x98);
static_assert(offsetof(D3DCaps8, vertex_processing_caps) == 0x9c);
static_assert(offsetof(D3DCaps8, max_active_lights) == 0xa0);
static_assert(offsetof(D3DCaps8, max_user_clip_planes) == 0xa4);
static_assert(offsetof(D3DCaps8, max_vertex_blend_matrices) == 0xa8);
static_assert(offsetof(D3DCaps8, max_vertex_blend_matrix_index) == 0xac);
static_assert(offsetof(D3DCaps8, max_point_size) == 0xb0);
static_assert(offsetof(D3DCaps8, max_primitive_count) == 0xb4);
static_assert(offsetof(D3DCaps8, max_vertex_index) == 0xb8);
static_assert(offsetof(D3DCaps8, max_streams) == 0xbc);
static_assert(offsetof(D3DCaps8, max_stream_stride) == 0xc0);
static_assert(offsetof(D3DCaps8, vertex_shader_version) == 0xc4);
static_assert(offsetof(D3DCaps8, max_vertex_shader_const) == 0xc8);
static_assert(offsetof(D3DCaps8, pixel_shader_version) == 0xcc);
static_assert(offsetof(D3DCaps8, max_pixel_shader_value) == 0xd0);
#endif

}  // namespace joan::orig
