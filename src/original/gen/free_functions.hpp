// Generated (re/facades/gen_free.py): typed free functions for the cdecl / stdcall functions game code calls
// through their slots. Each forwards to the function's slot (the registered replacement).
#pragma once
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PathSpace;

/// 0x00661340 group_order
std::int32_t group_order(std::int32_t a0, std::int32_t a1, std::int8_t a2, std::int8_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8);
/// 0x006517c0 unit_forward_54b480
std::int32_t unit_forward_54b480(std::int32_t a0, const char* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6);
/// 0x00653000 script_heading_delta
std::int32_t script_heading_delta(std::int32_t a0, std::int32_t a1);
/// 0x00600200 vertex_format
void* vertex_format(char a0);
/// 0x007b4e1d free
void free_(void* a0);
/// 0x0060e3f0 segment_hits_sphere
std::int32_t segment_hits_sphere(void* a0, void* a1);
/// 0x0060bd00 orbit_matrix
inline void* orbit_matrix(void* a0, float a1, float a2, float a3, float a4) { return ::joan::call::cdecl_call<void*>(::joan::fn::init_rotation_hvr, a0, a1, a2, a3, a4); }
/// 0x00586bc0 warning_box
template <class... A>
inline void warning_box(void* a0, const char* a1, A... rest) { ::joan::call::cdecl_call<void>(::joan::fn::Error_msg, a0, a1, rest...); }
/// 0x0060bc50 matrix_rotation
void* matrix_rotation(void* a0, float a1);
/// 0x005706f0 set_pixel_format_5706f0
std::int32_t set_pixel_format_5706f0(std::int32_t a0);
/// 0x0060bf40 matrix_rotation_ypr
inline void* matrix_rotation_ypr(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) { return ::joan::call::cdecl_call<void*>(::joan::fn::init_rotation_actor_hvr, a0, a1, a2, a3); }
/// 0x005aaaf0 call_hook_a67638
void call_hook_a67638(const char* a0);
/// 0x0060d990 vec_cross
void* vec_cross(void* a0, void* a1, void* a2);
/// 0x0060da30 plane_distance
float plane_distance(void* a0, void* a1);
/// 0x007b6c54 malloc
void* malloc_(std::uint32_t a0);
/// 0x00449af0 vector
void vector(void* a0, std::int32_t a1, std::int32_t a2, void* a3);
/// 0x0046f020 min_int
std::int32_t min_int(std::int32_t a0, std::int32_t a1);
/// 0x005711b7 asm_get_a15020
std::uint32_t asm_get_a15020();
/// 0x0060ce20 vec3_sub_assign
void* vec3_sub_assign(void* a0, void* a1);
/// 0x00664d70 point_segment_dist2
float point_segment_dist2(float a0, float a1, float a2, float a3, float a4, float a5);
/// 0x006a2b80 obb_to_box_frame
void* obb_to_box_frame(void* a0, void* a1, void* a2);
/// 0x005a6660 format_positional
template <class... A>
inline const char* format_positional(const char* a0, A... rest) { return ::joan::call::cdecl_call<const char*>(::joan::fn::format_positional, a0, rest...); }
/// 0x004893c0 int_max
std::int32_t int_max(std::int32_t a0, std::int32_t a1);
/// 0x006a0d80 sphere_sweep
std::uint8_t sphere_sweep(void* a0, void* a1, void* a2, void* a3);
/// 0x0060c180 matrix_axis_x
void* matrix_axis_x(void* a0, void* a1);
/// 0x0065e060 format_into
template <class... A>
inline std::uint32_t format_into(void* a0, const char* a1, A... rest) { return ::joan::call::cdecl_call<std::uint32_t>(::joan::fn::format_into, a0, a1, rest...); }
/// 0x0062c200 clamp_int
std::int32_t clamp_int(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x0060dc80 box_plane_extent
void box_plane_extent(void* a0, void* a1, void* a2, void* a3);
/// 0x0060cf40 vector_differs
std::uint8_t vector_differs(void* a0, void* a1);
/// 0x0065b9b0 unit_refresh_items
std::int32_t unit_refresh_items(std::int16_t a0);
/// 0x00527dd0 unit_class
std::int32_t unit_class(std::int16_t a0);
/// 0x0060d9e0 plane_normal
void* plane_normal(void* a0, void* a1);
/// 0x006618f0 group_release
std::int32_t group_release(std::int32_t a0);
/// 0x005a2bf0 format_int
const char* format_int(std::int32_t a0, std::int32_t a1);
/// 0x00694fc0 box_sweep_planes_694fc0
template <class T1, class T2, class T3>
inline std::uint8_t box_sweep_planes_694fc0(void* a0, T1 a1, T2 a2, T3 a3, std::uint8_t a4) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::box_sweep_planes_694fc0, a0, a1, a2, a3, a4); }
/// 0x00496d40 aim_velocity
template <class T1, class T2>
inline void* aim_velocity(void* a0, T1 a1, T2 a2, float a3, float a4, std::uint32_t a5, float a6, float a7, float a8) { return ::joan::call::cdecl_call<void*>(::joan::fn::aim_velocity, a0, a1, a2, a3, a4, a5, a6, a7, a8); }
/// 0x0057b030 wav_find_data
void* wav_find_data(void* a0);
/// 0x00655420 unit_reset_655420
std::int32_t unit_reset_655420(std::uint32_t a0);
/// 0x005d2aa0 triangle_kind
std::int32_t triangle_kind(void* a0, std::int32_t a1);
/// 0x00697140 sweep_sphere_box_697140
bool sweep_sphere_box_697140(void* a0, void* a1, void* a2, void* a3);
/// 0x00695d90 sweep_capsule_box
std::uint8_t sweep_capsule_box(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, float a12, float a13, float a14);
/// 0x0066dd30 script_random_range
std::int32_t script_random_range(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x0060c1d0 matrix_row1
void* matrix_row1(void* a0, void* a1);
/// 0x005befa0 mat_identity
void* mat_identity(void* a0);
/// 0x0054ef40 get_341
inline void* get_341(void* a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::get_341, a0); }
/// 0x006aae70 find_zone_pair_6aae70
std::uint32_t find_zone_pair_6aae70(void* a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, void* a7, void* a8, std::uint32_t a9);
/// 0x00600080 d3d_format
std::uint32_t d3d_format(std::uint32_t a0);
/// 0x00664c60 tri_normal_2d
std::int32_t tri_normal_2d(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, void* a12, void* a13, void* a14);
/// 0x0058e380 text_toupper
std::int32_t text_toupper(std::int32_t a0);
/// 0x004b08f0 table_a016a0
void* table_a016a0(std::int32_t a0);
/// 0x004d14f0 str_token
const char* str_token(const char* a0, char a1);
/// 0x0069d5d0 sphere_box
template <class T0, class T1>
inline std::uint8_t sphere_box(T0 a0, T1 a1) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::sphere_box, a0, a1); }
/// 0x006511f0 script_rgb_component
std::uint32_t script_rgb_component(const char* a0, std::int32_t a1);
/// 0x0045cec0 score_name
const char* score_name(std::int32_t a0);
/// 0x00598700 same16
std::int32_t same16(void* a0, void* a1);
/// 0x00600b60 primitive_type
std::uint32_t primitive_type(std::int32_t a0);
/// 0x0068d690 outcode
std::int32_t outcode(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8);
/// 0x0065b700 object_is_type1
std::int32_t object_is_type1(std::int32_t a0);
/// 0x006a3040 obb_transform
void* obb_transform(void* a0, void* a1);
/// 0x0060bca0 matrix_rotation_axis
void* matrix_rotation_axis(void* a0, void* a1, float a2);
/// 0x0060e590 line_plane
std::int32_t line_plane(void* a0, void* a1, void* a2);
/// 0x007aa310 jpeg add_huff_table
void jpeg(void* a0, void* a1, void* a2, void* a3);
/// 0x0054f530 get_b2
inline void* get_b2(void* a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::get_b2, a0); }
/// 0x006581f0 find_unit_in_groups
std::int32_t find_unit_in_groups(std::int16_t a0);
/// 0x0069d210 capsule_sphere
template <class T0, class T1>
inline std::uint8_t capsule_sphere(T0 a0, T1 a1) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::cylinder_sphere_intersection, a0, a1); }
/// 0x0069d390 capsule_box
template <class T0, class T1>
inline std::uint8_t capsule_box(T0 a0, T1 a1) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::cylinder_box_intersection_69d390, a0, a1); }
/// 0x005beef0 vec_cross
void* vec_cross_5beef0(void* a0, void* a1, void* a2);
/// 0x00653c60 unit_state_is_42
std::int32_t unit_state_is_42(std::int32_t a0);
/// 0x00652c10 unit_check_652c10
std::int32_t unit_check_652c10(std::int16_t a0);
/// 0x005a27d0 text_buffer
const char* text_buffer(const char* a0);
/// 0x0069d540 sphere_sphere
template <class T0, class T1>
inline std::uint8_t sphere_sphere(T0 a0, T1 a1) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::sphere_sphere_intersection, a0, a1); }
/// 0x0060c360 set_30
void* set_30(void* a0, void* a1);
/// 0x0065d0d0 script_set_unit_light
std::int32_t script_set_unit_light(std::int32_t a0, std::int32_t a1);
/// 0x00651010 script_random
std::int32_t script_random(std::int32_t a0);
/// 0x0065f600 script_flag
std::int32_t script_flag(std::int32_t a0);
/// 0x0069e640 ray_plane
template <class T1, class T2, class T3>
inline bool ray_plane(void* a0, T1 a1, T2 a2, T3 a3) { return ::joan::call::cdecl_call<bool>(::joan::fn::ray_plane, a0, a1, a2, a3); }
/// 0x00699a20 ray_hits_cylinder_699a20
template <class T1, class T2, class T3>
inline std::uint8_t ray_hits_cylinder_699a20(void* a0, T1 a1, T2 a2, T3 a3, float a4, float a5) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::sphere_cylinder_collision, a0, a1, a2, a3, a4, a5); }
/// 0x00663ad0 point_rect_dist2
std::int32_t point_rect_dist2(std::int32_t a0, std::int32_t a1, void* a2, void* a3);
/// 0x0065b760 object_is_type4
std::int32_t object_is_type4(std::int32_t a0);
/// 0x006a30c0 obb_from_box
void* obb_from_box(void* a0, void* a1, void* a2);
/// 0x005921a0 muldiv_signed
std::int32_t muldiv_signed(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x00592170 mul_div
std::int32_t mul_div(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x0060bc00 matrix_rotation
void* matrix_rotation_60bc00(void* a0, float a1);
/// 0x0060c270 matrix_copy3x3
void* matrix_copy3x3(void* a0, void* a1);
/// 0x0060c3b0 matrix_axis_angle
void* matrix_axis_angle(void* a0, void* a1, void* a2);
/// 0x00664b70 line_intersect_terms
std::uint32_t line_intersect_terms(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, void* a8, void* a9, void* a10);
/// 0x00600290 index_format
std::uint32_t index_format(void* a0);
/// 0x0054ee70 get_33d
inline void* get_33d(void* a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::get_33d, a0); }
/// 0x005a2d70 format_roman
const char* format_roman(std::int32_t a0);
/// 0x005a29a0 format_double
const char* format_double(std::uint32_t a0, std::uint32_t a1, std::int32_t a2);
/// 0x00634ae0 fmin
float fmin(float a0, float a1);
/// 0x00634aa0 fmax
float fmax(float a0, float a1);
/// 0x005919a0 fill_except
template <class... A>
inline void fill_except(void* a0, const char* a1, std::int32_t a2, std::uint32_t a3, std::int32_t a4, A... rest) { ::joan::call::cdecl_call<void>(::joan::fn::fill_except, a0, a1, a2, a3, a4, rest...); }
/// 0x0069d160 cylinders_overlap
std::uint8_t cylinders_overlap(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9);
/// 0x00423220 bone_id_of_name
std::int32_t bone_id_of_name(const char* a0);
/// 0x0060e6a0 z_plane_hit
std::int32_t z_plane_hit(void* a0, void* a1, float a2);
/// 0x0065b570 units_lookup
std::int32_t units_lookup(const char* a0);
/// 0x0065c050 unit_set_mood
std::int32_t unit_set_mood(std::int32_t a0, const char* a1);
/// 0x0065daf0 unit_order_65daf0
std::int32_t unit_order_65daf0(std::int32_t a0, std::uint32_t a1);
/// 0x00652820 unit_has_enemy_near
std::int32_t unit_has_enemy_near(std::int16_t a0);
/// 0x00494dc0 trigger_type_from_name
std::int32_t trigger_type_from_name(const char* a0);
/// 0x0069e970 sphere_sweep_plane
std::uint8_t sphere_sweep_plane(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11);
/// 0x006a09e0 sphere_sweep_cylinder
std::uint8_t sphere_sweep_cylinder(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, std::int32_t a11);
/// 0x0065ae60 spawn_fx_65ae60
std::int32_t spawn_fx_65ae60(std::int8_t a0, std::int8_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::int8_t a8);
/// 0x005eb470 sort_sectors
void sort_sectors(void* a0, std::uint32_t a1, void* a2);
/// 0x006a1460 solve_quartic_6a1460
double solve_quartic_6a1460(float a0, float a1, float a2, float a3, float a4, float a5, float a6);
/// 0x00652f80 script_unit_value
std::int32_t script_unit_value(std::int16_t a0, std::int32_t a1);
/// 0x00657670 script_unit_take_slot
std::int32_t script_unit_take_slot(std::int32_t a0, std::int32_t a1, const char* a2, std::int32_t a3);
/// 0x00657cc0 script_unit_release_slots
std::int32_t script_unit_release_slots(std::int16_t a0, std::int32_t a1);
/// 0x00653ca0 script_unit_in_zone
std::int32_t script_unit_in_zone(std::int32_t a0, std::int32_t a1);
/// 0x0065f0f0 script_unit_heading_to
std::int32_t script_unit_heading_to(std::int32_t a0, std::int32_t a1);
/// 0x00652b00 script_unit_busy
std::int32_t script_unit_busy(std::int32_t a0);
/// 0x0065cb20 script_unit_65cb20
std::uint32_t script_unit_65cb20(std::uint32_t a0, std::int32_t a1, std::uint32_t a2);
/// 0x00657610 script_unit_508b90
std::int32_t script_unit_508b90(std::int16_t a0);
/// 0x00656b50 script_unit_38c
std::int32_t script_unit_38c(std::int16_t a0);
/// 0x0065dfa0 script_unit_388
std::int32_t script_unit_388(std::int32_t a0);
/// 0x00656af0 script_unit_384
std::int32_t script_unit_384(std::int16_t a0);
/// 0x0065ddb0 script_target_hardpoint_dist
double script_target_hardpoint_dist(std::int32_t a0);
/// 0x0065af50 script_state_busy
std::int32_t script_state_busy();
/// 0x0065c530 script_post_event_65c530
std::int32_t script_post_event_65c530(std::uint32_t a0, std::int32_t a1);
/// 0x00657370 script_hardpoint_z
std::int32_t script_hardpoint_z(std::int32_t a0, std::int32_t a1);
/// 0x00657240 script_hardpoint_y
std::int32_t script_hardpoint_y(std::int32_t a0, std::int32_t a1);
/// 0x00657110 script_hardpoint_x
std::int32_t script_hardpoint_x(std::int32_t a0, std::int32_t a1);
/// 0x0047cf60 scan_dir
std::int32_t scan_dir(const char* a0, std::int32_t a1);
/// 0x0060e810 ray_triangle
std::int32_t ray_triangle(void* a0, void* a1, void* a2, void* a3, void* a4);
/// 0x0066fc50 rational_compare
std::int32_t rational_compare(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x006aac70 point_box_dist2
double point_box_dist2(float a0, float a1, float a2, void* a3);
/// 0x0066ddb0 path_tree_accumulate
void path_tree_accumulate(void* a0, std::int32_t a1, std::int32_t a2);
/// 0x007c1369 parse_cmdline
void* parse_cmdline(const char* a0, void* a1, const char* a2, void* a3, void* a4);
/// 0x007b1d2a operator delete
void operator_(void* a0);
/// 0x006524a0 object_d1
std::int32_t object_d1(std::int32_t a0);
/// 0x0069e4e0 obb_box
template <class T0, class T1>
inline std::uint8_t obb_box(T0 a0, T1 a1) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::obb_box, a0, a1); }
/// 0x0065cea0 message_65cea0
std::uint32_t message_65cea0(const char* a0, std::uint32_t a1);
/// 0x0065aef0 menu_text_65aef0
std::int32_t menu_text_65aef0(char a0, char a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, char a6);
/// 0x0060bb50 matrix_translation
void* matrix_translation(void* a0, void* a1);
/// 0x0060bbb0 matrix_from_quat
void* matrix_from_quat(void* a0, void* a1);
/// 0x0066fca0 key_compare
std::int32_t key_compare(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5);
/// 0x007a9b30 jpeg_destroy_compress
void jpeg_destroy_compress(void* a0);
/// 0x007aa020 jpeg_add_quant_table
void jpeg_add_quant_table(void* a0, std::int32_t a1, void* a2, std::int32_t a3, std::uint32_t a4);
/// 0x007a9a60 jpeg_CreateCompress
void jpeg_create_compress(void* a0, std::int32_t a1, std::uint32_t a2);
/// 0x0043fc10 grow_array
void* grow_array(void* a0, void* a1, std::int16_t a2);
/// 0x0066dd70 grid_590410
std::uint32_t grid_590410(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x007b7ed4 _getenv_lk
const char* _getenv_lk(const char* a0);
/// 0x0054f980 get_42
inline void* get_42(void* a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::get_42, a0); }
/// 0x005f0020 get24
void* get24(void* a0);
/// 0x0065b0e0 game_state_is_7
std::int32_t game_state_is_7();
/// 0x0065b130 game_flag_744
std::int32_t game_flag_744();
/// 0x005ff920 format_bits
std::int32_t format_bits(std::int32_t a0);
/// 0x00587f60 delete_file
std::int32_t delete_file(const char* a0);
/// 0x0068cb50 clip_line
std::uint32_t clip_line(void* a0, void* a1, void* a2, void* a3, float a4, float a5, float a6, float a7);
/// 0x007b7b87 calloc
void* calloc(std::uint32_t a0, std::uint32_t a1);
/// 0x0060da80 box_plane_distance
float box_plane_distance(void* a0, void* a1, void* a2);
/// 0x00416aa0 anim_kind_from_name
std::int32_t anim_kind_from_name(const char* a0);
/// 0x007a9989 alutUnloadWAV
void alut_unload_wav(std::int32_t a0, void* a1, std::int32_t a2, std::int32_t a3);
/// 0x005a9b90 Rec::set_buffers
std::uint32_t set_buffers(void* a0, void* a1, void* a2);
/// 0x005a99e0 Rec::export
std::uint32_t export_(void* a0, void* a1);
/// 0x0056fba0 fill_rect
void fill_rect(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6);
/// 0x0056fdb0 build_shade_tables
std::uint32_t build_shade_tables(std::int32_t a0);
/// 0x005744f0 blit8to16_key
void blit8to16_key(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5);
/// 0x0054eb20 world_entry
inline void* world_entry(std::int32_t a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::world_entry, a0); }
/// 0x007c11bf _wincmdln
const char* _wincmdln();
/// 0x004d7f80 video_copy_frame
std::uint32_t video_copy_frame();
/// 0x004d9a50 update_third
void update_third();
/// 0x0065dcf0 units_sweep_65dcf0
std::int32_t units_sweep_65dcf0(std::int32_t a0);
/// 0x0065dc20 units_order_kind_65dc20
std::int32_t units_order_kind_65dc20(std::int32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00650c10 unit_value_c6
std::uint32_t unit_value_c6(std::uint32_t a0);
/// 0x00652280 unit_value_2ba
std::uint32_t unit_value_2ba(std::int16_t a0);
/// 0x00650b90 unit_target
std::uint32_t unit_target(std::uint32_t a0);
/// 0x00650e70 unit_say
std::int32_t unit_say(std::uint32_t a0, std::uint32_t a1);
/// 0x0065c5f0 unit_order_65c5f0
std::uint32_t unit_order_65c5f0(const char* a0, std::int32_t a1, std::int32_t a2, std::uint32_t a3);
/// 0x0065dba0 unit_kick_65dba0
std::int32_t unit_kick_65dba0(std::int32_t a0);
/// 0x00652a40 unit_check_652a40
std::int32_t unit_check_652a40(std::uint32_t a0);
/// 0x00654020 unit_call_654020
std::uint32_t unit_call_654020(std::uint32_t a0, std::uint32_t a1);
/// 0x00652660 unit_byte_d0
std::int32_t unit_byte_d0(std::uint32_t a0);
/// 0x005a28e0 txt_alloc
void* txt_alloc(std::int32_t a0);
/// 0x0058e330 to_lower
std::int32_t to_lower(std::int32_t a0);
/// 0x0069e720 sweep_hit_time_69e720
std::uint8_t sweep_hit_time_69e720(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11);
/// 0x007c5afa _strdup
const char* _strdup(const char* a0);
/// 0x00685910 str_equal
std::int32_t str_equal(const char* a0, const char* a1);
/// 0x004168a0 static_dtor_4168a0
std::uint32_t static_dtor_4168a0();
/// 0x00425cb0 state_same
std::int32_t state_same(void* a0, void* a1);
/// 0x0041a900 state_differs
std::int32_t state_differs(void* a0, void* a1);
/// 0x00651c90 spawn_named
std::uint32_t spawn_named(std::uint32_t a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::int32_t a5, std::uint32_t a6);
/// 0x004945c0 spawn_effect_kind
template <class T1, class T2>
inline std::uint32_t spawn_effect_kind(void* a0, T1 a1, T2 a2, std::uint32_t a3) { return ::joan::call::cdecl_call<std::uint32_t>(::joan::fn::spawn_effect_kind, a0, a1, a2, a3); }
/// 0x006a1250 solve2x2
std::uint8_t solve2x2(void* a0, void* a1, float a2, float a3, float a4, float a5, float a6, float a7);
/// 0x006a3d10 shapes_overlap
std::uint32_t shapes_overlap(std::int32_t a0, void* a1, std::int32_t a2, void* a3);
/// 0x006a31d0 shapes_overlap
std::uint8_t shapes_overlap_6a31d0(void* a0, void* a1);
/// 0x007c1217 _setenvp
void _setenvp();
/// 0x007c12d0 _setargv
std::int32_t _setargv();
/// 0x004eb410 set_quarter
void set_quarter();
/// 0x007b4c45 _set_new_handler
inline void* _set_new_handler(void* a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::set_new_handler, a0); }
/// 0x005f45c0 set_a69ad0
void set_a69ad0();
/// 0x005aaad0 set_a67638
std::uint32_t set_a67638(std::uint32_t a0);
/// 0x0066f820 separate_circles
std::int32_t separate_circles(void* a0, void* a1, void* a2, void* a3, float a4);
/// 0x004b0930 select_preset
void select_preset(std::int32_t a0);
/// 0x0060df10 segment_closest_t
float segment_closest_t(void* a0, void* a1);
/// 0x00658420 script_units_set_group
std::int32_t script_units_set_group(std::int32_t a0, const char* a1);
/// 0x00658e10 script_units_set_2f
std::int32_t script_units_set_2f(std::int32_t a0, std::int32_t a1);
/// 0x00658d90 script_units_clear_1c5
std::int32_t script_units_clear_1c5(std::int32_t a0);
/// 0x00655a40 script_unit_unlink_1f5
std::int32_t script_unit_unlink_1f5(std::int16_t a0);
/// 0x00659400 script_unit_unit_dist
std::int32_t script_unit_unit_dist(std::int32_t a0, std::int32_t a1);
/// 0x006584e0 script_unit_spawn_members
std::int32_t script_unit_spawn_members(std::int32_t a0, const char* a1, std::int32_t a2);
/// 0x0065c2e0 script_unit_set_type
std::int32_t script_unit_set_type(std::int32_t a0, std::int32_t a1);
/// 0x00656350 script_unit_set_name
std::int32_t script_unit_set_name(std::int32_t a0, const char* a1);
/// 0x00655590 script_unit_set_heading
inline std::int32_t script_unit_set_heading(std::int16_t a0, std::int32_t a1) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::script_unit_set_heading, a0, a1); }
/// 0x00652de0 script_unit_set_52
std::int32_t script_unit_set_52(std::int16_t a0, std::int8_t a1);
/// 0x00655620 script_unit_set_50abd0
std::int32_t script_unit_set_50abd0(std::int16_t a0, std::int32_t a1);
/// 0x0065f9e0 script_unit_set_3c1
std::int32_t script_unit_set_3c1(std::int32_t a0, std::int32_t a1);
/// 0x0065f980 script_unit_set_3c1
std::int32_t script_unit_set_3c1_65f980(std::int32_t a0, std::int32_t a1);
/// 0x0065e9a0 script_unit_set_3c0
std::int32_t script_unit_set_3c0(std::int32_t a0, std::int32_t a1);
/// 0x0065fb50 script_unit_set_30
std::int32_t script_unit_set_30(std::int32_t a0, std::int8_t a1);
/// 0x00655e90 script_unit_set_2eb
std::int32_t script_unit_set_2eb(std::int16_t a0, std::int16_t a1);
/// 0x00653640 script_unit_set_2e
std::int32_t script_unit_set_2e(std::int16_t a0, std::int8_t a1);
/// 0x00653590 script_unit_set_2e
std::int32_t script_unit_set_2e_653590(std::int16_t a0, std::int8_t a1);
/// 0x00655e30 script_unit_set_2e9
std::int32_t script_unit_set_2e9(std::int32_t a0, std::int32_t a1);
/// 0x00655d80 script_unit_set_2e7
std::int32_t script_unit_set_2e7(std::int32_t a0, std::int32_t a1);
/// 0x00655c20 script_unit_set_2e3
std::int32_t script_unit_set_2e3(std::int16_t a0, std::int16_t a1);
/// 0x006532a0 script_unit_set_1c7
std::int32_t script_unit_set_1c7(std::int32_t a0, std::int32_t a1);
/// 0x00653210 script_unit_set_1c5
std::int32_t script_unit_set_1c5(std::int32_t a0, std::int32_t a1);
/// 0x006562b0 script_unit_set_15f
std::int32_t script_unit_set_15f(std::int32_t a0, std::int16_t a1);
/// 0x00656990 script_unit_set_135
std::int32_t script_unit_set_135(std::int32_t a0, std::int32_t a1);
/// 0x00656210 script_unit_set_114
std::int32_t script_unit_set_114(std::int32_t a0, std::int32_t a1);
/// 0x006554f0 script_unit_place
inline std::int32_t script_unit_place(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::script_unit_place, a0, a1, a2, a3); }
/// 0x006549c0 script_unit_object_dist
std::int32_t script_unit_object_dist(std::int32_t a0, std::int32_t a1);
/// 0x0065c930 script_unit_name_check
std::int32_t script_unit_name_check(std::int32_t a0, const char* a1);
/// 0x006563b0 script_unit_name
std::uint32_t script_unit_name(void* a0, std::int32_t a1);
/// 0x00661ca0 script_unit_list_remove
std::uint32_t script_unit_list_remove(std::int32_t a0, std::int32_t a1);
/// 0x006523c0 script_unit_kill_type
std::int32_t script_unit_kill_type(std::int16_t a0);
/// 0x00656a60 script_unit_key_alive
std::int32_t script_unit_key_alive(std::int32_t a0);
/// 0x00655680 script_unit_heading_deg
std::int32_t script_unit_heading_deg(std::int16_t a0);
/// 0x00658160 script_unit_group_size
std::int32_t script_unit_group_size(std::int16_t a0);
/// 0x00658810 script_unit_group_move
std::int32_t script_unit_group_move(std::int16_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, float a4);
/// 0x006574a0 script_unit_group_flag_2f
std::int32_t script_unit_group_flag_2f(std::int16_t a0, std::int32_t a1);
/// 0x00653360 script_unit_group_entry
std::uint32_t script_unit_group_entry(std::int16_t a0, const char* a1);
/// 0x00658650 script_unit_group_call_41fcc0
std::int32_t script_unit_group_call_41fcc0(std::int16_t a0);
/// 0x00658730 script_unit_group_442820
std::int32_t script_unit_group_442820(std::int16_t a0);
/// 0x006526e0 script_unit_facing_check
std::int32_t script_unit_facing_check(std::int16_t a0, std::int32_t a1);
/// 0x0065cb80 script_unit_count_allies
std::int32_t script_unit_count_allies(std::int32_t a0);
/// 0x0065cac0 script_unit_call_510170
std::uint32_t script_unit_call_510170(std::int32_t a0, std::int32_t a1);
/// 0x0065c870 script_unit_call_510030
std::int32_t script_unit_call_510030(std::int32_t a0, std::int32_t a1);
/// 0x00655870 script_unit_call_442820
std::int32_t script_unit_call_442820(std::int32_t a0);
/// 0x00658ea0 script_unit_attach
std::int32_t script_unit_attach(std::int16_t a0, std::int16_t a1, const char* a2, std::int8_t a3);
/// 0x0065d350 script_unit_apply_group
std::int32_t script_unit_apply_group(std::int32_t a0, std::int32_t a1);
/// 0x0065d450 script_unit_65d450
std::uint32_t script_unit_65d450(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x0065cfd0 script_unit_65cfd0
std::uint32_t script_unit_65cfd0(std::int32_t a0, std::int32_t a1);
/// 0x0065ca40 script_unit_65ca40
std::uint32_t script_unit_65ca40(const char* a0, std::int32_t a1);
/// 0x00658ad0 script_unit_658ad0
std::uint32_t script_unit_658ad0(std::uint32_t a0);
/// 0x006537a0 script_unit_6537a0
std::uint32_t script_unit_6537a0(std::int16_t a0, const char* a1, std::int32_t a2, std::uint32_t a3, std::int16_t a4, std::int16_t a5);
/// 0x0065c250 script_unit_523190
std::int32_t script_unit_523190(std::int32_t a0, std::int32_t a1);
/// 0x006559e0 script_unit_51a250
std::int32_t script_unit_51a250(std::int16_t a0, std::int16_t a1);
/// 0x00655930 script_unit_4fc220
std::int32_t script_unit_4fc220(std::int16_t a0, std::int16_t a1);
/// 0x00653300 script_unit_427b50
std::int32_t script_unit_427b50(std::int16_t a0, const char* a1);
/// 0x00655810 script_unit_41fcc0
std::int32_t script_unit_41fcc0(std::int32_t a0);
/// 0x0065d850 script_unit_3c9
std::int32_t script_unit_3c9(std::int32_t a0);
/// 0x0065b950 script_unit_374
std::int32_t script_unit_374(std::int16_t a0);
/// 0x006575c0 script_unit_2f_zero
std::int32_t script_unit_2f_zero(std::int16_t a0);
/// 0x00655f10 script_unit_2eb
std::int32_t script_unit_2eb(std::int16_t a0);
/// 0x00655de0 script_unit_2e9
std::int32_t script_unit_2e9(std::int16_t a0);
/// 0x00655d30 script_unit_2e7
std::int32_t script_unit_2e7(std::int16_t a0);
/// 0x00655c80 script_unit_2e5
std::int32_t script_unit_2e5(std::int16_t a0);
/// 0x00655bd0 script_unit_2e3
std::int32_t script_unit_2e3(std::int32_t a0);
/// 0x00651260 script_unit_2a1_1d
std::int32_t script_unit_2a1_1d(std::uint32_t a0);
/// 0x00655990 script_unit_1e2
std::int32_t script_unit_1e2(std::int16_t a0);
/// 0x0065c3a0 script_unit_1c1_word2
std::int32_t script_unit_1c1_word2(std::int32_t a0);
/// 0x0065c0d0 script_unit_1ae
std::int32_t script_unit_1ae(std::int16_t a0);
/// 0x00656300 script_unit_15f
std::int32_t script_unit_15f(std::int32_t a0);
/// 0x0065b500 script_unit_135_pct
std::int32_t script_unit_135_pct(std::int32_t a0);
/// 0x00656940 script_unit_135
std::int32_t script_unit_135(std::int32_t a0);
/// 0x0065c1a0 script_unit_124
std::int32_t script_unit_124(std::int32_t a0);
/// 0x00656260 script_unit_114
std::int32_t script_unit_114(std::int32_t a0);
/// 0x0065b190 script_type_activate
std::int32_t script_type_activate(const char* a0);
/// 0x0065b160 script_type_5270b0
std::uint32_t script_type_5270b0(const char* a0);
/// 0x0065b200 script_type_51fd50
std::uint32_t script_type_51fd50(const char* a0);
/// 0x00653930 script_toggle_a56398
std::int32_t script_toggle_a56398();
/// 0x0065c9b0 script_time_since
std::int32_t script_time_since();
/// 0x0065a7e0 script_table_word_aa
std::int32_t script_table_word_aa(std::int32_t a0);
/// 0x0065a670 script_table_word
std::int32_t script_table_word(std::int32_t a0);
/// 0x0065a6a0 script_table_set_word
std::int32_t script_table_set_word(std::int32_t a0, std::int32_t a1);
/// 0x0065a390 script_table_set_byte
std::int32_t script_table_set_byte(std::int32_t a0, std::int32_t a1);
/// 0x0065a810 script_table_set
std::int32_t script_table_set(std::int32_t a0, std::int32_t a1);
/// 0x0065aca0 script_store_rect
std::int32_t script_store_rect();
/// 0x0065ed70 script_state_is_4
std::int32_t script_state_is_4();
/// 0x0065a010 script_stat_table
std::int32_t script_stat_table(const char* a0, std::int32_t a1);
/// 0x00654830 script_squad_set_rates
std::int32_t script_squad_set_rates(std::int32_t a0, std::int32_t a1);
/// 0x006541a0 script_squad_1ca
std::int32_t script_squad_1ca(std::int32_t a0);
/// 0x00655150 script_spawn_unit
inline std::int32_t script_spawn_unit(const char* a0, const char* a1) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::script_spawn_unit, a0, a1); }
/// 0x0065da50 script_spawn_effect
std::int32_t script_spawn_effect(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x00652440 script_slot1_d1
std::int32_t script_slot1_d1();
/// 0x0065eda0 script_set_word_a4f3ac
std::int32_t script_set_word_a4f3ac(std::int16_t a0);
/// 0x00652010 script_set_type
std::int32_t script_set_type(std::uint32_t a0, const char* a1);
/// 0x00659f40 script_set_stat
std::int32_t script_set_stat(const char* a0, std::int32_t a1, std::int32_t a2);
/// 0x00659610 script_set_range_pair
std::int32_t script_set_range_pair(std::int32_t a0);
/// 0x006594e0 script_set_outcome
std::int32_t script_set_outcome(const char* a0);
/// 0x0065adb0 script_set_flag_a501da
std::int32_t script_set_flag_a501da(std::int32_t a0);
/// 0x00650f40 script_set_e6
std::int32_t script_set_e6(std::int32_t a0, std::uint32_t a1);
/// 0x00650c90 script_set_c2
std::int32_t script_set_c2(std::int32_t a0, std::uint32_t a1);
/// 0x0065fc90 script_set_byte_a5d386
std::int32_t script_set_byte_a5d386(std::uint8_t a0);
/// 0x0065f670 script_set_byte_a5ba64
std::int32_t script_set_byte_a5ba64(std::uint8_t a0);
/// 0x0065a960 script_set_bit
std::int32_t script_set_bit(std::int32_t a0, std::int32_t a1);
/// 0x0065c4a0 script_set_a6a254
std::int32_t script_set_a6a254(std::int32_t a0);
/// 0x0065fba0 script_set_a596c8
std::int32_t script_set_a596c8(std::int32_t a0);
/// 0x0065c500 script_set_a596ab
std::int32_t script_set_a596ab(std::int32_t a0);
/// 0x00659a90 script_set_a591d8
std::int32_t script_set_a591d8(std::int32_t a0);
/// 0x00659a30 script_set_a591d4
std::int32_t script_set_a591d4(std::int32_t a0);
/// 0x00659a00 script_set_a591d0
std::int32_t script_set_a591d0(std::int32_t a0);
/// 0x006598c0 script_set_a591cc
std::int32_t script_set_a591cc(std::int32_t a0);
/// 0x00659ce0 script_set_a591c6
std::int32_t script_set_a591c6(std::int8_t a0);
/// 0x00659d10 script_set_a591ba
std::int32_t script_set_a591ba(std::int32_t a0);
/// 0x00659830 script_set_a58faf
std::int32_t script_set_a58faf(std::int32_t a0);
/// 0x0065e880 script_set_a50526
std::int32_t script_set_a50526(std::int32_t a0);
/// 0x0065e7d0 script_set_a50522
std::int32_t script_set_a50522(std::int32_t a0);
/// 0x00651100 script_set_a50298
std::int32_t script_set_a50298(std::int16_t a0);
/// 0x006539a0 script_set_a5027e
std::int32_t script_set_a5027e(std::int32_t a0);
/// 0x00659890 script_set_a48cdc
std::int32_t script_set_a48cdc(std::int32_t a0);
/// 0x0065ebd0 script_set_6b8260
std::int32_t script_set_6b8260(const char* a0);
/// 0x0065aa20 script_set_19a
std::int32_t script_set_19a(std::int32_t a0);
/// 0x006619d0 script_send_order
std::int32_t script_send_order(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6);
/// 0x00658f70 script_select_unit
std::int32_t script_select_unit(std::int16_t a0, std::int16_t a1);
/// 0x00653a90 script_reset_first_unit
std::int32_t script_reset_first_unit();
/// 0x00651ea0 script_remove_objects
std::int32_t script_remove_objects(const char* a0);
/// 0x00659080 script_release_group
std::int32_t script_release_group(std::int32_t a0);
/// 0x00654b30 script_place_sum
std::int32_t script_place_sum(std::int16_t a0);
/// 0x00654d70 script_place_setup
std::int32_t script_place_setup(std::int16_t a0, std::int32_t a1, std::int8_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8);
/// 0x00654c20 script_place_set_98
std::int32_t script_place_set_98(std::int32_t a0, std::int32_t a1);
/// 0x00654bc0 script_place_set_97
std::int32_t script_place_set_97(std::int16_t a0, std::int8_t a1);
/// 0x00654ae0 script_place_set_13c
std::int32_t script_place_set_13c(std::int32_t a0, std::int32_t a1);
/// 0x0065d9c0 script_place_effect
std::int32_t script_place_effect(const char* a0, std::int16_t a1, std::int16_t a2);
/// 0x00654f10 script_place_654f10
std::uint32_t script_place_654f10(std::int16_t a0, std::int16_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x00654e30 script_place_654e30
std::uint32_t script_place_654e30(std::int16_t a0, std::uint32_t a1);
/// 0x00654c80 script_place_654c80
std::uint32_t script_place_654c80(std::int16_t a0, std::int16_t a1);
/// 0x00654960 script_place_467ea0
std::int32_t script_place_467ea0(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::int32_t a4);
/// 0x006548f0 script_place_467dd0
std::int32_t script_place_467dd0(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int32_t a3, const char* a4, const char* a5);
/// 0x00653890 script_open_buy
std::int32_t script_open_buy(std::int16_t a0, std::int16_t a1);
/// 0x0065a4a0 script_objective_state
std::uint32_t script_objective_state(std::int32_t a0, const char* a1);
/// 0x0065a720 script_objective_set
std::int32_t script_objective_set(std::int32_t a0, std::int32_t a1);
/// 0x00651fa0 script_object_set_heading
std::int32_t script_object_set_heading(std::int16_t a0, std::int32_t a1);
/// 0x006525f0 script_object_set_cf
std::int32_t script_object_set_cf(std::int32_t a0, std::int32_t a1);
/// 0x00650cf0 script_object_set_c6
std::int32_t script_object_set_c6(std::int32_t a0, std::int32_t a1);
/// 0x0065e930 script_object_set_7e
std::int32_t script_object_set_7e(std::int32_t a0, std::int32_t a1);
/// 0x0065b630 script_object_name_differs
std::int32_t script_object_name_differs(std::int32_t a0, const char* a1);
/// 0x0065eb50 script_object_height
float script_object_height(std::int32_t a0);
/// 0x0065ea50 script_object_height
float script_object_height_65ea50(std::int32_t a0);
/// 0x0065ea00 script_object_a2
std::int32_t script_object_a2(std::int32_t a0);
/// 0x0065dff0 script_object_65dff0
std::uint32_t script_object_65dff0(std::uint32_t a0, std::uint32_t a1);
/// 0x0065b5b0 script_object_49f4b0
std::uint32_t script_object_49f4b0(const char* a0);
/// 0x00652520 script_obj_set_ce
std::int32_t script_obj_set_ce(std::int32_t a0, std::int8_t a1);
/// 0x00650d50 script_obj_c6
std::uint32_t script_obj_c6(std::int32_t a0);
/// 0x0065eea0 script_not_a53cb0
std::int32_t script_not_a53cb0();
/// 0x00653c20 script_not_a4fc98_flag
std::int32_t script_not_a4fc98_flag();
/// 0x0065f950 script_not_a46040
std::int32_t script_not_a46040();
/// 0x0065fbf0 script_name_unused
std::int32_t script_name_unused(const char* a0);
/// 0x0065cf80 script_name_set_19
std::uint32_t script_name_set_19(std::int32_t a0, std::int32_t a1);
/// 0x00655200 script_load_unit
inline std::int32_t script_load_unit(const char* a0, const char* a1) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::script_load_unit, a0, a1); }
/// 0x0065ab50 script_list_a58fa7_call
std::int32_t script_list_a58fa7_call(const char* a0);
/// 0x0065c770 script_icon_65c770
std::int32_t script_icon_65c770(const char* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x0065ef40 script_group_transfer
std::int32_t script_group_transfer(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x0065d8a0 script_group_size_of
std::int32_t script_group_size_of(std::int32_t a0);
/// 0x00659b80 script_goldbags_to_barrels
std::int32_t script_goldbags_to_barrels();
/// 0x006539e0 script_fx_718010
template <class T1>
inline std::int32_t script_fx_718010(std::int16_t a0, T1 a1, std::int8_t a2, std::int8_t a3) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::script_fx_718010, a0, a1, a2, a3); }
/// 0x0065eee0 script_flush_436190
std::int32_t script_flush_436190();
/// 0x00653d60 script_find_unit_for
std::int32_t script_find_unit_for(std::int32_t a0);
/// 0x006596a0 script_current_1aa
std::int32_t script_current_1aa();
/// 0x00659db0 script_counter_post_inc
std::int32_t script_counter_post_inc();
/// 0x0065cd00 script_count_allies
std::int32_t script_count_allies(std::int32_t a0);
/// 0x0065ab90 script_copy_rect_a58f97
std::int32_t script_copy_rect_a58f97();
/// 0x00659b30 script_clear_a591a9
std::int32_t script_clear_a591a9();
/// 0x00659ef0 script_clear_a591a5
std::int32_t script_clear_a591a5();
/// 0x00659ea0 script_clear_a59199
std::int32_t script_clear_a59199();
/// 0x006599b0 script_clear_a59179
std::int32_t script_clear_a59179();
/// 0x00659960 script_clear_a59175
std::int32_t script_clear_a59175();
/// 0x00651130 script_capture_place
std::int32_t script_capture_place(std::int32_t a0);
/// 0x0065f820 script_call_7198d0
std::int32_t script_call_7198d0(std::int32_t a0);
/// 0x0065cf40 script_call_65cf40
std::uint32_t script_call_65cf40(const char* a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00656030 script_building_units_set_2eb
std::int32_t script_building_units_set_2eb(std::int16_t a0, std::int16_t a1, std::int16_t a2);
/// 0x00654240 script_building_4c4440
std::int32_t script_building_4c4440(std::int16_t a0, std::int32_t a1, std::int32_t a2);
/// 0x00653ef0 script_building_49c240
std::uint32_t script_building_49c240(std::int16_t a0, const char* a1);
/// 0x0065e0a0 script_bind_input
std::uint32_t script_bind_input(void* a0, const char* a1);
/// 0x0065a1b0 script_attribute
std::int32_t script_attribute(const char* a0, std::int32_t a1);
/// 0x00651ad0 script_attach
std::uint32_t script_attach(const char* a0, const char* a1);
/// 0x00653400 script_any_quest_item
std::int32_t script_any_quest_item(const char* a0);
/// 0x0065c220 script_add_a596c4
std::int32_t script_add_a596c4(std::int32_t a0);
/// 0x00659a60 script_add_a591d8
std::int32_t script_add_a591d8(std::int32_t a0);
/// 0x0065c4d0 script_a6a254_below
std::int32_t script_a6a254_below();
/// 0x0065a2f0 script_a5b9e8_432300
std::int32_t script_a5b9e8_432300(std::int32_t a0);
/// 0x00659e10 script_a58fa7_4cdc40
std::int32_t script_a58fa7_4cdc40(std::int16_t a0);
/// 0x0065a9c0 script_a58f90_set_bit
std::int32_t script_a58f90_set_bit(std::int32_t a0, std::int32_t a1);
/// 0x006596f0 script_a52928_57f990
std::uint32_t script_a52928_57f990(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x0065e900 script_a50518_6e01d0
std::int32_t script_a50518_6e01d0();
/// 0x0065b5f0 script_a501e0_4ce900
std::uint32_t script_a501e0_4ce900(const char* a0);
/// 0x0065edd0 script_a4f381_set_11
std::int32_t script_a4f381_set_11(std::int16_t a0);
/// 0x0065ec90 script_a4f381_call
std::int32_t script_a4f381_call(const char* a0, std::int32_t a1);
/// 0x0065f7c0 script_a4f381_7197f0
std::int32_t script_a4f381_7197f0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8);
/// 0x0065f780 script_a4f381_719770
std::int32_t script_a4f381_719770(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x0065f740 script_a4f381_7196e0
std::int32_t script_a4f381_7196e0(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4);
/// 0x0065c470 script_a48a38_byte
std::uint32_t script_a48a38_byte();
/// 0x00654280 script_a484d0_654530
inline std::uint32_t script_a484d0_654530(std::int32_t a0) { return ::joan::call::cdecl_call<std::uint32_t>(::joan::fn::script_a484d0_654530, a0); }
/// 0x00653eb0 script_a484d0_4c54a0
std::uint32_t script_a484d0_4c54a0(const char* a0);
/// 0x00651e70 script_a460a0_49f370
std::int32_t script_a460a0_49f370(std::int16_t a0);
/// 0x0065e850 script_6dff10
std::int32_t script_6dff10();
/// 0x0065fcc0 script_65fcc0
std::uint32_t script_65fcc0(std::int8_t a0);
/// 0x0065f910 script_65f910
std::uint32_t script_65f910(std::int16_t a0);
/// 0x0065a990 script_65a990
std::uint32_t script_65a990(std::uint32_t a0);
/// 0x0065a8d0 script_65a8d0
std::uint32_t script_65a8d0(std::uint32_t a0, std::uint8_t a1);
/// 0x00659550 script_659550
std::uint32_t script_659550(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5);
/// 0x006550d0 script_6550d0
std::uint32_t script_6550d0(const char* a0, std::uint32_t a1);
/// 0x006548b0 script_6548b0
std::uint32_t script_6548b0(const char* a0);
/// 0x00654160 script_654160
std::uint32_t script_654160(std::int16_t a0, const char* a1);
/// 0x00654120 script_654120
std::uint32_t script_654120(std::int16_t a0, const char* a1);
/// 0x006540e0 script_6540e0
std::uint32_t script_6540e0(std::int16_t a0, const char* a1);
/// 0x006540a0 script_6540a0
std::uint32_t script_6540a0(std::int16_t a0, std::int16_t a1);
/// 0x00654060 script_654060
std::uint32_t script_654060(std::int16_t a0, std::int16_t a1);
/// 0x00653a50 script_653a50
std::uint32_t script_653a50(std::int16_t a0, std::uint32_t a1, std::uint8_t a2);
/// 0x00653970 script_653970
std::uint32_t script_653970();
/// 0x00651a60 script_651a60
std::uint32_t script_651a60(const char* a0);
/// 0x0065b7c0 script_4cad00
std::uint32_t script_4cad00(const char* a0);
/// 0x00653fe0 script_49c470
std::uint32_t script_49c470(std::int16_t a0, std::int16_t a1);
/// 0x0065a320 script_42e040
std::int32_t script_42e040();
/// 0x004cd790 scenario_mode
std::int32_t scenario_mode(const char* a0);
/// 0x0066b770 scale_quarter_a6a284
void scale_quarter_a6a284();
/// 0x0066b6d0 scale_a6a27c
void scale_a6a27c();
/// 0x006ac050 route_around_6ac050
void route_around_6ac050(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, void* a4, void* a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, float a9, float a10, float a11, std::int32_t a12);
/// 0x005aa7c0 rle_blit16
void rle_blit16(void* a0, void* a1);
/// 0x005aa420 rle_blit
void rle_blit(void* a0, void* a1);
/// 0x005e8b20 reorder_release
void reorder_release();
/// 0x005499b0 remove_matching
template <class T4>
inline std::int32_t remove_matching(void* a0, std::int32_t a1, std::int32_t a2, void* a3, T4 a4, void* a5) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::remove_matching, a0, a1, a2, a3, a4, a5); }
/// 0x005aa130 read_u32
std::uint32_t read_u32(void* a0);
/// 0x006511c0 ratio_quarter
void ratio_quarter();
/// 0x00651060 ratio_half
void ratio_half();
/// 0x00661e10 random_table_lookup
std::int32_t random_table_lookup(const char* a0, void* a1);
/// 0x0060c880 quaternion_to_matrix
void* quaternion_to_matrix(void* a0, void* a1);
/// 0x0060c8b0 quaternion_slerp
void* quaternion_slerp(void* a0, void* a1, void* a2, float a3);
/// 0x0060c840 quat_from_matrix
void* quat_from_matrix(void* a0, void* a1);
/// 0x0060c7a0 quat_from_axis_angle
void* quat_from_axis_angle(void* a0, void* a1, float a2);
/// 0x006a12d0 quartic_root
double quartic_root(float a0, float a1, float a2, float a3, float a4, float a5);
/// 0x0068ce00 polygon_overlaps_rect
std::uint8_t polygon_overlaps_rect(void* a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5);
/// 0x00669cc0 point_segment_dist2
std::int32_t point_segment_dist2_669cc0(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5);
/// 0x004bbac0 pack_chars_12
const char* pack_chars_12(const char* a0, void* a1, void* a2, void* a3);
/// 0x00651f00 object_set_pos
template <class T1>
inline std::int32_t object_set_pos(std::int16_t a0, T1 a1) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::object_set_pos, a0, a1); }
/// 0x00650e10 object_field_ca
std::uint32_t object_field_ca(std::int32_t a0);
/// 0x00652590 object_byte_ce
std::int32_t object_byte_ce(std::int32_t a0);
/// 0x0056aaf0 obj_stub_56aaf0
inline void obj_stub_56aaf0() { ::joan::call::cdecl_call<void>(::joan::fn::obj_stub_56aaf0); }
/// 0x0056a050 obj_stub_56a050
inline void obj_stub_56a050() { ::joan::call::cdecl_call<void>(::joan::fn::obj_stub_56a050); }
/// 0x00566e30 obj_stub_566e30
inline void obj_stub_566e30() { ::joan::call::cdecl_call<void>(::joan::fn::obj_stub_566e30); }
/// 0x0069d7d0 obb_box_edges
template <class T0, class T1>
inline std::uint8_t obb_box_edges(T0 a0, T1 a1) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::obb_box_edges, a0, a1); }
/// 0x00592290 mul_div_round
std::uint32_t mul_div_round(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x0060c430 matrix_to_euler
void matrix_to_euler(void* a0, void* a1, void* a2, void* a3);
/// 0x0060c000 matrix_negate_row1
void* matrix_negate_row1(void* a0, void* a1);
/// 0x00547140 load_weather_ini
void load_weather_ini();
/// 0x0051c680 load_unit_constants
void load_unit_constants();
/// 0x005525b0 load_throw_ini
inline void load_throw_ini() { ::joan::call::cdecl_call<void>(::joan::fn::load_throw_ini); }
/// 0x00472ee0 load_difficulty_settings
void load_difficulty_settings();
/// 0x00454090 load_charmove_params
void load_charmove_params();
/// 0x005c65e0 lights_ambient
std::uint32_t lights_ambient();
/// 0x005e8f10 light_node_free_b
void light_node_free_b(void* a0);
/// 0x005e8d60 light_node_free_a
void light_node_free_a(void* a0);
/// 0x005513f0 lerp_5513f0
inline float lerp_5513f0(float a0, float a1, float a2) { return ::joan::call::cdecl_call<float>(::joan::fn::lerp_5513f0, a0, a1, a2); }
/// 0x0054a8a0 lerp_54a8a0
float lerp_54a8a0(float a0, float a1, float a2);
/// 0x00459b20 lerp
float lerp(float a0, float a1, float a2);
/// 0x0067a500 lerp3
void* lerp3(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6);
/// 0x007a9e80 jpeg_write_scanlines
std::uint32_t jpeg_write_scanlines(void* a0, void* a1, std::uint32_t a2);
/// 0x007a9b50 jpeg_suppress_tables
void jpeg_suppress_tables(void* a0, std::uint32_t a1);
/// 0x007aa2b0 jpeg std_huff_tables
void jpeg_7aa2b0(void* a0);
/// 0x00765f43 jpeg_std_error
void* jpeg_std_error(void* a0);
/// 0x007a9e00 jpeg_start_compress
void jpeg_start_compress(void* a0, std::uint32_t a1);
/// 0x007aa100 jpeg_set_linear_quality
void jpeg_set_linear_quality(void* a0, std::int32_t a1, std::uint32_t a2);
/// 0x007aa430 jpeg_set_colorspace
void jpeg_set_colorspace(void* a0, std::uint32_t a1);
/// 0x007aa140 jpeg_quality_scaling
std::int32_t jpeg_quality_scaling(std::int32_t a0);
/// 0x007a9ba0 jpeg_finish_compress
void jpeg_finish_compress(void* a0);
/// 0x007aa3b0 jpeg_default_colorspace
void jpeg_default_colorspace(void* a0);
/// 0x007a9b40 jpeg_abort_compress
void jpeg_abort_compress(void* a0);
/// 0x007aab10 jinit_memory_mgr
void jinit_memory_mgr(void* a0);
/// 0x007ac000 jinit_compress_master
void jinit_compress_master(void* a0);
/// 0x00430050 is_hero_name
std::int32_t is_hero_name(const char* a0);
/// 0x004c50d0 init_pi_derived
void init_pi_derived();
/// 0x00417ef0 init_pi_a4f81c
void init_pi_a4f81c();
/// 0x0054d820 init_pi_a3ce08
void init_pi_a3ce08();
/// 0x0052ab80 init_pi_a3cd8c
void init_pi_a3cd8c();
/// 0x0051ac60 init_pi_a3cbb4
void init_pi_a3cbb4();
/// 0x004d9710 init_pi_a3a328
void init_pi_a3a328();
/// 0x004d2ac0 init_pi_a3a294
void init_pi_a3a294();
/// 0x0042df20 init_pi_a39d14
void init_pi_a39d14();
/// 0x004934f0 init_pi_a39c84
void init_pi_a39c84();
/// 0x00426300 init_pi_a39c70
void init_pi_a39c70();
/// 0x0046cbd0 init_pi_a39158
void init_pi_a39158();
/// 0x00467090 init_pi_a38f90
void init_pi_a38f90();
/// 0x00448210 init_pi_a33728
void init_pi_a33728();
/// 0x00515c90 init_pi_515c90
void init_pi_515c90();
/// 0x004f4f10 init_pi_4f4f10
void init_pi_4f4f10();
/// 0x004eae10 init_pi_4eae10
void init_pi_4eae10();
/// 0x004c4eb0 init_pi
void init_pi();
/// 0x0056bef0 init_obj_56bef0
inline void init_obj_56bef0() { ::joan::call::cdecl_call<void>(::joan::fn::init_obj_56bef0); }
/// 0x0056b700 init_atexit_56b700
inline void init_atexit_56b700() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_56b700); }
/// 0x0056a070 init_atexit_56a070
inline void init_atexit_56a070() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_56a070); }
/// 0x005695c0 init_atexit_5695c0
inline void init_atexit_5695c0() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_5695c0); }
/// 0x00568b10 init_atexit_568b10
inline void init_atexit_568b10() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_568b10); }
/// 0x005680f0 init_atexit_5680f0
inline void init_atexit_5680f0() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_5680f0); }
/// 0x005673d0 init_atexit_5673d0
inline void init_atexit_5673d0() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_5673d0); }
/// 0x00566cd0 init_atexit_566cd0
inline void init_atexit_566cd0() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_566cd0); }
/// 0x00566780 init_atexit_566780
inline void init_atexit_566780() { ::joan::call::cdecl_call<void>(::joan::fn::init_atexit_566780); }
/// 0x006b05f0 init_a6a524
void init_a6a524();
/// 0x006510d0 init_a6a25c
void init_a6a25c();
/// 0x00650fd0 init_a6a250
void init_a6a250();
/// 0x0061dd40 init_a69d00
void init_a69d00();
/// 0x0061dcf0 init_a69cfc
void init_a69cfc();
/// 0x0061dc90 init_a69cf8
void init_a69cf8();
/// 0x005f4570 init_a69acc
void init_a69acc();
/// 0x0041a8d0 init_a5977c
void init_a5977c();
/// 0x00419a90 init_a59778
void init_a59778();
/// 0x00418d40 init_a59734
void init_a59734();
/// 0x0052da00 init_a3cda8
void init_a3cda8();
/// 0x0052b940 init_a3cda4
void init_a3cda4();
/// 0x00415040 init_a3cda0
void init_a3cda0();
/// 0x0052af00 init_a3cd90
void init_a3cd90();
/// 0x00524a80 init_a3cd80
void init_a3cd80();
/// 0x0041bd30 init_a3cd78
void init_a3cd78();
/// 0x00433ac0 init_a3cb7c
void init_a3cb7c();
/// 0x004f9250 init_a3cb78
void init_a3cb78();
/// 0x00431760 init_a3cb74
void init_a3cb74();
/// 0x004eb220 init_a3cad0
void init_a3cad0();
/// 0x004eaf50 init_a3cacc
void init_a3cacc();
/// 0x004d9b50 init_a3a334
void init_a3a334();
/// 0x004d9770 init_a3a32c
void init_a3a32c();
/// 0x004d4310 init_a3a2a0
void init_a3a2a0();
/// 0x00421d90 init_a3a298
void init_a3a298();
/// 0x00425810 init_a3a078
void init_a3a078();
/// 0x004230c0 init_a3a074
void init_a3a074();
/// 0x0049d5c0 init_a39da0
void init_a39da0();
/// 0x0042fec0 init_a39d9c
void init_a39d9c();
/// 0x0049d390 init_a39ca0
void init_a39ca0();
/// 0x00493650 init_a39c88
void init_a39c88();
/// 0x004926f0 init_a39c7c
void init_a39c7c();
/// 0x00492150 init_a39c68
void init_a39c68();
/// 0x00492080 init_a39c64
void init_a39c64();
/// 0x00491f40 init_a39c60
void init_a39c60();
/// 0x00491ec0 init_a39c5c
void init_a39c5c();
/// 0x00490b90 init_a39c50
void init_a39c50();
/// 0x00490a30 init_a39c4c
void init_a39c4c();
/// 0x0046ccd0 init_a39164
void init_a39164();
/// 0x0046cc80 init_a39160
void init_a39160();
/// 0x0046cc30 init_a3915c
void init_a3915c();
/// 0x004694f0 init_a39150
void init_a39150();
/// 0x004694a0 init_a3914c
void init_a3914c();
/// 0x00469450 init_a39148
void init_a39148();
/// 0x004693f0 init_a39144
void init_a39144();
/// 0x00467190 init_a38f9c
void init_a38f9c();
/// 0x00467140 init_a38f98
void init_a38f98();
/// 0x004670f0 init_a38f94
void init_a38f94();
/// 0x00448310 init_a33734
void init_a33734();
/// 0x00448270 init_a3372c
void init_a3372c();
/// 0x0060baf0 init_60baf0
std::uint32_t init_60baf0();
/// 0x00603570 init_603570
void init_603570();
/// 0x00603550 init_603550
std::uint32_t init_603550();
/// 0x0056db10 init_56db10
std::int32_t init_56db10();
/// 0x0056daf0 init_56daf0
std::uint32_t init_56daf0();
/// 0x0056d4e0 init_56d4e0
std::uint32_t init_56d4e0();
/// 0x0056bf10 init_56bf10
void init_56bf10();
/// 0x0056ba20 init_56ba20
void init_56ba20();
/// 0x0056ba00 init_56ba00
std::uint32_t init_56ba00();
/// 0x0056aba0 init_56aba0
void init_56aba0();
/// 0x0056ab10 init_56ab10
void init_56ab10();
/// 0x00569e90 init_569e90
void init_569e90();
/// 0x00569590 init_569590
void init_569590();
/// 0x00568ae0 init_568ae0
void init_568ae0();
/// 0x005680c0 init_5680c0
void init_5680c0();
/// 0x005673a0 init_5673a0
void init_5673a0();
/// 0x00566cb0 init_566cb0
std::int32_t init_566cb0();
/// 0x00566750 init_566750
void init_566750();
/// 0x00565d30 init_565d30
void init_565d30();
/// 0x00564280 init_564280
void init_564280();
/// 0x00564250 init_564250
std::uint32_t init_564250();
/// 0x00564150 init_564150
void init_564150();
/// 0x00523f80 init_523f80
void init_523f80();
/// 0x0051bab0 init_51bab0
void init_51bab0();
/// 0x005170b0 init_5170b0
void init_5170b0();
/// 0x005164a0 init_5164a0
void init_5164a0();
/// 0x004e0010 init_4e0010
std::uint32_t init_4e0010();
/// 0x004482c0 init_4482c0
void init_4482c0();
/// 0x0042e010 init_42e010
void init_42e010();
/// 0x00426420 init_426420
void init_426420();
/// 0x00422a00 init_422a00
void init_422a00();
/// 0x0041bbc0 init_41bbc0
void init_41bbc0();
/// 0x00477c80 iabs
std::int32_t iabs(std::int32_t a0);
/// 0x007bdb53 _heap_init
std::int32_t _heap_init(std::int32_t a0);
/// 0x00651830 group_try_find_651830
std::int32_t group_try_find_651830(std::int32_t a0, std::uint32_t a1);
/// 0x005aa980 gif_decode_palette
void gif_decode_palette(void* a0, void* a1);
/// 0x004c4a60 get_71
void* get_71(void* a0);
/// 0x004189a0 game_tick_4189a0
void game_tick_4189a0();
/// 0x0065b070 fx_at_cell_65b070
std::int32_t fx_at_cell_65b070(char a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6);
/// 0x0065b030 forward_6cd720
std::int32_t forward_6cd720(std::int8_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4);
/// 0x005aa2e0 fli_color256
void fli_color256(void* a0, void* a1);
/// 0x005aa610 fli_brun
void fli_brun(void* a0, void* a1);
/// 0x0046d820 facility_kind
std::int32_t facility_kind(const char* a0);
/// 0x0065ee60 f_65ee60
std::uint32_t f_65ee60();
/// 0x00615160 exp_float
float exp_float(float a0);
/// 0x0056d580 dtor_stub_56d580
inline void dtor_stub_56d580() { ::joan::call::cdecl_call<void>(::joan::fn::dtor_stub_56d580); }
/// 0x0056ab80 dtor_stub_56ab80
inline void dtor_stub_56ab80() { ::joan::call::cdecl_call<void>(::joan::fn::dtor_stub_56ab80); }
/// 0x0054d9c0 div_a3ce10
void div_a3ce10();
/// 0x0052b320 div_a3cd94
void div_a3cd94();
/// 0x0051c310 div_a3cbc0
void div_a3cbc0();
/// 0x006994d0 cylinder_sweep_hit
std::uint32_t cylinder_sweep_hit(void* a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, float a12, float a13);
/// 0x007c151d __crtGetEnvironmentStringsA
const char* __crt_get_environment_strings_a();
/// 0x0066f910 cross_terms
float cross_terms(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, void* a8, void* a9, void* a10);
/// 0x0066df30 cross3
std::int32_t cross3(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, void* a8, void* a9, void* a10);
/// 0x004bae70 cmp_byte4_desc
std::int32_t cmp_byte4_desc(void* a0, void* a1);
/// 0x00447150 check_v3d_module_447150
std::int32_t check_v3d_module_447150();
/// 0x00527e60 character_class
std::int32_t character_class(std::int16_t a0);
/// 0x0069a300 box_outside_region
template <class T0, class T1, class T2, class T3>
inline std::uint8_t box_outside_region(T0 a0, T1 a1, T2 a2, T3 a3, float a4, float a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::box_outside_region, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
/// 0x0057f690 bitmap_bytes16
std::int32_t bitmap_bytes16(std::int16_t a0, std::int16_t a1);
/// 0x00564830 atexit_564860
std::int32_t atexit_564860();
/// 0x00674fc0 array_remove_if
template <class T4>
inline std::int32_t array_remove_if(void* a0, std::int32_t a1, std::int32_t a2, void* a3, T4 a4, void* a5) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::array_remove_if, a0, a1, a2, a3, a4, a5); }
/// 0x007a973c alutLoadWAVMemory
void alut_load_wavmemory(void* a0, void* a1, void* a2, void* a3, void* a4, void* a5);
/// 0x007a949a alutLoadWAVFile
void alut_load_wavfile(const char* a0, void* a1, void* a2, void* a3, void* a4, void* a5);
/// 0x007a9420 alutInit
std::int32_t alut_init(void* a0, void* a1);
/// 0x007a9457 alutExit
std::int32_t alut_exit();
/// 0x004e4610 ai_task_from_name
std::int32_t ai_task_from_name(const char* a0);
/// 0x005a97b0 TFAnimation_NewMem
void* tfanimation_new_mem(void* a0);
/// 0x005a93f0 TFAnimation_NewFile
void* tfanimation_new_file(const char* a0);
/// 0x00565310 StaticInit::clear_a5b9a8
std::uint32_t clear_a5b9a8();
/// 0x0056d5a0 StaticInit::atexit_56d5c0
inline void atexit_56d5c0() { ::joan::call::cdecl_call<void>(::joan::fn::StaticInit_atexit_56d5c0); }
/// 0x0056d510 StaticInit::atexit_56d530
inline void atexit_56d530() { ::joan::call::cdecl_call<void>(::joan::fn::StaticInit_atexit_56d530); }
/// 0x004e36d0 StaticInit::array_a3a530
std::uint32_t array_a3a530();
/// 0x0061dd90 Init::div4_61dd90
void div4_61dd90();
/// 0x0060bb30 Init::copy_60bb30
std::uint32_t copy_60bb30();
/// 0x0066b670 Globals::init_pi_a6a274
void init_pi_a6a274();
/// 0x0066b720 Globals::init_a6a280
void init_a6a280();
/// 0x0054da70 Globals::div_a3ce08
void div_a3ce08();
/// 0x0052b7c0 Globals::div_a3cd8c
void div_a3cd8c();
/// 0x00524bd0 Globals::div_a3cd78
void div_a3cd78();
/// 0x00516ed0 Globals::div_a3cba0
void div_a3cba0();

