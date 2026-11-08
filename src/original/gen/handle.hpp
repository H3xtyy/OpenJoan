// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Handle class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Handle {
#include "handle_fields.inc"
#include "handle_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Handle, u32_0) == 0x0);
static_assert(offsetof(Handle, i32_64) == 0x64);
static_assert(offsetof(Handle, refs) == 0x68);
static_assert(offsetof(Handle, ptr_6c) == 0x6c);
static_assert(offsetof(Handle, ptr_70) == 0x70);
#endif

}  // namespace joan::orig
