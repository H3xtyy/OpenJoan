# .sce builtin reference — FUNC / ACTION / DIALOG ids 0..407

Reference build: `joan.exe`. Sources: dispatcher `0x649920` (switch on the builtin id, 21 KB) and the 350-odd handler functions it calls
(`0x650B90..0x65FD10`), name table `0xA1A24C` (see [structs/ScriptBuiltinTable.md](../re/structs/ScriptBuiltinTable.md)), call-object exec `0x643B60`,
WAIT exec `0x640370`. Function-level notes: [re/functions/script_builtins.md](../re/functions/script_builtins.md). Lexical level and census of the 42 shipped scripts:
[LANGUAGE.md](LANGUAGE.md) and §6 below.

Documentation status of the 408 ids (0..407): see §0.


> **Corrections from the world/nation analysis (2026-10-22, docs/re/functions/world_nation.md):** id 175
> (`number_of_people_by_nation_in_place`, handler 0x6540E0) returns a **boolean** and id 176
> (`people_by_nation_exist_in_place`, 0x654120) returns a **count** — the two handlers are swapped relative to
> their names; id 182 `set_place_nation_by_code` also rewrites the place's own nation (`Place::set_nation`
> 0x44F5C0) while `set_place_captured` does not; id 203 `set_nation_military_strength` is a no-op (0x4B5580 is
> empty). Place-relative queries take `(place, unit)`; id 111 takes `(type, place, value)`. Place lookup = first
> match scanning ascending, territory lookup = last match scanning descending (case-insensitive). Rows 93, 111,
> 168–174, 177–181, 183–185, 201, 202, 268, 342, 343, 397 are now "read" (R) through that page.

## 0. Coverage summary

| class | ids | count | status |
|---|---|---|---|
| id 0 | sentinel | 1 | empty name, never dispatched (not counted below) |
| name + handler address + arity | 1..315, 325, 328..353 (every id the dispatcher handles that has a name in the exported name table) | **342** | **documented**: id, name, handler address, argument count/widths, census; semantics `R` (handler body or its key lines read; ~50 rows) or `N` (name-derived only; the majority) — column "S" |
| DIALOG names, no FUNC-dispatcher case | 316..322 | 7 | **documented **: DIALOG class vtable `0x9DB084`: exec `0x65FD70`, WAIT poll `0x660E60` (§3a, §5.3) |
| ACTION names, no FUNC-dispatcher case | 323, 324, 326, 327 | 4 | **documented **: ACTION class vtable `0x9DB10C`: exec `0x661060`, WAIT poll `0x6612E0` (§3, §5) |
| ids 354..406 (named from the corrected 407-row table) | 354..406 | 53 | **documented** (§4): name, tag, handler, arity, effect (mostly `R`) |
| id 407 | 407 | 1 | `set_prisoner_can_be_attacked_flag`, documented |

Round 3: the table has 408 rows (sentinel + ids 1..407; loop bound `i <= 0x196` reaches id 407). **407 of the 407 named ids are documented**, including the 7 DIALOG ids (§3a) and the 4 ACTION ids (§3); the only gap left is `set_item_not_show_description` (used by one shipped script, no table row). ACTION: exec `0x661060`, poll `0x6612E0` (§3); DIALOG: exec `0x65FD70`, poll `0x660E60` (§3a); CAMERA class: stub (§3a). Remaining TODO(re) are only sub-effects (see §7).
The dispatcher has 396 `case` labels (ids 1..407 minus 0x13C..0x144, 0x146, 0x147); all are covered by §2 and §4.
Semantics depth: `R` = handler body (or its key lines) was read; `N` = semantics taken from the name only, the handler address is given for a later read; no row invents behaviour beyond that.

The "Args:" / "Returns:" fragments in the semantics cells give the argument roles and the value the handler returns.

Status census of the ids 1..353 table in §2: 321 `R`, 32 `N`.

## 1. How a builtin call executes (all addresses joan.exe)

1. **Parse** — `0x644190` (statement class 0x14, token kinds 0x17..0x1A) accepts `FUNC|DIALOG|CAMERA|ACTION ( args )`:
   message `"Expecting FUNC/DIALOG/CAMERA/ACTION!"` if the token kind is outside 0x17..0x1A; requires `(` (token type −0x25) after the keyword and `)` (−0x24) at the end.
   Token kind → call class (factory `0x640DB0`): 0x17→class 0x15 (object 0x22 bytes, ctor `0x643000`), 0x18→class 0x16 (`0x6430C0`), 0x19→class 0x17 (`0x643180`), 0x1A→class 0x18 (0x24 bytes, `0x643240`);
   by the order of the error message these are FUNC, DIALOG, CAMERA, ACTION (not proven). The first token inside the parentheses must be a word (token type 3) naming the builtin
   (`"Expecting a function name!"`); `0x643C00` stores `class_value_type = 1`, calls `0x661E10 script_builtin_lookup(name, &call+0xC)` → id (u16 at `call+0x20`; on 0 prints `"Wrong function name (%s)!"`), then parses comma-separated (token kind 7) argument expressions,
   each created with `0x63EA20(0x10)` (expression class 0x10) and appended to the call's argument array (`call+0x14`).
   For ACTION (`0x643D40`): id `0x143` (323 `give_order`) → argument 2 is executed once at parse time and resolved with `0x4BBBC0` (order-name → id) into `call+0x22`; ids `0x146`,`0x147` (326, 327 `give_task`, `give_team_task`) → argument 2 resolved with `0x4E4610` (task-name → id) into `call+0x22`.
   The **name table is shared by all kinds**: there is no check that e.g. `add_dialog` is used with `DIALOG(...)`.
2. **Exec** (vtable slot 3, `0x643B60`): evaluates the arguments **from the last to the first** (`for i = argc..1: arg[i]->exec(ctx)`; each argument writes its value into its own `+0x0C/+0x10`), then calls vtable slot 12 (`+0x30`) = the **dispatcher** `0x649920`, and always returns 1 (statement finished).
   A FUNC call therefore never suspends by itself.
3. **Dispatcher** `0x649920`: `this` (ECX) = the call object; fields used: `+0x04` parent, `+0x08` class id, `+0x0C` result value type tag (1 int, 2 float, 3 string), `+0x10` result payload (32 bit), `+0x14` argument array,
   `+0x20` u16 builtin id (switch value). Argument `i` (1-based) is fetched with `0x63E9F0(i)` (bounds checked; returns 0 outside 1..argc), the argument count with `0x63ECD0`.
   Each argument is a **value object**: `+0x0C` type tag, `+0x10` payload. Observed tags (consumer `0x6456F0`, the `IF` condition): 1 = int, 2 = float, 3 = string (payload = `char*`), 0x1B = 3×float vector (payload = pointer to 3 floats).
   The dispatcher reads the payload **raw, at the width the handler declares**: `i16` (recno, upper half ignored), `i32`, `f32`, `u8`, or the string pointer. There is **no int↔float conversion at the call boundary**: a script `INT` passed where a float is expected is reinterpreted bit-wise (the shipped scripts pass `float` literals there).
   The 32-bit return value of the handler is stored into `call+0x10` by every case (also for handlers declared `void`, whose result is then leftover EAX and must not be used by scripts).
   Float results are stored as IEEE-754 single (`fstp`): ids 211, 353 and the cases 0x168, 0x177..0x179. Case 0xD4 (212 `get_game_minute`) stores `__ftol(fmod(DAT_00a47bb4, 60.0))` (an int) although its table tag says float.
   Case 0x162 stores `(float)int`.
4. **String results**: cases 0x78 (120 `get_unit_name`), 0x16B (363, `format_string`) and 0x16C (364): `call+0x10` holds a pointer to a **401-byte buffer** (`operator new(0x191)`, initialised to `""` with `0x5A1170`) allocated on first use and reused; the handler fills it.
5. **Localised string arguments** — cases 0x04, 0x10A, 0x10B, 0x121, 0x122, 0x158, 0x159, 0x15A, 0x16B, 0x194. The string argument `s` is converted before the handler runs:
   (a) walk `this→+4` parents until an object whose class id (`+8`) is 1 (the **Script** object) is found; (b) `table = script+0x64` (`0x6509C0`, the dlgres/`<base>EX.res` text table, see [EsScriptSlot.md](../re/structs/EsScriptSlot.md));
   (c) if `table != 0` and `s` begins with `'['`, `0x6380B0(s)` scans entries 1..count comparing the entry's u16 id with `atoi(s+1)` (`FUN_007B15CB`) and returns the 1-based index (0 = not found); on success the entry text (16-byte entries, text pointer at `+4`, `0x638150`) is copied with `strncpy` limited by `min(len, 400)` and NUL-terminated;
   (d) otherwise the literal `s` is copied with `strncpy(dst, s, 400)` + NUL (`0x5A1440`). The handler receives the 401-byte copy. So `"[0123N]"`-style ids in scripts resolve through the per-script text table, plain text is passed through.
   Cases 0x159/0x15A additionally derive a **voice id**: the script's own name (`script+0x20`, cut at the position returned by `FUN_007B34E0`) and the integer value of an argument go through `0x639380` (→ `0x6391C0(.., 0x4001, short(table+4)+0x4000)`); a result `<1` is truncated to its high half. TODO(re) exact semantics.
6. **Arity overloads** (`0x63ECD0` = argument count decides the branch): 0x128 (296 `start_cut_scene`: n==6 → `0x65AEF0`, else 8 args → `0x65AE60`; both handlers take **one more parameter than arguments**, an implicit constant TODO(re)); 0x129 (297 `start_cut_scene_2`: n==7 → `0x65AEF0`, else 9 args → `0x65AE60`);
   0x12E (302 `change_camera_in_cut_scene`: n==5 → `0x65B030`, else 7 args → `0x65B070`); 0x158 (344 `attention_call_message`: 1 or more args → `0x65CEA0`); 0x159 (345, string + int or string second arg: type 3 → `0x65CF00`, type 1 → `0x65CF40`);
   0x166 (358: n==2 → `0x65DAF0(a,b)`, else `0x65DAF0(a)`); 0x16B (363 `format_string`, switch on n = 2..8 → `0x65E060`); 0x16E/0x171 (366/369: n==0 → handler without argument, else with 1);
   0x173 (371: n==2 two args, else three args → `0x65CB20`); 0x186 (390: n==1 → `0x65F600(arg)`, else `0x65F600()`). Extra script arguments beyond what the chosen branch consumes are evaluated by 0x643B60 and ignored (e.g. `call_gameover_msg(str, var)`).
7. **WAIT** — see §5. `FUNC("pause_script", ms)` only *stores a deadline* (`0x65C4A0`: `DAT_00A6A254 = DAT_00A500BA + ms`); the completion test is `0x6509E0` (slot 13 of the FUNC vtable 0x9DB040): `DAT_00A6A254 < DAT_00A500BA`.

Column legend for §2/§3: `S` semantics depth (R/N); args: kinds as observed in the 42 shipped scripts (`var` runtime int/recno variable, `int`/`float` literal, `str` literal, `expr` arithmetic) followed by the handler's parameter widths
(`w16` i16 recno, `w32` i32, `w8` u8, `f32`, `ptr` string pointer); `nA` = number of `0x63E9F0` fetches in the dispatcher case. Census = calls × observed signatures.
Recno/id conventions (inferred from the handlers): `0x428B70(recno)` ≠ 0 means "invalid/dead unit" and `0x41B3C0(recno)` returns the unit object; `0x425D50(recno)` ≠ 0 means "invalid item/object" and `0x425D20(recno)` returns it; `0x425CE0(recno)` = object definition.

## 2. FUNC builtins by id (1..353 named in the exported table)

