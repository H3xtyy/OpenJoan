// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RenderView class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Bone;
struct Fog;
struct Reorder;

#pragma pack(push, 1)
struct RenderView {
#include "render_view_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(RenderView, bone) == 0x0);
static_assert(offsetof(RenderView, fog) == 0x54);
static_assert(offsetof(RenderView, reorder) == 0xdc);
#endif

}  // namespace joan::orig
