# AudioDevice (ilib\audio.cpp, sizeof >= 0x372) — joan.exe

DirectSound + MCI CD-audio state object (a global; constructor/initialiser is 0x57A440). NOT the OpenAL manager (that is [Sound3D](Sound3D.md)).
Status: V verified (>= 2 functions), R read once, I inferred. Functions: [audio.md](../functions/audio.md).

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x000 | 1 | u8 | `ready` | `ds_ok \|\| cd_ok` after init | 0x57A440 | V |
| 0x001 | 1 | u8 | `music_open` | music (MCI) channel usable; checked by 0x57A9C0/0x57AA30/0x57AB20; set elsewhere (not in export) | 0x57A9C0, 0x57AA30 | R |
| 0x002 | 1 | u8 | `ds_ok` | DirectSoundCreate succeeded | 0x57A740, 0x57A920 | V |
| 0x003 | 1 | u8 | `cd_ok` | MCI CD-audio device opened | 0x57A810 | V |
| 0x004 | 1 | u8 | `music_enabled` | init 1 | 0x57A440, 0x57AA30 | R |
| 0x005 | 1 | u8 | `sfx_enabled` | init 1; gate for all DS voice functions | 0x57A440, 0x57BCA0, 0x57DD30 | V |
| 0x006 | 1 | u8 | `flag6` | init 1, no reader in export | 0x57A440 | I |
| 0x007 | 4 | u8* | `music_buf` | heap buffer for a music resource payload (realloc'd) | 0x57AA30, 0x57A9C0 | R |
| 0x00B | 4 | u8* | `scratch_buf` | 0x8000-byte scratch (min 0x4000 before 0x57BCA0) for wave header + `data` scan | 0x57A440, 0x57BCA0 | V |
| 0x00F | 4 | i32 | `music_cap` | capacity of `music_buf` | 0x57AA30 | R |
| 0x013 | 4 | i32 | `scratch_cap` | capacity of `scratch_buf` (0x8000, raised to 0x4000 if smaller) | 0x57A440, 0x57BCA0 | V |
| 0x281 | 4 | IDirectSound* | `ds` | | 0x57A740, 0x57A920 | V |
| 0x285 | 32 | IDirectSoundBuffer*[8] | `oneshot_buf` | one-shot voices; stopped+released in 0x57DD30 | 0x57DD30 | V |
| 0x2A5 | 32 | u32[8] | `oneshot_aux` | zeroed at init, purpose unknown | 0x57A440 | I |
| 0x309 | 16 | IDirectSoundBuffer*[4] | `stream_buf` | 4 streaming voices | 0x57BCA0, 0x57DE40 | V |
| 0x319 | 16 | i32[4] | `stream_id` | voice handle returned to the caller | 0x57BCA0 | V |
| 0x329 | 4 | i32 | `voice_id_counter` | 0x57EE10, wraps 0x7FFFFFFF -> 1 | 0x57EE10 | V |
| 0x32D | 16 | File*[4] | `stream_file` | open wave file object (0x10E B) per streaming voice | 0x57BCA0, 0x57DE40 | V |
| 0x33D | 8 | u16[4] | `stream_pos` | zeroed when a slot is (re)used | 0x57BCA0 | R |
| 0x345 | 8 | u16[4] | `stream_chunk` | chosen chunk size (quarter of DS buffer bytes) | 0x57BCA0 | R |
| 0x34D | 4 | i32 | `master_volume` | init 100 (percent assumed) | 0x57A440 | I |
| 0x351 | 1 | u8 | `has_played` | set after a successful voice start | 0x57A440, 0x57BCA0 | R |
| 0x352 | 16 | IDirectSoundBuffer*[4] | `channel_buf` | channel voices 1..4 | 0x57DF80 | V |
| 0x362 | 16 | File*[4] | `channel_file` | | 0x57DF80 | V |

Globals: `DAT_00a150e0` = CD volume read from the Windows mixer at init (0xFFFFFFFF if unavailable); `DAT_00a642e8` = `MCI_OPEN_PARMS`
(device id at `0xa642ec`); `DAT_00a50090` = main window HWND.
