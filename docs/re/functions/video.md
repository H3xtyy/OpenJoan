# Video playback, video capture, esstate2 — joan.exe (reference build)

Pseudocode: `$JOAN_WORK_DIR/decomp/engine/video/` (plus `engine/lights/` for 0x685A80/0x685DD0/0x6859E0 etc. and `engine/lights/004d48f0`, `006d5ae0`
for the draw sites). Structs: [VideoPlayer](../structs/VideoPlayer.md), [VideoCapture](../structs/VideoCapture.md),
[EsScriptSlot](../structs/EsScriptSlot.md) (esstate2 token/slot/parser), [Texture](../structs/Texture.md). Design notes:
docs/engine/video.md. Status: verified = >=2 consistent reads, read = one function, inferred = by call pattern.
Attribution: `d:\ww\uti3d\video.cpp`, `videocap.cpp`, `esstate2.cpp` assert/alloc sites (line numbers passed to the tracked allocator).

## 1. video.cpp — WMV/WMA sync-reader player
Platform use: **wmvcore.dll** loaded with `LoadLibraryA` and `WMCreateSyncReader` via `GetProcAddress` (COM vtable calls below, slot = offset/4);
**OpenAL** for audio output; **D3D8** `IDirect3DDevice8::CreateTexture` (via 0x7648BA) and `IDirect3DTexture8::LockRect/UnlockRect/PreLoad`.

| address | proposed name | one-line purpose | evidence | status |
|---|---|---|---|---|
| 0x684650 | `video_open(this, const char* path, SoundSource* snd)` | ctor/open: closes any previous file, creates `IWMSyncReader`, opens `path`, forces RGB565 video + PCM audio output, creates the texture, primes OpenAL | alloc sites `video.cpp`; strings `WMCreateSyncReader`, wmvcore error texts | verified |
| 0x686160 | `video_close` | stops/unqueues sound, releases texture (0x60A550), releases the reader, `open=0` | called from 0x4D48F0, 0x6D5AE0, 0x6D6230, 0x470630, 0x52F930 | verified |
| 0x686250 | `video_seek_ms(this, ms)` | `IWMSyncReader::SetRange(ms*10000, 0)` (slot 5), clears `clock_origin`, `last_video`, `last_audio`; used to loop `.wma` music (`seek(0)`) | caller 0x59B1C0 | verified |
| 0x685A80 | `video_pump(this, now_ms)` -> state 0..4 | decodes samples until the video frame / audio queue catch up with the clock; returns the 1..4 state the draw routines test | callers 0x4D48F0, 0x6D5AE0, 0x59B1C0 | verified |
| 0x685DD0 | `video_read_sample(this, now_ms, out_ts_ms*, upload_flag)` -> 0 / 1 / 2 | one `GetNextSample` (slot 7): returns 0 = end/none, 1 = video frame (and uploads it), 2 = audio buffer (and queues it to OpenAL) | slot 0x1C; `aulldiv(ts,10000)`; LockRect flags 0x800 | verified |
| 0x685940 / 0x685990 / 0x6859E0 / 0x685A30 | `video_has_video_stream` / `has_audio_stream` / `has_texture` / `has_sound` | trivial predicates on +0x0C / +0x0E / +0x04 / +0x08 (all false when `open==0`) | | verified |
| 0x685910 / 0x598700 | `guid_not_equal` / `guid_equal` | `memcmp(a,b,16)`; GUID tests on WM media types | | verified |
| 0x5FFE60 | `d3dfmt_bytes_per_pixel(d3dfmt)` | 0x14 R8G8B8 ->3; 0x15/0x16 (A8R8G8B8/X8R8G8B8) ->4; 0x17/0x18/0x19/0x1A (16-bit) ->2; 0x1B/0x1C/0x29 ->1; 0x28 ->2; 0x32,0x34 ->1; 0x33 ->2; 0x3C,0x3D ->2; 0x3E..0x41 ->4; 0x46,0x49,0x50 ->2; 0x47,0x4B,0x4D,0x4F ->4; else 0 | | verified |
| 0x600080 | `texfmt_to_d3dfmt(engine_fmt)` | engine pixel-format enum -> D3DFORMAT: 2->0x18 (X1R5G5B5), 3->0x17 (R5G6B5), 5->0x14 (R8G8B8), 7->0x16 (X8R8G8B8), 0xB->0x15 (A8R8G8B8), 8->0x29, 0x1F->0x3C; {0,1,4,6,10} and unknown ->0 (UNKNOWN) | | verified |
| 0x6094C0 | `texture_new_for_video()` | `operator new(0x5C)`, runs ctor 0x605CE0, stores owner at +0x14 then runs 0x60AD40 (register in texture manager) | | read |
| 0x605E20 | `texture_create(w, h, levels, fmt)` | `CreateTexture(w,h,levels,usage 0, D3DFORMAT 0x600080(fmt), D3DPOOL_MANAGED=1)`; stores IDirect3DTexture8* at +0x58 and w/h/levels via 0x605A40; on failure logs "Cannot create texture" | string | verified |
| 0x605A40 | `texture_attach(d3dtex, w, h, levels, fmt)` | stores D3D texture at +0x04 (AddRef), +0x3C width, +0x40 height, +0x44 levels, +0x08 fmt, +0x48/+0x4C = -1 | | read |
| 0x60A550 | `texture_release(tex)` | refcount-- (+0x0C), parks last ref in the cache when manager+0x34 set, else destroys | see Texture.md | verified |
| 0x59F1A0 | `soundbuffer_set_data(buf, data, len, channels, rate, bits)` | maps (channels,bits) -> `AL_FORMAT_*` 0x1100..0x1103 and calls `alBufferData` | | verified |
| 0x59F460 | `soundsource_queue(src, buf)` | `alSourceQueueBuffers(src,1,buf)`; buf.queue_refs++ | | verified |
| 0x59F4B0 | `soundsource_unqueue(src, n)` | `alSourceUnqueueBuffers`; for each buffer: queue_refs--; if 0 and flag at +0x119 destroy it (0x490BC0) | `d:\ww\ilib\sound3d.cpp` | verified |
| 0x48D120 | `sound_buffer_new()` | `alGenBuffers(1)`; allocates a 0x11A-byte SoundBuffer, appends it to the sound manager's buffer array (grows by 0x20 entries) | `ilib\sound3d.cpp` | read |
| 0x59F620 / 0x59F680 / 0x59FA80 / 0x59FAE0 / 0x59FB40 / 0x59FC90 / 0x59FD20 | `source_play` / `stop` / `set_looping` / `set_relative` / `set_position` / `get_state` / `buffers_processed` | thin OpenAL wrappers (`alSourcePlay`, `alSourceStop`, AL_LOOPING 0x1007, AL_SOURCE_RELATIVE 0x202, AL_POSITION 0x1004, AL_SOURCE_STATE 0x1010, AL_BUFFERS_PROCESSED 0x1016) | | verified |
| 0x4D48F0 (game-state 2/9 branch) / 0x6D5AE0 (movie screen) | draw sites (functions owned by other families) | pump + letterboxed blit; see engine/video.md "Drawing" | | verified |
| 0x59B1C0 | music tick (other family) | drives a second use of the player for `.wma` music loops: pump, and on 3/4 `seek(0)` | | read |

