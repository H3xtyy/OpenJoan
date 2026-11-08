// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ModelSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ModelSlot {
#include "model_slot_fields.inc"
#include "model_slot_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ModelSlot) == 0x21b);
static_assert(offsetof(ModelSlot, id) == 0x0);
static_assert(offsetof(ModelSlot, refs) == 0x23);
static_assert(offsetof(ModelSlot, handle) == 0x24);
static_assert(offsetof(ModelSlot, times) == 0x28);
static_assert(offsetof(ModelSlot, gravity_level) == 0x48);
static_assert(offsetof(ModelSlot, attack_force) == 0x4c);
static_assert(offsetof(ModelSlot, vertical_attack_force) == 0x50);
static_assert(offsetof(ModelSlot, emitter_flag) == 0x75);
static_assert(offsetof(ModelSlot, emitter_default) == 0x76);
static_assert(offsetof(ModelSlot, emitter_names) == 0x77);
static_assert(offsetof(ModelSlot, emitter_a) == 0x13d);
static_assert(offsetof(ModelSlot, emitter_b) == 0x155);
static_assert(offsetof(ModelSlot, emitter_c) == 0x16d);
static_assert(offsetof(ModelSlot, emitter_pos) == 0x185);
static_assert(offsetof(ModelSlot, emitter_bytes) == 0x1cd);
static_assert(offsetof(ModelSlot, emitter_sel) == 0x1d3);
static_assert(offsetof(ModelSlot, attach_model) == 0x1d9);
static_assert(offsetof(ModelSlot, attach_anim) == 0x1dd);
static_assert(offsetof(ModelSlot, anim_name) == 0x1e1);
static_assert(offsetof(ModelSlot, u8_202) == 0x202);
static_assert(offsetof(ModelSlot, emitter_txt_ids) == 0x203);
#endif

}  // namespace joan::orig
