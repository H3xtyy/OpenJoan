# Runtime

How the OpenJoan executable runs the game: the start-up sequence, the data image, every setting of `openjoan.ini`,
the logs and the debug tools. The overview is `ARCHITECTURE.md`, the 64-bit design is `X64.md`, the reading guide
for the code is `ARCHITECTURE.md` and the module READMEs under `src/`. Two builds exist (`src/build_flavor.hpp`): the *debug*
build reads every setting, the *release* (player) build reads only the settings of `src/openjoan_release.ini`;
"debug only" below means "a constant in the release build".

Contents: 1. Start-up sequence, 2. The data image, 3. Settings (`openjoan.ini`), 4. Logs and reports, 5. Debug tools.

## 1. Start-up sequence

Everything below is `src/runtime/` and the executable's entry (`src/runtime/loader/joan_re.cpp`). No code of the
original exists at run time: its addresses are slots of the code table (`joan_code[]`), see `ARCHITECTURE.md`
section 4.

**Before `main`** (static initialisation, no fixed order between files except the first item):

1. The data image start-up runs first (`image_runtime.cpp`: `init_priority(101)` on GCC / Clang, `init_seg(compiler)`
   on MSVC): sections checked, stored pointers relocated (section 2, steps 1-2).
2. Every `JOAN_REPLACE`, `JOAN_REPLACE_YIELDING`, `JOAN_REPLACE_IMPORT` and `JOAN_IMPORT_OVERRIDE` in `src/` registers
   itself (`src/registry.hpp`, `loader/import_overrides.hpp`); nothing is installed yet.

**`run()`** (`wWinMain` on Windows, `main` elsewhere), in this order:

| Step | What | Log line (`openjoan_loader.log`) |
|---|---|---|
| 1 | `platform::exe_dir()`; the working directory becomes the executable's folder (the game opens everything by relative path); the loader log is opened (debug build only) | `openjoan standalone loader` |
| 2 | `report_process()`: image base, ASLR / DEP state (Windows), whether the original's old range 0x00400000..0x00a96000 is overlapped | `image base ...` / `image ...` |
| 3 | settings file chosen (`platform::settings_path`: `openjoan.ini`, else `joan_re.ini`); `[Loader] PoisonDead` read | `[Loader] our code (code slots) + embedded data ...` |
| 4 | `report_image()`: the result of the start-up check and relocation; a failure ends the game with a message box. `PoisonDead=1` fills the dead ranges with 0xCD | `data image: ...`, `section ...`, `data relocations: ...` |
| 5 | `image_runtime::install_stray_guard()` | `stray guard: ...` |
| 6 | `resolve_imports()`: every import slot inside `joan_image` gets the registered override, else a dead-import trap (`loader/dead_imports.inc`), else the DLL's export (Windows only; a DLL whose imports are all overridden or dead is never loaded) | `imports: ...` per DLL, then a summary |
| 7 | `host::start(dir)` (below) | `code slots: ...` |
| 8 | `relocate_code_pointers()`: the image's 1,568 code pointers get the target of their code slot (`data_image::relocate_code`, `code_table::resolve`) | `seal: ... code relocs` |
| 9 | `protect_rdata()`: `.rdata` of the image becomes read-only | `joan_image .rdata ... read-only` |
| 10 | the loader log is closed; the original's CRT entry `fn::WinMainCRTStartup` (our port, in `src/lib/crt`) is called through its code slot. It initialises its own heap, stdio and environment, runs the static constructors of the `__xc` table (the relocated code pointers), calls `WinMain` and ends the process with `exit()` | `reimplementation installed; starting joan.exe entry ...` |

**`host::start`** (`src/runtime/host.cpp`):

1. `hook::load_settings(ini)`: reads `openjoan.ini` (section 3) and names retired keys (section 3.10). Notes the
   `joan_re.ini` fallback in `openjoan.log`.
2. `install_crash_handler()` (section 5).
3. `sound_archives::start()` (`[Audio] SoundsInMemory`) and `platform::files::start_memory_files()`
   (`[Platform] FilesInMemory`): background threads that read the sound archives and the item / particle / script
   files while the game starts.
4. `code_table::begin()`, then `hook::install_all()`: every registered replacement is installed into its slots
   (`redirect_function` for thunk slots, `patch_entry` for entry slots, `patch_import` for IAT slots), except those
   named by `[Hooks] Disable`, the `fix/uninit` module when `[Fixes] UninitReads=0`, and `JOAN_REPLACE_YIELDING`
   entries whose address another module replaces. `openjoan.log` gets one `replace` / `skip` / `FAILED` line each and
   `N of M replacements installed`.
5. `fixes::install()` (the `[Fixes]` patches that are not plain registrations), `display::apply_patches()` and
   `display::install_canvas_hooks()` (display size and HUD scale, `[Display]`).
6. `replay::install()` (`[Debug] Record` / `Replay`): wraps the import slots; it must run after every other slot
   change. `import_census::install()` (`[Debug] ImportCensus`) comes after it.
7. `code_table::seal()`: resolves the table, runs the `JOAN_LATE_CODE` variables. After this nothing may retarget a
   slot. `run()` then checks `code_table::sealed()`.

