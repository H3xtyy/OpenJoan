// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BmpRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BmpEntry;

#pragma pack(push, 1)
struct BmpRes {
#include "bmp_res_fields.inc"
#include "bmp_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BmpRes) == 0x9);
static_assert(offsetof(BmpRes, loaded) == 0x0);
static_assert(offsetof(BmpRes, count) == 0x1);
static_assert(offsetof(BmpRes, entries) == 0x5);
#endif

}  // namespace joan::orig
