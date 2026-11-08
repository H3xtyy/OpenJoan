// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's KeyIdArray10 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct KeyIdArray10 {
#include "key_id_array10_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(KeyIdArray10) == 0x28);
static_assert(offsetof(KeyIdArray10, ids) == 0x0);
#endif

}  // namespace joan::orig
