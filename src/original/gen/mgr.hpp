// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Mgr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Mgr {
#include "mgr_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Mgr, vtable) == 0x0);
static_assert(offsetof(Mgr, faders) == 0x4);
#endif

}  // namespace joan::orig