### Open sequence of 0x684650 (details)
1. `video_close(this)`; `last_video = last_audio = 0`; `MultiByteToWideChar(CP_ACP, path -> WCHAR[260])`.
2. wmvcore: first call `LoadLibraryA("wmvcore.dll")` (cached in `DAT_00a6a3c4`), `GetProcAddress("WMCreateSyncReader")`; if missing, logs once
   ("Windows Media Format 9 Series Runtime is not installed on your machine - you will not be able to hear background and environmental music in the game.", flag `DAT_00a6a3c8`) and fails;
   `WMCreateSyncReader(NULL, 0, &reader)` failure logs "Error in Windows Media Format 9 Series Runtime...".
3. `reader->Open(wpath)` (slot 3); failure -> fail silently. `QueryInterface(IID_IWMProfile @0x9D1240)`; `GetStreamCount` (profile slot 8), `GetStream(i)` (slot 9),
   per stream `GetStreamNumber` (cfg slot 4) and `GetStreamType` (cfg slot 3, GUID).
4. **Video stream** (GUID @0x9D1918 = WMMEDIATYPE_Video; first one only): `GetOutputNumberForStream` (reader slot 19), `GetOutputProps` (slot 15),
   `GetMediaType` (props slot 4, size query then fill — buffers from tracked malloc `video.cpp`). If the subtype GUID @+0x10 of the media type is not
   `0x9D18B8` (WMMEDIASUBTYPE_RGB565): enumerate `GetOutputFormatCount`/`GetOutputFormat` (slots 17/18) and for **every** output format whose subtype is RGB565 call
   `props->SetMediaType` + `reader->SetOutputProps` (slot 16); if none exists the open fails. Then the format-type GUID at +0x2C must be `0x9D1540` (VideoInfo) or `0x9D1528`
   (probably MPEG2Video); width = `rc.right-rc.left`, height = `rc.bottom-rc.top` of the rectangle at pbFormat+0x10..0x1C (VIDEOINFOHEADER.rcTarget) -> +0x10/+0x14.
   Texture size: start at (w,h); if device caps `DAT_00a524ac & 2` (D3DPTEXTURECAPS_POW2) round each up to the next power of two; if `& 0x20` (SQUAREONLY) both = max(w,h).
   Texture object = 0x6094C0, then `texture_create(tw, th, 1 level, engine fmt 3 = R5G6B5)`.
