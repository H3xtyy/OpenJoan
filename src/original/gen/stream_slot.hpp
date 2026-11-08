// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's StreamSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Sound3D;

#pragma pack(push, 1)
struct StreamSlot {
#include "stream_slot_fields.inc"
#include "stream_slot_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(StreamSlot) == 0x18);
static_assert(offsetof(StreamSlot, source) == 0x0);
static_assert(offsetof(StreamSlot, id) == 0x4);
static_assert(offsetof(StreamSlot, key) == 0x8);
static_assert(offsetof(StreamSlot, pos) == 0xc);
#endif

}  // namespace joan::orig
