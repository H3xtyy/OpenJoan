// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's KeyDesc class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct KeyDesc {
#include "key_desc_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(KeyDesc) == 0x28);
static_assert(offsetof(KeyDesc, code) == 0x0);
static_assert(offsetof(KeyDesc, name) == 0x4);
static_assert(offsetof(KeyDesc, type) == 0x27);
#endif

}  // namespace joan::orig
