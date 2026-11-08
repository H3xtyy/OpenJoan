// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PathRec class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct MeshVertex;

#pragma pack(push, 1)
struct PathRec {
#include "path_rec_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(PathRec) == 0x10);
static_assert(offsetof(PathRec, a) == 0x0);
static_assert(offsetof(PathRec, b) == 0x4);
static_assert(offsetof(PathRec, c) == 0x8);
static_assert(offsetof(PathRec, e) == 0xa);
static_assert(offsetof(PathRec, d) == 0xc);
#endif

}  // namespace joan::orig
