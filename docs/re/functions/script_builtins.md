# script_builtins — the `.sce` builtin dispatcher and its handlers (joan.exe)

Reference build joan.exe. Pseudocode: `$JOAN_WORK_DIR/decomp/script/script_vm/` (dispatcher + depth-1/2 callees). The per-id reference with argument
lists, return values and census is [scripting/BUILTINS.md](../../scripting/BUILTINS.md); the name table layout is [structs/ScriptBuiltinTable.md](../structs/ScriptBuiltinTable.md).
Neighbouring documentation (other owners): Script object / statement classes in [video.md §8](video.md), `ScriptSlot`/dlgres in [EsScriptSlot.md](../structs/EsScriptSlot.md).
Source attribution: the module belongs to `uti3d/esstate2.cpp` (the `.sce` engine; `0x645DE0` carries the file string); the dispatcher and handlers themselves have no source-file string.

## 1. Runtime core (call object, dispatcher, name lookup)

| address | proposed name | purpose | evidence | confidence |
|---|---|---|---|---|
| 0x649920 | `script_call_dispatch_func` | the 21 KB `switch (call->+0x20 /*u16 id*/)`; 396 case labels (ids 1..407 except 0x13C..0x144, 0x146, 0x147); each case fetches arguments, calls one handler, stores the 32-bit result at `call->+0x10`; SEH frame; first call `FUN_007B29B0` (probably the CRT stack probe for the large local frame); vtable slot 12 (+0x30) of the FUNC call class | called from `0x643B60` (`vtbl+0x30`); case bodies | verified |
| 0x643B60 | `script_call_exec` | vtable slot 3 of the call classes: `for i = argc..1: arg[i]->vtbl[3] (ctx)`, then `this->vtbl[12] (ctx)` (= the dispatcher); returns 1 | read | verified |
| 0x643C00 | `script_call_parse_name` | parses the builtin name: token type 3 else `"Expecting a function name!"`; `call+0xC = 1`; `call+0x20 = script_builtin_lookup(name, &call+0xC)`; `"Wrong function name (%s)!"` if 0; parses `,`-separated argument expressions (`0x63EA20(0x10)` creates an expression object, `->vtbl[4] (ctx)` parses it) | read | verified |
| 0x643D40 | `script_action_parse` | ACTION-class parse: runs 0x643C00, then for id 0x143 (`give_order`) executes arg 2 once (`vtbl[3] (0)`) and stores `0x4BBBC0(arg2.payload)` (order-name → id) at `call+0x22`; for ids 0x146/0x147 (`give_task`, `give_team_task`) stores `0x4E4610(arg2.payload)` (task-name → id) | read | verified |
| 0x644190 | `script_call_stmt_parse` | statement class 0x14 (wrapper) parse: token kinds 0x17..0x1A, `"Expecting ( after a function."`, `"Expecting ) at the end of a function."`, `"Expecting FUNC/DIALOG/CAMERA/ACTION!"`; creates call class 0x15/0x16/0x17/0x18 for kind 0x17/0x18/0x19/0x1A via `0x640DB0` into `wrapper+0x14` | read | verified |
| 0x644390 | `script_call_wait_poll` | `wrapper->+0x14 ->vtbl[13] (+0x34)(ctx)` — the **WAIT predicate** of the inner call object | read | verified |
| 0x644330 / 0x644360 | `script_call_stmt_slot7` / `…_kind` | `wrapper.inner->vtbl[7]()` (value pointer) / returns `wrapper.inner->+0xC` (value type tag) | read | verified |
| 0x640370 | `script_wait_exec` | WAIT statement exec (class 8, `+0x20` started byte): first tick executes all children, then polls `0x644390` for children of class 0x14/0x12 until all non-zero; see BUILTINS.md §5 | read | verified |
| 0x6404E0 | `script_wait_parse` | `Wait {` … `}` parse: accepts class-0x14 calls (token kinds 0x17..0x1A) and class-0x12 elements (token 0x21); `"Expecting Wait!"`, `"Expecting '{' after Wait."`, `"Expecting '}' after Wait."` | read | verified |
| 0x661E10 | `script_builtin_lookup(name, u32* out_type)` | linear strcmp (`FUN_007C07B0`) over the 407-entry table `{id,name[52],type}` @0xA1A250; returns the u16 id, writes the type tag; 0 when unknown | read | verified |
| 0x63E9F0 | `script_call_arg(i)` | 1-based argument fetch: `0x41B3F0(arr, i)` → element pointer, 0 when `i<1 || i>argc` | read | verified |
| 0x63ECD0 | `script_call_argc` | `args ? args->count (+0xC) : 0` | read | verified |
| 0x63EA20 | `script_call_new_arg(classid)` | lazily creates the argument array (`dynarr(4,1,0)`), creates an expression object `0x640DB0(classid)` and appends it | read | verified |
| 0x6509A0 / 0x6509C0 | `script_name_ptr` / `script_dlgtable` | `script+0x20` (script name) / `script+0x64` (dlgres text table) | read | verified |
| 0x6380B0 | `dlgres_find_index(tag)` | `tag[0]=='['`: scan entries 1..count (`i16` at obj+1) comparing the entry's u16 id with `atoi(tag+1)` (`FUN_007B15CB`); returns the 1-based index or 0 | read | verified |
| 0x638150 | `dlgres_entry(i)` | `entries + (i-1)*16`; text pointer at +4 | read | verified |
| 0x46F020 | `min_int(a,b)` | `a<b ? a : b` (used to clamp the copied length) | read | verified |
| 0x5A1440 | `str_copy400(dst, src)` | `strncpy(dst, src, 400); dst[400] = 0` | read | verified |
| 0x5A1170 | `str_init_empty(buf)` | `buf[0] = 0` | read | verified |
| 0x417D90 | `identity` | returns its fastcall argument (the decompiler uses it to pass a stack buffer address) | read | verified |
| 0x639380 | `dlgres_voice_id(table, a, b)` | `r = 0x6391C0(a,b,0x4001, short(table+4)+0x4000)`; if `(short)r < 1` returns `r & 0xFFFF0000` | read | read |
| 0x649060 | `script_run_step` | (documented in video.md §8) runs the current statement; result codes 0 suspend / 1 done / 2 / −1 | read | verified |
| 0x648560 | `script_block_exec` | iterates a statement list `0x585F20` calling `vtbl[3]`; propagates 0, −1, 2 | read | verified |

