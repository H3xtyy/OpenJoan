// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MiniResMap class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Overlay2D;
struct SceneObject;

#pragma pack(push, 1)
struct MiniResMap {
#include "mini_res_map_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MiniResMap) == 0xae);
static_assert(offsetof(MiniResMap, name) == 0x0);
static_assert(offsetof(MiniResMap, icon) == 0x40);
static_assert(offsetof(MiniResMap, kind) == 0x49);
static_assert(offsetof(MiniResMap, unit_name) == 0x4b);
static_assert(offsetof(MiniResMap, pos) == 0x8b);
static_assert(offsetof(MiniResMap, f32_97) == 0x97);
static_assert(offsetof(MiniResMap, radius) == 0x9b);
static_assert(offsetof(MiniResMap, enabled) == 0x9f);
static_assert(offsetof(MiniResMap, no_mesh) == 0xa0);
static_assert(offsetof(MiniResMap, u8_a1) == 0xa1);
static_assert(offsetof(MiniResMap, u32_a2) == 0xa2);
static_assert(offsetof(MiniResMap, object) == 0xa6);
static_assert(offsetof(MiniResMap, overlay) == 0xaa);
#endif

}  // namespace joan::orig
