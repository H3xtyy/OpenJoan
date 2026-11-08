// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Expiry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Expiry {
#include "expiry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Expiry) == 0x9);
static_assert(offsetof(Expiry, active) == 0x0);
static_assert(offsetof(Expiry, start) == 0x1);
static_assert(offsetof(Expiry, duration) == 0x5);
#endif

}  // namespace joan::orig
