# World arrays — the per-scenario array set loaded by `game_load_world_arrays` (0x470D40)

Reference build: joan.exe. Pseudocode: `$JOAN_WORK_DIR/decomp/game/world_nation/` (+ `game/savegame/`, `game/battle/00471010_game__fn_00471010.c`, `script/script_vm/`).
Status: V = read in >= 2 places, R = read once, I = inferred. Container primitives (`dynarr`/`dynarrb`) are specified in [functions/flocksys.md §2](../functions/flocksys.md);
struct pages of the members: [Nation](Nation.md), [Place](Place.md), [Territory](Territory.md), [World](World.md). Function table: [functions/world_nation.md](../functions/world_nation.md).

## Container convention

Every array is a global object wrapping a `dynarrb` of **pointers** (element size 4): `+0xC` count, `+0x10` element size, `+0x19` data pointer, free list at `+0x1D/+0x21/+0x25`
(total 0x29 bytes). Accessors are all the same code: `get(i) = (1 <= i <= count) ? data[i-1] : 0` (`0x41B3F0`), `is_free(i) = get(i) == 0`, `count = *(this+0xC)` (`0x417AD0`).
**Recnos are 1-based slot numbers**; removing an element zeroes the slot and puts its index on a LIFO free list that the next `push` reuses (so recnos of deleted records get recycled; scripts hold recnos across ticks, which is safe only because the owner checks `is_free`).
Typical iteration is `for i = count; i > 0; --i` (descending) unless stated; lookups by name are linear.
The wrapper objects that own the array carry a few extra fields in the bytes after the `dynarrb` (`+0x2E` "loaded" flag, cursors); they are listed per array.
`0x4B64C0`/`0x4B6530` etc. show that creation (`new` + `push`) and destruction (`dtor(1)` + `dynarrb::remove(i)` for `i = count..1`, then `dynarrb::reset` `0x585D20`) are done per element.

## Load sequence (0x470D40, `__fastcall(this = GameState)`)

Guard: only when `GameState.world_loaded (+4) == 0`. First it clears `+0x308`, `+0x744`, `+0x71B`, `+0x738`, `+0x73C`, `DAT_00A4D765`. If `GameState.scenario (+0x17) == 6` **nothing else is loaded** (scenario 6 has no world arrays) but `+4` is still set to 1.
Otherwise it zeroes `+0x309` (u16) and `DAT_00A4D80F`, then runs the 18 steps below, each preceded by a call to `0x471580("Loading <name>")` which is an **empty function** in this build (progress text is discarded), and finally sets `+4 = 1`.

The pairing of load and free functions in the table is by position in the two call sequences (0x470D40 forwards, 0x471010 backwards); only 0x4B0530, 0x4B6530, 0x4876D0 and 0x4E9F10 were confirmed by reading their bodies (all four have the same shape: descending loop, per-element delete, `dynarrb::remove`, `reset`).

Observation that matters for the port: **most `load` routines are empty**. The arrays are static global objects (constructed by the CRT before `main`); `load` only sets defaults where listed. The records are *added later* by the scenario file readers (`world_load_level` 0x4F2F80 and the `Create*` dispatchers), not here.

| # | log string | load fn | what it does | free fn (0x471010 order) | content owner |
|---|---|---|---|---|---|
| 1 | modify_light_array | 0x4B0510 | empty | 0x4B0530 (V): for i = count..1: if slot used: `delete`(0x4B05F0(1)) + `dynarrb::remove`; then `reset` | lights (`CreateLight`, 0x49B970) |
| 2 | world_array | 0x561050 | `this+0x32 = 0`, `this+0x36 = 0`, `this+0x2E = 1` (loaded flag) | 0x561090 | World container ([World.md](World.md)) |
| 3 | nation_array | 0x4B64C0 (V) | **creates one `Nation` per `nation_res` row** (see [Nation.md](Nation.md)); count `DAT_00A47BF8` | 0x4B6530 (V) | nation_res |
| 4 | place_array | 0x4C5240 | empty | 0x4C5260 | `CreatePlace` (0x559FF0 -> 0x55A090) |
| 5 | facility_array | 0x46CDA0 | empty | 0x46CDC0 | `CreateFacility` (0x55AE40) |
| 6 | territory_array | 0x4E4990 | empty | 0x4E49B0 | `CreateTerritory` (0x547590 -> 0x4F9330) |
| 7 | unit_array | 0x51DCC0 (V) | unit-manager defaults, below | 0x51DF00 | `.uni`/STG unit placement |
| 8 | item_array | 0x49E9A0 | empty | 0x49E9C0 | items |
| 9 | modify_item_array | 0x4AF9E0 | empty | 0x4AFA00 | `modify_item` |
| 10 | throw_item_array | 0x4E9C30 | empty | 0x4E9F10 (V) | projectiles |
| 11 | water_array | 0x546D60 | empty | 0x546D80 | water bodies (the per-tick `0x547030` is also empty) |
| 12 | water_wave_array | 0x56ED40 (V) | loads the shared wave model/animation, below | 0x56F010 | wave effect actors |
| 13 | talk_res | 0x4E3740 | `0x5919A0(&DAT_00A45FA8, this, 0, 0x32, 1, this+4, 0x2E)` then `dynarrb::reset`: (re)initialises the talk table to capacity 0x32 (50) | 0x4E3790 | talk / dialogue text |
| 14 | news_array | 0x4B9250 | `0x4B92C0()` + `0x4B9340(&DAT_00A48EC8)` (init of the news list) | 0x4B9290 | news |
| 15 | animation_particle_effect_array | 0x41B510 | empty | 0x41B530 | particle effect bindings |
| 16 | glowing_light_array | 0x4876B0 | empty | 0x4876D0 (V: iterate count..1, destroy, reset) | glowing lights |
| 17 | trigger_system | 0x4EEDF0 | empty | 0x4EEE10 | triggers (battle family) |
| 18 | dynamic_grass | `dyngrass::fn_004641A0(DAT_00A58058, DAT_00A58057)` | see [DynGrass.md](DynGrass.md) | `dyngrass::fn_00464390` | grass objects |

