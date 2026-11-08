// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ModelObj class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "effect_state.hpp"

namespace joan::orig {

struct Actor;
struct Bone;

#pragma pack(push, 1)
struct ModelObj {
#include "model_obj_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ModelObj) == 0xa3);
static_assert(offsetof(ModelObj, kind) == 0x0);
static_assert(offsetof(ModelObj, pos) == 0x2);
static_assert(offsetof(ModelObj, angle) == 0xe);
static_assert(offsetof(ModelObj, owner) == 0x12);
static_assert(offsetof(ModelObj, bone) == 0x16);
static_assert(offsetof(ModelObj, attached) == 0x1a);
static_assert(offsetof(ModelObj, start_time) == 0x1b);
static_assert(offsetof(ModelObj, u32_1f) == 0x1f);
static_assert(offsetof(ModelObj, end_time) == 0x23);
static_assert(offsetof(ModelObj, u32_27) == 0x27);
static_assert(offsetof(ModelObj, fx) == 0x2b);
#endif

}  // namespace joan::orig
