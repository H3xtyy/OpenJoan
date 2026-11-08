// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SceneNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SceneNode {
#include "scene_node_fields.inc"
#include "scene_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SceneNode) == 0xe4);
static_assert(offsetof(SceneNode, left) == 0x8);
static_assert(offsetof(SceneNode, right) == 0xc);
static_assert(offsetof(SceneNode, box) == 0x10);
static_assert(offsetof(SceneNode, object) == 0x34);
static_assert(offsetof(SceneNode, attachments_) == 0xce);
#endif

}  // namespace joan::orig
