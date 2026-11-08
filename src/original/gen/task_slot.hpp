// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TaskSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TaskSlot {
#include "task_slot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TaskSlot) == 0x19);
static_assert(offsetof(TaskSlot, kind) == 0x0);
static_assert(offsetof(TaskSlot, priority) == 0x4);
static_assert(offsetof(TaskSlot, target) == 0x5);
static_assert(offsetof(TaskSlot, arg_9) == 0x9);
static_assert(offsetof(TaskSlot, arg_d) == 0xd);
static_assert(offsetof(TaskSlot, expire_time) == 0x11);
static_assert(offsetof(TaskSlot, u32_15) == 0x15);
#endif

}  // namespace joan::orig
