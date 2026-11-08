// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WavHeader class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct WavHeader {
#include "wav_header_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(WavHeader) == 0x2c);
static_assert(offsetof(WavHeader, riff_id) == 0x0);
static_assert(offsetof(WavHeader, riff_size) == 0x4);
static_assert(offsetof(WavHeader, wave_id) == 0x8);
static_assert(offsetof(WavHeader, fmt_id) == 0xc);
static_assert(offsetof(WavHeader, fmt_size) == 0x10);
static_assert(offsetof(WavHeader, format_tag) == 0x14);
static_assert(offsetof(WavHeader, channels) == 0x16);
static_assert(offsetof(WavHeader, sample_rate) == 0x18);
static_assert(offsetof(WavHeader, byte_rate) == 0x1c);
static_assert(offsetof(WavHeader, block_align) == 0x20);
static_assert(offsetof(WavHeader, bits_per_sample) == 0x22);
static_assert(offsetof(WavHeader, data_id) == 0x24);
static_assert(offsetof(WavHeader, data_size) == 0x28);
#endif

}  // namespace joan::orig
