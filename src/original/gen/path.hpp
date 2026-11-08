// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Path class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Billboard;

#pragma pack(push, 1)
struct Path {
#include "path_fields.inc"
#include "path_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Path, ptr_c) == 0xc);
static_assert(offsetof(Path, u32_14) == 0x14);
static_assert(offsetof(Path, u32_18) == 0x18);
static_assert(offsetof(Path, u32_1c) == 0x1c);
static_assert(offsetof(Path, u32_20) == 0x20);
static_assert(offsetof(Path, i32_28) == 0x28);
static_assert(offsetof(Path, u32_2c) == 0x2c);
static_assert(offsetof(Path, ptr_30) == 0x30);
static_assert(offsetof(Path, ptr_34) == 0x34);
static_assert(offsetof(Path, i32_39) == 0x39);
static_assert(offsetof(Path, i32_3d) == 0x3d);
static_assert(offsetof(Path, u32_6c) == 0x6c);
static_assert(offsetof(Path, u8_104) == 0x104);
static_assert(offsetof(Path, u8_125) == 0x125);
static_assert(offsetof(Path, i32_258) == 0x258);
#endif

}  // namespace joan::orig