namespace action {
/// 0x004bbbc0 Action::from_name
std::int32_t from_name(const char* a0);
}  // namespace action

namespace angle {
/// 0x00576cb0 Angle::init_tables
void init_tables();
}  // namespace angle

namespace archive {
/// 0x0047d290 Archive::compare_47d290
std::int32_t compare_47d290(const char* a0, const char* a1, const char* a2);
}  // namespace archive

namespace blit {
/// 0x005745da Blit::blit8to32_key_5745da
void blit8to32_key_5745da(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5);
/// 0x00574561 Blit::blit8to24_key
void blit8to24_key(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5);
}  // namespace blit

namespace box_fns {
/// 0x006a2fa0 Box::transform
void* transform(void* a0, void* a1, void* a2);
/// 0x0069ea70 Box::face_hit_69ea70
template <class T1>
inline std::uint8_t face_hit_69ea70(void* a0, T1 a1) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::Box_face_hit_69ea70, a0, a1); }
}  // namespace box_fns

namespace camera {
/// 0x0065ade0 Camera::nudge_6cb9e0
std::int32_t nudge_6cb9e0(std::int32_t a0);
}  // namespace camera

namespace clip {
/// 0x0068cab0 Clip::outcode
std::int32_t outcode(float a0, float a1, float a2, float a3, float a4, float a5);
}  // namespace clip

