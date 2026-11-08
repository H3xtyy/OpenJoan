// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SoundBuf class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SoundBuf {
#include "sound_buf_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(SoundBuf, ptr_0) == 0x0);
static_assert(offsetof(SoundBuf, i32_4) == 0x4);
static_assert(offsetof(SoundBuf, i32_8) == 0x8);
static_assert(offsetof(SoundBuf, i32_c) == 0xc);
static_assert(offsetof(SoundBuf, ptr_10) == 0x10);
static_assert(offsetof(SoundBuf, i32_14) == 0x14);
static_assert(offsetof(SoundBuf, ptr_1c) == 0x1c);
static_assert(offsetof(SoundBuf, i32_24) == 0x24);
#endif

}  // namespace joan::orig
