// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Projectile class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "model_obj.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Projectile : ModelObj {
#include "projectile_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Projectile) == 0xa3);
#endif

}  // namespace joan::orig
