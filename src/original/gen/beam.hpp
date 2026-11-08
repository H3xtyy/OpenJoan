// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Beam class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../en3d.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Beam : Billboard {
#include "beam_fields.inc"
#include "beam_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Beam) == 0xbb);
static_assert(offsetof(Beam, light_slot) == 0x37);
static_assert(offsetof(Beam, from) == 0x3b);
static_assert(offsetof(Beam, from_delta) == 0x57);
static_assert(offsetof(Beam, from_steps) == 0x73);
static_assert(offsetof(Beam, from_time) == 0x77);
static_assert(offsetof(Beam, to) == 0x7b);
static_assert(offsetof(Beam, to_delta) == 0x97);
static_assert(offsetof(Beam, to_steps) == 0xb3);
static_assert(offsetof(Beam, to_time) == 0xb7);
#endif

}  // namespace joan::orig
