// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightningBolt class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Billboard;

#pragma pack(push, 1)
struct LightningBolt {
#include "lightning_bolt_fields.inc"
#include "lightning_bolt_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(LightningBolt, billboard) == 0x0);
static_assert(offsetof(LightningBolt, light) == 0x4);
static_assert(offsetof(LightningBolt, u32_8) == 0x8);
static_assert(offsetof(LightningBolt, u32_c) == 0xc);
static_assert(offsetof(LightningBolt, flash_time) == 0x10);
static_assert(offsetof(LightningBolt, gamma) == 0x14);
static_assert(offsetof(LightningBolt, flash) == 0x18);
#endif

}  // namespace joan::orig
