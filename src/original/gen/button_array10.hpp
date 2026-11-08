// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ButtonArray10 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Button;

#pragma pack(push, 1)
struct ButtonArray10 {
#include "button_array10_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ButtonArray10) == 0x28);
static_assert(offsetof(ButtonArray10, buttons) == 0x0);
#endif

}  // namespace joan::orig
