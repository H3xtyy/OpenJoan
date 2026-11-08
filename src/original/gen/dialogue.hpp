// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Dialogue class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../gameset.hpp"

namespace joan::orig {

struct List;
struct Speech;

#pragma pack(push, 1)
struct Dialogue {
#include "dialogue_fields.inc"
#include "dialogue_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Dialogue, text) == 0x191);
static_assert(offsetof(Dialogue, speech_text) == 0x322);
static_assert(offsetof(Dialogue, speech_len) == 0x326);
static_assert(offsetof(Dialogue, x) == 0x328);
static_assert(offsetof(Dialogue, y) == 0x32a);
static_assert(offsetof(Dialogue, w) == 0x32c);
static_assert(offsetof(Dialogue, h) == 0x32e);
static_assert(offsetof(Dialogue, shown) == 0x330);
static_assert(offsetof(Dialogue, done) == 0x334);
static_assert(offsetof(Dialogue, visible) == 0x335);
static_assert(offsetof(Dialogue, mode) == 0x336);
static_assert(offsetof(Dialogue, portrait) == 0x337);
static_assert(offsetof(Dialogue, i16_340) == 0x340);
static_assert(offsetof(Dialogue, u8_342) == 0x342);
static_assert(offsetof(Dialogue, unit_id) == 0x343);
static_assert(offsetof(Dialogue, speech) == 0x347);
static_assert(offsetof(Dialogue, font) == 0x34b);
static_assert(offsetof(Dialogue, sub_lines) == 0x34f);
static_assert(offsetof(Dialogue, choice) == 0x353);
#endif

}  // namespace joan::orig
