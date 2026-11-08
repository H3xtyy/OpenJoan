// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PtrList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PtrSlot;

#pragma pack(push, 1)
struct PtrList {
#include "ptr_list_fields.inc"
#include "ptr_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(PtrList, items) == 0x0);
static_assert(offsetof(PtrList, count) == 0x4);
static_assert(offsetof(PtrList, capacity) == 0x6);
static_assert(offsetof(PtrList, dirty) == 0x8);
#endif

}  // namespace joan::orig
