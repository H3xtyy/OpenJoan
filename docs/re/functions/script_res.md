# script_res family — `t_scrres.cpp`, `uscrres.cpp`, `animscr.cpp` (joan.exe)

Exported pseudocode: `$JOAN_WORK_DIR/decomp/script/script_res/` (call tree in `INDEX.md`), plus consumer code read from
`decomp/engine/{actor3,video5,particles2,camera2,audio3}/`. Structs: [TextureScriptRes (ScenarioScriptRes.md)](../structs/ScenarioScriptRes.md),
[UnitScriptRes](../structs/UnitScriptRes.md), [AnimScript](../structs/AnimScript.md). Format specs: [animscr.md](../../formats/animscr.md),
[unitai.md](../../formats/unitai.md), container [res.md](../../formats/res.md). Table style follows [resdb.md](resdb.md).

Status: **verified** = every field accounted for in the loader and cross-read in a second function; **read** = read once, role clear;
**inferred** = from callers/shape. All addresses are joan.exe. `this` is ECX (`__thiscall`/`__fastcall`).

## 0. Headline corrections (read this first)

1. **`t_scrres.cpp` 0x4DEB70 does NOT load the scenario `.sce` scripts.** It loads `RESOURCE\texteff.res`: the **texture-script**
   (`t_scr` = texture script) bank of animated material definitions (`BIGRED`, `WATERFALL01`, `S_TURNR90`, ...), 51 records of 0xD51 bytes.
   The `.sce` parser/VM lives elsewhere (`0x63F8A0`/`0x63FCA0` per [text_formats.md](text_formats.md), Script object per
   [video.md §8](video.md)); **nothing in this family loads, caches or indexes `.sce` files** (no `.sce` literal, no `SCENARIO\` literal,
   no StepNo handling). Relation to [scr.md](../../formats/scr.md): none — `scenario_NN.scr` is read by `Scenario::load_scr` 0x4CCFB0.
   The script_vm page (`docs/re/functions/script_vm.md`, owned by the script_vm documenter) is the place for `.sce` loading.
2. **`uscrres.cpp` 0x5462B0 reads only the 18 *names* of `unitAI.res`** (never the payloads) and classifies each name into a small integer
   (`ai_class`). The per-class unit AI is **native C++** (switch at 0x5467B0 -> 15 functions 0x531F90..0x533B10); every one of the 18 shipped
   script payloads is `{ exit }` followed only by `//` comments, i.e. the script fallback is a no-op in the shipped game.
3. The ResLX objects in all three loaders are opened with `(path, load_all = 1, shared = 0)` (0x59B920 stores arg2 at +0x11F; non-zero =
   whole file read into RAM and closed). `resdb.md` rows 0x418F30/0x5462B0 call this "streaming": that is wrong for these three callers.

## 1. Shared plumbing (all three loaders)

Open sequence, identical in 0x4DEB70 / 0x5462B0 / 0x418F30:

1. `ResLX` on stack (0x4179F0 ctor; animscr uses `operator new(0x135)` + same ctor) and a 401-byte path buffer.
2. Path buffer: `0x5A1170` clears it (`buf[0]=0`), `0x5A1440` = `strncpy(buf, s, 400); buf[400]=0`, `0x5A1550` = bounded `strcat` (limit 400).
   Paths: `"RESOURCE\\" + "texteff.res"`, `"SCRIPT\\UNIT\\" + "unitAI.res"`, `"RESOURCE\\" + "animscr.res"`. `File::open` (0x586DA0) prepends the
   install root (global 0xA1579C) and aborts via the fatal-error sink 0x586CB0 if the file is missing, so the loaders never see a failed open.
3. `ResLX::open(path, 1, 0)` 0x59B920 -> `count = read_u32` (stored at ResLX+0x121), whole payload area read into RAM, file closed.
4. Table = `tracked_malloc(count * record_size, "d:\\ww\\<module>.cpp", line)` (0x58E7B0, block pre-filled 0xEE) then `memset(0)` of the whole table.
5. For `i = 1..count` (1-based; `recno = i`): `payload = ResLX::get_payload(i)` 0x59BF70 (load-all: `image + off[i-1] - off[0]`),
   `size = ResLX::payload_size(i)` 0x59BDD0 (= `off[i] - off[i-1]`), parse into record `i`.
6. ResLX destructor (0x417A30) closes everything; the payload RAM image is **freed** — records never keep pointers into it.
7. For text banks the payload is wrapped in a `FileTxt` (0x589780: heap copy of `size` bytes plus a **0x1A terminator**, object 0x19A bytes) and consumed
   with the token reader 0x589C70; see "Token reader" below.

### Token reader (0x589C70 `FileTxt::next_token(consume)`) and friends

Verified from the pseudocode; this is the whole lexer of `animscr.res` and `texteff.res`.

- Separators: **space, comma, colon, TAB** (and `=` only terminates a token, it is not skipped *before* one). Newline handling is by CR: when the
  cursor is on `\r`, helper 0x5899B0 skips to the end of the line (`\r`, optional `\n`), then skips leading separators of the next line; it returns 0
  if it hit `0x1A` (EOF). A token ends at space/`=`/comma/colon/TAB/`\r`; at most 0x80 bytes are stored (longer tokens are cut, the cursor still
  advances over the whole token); result is NUL-terminated in the object's buffer at +0x116 and the pointer is returned. EOF (`0x1A`) returns NULL.
- Consequences: files must use CR LF (a bare LF never ends a token; the reader would run through the whole file); a blank value would swallow the next
  line's key; a value containing a space (e.g. `Door broke dust`) is split into several tokens.
- `0x589F10(dst, max)` = "rest of line": skips separators, copies bytes up to `\r`/`0x1A` or `max` into `dst`, NUL-terminates, then calls 0x5899B0.
  (Only used for the `AttachModel` bone name.)
- Key comparison uses `0x7C07B0` (case-insensitive `strcmp`, returns 0 on equal), see [resdb.md](resdb.md).
- Number parsing: `0x7B15CB` = `atoi`; `0x7B2110` = `atof` (float10 return, narrowed to float); `0x7B2020` = `strcpy`.

## 2. `t_scrres.cpp` — texture-script bank `texteff.res` (module name in the PE: `t_scrres`)

| address | proposed name | role | evidence | status |
|---|---|---|---|---|
| 0x4DEB70 | `texscr_load` | open `RESOURCE\texteff.res` (load-all); `count` from ResLX+0x121 stored as **i16 at table+4**; table ptr at +0; `count*0xD51` bytes (assert line `DAT_00A049B8+0xB`); per payload: `recno = i` (u16 at rec+0), `FileTxt(payload,size)`, `texscr_parse_record`. Caller 0x4DEAE0 then sets init byte table+6 = 1 | `d:\ww\t_scrres.cpp`, `texteff.res` literals | verified |
| 0x4DED90 | `texscr_parse_record(FileTxt*, rec*)` | **positional** parser (key tokens are read and discarded; only the value tokens matter, so key spelling is cosmetic): header `Version`, `NumMaterial`, `ModelName`; then per material `MaterialID`, `NumLayers`, `AlphaValue`, `AlphaValuePerSecond`, and per layer the 20-odd layer fields. Layout in [ScenarioScriptRes.md](../structs/ScenarioScriptRes.md). Last step: scan the **model table** from `DAT_00A50568` (count) down to 1 via accessor 0x41A180 (stride 0x2B, base ptr at +8, name at +10) for the first entry whose name equals `ModelName` (case-insensitive) and store the record's `recno` as a 32-bit value at **model_entry+0x16** | strings `NORMAL`, `OBJECT`, `true`; stores seen at fixed offsets | verified (model-table identity inferred = modelres table) |
| 0x4DF9F0 | `texscr_parse_d3d_enum(name)` | maps a lower-case word to a D3D8 texture-stage constant: TOP names (`disable`=1, `arg1`=2, `arg2`=3, `modulate`=4, `modulate2`=5, `modulate4`=6, `add`=7, `addsigned`=8, `addsigned2`=9, `subtract`=10, `addsmooth`=11, `blenddiffusealpha`=12, `blendtexturealpha`=13, `blendfactoralpha`=14, `blendtexturealphapm`=15, `blendcurrentalpha`=16, `premodulate`=17, `modulatealpha_addcolor`=18, `modulatecolor_addalpha`=19, `modulateinvalpha_addcolor`=20, `modulateinvcolor_addalpha`=21, `bumpenvmap`=22, `bumpenvmapluminance`=23, `dotproduct3`=24, `multiplyadd`=25, `lerp`=26, `force_dword`=0x7FFFFFFF) and TA names (`diffuse`=0, `current`=1, `texture`=2, `tfactor`=3, `specular`=4, `temp`=5, `selectmask`=0xF, `complement`=**0x12**, `alphareplicate`=0x20). Anything else returns **0**. `arg1`, `arg2`, `add`, `lerp`, `temp` are inferred from the unreadable literals 0x9D65BC, 0x9D65B4, 0x9D658C, 0x9D640C, 0x9D63C0 (values 2, 3, 7, 0x1A, 5 identify them as D3DTOP_SELECTARG1/2, ADD, LERP and D3DTA_TEMP). `complement` returning 0x12 (not 0x10) is as read | strings table | verified (5 literals inferred) |
| 0x4DEA50 | `texscr_free` | if init byte (+6) == 1: loop `1..count` over accessor 0x4B27B0 (no-op), `tracked_free(table)`, clear ptr/count/flag. Caller: `game_free_resources` 0x470630 | `d:\ww\t_scrres.cpp` | verified |
| 0x4DEB10 | `texscr_release_all` | the per-record loop of the above; nothing to release | | verified |
| 0x4B27B0 | `texscr_record(i)` | `base + (i-1)*0xD51` | | verified |
| 0x4DEAE0 | `texscr_init_wrapper` | calls 0x4DEB70 and sets init flag; reached from `game_init_resources` | gameloop2 | verified |

The runtime consumer of the loaded records (animating UV offset/alpha per frame, building material stages) is not in this export:
TODO(re): find readers of `model_entry+0x16` (texture-script recno) in modelres/actor render code.

## 3. `uscrres.cpp` — unit AI table `unitAI.res`

| address | proposed name | role | evidence | status |
|---|---|---|---|---|
| 0x5462B0 | `uscr_load` | open `SCRIPT\UNIT\unitAI.res` (load-all); table object: `+0 i32 count`, `+4 ptr`, `+8 u8 init` (set by wrapper 0x5461D0); array `count * 0x28` (assert line `DAT_00A0A890+8`); per entry `i`: `u16 recno = i` @+0, `strncpy(rec+2, entry_name(i), 0x20)` where `entry_name` = `ResLX::entry(i)` 0x59C3B0 (pointer to the 38-byte index entry, name first), `u16 ai_class @+0x26 = uscr_class_from_name(rec+2)`. **The payload is never read** | `SCRIPT\UNIT\`, `unitAI.res`, `d:\ww\uscrres.cpp` | verified |
| 0x546520 | `uscr_class_from_name` | case-insensitive ladder, see table in [unitai.md](../../formats/unitai.md) §3 (`human_light_infantry`=1 ... `cart`=15, others incl. `PIG`=0) | 15 literals | verified (2 literals inferred) |
| 0x546140 | `uscr_free` | if init (+8): `uscr_release_scripts`, free array, zero count/ptr, clear flag | `d:\ww\uscrres.cpp` | verified |
| 0x546200 | `uscr_release_scripts` | for `1..count`: if `rec+0x22 != 0` call its **virtual destructor** (`vtable[0] (1)`, i.e. `delete`), then zero `rec+0x22`. So `rec+0x22` is a lazily created `Script*` (video.md §8 Script vtable, slot 0 = dtor) | | verified |
| 0x484540 | `uscr_record(i)` | `ptr + (i-1)*0x28` | | verified |
| 0x5467B0 | `uscr_run_native_ai(rec*, actor*)` | `switch (rec->ai_class)` (u16 @+0x26): 1->0x531F90, 2->0x5322C0, 3->0x532520, 4->0x532780, 5->0x532A00, 6->0x532C60, 7->0x532EF0, 8->0x533170, 9->0x533370, 10->0x533440, 11->0x5335E0, 12->0x5336E0, 13->0x533810, 14->0x533920, 15->0x533B10; default (0) returns 0. Return != 0 means "native AI handled the tick" | actor3 export | verified |
| 0x49D810 | `actor_ai_tick` (owner: actor family) | see §3.1 | actor3 export | read |
| 0x546970 | `uscr_init_native_ai(rec*, actor*)` | CLOSED : does **not** parse any payload. `switch(rec->ai_class)`: 1->0x535C20, 2->0x538340, 3->0x53ABD0, 4->0x53CB10, 5->0x53F670, 6->0x541270(actor,1,0), 7->0x5432A0(…,1,0), 8->0x544C40(…,1,0), 9->0x544E00(…,1,0), 10->0x545380, 14->0x545D00(…,1,0); every other class (0, 11, 12, 13, 15) returns 1. Called from 0x4BF240 as the per-class counterpart (setup/reset) of the think handlers. So `unitAI.res` payload text is never read anywhere in the shipped flow; `rec+0x22` stays 0 and `{ exit }` is never executed | script_vm2 export | verified |

### 3.1 How the table is used per tick (0x49D810, read)

- Correction: the "actor" in this section and in `uscr_run_native_ai(rec*, actor*)` is a **`Unit`** record (0x4B4 bytes, see [Unit.md](../structs/Unit.md)): `+0x3B` = `Unit.type`, `+0x247` = `think_forced`, `+0x299` = `Unit.script` (Script*), `+0x29D` = `last_think_ms`. `0x4FD630` only clears `+0x247`.
- Actor fields: `+0x3B` unit-type id, `+0x247` busy/forced-think flag, `+0x299` `Script*`, `+0x29D` last-think timestamp (ms, global clock `DAT_00A500BA`).
- Unit-type record (accessor 0x42D0A0, unitres): `+0x121` = think interval (ms), `+0x125` = `uscr` recno (u16; 0 = no AI).
- If `actor+0x29D == 0` it is initialised to `now + rand(500)` (`0x590540(500)`), spreading first thinks. When `+0x247 == 0` the actor thinks only if
  `timestamp + interval < now`; when `+0x247 != 0` it thinks every call and then calls 0x4FD630.
- A think = `0x528440()`, then if `ai_recno != 0`: `handled = uscr_run_native_ai(uscr_record(ai_recno), actor)`; if `handled == 0` and `actor+0x299 != 0`:
  `0x63ED40()` then `(*actor->script->vtable[3])(actor)` (**slot 3 = `run_step`** of the Script object, video.md §8); finally `timestamp = now`.
- Port consequence: reproduce the 15 native handlers (task-state machines, not yet documented; TODO(re) 0x531F90..0x533B10 for the game-logic documenter);
  the `unitAI.res` scripts can be loaded but are inert in the shipped data.

## 4. `animscr.cpp` — animation-script bank `animscr.res`

| address | proposed name | role | evidence | status |
|---|---|---|---|---|
| 0x418F30 | `animscr_load` | open `RESOURCE\animscr.res` (load-all); table object `+0 ptr`, `+4 i16 count`, `+6 u8 init`; `count*0x21B` records (assert line `DAT_009F6A6C+0xB`); per payload `i`: `u16 recno=i` @+0, `new FileTxt` (0x19A) over (payload,size) via 0x589780, `animscr_parse_record`, then `FileTxt` dtor+delete (0x419280); finally the ResLX is destroyed (0x4192D0). Wrapper 0x418E90 sets init flag | `animscr.res`, `d:\ww\animscr.cpp` | verified |
| 0x419320 | `animscr_parse_record(FileTxt*, rec*)` | **key-driven** parser (case-insensitive key match, any order) with a **positional** emitter sub-block; defaults applied first: `GravityLevel = 3.0f` (rec+0x48 = 0x40400000), `AttackForce = 3000` (+0x4C), `VerticalAttackForce = 500` (+0x50). Full grammar: [animscr.md](../../formats/animscr.md). Ends by calling 0x418C60 | 15 key literals | verified |
| 0x418C60 | `animscr_resolve_emitters(rec*)` | if `rec->emitter_flag`: for each of 6 slots whose name != `"none"` (literal 0x9CF608, case-insensitive): build `name [+ "_default" if terrain_emitter_flag] + <suffix literal 0x9CF5F4>` and resolve with `pemires_find(name, load=1)` 0x4C2060 (see [particles.md](particles.md)); store result at `rec+0x203+4*slot`. The suffix literal is presumably `.txt` (pscript files live in `SCRIPT\PSCRIPT\*.txt`; `run_default.txt`, `walk_s_other.txt`, `landdust_other.txt` exist) | | verified (suffix inferred) |
| 0x418E00 | `animscr_free` | if init (+6): loop `1..count` (0x418EC0: accessor 0x416F20 + empty 0x418D80, nothing is released: emitter templates and attach models are not freed here), free table | `d:\ww\animscr.cpp` | verified |
| 0x416F20 | `animscr_record(recno)` | `base + (recno-1)*0x21B`; **no bounds check** | | verified |
| 0x41A0E0 | `animscr_find_by_name(name)` | CLOSED: linear scan `i=0..count-1`, `0x7C07B0` (case-insensitive, full string) of `rec[i].code (+2)` vs `name`; returns `i+1` of the **first** match, else 0 (duplicate `ENG_H_INF_ATK_AA` resolves to the earlier record). Caller `animgrp::fn_00414a30` (rtrimmed column at ANIMATE-row +0x9A) stores it at animgrp-entry +0x72 | script_vm2 export | verified |
| 0x419CF0, 0x419D50, 0x419FF0, 0x41A020 | (not exported except 0x41A020) per-record async load / is-loaded / release | 0x41A020: spin `while(!is_loaded(rec)) { SleepEx(5); step(); }`, then `--rec->refcount (+0x24)`; at 0 and `attach_model (+0x1D9) != 0`: release model (0x4B16A0) and, if `rec+0x1DD != 0`, release that handle (0x417E00) | actor/camera2 exports | read |

### 4.1 Consumers of the record (what the numbers do)

- Lookup chain: unit/animation DBF row (`ANIMATE` table, see [anim.md](anim.md)) -> animgrp entry `+0x72` = animscr recno -> `animscr_record(recno)`.
  Callers: 0x501190 (attack/action start), 0x504010 (action-table lookup), 0x415300/0x4156D0 (preload/unload every animgrp entry of a group:
  mark `state 1`: for each entry `0x417AF0(anim)` + `0x419CF0(rec)`; state 2: poll until loaded with a 5-tick budget `iVar - t0 > 5`).
- **Attack event timing** (0x501190, read for action ids 0x49, 0x22, 0x3A): the engine builds a timed event with
  `0x4EEF00(kind, sub, ..., owner_id, ...)`, whose time argument is **`animscr.StartTime (rec+0x28) + now`** (`now` = `DAT_00A500BA`, ms), and whose
  reach/radius argument is **`animscr.Range (rec+0x30) + weapon_reach`** (`weapon_reach` = max of the two held items' `+0x32` field). Without a record
  the fallbacks are `weapon_reach + 0x32` (0x49/0x22 branches) and the literals `100.0` and `50.0` (0x3A branch, 0x200/0x10 kinds). See [animscr.md](../../formats/animscr.md) §4.
- `EmitterFileName`/`SoundScriptName` slots: 0x501190 compares `rec+0x77` (slot-0 name) and `rec+0x98` (slot-1 name) with the `"none"` literal to decide
  whether to spawn effects at animation start; `rec+0x1D5` (weapon_type slot 2) selects which hand's item to use for attached effects.
- TODO(re): the readers of `EndTime` (+0x2C), `StartForceTime` (+0x34), the four angles (+0x38..+0x44), `GravityLevel` (+0x48), `AttackForce` (+0x4C),
  `VerticalAttackForce` (+0x50) are inside the damage/physics code reached from 0x4EEF00 -> 0x47E200 (not exported).

## 5. Constants

| value | meaning | function |
|---|---|---|
| 0x21B (539) | sizeof(AnimScript) | 0x418F30 |
| 0xD51 (3409) | sizeof(TextureScript record) | 0x4DEB70 |
| 0x28 (40) | sizeof(UnitScript record) | 0x5462B0 |
| 0x19A | sizeof(FileTxt) | 0x418F30 |
| 0x135 | sizeof(ResLX) | 0x418F30 |
| 400 / 401 | path buffer capacity / size | 0x5A1440, 0x5A1550 |
| 0x80 | max stored token length | 0x589C70 |
| 3.0f, 3000, 500 | animscr defaults GravityLevel, AttackForce, VerticalAttackForce | 0x419320 |
| 6 | emitter slots per animscr record | 0x419320, 0x418C60 |
| 6 materials x 4 layers | texture-script limits implied by array sizes | 0x4DED90 |
| rand(500) | initial AI think phase (ms) | 0x49D810 |

## 6. Cross-module requests

- **resdb.md (owner: resdb documenter)**: rows 0x418F30 and 0x5462B0 say "streaming"; the calls pass `load_all = 1`. Row 0x5462B0 should also say the
  record's `+0x22` is a `Script*` slot and `+0x26` the AI class. `res.md` payload table: `texteff.res` = texture-script text (grammar in
  [ScenarioScriptRes.md](../structs/ScenarioScriptRes.md)); `unitAI.res` payloads are `.sce`-dialect scripts that only contain `exit`.
- **Ghidra names**: `0x4DEB70 texscr_load`, `0x4DED90 texscr_parse_record`, `0x4DF9F0 texscr_parse_d3d_enum`, `0x5462B0 uscr_load`, `0x546520 uscr_class_from_name`,
  `0x5467B0 uscr_run_native_ai`, `0x418F30 animscr_load`, `0x419320 animscr_parse_record`, `0x418C60 animscr_resolve_emitters`, `0x416F20 animscr_record`,
  `0x484540 uscr_record`, `0x589C70 FileTxt::next_token`, `0x5899B0 FileTxt::skip_line`, `0x589F10 FileTxt::read_rest_of_line`. Struct names: `AnimScript` (0x21B),
  `UnitScriptRes` (0x28), `TextureScript` (0xD51).
- **unitres documenter**: unit-type record `+0x121` u32 AI think interval (ms), `+0x125` u16/u32 AI recno into the `uscr` table (0x49D810).
- **script_vm documenter**: Script vtable slot 3 (+0xC) is called by the actor AI tick as `run_step(actor)` after `0x63ED40()`; the `uscr` record keeps the owning
  `Script*` at `+0x22` and deletes it through slot 0. The `.sce` loader is NOT in `t_scrres.cpp` 0x4DEB70 (see §0).
- **actor documenter**: actor `+0x299` Script*, `+0x29D` last AI time, `+0x247` AI-forced flag; animgrp entry `+0x72` = animscr recno.
- **particles documenter**: `animscr_resolve_emitters` is a caller of `pemires_find` 0x4C2060; name = file stem + optional `_default`, suffix literal 0x9CF5F4.

## 7. Open questions

- 0x546970 (attach script to actor / lazy parse of `unitAI.res` payload): export and read; shows where payloads are read and whether any `script\unit\*.txt` is ever opened (none exist in the install).
- (closed in round 2: 0x41A0E0 and 0x546970, see §3/§4 tables; 0x546970's per-class init targets 0x535C20..0x545D00 are undocumented.)
- 0x419CF0 / 0x419D50 / 0x419FF0: per-record async resource load (emitter templates and attach model).
- 0x47E200 (called by 0x4EEF00): where `AttackForce`, `StartForceTime` and angle windows are consumed.
- Suffix literal 0x9CF5F4 and "none" literal 0x9CF608, key literals 0x9CF750 (`Code`) and 0x9CF644 (`END`) are inferred from the file contents, not read from `.rdata`.
- Model table identity at 0x41A180 (stride 0x2B, count `DAT_00A50568`) is inferred to be the modelres table; confirm from `modelres.cpp`.
