// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Child class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Child {
#include "child_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Child) == 0x5);
static_assert(offsetof(Child, u8_0) == 0x0);
static_assert(offsetof(Child, fit) == 0x2);
static_assert(offsetof(Child, enabled) == 0x3);
static_assert(offsetof(Child, visible) == 0x4);
#endif

}  // namespace joan::orig