namespace clock {
/// 0x00659790 Clock::minutes_mod
double minutes_mod();
}  // namespace clock

namespace code_page {
/// 0x0058b2d0 CodePage::setup
std::uint32_t setup(const char* a0, std::uint32_t a1, std::uint32_t a2, const char* a3);
}  // namespace code_page

namespace collide {
/// 0x006a1bb0 Collide::test_6a1bb0
std::uint32_t test_6a1bb0(void* a0, void* a1, void* a2, void* a3, void* a4);
/// 0x0069bd60 Collide::cylinder_sphere
template <class T0, class T1, class T2, class T3>
inline std::uint8_t cylinder_sphere(T0 a0, T1 a1, T2 a2, T3 a3) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::Collide_cylinder_sphere, a0, a1, a2, a3); }
/// 0x0069b390 Collide::cylinder_cylinder
template <class T0, class T1, class T2, class T3>
inline std::uint8_t cylinder_cylinder(T0 a0, T1 a1, T2 a2, T3 a3) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::Collide_cylinder_cylinder, a0, a1, a2, a3); }
/// 0x0069c0f0 Collide::cylinder_box
template <class T0, class T1, class T2, class T3>
inline std::uint8_t cylinder_box(T0 a0, T1 a1, T2 a2, T3 a3) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::cylinder_box_intersection, a0, a1, a2, a3); }
/// 0x00698cb0 Collide::box_sweep_698cb0
template <class T1, class T2, class T3>
inline std::uint32_t box_sweep_698cb0(void* a0, T1 a1, T2 a2, T3 a3) { return ::joan::call::cdecl_call<std::uint32_t>(::joan::fn::Collide_box_sweep_698cb0, a0, a1, a2, a3); }
}  // namespace collide

