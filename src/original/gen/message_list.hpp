// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MessageList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct MessageList {
#include "message_list_fields.inc"
#include "message_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(MessageList, list) == 0x0);
static_assert(offsetof(MessageList, icons) == 0x2e);
static_assert(offsetof(MessageList, u32_32) == 0x32);
static_assert(offsetof(MessageList, message_count) == 0x36);
static_assert(offsetof(MessageList, paused) == 0x3a);
static_assert(offsetof(MessageList, pause_time) == 0x3b);
static_assert(offsetof(MessageList, flag_3f) == 0x3f);
static_assert(offsetof(MessageList, message_texture) == 0x40);
static_assert(offsetof(MessageList, dirty) == 0x44);
#endif

}  // namespace joan::orig
