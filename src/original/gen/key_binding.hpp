// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's KeyBinding class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct KeyBinding {
#include "key_binding_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(KeyBinding) == 0x30);
static_assert(offsetof(KeyBinding, name) == 0x0);
static_assert(offsetof(KeyBinding, joy_button) == 0x22);
static_assert(offsetof(KeyBinding, mouse_button) == 0x23);
static_assert(offsetof(KeyBinding, key) == 0x24);
static_assert(offsetof(KeyBinding, key_alt) == 0x28);
static_assert(offsetof(KeyBinding, key_mouse) == 0x2c);
#endif

}  // namespace joan::orig
