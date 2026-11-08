// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Attachment class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Attachment {
#include "attachment_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Attachment, vtable) == 0x0);
static_assert(offsetof(Attachment, pos_09) == 0x9);
static_assert(offsetof(Attachment, pos_44) == 0x44);
#endif

}  // namespace joan::orig
