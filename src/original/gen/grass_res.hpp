// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's GrassRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct GrassType;

#pragma pack(push, 1)
struct GrassRes {
#include "grass_res_fields.inc"
#include "grass_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(GrassRes) == 0x9);
static_assert(offsetof(GrassRes, count) == 0x0);
static_assert(offsetof(GrassRes, rows) == 0x4);
static_assert(offsetof(GrassRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
