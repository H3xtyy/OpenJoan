# VideoPlayer (`uti3d/video.cpp`) — WMV / WMA sync-reader player

Reference build joan.exe. Functions: [video.md](../functions/video.md); design notes: engine/video.md.
The same class plays cut-scene `.wmv` (video + audio) and `.wma` music (audio only). One global instance lives at
`0xA5D2F0` (the exported code accesses it through absolute addresses `DAT_00a5d2f0..`), and `this` is passed in ECX.
**sizeof = 0x30**: ctor 0x6845D0 = `memset(this,0,0x30); CoInitialize(NULL)` (verified; music players are `operator new(0x30)` + ctor in 0x59AE60 / 0x586210;
the movie instance is a static initialised by 0x567160). There are therefore **several instances**: the global *movie* player at 0xA5D2F0, one *music* player
(held in the music manager at mgr+0x20, OpenAL source `DAT_00a5cf9c`) and three *ambient* players (mgr+0x44.., sources `DAT_00a5cfa0/a4/a8`).

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | int32 | open | 1 = file opened and all streams configured; 0 = closed/failed. Every query helper starts with `if (!open) return 0` | verified: 0x684650, 0x686160, 0x685A80, 0x685DD0, 0x685940/90/9E0/A30 |
| 0x04 | 4 | Texture* | texture | engine texture object (0x5C bytes, see [Texture.md](Texture.md)); IDirect3DTexture8 is at texture+0x58. 0 for audio-only files | verified: 0x684650, 0x6859E0, 0x685DD0, 0x4D48F0 (`DAT_00a5d2f4`) |
| 0x08 | 4 | SoundSource* | sound | OpenAL source object (first dword = ALuint) supplied by the caller (2nd arg of the ctor); 0 = mute / not created. Looping is switched off on it at open | verified: 0x684650, 0x685A30, 0x686160 (`DAT_00a5d2f8`) |
| 0x0C | 2 | uint16 | video_stream | WM stream number of the video stream; 0 = none | verified: 0x684650, 0x685940, 0x685DD0 |
| 0x0E | 2 | uint16 | audio_stream | WM stream number of the audio stream; 0 = none | verified: 0x684650, 0x685990, 0x685DD0 |
| 0x10 | 4 | int32 | width | video width in pixels = `rcTarget.right-rcTarget.left` of the output VIDEOINFOHEADER | verified: 0x684650; used 0x685DD0, 0x4D48F0 (`DAT_00a5d300`) |
| 0x14 | 4 | int32 | height | video height = `rcTarget.bottom-rcTarget.top` | verified (`DAT_00a5d304`) |
| 0x18 | 2 | uint16 | channels | WAVEFORMATEX.nChannels of the PCM output | verified: 0x684650 -> 0x59F1A0 |
| 0x1A | 2 | uint16 | bits | WAVEFORMATEX.wBitsPerSample | verified |
| 0x1C | 4 | uint32 | sample_rate | WAVEFORMATEX.nSamplesPerSec | verified |
| 0x20 | 4 | uint32 | clock_origin | `timeGetTime()` value that corresponds to media time 0 (ms). 0 = unset. Cleared by close/seek | verified: 0x685A80, 0x686160, 0x686250 |
| 0x24 | 4 | IWMSyncReader* | reader | wmvcore sync reader (COM) | verified |
| 0x28 | 4 | uint32 | last_video_ms | timestamp (ms = 100 ns / 10000) of the newest decoded video frame; 0 = none yet | verified: 0x685A80 |
| 0x2C | 4 | uint32 | last_audio_ms | timestamp of the newest decoded/queued audio buffer; 0 = none yet | verified |

Helpers: 0x6859E0 `has_texture` (`open && texture`), 0x685940 `has_video_stream` (`open && video_stream`), 0x685990 `has_audio_stream`
(`open && audio_stream`), 0x685A30 `has_sound` (`open && sound`).

## Sound source object (OpenAL wrapper, `ilib/sound3d`) — fields used by the video code
| offset | size | name | meaning | evidence |
|---|---|---|---|---|
| 0x00 | 4 | al_source | ALuint | 0x59F620 `alSourcePlay`, 0x59F680 `alSourceStop`, 0x59FD20 `alGetSourcei(AL_BUFFERS_PROCESSED 0x1016)`, 0x59FC90 `AL_SOURCE_STATE 0x1010` -> 1 initial /2 playing /3 paused /4 stopped |
| 0x2D | 1 | looping | `AL_LOOPING` (0x1007) | 0x59FA80 |
| 0x2E | 1 | relative | `AL_SOURCE_RELATIVE` (0x202) | 0x59FAE0 |
| 0x2F..0x3A | 12 | position | 3 floats, `AL_POSITION` (0x1004) | 0x59FB40 |

## SoundBuffer object (0x11A bytes, allocated by 0x48D120)
| offset | size | name | meaning | evidence |
|---|---|---|---|---|
| 0x00 | 4 | al_buffer | ALuint from `alGenBuffers` | 0x48D120 |
| 0x04 | 2 | queue_refs | ++ when queued on a source (0x59F460), -- on unqueue (0x59F4B0) | verified |
| 0x06 / 0x07 | 1 / 1 | uploaded / flag | set 1 / 0 by `alBufferData` wrapper 0x59F1A0 | read |
| 0x10C | 4 | al_format | 0x1100 mono8, 0x1101 mono16, 0x1102 stereo8, 0x1103 stereo16 (picked from channels,bits) | 0x59F1A0 |
| 0x110 | 4 | byte_len | | 0x59F1A0 |
| 0x114 | 4 | frequency | | 0x59F1A0 |
| 0x119 | 1 | free_on_unqueue | when set the buffer is destroyed once its queue count hits 0 (0x490BC0). The pump sets it via 0x59F2B0(1) for streamed chunks | 0x59F4B0, 0x685DD0 |

(The `ilib/sound3d` objects belong to the audio family; the offsets above are only those the player touches.)