namespace comdlg32 {
/// 0x00763f4e comdlg32 GetOpenFileNameA
inline std::int32_t get_open_file_name_a(void* a0) { return ::joan::call::stdcall_call<std::int32_t>(::joan::fn::comdlg32_GetOpenFileNameA, a0); }
}  // namespace comdlg32

namespace d3d8 {
/// 0x00763fba d3d8 Direct3DCreate8
inline void* direct3dcreate8(std::uint32_t a0) { return ::joan::call::stdcall_call<void*>(::joan::fn::d3d8_Direct3DCreate8, a0); }
}  // namespace d3d8

namespace d3dx {
/// 0x007648ba D3DXCreateTexture
std::int32_t create_texture(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, void* a7);
/// 0x00765c3d D3DXCreateTextureFromFileInMemory
std::int32_t create_texture_from_file_in_memory(void* a0, void* a1, std::uint32_t a2, void* a3);
/// 0x007669fb D3DXMatrixMultiply
void* matrix_multiply(void* a0, void* a1, void* a2);
/// 0x00768e60 D3DXMatrixReflect
void* matrix_reflect(void* a0, void* a1);
/// 0x007683ee D3DXVec3Normalize
void* vec3_normalize(void* a0, void* a1);
/// 0x00767967 D3DXQuaternionRotationMatrix
void* quaternion_rotation_matrix(void* a0, void* a1);
/// 0x00766f34 D3DXMatrixTranslation
void* matrix_translation(void* a0, float a1, float a2, float a3);
/// 0x00767101 D3DXMatrixRotationQuaternion
void* matrix_rotation_quaternion(void* a0, void* a1);
/// 0x0076883c D3DXMatrixRotationAxis
void* matrix_rotation_axis(void* a0, void* a1, float a2);
/// 0x007674a4 D3DXMatrixPerspectiveFovLH
void* matrix_perspective_fov_lh(void* a0, float a1, float a2, float a3, float a4);
/// 0x00767723 D3DXMatrixOrthoLH
void* matrix_ortho_lh(void* a0, float a1, float a2, float a3, float a4);
/// 0x00768c7c D3DXMatrixLookAtLH
void* matrix_look_at_lh(void* a0, void* a1, void* a2, void* a3);
/// 0x00766b4b D3DXMatrixInverse
void* matrix_inverse(void* a0, void* a1, void* a2);
/// 0x00765aab D3DXCreateTextureFromFileExA
std::int32_t create_texture_from_file_ex_a(void* a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, void* a11, void* a12, void* a13);
/// 0x00768495 D3DXVec3TransformCoord
void* vec3_transform_coord(void* a0, void* a1, void* a2);
/// 0x00768f1f D3DXQuaternionToAxisAngle
void quaternion_to_axis_angle(void* a0, void* a1, void* a2);
/// 0x00767eeb D3DXQuaternionSlerp
void* quaternion_slerp(void* a0, void* a1, void* a2, float a3);
/// 0x00768f9c D3DXQuaternionRotationAxis
void* quaternion_rotation_axis(void* a0, void* a1, float a2);
/// 0x00767bc7 D3DXQuaternionMultiply
void* quaternion_multiply(void* a0, void* a1, void* a2);
/// 0x007680a8 D3DXPlaneNormalize
void* plane_normalize(void* a0, void* a1);
/// 0x00768ff8 D3DXPlaneFromPoints
void* plane_from_points(void* a0, void* a1, void* a2, void* a3);
/// 0x007681df D3DXPlaneFromPointNormal
void* plane_from_point_normal(void* a0, void* a1, void* a2);
/// 0x00766ade D3DXMatrixTranspose
void* matrix_transpose(void* a0, void* a1);
/// 0x00767085 D3DXMatrixRotationZ
void* matrix_rotation_z(void* a0, float a1);
/// 0x0076700a D3DXMatrixRotationY
void* matrix_rotation_y(void* a0, float a1);
/// 0x00766f90 D3DXMatrixRotationX
void* matrix_rotation_x(void* a0, float a1);
/// 0x007693c4 D3DXGetErrorStringA
std::int32_t get_error_string_a(std::int32_t a0, const char* a1, std::uint32_t a2);
/// 0x0076ba93 D3DXDeclaratorFromFVF
std::int32_t declarator_from_fvf(std::uint32_t a0, void* a1);
/// 0x00765d4e D3DXCreateTextureFromFileA
std::int32_t create_texture_from_file_a(void* a0, const char* a1, void* a2);
/// 0x00769dc0 D3DXCreateMatrixStack
std::int32_t create_matrix_stack(std::uint32_t a0, void* a1);
/// 0x00765669 D3DXCreateCubeTextureFromFileInMemoryEx
std::int32_t create_cube_texture_from_file_in_memory_ex(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, void* a11, void* a12, void* a13);
/// 0x00765d26 D3DXCreateCubeTextureFromFileInMemory
std::int32_t create_cube_texture_from_file_in_memory(void* a0, void* a1, std::uint32_t a2, void* a3);
/// 0x00765c66 D3DXCreateCubeTextureFromFileExA
std::int32_t create_cube_texture_from_file_ex_a(void* a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, void* a10, void* a11, void* a12);
/// 0x00765dea D3DXCreateCubeTextureFromFileA
std::int32_t create_cube_texture_from_file_a(void* a0, const char* a1, void* a2);
/// 0x00764948 D3DXCreateCubeTexture
std::int32_t create_cube_texture(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, void* a6);
/// 0x00763fc0 D3DX format table entry
void* format_table_entry(std::uint32_t a0);
}  // namespace d3dx


