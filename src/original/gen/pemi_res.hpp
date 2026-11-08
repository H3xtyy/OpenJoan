// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PemiRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PemiScript;

#pragma pack(push, 1)
struct PemiRes {
#include "pemi_res_fields.inc"
#include "pemi_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PemiRes) == 0x9);
static_assert(offsetof(PemiRes, count) == 0x0);
static_assert(offsetof(PemiRes, scripts) == 0x4);
static_assert(offsetof(PemiRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
