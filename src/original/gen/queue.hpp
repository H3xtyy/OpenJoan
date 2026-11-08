// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Queue class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "queue_pair.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Queue {
#include "queue_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Queue, i8_0) == 0x0);
static_assert(offsetof(Queue, i8_1) == 0x1);
static_assert(offsetof(Queue, i32_c) == 0xc);
static_assert(offsetof(Queue, i32_18) == 0x18);
static_assert(offsetof(Queue, i32_20) == 0x20);
static_assert(offsetof(Queue, u32_7a) == 0x7a);
static_assert(offsetof(Queue, pair_count) == 0x247);
static_assert(offsetof(Queue, pairs) == 0x249);
#endif

}  // namespace joan::orig