namespace error {
/// 0x00586cb0 Error::fatal
template <class... A>
inline void fatal(void* a0, const char* a1, A... rest) { ::joan::call::cdecl_call<void>(::joan::fn::Error_fatal, a0, a1, rest...); }
}  // namespace error

namespace font {
/// 0x00575940 Font::pixel_575940
std::uint32_t pixel_575940(void* a0, std::uint32_t a1, std::uint32_t a2);
}  // namespace font

namespace format {
/// 0x005ffe60 Format::bytes_per_pixel
std::int32_t bytes_per_pixel(std::uint32_t a0);
/// 0x005fff90 Format::bit_info
std::int32_t bit_info(std::uint32_t a0);
}  // namespace format

namespace formation {
/// 0x0065f160 Formation::assign_slot
std::int32_t assign_slot(std::int16_t a0, std::int16_t a1, const char* a2);
}  // namespace formation

namespace game {
/// 0x00418bc0 Game::tick_418bc0
void tick_418bc0();
}  // namespace game

namespace grid {
/// 0x006accf0 Grid::slope
float slope(void* a0);
}  // namespace grid

namespace image {
/// 0x005aa770 Image::copy_pixels
void* copy_pixels(void* a0, void* a1);
/// 0x005aa5c0 Image::clear
void* clear(void* a0);
}  // namespace image

