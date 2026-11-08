// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Texture2D class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "texture.hpp"

namespace joan::orig {

struct IDirect3DTexture8;

#pragma pack(push, 1)
struct Texture2D : Texture {
#include "texture2d_fields.inc"
#include "texture2d_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Texture2D) == 0x5c);
static_assert(offsetof(Texture2D, d3d_texture) == 0x58);
#endif

}  // namespace joan::orig
