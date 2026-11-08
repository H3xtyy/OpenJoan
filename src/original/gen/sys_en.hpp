// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SysEn class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../colour.hpp"
#include "d3dcaps8.hpp"

namespace joan::orig {

struct DynArray;
struct ID3DXMatrixStack;
struct IDirect3D8;
struct IDirect3DDevice8;
struct IDirect3DIndexBuffer8;
struct IDirect3DSurface8;
struct IDirect3DTexture8;
struct IDirect3DVertexBuffer8;
struct PixelShaderCache;
struct Resource;
struct VsCache;

#pragma pack(push, 1)
struct SysEn {
#include "sys_en_fields.inc"
#include "sys_en_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SysEn) == 0x337);
static_assert(offsetof(SysEn, started) == 0x0);
static_assert(offsetof(SysEn, d3d) == 0x4);
static_assert(offsetof(SysEn, device) == 0x8);
static_assert(offsetof(SysEn, back_buffer) == 0xc);
static_assert(offsetof(SysEn, depth_stencil) == 0x10);
static_assert(offsetof(SysEn, device_type) == 0x14);
static_assert(offsetof(SysEn, pp_back_buffer_width) == 0x18);
static_assert(offsetof(SysEn, pp_back_buffer_height) == 0x1c);
static_assert(offsetof(SysEn, pp_back_buffer_format) == 0x20);
static_assert(offsetof(SysEn, pp_back_buffer_count) == 0x24);
static_assert(offsetof(SysEn, pp_multisample_type) == 0x28);
static_assert(offsetof(SysEn, pp_swap_effect) == 0x2c);
static_assert(offsetof(SysEn, pp_device_window) == 0x30);
static_assert(offsetof(SysEn, pp_windowed) == 0x34);
static_assert(offsetof(SysEn, pp_enable_auto_depth_stencil) == 0x38);
static_assert(offsetof(SysEn, pp_auto_depth_stencil_format) == 0x3c);
static_assert(offsetof(SysEn, pp_flags) == 0x40);
static_assert(offsetof(SysEn, pp_refresh_rate) == 0x44);
static_assert(offsetof(SysEn, pp_presentation_interval) == 0x48);
static_assert(offsetof(SysEn, u32_4c) == 0x4c);
static_assert(offsetof(SysEn, caps) == 0x50);
static_assert(offsetof(SysEn, matrix_stack) == 0x124);
static_assert(offsetof(SysEn, frame_index) == 0x128);
static_assert(offsetof(SysEn, dot3_vertex_shader) == 0x12c);
static_assert(offsetof(SysEn, cur_vertex_type) == 0x130);
static_assert(offsetof(SysEn, cur_blend) == 0x131);
static_assert(offsetof(SysEn, cur_texture_mode) == 0x135);
static_assert(offsetof(SysEn, cur_filter) == 0x139);
static_assert(offsetof(SysEn, cur_flags) == 0x13d);
static_assert(offsetof(SysEn, cur_texture_factor) == 0x141);
static_assert(offsetof(SysEn, cur_raster) == 0x145);
static_assert(offsetof(SysEn, vs_bound) == 0x149);
static_assert(offsetof(SysEn, new_vertex_type) == 0x14a);
static_assert(offsetof(SysEn, new_blend) == 0x14b);
static_assert(offsetof(SysEn, new_texture_mode) == 0x14f);
static_assert(offsetof(SysEn, new_filter) == 0x153);
static_assert(offsetof(SysEn, new_texture_factor) == 0x157);
static_assert(offsetof(SysEn, new_flags) == 0x15b);
static_assert(offsetof(SysEn, new_raster) == 0x15f);
static_assert(offsetof(SysEn, dirty) == 0x163);
static_assert(offsetof(SysEn, cur_overlay_stage) == 0x167);
static_assert(offsetof(SysEn, cur_tint_stage) == 0x16b);
static_assert(offsetof(SysEn, new_overlay_stage) == 0x16f);
static_assert(offsetof(SysEn, new_tint_stage) == 0x173);
static_assert(offsetof(SysEn, vs_cache) == 0x177);
static_assert(offsetof(SysEn, ps_cache) == 0x17b);
static_assert(offsetof(SysEn, light_slot_count) == 0x17f);
static_assert(offsetof(SysEn, u8_183) == 0x183);
static_assert(offsetof(SysEn, dyn_vb) == 0x18f);
static_assert(offsetof(SysEn, dyn_ib) == 0x1b3);
static_assert(offsetof(SysEn, dyn_vb_used) == 0x1b7);
static_assert(offsetof(SysEn, dyn_ib_used) == 0x1db);
static_assert(offsetof(SysEn, locked_vb) == 0x1df);
static_assert(offsetof(SysEn, locked_vertex_type) == 0x1e3);
static_assert(offsetof(SysEn, locked_start) == 0x1e7);
static_assert(offsetof(SysEn, locked_stride) == 0x1eb);
static_assert(offsetof(SysEn, locked_count) == 0x1ef);
static_assert(offsetof(SysEn, index_start) == 0x1f3);
static_assert(offsetof(SysEn, index_count) == 0x1f7);
static_assert(offsetof(SysEn, vb_usage) == 0x1fb);
static_assert(offsetof(SysEn, ib_usage) == 0x1ff);
static_assert(offsetof(SysEn, vs_usable) == 0x203);
static_assert(offsetof(SysEn, dot3_usable) == 0x207);
static_assert(offsetof(SysEn, hw_skinning) == 0x20b);
static_assert(offsetof(SysEn, mipmaps_supported) == 0x20f);
static_assert(offsetof(SysEn, bump_supported) == 0x213);
static_assert(offsetof(SysEn, texture_caps_800) == 0x217);
static_assert(offsetof(SysEn, stats) == 0x21b);
static_assert(offsetof(SysEn, stats_last) == 0x247);
static_assert(offsetof(SysEn, stats_sum) == 0x273);
static_assert(offsetof(SysEn, stats_avg) == 0x29f);
static_assert(offsetof(SysEn, stats_frames) == 0x2cb);
static_assert(offsetof(SysEn, stats_time) == 0x2cf);
static_assert(offsetof(SysEn, prev_frame_timer) == 0x2d3);
static_assert(offsetof(SysEn, frame_timer) == 0x2d7);
static_assert(offsetof(SysEn, dot3_created) == 0x2db);
static_assert(offsetof(SysEn, dot3_buffer_a) == 0x2df);
static_assert(offsetof(SysEn, u32_2e3) == 0x2e3);
static_assert(offsetof(SysEn, dot3_buffer_b) == 0x2e7);
static_assert(offsetof(SysEn, u32_2eb) == 0x2eb);
static_assert(offsetof(SysEn, brightness) == 0x2ef);
static_assert(offsetof(SysEn, contrast) == 0x2f3);
static_assert(offsetof(SysEn, gamma) == 0x2f7);
static_assert(offsetof(SysEn, gamma_offset) == 0x2fb);
static_assert(offsetof(SysEn, depth_format) == 0x307);
static_assert(offsetof(SysEn, stencil_bits) == 0x30b);
static_assert(offsetof(SysEn, mirrored) == 0x30f);
static_assert(offsetof(SysEn, user_clip_planes) == 0x313);
static_assert(offsetof(SysEn, depth_target) == 0x317);
static_assert(offsetof(SysEn, depth_target_width) == 0x31b);
static_assert(offsetof(SysEn, depth_target_height) == 0x31f);
static_assert(offsetof(SysEn, light_target) == 0x323);
static_assert(offsetof(SysEn, light_target_width) == 0x327);
static_assert(offsetof(SysEn, light_target_height) == 0x32b);
static_assert(offsetof(SysEn, resources_head) == 0x32f);
static_assert(offsetof(SysEn, resources_tail) == 0x333);
#endif

}  // namespace joan::orig