**Reaching an address nobody replaced** ends the game: the trap stub of that slot logs `original code needed: <address>
<name>` and the crash report (`code_table.cpp`). The 32-bit reference build fires none during the smoke checklist.

**Exit.** The game's CRT `exit()` calls `ExitProcess`, which is our `exit_process` (an import override): it logs the
caller (x64), writes the OpenAL trace summary, calls `host::shutdown()` (counter report of every registered replacement,
the import census, the code-table report) and ends with `std::_Exit`. A call to a dead import does the same through
`dead_import_called` with exit code 3.

## 2. The data image

The original executable's `.rdata`, `.data`, `.bss` and `.idata` are one object of ours, `joan_image`, at their
original offsets from 0x009cf000 (`img::kVa`) to 0x00a70000. Its layout is `src/data/layout.hpp`, its values
are the committed typed source `src/data/rdata.inc` and `data.inc` included by `src/data/image.cpp` into the one
`constinit` object (`src/data/README.md`, module README `src/data/README.md`). It is the only
data path: `joan.exe` is opened neither by the build nor by the game.

**Start-up.** `src/runtime/image_runtime.cpp` runs from a compiler-segment static initialiser, before any other static
constructor of ours could read a pointer out of the image:

1. `data_image::check_sections()` (`src/runtime/loader/data_image.cpp`): every dead range of `src/data/dead.cpp` lies in
   its section (structure only: the values are our own source, a checksum of them would prove nothing).
2. `data_image::relocate(joan_image, img::base())`: the 770 stored data pointers of `src/data/data_relocs.inc`
   move from their original values to `base + offset`; each must still hold its generated value. The 1,568 code
   pointers (vtable entries, CRT initialiser tables, callbacks) keep their original code addresses for now.
3. `openjoan_loader.log` gets the result (`report_image`): image base, both sections with their typed and dead
   bytes, "data relocations applied". With `[Loader] PoisonDead=1` the dead ranges are filled with 0xCD so a game
   run proves nothing reads them.
4. After `host::start` has installed every replacement and `code_table::seal()` has run,
   `relocate_code_pointers()` (`src/runtime/loader/joan_re.cpp`) sets each code pointer to what its code slot holds
   (`data_image::relocate_code`, resolver `code_table::resolve`), then `.rdata` is made read-only and the CRT entry
   is called through its slot.

Nothing of the original is at a fixed address: the executable is relocatable (ASLR), nothing is reserved at
0x00400000..0x00a96000, and a fault at an old address where nothing is mapped is caught by the stray guard
(`image_runtime::install_stray_guard`), logged with its data symbol or code slot, and ends the game.

**Access.** Original addresses are `img::Addr` values, named in `src/data_names.hpp` (`d::g_units`,
`d::s_PANEL`), never plain integers: `img::ref(d::x)`, `img::obj(d::x)`, `hook::global<T>(d::x)`, `img::str(d::x)`.
A pointer value computed at run time goes through the raw overloads of `src/runtime/image.hpp`, which rebase an
unrelocated original address and log it once as a `stray image address`.

**x64.** Stored pointers are 8 bytes, so the x64 object is the compiler's layout of `layout.hpp` (0xa8a6c bytes
instead of 0xa1000; `docs/X64.md`) and an original address maps to its offset through the runs of `src/data/image_xlate.inc`
(`img::offset_of`, constant-evaluated for the names). A 4-byte slot stays `u32`; a widened slot is a pointer and is
read as one.

**Saves.** Records are written and read field by field (`src/save`, `docs/X64.md` section 5). A dword that points into
the image is written to disk as its original address (what the original wrote) and relocated back on load
(`img::save_words` / `load_words`, `Ptr::Image` in `src/save/io.hpp`).

**Verification.** `tests/data_image_reference_test` (`ctest`): structure, relocation at many bases and in place, and
every byte of the committed `.rdata` / `.data` against the reference `joan.exe` when `JOAN_GAME_DIR` holds it
(`Skipped` otherwise). The replay's per-frame state hash covers `.data` / `.bss` of the image through the typed
table `src/runtime/replay/hash_types.inc`, generated from `layout.hpp`.

## 3. Settings (`openjoan.ini`)

`src/openjoan.ini` is the settings file of the debug build; copy it next to the executable as `openjoan.ini`
(`joan_re.ini` is read, with a log note, when `openjoan.ini` does not exist: `platform::settings_path`,
`src/platform/window.hpp`). `src/openjoan_release.ini` is the file of the release build, installed as `openjoan.ini`
too. The file works as it is: a key left out takes the default below, which is the value of a normal game run.
Keys are read once at start (`hook::load_settings`, `src/runtime/registry.cpp`), except those the options menu
changes (`interfac/graphics_page.cpp` writes `[Display]` and `[Graphics]` keys back to the file) and the few read by
their own module (marked "module").

Column "Rel": **R** = also read by the release build; **D** = debug build only (a constant in the release build,
`JOAN_DEBUG_SETTING` in `src/registry.hpp`, with the constant shown when it differs from the default).
Booleans are `0` / `1`.

