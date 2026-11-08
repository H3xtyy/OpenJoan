// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MciOpenParms class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MciOpenParms {
#include "mci_open_parms_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MciOpenParms) == 0x14);
static_assert(offsetof(MciOpenParms, callback) == 0x0);
static_assert(offsetof(MciOpenParms, device_id) == 0x4);
static_assert(offsetof(MciOpenParms, device_type) == 0x8);
static_assert(offsetof(MciOpenParms, element_name) == 0xc);
static_assert(offsetof(MciOpenParms, alias) == 0x10);
#endif

}  // namespace joan::orig
