# ScriptBuiltinTable — the `.sce` builtin name table (joan.exe 0xA1A214)

Reference build joan.exe. Companion pages: [functions/script_builtins.md](../functions/script_builtins.md) (dispatcher + handlers),
[scripting/BUILTINS.md](../../scripting/BUILTINS.md) (per-id reference). Authoritative dump (from the data section):
`$JOAN_WORK_DIR/decomp/script/builtin_table_0xa1a214.tsv` (columns address, id, name, type_tag; 407 rows = ids 0..406; the 408th row, id 407, is described below).
The older `builtin_table_0xa1a210.tsv` was mis-framed (see §1.1) and truncated at 354 rows; use only the new one.

## 1. Entry layout (60 = 0x3C bytes)

```
id        u32   @ 0xA1A214 + 0x3C*k          (row k; row 0 = sentinel id 0, empty name, tag 0)
name      char[52] @ +4
type_tag  u32   @ +0x38
```
The scan `0x661E10 script_builtin_lookup(name, &call+0xC)` starts at the entry named `random_number` (0xA1A250) and reads `name = 0xA1A254+i*0x3C`, `id = 0xA1A250+i*0x3C`, `tag = 0xA1A288+i*0x3C`.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | u32 | id | builtin id; low 16 bits stored at `call+0x20`, switched on by the dispatcher 0x649920 | 0x661E10, 0x643C00 |
| 0x04 | 52 | char[52] | name | lower-case snake_case, compared with `FUN_007C07B0` (strcmp-class) | 0x661E10 |
| 0x38 | 4 | u32 | type_tag | **result value type**: 1 = int, 2 = float, 3 = string (0 for the sentinel). Copied to `call+0x0C` by the lookup | 0x661E10; dispatcher result stores |

`sizeof(entry) = 0x3C`; **408 rows = sentinel + ids 1..407** (the row after `show_ending` 406 is `set_prisoner_can_be_attacked_flag` 407, tag 1, at 0xA2013C+0x3C, found in the raw data; an earlier export of the table stopped one row early). Rows are scanned **in table order**, first strcmp match wins, no hash; 39 rows are stored out of id order, the `id` field is authoritative.
**Loop bound re-checked** (0x661E10: `local_8 = 0; while (true) { if (0x196 < local_8) return 0; … }`): the test is `i <= 0x196`, i.e. **407 iterations, index 0 = the `random_number` row (id 1)**, so the last examined row is the 407th after the sentinel = id 407. Id 407 **is reachable**; any row after it (none known) would not be. The dispatcher's `case 0x197` is therefore live.
Unknown name → returns 0 → `"Wrong function name (%s)!"` via 0x63EB80, which is **fatal**: it builds `"<script name>\n line:N <msg>"`, calls 0x586CB0 → `OutputDebugStringA`, `MessageBoxA(hwnd, text, "Wars & Warriors: Joan of Arc", MB_ICONERROR)`, then `FUN_007B47FC(0xFFFFFFFE)` (process exit with code −2). Note 0x586CB0 formats into a 100-byte stack buffer (long messages overflow). 0x643C00 does not return after the error call, but the exit makes it moot.
Tag census: 394 rows tag 1 (int), 8 rows tag 2 (float: `get_game_hour` 211, `get_game_minute` 212, `unit_dist_to_track` 353, `get_unit_cur_track_recno` 354, `dist_to_attaching_point` 360, `get_item_pos_x/y/z` 375..377), 4 rows tag 3 (string: `get_unit_name` 120, `dialog_text` 321, `format_string` 363, `get_key_string` 364), 1 row tag 0.

### 1.1 Correction of the first dump
The 0xA1A210 dump parsed `{u32 family, u32 id, char name[52]}`, 4 bytes early; its "family" column was the tag of the **previous** row. The "family codes 0/1/2/3" are therefore **not** FUNC/ACTION/DIALOG selectors but value types (§1). Which call kind (FUNC/DIALOG/CAMERA/ACTION) a name may be used with is not in the table: the same table serves all kinds (0x643C00 does not filter; `stop_curr_task` is used as FUNC and as ACTION in shipped scripts).
`get_game_minute` is tagged float although the dispatcher stores an `__ftol` int (case 0xD4) — original bug; not used by shipped scripts.

## 2. Id ranges
| ids | content | dispatcher 0x649920 (FUNC class vtable 0x9DB040 slot 12) |
|---|---|---|
| 0 | sentinel | no |
| 1..315, 325, 328..407 | FUNC-callable builtins | yes (cases 0x01..0x197) |
| 316..322 | DIALOG names: `add_dialog`, `del_dialog`, `del_all_dialog`, `add_choice_dialog`, `get_choice`, `dialog_text`, `set_dialog_voice` | **no**; DIALOG call class (token 0x18, class 0x16, vtable 0x9DB084): slot 12 = `0x65FD70`, slot 13 = `0x660E60` |
| 323, 324, 326, 327 | ACTION names `give_order`, `stop_all_order`, `give_task`, `give_team_task` | **no**; ACTION class vtable 0x9DB10C slot 12 = `0x661060` (see functions page §1a) |
| 166 | `stop_curr_task` | FUNC case 0xA6; the ACTION class has no case for it (0x661060 handles only 0x143, 0x144, 0x146, 0x147) |
| 407 | `set_prisoner_can_be_attacked_flag` (tag 1) | yes (case 0x197, sets u8 `unit+0x358`) |
| (CAMERA) | no builtin of its own; CAMERA call class 0x17, vtable 0x9DB0C8: exec `0x661020` (empty), poll `0x661040` (returns 1) — a stub that never occurs in shipped scripts | n/a |