namespace jpeg_fns {
/// 0x00788950 jpeg::jpeg_alloc_huff_table
void* jpeg_alloc_huff_table(void* a0);
/// 0x007aa180 jpeg::jpeg_set_quality
void jpeg_set_quality(void* a0, std::int32_t a1, std::uint32_t a2);
/// 0x007aa9e0 jpeg::jpeg_stdio_dest
void jpeg_stdio_dest(void* a0, void* a1);
/// 0x007aa1b0 jpeg::jpeg_set_defaults
void jpeg_set_defaults(void* a0);
/// 0x00788919 jpeg::jpeg_destroy
void jpeg_destroy(void* a0);
/// 0x00788934 jpeg::jpeg_alloc_quant_table
void* jpeg_alloc_quant_table(void* a0);
/// 0x007888f7 jpeg::jpeg_abort
void jpeg_abort(void* a0);
}  // namespace jpeg_fns

namespace keyboard {
/// 0x00599240 Keyboard::translate
std::uint32_t translate(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
}  // namespace keyboard

namespace libc {
/// 0x007b2184 crt::atan
double atan(double a0);
/// 0x007b2952 crt::sprintf
template <class... A>
inline std::int32_t sprintf(const char* a0, const char* a1, A... rest) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_sprintf, a0, a1, rest...); }
/// 0x007b1f6b crt::fabs
double fabs(double a0);
/// 0x00427d60 crt_7b1f6b
inline double fabs_f(float a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_7b1f6b, a0); }
/// 0x004256d0 crt_sqrt
double sqrt_f(float a0);
/// 0x007b2110 crt::atof
double atof(const char* a0);
/// 0x007b29a4 crt::abs
std::int32_t abs(std::int32_t a0);
/// 0x007b2210 crt::strrchr
const char* strrchr(const char* a0, std::int32_t a1);
/// 0x007b2344 crt::sqrt
inline double sqrt(double a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_sqrt_7b2344, a0); }
/// 0x007b2404 crt::sin
inline double sin(double a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_sin_7b2404, a0); }
/// 0x007b24b4 crt::cos
inline double cos(double a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_cos_7b24b4, a0); }
/// 0x007b1780 crt::strstr
const char* strstr(const char* a0, const char* a1);
/// 0x00421020 crt_atan2
inline double atan2_f(float a0, float a1) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_atan2_421020, a0, a1); }
/// 0x007b2f98 crt::eh_vector_constructor_iterator
void eh_vector_constructor_iterator(void* a0, std::uint32_t a1, std::int32_t a2, std::uintptr_t a3, std::uintptr_t a4);
/// 0x007b2d55 crt::qsort
template <class T3>
inline void qsort(void* a0, std::uint32_t a1, std::uint32_t a2, T3 a3) { ::joan::call::cdecl_call<void>(::joan::fn::crt_qsort, a0, a1, a2, a3); }
/// 0x007b3054 crt::eh_vector_destructor_iterator
void eh_vector_destructor_iterator(void* a0, std::uint32_t a1, std::int32_t a2, std::uintptr_t a3);
/// 0x007b1800 crt::_strupr
const char* _strupr(const char* a0);
/// 0x007b34e0 crt::strchr
const char* strchr(const char* a0, std::int32_t a1);
/// 0x007b29e0 crt::strncmp
std::int32_t strncmp(const char* a0, const char* a1, std::uint32_t a2);
/// 0x007b2620 crt::memcmp
std::int32_t memcmp(void* a0, void* a1, std::uint32_t a2);
/// 0x00425610 crt_asin
inline double asin_f(float a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_asin_425610, a0); }
/// 0x007b3932 crt::_makepath
void _makepath(void* a0, void* a1, void* a2, void* a3, void* a4);
/// 0x007b3ef6 crt::fclose
std::int32_t fclose(void* a0);
/// 0x007b39c3 crt::_splitpath
void _splitpath(void* a0, void* a1, void* a2, void* a3, void* a4);
/// 0x007b2920 crt::fmod
inline double fmod(double a0, double a1) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_fmod, a0, a1); }
/// 0x007b37e6 crt::_findnext
std::int32_t _findnext(std::int32_t a0, void* a1);
/// 0x007b3719 crt::_findfirst
std::int32_t _findfirst(const char* a0, void* a1);
/// 0x00425670 crt_cos
inline double cos_f(float a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_cos_425670, a0); }
/// 0x007c0960 crt::_strnicmp
template <class T2>
inline std::int32_t _strnicmp(const char* a0, const char* a1, T2 a2) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_strnicmp, a0, a1, a2); }
/// 0x007b3fe0 crt::fopen
void* fopen(const char* a0, const char* a1);
/// 0x007b47fc crt::exit
void exit(std::uint32_t a0);
/// 0x007b2240 crt::atan2
inline double atan2(double a0, double a1) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_atan2_7b2240, a0, a1); }
/// 0x007b3148 crt::strtok
const char* strtok(const char* a0, const char* a1);
/// 0x007b3d70 crt::strncat
const char* strncat(const char* a0, const char* a1, std::uint32_t a2);
/// 0x007b42c2 crt::modf
double modf(double a0, void* a1);
/// 0x007b4910 crt::memcpy
void* memcpy(void* a0, void* a1, std::uint32_t a2);
/// 0x007b6f4d crt::getc
std::int32_t getc(void* a0);
/// 0x007b6e63 crt::fseek
std::int32_t fseek(void* a0, std::int32_t a1, std::int32_t a2);
/// 0x007b6b3d crt::fread
std::uint32_t fread(void* a0, std::uint32_t a1, std::uint32_t a2, void* a3);
/// 0x007b38ae crt::_findclose
std::int32_t _findclose(std::int32_t a0);
/// 0x007b830b crt::_unlock
void _unlock(std::int32_t a0);
/// 0x004256a0 crt_sin
inline double sin_f(float a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_sin_4256a0, a0); }
/// 0x007b2719 crt::pow
inline double pow(double a0, double a1) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_pow_7b2719, a0, a1); }
/// 0x007b82aa crt::_lock
void _lock(std::int32_t a0);
/// 0x007b3bdd crt::isspace
std::int32_t isspace(std::int32_t a0);
/// 0x007b3f73 crt::fprintf
template <class... A>
inline std::int32_t fprintf(void* a0, const char* a1, A... rest) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_fprintf, a0, a1, rest...); }
/// 0x007b5962 crt::wcslen
std::uint32_t wcslen(void* a0);
/// 0x007b4c8a crt::vsprintf
std::int32_t vsprintf(const char* a0, const char* a1, const char* a2);
/// 0x007b3ec2 crt::sscanf
template <class... A>
inline std::int32_t sscanf(const char* a0, const char* a1, A... rest) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_sscanf, a0, a1, rest...); }
/// 0x00428810 crt_pow
inline double pow_f(float a0, float a1) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_pow_428810, a0, a1); }
/// 0x007c4730 crt::_itoa
const char* _itoa(std::int32_t a0, const char* a1, std::int32_t a2);
/// 0x007b3b88 crt::isdigit
std::int32_t isdigit(std::int32_t a0);
/// 0x007b7f51 crt::_isctype
std::int32_t _isctype(std::int32_t a0, std::int32_t a1);
/// 0x007b3b0a crt::isalpha
std::int32_t isalpha(std::int32_t a0);
/// 0x007b6ce0 crt::ftell
std::int32_t ftell(void* a0);
/// 0x007b365b crt::bsearch
template <class T4>
inline void* bsearch(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3, T4 a4) { return ::joan::call::cdecl_call<void*>(::joan::fn::crt_bsearch, a0, a1, a2, a3, a4); }
/// 0x007b2274 crt::acos
inline double acos(double a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_acos_7b2274, a0); }
/// 0x007b4534 crt::toupper
std::int32_t toupper(std::int32_t a0);
/// 0x007b33bf crt::time
std::int32_t time(void* a0);
/// 0x007b3e93 crt::srand
void srand(std::uint32_t a0);
/// 0x007b338a crt::remove
std::int32_t remove(const char* a0);
/// 0x007b349b crt::_mkdir
std::int32_t _mkdir(const char* a0);
/// 0x007b7024 crt::log
inline double log(double a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_log, a0); }
/// 0x007b4079 crt::_getcwd
const char* _getcwd(const char* a0, std::int32_t a1);
/// 0x007b7c14 crt::fwrite
std::uint32_t fwrite(void* a0, std::uint32_t a1, std::uint32_t a2, void* a3);
/// 0x007b32bb crt::floor
double floor(double a0);
/// 0x007b7313 crt::fast_error_exit
void fast_error_exit(std::uint32_t a0);
/// 0x007bbfb5 crt::_errno
void* _errno();
/// 0x007b44ed crt::_endthread
void _endthread();
/// 0x007b6ac5 crt::clock
std::int32_t clock();
/// 0x007b3ff3 crt::_chdir
std::int32_t _chdir(const char* a0);
/// 0x007b43e3 crt::_beginthread
std::uint32_t _beginthread(void* a0, std::uint32_t a1, void* a2);
/// 0x007b1540 crt::atol
std::int32_t atol(const char* a0);
/// 0x007b2564 crt::asin
inline double asin(double a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_asin_7b2564, a0); }
/// 0x007b72ee crt::_amsg_exit
void _amsg_exit(std::uint32_t a0);
/// 0x00635e50 crt_acos
inline double acos_f(float a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_acos_635e50, a0); }
/// 0x007c20ce crt::__wtomb_environ
std::int32_t __wtomb_environ();
/// 0x007c6031 crt::_wcsupr
void* _wcsupr(void* a0);
/// 0x007c0a61 crt::_wcsnicmp
template <class T2>
inline std::int32_t _wcsnicmp(void* a0, void* a1, T2 a2) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_wcsnicmp, a0, a1, a2); }
/// 0x007b61bc crt::wcsncmp
template <class T2>
inline std::int32_t wcsncmp(void* a0, void* a1, T2 a2) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_wcsncmp, a0, a1, a2); }
/// 0x007c5ef4 crt::_wcslwr
void* _wcslwr(void* a0);
/// 0x007c0880 crt::_wcsicmp
std::int32_t _wcsicmp(void* a0, void* a1);
/// 0x007b5a9a crt::wcscpy
void* wcscpy(void* a0, void* a1);
/// 0x007b60bf crt::wcscmp
std::int32_t wcscmp(void* a0, void* a1);
/// 0x007b7b37 crt::_vsnprintf
template <class T1>
inline std::int32_t _vsnprintf(const char* a0, T1 a1, const char* a2, const char* a3) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_vsnprintf, a0, a1, a2, a3); }
/// 0x007b419e crt::_validdrive
std::int32_t _validdrive(std::uint32_t a0);
/// 0x007c30c8 crt::_ungetc_lk
std::int32_t _ungetc_lk(std::int32_t a0, void* a1);
/// 0x007c47e3 crt::_ultoa
const char* _ultoa(std::uint32_t a0, const char* a1, std::int32_t a2);
/// 0x007bd4b5 crt::_tzset_lk
void _tzset_lk();
/// 0x007bd471 crt::__tzset
void __tzset();
/// 0x007b4ce3 crt::tolower
std::int32_t tolower(std::int32_t a0);
/// 0x007b38ce crt::__timet_from_ft
std::int32_t __timet_from_ft(void* a0);
/// 0x007b4224 crt::tan
double tan(double a0);
/// 0x007c5dc0 crt::_strlwr
const char* _strlwr(const char* a0);
/// 0x007b1eb0 crt::strcspn
std::uint32_t strcspn(const char* a0, const char* a1);
/// 0x007b77e3 crt::_snprintf
template <class T1, class... A>
inline std::int32_t _snprintf(const char* a0, T1 a1, const char* a2, A... rest) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_snprintf, a0, a1, a2, rest...); }
/// 0x007b4e65 crt::_setmbcp
std::int32_t _setmbcp(std::int32_t a0);
/// 0x007b5269 crt::setlocale
const char* setlocale(std::int32_t a0, const char* a1);
/// 0x007bb643 crt::_output
std::int32_t _output(void* a0, const char* a1, const char* a2);
/// 0x007b8b9c crt::_mtinit
std::int32_t _mtinit();
/// 0x007c2f55 crt::mbtowc
std::int32_t mbtowc(void* a0, const char* a1, std::uint32_t a2);
/// 0x007b6a3b crt::_mbsupr
void* _mbsupr(void* a0);
/// 0x007b58e2 crt::_mbsnextc
std::uint32_t _mbsnextc(void* a0);
/// 0x007c208f crt::_mbsnbicoll
std::int32_t _mbsnbicoll(void* a0, void* a1, std::uint32_t a2);
/// 0x007b69b1 crt::_mbslwr
void* _mbslwr(void* a0);
/// 0x007b5917 crt::_mbslen
std::uint32_t _mbslen(void* a0);
/// 0x007b5900 crt::_mbsinc
void* _mbsinc(void* a0);
/// 0x007b5980 crt::_mbscpy
void* _mbscpy(void* a0, void* a1);
/// 0x007bd27e crt::_mbctoupper
std::uint32_t _mbctoupper(std::uint32_t a0);
/// 0x007c47b9 crt::_ltoa
const char* _ltoa(std::int32_t a0, const char* a1, std::int32_t a2);
/// 0x007bbfc7 crt::__loctotime_t
std::int32_t __loctotime_t(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6);
/// 0x007b36e7 crt::_lfind
template <class T4>
inline void* _lfind(void* a0, void* a1, void* a2, std::uint32_t a3, T4 a4) { return ::joan::call::cdecl_call<void*>(::joan::fn::crt_lfind, a0, a1, a2, a3, a4); }
/// 0x007c4a97 crt::_ismbblead
std::int32_t _ismbblead(std::uint32_t a0);
/// 0x007bd75d crt::_isindst_lk
std::int32_t _isindst_lk(void* a0);
/// 0x007bd73c crt::_isindst
std::int32_t _isindst(void* a0);
/// 0x007b3c2d crt::isalnum
std::int32_t isalnum(std::int32_t a0);
/// 0x007c0e73 crt::_ioinit
void _ioinit();
/// 0x007bc273 crt::_input
std::int32_t _input(void* a0, void* a1, const char* a2);
/// 0x007b524d crt::__initmbctable
void __initmbctable();
/// 0x007bda49 crt::gmtime
void* gmtime(void* a0);
/// 0x007b8c21 crt::_getptd
void* _getptd();
/// 0x007b40c9 crt::_getdcwd_lk
const char* _getdcwd_lk(std::int32_t a0, const char* a1, std::int32_t a2);
/// 0x007c2637 crt::_fptostr
void _fptostr(const char* a0, std::int32_t a1, void* a2);
/// 0x007b9c9d crt::_fltin
void* _fltin(void* a0, const char* a1);
/// 0x007bb52b crt::_flsbuf
std::int32_t _flsbuf(std::int32_t a0, void* a1);
/// 0x007c0b59 crt::_filbuf
std::int32_t _filbuf(void* a0);
/// 0x007b7d4d crt::fflush
std::int32_t fflush(void* a0);
/// 0x007c616e crt::_fcvt
const char* _fcvt(double a0, std::int32_t a1, void* a2, void* a3);
/// 0x007b70e0 crt::exp
inline double exp(double a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_exp, a0); }
/// 0x007b480d crt::_exit
void _exit(std::uint32_t a0);
/// 0x007bbf42 crt::_dosmaperr
void* _dosmaperr(std::uint32_t a0);
/// 0x007bbfbe crt::__doserrno
void* __doserrno();
/// 0x007bd909 crt::cvtdate
void cvtdate(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10);
/// 0x007b7fc6 crt::__crtLCMapStringA
template <class T0, class T1, class T7>
inline std::int32_t __crt_lcmap_string_a(T0 a0, T1 a1, const char* a2, std::int32_t a3, const char* a4, std::int32_t a5, std::int32_t a6, T7 a7) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_crtLCMapStringA, a0, a1, a2, a3, a4, a5, a6, a7); }
/// 0x007c0514 crt::__crtCompareStringW
template <class T0, class T1>
inline std::int32_t __crt_compare_string_w(T0 a0, T1 a1, void* a2, std::int32_t a3, void* a4, std::int32_t a5, std::int32_t a6) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_crtCompareStringW, a0, a1, a2, a3, a4, a5, a6); }
/// 0x007c026c crt::__crtCompareStringA
template <class T0, class T1>
inline std::int32_t __crt_compare_string_a(T0 a0, T1 a1, const char* a2, std::int32_t a3, const char* a4, std::int32_t a5, std::int32_t a6) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::crt_crtCompareStringA, a0, a1, a2, a3, a4, a5, a6); }
/// 0x007b78af crt::_controlfp
inline std::uint32_t _controlfp(std::uint32_t a0, std::uint32_t a1) { return ::joan::call::cdecl_call<std::uint32_t>(::joan::fn::crt_controlfp, a0, a1); }
/// 0x007b47cf crt::_cinit
void _cinit();
/// 0x00425640 crt_atan
inline double atan_f(float a0) { return ::joan::call::cdecl_call<double>(::joan::fn::crt_atan_425640, a0); }
/// 0x007bd2f9 crt::_XcptFilter
std::int32_t _xcpt_filter(std::uint32_t a0, void* a1);
}  // namespace libc

