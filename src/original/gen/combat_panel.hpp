// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CombatPanel class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct CombatPanel {
#include "combat_panel_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CombatPanel, tex_iattcomb) == 0x0);
static_assert(offsetof(CombatPanel, tex_commele) == 0x4);
#endif

}  // namespace joan::orig
