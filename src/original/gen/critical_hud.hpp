// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CriticalHud class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct CriticalHud {
#include "critical_hud_fields.inc"
#include "critical_hud_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(CriticalHud, tex_critical) == 0x0);
static_assert(offsetof(CriticalHud, tex_4) == 0x4);
static_assert(offsetof(CriticalHud, tex_criti_k) == 0x8);
static_assert(offsetof(CriticalHud, sheet_a) == 0xc);
static_assert(offsetof(CriticalHud, sheet_b) == 0x10);
static_assert(offsetof(CriticalHud, sheet_c) == 0x14);
static_assert(offsetof(CriticalHud, combo_active) == 0x18);
static_assert(offsetof(CriticalHud, combo_success) == 0x19);
static_assert(offsetof(CriticalHud, i8_1a) == 0x1a);
static_assert(offsetof(CriticalHud, i8_1b) == 0x1b);
static_assert(offsetof(CriticalHud, u8_1c) == 0x1c);
static_assert(offsetof(CriticalHud, progress) == 0x1d);
static_assert(offsetof(CriticalHud, keys) == 0x1f);
static_assert(offsetof(CriticalHud, key_count) == 0x25);
static_assert(offsetof(CriticalHud, start_time) == 0x27);
static_assert(offsetof(CriticalHud, success_time) == 0x2f);
static_assert(offsetof(CriticalHud, done) == 0x37);
static_assert(offsetof(CriticalHud, enabled) == 0x38);
static_assert(offsetof(CriticalHud, slots) == 0x39);
#endif

}  // namespace joan::orig
