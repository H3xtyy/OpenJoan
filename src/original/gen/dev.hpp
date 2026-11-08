// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Dev class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Dev {
#include "dev_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Dev) == 0x14);
static_assert(offsetof(Dev, vtable) == 0x0);
static_assert(offsetof(Dev, instance) == 0x4);
static_assert(offsetof(Dev, hwnd) == 0x8);
static_assert(offsetof(Dev, active) == 0xc);
static_assert(offsetof(Dev, marked) == 0xd);
static_assert(offsetof(Dev, u8_e) == 0xe);
static_assert(offsetof(Dev, is_open) == 0xf);
static_assert(offsetof(Dev, u8_10) == 0x10);
static_assert(offsetof(Dev, need_paint) == 0x11);
static_assert(offsetof(Dev, closing) == 0x12);
static_assert(offsetof(Dev, u8_13) == 0x13);
#endif

}  // namespace joan::orig
