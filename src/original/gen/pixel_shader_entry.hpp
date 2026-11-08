// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PixelShaderEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PixelShaderEntry {
#include "pixel_shader_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PixelShaderEntry) == 0x8);
static_assert(offsetof(PixelShaderEntry, key) == 0x0);
static_assert(offsetof(PixelShaderEntry, handle) == 0x4);
#endif

}  // namespace joan::orig
