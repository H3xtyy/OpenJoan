# audio family — `ilib\audio.cpp`, `ilib\sound3d.cpp`, `ilib\se.cpp`, `musicres.cpp`, `speech_s.cpp`

Reference build: `joan.exe`. Source of truth: `decomp/engine/audio/` (116 functions, depth <= 2; `INDEX.md`).

Structs: [AudioDevice](../structs/AudioDevice.md), [Sound3D](../structs/Sound3D.md) (manager + `Sound3DSource` + `Sound3DBuffer`),
[SeTable](../structs/SeTable.md), [MusicEntry](../structs/MusicEntry.md), [SpeechScript](../structs/SpeechScript.md).
Container formats are not repeated here: [res.md](../../formats/res.md) (ResLX banks `A_wave1/2.res`, `Wave.res`, `S?L1_*.RES` — payloads are
complete RIFF/WAVE files), [gameset.md](../../formats/gameset.md) schema R (the `MUSIC` dBase table), [ResLX](../structs/ResLX.md),
[ResX](../structs/ResX.md), [Db](../structs/Db.md). `docs/formats/wav.md` is unrelated (scenario `.wav` track lists).

**Big picture (important correction to the module summary):** the exported audio code contains THREE different back ends.
1. `audio.cpp` = **DirectSound 8-style (ordinal import `DSOUND #1` = DirectSoundCreate) + WinMM MCI CD-audio + WinMM mixer**, not OpenAL.
2. `sound3d.cpp` = a thin **OpenAL 1.0 wrapper** (Creative OpenAL32, device string `"DirectSound3D"`), objects for buffers, sources, listener.
3. `se.cpp` / `musicres.cpp` / `speech_s.cpp` = **data tables** (sound-effect bank index, music dBase table, speech text script).
No EAX property-set call exists in any exported function (see Open questions). The playback/trigger code (play SE by name, 3D emit, music
player, speech queue) is NOT in the export: only constructors/destructors/loaders were attributed by assert strings.

Confidence: V verified (>= 2 consistent reads), R read once, I inferred.

## audio.cpp — `AudioDevice` (DirectSound + MCI + mixer)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x0057A440 | `audio_init` (fastcall, this=AudioDevice) | zero state; try DirectSound init; if ok open `RESOURCE\A_WAVE2.RES` via ResLX::open(name,0,0) and alloc 0x8000-byte scratch; open MCI CD audio; read CD mixer volume into `DAT_00a150e0`; set `ready = dsound_ok \|\| cd_ok`; always returns 1 | strings `RESOURCE\\A_WAVE2.RES`, `audio.cpp`; calls 0x57A740, 0x57A810, 0x57EBF0, 0x59B920 | V |
| 0x0057A740 | `audio_dsound_create` | `DirectSoundCreate(NULL,&this->ds,NULL)` (DSOUND ordinal 1) then `IDirectSound::SetCooperativeLevel(hwnd=DAT_00a50090, level)` (vtbl+0x18; level passed is **1 = DSSCL_NORMAL**); sets `ds_ok` | `Ordinal_1`, vtbl +0x18, arg 1 | V |
| 0x0057A810 | `audio_mci_open_cd` | `mciSendCommand(0, MCI_OPEN 0x803, flags 0x3000, parms@0xA642E8)` with device-type id 0x204 (MCI_DEVTYPE_CD_AUDIO); on failure retry with flags 0x3100; then `MCI_SET 0x80D / MCI_SET_TIME_FORMAT 0x400` with `MCI_FORMAT_TMSF` (10); sets `cd_ok` | WinMM constants | V |
| 0x0057EBF0 | `audio_mixer_get_cd_volume` | opens mixer 0, line type `0x1005` (MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC), control type `0x50030001` (volume), reads current value (1 channel, 4 bytes); returns it or `0xFFFFFFFF` on any failure. READ ONLY (original never writes the mixer in this export) | `mixerGetLineInfoA`, `mixerGetLineControlsA`, `mixerGetControlDetailsA` | V |
| 0x0057A920 | `audio_shutdown_ds` | `audio_stop_all_voices` (0x57DD30), free scratch buffer (+0xB), `IDirectSound::Release` | audio.cpp assert line | V |
| 0x0057A9C0 | `audio_shutdown_music` | if `music_open (+1)`: MCI stop, free music buffer (+7), clear flag | audio.cpp | R |
| 0x0057AA30 | `audio_load_music_resource(name)` (thiscall) | if `music_open && music_enabled`: MCI stop (0x57AB20); look the name up in a **ResX** (13-byte entries; `FUN_0059CE40` -> `FUN_0059CA60` = binary search on a sorted name index, case sensitive `strcmp`, returns idx+1 or 0), compute payload size from consecutive offsets, grow the music buffer (+7, +0xF = capacity, via tracked realloc 0x58EB30) and read the payload into it. No playback call inside | audio.cpp; 0x59CE40/0x59CA60 stride 0xD | R |
| 0x0057AB20 | `audio_mci_stop` | if `music_open && music_enabled`: `mciSendCommand(DAT_00a642ec, MCI_STOP 0x808,0,0)` | WinMM | V |
| 0x0057BCA0 | `audio_play_wave_file(path, ?, volume, pan)` (thiscall, 2114 B) | opens a loose wave file by path (relative to game dir, `Cannot open %s` message box on failure); reads first 0x80 bytes; `FUN_0057B030` finds the `data` chunk inside them; picks a DirectSound buffer size (see Constants); finds a free slot among **4** streaming voices (slot free = null or `IDirectSoundBuffer::GetStatus` bit0 (playing) clear; a finished slot is Released + its file object deleted first); `CreateSoundBuffer` with `DSBUFFERDESC{size 0x14, flags 0x100E0, bytes = 4*chunk, wfx = file header+0x14}`; Lock/memcpy first buffer-full of PCM (zero-fills tail if the file is shorter, then plays non-looping), Unlock, `SetVolume(param_3)` (vtbl+0x3C), `SetPan(param_4)` (vtbl+0x40), `Play(0,0,looping?)` (vtbl+0x30; flag 1 = DSBPLAY_LOOPING only while the file still has more data); on success stamps `has_played (+0x351)=1` and returns a new voice handle (see 0x57EE10). Refill/loop service for the remaining data is elsewhere (TODO) | DS vtbl offsets 0x2C/0x30/0x3C/0x40/0x4C; string `Cannot open %s` | R |
| 0x0057B030 | `wav_find_data_chunk` | walks RIFF chunks starting at `buf+0x14+fmtLen` until tag `data`; gives up if the walk exceeds 0x80 bytes | `_strncmp(...,"data",4)`, `0x80` | V |
| 0x0057EE10 | `audio_next_voice_id` | monotonically increasing voice handle: `++*counter`, wraps `0x7FFFFFFF -> 1` (0 = invalid). Counter at `+0x329` | code | V |
| 0x0057DD30 | `audio_stop_all_voices` | for the 8 one-shot slots (+0x285): `Stop`(vtbl+0x48) + `Release`; then 0x57DE40 (4 streaming voices) and 0x57DF80 for slots 1..4 (the 4 channel voices at +0x352) | DS vtbl +0x48/+8 | V |
| 0x0057DE40 | `audio_stop_stream_voices` | stop/release the 4 streaming voices (+0x309) and delete their file objects (+0x32D) | | V |
| 0x0057DF80 | `audio_stop_channel(n)` | stop/release channel voice `n-1` (+0x352 / file +0x362) | | V |
| 0x0057C700 | `file_delete(1)` (scalar deleting dtor of the 0x10E-byte file object) | calls 0x5872B0 (close) then `operator delete` when flag&1 | | V |
| 0x004179C0 | `file_ctor` | `handle (+0x105) = INVALID_HANDLE_VALUE (-1)`; object is 0x10E bytes ([File](../structs/File.md)) | | V |
| 0x00586DA0 | `file_open(path, mode, ?)` | `CreateFileA(basedir + path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, NORMAL)`; error text `Error opening file %s.`; shared helper (see resdb.md) | | V |

