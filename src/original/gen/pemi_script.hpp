// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PemiScript class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct EffectState;

#pragma pack(push, 1)
struct PemiScript {
#include "pemi_script_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PemiScript) == 0x28);
static_assert(offsetof(PemiScript, name) == 0x0);
static_assert(offsetof(PemiScript, id) == 0x22);
static_assert(offsetof(PemiScript, effect) == 0x24);
#endif

}  // namespace joan::orig
