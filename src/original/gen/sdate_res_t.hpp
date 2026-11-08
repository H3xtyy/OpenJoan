// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SdateRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SdateEntry;

#pragma pack(push, 1)
struct SdateRes {
#include "sdate_res_t_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SdateRes) == 0x7);
static_assert(offsetof(SdateRes, dates) == 0x0);
static_assert(offsetof(SdateRes, count) == 0x4);
static_assert(offsetof(SdateRes, loaded) == 0x6);
#endif

}  // namespace joan::orig
