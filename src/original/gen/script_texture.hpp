// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptTexture class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "resource.hpp"

namespace joan::orig {

struct IUnknown;

#pragma pack(push, 1)
struct ScriptTexture {
#include "script_texture_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ScriptTexture, d3d_a) == 0x90);
static_assert(offsetof(ScriptTexture, d3d_b) == 0x94);
static_assert(offsetof(ScriptTexture, device_resource) == 0x98);
#endif

}  // namespace joan::orig
