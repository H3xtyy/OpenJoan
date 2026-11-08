// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MessageLog class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BmpRes;
struct Button;
struct Label;
struct RenderTexture;

#pragma pack(push, 1)
struct MessageLog {
#include "message_log_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(MessageLog, open) == 0x0);
static_assert(offsetof(MessageLog, res) == 0x1);
static_assert(offsetof(MessageLog, sprites) == 0x11);
static_assert(offsetof(MessageLog, font) == 0x15);
static_assert(offsetof(MessageLog, icons) == 0x19);
static_assert(offsetof(MessageLog, prev_button) == 0x1d);
static_assert(offsetof(MessageLog, next_button) == 0x21);
static_assert(offsetof(MessageLog, turn_kind) == 0x25);
static_assert(offsetof(MessageLog, page_texture) == 0x31);
static_assert(offsetof(MessageLog, close_button) == 0x39);
static_assert(offsetof(MessageLog, title) == 0x3d);
static_assert(offsetof(MessageLog, res_41) == 0x41);
static_assert(offsetof(MessageLog, u16_45) == 0x45);
static_assert(offsetof(MessageLog, page) == 0x47);
static_assert(offsetof(MessageLog, entry) == 0x49);
static_assert(offsetof(MessageLog, page_count) == 0x4b);
static_assert(offsetof(MessageLog, mode) == 0x4d);
#endif

}  // namespace joan::orig