Free order in `game_world_unload` (0x471010, reached via `world_unload` 0x41C780; only if `scenario != 5 || param_2 == 0`; with `scenario == 5 && param_2 != 0` the arrays are **kept** and only some per-unit fields are cleared): after interface/HUD teardown it calls
`0x42DF50(0)`, `0x689D00(0)` (particles), `0x459B70`, then the frees in the order **animation_particle (0x41B530), news (0x4B9290), talk (0x4E3790), water (0x546D80), water_wave (0x56F010), throw_item (0x4E9F10), modify_item (0x4AFA00), item (0x49E9C0), unit (0x51DF00), facility (0x46CDC0), place (0x4C5260), territory (0x4E49B0), nation (0x4B6530), `DAT_00A3CE48 = 0`, world (0x561090), modify_light (0x4B0530), glowing_light (0x4876D0), trigger (0x4EEE10), dyngrass (0x464390)`**, then `0x467260()` x 0x28 (clears the 40 end-condition slots), `GameState.+0x309 = 0`, `DAT_00A4D80F = 0`, `scenario::fn_004CC850`, `DAT_00A4D765 = 0`, `+0x308 = 0`, `+4 = 0`.
So unload is the **reverse of load order except world (2)/modify_light (1)** - units and everything that refers to nations/places go first, nations/world last.

### unit_array load defaults (0x51DCC0; `this` = unit manager; owner: unit family)

Sets `u16` values: `+0x30,+0x32,+0x34,+0x36 = 1` and `+0x38,+0x3A,+0x3C,+0x3E = 1`; `+0x40,+0x42,+0x46,+0x48,+0x4A,+0x4C,+0x50,+0x52,+0x54,+0x56 = 1`; `+0x58,+0x5A = 0x32`; `+0x5E,+0x60,+0x62,+0x64 = 10`; `+0x68,+0x6A,+0x6C,+0x6E = 3`.
Loads four selection-ring models through `modelres::fn_004B18D0("MODEL\\", name, 0)` into `+0x70,+0x74,+0x78,+0x7C` for `name = "select", "select2", "select3", "select4"`, each followed by `0x4B0EC0(id,0,0,0)` (model use-count bump). The meaning of the small u16s is TODO(re) (they look like per-team-slot flags, counts and limits).

### water_wave load (0x56ED40)

Wave manager object `W`: if `W.+0x2E == 0`: `W.+0x2F = 800` (i32 **wave activation radius**), build the shared wave animation (`operator new(0x18B)`, ctor 0x5B2390, loaded from `ITEM\` + `Object\Models_Items\WaterWave\SmallWave\anim\`, anim name `"WAVE_EFF"`, loader 0x5B23D0) into `W.+0x37`, build the model (`operator new(0x2D8)`, ctor 0x5AF390, from `...\SmallWave\`, name `"WAVE_EFF"`, loader 0x5AF3D0) into `W.+0x33`, then set bit `0x100` in `+0x20` of every 0x54-byte material record of the model; `W.+0x2E = 1`.

## Per-array element data (what is proven)

| array | element | size | fields |
|---|---|---|---|
| nation_res rows | | 0x4E | see [Nation.md](Nation.md) |
| Nation | `Nation` | 0x1545 | see [Nation.md](Nation.md) |
| unit_res types | | 0x203 | `0x42D0A0(id)`; fields in [Place.md](Place.md) |
| Place / Territory | pointer elements | ? | [Place.md](Place.md), [Territory.md](Territory.md) |
| modify_light | `ModifyLight` | >= 0x24 | below |
| glowing_light | `GlowingLight` | >= 0x55 | below |
| throw_item | `ThrowItem` | >= 0x6A | below |
| dyngrass entries | | 0x66 | [DynGrass.md](DynGrass.md) |

### ModifyLight (one animated light *group*; `0x4B0780` update, `0x49B970` creation)

Created while parsing `CreateLight` blocks (0x49B970): tokens `Group <n>`, `ModifyRate <f>`, `ModifyRange <f>` appear after the fixed light fields; a light with `Group >= 0` joins the entry whose `+4 == Group` (`0x4B0810`, else a new entry from `0x4B0640` + `dynarrb`), appends itself to the entry's light list (`0x4B0250`), stores `light.+8 = entry`, and (for mode 1 only) calls `0x4AFD20(min, max, rate)` with `min = I - range*I`, `max = I + range*I`, `rate = ModifyRate`, where `I` = the light's `+0x4C` float at creation. The mode (`0x4AFCF0`) comes from a name token (`0x4B03E0`; 1 or 2).

| offset (as `int` index in the decompile) | size | type | meaning | status |
|---|---|---|---|---|
| 0x00 | 4 | i32 | `mode` (1 = random flicker, 2 = ping-pong ramp) | V (0x4AFD60) |
| 0x04 | 4 | i32 | `group` id (script/file `Group`) | V |
| 0x08 | 4 | i32 | `dir` (+1 / -1) | V |
| 0x0C | 4 | f32 | `min` | V |
| 0x10 | 4 | f32 | `max` | V |
| 0x14 | 4 | f32 | `rate` | V |
| 0x18 | 4 | u32 | `phase_start_ms` (absolute sim ms; 0 = not started) | V |
| 0x1C | 4 | i32 | `phase_len_ms` | V |
| 0x20 | 4 | ptr | light list (a dynarr; count `0x417AD0`, element `light*`) | V |

Per-tick `0x4B0780`: for `i = count..1`: if used: `0x4AFD60(entry)` (below); then `if (entry.light_count == 0) { delete entry; remove }` (an entry whose list is empty - or null - is removed). `DAT_00A500BA` = absolute sim time in ms, `DAT_00A500BE` = this tick's step in ms.

`0x4AFD60` writes the value into each member light's `+0x4C` (float, the light's intensity/range scale used by the lights family) and sets the light's dirty flag (`0x5C5C00`: `light.+0x5A = 1`):

* **mode 1 (flicker)**. If `phase_start == 0`, or the progress `t` is `<= 0`: restart: `value = 0`, `dir = 1`, `phase_start = now`, `phase_len = ftol(<random duration>)` where the duration is derived from `rand_unit` (`0x590660 = rand15 * DAT_009D8C10`, `DAT_009D8C10 = 1/32767`, so `u` in [0,1]; the multiplier that turns `u` into milliseconds is hidden by the export, still TODO(re)). `t = (dir == 1) ? (now - start) / len : (start + len - now) / len`. If `0 < t < 1.0`: `value = (max - min) * t^4`. If `t >= 1.0`: `value = max - min`, `dir = -1`, new `phase_start = now`, new random `phase_len` (same call). Output `x = clamp(min + (rand_unit() - 0.5) * 1.0 * value, min, max)` (`0x590B00` = clamp(x, lo, hi)); written to every light. One `rand_unit` call per update plus one per phase restart (so 1-2 RNG draws per entry per tick).
* **mode 2 (ramp)**. Using the first light's value `v`: if `v == min` then `dir = +1`; if `v == max` then `dir = -1` (exact float compare). Each light: `v' = clamp(v + (DAT_00A500BE * rate / 1000.0) * dir, min, max)` (`1000.0 = _DAT_009CF778`). Deterministic, no RNG.

