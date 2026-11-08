// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DlgRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DlgResEntry;

#pragma pack(push, 1)
struct DlgRes {
#include "dlg_res_fields.inc"
#include "dlg_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(DlgRes) == 0xf);
static_assert(offsetof(DlgRes, loaded) == 0x0);
static_assert(offsetof(DlgRes, count_) == 0x1);
static_assert(offsetof(DlgRes, entries) == 0x3);
static_assert(offsetof(DlgRes, data) == 0x7);
static_assert(offsetof(DlgRes, data_size) == 0xb);
#endif

}  // namespace joan::orig
