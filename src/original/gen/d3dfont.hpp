// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's D3DFont class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirect3DDevice8;
struct IDirect3DTexture8;
struct IDirect3DVertexBuffer8;

#pragma pack(push, 1)
struct D3DFont {
#include "d3dfont_fields.inc"
#include "d3dfont_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(D3DFont) == 0x678);
static_assert(offsetof(D3DFont, font_name) == 0x0);
static_assert(offsetof(D3DFont, font_height) == 0x50);
static_assert(offsetof(D3DFont, font_flags) == 0x54);
static_assert(offsetof(D3DFont, device) == 0x58);
static_assert(offsetof(D3DFont, texture) == 0x5c);
static_assert(offsetof(D3DFont, vertex_buffer) == 0x60);
static_assert(offsetof(D3DFont, tex_width) == 0x64);
static_assert(offsetof(D3DFont, tex_height) == 0x68);
static_assert(offsetof(D3DFont, text_scale) == 0x6c);
static_assert(offsetof(D3DFont, tex_coords) == 0x70);
static_assert(offsetof(D3DFont, saved_state_block) == 0x670);
static_assert(offsetof(D3DFont, draw_text_state_block) == 0x674);
#endif

}  // namespace joan::orig
