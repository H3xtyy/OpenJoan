# Game loop, startup, shutdown, game/profile/msglog/world/cpuusage/gameplay — joan.exe (reference build)

Pseudocode: `$JOAN_WORK_DIR/decomp/engine/gameloop/` (325 functions, depth 0-2).
Structs: [GameApp](../structs/GameApp.md), [GameConfig](../structs/GameConfig.md), [GameState](../structs/GameState.md), [World](../structs/World.md),
[ProfileArray](../structs/ProfileArray.md), [MsgLog](../structs/MsgLog.md), [CpuUsage](../structs/CpuUsage.md), [GameplayTable](../structs/GameplayTable.md).
The message loop `0x499960` and the frame scheduler `0x58A830..` are documented in `docs/re/TIMING.md` - not repeated here.
`0x4E5FC0` (per-frame update) and `0x499960` were **not exported** with this family; everything about them below is `TODO(re)`.

Confidence: **V** verified (>=2 consistent readings / strings), **R** read in one function, **I** inferred (naming/semantics).
Source-file attribution: `game.cpp`, `gameplay.cpp`, `cpuusage.cpp`, `profile.cpp`, `msglog.cpp`, `world.cpp` from assert strings (`d:\ww\<file>.cpp`
passed to the tracked allocator 0x58E7B0/0x58EB30/0x58ED70). `WinMain`, `0x4D6C50`, `0x4D6CE0` carry no file string (they are probably `main.cpp`/`app.cpp`).

