// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SysEnWindow class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "dev.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SysEnWindow : Dev {
#include "sys_en_window_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SysEnWindow) == 0x20);
static_assert(offsetof(SysEnWindow, u8_14) == 0x14);
static_assert(offsetof(SysEnWindow, heap) == 0x1c);
#endif

}  // namespace joan::orig
