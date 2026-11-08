// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Treaty class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Treaty {
#include "treaty_fields.inc"
#include "treaty_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Treaty) == 0x17);
static_assert(offsetof(Treaty, kind) == 0x0);
static_assert(offsetof(Treaty, arg) == 0x2);
static_assert(offsetof(Treaty, arg2) == 0x4);
static_assert(offsetof(Treaty, time) == 0xa);
static_assert(offsetof(Treaty, from) == 0xe);
static_assert(offsetof(Treaty, to) == 0xf);
static_assert(offsetof(Treaty, reply) == 0x11);
static_assert(offsetof(Treaty, u32_12) == 0x12);
#endif

}  // namespace joan::orig
