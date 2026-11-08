// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BlockArray class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct I32Array;
struct U8PtrArray;

#pragma pack(push, 1)
struct BlockArray {
#include "block_array_fields.inc"
#include "block_array_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BlockArray) == 0x24);
static_assert(offsetof(BlockArray, blocks) == 0x0);
static_assert(offsetof(BlockArray, block_cap) == 0x4);
static_assert(offsetof(BlockArray, ele_size) == 0x8);
static_assert(offsetof(BlockArray, per_block) == 0xc);
static_assert(offsetof(BlockArray, count) == 0x10);
static_assert(offsetof(BlockArray, block_num) == 0x14);
static_assert(offsetof(BlockArray, free_ids) == 0x18);
static_assert(offsetof(BlockArray, free_count) == 0x1c);
static_assert(offsetof(BlockArray, free_cap) == 0x20);
#endif

}  // namespace joan::orig
