// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MessageIndicator class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct MessageIndicator {
#include "message_indicator_fields.inc"
#include "message_indicator_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MessageIndicator) == 0xd);
static_assert(offsetof(MessageIndicator, active) == 0x0);
static_assert(offsetof(MessageIndicator, texture) == 0x5);
static_assert(offsetof(MessageIndicator, sprites) == 0x9);
#endif

}  // namespace joan::orig
