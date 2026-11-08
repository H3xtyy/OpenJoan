// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RenderTexture class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "resource.hpp"
#include "texture.hpp"

namespace joan::orig {

struct IDirect3DTexture8;

#pragma pack(push, 1)
struct RenderTexture : Texture {
#include "render_texture_fields.inc"
#include "render_texture_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(RenderTexture) == 0x74);
static_assert(offsetof(RenderTexture, d3d_texture) == 0x58);
static_assert(offsetof(RenderTexture, device_resource) == 0x5c);
#endif

}  // namespace joan::orig