## 2. Helper accessors seen inside the handlers (names are proposals, semantics inferred from use)

| address | proposed name | use pattern | confidence |
|---|---|---|---|
| 0x428B70 | `unit_is_invalid(recno)` | `if (0x428B70(r)==0) { u = 0x41B3C0(r); … }` guard in every unit handler | inferred |
| 0x41B3C0 | `unit_get(recno)` | returns the unit object (fields used: +0x04/+0x06 slot words, +0x30 u8, +0x31 team id i16, +0x33 team ptr, +0x3B def id, +0x167 mode, +0x215 type, +0x2A1 order, +0x2F5 physics, +0x33x…) | inferred |
| 0x425D50 / 0x425D20 / 0x425CE0 | `item_is_invalid` / `item_get` / `item_def_get` | same pattern for items/gates/walls/boxes (`item_get` object has virtual accessors +0x4C gate part, +0x58 wall part) | inferred |
| 0x4A36C0 | `item_name_to_recno(str)` | argument of `item_code_to_id`, `add_item_ref`, `add_item` | inferred |
| 0x4B91A0 / 0x4B67E0 | `nation_name_lookup` / `nation_id` | nation string → nation id (`"Neutral"` → −1 in 0x65F850) | inferred |
| 0x4BBBC0 / 0x4BBAC0 | `order_name_to_id` / `order_parse_params` | ACTION `give_order` parse step; `string_to_order_para` | inferred |
| 0x4E4610 | `task_name_to_id` | ACTION `give_task` parse step | inferred |
| 0x41B3F0 | `dynarr_get(i)` | 1-based element fetch (`base + (i-1)*stride`, 0 outside) | verified |
| 0x417AD0 | `dynarr_count` | returns `this+0xC` | verified |
| 0x4EEF00 | `post_timed_event(kind, …, time)` | used by `break_wall` (0x40) and `add_gate_trigger` (0x20); time = `DAT_00A500BA + ms` | inferred |
| DAT_00A500BA | `g_game_time_ms` | clock added to the `pause_script`/`add_gate_trigger`/cut-scene durations | inferred |
| DAT_00A6A254 | `g_script_pause_until` | written only by `pause_script` (0x65C4A0) | read |
| DAT_00A47BB0..BC0 | `g_game_date` fields | date/day/month/year (ids 210, 213..215), `DAT_00A47BB4` minutes source for 212 | read |

## 3. Handler functions, by builtin id (dispatcher `case` → handler)

Proposed names `script_fn_<builtin>`; ids ≥354 use the names of the corrected table (`builtin_table_0xa1a214.tsv`). Handlers are plain functions (`stdcall`/`cdecl` with the decoded parameter list); the dispatcher passes
recno arguments at the width in the "w" column of BUILTINS.md. Results are stored raw. "R" = body read, otherwise the effect is name-derived.