5. **Audio stream** (GUID @0x9D1708 = WMMEDIATYPE_Audio; first one, only if a sound source was passed): same procedure but the wanted subtype is `0x9D16F0`
   (WMMEDIASUBTYPE_PCM; again the last PCM format in the enumeration wins). Format-type must be `0x9D1510` (WaveFormatEx) with `wFormatTag==1`; stores channels (+0x18), sample rate (+0x1C), bits (+0x1A);
   keeps the source pointer (+0x08) and calls `set_looping(false)`.
6. `reader->SetRange(0, 0)` (slot 5; 64-bit start 0, duration 0 = play whole file). Any failure -> close.
7. If the file has audio and a source: allocate a SoundBuffer, fill 0x300 zero bytes (silence) with the stream's PCM format, queue it, `alSourcePlay`, `Sleep(((0x180/channels*16)/bits)*1000/rate + 5)` ms (length of the
   silence + 5 ms), then unqueue the processed buffer. Purpose: prime/start the OpenAL source before the first pump (inferred).

### Pump state machine 0x685A80 (returns)
`video_pump(now_ms)`; returns 0 if closed or neither texture nor sound.
| return | meaning | evidence |
|---|---|---|
| 0 | not open / nothing to do | |
| 1 | playing, nothing to draw (audio-only, or last decoded event was audio, or no frame decoded yet) | |
| 2 | playing and a video frame is in the texture -> draw it | |
| 3 | end of file reached and no video frame was ever shown (audio-only file ended, or no video) -> caller closes | |
| 4 | end of file reached, last video frame still in the texture -> draw it this frame, then caller closes | |
Inside: `status` = last result of 0x685DD0 (0 end, 1 video, 2 audio, -2 = nothing decoded). Return mapping: status 0 -> (has_texture && last_video!=0) ? 4 : 3; status -2 -> (has_texture && last_video!=0) ? 2 : 1; status 1 -> 2; status 2 -> 1.

## 2. videocap.cpp — developer screen recorder (shipped but unreachable in normal play)
Triggered only from the hidden debug-key handler (0x52F930: key combo `0x4400` toggles; also calls 0x60FB60 / 0x60FD20 for the back-buffer read-back set-up/teardown). Output file `WW_CAP.WMV` in the working directory,
profile file `640x480.prx` read from the working directory (not shipped). Not used by OpenJoan.
| address | proposed name | purpose | evidence | status |
|---|---|---|---|---|
| 0x686350 | `videocap_open(this, path, int fps_unused, int engine_fmt, int w, int h)` | `LoadLibrary wmvcore`, `WMCreateWriter` + `WMCreateProfileManager`, loads `640x480.prx` with `IWMProfileManager::LoadProfileByData` (slot 5), `writer->SetProfile` (slot 4), `SetOutputFilename` (slot 5, wide path), then selects the writer input whose stream is video (GUID 0x9D1918) and the input format whose subtype equals the engine format (2 -> 0x9D18A0 RGB555, 3 -> 0x9D18B8 RGB565, 5 -> 0x9D1888 RGB24, 7/0xB -> 0x9D1870 RGB32); patches the media type (rcSource/rcTarget = w x h, biWidth/biHeight, biPlanes 1, biBitCount = bpp*8, biSizeImage = bpp*w*h) and `SetInputProps` (slot 8); `BeginWriting` (slot 11); `AllocateSample(bpp*w*h)` (slot 13); `GetBuffer` (INSSBuffer slot 6) -> `frame_ptr` | strings `640x480.prx`, `WMCreateWriter`, `WMCreateProfileManager`, `videocap.cpp` | read |
| 0x686F50 | `videocap_close` | `EndWriting` (slot 12), releases sample, writer_adv, writer, `FreeLibrary` | | verified |
Per-frame grab (copy of the back buffer into `frame_ptr` + `WriteSample`) is not in the export: TODO(re): find the writer of `VideoCapture.frame_ptr` (xrefs to the global instance).

