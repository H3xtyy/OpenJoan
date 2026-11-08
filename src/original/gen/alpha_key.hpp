// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AlphaKey class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AlphaKey {
#include "alpha_key_fields.inc"
#include "alpha_key_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AlphaKey) == 0x8);
static_assert(offsetof(AlphaKey, time) == 0x0);
static_assert(offsetof(AlphaKey, alpha) == 0x4);
#endif

}  // namespace joan::orig
