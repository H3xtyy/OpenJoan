# unit family — `unit.cpp`, `unitres.cpp`, `unitures.cpp`, `uscrres.cpp`, `unitlvl.cpp`, `levelres.cpp`, `upgrares.cpp`, native AI (joan.exe)

Exports: `$JOAN_WORK_DIR/decomp/game/unit/` (703 functions; INDEX.md), plus a few helpers read from `decomp/game/battle`, `decomp/script/script_vm`, `decomp/binfmt*`.
Structs: [Unit](../structs/Unit.md), [UnitRes](../structs/UnitRes.md), [UnitLevel](../structs/UnitLevel.md), [UnitAi](../structs/UnitAi.md), existing [UnitScriptRes](../structs/UnitScriptRes.md).
 Status tags: **V** verified (two independent reads), **R** read once, **I** inferred from shape/callers.
All addresses are joan.exe. `this`=ECX. Most callee thunks lose their arguments in the decompiler; where an argument is only inferred it is called out.

Naming: `unit_*` = `unit.cpp` (the only file whose path survives: `D:\ww\unit.cpp` in 0x461F10/0x4F8550), `ai_cls<N>_*` = native AI of `uscr_run_native_ai` class N.
Global state used by the family: `DAT_00A500BA` = game clock ms (called `now`), `DAT_00A500BE` = per-frame step ms (called `dt`, 0 while paused), `DAT_00A5B9E8` = main-character recno,
`DAT_00A5B9F8` = control mode (4 = strategy/RTS camera, 2 = ?), `DAT_00A45F3E` = current `World*`, `DAT_00A06BF4` = "world running" flag, `DAT_00A596AB` = `set_stop_unit_process_flag`,
`DAT_00A57FC2` = difficulty (0 easy .. 3 very hard), `DAT_00A5D384` = freeze flag that blocks HP/EP regen, `DAT_00A596C4` = party gold, `DAT_00A5B9F0` = command points (clamped to `MAX_COM` 2000).

## 1. Function index

### 1.1 Resource loaders and lookups

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x524E30 | `unitres_load_all` | open `UNIT\unit.res` (load-all ResLX), alloc `count*0x203`, per entry set `recno`, parse, copy ResLX name over `Code` | `unitres.cpp`, `unit.res` | V |
| 0x525080 | `unitres_parse_record` | positional text parser of one unit record (field table in UnitRes.md) | 6.5 KB, keywords | V |
| 0x524D00 / 0x524DC0 / 0x5273B0 | `unitres_free` / `unitres_free_records` / `unitres_free_attach` | frees attach arrays of all records, then the table | `unitres.cpp` | V |
| 0x42D0A0 | `unitres_get(id)` | `base + (id-1)*0x203` | all callers | V |
| 0x527130 | `unitres_rank_from_name` | INFANTRY 0, ARCHER 1, CAVALRY 2, CAPTAIN 3, GENERAL 4, `<0x9D0B10>` 5 | literals | V |
| 0x527220 | `unitres_class_from_name` | CITIZEN 0 .. SIEGE_WEAPON 8, `<0x9D7B3C>` 9, else 1 | literals | V |
| 0x527D20 | `unitres_collision_from_name` | None 0, Sphere 1, Box3d 2, Cylinder 3 | literals | V |
| 0x527790 | `unitres_target_priority(target_type)` | priority 0..7 of a target class for the searching unit's class (matrix in UnitAi.md §6); used by NEAREST_HIGH_PRIORITY | switch tables | V |
| 0x5464A0 | `uscr_find_by_name` | AI table lookup by script file stem (stride 0x28, 1-based) | `unitAI.res` names | V |
| 0x531DF0 | `unitures_find_list` | upgrade-list lookup by name (stride 0x35) | | V |
| 0x463ED0 | `dropres_find_list` | drop-item list by name (stride 0x2B) | | V |
| 0x4CA400 | `raceres_find` | race code -> index (stride 0x34) | | V |
| 0x4A36C0 | `itemres_find_by_code` | item code -> recno (bsearch when sorted, else linear on stride 0x2BF) | | R |
| 0x418430 | `animgroup_find` | animation-group name -> id (stride 0x3A) | | V |
| 0x430050 | `unit_is_hero_code(code)` | true if code is one of the 4 controllable-hero names (0x9F861C, stride 0x104) | | V |
| 0x5462B0 / 0x546140 / 0x546200 / 0x546520 / 0x484540 | `uscr_load` / `uscr_free` / `uscr_release_scripts` / `uscr_class_from_name` / `uscr_record` | see [script_res.md](script_res.md) §3 | | V |
| 0x5467B0 | `uscr_run_native_ai(rec, unit)` | `switch(rec.ai_class)` -> 15 handlers (§12); returns 0 for class 0 | | V |
| 0x546970 | `uscr_decide_attack(rec, unit)` | class -> "attack decision" function (§12.2); not a script loader | script_res.md §3 | V |
| 0x4A4CA0 / 0x4A4C30 / 0x4A4E60 | `levelres_load` / `levelres_free` / `levelres_find(level)` | LEVEL table (7-byte rows) | `levelres.cpp` | V |
| 0x530D70 / 0x530D00 | `unitures_load` / `unitures_free` | unit upgrade lists | `unitures.cpp` | V |
| 0x531790 / 0x531BA0 / 0x50D240 | `upgrares_load` / `upgrare_type_from_name` / `upgrare_get(n)` | UPITEM records (100 B) | `upgrares.cpp` | V |
| 0x531E70 / 0x531ED0 / 0x5239E0 | `unitures_get_list` / `upgrade_is_skill(idx)` / `unitures_entry(n)` | | | V |

### 1.2 Unit lifecycle

| address | proposed name | purpose | evidence | conf |
|---|---|---|---|---|
| 0x4155E0 | `Unit::Unit` | ctor (0x4B4 bytes, registers raw-save image) | | V |
| 0x51DFC0 | `unit_create(type_id)` | `new Unit` + `unit_init` + table insert; if `DAT_00A58F95` (MP re-use) reuses a matching dead slot | | R |
| 0x4F8550 | `unit_init(type_id)` | initialise every field from the UnitRes (§2) | `D:\ww\unit.cpp` | V |
| 0x461F10 | `unit_destroy` | release team membership, nation registration, orders, effects, actors, items, mount/cart links (§2.2) | `D:\ww\unit.cpp` | V |
| 0x51E1F0 | `unit_table_remove(recno)` | frees the record (virtual dtor 0x471530) and removes it from the table; clears the "selected" recno (`table+0x2E`) if equal | | V |
| 0x5089E0 | `unit_set_rank(rank)` | sets `+0x104`, leaves team when rank > 2, refreshes nation rank counters | | V |
| 0x509250 | `unit_set_nation(nation)` | unregisters from the old nation (army-strength delta `-maxhp*weight(type)`, nation count--) and registers in the new one | | V |
| 0x527DD0 | `unit_strength_weight(type)` | `UnitRes.Ranking` of the type: 0/1/2 -> 1, 3 -> 2, 4/5 -> 3, other -> 0 (see §9.10.6) | | V |
| 0x4B5520 | `nation_add_strength(delta)` | `nation+0x1389 += delta`, clamped at 0 | | V |
| 0x42D0E0 | `unit_is_dead` | `hp < 1 && lives == 0` | | V |
| 0x432B90 | `unit_is_prisoner` | `+0x2E1` | | V |
| 0x430190 | `unit_mount` | `+0x1E2` | | V |
| 0x44A660 | `unit_is_leader` | `+0x33 != 0` | | V |
| 0x4216D0 | `unit_leader_or_self` | self if leader, else the leader `Unit*` (via `+0x37`) or NULL | | V |
| 0x6569F0 | `unit_leader_recno` | `get_leader_recno`: leader recno, 0 when none or self | | V |
| 0x4301C0 | `game_cutscene_active` | `*(this+0x744)` of the game object | | V |

### 1.3 Stat accessors and setters

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x523320 | `unit_max_hp` | `+0x13D + equip_bonus(1)` | V |
| 0x523700 | `unit_max_ep` | `+0x149 + equip_bonus(2)` | V |
| 0x5238C0 | `unit_defense_total` | `+0x159 + equip_bonus(5)` | V |
| 0x5237E0 / 0x523860 / 0x523920 / 0x5103C0 | strength / dexterity / leadership / ? totals (stats 3,4,6,0x12) | I |
| 0x497E10 | `unit_equip_bonus(stat_id, recno)` | combine the bonuses of the held weapon set + the three inventory arrays (§7) | V |
| 0x4A41C0 / 0x4A4200 / 0x4A42F0 | `item_bonus_value` / `item_bonus_find` / `item_bonus_requirements_ok` | item-def helper: bonus = base + (item_level - lvl0)*step; requirement list tests HP/EP/stat thresholds | V |
| 0x4FE510 | `unit_set_hp(v)` | clamp `[0,max]`; at 0 with `lives>0`: `lives--`, `hp = hp_mark = max` | V |
| 0x4FE5D0 | `unit_set_ep(v)` | clamp `[0,max]` | V |
| 0x4FE640 / 0x4FE670 | `unit_set_lives_max` / `unit_set_lives` | `+0x11C` / `+0x120` | V |
| 0x4FE6E0.. 0x4FECD0 | `unit_set_strength/dex/def/leadership/archery/...` | plain field setters (`+0x155 +0x157 +0x159 +0x15B +0x16F +0x173 +0x177 +0x17B +0x17F +0x183 +0x187 +0x18B +0x18F +0x193 +0x197`) | V |
| 0x45B720 | `unit_set_base_max_hp(v)` | raising the base also raises current HP by the delta | V |
| 0x4FEA80 | `unit_set_balance(v)` | clamp `[0,100]` into `+0x1CB`; 0 -> knock-down (`0x509000(0)`; rider: action 0x4E + `0x4FF9F0`) | V |
| 0x4FEDB0 / 0x4FEDF0 | `unit_get_pos(vec*)` / `unit_set_pos(x,y,z)` | position via actor when instantiated (0x6AEE40) else raw fields + relink (0x4FEE60) | V |
| 0x4FF850 / 0x4FF870 / 0x50ABD0 / 0x50AC00 | heading get/set (rad) / target-heading set/get (deg) | | V |
| 0x4FEE60 | `unit_relink_block` | recompute block/scene from the position, relink in the block's unit list, re-parent actors, attach-point children placement | V |
| 0x505960 / 0x5059B0 / 0x505A00 | `unit_angle_to_unit(recno)` / `..._to_item` / `unit_angle_to_point(p*)` | signed angle (deg, +-180) between the unit's facing and the direction to the target | V |
| 0x505B50 | `unit_pitch_to(recno)` | vertical angle (used by ranged range check) | R |
| 0x522C40 | `unit_try_level_up(silent)` | §8 | V |
| 0x5229A0 | `unit_add_experience(amount, to_team, silent)` | §8 | V |
| 0x523240 | `unit_unlock_upgrades(level)` | | V |
| 0x4949F0 / 0x45B720 / 0x5315A0 / 0x5313D0 / 0x531520 | `unit_apply_upgrade(slot)` / `set_base_max_hp` / `slot_advance` / `slot_unlock(id)` / `upgrade_buy(id)` | | V |
| 0x4A95A0 | `unitlvl_ai_spend_points` | AI auto allocation | V |

### 1.4 Frame step and per-unit phases

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x51E280 | `units_update` | per-frame scheduler over the whole unit table (§3) | V (structure), I (phase numbers) |
| 0x508880 | `unit_compute_lod` | 1..4 from squared distance to the controlled unit / camera | V |
| 0x52C490 | `unit_update(phase)` | `phase` 0..3 (§4) | V |
| 0x49DA30 | `unit_pickup_items` | rank > 3 generals pick up items/coins/potions in reach (§9.6) | V |
| 0x416680 | `unit_player_tick` | main-character-only: mode refresh (ACTIVE/IDLE from enemy within 800), critical-HP flash, footstep effect | V |
| 0x4655A0 | `unit_team_autorecruit` | leader with free slots grabs nearby unassigned (team 9999) same-nation units | V |
| 0x5174E0 | `unit_mount_tick` | rider/horse alignment each tick (copy heading, delayed mount animation) | R |
| 0x454B20 | `unit_link_tick` | per-tick link maintenance: if `link_a` (+0x1F5) names a valid unit: reset the controller (`0x6AD980(ctl,0)`), clear `ctl+0xB8` and `ctl_state+0x298`, adopt the turn value `0x4FF850` as heading/target (`0x4FF870`, `0x50ABD0`); if own action is IDLE (1) and `link_b` (+0x1F9) names a valid unit: `unit_set_action(0x68 CA_IDLE, 0, 1)` (§9.11) | V |
| 0x4F2190 | `unit_turn_step` | turn `heading` towards `target_heading` this tick (§10.2); returns **true while still turning** | V |
| 0x52DAA0 | `unit_move_step` | path following / wait / re-path / animation state machine (§10) | V |
| 0x4F0B50 | `unit_apply_velocity` | speed selection and velocity push (§10.1) | V |
| 0x4F7BF0 | `unit_sidestep_unblock` | random sidestep when the path is blocked (§10.3) | V |
| 0x4FA0F0 | `team_update_formation(force, arg)` | grid formation slot assignment for a leader's team (§11.3) | V |
| 0x49D810 | `unit_ai_think` | AI cadence + dispatch (§12.1) | V |
| 0x528440 | `unit_expire_task` | `deadline != 0 && deadline < now` -> clear task | V |
| 0x4FD630 | `unit_clear_think_flag` | only `*(i16*)(unit+0x247) = 0`. Called by `unit_ai_think` when `think_forced != 0`; there is **no** player-specific think logic in it | V |

### 1.5 Orders (see §5)

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x661340 | `give_order(unit, order, prio, team, p1, p2, p3, p4, ms)` | queue/replace an order, optionally broadcast to the team | V |
| 0x6618F0 | `stop_all_order(unit)` | cancel current order, free the Orders object, reset path | V |
| 0x4BAB50 | `Orders::Orders` | ctor | V |
| 0x4BAC60 | `orders_add(unit, id, prio, p1..p4, deadline, flag, leader)` | merge/insert into the queue | V |
| 0x4BB950 / 0x4BB9D0 / 0x4BAE70 | `order_init` / `order_same_id` / `order_cmp_prio` | | V |
| 0x484570 | `orders_get(i)` | queue element i (1-based) | V |
| 0x4BAEE0 | `orders_remove(i)` | | R |
| 0x4BAF10 | `orders_pop_into_current` | queue[1] -> current slot, remove from queue | V |
| 0x4BAF80 | `orders_update(unit)` | purge expired, promote, dispatch per-order tick | V |
| 0x4BB3E0 | `order_can_interrupt(unit)` | per-order predicate table (0x4C0C40 ... not exported) | I |
| 0x4BB660 | `order_cancel(unit)` | per-order cancel + `id=0` | V |
| 0x4BB8D0 | `order_is_pending(id)` | current or queued | V |
| 0x4BBBC0 / 0x4BBAC0 | `order_from_name` / `combo_name_to_codes` | | V |
| 0x4BBF20 .. 0x4C2680 | per-order tick handlers (table in §5.3) | | V/R |
| 0x4C11F0 ... 0x4C18F0, 0x435280, 0x4378A0, 0x43DA10, 0x42D410, 0x425C50, 0x43FBE0, 0x45C430 | per-order cancel handlers | each is "stop + clear" (calls 0x508B90 / 0x501190) | R |

### 1.6 Tasks and script helpers (FUNC builtins used by the AI)

