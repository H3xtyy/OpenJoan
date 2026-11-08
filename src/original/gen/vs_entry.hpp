// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VsEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "shader_constants.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct VsEntry {
#include "vs_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VsEntry) == 0x80);
static_assert(offsetof(VsEntry, key_format) == 0x0);
static_assert(offsetof(VsEntry, key_flags) == 0x4);
static_assert(offsetof(VsEntry, key_extra) == 0x8);
static_assert(offsetof(VsEntry, constant_table) == 0xc);
static_assert(offsetof(VsEntry, u32_10) == 0x10);
static_assert(offsetof(VsEntry, point_lights) == 0x14);
static_assert(offsetof(VsEntry, dir_lights) == 0x18);
static_assert(offsetof(VsEntry, spot_lights) == 0x1c);
static_assert(offsetof(VsEntry, constants) == 0x20);
#endif

}  // namespace joan::orig
