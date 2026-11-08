// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BlkArr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "block_array.hpp"

namespace joan::orig {

struct I32Array;

#pragma pack(push, 1)
struct BlkArr : BlockArray {
#include "blk_arr_fields.inc"
#include "blk_arr_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BlkArr) == 0x30);
static_assert(offsetof(BlkArr, ids) == 0x24);
static_assert(offsetof(BlkArr, id_cap) == 0x28);
static_assert(offsetof(BlkArr, id_count) == 0x2c);
#endif

}  // namespace joan::orig
