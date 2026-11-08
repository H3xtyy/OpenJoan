// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Key class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Key {
#include "key_fields.inc"
#include "key_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Key, code) == 0x0);
static_assert(offsetof(Key, kind) == 0x4);
static_assert(offsetof(Key, u32_5) == 0x5);
static_assert(offsetof(Key, u32_9) == 0x9);
static_assert(offsetof(Key, u32_d) == 0xd);
#endif

}  // namespace joan::orig
