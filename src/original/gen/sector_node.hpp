// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SectorNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SceneS;

#pragma pack(push, 1)
struct SectorNode {
#include "sector_node_fields.inc"
#include "sector_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SectorNode) == 0x38);
static_assert(offsetof(SectorNode, parent) == 0x0);
static_assert(offsetof(SectorNode, axis) == 0x4);
static_assert(offsetof(SectorNode, left) == 0x8);
static_assert(offsetof(SectorNode, right) == 0xc);
static_assert(offsetof(SectorNode, box) == 0x10);
static_assert(offsetof(SectorNode, centre) == 0x28);
static_assert(offsetof(SectorNode, sector) == 0x34);
#endif

}  // namespace joan::orig