## sound3d.cpp — OpenAL wrapper (`Sound3D`)

All AL calls live in tiny methods; `this` of the manager is in ECX (Ghidra shows `local_8`). See [Sound3D](../structs/Sound3D.md).

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x005A0330 | `sound3d_init` | allocate 10-byte sentinel (+0x12: non-null = "initialised"); `alcOpenDevice("DirectSound3D")` + `alcCreateContext(dev,NULL)` + `alcMakeContextCurrent` (FUN_007A9420); `alGetError`; on error show message (0x5A8BE0) and abort via CRT exit(-3) (FUN_007B47FC(0xFFFFFFFD)) after freeing; alloc listener object (0x35 B, +0x28) and init; set listener gain 1.0 / default pose; `set_distance_model(DAT_00a57ffb != 0 ? 2 : 0)`; `set_doppler_factor(0)`; `set_doppler_velocity(343.0f)` | strings, `alGetError`, constants 0x43AB8000 | V |
| 0x005A04E0 | `sound3d_shutdown` | if initialised: free listener, delete all sources (0x5A0CF0, last to first) and all buffers (0x490BC0), `alcMakeContextCurrent(NULL)`/`alcDestroyContext`/`alcCloseDevice` of the current context (FUN_007A9457), free sentinel | | V |
| 0x007A9420 | `al_open_device` | `alcOpenDevice("DirectSound3D")`; `alcCreateContext(dev,0)`; `alcMakeContextCurrent` | import calls, string `s_DirectSound3D_00a308dc` | V |
| 0x007A9457 | `al_close_device` | inverse of the above | | V |
| 0x0048D120 | `sound3d_create_buffer` | `alGenBuffers(1)`; `alIsBuffer`; grows pointer array (+0x18) by 32 entries when full; allocates a 0x11A-byte wrapper (`Sound3DBuffer`, zeroed by 0x59ED50, `+0 = ALuint`), inserts it into the array **sorted by ALuint** (binary search 0x5911A0 / insert 0x5910F0); count `+0x1C`++; returns wrapper or NULL | | V |
| 0x004907E0 | `sound3d_create_source` | same for `alGenSources`, 0x53-byte `Sound3DSource` (zeroed by 0x59F2E0, defaults applied by 0x59F340), array at +0x20/+0x24/+0x26 | | V |
| 0x00490BC0 | `sound3d_destroy_buffer` | removes wrapper from sorted array (0x591070), frees it, shrinks array by 32 when slack >= 32 (free array at 0), `alDeleteBuffers(1)` | | V |
| 0x005A0CF0 | `sound3d_destroy_source` | `source_stop_and_unqueue` (0x59F400) then removes/frees wrapper, shrinks array, `alDeleteSources(1)` | | V |
| 0x0059F400 | `source_stop_and_unqueue` | `alSourceStop`; if state read back (`AL_SOURCE_STATE`) == INITIAL, `alSourcePlay` then `alSourceStop` again; then unqueue every queued buffer (0x59F4B0 with `AL_BUFFERS_QUEUED`) | 0x59F680/0x59F620/0x59FC90/0x59FD60 | V |
| 0x0059F4B0 | `source_unqueue_buffers(n)` | `alSourceUnqueueBuffers(src,n,ids)`; for each id look the wrapper up (0x5A0870 binary search); `refcount (+4)--`; if it reaches 0 and `auto_delete (+0x119)` is set -> `sound3d_destroy_buffer` | | V |
| 0x005A0870 | `sound3d_find_buffer(ALuint)` | lookup in sorted array (0x5911A0), returns wrapper* or 0 | | V |
| 0x005911A0 | `sorted_array_search(key, arr, n, stride, cmp, out_pos)` | generic binary search + insertion point (1-based; 0 = not found) | | V |
| 0x005910F0 / 0x00591070 | `sorted_array_insert` / `sorted_array_remove` | memmove-based insert/remove (1-based pos) | | V |
| 0x00454000 | `sound3d_set_distance_model(m)` | stores `m` in byte `this+0`; 0 -> `alDistanceModel(AL_NONE 0)`, 1 -> `AL_INVERSE_DISTANCE 0xD001`, 2 -> `AL_INVERSE_DISTANCE_CLAMPED 0xD002` | | V |
| 0x005A06F0 | `sound3d_set_doppler_factor` | store (+0xA) + `alDopplerFactor` | | V |
| 0x005A0790 | `sound3d_set_doppler_velocity` | store (+0xE) + `alDopplerVelocity` | | V |
| 0x005A1110 | `sound3d_is_initialised` | `this->sentinel(+0x12) != 0`; every public method early-outs on this | | V |
| 0x0059F340 | `source_set_defaults` | pitch 1, gain 1, min_gain 0, max_gain 1, cone_outer_gain 1, cone angles 2*pi (full sphere), max_distance 1e7, reference_distance 1000, rolloff 1 | setter call args | V |
| 0x0059F6B0 | `source_set_pitch` | field +4, `alSourcef(AL_PITCH 0x1003)` | | V |
| 0x0059F700 | `source_set_gain` | field +8, `AL_GAIN 0x100A`; **0 while `muted (+0x2C)`** | | V |
| 0x0059F770 / 0x0059F7C0 | `source_set_min_gain` / `_max_gain` | `AL_MIN_GAIN 0x100D` / `AL_MAX_GAIN 0x100E` | | V |
| 0x0059F810 | `source_set_cone_outer_gain` | `AL_CONE_OUTER_GAIN 0x1022` | | V |
| 0x0059F860 / 0x0059F8C0 | `source_set_cone_inner_angle` / `_outer_angle` | stored in radians, sent to AL in degrees (`rad / DAT_009CF884`, constant = pi/180 inferred from default 2pi -> 360) `AL_CONE_INNER_ANGLE 0x1001` / `OUTER 0x1002` | | R |
| 0x0059F920 | `source_set_max_distance` | `AL_MAX_DISTANCE 0x1023` | | V |
| 0x0059F970 | `source_set_reference_distance` | `AL_REFERENCE_DISTANCE 0x1020` | | V |
| 0x0059F9C0 | `source_set_rolloff` | `AL_ROLLOFF_FACTOR 0x1021` | | V |
| 0x0059FC90 | `source_get_state` | `alGetSourcei(AL_SOURCE_STATE 0x1010)` mapped to 0 other, 1 INITIAL, 2 PLAYING, 3 PAUSED, 4 STOPPED | | V |
| 0x0059FD60 | `source_get_queued` | `AL_BUFFERS_QUEUED 0x1015` | | V |
| 0x0059F620 / 0x0059F680 | `source_play` / `source_stop` | `alSourcePlay` / `alSourceStop` | | V |
| 0x0059FE40 | `listener_init` | gain 1.0; init pose struct (0x616D50: zero 0x28 B, +0x1C = 100.0, +0x20 = +0x24 = 1.0); compute and apply pose (0x5A0110) | | R |
| 0x0059FEB0 | `listener_set_gain` | field +0, `alListenerf(AL_GAIN 0x100A)`; 0 while `muted (+4)` | | V |
| 0x005A0110 | `listener_apply_pose(pose*)` | orbit-style listener: yaw `pose+0x10`, pitch `pose+0x14`; forward = (cos p cos y, cos p sin y, -sin p), up = (sin p cos y, sin p sin y, cos p); position = `pose+4` + `pose+0x1C` * forward (distance 100 default); hands the result to 0x59FF70 (`alListener3f/alListenerfv`, not exported) | | R |
| 0x0059FF70, 0x005A0050 | `listener_set_position`, `listener_set_orientation` | not exported | TODO(re) | - |
| 0x0058E8D0 | `tracked_alloc_zero_linked` | tracked, zero-filled alloc with leak-list link (alloc `n+0x11`, 16-byte header); used for the sentinel | | R |

