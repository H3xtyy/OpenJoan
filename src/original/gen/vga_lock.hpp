// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VgaLock class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VgaLock {
#include "vga_lock_fields.inc"
#include "vga_lock_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VgaLock) == 0x4);
static_assert(offsetof(VgaLock, buffer) == 0x0);
#endif

}  // namespace joan::orig