### GlowingLight (`0x487960` per tick, `0x486910` per entry)

Manager has a round-robin cursor at `manager+0x56` (1-based). Per tick: (a) clamp cursor to `[1, count]`; (b) for every used entry `i = count..1`: `0x486910(0)` (fade only, no occlusion test); (c) then, starting at the cursor and wrapping once, run `0x486910(1)` (with occlusion test) on entries until one returns 1 (= it needed the test: `needs_los != 0`), set `cursor = that index + 1` (wrapping to 1 past `count`). So **at most one occlusion-tested light per tick** is refreshed, in round-robin order.
Entry (`this` hidden): `+0x08` f32[3] position, `+0x44` u8 `lit` (fading-in state), `+0x45` f32 `intensity` (current, 0..1), `+0x49` f32 `target` (distance based), `+0x4D` u32 `last_ms`, `+0x51` i32 `needs_los`.
`0x486910(with_test)`: `d = |light.pos - camera position|` (camera from `0x6140A0`/`0x60CC90`); `target = 1.0` if `d <= 1000.0 (DAT_009CF778)`; `1.0 - (d - 1000.0) / 1500.0` (`DAT_009D3778 = 1500.0`) if `1000 < d <= 2500.0` (`DAT_009CFF30`); `0` beyond 2500. With `step = (now - last_ms) * 0.007` (`DAT_009D3774`, per ms: a full 0..1 fade takes about 143 ms):
`with_test == 0`: if `lit == 0`: `intensity -= step` (floor 0) else `intensity = min(target, intensity + step)`. `with_test == 1`: if `target == 0`: fade out and `lit = 0`; else if `needs_los == 0` **or** the line of sight `0x486DE0(scene)` fails: fade out, `lit = 0`; else fade in toward `target`, `lit = 1`. Always `last_ms = now`. Returns `needs_los != 0`.
`0x486DE0` is a ray test against scene objects (planes of collision kinds 0..3: none/sphere/box/cylinder-like via 0x6A0F90, 0x698970, 0x699EC0) from the camera to the light; returns 1 when unobstructed and `d <= 2500.0`. This is rendering-side visibility; a port may re-author it, keeping the fade-rate behaviour.

