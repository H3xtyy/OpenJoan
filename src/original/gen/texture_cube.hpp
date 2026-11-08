// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TextureCube class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "texture.hpp"

namespace joan::orig {

struct IDirect3DCubeTexture8;

#pragma pack(push, 1)
struct TextureCube : Texture {
#include "texture_cube_fields.inc"
#include "texture_cube_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TextureCube) == 0x5c);
static_assert(offsetof(TextureCube, d3d_texture) == 0x58);
#endif

}  // namespace joan::orig
