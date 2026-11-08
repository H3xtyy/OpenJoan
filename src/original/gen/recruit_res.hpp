// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RecruitRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct RecruitEntry;

#pragma pack(push, 1)
struct RecruitRes {
#include "recruit_res_fields.inc"
#include "recruit_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(RecruitRes) == 0x9);
static_assert(offsetof(RecruitRes, count) == 0x0);
static_assert(offsetof(RecruitRes, entries) == 0x4);
static_assert(offsetof(RecruitRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
