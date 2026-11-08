// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Chain class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Boid;

#pragma pack(push, 1)
struct Chain {
#include "chain_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Chain) == 0xe);
static_assert(offsetof(Chain, flock_id) == 0x0);
static_assert(offsetof(Chain, index) == 0x2);
static_assert(offsetof(Chain, count) == 0x6);
static_assert(offsetof(Chain, head) == 0xa);
#endif

}  // namespace joan::orig