## 3. esstate2.cpp — `Script( name )` statement in the state-script parser
| address | proposed name | purpose | evidence | status |
|---|---|---|---|---|
| 0x645DE0 | `esstate2_parse_script_stmt` | handles the keyword statement `Script ( <name> )`: consumes tokens `Script` (type 0x1C), `(` (-0x25), a word/string token (type 3), then loads/compiles the named script file (cached by name) and its companion dialog resource, then consumes `)` (-0x24) | error strings "Expecting Script!", "Expecting ( after Script!", "Expecting script name", "Cannot open script", "Expecting ) for the ending script!" | verified |
| 0x642370 | `tok_peek_skip_newlines` | current token; while type==9 (newline) bump `line_counter` and advance | | read |
| 0x6423E0 | `tok_next` | throws "Unexpected end-of-file." if no token left, advances, then skip newlines | | read |
| 0x4B9C30 / 0x417200 | `tok_current` / `array_at_1based` | token pointer from index; generic 1-based array accessor (`base+0x19`, stride at +0x10, count at +0x0C) | | verified |
| 0x63EB80 | `esstate_error(fmt, line, ...)` | builds "`<file>` `<sep>` line:%d `<msg>`" in the 0x...A69F38 global buffer and logs it through the log sink 0x586CB0 (the script file name comes from `DAT_00a6a218`) | | read |
| 0x648C50 | `script_cache_find(name)` | scans slots from the highest index down for an in-use slot whose name equals `name`; returns index or 0 | | verified |
| 0x648BD0 | `script_cache_add(name, parser)` | appends a 0x10C-byte slot: refcount 1, name, parser pointer | | verified |
| 0x648D20 / 0x648D60 | `script_cache_addref` / `release` | `InterlockedIncrement/Decrement` of slot.refcount; at 0 deletes the parser (vtable slot 0) and removes the slot | | verified |
| 0x586DA0 | `file_open_rel(name, flags, mode)` | `CreateFileA(basepath + name)`; logs "Error opening file %s." | | read |
| 0x5873A0 / 0x587F30 / 0x587260 | `file_read_all` / `file_size` / `file_close` | read whole file (retry dialog "Error reading file %s, Retry ?") | | read |
| 0x63F190 | `esscript_parser_ctor` | 0x70-byte parser object, vtable 0x9DA9C8 | | read |
| 0x63E450 | `parser_alloc(size)` | `operator new` or bump allocation in the parse arena (`DAT_00a6a0bc`) | | read |
| 0x649280 | `esscript_compile(parser, text, filename)` | resets the parser (vtable +8), logs "0 error(s), you're good to go!" style status via 0x6498A0, creates the arena, runs the lexer 0x6840D0, then `finish` (vtable +0x10) | | read |
| 0x6840D0 | `esscript_lex(parser, text)` | character state machine tokenising the script text and pushing tokens to `parser->on_token(text,line)`; see engine/video.md "esstate2" | | verified |
| 0x63F580 | `esstate2_load_companion_res(base)` | builds `<base>EX.res`; if the file exists creates the dialog resource (`dlgres`, 0x637E90/0x637EC0) and loads it, then 0x639560 parses it | string `EX.res`; `dlgres.cpp` | read |
| 0x637EC0 | `dlgres_init(this, buf)` | allocates 0x12C0 bytes, clears counters, optionally ingests `buf` | `d:\ww\uti3d\dlgres.cpp` | read |
| 0x639560 | `dlgres_load_from(name)` | 1129 bytes; TODO(re) not read here (dlgres belongs to the game-logic dialog family) | | not read |

## 4. Callers / globals worth knowing
| symbol | meaning |
|---|---|
| `DAT_00a5d2f0` | the global `VideoPlayer` instance (fields at +0x04 `DAT_00a5d2f4` texture, +0x08 `DAT_00a5d2f8` sound, +0x10/+0x14 `DAT_00a5d300/304` size) |
| `DAT_00a5d874` | ms timestamp of the first pump of the current movie (0 = not started). Caller computes `now - DAT_00a5d874` and passes it to 0x685A80, resetting it to 0 on close |
| `DAT_00a5cf9c` | global sound source used for video/music audio; position set to `DAT_00a69cd8` (listener) and `AL_SOURCE_RELATIVE=1` by the debug caller (0x52F930 key 0x6D "play WW_CAP.WMV") |
| `DAT_00a524ac` | device texture caps (`D3DCAPS8.TextureCaps`): bit 1 = POW2, bit 5 = SQUAREONLY |
| `DAT_00a4885c` / `DAT_00a48860` | back-buffer width / height |
| `DAT_00a6a3c4` / `DAT_00a6a3c8` | cached `wmvcore.dll` HMODULE / "runtime missing, already reported" flag |