### ThrowItem / projectile (`0x4913C0` per tick; flight step `0x4FCC00`)

Entry (`uint*` view): `[0]` own id (passed to `0x4EA9C0(id)` = delete + `dynarrb::remove`), `[1]` owner reference (resolved by `0x6AF310`; null -> delete), `[2]` item recno (`0x425D50(recno) != 0` = invalid -> delete), `[5]` start time ms (non-zero = active), `+0x18` f32[3] reference point (launch/aim point), `+0x24` i16 `state` (non-zero -> delete), `+0x26` i32 `persist`, `+0x32` i32 `lifetime_ms`, `+0x36` i32 `max_range`.
Per tick (descending): skip free slots and entries with `[5] == 0`; delete if `state != 0`, item invalid, or owner null. Else `dist = |item.pos(x,y,0) - ref|` (item position via `0x499930`, z forced 0): if `dist > (float)max_range` -> delete. Else if `persist == 0` and (`owner.+0x298 != 0` or `[5] + lifetime_ms < now`) -> delete. Else run the flight step `0x4FCC00`: arrow/bolt kind (`entry.+0xC` 1 or 2) spawns the `"ARROW"` trail particle on first step and updates the aim vector from `owner.+0x142`; on impact (`owner.+0x216 != 0`, once: `entry.+0x6A = 1`) it raises two events (`0x4EEF00` mask `0x2000`, layers 1 and 2) and plays `"EXPLOSIVEHIT1|2|3"` selected by `0x590540` (RNG object method: `s = s*22695477 + 1; ((s >> 16) & 0x7FFF) * n >> 15`, state at `rng+0x7A`); cannon kinds (3, 4) raise event mask `0x40` and play `"CANNONHL"`. Owner/impact semantics belong to the battle/items families; this page only fixes the container behaviour.

### Water / waves (visual only)

`0x547030` (water update) is an empty function. `0x431F20` (wave activation, `this` = wave manager, active when `W.+0x2E != 0`): take the camera-followed unit `u` (recno `*0x4203C0`), box = `u.pos +- W.+0x2F (800)` on all three axes; for every wave actor (descending): compute its position (`0x5AC270` from actor `+0x8C`); if outside the box -> `0x42CF20(0)` (clear actor flag `+0x10A`); else find the water body whose XY rectangle (expanded by the radius) contains it (`0x469D10`), re-parent the actor into that body's actor list (`0x5EFC50`/`0x5EB590`; skip when the actor's owner `+0x18` is 0, the unit's `+0x345` object, or the body's `+0x30`) and set the flag to 1. Purely cosmetic: **no gameplay state**.

## Name/recno lookups of this family (summary)

| lookup | fn | scan | compare | key stored at |
|---|---|---|---|---|
| nation name -> `nation_res` row | 0x4B91A0 | ascending rows, 0x4E stride, returns row+1 | `0x7C07B0` case-insensitive, first match | row +0 |
| row -> live nation recno | 0x4B67E0 | descending recnos | `Nation.+2 == row` | |
| place name -> recno | 0x4C54A0 | ascending, first match | `0x7C07B0` | place +5 |
| territory name -> recno | 0x4E4B20 | **descending, last match** | `0x7C07B0` | territory +4 |
| facility name -> recno | `search_facility_by_name` 0x6548B0 | (facility family) | | |

All names are matched as raw 8-bit strings in the install's code page; no localisation lookup happens (scripts pass the same literal strings the data files contain; `"[0123N]"` string-ids are resolved only for the text builtins listed in BUILTINS.md §1.5, not for these lookups).
