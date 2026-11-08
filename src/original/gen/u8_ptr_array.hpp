// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's U8PtrArray class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct U8PtrArray {
#include "u8_ptr_array_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(U8PtrArray) == 0x4);
static_assert(offsetof(U8PtrArray, buffer) == 0x0);
#endif

}  // namespace joan::orig
