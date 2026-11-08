// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's QueuePair class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct QueuePair {
#include "queue_pair_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(QueuePair) == 0x8);
static_assert(offsetof(QueuePair, key) == 0x0);
static_assert(offsetof(QueuePair, b) == 0x4);
static_assert(offsetof(QueuePair, c) == 0x6);
#endif

}  // namespace joan::orig
