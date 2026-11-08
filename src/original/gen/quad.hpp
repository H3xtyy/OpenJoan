// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Quad class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Quad {
#include "quad_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Quad, v) == 0x0);
static_assert(offsetof(Quad, plane_nx) == 0x30);
static_assert(offsetof(Quad, plane_ny) == 0x34);
static_assert(offsetof(Quad, plane_nz) == 0x38);
static_assert(offsetof(Quad, plane_d) == 0x3c);
static_assert(offsetof(Quad, edges) == 0x40);
static_assert(offsetof(Quad, valid) == 0x70);
#endif

}  // namespace joan::orig