## 1. Process entry, startup and shutdown

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x420EA0 | `WinMain` | v3d swap check -> CPU check -> CONFIG.DAT load (or defaults + TSC calibration) -> `GlobalMemoryStatus` -> `app_init` -> engine setup -> `game_run` -> `app_shutdown`. No CD check in this (Steam) build | strings `CONFIG.DAT`, `V3d_87 invalid` | V |
| 0x447150 | `v3d_swap_check` | looks up loaded `V3D_87.DLL`, probes `V3D_SSE/…` via exported `vertexf_check_cpu`, copies the best DLL over `V3D_87.DLL` and asks the user to restart (message boxes). Returns true = "restart needed" so WinMain exits. Not needed in OpenJoan (SIMD is compiled in) | strings | V |
| 0x459E70 | `app_pre_config_noop` | empty; called before CONFIG.DAT | body | V |
| 0x45AAF0 | `config_load(path)` | opens `CONFIG.DAT` via the ilib File wrapper (0x586DA0), requires `GetFileSize == 0xB05`, then `GameConfig::read` (0x45ADC0); on success clears bytes `+0x14..+0x16` (see GameConfig). Returns 0 on missing/short file | consts 0xB05 | V |
| 0x45ADC0 | `GameConfig::read(file, keep_prefs)` | saves ~35 preference fields + the 0xA50 block at `+0xB5`, `ReadFile`s 0xB05 raw bytes over `this`, and if `keep_prefs != 0` restores the saved fields. WinMain passes 0 (file wins). Callers with 1 are outside this export (options screen/profile switch) | decompile | V |
| 0x459E30 | `config_defaults` | calls 0x459EC0, 0x459F30, 0x459FB0, 0x45A010 (see §2) | body | V |
| 0x45C8C0 | `cpu_calibrate_tsc` | `rdtsc`, `Sleep(100)`, `rdtsc`, scaled by `QueryPerformanceFrequency`/counter delta -> TSC ticks per second, stored as u64 at `this+0x44` (`CpuUsage.tsc_hz`, global read as `DAT_00A485EC/F0`). Called only when CONFIG.DAT is missing | body | V |
| 0x4D6C50 | `app_init(hInstance)` | `GameApp::create_window_and_device` (0x603710) then `app_load_services` (0x4D6FE0); then `frame_scale_x=frame_scale_y=1.0f` (`+0x40/+0x44`), `+0x48=1 (u16)`, `+0x4A=1`, `+0x152=1`. Returns 0 on failure -> WinMain returns 0 | body | V |
| 0x603710 | `GameApp::create` | `+0x15=0`, `+0x18=1`, virtual[+0x18] (pre-init), `0x5A1BD0(hInstance)`, `0x6037F0` | body | V |
| 0x5A1BD0 | `GameApp::init_window` | stores hInstance at `+4`, `+0x12=0`, virtual[+8] (create window/D3D), `0x5A1D40`, sets `+0x0F=1` (initialised) | body | R |
| 0x6037F0 | `renderer_init` | `0x4B0930(0)`, `0x5F6620` (device), `0x60F5C0(DAT_00A52428)` (D3D8 device), loads `RESOURCE\PAL_STD.RES` (0x60F760), creates palette/render helpers (0x578D70, 0x604110, 0x608E10, 0x5E9AC0, 0x613730, 0x613BF0 with rect `DAT_00A48880..8C`, 0x613DB0, 0x631850, 0x592380(hInst,hWnd,0), 0x493EB0, 0x596DB0(1), 0x4918D0, 0x592BD0) | body, string | R |
| 0x4D6FE0 | `app_load_services` | ordered service init (audio, sound3d, SE, codepage, textures, fonts, help, gameset) — see engine page §2.2 | body | V |
| 0x45A350 | `se_init_wave1` | `ilib::se` init; if the SE table has a non-zero bank count, opens `RESOURCE\A_WAVE1.RES` (ResLX) and registers it | strings | R |
| 0x4D7890 | `app_unload_services` | reverse of 0x4D6FE0: 0x63C0A0, 0x63F2C0, se::0x59E0C0, 0x57A6C0, sound3d::0x5A04E0, gameset::0x58AE30, help::0x4884C0, 46x `font2d::0x624DC0`, 2x `txtres::0x5A2070`, 0x459E90 | body | R |
| 0x56DD30 | `set_u32_callback(fn)` | stores `0x56DD70` at `*this` (a hook; purpose `TODO(re)`) | body | R |
| 0x4171B0 | `set_two_callbacks(a,b)` | stores `0x4189A0`,`0x418BC0` at `+4/+8` (callback pair used by animation/script systems, `TODO(re)`) | body | R |
| 0x5904E0 | `rng_seed_time` | `time(NULL)` -> `0x590510` -> `*(this+0x7A) = seed`. Called twice in WinMain (two different RNG objects, `TODO(re)` which) | body | R |
| 0x45B240 | `config_clamp_to_caps` | after the device exists: clamps `GameConfig.+0x9C/+0x9D/+0x9E` to what the D3D8 caps/format probes allow (see GameConfig) | body | V |
| 0x6032D0 | `create_depth_surface(w,h)` | `CreateDepthStencilSurface` (device vtable +0x68); WinMain calls it with 0x400x0x400 only when `DAT_00A58055 == 2` (shadow quality 2). Fails -> log `CreateDepthStencilSurface fails (%s)` | string | V |
| 0x603400 | `create_rt_texture(w,h)` | `CreateTexture` render-target (usage 1) 0x200x0x100; log `CreateTexture fails (%s)` | string | V |
| 0x479720 | `game_bringup_and_run` | `0x46F1C0` (reset) -> `0x46F3A0` (load all res tables + interfaces) -> `0x470D40` (world arrays) -> `Calendar(2002,1,1)` (0x495290) -> `0x42CA90(0)` (run loop) -> after loop: `game_world_unload(0)` -> `0x470630` (free res tables) -> `0x46F200` (reset) | body | V |
| 0x46F1C0 | `game_reset_thunk` | forwards to `0x46F200` | body | V |
| 0x42CA90 | `game_run(resume)` | if `resume==0` -> `set_next_state(0xE)`; `0x5959C0` (`TODO(re)`); `DAT_00A50099=1`; **`0x499960(resume)` = main message loop (TIMING.md)**; afterwards stop audio handles (0x59AE00, 0x5861A0(0)) and free per-item sound handles (0x4AEC10, 0x63C800) | body | V |
| 0x4D7E70 | `GameApp::set_next_state(s)` | `+0x3C = s` (requested state; applied by `0x499540`) | 3 callers | V |
| 0x499540 | `GameApp::switch_state(new)` | **state-change dispatcher**: calls the *exit* handler of current state (`+0x3A`), if `new != cur` stores `prev(+0x3B)=cur`, `cur(+0x3A)=new`, `req(+0x3C)=new`, then calls the *enter* handler of the new state. Tables in engine page §3 | body | V |
| 0x4D6CE0 | `app_shutdown` | see engine page §2.3. Order: video player (0x60FD20, 0x686F50, flag `DAT_00A3A30C=0`) -> delete object `DAT_00A5D878` (virtual dtor) -> if `+0x12==0` virtual[+0xC] else if initialised: `+0x10=1`, `0x41C780(0)`, `0x470630`, `0x46F200`, `0x499540(0)`, `0x4D7890`, `0x603780` | body | V |
| 0x603780 | `GameApp::destroy` | `+0x12==0` -> virtual[+0xC]; else if `+0xF`: `+0x10=1`, `0x6039D0`, `0x5A1C40` (device/window teardown) | body | R |
| 0x41C780 | `world_unload(keep_arrays)` | `if (scenario!=5 \|\| keep_arrays==0) 0x5624C0()`; `game_world_unload(keep_arrays)` (0x471010); `*this=0` | body | V |
| 0x4D48F0 | `game_draw` | per-frame draw; state-dependent (owned by lights/UI docs; engine page §3.2 lists the state switch and the pass order) | body | V |

