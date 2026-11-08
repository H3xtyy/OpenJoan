// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's QuitPopup class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "label.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct QuitPopup {
#include "quit_popup_fields.inc"
#include "quit_popup_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(QuitPopup, texture) == 0x0);
static_assert(offsetof(QuitPopup, background) == 0x4);
static_assert(offsetof(QuitPopup, step) == 0xbb);
static_assert(offsetof(QuitPopup, question) == 0xbd);
static_assert(offsetof(QuitPopup, open) == 0xbe);
#endif

}  // namespace joan::orig
