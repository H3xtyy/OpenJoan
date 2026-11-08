// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Triggers class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "list.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Triggers : List {
#include "triggers_fields.inc"
#include "triggers_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Triggers, u8_1d) == 0x1d);
#endif

}  // namespace joan::orig
