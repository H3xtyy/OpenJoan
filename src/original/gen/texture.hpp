// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Texture class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct FileLoader;
struct IDirect3DBaseTexture8;
struct TextureManager;

#pragma pack(push, 1)
struct Texture {
#include "texture_fields.inc"
#include "texture_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Texture) == 0x58);
static_assert(offsetof(Texture, vtable) == 0x0);
static_assert(offsetof(Texture, d3d_base) == 0x4);
static_assert(offsetof(Texture, usage) == 0x8);
static_assert(offsetof(Texture, ref_count) == 0xc);
static_assert(offsetof(Texture, last_used) == 0x10);
static_assert(offsetof(Texture, manager) == 0x14);
static_assert(offsetof(Texture, name) == 0x18);
static_assert(offsetof(Texture, width) == 0x3c);
static_assert(offsetof(Texture, height) == 0x40);
static_assert(offsetof(Texture, levels) == 0x44);
static_assert(offsetof(Texture, state) == 0x48);
static_assert(offsetof(Texture, alpha_bits) == 0x4c);
static_assert(offsetof(Texture, loader) == 0x50);
static_assert(offsetof(Texture, priority) == 0x54);
#endif

}  // namespace joan::orig
