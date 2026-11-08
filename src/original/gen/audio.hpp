// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Audio class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../res_idx.hpp"
#include "../res_idx.hpp"

namespace joan::orig {

struct IDirectSound;
struct IDirectSoundBuffer;
struct WavHeader;

#pragma pack(push, 1)
struct Audio {
#include "audio_fields.inc"
#include "audio_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Audio) == 0x3aa);
static_assert(offsetof(Audio, init_flag) == 0x0);
static_assert(offsetof(Audio, mid_init_flag) == 0x1);
static_assert(offsetof(Audio, wav_init_flag) == 0x2);
static_assert(offsetof(Audio, cd_init_flag) == 0x3);
static_assert(offsetof(Audio, mid_flag) == 0x4);
static_assert(offsetof(Audio, wav_flag) == 0x5);
static_assert(offsetof(Audio, cd_flag) == 0x6);
static_assert(offsetof(Audio, mid_buf) == 0x7);
static_assert(offsetof(Audio, wav_buf) == 0xb);
static_assert(offsetof(Audio, mid_buf_size) == 0xf);
static_assert(offsetof(Audio, wav_buf_size) == 0x13);
static_assert(offsetof(Audio, wav_res) == 0x17);
static_assert(offsetof(Audio, wav_res2) == 0x14c);
static_assert(offsetof(Audio, direct_sound) == 0x281);
static_assert(offsetof(Audio, wav_dsb) == 0x285);
static_assert(offsetof(Audio, wav_serial_no) == 0x2a5);
static_assert(offsetof(Audio, max_wav_serial_no) == 0x2c5);
static_assert(offsetof(Audio, wav_start_time) == 0x2c9);
static_assert(offsetof(Audio, wav_duration) == 0x2e9);
static_assert(offsetof(Audio, lwav_dsb) == 0x309);
static_assert(offsetof(Audio, lwav_serial_no) == 0x319);
static_assert(offsetof(Audio, max_lwav_serial_no) == 0x329);
static_assert(offsetof(Audio, lwav_file) == 0x32d);
static_assert(offsetof(Audio, lwav_bank) == 0x33d);
static_assert(offsetof(Audio, lwav_bank_size) == 0x345);
static_assert(offsetof(Audio, wav_volume) == 0x34d);
static_assert(offsetof(Audio, stream_pending) == 0x351);
static_assert(offsetof(Audio, loop_dsb) == 0x352);
static_assert(offsetof(Audio, loop_file) == 0x362);
static_assert(offsetof(Audio, loop_start) == 0x372);
static_assert(offsetof(Audio, loop_bank) == 0x382);
static_assert(offsetof(Audio, loop_fade_time) == 0x38a);
static_assert(offsetof(Audio, loop_fade_volume) == 0x39a);
#endif

}  // namespace joan::orig
