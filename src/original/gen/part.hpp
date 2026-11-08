// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Part class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Scene;

#pragma pack(push, 1)
struct Part {
#include "part_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Part, u32_18) == 0x18);
static_assert(offsetof(Part, value) == 0x1c);
#endif

}  // namespace joan::orig