## 5. Round-3 additions: who opens movies and music (callers of 0x684650)
Every `video_open` caller in the export (round 3 searched all roots): only these four sites exist.
| address | proposed name | purpose | evidence | status |
|---|---|---|---|---|
| 0x6D6370 | `intro_seq_tick(screen)` | start-up movie sequence, `screen+10` = active flag, `screen+8` = step 1..3 (>=4 -> `0x4D7E70(1)` = leave to the next game state). Step 1: if `DAT_00a5d2f0 != 0` (movie busy) wait; if file `MOVIE\ENLIGHT.WMV` exists, `video_open(path, DAT_00a5cf9c)` and set music-source volume to 0x4D (77) via 0x59B150; if it does not exist remember the tick (`+0xB`) and continue. Step 2: pure wait until 4001 ms after the tick `+0xB`. Step 3: same as 1 with the third 32-byte table entry (offset 0x40 of the same string table, i.e. the intro movie; install log lists `movie\enlight.wmv` 1,994,934 B and `movie\intro.wmv` 18,803,528 B) | string `MOVIE\ENLIGHT.WMV` indexed `(step-1)*0x20`; INSTALL.LOG | verified (entry text of step 3 inferred = `MOVIE\INTRO.WMV`) |
| 0x6D6130 | `intro_skip` | on any key (`DAT_00a51d5e != 0`), mouse button 2 (0x594700), or `DAT_00a51d1d`: `video_close`, `DAT_00a5d874=0`, step++ (step 2 is skipped: 2 -> 3), tick `+0xB` = `DAT_00a500b2` | | verified |
| 0x6D5AE0 | `intro_draw` | step 1/3: pump + letterbox draw (full-screen black when no frame yet); on state 3/4 close and step++ and store tick; step 2: black fill then a logo texture (`*screen`, blitted at 0x100,0x140, layer 1) whose alpha ramps `(tick - t0)*255/2000` clamped to 255 | | verified |
| 0x59AE60 | `music_play(mgr, track_id, mode)` | music manager start of the single background track: stops current (0x59AE00), allocates a player (`operator new(0x30)`+0x6845D0), path = `"MUSIC\\" + trackrow+0x13` (file name field of the music record fetched with 0x4D8BE0(id); valid ids 1..`DAT_00a45d20`), `video_open(path, DAT_00a5cf9c)`; if the player got a sound source: source position = `DAT_00a69cd8` (listener) and `AL_SOURCE_RELATIVE=1`; stores `track`(+4), `mode`(+8), start tick(+0x10)=`DAT_00a500b2`, fade time(+0x18)=8000 ms, then applies volume (+0xC) via 0x59B150; returns `has_audio_stream` | | verified |
| 0x586210 | `ambient_play(mgr, track_id, channel 0..2, mode)` | same for the three ambient/environment channels (sources `DAT_00a5cfa0/a4/a8`; channel 0 can only start if its source exists), fade time 5000 ms, volume via 0x586620 | | verified |
| 0x4E6FE0 | `music_manager_tick` | per-frame choice of music/ambience: **when a movie is playing (`DAT_00a5d2f0 != 0`) only stops music (0x59AE00)**; otherwise picks a music category `local_c` 1..10 from the current game state (state byte `this+0x3A` 3/1/0/0xF -> category 3), battle/peace/etc., looks up a track with 0x4B3940(category, 0, nation) and (re)starts, cross-fading when the category changes; ambient channels 0..2 chosen with 0x4B3C70/0x4B3940(4|5, ...) with 30 s +random timers (`rand%30000+30000`) | | read (selection logic belongs to the audio family) |
| 0x59B070 / 0x5864F0 | `music_is_playing(mgr, track)` / `ambient_is_playing(mgr, track, ch)` | true while mode==1 (loop) or pump state not in {3,4} and not a finished fade-out (mode 2 with remaining 0) | | verified |
| 0x59B1C0 | `music_tick` | mode 1 (loop): `video_pump(DAT_00a500b2 - start_tick)`; on 3/4 `video_seek_ms(0)` and restart tick; mode 2 (fade-out): sets AL_GAIN = `vol*const*remaining/const` (0x59F700) and subtracts the frame delta `DAT_00a500b6` from remaining until 0 | | verified |
| 0x59F700 | `soundsource_set_gain(src, g)` | `AL_GAIN` (0x100A); 0 if the source's mute flag (+0x2C) is set | | verified |
| 0x6845D0 | `video_player_ctor` | memset 0x30, `CoInitialize(NULL)`; `DAT_00a5d2f0` is initialised by the static init wrapper 0x567160 | | verified |
| 0x470630 | `shutdown_all` | calls `video_close` and clears `DAT_00a5d874` during global teardown | | read |
| 0x4D48F0 / 0x52F930 | in-game movie draw / debug keys | game-state byte 2 or 9 draw branch; **only in-game opener found is the debug key 0x6D which plays the captured `WW_CAP.WMV`** (and 0x4400 records it): videocap + this pair is a developer capture/replay tool | | verified |
Conclusion: shipped gameplay only plays `MOVIE\ENLIGHT.WMV` and one more movie at start-up (the intro) through the global player; everything else is `.wma` audio through `MUSIC\<file>`.
Install log lists 44+ `music\*.wma` files (`env_*.wma` ambient loops, `JOA_*.wma` score tracks).

