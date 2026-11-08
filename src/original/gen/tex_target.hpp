// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexTarget class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "texture_bump.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TexTarget : TextureBump {
#include "tex_target_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexTarget) == 0x5c);
#endif

}  // namespace joan::orig
