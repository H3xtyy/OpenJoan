// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BatchKey class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct BatchKey {
#include "batch_key_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BatchKey) == 0x14);
static_assert(offsetof(BatchKey, key) == 0x0);
static_assert(offsetof(BatchKey, vertex_format) == 0x4);
static_assert(offsetof(BatchKey, pad_5) == 0x5);
static_assert(offsetof(BatchKey, states) == 0x8);
static_assert(offsetof(BatchKey, material) == 0xc);
static_assert(offsetof(BatchKey, prim) == 0x10);
#endif

}  // namespace joan::orig
