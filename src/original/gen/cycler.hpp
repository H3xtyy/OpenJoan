// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Cycler class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Cycler {
#include "cycler_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Cycler, items) == 0x0);
static_assert(offsetof(Cycler, count) == 0x4);
static_assert(offsetof(Cycler, u16_6) == 0x6);
static_assert(offsetof(Cycler, sorted) == 0x8);
static_assert(offsetof(Cycler, prev_key) == 0x9);
static_assert(offsetof(Cycler, next_key) == 0xa);
static_assert(offsetof(Cycler, current) == 0xb);
#endif

}  // namespace joan::orig
