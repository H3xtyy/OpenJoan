// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Popup class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Font2d;

#pragma pack(push, 1)
struct Popup {
#include "popup_fields.inc"
#include "popup_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Popup, text_buf) == 0x122);
static_assert(offsetof(Popup, u8_226) == 0x226);
static_assert(offsetof(Popup, u32_444) == 0x444);
static_assert(offsetof(Popup, image) == 0x448);
static_assert(offsetof(Popup, x) == 0x44c);
static_assert(offsetof(Popup, y) == 0x44e);
static_assert(offsetof(Popup, title) == 0x450);
static_assert(offsetof(Popup, text) == 0x454);
#endif

}  // namespace joan::orig
