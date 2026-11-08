// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Buffer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PfdZone;
struct TerrainBlock;

#pragma pack(push, 1)
struct Buffer {
#include "buffer_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Buffer) == 0x20);
static_assert(offsetof(Buffer, a) == 0x0);
static_assert(offsetof(Buffer, parent) == 0x4);
static_assert(offsetof(Buffer, children) == 0x8);
static_assert(offsetof(Buffer, u32_c) == 0xc);
static_assert(offsetof(Buffer, cx) == 0x10);
static_assert(offsetof(Buffer, cy) == 0x14);
static_assert(offsetof(Buffer, cz) == 0x18);
static_assert(offsetof(Buffer, child_count) == 0x1a);
static_assert(offsetof(Buffer, capacity) == 0x1c);
static_assert(offsetof(Buffer, u8_1e) == 0x1e);
static_assert(offsetof(Buffer, u8_1f) == 0x1f);
#endif

}  // namespace joan::orig