## 2. game.cpp — Game object `GameState` (singleton at 0xA58F90)

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x46F200 | `game_reset` | if `active(+0)`: free `list_a` (+0x71C) and `list_b` (+0x728), zero counts/caps (+0x720..0x733), run `0x57DD30` (audio-device release for 8 slots + `0x57DE40`, `0x57DF80(1..4)`), `0x59AE00`, `0x5861A0(0)`, `0x4AEC10`, `0x63C800`, then `active=0` | `game.cpp` allocs | V |
| 0x46F3A0 | `game_load_resources` | guarded by `+2` (resources loaded). Loads, in order, every `*_res` table and every UI interface (list in engine page §2.4); each stage calls `0x471580(msg)` to update the loading text; ends with `msg=0` and `+2=1` | strings `Loading …` | V |
| 0x470630 | `game_free_resources` | guarded by `+2`: destroys all interface objects, then res tables in reverse (bmpres, music, t_scr, ditem, sitem, recru, item, unit, uscr, upgrade, level, race, sdate, model, anim, animscr, gameplay `0x47C9E0`, bonetree names, remapkey) and sets `+2=0`; also zeroes `DAT_00A5D874` (video clock) | body | V |
| 0x470D40 | `game_load_world_arrays` | guarded by `+4==0`: clears runtime scratch (`+0x308`, `+0x744`, `+0x71B`, `+0x738/0x73C`, `DAT_00A4D765`), and, **unless scenario (+0x17) == 6**, loads: modify_light, world, nation, place, facility, territory, unit, item, modify_item, throw_item, water, water_wave, talk, news, animation_particle_effect, glowing_light, trigger_system arrays + `dyngrass(density=DAT_00A58058, enable=DAT_00A58057)`; sets `+4=1` | strings | V |
| 0x471010 | `game_world_unload(from_save)` | guarded by `+4`: frees the arrays above in near-reverse order, resets the 40 end-condition records (0x467260, `end_con.cpp`), `scenario::0x4CC850`, then `+0x308=0`, `+4=0`. With `scenario(+0x17)==5 && from_save` only per-unit/-profile references are cleared | body | V |
| 0x471530 | `object_delete(p)` | `p` (virtual dtor with delete flag 1) | many callers | R |
| 0x471580 | `loading_text(msg)` | updates the loading-screen text/progress (`0x416580` -> 3 observers) | callers | R |
| 0x472620 | `game_list_a_add_unique(id:i16)` | appends `id` to `list_a` (+0x71C, cap grows +10 u16) if not already present | body | V |
| 0x4729C0 | `game_list_b_add(id:u16)` | appends to `list_b` (+0x728, cap grows +10), duplicates allowed | body | V |
| 0x4727B0 | `game_list_a_save_to_profile` | copies `list_a` into the current profile record's list (`profile+0x28/+0x2C`), freeing it when empty | body | V |
| 0x4728F0 | `game_list_a_load_from_profile` | reverse of the above | body | V |
| 0x472B00 / 0x472C40 | `game_list_b_save_to_profile` / `_load_from_profile` | same for `list_b` (`profile+0x30/+0x34`) | body | V |
| 0x4CA290 | `profile_current` | `index = GameConfig-or-Game +0x2E (i16)`; returns the ProfileArray (0x485790) unless the slot is deleted (0x485900) | body | I |

What `list_a`/`list_b` mean (unlocked scenarios / completed missions / seen cutscenes?) is `TODO(re)`: both are persisted per profile, u16 ids.

