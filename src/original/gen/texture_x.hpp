// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TextureX class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "render_texture.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TextureX : RenderTexture {
#include "texture_x_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TextureX) == 0x74);
#endif

}  // namespace joan::orig
