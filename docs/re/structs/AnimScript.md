# AnimScript (animscr.cpp record, sizeof = 0x21B = 539) — joan.exe

One record per payload of `Resource/animscr.res` (347 records), array allocated by `animscr_load` 0x418F30 as `count * 0x21B`, 1-based
(`animscr_record(recno)` 0x416F20 = `base + (recno-1)*0x21B`). Zero-filled before parsing. Grammar and examples: [formats/animscr.md](../../formats/animscr.md).
Function notes: [functions/script_res.md](../functions/script_res.md) §4. The table header object (the "AnimScrRes"): `+0 AnimScript* records`, `+4 i16 count`, `+6 u8 initialised`.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 2 | u16 | recno | 1-based index = ResLX payload index | 0x418F30 |
| 0x02 | 0x20 | char[32] | code | value of `Code` key, `strncpy(…, 0x20)`; may fill all 32 bytes (longest shipped name is 32) so it is only NUL-terminated by the zero at 0x22 | 0x419320 |
| 0x22 | 2 | – | (pad) | always 0 | inferred |
| 0x24 | 4 | i32 | refcount | decremented by `release` 0x41A020 (async load/unload of attach model); 0 after load | 0x41A020 |
| 0x28 | 4 | i32 | start_time | `StartTime`, ms; hit-event delay (`now + start_time`) | 0x419320, 0x501190 |
| 0x2C | 4 | i32 | end_time | `EndTime`, ms | 0x419320 |
| 0x30 | 4 | i32 | range | `Range`; added to weapon reach | 0x419320, 0x501190 |
| 0x34 | 4 | i32 | start_force_time | `StartForceTime`, ms | 0x419320 |
| 0x38 | 4 | i32 | start_vertical_angle | `StartVerticalAngle`, degrees | 0x419320 |
| 0x3C | 4 | i32 | end_vertical_angle | `EndVerticalAngle` | 0x419320 |
| 0x40 | 4 | i32 | start_horizontal_angle | `StartHorizontalAngle` | 0x419320 |
| 0x44 | 4 | i32 | end_horizontal_angle | `EndHorizontalAngle` | 0x419320 |
| 0x48 | 4 | f32 | gravity_level | `GravityLevel` (atof); default **3.0f** (bit pattern 0x40400000) | 0x419320 |
| 0x4C | 4 | i32 | attack_force | `AttackForce`; default **3000** | 0x419320 |
| 0x50 | 4 | i32 | vertical_attack_force | `VerticalAttackForce`; default **500** | 0x419320 |
| 0x54 | 0x21 | char[33] | sound_script_name | `SoundScriptName` (`strcpy`, "none" in every shipped record); ends at 0x74 | 0x419320 |
| 0x75 | 1 | u8 | emitter_flag | `EmitterFlag` (atoi); only `== 1` triggers the emitter sub-block | 0x419320, 0x418C60 |
| 0x76 | 1 | u8 | terrain_emitter_flag | `TerrainEmitterFlag` (0, 1, 2 in shipped data); non-zero adds the `min_separate_dist` field and the `_default` suffix | 0x419320 |
| 0x77 | 6 x 0x21 | char[6][33] | emitter_name | `EmitterFileName`, one per slot (pscript file stem, or `none`); ends at 0x13D | 0x419320 |
| 0x13D | 6 x 4 | i32[6] | emitter_start_time | `EmitterStartTime`, ms | 0x419320 |
| 0x155 | 6 x 4 | i32[6] | emitter_end_time | `EmitterEndTime`, ms | 0x419320 |
| 0x16D | 6 x 4 | i32[6] | emitter_min_separate_dist | `EmitterMinSeparateDist`; read **only if terrain_emitter_flag != 0**, else stays 0 | 0x419320 |
| 0x185 | 6 x 12 | f32[6][3] | emitter_pos | `EmitterX`, `EmitterY`, `EmitterZ` at +0/+4/+8 of each 12-byte slot; parsed with `atoi` and converted to float (fractions are lost) | 0x419320 |
| 0x1CD | 6 | u8[6] | emitter_lock_type | `EmitterLockType` | 0x419320 |
| 0x1D3 | 6 | u8[6] | emitter_weapon_type | `EmitterWeaponType` (slot 2 is tested at `rec+0x1D5` by 0x501190 to pick a held item) | 0x419320, 0x501190 |
| 0x1D9 | 4 | u32 | attach_model | `AttachModel` model handle from `modelres_load("ITEM\\", name, 0)` 0x4B18D0; 0 = none (key unused in shipped file) | 0x419320, 0x41A020 |
| 0x1DD | 4 | u32 | attach_aux | handle released together with `attach_model`; not written by the loader (stays 0) | 0x41A020 |
| 0x1E1 | 0x21 | char[33] | attach_bone | rest-of-line value of the second `AttachModel` row; ends at 0x202 | 0x419320 |
| 0x202 | 1 | char | attach_side | first character of the third `AttachModel` value (`strncpy(…,1)`) | 0x419320 |
| 0x203 | 6 x 4 | u32[6] | emitter_handle | pemires index+1 from `pemires_find(name [+"_default"] + suffix, 1)` 0x4C2060, 0 if the slot is `none`; filled by `animscr_resolve_emitters` 0x418C60 | 0x418C60 |

Check: 0x203 + 0x18 = 0x21B = sizeof. Status: **verified** (each offset is written by one function and the array extents tile exactly), except `refcount`/`attach_aux` (read in 0x41A020 only).
