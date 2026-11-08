// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MciSetParms class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MciSetParms {
#include "mci_set_parms_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MciSetParms) == 0xc);
static_assert(offsetof(MciSetParms, callback) == 0x0);
static_assert(offsetof(MciSetParms, time_format) == 0x4);
static_assert(offsetof(MciSetParms, audio) == 0x8);
#endif

}  // namespace joan::orig
