// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Sms class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d.hpp"
#include "label.hpp"

namespace joan::orig {

struct SmsNode;

#pragma pack(push, 1)
struct Sms {
#include "sms_fields.inc"
#include "sms_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Sms) == 0x191);
static_assert(offsetof(Sms, sound) == 0x0);
static_assert(offsetof(Sms, font) == 0x4);
static_assert(offsetof(Sms, text) == 0x8);
static_assert(offsetof(Sms, bar) == 0xbf);
static_assert(offsetof(Sms, shown) == 0x176);
static_assert(offsetof(Sms, show_time) == 0x178);
static_assert(offsetof(Sms, duration) == 0x17c);
static_assert(offsetof(Sms, u32_180) == 0x180);
static_assert(offsetof(Sms, head) == 0x184);
static_assert(offsetof(Sms, tail) == 0x188);
static_assert(offsetof(Sms, count) == 0x18c);
static_assert(offsetof(Sms, visible) == 0x190);
#endif

}  // namespace joan::orig
