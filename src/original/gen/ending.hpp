// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Ending class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d.hpp"
#include "label.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Ending {
#include "ending_fields.inc"
#include "ending_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Ending) == 0x236);
static_assert(offsetof(Ending, picture) == 0x0);
static_assert(offsetof(Ending, picture2) == 0x4);
static_assert(offsetof(Ending, title) == 0x8);
static_assert(offsetof(Ending, text) == 0xbf);
static_assert(offsetof(Ending, frame) == 0x176);
static_assert(offsetof(Ending, u32_22d) == 0x22d);
static_assert(offsetof(Ending, score) == 0x231);
static_assert(offsetof(Ending, kind) == 0x233);
static_assert(offsetof(Ending, is_open_) == 0x234);
static_assert(offsetof(Ending, u8_235) == 0x235);
#endif

}  // namespace joan::orig