namespace material {
/// 0x005d56c0 Material::texture_ref
std::uint32_t texture_ref(void* a0);
}  // namespace material

namespace msg {
/// 0x0065cf00 Msg::post_65cf00
std::uint32_t post_65cf00(const char* a0, const char* a1, std::uint32_t a2);
/// 0x005aa1b0 Msg::dispatch_5aa1b0
void dispatch_5aa1b0(void* a0, void* a1);
}  // namespace msg

namespace music_stream {
/// 0x005a98a0 MusicStream::release
std::uint8_t release(void* a0);
/// 0x005aa040 MusicStream::rewind
std::uint8_t rewind(void* a0);
/// 0x005a9b40 MusicStream::reset
void* reset(void* a0);
/// 0x005a9ad0 MusicStream::read_header
std::uint8_t read_header(void* a0, void* a1);
}  // namespace music_stream

namespace openal {
/// 0x007a9a42 OpenAL32 alcOpenDevice
inline void* alc_open_device(const char* a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::OpenAL32_alcOpenDevice, a0); }
/// 0x007a9a36 OpenAL32 alcMakeContextCurrent
inline std::int32_t alc_make_context_current(void* a0) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::OpenAL32_alcMakeContextCurrent, a0); }
/// 0x007a9a5a OpenAL32 alcGetCurrentContext
inline void* alc_get_current_context() { return ::joan::call::cdecl_call<void*>(::joan::fn::OpenAL32_alcGetCurrentContext); }
/// 0x007a9a54 OpenAL32 alcGetContextsDevice
inline void* alc_get_contexts_device(void* a0) { return ::joan::call::cdecl_call<void*>(::joan::fn::OpenAL32_alcGetContextsDevice, a0); }
/// 0x007a9a4e OpenAL32 alcDestroyContext
inline std::int32_t alc_destroy_context(void* a0) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::OpenAL32_alcDestroyContext, a0); }
/// 0x007a9a3c OpenAL32 alcCreateContext
inline void* alc_create_context(void* a0, void* a1) { return ::joan::call::cdecl_call<void*>(::joan::fn::OpenAL32_alcCreateContext, a0, a1); }
/// 0x007a9a48 OpenAL32 alcCloseDevice
inline std::int32_t alc_close_device(void* a0) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::OpenAL32_alcCloseDevice, a0); }
}  // namespace openal

namespace p2 {
/// 0x006639e0 P2::sub
template <class T1, class T2>
inline void* sub(void* a0, T1 a1, T2 a2) { return ::joan::call::cdecl_call<void*>(::joan::fn::op_sub, a0, a1, a2); }
}  // namespace p2

namespace palette {
/// 0x00613310 Palette::load_act
std::int32_t load_act(const char* a0);
}  // namespace palette

namespace path {
/// 0x0066ce30 Path::astar_66ce30
std::int32_t astar_66ce30(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::int32_t a8, void* a9, std::uint32_t a10, std::int32_t a11, std::int32_t a12, std::int32_t a13);
}  // namespace path

namespace pixel {
/// 0x00571205 Pixel::pack_color
std::uint32_t pack_color(std::uint32_t a0);
/// 0x0057145a Pixel::init_rgb32
void init_rgb32();
/// 0x005714d2 Pixel::init_bgr32
void init_bgr32();
/// 0x005712df Pixel::init_rgb565
void init_rgb565();
/// 0x00571248 Pixel::init_rgb555
void init_rgb555();
/// 0x005713f3 Pixel::init_bgr565
void init_bgr565();
/// 0x00571369 Pixel::init_bgr555
void init_bgr555();
}  // namespace pixel

namespace placements {
/// 0x004a2b20 Placements::find
void* find(void* a0, std::int32_t a1, PathSpace* a2);
}  // namespace placements

namespace players {
/// 0x006ab4e0 Players::find_match
template <class T2>
inline std::int32_t find_match(void* a0, std::int32_t a1, T2 a2, void* a3, std::uint32_t a4, void* a5, void* a6, std::uint32_t a7, std::uint32_t a8) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::Players_find_match_body, a0, a1, a2, a3, a4, a5, a6, a7, a8); }
/// 0x006ab4e0 Players::find_match
template <class T2>
inline std::int32_t find_match_6ab4e0(void* a0, std::int32_t a1, T2 a2, void* a3, std::uint32_t a4, void* a5, void* a6, std::uint32_t a7, std::uint32_t a8) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::path_space_rec_array_find_polygon_6ab4e0, a0, a1, a2, a3, a4, a5, a6, a7, a8); }
}  // namespace players

namespace point {
/// 0x00663a30 Point::scale
void* scale(void* a0, std::int32_t a1, std::int32_t a2, float a3);
/// 0x00663990 Point::add
void* add(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4);
}  // namespace point

namespace pool {
/// 0x0063e450 Pool::alloc
void* alloc(std::uint32_t a0);
/// 0x0063e4f0 Pool::free
void free_(void* a0);
}  // namespace pool

namespace quat {
}  // namespace quat

namespace reorder {
/// 0x005e8de0 Reorder::alloc_30
void* alloc_30(std::int32_t a0, std::int32_t a1);
/// 0x005e8c40 Reorder::alloc_20
void* alloc_20(std::int32_t a0, std::int32_t a1);
}  // namespace reorder

namespace save_list {
/// 0x0065bed0 SaveList::add
std::int32_t add(const char* a0, const char* a1, std::uint32_t a2);
}  // namespace save_list

namespace scene {
/// 0x005eadd0 Scene::partition
template <class T4>
inline std::int32_t partition(void* a0, std::int32_t a1, std::int32_t a2, void* a3, T4 a4, void* a5) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::Scene_partition, a0, a1, a2, a3, a4, a5); }
}  // namespace scene

