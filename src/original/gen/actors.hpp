// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Actors class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct ActorDef;
struct AnimDef;

#pragma pack(push, 1)
struct Actors : DynArrayB {
#include "actors_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Actors, default_def) == 0x33);
static_assert(offsetof(Actors, default_anim) == 0x37);
#endif

}  // namespace joan::orig