## 3. gameplay.cpp — `GAMEPLAY.RES` text index in memory (class [GameplayTable](../structs/GameplayTable.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x47C3D0 | `gameplay_parse_file(path)` | opens `path`, reads the whole file into a tracked buffer, calls the line parser 0x47C6C0, frees | V |
| 0x47C6C0 | `gameplay_parse_text(buf,len)` | line loop (max line 0x109). First char: `NUL/TAB/space` blank, `/` comment, `{name}` select/create category (0x47CCA0), `[key]value` add entry to the current category (0x47C250), `~` stop, anything else abort. Finally sorts categories (qsort 0x47CF20) and each category's entries (0x47C330, comparator 0x47C210) | V |
| 0x47C250 | `gameplay_category_add_entry(raw[0x109])` | grows entry pointer array (u16 count at +0xD), allocates 0x109 bytes, memcpy | V |
| 0x47CCA0 | `gameplay_find_or_add_category(name)` | bsearch (0x7B36E7) by name (0xF-byte object: `char name[9]; Entry** entries(+9); u16 count(+0xD)`); creates on miss | V |
| 0x47C070 | `gameplay_category_free` | frees each entry then the array | V |
| 0x47C9E0 | `gameplay_free` | frees all categories (called by `game_free_resources`) | V |
| 0x47C150 / 0x47CB30 | `entry_at(i)` / `category_at(i)` | 1-based element getter | V |
| 0x47C520 | `gameplay_load(path, use_reslx)` | flag 0: `ResX` open (0x47CB60), for record i = n..1: `size=0x59CA10(i)`, `data=ilib::resx::0x59CBB0(i)` -> `gameplay_parse_text`; then 0x47CAC0 (close). flag 1: same over ResLX (0x47CDF0, 0x59BDD0/0x59BF70, close 0x417A30). The shipped file is a ResX container whose records are text sections (resolved) | V |

## 1b. State-machine handlers and per-frame update 

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x4E5FC0 | `game_update` | per-tick update; ordered list in engine page §4 | V |
| 0x471620 | `scenario_flow_update` | countdown timers `GameState+0x2B4[4][5]` / `+0x304` by `DAT_00A500BE`, end-condition scan, stage switch (`battle::0x41CC80` + state 9) | R |
| 0x476BB0 | `calendar_update` | calls 0x4953D0 on the Calendar object | I |
| 0x51E280 | `units_update` | big unit step (5.4 KB) | I |
| 0x4EF090 | `triggers_update` | trigger-system step (3 KB) | I |
| 0x4E6FE0 | `audio_manager_update` | streams/music (2.2 KB) | I |
| 0x70ACB0 | `main_menu_update` | state 3 logic | R |
| 0x6D6370 | `intro_update` | state 0xE: plays `MOVIE\ENLIGHT.WMV`, then `set_next_state` | V |
| 0x6B5900 | `credits_update` | state 0xF; back to 3 | R |
| 0x75F180 / 0x75FA50 | `scenario_start_update` / `_start_pressed` | state 0xB -> 9 | R |
| 0x755D80 | `scenario_end_continue` | `battle::0x41CC80(scenario+1,1,1)`, next state 0xC | V |
| 0x4797A0 / 0x47B2B0 | `sample_world_enter` / `_exit` | state 2 developer viewer | R |
| 0x47B810..0x47BFD0 | state stubs | empty for states 1,3(enter/exit),4-8; wrappers listed in engine page §3.1 | V |
| 0x45ACB0 | `GameConfig::set_language(idx)` | `+0x0A=idx`, `+0 = (i32)i16_table[idx]` (0x9FB1CA), `+4 = u8_table[idx]` (0x9FB1DB) | V |
| 0x45A8B0 / 0x45A950 | `config_defaults_a` / `_b` | set `+0x45..0x48=1, +0x49=0, +0x53=1` / `+0x4A..0x4E=1` | V |

