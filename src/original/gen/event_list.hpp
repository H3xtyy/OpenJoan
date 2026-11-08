// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's EventList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct AnimEvent;

#pragma pack(push, 1)
struct EventList {
#include "event_list_fields.inc"
#include "event_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(EventList) == 0x8);
static_assert(offsetof(EventList, count) == 0x0);
static_assert(offsetof(EventList, events) == 0x4);
#endif

}  // namespace joan::orig
