// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PixelShaderCache class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PixelShaderEntryPtr;

#pragma pack(push, 1)
struct PixelShaderCache {
#include "pixel_shader_cache_fields.inc"
#include "pixel_shader_cache_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PixelShaderCache) == 0xc);
static_assert(offsetof(PixelShaderCache, entries) == 0x0);
static_assert(offsetof(PixelShaderCache, capacity) == 0x4);
static_assert(offsetof(PixelShaderCache, count) == 0x8);
#endif

}  // namespace joan::orig
