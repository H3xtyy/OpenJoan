// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VQueue class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VQueue {
#include "vqueue_fields.inc"
#include "vqueue_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VQueue) == 0x10);
static_assert(offsetof(VQueue, data) == 0x0);
static_assert(offsetof(VQueue, capacity) == 0x4);
static_assert(offsetof(VQueue, length) == 0x8);
static_assert(offsetof(VQueue, cursor) == 0xc);
#endif

}  // namespace joan::orig