## se.cpp — sound-effect bank index (`SeTable`)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x00490D30 | `se_table_build` (fastcall, this=SeTable) | `n_wave = *(this+0x189)`, `n_extra = *(*(this+0x19D)+0x121)` (the ResLX count of the second bank), `total = n_wave+n_extra`; allocates `total*0x28` bytes of entries (`+4`) and `total` bytes of flags (`+8`); entry i < n_wave: `ptr=ResLX::get_payload(i+1)` (0x59BF70, 1-based), `id=i+1`; the `n_extra` entries get `ptr=NULL`, `id=1..n_extra`; all flag bytes 0 | se.cpp | V |
| 0x0059E0C0 | `se_table_free` | if `initialised (+0x50)`: clear; if `owns (+0x54)` free entries (+4) and flags (+8) | se.cpp | V |
| 0x0059BF70 | `reslx_get_payload(idx)` | see [res.md](../../formats/res.md); in streaming mode (`load_all==0`, `scratch==0`) caches ONE payload in a growable buffer keyed by index (`+0x125`), in scratch mode reads into the global buffer `DAT_00a500a4`, in load-all mode returns `base + (offset - first_offset)` | reslx.cpp | V |

The function that maps game events to wave names and plays them (priority, random variation, voice limit, categories) is **not in the export**.