| address | proposed name | id | note |
|---|---|---|---|
| 0x651010 | script_fn_random_number | 1 | → 0x590540 |
| 0x651100 | script_fn_set_snow_level | 2 | → 0x652A10 |
| 0x651130 | script_fn_start_weather_effect | 3 | R: copies `DAT_00A47BB4` to `DAT_00A4839C`/`DAT_00A483AC` |
| 0x65EBD0 | script_fn_call_gameover_msg | 4 | localised str |
| 0x6519D0 | script_fn_search_for_item | 5 | → 0x503680 |
| 0x653000 | script_fn_get_item_rotation | 6 | |
| 0x653060 | script_fn_get_item_dist | 7 | |
| 0x653110 | script_fn_get_read_letter_once | 8 | |
| 0x651A30 | script_fn_item_code_to_id | 9 | → 0x4A36C0 |
| 0x651A60 | script_fn_add_item_ref | 10 | R |
| 0x651AD0 | script_fn_add_item | 11 | R |
| 0x651E70 | script_fn_del_item | 12 | → 0x49F370 |
| 0x651EA0 | script_fn_del_item_by_name | 13 | R (loop with 0x65B570) |
| 0x651F00 | script_fn_set_item_pos | 14 | R |
| 0x651FA0 | script_fn_set_item_dir | 15 | R (deg→rad `DAT_009CF884`) |
| 0x652010 | script_fn_set_item_nation | 16 | R |
| 0x651C90 | script_fn_add_item_box | 17 | R |
| 0x65B570 | script_fn_search_item_by_name | 18 | |
| 0x65B5B0 | script_fn_search_item_by_code | 19 | |
| 0x65B5F0 | script_fn_search_item_sell_list_by_code | 20 | |
| 0x65B630 | script_fn_is_item_deleted_by_name | 21 | |
| 0x65B6A0 | script_fn_is_item_box | 22 | |
| 0x65B700 | script_fn_is_item_gate | 23 | |
| 0x65B760 | script_fn_is_item_wall | 24 | |
| 0x65B7C0 | script_fn_search_recruit_list_by_code | 25 | |
| 0x650B90 | script_fn_get_gate_hp | 26 | R |
| 0x650C10 | script_fn_get_gate_max_hp | 27 | R |
| 0x650C90 | script_fn_set_gate_hp | 28 | |
| 0x650CF0 | script_fn_set_gate_max_hp | 29 | R |
| 0x652440 | script_fn_gate_open | 30 | |
| 0x6524A0 | script_fn_get_gate_state | 31 | |
| 0x652520 | script_fn_set_gate_need_key | 32 | |
| 0x652590 | script_fn_get_gate_need_key | 33 | |
| 0x6525F0 | script_fn_set_gate_invincible | 34 | |
| 0x652660 | script_fn_get_gate_attacked | 35 | |
| 0x650D50 | script_fn_get_wall_hp | 36 | R |
| 0x650DB0 | script_fn_set_wall_hp | 37 | |
| 0x650E10 | script_fn_get_wall_max_hp | 38 | R |
| 0x650E70 | script_fn_break_wall | 39 | |
| 0x650F40 | script_fn_set_wall_invincible | 40 | R |
| 0x65B160 | script_fn_unit_code_to_id | 41 | |
| 0x65B190 | script_fn_add_unit_ref | 42 | |
| 0x65B200 | script_fn_search_unit_by_code | 43 | |
| 0x65B240 | script_fn_search_unit_by_name | 44 | |
| 0x65B280 | script_fn_get_unit_range | 45 | |
| 0x65B500 | script_fn_get_unit_hp_percentage | 46 | |
| 0x6515D0 | script_fn_set_unit_mode | 47 | R ("IDLE"=3, "ATTACK"=1, "ACTIVE"=2 at unit+0x167) |
| 0x651690 | script_fn_set_unit_attack_behaviour | 48 | R (5 strings) |
| 0x6512D0 | script_fn_is_attacked | 49 | R (partial) |
| 0x651430 | script_fn_will_be_attacked | 50 | R |
| 0x651550 | script_fn_is_idle_mode | 51 | R |
| 0x651590 | script_fn_is_attack_mode | 52 | R |
| 0x6517C0 | script_fn_search_for_unit | 53 | → 0x54B480 |
| 0x651830 | script_fn_is_enemy_exist_for_unit_team | 54 | |
| 0x6526E0 | script_fn_is_facing_enemy | 55 | |
| 0x652820 | script_fn_is_facing_knock_down_enemy | 56 | |
| 0x652A40 | script_fn_is_in_danger | 57 | |
| 0x652AA0 | script_fn_can_attack_down | 58 | |
| 0x652B00 | script_fn_can_rush_attack | 59 | |
| 0x652C10 | script_fn_can_energy_attack | 60 | |
| 0x652EE0 | script_fn_get_rotation | 61 | |
| 0x652F80 | script_fn_get_angle_diff | 62 | |
| 0x653130 | script_fn_get_character_recno | 63 | |
| 0x653C60 | script_fn_is_died | 64 | |
| 0x653CA0 | script_fn_is_inside_facility | 65 | |
| 0x653D60 | script_fn_is_unit_inside_facility | 66 | |
| 0x653150 | script_fn_get_unit_dist | 67 | |
| 0x653210 | script_fn_set_unit_drop_item_id | 68 | |
| 0x6532A0 | script_fn_set_unit_drop_item_level | 69 | |
| 0x653300 | script_fn_set_unit_track | 70 | |
| 0x653360 | script_fn_unit_track_to_id | 71 | |
| 0x653400 | script_fn_is_unit_track_exist | 72 | |
| 0x653590 | script_fn_set_unit_can_process_in_cut_scene | 73 | |
| 0x653640 | script_fn_set_unit_team_can_process_in_cut_scene | 74 | |
| 0x65C0D0 | script_fn_get_unit_medal_number | 75 | |
| 0x65C130 | script_fn_get_unit_killed_number | 76 | |
| 0x65C1A0 | script_fn_get_unit_level | 77 | |
| 0x65C250 | script_fn_set_unit_level | 78 | |
| 0x65C2E0 | script_fn_set_unit_id | 79 | |
| 0x65C3A0 | script_fn_get_unit_arrow_quantity | 80 | |
| 0x65C200 | script_fn_get_unit_gold | 81 | |
| 0x65C220 | script_fn_add_gold | 82 | |
| 0x652DE0 | script_fn_set_unit_hide_hit_point_bar_flag | 83 | |
| 0x652E60 | script_fn_activate_all_unit | 84 | |
| 0x655150 | script_fn_add_unit | 85 | |
| 0x655200 | script_fn_load_unit | 86 | |
| 0x655420 | script_fn_del_unit | 87 | |
| 0x6554F0 | script_fn_set_unit_pos | 88 | |
| 0x655590 | script_fn_set_unit_dir | 89 | |
| 0x655620 | script_fn_set_unit_degree | 90 | |
| 0x655680 | script_fn_get_unit_dir | 91 | |
| 0x6556E0 | script_fn_set_unit_ranking | 92 | |
| 0x655750 | script_fn_set_unit_nation | 93 | |
| 0x655810 | script_fn_deactivate_unit | 94 | |
| 0x655870 | script_fn_activate_unit | 95 | |
| 0x6558D0 | script_fn_set_unit_target_unit_recno | 96 | |
| 0x655930 | script_fn_set_unit_ride_horse | 97 | |
| 0x655990 | script_fn_get_unit_riding_recno | 98 | |
| 0x655B20 | script_fn_get_unit_horse_recno | 99 | |
| 0x6559E0 | script_fn_set_unit_ride_cart | 100 | |
| 0x655A40 | script_fn_set_unit_ride_off_cart | 101 | |
| 0x655BD0 | script_fn_get_interact_unit_recno | 102 | |
| 0x655C20 | script_fn_set_interact_unit_recno | 103 | |
| 0x655C80 | script_fn_get_interact_item_recno | 104 | |
| 0x655CD0 | script_fn_set_interact_item_recno | 105 | |
| 0x655DE0 | script_fn_get_can_interact_flag | 106 | |
| 0x655E30 | script_fn_set_can_interact_flag | 107 | |
| 0x655E90 | script_fn_set_can_bribe_flag | 108 | |
| 0x655F10 | script_fn_get_can_bribe_flag | 109 | |
| 0x655F80 | script_fn_set_units_can_bribe_flag | 110 | |
| 0x656030 | script_fn_set_units_in_place_can_bribe_flag | 111 | |
| 0x6561B0 | script_fn_is_unit_bribed | 112 | |
| 0x655D30 | script_fn_get_interact_stage | 113 | |
| 0x655D80 | script_fn_set_interact_stage | 114 | |
| 0x656210 | script_fn_set_unit_retreat | 115 | |
| 0x656260 | script_fn_get_unit_retreat | 116 | |
| 0x6562B0 | script_fn_set_unit_armor_id | 117 | |
| 0x656300 | script_fn_get_unit_armor_id | 118 | |
| 0x656350 | script_fn_set_unit_name | 119 | |
| 0x6563B0 | script_fn_get_unit_name | 120 | string result |
| 0x656410 | script_fn_set_unit_dead | 121 | |
| 0x6568D0 | script_fn_reset_unit_hp | 122 | |
| 0x656990 | script_fn_set_unit_hp | 123 | |
| 0x656940 | script_fn_get_unit_hp | 124 | |
| 0x6569F0 | script_fn_get_leader_recno | 125 | |
| 0x656A60 | script_fn_contain_team_member | 126 | |
| 0x657670 | script_fn_attach_unit | 127 | |
| 0x656AF0 | script_fn_get_attaching_unit_recno | 128 | |
| 0x657CC0 | script_fn_detach_unit | 129 | |
| 0x656B50 | script_fn_can_attach | 130 | |
| 0x656BB0 | script_fn_is_all_attached | 131 | |
| 0x656DC0 | script_fn_get_nearest_attach_point | 132 | |
| 0x657110 | script_fn_get_nearest_attach_point_x | 133 | |
| 0x657240 | script_fn_get_nearest_attach_point_y | 134 | |
| 0x657370 | script_fn_get_nearest_attach_point_z | 135 | |
| 0x6574A0 | script_fn_set_control_flag | 136 | |
| 0x6575C0 | script_fn_get_control_flag | 137 | |
| 0x6592C0 | script_fn_get_follow_unit_distance | 138 | |
| 0x659400 | script_fn_get_two_unit_distance | 139 | |
| 0x657610 | script_fn_set_idle | 140 | |
| 0x657FD0 | script_fn_set_unit_team | 141 | |
| 0x658160 | script_fn_get_unit_team_size | 142 | |
| 0x6581F0 | script_fn_get_unit_team_leader_by_team_id | 143 | |
| 0x6582F0 | script_fn_set_team_nation | 144 | |
| 0x658420 | script_fn_set_team_nation2 | 145 | |
| 0x6584E0 | script_fn_recruit_team_member | 146 | |
| 0x658650 | script_fn_deactivate_team_member | 147 | |
| 0x658730 | script_fn_activate_team_member | 148 | |
| 0x658810 | script_fn_set_formation_pos | 149 | |
| 0x658920 | script_fn_set_team_member_pos | 150 | |
| 0x658AD0 | script_fn_kill_team | 151 | |
| 0x658D90 | script_fn_disable_team_drop_item | 152 | |
| 0x65DC20 | script_fn_set_team_prisoner | 153 | R |
| 0x65DCF0 | script_fn_free_team_prisoner | 154 | R |
| 0x658E10 | script_fn_set_team_control_flag | 155 | |
| 0x658EA0 | script_fn_set_unit_curr_item | 156 | |
| 0x658F70 | script_fn_set_main_character | 157 | |
| 0x659060 | script_fn_get_main_character | 158 | |
| 0x659080 | script_fn_del_unit_team | 159 | |
| 0x659210 | script_fn_del_unit_team_member | 160 | |
| 0x6511F0 | script_fn_string_to_order_para | 161 | R (→ 0x4BBAC0) |
| 0x6520D0 | script_fn_get_curr_task | 162 | |
| 0x652190 | script_fn_get_curr_task_para | 163 | |
| 0x652280 | script_fn_get_curr_task_state | 164 | |
| 0x6522F0 | script_fn_set_curr_task_state | 165 | |
| 0x652370 | script_fn_stop_curr_task | 166 | → 0x528300 |
| 0x6523C0 | script_fn_stop_all_team_task | 167 | |
| 0x653EB0 | script_fn_search_place_by_name | 168 | |
| 0x653EF0 | script_fn_search_for_unit_in_place | 169 | |
| 0x653F30 | script_fn_is_unit_inside_place | 170 | |
| 0x653FE0 | script_fn_number_of_enemy_in_place | 171 | |
| 0x654020 | script_fn_number_of_fightable_enemy_in_place | 172 | |
| 0x654060 | script_fn_enemy_exist_in_place | 173 | |
| 0x6540A0 | script_fn_fightable_enemy_exist_in_place | 174 | |
| 0x6540E0 | script_fn_number_of_people_by_nation_in_place | 175 | |
| 0x654120 | script_fn_people_by_nation_exist_in_place | 176 | |
| 0x654160 | script_fn_general_by_nation_exist_in_place | 177 | |
| 0x6541A0 | script_fn_get_spawn_unit_count_in_place | 178 | |
| 0x654240 | script_fn_change_all_item_in_place | 179 | |
| 0x654280 | script_fn_get_place_captured | 180 | |
| 0x6542B0 | script_fn_set_place_captured | 181 | |
| 0x654560 | script_fn_set_place_nation_by_code | 182 | |
| 0x6547A0 | script_fn_get_place_unit_view_angle | 183 | |
| 0x6547E0 | script_fn_set_place_unit_view_angle | 184 | |
| 0x654830 | script_fn_set_parking_dir | 185 | |
| 0x6548B0 | script_fn_search_facility_by_name | 186 | |
| 0x6548F0 | script_fn_facility_barrack_add_leader | 187 | |
| 0x654960 | script_fn_facility_barrack_add_member | 188 | |
| 0x6549C0 | script_fn_get_unit_facility_distance | 189 | |
| 0x654AE0 | script_fn_set_facility_follow_unit_recno | 190 | |
| 0x654B30 | script_fn_get_spawning_count | 191 | |
| 0x6541D0 | script_fn_reset_spawn_count_in_facility | 192 | |
| 0x654BC0 | script_fn_set_can_recruit_flag | 193 | |
| 0x654C20 | script_fn_set_recruit_level | 194 | |
| 0x654C80 | script_fn_set_barrack_captured | 195 | |
| 0x654D70 | script_fn_facility_barrack_add_leader_order | 196 | |
| 0x654E30 | script_fn_shop_set_sell_item_list | 197 | |
| 0x654EA0 | script_fn_set_can_regen_sell_item | 198 | |
| 0x654F10 | script_fn_barrack_set_recruit_list | 199 | |
| 0x654FA0 | script_fn_workshop_set_recruit_list | 200 | |
| 0x653DF0 | script_fn_set_nation_friendship | 201 | |
| 0x655010 | script_fn_set_relation_status | 202 | |
| 0x6550D0 | script_fn_set_nation_military_strength | 203 | |
| 0x6594E0 | script_fn_set_game_finish | 204 | |
| 0x659550 | script_fn_set_game_start_date | 205 | 6 args |
| 0x6595E0 | script_fn_is_game_finish | 206 | |
| 0x659610 | script_fn_set_game_time_limit | 207 | |
| 0x6596A0 | script_fn_get_game_time_limit | 208 | |
| 0x6596F0 | script_fn_calculate_date | 209 | |
| 0x659720 | script_fn_get_game_date | 210 | R |
| 0x659740 | script_fn_get_game_hour | 211 | R (float) |
| 0x659790 | script_fn_get_game_minute | 212 | R (`fmod(…,60)` + `__ftol`) |
| 0x6597D0 | script_fn_get_game_day | 213 | R |
| 0x6597F0 | script_fn_get_game_month | 214 | R |
| 0x659810 | script_fn_get_game_year | 215 | R |
| 0x659830 | script_fn_change_scenario_step | 216 | |
| 0x659860 | script_fn_set_ride_mode | 217 | |
| 0x659890 | script_fn_set_fog_distance | 218 | |
| 0x6598C0 | script_fn_set_objective_count_time | 219 | |
| 0x6598F0 | script_fn_get_objective_count_time | 220 | |
| 0x659910 | script_fn_set_scenario_count_time | 221 | |
| 0x659940 | script_fn_get_scenario_count_time | 222 | |
| 0x659960 | script_fn_set_scenario_count_time_2 | 223 | |
| 0x659990 | script_fn_get_scenario_count_time_2 | 224 | |
| 0x6599B0 | script_fn_set_scenario_count_time_3 | 225 | |
| 0x6599E0 | script_fn_get_scenario_count_time_3 | 226 | |
| 0x659A00 | script_fn_set_scenario_rescue_count | 227 | |
| 0x659A30 | script_fn_set_scenario_rescue_limit | 228 | |
| 0x659A60 | script_fn_add_reputation | 229 | |
| 0x659A90 | script_fn_set_reputation | 230 | |
| 0x659AC0 | script_fn_get_reputation | 231 | |
| 0x659AE0 | script_fn_set_citizen_killed_count | 232 | |
| 0x659E80 | script_fn_get_citizen_killed_count | 233 | |
| 0x659EA0 | script_fn_set_fcitizen_killed_count | 234 | |
| 0x659ED0 | script_fn_get_fcitizen_killed_count | 235 | |
| 0x659B30 | script_fn_set_enemy_bribed_count | 236 | |
| 0x659B10 | script_fn_get_enemy_bribed_count | 237 | |
| 0x659E40 | script_fn_get_enemy_bribed_cost | 238 | |
| 0x659B60 | script_fn_get_gold_crates_get | 239 | |
| 0x659B80 | script_fn_change_mission_gold_to_real_gold | 240 | |
| 0x659CA0 | script_fn_get_food_crates_get | 241 | |
| 0x659CC0 | script_fn_get_food_picked_up | 242 | |
| 0x659CE0 | script_fn_set_food_can_be_picked | 243 | |
| 0x659D10 | script_fn_set_total_food | 244 | |
| 0x659D40 | script_fn_get_total_food | 245 | |
| 0x659D60 | script_fn_set_citizen_saved | 246 | |
| 0x659D90 | script_fn_get_citizen_saved | 247 | |
| 0x659DB0 | script_fn_increase_citizen_saved | 248 | |
| 0x659DE0 | script_fn_add_scenario_general | 249 | |
| 0x659E10 | script_fn_remove_scenario_general | 250 | |
| 0x659E60 | script_fn_get_bribed_enemy_kill_count | 251 | |
| 0x659EF0 | script_fn_set_house_burnt_count | 252 | |
| 0x659F20 | script_fn_get_house_burnt_count | 253 | |
| 0x659F40 | script_fn_set_challenge_restart_duration | 254 | |
| 0x65A010 | script_fn_get_challenge_restart_duration | 255 | |
| 0x65A0E0 | script_fn_set_challenge_flag | 256 | |
| 0x65A1B0 | script_fn_get_challenge_flag | 257 | |
| 0x65A2A0 | script_fn_set_challenge_duration | 258 | |
| 0x65A280 | script_fn_get_challenge_duration | 259 | |
| 0x65A2D0 | script_fn_battle_is_in_game | 260 | |
| 0x65A2F0 | script_fn_switch_controllable_unit | 261 | |
| 0x65A320 | script_fn_quit_rts_mode | 262 | |
| 0x65A350 | script_fn_add_end_condition | 263 | |
| 0x65A370 | script_fn_del_end_condition | 264 | |
| 0x65A390 | script_fn_set_end_condition_type | 265 | |
| 0x65A3C0 | script_fn_set_end_condition_des | 266 | localised str |
| 0x65A400 | script_fn_set_end_condition_des_2 | 267 | localised str |
| 0x65A460 | script_fn_set_end_condition_territory_link | 268 | |
| 0x65A4A0 | script_fn_set_end_condition_finish | 269 | |
| 0x65A670 | script_fn_get_end_condition_finish | 270 | |
| 0x65A6A0 | script_fn_set_end_condition_para1 | 271 | |
| 0x65A6E0 | script_fn_set_end_condition_para2 | 272 | |
| 0x65A720 | script_fn_set_end_condition_enabled | 273 | |
| 0x65A7E0 | script_fn_get_end_condition_enabled | 274 | |
| 0x65A810 | script_fn_set_end_condition_gold_award | 275 | |
| 0x65A840 | script_fn_set_end_condition_experience_award | 276 | |
| 0x65A870 | script_fn_set_scenario_bit_flag | 277 | |
| 0x65A8A0 | script_fn_get_scenario_bit_flag | 278 | |
| 0x65A8D0 | script_fn_set_scenario_event_bit_flag | 279 | |
| 0x65A900 | script_fn_get_scenario_event_bit_flag | 280 | |
| 0x65A930 | script_fn_reset_scenario_event_bit_flag | 281 | |
| 0x65A960 | script_fn_set_scenario_event_bit_flag_2 | 282 | |
| 0x65A990 | script_fn_get_scenario_event_bit_flag_2 | 283 | |
| 0x65A9C0 | script_fn_set_scenario_event_bit_flag_3 | 284 | |
| 0x65A9F0 | script_fn_get_scenario_event_bit_flag_3 | 285 | |
| 0x65AA20 | script_fn_set_scenario_stage | 286 | |
| 0x65AA80 | script_fn_get_scenario_stage | 287 | |
| 0x65AAD0 | script_fn_set_scenario_mode | 288 | |
| 0x65AB10 | script_fn_set_scenario_name | 289 | localised str |
| 0x65AB50 | script_fn_set_scenario_description | 290 | localised str |
| 0x65AB90 | script_fn_backup_bit_flag | 291 | |
| 0x65ACA0 | script_fn_restore_bit_flag | 292 | |
| 0x65ADB0 | script_fn_set_music_side_environment_enable | 293 | |
| 0x65ADE0 | script_fn_set_cut_scene_flag | 294 | |
| 0x65B130 | script_fn_get_cut_scene_flag | 295 | R (`0x4301C0()` byte) |
| 0x65AEF0 | script_fn_start_cut_scene_short | 296/297 | R: `0x6CB9E0(now,128.0,…)`; arity overload |
| 0x65AE60 | script_fn_start_cut_scene_long | 296/297 | R: `0x6CAD50(now,128.0,…)` |
| 0x65AFF0 | script_fn_end_cut_scene | 298/299 | R: `0x6CC6A0(now,-128.0,a,b)` |
| 0x65AFD0 | script_fn_get_cut_scene_status | 300 | → 0x65AFB0 (`obj->+0xF`) |
| 0x6537A0 | script_fn_start_cut_scene_camera | 301 | |
| 0x65B030 | script_fn_change_camera_in_cut_scene_5 | 302 | R: → 0x6CD720 |
| 0x65B070 | script_fn_change_camera_in_cut_scene_7 | 302 | R: → 0x6CCF00 (args converted to float) |
| 0x65B0E0 | script_fn_is_cut_scene_status_black_screen | 303 | R: `!0x42CED0() ? (0x65AFB0()==7) : true` |
| 0x653890 | script_fn_start_buy_interface | 304 | |
| 0x653930 | script_fn_close_inventory_interface | 305 | |
| 0x653970 | script_fn_show_objective | 306 | |
| 0x6539A0 | script_fn_start_scenario_part_interface | 307 | |
| 0x6539E0 | script_fn_minimap_set_way_point | 308 | |
| 0x653A50 | script_fn_minimap_set_way_point_follow_unit | 309 | |
| 0x65B800 | script_fn_get_distance | 310 | |
| 0x65B8C0 | script_fn_set_getting_weapon_flag | 311 | |
| 0x65B950 | script_fn_is_getting_weapon | 312 | |
| 0x65B9B0 | script_fn_get_default_weapon | 313 | |
| 0x65BCE0 | script_fn_drop_item_on_floor | 314 | |
| 0x65BED0 | script_fn_save_game | 315 | |
| 0x651260 | script_fn_get_curr_order | 325 | R |
| 0x65C050 | script_fn_change_animation_special | 328 | |
| 0x65C420 | script_fn_display_title_screen | 329 | R: → 0x761680(`DAT_00A500BA`, 4 args) |
| 0x65C4A0 | script_fn_pause_script | 330 | R |
| 0x65C500 | script_fn_set_stop_unit_process_flag | 331 | R |
| 0x65C530 | script_fn_add_gate_trigger | 332 | R |
| 0x65C5F0 | script_fn_give_item_to_unit | 333 | |
| 0x65C770 | script_fn_give_item_to_unit_troop | 334 | |
| 0x65C870 | script_fn_set_unit_training_mode | 335 | → 0x510030 |
| 0x65C8D0 | script_fn_is_unit_dead | 336 | |
| 0x65C930 | script_fn_is_unit_dead_by_name | 337 | |
| 0x65C9B0 | script_fn_get_scenario_passed_real_time | 338 | R: `DAT_00A500BA - DAT_00A5916D` |
| 0x65C9E0 | script_fn_add_unit_experience | 339 | |
| 0x65CA40 | script_fn_get_item_class_quantity_in_unit_inventory | 340 | |
| 0x65CAC0 | script_fn_get_item_quantity_in_unit_inventory | 341 | |
| 0x65CB80 | script_fn_number_of_enemy_in_world | 342 | |
| 0x65CD00 | script_fn_number_of_friend_in_world | 343 | |
| 0x65CEA0 | script_fn_attention_call_message | 344 | localised str |
| 0x65CF00 | script_fn_attention_call_message_2_str | 345 | arg 2 type 3 |
| 0x65CF40 | script_fn_attention_call_message_2_int | 345 | arg 2 type 1 |
| 0x65CF80 | script_fn_set_attention_call_message_voice | 346 | localised str |
| 0x65CFD0 | script_fn_switch_unit_model | 347 | |
| 0x65D070 | script_fn_enable_unit_physic | 348 | |
| 0x65D0D0 | script_fn_enable_unit_bounding_volume | 349 | R |
| 0x65D350 | script_fn_enable_unit_team_bounding_volume | 350 | R |
| 0x65D450 | script_fn_mount_siege_tower_to_wall | 351 | R |
| 0x65D560 | script_fn_is_unit_parked | 352 | R |
| 0x65D6F0 | script_fn_unit_dist_to_track | 353 | R (float) |
| 0x65D850 | script_fn_get_unit_cur_track_recno | 354 | R (float) |
| 0x65D8A0 | script_fn_get_team_size | 355 | R |
| 0x65D9C0 | script_fn_display_string | 356 | R |
| 0x65DA50 | script_fn_display_number | 357 | R |
| 0x65DAF0 | script_fn_set_prisoner | 358 | R |
| 0x65DBA0 | script_fn_free_prisoner | 359 | R |
| 0x65DDB0 | script_fn_dist_to_attaching_point | 360 | R (float) |
| 0x65DFA0 | script_fn_get_attaching_index | 361 | R |
| 0x65DFF0 | script_fn_set_item_path_item_owner | 362 | R |
| 0x65E060 | script_fn_format_string | 363 | R: `0x5A31E0(fmt, va_list)` + `str_copy400` into the result buffer |
| 0x65E0A0 | script_fn_get_key_string | 364 | R: action-name → bound key string (0x4CB5B0 on the key-binding table of mode `DAT_00A5B9F8`) |
| 0x65E7D0 | script_fn_set_kill_citizen_limit | 365 | R |
| 0x65E810 | script_fn_start_kill_interface | 366 | R |
| 0x65E850 | script_fn_end_kill_interface | 367 | R |
| 0x65E880 | script_fn_set_kill_citizen_limit_2 | 368 | R |
| 0x65E8C0 | script_fn_start_kill_interface_2 | 369 | R |
| 0x65E900 | script_fn_end_kill_interface_2 | 370 | R |
| 0x65CB20 | script_fn_remove_item_from_unit_inventory | 371 | R: unit valid → `0x5101C0(item, count)` |
| 0x65E930 | script_fn_set_item_can_interact_flag | 372 | R |
| 0x65E9A0 | script_fn_set_unit_can_rided_flag | 373 | R |
| 0x65EA00 | script_fn_is_item_on_floor | 374 | R |
| 0x65EA50 / 0x65EAD0 / 0x65EB50 | script_fn_get_item_pos_x / _y / _z | 375..377 | R (float) |
| 0x65EC90 / 0x65ECF0 | script_fn_set_minimap_icon_display_enable / get_… | 378/379 | R (guarded by 0x44CF30/0x44CF10) |
| 0x65ED50 / 0x65ED70 | script_fn_get_charmove_mode / is_rts_mode | 380/381 | R |
| 0x65EDA0 / 0x65EDD0 / 0x65EE30 | script_fn_set_minimap_rotation / set_minimap_display_gold_enable / set_critical_attack_enabled | 382..384 | R |
| 0x65EE60 | script_fn_show_scenario_brief | 385 | R |
| 0x65EEE0 | script_fn_turn_camera | 386 | R (blocking loop) |
| 0x65EF40 | script_fn_share_team_member | 387 | R |
| 0x65F0F0 | script_fn_get_item_angle_diff | 388 | R |
| 0x65F160 | script_fn_move_and_attach_unit | 389 | R |
| 0x65F600 | script_fn_is_cut_scene_finished | 390 | R |
| 0x65F670 | script_fn_set_charmove_detect_block_flag | 391 | R |
| 0x65F6A0 | script_fn_set_victory_display_flag | 392 | R |
| 0x65F740 / 0x65F780 / 0x65F7C0 / 0x65F820 | script_fn_set_minimap_rectangular / circular / quadrilateral_vision_area_enable / disable_minimap_vision_area (forward to 0x7196E0 / 0x719770 / 0x7197F0 / 0x7198D0) | 393..396 | R |
| 0x65F850 | script_fn_set_territory_nation | 397 | R |
| 0x65F910 | script_fn_show_letter_interface | 398 | R (`interfac::i_letter`) |
| 0x65F980 | script_fn_set_unit_vision_range | 399 | R |
| 0x65F9E0 | script_fn_set_unit_team_vision_range | 400 | R |
| 0x65FB50 | script_fn_set_unit_hide_in_command_interface | 401 | R |
| 0x65FBA0 | script_fn_set_delay_before_win | 402 | R |
| 0x65FBD0 | script_fn_is_objective_menu_opened_in_scenario | 403 | R |
| 0x65FBF0 | script_fn_is_attention_call_message_shown | 404 | R |
| 0x65FC90 / 0x65FCC0 | script_fn_set_end_cut_scene_type / show_ending | 405/406 | R |
| 0x65FD10 | script_fn_set_prisoner_can_be_attacked_flag | 407 | R |

