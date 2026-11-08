// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CubeTexture class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "texture_cube.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CubeTexture : TextureCube {
#include "cube_texture_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CubeTexture) == 0x5c);
#endif

}  // namespace joan::orig
