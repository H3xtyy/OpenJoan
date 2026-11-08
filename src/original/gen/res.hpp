// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Res class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../res_idx.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Res {
#include "res_fields.inc"
#include "res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Res, u32_54) == 0x54);
static_assert(offsetof(Res, index_base) == 0x58);
static_assert(offsetof(Res, index) == 0x68);
static_assert(offsetof(Res, extra_index) == 0x19d);
#endif

}  // namespace joan::orig