Depth-2 helpers behind handlers (not independent builtins): 0x652A10 (`set_snow_level` impl, thiscall `(this, i16)`), 0x659760 (`float` `this->+0xC / DAT_009CF9CC`), 0x65AFB0 (`obj->+0xF`, cut-scene state), 0x65EE00
(thiscall setter `this->+0x11 = i16`), 0x654530, 0x654A80/0x654AB0 (field setters `+0x44`/`+0x4A`), 0x655B70/0x655BA0 (setters `+0x12`/`+0x16`). Some depth-2 setters may be vtable slots of unexported classes (e.g. candidates for the
FUNC/ACTION/DIALOG slot 13): **TODO(re)**: none of them is called by the dispatcher, so they are not the WAIT predicate unless reached through the call class' vtable (not exported).

## 3a. Call-class vtables and ACTION/poll functions (from the vtable dump)

| vtable | class | slots (0..13) |
|---|---|---|
| 0x9DB040 | FUNC call class 0x15 | dtor `0x643040`, set_parent `0x63E630`, free `0x63E7F0`, **exec `0x643B60`**, parse `0x643C00`, find_by_id `0x643700`, `0x63EE70`, value_ptr `0x63F140`, kind `0x642470`, self `0x6424D0`, null `0x6424B0`, memory_size `0x642190`, **dispatch `0x649920`**, **WAIT poll `0x6509E0`** |
| 0x9DB10C | ACTION call class 0x18 | `0x643280`, `0x63E630`, `0x63E7F0`, exec `0x643B60`, parse `0x643D40`, `0x643700`, `0x63EE70`, `0x63F140`, `0x642470`, `0x6424D0`, `0x6424B0`, memory_size `0x642220`, **dispatch `0x661060`**, **WAIT poll `0x6612E0`** |
| 0x9DB25C | statement wrapper class 0x14 (12 slots) | `0x643E80`, `0x63E630`, `0x63E660`, exec `0x643F80`, parse `0x644190`, `0x6436B0`, `0x63EDD0`, `0x644330`, `0x644360`, `0x642490`, `0x6424B0`, `0x642250` (no slot 12/13; the WAIT reaches the inner call through `0x644390`) |
| 0x9DB084 | DIALOG call class 0x16 | exec `0x643B60`, **dispatch `0x65FD70`** (ids 0x13C..0x142), **WAIT poll `0x660E60`** |
| 0x9DB0C8 | CAMERA call class 0x17 | exec `0x643B60`, dispatch `0x661020` (empty), poll `0x661040` (returns 1) |

