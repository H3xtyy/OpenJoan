// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Callback class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Callback {
#include "callback_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Callback) == 0x10);
static_assert(offsetof(Callback, fn) == 0x0);
static_assert(offsetof(Callback, arg) == 0x4);
static_assert(offsetof(Callback, arg2) == 0x8);
static_assert(offsetof(Callback, state) == 0xc);
#endif

}  // namespace joan::orig
