// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Relation class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Relation {
#include "relation_fields.inc"
#include "relation_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Relation) == 0xf4);
static_assert(offsetof(Relation, u8_1) == 0x1);
static_assert(offsetof(Relation, status) == 0x2);
static_assert(offsetof(Relation, attitude) == 0x3);
static_assert(offsetof(Relation, since) == 0x4);
static_assert(offsetof(Relation, opinion) == 0x8);
static_assert(offsetof(Relation, f32_c) == 0xc);
static_assert(offsetof(Relation, u16_10) == 0x10);
static_assert(offsetof(Relation, i16_12) == 0x12);
static_assert(offsetof(Relation, last_gift_day) == 0x14);
static_assert(offsetof(Relation, gift_total) == 0x18);
static_assert(offsetof(Relation, import_value) == 0x1c);
static_assert(offsetof(Relation, import_prev) == 0x20);
static_assert(offsetof(Relation, export_value) == 0x24);
static_assert(offsetof(Relation, export_prev) == 0x28);
static_assert(offsetof(Relation, deadlines) == 0x78);
#endif

}  // namespace joan::orig
