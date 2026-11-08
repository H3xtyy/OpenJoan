// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WaveFormatEx class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct WaveFormatEx {
#include "wave_format_ex_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(WaveFormatEx) == 0x12);
static_assert(offsetof(WaveFormatEx, format_tag) == 0x0);
static_assert(offsetof(WaveFormatEx, channels) == 0x2);
static_assert(offsetof(WaveFormatEx, samples_per_sec) == 0x4);
static_assert(offsetof(WaveFormatEx, avg_bytes_per_sec) == 0x8);
static_assert(offsetof(WaveFormatEx, block_align) == 0xc);
static_assert(offsetof(WaveFormatEx, bits_per_sample) == 0xe);
static_assert(offsetof(WaveFormatEx, cb_size) == 0x10);
#endif

}  // namespace joan::orig
