// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's News class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct News : DynArray {
#include "news_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(News, flag_1d) == 0x1d);
static_assert(offsetof(News, flag_1e) == 0x1e);
static_assert(offsetof(News, dirty) == 0x1f);
static_assert(offsetof(News, cached_size) == 0x20);
#endif

}  // namespace joan::orig
