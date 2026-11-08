// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DlgResEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct DlgResEntry {
#include "dlg_res_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(DlgResEntry) == 0x10);
static_assert(offsetof(DlgResEntry, id) == 0x0);
static_assert(offsetof(DlgResEntry, kind) == 0x2);
static_assert(offsetof(DlgResEntry, text) == 0x4);
static_assert(offsetof(DlgResEntry, text2) == 0x8);
static_assert(offsetof(DlgResEntry, len) == 0xc);
static_assert(offsetof(DlgResEntry, len2) == 0xe);
#endif

}  // namespace joan::orig
