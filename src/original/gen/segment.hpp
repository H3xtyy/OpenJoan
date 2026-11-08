// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Segment class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct P2;

#pragma pack(push, 1)
struct Segment {
#include "segment_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Segment) == 0x8);
static_assert(offsetof(Segment, a) == 0x0);
static_assert(offsetof(Segment, b) == 0x4);
#endif

}  // namespace joan::orig
