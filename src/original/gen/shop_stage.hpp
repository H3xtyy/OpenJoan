// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ShopStage class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "scene.hpp"

namespace joan::orig {

struct Actor;
struct ActorDef;
struct Camera;

#pragma pack(push, 1)
struct ShopStage : Scene {
#include "shop_stage_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ShopStage) == 0xe9);
static_assert(offsetof(ShopStage, camera) == 0xd5);
static_assert(offsetof(ShopStage, model) == 0xd9);
static_assert(offsetof(ShopStage, u32_dd) == 0xdd);
static_assert(offsetof(ShopStage, u32_e1) == 0xe1);
static_assert(offsetof(ShopStage, model_res) == 0xe5);
#endif

}  // namespace joan::orig
