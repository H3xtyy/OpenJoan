// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexProjector class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct TexProjector {
#include "tex_projector_fields.inc"
#include "tex_projector_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexProjector) == 0x30);
static_assert(offsetof(TexProjector, u32_0) == 0x0);
static_assert(offsetof(TexProjector, params) == 0x4);
static_assert(offsetof(TexProjector, texture) == 0x24);
static_assert(offsetof(TexProjector, rate) == 0x28);
static_assert(offsetof(TexProjector, item_index) == 0x2c);
#endif

}  // namespace joan::orig