(Note: the earlier pass named `0x643B60` "exec of the call classes" and `0x643C00` "parse"; the vtables confirm both. The FUNC memory_size slot is `0x642190` (not `0x642220`).)

| address | proposed name | purpose | confidence |
|---|---|---|---|
| 0x6509E0 | `script_func_wait_poll` | slot 13 of the FUNC class: per-id predicate (ids 0x12D, 0x130, 0x149, 0x14A, 0x181, 0x128/0x129/0x12E, 0x186, 0x12F, 0x18E); any other id → 1 (done). Details in BUILTINS.md §5.1 | verified |
| 0x653A90 | `script_poll_cut_scene_camera` | poll of `start_cut_scene_camera` | read |
| 0x653C20 | `script_poll_shop_closed` | `!0x6C9040()` | verified |
| 0x65C470 | `script_poll_title_screen` | `0x7616F0()` | verified |
| 0x65C4D0 | `script_poll_pause_elapsed` | `DAT_00A6A254 < DAT_00A500BA` | verified |
| 0x65EEA0 | `script_poll_scenario_brief` | `!0x4303D0()` | verified |
| 0x65AF50 | `script_poll_cut_scene_state` | not done iff `0x65AFB0() != 0 && != 2 && DAT_00A5D3C0 == 0` | verified |
| 0x65F950 | `script_poll_letter_closed` | `!0x6E0A90()` | verified |
| 0x661060 | `script_action_dispatch` | ACTION slot 12: ids 0x143 → 0x661340, 0x144 → 0x6618F0, 0x146 → 0x6619D0, 0x147 → 0x661B40; sets `call+0xC = 1` via 0x660FF0 and stores the handler result at `call+0x10` | verified |
| 0x6612E0 | `script_action_wait_poll` | ACTION slot 13: id 0x143 → `0x661CA0(unit=arg1, order=call+0x22)`, else 1 | verified |
| 0x661CA0 | `order_wait_done(unit, order)` | done iff unit invalid / no current order / order neither current nor pending (with a force-complete branch in cut-scene states 0, 6..8) | read |
| 0x661340 / 0x6618F0 / 0x6619D0 / 0x661B40 | `script_action_give_order` / `…stop_all_order` / `…give_task` / `…give_team_task` | ACTION handlers (0x661340 is a 1154-byte routine using the order tables 0x4BB8D0/0x4BAC60/0x4BAB50) | read (only 0x6618F0 fully) |
| 0x4BB8D0 | `order_is_current_or_pending(order)` | `this+0x1D == order` or present in the pending list | read |

## 4. Open questions (also in BUILTINS.md §7)
* Round 3 added: `0x65FD70 script_dialog_dispatch`, `0x660E60 script_dialog_wait_poll`, `0x63D7B0 dialogs_all_finished`, `0x63D860 dialog_choice_poll` (stores the chosen index at `dialog+0x353`), `0x63D150 dialogs_remove_finished`, `0x63D120 dialog_remove`, `0x63D770 dialog_get_choice`, `0x63CDC0 dialog_create`, `0x63D250 dialog_add_choice`, `0x661020/0x661040` CAMERA stubs, `0x63EB80 script_error` (fatal: `MessageBoxA` + exit −2 via `0x586CB0`/`0x7B47FC`), `0x661E10` loop bound `i <= 0x196` (id 407 reachable).
* TODO(re) dialog manager internals (0x63CDC0, 0x63D250, 0x63E3F0), DIALOG parse step.
* TODO(re) bodies of 0x661340, 0x6619D0, 0x661B40 and the effect of 0x4BB660/0x4BAEE0.
