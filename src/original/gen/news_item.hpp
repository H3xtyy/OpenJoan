// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NewsItem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct NewsItem {
#include "news_item_fields.inc"
#include "news_item_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(NewsItem) == 0x52);
static_assert(offsetof(NewsItem, kind) == 0x0);
static_assert(offsetof(NewsItem, side) == 0x2);
static_assert(offsetof(NewsItem, unit) == 0xc);
static_assert(offsetof(NewsItem, player) == 0x10);
static_assert(offsetof(NewsItem, date) == 0x14);
static_assert(offsetof(NewsItem, count) == 0x18);
static_assert(offsetof(NewsItem, value) == 0x1c);
static_assert(offsetof(NewsItem, name) == 0x2c);
static_assert(offsetof(NewsItem, arg_b) == 0x41);
static_assert(offsetof(NewsItem, arg_w1) == 0x42);
static_assert(offsetof(NewsItem, arg_w2) == 0x44);
static_assert(offsetof(NewsItem, args) == 0x46);
#endif

}  // namespace joan::orig
