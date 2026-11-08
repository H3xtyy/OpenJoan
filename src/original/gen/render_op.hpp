// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RenderOp class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "particle_key.hpp"
#include "script_color.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct RenderOp : ParticleKey {
#include "render_op_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(RenderOp) == 0x23);
static_assert(offsetof(RenderOp, value) == 0x1b);
#endif

}  // namespace joan::orig
