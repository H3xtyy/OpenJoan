// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Triple class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Triple {
#include "triple_fields.inc"
#include "triple_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Triple) == 0xc);
static_assert(offsetof(Triple, a) == 0x0);
static_assert(offsetof(Triple, b) == 0x4);
static_assert(offsetof(Triple, c) == 0x8);
#endif

}  // namespace joan::orig
