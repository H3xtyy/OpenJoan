// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ModelRef class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ModelRef {
#include "model_ref_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ModelRef, u32_68) == 0x68);
static_assert(offsetof(ModelRef, u32_6c) == 0x6c);
static_assert(offsetof(ModelRef, i16_84) == 0x84);
static_assert(offsetof(ModelRef, i32_b0) == 0xb0);
#endif

}  // namespace joan::orig
