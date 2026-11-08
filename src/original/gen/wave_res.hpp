// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WaveRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Sound3DBuffer;

#pragma pack(push, 1)
struct WaveRes {
#include "wave_res_fields.inc"
#include "wave_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(WaveRes) == 0x93);
static_assert(offsetof(WaveRes, vtable) == 0x0);
static_assert(offsetof(WaveRes, index) == 0x4);
static_assert(offsetof(WaveRes, name) == 0x6);
static_assert(offsetof(WaveRes, file) == 0x37);
static_assert(offsetof(WaveRes, max_play) == 0x68);
static_assert(offsetof(WaveRes, volume) == 0x69);
static_assert(offsetof(WaveRes, min_pitch) == 0x6d);
static_assert(offsetof(WaveRes, max_pitch) == 0x71);
static_assert(offsetof(WaveRes, min_dist) == 0x75);
static_assert(offsetof(WaveRes, max_dist) == 0x79);
static_assert(offsetof(WaveRes, rolloff) == 0x7d);
static_assert(offsetof(WaveRes, sound_time_ms) == 0x81);
static_assert(offsetof(WaveRes, flags) == 0x85);
static_assert(offsetof(WaveRes, delay_begin) == 0x86);
static_assert(offsetof(WaveRes, delay_end) == 0x88);
static_assert(offsetof(WaveRes, stream) == 0x8a);
static_assert(offsetof(WaveRes, buffer_) == 0x8b);
static_assert(offsetof(WaveRes, priority) == 0x8f);
#endif

}  // namespace joan::orig
