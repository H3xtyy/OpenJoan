// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Help class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct HelpEntry;

#pragma pack(push, 1)
struct Help {
#include "help_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Help, loaded) == 0x0);
static_assert(offsetof(Help, title) == 0x1);
static_assert(offsetof(Help, text) == 0x52);
static_assert(offsetof(Help, label) == 0x11b);
static_assert(offsetof(Help, box) == 0x124);
static_assert(offsetof(Help, box_cur) == 0x134);
static_assert(offsetof(Help, box_time) == 0x144);
static_assert(offsetof(Help, entry_count) == 0x148);
static_assert(offsetof(Help, name_first) == 0x14a);
static_assert(offsetof(Help, name_end) == 0x14c);
static_assert(offsetof(Help, num_first) == 0x14e);
static_assert(offsetof(Help, num_end) == 0x150);
static_assert(offsetof(Help, entries) == 0x152);
static_assert(offsetof(Help, text_buf) == 0x156);
static_assert(offsetof(Help, text_buf_size) == 0x15a);
static_assert(offsetof(Help, tooltip_style) == 0x15e);
#endif

}  // namespace joan::orig
