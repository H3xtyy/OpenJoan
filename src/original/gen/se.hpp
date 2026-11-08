// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Se class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../res_idx.hpp"

namespace joan::orig {

struct Audio;
struct Top4;

#pragma pack(push, 1)
struct Se {
#include "se_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Se, entries) == 0x4);
static_assert(offsetof(Se, playing) == 0x8);
static_assert(offsetof(Se, loaded) == 0x50);
static_assert(offsetof(Se, enabled) == 0x54);
static_assert(offsetof(Se, count_main) == 0x58);
static_assert(offsetof(Se, count_ext) == 0x5c);
static_assert(offsetof(Se, count) == 0x60);
static_assert(offsetof(Se, audio) == 0x64);
static_assert(offsetof(Se, archive) == 0x68);
static_assert(offsetof(Se, ext_archive) == 0x19d);
#endif

}  // namespace joan::orig
