// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Lookup class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../res_idx.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Lookup {
#include "lookup_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Lookup, u32_0) == 0x0);
static_assert(offsetof(Lookup, entries) == 0x4);
static_assert(offsetof(Lookup, playing) == 0x8);
static_assert(offsetof(Lookup, u32_50) == 0x50);
static_assert(offsetof(Lookup, loaded) == 0x54);
static_assert(offsetof(Lookup, shared_base) == 0x58);
static_assert(offsetof(Lookup, count_ext) == 0x5c);
static_assert(offsetof(Lookup, u32_60) == 0x60);
static_assert(offsetof(Lookup, owner) == 0x64);
static_assert(offsetof(Lookup, index) == 0x68);
static_assert(offsetof(Lookup, shared_index) == 0x19d);
#endif

}  // namespace joan::orig