| address | builtin | effect on unit state | conf |
|---|---|---|---|
| 0x6619D0 | `give_task` | `4E4520(temp)` then `assign_task` 0x527FB0 (priority rule), RETREAT with p3==1 also on the whole team | V |
| 0x527FB0 | `assign_task(unit, task*)` | validity 0x527F10; hooks for ATTACK_UNIT/HELP_TROOP/FLEE/RTS_ATTACK_UNIT (0x4BA1F0/0x4BA170/0x4BA270); clear old (0x528300); write; LARGE_WEAPON_PUSH forces an immediate think | V |
| 0x527F10 | `task_can_replace` | prisoner -> no; no task -> yes; else `cur.prio < new.prio` or expired | V |
| 0x528300 | `stop_curr_task` core | if task LARGE_WEAPON_PUSH and a crew slot is reserved (`+0x380 >= 0`), release it on the weapon (`+0x378/+0x37C[slot]=0`, `+0x384/+0x388/+0x380` reset); zero the task | V |
| 0x652370 | `stop_curr_task(unit)` | wrapper | V |
| 0x6520D0 | `get_curr_task(unit)` | own task id, else the leader's (not RTS_MOVE_TO) | V |
| 0x652190 | `get_curr_task_para(unit, n)` | p1..p3 (own or leader's) | V |
| 0x652280 | `get_curr_task_state(unit)` | `Task.state` (own or leader's) | V |
| 0x651690 | `set_unit_attack_behaviour(unit, name)` | `+0x16B` 1..5 | V |
| 0x6515D0 | `set_unit_mode(unit, "IDLE"/"ATTACK"/"ACTIVE")` | `+0x167` = 3/1/2 | V |
| 0x651550 / 0x651590 | `is_idle_mode` / `is_attack_mode` | `mode > 1` / `mode == 1` | V |
| 0x6512D0 | `is_attacked(unit)` | action in reaction set {0x23..0x2A, 0x40..0x42} | V |
| 0x651430 | `will_be_attacked(a, b)` | b's action in {1C,1E..21,53} and distance in (`DAT_009CFC0C`, `DAT_009CFBA8`) | V |
| 0x6517C0 | `search_for_unit(unit, type, range, a, b, c)` | wrapper of 0x54B480 (§11.1) | V |
| 0x54B480 | `unit_search(type, range, use_leader, place, max_xy, max_z)` | target acquisition (§11.1) | V |
| 0x6558D0 | `set_unit_target(unit, recno)` | writes `+0x2D7` | V |
| 0x65B280 | `get_unit_range(unit, which)` | table by UnitClass (§11.2) | V |
| 0x652EE0 / 0x653000 | `get_rotation(unit, unit)` / `(unit, item)` | `ftol(angle_deg)` | V |
| 0x653150 / 0x653060 / 0x65B800 | `get_unit_dist` / `get_item_dist` / `get_point_dist` | `ftol(length)` of the 3D vector (unit +0xE / item +0x56 / x,y,z args) | V |
| 0x653C60 | `is_died(unit)` | `action == 0x2A` (DIE) — **not** `is_dead` | V |
| 0x65C8D0 | `is_dead_unit(unit)` | valid and `hp<1 && lives==0` | V |
| 0x65B6A0 | `is_item_box(item)` | `item.def.+0x4E == 10` | V |
| 0x65C050 | `change_animation_special(unit, "DEFAULT"..)` | `+0x21D` = SPECIAL enum | V |
| 0x656BB0 | `siege_slots_manned(unit, "M"/"F")` | true if every attach point whose usage bit matches ('M'=1,'F'=2) holds a living crew member linked to this weapon (clears stale entries) | V |
| 0x656AF0 / 0x656B50 | crew counters (unexported semantics) | | I |
| 0x6511F0 | `combo_code(name, idx)` | `4BBAC0` splits a combo name into three 4-char codes; returns code idx (1..3) | V |
| 0x651010 | `random_number(n)` | `0x590540(n)` | V |
| 0x652AA0 / 0x652820 / 0x652B00 / 0x652C10 | `can_attack_down` / `is_facing_knock_down_enemy` / `can_rush_attack` / `can_energy_attack` | §9.4 | V |

### 1.7 Combat

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x501190 | `unit_set_action(action, combo_char, flag)` | the action state machine entry: validation/translation, effects, timed hit events (§9.1) | V |
| 0x504010 | `unit_select_anim_entry` | find the animgrp entry for (position, action, item-using, special, combo) | V |
| 0x50DB80 | `unit_anim_start_sounds(entry, footstep_flag)` | **not** the animation start: called by `unit_set_action` after the entry is chosen; it only schedules the entry's two sound slots as timed type-0x800 events (`event_post` 0x4EEF00, kind 4, at `now + delay`). Slot 1 = sound id `entry+0x76`, delay `entry+0x83`; slot 2 = id `entry+0x87`, delay `entry+0x94`. An id that `0x4D14C0` flags as a "footstep family" is resolved to a terrain variant (water if the point is below the water plane `0x469D10`, otherwise "GRASS"/"GROUND" from the tile name via 0x4D12F0) and rate-limited by the sound record's min-gap (`rec+0x81`) against the per-unit last-played stamps `+0x3A0`, `+0x3AC`, `+0x3B0`; ids with a `rec+0x8A` "player only" flag play only for the controlled unit; multi-variant ids add `recno % rec+0x68`. For action 0x33/0x38 (SHTFIRE/...) the sound plays only if the held weapon class is 4/5/6/9/10/12 (slot 1) or the first held item is class 0xB/0xC. See §9.11 | R |
| 0x4FF9F0 | `unit_start_anim(entry, start_ms, loop)` | per-action loop/stagger selection | V |
| 0x508B90 | `unit_stop(arg0, arg1)` | go to the idle action for the current state (IDLE/ACTIVE/ATTACK stance, prisoner, mounted, crew) | V |
| 0x508AB0 | `unit_is_stationary` | action in {1..6,8,0x43,0x59,0x5A} | V |
| 0x510F80 | `unit_is_attacking` | action in {8,0x19,0x1C..0x22,0x46,0x49,0x53} | V |
| 0x500D40 | `unit_can_do(action)` | animation entry exists for 7/0x1E/0x22/0x53 (+ upgrade/AI permission) | V |
| 0x500BA0 / 0x416DC0 / 0x416ED0 | `anim_has_entry_check` / `anim_find_by_action` / `anim_entry_enabled` | | R |
| 0x4EEF00 / 0x47E200 / 0x4EFF60 | `event_post(...)` / `Event::Event` / `Event::clear` | timed hit event record (0x43 bytes) | V |
| 0x4EF090 | `events_update` | trigger system (battle family) | R |
| 0x75D540 | `unit_on_hit_event(event)` | hit/arrow/throw reaction dispatcher (§9.3) | V |
| 0x5048B0 / 0x5051E0 | `unit_melee_hit(attacker)` / `unit_projectile_hit(attacker, kind, flag)` | validity + block + facing code, then damage | V |
| 0x509950 | `unit_take_melee_damage(attacker, facing_code)` (this = victim) | melee damage amount, applies it via `unit_set_hp` (0x4FE510), returns the float damage; §9.10.1 | V |
| 0x509E90 | `unit_take_projectile_damage(attacker, kind, facing_code)` (this = victim) | arrow/stone/bomb damage; §9.10.2 | V |
| 0x5104F0 / 0x510BF0 | `unit_weapon_base_melee` / `unit_weapon_base_ranged` | attacker's base attack value; §9.10.3 | V |
| 0x510750 / 0x510E40 / 0x510E80 / 0x510F10 | `unit_attack_type_mult` / `unit_ranged_skill_mult` / `unit_melee_ignore_factor` / `unit_ranged_ignore_factor` | multipliers; §9.10.3 | V |
| 0x5237E0 / 0x523860 / 0x5238C0 | `unit_total_str` / `unit_total_dex` / `unit_defense_total` | stat + equipment totals (0x497E10 sums); §7 | R |
| 0x4FE8C0 | `unit_set_morale(v)` | morale setter with leader-equipment floor; §9.10.5 | V |
| 0x509D90 | `unit_heal_from_anim` | HP delta from the animation entry | R |
| 0x509000 | `unit_knock_down` | random KNOCKDN/2/3, `attack_chance <<= 1` | V |
| 0x4FD980 | `unit_try_auto_potion(force)` | below `INDANGER` (20 %) HP: use the best potion (item type 0x15 red, then 0x19 all potion) from the inventory pockets; spiritual.txt effect; returns 1 = in danger, nothing found (§9.8) | V |
| 0x50B4F0 / 0x50BCD0 | `unit_use_health_item` / `..._energy_item` | heal self or team (§9.7) | V |
| 0x7C6240 | `unit_on_death(killer)` | same body as 0x64F320 as exported in unit3: kill rewards (EP gain, kill counters, experience factor, morale), §9.12 | R |
| 0x64F320 | `unit_on_death` | death bookkeeping (§9.8) | V |

### 1.8 AI (native, per class — details in [UnitAi.md](../structs/UnitAi.md))

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x531F90 | `ai_cls1_light_infantry` | class 1 dispatcher | V |
| 0x5322C0 | `ai_cls2_light_general` | class 2 | V |
| 0x532520 | `ai_cls3_special_general` | class 3 | V |
| 0x532780 | `ai_cls4_heavy_infantry` | class 4 | V |
| 0x532A00 | `ai_cls5_heavy_general` | class 5 | V |
| 0x532C60 | `ai_cls6_archer` | class 6 | V |
| 0x532EF0 | `ai_cls7_archer_general` | class 7 | V |
| 0x533170 | `ai_cls8_cannon` | class 8 | V |
| 0x533370 | `ai_cls9_autobow` | class 9 | V |
| 0x533440 | `ai_cls10_batterram` | class 10 | V |
| 0x5335E0 | `ai_cls11_camp_horse` | class 11 | V |
| 0x5336E0 | `ai_cls12_horse` | class 12 | V |
| 0x533810 | `ai_cls13_citizen` | class 13 | V |
| 0x533920 | `ai_cls14_trebuchet` | class 14 | V |
| 0x533B10 | `ai_cls15_cart` | class 15 (random wander) | V |
| 0x535C20 / 0x538340 / 0x53ABD0 / 0x53CB10 / 0x53F670 / 0x541270 / 0x5432A0 / 0x544C40 / 0x544E00 / 0x545380 / 0x545D00 | `ai_clsN_decide_attack` | the per-class attack decision (also reached via 0x546970) | V |
| 0x534090 .. 0x545B80 | per-class, per-task state functions | table in UnitAi.md | R |
| 0x545D50 / 0x545EA0 / 0x546050 | `ai_task_retreat` / `ai_task_rts_move_to` / `ai_task_approach_place` | shared by all combat classes | V |
| 0x545590 / 0x545900 / 0x545A60 | `ai_citizen_wander` / `ai_citizen_follow` / `ai_citizen_special1` | | R |

### 1.9 Teams, links, controllable switching

| address | proposed name | purpose | conf |
|---|---|---|---|
| 0x438F80 | `unit_join_team(leader)` | max 20; copies `team_id`; registers rank counter; level sync `0x523190` | V |
| 0x42FCB0 | `unit_leave_team` | `0x5288F0` removal, clear leader/team_id/`+0x2F`, nation re-registration | V |
| 0x42DD70 | `team_disband` | each member leaves with `team_flag = 1`, frees the Team | V |
| 0x5288F0 | `team_remove_member(recno)` | | V |
| 0x482930 | `horse_set_rider(rider)` | mount/dismount a horse: `+0x1E6`, horse inherits the rider's `ai_flag`; dismount -> `ai_flag = 1` | V |
| 0x4FC220 | `unit_set_mount(horse)` | rider side: writes `Unit+0x1E2` (mount recno); returns 0 = same unit / refused, 1 = done, 2 = recno invalid (`0x428B70`); `horse == 0` dismounts. Mount/dismount action ids 0x4F/0x50/0x51/0x52 (§9.11) | V |
| 0x51A8A0 / 0x51A250 | `unit_can_link_a(self_recno)` / `unit_set_link_a(partner)` | cart/escort link: writes `Unit+0x1F5`; returns 0 refused, 1 done (also when already linked to that partner), 2 recno invalid; `partner == 0` releases (§9.11) | V |
| 0x432300 | `player_switch_unit(n)` | controllable-unit switching (§11.4); builtin `switch_controllable_unit` -> 0x65A2F0 | V |
| 0x4C09A0 | `order_recruit_troop` | spawn up to 20 free infantry units of the nation's race into the team | V |

## 2. Unit init and destroy

### 2.1 `unit_init(type_id)` 0x4F8550 (field effects)

Sets `+0x3B = type`; `+0x299 = AI Script* of uscr record UnitRes.+0x125` (0 if none); action state `+0x213=0, +0x215=1, +0x21B=1, +0x21D=0`; combo `+0x227 = +0x22D = 0xFF`, `+0x233 = +0x234 = 0`, `+0x235 = 0`,
`+0x355/+0x356 = 0`, `+0x351 = 0`, `+0x3E6 = +0x3EA = 0`, `+0x3FA = INIT_W_C`, `+0x3F6 = +0x3FA`, `+0x3E5 = 1`; copies `Name` -> `+0x3D`, `ShortName` -> `+0x7F`, `Title` -> `+0xC1`; `+0x104 = 0`;
HP: `base_max_hp = UnitRes.HP`, `hp = base_max_hp` (via setter), `hp_mark = hp`; EP likewise; lives: `+0x11C = +0x120 = UnitRes.Life`; stats (strength, dex, def, leadership, 11 skills) via setters;
`+0x15D..+0x165`; **equipment object**: `new 0x1E`, zeroed; weapons equipped from UnitRes (`0x4970C0` x2, `0x497270`, `0x4973A0`); `active_set = 1` if PWeapon else 2 if PRangeWeapon;
if `Ranking > 2` the three inventory arrays are created (0x495B40);
**initial combo prefix**: `n = rand_range(0,3)` (0x590690); `+0x234 = n`; `+0x22D[0..n-1] = 'A'`;
**drop roll**: for `Ranking < 4` and `UnitRes.DropItemList != 0`: percentage by class and rank: `Ranking < 3`: LIGHT_INF `LINF_DI` 10, HEAVY_INF `HINF_DI` 12, POLEMAN `POLE_DI` 10, MACEMAN `MACE_DI` 8, ARCHER `ARC_DI` 8, CROSSBOW `CRO_DI` 8;
`Ranking == 3` (captain): `LCAP_DI/HCAP_DI/PCAP_DI/MCAP_DI/ACAP_DI/CCAP_DI` (all 100); `Ranking >= 4` always; other classes never. `hit = (rand(100) <= pct)` (**inclusive**, so pct+1 percent).
On hit the item type/level come from the list: `0x464030` = weighted pick: `r = rand(total_weight)+1`; walk the list from the **last** entry to the first subtracting each weight until `r < 1` (entry stride 0x2B: `u16 item, i16 level, i32 weight`);
`+0x124 = Level`, `+0x126 = Level < 2 ? 0 : levelres_find(Level-1).exp`; `+0x12E = UnitRes.+0x19B`; `+0x167 = Behavior (1..4)`; `+0x141 = RegenHp`, `+0x14D = RegenEp`, `+0x1CB = Falling`;
`+0x23F = 0`; `+0x365.. = zero vector`; crew arrays `+0x378/+0x37C` (`AttachPointCount*4` each, zeroed) with `+0x38C = 1`; `+0x380 = -1`; **unlock level-0 upgrades** `0x523240(0)` and register all slots (`0x531520`);
`+0x16B = 1` (NEAREST); `+0x2DF = 0x511E70()` base aggressiveness; `+0x3C0 = 1`; `+0x3C1 = VISION_R`; finally `0x4E4520(0,0,1,0,0,0)` on the task area.
The unit is **not** placed in the world, has no actor, no team and no orders; `+0x335` (world) is set by the placement code (`0x4FED00(world)`).

### 2.2 `unit_destroy` 0x461F10

In order: leave team (`0x42FCB0` if `+0x37`), disband if leader (`0x42DD70`), nation unregister (`0x47B600(nation)`, `0x4B5CC0`), `0x422F10` (path reset), free `Orders`, release the three effect/object slots, release both hand items (`0x50A480(0,..)` then delete the item if not owned),
free crew arrays, dismount (`0x482930(0)` on the mount, `0x4FC220(0)` on the rider), free the cart links (`0x51A8A0/0x51A250(0)`), free shadow/attach/armour actors, free the collision handle (`0x527690`), clear `+0x2ED`.

## 3. Per-frame scheduler `units_update` 0x51E280

Gate: runs only while `DAT_00A06BF4 != 0` (world running) and `DAT_00A596AB == 0` (`set_stop_unit_process_flag`). `U` = every unit with `+0x335 == DAT_00A45F3E`.

**A. Cut scene active (`0x4301C0`)** — loop recno `n..1`: units with `+0x2E == 0` (not flagged `can_process_in_cut_scene`) that are not prisoners: `0x508B90(0,1)` (force idle stance) and **2 calls** of `unit_update`;
units with `+0x2E != 0`: `lod := 1` and **4 calls** (all phases 0..3, in this order). Nothing else runs.

**B. Normal play**
1. *Classification pass* over all units (ascending recno): `lod = unit_compute_lod()` (0x508880: 4 if the controlled unit is gone; 1 if it is the unit itself; else squared XY distance to the main character — or to the camera target when `DAT_00A5B9F8 == 4` —
   compared with three thresholds `_DAT_009D751C = 4.0e6`, `_DAT_009D7518 = 1.6e7`, `_DAT_009D7514 = 6.4e7` (squared world units, i.e. radii 2000 / 4000 / 8000) -> lod 1, 2, 3, 4 (disassembly).
   Then: *keep-alive* units (`ai_flag == 0` **or** `hp > 0` **or** `lives != 0` **or** `death_state == -2` **or** an actor still fading out: `actor.fade_rate != 0` and `now - actor.fade_start <= 1000/|rate|`)
   with `lod < 2` get **2 `unit_update` calls**. Every other unit — an AI unit (`ai_flag != 0`) with `hp < 1`, `lives == 0`, `death_state != -2` and no running fade-out (corpse fade finished) — is **destroyed**: removed from its leader's team (`0x5288F0`), then `0x51E1F0(recno)` frees it.
   (So dead player-slot units, `ai_flag == 0`, are never removed by this pass.)
2. Twelve *time-sliced round-robin passes* (cursor array `i16[12]` at `table+0x40..0x56`, budget array `i16[12]` at `+0x58..+0x6E`): for each pass the cursor continues where it left off, visits units cyclically (wrapping at `count`),
   calls `unit_update` for units that match the pass filter until `budget+1` matching units were processed or the cursor returned to its start. Filters (L = lod, R = `rank`):
   | pass | filter | calls per unit |
   |---|---|---|
   | 1 | `L == 2` | 2 |
   | 2 | `L >= 3` | 2 |
   | 3 | `L < 2 && R > 3` | 1 |
   | 4 | `L < 2 && R < 4` | 1 |
   | 5 | `L == 2` | 1 |
   | 6 | `L == 3` | 1 |
   | 7 | `L >= 4` | 1 |
   | 8 | `L < 2 && R > 3` | 1 |
   | 9 | `L < 2` | 1 |
   | 10 | `L == 2` | 1 |
   | 11 | `L == 3` | 1 |
   | 12 | `L >= 4` | 1 |
   **Correction :** the `0x45E750` / `0x45E7F0` helpers are `rdtsc` **profiling counters only** (0x45E750 stores `rdtsc` into an 8-byte output; 0x45E7F0 is an empty function in this build). Their results go into the 16-bit words at `+0x40..+0x6C` of a stats object (ECX = 0xA485A8) and never feed back into the scheduling. An earlier reading that the budget is replaced by `(int)elapsed` was a decompiler artefact; the pass budgets are not machine-speed dependent as far as the export shows. Initial budget values come from the saved `table` object (not exported).
3. **Phase argument (disassembly of 0x51E280):** the stack argument of `unit_update` (0x52C490) is the phase 0..3 of §4. The *near* group (lod 1, after the unit's `+0x22` is set to 1) runs phases **0, 1, 2, 3 in sequence**; the *middle* groups (lod 2..3, rank >= 4) run phases **0 then 1**, guarded by `+0x22 <= 1 / 2 / 3` and `+0x104 >= 4`; the *far* groups run **only phase 2 or only phase 3**, guarded by `+0x22 > 2 / 3` and `+0x104 < 4`. `+0x22` is therefore the unit's LOD level (`lod` above), and a pass entry in the table above means "run those phases of the group" rather than "N calls of one phase".

## 4. `unit_update(phase)` 0x52C490 (verified structure)

Returns 0 immediately if `unit.world != DAT_00A45F3E`; returns 1 otherwise. `phase` selects one of four blocks:

**Phase 0 (pre-step)** — clear `hit_by` (`+0x2E3`, `+0x2E5`); `unit_pickup_items` (0x49DA30); main character: `unit_player_tick` (0x416680); update the two hand-effect objects `+0x32D/+0x331` (0x687A60); `0x518B90` (tiny, no callees);
**prisoners** with an animation group and action != 7 get `0x508B90(0,1)` then action IDLE (1) or, with `prisoner_pose`, IDLE_T (7) if the table has it (`0x500D40`);
a `BATTERING_RAM` (Special 11) whose block has data: stop the particle effect `BAT_TRK` (0x427B50);
**pushers**: a unit whose task is LARGE_WEAPON_PUSH (15) and whose task `p1` (the weapon recno) is a valid unit: if it is farther than `weapon collision size + max(200, ..)` from the weapon (computed from the weapon's sphere/box/cylinder, 200 minimum, `_DAT_009D5334` margin) its collision shape type (`actor+0xB8`) is cleared and `+0x38E = 1` (it may walk through the weapon), otherwise the shape is restored from `UnitRes.CollisionType` via `0x693170`; the animation controller of a unit that is not currently crew is set idle (`0x6AD980(ctl,0)`);
when `+0x38E == 1` and the shape restore succeeds the unit is re-seated on the controller (`0x6AF370`) and `+0x38E = 0`; for RETREAT (task 11) units with `p3 > 0`, `p3 -= dt`.

**Phase 1 (main step)** — (a) `unit_mount_tick` and `unit_link_tick`; (b) level-up flag expiry: `levelup_flag && now - levelup_time > LEVELARR(5000)` -> clear; (c) if an actor exists: `0x7C7AB0`, then `r = 0x4F2190()` (turn step; **true while the unit is still turning**) and, when `r == false || (ai_flag == 0 && no cut scene)`, `unit_move_step` (0x52DAA0), i.e. AI units do not translate while still turning, the player unit always may;
while `r == true` the animation controller's movement vector (controller `+0x142,+0x146,+0x260`) is zeroed; (d) refresh of the attach actor (`0x4956F0`: delete when its animation time exceeded); (e) the "turn-on-spot" flag `+0x202`: re-evaluated each tick (set collision type, `0x693170`), on failure `+0x3FE = 20, +0x400 = 0`;
(f) **EP regeneration**: if `hp == 0 || ep >= max_ep` then `last_ep_regen = now`; else if `now - last_ep_regen >= RECOV_T(10000)`: `last_ep_regen = now`, and unless `DAT_00A5D384`, `ep += RECOV_MP(1)` (setter clamps);
(g) **balance regeneration** (once per 1000 ms, only if `balance < 100`; amount hidden, see Unit.md); (h) **HP regeneration**: if `hp == 0 || hp >= max_hp || death_state == -1` then `last_hp_regen = now`; else if `now - last_hp_regen >= RECOV_HP(10000)`:
`last_hp_regen = now`, unless `DAT_00A5D384` HP is raised through `0x4FE510` by `max(1, defense_total * DEF_HP_R(0.1))` (the FPU expression is only partly visible: `defense_total()` is evaluated, compared with `1.0` after the `0.1` factor and the result passed to the setter — treat as `hp += max(1.0, defense_total*0.1)` truncated, `I`);
(i) `invuln_ms -= dt` while `> 0`; (j) **AI upgrade timer**: if `+0x3BC != 0 && now - +0x3BC >= UPG_TIME(600000)` and the type is not a hero: `unitlvl_ai_spend_points`, `+0x3BC = 0`; (k) `last_update_ms = now`.

**Phase 2 (orders)** — returns 1 immediately if the helper `0x6E7B50` is true (UI/pause state); `0x4655A0` (team auto-recruit, leaders only), leaders: `0x4FA0F0(0,1)` formation update, then `0x4BAF80` orders_update when `orders != 0`;
then the *animation LOD guard* on the animation controller (`actor+0x5C`): for `lod == 1`, an actor with a controller, no mount, no `link_a`, a valid siege `weapon` link absent..., action not 0x51/0x52, `+0x1FD == 0` and `+0x3B8 == 0`: `0x6AD980(ctl, 1)` (full skeletal update on);
for `lod 2..4` units not in any movement/attack/reaction/push/mount action and whose controller reports no pending blend (`ctl+0x246 <= 0 && ctl+0x24E == 0`): `0x6AD980(ctl, 0)` (skeletal update off).

**Phase 3 (AI)** — `0x6E7B50` pause check, then `unit_ai_think` (0x49D810).

(The mapping of scheduler groups to phases is given in §3 step 3.)

## 5. Orders

### 5.1 Insertion — `give_order` 0x661340 (V)

Parameters: `(unit, order_id, priority:i8, team_flag:u8, p1, p2, p3, p4, duration_ms)`; `deadline = now + duration_ms` (so `0` expires next tick). Returns 0 for an invalid unit else 1.
1. Create the `Orders` object if missing. Special: for FOLLOW_FORMATION (15) with `team_flag`, if the leader's team already has a member with a pending/current FOLLOW_FORMATION order the flag is cleared (no duplicate broadcast).
2. `orders_add(unit, id, prio, p1..p4, deadline, flag 0, leader 0)` (0x4BAC60):
   * same id as the **current** order: BLOCK(4) only refreshes the deadline (`+0x3A`); APPROACH_ENEMY(12) with different `p1..p3` cancels the old one (0x4378A0), overwrites `p1..p3,p4` and keeps id 12; other ids: nothing (the new order is dropped, the old keeps running);
   * else scan the **queue** from last to first: a queued order with the same id is overwritten in place (p1..p4, deadline) and the call ends; otherwise the insertion index is the lowest index whose element has strictly lower priority than the new order (queue is sorted by descending priority, FIFO among equals);
3. If `team_flag != 0` or `order == APPROACH_ENEMY (12)` the order is **broadcast** to the unit's team (leader = `0x4216D0`): each living member `m != unit` receives it with `priority-1`, `flag = 1`, `leader = unit.recno`, if
   (`m.task` and the leader's `task` are not both set and different) and, for FOLLOW_FORMATION, `m` is not a siege weapon, and for APPROACH_ENEMY `m` has no target (`+0x2D7 == 0`) and is not already on order 12. The leader itself also receives it when it is not the originator (except FOLLOW_FORMATION).
   Members create their own `Orders` object when needed.

### 5.2 Per-tick: `orders_update(unit)` 0x4BAF80 (V)

1. Delete every queued order whose `deadline < now`.
2. If the unit's action is one of {0x40,0x41,0x42 KNOCKDN*, 0x3C STANDUP, 0x29 DNATKED, 0x4E H_DROP}: **do nothing else** (orders neither start nor run while down).
3. If no current order: pop `queue[1]` (0x4BAF10); stop if the queue is empty. Else, if the queue is non-empty and `current.priority < queue[1].priority` and the current order is interruptible (`0x4BB3E0`): cancel the current one (`0x4BB660`: per-order cancel + `id=0`) and pop.
4. Dispatch on the current id to the per-order tick (table below). **A tick handler ends its order by writing `id = 0`** (directly or through its cancel function); WAIT on `give_order` (BUILTINS §5.2) completes exactly when the order is neither current nor queued.

### 5.3 Order table (p1..p4 as consumed; completion = when the handler clears the id)

Common abort: for almost every order the handler clears the id immediately when the unit's action is KNOCKDN (0x40..0x42), STANDUP (0x3C), DNATKED (0x29) or H_DROP (0x4E).

| id | name | tick handler | parameters | behaviour / completion |
|---|---|---|---|---|
| 1 | COMBO_ATTACK | 0x4BBF20 | `p1,p2,p3` = up to three packed 4-char words of the combo string (`combo_code`/0x4BBAC0 splits `"ABAB..."` into 4 chars per word, big-endian; the AI passes one word); chars read via 0x4BBA00(word, charIdx) | Unmounted: ends immediately for ENGATK/DOWNATK/knock-down actions. If the action is ATTACK (0x1C): compares the chars already executed (`combo_buf[0..combo_len)`) with the planned prefix; if equal and a next planned char exists, the previous attack's animation time has passed `animscr.StartTime` of the running entry (`now - actor.start_time >= StartTime`) and the table has an ATTACK entry for `prefix+char` (`0x416E30`), starts `ATTACK (0x1C)` with that char (`combo_len++`). Ends (buffers cleared, `unit_stop`) when no animation time remains (`0x5045F0 == 0`) after the last step. Reaction actions 0x23..0x28 / DIE: wait for the deadline or end. Otherwise, if stationary and `anim_position != 1`: clear combo buffers and start the first char. Mounted units use `H_ATTACK` (0x49) with `p3` = remaining swings (`p3--` per swing, `AAA` special) |
| 2 | SHOOT | 0x4BD090 | `p1` = 1 unit / 2 item target, `p2` = target recno, `p3` = state (1 once started) | Already started (`p3 == 1`): ends once the unit is stationary again. Else: if not stationary: ends on death/DIE or when the deadline passed and the action is not one of the shoot states {0x30,0x32,0x33,0x36,0x38,0x39}. If stationary: after the deadline -> ends; else stores the target in `+0x2DB` (unit) / `+0x2DD` (item), requires the **angle** to the target (`0x505960`/`0x5059B0`, deg) inside `[DAT_009D0750, DAT_009CFC20]` (the unit must have finished turning), then clears the combo buffers, starts `SHTLOAD (0x30)`, resets the path wait counter and sets `p3 = 1` |
| 3 | JUMP | 0x4BF510 | — | starts action `0x1A UP`; while `anim_position == 1` ends at the deadline (stand-up from lying) |
| 4 | BLOCK | 0x4BF240 | `p1` = attacker recno (0 = none) | starts GUARD1 (0x2B) and holds until the deadline; releases when the attacker is gone / no longer will-be-attacked; on release it re-runs the unit's attack decision (`0x546970`) |
| 5 | MOVE | 0x4BF5F0 | `p1`: 0/1 -> WALK(10), 2 -> RUN(11) | straight movement mode: while moving keeps the action, ends by deadline |
| 6 | SHIFT | 0x4BF780 | `p1`: 1 MOVE_L(0xE), 2 MOVE_R(0xF), 3 MOVE_B(0x10), 4 JUMP_B(0x15) | strafe until the deadline (also ends when the path-length query reports 0) |
| 7 | MOVE_TO_TARGET | 0x4BFA10 | `p1,p2,p3` = x,y,z | request path (0x505DD0) towards the point; arrival radius `REACH_DT` (15; **30 when mounted, siege, or `lod > 1`**) squared; ends on arrival or deadline |
| 8 | MOVE_TO_RELATIVE_TARGET | 0x4BFA10 | `p1..p3` offsets added to the unit's current position at queue time of the *tick* (note: re-evaluated each tick) | as 7 |
| 9 | MOVE_ON_TRACK | 0x4BFD70 | `p1` = track recno, `p2` = 0 start/1 end, `p3` flip-at-end | follows a `trk` path (0x453CC0/0x49AB40); ends when the track has < 2 points or the deadline passed |
| 10 | TURN | 0x49CEF0 | `p1` = angle in degrees relative to the current facing (the AI passes `get_rotation(self, target)`; scripts pass an expression) | turns on the spot: each tick computes the remaining angle (integer degrees via 0x590A20); if it is outside `[2, 358]` the order ends, else `target_heading` is (re)set and the turn step rotates the unit. Ends at once while the unit is attacking; siege weapons (class 8, except crossbow/trebuchet specials) only when manned |
| 11 | IDLE | 0x4C25A0 | — | calls `unit_stop` when not stationary; ends at the deadline or once stationary |
| 12 | APPROACH_ENEMY | 0x4BD420 | `p1` = target recno, `p2` = min distance, `p3` = extra random distance, `p4` = chosen-standoff cached flag | while target valid and alive: if squared distance in `(p2², (p2+p3)²]` -> ends; else picks (once, cached in `+0x3D9..`) a standoff point at distance `p2 + rand(p3)` and bearing `-heading_to_target + rand(270) - 135` degrees from the target, paths to it with timeout 500 ms (1000 if rank > 2 and far); ends if within REACH_DT of the point, when the target dies or at the deadline |
| 13 | APPROACH_ITEM | 0x4BDBC0 | `p1` = item recno, `p2,p3` distances | same pattern for items |
| 14 | APPROACH_PLACE | 0x4BDF90 | `p1` = place recno, `p2` = area id inside the place's map (region record via 0x43C450) or a place slot (when `0x44CF80(p2) != 0`), `p3` | waits for the deadline guard, then for non-slot areas: if already inside the place's region (`0x60D5F0(pos)`) the order ends (`0x43DA10`); else walks to a random point of the region (`region.x0 + rand(w)`, `region.y0 + rand(h)`); for slots it takes the slot position (`0x4C2470`) and for leaders reserves it (`0x4C2540`) and forces a formation refresh (`0x4FA0F0(1,1)`); ends within REACH_DT of the destination; path timeouts 500/800/1000/1500/2000 ms by distance² steps (`_DAT_009D5668/565C/5664/5660`) |
| 15 | FOLLOW_FORMATION | 0x4BE770 | `p3 == 1` : `p1,p2` fixed point (cached in the order); else the unit's formation slot (`0x448870`) | paths to the slot (timeout 500/1000 ms), tolerant radius REACH_DT (30 for lod>1/mounted/siege); ends at the slot, if its target moved too far (`LD_FM_DT`), or at the deadline; on arrival rotates to the leader's heading (`+0x44`) |
| 16 | FOLLOW_UNIT | 0x4BED40 | `p1` = unit, `p2` = bearing offset (deg), `p3` = distance | tracks a point at `distance` on the bearing relative to the followed unit's heading |
| 17 | GATE_OPEN | 0x4A71F0 | `p1` = gate | |
| 18 | RECRUIT_TROOP | 0x4C09A0 | — | only for leaders: while `team.count < 20` and the nation can afford (`0x4B5BF0`) creates `0x51DFC0(type)` units of the highest recruit-eligible infantry type of the leader's race (`kingdom == race`, `Ranking 0`), sets `ai_flag = 1`, joins the team, places them behind the leader on the formation offsets; then ends |
| 19 | DOWN_ATTACK | 0x4BC8A0 | — | needs an enemy knocked down within range: starts DOWNATK (0x22); clears the id if the target is already down-attacked or the animation is missing |
| 20 | ENERGY_ATTACK | 0x4BCA60 | — | if the unit has the ENGATK animation (0x1E), `can_energy_attack` holds and cost is affordable: starts 0x1E and charges **EP `-= ftol(max_ep * stat15%)`, HP `-= ftol(max_hp * ...)`** (not for the main character's friends: only when the actor is not the player's ally) and `last_ep_regen = now` |
| 21 | RUSH_ATTACK | 0x4BCDB0 | — | needs animation 0x53 and EP `>= entry[6]` (ENERGY_PT, scaled by equipment stat 0x15 / 100): `EP -= cost`, starts RUSH (0x53) |
| 22 | SPECIAL_MOVEMENT | 0x4C2680 | `p1` = movement kind, `p3` = ... | jump/roll style action (0x15..0x17) |

Priority convention used by the native AI: 1 = routine movement/shift, 2 = attacks and routed combos, 3/4 = TURN towards the target, `-2`/`-3` = FOLLOW_FORMATION/APPROACH_PLACE (background), 7 = camp-horse SPECIAL_MOVEMENT, 10 = scripted siege movement.

Order names are resolved by the name parser **0x4BBBC0** (case-insensitive strcmp ladder; unknown -> 0), confirmed: 1 COMBO_ATTACK, 2 SHOOT, 3 JUMP (literal at 0x9CF03C), 4 BLOCK, 5 MOVE, 6 SHIFT, 7 MOVE_TO_TARGET, 8 MOVE_TO_RELATIVE_TARGET, 9 MOVE_ON_TRACK, 10 TURN, 11 IDLE, 12 APPROACH_ENEMY, 13 APPROACH_ITEM, 14 APPROACH_PLACE, 15 FOLLOW_FORMATION, 16 FOLLOW_UNIT, 17 GATE_OPEN, 18 RECRUIT_TROOP, 19 DOWN_ATTACK, 20 ENERGY_ATTACK, 21 RUSH_ATTACK, 22 SPECIAL_MOVEMENT.

Task names are resolved by the parser **0x4E4610** (strcmp ladder via the case-insensitive compare 0x7C07B0): 1 ATTACK_PLACE, 2 ATTACK_UNIT, 3 RECRUIT_TROOP, 4 HELP_TROOP, 5 FLEE, 6 DEFEND_PLACE, 7 IDLE, 8 TOWER_ATTACK, 9 SCOUT, 10 FOLLOW_UNIT, 11 RETREAT, 12 RTS_MOVE_TO, 13 RTS_ATTACK_UNIT, 14 APPROACH_PLACE, 15 LARGE_WEAPON_PUSH, 17 RTS_ATTACK_ITEM, 18 DEFEND_UNIT, `SPECIAL<n>` -> 100 + n (suffix converted with atoi, 0x7B15CB). Ids 16 and 19 have no name (internal only); an unknown name yields 0.

### 5.3a Interruptibility — `order_can_interrupt` (0x4BB3E0 dispatch; V, all 22 predicates read)

`orders_update` (§5.2 step 3) cancels the current order in favour of a higher-priority queued one only if the predicate of the *current* order id returns true. The dispatcher maps id -> predicate; id 0 (no order) -> true. "Action" is `unit+0x215`; "anim_time" is `now`-relative state `DAT_00A500BA` = the frame clock in ms.

| id | order | predicate | returns true (interruptible) when |
|---|---|---|---|
| 1 | COMBO_ATTACK | 0x4C0C40 | action is not ATTACK (0x1C) and not H_ATTACK (0x49) |
| 2 | SHOOT | 0x4C0D90 | action is none of {0x30, 0x32, 0x33, 0x34, 0x36, 0x38, 0x39} (the shoot ladder SHTLOAD..SHTFIRE) |
| 3 | JUMP | 0x4C0F30 | `unit+0x213 != 1` (not in the airborne state), or `0x5045F0()` (remaining animation time) == 0.0 |
| 4 | BLOCK | 0x4C0EE0 | action is not 0x2B (GUARD1) and not 0x2D |
| 5 | MOVE | 0x4C0F90 | action is none of {9, 10, 11} (turn-walk, WALK, RUN) |
| 6 | SHIFT | 0x4C0FF0 | always true |
| 7, 8 | MOVE_TO_TARGET / MOVE_TO_RELATIVE_TARGET | 0x425B60 | always true |
| 9 | MOVE_ON_TRACK | 0x42F5B0 | always true |
| 10 | TURN | 0x433A90 | `unit+0x3A` (order deadline) `< now` (i.e. only once the order has expired) |
| 11 | IDLE | 0x4C1090 | deadline `< now`, or remaining animation time (0x5045F0) == 0.0 |
| 12 | APPROACH_ENEMY | 0x4C0E40 | always true |
| 13 | APPROACH_ITEM | 0x4C0E60 | always true |
| 14 | APPROACH_PLACE | 0x4C0E80 | always true |
| 15 | FOLLOW_FORMATION | 0x429B70 | always true |
| 16 | FOLLOW_UNIT | 0x42A1A0 | always true |
| 17 | GATE_OPEN | 0x4C10F0 | deadline `< now`, or remaining animation time == 0.0 (same body as id 11) |
| 18 | RECRUIT_TROOP | 0x4C1150 | same body as id 11 |
| 19 | DOWN_ATTACK | 0x4C0D40 | action is not ATTACK (0x1C) and not H_ATTACK (0x49) (same body as id 1) |
| 20 | ENERGY_ATTACK | 0x4C0C90 | action is none of {0x1E, 0x1F, 0x20, 0x21} (ENGATK stages) |
| 21 | RUSH_ATTACK | 0x43BBC0 | action != 0x53 (RUSH) |
| 22 | SPECIAL_MOVEMENT | 0x4C11B0 | always true (body is `return 1`) |

Note: 0x4C11F0 next to it is a different function, a per-order *cancel* stub (`0x508B90(0,1); *(u32*)(this+0x1D) = 0`). All 22 predicates are now known. Ids 17..21 use the hex slots 0x11..0x15 of the dispatcher (0x4C10F0, 0x4C1150, 0x4C0D40, 0x4C0C90, 0x43BBC0).

### 5.4 Cancel

`order_cancel` (0x4BB660) calls the per-order cancel (all of them end in `unit_stop`/`0x508B90(0,1)` or clear siege state) and writes `id = 0`. `stop_all_order` (0x6618F0): cancel + free `Orders`, set pointer 0, `path_reset` (0x422F10).

## 6. Tasks and the action ladder

Task struct, ids and replacement rule: [Unit.md](../structs/Unit.md) "Task". `give_task(unit, task, prio, p1, p2, p3, ms)`: `deadline = now+ms` (0 if `ms == 0`).
**Think-time expiry**: every AI think starts with `0x528440`, which zeroes the task when its deadline passed. A task is therefore "done" when the AI/script clears it (`stop_curr_task`, handler `stop_curr_task(unit)`) or when it expires.

### 6.1 Action ids (`Unit+0x215`) — animation-table `MOVEMENT` codes (loader 0x415820)

0 DEFAULT, 1 IDLE, 2 IDLE2, 3 IDLE3, 4 IDLE_BAT (battle stance), 5 IDLEUP, 6 IDLEDN, 7 IDLE_T (prisoner/tied), 8 JATTBI, 9 TIPTOE, 0xA WALK, 0xB RUN, 0xC T_LEFT, 0xD T_RIGHT, 0xE MOVE_L, 0xF MOVE_R, 0x10 MOVE_B,
0x11 MOVE_FL, 0x12 MOVE_FR, 0x13 MOVE_BL, 0x14 MOVE_BR, 0x15 JUMP_B, 0x16 JUMP_R, 0x17 JUMP_L, 0x18 DOWN, 0x19 JATTBD, 0x1A UP, 0x1B CHAIN, **0x1C ATTACK**, 0x1D JATTACKB, 0x1E ENGATK, 0x1F ENGATK2, 0x20 ENGATK3, 0x21 ENGATK4,
**0x22 DOWNATK**, **0x23 ATTACKED** (front hit), 0x24 ATTKED2, 0x25 ATTKEDR, 0x26 BATTKED (back), 0x27 LATTKED, 0x28 RATTKED, 0x29 DNATKED (hit while down), **0x2A DIE**, 0x2B GUARD1, 0x2C GUARD2, 0x2D GUARDR (block reaction), 0x2E BACK, 0x2F CLIMB,
0x30 SHTLOAD, 0x31 SHTLOADC, 0x32 SHTLOAD2, 0x33 SHTOUT, 0x34 SHTOUT2, 0x35 SHTOUTC, 0x36 SHTIDLE, 0x37 SHTIDLEC, 0x38 SHTOUTH, 0x39 SHTIDLEH (bow/crossbow/siege shooting states), 0x3A THROW, 0x3B THROWN, 0x3C STANDUP, 0x3D DIZZY, 0x3E STOP, 0x3F STOP_B,
0x40 KNOCKDN, 0x41 KNOCKDN2, 0x42 KNOCKDN3, 0x43 H_IDLE, 0x44 H_WALK, 0x45 H_BACK, 0x46 H_RUN, 0x47 H_STOP_B, 0x48 H_STOP, 0x49 H_ATTACK, 0x4A H_HITED, 0x4B H_BHITED, 0x4C H_LHITED, 0x4D H_RHITED, 0x4E H_DROP (mounted states),
0x4F H_MNT_L, 0x50 H_MNT_R, 0x51 H_DMNT_L, 0x52 H_DMNT_R, 0x53 RUSH, 0x54 PUSH, 0x55 PUSH2, 0x56 PUSH_L, 0x57 PUSH_R, 0x58 BATTER_C, 0x59 C_IDLE_A, 0x5A C_IDLE_B, 0x5B..0x67 C_DN_*/C_UP_*/C_DROP (cannon crew), 0x68 CA_IDLE, 0x69 CA_RUN, 0x6A CA_WALK, `EXTRAn` = 0x6B+n, unknown = -1.
Related enums (same loader family): `ITEM_USING` (0x416910): DEFAULT 0, 1SWORD 1, 2SWORD 2, 2HAMMER 3, 2SPEAR 4, 2STAFF 5, 2FLAIL 6, BOW 7, CROSSBOW 8, SHIELD 9; `SPECIAL` (0x416AA0): DEFAULT 0, TIRED 1, HAPPY 2, POISON 3, CAMP 4, CANNON 5, FAUCONN 6, BOMBARD 7, CROSSBOW 8, SIEGET 9, TREB_L 10, TREB_R 11, TREB_F 12, BATTER 13, BATTER_L 14, BATTER_R 15.
Derived sets used everywhere: *reaction* {0x23..0x28, 0x40..0x42, 0x29, 0x2A} (`is_attacked`), *stationary* {1..6, 8, 0x43, 0x59, 0x5A}, *attacking* {8, 0x19, 0x1C..0x22, 0x46, 0x49, 0x53}, *down* {0x40..0x42, 0x4E, 0x29, 0x3C}.

### 6.2 `unit_set_action` 0x501190 (V, key points)

1. Calls the per-frame housekeeping (0x48B460, 0x459970, 0x518D10). Main character: `0x503FE0` unless action == 0xB (RUN).
2. For hit/knock actions {0x23..0x2A, 0x3D, 0x40..0x42, 0x4A..0x4D}: crew of a siege weapon the unit operates are detached (`+0x378/+0x37C[slot] = 0`, `+0x384 = 0`...) and the path is reset (`0x500D10`).
3. Siege weapons propagate their action to the crew they carry (attach usage bit1 = `F` seats): `Special` 4,5,6,9,10 (cannon, fauconneau, crossbow, bombard, trebuchet): weapon action 0x30 -> F-seat crew 0x31; 0x36/0x39 -> 0x37; 0x33/0x38 -> 0x35; weapon idle actions 1..4 -> every crew member that is moving is stopped (`0x508B90`).
   Battering ram (11): weapon action 0x1C -> F-seat crew get 0x58 (BATTER_C), M-seat crew are stopped if moving; idle 1..4 -> all crew stopped.
4. State filters (verified against the disassembly, 2026-10-22): if the **current** action is DIE (0x2A) and `Unit+0x10C` (i32, writer TODO(re)) `!= 1`, every request is ignored (dead units stay dead; `death_state` is not involved). If the current action is a down action (0x40..0x42, 0x4E) or DNATKED (0x29) and the request is not 0x29: hit-type requests {0x23,0x26,0x27,0x28,0x40,0x41,0x42} (and 0x29 itself in the DNATKED case) are rewritten to DNATKED (0x29); any other request is ignored when `fell_timer` (+0x1D1) > 0, the unit is not the controlled unit (0xA5B9E8) and `ai_flag != 0`; a DIE request is ignored (a downed unit does not start DIE).
5. Variation: a request for ATTACKED (0x23) is changed to ATTKED2 (0x24) when the table has it: cur==0x23 -> 0x24; cur==0x24 -> 0x23; cur==0x25 -> `0x24 - (rand != 0)`; other cur with the entry present -> `0x24 - (rand != 0)` (**the `rand` argument is hidden**).
6. Bookkeeping: `prev_action = action`, `action = new`, `combo_buf[combo_len] = combo_char; if combo_char != 0: combo_len++`; picks the animation entry (`0x504010`), starts it (`0x4FF9F0`) and schedules its two sound events (`0x50DB80`, §1.7); spawns the sound/emitter scripts named in the animscr record and the weapon attach effects.
7. Knock-down actions {0x40,0x41,0x42,0x4E}: `fell_timer = FELL_DUR/5 + rand(?)`; for rank > 3 `invuln_ms = max(invuln_ms, INVC_DUR)`.
8. **Attack events** (only if `0x504010` found an animation entry for the new action — otherwise the function returns before this point): for actions {0x1C, 0x1D, 8, 0x19, 0x1E..0x21, 0x49, 0x53, 0x46, 0x44}: `reach = max(item_def(hand_item[0]).+0x32, item_def(hand_item[1]).+0x32)` (0 for empty hands).
   Two timed events are posted with `0x4EEF00` (event record 0x43 bytes: `mask, class, victim, owner type, owner recno, flag, pos(x,y,z), range, time, ...`): `(mask 1 = TRIGGER_ATTACK, class 1 = unit victims)` and `(mask 1, class 2 = item victims: gates/walls/boxes)`, both with
   `time = now + animscr.StartTime (rec+0x28)`, `pos = unit.pos`, `range = animscr.Range (rec+0x30) + reach`, owner type 1 (unit). Without an animscr record the fallback is `range = reach + 50` and `time = now`; with no actor at all the unit and item events fire at once with `range = reach+50` and a third event (mask 0x200 TRIGGER_INTERACT, range 100.0).
   DOWNATK (0x22): one event `mask 0x400`, class 1, range `animscr.Range` (no reach added) or `reach+50`. THROW (0x3A): mask `0x10`, class 1, target = `+0x2D7`, range 50.0 (or animscr).
   For the mounted attack (0x49) with the mount running, the RUN-action animscr of the horse's entry (action 0x46) provides `StartTime/Range` and the events are posted twice (unit class, item class).
   `animscr` = `animscr_record(animgrp_entry.+0x72)`, see [AnimScript.md](../structs/AnimScript.md). Weapon-attached particle/sound scripts named in the animscr (emitter slots 0/1, SoundScript) are started here as well.

### 6.3 Animation position, instantiation, remaining time (V, 2026-10-23)

* **`set_anim_position(v)` 0x500D10** writes the i16 `Unit+0x213` (`POSITION` column: `STAND` 0, `JUMP` 1). Its only
  callers are inside `unit_set_action` 0x501190 (disassembly 0x50135B, 0x501FF5, 0x50202C):
  hit / death / dizzy requests {0x28, 0x2A, 0x3D, 0x40..0x42, 0x4A..0x4D} -> `0` (first, with the siege-crew detach of
  §6.2 step 2); after the bookkeeping and **before** the row is selected: `UP 0x1A` with position 0 -> `1`; `DOWN 0x18`
  with position 0 or 1 -> `0`, and DOWN becomes `JATTBD 0x19` when the previous action was `JATTACKB 0x1D` or `JATTBI 8`;
  `IDLE 1` after `JATTACKB 0x1D` becomes `JATTBI 8`.
* **`unit_instantiate` 0x442820** (callers 0x4FEE60 relink after a position change, 0x4329E0): once per unit
  (`+0x2ED`, `+0x2EE` set to 1) it spawns the actor (0x52B970) and refreshes the hand items. The actor spawn 0x52B970
  starts the current action's row when no animation runs yet (`+0x0A < 1` or `+0x21F == 0`): `+0x0A = +0x0E = 0`,
  `0x4FF9F0(0x504010(now, 0), now, 0)`; otherwise it resumes the stored clip at `now - +0x21F`.
* **`unit_start_anim(entry, start, blend)` 0x4FF9F0**: the third argument is the blend parameter (`-1` -> 0); looping
  and the idle stagger follow the action (actions 1/4/7 loop, `rand(5)*200` ms stagger except for the main character).
* **`unit_anim_remaining_ms` 0x5045F0**: 0 without an actor or a row (`+0x0A == 0`); `elapsed = now - actor+0x4C`,
  `elapsed %= clip_length` when the clip loops (`actor+0x50 != 0`); returns `length - elapsed`, or 0 when
  `elapsed >= length`. A looping clip therefore never reports 0.
* **`combo_reset` 0x50D0D0**: clears the six combo letters `+0x227..+0x22C`, `combo_len` `+0x233` unless it is 0xFF,
  and the i16 `+0x235`. `combo_len = 0xFF` marks a closed combo: the next letter `set_action` writes lands at
  `+0x226` (index -1) and the count restarts at 0; the orders that start a combo (0x4BBF20, 0x4BD090, ...) write
  `+0x233 = 0` themselves.
* **`unit_stop(blend, ...)` 0x508B90** (V, 2026-10-23): does nothing while the unit holds a stationary action
  (0x508AB0: {1..6, 8, 0x43, 0x59, 0x5A}). Mounted (0x430190) -> H_IDLE 0x43 (and the horse idles, 0x429EC0); a horse
  with a rider (`+0x1E6`) -> 0x429EC0; escort / cart links (0x508F80, 0x4F2F60) -> CA_IDLE 0x68 on the partner; a siege
  seat (`+0x1FD`) -> crew idle 0x59 for {0x59, 0x5D, 0x5E, 0x63, 0x66} / 0x5A for {0x5A, 0x5B, 0x5F, 0x61}; else UP or
  IDLEUP -> IDLEDN 6; JATTBI 8 / JATTACKB 0x1D / IDLEDN 6 unchanged; behaviour (`+0x167`) 1 or 2 -> IDLE_BAT 4 (no
  row check); a tied prisoner (0x432B90 and `+0x2E2`) -> IDLE_T 7; else IDLE 1.
* **Landing, in `unit_move_step` 0x52DAA0 (0x52E6A5)**: when the body is back on the ground (body `air_frames`
  `+0x216`, ground height `+0x218` vs `z`, impulse end `+0x230` past, vertical move `+0x14A == 0.0`) a unit whose
  `Special` is not HORSE, in position 1 and not in UP, gets `set_action(DOWN 0x18, 0, 1)`.

### 6.4 `unit_anim_tick` 0x7C7AB0 (V; decompiled to `$JOAN_WORK_DIR/decomp/game/anim_sm/`)

Run from phase 1 step (c) when the unit has an actor. `remaining = 0x5045F0()`, "done" = `remaining == 0.0`.
Restart prologue: when the byte `+0x356` is set the current row is restarted and the byte cleared (writer TODO(re)).

First switch, every unit:

| action | rule |
|---|---|
| 1 IDLE | `r = rand_unit()` every tick (`x4` while `get_cut_scene_flag`); if `remaining <= 100`, the unit has an animation group (`+0x2F1`), position 0 and `r <= IDLE_CC (0.01)`: `p = rand_unit()`, `e2/e3` = rows for IDLE2/IDLE3 (0x416DC0), IDLE3 also needs `rider == 0` (0x4F2170): `p < 0.5 && e2` -> IDLE2; `p >= 0.5 && e3` -> IDLE3; else whichever exists (IDLE2 first) |
| 2, 3 IDLE2/3 | done -> IDLE |
| 0xE..0x17 strafes, jump hops | `rider == 0` and done -> `combo_reset`, `unit_stop` (0x508B90) |
| 0x18 DOWN, 0x19 JATTBD | done -> `combo_reset`, `unit_stop`; return |
| 0x1A UP | position 1: done -> `combo_reset`, `set_action(IDLEUP 5)`; return |
| 0x2B GUARD1, 0x2D GUARDR | return |
| 0x2C GUARD2 | done -> `combo_reset`, `unit_stop` |
| 0x30, 0x32, 0x33/0x38, 0x34, 0x36, 0x39 | bow ladder: arrow attach (hand items `+0x1BD/+0x1BF`), aim vector, release (`0x431480` projectile at `remaining <= 650` ms, 0 for Special 10, 1667 for 4/5/9, 1300 for an item class 0xC weapon), then SHTIDLE 0x36 / SHTIDLEH 0x39 by the aim's z (`< 0.1`), SHTOUT 0x33/0x38 for AI units, SHTOUT2 0x34 for trebuchets |
| 0x3E, 0x3F STOP | done -> the controller's movement vector is reset, `unit_stop`, row restarted |
| 0x47, 0x48 H_STOP | done -> `set_action(H_IDLE 0x43)` |
| 0x4F/0x50, 0x51/0x52 | mount / dismount end: heading and placement beside the horse, collision shape from UnitRes `+0x1C3`, then idle (dismount: DIE when dead) |

Second switch, units that are not the player-driven main character (`rec != main || ai_flag != 0`); its `default`
returns, so AI units never reach the third part:

| action | rule |
|---|---|
| 0x1E..0x20 ENGATK.. | done -> next link (0x1F/0x20/0x21) if the group has it and (`0x416ED0` entry flag or skill `0x50D3D0(13)` energy attack) ; else `+0x233 = 0xFF`, `+0x23F = now`, `unit_stop` |
| 0x21, 0x22, 0x53 | done -> close the combo, `unit_stop` |
| 0x23, 0x24 ATTACKED | done -> ATTKEDR 0x25 when the row exists, else `combo_reset`, `unit_stop` |
| 0x25..0x28, 0x3C, 0x4A..0x4D | done -> `combo_reset`, `unit_stop` |
| 0x29, 0x40..0x42, 0x4E | done -> `combo_reset`, `fell_timer -= dt`; when `< 0`: `set_balance(100)`; dead -> action DIE without a row; else STANDUP 0x3C, `downed_hit = 0` |
| 0x3B THROWN | done -> `combo_reset`; alive: collision type 3, `set_balance(100)`, STANDUP |
| 0x5B..0x66 | ladder / seat actions (`+0x1FD`, 0x4FAB10 release) |

Third part, the player-driven main character: a looping clip returns; `+0x235 == 1` returns; ATTACK / JATTACKB done
-> `combo_reset`; CHAIN/JATTACKB/DOWNATK/ATTACK done -> close the combo; the ENGATK chain as above; ENGATK4 / RUSH
done -> close, `unit_stop`; knock-downs: `fell_timer -= dt` every tick, done -> stand up as above; THROW done -> close
and turn 180 degrees; ladder moves (0x5B..0x66) adjust z by per-general constants. Finally **every finished clip
ends in `unit_stop`** unless the action is IDLEUP 5, IDLEDN 6, JATTBD 0x19, JATTBI 8, ENGATK2..4 or the unit holds a
ladder seat (`+0x1FD`); THROW restarts its row after the stop.

The bow, mount-end and ladder rows are `TODO(re)` (the shoot order and the mount code end those actions).

## 7. Stat bonuses `unit_equip_bonus(stat, recno)` 0x497E10

For stat ids 1..0x17: base `0` (ids 0x13 and 0x15: base 100). Sources, in this order: the equipped item of the active set slot 0, slot 1 (0x4974F0(0/1)) and the weapon pointer `equip+2`... (each via `0x4A4200(stat, item.level, &v)` = `base_i + (item.level - lvl0) * step_i` from the item definition tables at item-def `+0x4A..`, `+0x24D` list), then, if `equip+0x12` is non-null,
each *distinct* worn item of the inventory arrays (only the highest level / lowest index duplicate of the same item id counts; items failing `0x4A42F0` requirements — HP/EP% or stat thresholds — are skipped).
Combination per stat: ids 1..6, 0x14: `acc += v`; 7,8,10..0x11,0x17: `acc += (acc+100)*v/100`; 9, 0x16: `acc += (100-acc)*v/100`; 0x12: `acc = max(acc, v)`; 0x13: `if v != 0 return 0` (veto); 0x15: `acc = acc*v/100`.
Known stat ids (additive group 1..6): 1 max HP, 2 max EP, 3 strength (`0x5237E0`), 4 dexterity (`0x523860`), 5 defense (`0x5238C0`), 6 leadership (`0x523920`); 0x12 (`0x5103C0`, "max" group); 10 = chance (percent) to ignore an arrow/explosive hit (checked in `unit_on_hit_event`); 0x15 = special-attack cost scale (percent, base 100); 7, 8, 11..0x11 and 0x17 are percent boosts (damage/speed/etc. — consumers in the unexported damage code).
The item requirement test (`0x4A42F0`) compares the unit's current HP / EP (as percent of max) or stat 3/4/5/6/0x12 against the requirement list of the item definition.

## 8. Experience and levels

* `unit_add_experience(amount, to_team, silent)` 0x5229A0: ignored when the scenario state (`0x4CD510`) is 2, or the unit is a siege weapon (class 8).
  If `to_team`: a leader grants `ftol(amount * MEM_EXP(0.4))` to every living non-dead member (recursive call with `to_team = 0`); a member with a leader grants `ftol(amount * LD_EXP(0.4))` to the leader (the multiplication is an FPU expression; factors from Gameplay `EXP/MEM_EXP`, `EXP/LD_EXP`; the leader/member direction is inferred from the two branches, `I`).
  `experience += amount`. Main character: HUD update (0x6FE520) and **command points** `DAT_00A5B9F0 += amount` capped at `MAX_COM` 2000 (not in RTS view `DAT_00A5B9F8 == 4`). Then `unit_try_level_up(silent)`; main character: sound/event "EXPUP".
* `unit_try_level_up` 0x522C40: returns 0 if the unit `is_dead`, or `levelres_find(level)` / `levelres_find(level+1)` is NULL (so **level 30 is the cap**), or `experience < row(level).exp`. Otherwise loop while `experience >= row(level).exp`:
  `unlock_upgrades(level)`; `level++`; main character: HUD (0x703680); `levelup_flag = 1`, `levelup_time = now`; `attr_points += row.att`, `skill_points += row.skill`; reload the row. After the loop: `upg_timer = now`; for a non-main unit of the same nation while the main character is alive and `rank == 4` (generals) a level text is queued (female/male text by `UnitRes.Sex`), and
  **`hp = max_hp`, `hp_mark = max_hp`, `ep = max_ep`** (full heal); unless `silent`, effect `flash.txt` (5000 ms) at the unit; main character: event "LEVELUP"; leaders notify members' levels (`0x523190(level)`).
* Experience *rewards* for kills (`UnitRes.Experience`, Gameplay `KILL/EP_NOR 6, EP_CAP 15, EP_GEN 30`, `CRITEXP 3.0`): **result: the damage functions 0x509950 / 0x509E90 contain no experience award** (their callee lists have no `0x5229A0`; the only kill-related code in 0x509950 is the main character's critical-kill counter, §9.10.1). The globals `EP_NOR/EP_CAP/EP_GEN` (A3CC60/A3CC48/A3CC50, ints via `atoi`) and `CRITEXP` (A3CBD4) have no reader in any exported module; the reader lives in the (unexported) caller of the hit events, **Closed:** 0x51C680 is only the Gameplay loader (the xrefs 0x51CB58 / 0x51D72C are its stores). The reader is the death routine 0x7C6240, §9.12: `EP_NOR/EP_CAP/EP_GEN` are the **energy (EP) gain** of the killer by victim rank (rank <3 / 3 / >=4), `CRITEXP` is the experience factor of a critical kill, and the experience itself is `unit_add_experience(ftol(base * f), 1, 0)` on the killer with `f` from level difference, critical flag and the main character's chain bonus. Scenario end-condition rewards call `0x5229A0(amount, 1, 0)` for every rank > 3 unit (0x471620).

## 9. Combat

### 9.1 Attack sequence (melee)

1. AI/player issues `COMBO_ATTACK` (order 1) with a combo code, or `DOWN_ATTACK` / `ENERGY_ATTACK` / `RUSH_ATTACK` orders. 2. `order tick` starts `ATTACK (0x1C)` via `unit_set_action(0x1C, char)`: `combo_buf` grows by the combo char.
3. `unit_set_action` posts one **timed hit event for unit victims and one for item victims** (§6.2.8), due at `now + animscr.StartTime` (ms) with reach `animscr.Range + weapon reach`. 4. The trigger system (0x4EF090, battle family — see battle.md) runs each frame: for each pending event with `event.time <= now` it visits every unit `u` other than the owner whose own
`UnitRes.TriggerType & event.mask != 0` (the owner's type must allow the mask as well) and, for mask 1/0x400/0x10, `distance²(u.pos, event.pos) <= range²` (the event position is the attacker's position at creation; masks 0x40/0x2000/0x4000 use the victim's collision volume: sphere/box/cylinder test), then calls `unit_on_hit_event(u, event)` (0x75D540). A result of 2 ends the scan of that event.
5. `unit_on_hit_event`: mask 1/2/0x40/0x2000 -> §9.3; mask 0x400 (down attack) -> victim must be knocked down (action 0x40..0x42/0x29) and `unit_melee_hit` must return non-zero -> victim goes to DNATKED 0x29, `downed_hit = 1`, `fell_timer = 0`; mask 0x4000 (explosive keg) with item targets.

### 9.2 Hit validity and facing code — `unit_melee_hit(attacker)` 0x5048B0 (V; returns the damage amount, 0 = no hit)

Returns 0 when: the victim is dying (0x2A) or dead; the victim attacks itself; the victim is in an uninterruptible action {0x1E..0x21, 0x53, 0x4F..0x52}; the victim is down (0x40..0x42, 0x4E, 0x29) and the attacker's action is **not** DOWNATK, or it is DOWNATK against a standing victim;
`invuln_ms > 0` and the attacker is not DOWNATK; attacker DOWNATK and `downed_hit` already set; the attacker's `hittable == 0`; the nations are not hostile (`0x4B6860(a,b) == 0`, with the neutral nation 0 always hostile to everyone but itself).
Geometry: `d² = |attacker.pos - victim.pos|²` must be `<= (animscr.Range-ish reach_victim_side)`: `(range_entry.+0x30 + weapon_reach)²` for both unit ends; and the angle between the **attacker's** facing and the direction to the victim (`0x505960` called on the attacker with the victim's recno at 0x504CA9, +-180 deg) must lie in the animscr angle window `[+0x40, +0x44]` (normalised by +-360).
Block: if the victim's action is GUARD1 (0x2B) or GUARDR (0x2D) and `|angle| <= BLOCKANG (70)` -> plays DEFEND sound and "GUARD" particle, starts GUARDR (0x2D) (if the table has it), returns 0 (no damage). *The probability `BLOCKPER` (50) is applied in `0x512070`, which scales it by the difficulty and returns the effective block percent (see §9.9).*
Otherwise: plays the animscr hit sound/effect, then the **facing code**: attacker DOWNATK: `bonus = rand(50..100)` (`0x590690(0x32, 100)`); else with `a = |ftol(angle)|`, where the angle is between the **victim's** facing and the direction to the attacker (`0x505960` called on the victim with the attacker's recno at 0x504D75 — the same angle the block test uses): `a <= 45 -> 0`, `a <= 135 -> 20`, else `40` (attacked from the front/flank/back). The DOWNATK bonus uses `rng_range(50, 100)` on the RNG object at 0xA45FA8.
Result = `0x509950(attacker, bonus)` (§9.10.1). If the attacker is the main character the HUD hit marker (0x6FE280) fires. If damage != 0 and the victim is an AI unit, `attack_chance += 10`.
`unit_projectile_hit` 0x5051E0 is the same for arrows/stones/bombs (flag `param_4` allows hits on downed units): the block test uses `GUARDARR`, the facing code is the same, damage = `0x509E90(attacker, kind, bonus)` (§9.10.2), `attack_chance += 5`.

### 9.3 Hit reaction `unit_on_hit_event` 0x75D540 (V)

For melee (mask 1) after `damage != 0` the victim: records the attacker as target if it has none (not while it is attacking/shooting: actions 0x1C/0x49/0x30/0x32/0x36/0x33/0x39/0x38); the attacker's target is set to the victim if it has none; resets the path (0x422F10); plays blood (`0x414F50(1,0,0)`);
if the victim is not a prisoner it picks the reaction from `a = victim->attacker angle`: sector thresholds `_DAT_009D7C64`, `_DAT_009CFC14`, `_DAT_009D7C60`, `_DAT_009D7C5C` (values not readable; evidently +-45 and +-135 deg): front -> ATTACKED 0x23, left -> LATTKED 0x27, right -> RATTKED 0x28, back -> BATTKED 0x26; units in energy-attack states (0x1E..0x21, 0x53) instead flip a coin (`rand(2) == 1` -> `unit_knock_down`).
Balance: from the attacker's current animscr `AttackForce` (default 3000, `rec+0x4C`) and `VerticalAttackForce` (default 500, `rec+0x50`): `if (AF + VF < T_FALLF(6000) && VF < V_FALLF(3000))  balance -= (AF+VF)/F_FALLR(250)` else `balance = 0` (knock-down). Arrows (mask 2, 0x2000, 0x40): `balance -= ARR_FALL(25)`.
Knock-back: force `(cos,sin of the direction attacker->victim) * AF` in the ground plane, `VF` vertical, applied for 100 ms via the physics controller (0x6B11A0; the integrator divides by the body mass, flocksys.md 8.1 — e.g. 3000 / 50 = 60 physics units/s² for 0.1 s, then ground friction) — `x2.0` while 0x4E (H_DROP), `x0.7` while DNATKED (0x29). Units whose type code is `AUTOBOW` get none (`_stricmp`). Projectile hits (masks 2/0x40/0x2000) push with the constants `3000` / `500` (`_DAT_009CFB64`, `0x43FA0000`), `x3.0` (or `x2.5` on the second path) while H_DROP, `x0.7` while DNATKED — TODO(port): the port applies only the balance loss for projectiles.
Evade: for masks 2 and 0x2000 with an equipment object, `r = rand(100)`; if `r < equip_bonus(10)` the arrow is ignored.
Mounted victims (0x49/0x46 with the horse running) are knocked towards the opposite side (`+-` constant) and may fall (H_DROP 0x4E), see `unit_set_balance`.
Gamified post-step: for rank > 3 units, HP percent crossing below 25 % (`< 0x19`) or below 50 % after being above triggers a message (text ids per unit via `0x453DA0`). *(0x75D540 lines 484-509, text pointers unreadable.)*

### 9.4 Special-attack availability (V)

* `can_attack_down(unit)` = animation table has DOWNATK (`0x500D40(0x22)`).
* `is_facing_knock_down_enemy(unit)`: some other unit in the table with action KNOCKDN/2/3 (0x40..0x42), hostile (`0x5094D0`), at distance in `(DAT_009CFC0C, DAT_009CFBA8)` and `d² <= DAT_009CFC28`.
* `can_energy_attack(unit)`: hostile to the main character -> true; else `cost = ftol(max_ep * equip_bonus(0x15)/100...)`; requires `cost <= ep` and `hp > 3 * hpcost` and an ENGATK entry plus either the animation flag (`0x416ED0`) or the learned skill (`0x50D3D0(13) > 0`).
* ENERGY_ATTACK costs: `EGATK_MP` 40 / `EGATK_HP` 20 in Gameplay (`UNIT`), applied as percentages of max EP / max HP scaled by equipment stat 0x15 (0x4BCA60); RUSH uses the animation entry's `ENERGY_PT`.
* `HATK_MP` 6 (heavy attack EP) has no reader in the exported code.

### 9.5 Death — `unit_on_death` 0x64F320 (battle export, V)

(called when HP reached 0 with no life left; exact caller outside the export)
1. For generals/rank > 3 leaves the equipment bookkeeping (quest items of class 0xF are removed from the inventory arrays) and notifies the scenario (`scenario::fn_004CDA10`).
2. Siege weapon crew link release; crew members of a dying siege weapon are detached, their tasks/orders cleared, ram crew with usage bit 2 are killed (`lives = 0`, `hp = 0`, recursive death).
3. Linked cart/escort (`+0x1F9`) is killed likewise. Nation army strength `-= max_hp * unit_strength_weight(type)` through `nation_add_strength` (0x4B5520, §9.10.5).
4. Morale: for every unit of the same nation, `0x4FE8C0(ftol(...))` adjusts morale (`MOR_CH_U` for the unit's team... / `MOR_CH_T` for troop); the exact FPU expression is not visible. The setter `unit_set_morale` is described in §9.10.4.
5. **Death-protected units** (`death_state == 1`): `invuln_ms = 0x7FFFFFFF`, `hp = 1`, task := IDLE(7) with priority `task.prio+1`, `death_state = -1`; no DIE action.
   Others: stop task, set action **DIE (0x2A)** (`unit_set_action(0x2A)` unless already THROWN 0x3B), start the fade-out of all actors (`fade_rate = -0.1`, `fade_start = now`, `fade_max = 255` or the current alpha when `death_state == -1`); the shadow actor is deleted at once.
   Siege: a destroyed BOMBARD... triggers 0x510220.
6. Statistics: if the dying unit is class 0 (citizen) and the main character is hostile to it, civilian-kill counters increment (`DAT_00A5918D`, and `DAT_00A59199` for female).
7. The corpse (action DIE) stays until the unit pass (§3.B.1) deletes it once `hp<1 && lives==0 && ai_flag != 0 && death_state != -2` and the fade has finished (`1000/|0.1|` = 10 s... fade rate -0.1 per `1000/|rate|` ms).
   **Respawn**: only the `Life` counter (`UnitRes.Life`): `unit_set_hp` re-fills HP in place when `lives > 0` (no animation).

### 9.6 Item pickup (generals) — `unit_pickup_items` 0x49DA30 (V)

For `rank > 3`, an actor, the unit's block, `lod < 4`: collects item objects (type check `Item.def.+0xE2` pickable) inside a radius `item.radius*scale + unit.cylinder_radius*scale + const` (XY) and `|dz| <= cylinder height`. By item class: 0x15 potions (only if `item.heal < max_hp - hp`: use immediately `0x50B4F0`), 0xF quest/key items (special names `S7_food` counted via `DAT_00A591C2`, `DAT_00A591B6`), 8 coins (`gold += def.+0x2E * count`, `DAT_00A596C4`),
all others -> inventory `0x495F40`. Effects: pickup particles, sound "PICKUP", HUD message.

### 9.7 Healing — `0x50B4F0(item, force, ...)`

`delta = item.def.+0x4E`; `hp = min(hp + delta, max_hp)`; if the unit has no team (`+0x33 == 0`) the remainder `local_1c = delta - healed` is distributed to team members/leader in ascending HP% order; the leader branch heals each member by `delta/2`. Returns false if the unit is full and not forced.

### 9.8 Auto potion — `unit_try_auto_potion(force)` 0x4FD980 (V, read in full 2026-10-22)

Returns **1 = "in danger and no potion used"**, 0 otherwise. Script builtin `is_in_danger(unit)` (id 57, 0x652A40) is `valid(unit) ? unit_try_auto_potion(0) : 0`, so a script polling it also makes the unit drink.
1. If `0x42D0E0()` is true and `force == 0`: return 0. `0x42D0E0` is the **dead test** `hp (+0x135) < 1 && lives (+0x120) == 0` (V, body read 2026-10-22), so a forced call (the hit handler 0x75D540 passes 1 after every damage branch, before its own death test) can make a unit killed by the hit drink and survive.
2. If the unit is the controlled unit (`DAT_00A5B9E8`) and the control mode `DAT_00A5B9F8 != 4` (not RTS view): return 0.
3. If `hp / base_max_hp >= INDANGER` (float compare, `0xA3CD24` = 0.2): return 0. Note: `hp / base_max_hp` uses `+0x135 / +0x13D` (not the equipment-boosted max).
4. `missing = max_hp() (0x523320) − hp`.
5. **Rank > 2** (`+0x104 > 2`, captains and generals): if `equip` (+0x1C1) is null or `equip+0x16` (healing-potion list) is null → return 1. Otherwise pick, from the unit's inventory list (iterated from the highest index down, 0x417AD0 count, 0x457F60 "slot empty", 0x457E90 entry), the entry whose item template (0x425CE0) has class `+2 == 0x15` (healing potion) with the smallest `|template.+0x4E − missing|` (ties → the later-visited, i.e. lower index, because the test is `<=`). If found: `0x50B4F0(item, force, 1)` drinks it; on success the stack count (`entry[1]`) is decremented and the slot freed at 0 (0x457EC0), and if the unit has an actor (+0x2F5) the `spiritual.txt` particle script is attached for 5000 ms. Return 0 (whether or not the drink succeeded). If no class-0x15 item: the same search over class **0x19** with `0x50BCD0(item, force, 1)`; found → same bookkeeping, return 0.
6. **Rank ≤ 2, or rank > 2 with neither potion class in the inventory:** the potion source is `src = this` when rank > 2, else the leader (`0x4216D0`); no leader (rank ≤ 2) → return 1. If `src.equip` is null or `src.equip+0x1A` (the second potion list) is null → return 1. Otherwise the class-0x15 search then the class-0x19 search as in step 5 (same tie rule, same drink functions, same effect), return 0 when an item was found (drunk or not), else return 1.
### 9.9 Difficulty hooks

`0x511F80` returns the AI's attack willingness: `attack_chance` scaled by `GAME/{EASY,NORM,DIFF,VDIFF}_A` = 0.5 / 1.0 / 1.5 / 2.0 for `DAT_00A57FC2 = 0..3` (`default:` case keeps the raw value). `0x512070` does the same for the block percent `BLOCKPER` with `…_B` (0.5/1/1.5/2). The `…_D` family (0.7/1/1.5/2) multiplies the **attacker's base attack value** when the attacker is hostile to the main character (i.e. the enemy's damage grows with difficulty), inside 0x5104F0 / 0x510BF0 (§9.10.3): `_DAT_00A391AC / 00A3918C / 00A391B0 / 00A39194` for `DAT_00A57FC2 = 0..3` (EASY/NORM/DIFF/VDIFF `_D`; keys read by 0x472EE0 from section GAME).

### 9.10 Damage, defence, morale and strength (0x509950 / 0x509E90 and helpers, V unless marked)

Notation: `A` attacker, `V` victim, `eb(k, u)` = `unit_equip_bonus(k, u.recno)` (0x497E10; 0 for units without Equipment), `morale` = `Unit+0x163` (0..100), `armor_id` = `Unit+0x15D`. Constants are Gameplay globals (§13); the literals `_DAT_009CF9C4`, `_DAT_009D5344`, `_DAT_009D5360`, `_DAT_009D7558`, `_DAT_009CF794`, `_DAT_009DD6D0` were read from `.rdata` : `0x9CF9C4` = 100.0, `0x9D5344` = 0.01, `0x9D5360` = 1e-4, `0x9CF794` = 2.0, **`0x9D7558` = 11.0**, **`0x9DD6D0` = 250.0** (V). Everything is single-precision `float` math; the only integer truncations are listed explicitly.

#### 9.10.1 `unit_take_melee_damage(V, A, facing_code)` 0x509950

1. **Instant-kill proc.** If A has Equipment and `rand(100) < eb(0x14, A)` the damage is V's current HP (V dies; goes straight to step 7). (`rand` = `0x590540`.)
2. `base = unit_weapon_base_melee(A)` (§9.10.3), `m1 = (A.morale + 100 - MOR_NOR) / 100`, `mult = unit_attack_type_mult(A)` (§9.10.3). `att = mult * m1 * base`.
3. `armor = V.armor_id * AMR_DEF` (AMR_DEF = 1). `D = unit_defense_total(V)` (0x5238C0). `def = ((V.morale + 100 - MOR_NOR) / 100) * ( D * (1 - facing_code/100) * DEF_RAT / 100 + armor )`. `facing_code` is 0 (front), 20 (flank) or 40 (back) from §9.2, or 50..100 for DOWNATK (so DOWNATK removes 50..100 % of the defence term). **`DEF_RAT` is the `atoi`-truncated 0 as shipped (0.3 in the file), so the whole defence term collapses to `armor * morale-factor`: only the armour id counts as shipped; defence stats do not reduce damage** (see GAPS in game/unit.md).
4. `diff = (att - def) / 2`, `extra = max(0, diff)`, `dmg = ignore(A) * (att/2 + extra)` where `ignore(A) = 0x510E80 = (100 - eb(9, A)) * (100 - eb(0x16, A)) * 0.0001` (**I** for the constant; 1.0 without Equipment).
5. **Main-character attacker only** (A is `DAT_00A5B9E8`): if the "critical" flag `+0x1B4` of the main character's stat object (`psVar4[0xDA]`) is set, `dmg *= 2`; then `dmg *= (signed byte at +0x9D)` (0x509D40, a per-weapon-skill multiplier; its writer is not in the export).
6. **Friend/foe scaling** (only while the main character exists and is alive): when the scenario id `DAT_00A58FA7 == 8` and A's nation differs from the main character's, `dmg *= F_ATTACK / 250` (`_DAT_009DD6D0` = 250.0, i.e. 130/250 = x0.52 in scenario 8 for foreign nations); when V's nation differs from the main character's `dmg *= F_DEFEND / 100`. Then, unconditionally: A's nation == main's -> `dmg *= F_ATTACK / 100` (130 %); V's nation == main's -> `dmg *= F_DEFEND / 100` (70 %). So the player's side deals +30 % and takes -30 %.
7. **Leader factor:** if V has a leader (`0x4216D0(V)` != V) then `dmg *= (min(leader.leadership(+0x15B), 100) + 100) / 100`, i.e. 1.0 .. 2.0. The factor is applied from the *victim's* leader, and is >= 1, so a high-leadership leader makes damage dealt to his own followers larger; whether the code intends the attacker's leader (the decompiler shows `0x4216D0` called on the victim register) is unverified (**I**).
8. **Apply:** `unit_set_hp(V, ftol(hp_expression))` (0x4FE510) where the FPU argument is hidden and read as `V.hp - dmg`; returns `dmg` (float). Truncation: `ftol` toward zero on the new HP (fractional damage is lost against HP, not against `dmg`).
9. **Critical-kill bookkeeping (main character attacker with the crit flag):** `+0x1B5++` (crit counter); if V is dead (`hp == 0 && lives == 0`) the HUD message "CRITKILL" is raised, 0x509D70 sets the flag at the stat object `+0x1B` and `+0x1B7++`.

#### 9.10.2 `unit_take_projectile_damage(V, A, kind, facing_code)` 0x509E90

Same skeleton as melee with these differences:
* `base = unit_weapon_base_ranged(A)` (§9.10.3), `att = ((A.morale + 100 - MOR_NOR)/100) * base * unit_ranged_skill_mult()` where `0x510E40 = (skill_level(type 0xE) + 10) / 11` (`_DAT_009CFC20` = 10, `_DAT_009D7558` = 11.0: the multiplier is 10/11 with no skill and 1.0 at level 1; `skill_level` = 0x50D3D0 over the unit's learned-skill list).
* Defence term identical (`armor_id * AMR_DEF`, `D * (1 - facing/100) * DEF_RAT / 100`, morale factor).
* `dmg = ignore_ranged(A) * (att/2 + max(0, (att - def)/2))` with `ignore_ranged = 0x510F10 = (100 - eb(0x16, A)) * 0.01` (1.0 without Equipment).
* **Range falloff** (V is not a siege weapon, UnitClass != 8): `d = ftol(|A.pos - V.pos|)`; `d >= FALL_E (2400)`: `dmg *= DIST_P (0.5)`; `FALL_S (800) < d < FALL_E`: `dmg *= 1 - (d - 800) * 0.5 / (2400 - 800)`; below 800 no change.
* **Armour/ammo special cases:** if `V.armor_id == 3` and `kind` is 0x40 (stone) or 0x2000: `dmg *= AMREXDEF (0.5)`; for `kind == 0x40` (stone) `dmg *= SIE_UDAM (1.5)`.
* Friend/foe scaling, leader aura and the final `unit_set_hp(ftol(...))` are the same as melee (steps 6..8); there is no instant-kill proc and no crit multiplier.

#### 9.10.3 Base attack values

* `unit_weapon_base_melee` 0x5104F0: `anim_dmg` = DAMAGE field (`+0x1C`) of the current animation entry (`0x4168D0(unit.+0x0A)`). With Equipment: `W = (level_value(held item 0) + level_value(held item 1)) * (eb(7) + 100) / 100`, where `level_value = 0x4A41C0(item.level)`. `x = W - WP_DAMG(0) + 100`; `base = ((STR_total * STR_DAM(3) + x) * anim_dmg) / 100` (STR_total = 0x5237E0, the `stat + equipment` sum). With Equipment `base *= (eb(8) + 100) / 100`. If A is hostile to the main character (`0x5094D0` true) `base *= difficulty_D[DAT_00A57FC2]` (§9.9).
* `unit_weapon_base_ranged` 0x510BF0: `W` = sum of level values of the two bow-slot items (0x497560 / 0x497580), `*= (eb(7)+100)/100`; divisor `div = 200` when a second ranged item exists else 100; `base = ((DEX_total * DEX_DAM(1) + 100) * W) / div` (DEX_total = 0x523860); with Equipment `base *= (eb(8) + 100)/100`; same difficulty `_D` multiplier.
* `unit_attack_type_mult` 0x510750 (melee only; keyed by the attacker's current action): ATTACK 0x1C: `(skill(9, combo_string) + 10)/11 * (eb(0xB if the last combo char is 'A' else 0xC) + 100)/100`; DOWNATK 0x22: `(skill(0xB)+10)/11 * (eb(0xF)+100)/100`; JATTACKB/JATTBI/JATTBD (0x1D, 8, 0x19): `(skill(10)+10)/11 * (eb(0xD)+100)/100`; RUSH 0x53: `skill(0xC)`, `eb(0xE)`; ENGATK stages 0x1E..0x21: `skill(0xD)` and no equipment term; H_ATTACK 0x49: `'A'` -> `skill(0xF)`, `eb(0xB)`, else `skill(0x10)`, `eb(0xC)`; any other action: 1.0. (`skill(t, name)` = 0x50D3D0: level of the first learned skill of type `t` whose name matches and whose current level == max level; the "learned" list comes from the unit-level upgrades, UnitLevel.md. The `+10` (`0x9CFC20`) and `/11` (`0x9D7558`) constants are read from `.rdata`.)

#### 9.10.4 `unit_set_morale(v)` 0x4FE8C0

Default: `morale = clamp(v, 0, 100)` (0x590AA0). Exception: if `v < morale` (a decrease) and the unit has a leader with Equipment: `floor = eb(0x12, leader)`, `gate = eb(0x13, leader)`; if `gate == 0` the decrease is ignored; if `floor < morale` and `v < floor` the morale becomes `floor` (a leader's gear can set a morale floor for his team); if `floor >= morale` the call is ignored. Increases are always applied.

#### 9.10.5 Strength weight and nation strength

`unit_strength_weight(type)` 0x527DD0 = f(`UnitRes.Ranking`): 0,1,2 -> 1; 3 -> 2; 4,5 -> 3; anything else 0. `nation_add_strength(delta)` 0x4B5520: `nation.+0x1389 += delta` (clamped >= 0). 0x509250 (set nation) adds/subtracts `max_hp * weight` when a unit changes nation, 0x64F320 subtracts it on death (§9.5).

#### 9.10.6 Summary of the shipped numbers (default difficulty)

Player-side hit: `att` (STR 3 per point + weapon levels) is multiplied by 1.3 (F_ATTACK). The enemy's hit on the player has its base multiplied by `difficulty_D` (0.7 / 1 / 1.5 / 2) and the final damage by 0.7 (F_DEFEND). Armour adds `armor_id` (a small integer) to `def`; with DEF_RAT truncated to 0 the defence stat and the facing code do not change the damage at all as shipped.

### 9.12 Kill rewards — `unit_on_death(this = victim, killer_recno)` 0x7C6240 (R from the raw decompiler output)

This body is the death routine: `this` = the dying unit V, `param_1` = the killer's recno K (K is used as the unit record `c`). It is the same routine §9.5 cites as 0x64F320 from the battle export (identical statement sequence; another export labels the address 0x7C6240 — `TODO(re)`: reconcile the two addresses). It is the **only reader** of the Gameplay keys `EP_NOR/EP_CAP/EP_GEN` (A3CC60/48/50), `CRITEXP` (A3CBD4), `C_BONUSL/E_BONUSL/M_BONUSL` (A3CCCC/A3CCB4/A3CC3C). Order of the reward part (after the crew/cart release already described in §9.5):

1. **Energy reward (not experience!).** The killer's EP is raised through the EP setter `0x4FE5D0(K.ep + gain)` (clamped to max EP by the setter): `gain = EP_NOR (6)` if `V.rank < 3`, `EP_CAP (15)` if `V.rank == 3`, `EP_GEN (30)` if `V.rank >= 4`. Integer addition, no rounding. (So the keys' "kill EP gain" reading in §13 is correct: EP = energy points.)
2. **Kill counters on the killer** (stat object, shorts): `+0x1AE += 1` if `V.rank < 3` else `+= 10` (kill score), `+0x1B0 += 1` (kills), `+0x1B2 += 1` if `V.rank > 2` (officer kills); the killer's leader (`0x4216D0`, when the 0x44A660 guard is false) gets `+0x1BB += 1` and, in a second identical block, `+0x1B9 += 1` (team kill counters; the guard function is not exported).
3. **Experience factor** `f` (float): `f = 1.0`; if the killer's critical flag byte (`+0x1B4`) is set, `f = CRITEXP (3.0)`; if `V.level < K.level` then `f *= 1 / 2^((K.level - V.level) & 0x1F)` (halved per level of difference, killer stronger); if K is the controlled main character and the view is not RTS (`DAT_00A5B9F8 != 4`): `n = (int)DAT_00A5BA62 / C_BONUSL (50)`, `n = min(n, M_BONUSL (3))`, `f *= E_BONUSL (100) * 0.01 * n + 1.0` — a combo/chain counter in `DAT_00A5BA62` multiplies the reward by up to 1 + 3 = 4.
4. **Award:** `unit_add_experience(ftol(base * f), to_team = 1, silent = 0)` (0x5229A0) on the **killer** K; the FPU expression for `base` is hidden by the decompiler (`__ftol(1,0)`), the natural reading is the victim's `UnitRes.Experience` (**I**). `ftol` truncates toward zero. The team/leader flow of `unit_add_experience` is in §8 (leader -> every member x `MEM_EXP`, member -> leader x `LD_EXP`, 0.4 each; main character gets command points). No rand is drawn.
5. **Nation strength:** if `0x509400()` (V's side counts for the army total) is non-zero: `nation_add_strength(-max_hp(V) * weight(V.type))` (§9.10.5). `local_80 = weight(V.type) + 0x527E60(K.type)` is then computed for the morale step.
6. **Morale:** for every living unit U (`0x417AD0` count down to 1, `0x41B3F0` lookup): if K has a nation and `U.nation == K.nation` and K's team id is non-zero and `U.team == K.team` -> `0x4FE8C0(ftol(hidden))` (the killer's team gains morale); else if V has a nation, `U.nation == V.nation`, V's team id non-zero and `U.team == V.team` -> `0x4FE8C0(ftol(hidden))` (the victim's team loses morale). The magnitudes use `local_80` and `MOR_CH_U/MOR_CH_T`, hidden FPU (**I**).
7. **Voice / presentation:** for victims of rank > 3 that do not just revert to death-protected state, the sound `RTS_OTHER` + the unit's name stem + one of three suffixes chosen by `rand(3)` (`0x590540(3)`, the only rand in this function) is played at the unit position (`0x494810`). If V is the controlled unit and is not `JOAN_OF_ARC`, control switches to the first available unit (`player_switch_unit(0)`, §11.4). Statistics: `DAT_00A591B1++` when K is in the "English" nation of record `nation[K.nation]` (+0x14 name) at the head of the routine; `DAT_00A5918D++` (and `DAT_00A59199++` for females, `UnitRes.+0x117 == 'F'`) for a killed class-0 citizen the main character is hostile to (§9.5). The whole routine is skipped when the scenario state (`0x4CD510`) is 2 and the guard fields `V.+0x10C == 1`, `K.+0x10C != 0` hold.
8. The remaining body is §9.5 (death-protected `death_state == 1`, DIE action 0x2A, fade-out, shadow/attach actor release, siege `0x510220`).

### 9.11 Anim sounds, mount and link fields 

* **Mount (rider side), `unit_set_mount(horse)` 0x4FC220, field `Unit+0x1E2`.** Refused (0) when `horse == self`, when the horse's type class fails `0x42A4D0(class)` (rideable?), when the horse refuses the rider (`0x482930(self)` = `horse_set_rider`), when the rider has no actor controller, or when it is already mounted. Mounting: the animation controller is reset (`0x6AD980(ctl, 0)`), then the horse's facing relative to the rider (`0x505960`) selects the side: angle `>= 0` (`_DAT_009CF5C4` = 0.0) -> action **0x50 H_MNT_R**, else **0x4F H_MNT_L**; the rider is moved to a point beside the horse at an offset whose length is 30.0 (the literal `0x41F00000`) except 45.0 (`0x42340000`) for the generals named **LAHIRE** and **ALENCON** (JEAN_DE_METZ explicitly 30.0); the turn/heading is aligned to the horse (`0x421020`, +-`_DAT_00A3CBB4 / 2` half-turn), the action is started with `0x4FF9F0(entry, now, -1)`, and the flag bytes become `Unit+0x1EA = 0xFF`, `+0x1EB = 0`, `+0x356 = 0`, `+0x1E2 = horse`. Dismounting (`horse == 0` with a current mount; refused if there is no actor): a probe point at the same side offset is tested with the collision probe `0x693170` using the rider's collision type (`ctl+0xB8 = UnitRes.+0x1C3` during the probe); if the first side is blocked it tries the other side; both blocked -> refused (0). Action **0x51 H_DMNT_L** is used when the first side succeeded, **0x52 H_DMNT_R** when the second did; flags become `+0x1EA = 0`, `+0x1EB = 0xFF`, `+0x1E2 = 0`. The rider's `unit+0x2F5` controller `+0x5C` entry receives a refresh (`0x6AF370`).
* **Link A, `unit_set_link_a(partner)` 0x51A250, field `Unit+0x1F5`** (cart/escort pair): returns 1 at once when `partner` is already linked; 0 for `partner == self` or when the partner side refuses (`0x51A8A0(self)`); the partner is placed at a point 80.0 units (`0x42A00000`) from the unit and turned to align (same `0x421020` +- `_DAT_00A3CBB4/2` rule, side chosen by `0x505960` sign); the unit's controller is reset when it is newly linked; release (`partner == 0`) probes a free spot (offset 80.0) with `0x693170`, clears `+0x1F5`, and refreshes the controller.
* `+0x1F9` (`link_b`) has no setter among the exported functions; `unit_link_tick` (0x454B20, phase 1) clears the collision word of link A's controller and, when the unit's action is IDLE (1) and `link_b` names a valid unit, starts action **0x68 CA_IDLE** (cannon-attendant idle), so link B is the crew-to-weapon link.
* `0x4FD630` is the 1-line `Unit+0x247 = 0` (§1.4); `think_forced` in `unit_ai_think` triggers it.

## 10. Movement

### 10.1 Speed — `unit_apply_velocity` 0x4F0B50 (V)

`action_used = (use_action_alt ? +0x217 : +0x215)`. Base speed `v` (world units per **second**... see below):
| action | v |
|---|---|
| 9 TIPTOE | 1.0 |
| 0xA WALK | `UnitRes.WalkSpeed` (+0x9C) |
| 0xB RUN | `RunSpeed` (+0xA0) |
| 0xE, 0x11..0x14 | `ShiftLeftSpeed` (+0xA4) (diagonals share it) |
| 0xF | `ShiftRightSpeed` (+0xA8) |
| 0x10 | `ShiftBackSpeed` (+0xAC) |
| 0x44 H_WALK | 3.0 |
| 0x45 H_BACK | -3.0 |
| 0x46 H_RUN | 5.0 |
| 0x15..0x17 (jumps) | 0 (the physics impulse moves the unit) |
Dexterity factor (not for 0xE/0xF/0x10 shifts): with `d = dexterity` of the unit — **of the leader when the unit is a follower** — `if d > MINDEXS(30): v *= (min(d, MAXDEXS(200)) - 30)/(200 - 30) + 1.0` (so up to x2).
`if ai_flag != 0: v *= AI_MOVE (1.2)`; `v *= _DAT_009D51E8` (unit scale, unreadable). If the unit has no mount: LOD compensation `lod >= 3: v *= _DAT_009D7498`; `lod == 2 and v < _DAT_009D5334: v *= _DAT_009D7494`.
If a path exists and `ai_flag != 0` the step is clamped to the remaining distance to the next waypoint: `v = min(v, dist * 1.0/_DAT_009CF780 * _DAT_009CF778 / dt)` (a "snap to waypoint" clamp, constants 1000.0-style).
The velocity vector is `(cos(heading), sin(heading)) * v` handed to the physics/actor system (0x6B10A0 with an interpolation time `min(dt*dist/v*_DAT_009D727C, dt)` clamped to 300 ms/1000 ms), i.e. **units/ms ~ v/1000 with v from the unit.res speeds (3 = walk, 5 = run)**; the physics side is the movement family (flocksys.md).
Scripted movement (`+0x3B8 != 0`): straight line to `+0x3CD` at speed v, heading set by atan2; stops (0x508B90) within `_DAT_009CF878`.

### 10.2 Turning — `unit_turn_step` 0x4F2190 (V)

`target = target_heading` (deg), `cur = heading*180/pi` (constants `_DAT_009CFBF8 = pi`, `_DAT_009CFBE8 = 180`). Siege weapons first check that their pushers (attach usage bit0) are linked. `delta = atan2-based signed angle in (0,360)`; max step
`step = (now - last_update_ms)/_DAT_009D74D0 * K`, with `K = 20` for ordinary units, `1` for Special 4/8/9/10/11/13 (cannon, siege tower, bombard, trebuchet, ram, cart), `2` for FAUCONNEAU, `5` for CROSSBOW; then clamped to `<= 25` degrees per call for `lod < 2` (`_DAT_009D74CC` test) or `<= 90` (`_DAT_009CFBA8`) for far units.
The unit rotates by `min(step, |delta|)` in the shorter direction (`delta >= 180 -> negative`). Crews follow the weapon (attach point positions `p = actor_matrix * attach_pos[i]`, heading `+= AttachDir*pi/180`) and are given walk/push actions (10 / 0x54 / 0x56) while it turns. Returns `step <= |delta|` (i.e. "still turning").
Constant `_DAT_009D74D0` unreadable; (TIMING.md's `3.4906585e-05 rad/ms` belongs to the actor-animation turn in 0x4352F0, not here.)

### 10.3 Path following — `unit_move_step` 0x52DAA0 (R, structure verified)

* Siege crews (class 8 with attach points): if the weapon is not manned, pushers go idle (0x508B90); manned weapons call `0x501190` to give crew actions 10/0x56/0x54/1 depending on the usage bit and `Special`.
* Movement actions {9, 0xA, 0xB, 0x44..0x46, 0xE..0x17} run `unit_apply_velocity`, with a rotation-on-the-spot fix for mid-turn.
* When the animation is done (`+0x355`) and the unit is not in a stationary state the next waypoint is processed: waypoint list = path objects (0x66E8A0 count, 0x66E8D0(i) point). Arrival radius `REACH_DT (15)` — `x2` (30) for mounted / class 8 / `lod > 1`, `x5` (75) ... `*2/*5/*10` in the table form for the final point of long paths; passed waypoints are skipped (`+0x3E6++`). When the final waypoint is reached `0x4EECA0` clears the path and the unit goes idle (`unit_stop`).
* **Blocked / stuck handling**: `wait_count` (`+0x3F6`) is decremented each move tick; when it reaches 0 and the path is not complete the unit re-requests its path (`0x422F10` then `0x4F7BF0`), and `wait_limit` grows by 1 up to `MAX_W_C (30)` then resets to `INIT_W_C (5)`. If the next waypoint cannot be reached because another unit pushes, `stuck_count` is incremented per push; after 500 ms the counters are cleared and `wait_count--`.
* WALK vs RUN: when `path_remaining (+0x40E) <= WK_DIST (50)` the unit selects WALK else RUN (for mounted units `HORSE_WD (500)`).

### 10.4 Sidestep — `unit_sidestep_unblock` 0x4F7BF0 (V)

Rotates through four phases (`+0x3EE`, 0..3, then `& 1` wrap): phase 0/1: heading offset `rand(25)*pi/180 + const(_DAT_009D74C0, ~pi/2)`, sign chosen by which side the leader/ref heading `+0x40A` lies on, a point `rand(100)+20` units away (x and y independently) is requested if reachable (`0x523E50`) and a path exists, then a 100 ms path is built through `0x66CB40` with the unit's collision radius (cylinder/box/sphere max extents `x 3.0`(`_DAT_009CF798`) for siege); phase 2: bearing `-(const + rand(45) deg)`; phase 3: `+(const + rand(45) deg)`.

### 10.5 Formation — `team_update_formation(force, arg)` 0x4FA0F0 (V)

Runs for leaders when the team was marked dirty or the leader moved: if `|leader.xy - team.last_xy|² < LD_FM_DT (160000)` and not forced nothing happens. Otherwise stores the new `last_pos/last_heading`; if `team.formation == 1`: columns `C = 1 (n<2), 2 (n<5), 3 (n<10), 4 (n<17), 5 (n<26)`, spacing 100, `width = (C-1)*100`; member `k` gets the unrotated slot `(x = -100 - (k / C)*100, y = width/2 - (k % C)*100)`
rotated by the leader heading into `form_off_*`. Slots are handed out by class group in this order: first all members of UnitClass {1,4} (light infantry, maceman), then {2,3} (heavy, pole), then {5,6,8} (archers, crossbow, siege), each member taking the nearest free slot (`0x45DE50`, search window `100/10`).

## 11. Search, ranges, teams, switching

### 11.1 `unit_search(type, range, use_leader, place, max_xy, max_z)` 0x54B480 (V)

`type[0]`: `'T'` current target (valid -> returns `+0x2D7`), `'E'` enemy, `'F'` friend (0x5095E0), `'L'` leader (leader if within `range`), `'P'` the main character, `'S'` siege units (UnitClass 8 with `TriggerType & 0x40`), `'O'` units without an actor; `type[1] == 'P'` additionally restricts to a place (`0x456C70(place)`, point-in-place `0x60D5F0`).
`range` default 1600 (0x640). Centre = own position, or the leader's when `use_leader`. `max_xy`/`max_z` limit distances to the candidate (0x505960-style `d²` checks).
* **Target stickiness ('E')**: if the current target is valid, alive, `death_state != -1`, within `range²`, visible (not in a blocked LOS test 0x560D80) and `rand(100) < 96` it is kept and returned. Otherwise `+0x2D7 = 0`.
* Candidates: units from the spatial index (`0x5EF610` cell query on the cube `centre +- range`), skipping self, dead units (`hp < 1 && lives == 0`), `death_state == -1`, and units whose hostility fails; at most `cap` accepted: `range < 200: 2`, `< 1000: 5`, `< 2000: 10`, `< 3000: 20`, else 100.
* Selection (mode `+0x16B`): 1 NEAREST: first candidate wins; a later nearer one replaces it with probability 51/100 (`rand(100) < 51`); 2 WEAKEST: key `lives*max_hp + hp` smallest, replace prob 51 %; 3 IMPORTANT: largest `rank`, replace 51 %; 4 NEAREST_ARCHER: prefers archers/crossbow (UnitClass 5/6) when the searcher is one; replace probability 81 % ('<' 0x51), non-archer candidate beats archer only if closer than half; 5 NEAREST_HIGH_PRIORITY: highest `unitres_target_priority`, ties by distance with 81 %.
* After the scan, for `'E'` with prob 51/100 the result is stored in `+0x2D7` (and returned either way).
For `'E'` candidates the victim must also have `TriggerType & 1` (hittable by melee) unless it is a siege unit, in which case a unit with usage-2 crew is skipped.

### 11.2 `get_unit_range(unit, which)` 0x65B280 (V)

By target-unit-class of `unit` (UnitClass): `(which = 1 / 2 / 3)`:
| UnitClass | 1 | 2 | 3 | 4 |
|---|---|---|---|---|
| 1 LIGHT_INF | 800 | 1600 | 2500 | |
| 2 HEAVY_INF | 600 | 1200 | 2500 | |
| 3 POLEMAN | 600 | 800 | 2500 | |
| 4 MACEMAN | 1000 | 2000 | 2500 | |
| 5 ARCHER | 250 | 1200 | 1600 | |
| 6 CROSSBOW_MAN | 250 | 1400 | 1800 | |
| 8 SIEGE_WEAPON | 400 | 1600 | 1600 | 3200 |
| other | 0 | 0 | 0 | 0 |
(1 = close-defence/aggro, 2 = chase/search, 3 = leash — as used by the AI scripts; for `which == 4` only siege.)

### 11.3 Team join/leave rules (V)

`join_team(member, leaderRecno)`: leader must have `team.count < 20` and `(member.nation == 0 || leader.nation == 0 || equal)`; copies `team_id`, clears `team_flag`; a leader's own `+0x37` stays 0. `leave_team`: removes from the leader's array and clears `leader`, `team_id`, `+0x2F`. `set_unit_team(unit, id)` (0x657FD0), `kill_team`... are script wrappers outside the export.

### 11.4 Controllable-unit switching — `player_switch_unit(n)` 0x432300 (V)

Hero list = 4 names at 0x9F8B10 (stride 0x104, first `JOAN_OF_ARC`); `this->recno` = current controlled unit. `n == 0`: cycle to the next hero in the list order after the current one; `n > 0`: select hero `n-1`.
A candidate must exist (`0x51FD50(type)` -> recno), be alive, `+0x2F == 0`, `death_state != -1`, not a prisoner and be on screen (`0x717A90(pos)`) — the second `is_dead` test in the code refers to the *currently controlled* unit, so an off-screen candidate is accepted when the current hero is dead (inferred). Then: `0x42B6E0(1)` (leave control of the old unit), `0x42F9C0(newRecno)` makes it the main character (`DAT_00A5B9E8`), `DAT_00A57FAE = nation`, HUD/camera hooks, new unit: current order queue cancelled, `0x508B90(0,1)`, **its task is saved to `task_backup` (+0x2BE) and cleared; the previous unit's `task_backup` is restored into `+0x2A5`**; the new unit's team members get their orders cancelled and tasks cleared. A flag `DAT_00A48CE2 = 1`, `DAT_00A48CE0 = 0` is set after a successful switch.

## 12. AI

### 12.1 Think cadence — `unit_ai_think` 0x49D810 (V)

Skipped (4FD630 player hook instead) when (`rec == main char && ai_flag == 0`) or the unit has a rider. If the unit has an `Orders` object whose **queue is non-empty** the think is skipped entirely. If `think_forced == 0`: `if last_think_ms == 0: last_think_ms = now + rand(500)`; thinks only when `last_think_ms + UnitRes.ScriptDuration < now` (**strictly later**).
Think body: `expire_task` (0x528440); if `UnitRes.+0x125 (AI recno) != 0`: `handled = uscr_run_native_ai(rec, unit)`; if `handled == 0` and `unit.script != 0`: `0x63ED40()` then `script->vtable slot 3 called with unit` (inert `{exit}` in the shipped data); `last_think_ms = now`. With `think_forced != 0` it thinks every call then calls 0x4FD630.

### 12.2 Dispatch skeleton and per-class tables — see [UnitAi.md](../structs/UnitAi.md).

## 13. Gameplay.res `UNIT` / `EXP` / `KILL` / `GAME` constants (loader 0x51C680; values as shipped)

Loaded once by `0x51C680` through the Gameplay getters; ints with `atoi`, floats with `atof`. **Quirk: keys parsed with `atoi` that contain a fraction are truncated** (`STR_DAM 3.0 -> 3`, `DEX_DAM 1.5 -> 1`, `DEF_RAT 0.3 -> 0`) — they are stored in 16-bit globals; their readers (damage code) are not in the export.

| key | global | value | used by |
|---|---|---|---|
| BODY_R | A3CC9C | 32 | fallback body radius (sidestep, siege) |
| MAX_W_C / INIT_W_C | A3CCDC / A3CD04 | 30 / 5 | path wait limit / initial |
| WK_DIST | A3CCD8 | 50 | walk/run switch (path length) |
| HORSE_WD | A3CBE0 | 500 | same for horses |
| LD_FM_DT | A3CC74 | 160000 (d²) | formation recompute / follower resync distance² |
| REACH_DT | A3CC28 | 15 | arrival radius |
| FELL_DUR | A3CC88 | 2500 | knock-down duration; fell timer = /5 + rand |
| INVC_DUR | A3CBE4 | 6000 | post-knock-down invulnerability for rank > 3 |
| STR_HP / DEF_HP | A3CC1C / A3CCBA | 0 / 0 | HP per upgrade point |
| DEF_FALL | A3CCD0 | 0.2 | balance recovery per defense (inferred) |
| DEF_HP_R | A3CBF0 | 0.1 | HP regen per defense point |
| RECOV_HP | A3CC04 | 10000 | HP regen period |
| EGATK_MP / EGATK_HP | A3CD28 / A3CD1C | 40 / 20 | energy-attack cost |
| HATK_MP | A3CBD0 | 6 | unused in export |
| RECOV_MP / RECOV_T | A3CD20 / A3CCAC | 1 / 10000 | EP regen amount / period |
| ARW_ATH / ARW_TIME | A3CC40 / A3CC68 | 10 / 30000 | arrows stuck in a body: max / life |
| CLB_IDLE | A3CCE8 | 20 | idle after climbing |
| MAX_COM / COM_LIM / COM_TIME / COM_CONS | A3CC64 / A3CBC8 / A3CCC4 / A3CBD8 | 2000 / 200 / 1000 / 1 | command points |
| CRI_DIFF / CRITEXP | A3CC78 / A3CBD4 | 10 / 3.0 | critical flash HP drop / kill-exp factor for criticals |
| LEVELARR / LEVSTEP | A3CC58 / A3CC14 | 5000 / 300 | level arrow |
| AI_MOVE | A3CC70 | 1.2 | AI speed factor |
| MINDEXS / MAXDEXS | A3CD00 / A3CBDC | 30 / 200 | dexterity speed window |
| MOR_NOR / MOR_CH_U / MOR_CH_T | A3CC8C / A3CC80 / A3CD08 | 50 / 1.0 / 0.3 | morale |
| WP_DAMG / AMR_DEF / AMREXDEF / SIE_UDAM | A3CD14 / A3CC00 / A3CD0C / A3CBEC | 0 / 1 / 0.5 / 1.5 | damage model (0x5104F0, 0x509950, 0x509E90; §9.10) |
| F_ATTACK / F_DEFEND | A3CCF4 / A3CC2C | 130 / 70 | friendly bonus percents |
| IDLE2_CH / IDLE3_CH / IDLE_CC / STOP_CC | A3CC94 / A3CC30 / A3CCF0 / A3CCE4 | 0.2 / 0.4 / 0.01 / 0.3 | idle animation chances (no reader in export) |
| ARR_ACCU | A3CCA4 | 50 | arrow accuracy distance |
| BLOCKPER / BLOCKANG | A3CC90 / A3CCB0 | 50 / 70 | block percent / block half-angle (deg) |
| ARR_FALL / T_FALLF / V_FALLF / F_FALLR | A3CC08 / A3CC84 / A3CCC0 / A3CC34 | 25 / 6000 / 3000 / 250 | balance model |
| *_DI (12 keys) | A3CCD4 LINF, A3CC1A LCAP, A3CC5C HINF, A3CBE8 HCAP, A3CD18 ARC, A3CCCA ACAP, A3CC7C CRO, A3CBCC CCAP, A3CCB8 POLE, A3CCA0 PCAP, A3CCA2 MACE, A3CC38 MCAP | 10,100,12,100,8,100,8,100,10,100,8,100 | drop percentages (init) |
| PEFF_R | A3CCC8 | 7 | particle effect ratio (footsteps `7 * walk speed` distance threshold) |
| HP_PER / EP_PER / STR_PER / DEF_PER / DEX_PER / LDS_PER | A3CC7E / A3CC4C / A3CD1A / A3CC1E / A3CC18 / A3CC10 | 4 / 3 / 1 / 1 / 1 / 1 | UI per-point |
| STR_DAM / DEX_DAM / DEF_RAT | A3CCE2 / A3CCA8 / A3CCE0 | 3 / 1 / 0 (truncated) | damage model |
| FADE_IN1.. GLOW_IN3 | A3CCFC, A3CCF8, A3CC98, A3CD2C, A3CBFC, A3CBF8, A3CBF4 | 200,200,600,350,200,200,200 | CRITICAL HUD effect |
| INDANGER | A3CD24 | 0.2 | auto-potion HP fraction |
| UPG_TIME | A3CC24 | 600000 | AI upgrade delay |
| VISION_R | A3CC44 | 2000 | `Unit+0x3C1` vision range |
| LD_EXP / MEM_EXP | A3CCEC / A3CCBC | 0.4 / 0.4 | team experience share |
| BRI_COMM / BRI_CAPT | A3CD10 / A3CC54 | 5 / 10 | gold per bribe (common / captain) |
| FALL_S / FALL_E / DIST_P | A3CC0C / A3CC6C / A3CC20 | 800 / 2400 / 0.5 | arrow damage falloff start/end distance / max penalty |
| C_BONUSL / E_BONUSL / M_BONUSL | A3CCCC / A3CCB4 / A3CC3C | 50 / 100 / 3 | |
| EP_NOR / EP_CAP / EP_GEN | A3CC60 / A3CC48 / A3CC50 | 6 / 15 / 30 | kill EP gain by victim rank |
| {EASY,NORM,DIFF,VDIFF}_{D,A,B} | (floats) | D 0.7/1/1.5/2, A 0.5/1/1.5/2, B 0.5/1/1.5/2 | difficulty multipliers (damage, attack willingness, block) |

### 13.1 Float literals read from the data segment

| address | value | used for |
|---|---|---|
| 0x9D751C / 0x9D7518 / 0x9D7514 | 4.0e6 / 1.6e7 / 6.4e7 | LOD thresholds, squared distance (radii 2000 / 4000 / 8000) in `unit_compute_lod` 0x508880 |
| 0x9D74D0 | 33.0 | turn-step divisor (`unit_turn_step`) |
| 0x9D74CC / 0x9CFBA8 | 25.0 / 90.0 | turn caps |
| 0x9D51E8 | 1.3 | speed scale |
| 0x9D7494 / 0x9D7498 | 1.4 / 1.8 | far-LOD speed factors |
| 0x9D7C60 / 0x9D7C64 / 0x9D7C5C / 0x9CFC14 | -135 / -45 / 135 / 45 (deg) | hit-reaction sector angles (`unit_on_hit_event`) |
| 0x9D0750 / 0x9CFC20 | -10 / 10 (deg) | SHOOT order angle window |
| 0x9CFC0C / 0x9CFBA8 / 0x9CFC28 | -90 / 90 (deg) / 3600 (d², i.e. 60) | will-be-attacked window |
| 0x9D565C / 0x9D5660 / 0x9D5664 / 0x9D5668 | 2.25e6 / 1.225e7 / 6.25e6 / 250000 (= 1500², 3500², 2500², 500²) | path-timeout distance steps (APPROACH_PLACE etc.) |
| 0x9D7280 / 0x9D727C | 300 / 0.8 | see UnitAi.md |
| 0x9CF878 / 0x9D3968 | 5 / 90000 (= 300²) | see Unit.md |

The FPU-hidden literals of the damage code (`0x9CF9C4`, `0x9D5344`, `0x9D5360`, `0x9D7558`, `0x9CF794`, `0x9DD6D0`) are listed in §9.10 as inferred.

## 14. RNG

**Generator (V):** a 32-bit linear congruential generator held in a global RNG object (the ECX of `0x590540`), state at `object+0x7A`: `state = state * 0x015A4E35 + 1` (32-bit wrap), `rand15 = (state >> 16) & 0x7FFF` (0x5905A0, arithmetic shift of the signed state, then mask). Public forms:
* `rng_below(n)` 0x590540: `n * rand15 >> 15` (uniform `[0, n)`, integer multiply, so `n` up to 65535 is safe);
* `rng_range(lo, hi)` 0x590690(lo, hi): `lo + ftol(rand15 * k)` with the float constant hidden by the decompiler; its callers pass inclusive ranges such as `(0,3)`, `(5,7)`, `(8,10)`, `(0xB,0xD)`, `(2,4)`, `(0x32,100)`; consistent with `lo + rand15 * (hi - lo + 1) / 32768` (**I**, confirmed only by the argument conventions);
* `rng_float()` 0x590660: `rand15 * _DAT_009D8C10`; `rng_float_range(a, b)` 0x5906F0: `a + (b - a) * rand15 / _DAT_009D8C14` (the divisor is 32768.0 or 32767.0, **I**).
**Seeding (V):** `0x5904E0` calls `0x590510(seed)` with `seed = 0x7B33BF(0)`, the C runtime `time(NULL)`; `0x590510` is a plain store `object+0x7A = seed`. The generator is therefore seeded from wall-clock seconds at start-up and is not deterministic across runs (a deterministic port must seed it explicitly; replays / lockstep need a fixed seed per scenario). The object itself is a process-wide singleton passed implicitly (ECX); its address is not visible in the export (`TODO(re)`: the `thiscall` thunk loses it).
Callers of `rng_range` in this export (all inclusive ranges, call sites in battle/UI/effect code): unit_init (0,3) combo prefix; melee hit DOWNATK bonus (0x32,100); RTS/siege voices `(2,4)`, `(5,7)`, `(8,10)`, `(0xB,0xD)` (0x440B30, 0x44CFB0, 0x44EB80, 0x44F1F0, 0x44F750, 0x449B30, 0x44AAD0: choice among numbered voice lines such as `RTS_LTINF_00n`); 0x67DAA0 (stat range from a record's byte pair); 0x68A170/0x68BBB0 (effects, thunder).

| site | function | decides |
|---|---|---|
| 0x4F8550 | `unit_init` | initial combo prefix length `rand_range(0,3)`; drop roll `rand(100) <= pct`; weighted drop pick `rand(total_weight)` |
| 0x49D810 | `unit_ai_think` | first-think phase `rand(500)` |
| 0x54B480 | `unit_search` | target stickiness `rand(100) < 96`; candidate replacement `rand(100) < 51` / `< 81`; final commit `rand(100) < 51` |
| 0x4A95A0 | `unitlvl_ai_spend_points` | `rand(count)` combo sink |
| 0x4F7BF0 | sidestep | `rand(25)`, `rand(100)` x2 (+20), `rand(45)` |
| 0x4BD420 | APPROACH_ENEMY | extra distance `rand(p3)`, bearing `rand(270)` |
| 0x4BDF90 | APPROACH_PLACE | random point in the place (`rand(w)`, `rand(h)`) |
| 0x509000 | knock-down | which KNOCKDN variant `rand(1..3)` |
| 0x501190 | set_action | ATTACKED variant coin (arg hidden); `fell_timer` jitter (arg hidden) |
| 0x4FF9F0 | start_anim | idle start stagger `rand(5)*200` ms |
| 0x5048B0 | melee hit | back-attack roll `rand_range(50,100)` for DOWNATK |
| 0x509950 | melee damage | instant-kill proc `rand(100) < equip_bonus(0x14, attacker)` (the only RNG use in the damage code; 0x509E90 has none) |
| 0x4F8550 | unit_init (drops) | 12 drop rolls `rand(100) <= DI-percent` by item class (case 1..6 x two rank bands) |
| 0x464030 | weighted pick | `rand(total) + 1` against a weight list, decrement until <= 0 |
| 0x4FF9F0 | start_anim | `rand(5)` stagger |
| 0x49D810 / 0x4A95A0 / 0x67F380 / 0x49D810 | misc | `rand(500)`, `rand(count)`, `rand(record.+0x128)` |
| 0x75D540 | hit event | `rand(100) < equip_bonus(10)` evade; `rand(2)==1` knock-down in energy states; `rand(2)` in the ATTACK-vs-ATTACK-flash branch |
| 0x503680 | `search_for_item` | tie-breaking (3 sites, args hidden) |
| 0x462970 | player tick | random hint index |
| 0x535C20 .. 0x545D00 | `ai_clsN_decide_attack` and task functions | combo choice, shift choice, wandering (UnitAi.md) |
| 0x545590, 0x533B10 | citizen/cart wander | random destination offsets |
| 0x651010 | `random_number` builtin | script-level RNG (same generator) |

## 15. Cross-module requests

* **World/GameState/gameloop**: `DAT_00A500BE` is the per-frame step ms (0 when paused); `DAT_00A500BA` is the same clock used for deadlines. TIMING.md updated in round 2.
* **battle / gameplay**: kill-experience (`EP_NOR/EP_CAP/EP_GEN`, `CRITEXP`) has no reader in the unit/damage code; the caller of the death path or the hit-event consumer holds it. Please search xrefs of `0xA3CC60`, `0xA3CC48`, `0xA3CC50`, `0xA3CBD4`. Also the readers of `+0x9D` (0x509D40 multiplier writer) and of the stat object's crit flag `+0x1B4`.
* **Ghidra names to apply :** `0x509950 unit_take_melee_damage`, `0x509E90 unit_take_projectile_damage`, `0x5104F0 unit_weapon_base_melee`, `0x510BF0 unit_weapon_base_ranged`, `0x510750 unit_attack_type_mult`, `0x510E80 unit_melee_ignore_factor`, `0x510F10 unit_ranged_ignore_factor`, `0x510E40 unit_ranged_skill_mult`, `0x527DD0 unit_strength_weight`, `0x4B5520 nation_add_strength`, `0x4FE8C0 unit_set_morale`, `0x4FD630 unit_clear_think_flag`, `0x50DB80 unit_anim_start_sounds`, `0x4FC220 unit_set_mount`, `0x51A250 unit_set_link_a`, `0x454B20 unit_link_tick`, `0x590540 rng_below`, `0x590690 rng_range`, `0x5905A0 rng_next15`, `0x590510 rng_seed`, `0x5904E0 rng_seed_from_time`, `0x590660 rng_float`, `0x5906F0 rng_float_range`, `0x4BB3E0 order_can_interrupt`, `0x4BBBC0 order_id_from_name`, `0x4E4610 task_id_from_name`, `0x45E750 prof_rdtsc`.
* **items_facilities**: item-def fields used by the unit code: `+0x32` reach, `+0x36`/`+0x42` level fields, `+0x4E` heal amount / item sub-type (10 = box), `+0x2E` coin value, `+0xE2` pickable, `+0x4A..` bonus tables. `UpgradeRes` is on UnitLevel.md.
* **nation**: `0x47B600(n)` nation record (+0x10 unit count, `+0x152D` unit-id list), `0x4B6860(a,b)` hostility (0 = same nation, 1 if either is nation 0, else relation byte at `0x4B42E0(b)+1`).
* **animation family**: the `POSITION` key enum (0x4157C0) is not exported; the MOVEMENT/ITEM_USING/SPECIAL enums are in §6.1.
* **save family**: a unit is saved as `0x432` raw bytes + two 0x41-byte strings (Unit.md); runtime pointers must be reset on load (ProfileArray.md list).
* **Ghidra names to apply**: `0x51E280 units_update`, `0x52C490 unit_update`, `0x4F8550 unit_init`, `0x461F10 unit_destroy`, `0x661340 give_order`, `0x6619D0 give_task`, `0x4BAF80 orders_update`, `0x501190 unit_set_action`, `0x54B480 unit_search`, `0x49D810 unit_ai_think`, `0x64F320 unit_on_death`, `0x75D540 unit_on_hit_event`, `0x5048B0 unit_melee_hit`, `0x5051E0 unit_projectile_hit`, `0x522C40 unit_try_level_up`, `0x5229A0 unit_add_experience`, `0x4A95A0 unitlvl_ai_spend_points`, `0x52DAA0 unit_move_step`, `0x4F0B50 unit_apply_velocity`, `0x4F2190 unit_turn_step`, `0x4FA0F0 team_update_formation`, `0x432300 player_switch_unit`, `0x590540 rng_below`, struct `Unit` (0x4B4), `UnitRes` (0x203), `Task` (0x19), `OrderRecord` (0x21), `Team`.

## 16. Open questions (addresses)

Answered in round 2 (kept for traceability): scheduler phases and rdtsc counters (§3), damage/morale/strength (§9.10), the 22 interrupt predicates (§5.3a, except id 22), float constants (§13.1), RNG object/seed (§14), order and task names (§5.3), `0x4FD630` / `0x50DB80` / mount and link fields (§1.4, §9.11).

Still open:
1. The initial cursor/budget values of the 12 scheduler passes (saved `table` object). (Id 22 predicate closed in round 3: `return 1`.)
2. Kill experience is decoded (§9.12); what remains: the hidden `base` and morale expressions in 0x7C6240 (assumed victim `UnitRes.Experience`), the meaning of `DAT_00A5BA62` (chain counter), the address alias 0x7C6240 vs 0x64F320, writer of the per-weapon multiplier byte `+0x9D` (0x509D40) and of the crit flag `+0x1B4`.
3. The 0x590690 scaling literal (the damage-code literals were confirmed in round 3: 100, 0.01, 1e-4, 11, 2, 250).
4. `0x4FE510` hidden argument expression (assumed `hp - dmg`) and the `0x4FE8C0` callers' morale expressions.
5. `Task.state` (+0x2BA) writers; task ids 16 and 19 (unnamed, internal).
6. Whether the leader factor in 0x509950 / 0x509E90 refers to the victim's or the attacker's leader.
7. `link_b` (+0x1F9) setter and the meaning of action 0x68; `Unit+0x356`.
8. `Unit+0x0A`, `+0x26`, `+0x2D9`, `+0x2E5`, `+0x3B8` writers (see Unit.md `R` rows).