### 3.1 `[Loader]`

| Key | Default | Rel | Meaning |
|---|---|---|---|
| `PoisonDead` | 0 | D | 1: the proven-unused ranges of the data image (`src/data/dead.cpp`) are filled with 0xCD instead of staying zero, so a run proves nothing reads them. Reported in `openjoan_loader.log` |

### 3.2 `[Hooks]`

| Key | Default | Rel | Meaning |
|---|---|---|---|
| `Log` | 1 | D (0) | write `openjoan.log` next to the executable. The release build has no log at all (`log_line` is empty) |
| `Disable` | empty | D | comma-separated function names (`ResourceIdx::get_index`) or module names (`ilib/resx.cpp`, `crt/heap`, `render/nvlink`, `debug/menu`) whose replacements are not installed; `*` = all except `debug/menu`. Tests only: a disabled function the game reaches ends it with `original code needed` (except `render/nvlink`, which falls back to loading `nvlinker.dll`, and `debug/menu`) |
| `Enable` | empty | D | no longer needed: every replacement is on. An old list is checked (names matching nothing are warned about) and otherwise ignored |

### 3.3 `[Graphics]`

The game's Direct3D 8 device is our OpenGL 4.6 implementation (`src/gl`); the system's Direct3D 8 is never started.

| Key | Default | Rel | Meaning |
|---|---|---|---|
| `GLDebug` | 0 | D | 1: OpenGL debug context; driver errors and warnings in `openjoan.log`, per-frame statistics, pictures of frames 100 and 1000 (`openjoan_frame*.bmp`) |
| `PerfLog` | 0 | D | every N seconds a log line with the frame rate and where the frame time went; 0 = off |
| `StreamCache` | 1 | R | models whose vertices / triangles did not change are drawn from a copy on the graphics card; 0 = copied every frame. Part of the replay's layout-dependent state (`X64_STATE_CHECK.md`) |
| `Shadows` | 1 | D (1) | 1 = the original stencil shadows, 0 = none, 2 = sun shadow map (not in the original). Only while the game's own shadow option is on. The release build always uses 1 |
| `GpuShadowVolumes` | 1 | R | with `Shadows=1`: the volumes are made by the GPU (same shadows, less CPU); 0 = by the CPU as the original |
| `GrassBatching` | 1 | R | each grass patch's draws are recorded once and replayed; 0 = the original path every frame |
| `GrassMultiDraw` | 1 | R | consecutive grass draws with the same state go out as one call; 0 = one call each |
| `GrassBindless` | 1 | R | grass textures passed per draw through bindless handles; 0 = bound one by one |
| `ShadowDistance` | 4000 | D | `Shadows=2`: reach from the camera, world units (Joan is about 79 tall) |
| `ShadowMapSize` | 4096 | D | `Shadows=2`: texels per side |
| `ShadowStrength` | 45 | D | `Shadows=2`: darkness of full shadow, % |
| `ShadowSoftness` | 3 | D | `Shadows=2`: edge softness, shadow-map texels |
| `ShadowFoliage` | 1 | D | `Shadows=2`: trees, bushes and other cut-outs of the transparent pass cast shadows (replayed from the previous frame) |
| `ShadowCasterMargin` | 1500 | D | `Shadows=2`: objects up to this many world units outside the picture still cast; 0 = only what is on screen |
| `BakedShadows` | 25 | D (100) | `Shadows=2`: % of the shadows baked into the levels' ground lightmaps that is kept; 100 = original lightmaps |
| `Water` | 1 | R | 1 = modern water (reflection, refraction, waves, glint; not in the original), 0 = the original flat water |
| `WaterClarity` | 160 | R | `Water=1`: world units the light crosses before it is mostly the water colour |
| `WaterWaves` | 60 | R | `Water=1`: wave strength, % |
| `WaterReflection` | 100 | R | `Water=1`: reflection strength, % |
| `WaterFlow` | 60 | R | `Water=1`: speed of streams and rivers, % of the original (which flows very fast) |
| `CameraFade` | 0 | R | 1 = objects between the camera and the player turn see-through (the original behaviour), 0 = solid |
| `PixelLighting` | 1 | R | 1 = lit models are lit per pixel (not in the original), 0 = per vertex |
| `VSync` | 0 | R | 1 = wait for vertical sync, 0 = off, -1 = adaptive |
| `MaxFps` | -1 | R | frame limiter: -1 = monitor refresh rate, 0 = off, else frames per second (the game's menu animations stall at very high rates) |
| `CullFlip` | 0 | D | 1 = swap front / back face culling (only if models look inside-out) |
| `Borderless` | 1 | R | a full-screen request becomes a borderless window covering the monitor (picture scaled, aspect kept); 0 = the game's window as it is |
| `Anisotropy` | 16 | R | anisotropic filtering of mipmapped 3D textures: 0, 2, 4, 8, 16 (capped by the card); menus, HUD and pixel art untouched |
| `MSAA` | 0 | R | multisampling: 0, 2, 4, 8; alpha-tested edges are not smoothed |
| `TextureFilter` | 1 | R | 1 = trilinear where the game asks for point mip filtering; 0 = as the game asks |
| `AmbientOcclusion` | 1 | R | 0 = off (original look), 1 = on, 2 = show only the occlusion (tuning). The developer menu has a page for it |
| `AmbientOcclusionRadius` | 40 | R | reach, world units |
| `AmbientOcclusionStrength` | 1.8 | R | darkness, 0 (none) to 4 |
| `AmbientOcclusionDistance` | 2500 | R | full effect up to this distance, fading out until twice it |
| `ScreenshotKey` | 122 | D (122) | Windows virtual-key code (decimal; 122 = F11, 0 = off) that saves `openjoan_shotNNN.bmp` next to the executable; F12 is the game's Mission Brief. TODO(docs): how the code maps virtual-key codes on Linux |
| `ShaderLog` | 0 | D | 1 = log every vertex shader `nvlinker.dll` would link (fragments, declaration, tokens, constant registers), each once (module `en3d/helpers22`) |

### 3.4 `[Display]`

| Key | Default | Rel | Meaning |
|---|---|---|---|
| `Resolution` | `desktop` | R | `desktop` = the primary monitor's size, `original` = 1024x768, or `WIDTHxHEIGHT` (scaled to the monitor, aspect kept) |
| `DpiAware` | 1 | R | tell Windows the game handles DPI itself (real pixels at 125 % / 150 % scaling) |
| `UiScale` | `auto` | R | in-mission HUD size: 1 = original pixels, 1.5 / 2 / ..., `auto` = from the resolution (1080p 1, 1440p 1.5, 4K 2.5). A value <= 0 reads as 1 |

### 3.5 `[Audio]` and `[Video]`

Sound goes through OpenAL Soft (`src/lib/openal/openal_soft.cpp`), movies and music through FFmpeg
(`src/media`). `[Video]` has no key in `openjoan.ini`; `media/wm_reader.cpp` reads `[Video] Trace` (below).

| Key | Default | Rel | Meaning |
|---|---|---|---|
| `HRTF` | 0 | D (0) | module: OpenAL Soft's binaural 3D sound: 0 = off (like the original's DirectSound3D panning), 1 = on, `auto` = OpenAL Soft decides |
| `NearSoundsCentered` | 1 | D (1) | module: sounds closer than their reference distance (the hero's own steps and blows) come from the middle, as with the original's OpenAL 1.0; 0 = exact direction |
| `SoundsInMemory` | 1 | R | `WAVE.RES` (80 MB) and the music / ambience tracks (40 MB) are read into memory while the game starts, a mission's voice archives while it loads, each in the background; 0 = from disk each time as the original |
| `Trace` | 0 | D | module: log every OpenAL call of the game to `openjoan_audio.log` |
| `TraceLines` | 20000 | D | module: calls logged in full; after that only failing calls |
| `[Video] Trace` | 0 | D | module, **not in `openjoan.ini`**: 1 = trace of the FFmpeg movie / music reader to `openjoan_video_ffmpeg.log` (`media/wm_reader.cpp`) |

### 3.6 `[Platform]`

The window and its messages are an SDL3 window (`src/platform/sdl_window.cpp`), keyboard, mouse and pads are read
from SDL events (`sdl_input.cpp`), files, clock, critical sections and COM are our own code (`files.cpp`,
`system.cpp`).

| Key | Default | Rel | Meaning |
|---|---|---|---|
| `FilesInMemory` | 1 | R | the item, particle and item-script files (`ITEM`, `TEXTURE\PARTICLE`, `SCRIPT\PSCRIPT`, about 65 MB) are read into memory in the background while the game starts, and `TEXTURE\ITEM` is opened once; 0 = every file opened from disk |
| `CjkFont` | empty | module, both builds | double-byte editions only (a `RESOURCE\CODEPAGE.RES` naming code page 932, 936, 949 or 950): the font that supplies characters the game's fonts lack. Empty or `auto` = the first existing system CJK font (list in the ini comment), a path = that font file, `off` = none. `platform/dbcs.cpp`; not listed in `openjoan_release.ini` |
| `Gamepad` | 1 | always on in release | up to four pads through SDL in the game's joystick slots (left stick X/Y, right stick Z/Rz, D-pad hat, buttons 1-12 = A B X Y LB RB Back Start LS RS LT RT); hot-plugging works. 0 = no pads |
| `PadMenu` | 1 | R | the first pad drives the menus (left stick cursor, D-pad jumps between buttons, A click, B back, right stick scroll); 0 = mouse only |
| `PadCursorSpeed` | 100 | R | menu cursor speed on the left stick, % (100 = about the screen width per second) |
| `CursorSensitivity` | 10 | R | the Cursor Sensitivity slider's position, 0..20 (`[Fixes] MouseOptions`): 10 = the original's speed, 0 = 25%, 20 = 400%. The game writes it when the slider moves |
| `PadDeadzone` | 15 | R | stick deadzone, % of the throw |
| `PadCurve` | 1.0 | R | stick response curve (1.0 linear; 1.5 - 2.0 finer near the centre) |
| `PadTrigger` | 50 | R | how far a trigger goes down before it counts as pressed, % |
| `PadRumble` | 100 | R | vibration strength, % (0 = off) |
| `PadCamera` | 100 | R | right stick as camera (joystick input method) or mouse cursor (RTS mode): speed, % (0 = off) |
| `PadCameraInvert` | 0 | R | 1 = up / down reversed |
| `PadAutoInput` | 1 | R | the input method of INPUT OPTIONS follows the device used last; 0 = as chosen there |
| `CodePage` | `auto` | D (`auto`) | character set of the game's text (`platform/text.cpp`): `auto` = Windows' ANSI code page on Windows, 1252 elsewhere; or 1250, 1251, 1252, 1253, 1254, 1257 |

### 3.7 `[Fixes]`

Corrections of bugs of the original. `0` = exactly as the original, `1` = fixed. All default to 1 and are 1 in the
release build (constants). Fixes that change game state are recorded in a recording's flags
(`src/runtime/replay/stream.hpp`, `kFlag*`) and a replay uses the recording's value, never the ini's.

| Key | Fixes | Where |
|---|---|---|
| `Shaders` | the vertex shader linker's lighting and fog: diffuse no longer replaced by the specular factor, working specular highlights, normals without the object position, per-light spot attenuation and range, fog distance from the real vertex position, lit alpha = material alpha | `render/nvlink` (our replacement of `SHADER\nvlinker.dll`) |
| `GlowDepth` | light glows (lanterns, torches) shining through walls: the original draws them without a depth test (OpenGL renderer only) | 0x00486d10 |
| `SoundSlots` | 12 sound slots in the original, 32 now; with 12, busy scenes used them all and ambient sounds cut the farthest slot short | `game/sound_slots.hpp` |
| `WetSounds` | in rain only the `_WET` version of step / jump / dash sounds played and those recorded without it were silent; they now play their ground version | `game/unknown_unit_core.cpp` |
| `MissingSounds` | sounds the data forgot to name: idle lines of the Bastard of Orleans and Richemont (ORIDLE2, RIIDLE2), attack swings of the Bastard, Richemont and Charles VII (played as AllHeroes heroes) | `game/animgrp.cpp` |
| `IdleRate` | idle variety (IDLE2 / IDLE3, idle voice lines) rolled once per frame in the original: as often at any frame rate as at 60 fps | `game/unknown_unit_addon.cpp` |
| `TriggerSort` | the trigger list is sorted by the records' own key byte (+0x12); the original's comparator 0x0045bdf0 reads it next to the pointer slot, so the order came from neighbouring heap address bits. Recorded: `kFlagTriggerSortFix` | `game/unknown_chatbox.cpp` |
| `UnitStuck` | soldiers escorting their leader no longer run in place against walls: the formation spot moves onto walkable ground, a unit that gets no closer to its next waypoint for a second stops, a spot is not retried for five seconds. Recorded: `kFlagUnitStuckFix`, and the progress check (part 2, the same ini key) `kFlagUnitStuckWatch` | `game/unknown_ai_behav.cpp` `Escort::update_4be770`, `game/unit_stuck.hpp` |
| `ImmovableSiege` | ordering a siege weapon no crew can move (trebuchet, arbalest) to move answers "unable to comply" (`RTS_LTINF_011`) instead of "need more men" (`RTS_SIEGE_005`). Recorded: `kFlagImmovableSiege` | `game/unknown_rts2.cpp` |
| `AnimalPen` | animals outside the walkable area (the pigs in their pens) graze in place instead of walking on the spot. Recorded: `kFlagAnimalPen` | `game/animal_pen.hpp` |
| `MouseOptions` | the input options' camera slider gets ten more steps below the original's minimum (down to a tenth of it), the labels read "Camera Sensitivity" / "Cursor Sensitivity", and a new slider sets the cursor's speed (`src/interfac/mouse_options.hpp`). A recording stores the switch and the cursor value | `src/interfac/mouse_options.hpp` |
| `MissionCount` | the campaign ends after the last `scenario\scenario_NN.scr` in the game folder, not after mission 8, so missions can be added (missions after 8 grade with mission 8's limits). Recorded: `kFlagMissionCount` | `game/mission_count.hpp` |
| `MusicStartWait` | music and ambience start without the original's ~9 ms sleep of the game thread; movies keep it | `uti3d/video.cpp` `Video::open` |
| `CutsceneGrass` | keep the grass when a scripted camera takes over (the original removes it) | 0x00429860 |
| `MouseTurn` | mouse look in floating point, independent of the frame rate (the original turned in whole degrees per frame) | 0x00431790 |
| `UninitReads` | replaces the functions whose original code reads never-written memory (stack garbage) by the value the code meant (module `fix/uninit`). Keep 1: with 0 those functions have no original code to fall back to and trap with `original code needed`. Recorded: `kFlagUninitFix` | module `fix/uninit` |

### 3.8 `[Game]`

| Key | Default | Rel | Meaning |
|---|---|---|---|
| `UnitIndex` | 1 | R | "which units are near" queries (recruiting, target search) walk a kept list of each sector's units instead of every object of the sector; 0 = the original walk (`en3d/scene_lists.hpp`) |
| `ItemPreload` | 1 | R | the models of every droppable item are built while the first mission loads and kept; 0 = built on each drop (`game/item_preload.hpp`) |
| `AnimCache` | 1 | R | converted animations are kept in `ANIM_TMP\` in the game folder; the first start converts all of them (a longer start), later starts load them; only used for exactly the files they came from; 0 = converted on every load (`en3d/anim_cache.hpp`) |
| `AllHeroes` | 1 debug, **0 release** | R | the keys 5, 6, 7 (Bastard of Orleans, Richemont, Charles VII) also take that hero over in action mode; 0 = RTS mode only, as the original. Not in the original. The code default differs by build (`registry.cpp`: `kReleaseBuild ? 0 : 1`); `openjoan_release.ini` ships 0 |

### 3.9 `[Debug]`

All keys of this section are debug-build only unless stated. Several tools compile away in the release build (section 5).

| Key | Default | Meaning |
|---|---|---|
| `Profile` | 0 | every N seconds a sampling profile of the main thread in `openjoan.log` (needs `openjoan.pdb` next to the executable); costs a few % |
| `HitchLog` | 0 | every frame longer than N ms is logged with what happened in it (files opened, shaders compiled, textures made, sounds loaded) |
| `AnimCensus` | 0 | every N seconds: which animations play how often, how long they are, whether they move anything (`STATIC`) |
| `UnitIndexCheck` | 64 (release constant 0) | every Nth `UnitIndex` lookup is compared with the full walk; on a difference the index switches itself off. 1 = every lookup (replay verification), 0 = never |
| `HeapGuard` | 0 | 1 = guard-page heap: each heap block gets its own pages, the first byte written past the end (or into a freed block) faults at the writing instruction and the log names the function. Slow; ignored while recording / replaying. Read once when the heap is created (`guard_heap.cpp`) |
| `ImportCensus` | 0 | 1 = count every call to the original's imports and write `openjoan_imports.txt` at exit |
| `SaveAudit` | 0 | 1 = log every dword of a written save file that lies in the original's address range 0x00400000..0x00a96000 (what it is: data object, vtable, string, thunk, code; the calling code); 2 = also heap values and, per field-wise record, each image / heap value with its record field. Logs only |
| `SaveFieldwise` | 1 | 1 = every save record is written and read field by field (`src/save`); 0 = the old raw writes, **x86 only**: the 64-bit build ignores the key (`registry.cpp`: raw x64 objects are not a save format) |
| `SaveCompare` | 0 | x86 only: every field-wise record is also built the old raw way and compared byte for byte; mismatches logged (`SaveCompare MISMATCH ...`) |
| `MenuKey` | 192 | Windows virtual-key code of the developer menu (module `debug/menu`, on unless `[Hooks] Disable=debug/menu`): 192 = the backtick key, 120 = F9, 119 = F8 (`src/debug_menu.cpp`) |
| `FreecamKey` | 120 | key (virtual-key code) that toggles the free camera: while on, the game gets no input; W A S D move, Q / E down / up, Shift faster, Ctrl slower, right mouse button or arrows turn (`src/freecam.cpp`) |
| `FreecamSpeed` | 1000 | world units per second |

**Record / replay keys** (`src/runtime/replay/replay.cpp`). `--record FILE` and
`--replay FILE` on the command line override `Record` / `Replay`. A relative file name is relative to the game folder.

| Key | Default | Meaning |
|---|---|---|
| `Record` | empty | record the session to FILE: every nondeterministic input (time, keyboard / mouse / pads, window messages, file listings, display modes, sound stream positions ...) plus a hash of the game state at every frame |
| `Replay` | empty | run from FILE instead of real input and compare each frame's hash. Result in `openjoan.log` (`replay:` lines) and `FILE.report.txt`; exit code 0 = identical, 3 = divergence (another call than recorded), 4 = state hash mismatches, 5 = FILE unusable. Needs the same game files, the same save-folder contents and an audio device if the recording had one |
| `ReplayDetHeap` | 1 | while recording / replaying the game's CRT heap is the deterministic heap (section 5). Read by `replay.cpp`, not in `openjoan.ini` |
| `ReplayHashHeap` | 0 | set while recording: also hash the CRT heap (Windows only; experimental) |
| `ReplayHashBackBuffer` | 0 | set while recording: also hash 64x48 samples of the back buffer at each Present (comparable only on the same machine and driver) |
| `ReplayHashTyped` | 1 | set while recording: the typed image hash (`src/runtime/replay/hash_types.inc`); 0 = the old heuristic scheme. A recording is always replayed with its own scheme |
| `ReplayHashSkip` | empty | set while recording: `ADDR[+LEN],...` more original data addresses (0x009f4000..0x00a6d000) left out of the hash (LEN defaults to 4) |
| `ReplayHashSkipReplay` | empty | replay only: `ADDR[+LEN],...` or `@FILE`: more ranges left out by this replay, e.g. the pointer-width-dependent words of an x64 replay of an x86 recording |
| `ReplayHashFiles` | 1 | record size and a fingerprint (first / last 64 KB) of every file the game opens for reading; a replay warns about differing files. 2 = whole files (slow), 0 = off |
| `ReplaySnapshotEvery` | 0 | store the whole normalised image every N frames (~0.5 MB each; 0 = never); a replay then lists the differing words with their data symbols |
| `ReplaySnapshotFirst` | 8 | also a snapshot in each of the first N frames |
| `ReplayEnvDirs` | `SAVE,CONFIG.DAT` | folders / files (relative to the game folder) fingerprinted and carried in the recording, compared before a replay |
| `ReplayRestoreSaves` | 1 | replay: when those folders differ, rename each to `<folder>.before-replay-<date>-<time>`, put the recording's files in place, and move yours back at the end (the replay's copy stays as `<folder>.replayed-...`; nothing is deleted). 0 = warn only |
| `ReplayOnDivergence` | `stop` | `stop` ends the game with the report, `continue` switches to live input |
| `ReplayPace` | 1 | 1 = recorded speed; 0 = as fast as possible (the `MaxFps` limiter is off then; turn `VSync` off too) |
| `ReplayExitAtEnd` | 1 | end the game when the recording ends; 0 = continue live |
| `ReplayDumpPage` | empty | diagnostics: the raw words of that page of the hashed range, every frame, to `<recording>.pages-record.bin` / `.pages-replay.bin` |
| `ReplayDumpImage`, `ReplayDumpImageEvery` (1), `ReplayDumpImageFrom` (0) | empty | diagnostics: the normalised hashed words of every Nth frame from a frame on, for `tools/replay_image_diff.py` (`X64_STATE_CHECK.md`) |
| `ReplayWatch`, `ReplayWatchHits` (48), `ReplayHeapTrace` (4000) | empty | read by `replay.cpp`, not in `openjoan.ini`: hardware write watchpoints on up to four original data addresses (Windows only); the first N heap events logged |

### 3.10 Retired keys

An old ini may still hold keys of modes that no longer exist. They are ignored; the debug build names each one that
has a non-empty value once in `openjoan.log` (`[Section] Key= ignored: a retired key`). The table is `kRetired` in
`src/runtime/registry.cpp`:

| Section | Keys | Was |
|---|---|---|
| `[Loader]` | `Code`, `Data`, `StrayDataGuard` | the code and data modes of the `joan.exe`-based loader; the stray guard is always on |
| `[Hooks]` | `Verify`, `Census`, `Trusted` | verification and census of the replacement installer |
| `[Graphics]` | `Renderer`, `NativeCaps`, `Trace`, `TraceFrame`, `TraceStartup`, `Census`, `DrawProfile` | the Direct3D 8 renderer path, its traces and diagnostics; the renderer is always the OpenGL device |
| `[Audio]` | `OpenAL` | the choice of the OpenAL library; OpenAL Soft is always used |
| `[Video]` | `Decoder` | the choice of movie decoder; FFmpeg is always used |
| `[Platform]` | `Window`, `Input`, `Files`, `System` | the Win32 / DirectInput paths; SDL3 and our portable layer are always used |
| `[Debug]` | `SehSelfTest` | the structured-exception self-test |

## 4. Logs and reports

All files are written next to the executable (the game folder); the loader changes the working directory there. The
release build writes none of the `openjoan*.log` files (`log_line` is empty, no loader log).

| File | Written by | Content |
|---|---|---|
| `openjoan_loader.log` | `loader/joan_re.cpp`, debug build, overwritten each start | the start-up sequence of section 1: image base, data image check and relocations, imports per DLL, stray guard, code-slot statistics (`code slots: N filled / M traps`, `seal: 1568 code relocs`), `.rdata` protection, entry point |
| `openjoan.log` | `hook::log_line`, debug, `[Hooks] Log=1`; appended | settings notes (fallback ini, retired keys), one `replace` / `skip` / `FAILED` line per registration, `N of M replacements installed`, crash and trap reports, `original code needed`, GL driver messages (`GLDebug`), `PerfLog`, `HitchLog`, profiler, `SaveAudit`, `replay:` lines, and at exit the counter report ("calls into reimplemented code, functions never called") and the code-table report. Lines start with a local time stamp |
| `openjoan_audio.log` | `lib/openal/openal_soft.cpp`, `[Audio] Trace=1` | every OpenAL call of the game |
| `openjoan_video_ffmpeg.log` | `media/wm_reader.cpp`, `[Video] Trace=1`, debug build | the FFmpeg movie / music reader's trace; overwritten each start |
| `openjoan_imports.txt` | `import_census.cpp`, `[Debug] ImportCensus=1`, at exit | call counts per original import |
| `openjoan_shotNNN.bmp` | `gl/device.cpp`, `ScreenshotKey` | screenshots as shown (without black bars); the key is also live in the release build |
| `openjoan_frame<N>.bmp`, `openjoan_frame<N>_window.bmp` | `gl/device.cpp`, `GLDebug=1` | frames 100 and 1000 |
| `<recording>.report.txt` | `replay.cpp` | the replay's result: first divergence (frame, call), hash mismatches; the file name is the recording's plus `.report.txt` |
| `ANIM_TMP\` | `en3d/anim_cache.hpp`, `[Game] AnimCache=1` | converted animations (a cache, safe to delete). Written in both builds |

The crash and trap reports go to `openjoan.log`; a fatal start-up error also shows a message box (the debug build's
text names `openjoan_loader.log`).

## 5. Debug tools

Which exist in the release flavour is decided in the code, not by a build option of their own: `kReleaseBuild`
(`src/build_flavor.hpp`) or a `JOAN_DEBUG_SETTING` constant folds the tool away.

| Tool | Files | Switch | Release build |
|---|---|---|---|
| Record / replay | `src/runtime/replay/` (README there) | `[Debug] Record`, `Replay`, `--record`, `--replay` | compiled to no-ops: `ini_string` / `ini_int` return the default (`replay.cpp`), nothing is wrapped |
| Deterministic heap | `src/runtime/det_heap.{hpp,cpp}`, used by `src/lib/crt/heap.cpp` | on while recording / replaying (`ReplayDetHeap=1`) | never used (it follows replay) |
| Guard heap | `src/runtime/guard_heap.{hpp,cpp}` | `[Debug] HeapGuard=1` | off (`!kReleaseBuild` in `requested()`) |
| Save audit | `src/runtime/save_audit.{hpp,cpp}` | `SaveAudit`, `SaveCompare`; `SaveFieldwise` | settings are constants (0 / 0 / 1): no audit; the field-wise saves are the normal save path in both builds |
| Crash handler | `src/runtime/crash.{hpp,cpp}` | always installed by `host::start` | installed, but its report goes through `log_line`, which writes nothing in the release build. TODO(docs): confirm whether any crash information reaches the player in the release build |
| Profiler | `src/runtime/profiler.{hpp,cpp}` | `[Debug] Profile=N` | constant 0 |
| Hitch log | `src/runtime/hitch.{hpp,cpp}` | `[Debug] HitchLog=N` | constant 0 |
| Import census | `src/runtime/import_census.{hpp,cpp}` | `[Debug] ImportCensus=1` | returns at once (`kReleaseBuild`) |
| Counter report | `registry.cpp` `write_counter_report` | `[Hooks] Log` | no counts, no report |
| Developer menu, free camera | `src/debug_menu.cpp`, `src/freecam.cpp` | `MenuKey`, `FreecamKey` (module `debug/menu`) | absent: the release build disables every `debug/` module (`disabled_by_name`, `src/runtime/registry.cpp`) |

**Crash handler.** Windows: an unhandled-exception filter, and on x64 also a vectored handler plus the CRT's
invalid-parameter, pure-call, `SIGABRT` and `std::terminate` hooks. Elsewhere: the platform layer's fault handler
(`platform::memory::add_fault_handler`), `SIGABRT` and `std::terminate`. The report holds the exception, the registers
and a call stack (frame chain plus code addresses found on the stack, each as `module+offset`; on Linux
`-rdynamic` makes our function names visible, `openjoan.pdb` does on Windows). The stray guard
(`image_runtime::install_stray_guard`) and the code-slot traps report in the same style.

**Deterministic heap.** The game orders some arrays by heap address (for example the sound buffer keys), so its
behaviour depends on where blocks land; a replay would diverge if the allocator placed them differently. While
recording or replaying, the CRT heap is therefore one reserved region (512 MB on 32-bit Windows, 1.5 GB elsewhere: `kReserve`) with a bump pointer and LIFO free
lists per size class: identical requests give identical offsets from the base. The base is fixed (the first free of
0x20000000, 0x30000000, 0x40000000, 0x50000000; on x64 the same values plus 0x7e00'0000'0000, so the heap lies far
above 4 GB like any 64-bit address), so replay hashes can name a heap pointer by its offset. Requests above 2 GB are
refused; its size fields are 32-bit on every target (`det_heap.cpp`; its own comment still says "512 MB" for that limit).

**Guard heap.** One reserved region; each block gets whole pages plus a never-committed guard page at its end, and
freed blocks are decommitted but stay reserved. A fault inside the region is described (bytes past the end of
block B of size S, or inside a freed block) with the allocation call stack before the crash report. Mutually exclusive
with record / replay: the replay keeps its heap and a warning is logged.

**Save audit.** `File::file_write` and its variants (`src/ilib/file.cpp`, 0x005872e0) pass each written buffer to
`save_audit::scan`. It exists since the data image moved: it showed which saved dwords were absolute image
addresses (answer: they are written as original addresses, `Ptr::Image`). `SaveAudit=2` names the record field of
each value, a finding for `re/save/records.json`.

**Profiler, hitch log.** The profiler samples the game's main thread about 1000 times a second (dbghelp
`StackWalk64` on Windows, symbols from `openjoan.pdb`; MSVC builds only: elsewhere `[Debug] Profile` only logs that it needs Windows and MSVC, and `profiler::symbol_at` names addresses through `dladdr` for crash reports) and reports every N seconds the "self" and
"total" counts per function, other modules by module name. The hitch log records, for a frame longer than the
threshold, the events counted during it.
