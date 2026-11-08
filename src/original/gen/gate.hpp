// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Gate class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "map_object.hpp"

namespace joan::orig {

struct ArrU32;

#pragma pack(push, 1)
struct Gate : MapObject {
#include "gate_fields.inc"
#include "gate_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Gate) == 0xdb);
static_assert(offsetof(Gate, hp) == 0xc2);
static_assert(offsetof(Gate, max_hp) == 0xc6);
static_assert(offsetof(Gate, need_key) == 0xce);
static_assert(offsetof(Gate, invincible) == 0xcf);
static_assert(offsetof(Gate, attacked) == 0xd0);
static_assert(offsetof(Gate, state) == 0xd1);
static_assert(offsetof(Gate, side) == 0xd2);
static_assert(offsetof(Gate, blocked_count) == 0xd3);
static_assert(offsetof(Gate, blocked_cells) == 0xd7);
#endif

}  // namespace joan::orig
