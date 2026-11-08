# SePlayer, SeSlot, SoundDef (sound-effect playback) — joan.exe

Functions: 0x45A350, 0x4CF0F0, 0x4CEEF0, 0x45F450, 0x4CEB90 ([audio.md](../functions/audio.md)). Back end: [Sound3D](Sound3D.md).

## SePlayer (this of 0x4CF0F0; partially known)

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `volume` | multiplies def volume in 0x4CF0F0 (0x4CEEF0 uses global byte `DAT_00A57FF9` instead) | 0x4CF0F0 | R |
| 0x04,0x08,0x0C,0x10 | 4 each | Source* | `special_src[4]` | stopped by `stop_all` (not slot sources) | 0x4CEB90 | R |
| 0x14 + (n-1)*0x18 | 0x18 | SeSlot | `slot[1..12]` | slot n at `this - 4 + n*0x18` (0x4CF690) | 0x4CF690 | V |
| 0x50 | 4 | i32 | `ready` | set by 0x45A350 | | R |
| 0x54 | 4 | i32 | `enabled` | `settings[+2]` byte at init; if 0 no bank is opened | 0x45A350 | R |
| 0x64 | 4 | Options* | `options` | byte at `+2` = SFX enable | 0x45A350 | R |

SeTable (bank index, embedded) is [SeTable](SeTable.md); `A_WAVE1.RES` is opened load-all.

## SeSlot (0x18 bytes, 12 slots, 1-based)

| offset | type | name | meaning |
|---|---|---|---|
| 0x00 | Sound3DSource* | `src` | created at init (not exported); 0 = slot unavailable |
| 0x04 | i32 | `sound_id` | `def.id`, -1 when free |
| 0x08 | i32 | `flag` | 0 on start |
| 0x0C..0x17 | | | unknown |

Free slot = `src != 0` and `AL_BUFFERS_QUEUED < 1`. All 12 busy -> the request returns 0 (no stealing/priority).

## SoundDef (table A: 0x8F bytes, ids 1..0x4000 at index id-1, sorted by name; table B: 0x93 bytes, ids >= 0x4001, speech lines)

Table A comes from the `SOUND` dBase section (gameset.md schema O, 505 records) by 0x4D0DE0; table B from the `S?L1_*.RES` speech banks by 0x639560.

| offset | type | name | source column (schema O) | meaning | status |
|---|---|---|---|---|---|
| 0x04 | i16 | `id` | - | rank in the sorted table (B: 0x4001+rank) | V |
| 0x06 | char[49] | `name` | `SOUND_CODE` C48 | lookup key (B: 4-digit string id) | V |
| 0x37 | char[49] | `wave` | `WAVE_FILE` C48 | wave name in `Resource/Wave.res` (B: bank name) | V |
| 0x68 | u8 | `rand_max` | `WF_RANDMAX` N2 (via 0x4893C0) | number of wave variants | R |
| 0x69 | f32 | `gain` | `GAIN` N6.3 | multiplied by player volume and 0.01 | V |
| 0x6D | f32 | `min_pitch` | `MIN_PITCH` N5.2 | not applied in exported play paths | R |
| 0x71 | f32 | `max_pitch` | `MAX_PITCH` N5.2 | not applied | R |
| 0x75 | f32 | `ref_distance` | `MIN_DIST` N8.2 | AL_REFERENCE_DISTANCE | V |
| 0x79 | f32 | `max_distance` | `MAX_DIST` N8.2 | AL_MAX_DISTANCE | V |
| 0x7D | f32 | `rolloff` | `ROLLOFF` N7.2 | AL_ROLLOFF_FACTOR | V |
| 0x81 | i32 | `sound_time` | `SOUNDTIME` N5 | truncated to int; use unknown | R |
| 0x85 | u8 | `flags` | `LOOP_FLAG` C1 ('Y' -> bit0), `SPEED_INDE` C1 ('Y' -> bit2) | `& 3` non-zero -> AL_LOOPING | V |
| 0x86 | i16 | `delay_beg` | `DELAY_BEG` N5 | | R |
| 0x88 | i16 | `delay_end` | `DELAY_END` N5 | | R |
| 0x8A | u8 | `main_char` | `MAIN_CHAR` C1 ('Y'/'y') | | R |
| 0x8B | Sound3DBuffer* | `buffer` | - | lazily loaded (0x4D0A60) | V |
| 0x8F | i32 | `string_id` | (table B only) | `atoi(name)` | V |
