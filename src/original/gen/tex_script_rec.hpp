// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexScriptRec class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TexScriptRec {
#include "tex_script_rec_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexScriptRec) == 0xd51);
static_assert(offsetof(TexScriptRec, id) == 0x0);
static_assert(offsetof(TexScriptRec, name) == 0x2);
static_assert(offsetof(TexScriptRec, layer_count) == 0x23);
static_assert(offsetof(TexScriptRec, material_indices) == 0x25);
static_assert(offsetof(TexScriptRec, stage_count) == 0x31);
static_assert(offsetof(TexScriptRec, tfactor_alphas) == 0x3d);
static_assert(offsetof(TexScriptRec, f32_49) == 0x49);
static_assert(offsetof(TexScriptRec, tex_op_61) == 0x61);
static_assert(offsetof(TexScriptRec, tex_op_c1) == 0xc1);
static_assert(offsetof(TexScriptRec, tex_op_121) == 0x121);
static_assert(offsetof(TexScriptRec, tex_op_181) == 0x181);
static_assert(offsetof(TexScriptRec, tex_op_1e1) == 0x1e1);
static_assert(offsetof(TexScriptRec, tex_op_241) == 0x241);
static_assert(offsetof(TexScriptRec, tex_op_2a1) == 0x2a1);
static_assert(offsetof(TexScriptRec, f32_301) == 0x301);
static_assert(offsetof(TexScriptRec, i32_361) == 0x361);
static_assert(offsetof(TexScriptRec, stage_names) == 0x3c1);
static_assert(offsetof(TexScriptRec, object_mode) == 0x6d9);
static_assert(offsetof(TexScriptRec, scroll_speeds_u) == 0x6f1);
static_assert(offsetof(TexScriptRec, address_modes_u) == 0x751);
static_assert(offsetof(TexScriptRec, scroll_speeds_v) == 0x7b1);
static_assert(offsetof(TexScriptRec, address_modes_v) == 0x811);
static_assert(offsetof(TexScriptRec, has_extra) == 0x871);
static_assert(offsetof(TexScriptRec, sphere_map_flags) == 0x8d1);
static_assert(offsetof(TexScriptRec, projected_flags) == 0x931);
static_assert(offsetof(TexScriptRec, bump_env_mat00s) == 0x991);
static_assert(offsetof(TexScriptRec, bump_env_mat01s) == 0x9f1);
static_assert(offsetof(TexScriptRec, bump_env_mat10s) == 0xa51);
static_assert(offsetof(TexScriptRec, bump_env_mat11s) == 0xab1);
static_assert(offsetof(TexScriptRec, bump_env_lscales) == 0xb11);
static_assert(offsetof(TexScriptRec, bump_env_loffsets) == 0xb71);
static_assert(offsetof(TexScriptRec, scroll_offsets_u) == 0xbd1);
static_assert(offsetof(TexScriptRec, scroll_offsets_v) == 0xc31);
static_assert(offsetof(TexScriptRec, scales_u) == 0xc91);
static_assert(offsetof(TexScriptRec, scales_v) == 0xcf1);
#endif

}  // namespace joan::orig
