// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VsCache class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct VsEntry;
struct VsEntryPtr;

#pragma pack(push, 1)
struct VsCache {
#include "vs_cache_fields.inc"
#include "vs_cache_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(VsCache, u32_0) == 0x0);
static_assert(offsetof(VsCache, u32_4) == 0x4);
static_assert(offsetof(VsCache, entries) == 0x8);
static_assert(offsetof(VsCache, i32_c) == 0xc);
static_assert(offsetof(VsCache, i32_10) == 0x10);
static_assert(offsetof(VsCache, current) == 0x14);
static_assert(offsetof(VsCache, u32_18) == 0x18);
static_assert(offsetof(VsCache, ptr_1c) == 0x1c);
static_assert(offsetof(VsCache, slider_factors) == 0x50);
#endif

}  // namespace joan::orig