## musicres.cpp — `MUSIC` dBase table

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x004B3360 | `musicres_load` (fastcall, this=MusicRes) | opens the `MUSIC` table (name lookup in the gameplay ResX bank via 0x58B180 -> 0x59C950 -> `Db::open_memory` 0x580AE0; error `Error Loading %s` unless name is `RELEASE`), `count = Db::record_count`, allocates `count*0x68` zeroed bytes (+4), then for each record fills a [MusicEntry](../structs/MusicEntry.md) (code, file name, two floats, two shorts, two enums) | musicres.cpp, 0x580C70, 0x415280 | V |
| 0x004B32F0 | `musicres_free` | if `loaded (+8)`: free entries, clear flag | musicres.cpp | V |
| 0x0058B180 | `db_open_by_name(name)` | returns `this+0x13E` (embedded [Db](../structs/Db.md)) after opening table `name` from the ResX | | R |
| 0x00580C70 | `db_get_record(idx)` | 1-based record accessor (0 or out of range -> NULL; idx<1 uses `cur_record`) | Db.md | V |
| 0x00590210 | `copy_trimmed(dst, src, n)` | copy `strnlen(src, n)` bytes (0x58F390) and NUL-terminate | | V |
| 0x00590170 | `field_copy_rtrim(dst, src, n)` | copy `min(n,0x78)` bytes, strip trailing spaces, NUL; used for numeric dBase fields before `atoi` (FUN_007B15CB) | | V |

## speech_s.cpp — speech script text

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x004D1770 | `speech_open(FileTxt*)` (thiscall, this=SpeechScript) | stores the text-file object (+0); reads the first line (<= 255 chars, separators " ,:\t", terminated by CR or 0x1A; FileTxt reader 0x589F10), splits it on `','` (strtok-like 0x4D14F0, static cursor `DAT_00a3a290`) into a table of **0x50-byte strings** (+0x24, grown 5 entries at a time via realloc); remembers (+0x28) the index of the entry that equals the global string `DAT_009D5B64` (default `-1`); allocates a 400-byte text accumulator (+8, len +0xC, cap +0x10); sets `+0x130 = 1` (active) and `+0x134 = 1.0f` | speech_s.cpp | R |
| 0x004D1980 | `speech_close` | delete the FileTxt (via dtor 0x419280->0x589890), free text buffer (+8) and table (+0x24), reset all counters (`+0x20 = -1`, `+0x28 = -1`, `+0x2C = 0`, `+0x130 = 0`) | speech_s.cpp | V |
| 0x004D27E0 | `speech_append(const char*)` | appends a string to the accumulator, inserting one `' '` (0x20) separator when non-empty; grows buffer by 400 bytes when `len + 2 + strlen > cap` (zero-filling the new chunk) | speech_s.cpp | V |
| 0x00589F10 | `filetxt_read_line_token(dst, max)` | skip leading `' ' ',' ':' '\t'`; copy until CR or 0x1A or `max` bytes; NUL; advance past line ending (0x5899B0) | | V |

Playback of the speech WAVs (`S?L1_tactic.RES` / `S?L1_finish.RES`, 4-digit IDs) and the queue/interrupt logic are not in the export.

## Shared helpers seen in this export (documented elsewhere)

0x58E7B0 tracked malloc, 0x58ED70 tracked free, 0x58EB30 tracked realloc, 0x58E720/0x58E650/0x58E6A0 allocator bookkeeping (leak list),
0x586AF0 allocation-failure report, 0x586CB0 fatal error box (`MessageBoxA`, title "Wars & Warriors: Joan of Arc"), 0x5A8CF0/0x5A8BE0
message-box wrappers, 0x5873A0 file read (`Error reading file %s, Retry ?`), 0x587EA0 file seek, 0x587EF0 file tell, 0x587260 file close,
0x59B920 ResLX::open, 0x59BB40 ResLX::close, 0x59C440 ResLX::build_sorted_index, 0x59CA60 ResX::find_index, 0x59C950 ResX::find_by_name,
0x58B180 / 0x580AE0 Db open.

## Round 3 (export `decomp/engine/audio2/`): SE player, listener, music/ambience tick, speech = lip-sync script

**Verified: there is no EAX anywhere in the binary (string search) — plain OpenAL 1.x.** Streaming music/ambience/voice use the wmvcore `VideoPlayer`
([structs/VideoPlayer.md](../structs/VideoPlayer.md), engine/video.md 1 and 4), which decodes WMA with `IWMSyncReader` and pushes PCM into AL buffers.