## 6. esstate2 / lexer — corrections and the dialog-ish resource loader
* **0x684590 is not a token classifier.** It is `string_pool_copy(text, len)` -> 0x5A18D0: a chunked string pool (pages of 0xFA8 bytes, `strlen` when `len<0`) that stores the lexeme and returns a pointer to the stored copy. The lexer (0x6840D0) hands that pointer plus the line number to the parser virtual `on_token(text_ptr, line)`; token *types* (3 word, 9 newline, 0x1C `Script`, 0x24/0x25 parens) are assigned inside the concrete parser's `on_token`, which is not among the exported roots. The caller of 0x645DE0 is also not in the export (reached through a vtable). TODO(re): export the vtable at 0x9DA9C8 slots to get the keyword table — it cannot be derived from this round.
* 0x639560 `sound_table_append(base_name)` (the loader behind `<base>EX.res`, called from 0x63F580): opens `"RESOURCE\\" + base + <suffix @0x9DA880>` if it exists, with the generic ResLX reader (0x47CDF0 -> `ilib::reslx` 0x59B920); header: entry count `n` (i16 at +4 of the header block) and an array of `n` names, stride 0x26 (38). It grows the **global sound-effect record table** (record stride **0x93 = 147**, ids start at 0x4001, accessor 0x4D0860 `rec(id) = base + (id-0x4001)*0x93`) by `n` records and fills each new record: name at +0x06 (copied from the file, 38-byte slot), owning script/base name at +0x37, hash of the name at +0x8F (0x7B15CB), enabled byte +0x68 = 1, floats +0x69 = 1.0, +0x6D = 1.0, +0x71 = 2.0, +0x75 = 10000.0 (0x461C4000), +0x79 = 15000.0 (0x466A6000), +0x7D = 1.0, +0x81 = 0, flags +0x85 &= ~0x0F, +0x86/+0x88 = 0 (i16), +0x8A = 0; then re-sorts the table (qsort 0x7B2D55, comparator 0x40431D) and renumbers record +4 = id. Interpretation (inferred, audio family to confirm): volume/pitch/ref-distance/min/max distance defaults of 3-D sound effects. So `<script>EX.res` = per-script **extra sound effects**, not dialogue text. Earlier wording "dialog resource" should be read as this.