## 4. profile.cpp — savegame profiles (class [ProfileArray](../structs/ProfileArray.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x4C7E70 | `profile_free_all` | frees every profile's `SaveGame* current` (+0x21) and its 20 slot pointers (+0x27+4k), then the array, the two u16 lists and `+0x40=0` | V |
| 0x4C80C0 | `profile_add(name)` | `count++`, realloc `records` (stride **0x77**), zero the new record, `strncpy(name, 0x21)`; returns record pointer | V |
| 0x4C8490 | `profile_load_index` | builds `SAVE\` + profile-file path (0x5A1170/0x5A1440/0x5A1550), reads 0x48 header bytes + 0x20 more, `count` (int) via 0x587E20, allocates `count*0x77`, reads each record's first 0x20 bytes (name); zeroes list/aux fields. Header-only variant | V |
| 0x4C8BB0 | `profile_load_full` | same, plus per record `0x4C9530` (loads the 20+1 save slots: `operator new(0x4B4)` -> ctor 0x4155E0, payload 0x4C9930 = 0x432 raw bytes + sub-reads 0x47E600/0x47E700/0x47E9F0), then two u16 lists (`+0x28/+0x2C`, `+0x30/+0x34`) and `+0x40` | V |
| 0x4C9420 | `profile_read_name` | `read(buf,0x20)` | V |
| 0x4C9930 | `savegame_read_body` | reads 0x432 bytes, nested readers, then **resets runtime-only fields** to defaults (see ProfileArray §SaveGame resets) | R |

## 5. msglog.cpp — in-game message log (class [MsgLog](../structs/MsgLog.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x4B2940 | `MsgRecord::init(category,flags,text,value,extra)` | fills a record: `+0 u16 category`, `+2 u8 flags`, `+0xF i32 value`, `+0x13/+0x15 u16 lengths`, `+0x17 char* text`, `+0x1B char* extra` (both heap copies) | V |
| 0x4B2C00 | `MsgLog::clear` | walks the global dynarr from the back, frees non-null records (text, extra, record), dynarr clear (0x585D20), `count(+0x2E)=0`, `cursor(+0x32)=0` | V |
| 0x4B2F10 | `MsgLog::remove(index)` | unlinks a record from its doubly linked list(s): decrements the per-category counter, repairs the per-category head and the global cursor, frees strings, `count--` | V |

## 6. cpuusage.cpp — developer CPU-usage profiler (class [CpuUsage](../structs/CpuUsage.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x45C040 | `CpuUsage::init(slots)` | allocates 7 per-slot arrays (sizes N*8, N*8, N*8, N*4, N*8, N*0x1E0, N*0xF0), a table of N+1 nodes of 0x1C bytes (`id` first), a 0x50-byte list, runs the same rdtsc/`Sleep(100)` calibration as 0x45C8C0, then `rdtsc` baseline; `initialised(+0)=1` | V |
| 0x45C5C0 | `CpuUsage::free` | releases everything, `+0=0`, `+4=0` | V |
| 0x45C990 / 0x45E750 | `rdtsc_read(u64*)` | `rdtsc` | V |
| 0x45E7F0 | `CpuUsage::frame_time_ms()` | feeds the "Frame Time:" debug readout (not exported) | I |
| 0x45E860 | `CpuUsage::Node::dtor` | element destructor for the 0x1C-byte nodes | I |

Nothing here affects gameplay; the port may drop it (use a normal frame-time HUD).

## 7. world.cpp — `World` container (class [World](../structs/World.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x547BF0 | `World::add_blocks(n)` | drops the BVH (0x54A040), grows `blocks[]`/`block_aux[]` (+0x7A/+0x82, capacity += 0x10+n), `new WorldBlock` (0xAC bytes, ctor 0x41B5C0, `block->world = this`) n times; `block_count += n`; returns the 1-based index of the first new block | V |
| 0x5482C0 | `World::remove_block(i)` | 1-based; drops BVH, deletes block (0x548430), compacts array (0x591070) | V |
| 0x54A040 | `World::drop_bvh` | `bvh(+0x7E)` array-delete (node dtor 0x427930) and null | V |
| 0x44A260 | `World::build_bvh` | if no BVH and `block_count>0`: temp copy of block pointers (`DAT_00A3CE04`), allocate `2*n-1` nodes x 0x38, recursive split `0x548880(parent=0,lo=0,hi=n)` | V |
| 0x548880 | `bvh_build(parent,lo,hi)` | builds the tree: node AABB = union of blocks `[lo,hi)`; split axis = longest extent (0=X,1=Y,2=Z); median-ish partition via `0x5499B0` (introselect-style with per-axis comparators 0x549C30/0x549DC0/0x549F90); equal-sized degenerate box -> split at `(lo+hi)/2`; leaf links block<->node | V |
| 0x42CE10 | `World::add_scene()` | appends a new en3d scene (0x5EA460, 0xE4-byte object, back pointer to the scene container) to `scenes[]` (+0x8E, cap +10); returns 1-based index | V |
| 0x440320 | `World::destroy` | `unload (0x4426A0)`, remove blocks last->first, drop BVH, free arrays, `+0=0`, `+0x60=0` | V |
| 0x4426A0 | `World::unload_contents` | if `loaded(+1)`: lightsys teardown (0x4A5970), 0x689D00(0), 0x4876D0, 0x4E9F10, dyngrass 0x464600(0) then (density), per-block `0x42AC40`, `DAT_00A48CD0=0`, 0x41BA00, frees `+0x6A`/`+0x6E` objects, `+1=0`, bmpres release | V |

## 8. Cross references

* Main loop / scheduler: [TIMING.md](../TIMING.md) (0x499960, 0x58A830).
* Fonts loaded at startup: [Font2D](../structs/Font2D.md), [fin](fin.md). Audio/OpenAL init: [audio](audio.md). Video: [video](video.md).
* Gameset (`Resource\*.SET` selection at startup): [GameSet](../structs/GameSet.md).
