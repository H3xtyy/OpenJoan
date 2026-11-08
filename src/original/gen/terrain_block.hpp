// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TerrainBlock class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "buffer.hpp"

namespace joan::orig {

struct PathSpace;
struct PfdPointPtrArray;

#pragma pack(push, 1)
struct TerrainBlock {
#include "terrain_block_fields.inc"
#include "terrain_block_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(TerrainBlock, index) == 0x0);
static_assert(offsetof(TerrainBlock, u32_4) == 0x4);
static_assert(offsetof(TerrainBlock, count) == 0x28);
static_assert(offsetof(TerrainBlock, points) == 0x30);
static_assert(offsetof(TerrainBlock, links) == 0x34);
static_assert(offsetof(TerrainBlock, buffer) == 0x38);
static_assert(offsetof(TerrainBlock, key) == 0x5c);
static_assert(offsetof(TerrainBlock, kind) == 0x64);
static_assert(offsetof(TerrainBlock, key_b) == 0x66);
static_assert(offsetof(TerrainBlock, key_c) == 0x68);
static_assert(offsetof(TerrainBlock, space) == 0x6c);
#endif

}  // namespace joan::orig
