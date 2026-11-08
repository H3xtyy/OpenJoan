// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Messages class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Messages : DynArray {
#include "messages_fields.inc"
#include "messages_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Messages, hidden) == 0x3a);
static_assert(offsetof(Messages, flag_3f) == 0x3f);
static_assert(offsetof(Messages, dirty) == 0x44);
#endif

}  // namespace joan::orig