| id | name | ty | args (nA, kinds, widths) | ret | handler | S | semantics | census (calls; signatures) |
|---|---|---|---|---|---|---|---|---|
| 1 | random_number | 1 | nA=1 (int) w32 | int | 0x651010 → 0x590540 | R | `random_number(n)`: uniform integer in `[0, n)` from the game's own `ilib` LCG (`rng_next_below`: `s = s*0x015A4E35+1; r15 = (s>>16)&0x7FFF; return (n*r15)>>15`; `n == 0` gives 0). Because the generator is per world, replays are reproducible. RE: 0x651010 -> 0x590540 (docs/re/functions/unit.md section 14). | 1; (int)×1 |
| 2 | set_snow_level | 1 | nA=1 w16 | 1 | 0x651100 → 0x652A10 | R | `set_snow_level(level)`: stores `level` in the weather/snow level used by the renderer (no simulation effect). Returns 1. RE: 0x651100 -> 0x652A10. | — |
| 3 | start_weather_effect | 1 | nA=1 (int) w32 | int | 0x651130 | R | validates the id with 0x41BF10, then copies `DAT_00A47BB4` into `DAT_00A4839C` / `DAT_00A483AC` (weather start times). TODO(re): 0x651130 consumers of the start times | 1; (int)×1 |
| 4 | call_gameover_msg | 1 | nA=1 localised str (+ extra ignored) | int | 0x65EBD0 | R | `call_gameover_msg(text [, ignored])`: stores the localised text (section 1.5) as the game-over message and increments the game-over counter. Returns 1. RE: 0x65EBD0. | 30; (str)×20,(str,var)×10 |
| 5 | search_for_item | 1 | nA=4 w16,w32,w32,w32 (unit, x, y, z) | int | 0x6519D0 → 0x503680 | N | nearest-item search around a point for a unit (returns recno or 0) | — |
| 6 | get_item_rotation | 1 | nA=2 (var,var) w16,w32 | int | 0x653000 | R | `get_item_rotation(unit, item)`: the item's bearing relative to the unit's facing, in degrees, truncated (`TODO(re)` 0x653000 → 0x4FF850 / 0x5059B0 bodies not read; BUILTINS.md corrections note). 0 for an invalid item. Args: item = recno; unit_pos = unit position; unit_yaw = unit facing, radians. Returns: degrees in (-180, 180]. | 4; (var,var)×4 |
| 7 | get_item_dist | 1 | nA=2 w16,w32 | int | 0x653060 | R | `get_item_dist(unit, item)`: `ftol(length(item.pos - unit.pos))`, 0 when the item is invalid (the caller checks the unit). Uses the item's own position (`+0x56`), not the floor holder's. RE: 0x653060. Args: item = recno; unit_pos = the unit's position. Returns: distance in world units, truncated. | — |
| 8 | get_read_letter_once | 1 | nA=0 | int | 0x653110 | R | `get_read_letter_once()`: returns the flag set when the letter interface was closed after reading. RE: 0x653110. | 4; ()×4 |
| 9 | item_code_to_id | 1 | nA=1 (str) ptr | int | 0x651A30 → 0x4A36C0 | R | `item_code_to_id(code)`: template recno or 0. RE: 0x651A30 → 0x4A36C0. Args: code = item code. Returns: recno or 0. | 54; (str)×54 |
| 10 | add_item_ref | 1 | nA=1 (str) ptr | 1 | 0x651A60 | R | resolves the item name (0x4A36C0), registers it (0x4A39F0) and calls `scenario::fn_004cd960`; returns 1 | 1; (str)×1 |
| 11 | add_item | 1 | nA=2 (str,str) ptr,ptr | int | 0x651AD0 | R | creates an item object (0x52-byte object via operator new) from item name + nation name; returns the new recno | 1; (str,str)×1 |
| 12 | del_item | 1 | nA=1 (var) w16 | 1 | 0x651E70 → 0x49F370 | R | `del_item(item)`: deletes and returns 1. RE: 0x651E70. Args: item = recno. Returns: 1. | 30; (var)×30 |
| 13 | del_item_by_name | 1 | nA=1 (str) ptr | 1 | 0x651EA0 | R | `while ((r = search_item_by_name(name)) != 0) del_item(r)` — deletes **all** items with that name | 12; (str)×12 |
| 14 | set_item_pos | 1 | nA=4 (var,float,float,float) w16,f32×3 | int | 0x651F00 | R | if the item is valid, sets its position (0x41FF50(x,y,z)) | 1; (var,float,float,float)×1 |
| 15 | set_item_dir | 1 | nA=2 w16,w32 | bool | 0x651FA0 | R | direction in degrees → radians (`deg * DAT_009CF884`), applied through 0x499FD0 | — |
| 16 | set_item_nation | 1 | nA=2 (var,str) w16,ptr | int | 0x652010 | R | item's owner nation from the nation name (0x4B91A0 → 0x4B67E0) | 54; (var,str)×54 |
| 17 | add_item_box | 1 | nA=7 (str,str,float,float,float,int,str) ptr,ptr,f32,f32,f32,w32,ptr | int | 0x651C90 | R | creates an item box: item code (arg 1), box name copied (0x40 chars) into the object, position x,y,z, rotation in degrees (×`DAT_009CF884`), arg 7 = nation/owner string; returns recno | 10; (str,str,float,float,float,int,str)×10 |
| 18 | search_item_by_name | 1 | nA=1 (str) ptr | int | 0x65B570 | R | `search_item_by_name(name)`: last → first scan; empty name ⇒ 0. RE: 0x65B570. Args: name = instance name. Returns: recno or 0. | 152; (str)×152 |
| 19 | search_item_by_code | 1 | nA=1 (str) ptr | int | 0x65B5B0 | R | `search_item_by_code(code)`: last → first scan on the template code. RE: 0x65B5B0. Args: code = template code. Returns: recno or 0. | 10; (str)×10 |
| 20 | search_item_sell_list_by_code | 1 | nA=1 (str) ptr | int | 0x65B5F0 | R | `search_item_sell_list_by_code(code)`: id of the shop sell list (`sell.res`) with that code, 0 when unknown (`SellTable::find_by_name`). RE: 0x65B5F0. | 38; (str)×38 |
| 21 | is_item_deleted_by_name | 1 | nA=2 (var,str) w32,ptr | bool | 0x65B630 | R | `is_item_deleted_by_name(item, name)`: true when the slot is empty **or** the item at that recno has a different name (guards against recno reuse). RE: 0x65B630. Args: item = recno; name = expected instance name. Returns: the verdict. | 38; (var,str)×38 |
| 22 | is_item_box | 1 | nA=1 w32 | bool | 0x65B6A0 | R | `is_item_box(item)`: `class == 10`. RE: 0x65B6A0. Args: item = recno. Returns: the verdict (false for invalid). | — |
| 23 | is_item_gate | 1 | nA=1 w32 | bool | 0x65B700 | R | `is_item_gate(item)`: `class == 1`. RE: 0x65B700. Args: item = recno. Returns: the verdict. | — |
| 24 | is_item_wall | 1 | nA=1 w32 | bool | 0x65B760 | R | `is_item_wall(item)`: `class == 4`. RE: 0x65B760. Args: item = recno. Returns: the verdict. | — |
| 25 | search_recruit_list_by_code | 1 | nA=1 (str) w32 | int | 0x65B7C0 | R | `search_recruit_list_by_code(code)`: id of the workshop recruit list (`recruit.res`) with that code, 0 when unknown (`RecruitTable::find_by_name`). RE: 0x65B7C0. | 37; (str)×37 |
| 26 | get_gate_hp | 1 | nA=1 (var) w32 | int | 0x650B90 | R | if the item is valid and its type (`+0x4E`)==1: returns `gate->+0xC2` | 4; (var)×4 |
| 27 | get_gate_max_hp | 1 | nA=1 (var) | int | 0x650C10 | R | same guard, returns the gate max-HP field | 3; (var)×3 |
| 28 | set_gate_hp | 1 | nA=2 (var,int) | int | 0x650C90 | R | `set_gate_hp(item, hp)`: writes `hp` of a valid gate. RE: 0x650C90. Args: item = recno; hp = hit points. Returns: 0. | 3; (var,int)×3 |
| 29 | set_gate_max_hp | 1 | nA=2 (var,int) | int | 0x650CF0 | R | writes the gate's max-HP field (`gate->+0xC6`) through the item's virtual accessor (+0x4C); returns 0 | 1; (var,int)×1 |
| 30 | gate_open | 1 | nA=0 | int | 0x652440 | R | `gate_open()` **as decompiled** (BUILTINS.md corrections note): takes no argument and returns the `state` of the item registered in modify-list slot 1, or -1 when that slot is empty. No shipped script calls it. RE: 0x652440. Returns: state or -1. | — |
| 31 | get_gate_state | 1 | nA=1 (var) w32 | int | 0x6524A0 | R | `get_gate_state(item)`: valid and `class == 1` ⇒ `state` (0 closed, 1 open), else -1. RE: 0x6524A0. Args: item = recno. Returns: state or -1. | 260; (var)×260 |
| 32 | set_gate_need_key | 1 | nA=2 (var,int) w32,w8 | int | 0x652520 | R | `set_gate_need_key(item, on)`: -1 for an invalid item. RE: 0x652520. Args: item = recno; on = flag. Returns: the stored value or -1. | 4; (var,int)×4 |
| 33 | get_gate_need_key | 1 | nA=1 w32 | int | 0x652590 | R | `get_gate_need_key(item)`: -1 for an invalid item. RE: 0x652590. Args: item = recno. Returns: the flag or -1. | 1; (var)×1 |
| 34 | set_gate_invincible | 1 | nA=2 (var,int) w32,w8 | int | 0x6525F0 | R | `set_gate_invincible(item, on)`: -1 for an invalid item. RE: 0x6525F0. Args: item = recno; on = flag. Returns: the stored value or -1. | 8; (var,int)×8 |
| 35 | get_gate_attacked | 1 | nA=1 w32 | int | 0x652660 | R | `get_gate_attacked(item)`: -1 for an invalid item. RE: 0x652660. Args: item = recno. Returns: the flag or -1. | 1; (var)×1 |
| 36 | get_wall_hp | 1 | nA=1 (var) w32 | int | 0x650D50 | R | returns `wall->+0xC6` via the item's virtual accessor (+0x58); 0 for an invalid item | 63; (var)×63 |
| 37 | set_wall_hp | 1 | nA=2 (var,int) | int | 0x650DB0 | R | `set_wall_hp(item, hp)`. RE: 0x650DB0. Args: item = recno; hp = hit points. Returns: 0. | 1; (var,int)×1 |
| 38 | get_wall_max_hp | 1 | nA=1 (var) | int | 0x650E10 | R | returns `wall->+0xCA` via accessor +0x58 | 7; (var)×7 |
| 39 | break_wall | 1 | nA=2 (var,var) | int | 0x650E70 | R | `break_wall(wall, unit)`: with a valid unit and item, posts event 0x40 (sub 2) at the wall's position, now, parameter 1000.0. The builtin itself returns 0 always. RE: 0x650E70. Args: wall = wall item recno; unit = the script's unit; unit_valid = whether the unit is live. Returns: the event or nullopt. | 1; (var,var)×1 |
| 40 | set_wall_invincible | 1 | nA=2 (var,int) w32,w8 | int | 0x650F40 | R | writes u8 `wall->+0xE6`; returns the value or −1 for an invalid item | 2; (var,int)×2 |
| 41 | unit_code_to_id | 1 | nA=1 (str) | int | 0x65B160 | R | RE: 0x65B160. Args: code = unit code. Returns: type id or 0. | 247; (str)×247 |
| 42 | add_unit_ref | 1 | nA=1 (str) | int | 0x65B190 | R | `add_unit_ref(code)`: resolves the unit code in the unit resource table and, when found and not yet present, appends the type id to the scenario's unit-definition reference list (asset preload bookkeeping). Always returns 1. RE: 0x65B190. | 132; (str)×132 |
| 43 | search_unit_by_code | 1 | nA=1 (str) | int | 0x65B200 | R | RE: 0x65B200. Args: code = unit code. Returns: first live unit of the type. | 96; (str)×96 |
| 44 | search_unit_by_name | 1 | nA=1 (str) | int | 0x65B240 | R | `search_unit_by_name(name)`: recno of the first live unit whose instance name equals `name` (ascending scan, case-insensitive), 0 if none. RE: 0x65B240. | 1163; (str)×1163 |
| 45 | get_unit_range | 1 | nA=2 w32,w32 | int | 0x65B280 | R | RE: 0x65B280. Args: unit = unit; which = 1..4. Returns: range. | — |
| 46 | get_unit_hp_percentage | 1 | nA=1 (var) | int | 0x65B500 | R | RE: 0x65B500. Args: unit = unit. Returns: `hp * 100 / max_hp`. | 9; (var)×9 |
| 47 | set_unit_mode | 1 | nA=2 (var,str) w16,ptr | 0 | 0x6515D0 | R | mode string → `unit->+0x167`: `"IDLE"`=3, `"ATTACK"`=1, `"ACTIVE"`=2 (other strings ignored); returns 0 | — |
| 48 | set_unit_attack_behaviour | 1 | nA=2 (var,str) w16,ptr | int | 0x651690 | R | string compared against `"NEAREST"`, `"WEAKEST"`, `"IMPORTANT"`, `"NEAREST_ARCHER"`, `"NEAREST_HIGH_PRIORITY"` (target-selection mode) | — |
| 49 | is_attacked | 1 | nA=1 (var) w16 | bool | 0x6512D0 | R | RE: 0x6512D0. Args: unit = unit. Returns: in a hit reaction. (RE notes: alive and unit type (`+0x215`) ∈ {0x23..0x2A, 0x40..0x42} gives the first test (rest of the body not read)) | 8; (var)×8 |
| 50 | will_be_attacked | 1 | nA=2 w16,w16 | bool | 0x651430 | R | both alive; target type ∈ {0x1C,0x1E,0x1F,0x20,0x21,0x53} and distance (0x505960) strictly between `DAT_009CFC0C` and `DAT_009CFBA8` | — |
| 51 | is_idle_mode | 1 | nA=1 w16 | bool | 0x651550 | R | `unit->+0x167 > 1` (IDLE or ACTIVE) | — |
| 52 | is_attack_mode | 1 | nA=1 w16 | bool | 0x651590 | R | `unit->+0x167 == 1` | — |
| 53 | search_for_unit | 1 | nA=6 w16,w32×5 | int | 0x6517C0 → 0x54B480 | R | RE: 0x6517C0 -> 0x54B480. Args: unit = searcher; type = search type string; range = radius; use_leader = centre on leader; place = place recno; max_xy = planar limit. Returns: found recno or 0. | — |
| 54 | is_enemy_exist_for_unit_team | 1 | nA=2 (var,int) w16,w32 | bool | 0x651830 | N | enemy exists for the unit's team (scans team members, 0x54B480) | 1; (var,int)×1 |
| 55 | is_facing_enemy | 1 | nA=2 | int | 0x6526E0 | N | facing test: angle from `DAT_009CFA10/DAT_009CFBF8` and 0x4FF850 | — |
| 56 | is_facing_knock_down_enemy | 1 | nA=1 | int | 0x652820 | R | RE: 0x652820. Args: unit = unit. Returns: 0/1. | — |
| 57 | is_in_danger | 1 | nA=1 | int | 0x652A40 | R | is_in_danger(unit): for a valid unit the auto potion with force 0, else 0; polling it therefore also makes the unit drink (RE: 0x652A40, 0x4FD980; docs/re/functions/unit.md 9.8). Args: unit = unit. Returns: 1 = below `INDANGER` and no potion found, else 0. | 2; (var)×2 |
| 58 | can_attack_down | 1 | nA=1 | int | 0x652AA0 | R | RE: 0x652AA0. Args: unit = unit. Returns: 0/1. | — |
| 59 | can_rush_attack | 1 | nA=1 | int | 0x652B00 | R | RE: 0x652B00. Args: unit = unit. Returns: 0/1. | — |
| 60 | can_energy_attack | 1 | nA=1 | int | 0x652C10 | R | RE: 0x652C10. Args: unit = unit. Returns: 0/1. | — |
| 61 | get_rotation | 1 | nA=2 | int | 0x652EE0 | R | RE: 0x652EE0. Args: a = unit; b = other. Returns: `ftol(angle)` degrees. | — |
| 62 | get_angle_diff | 1 | nA=2 | int | 0x652F80 | R | RE: 0x652F80. Args: a = unit; b = other. Returns: \|angle\| degrees. | — |
| 63 | get_character_recno | 1 | nA=0 | int | 0x653130 | R | `get_character_recno()`: recno of the player's main character (glue `main_character`, set by `set_main_character`). RE: 0x653130. | — |
| 64 | is_died | 1 | nA=1 | bool | 0x653C60 | R | RE: 0x653C60. Args: unit = unit. Returns: action is DIE. | 1; (var)×1 |
| 65 | is_inside_facility | 1 | nA=2 | bool | 0x653CA0 | R | `is_inside_facility(unit, facility)`: both exist, same scene, unit inside the place region. RE: 0x653CA0. Args: reg = registry; f = facility recno; unit_scene = the unit's scene id; unit_inside_place = result of the place region test for the unit's position. Returns: the verdict. | — |
| 66 | is_unit_inside_facility | 1 | nA=1 (var) | bool | 0x653D60 | N | unit inside any facility | 16; (var)×16 |
| 67 | get_unit_dist | 1 | nA=2 (var,var) w16,w16 | int | 0x653150 | R | RE: 0x653150. Args: a = unit; b = unit. Returns: `ftol(\|a-b\|)`. | 26; (var,var)×26 |
| 68 | set_unit_drop_item_id | 1 | nA=2 (var,var) w16,w16 | int | 0x653210 | R | RE: 0x653210. Args: unit = unit; item = item type. Returns: 1. | 62; (var,var)×59,(var,int)×2,(var)×1 |
| 69 | set_unit_drop_item_level | 1 | nA=2 (var,int) | bool | 0x6532A0 | R | RE: 0x6532A0. Args: unit = unit; level = level. Returns: 1. | 14; (var,int)×14 |
| 70 | set_unit_track | 1 | nA=2 (var,str) w16,ptr | bool | 0x653300 | R | `set_unit_track(unit, name)` (RE: 0x653300 → 0x427B50): needs a linked object; the unit's `track_id` becomes `find_by_name(name)` (0 when unknown). Returns 1 (unit exists). Args: m = the unit's movement fields; has_linked_object = `unit+0x339` and its body; tracks = the stage tracks; name = track code. Returns: true. | 104; (var,str)×104 |
| 71 | unit_track_to_id | 1 | nA=2 (var,str) | int | 0x653360 | R | `unit_track_to_id(unit, name)` (RE: 0x653360): 0 without a linked object or an empty name, else the track index. Args: has_linked_object = `unit+0x339`; tracks = the stage tracks; name = track code. Returns: the id. | 18; (var,str)×18 |
| 72 | is_unit_track_exist | 1 | nA=1 (str) | int | 0x653400 | R | `is_unit_track_exist(name)` (RE: 0x653400): true when the name resolves and any live team/group record holds a flagged unit with a linked object. Args: tracks = the stage tracks; name = track code; any_flagged_unit_with_linked_object = the team-record scan result. Returns: whether the track "exists". | 4; (str)×4 |
| 73 | set_unit_can_process_in_cut_scene | 1 | nA=2 (var,int) w16,w8 | int | 0x653590 | R | RE: 0x653590. Args: unit = unit; flag = value. Returns: 1. | 971; (var,int)×525,(int,int)×446 |
| 74 | set_unit_team_can_process_in_cut_scene | 1 | nA=2 (var,int) w16,w8 | int | 0x653640 | R | RE: 0x653640. Args: unit = unit (and its team); flag = value. Returns: 1. | 7; (var,int)×7 |
| 75 | get_unit_medal_number | 1 | nA=1 | int | 0x65C0D0 | N | medals | — |
| 76 | get_unit_killed_number | 1 | nA=1 | int | 0x65C130 | R | RE: 0x65C130. Args: unit = unit. Returns: kills. | — |
| 77 | get_unit_level | 1 | nA=1 | int | 0x65C1A0 | R | RE: 0x65C1A0. Args: unit = unit. Returns: level. | 1; (var)×1 |
| 78 | set_unit_level | 1 | nA=2 (var,int) | bool | 0x65C250 | R | RE: 0x65C250. Sets the level and the experience floor of that level. Args: unit = unit; level = level. Returns: 1. | 60; (var,int)×60 |
| 79 | set_unit_id | 1 | nA=2 | int | 0x65C2E0 | N | sets unit type id | — |
| 80 | get_unit_arrow_quantity | 1 | nA=1 | int | 0x65C3A0 | R | `get_unit_arrow_quantity(unit)`: quantity of the quiver (ammo) stack in the unit's inventory, 0 when the unit has no inventory or quiver. RE: 0x65C3A0. | 1; (var)×1 |
| 81 | get_unit_gold | 1 | nA=0 | int | 0x65C200 | R | `get_unit_gold()`: the gold global. RE: 0x65C200. Returns: gold. | 2; ()×2 |
| 82 | add_gold | 1 | nA=1 | int | 0x65C220 | R | `add_gold(n)`: `gold += n`, no clamp. RE: 0x65C220. Args: amount = delta (may be negative). Returns: the new gold. | 3; (int)×2,(var)×1 |
| 83 | set_unit_hide_hit_point_bar_flag | 1 | nA=2 (var,int) | int | 0x652DE0 | R | `set_unit_hide_hit_point_bar_flag(unit, on)`: `unit.hide_hp_bar = (on != 0)` for a valid unit. Returns 1. | 6; (var,int)×6 |
| 84 | activate_all_unit | 1 | nA=0 | int | 0x652E60 | R | `activate_all_unit()`: sets `active = true` on every existing unit. Returns 1. | — |
| 85 | add_unit | 1 | nA=2 (str,str) | int | 0x655150 | R | `add_unit(code, name)`. RE: 0x655150. Args: code = unit code; name = instance name (empty = template name). Returns: recno or 0. | 81; (str,str)×81 |
| 86 | load_unit | 1 | nA=2 (str,str) | int | 0x655200 | R | `load_unit(code, name)`. RE: 0x655200 (same creation path). Args: code = unit code; name = instance name. Returns: recno or 0. | 8; (str,str)×8 |
| 87 | del_unit | 1 | nA=1 (var) | int | 0x655420 | R | RE: 0x655420. Args: unit = unit. Returns: 1. | 98; (var)×98 |
| 88 | set_unit_pos | 1 | nA=4 (var,float,float,float) w16,f32×3 | int | 0x6554F0 | R | RE: 0x6554F0. Args: unit = unit; x = x; y = y; z = z. Returns: 1. | 81; (var,float,float,float)×81 |
| 89 | set_unit_dir | 1 | nA=2 (var,int/expr) | bool | 0x655590 | R | RE: 0x655590 (degrees). Args: unit = unit; deg = facing. Returns: 1. | 85; (var,int)×83,(var,expr)×2 |
| 90 | set_unit_degree | 1 | nA=2 (var,int/expr) | bool | 0x655620 | R | RE: 0x655620. Args: unit = unit; deg = facing degrees. Returns: 1. | 35; (var,int)×21,(var,expr)×14 |
| 91 | get_unit_dir | 1 | nA=1 | int | 0x655680 | R | RE: 0x655680. Args: unit = unit. Returns: facing degrees 0..359. | 172; (var)×172 |
| 92 | set_unit_ranking | 1 | nA=2 (var,str) | int | 0x6556E0 | R | RE: 0x6556E0. Args: unit = unit; rank = ranking name. Returns: 1. | 48; (var,str)×48 |
| 93 | set_unit_nation | 1 | nA=2 (var,str) | int | 0x655750 | R | `set_unit_nation(unit, nation)` (RE: 0x655750): unknown name → `unit.nation = 0` with no roster work; else the full `change_unit_nation`. The caller writes `unit.nation` back. Args: unit = the unit's roster view (updated); nation_name = nation code. Returns: 1. | 68; (var,str)×68 |
| 94 | deactivate_unit | 1 | nA=1 | int | 0x655810 | R | `deactivate_unit(unit)`: `active = false` for a valid unit (AI/physics skip it). Returns 1. | — |
| 95 | activate_unit | 1 | nA=1 | int | 0x655870 | R | `activate_unit(unit)`: `active = true` for a valid unit. Returns 1. | — |
| 96 | set_unit_target_unit_recno | 1 | nA=2 | int | 0x6558D0 | R | RE: 0x6558D0. Args: unit = unit; target = target. Returns: 1. | — |
| 97 | set_unit_ride_horse | 1 | nA=2 | int | 0x655930 | R | RE: 0x655930. Args: unit = rider; horse = horse (0 = dismount). Returns: 0 refused, 1 done, 2 invalid. | 1; (var,var)×1 |
| 98 | get_unit_riding_recno | 1 | nA=1 | int | 0x655990 | R | RE: 0x655990. Args: unit = unit. Returns: mount recno. | 1; (var)×1 |
| 99 | get_unit_horse_recno | 1 | nA=1 | int | 0x655B20 | R | `get_unit_horse_recno(unit)`: recno of the mount of the unit (`Unit::mount`), 0 for an invalid unit or none. | — |
| 100 | set_unit_ride_cart | 1 | nA=2 (var,var) | int | 0x6559E0 | N | valid unit: `unit_set_link_a(cart)` (0x51A250, cart/escort link `+0x1F5`), returns the unit recno; invalid: 0 | 6; (var,var)×6 |
| 101 | set_unit_ride_off_cart | 1 | nA=1 | int | 0x655A40 | R | valid unit with a cart link: `+0x1F5 = 0` and the partner's `+0x1F9 = 0`; returns 1 (0 invalid) | 2; (var)×2 |
| 102 | get_interact_unit_recno | 1 | nA=1 | int | 0x655BD0 | R | `get_interact_unit_recno(unit)`: the unit the given unit is interacting with (`Unit::interact_unit`), 0 if invalid. | 11; (var)×11 |
| 103 | set_interact_unit_recno | 1 | nA=2 (var,int) | int | 0x655C20 | R | `set_interact_unit_recno(unit, other)`: stores `other` as the unit's interaction partner. Returns 1. | 67; (var,int)×67 |
| 104 | get_interact_item_recno | 1 | nA=1 | int | 0x655C80 | R | `get_interact_item_recno(unit)`. RE: 0x655C80. Args: unit = the unit. Returns: `interact_item`. | 11; (var)×11 |
| 105 | set_interact_item_recno | 1 | nA=2 | int | 0x655CD0 | R | `set_interact_item_recno(unit, item)`. RE: 0x655CD0. Args: unit = the unit; item = item recno. Returns: 1. | 11; (var,int)×11 |
| 106 | get_can_interact_flag | 1 | nA=1 (var) | int | 0x655DE0 | R | `get_can_interact_flag(unit)`: per-unit script flag from the glue map (`GlueState::can_interact`), 0 when never set. | 40; (var)×40 |
| 107 | set_can_interact_flag | 1 | nA=2 (var,int) w16,w16 | int | 0x655E30 | R | `set_can_interact_flag(unit, value)`: stores the flag value for the unit in the glue map. Returns 1. | 306; (var,int)×306 |
| 108 | set_can_bribe_flag | 1 | nA=2 (var,int) w16,w16 | int | 0x655E90 | R | `set_can_bribe_flag(unit, on)`: `Unit::bribable = (on != 0)` for a valid unit. Returns 1. | 27; (var,int)×27 |
| 109 | get_can_bribe_flag | 1 | nA=1 (var) | int | 0x655F10 | R | `get_can_bribe_flag(unit)`: `Unit::bribable` as 0/1 (0 for an invalid unit). | 1; (var)×1 |
| 110 | set_units_can_bribe_flag | 1 | nA=2 (var,int) w16,w16 | int | 0x655F80 | R | `set_units_can_bribe_flag(unit, on)`: sets `bribable` on the unit's team leader and every unit whose leader is that leader (the whole team). Returns 0 for an invalid unit, else 1. | 70; (var,int)×70 |
| 111 | set_units_in_place_can_bribe_flag | 1 | nA=3 w16,w16,w16 | int | 0x656030 | R | `set_units_in_place_can_bribe_flag(type, place, value)` (RE: 0x656030): for every block whose **head** unit has `unit_type == type` (original quirk), every chain member inside the box with `bribe_flag != 2` is rewritten. Args: type = unit-type id; place = place recno; value = flag value; blocks = unit chains, one span per world block (chain order). Returns: the units to rewrite and the handler result. | — |
| 112 | is_unit_bribed | 1 | nA=1 (var) w16 | int | 0x6561B0 | R | `is_unit_bribed(unit)`: `Unit::bribed` as 0/1 (0 for an invalid unit). | 2; (var)×2 |
| 113 | get_interact_stage | 1 | nA=1 (var) w16 | int | 0x655D30 | R | `get_interact_stage(unit)`: per-unit interaction stage counter from the glue map (`GlueState::interact_stage`), 0 when never set. | 81; (var)×81 |
| 114 | set_interact_stage | 1 | nA=2 (var,int) w16,w16 | int | 0x655D80 | R | `set_interact_stage(unit, stage)`: stores the stage for the unit in the glue map. Returns 1. | 31; (var,int)×31 |
| 115 | set_unit_retreat | 1 | nA=2 (var,int) | int | 0x656210 | R | RE: 0x656210. Args: unit = unit; v = flag. Returns: 1. | 78; (var,int)×78 |
| 116 | get_unit_retreat | 1 | nA=1 (var) | int | 0x656260 | R | RE: 0x656260. Args: unit = unit. Returns: flag. | 63; (var)×63 |
| 117 | set_unit_armor_id | 1 | nA=2 (var,var) w32,w16 | int | 0x6562B0 | R | RE: 0x6562B0. Args: unit = unit; id = armour id. Returns: 1. | 2; (var,var)×2 |
| 118 | get_unit_armor_id | 1 | nA=1 (var) | int | 0x656300 | R | RE: 0x656300. Args: unit = unit. Returns: armour id. | 2; (var)×2 |
| 119 | set_unit_name | 1 | nA=2 (var,str) w32,ptr | int | 0x656350 | R | RE: 0x656350. Args: unit = unit; name = UTF-8 name. Returns: 1. | 48; (var,str)×48 |
| 120 | get_unit_name | **3** | nA=1 (var) | **str** | 0x6563B0 | R | RE: 0x6563B0 (string result). Args: unit = unit. Returns: UTF-8 name. | — |
| 121 | set_unit_dead | 1 | nA=1 (var) | int | 0x656410 | R | RE: 0x656410 (silent removal). Args: unit = unit. Returns: 1. | 10; (var)×10 |
| 122 | reset_unit_hp | 1 | nA=1 | int | 0x6568D0 | R | RE: 0x6568D0. Args: unit = unit. Returns: 1. | — |
| 123 | set_unit_hp | 1 | nA=2 (var,int) | bool | 0x656990 | R | RE: 0x656990. Args: unit = unit; hp = value. Returns: 1. | 1; (var,int)×1 |
| 124 | get_unit_hp | 1 | nA=1 | int | 0x656940 | R | RE: 0x656940. Args: unit = unit. Returns: hp. | — |
| 125 | get_leader_recno | 1 | nA=1 | int | 0x6569F0 | R | RE: 0x6569F0. Args: unit = unit. Returns: leader recno or 0. | — |
| 126 | contain_team_member | 1 | nA=1 (int) | int | 0x656A60 | R | 1 when a unit of the team exists that is not a hired mercenary (`+0x2EB != 2`; hiring is not ported, so any member) | 7; (int)×7 |
| 127 | attach_unit | 1 | nA=4 w16,w16,ptr,w32 | int | 0x657670 | N | attaches a unit to another at a named point | — |
| 128 | get_attaching_unit_recno | 1 | nA=1 | int | 0x656AF0 | N | — | — |
| 129 | detach_unit | 1 | nA=2 w16,w32 | int | 0x657CC0 | N | detaches | — |
| 130 | can_attach | 1 | nA=1 | int | 0x656B50 | N | — | — |
| 131 | is_all_attached | 1 | nA=2 (var,str) w16,ptr | int | 0x656BB0 | R | `is_all_attached(unit, "M"\|"F")`. RE: 0x656BB0. Args: unit = weapon; usage = seat letter. Returns: 0/1. | 2; (var,str)×2 |
| 132 | get_nearest_attach_point | 1 | nA=3 w16,w16,ptr | int | 0x656DC0 | N | nearest attach point index | — |
| 133 | get_nearest_attach_point_x | 1 | nA=2 w16,w32 | f32/int | 0x657110 | N | — | — |
| 134 | get_nearest_attach_point_y | 1 | nA=2 | int | 0x657240 | N | — | — |
| 135 | get_nearest_attach_point_z | 1 | nA=2 | int | 0x657370 | N | — | — |
| 136 | set_control_flag | 1 | nA=2 (var,int) w16,w32 | int | 0x6574A0 | R | `set_control_flag(unit, value)`: stores a per-unit control flag in the glue map (`GlueState::control_flag`). Returns 1. | 264; (var,int)×263,(int,int)×1 |
| 137 | get_control_flag | 1 | nA=1 (var) | uint | 0x6575C0 | R | `get_control_flag(unit)`: reads the per-unit control flag (0 when never set). | 1; (var)×1 |
| 138 | get_follow_unit_distance | 1 | nA=1 | int | 0x6592C0 | N | — | — |
| 139 | get_two_unit_distance | 1 | nA=2 (var,var) | int | 0x659400 | R | RE: 0x659400. Args: a = unit; b = unit. Returns: distance. | 1; (var,var)×1 |
| 140 | set_idle | 1 | nA=1 (var) w16 | bool | 0x657610 | R | RE: 0x657610. Args: unit = unit. Returns: 1. | 492; (var)×492 |
| 141 | set_unit_team | 1 | nA=2 (var,int) w16,w16 | int | 0x657FD0 | R | `set_unit_team(unit, team_id)`: joins the first unit of that team id as leader (or becomes the leader). RE: 0x657FD0. Args: unit = unit; team_id = id. Returns: 1. | 134; (var,int)×134 |
| 142 | get_unit_team_size | 1 | nA=1 (var) | int | 0x658160 | R | RE: 0x658160. Args: unit = unit. Returns: team size (members + leader). | 4; (var)×4 |
| 143 | get_unit_team_leader_by_team_id | 1 | nA=1 (int) | int | 0x6581F0 | R | RE: 0x6581F0. Args: team_id = id. Returns: leader recno or 0. | 320; (int)×320 |
| 144 | set_team_nation | 1 | nA=2 (int,str) w16,w32 | int | 0x6582F0 | R | `set_team_nation(team, nation_name)`: resolves the nation name and runs `change_unit_nation` on every unit whose `team_id == team`. Returns 1. | 20; (int,str)×20 |
| 145 | set_team_nation2 | 1 | nA=2 (int,str) | int | 0x658420 | R | `set_team_nation2(team, nation_name)`: same binding as id 144 (the table row is copied). Returns 1. | 1; (int,str)×1 |
| 146 | recruit_team_member | 1 | nA=3 | int | 0x6584E0 | N | — | — |
| 147 | deactivate_team_member | 1 | nA=1 | int | 0x658650 | N | — | — |
| 148 | activate_team_member | 1 | nA=1 | int | 0x658730 | N | — | — |
| 149 | set_formation_pos | 1 | nA=5 w16,w32,w32,w32,f32 | int | 0x658810 | N | formation slot position | — |
| 150 | set_team_member_pos | 1 | nA=1 | int | 0x658920 | N | — | — |
| 151 | kill_team | 1 | nA=1 (int) | int | 0x658AD0 | N | kills a team | 5; (int)×5 |
| 152 | disable_team_drop_item | 1 | nA=1 (int) | int | 0x658D90 | R | `disable_team_drop_item(team)`: clears `drop_item` of every unit of @p team. RE: 0x658D90. Args: units = every live unit; team = team id. Returns: 1. | 16; (int)×16 |
| 153 | set_team_prisoner | 1 | nA=3 (int,int,int) w32,w32,u16 | int | 0x65DC20 | R | for every unit whose team (`+0x31`)==arg1 and which is not yet a prisoner (0x432B90==0) calls 0x519800(arg2,arg3): stop, IDLE_T when tied (arg3) and the row exists else IDLE, `+0x2E1 = arg2`, `+0x2E2 = arg3`, both hands emptied | 10; (int,int,int)×10 |
| 154 | free_team_prisoner | 1 | nA=1 (int) | 1 | 0x65DCF0 | R | for every unit of the team with 0x432B90!=0 calls 0x5199B0 (release: hand weapons re-created, flags cleared, `unit_stop`) | 11; (int)×11 |
| 155 | set_team_control_flag | 1 | nA=2 (int,int) | int | 0x658E10 | R | `set_team_control_flag(team, value)`: writes the control flag (as id 136) for every unit with `team_id == team`. Returns 1. | 6; (int,int)×6 |
| 156 | set_unit_curr_item | 1 | nA=4 (var,int,str,int) w16,w16,w32,w8 | int | 0x658EA0 | R | `set_unit_curr_item(unit, item, bone, hand)`: `item == 0` unequips the hand; else the bone name must resolve (the actor family's 10-entry table; @p bone_id 0 ⇒ 0) and the world item is attached. RE: 0x658EA0. Args: unit = the unit; item = world item recno (0 = unequip); bone_id = resolved bone id (0 = unknown name); hand = 0 or 1. Returns: 1, or 0 when the bone name did not resolve. | 14; (var,int,str,int)×13,(var,var,str,int)×1 |
| 157 | set_main_character | 1 | nA=2 (var,int) | int | 0x658F70 | R | `set_main_character(unit, player)`: makes a valid unit the player's character (the main-character global and the item state's player unit); if `player != 0` it sets `ai_flag = 0` (player-controlled). Returns 0 for an invalid unit, else 1. RE: 0x658F70 (DAT_00A5B9E8). | 14; (var,int)×14 |
| 158 | get_main_character | 1 | nA=0 | int | 0x659060 | R | `get_main_character()`: recno of the player's main character (same glue value as id 63). | 18; ()×18 |
| 159 | del_unit_team | 1 | nA=1 (int) | int | 0x659080 | R | `del_unit_team(team)`: dissolves every team carrying the id: a leader disbands its team, a member leaves it, and `team_id` is reset to 0 for all of them. Returns 1. RE: 0x659080. | 66; (int)×66 |
| 160 | del_unit_team_member | 1 | nA=1 (int) | int | 0x659210 | R | `del_unit_team_member(unit)`: removes a valid unit that has a leader from its team (`UnitWorld::leave_team`). Returns 1. | 1; (int)×1 |
| 161 | string_to_order_para | 1 | nA=2 w32,w32 | int | 0x6511F0 | R | parses an order string (0x4BBAC0 → three order parameters) and returns parameter 1, 2 or 3 as selected by arg 2 (else 0) | — |
| 162 | get_curr_task | 1 | nA=1 (var) | int | 0x6520D0 | R | RE: 0x6520D0. Args: unit = unit. Returns: task id. | 16; (var)×16 |
| 163 | get_curr_task_para | 1 | nA=2 (var,int) w16,w32 | int | 0x652190 | R | RE: 0x652190. Args: unit = unit; n = 1..3. Returns: parameter. | 3; (var,int)×3 |
| 164 | get_curr_task_state | 1 | nA=1 | int | 0x652280 | R | RE: 0x652280. Args: unit = unit. Returns: task state (always 0). | — |
| 165 | set_curr_task_state | 1 | nA=2 w16,w32 | int | 0x6522F0 | R | `set_curr_task_state(unit, state)`: writes the unit's current task state (`task.state`). Returns 1. | — |
| 166 | stop_curr_task | 1 | nA=1 (var) w16 | int | 0x652370 → 0x528300 | R | RE: 0x652370. Args: unit = unit. Returns: 1. | 668; (var)×668 (+ACTION 21) |
| 167 | stop_all_team_task | 1 | nA=1 | int | 0x6523C0 | R | RE: 0x6523C0. Args: unit = any unit of the team. Returns: 1. | — |
| 168 | search_place_by_name | 1 | nA=1 (str) w32 | int | 0x653EB0 | R | `search_place_by_name(name)` (RE: 0x653EB0 → 0x4C54A0). Args: name = place name (empty = the null-name case → 0). Returns: recno or 0. | 656; (str)×656 |
| 169 | search_for_unit_in_place | 1 | nA=2 (var,str) | int | 0x653EF0 | R | `search_for_unit_in_place(place, mode)` (RE: 0x653EF0 → 0x49C240): first live unit inside in @p units order with: `'P'` the player unit; `'E'` type flags & 0x2443 and hostile to the owner; `'F'` type flags & 0x2443, owner and unit nation live, same nation or owner's status toward the unit's nation `> 3`. Any other mode → 0. Args: place = place recno; mode = `"P"`, `"E"` or `"F"` (first character); units = candidate units in block / chain order; player_unit = the player-controlled unit recno (`DAT_00A5B9E8`). Returns: unit recno or 0. | 241; (var,str)×241 |
| 170 | is_unit_inside_place | 1 | nA=2 (var,var) | int | 0x653F30 | R | `is_unit_inside_place(place, unit)` (RE: 0x653F30): place used, unit slot used, position inside the box; **no alive test**. Args: place = place recno; unit = the unit (nullptr = unused slot). Returns: 0 / 1. | 219; (var,var)×219 |
| 171 | number_of_enemy_in_place | 1 | nA=2 (var,var) w16,w16 | int | 0x653FE0 | R | `number_of_enemy_in_place(place, unit)` (RE: 0x653FE0 → 0x49C470): live units inside with `nation != 0` hostile to the unit's nation, plus the queued barrack troops of the place when the owner exists and is hostile to the unit's nation. Args: place = place recno; unit = the requesting unit (nullptr → 0); units = candidate units; barracks = barrack facilities (any place; filtered by `place`). Returns: the count. | 57; (var,var)×57 |
| 172 | number_of_fightable_enemy_in_place | 1 | nA=2 | int | 0x654020 | R | `number_of_fightable_enemy_in_place(place, unit)` (RE: 0x654020 → 0x49E500): as 171 minus classes 0/8/9 and subtypes 2/3/7. Args: place = place recno; unit = the requesting unit (nullptr → 0); units = candidate units; barracks = barrack facilities (any place; filtered by `place`). Returns: the count. | 18; (var,var)×18 |
| 173 | enemy_exist_in_place | 1 | nA=2 | int | 0x654060 | R | `enemy_exist_in_place(place, unit)` (RE: 0x654060 → 0x4A65D0): boolean form of 171. Args: place = place recno; unit = the requesting unit (nullptr → 0); units = candidate units; barracks = barrack facilities (any place; filtered by `place`). Returns: 0 / 1. | — |
| 174 | fightable_enemy_exist_in_place | 1 | nA=2 | int | 0x6540A0 | R | `fightable_enemy_exist_in_place(place, unit)` (RE: 0x6540A0 → 0x4A8C20): boolean form of 172. Args: place = place recno; unit = the requesting unit (nullptr → 0); units = candidate units; barracks = barrack facilities (any place; filtered by `place`). Returns: 0 / 1. | 12; (var,var)×12 |
| 175 | number_of_people_by_nation_in_place | 1 | nA=2 | int | 0x6540E0 | R | handler 0x6540E0 → 0x4AB020 (table name `number_of_people_by_nation_in_place`, but a **boolean**): any live unit inside (classes 0/8/9, subtypes 2/3/7 excluded) of the named nation, or — when the place owner is that nation — any queued barrack troop. Args: place = place recno; nation_name = nation code; units = candidate units; barracks = barrack facilities. Returns: 0 / 1. | — |
| 176 | people_by_nation_exist_in_place | 1 | nA=2 (var,str) | int | 0x654120 | R | handler 0x654120 → 0x4B79A0 (table name `people_by_nation_exist_in_place`, but a **count**): the same units counted, plus the barrack leader and queued members when the owner is that nation. Args: place = place recno; nation_name = nation code; units = candidate units; barracks = barrack facilities. Returns: the count. | 69; (var,str)×69 |
| 177 | general_by_nation_exist_in_place | 1 | nA=2 (var,str) | int | 0x654160 | R | `general_by_nation_exist_in_place(place, nation)` (RE: 0x654160 → 0x4C2B50): count of live units inside of that nation with the class/subtype exclusions and `rank >= 4`; no barrack addition. Args: place = place recno; nation_name = nation code; units = candidate units. Returns: the count. | 3; (var,str)×3 |
| 178 | get_spawn_unit_count_in_place | 1 | nA=1 (var) | int | 0x6541A0 | R | `get_spawn_unit_count_in_place(place)` (RE: 0x6541A0). Args: place = place recno. Returns: `spawn_count` (0 for an unused place). | 47; (var)×47 |
| 179 | change_all_item_in_place | 1 | nA=3 (var,var,var) | int | 0x654240 | R | `change_all_item_in_place(place, old_type, new_type)` (RE: 0x654240 → 0x4C4440): every item of `old_type` inside the box is listed for replacement; handler returns 1. Args: place = place recno; old_type = item type to replace; new_type = replacement type; items = candidate items. Returns: the replacements (empty for an unused place). | 24; (var,var,var)×24 |
| 180 | get_place_captured | 1 | nA=1 (var) | int | 0x654280 | R | `get_place_captured(place)` (RE: 0x654280 → 0x654530). Args: place = place recno. Returns: `captured` (0 for an unused place). | 285; (var)×285 |
| 181 | set_place_captured | 1 | nA=2 (var,int) | int | 0x6542B0 | R | `set_place_captured(place, flag)` (RE: 0x6542B0): stores the flag; for a French place the new owner is French when `flag == 0` else English, for an English place French when `flag != 0` else English; any other owner stores the flag only. `place.nation` is **not** rewritten. Lists class-8 live units inside with another nation (descending recno) and class 1/4 items inside. Args: place = place recno; flag = capture flag; units = all units (any order; the result is sorted descending); items = all items. Returns: the change (empty lists for an unused place or a non-English/French owner). | 85; (var,int)×85 |
| 182 | set_place_nation_by_code | 1 | nA=2 (var,str) | int | 0x654560 | R | `set_place_nation_by_code(place, nation)` (RE: 0x654560): unknown nation → nothing (ret 1); else `Place::set_nation` (rosters updated) and the same unit/item lists as 181 toward the new nation. Args: place = place recno; nation_name = nation code; units = all units; items = all items. Returns: the change. | 1; (var,str)×1 |
| 183 | get_place_unit_view_angle | 1 | nA=1 | int | 0x6547A0 | R | `get_place_unit_view_angle(place)` (RE: 0x6547A0): the stored radians converted back to degrees and truncated (`__ftol`; the exact scale of the original's return is TODO(re)). Args: place = place recno. Returns: degrees (0 for an unused place). | — |
| 184 | set_place_unit_view_angle | 1 | nA=2 (var,int) | int | 0x6547E0 | R | `set_place_unit_view_angle(place, deg)` (RE: 0x6547E0): stores `deg * pi/180`. Args: place = place recno; degrees = angle. Returns: 1 / 0 for an unused place. | 2; (var,int)×2 |
| 185 | set_parking_dir | 1 | nA=2 (var,int) | int | 0x654830 | R | `set_parking_dir(place, deg)` (RE: 0x654830): every parking heading = `deg * pi/180`. Args: place = place recno; degrees = heading. Returns: 1 / 0 for an unused place. | 4; (var,int)×4 |
| 186 | search_facility_by_name | 1 | nA=1 (str) | int | 0x6548B0 | R | `search_facility_by_name(name)`. RE: 0x6548B0 → 0x46D200. Args: reg = registry; name = instance name. Returns: recno or 0. | 1374; (str)×1374 |
| 187 | facility_barrack_add_leader | 1 | nA=6 (var,var,int,int,str,str) w16,w16,w32,w32,w32,w8 | int | 0x6548F0 | R | `facility_barrack_add_leader(f, type, team, level, name, short)`: ignored while a leader is set; otherwise stores the leader and `place.spawn_count += 1`. RE: 0x467DD0. Args: f = the facility; type = unit-type recno; team = team id; level = unit level; name = full name; short_name = short name. Returns: 1. | 315; (var,var,int,int,str,str)×315 |
| 188 | facility_barrack_add_member | 1 | nA=5 (var,var,int,int,int) w16×3,w32,w32 | int | 0x654960 | R | `facility_barrack_add_member(f, type, team, count, level)`: appends a group when fewer than 4 exist, sets `team`, `place.spawn_count += count`. RE: 0x467EA0. Args: f = the facility; type = unit-type recno; team = team id; count = units per wave; level = unit level. Returns: 1. | 1080; (var,var,int,int,int)×1076,(var,var,var,int,int)×4 |
| 189 | get_unit_facility_distance | 1 | nA=2 (var,var) | int | 0x6549C0 | R | `get_unit_facility_distance(unit, facility)`: `ftol(length(unit.pos - facility.pos))`, `0xFFFF` when the facility is invalid (the caller checks the unit). RE: 0x6549C0. Args: reg = registry; f = facility recno; unit_pos = unit position. Returns: distance or 0xFFFF. | 20; (var,var)×20 |
| 190 | set_facility_follow_unit_recno | 1 | nA=2 (var,var) | int | 0x654AE0 | R | `set_facility_follow_unit_recno(f, unit)`. RE: 0x654AE0. Args: f = the facility; unit = unit recno. Returns: 1. | 40; (var,var)×40 |
| 191 | get_spawning_count | 1 | nA=1 | int | 0x654B30 | R | `get_spawning_count(f)`: Σ `member_count`. RE: 0x654B30. Args: f = the facility. Returns: the sum. | — |
| 192 | reset_spawn_count_in_facility | 1 | nA=1 (var) | int | 0x6541D0 | R | `reset_spawn_count_in_facility(f)`: `place.spawn_count = 0` and the spawn reset. RE: 0x6541D0. Args: f = the facility. Returns: 1. | 9; (var)×9 |
| 193 | set_can_recruit_flag | 1 | nA=2 | int | 0x654BC0 | R | `set_can_recruit_flag(f, on)`. RE: 0x654BC0. Args: f = the facility; on = flag. Returns: 1. | — |
| 194 | set_recruit_level | 1 | nA=2 (var,int) | int | 0x654C20 | R | `set_recruit_level(f, level)`. RE: 0x654C20. Args: f = the facility; level = recruit level. Returns: 1. | 5; (var,int)×5 |
| 195 | set_barrack_captured | 1 | nA=2 (var,int) | int | 0x654C80 | R | `set_barrack_captured(f, flag)`: only `flag == 1` acts — `place.captured = 1`, `place.owner_nation = player nation`, pool refilled, the paired spawn point's configuration cleared, and every English flag item becomes a French flag (`captured == 1`) through `replace_items_in_place`. RE: 0x654C80 + 0x468C10. Args: reg = registry (the paired spawn facility); f = facility recno; flag = the script argument; player_nation = nation of `DAT_00A5B9E8`; inside = region test for the flag swap. Returns: 1. | 14; (var,int)×14 |
| 196 | facility_barrack_add_leader_order | 1 | nA=9 (var,str,int,var,int,int,int,int,int) | int | 0x654D70 | R | `facility_barrack_add_leader_order(f, order, flag, a1, a2, a3, a4, a5)`: stores the resolved order id and its parameters (argument 4 of the script form is ignored by the handler). RE: 0x654D70. Args: f = the facility; order = order id (name resolved by the caller through 0x4BBBC0); flag = `order_flag`; args = the five parameters. Returns: 1. | 2; (var,str,int,var,int,int,int,int,int)×2 |
| 197 | shop_set_sell_item_list | 1 | nA=2 (var,var) | int | 0x654E30 | R | `shop_set_sell_item_list(shop, list)`: fills the shop facility's stock from sell list `list` (`sell.res`), pricing through the item table; 0 for an invalid shop, else 1. | 38; (var,var)×38 |
| 198 | set_can_regen_sell_item | 1 | nA=2 (var,int) | int | 0x654EA0 | R | `set_can_regen_sell_item(f, on)`. RE: 0x654EA0. Args: reg = registry; f = facility recno; on = flag. Returns: true when the facility was valid. | 3; (var,int)×3 |
| 199 | barrack_set_recruit_list | 1 | nA=4 (var,var,var,var) | int | 0x654F10 | R | `barrack_set_recruit_list(f, spawn_facility, list_a, list_b)`. RE: 0x654F10. Args: reg = registry; f = facility recno; spawn_facility = paired spawn point recno; recruits = recruit lists; list_a = list A; list_b = list B. Returns: true when the facility was valid. | 20; (var,var,var,var)×20 |
| 200 | workshop_set_recruit_list | 1 | nA=2 | int | 0x654FA0 | R | `workshop_set_recruit_list(workshop, list)`: fills the workshop's recruit list from recruit list `list`, costing each type by its unit-res gold; 0 for an invalid facility, else 1. | — |
| 201 | set_nation_friendship | 1 | nA=3 (str,str,int) | int | 0x653DF0 | R | `set_nation_friendship(a, b, value)` (RE: 0x653DF0): both names must resolve. Args: a = storing nation code; b = other nation code; value = friendship (directional, unclamped). Returns: 1 / 0. | 26; (str,str,int)×26 |
| 202 | set_relation_status | 1 | nA=3 (str,str,int) w32,w32,w8 | int | 0x655010 | R | `set_relation_status(a, b, status)` (RE: 0x655010 → 0x499770). Args: a = first nation code; b = second nation code; status = 0..4 (the dispatcher passes a `u8`); game_date = current game date, days. Returns: 1 / 0. | 13; (str,str,int)×13 |
| 203 | set_nation_military_strength | 1 | nA=2 (str,int) | int | 0x6550D0 | R | `set_nation_military_strength(nation, value)` (RE: 0x6550D0 → 0x4B5580, an **empty** function): resolves the nation and does nothing else. Args: nation_name = nation code; value = ignored. Returns: 1 when the nation resolves, else 0. | 2; (str,int)×2 |
| 204 | set_game_finish | 1 | nA=1 | int | 0x6594E0 | R | `set_game_finish("WIN"\|"LOSE")`: `end_state = victory / defeat` directly (no delay, no award, no sequence); unknown strings change nothing. RE: joan.exe 0x6594E0. Args: text = the argument. Returns: 1. | — |
| 205 | set_game_start_date | 1 | nA=6 (int×6) | int | 0x659550 | R | Script `set_game_start_date(y, m, d, minute, lo, hi)` (205): rebuilds the calendar, then `day_start_minute = lo`, `day_length = hi`, `minute_of_day = clamp(minute, lo, hi)`. The lighting re-evaluation the handler also triggers belongs to the lights family. RE: joan.exe 0x659550, docs/re/structs/Calendar.md §3. Args: year = start year; month = 1..12; day = 1..31; minute = start minute of day; lo = day start minute (reset value at midnight); hi = day length in minutes. | 9; (int×6)×8,(int×4)×1 |
| 206 | is_game_finish | 1 | nA=0 | bool | 0x6595E0 | R | `is_game_finish()`: `end_state != running`. RE: joan.exe 0x6595E0. Returns: 0/1. | 10; ()×10 |
| 207 | set_game_time_limit | 1 | nA=1 | int | 0x659610 | R | `set_game_time_limit(ms)`: stage `time_limit = time_left = ms`. RE: joan.exe 0x659610. Args: ms = game ms. Returns: 1. | — |
| 208 | get_game_time_limit | 1 | nA=0 | int | 0x6596A0 | R | `get_game_time_limit()`: the remaining `time_left`. RE: joan.exe 0x6596A0. Returns: remaining game ms (-1 = none). | — |
| 209 | calculate_date | 1 | nA=3 | void | 0x6596F0 | R | `calculate_date(y, m, d)`: `calc_serial` (-1 for an invalid date). RE: joan.exe 0x6596F0. Args: year = year; month = month; day = day. Returns: the serial. | — |
| 210 | get_game_date | 1 | nA=0 | int | 0x659720 | R | returns `DAT_00A47BB0` | — |
| 211 | get_game_hour | **2** | nA=0 | **float** | 0x659740 → 0x659760 | R | `float` result `*(float*)(x+0xC) / DAT_009CF9CC` (game-clock field divided by a constant) | — |
| 212 | get_game_minute | 2 | nA=0 | int (via `__ftol`) | 0x659790 | R | `ftol(fmod((double)DAT_00A47BB4, 60.0))`; tag says float but the stored value is an int (see §1.3) | — |
| 213 | get_game_day | 1 | nA=0 | int | 0x6597D0 | R | returns `DAT_00A47BB8` | — |
| 214 | get_game_month | 1 | nA=0 | int | 0x6597F0 | R | returns `DAT_00A47BBC` | — |
| 215 | get_game_year | 1 | nA=0 | int | 0x659810 | R | returns `DAT_00A47BC0` | — |
| 216 | change_scenario_step | 1 | nA=1 (int) | int | 0x659830 | R | `change_scenario_step(n)`: `target_stage = n`. RE: joan.exe 0x659830. Args: f = battle flow; n = stage. Returns: 1. | 1; (int)×1 |
| 217 | set_ride_mode | 1 | nA=1 w8 | int | 0x659860 | R | `set_ride_mode(mode)`: stores the ride mode in the glue state (`GlueState::ride_mode`; read by the control module). Returns 1. | — |
| 218 | set_fog_distance | 1 | nA=1 (int) | int | 0x659890 | R | `set_fog_distance(distance)`: stores the fog distance (renderer-side setting, no simulation effect). Returns 1. | 16; (int)×16 |
| 219 | set_objective_count_time | 1 | nA=1 (int) | int | 0x6598C0 | R | `set_objective_count_time(n)`. RE: joan.exe 0x6598C0. Args: n = value. Returns: 1. | 2; (int)×2 |
| 220 | get_objective_count_time | 1 | nA=0 | int | 0x6598F0 | R | `get_objective_count_time()`. RE: joan.exe 0x6598F0. Returns: value. | — |
| 221 | set_scenario_count_time | 1 | nA=0 | int | 0x659910 | R | `set_scenario_count_time[_2/_3]()`: zeroes stopwatch @p index (0..2). RE: joan.exe 0x659910 / 0x659960 / 0x6599B0. Args: index = 0, 1 or 2. Returns: 1. | 26; ()×26 |
| 222 | get_scenario_count_time | 1 | nA=0 | int | 0x659940 | R | `get_scenario_count_time[_2/_3]()`: stopwatch @p index in game ms. RE: joan.exe 0x659940 / 0x659990 / 0x6599E0. Args: index = 0, 1 or 2. Returns: ms. | 21; ()×21 |
| 223 | set_scenario_count_time_2 | 1 | nA=0 | int | 0x659960 | R | `set_scenario_count_time[_2/_3]()`: zeroes stopwatch @p index (0..2). RE: joan.exe 0x659910 / 0x659960 / 0x6599B0. Args: index = 0, 1 or 2. Returns: 1. | 2; ()×2 |
| 224 | get_scenario_count_time_2 | 1 | nA=0 | int | 0x659990 | R | `get_scenario_count_time[_2/_3]()`: stopwatch @p index in game ms. RE: joan.exe 0x659940 / 0x659990 / 0x6599E0. Args: index = 0, 1 or 2. Returns: ms. | 1; ()×1 |
| 225 | set_scenario_count_time_3 | 1 | nA=0 | int | 0x6599B0 | R | `set_scenario_count_time[_2/_3]()`: zeroes stopwatch @p index (0..2). RE: joan.exe 0x659910 / 0x659960 / 0x6599B0. Args: index = 0, 1 or 2. Returns: 1. | 12; ()×12 |
| 226 | get_scenario_count_time_3 | 1 | nA=0 | int | 0x6599E0 | R | `get_scenario_count_time[_2/_3]()`: stopwatch @p index in game ms. RE: joan.exe 0x659940 / 0x659990 / 0x6599E0. Args: index = 0, 1 or 2. Returns: ms. | 2; ()×2 |
| 227 | set_scenario_rescue_count | 1 | nA=1 (var) | int | 0x659A00 | R | `set_scenario_rescue_count(n)`. RE: joan.exe 0x659A00. Args: n = value. Returns: 1. | 7; (var)×7 |
| 228 | set_scenario_rescue_limit | 1 | nA=1 (int) | int | 0x659A30 | R | `set_scenario_rescue_limit(n)`. RE: joan.exe 0x659A30. Args: n = value. Returns: 1. | 5; (int)×5 |
| 229 | add_reputation | 1 | nA=1 (int) | int | 0x659A60 | R | `add_reputation(n)`: `+= n`. RE: joan.exe 0x659A60. Args: n = delta. Returns: 1. | 13; (int)×13 |
| 230 | set_reputation | 1 | nA=1 (int) | int | 0x659A90 | R | `set_reputation(n)`. RE: joan.exe 0x659A90. Args: n = value. Returns: 1. | 1; (int)×1 |
| 231 | get_reputation | 1 | nA=0 | int | 0x659AC0 | R | `get_reputation()`. RE: joan.exe 0x659AC0. Returns: value. | 2; ()×2 |
| 232 | set_citizen_killed_count | 1 | nA=0 | int | 0x659AE0 | R | `set_citizen_killed_count()`: resets to 0 (no argument). RE: joan.exe 0x659AE0. Returns: 1. | 2; ()×2 |
| 233 | get_citizen_killed_count | 1 | nA=0 | int | 0x659E80 | R | `get_citizen_killed_count()`. RE: joan.exe 0x659E80. Returns: value. | 5; ()×5 |
| 234 | set_fcitizen_killed_count | 1 | nA=0 | int | 0x659EA0 | R | `set_fcitizen_killed_count()`: resets to 0. RE: joan.exe 0x659EA0. Returns: 1. | 1; ()×1 |
| 235 | get_fcitizen_killed_count | 1 | nA=0 | int | 0x659ED0 | R | `get_fcitizen_killed_count()`. RE: joan.exe 0x659ED0. Returns: value. | 2; ()×2 |
| 236 | set_enemy_bribed_count | 1 | nA=0 | int | 0x659B30 | R | `set_enemy_bribed_count()`: resets to 0. RE: joan.exe 0x659B30. Returns: 1. | — |
| 237 | get_enemy_bribed_count | 1 | nA=0 | int | 0x659B10 | R | `get_enemy_bribed_count()`. RE: joan.exe 0x659B10. Returns: value. | 1; ()×1 |
| 238 | get_enemy_bribed_cost | 1 | nA=0 | int | 0x659E40 | R | `get_enemy_bribed_cost()`. RE: joan.exe 0x659E40. Returns: value. | 6; ()×6 |
| 239 | get_gold_crates_get | 1 | nA=0 | int | 0x659B60 | R | `get_gold_crates_get()`. RE: joan.exe 0x659B60. Returns: value. | 4; ()×4 |
| 240 | change_mission_gold_to_real_gold | 1 | nA=0 | int | 0x659B80 | R | `change_mission_gold_to_real_gold()`: adds the scenario's mission gold (`GameState+0x734`) to the real purse (`DAT_00A596C4`) and zeroes the mission gold. Returns 1. | 1; ()×1 |
| 241 | get_food_crates_get | 1 | nA=0 | int | 0x659CA0 | R | `get_food_crates_get()`. RE: joan.exe 0x659CA0. Returns: value. | 1; ()×1 |
| 242 | get_food_picked_up | 1 | nA=0 | int | 0x659CC0 | R | `get_food_picked_up()`. RE: joan.exe 0x659CC0. Returns: value. | 1; ()×1 |
| 243 | set_food_can_be_picked | 1 | nA=1 (int) w8 | int | 0x659CE0 | R | `set_food_can_be_picked(b)`. RE: joan.exe 0x659CE0. Args: b = flag. Returns: 1. | 1; (int)×1 |
| 244 | set_total_food | 1 | nA=1 | int | 0x659D10 | R | `set_total_food(n)`. RE: joan.exe 0x659D10. Args: n = value. Returns: 1. | 10; (var)×9,(int)×1 |
| 245 | get_total_food | 1 | nA=0 | int | 0x659D40 | R | `get_total_food()`. RE: joan.exe 0x659D40. Returns: value. | 10; ()×10 |
| 246 | set_citizen_saved | 1 | nA=0 | int | 0x659D60 | R | `set_citizen_saved()`: resets to 0. RE: joan.exe 0x659D60. Returns: 1. | 2; ()×2 |
| 247 | get_citizen_saved | 1 | nA=0 | int | 0x659D90 | R | `get_citizen_saved()`. RE: joan.exe 0x659D90. Returns: value. | 1; ()×1 |
| 248 | increase_citizen_saved | 1 | nA=0 | int | 0x659DB0 | R | `increase_citizen_saved()`: post-increment, returns the OLD value. RE: joan.exe 0x659DB0. Returns: the value before the increment. | 5; ()×5 |
| 249 | add_scenario_general | 1 | nA=1 w16 | int | 0x659DE0 | N | — | — |
| 250 | remove_scenario_general | 1 | nA=1 w16 | int | 0x659E10 | N | — | — |
| 251 | get_bribed_enemy_kill_count | 1 | nA=0 | int | 0x659E60 | R | `get_bribed_enemy_kill_count()`. RE: joan.exe 0x659E60. Returns: value. | 3; ()×3 |
| 252 | set_house_burnt_count | 1 | nA=0 | int | 0x659EF0 | R | `set_house_burnt_count()`: resets to 0. RE: joan.exe 0x659EF0. Returns: 1. | — |
| 253 | get_house_burnt_count | 1 | nA=0 | int | 0x659F20 | R | `get_house_burnt_count()`. RE: joan.exe 0x659F20. Returns: value. | — |
| 254 | set_challenge_restart_duration | 1 | nA=3 | int | 0x659F40 | R | `set_challenge_restart_duration(attr, idx, ms)`. RE: joan.exe 0x659F40. Args: attr = `"STRENGTH"` / `"DEXTERITY"` / `"LEADERSHIP"` / `"DEFENSE"`; idx = 1..5; ms = countdown. Returns: 1. | — |
| 255 | get_challenge_restart_duration | 1 | nA=2 | int | 0x65A010 | R | `get_challenge_restart_duration(attr, idx)`. RE: joan.exe 0x65A010. Args: attr = row name; idx = 1..5. Returns: remaining ms. | — |
| 256 | set_challenge_flag | 1 | nA=3 | int | 0x65A0E0 | R | `set_challenge_flag(attr, idx, b)`. RE: joan.exe 0x65A0E0. Args: attr = row name; idx = 1..5; b = flag byte. Returns: 1. | — |
| 257 | get_challenge_flag | 1 | nA=2 | int | 0x65A1B0 | R | `get_challenge_flag(attr, idx)`. RE: joan.exe 0x65A1B0. Args: attr = row name; idx = 1..5. Returns: the flag byte (sign-extended). | — |
| 258 | set_challenge_duration | 1 | nA=1 | int | 0x65A2A0 | R | `set_challenge_duration(ms)`. RE: joan.exe 0x65A2A0. Args: ms = countdown. Returns: 1. | — |
| 259 | get_challenge_duration | 1 | nA=0 | int | 0x65A280 | R | `get_challenge_duration()`. RE: joan.exe 0x65A280. Returns: remaining ms. | — |
| 260 | battle_is_in_game | 1 | nA=0 | int | 0x65A2D0 | R | `battle_is_in_game()`: `battle_is_world_ready()`. RE: joan.exe 0x65A2D0. Args: f = battle flow. Returns: 0/1. | 1; ()×1 |
| 261 | switch_controllable_unit | 1 | nA=1 (int) | int | 0x65A2F0 | R | RE: 0x65A2F0. Args: n = hero index (0 = next). Returns: 1 when switched. | 8; (int)×8 |
| 262 | quit_rts_mode | 1 | nA=0 | int | 0x65A320 | R | `quit_rts_mode()`: clears the glue RTS-mode flag (`GlueState::rts_mode = 0`). Returns 1. | 6; ()×6 |
| 263 | add_end_condition | 1 | nA=0 | int | 0x65A350 | R | `add_end_condition()`: the new record's 1-based id (0 when full). RE: joan.exe 0x65A350. Returns: the id. | 164; ()×164 |
| 264 | del_end_condition | 1 | nA=0 | int | 0x65A370 | R | `del_end_condition()`: removes the last record. RE: joan.exe 0x65A370. Returns: the new count. | — |
| 265 | set_end_condition_type | 1 | nA=2 (var,int) | int | 0x65A390 | R | `set_end_condition_type(n, type)`. RE: joan.exe 0x65A390. Args: n = id; type = 0..3. Returns: 1. | 164; (var,int)×164 |
| 266 | set_end_condition_des | 1 | nA=2 (var,str-localised) | int | 0x65A3C0 | R | `set_end_condition_des(n, text)`. RE: joan.exe 0x65A3C0. Args: n = id; text = localised UTF-8. Returns: 1. | 164; (var,str)×164 |
| 267 | set_end_condition_des_2 | 1 | nA=2 (int,str-localised) | int | 0x65A400 | R | `set_end_condition_des_2(n, text)`: with the update event and hook. RE: joan.exe 0x65A400. Args: n = id; text = localised UTF-8. Returns: 1. | 5; (int,str)×5 |
| 268 | set_end_condition_territory_link | 1 | nA=2 (var,str) | int | 0x65A460 | R | `set_end_condition_territory_link(n, name)`. RE: joan.exe 0x65A460. Args: n = id; name = place / territory name. Returns: 1. | 59; (var,str)×54,(int,str)×5 |
| 269 | set_end_condition_finish | 1 | nA=2 (int,str) | int | 0x65A4A0 | R | `set_end_condition_finish(n, "WIN"\|"LOSE"\|"NORMAL")`. RE: joan.exe 0x65A4A0. Args: n = id; text = the status name (unknown = ignored). Returns: 1. | 228; (int,str)×228 |
| 270 | get_end_condition_finish | 1 | nA=1 (int) | int | 0x65A670 | R | `get_end_condition_finish(n)`: 0 / 1 / 2. RE: joan.exe 0x65A670. Args: n = id. Returns: the status. | 172; (int)×172 |
| 271 | set_end_condition_para1 | 1 | nA=2 (var,int) | int | 0x65A6A0 | R | `set_end_condition_para1(n, v)`. RE: joan.exe 0x65A6A0. Args: n = id; v = value. Returns: 1. | 2; (var,int)×2 |
| 272 | set_end_condition_para2 | 1 | nA=2 | int | 0x65A6E0 | R | `set_end_condition_para2(n, v)`. RE: joan.exe 0x65A6E0. Args: n = id; v = value. Returns: 1. | — |
| 273 | set_end_condition_enabled | 1 | nA=2 (int,int) | int | 0x65A720 | R | `set_end_condition_enabled(n, v)`. RE: joan.exe 0x65A720. Args: n = id; v = 0 = disabled. Returns: 1. | 263; (int,int)×258,(var,int)×5 |
| 274 | get_end_condition_enabled | 1 | nA=1 (int) | int | 0x65A7E0 | R | `get_end_condition_enabled(n)`. RE: joan.exe 0x65A7E0. Args: n = id. Returns: the enabled word. | 94; (int)×94 |
| 275 | set_end_condition_gold_award | 1 | nA=2 (var,int) | int | 0x65A810 | R | `set_end_condition_gold_award(n, gold)`. RE: joan.exe 0x65A810. Args: n = id; gold = amount. Returns: 1. | 74; (var,int)×74 |
| 276 | set_end_condition_experience_award | 1 | nA=2 (var,int) | int | 0x65A840 | R | `set_end_condition_experience_award(n, exp)`. RE: joan.exe 0x65A840. Args: n = id; exp = amount. Returns: 1. | 93; (var,int)×93 |
| 277 | set_scenario_bit_flag | 1 | nA=2 (int,int) w32,w8 | int | 0x65A870 | R | `set_scenario_bit_flag(n, v)`: bank 0. RE: joan.exe 0x65A870. Args: n = 1-based bit; v = value. Returns: 1. | 271; (int,int)×244,(var,int)×27 |
| 278 | get_scenario_bit_flag | 1 | nA=1 (int) | int | 0x65A8A0 | R | `get_scenario_bit_flag(n)`. RE: joan.exe 0x65A8A0. Args: n = 1-based bit. Returns: 0/1. | 355; (int)×324,(var)×31 |
| 279 | set_scenario_event_bit_flag | 1 | nA=2 (int,int) w32,w8 | int | 0x65A8D0 | R | `set_scenario_event_bit_flag(n, v)`: bank 1. RE: joan.exe 0x65A8D0. Args: n = 1-based bit; v = value. Returns: 1. | 220; (int,int)×198,(var,int)×22 |
| 280 | get_scenario_event_bit_flag | 1 | nA=1 (int) | int | 0x65A900 | R | `get_scenario_event_bit_flag(n)`. RE: joan.exe 0x65A900. Args: n = 1-based bit. Returns: 0/1. | 256; (int)×218,(var)×38 |
| 281 | reset_scenario_event_bit_flag | 1 | nA=0 | int | 0x65A930 | R | `reset_scenario_event_bit_flag()`: bank 1 = 0. RE: joan.exe 0x65A930. Returns: 1. | 6; ()×6 |
| 282 | set_scenario_event_bit_flag_2 | 1 | nA=2 (int,int) w32,w8 | int | 0x65A960 | R | `set_scenario_event_bit_flag_2(n, v)`: bank 2. RE: joan.exe 0x65A960. Args: n = 1-based bit; v = value. Returns: 1. | 106; (int,int)×106 |
| 283 | get_scenario_event_bit_flag_2 | 1 | nA=1 (int) | int | 0x65A990 | R | `get_scenario_event_bit_flag_2(n)`. RE: joan.exe 0x65A990. Args: n = 1-based bit. Returns: 0/1. | 128; (int)×128 |
| 284 | set_scenario_event_bit_flag_3 | 1 | nA=2 (int,int) w32,w8 | int | 0x65A9C0 | R | `set_scenario_event_bit_flag_3(n, v)`: bank 3. RE: joan.exe 0x65A9C0. Args: n = 1-based bit; v = value. Returns: 1. | 64; (int,int)×64 |
| 285 | get_scenario_event_bit_flag_3 | 1 | nA=1 (int) | int | 0x65A9F0 | R | `get_scenario_event_bit_flag_3(n)`. RE: joan.exe 0x65A9F0. Args: n = 1-based bit. Returns: 0/1. | 64; (int)×64 |
| 286 | set_scenario_stage | 1 | nA=1 (int) | int | 0x65AA20 | R | `set_scenario_stage(v)`: `stage_value`. RE: joan.exe 0x65AA20. Args: v = value. Returns: 1. | 31; (int)×31 |
| 287 | get_scenario_stage | 1 | nA=0 | int | 0x65AA80 | R | `get_scenario_stage()`. RE: joan.exe 0x65AA80. Returns: `stage_value`. | 113; ()×113 |
| 288 | set_scenario_mode | 1 | nA=1 (str) | int | 0x65AAD0 | R | `set_scenario_mode(str)`. RE: joan.exe 0x65AAD0. Args: text = mode name. Returns: 1. | 15; (str)×15 |
| 289 | set_scenario_name | 1 | nA=1 (str-localised) | int | 0x65AB10 | R | `set_scenario_name(text)`. RE: joan.exe 0x65AB10. Args: text = localised UTF-8. Returns: 1. | 3; (str)×3 |
| 290 | set_scenario_description | 1 | nA=1 (str-localised) | int | 0x65AB50 | R | `set_scenario_description(text)` (an optional second argument is ignored). RE: joan.exe 0x65AB50. Args: text = localised UTF-8. Returns: 1. | 3; (str)×2,(str,var)×1 |
| 291 | backup_bit_flag | 1 | nA=0 | int | 0x65AB90 | R | `backup_bit_flag()`. RE: joan.exe 0x65AB90. Returns: 1. | 1; ()×1 |
| 292 | restore_bit_flag | 1 | nA=0 | int | 0x65ACA0 | R | `restore_bit_flag()`. RE: joan.exe 0x65ACA0. Returns: 1. | 1; ()×1 |
| 293 | set_music_side_environment_enable | 1 | nA=1 (int) | int | 0x65ADB0 | R | `set_music_side_environment_enable(on)`: stores the flag in the glue state (`music_side_env`, read by the audio layer). Returns 1. | 2; (int)×2 |
| 294 | set_cut_scene_flag | 1 | nA=1 (int) | int | 0x65ADE0 | R | `set_cut_scene_flag(b)`: 1 → `start` with no camera move / track / wipe; else `end` with no return track / wipe. Returns the method's result. RE: joan.exe 0x65ADE0. Args: now = sim ms; b = the argument. Returns: 0/1. | 454; (int)×454 |
| 295 | get_cut_scene_flag | 1 | nA=0 | int | 0x65B130 | R | `get_cut_scene_flag()`. RE: joan.exe 0x65B130. Returns: 0/1. | 1; ()×1 |
| 296 | start_cut_scene | 1 | nA=6 or 8 (see §1.6) | int | 0x65AEF0 / 0x65AE60 | R | `start_cut_scene(...)` / `start_cut_scene_2(...)` arity overloads: 6 arguments `(a1, a2, unit, yaw, pitch, dist)` or 8 `(a1, a2, x, y, z, yaw, pitch, dist)`; the `_2` forms carry one more trailing argument, the wipe-in mode. Other arities are ignored (return 0). RE: joan.exe 0x65AEF0 / 0x65AE60 (docs/re/functions/battle.md §10). Args: now = sim ms; args = the script arguments; with_wipe = true for `start_cut_scene_2`. Returns: the start result. | 45; (int×8)×44,(int,int,var,int,int,int)×1 |
| 297 | start_cut_scene_2 | 1 | nA=7 or 9 (see §1.6) | int | 0x65AEF0 / 0x65AE60 | R | `start_cut_scene(...)` / `start_cut_scene_2(...)` arity overloads: 6 arguments `(a1, a2, unit, yaw, pitch, dist)` or 8 `(a1, a2, x, y, z, yaw, pitch, dist)`; the `_2` forms carry one more trailing argument, the wipe-in mode. Other arities are ignored (return 0). RE: joan.exe 0x65AEF0 / 0x65AE60 (docs/re/functions/battle.md §10). Args: now = sim ms; args = the script arguments; with_wipe = true for `start_cut_scene_2`. Returns: the start result. | 18; (int×9)×17,(int,int,var,int,int,int,int)×1 |
| 298 | end_cut_scene | 1 | nA=1 (int) w8 | int | 0x65AFF0 | R | `end_cut_scene(a[, b])`. RE: joan.exe 0x65AFF0. Args: now = sim ms; a = return camera track; b = wipe-out mode (0 when absent). Returns: the end result. | 43; (int)×43 |
| 299 | end_cut_scene_2 | 1 | nA=2 (int,int) w8,w8 | int | 0x65AFF0 | R | `end_cut_scene(a[, b])`. RE: joan.exe 0x65AFF0. Args: now = sim ms; a = return camera track; b = wipe-out mode (0 when absent). Returns: the end result. | 94; (int,int)×94 |
| 300 | get_cut_scene_status | 1 | nA=0 | void | 0x65AFD0 | R | `get_cut_scene_status()`: the raw state. RE: joan.exe 0x65AFB0. Returns: 0..8. | — |
| 301 | start_cut_scene_camera | 1 | nA=6 (var,str,int,float,int,int) | int | 0x6537A0 | N | starts a camera track for the cut scene | 2; (var,str,int,float,int,int)×2 |
| 302 | change_camera_in_cut_scene | 1 | nA=5 → 0x65B030, else 7 → 0x65B070 | int | see §1.6 | R | `change_camera_in_cut_scene(...)`: 5-argument form -> 0x65B030 (-> 0x6CD720), 7-argument form -> 0x65B070 (ints converted to float, -> 0x6CCF00): rewrites the cut-scene camera track (docs/re/functions/camera.md, battle.md section on 302). TODO(docs): the WAIT poll of this builtin. | 265; (int×7)×257,(int,var,int,int,int)×8 |
| 303 | is_cut_scene_status_black_screen | 1 | nA=0 | bool | 0x65B0E0 | R | `is_cut_scene_status_black_screen()`. RE: joan.exe 0x65B0E0. Returns: 0/1. | 180; ()×180 |
| 304 | start_buy_interface | 1 | nA=2 (var,var) | int | 0x653890 | R | `start_buy_interface(shop, keeper)`: if the shop facility and the keeper unit are both valid, opens the shop screen for that pair and returns 1; otherwise nothing happens and 0 is returned (the WAIT poll `!shop_is_open` then passes at once). Closing is up to the player. RE: 0x653890 (docs/re/functions/hud.md). | 49; (var,var)×49 |
| 305 | close_inventory_interface | 1 | nA=0 | int | 0x653930 | R | `close_inventory_interface()`: closes the character (inventory) screen if it is open. Returns 1. RE: 0x653930 (hud.md). | — |
| 306 | show_objective | 1 | nA=0 | int | 0x653970 | R | `show_objective()`: starts the HUD objective banner (not the objective menu): inactive -> active at the current time. Returns 1. RE: 0x653970 -> 0x707CE0 (hud.md). | 24; ()×24 |
| 307 | start_scenario_part_interface | 1 | nA=1 w8 | int | 0x6539A0 | R | Request: `start_scenario_part_interface(b)` — loading screen in "interstitial" mode holding `k_scenario_part_hold_ms`, then back to the game. | — |
| 308 | minimap_set_way_point | 1 | nA=6 (int,float,float,float,int,int) | int | 0x6539E0 | R | `minimap_set_way_point(id, x, y, z, on, icon)`: enables/disables way-point slot `id` on the minimap at world position (x, y, z) with icon/number `icon`. Returns 1. RE: 0x6539E0 -> 0x718010(id, on, x, y, z, icon). | 116; (int,float,float,float,int,int)×116 |
| 309 | minimap_set_way_point_follow_unit | 1 | nA=3 (int,var or int,int) | int | 0x653A50 | R | `minimap_set_way_point_follow_unit(id, unit, icon)`: way-point slot `id` follows the unit `unit` on the minimap, icon/number `icon`. Returns 1. RE: 0x653A50 -> 0x7180F0. | 8; (int,var,int)×5,(int,int,int)×3 |
| 310 | get_distance | 1 | nA=4 (var,int,int,int) w16,w32,w32,w32 | int | 0x65B800 | R | `get_distance(unit, x, y, z)`. RE: 0x65B800. Args: unit = unit; x = x; y = y; z = z. Returns: `ftol` distance. | 2; (var,int,int,int)×2 |
| 311 | set_getting_weapon_flag | 1 | nA=2 w16,w32 | int | 0x65B8C0 | N | — | — |
| 312 | is_getting_weapon | 1 | nA=1 w16 | int | 0x65B950 | N | — | — |
| 313 | get_default_weapon | 1 | nA=1 w16 | int | 0x65B9B0 | N | — | — |
| 314 | drop_item_on_floor | 1 | nA=3 w16,w16,w8 | int | 0x65BCE0 | R | `drop_item_on_floor(unit, held_item, hand)` (BUILTINS.md corrections note): turns a *held* item object into a free world object of the same template at the hand's attach transform, then unequips the hand. RE: 0x65BCE0. Args: unit = the unit; held_item = world item recno currently held; hand = 0 or 1; attach_pos = world position of the attach bone (actor family); attach_yaw = facing at the attach bone, radians. Returns: 1, or 0 when the item was invalid. | — |
| 315 | save_game | 1 | nA=2 (str,str) | int | 0x65BED0 | R | `save_game(name, title)` (0x65BED0): queues a save of `name + ".SAV"` with the autosave flag from the options (the VM passes no third argument — docs/scripting/ BUILTINS.md open question 4) and returns 1 like the original. The file is written by the next `flush_pending`. Args: name = file base name from the script; title = display title from the script. Returns: 1. | 1; (str,str)×1 |
| 316 | add_dialog | 1 | DIALOG class; nA=2,3,6,7 (unit, text, [a3..a6], [last]) | int (dialog handle) | 0x65FD70 case 0x13C → 0x63CDC0 | R | creates a dialog line: first `0x63D150` (drops finished lines), then `0x63CDC0(unit, buf, voice_id, 5, tbl+8, tbl+0xE, a3..a6 or −1×4)` → handle stored at `call+0x10`. Text = arg 2 localised (§1.5; table entry fields +8 u32 / +0xE u16 are copied as extra line parameters); voice id = `0x639380(script name up to '.', atoi(arg2+1))`. A trailing numeric argument (arg 3 if nA==3, arg 7 if nA==7) is read as a value; the value −100 is also set when the text consists only of `'.'`/`' '` characters (empty line); with it unequal to −100 the unit is touched via `0x41B3C0` (no visible effect). WAIT poll: §5.3 | 900; (var,str)×352,(var,str,var)×312,(var,str,var,var)×158 |
| 317 | del_dialog | 1 | DIALOG class, nA=1 (dialog handle) | — | 0x63D120 | R | removes the dialog line with that handle. Poll: done | — |
| 318 | del_all_dialog | 1 | DIALOG class, nA=0 | — | 0x63D150 | R | if the dialog manager is active, removes every line for which `0x63E3F0(i)==0`, then `0x585D20`. Poll: done | 380; ()×380 |
| 319 | add_choice_dialog | 1 | DIALOG class, nA=2 + pairs (unit, text, then (choice_text, flag)*) | int (dialog handle) | 0x65FD70 case 0x13F | R | creates the dialog like 316 (no voice args; text via §1.5), then for every pair at args (3,4),(5,6)… copies the localised choice text and calls `0x63D250(dialog, choice_buf, flag != 0, −1×4)`. WAIT poll: waits for a choice (§5.3) | — |
| 320 | get_choice | 1 | DIALOG class, nA=1 (dialog handle) | int | 0x63D770 | R | returns the chosen index stored by the poll: `dialog->+0x353`. Poll: done | — |
| 321 | dialog_text | **3** | DIALOG class, nA=1 (text) | str | 0x65FD70 case 0x141 | R | string result (401-byte buffer, tag set to 3 with `0x660FF0(3)`): arg 1 resolved through the script text table (`[id]` → entry text, ≤400 chars) and copied to the result. Poll: done | — |
| 322 | set_dialog_voice | 1 | DIALOG class, nA=2 (dialog handle, text `"[id]"`) | int (1) | 0x65FD70 case 0x142 | R | voice id = `0x639380(script name up to '.', atoi(arg2+1))`, stored as u16 at `dialog->+0x340`; returns 1. Poll returns 0x101 (done) | 1; (var,str,var)×1 |
| 323 | give_order | 1 | ACTION class, 9 args: (unit, order-name str [parse-time], u8, u8, i32, i32, ×3 evaluated but unused) | int | 0x661340 via 0x661060 | R | `0x661340(unit=arg1, order_id=call+0x22, u8 arg3, u8 arg4, arg5, arg6)`; result → `call+0x10`; WAIT poll `0x6612E0` → `0x661CA0` (§5) | 478; (var,str,int,int,expr,int,int,int,int)×383,(…var…)×62,(…int…)×27 |
| 324 | stop_all_order | 1 | ACTION class, 1 arg (unit) | int | 0x6618F0 via 0x661060 | R | valid unit: if its current order (`unit->+0x2A1`) has a non-zero id calls `0x4BB660(unit)`; frees the order (`0x415220(1)`), sets `unit->+0x2A1 = 0`, `0x422F10()`; returns 1 (0 if the unit is invalid). WAIT poll: always done | 417; (var)×417 |
| 325 | get_curr_order | 1 | nA=1 (var) w16 | int | 0x651260 | R | `unit->+0x2A1` (current order object) `→ +0x1D` (order id), 0 if none/dead | 8; (var)×8 |
| 326 | give_task | 1 | ACTION class, 7 args: (unit, task-name str [parse-time], u8 arg3, i32 arg4, arg5, arg6, arg7 unused) | int | 0x6619D0 via 0x661060 | R | `0x6619D0(unit, task_id=call+0x22, u8, a4, a5, a6)` (valid unit; uses `0x4E4520`, `0x527FB0`); WAIT poll: always done | 1425; (var,str,int,int,int,int,int)×733,(var,str,int,var,int,int,int)×640,(expr,str,int,int,int,int,int)×28 |
| 327 | give_team_task | 1 | ACTION class, same 7-arg layout as 326 | int | 0x661B40 via 0x661060 | R | `0x661B40(unit, task_id, u8, a4, a5, a6)` (team variant of 326); WAIT poll: always done | 109; (var,str,int,var,int,int,int)×60,(var,str,int,int,int,int,int)×48,(expr,str,int,int,int,int,int)×1 |
| 328 | change_animation_special | 1 | nA=2 w16,w32 | bool | 0x65C050 | R | RE: 0x65C050. Args: unit = unit; name = SPECIAL name. Returns: 1. | — |
| 329 | display_title_screen | 1 | nA=4 (str,int,int,int) | int | 0x65C420 | R | `display_title_screen(text, duration, fade_in, fade_out)`: shows a title card with the (localised) text; durations in ms. WAIT completes when the title screen has finished. Returns 1. RE: 0x65C420 -> 0x761680. | 4; (str,int,int,int)×4 |
| 330 | pause_script | 1 | nA=1 (int ms) w32 | 1 | 0x65C4A0 | R | `DAT_00A6A254 = DAT_00A500BA + ms`; returns 1. Only stores the deadline (see §5) | 208; (int)×208 |
| 331 | set_stop_unit_process_flag | 1 | nA=1 (int) w8 | 1 | 0x65C500 | R | `DAT_00A596AB = value`; returns 1 | 6; (int)×6 |
| 332 | add_gate_trigger | 1 | nA=2 (var,int) | bool | 0x65C530 | R | if the item is valid, posts a delayed event: `0x4EEF00(0x20, 2, …, time = DAT_00A500BA + arg2)` (arg 2 is a delay in ms, same clock as `pause_script`) | 95; (var,int)×95 |
| 333 | give_item_to_unit | 1 | nA=4 (str,int,int,var) | int | 0x65C5F0 | R | `give_item_to_unit(code, level, qty, unit)`. RE: 0x65C5F0. The caller checks the unit. Args: unit = the unit; code = item code; level = item level; quantity = units. Returns: result and HUD hooks. | 125; (str,int,int,var)×117,(str,int,expr,var)×8 |
| 334 | give_item_to_unit_troop | 1 | nA=4 (str,int,int,var) | int | 0x65C770 | R | `give_item_to_unit_troop(code, level, qty, unit)`: `force = 1`, message only. RE: 0x65C770. Args: unit = the unit; code = item code; level = item level; quantity = units. Returns: result and HUD hooks (never the armory hint). | 1; (str,int,int,var)×1 |
| 335 | set_unit_training_mode | 1 | nA=2 | bool | 0x65C870 | N | `set_unit_training_mode(unit, on)`: TODO(re): 0x65C870 body not documented in docs/re. | — |
| 336 | is_unit_dead | 1 | nA=1 (var) | int | 0x65C8D0 | R | RE: 0x65C8D0. Args: unit = unit. Returns: invalid or `hp<1 && lives==0`. | 95; (var)×95 |
| 337 | is_unit_dead_by_name | 1 | nA=2 (var,str) | bool | 0x65C930 | R | `is_unit_dead_by_name(unit, name)`: returns 1 when the unit slot is empty, the unit is dead, **or** its instance name differs from `name` (recno-reuse guard); else 0. RE: 0x65C930. | 232; (var,str)×232 |
| 338 | get_scenario_passed_real_time | 1 | nA=0 | int | 0x65C9B0 | R | `get_scenario_passed_real_time()`: milliseconds since the scenario start stamp (`sim_ms - start_stamp`). | — |
| 339 | add_unit_experience | 1 | nA=2 | bool | 0x65C9E0 | R | RE: 0x65C9E0. Args: unit = unit; amount = points. Returns: 1. | 11; (var,int)×6,(var,var)×4,(expr,int)×1 |
| 340 | get_item_class_quantity_in_unit_inventory | 1 | nA=2 | bool | 0x65CA40 | R | `get_item_class_quantity_in_unit_inventory(class_code, unit)`. RE: 0x65CA40. Args: unit = the unit; class_code = `ItemClass` text. Returns: the count (0 for an unknown class or no inventory). | — |
| 341 | get_item_quantity_in_unit_inventory | 1 | nA=2 (var,var) | int | 0x65CAC0 | R | `get_item_quantity_in_unit_inventory(item, unit)`. RE: 0x65CAC0 → 0x510170. Args: unit = the unit; item = template recno. Returns: the count (0 without inventory). | 69; (var,var)×60,(expr,var)×9 |
| 342 | number_of_enemy_in_world | 1 | nA=1 (var) | int | 0x65CB80 | R | `number_of_enemy_in_world(unit)` (RE: 0x65CB80): when `unit.group_id != 0`, the live units of the same group with subtype 0/1, class != 0 and hostile to the unit's nation. Args: unit = the unit (nullptr → 0); units = all units. Returns: the count. | 3; (var)×3 |
| 343 | number_of_friend_in_world | 1 | nA=1 (var) | int | 0x65CD00 | R | `number_of_friend_in_world(unit)` (RE: 0x65CD00): same group, `field_2f == 0`, alive, subtype 0/1, class not 0/8/9, same nation (the unit itself included). Args: unit = the unit (nullptr → 0); units = all units. Returns: the count. | 1; (var)×1 |
| 344 | attention_call_message | 1 | nA=1.. (str-localised[,var[,int]]) | int | 0x65CEA0 | R | `attention_call_message(text [, tutorial])`: `tutorial != 0` -> `TMESS` style. Args: text = UTF-8 text (already localised); tutorial = non-zero = `TMESS` tutorial style. | 377; (str)×285,(str,var)×43,(str,var,int)×13 |
| 345 | attention_call_message_2 | 1 | nA=2..4 (str,var[,var[,var]]) | int | 0x65CF00 (arg 2 type 3) / 0x65CF40 (type 1) | R | `attention_call_message_2(text, unit [, voice])` (unit recno, 0x65CF40): the caller resolves the unit's type name and armour item (empty type = invalid unit). Args: text = localised text; unit_type_name = unit type code name (empty = invalid unit); armour_item = armour item name of the speaker unit (hero tiers); voice = voice id. | 233; (str,var)×89,(str,var,var)×52,(str,var,var,var)×35 |
| 346 | set_attention_call_message_voice | 1 | nA=? | int | 0x65CF80 | R | `set_attention_call_message_voice(rec, voice)`: sets the voice id on attention-call record `rec`; 0 if the slot is empty, else 1. RE: 0x65CF80. | — |
| 347 | switch_unit_model | 1 | nA=2 (var,int) | int | 0x65CFD0 | N | — | 2; (var,int)×2 |
| 348 | enable_unit_physic | 1 | nA=2 | bool | 0x65D070 | R | `enable_unit_physic(unit, on)` (RE: 0x65D070 → 0x5102A0): off → `physics_disabled`, body disabled, position saved; on → flag cleared, body created (mass 50, kind = `unit+0x102`, layer 2) or enabled. Args: m = movement fields; body_id = the unit's body id (updated when created); on = enable flag; params = creation parameters used when the unit has no body yet. | — |
| 349 | enable_unit_bounding_volume | 1 | nA=2 (var,int) | int | 0x65D0D0 | R | `enable_unit_bounding_volume(unit, on)` (RE: 0x65D0D0): off → type none; on → the unit-res volume (type 0 → the model's bounding sphere). Args: v = the actor volume (updated); on = enable flag. (RE notes: for a valid unit with value 0: clears and re-initialises the unit's physics/collision descriptor (`unit->+0x2F5`: state at +0xB8, vectors at +0xBC.., copied from the unit definition `0x42D0A0` fields +0x1C3..); the non-zero branch was not read) | 80; (var,int)×80 |
| 350 | enable_unit_team_bounding_volume | 1 | nA=2 (var,int) | int | 0x65D350 | R | calls 349's handler for the unit and for every member of its team (`unit->+0x33`: count at +2, ids at +3+2i) | 2; (var,int)×2 |
| 351 | mount_siege_tower_to_wall | 1 | nA=3 | int | 0x65D450 | R | drives a per-unit state field (`unit->+0x3B4`) through states 0..3 depending on the third argument | — |
| 352 | is_unit_parked | 1 | nA=1 (var) | int | 0x65D560 | R | true if the unit's slot assignment (`+4`, `+6`) is valid and its distance to the slot < `DAT_009CFC20` and the angle difference < `DAT_009D7270`. TODO(re): 0x65D560 (slot distance/angle test) vs the implementation (`place_recno != 0`) disagree. | 8; (var)×8 |
| 353 | unit_dist_to_track | **2?** | nA=3 w32,u16,w32 | **float** | 0x65D6F0 | R | planar (x,z) distance of the unit to the first (arg 3==0) or last point of a track (`track->+0x4A` array of 0x54-byte points); 0.0 for invalid unit/no track | — |

## 3. ACTION builtins

The ACTION call class (token kind 0x1A, class 0x18, object 0x24 bytes, ctor `0x643240`) has vtable `0x9DB10C` = [dtor `0x643280`, set_parent `0x63E630`, free `0x63E7F0`, exec `0x643B60` (shared with FUNC: evaluate args last→first, then slot 12), parse `0x643D40`, find_by_id `0x643700`, `0x63EE70`, `0x63F140`, kind `0x642470`, self `0x6424D0`, null `0x6424B0`, memory_size `0x642220`, **slot 12 = `0x661060` (action dispatch)**, **slot 13 = `0x6612E0` (WAIT poll)**]. `0x661060` handles exactly four ids (it sets `call+0xC = 1` first): `0x143` give_order → `0x661340`, `0x144` stop_all_order → `0x6618F0`, `0x146` give_task → `0x6619D0`, `0x147` give_team_task → `0x661B40`; other ids do nothing. Names and ids: `give_order` 323, `stop_all_order` 324,
`give_task` 326, `give_team_task` 327 (all four absent from the dispatcher switch) and `stop_curr_task` 166 (present in the dispatcher as FUNC, see §2). Parse-time resolution of the string argument 2 into `call+0x22` is described in §1.1.
Order names in shipped scripts (arg 2 of `give_order`): `"TURN"`×408, `"APPROACH_PLACE"`×61, `"SHOOT"`×6, `"MOVE_TO_TARGET"`×3 — the engine-side order table is read by `0x4BBBC0` (TODO(re)).
Task names (arg 2 of `give_task`/`give_team_task`): `"IDLE"`×645, `"LARGE_WEAPON_PUSH"`×206, `"FOLLOW_UNIT"`×169, `"TOWER_ATTACK"`×160, `"DEFEND_PLACE"`×152, `"APPROACH_PLACE"`×104, `"ATTACK_PLACE"`×79, `"SPECIAL2"`×4, `"RTS_ATTACK_ITEM"`×4, `"SPECIAL5"`×3, `"SCOUT"`×2, `"RETREAT"`×2, `"SPECIAL1"`, `"SPECIAL3"`, `"SPECIAL4"`, `"RTS_ATTACK_UNIT"` ×1 each; task-name table read by `0x4E4610` (TODO(re)).

| name | id | calls | observed signatures | status |
|---|---|---|---|---|
| `give_task` | 326 | 1425 | `(var,str,int,int,int,int,int)`×733; `(var,str,int,var,int,int,int)`×640; `(expr,str,int,int,int,int,int)`×28 | exec 0x661060 → 0x6619D0; poll 0x6612E0: always done |
| `give_order` | 323 | 478 | `(var,str,int,int,expr,int,int,int,int)`×383; `(var,str,int,int,var,int,int,int,int)`×62; `(var,str,int,int,int,int,int,int,int)`×27 | exec 0x661060 → 0x661340; poll 0x6612E0 → 0x661CA0 (waits) |
| `stop_all_order` | 324 | 417 | `(var)`×417 | exec 0x661060 → 0x6618F0; poll: always done |
| `give_team_task` | 327 | 109 | `(var,str,int,var,int,int,int)`×60; `(var,str,int,int,int,int,int)`×48; `(expr,str,int,int,int,int,int)`×1 | exec 0x661060 → 0x661B40; poll: always done |
| `stop_curr_task` | 166 | 21 as ACTION | `(var)`×19; `(str)`×2 | the FUNC form is handled by 0x652370 (§2). The ACTION vtable has **no case for id 166** (0x661060 ignores it): used as `ACTION("stop_curr_task", …)` it parses (the lookup is shared) but executes nothing and its poll returns done — effectively a no-op (2 of the 21 calls even pass a `str`) |

## 3a. DIALOG builtins

DIALOG call class = token kind 0x18 (class 0x16, ctor `0x6430C0`), vtable `0x9DB084`: exec `0x643B60` (shared), **slot 12 = `0x65FD70` (dialog dispatch, 3460 bytes)**, **slot 13 = `0x660E60` (WAIT poll)**. `0x65FD70` handles ids `0x13C`..`0x142` (316..322): add_dialog, del_dialog, del_all_dialog, add_choice_dialog, get_choice, dialog_text, set_dialog_voice; per-id details are in the §2 rows. The dialog manager is the object at `0x63CDC0`/`0x63D1xx`/`0x63D2xx` (`mgr+1` = enabled byte, entries via `0x63E3F0`, count `0x63D210`; entry `+0x334` finished flag, `+0x335` blocking flag, `+0x34F` choice list, `+0x353` chosen index, `+0x340` voice id u16).

**CAMERA call class** (token kind 0x19, class 0x17, ctor `0x643180`, vtable `0x9DB0C8`): slot 12 = `0x661020` (an empty function) and slot 13 = `0x661040` (returns 1): a CAMERA call evaluates its arguments (shared exec) and then does nothing and never waits. No shipped script uses it.

| name | id | calls | observed signatures |
|---|---|---|---|
| `add_dialog` | 316 | 900 | `(var,str)`×352; `(var,str,var)`×312; `(var,str,var,var)`×158 |
| `del_all_dialog` | 318 | 380 | `()`×380 |
| `set_dialog_voice` | 322 | 1 | `(var,str,var)`×1 |

## 4. Builtins 354..406 (+ dead case 407) — names from the corrected table

Names and type tags come from the name table at 0xA1A214 (structs/ScriptBuiltinTable.md). Handler address, argument count (number of `0x63E9F0` fetches) and effect are from the dispatcher and handler bodies; R = body read.

| id (case) | name | handler | nA / widths | ret | what the handler does | census |
|---|---|---|---|---|---|---|
| 354 (0x162) | get_unit_cur_track_recno | 0x65D850 | 1, w32 | **float** (`(float)int`) | R: int `unit->+0x3C9` of a valid unit (0 if invalid) converted to float by the case | — |
| 355 (0x163) | get_team_size | 0x65D8A0 | 1, w32 | int | R: scans a per-nation unit list (`0x47B600(n)`: count i16 at +0x152D, ids at +0x1531) for a valid unit with team id (`+0x31`) == arg and returns that unit's team member count (`team->+2`); 0 if none | 15; (int)×15 |
| 356 (0x164) | display_string | 0x65D9C0 | 3, ptr,w16,w16 | 1 | R: draws an on-screen text via `0x6255D0(x,y,1,text,0xFFFFFFFF,…)` | — |
| 357 (0x165) | display_number | 0x65DA50 | 3, w32,w16,w16 | 1 | R: same, text produced by `0x58FCA0(arg1,4)` (number → string) | — |
| 358 (0x166) | set_prisoner | 0x65DAF0 | 2 or 1 (arity overload) | int | R: valid unit; if already flagged (0x432B90) and type (`+0x215`)==7 returns 1; else `0x519800(arg2 or default, 1)`; returns 1 | 29; (var,int)×26,(var)×3 |
| 359 (0x167) | free_prisoner | 0x65DBA0 | 1, w32 | int | R: valid unit; if flagged (0x432B90) calls `0x5199B0` (release); returns 1 | 4; (var)×4 |
| 360 (0x168) | dist_to_attaching_point | 0x65DDB0 | 1, w32 | **float** | R: unit's linked object (`unit->+0x384`) → transforms the table point `+0x388` index with the object's matrix and returns the planar (x,z) distance; fallback constants `DAT_009CF834` / `DAT_009CF5C4` | — |
| 361 (0x169) | get_attaching_index | 0x65DFA0 | 1, w32 | int | R: returns `unit->+0x388` (0 for invalid) | — |
| 362 (0x16A) | set_item_path_item_owner | 0x65DFF0 | 2, w32,w32 | bool | R: valid object → virtual call `vtbl+0x64(2, arg2)` then `0x491C40(2,arg2)` | — |
| 363 (0x16B) | format_string | 0x65E060 | 2..8 (switch on argc), string result, tag 3 | str | R: `0x65E060(buf, fmt, …)` = printf-style: `0x5A31E0(fmt, va_list)` formats, result copied with `str_copy400` into the 401-byte result buffer; first argument localised (§1.5); the switch passes 0..6 trailing values by argc | 21; (str,expr)×8,(str,var,expr)×3,(str,var,expr,expr,expr)×2 |
| 364 (0x16C) | get_key_string | 0x65E0A0 | 1, string result, tag 3 | str | R: argument = action name (`A_ATTACK`, `B_ATTACK`, `DEFEND`, `DOWN_ATTACK`, `ARROW_SHOOTING`, `INTERACT`, `USE_HEAL_ITEM`, `CHARACTER_INTERFACE`, `COMBO_INTERFACE`, `GAME_OBJECTIVE_KEY`, `RTS_MODE`, `SHIFT`, `SHIFT_CAMERA`, `SHIFT_LEFT/RIGHT`, `SWITCH_CHARACTER/JEAN/JOAN/LA_HIRE`, `TURN_LEFT/RIGHT`, `FORWARD`, `BACKWARD`, `ZOOM_IN/OUT`, …) compared with strcmp; the name selects an entry of the key-binding table of the current control mode (`DAT_00A5B9F8`, 0x30-byte entries via `0x4CB5B0`) and the bound key's display string is copied (400 chars) into the result buffer | — |
| 365 (0x16D) | set_kill_citizen_limit | 0x65E7D0 | 1, w32 | bool | R: `if (n>0) DAT_00A50522 = n; return n>0` | 6; (int)×6 |
| 366 (0x16E) | start_kill_interface | 0x65E810 | 0 or 1 (overload), w8 | 1 | R: `if (DAT_00A50520 == 0) 0x6DFE10(arg)` | 10; ()×7,(int)×3 |
| 367 (0x16F) | end_kill_interface | 0x65E850 | 0 | 1 | R: `0x6DFF10()` | 7; ()×7 |
| 368 (0x170) | set_kill_citizen_limit_2 | 0x65E880 | 1, w32 | bool | R: `if (n>0) DAT_00A50526 = n` | 1; (int)×1 |
| 369 (0x171) | start_kill_interface_2 | 0x65E8C0 | 0 or 1 (overload), w8 | 1 | R: `if (DAT_00A50521 == 0) 0x6E00B0(arg)` | 2; (int)×2 |
| 370 (0x172) | end_kill_interface_2 | 0x65E900 | 0 | 1 | R: `0x6E01D0()` | 1; ()×1 |
| 371 (0x173) | remove_item_from_unit_inventory | 0x65CB20 | 2 or 3 (overload), w32×3 | int | R: handler `(item, unit, count)`: if the unit (param 2) is valid calls `0x5101C0(item, count)` (inventory removal, → `0x498970`), returns 1 else 0; the 2-argument form passes no explicit count (default not visible — TODO(re)) | 43; (var,var)×32,(var,var,int)×11 |
| 372 (0x174) | set_item_can_interact_flag | 0x65E930 | 2, w32,w32 | 1/0 | R: valid item: `item->+0x7E = (arg2 ? 0x200 : 0)` | 12; (var,int)×12 |
| 373 (0x175) | set_unit_can_rided_flag | 0x65E9A0 | 2, w32,w8 | bool | R: valid unit: `unit->+0x3C0 = u8` | — |
| 374 (0x176) | is_item_on_floor | 0x65EA00 | 1, w32 | int | R: returns the item's sub-object pointer `item->+0xA2` (non-zero when the item lies on the floor), 0 if invalid | — |
| 375 (0x177) | get_item_pos_x | 0x65EA50 | 1, w32 | **float** | R: `item->+0xA2 ? *(float*)(…+0x12) : *(float*)(item+0x56)`; 0.0 if invalid | — |
| 376 (0x178) | get_item_pos_y | 0x65EAD0 | 1, w32 | **float** | R: offsets `+0x16` / `+0x5A` | — |
| 377 (0x179) | get_item_pos_z | 0x65EB50 | 1, w32 | **float** | R: offsets `+0x1A` / `+0x5E` | — |
| 378 (0x17A) | set_minimap_icon_display_enable | 0x65EC90 | 2, w32,w32 | int | R: `if (0x44CF30() && 0x44CF10() != 0) return 0x71D150(a,b)` else 0 | 482; (str,int)×482 |
| 379 (0x17B) | get_minimap_icon_display_enable | 0x65ECF0 | 1, w32 | int | R: same guard, `0x71D1E0(a)` | — |
| 380 (0x17C) | get_charmove_mode | 0x65ED50 | 0 | int | R: returns `DAT_00A5B9F8` | 2; ()×2 |
| 381 (0x17D) | is_rts_mode | 0x65ED70 | 0 | bool | R: `DAT_00A5B9F8 == 4` | 7; ()×7 |
| 382 (0x17E) | set_minimap_rotation | 0x65EDA0 | 1, w16 | 1 | R: `DAT_00A4F3AC = value` | 6; (int)×6 |
| 383 (0x17F) | set_minimap_display_gold_enable | 0x65EDD0 | 1, w16 | 1 | R: thiscall setter `this->+0x11 = value` (global minimap object, `this` lost) | 3; (int)×3 |
| 384 (0x180) | set_critical_attack_enabled | 0x65EE30 | 1, w8 | 1 | R: `DAT_00A4F858 = value` | 3; (int)×3 |
| 385 (0x181) | show_scenario_brief | 0x65EE60 | 0 | 1 | R: `DAT_00A52934 = 6; 0x6F5CC0()` (switch UI/game state) | 7; ()×7 |
| 386 (0x182) | turn_camera | 0x65EEE0 | 0 | 1 | R: `0x436190(1); 0x436190(0);` then **runs `0x4384E0()` in a loop while `DAT_00A48CEF==0 && DAT_00A5BA5C!=0`** (blocks the script thread pumping the game until a flag changes) | 5; ()×5 |
| 387 (0x183) | share_team_member | 0x65EF40 | 3, w32,w32,w32 | 1/0 | R: both units valid: moves up to `arg3` members of the second unit's team (`unit->+0x33`: count +2, ids +3+2i) into the first unit's team (`0x438F80(team)`), re-initialising each (0x42FCB0, 0x5213B0, 0x520E80) | 3; (var,var,expr)×3 |
| 388 (0x184) | get_item_angle_diff | 0x65F0F0 | 2, w32,w32 | int | R: valid unit and valid item: `ftol(0x5059B0(item))` (angle difference of the unit to the item) | — |
| 389 (0x185) | move_and_attach_unit | 0x65F160 | 3, w16,w16,ptr(`"M"`/`"F"`) | int | R: assigns the first free slot whose gender mask matches ('M'=1,'F'=2; other letter → −1) in the second unit's definition table to the first unit, moves/attaches it, sets anim state `unit->+0x21D` (5..0xF), returns 1/0/−1 | — |
| 390 (0x186) | is_cut_scene_finished | 0x65F600 | 1 or 0 (overload), w32 | bool | R: `0` if (`0x4301C0()` or `0x42CED0()`) and (arg==0 or `DAT_00A5D3C0==0`); otherwise 1 | 371; ()×368,(int)×3 |
| 391 (0x187) | set_charmove_detect_block_flag | 0x65F670 | 1, w8 | 1 | R: `DAT_00A5BA64 = value` | 8; (int)×8 |
| 392 (0x188) | set_victory_display_flag | 0x65F6A0 | **0 per dispatcher** | 1 | R: if `DAT_00A500D2` clears it and sets `DAT_00A500D0 = ftol(..)`, `0x42F930(0)`; if `!0x4AF410()` then `0x6E8C80()` and clears `DAT_00A59171/75/79`. Name and 0-arg shape disagree (TODO(re)) | — |
| 393 (0x189) | set_minimap_rectangular_vision_area_enable | 0x65F740 | 5 | 1 | R: forwards 5 args to `0x7196E0` | — |
| 394 (0x18A) | set_minimap_circular_vision_area_enable | 0x65F780 | 4 | 1 | R: forwards 4 args to `0x719770` | — |
| 395 (0x18B) | set_minimap_quadrilateral_vision_area_enable | 0x65F7C0 | 9 | 1 | R: forwards 9 args to `0x7197F0` | — |
| 396 (0x18C) | disable_minimap_vision_area | 0x65F820 | 1 | 1 | R: forwards to `0x7198D0` | — |
| 397 (0x18D) | set_territory_nation | 0x65F850 | 2 (str,str) | 1 | R: place by name (`0x4E4B20`) gets owner nation from the nation name (`0x4B91A0`→`0x4B67E0`; the literal `"Neutral"` → −1); writes `territory->+0x38 = nation`, `+0x46 = 0` | 146; (str,str)×146 |
| 398 (0x18E) | show_letter_interface | 0x65F910 | 1, w16 | 1 | R: `interfac::i_letter::fn_006E0B70(id); 0x6E0900()` (opens the letter UI; source file `interfac/i_letter`) | 3; (var)×3 |
| 399 (0x18F) | set_unit_vision_range | 0x65F980 | 2, w32,w32 | bool | R: valid unit: `unit->+0x3C1 = value` | 32; (var,int)×32 |
| 400 (0x190) | set_unit_team_vision_range | 0x65F9E0 | 2, w32,w32 | 1/0 | R: same field for the unit and for all members of its team (units with equal `+0x31`, or the member list `+0x33`) | 39; (var,int)×39 |
| 401 (0x191) | set_unit_hide_in_command_interface | 0x65FB50 | 2, w32,w8 | bool | R: valid unit: `unit->+0x30 = u8` | 14; (var,int)×14 |
| 402 (0x192) | set_delay_before_win | 0x65FBA0 | 1, w32 | 1 | R: `DAT_00A596C8 = value` | 8; (int)×8 |
| 403 (0x193) | is_objective_menu_opened_in_scenario | 0x65FBD0 | 0 | int | R: returns byte `DAT_00A591C7` | — |
| 404 (0x194) | is_attention_call_message_shown | 0x65FBF0 | 1, localised str | int | R: iterates the active attention-call list (`0x4AF300`/`0x4AF2D0`); returns 0 if an entry's name (`entry+0x21`) equals the string, else 1 (**inverted relative to the name** — TODO(re) whether `0x4AF300` marks free slots) | — |
| 405 (0x195) | set_end_cut_scene_type | 0x65FC90 | 1, w8 | 1 | R: `DAT_00A5D386 = value` | 7; (int)×7 |
| 406 (0x196) | show_ending | 0x65FCC0 | 1, w8 | 1 | R: if `0 <= v <= 1` calls `0x6D19E0(v)` | 1; (int)×1 |
| 407 (0x197) | set_prisoner_can_be_attacked_flag | 0x65FD10 | 2, w32,w8 | bool | R: valid unit: `unit->+0x358 = u8` (table row 408 found in the raw data; loop bound `i <= 0x196` reaches it) | 1; (var,int)×1 |

### 4.1 Census names that are not in the table
`set_prisoner_can_be_attacked_flag` resolves to id 407. Only `set_item_not_show_description` (`(var,int)`×2, S8L1_tactic.sce:1814-1815) has **no row** in joan.exe's table. In the original, an unknown name is **fatal**: `0x643C00` → `0x63EB80` formats `"<script>\n line:N Wrong function name (<name>)!"` and `0x586CB0` shows `MessageBoxA(…, MB_ICONERROR)` then exits the process with code −2 (`FUN_007B47FC(0xFFFFFFFE)`). So either S8L1_tactic.sce is never loaded in this build, or this build (Steam) lacks the name that the shipped retail script uses; TODO(docs): which of the two holds. All other census names (§2, §4) resolve.

## 5. WAIT — completion rule (all addresses joan.exe)

`WAIT { <call> … }` is statement class 8 (token kind 0x0E; ctor `0x642A10`, 0x21-byte object, **`+0x20` = "started" byte**). Parse `0x6404E0`: after the keyword, `{`, then any number of function-call statements (token kinds 0x17..0x1A → class 0x14 wrapper whose `+0x14` is the call object) and/or class-0x12 elements (token 0x21: the lone `WAIT { dialog_recno }` form of the census), then `}`; a WAIT with no children completes at once.
**Exec `0x640370`** (return protocol: 0 = suspended, 1 = finished, see `0x649060 script_run_step`):

1. If the WAIT has no children → return 1.
2. If `+0x20 == 0` (first tick): execute **every** child once, in order (`child->slot3`, for a call = `0x643B60` → evaluate args, run the builtin): this *issues* all the commands. Set `+0x20 = 1`.
3. Every tick (including the first): for each child of class 0x14 or 0x12 call `0x644390`, i.e. **`child.inner->vtable[13] (+0x34)(ctx)`** → a `bool`. If any returns 0 → return 0 (script suspended; the cursor stays on the WAIT, it is polled again at the next script step).
4. If all return non-zero: `+0x20 = 0`, return 1 (the script advances).

So the completion rule of `WAIT { FUNC(..) }`, `WAIT { ACTION(..) }`, `WAIT { DIALOG(..) }` is the **virtual method in slot 13 of the respective call class** (FUNC class 0x15 vtable `0x9DB040`, ACTION class 0x18 vtable `0x9DB10C`; the wrapper class 0x14 vtable `0x9DB25C` has only 12 slots and forwards slot 13 through `0x644390`; the DIALOG class 0x16 vtable is still not exported). Round 2 resolved both polls:

### 5.1 FUNC poll `0x6509E0` (slot 13 of vtable 0x9DB040; `call+0x20` = id)
Returns 1 (**done**) unless the id is one of the following, in which case it returns a non-zero test result:

| id | builtin | done when |
|---|---|---|
| 0x12D (301) | start_cut_scene_camera | `0x653A90()`: scans the world's entity array (`DAT_00A45F3E`: count `+0x72`, pointers `+0x7A`) for an entity with `+0x43 != 0`, `+0x60 != 0` and `0x428AA0() != 0`; none found → done; found → done only if the cut-scene flag `0x4301C0()` is clear (and then it runs a camera/entity reset: `0x4289E0`, `0x654AB0(0)`, `0x563C80`, `0x423FE0/0x423EC0` using the main character `DAT_00A5B9E8`), else not done |
| 0x130 (304) | start_buy_interface | `!0x6C9040()` — the shop UI is closed |
| 0x149 (329) | display_title_screen | `0x7616F0()` non-zero (title screen finished) |
| 0x14A (330) | pause_script | `DAT_00A6A254 < DAT_00A500BA` (strictly later than the stored deadline) |
| 0x181 (385) | show_scenario_brief | `!0x4303D0()` |
| 0x128, 0x129 (296, 297) and 0x12E (302) | start_cut_scene / _2 / change_camera_in_cut_scene | `0x65AF50()`: not done iff cut-scene state `s = cutscene->+0xF` is non-zero **and** `s != 2` **and** `DAT_00A5D3C0 == 0` |
| 0x186 (390) | is_cut_scene_finished | `0x65F600(0)` (note: always evaluated with arg 0, i.e. `!(0x4301C0() \|\| 0x42CED0())`) |
| 0x12F (303) | is_cut_scene_status_black_screen | `0x65B0E0()` |
| 0x18E (398) | show_letter_interface | `0x65F950()` = `!0x6E0A90()` (letter UI closed) |
| any other id | | **done immediately** (poll returns 1) |

So `WAIT { FUNC(x) }` for an `x` not listed above never waits beyond the exec tick (e.g. `search_unit_by_name`, `set_idle`). Because the first tick executes the call once and only the poll is evaluated afterwards, `pause_script` stores the deadline exactly once.

### 5.2 ACTION poll `0x6612E0` (slot 13 of vtable 0x9DB10C)
Only `give_order` (id 0x143) waits: `0x661CA0(unit = arg1.payload, order_id = call+0x22)`; the ACTION ids 0x144, 0x146, 0x147 (stop_all_order, give_task, give_team_task) return **done immediately**.
`0x661CA0(unit, order)`: unit invalid → done; unit has no current order (`unit->+0x2A1 == 0`) → done. Otherwise, with `s = cutscene->+0xF` ( `0x65AFB0`): if `s == 0` or (`s ∈ {6,7,8}` and `DAT_00A5D3BF == 0`): when the unit's current order id (`order->+0x1D`) equals the awaited `order`, it calls `0x4BB660(unit)` and returns done; else it scans the global pending-order list (`0x417AD0` entries via `0x484570(i)`, first field = order id) and, on a match, calls `0x4BAEE0(i)` and returns done. In every other case (no match, or other cut-scene state) the result is `0x4BB8D0(order) == 0`: **done iff the order is neither the unit's current order nor in the pending list**.
(Practical reading: during normal play WAIT on a `give_order` ends when the order has finished; inside the permitted cut-scene states it force-completes/removes a matching order instead. The exact effect of `0x4BB660`/`0x4BAEE0` — presumably "finish/dequeue the order" — is TODO(re).)

### 5.3 DIALOG poll `0x660E60` (slot 13 of vtable 0x9DB084) and CAMERA poll
| id | builtin | done when |
|---|---|---|
| 0x13C (316) | add_dialog | **not done** (returns 0) iff all three hold: the dialog manager still has an unfinished line (`0x63D7B0()==0`: manager active, and some line with `0x63E3F0(i)==0` has `+0x334 == 0`), a cut scene is running (`0x4301C0() != 0`) and `DAT_00A5D3C0 == 0`; otherwise done (it also re-reads the trailing numeric argument and touches the unit via `0x41B3C0`, no effect). **Consequence: outside cut scenes `WAIT { DIALOG(add_dialog …) }` completes on the first tick; inside a cut scene it waits until every displayed line has finished.** |
| 0x13F (319) | add_choice_dialog | `0x63D860(handle)`: manager disabled (`mgr+1==0`) → not done; dialog has no choice list (`dialog->+0x34F == 0`) → done; otherwise not done while some line with `+0x335 != 0` (or manager flag `+6`) is unfinished; once all are finished it hit-tests each choice rectangle (`0x5940F0`, screen position from the line's `+0x328/+0x32A`); the first hit stores its 1-based index at `dialog->+0x353` and reports done — so the WAIT ends when the player clicks a choice and `get_choice(handle)` then returns that index |
| 0x142 (322) | set_dialog_voice | returns 0x101 (done) |
| 0x13D, 0x13E, 0x140, 0x141 (317, 318, 320, 321) | | done immediately |
CAMERA class poll `0x661040` always returns 1.
(`0x643D40`'s id tests 0x143 / 0x146..0x147 belong to the ACTION parse.)

What else is proven:
* `pause_script(ms)` merely stores `DAT_00A6A254 = DAT_00A500BA + ms` (global clock `DAT_00A500BA` in ms, also used by cut-scene/effect scheduling and `add_gate_trigger`) and returns 1; the poll (§5.1) is `DAT_00A6A254 < DAT_00A500BA`. `DAT_00A500BA` is the game-time clock in ms (also used by the cut-scene queue and `add_gate_trigger`); whether it advances during pauses/cut scenes is TODO(re).
* The cut-scene builtins only *queue* work in a separate cut-scene object when executed (`start_cut_scene` → `0x6CB9E0/0x6CAD50`, `end_cut_scene` → `0x6CC6A0`, `change_camera_in_cut_scene` → `0x6CCF00/0x6CD720`, each called with `now = DAT_00A500BA` and a ±128.0 constant). `is_cut_scene_finished` (id 390) is false while `0x4301C0()`/`0x42CED0()` report a running cut scene (§4). The poll is a fixed per-id predicate (§5.1), **not** a re-execution of the FUNC.
* ACTION completion: §5.2 (only `give_order` waits). DIALOG completion: §5.3 (`add_dialog` / `add_choice_dialog` wait, the rest return at once).
* Statement exec return codes seen in the block execs `0x648560`/`0x6456F0`/`0x649060`: `0` suspended, `1` finished, `2` propagated "leave" (the cursor is reset with `0x4B9D40`), `-1` error. A WAIT returns only 0 or 1.
* Only children of class 0x14 and 0x12 are polled in step 3 (other child kinds are executed in step 2 only).

## 6. Keyword / census data

> **Note:** the keyword frequencies below were produced by a regex over the raw files and count words inside string literals too. The real keyword set of the language is only
> `INT init exit if else WAIT FUNC ACTION DIALOG SCRIPT`; `for while switch case break continue return and or not main` never appear as code tokens. Code-only counts: `FUNC` 20304, `ACTION` 2450, `DIALOG` 1286, `SCRIPT` 19, `if` 3456, `else` 588, `exit` 38. Conditions use `&&`/`||`. See [LANGUAGE.md](LANGUAGE.md).
> (Disassembly note: token kinds 0x17..0x1A accepted by `0x644190` are FUNC/DIALOG/CAMERA/ACTION; **CAMERA** is a fourth call kind that does not occur in shipped scripts.)

Keyword frequency (raw regex): `INT`×4671, `if`×3538, `WAIT`×1797, `else`×590, `and`×529, `for`×345, `not`×170, `init`×42, `exit`×40, `return`×34, `or`×34, `while`×16, `continue`×9, `break`×7, `switch`×4, `main`×2, `case`×2.

WAIT forms: `WAIT { DIALOG }`×902, `WAIT { FUNC }`×840, `WAIT { ACTION }`×54, `WAIT { dialog_recno }`×1.

Census totals: FUNC 274 distinct names / 19602 calls (all listed in §2 and §4.1; per-name numbers are in the last column of those tables), ACTION 5 distinct / 2450 calls (§3), DIALOG 3 distinct / 1281 calls (§3a), SCRIPT 19 / 19.

SCRIPT includes (`SCRIPT("path")`, 1 call each, all `()`): `script\scenario\S1L1_finish.sce`, `S1L1_tactic.sce`, `S2L0_cut_scene_1.sce`, `S2L1_finish.sce`, `S2L1_tactic.sce`, `S3L1_finish.sce`, `S3L1_tactic.sce`, `S4L1_finish.sce`, `S4L1_tactic.sce`, `S5L1_finish.sce`, `S5L1_tactic.sce`, `S6L1_finish.sce`, `S6L1_tactic.sce`, `S7L1_finish.sce`, `S7L1_tactic.sce`, `S8L1_finish.sce`, `S8L1_tactic.sce`, `S9L1_finish.sce`, `S9L1_tactic.sce`.
The `SCRIPT` statement is parsed by `0x645DE0` (`uti3d/esstate2.cpp`: "Expecting Script!", "Cannot open script"); it is not part of the builtin id space.

## 7. Open questions / TODO(re)

1. (done in round 2) table tail and tags: the name table at 0xA1A214.
2. (done in round 3) DIALOG 0x9DB084 and CAMERA 0x9DB0C8 vtables. Remaining detail: the dialog manager internals (`0x63CDC0` create line, `0x63D250` add choice, `0x63E3F0`), the DIALOG parse step (vtable slot 4) is not read.
3. Effects of `0x4BB660`, `0x4BAEE0`, `0x4BB8D0` (order queue) and of the default `count` when `remove_item_from_unit_inventory` is called with two args; why `set_victory_display_flag` (392) takes no argument.
3a. `set_item_not_show_description` (S8L1_tactic.sce:1814-1815) has no table row: the original would hit the fatal "Wrong function name" box (§4.1). Check whether S8L1_tactic.sce is ever loaded in the Steam build / compare with retail joan.exe's table.
4. What is the implicit extra parameter of `0x65AEF0`/`0x65AE60` (start_cut_scene) and `0x65BED0` (save_game)?
5. `0x63E9F0` returns 0 for argument indices beyond argc; cases that dereference `arg+0x10` unconditionally (e.g. 205 with 4 args) would fault — check under the debugger whether the SEH frame (`FUN_007B29B0` prologue) swallows it.
6. 1 type-tag anomaly: `get_game_minute` (212) is tagged float but stores an int.
7. Truthiness/typing of `INT` vs `float` values at the call boundary (no conversion at call time, §1.3) must be confirmed against the expression evaluator `0x6463F0` (other documenter).

