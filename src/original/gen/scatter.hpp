// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Scatter class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Scatter {
#include "scatter_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Scatter, u32_0) == 0x0);
static_assert(offsetof(Scatter, f32_6c) == 0x6c);
static_assert(offsetof(Scatter, f32_70) == 0x70);
static_assert(offsetof(Scatter, u32_74) == 0x74);
static_assert(offsetof(Scatter, f32_78) == 0x78);
static_assert(offsetof(Scatter, f32_7c) == 0x7c);
#endif

}  // namespace joan::orig
