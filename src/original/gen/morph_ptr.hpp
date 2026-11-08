// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MorphPtr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Morph;

#pragma pack(push, 1)
struct MorphPtr {
#include "morph_ptr_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MorphPtr) == 0x4);
static_assert(offsetof(MorphPtr, p) == 0x0);
#endif

}  // namespace joan::orig
