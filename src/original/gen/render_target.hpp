// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RenderTarget class.
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
struct RenderTarget : Texture {
#include "render_target_fields.inc"
#include "render_target_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(RenderTarget, d3d_texture) == 0x58);
static_assert(offsetof(RenderTarget, device_resource) == 0x5c);
#endif

}  // namespace joan::orig
