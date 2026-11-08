// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Slider class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Scroll;
struct UiButton;

#pragma pack(push, 1)
struct Slider {
#include "slider_fields.inc"
#include "slider_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Slider, scroll) == 0x7c);
static_assert(offsetof(Slider, down_button) == 0x80);
static_assert(offsetof(Slider, up_button) == 0x84);
static_assert(offsetof(Slider, thumb) == 0x8c);
static_assert(offsetof(Slider, repeat_acc) == 0x91);
static_assert(offsetof(Slider, hold_time) == 0x95);
#endif

}  // namespace joan::orig