## 7. Round-7: vtable 0x9DA9C8 and the movie table (corrections)
* **Movie path table confirmed** (`.rdata` 0x9DE0EC, 3 entries of 0x20 bytes, selected by `(step-1)*0x20`): entry 0 `MOVIE\ENLIGHT.WMV` (step 1), entry 1 `MOVIE\TREVOR.WMV` (publisher logo; its offset 0x20 is **never opened** by 0x6D6370 — steps 1 and 3 use offsets 0 and 0x40; step 2 shows the still logo instead), entry 2 `MOVIE\INTRO.WMV` (step 3, **read**). `TREVOR.WMV` is not in `INSTALL.LOG`, so it is dead data. The table follows the cursor-name strings `_OT`, `CBUT_OV`, `CBUT_CON`, `CBUT_DW`.
* **vtable 0x9DA9C8 is the compiled-script container, not the keyword dispatcher.** Exported bodies (the other slots are thunks to shared base-class code in the 0x9DA980 vtable; only three distinct bodies were produced):
| address | proposed name | purpose | status |
|---|---|---|---|
| 0x63E630 | `esscript_set_owner(this, p)` | `this+4 = p`; 0x645DE0 calls it with 0 on a fresh object (slot 1, vtable +4) | verified |
| 0x63F2C0 | `esscript_clear(this)` | redirects the parse arena globals to this object's arena (+0x68/+0x6C) if present, deletes every element of the arrays at +0x1C/+0x14/+0x18 (via 0x63E7F0, 0x63ECD0/0x63EB00, 0x63F100/0x63F080), deletes the companion table at +0x64 (0x63F4E0) and the arena (0x63F530), then restores the globals (slot 2, vtable +8, inferred) | read |
| 0x63E7F0 | `esscript_free_children` | frees array +0x1C items (virtual dtor with flag 1 + remove), then arrays +0x14 and +0x18 (element removal 0x63EB00/0x63F080), then 0x63E660 | read |
| 0x641C30 | `esscript_memory_size(this)` | diagnostic footprint: `0x641A60()+0x50 +` (if +0x64: `0x638D50()`); 0x641A60 sums `0xC + Σ(array.count*stride(+0x10) + Σ item->vtbl[0x2C]())` over the three arrays (slot 11 of every item class is a `memory_size` method) | read |
| 0x638D50 | `dlgres_memory_size(this)` | `0xF + Σ(strlen+1)` over the 16-byte entries (count i16 at +1, entry buffer ptr at +3 with text pointer at entry+4, shared pool ptr +7 / size +0xB) | read |
Layout is in [EsScriptSlot.md](../structs/EsScriptSlot.md). **There is no keyword table in these functions**: the object only stores three arrays of polymorphic items and the two attached resources. The `Script` keyword (type 0x1C) and the other keywords are decided by the front-end that feeds tokens (the object whose `+4` slot receives `(lexeme, line)` in 0x6840D0, i.e. *not* an instance of 0x9DA9C8, since slot 1 of this class is the owner setter). TODO(re): identify that front-end class (the `this` ECX of 0x649280 / 0x6840D0 in 0x645DE0; xrefs of 0x649280 and RTTI `.?AV` names near 0x9DA980).
* **Correction of the round-3 note**: `<base>EX.res` is attached through the `dlgres` object (0x637E90/0x637EC0 -> loader 0x6381A0): a table of up to 0x12C0/16 = **300 text entries of 16 bytes** with a shared string pool (so it IS dialogue/strings), while 0x639560 separately appends 147-byte sound-effect records from `RESOURCE\<base><suffix@0x9DA880>`; both are triggered from 0x63F580. Which file each reads is unconfirmed.

