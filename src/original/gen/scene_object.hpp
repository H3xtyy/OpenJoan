// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SceneObject class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "actor.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SceneObject : Actor {
#include "scene_object_fields.inc"
#include "scene_object_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SceneObject) == 0x10c);
#endif

}  // namespace joan::orig
