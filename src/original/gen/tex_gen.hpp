// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexGen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TexGen {
#include "tex_gen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(TexGen, disabled) == 0x0);
static_assert(offsetof(TexGen, position) == 0x4);
static_assert(offsetof(TexGen, orbit) == 0x10);
static_assert(offsetof(TexGen, width) == 0x1c);
static_assert(offsetof(TexGen, height) == 0x20);
static_assert(offsetof(TexGen, texture) == 0x24);
static_assert(offsetof(TexGen, f32_28) == 0x28);
static_assert(offsetof(TexGen, f32_2c) == 0x2c);
#endif

}  // namespace joan::orig
