// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WmMediaType class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct VideoInfoHeader;

#pragma pack(push, 1)
struct WmMediaType {
#include "wm_media_type_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(WmMediaType) == 0x48);
static_assert(offsetof(WmMediaType, major_type) == 0x0);
static_assert(offsetof(WmMediaType, sub_type) == 0x10);
static_assert(offsetof(WmMediaType, fixed_size_samples) == 0x20);
static_assert(offsetof(WmMediaType, temporal_compression) == 0x24);
static_assert(offsetof(WmMediaType, sample_size) == 0x28);
static_assert(offsetof(WmMediaType, format_type) == 0x2c);
static_assert(offsetof(WmMediaType, unk) == 0x3c);
static_assert(offsetof(WmMediaType, format_size) == 0x40);
static_assert(offsetof(WmMediaType, format) == 0x44);
#endif

}  // namespace joan::orig