### Sound-effect bank / player

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x0045A350 | `sepl_init` (fastcall, this=SoundEffectPlayer) | `se_table_free`; `enabled (+0x54) = *(byte*)(*(this+0x64)+2)` (settings byte at +2 of an options struct); if 0 just mark ready (`+0x50 = 1`); else open `RESOURCE\A_WAVE1.RES` (11 waves) as a ResLX **load-all** bank (`open(path, load_all=1, 0)`), build the `SeTable` (0x490D30) and clear 0x28-byte entry state for all `total` entries (0x59E220/0x59DED0: entry `+6 = 0`) | strings `RESOURCE\`, `A_WAVE1.RES` | V |
| 0x004D6FE0 | `game_audio_init` (part of the app init, 1769 B) | init order: `audio_init` 0x57A440, `sound3d_init` 0x5A0330, music player init 0x59AD30, ambience player init 0x585FD0, SE player init 0x45A350; then CODEPAGE.RES/fonts | calls in order | V |
| 0x004CF0F0 | `sepl_play(id)` (thiscall, this=player; `*this` = volume int) | returns voice slot 1..12 or 0. Refuses when game state `DAT_00a500c2 == 12`, when listener `DAT_00a57ef8 == NULL`, or `id == 0`. Looks up the sound definition: `id <= 0x4000` -> table A record `base + (id-1)*0x8F`; `id > 0x4000` -> table B record `base + (id-0x4001)*0x93` (0x4D0820/0x4D0860); lazily loads its buffer (`def+0x8B`, virtual call through the table object when null, 0x4D09C0); `find_free_voice` (0x45F450: first of **12** slots whose source exists and has no queued buffer); then: reset slot (0x4CF690 = `slot = base-4 + n*0x18`), queue the buffer on the slot's source (0x59F460, refcount++), **set source position = listener position** (`DAT_00a57ef8 + 5`), `AL_LOOPING = def.loop(+0x85)&3 != 0`, `gain = *this(volume) * def.volume(+0x69) * K` (K = `_DAT_009D5344`), `max_distance = def+0x79`, `reference_distance = def+0x75`, `AL_SOURCE_RELATIVE = false`, `rolloff = def+0x7D`, slot.id (+4) = `def.id (+4 short)`, slot.flag (+8) = 0, `alSourcePlay` | 0x4CF0F0 | V |
| 0x004CEEF0 | `sepl_play_def(def)` | same as above but takes a definition pointer (0x4D0D80 resolves) and uses the global SFX volume byte `DAT_00a57FF9` as the volume factor | 0x4CEEF0 | V |
| 0x0045F450 | `sepl_find_free_voice` | scan slots 1..12; free = slot has a source and `AL_BUFFERS_QUEUED < 1`; returns index or 0 (**no voice stealing, no priority**: when all 12 are busy the new effect is dropped) | 0x45F450 | V |
| 0x004CEA40 | `sepl_stop_slot` | stop source, unqueue buffers (0x59F4B0); `slot.flag = 0; slot.id = -1` | | V |
| 0x004CEB90 | `sepl_stop_all(keep_ids)` | stop four special sources (+4, +8, +0x0C, +0x10) and all 12 slots; unless `keep_ids` also resets slot ids | | R |
| 0x0059F460 | `source_queue_buffer` | `alSourceQueueBuffers(src,1,&buf)`, `buf.refcount++` | | V |
| 0x0059F1A0 | `buffer_upload_pcm(data,size,channels,freq,bits)` | `alBufferData` with AL format 0x1100 mono8 / 0x1101 mono16 / 0x1102 stereo8 / 0x1103 stereo16 (stored at buffer +0x10C; size +0x110; freq +0x114); marks `loaded` | | V |
| 0x0059FA80 | `source_set_looping` | byte at source `+0x2D`, `AL_LOOPING (0x1007)` | | V |
| 0x0059FAE0 | `source_set_relative` | byte at source `+0x2E`, `AL_SOURCE_RELATIVE (0x202)` | | V |
| 0x0059FB40 | `source_set_position(vec3*)` | source `+0x2F/+0x33/+0x37`, `alSource3f(AL_POSITION 0x1004)` | | V |
| 0x0059FD20 | `source_get_processed` | `AL_BUFFERS_PROCESSED (0x1016)` | | V |
| 0x0059F2B0 | `buffer_set_auto_delete` | byte at buffer `+0x119` | | V |
| 0x0059FF70 | `listener_set_position(vec3*)` | listener `+5/+9/+0x0D`, `alListener3f(AL_POSITION)` | | V |
| 0x005A0050 | `listener_set_orientation(fwd*, up*)` | listener `+0x1D/+0x21/+0x25` (forward) and `+0x29/+0x2D/+0x31` (up); `alListenerfv(AL_ORIENTATION 0x100F)` | | V |
| 0x005A0110 | `listener_apply_pose(pose*)` | orbit pose -> forward/up (formulas in round 1), position = `pose.target + pose.dist * forward`; calls the two above | | V |

### Music and ambience tick (per frame, `DAT_00a500b2` game ms, `DAT_00a500b6` frame ms)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x0059B1C0 | `music_tick` (fastcall, this=MusicPlayer) | if enabled: mode 1 (playing) -> if the stream state from `video_pump(now - start_ms)` (0x685A80) is 3 or 4 (**end of stream**) then rewind to 0 (0x686250: `IWMSyncReader::SetRange(pos*10000 hns)`) and restart `start_ms = now` = **looping**. Mode 2 (fade-out) -> `gain = volume * K * fade_remaining / FADE_TOTAL` (K=`_DAT_009D5344`, FADE_TOTAL=`_DAT_009CF7A4`) if audio output enabled `DAT_00a5cf9c`; fade_remaining -= frame ms (clamped at 0, once per new `now`). `last_tick = now` | 0x59B1C0 | V |
| 0x0059B070 | `music_is_active(id)` | true if enabled, a track and a player exist and (`id == 0` or `id == current`) and not (ended/finished or fade finished with remaining 0) | | V |
| 0x005866D0 | `ambience_tick` (this=AmbiencePlayer) | the same loop for **3 channels** (mode array `+0x10`, start `+0x28`, fade `+0x34`, volume `+0x1C`); per-channel enable flags `DAT_00a5cfa0/a4/a8`; fade scale `_DAT_009CFC34`; mode 0 channels are only polled | 0x5866D0 | V |
| 0x005864F0 | `ambience_is_active(id, ch)` | per-channel version of 0x59B070 | | V |
| 0x0059AD30 / 0x00585FD0 | `music_player_init` / `ambience_player_init` | stop everything, `volume = settings byte` (`DAT_00a57FF2` music, `DAT_00a57FF4` ambience), `enabled = 1` | | V |
| 0x00685A80 | `video_pump(elapsed_ms)` | the stream service shared with video (see video.md): reads WMA/WMV samples until about **200 ms (300 on the first fill of a video)** of audio is buffered ahead of `elapsed_ms`, uploads PCM to AL buffers through 0x685DD0, starts the source (`alSourcePlay`) once data is queued; returns 1 not started, 2 playing, 3 finished, 4 finished-with-data | 0x685A80, 0x685DD0 | V |
| 0x00685DD0 | `video_read_sample` | `IWMSyncReader::GetNextSample` (vtbl+0x1C): stream number == audio stream -> create AL buffer (0x48D120, auto-delete) and `alBufferData` + queue; == video stream -> frame copy | | V |
| 0x00686160 | `video_close_audio` | stop+unqueue source, release reader | | V |
| 0x004B32B0 | `musicres_reload` | `musicres_free`, `musicres_load`, `loaded = 1` | | V |

### Speech = lip-sync / subtitle script (corrects round 1)

`SpeechScript` (0x138 bytes) is a **time-keyed facial-animation + subtitle script**, advanced by wall time (`DAT_00a500BA` ms clock) times a speed factor.

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x004D1620 | `speech_load(dir, name)` | builds a path, checks existence (0x590B80), allocates a 0x19A-byte FileTxt object (0x589550) and calls `speech_open`; error text `Error loading Speech Script : %1s` | | V |
| 0x004D1BE0 | `speech_update` (fastcall, this=SpeechScript) | state `+0x130` 2 = playing, 3 = seek-to-first-key; each tick `time_f += (now - last) * speed(+0x134)`, `time(+0x14) = (int)time_f`; when `time >= next_key(+0x20)` (or no key yet): the *current* row is applied and the next row read: a row is `time_ms, col1, col2, ...` comma separated (cols = `n_names (+0x2C)`; column `text_col (+0x28)` is the subtitle text, `<comma>` -> ','); other columns are strings like `A[50]|B[100]` (tokens split on `|`; the subtitle column header is `text`): phoneme/morph names (names table +0x24) with optional weight in `[...]` divided by `_DAT_009CF9C4` (percent, presumably 100.0); weights are summed and the first element gets `_DAT_009CF784` (1.0) minus the sum added (weights normalised to total 1); the blend vector (one float per morph target of the target model, count from record `+0x34`) is applied immediately for the previous row (0x5C9120) or interpolated towards the next row over `next_key - cur` ms (0x5C9350); EOF -> state 4 | 0x4D1BE0 | V |
| 0x004D27E0 | `speech_append_text` | subtitle string accumulation | | V |
| 0x0063B5E0 | `talker_update` | per-NPC/actor: only while global fade/state `DAT_00a5d38b == 255.0f && DAT_00a5d38f == 2`: plays its sound id (`+0x340`) once (`+0x342` flag) via `sepl_play`; when `+0x336 == 1` lazily creates the script (FileTxt over an embedded buffer `+0x322` size `+0x326`, SpeechScript `+0x347`, actor `+0x343`) and calls `speech_update` | | R |
| 0x0063C1C0 | `talkers_debug_poll` | iterates all talker objects (debug toggle "Debug Mode On/Off") | | R |

No speech *queue* exists in this code: each talker owns one script and one SE voice; there is no cross-actor ordering or interrupt logic (an effect is simply dropped when 12 voices are busy).

## Round 9 (export `decomp/engine/audio3/`): positional SE, music director, SOUND tables A/B

### SOUND definition tables

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x004D0DE0 | `soundtab_load` (fastcall, this=SoundTable) | opens dBase section `SOUND` of the gameset bank (0x58B180; gameset.md **schema O**, 505 records, record_len 156), allocates `n*0x8F+4` bytes (count stored before the array, per-element ctor/dtor 0x4D08F0/0x4D0940), fills each 0x8F-byte [SoundDef](../structs/SePlayer.md) from the record (field offsets in the struct page), **qsorts by name** (stride 0x8F, comparator 0x408710) and then assigns `def.id (+4) = 1-based sorted index`. So table A id = rank of the `SOUND_CODE` in sorted order | `"SOUND"`, 0x4D0820 index math | V |
| 0x00639560 | `soundtab_load_voice_bank(name)` | **table B**: opens `RESOURCE\<name><ext>` (the `S?L1_tactic/finish.RES` speech banks, ResLX via 0x47CDF0) and appends one 0x93-byte def per payload: `name = 4-digit string id`, `wave = bank name`, `+0x8F int = atoi(name)`, defaults gain 1.0 (+0x69), +0x6D 1.0, +0x71 2.0, `min_dist` 10000.0 (0x461C4000), `max_dist` 15000.0 (0x466A6000), rolloff 1.0, no loop, then re-sorts (cmp 0x40431D) and renumbers `id = 0x4001 + rank`. Reallocates the table each time a bank is added | 0x639560 | V |
| 0x006391C0 / 0x00639030 | `voicetab_find_by_string_id` | search table B by the integer id `+0x8F` (binary search, `strcmp`-style compare); returns the 0x4001-based id | | R |
| 0x004D0820 / 0x004D0860 | `soundtab_a_at(id)` / `soundtab_b_at(id)` | `base + (id-1)*0x8F` / `base + (id-0x4001)*0x93` | V |
| 0x004D0D80 / 0x004D1220 | `soundtab_find(name)` | name -> def by binary search on the sorted names (`strcmp`), returns def* or 0 | | V |
| 0x004D0A60 | `sounddef_load_buffer` (virtual slot 0 of the def's owner) | creates the AL buffer (0x48D120) and loads the wave: picks the wave name through virtual +8 (`WAVE_FILE` plus random variant index 1..`WF_RANDMAX`, name builder 0x4D0B10/0x5A31E0), reads it from `RESOURCE\WAVE.RES` (ResLX, 503 waves) via 0x59EF00 into the buffer; stored at `def+0x8B`. The variant is therefore chosen **once per def per run** (the buffer is cached) | `RESOURCE\\WAVE.RES` | R |
| 0x004D0C20 / 0x004D0BC0 / 0x004D0BF0 | `soundtab_free` / `soundtab_free_hook` / `soundtab_reload` | destroys all def buffers (0x4D0A00 -> 0x490BC0) and the arrays | | R |

### Positional SE emitters (world positions)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x004C0680 | `sepl_play_emitter(emitter_idx)` | per-frame start/maintain for **placed sound emitters** (record from 0x485F20 -> object table 0x41B3F0: `+8` def id, `+0x13` vec3 position, `+0x1F` max audible radius, `+0x2A` flag). Skips if a voice already plays that emitter's position (0x4D00E0), if the listener is farther than the radius (radius `== _DAT_009CF5C4` means unlimited), else takes a free voice (0x45F450) or **steals the voice farthest from the emitter** (0x493340: stop it, reuse the slot). Sets min_gain 0, max_gain 1, gain = `volume*def.gain*0.01`, max/ref distance/rolloff from def, `SOURCE_RELATIVE` off, **AL_POSITION = emitter position (world units)**, slot `+4`=def id, `+8`=emitter idx, slot position (0x4D0770), play | 0x4C0680 | V |
| 0x00494810 | `sepl_play_name_at(name, pos)` (this=SePlayer) | find def by name (0x4D0D80), position `pos` (vec3 on stack), loop bit, `gain = volume*def.gain*0.01`, max/ref/rolloff from def, slot flag 0; refuses in state `DAT_00A500C2 == 12`; no stealing. Used by projectile code (`"ARROW"`, `"CANNONHL"`, `"EXPLOSIVEHIT1..3"` in 0x4FCC00 at the hit/impact position) | 0x494810, 0x4FCC00 | V |
| 0x004950A0 | `sepl_play_id_at(id, pos)` | same by numeric id (table A/B) | | V |
| 0x00492C40 | `sepl_play_variant_at` | third variant (same sequence; name resolved) | | R |
| 0x004FCC00 | `projectile_sound_tick` | for arrows (mode 1/2) creates a looped `ARROW` voice if option `DAT_00A57FFB` (3D audio) is on and then **moves the source (0x59FB40) to the arrow position every frame**; cannon/explosion one-shots at impact | 0x4FCC00 | R |
| 0x004CF0F0 / 0x004CEEF0 | UI/2D variants | as in round 3; `sepl_play_id` callers are talkers (0x63B5E0), `sepl_play_def` callers are all UI (`BUTTON_SLIDE_MANY`, `MENU_SLIDE_IN/OUT`, `IF_BUTTON_MOUSE_DOWN`, `OBJECTIVE_ADD/UPDATE/FINISH`, `MISSION_BRIEF_n`, `ATTENTION_CALL`, `IE_ARROW/BOW/SWORD`) — name-keyed effects fired by the 2D interface | callers | V |

World units: listener and emitters share world coordinates; defaults `MIN_DIST/MAX_DIST` of the SOUND table (N8.2) and speech 10000/15000 show a centimetre-like scale. The attenuation is OpenAL inverse-distance-clamped with `ref = MIN_DIST`, `max = MAX_DIST`, `rolloff = ROLLOFF` (all per def). No doppler/velocity.

### Music director

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x0059AE60 | `music_play(track_id, mode)` | validates `1 <= id <= DAT_00A45D20` (MusicRes count) and `DAT_00A5CF9C` (music volume percent) non-zero; stops the old stream (0x59AE00 = 0x686160 close); creates a `VideoPlayer` (0x30 B, ctor 0x6845D0) if needed; opens `MUSIC\` + `MusicEntry.filename` (entry +0x13, via 0x4D8BE0) with `uti3d::video::fn_00684650(path, volume)`; sets the AL source **relative at DAT_00A69CD8 (origin)**; `track=id, mode=param, start_ms=now, fade_ms=8000`, applies volume (0x59B150) | 0x59AE60 | V |
| 0x00586210 | `ambience_play(track_id, channel, mode)` | same for 3 channels (`DAT_00A5CFA0/A4/A8` = per-channel volume percent; 0 disables a channel), fade 5000 | 0x586210 | V |
| 0x0059AE00 / 0x005861A0 | `music_stop` / `ambience_stop(ch)` | close stream | | V |
| 0x0059B330 | `music_fade_out(mode=2)` | begin fade-out (mode 2, `fade_ms = 8000`) | | R |
| 0x004B3940 | `musicres_pick(kind, etype, scenario)` | candidate set = entries with `kind == k` and (`scenario == 0` or `start_sce <= scenario <= end_sce`) and (`etype == 0` or entry.e_type == etype); picks **uniformly at random** (RNG 0x590540(n) in [0,n)). For `kind == 5` ('S' stingers) `etype` must be non-zero, the scenario range is ignored and the pick is random among that e_type; returns 1-based entry id or 0 | 0x4B3940 | V |
| 0x004B3C70 | `musicres_pick_by_time(kind, t)` | first entry of the kind whose `[start_min, end_min)` window contains the day-time float `t` (windows with `end <= start` wrap midnight) — so STARTMIN/ENDMIN are **time-of-day** bounds used for ambience (kind 4 'E') | 0x4B3C70 | V |
| 0x004E6FE0 | `music_director_tick` | per frame (`this+0x3A` = game mode byte): `DAT_00A5D2F0 != 0` -> stop. Modes 3/1/0/0xF (menus/map) -> kind **3** with scenario `DAT_00A58FA7`; mode 0xC -> stop; end-of-mission result (0x6D1EF0 and `DAT_00A57ECB` 0/1) -> kind 9/10; `DAT_00A59298` 2 -> kind 6, 1 -> kind 7 (game over / victory); in battle default kind **1**, kind **2** when the player's army is in combat (`0x4CD840()+0x1AE == 3`) or other fight conditions, kind 8 for special siege/castle units. A different wanted kind replaces the current track; leaving combat (2 -> 1) waits **8000 ms** (`DAT_00A5D0E4` timer) before fading; `DAT_00A5D0D8 == 1` loop-and-switch policy, `== 2` replay when idle. Ambience channels: ch0 kind 4 by time-of-day (0x4B3C70 with `DAT_00A47BB4`) or kind 4 (0x4B3940(4,4,0)) when 0x41BF10(1); ch1/ch2 kind 5 stingers (`e_type` 6..9 by situation) at random intervals `rand(30000)+30000` ms (`DAT_00A5CD90`) | 0x4E6FE0 | V |

## Open questions
- RESOLVED in rounds 3/9: items listed above; remaining open: exact meaning of `MusicEntry.kind` letters beyond usage (1 N normal, 2 B battle, 3 M menu, 4 E environment by time-of-day, 5 S stinger, 6 G game-over, 7 W win, 8 C special battle, 9/10 result tracks), who sets `DAT_00A5D0D8` policy, `WF_RANDMAX` suffix format (0x4D0B10), `MIN_PITCH/MAX_PITCH` (never applied in the exported play paths), `DELAY_BEG/END`, `SPEED_INDE`, `MAIN_CHAR` consumers.
- RESOLVED constants (.rdata float32): `_DAT_009D5344` K = **0.01** (volume bytes are percent 0..100; 0x9D5340 = 0.1), `_DAT_009CF7A4` music FADE_TOTAL = **8000.0 ms**, `_DAT_009CFC34` ambience FADE_TOTAL = **5000.0 ms** (0x9CFC38 = 80.0),
  `_DAT_009CF884` = 0.017453292 (pi/180, confirmed), `_DAT_009CF9C4` = 100.0, `_DAT_009CF784` = 1.0. Strings: `DAT_009D5B64` = `"text"` (name of the subtitle column), `DAT_009D5B84` = `"|"` (weight-token separator), `<comma>` escape, next string `super_cheat_mode`.
- TODO(re): who calls `music_play(track)` (sets mode, track id, creates the VideoPlayer from the `MUSIC` row `FILENAME`) and selects tracks by `TYPE`/`E_TYPE` and scenario range; not in the export (callers of 0x59B070 and writers of `MusicPlayer+4/+8`).
- TODO(re): the positional (3D) SE entry — every exported SE start puts the source at the listener position with `SOURCE_RELATIVE=false`; callers that pass world positions (probably through slot `+0` source `set_position`) are unexported. Units of positions/velocity: world units unscaled; listener position = orbit camera target + 100*forward.
- TODO(re): definition tables A/B (0x8F / 0x93-byte records) loader and field names; the settings struct holding `DAT_00A57FF2/F4/F9/FB` (music, ambience, SFX volume bytes, 3D flag): range/UI mapping.
- TODO(re): `DAT_00A500C2 == 12` meaning (game state that mutes SFX).
