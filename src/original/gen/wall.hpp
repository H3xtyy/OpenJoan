// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Wall class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "map_object.hpp"

namespace joan::orig {

struct Actor;

#pragma pack(push, 1)
struct Wall : MapObject {
#include "wall_fields.inc"
#include "wall_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Wall) == 0xef);
static_assert(offsetof(Wall, u32_c6) == 0xc6);
static_assert(offsetof(Wall, max_hp) == 0xca);
static_assert(offsetof(Wall, u32_ce) == 0xce);
static_assert(offsetof(Wall, actor_a) == 0xd2);
static_assert(offsetof(Wall, level_a) == 0xd6);
static_assert(offsetof(Wall, actor_b) == 0xda);
static_assert(offsetof(Wall, level_b) == 0xde);
static_assert(offsetof(Wall, u16_e2) == 0xe2);
static_assert(offsetof(Wall, u16_e4) == 0xe4);
static_assert(offsetof(Wall, invincible) == 0xe6);
static_assert(offsetof(Wall, block_count) == 0xe7);
static_assert(offsetof(Wall, block_ids) == 0xeb);
#endif

}  // namespace joan::orig
