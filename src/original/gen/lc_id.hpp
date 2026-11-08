// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LcId class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LcId {
#include "lc_id_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LcId) == 0x6);
static_assert(offsetof(LcId, language) == 0x0);
static_assert(offsetof(LcId, country) == 0x2);
static_assert(offsetof(LcId, codepage) == 0x4);
#endif

}  // namespace joan::orig
