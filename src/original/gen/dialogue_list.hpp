// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DialogueList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"

namespace joan::orig {

struct List;

#pragma pack(push, 1)
struct DialogueList {
#include "dialogue_list_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(DialogueList) == 0x36a);
static_assert(offsetof(DialogueList, open) == 0x0);
static_assert(offsetof(DialogueList, busy) == 0x1);
static_assert(offsetof(DialogueList, line_gap) == 0x2);
static_assert(offsetof(DialogueList, debug) == 0x6);
static_assert(offsetof(DialogueList, lines) == 0x7);
static_assert(offsetof(DialogueList, last_time) == 0xb);
static_assert(offsetof(DialogueList, texture) == 0xf);
static_assert(offsetof(DialogueList, debug_button) == 0x17);
static_assert(offsetof(DialogueList, x) == 0x35d);
static_assert(offsetof(DialogueList, y) == 0x35f);
static_assert(offsetof(DialogueList, w) == 0x361);
static_assert(offsetof(DialogueList, h) == 0x363);
static_assert(offsetof(DialogueList, blink) == 0x365);
static_assert(offsetof(DialogueList, blink_time) == 0x366);
#endif

}  // namespace joan::orig
