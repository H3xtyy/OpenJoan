// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RemapKeys class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct KeyBinding;
struct KeyDesc;

#pragma pack(push, 1)
struct RemapKeys {
#include "remap_keys_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(RemapKeys) == 0x14);
static_assert(offsetof(RemapKeys, desc_count) == 0x0);
static_assert(offsetof(RemapKeys, descs) == 0x4);
static_assert(offsetof(RemapKeys, bindings) == 0x8);
static_assert(offsetof(RemapKeys, defaults) == 0xc);
static_assert(offsetof(RemapKeys, binding_count) == 0x10);
#endif

}  // namespace joan::orig
