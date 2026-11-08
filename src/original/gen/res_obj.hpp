// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ResObj class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Sound3DBuffer;

#pragma pack(push, 1)
struct ResObj {
#include "res_obj_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ResObj, vtable) == 0x0);
static_assert(offsetof(ResObj, name) == 0x6);
static_assert(offsetof(ResObj, res_name) == 0x37);
static_assert(offsetof(ResObj, buffer) == 0x8b);
static_assert(offsetof(ResObj, text) == 0x8f);
#endif

}  // namespace joan::orig