## 8. the vtable 0x9DA9C8 resolved — this is the **.sce script engine**
Callers of the compiler 0x649280 are the scenario starters `battle::fn_0041CC80 / 0041EA60 / 0041DFE0 / 0041BF40 / 0041D400` (path prefix `SCRIPT\SCENARIO\`), plus 0x645DE0 (`Script(name)` include); the same lexer 0x6840D0 is shared with the particle-script parser 0x677390 ("First word must be ParticleSystem", pscript family). So the class with vtable 0x9DA9C8 (base 0x9DA980) is the **Script** object of the `.sce` language ([formats/sce.md](../../formats/sce.md), [scripting/BUILTINS.md](../../scripting/BUILTINS.md)). Its 12 slots:
| slot (off) | address | proposed name | behaviour | status |
|---|---|---|---|---|
| 0 (+0x00) | 0x63F200 | `script_dtor(flags)` | clear (0x63F250) then delete self if `flags&1` | verified |
| 1 (+0x04) | 0x63E630 | `script_set_parent(p)` | `this+4 = p` (enclosing scope; 0 for a top-level script) | verified |
| 2 (+0x08) | 0x63F2C0 | `script_clear` | frees statement arrays, string table, arena | verified |
| 3 (+0x0C) | 0x649060 | `script_run_step(ctx)` | executes statements from the cursor: takes the current statement (0x585F20) and calls its slot 3 `exec(ctx)`; result -1 = error/abort (resets cursor, returns -1), 0 = suspended (e.g. `WAIT`, returns 0 and keeps the cursor), 1 = statement finished, advance (0x63EE20: `cur < count ? ++cur`), return 1 after the last statement and reset the cursor | read |
| 4 (+0x10) | 0x649140 | `script_parse(ctx)` | parses `<Name> { statements }`: needs >= 2 tokens ("This script is too small to be valid."), token kind 3 = script name ("Must name script.", stored at `this+0x20`, 0x649860), then kind 4 `{` ("Missing opening brace for Script block."), then loops until kind 5 `}`: for each token `0x643550(kind, line)` creates/returns the statement object and calls its slot 4 `parse(ctx)` | verified |
| 5 (+0x14) | 0x643700 | `script_find_by_id(id)` | walks the statement array (+0x14) backwards for an item with `item+8 == 2` (kind "Id") and `id == item+0x21`; if not found asks the parent (`this+4` slot 5) | verified |
| 6 (+0x18) | 0x63EE70 | `script_find_by_name(name)` | walks the named-block array (+0x18) backwards comparing names (`strcmp`-like 0x7C07B0, name at item+0x14), falls back to the parent (slot 6) | verified |
| 7 (+0x1C) | 0x63F140 | `value_ptr()` | value-slot accessor shared with the base class: kinds 1 and 2 return `this+0x10` (inline), other kinds the pointer stored at `+0x10` | read |
| 8 (+0x20) | 0x642470 | `kind()` | returns `this+0x0C` | read |
| 9 (+0x24) | 0x6424D0 | `self()` | returns `this` | read |
| 10 (+0x28) | 0x6424B0 | `null()` | returns 0 | read |
| 11 (+0x2C) | 0x641C30 | `memory_size()` | see section 7 | verified |

**Token kind -> statement-class table** (0x6488D0 `make_statement(kind, line)`, switch; the number is the argument of the statement factory 0x640DB0, i.e. the statement *class id*). Kinds not listed (`0x1D..0x20`) are **named blocks** created by 0x63EF50 (0x48-byte objects pushed in array +0x18, `set_parent` slot 1 = this script) and not by this switch; any other kind prints "Token type does not correspond to a statement!".
| token kind | class id | note |
|---|---|---|
| 0x0B | 2 | `Id` statement (sce.md: "Expecting Id!"; lookup target of `find_by_id`) |
| 0x0C | 3 | `Init` block (sce.md: "Expecting Init!") |
| 0x0D | 4 | |
| 0x0E | 8 | |
| 0x0F | 0x0B | |
| 0x10 | 0x0C | |
| 0x11 | 0x0D | |
| 0x12 | 5 | |
| 0x13 | 6 | |
| 0x14 | 7 | |
| 0x15 | 9 | |
| 0x16 | 10 | |
| 0x17, 0x18, 0x19, 0x1A | 0x14 | `FUNC` / `DIALOG` / `CAMERA` / `ACTION` call wrapper (keyword table 0xA214C8 dump; see [script_vm.md](script_vm.md)) |
| 0x1C | 0x1A | the `Script(name)` call statement (parsed by 0x645DE0) |
| 0x21 | 0x12 | |
| 0x1D..0x20 | variable declarations (array +0x18) | `INT` / `FLOAT` / `STRING` / `VECTOR` (table dump); the 0x48-byte objects are SceVar, not named blocks |
| 1, 3, 4, 5 | punctuation/literals handled outside the switch | 1 integer, 3 word (script name), 4 `{`, 5 `}` |
Names of kinds 0x0D..0x21 (which keyword is which number) are **not visible**: the text -> kind classification lives in the tokenizer sink `add_token(text,line)` (slot 1 of the tokenizer object built by 0x661EA0, vtable 0x9DBB54, base 0x9DCD98 built by 0x684010 with a 12-byte-element dynarray of initial size 0x1E — most probably the keyword table). Its slots are reached only through a jump table and were not in any export. **`TODO(re)`: export vtable 0x9DBB54 slot 1 and the statement classes created by 0x640DB0 (ids 2..0x1A) to finish the keyword/statement table.**

## Open questions
* RESOLVED: `add_token` = 0x6622D0 / classifier 0x662330 with the 44-entry table 0xA214C8, and the 27 statement classes of factory 0x640DB0: see [script_vm.md](script_vm.md), [SceStatement](../structs/SceStatement.md), [SceToken](../structs/SceToken.md).
* TODO(re): the x87 operands of the two `__ftol` calls in the draw sites (scaled size); presumed `ftol(vidW*scale)`, `ftol(vidH*scale)`.
* TODO(re): GUID identities at 0x9D1528 / 0x9D1540 (data section not exported).
* TODO(re): the videocap per-frame writer (see section 2).
* TODO(re): the string table at the `MOVIE\ENLIGHT.WMV` symbol (3 entries of 0x20 bytes): entry 1 text and the third entry; a `strings` dump of `.data` around it.
* Skipped as unrelated depth-0 neighbours: 0x4D48F0 body (debug HUD/renderer), 0x470630 (global reset list), 0x52F930 (debug key switch) beyond the movie/capture branches.
