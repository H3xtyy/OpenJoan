// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexCache class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirect3DTexture8;

#pragma pack(push, 1)
struct TexCache {
#include "tex_cache_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexCache) == 0x10);
static_assert(offsetof(TexCache, texture) == 0x0);
static_assert(offsetof(TexCache, height) == 0x4);
static_assert(offsetof(TexCache, width) == 0x8);
static_assert(offsetof(TexCache, filled) == 0xc);
#endif

}  // namespace joan::orig
