// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Overlay2D class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;

#pragma pack(push, 1)
struct Overlay2D {
#include "overlay2d_fields.inc"
#include "overlay2d_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Overlay2D) == 0x4);
static_assert(offsetof(Overlay2D, actor) == 0x0);
#endif

}  // namespace joan::orig
