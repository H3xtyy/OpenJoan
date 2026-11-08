// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PShader class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "pixel_shader_cache.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PShader : PixelShaderCache {
#include "pshader_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PShader) == 0xc);
#endif

}  // namespace joan::orig