namespace script {
/// 0x00652190 Script::formation_slot
std::int32_t formation_slot(std::int32_t a0, std::int32_t a1);
/// 0x006515d0 Script::set_mode
std::int32_t set_mode(std::int32_t a0, const char* a1);
/// 0x00652ee0 Script::angle_to
std::int32_t angle_to(std::int32_t a0, std::int32_t a1);
/// 0x00651590 Script::mode_is1
std::int32_t mode_is1(std::int32_t a0);
/// 0x00651550 Script::mode_gt1
std::int32_t mode_gt1(std::int32_t a0);
/// 0x00652370 Script::unit_528300
std::int32_t unit_528300(std::int32_t a0);
/// 0x00653150 Script::distance
std::int32_t distance(std::int32_t a0, std::int32_t a1);
/// 0x00651690 Script::set_target_mode
std::int32_t set_target_mode(std::int32_t a0, const char* a1);
/// 0x006512d0 Script::is_building
std::int32_t is_building(std::int32_t a0);
/// 0x00651430 Script::in_arc
std::int32_t in_arc(std::int32_t a0, std::int32_t a1);
/// 0x006558d0 Script::set_2d7
std::int32_t set_2d7(std::int32_t a0, std::int32_t a1);
/// 0x006569f0 Script::leader_id
std::int32_t leader_id(std::int32_t a0);
/// 0x00653060 Script::distance_to_object
std::int32_t distance_to_object(std::int32_t a0, std::int32_t a1);
/// 0x00653f30 Script::group_contains
std::int32_t group_contains(std::int32_t a0, std::int32_t a1);
/// 0x0063eb80 Script::error
template <class... A>
inline void error(void* a0, const char* a1, std::int32_t a2, A... rest) { ::joan::call::cdecl_call<void>(::joan::fn::Script_error, a0, a1, a2, rest...); }
/// 0x006547e0 Script::group_set_angle
std::int32_t group_set_angle(std::int32_t a0, std::int32_t a1);
/// 0x006547a0 Script::group_angle
std::int32_t group_angle(std::int32_t a0);
/// 0x00652aa0 Script::get_unit_flag22
std::int32_t get_unit_flag22(std::int16_t a0);
/// 0x006520d0 Script::formation
std::int32_t formation(std::int32_t a0);
/// 0x0065aff0 Script::fn_65aff0
std::int32_t fn_65aff0(char a0, char a1);
/// 0x0065e8c0 Script::call_6e00b0
std::int32_t call_6e00b0(char a0);
/// 0x0065e810 Script::call_6dfe10
std::int32_t call_6dfe10(char a0);
/// 0x0065afd0 Script::wrap_65afd0
std::uint32_t wrap_65afd0();
/// 0x0065a370 Script::wrap_65a370
std::uint32_t wrap_65a370();
/// 0x0065a350 Script::wrap_65a350
std::uint32_t wrap_65a350();
/// 0x0065a2d0 Script::wrap_65a2d0
std::uint32_t wrap_65a2d0();
/// 0x00659740 Script::wrap_659740
float wrap_659740();
/// 0x00655f80 Script::units_set_2eb
std::int32_t units_set_2eb(std::uint32_t a0, std::uint32_t a1);
/// 0x00655750 Script::unit_set_side
std::int32_t unit_set_side(std::uint32_t a0, const char* a1);
/// 0x006556e0 Script::unit_set_5089e0
std::int32_t unit_set_5089e0(std::uint32_t a0, const char* a1);
/// 0x00655cd0 Script::unit_set_2e5
std::int32_t unit_set_2e5(std::uint32_t a0, std::uint32_t a1);
/// 0x00657fd0 Script::unit_join_group
std::int32_t unit_join_group(std::uint32_t a0, std::uint32_t a1);
/// 0x00655b20 Script::unit_field_1e2
std::uint32_t unit_field_1e2(std::uint32_t a0);
/// 0x006568d0 Script::unit_copy_13d
std::int32_t unit_copy_13d(std::uint32_t a0);
/// 0x006561b0 Script::unit_2eb_is_2
std::int32_t unit_2eb_is_2(std::uint32_t a0);
/// 0x0065c130 Script::soldier_sum_1b0
std::int32_t soldier_sum_1b0(std::int32_t a0);
/// 0x0065b8c0 Script::soldier_set_374
std::int32_t soldier_set_374(std::int16_t a0, std::uint32_t a1);
/// 0x0065fd10 Script::soldier_set_358
std::int32_t soldier_set_358(std::int32_t a0, std::uint8_t a1);
/// 0x0065d560 Script::soldier_near_target
std::int32_t soldier_near_target(std::int32_t a0);
/// 0x0065c9e0 Script::soldier_5229a0
std::int32_t soldier_5229a0(std::int32_t a0, std::uint32_t a1);
/// 0x0065b240 Script::soldier_51fdd0
std::uint32_t soldier_51fdd0(const char* a0);
/// 0x0065a0e0 Script::set_stat
std::int32_t set_stat(const char* a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00653df0 Script::set_side_relation
std::int32_t set_side_relation(const char* a0, const char* a1, std::uint32_t a2);
/// 0x0065f850 Script::set_side
std::int32_t set_side(const char* a0, const char* a1);
/// 0x00650db0 Script::set_c6
std::int32_t set_c6(std::int32_t a0, std::uint32_t a1);
/// 0x0065a6e0 Script::set_a592a8_word
std::int32_t set_a592a8_word(std::int32_t a0, std::uint16_t a1);
/// 0x0065a2a0 Script::set_a59294
std::int32_t set_a59294(std::uint32_t a0);
/// 0x0065ee30 Script::set_a4f858
std::int32_t set_a4f858(std::uint8_t a0);
/// 0x006522f0 Script::set_2ba
std::int32_t set_2ba(std::int16_t a0, std::uint32_t a1);
/// 0x006541d0 Script::reset_object_1ca
std::int32_t reset_object_1ca(std::uint32_t a0);
/// 0x00652e60 Script::reset_all
std::int32_t reset_all();
/// 0x006542b0 Script::place_switch_side
std::int32_t place_switch_side(std::uint32_t a0, std::uint32_t a1);
/// 0x00654560 Script::place_set_side
std::int32_t place_set_side(std::uint32_t a0, const char* a1);
/// 0x0065a400 Script::objective_update
std::int32_t objective_update(std::uint32_t a0, const char* a1);
/// 0x0065a840 Script::objective_set_15
std::uint32_t objective_set_15(std::uint32_t a0, std::uint32_t a1);
/// 0x0065a460 Script::objective_467390
std::int32_t objective_467390(std::uint32_t a0, const char* a1);
/// 0x0065a3c0 Script::objective_467300
std::int32_t objective_467300(std::uint32_t a0, const char* a1);
/// 0x00654ea0 Script::object_set_81
std::int32_t object_set_81(std::uint32_t a0, std::uint32_t a1);
/// 0x0065ead0 Script::object_height
float object_height(std::uint32_t a0);
/// 0x00654fa0 Script::object_call_46b730
std::int32_t object_call_46b730(std::uint32_t a0, std::uint32_t a1);
/// 0x0065bce0 Script::mount_65bce0
std::int32_t mount_65bce0(std::int16_t a0, std::int16_t a1, char a2);
/// 0x006582f0 Script::group_set_side
std::int32_t group_set_side(std::uint32_t a0, const char* a1);
/// 0x00658920 Script::group_follow_leader
std::int32_t group_follow_leader(std::uint32_t a0);
/// 0x00653130 Script::get_word_ptr
std::int32_t get_word_ptr(void* a0);
/// 0x0065ed50 Script::get_a5b9f8
std::uint32_t get_a5b9f8();
/// 0x00659060 Script::get_a5b9e8
std::uint32_t get_a5b9e8();
/// 0x0065c200 Script::get_a596c4
std::uint32_t get_a596c4();
/// 0x0065a280 Script::get_a59294
std::uint32_t get_a59294();
/// 0x00659ac0 Script::get_a591d8
std::uint32_t get_a591d8();
/// 0x006598f0 Script::get_a591cc
std::uint32_t get_a591cc();
/// 0x00659d90 Script::get_a591c8
std::uint32_t get_a591c8();
/// 0x0065fbd0 Script::get_a591c7
std::int32_t get_a591c7();
/// 0x00659cc0 Script::get_a591c2
std::uint32_t get_a591c2();
/// 0x00659ca0 Script::get_a591be
std::uint32_t get_a591be();
/// 0x00659d40 Script::get_a591ba
std::uint32_t get_a591ba();
/// 0x00659b60 Script::get_a591b6
std::uint32_t get_a591b6();
/// 0x00659e60 Script::get_a591b1
std::uint32_t get_a591b1();
/// 0x00659e40 Script::get_a591ad
std::uint32_t get_a591ad();
/// 0x00659b10 Script::get_a591a9
std::uint32_t get_a591a9();
/// 0x00659f20 Script::get_a591a5
std::uint32_t get_a591a5();
/// 0x00659ed0 Script::get_a59199
std::uint32_t get_a59199();
/// 0x00659e80 Script::get_a5918d
std::uint32_t get_a5918d();
/// 0x006599e0 Script::get_a59179
std::uint32_t get_a59179();
/// 0x00659990 Script::get_a59175
std::uint32_t get_a59175();
/// 0x00659940 Script::get_a59171
std::uint32_t get_a59171();
/// 0x00659810 Script::get_a47bc0
std::uint32_t get_a47bc0();
/// 0x006597f0 Script::get_a47bbc
std::uint32_t get_a47bbc();
/// 0x006597d0 Script::get_a47bb8
std::uint32_t get_a47bb8();
/// 0x00659720 Script::get_a47bb0
std::uint32_t get_a47bb0();
/// 0x00653110 Script::get_a46059
std::int32_t get_a46059();
/// 0x0065d070 Script::fn_65d070
std::int32_t fn_65d070(std::int32_t a0, std::uint32_t a1);
/// 0x00655010 Script::fn_655010
std::int32_t fn_655010(const char* a0, const char* a1, char a2);
/// 0x0065f6a0 Script::flush_a50088
std::int32_t flush_a50088();
/// 0x006595e0 Script::flag_a59298
std::int32_t flag_a59298();
/// 0x0065aa80 Script::current_19a
std::uint32_t current_19a();
/// 0x00659d60 Script::clear_a591c8
std::int32_t clear_a591c8();
/// 0x00659ae0 Script::clear_a5918d
std::int32_t clear_a5918d();
/// 0x00659910 Script::clear_a59171
std::int32_t clear_a59171();
/// 0x0065c420 Script::call_761680
std::int32_t call_761680(const char* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x0065ecf0 Script::call_71d1e0
std::uint32_t call_71d1e0(const char* a0);
/// 0x00659860 Script::call_51c4b0
std::int32_t call_51c4b0(std::uint32_t a0);
/// 0x0065ab10 Script::call_4cde30
std::int32_t call_4cde30(const char* a0);
/// 0x00659de0 Script::call_4cdd40
std::int32_t call_4cdd40(std::uint32_t a0);
/// 0x0065aad0 Script::call_4cd570
std::int32_t call_4cd570(const char* a0);
/// 0x00651a30 Script::call_4a36c0
std::int32_t call_4a36c0(const char* a0);
/// 0x0065a930 Script::call_472540
std::int32_t call_472540();
/// 0x0065a9f0 Script::call_4724c0
std::int32_t call_4724c0(std::uint32_t a0);
/// 0x0065a900 Script::call_472220
std::int32_t call_472220(std::uint32_t a0);
/// 0x0065a8a0 Script::call_4720d0
std::int32_t call_4720d0(std::uint32_t a0);
/// 0x0065a870 Script::call_472000
std::int32_t call_472000(std::uint32_t a0, std::uint32_t a1);
}  // namespace script

namespace seat {
/// 0x00656dc0 Seat::find_free_656dc0
inline std::int32_t find_free_656dc0(std::int16_t a0, std::int16_t a1, const char* a2) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::Seat_find_free_656dc0, a0, a1, a2); }
/// 0x00656dc0 Seat::find_free_656dc0
std::int32_t find_free_656dc0_656dc0(std::int16_t a0, std::int16_t a1, const char* a2);
}  // namespace seat

namespace shape {
/// 0x006a0f90 Shape::sphere_hit
template <class T1, class T2>
inline std::uint8_t sphere_hit(void* a0, T1 a1, T2 a2, void* a3) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::Shape_sphere_hit, a0, a1, a2, a3); }
/// 0x00699ec0 Shape::cylinder_hit
template <class T1, class T2, class T3>
inline std::uint8_t cylinder_hit(void* a0, T1 a1, T2 a2, T3 a3) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::Shape_cylinder_hit, a0, a1, a2, a3); }
/// 0x00698970 Shape::box_hit
template <class T1, class T2, class T3>
inline std::uint8_t box_hit(void* a0, T1 a1, T2 a2, T3 a3) { return ::joan::call::cdecl_call<std::uint8_t>(::joan::fn::Shape_box_hit, a0, a1, a2, a3); }
}  // namespace shape

namespace sound {
/// 0x004b03e0 Sound::attenuation_of_name
std::int32_t attenuation_of_name(const char* a0);
}  // namespace sound

namespace sp_index {
/// 0x0066fb40 SpIndex::add_rect
std::int32_t add_rect(std::uint32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, void* a5);
}  // namespace sp_index

namespace strings {
/// 0x005a3090 Strings::get
template <class... A>
inline void* get(void* a0, const char* a1, std::int32_t a2, A... rest) { return ::joan::call::cdecl_call<void*>(::joan::fn::Strings_get, a0, a1, a2, rest...); }
/// 0x005a31e0 Strings::format
const char* format(const char* a0, void* a1);
}  // namespace strings

namespace text {
/// 0x005a4c30 Text::format_5a4c30
const char* format_5a4c30(const char* a0, void* a1);
}  // namespace text

namespace text_res {
/// 0x005a2860 TextRes::small_buffer
const char* small_buffer(const char* a0);
}  // namespace text_res

namespace texture_ref {
/// 0x0042cef0 TextureRef::release
std::int32_t release(void* a0);
}  // namespace texture_ref

namespace tfframe {
/// 0x005a9d80 TFFrame::decode
std::uint8_t decode(void* a0);
}  // namespace tfframe

namespace tri {
/// 0x00669e50 Tri::segment_hits
std::int32_t segment_hits(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9);
}  // namespace tri

namespace unit {
/// 0x0065b280 Unit::range_for_level
std::int32_t range_for_level(std::int32_t a0, std::int32_t a1);
/// 0x0065b6a0 Unit::is_kind10
std::int32_t is_kind10(std::int32_t a0);
/// 0x0065c8d0 Unit::idle
std::int32_t idle(std::int32_t a0);
/// 0x00656bb0 Unit::guard_check
std::int32_t guard_check(std::int16_t a0, const char* a1);
/// 0x0065b800 Unit::distance_to
std::int32_t distance_to(std::int16_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x0042ff90 Unit::is_hero
std::int32_t is_hero(std::int32_t a0);
/// 0x006592c0 Unit::rank_value
std::int32_t rank_value(std::int16_t a0);
/// 0x0065d6f0 Unit::marker_distance_65d6f0
float marker_distance_65d6f0(std::int32_t a0, std::int32_t a1, std::int32_t a2);
}  // namespace unit

namespace units {
/// 0x006519d0 Units::call_503680_6519d0
std::uint32_t call_503680_6519d0(std::int32_t a0, const char* a1, std::uint32_t a2, std::uint32_t a3);
/// 0x00656410 Units::detach_656410
std::int32_t detach_656410(std::int32_t a0);
/// 0x00659210 Units::release_owned
std::uint32_t release_owned(std::int32_t a0);
}  // namespace units

namespace vec3 {
}  // namespace vec3

namespace world {
/// 0x0054e930 World::alloc_objects
inline std::int32_t alloc_objects(std::int32_t a0) { return ::joan::call::cdecl_call<std::int32_t>(::joan::fn::World_alloc_objects, a0); }
}  // namespace world

namespace zones {
/// 0x006ab1b0 Zones::find_path_6ab1b0
std::int32_t find_path_6ab1b0(void* a0, std::int32_t a1, float a2, float a3, float a4, std::int32_t a5, std::int32_t a6, void* a7, void* a8, void* a9);
}  // namespace zones

}  // namespace joan::orig
