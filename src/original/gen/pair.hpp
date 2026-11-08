// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Pair class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Triple;

#pragma pack(push, 1)
struct Pair {
#include "pair_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Pair) == 0x8);
static_assert(offsetof(Pair, a) == 0x0);
static_assert(offsetof(Pair, b) == 0x4);
#endif

}  // namespace joan::orig
