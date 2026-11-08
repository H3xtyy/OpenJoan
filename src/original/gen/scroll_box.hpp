// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScrollBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "gadget.hpp"

namespace joan::orig {

struct BmpRes;
struct Button;
struct Scroll;

#pragma pack(push, 1)
struct ScrollBox : Gadget {
#include "scroll_box_fields.inc"
#include "scroll_box_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScrollBox) == 0x99);
static_assert(offsetof(ScrollBox, built) == 0x70);
static_assert(offsetof(ScrollBox, u32_74) == 0x74);
static_assert(offsetof(ScrollBox, u32_78) == 0x78);
static_assert(offsetof(ScrollBox, bar) == 0x7c);
static_assert(offsetof(ScrollBox, up) == 0x80);
static_assert(offsetof(ScrollBox, down) == 0x84);
static_assert(offsetof(ScrollBox, u32_88) == 0x88);
static_assert(offsetof(ScrollBox, body) == 0x8c);
static_assert(offsetof(ScrollBox, highlight) == 0x90);
static_assert(offsetof(ScrollBox, step_time) == 0x91);
static_assert(offsetof(ScrollBox, step_delay) == 0x95);
#endif

}  // namespace joan::orig
