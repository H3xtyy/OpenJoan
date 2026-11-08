# Unit (`unit.cpp`) — the runtime unit record, sizeof = **0x4B4** (1204), `#pragma pack(1)`

The in-memory record of every soldier, hero, civilian, horse and siege weapon. (The earlier docs call it "actor" in [../functions/script_res.md](../functions/script_res.md) and
"SaveGame" in [ProfileArray.md](ProfileArray.md); both are this struct — `Unit+0x3B` is the unit-type id, `Unit+0x299` the AI `Script*`, and the save-slot field list in ProfileArray.md
(`+0x213 +0x215 +0x21B +0x2A1 +0x351 +0x378 ...`) is exactly the set of runtime pointers/states that must be reset after loading a unit image.)

* ctor `0x4155E0` (`this` = 0x4B4-byte block from `operator new(0x4B4)`; it also constructs two path sub-objects (0x66E4B0) and registers the raw-save region with the global
  singleton at `0xA45FA8`: `{base=this, 0, len=0x4B4, 2 string buffers at +0x432 and +0x473, 0x41 bytes each}` — i.e. a unit is **saved as a 0x432-byte memory image plus two 0x41-byte strings**).
* factory `0x51DFC0(type_id)`: `new Unit`, optional multiplayer-style re-use check, then `unit::fn_004F8550(type_id)` (**the initialiser**, §A) and insertion in the unit table (`dynarrb`,
  1-based `recno` stored at `Unit+0`).
* destroy `0x461F10` (`unit::fn_00461F10`, §B) releases everything the unit owns; table removal `0x51E1F0(recno)`.
* unit table accessors (this = table object): `0x41B3F0(recno)` -> `Unit*` or NULL (bounds `1..count`, count at table+0xC, stride at +0x10, data at +0x19);
  `0x41B3C0` same (used when validity is already known); **`0x428B70(recno)` -> true when the recno is invalid/dead-removed**; `0x417AD0` -> count.
* type record accessor `0x42D0A0(Unit+0x3B)` -> [UnitRes](UnitRes.md).

Evidence tags: **V** verified (>= 2 functions agree), **R** read in one function, **I** inferred. Offsets in hex. `rec` = the unit's `recno`.

## Fields

| offset | size | type | name | meaning | status / evidence |
|---|---|---|---|---|---|
| 0x000 | 2 | i16 | `recno` | 1-based id in the unit table; compared with `DAT_00A5B9E8` (main character recno) everywhere | V |
| 0x002 | 2 | i16 | `nation` | nation recno (0 = none/neutral). `0x47B600(nation)` -> nation record (unit list, counters); hostility via `0x4B6860(a,b)` | V 0x461F10, 0x509250, 0x5094D0 |
| 0x004 | 2 | i16 | `place_recno` | place/facility the unit belongs to (guard post) | R 0x4FA0F0 |
| 0x006 | 2 | i16 | `place_slot` | slot index inside that place; with task ATTACK_PLACE/DEFEND_PLACE and `task.p1 == place_recno` the formation code uses the place slot position (`0x4C2470(slot)`) instead of the grid | R 0x4FA0F0 |
| 0x00A | 4 | i32 | `cur_anim_entry` | 1-based animgrp entry index of the running animation (`0x4168D0(entry)`; entry+0x72 = animscr recno) | I 0x5048B0, 0x509D90, 0x4F2190 |
| 0x00E | 12 | f32[3] | `pos` x,y,z | world position (`0x4FEDB0` copies it, `0x4FEDF0(x,y,z)` writes it — through the actor when `+0x2F5 != 0`). **Ground plane = (x,y); z is height** (cylinder `CenterZ`) | V |
| 0x01A | 4 | f32 | `heading` | facing in **radians** (`0x4FF850` getter, `0x4FF870(rad)` setter — rejected if the actor model cannot be rotated, 0x6AF190) | V |
| 0x01E | 4 | f32 | `target_heading` | desired facing in **degrees** (`0x50ABD0` setter, `0x50AC00` getter); `0x4F2190` turns `heading` towards it | V |
| 0x022 | 4 | i32 | `lod` | update-detail class 1..4 computed each tick by 0x508880 (1 = near) | V 0x51E280 |
| 0x02A | 4 | u32 | `last_update_ms` | `DAT_00A500BA` at the end of unit phase 1; used as `dt` by the turn step | V 0x52C490, 0x4F2190 |
| 0x02E | 1 | u8 | `cutscene_run` | 1 = keeps being simulated during a cut scene (`set_unit_can_process_in_cut_scene`) | V 0x51E280 |
| 0x02F | 1 | u8 | `team_cutscene_run` | team variant of the same flag (cleared by leave-team 0x42FCB0); a candidate for controllable-unit switching must have it 0 | I 0x432300 |
| 0x031 | 2 | i16 | `team_id` | script team id (`set_unit_team`); **9999 = free/unassigned pool** (auto-recruit targets) | R 0x4655A0, 0x438F80 |
| 0x033 | 4 | Team* | `team` | non-NULL only on the **leader**; see "Team" below. `0x44A660()` = `team != 0` = is_leader | V |
| 0x037 | 2 | i16 | `leader` | leader recno (0 = none). `0x4216D0()` returns the leader `Unit*` (self when this is a leader) | V |
| 0x039 | 2 | i16 | `team_flag` | 0 when joined by `0x438F80`; 1 when the team was disbanded by `0x42DD70` | R |
| 0x03B | 2 | i16 | `unit_type` | [UnitRes](UnitRes.md) recno | V |
| 0x03D | 0x42 | char[] | `name` | copy of `UnitRes.Name` | R |
| 0x07F | 0x42 | char[] | `short_name` | copy of `UnitRes.ShortName` | R |
| 0x0C1 | 0x21 | char[] | `title` | copy of `UnitRes.Title` | R |
| 0x102 | 2 | i16 | `ai_flag` | **0 = player-controllable slot** (when `rec == main char` the unit is driven by input, no AI think, no AI speed boost); non-zero = AI unit. `set_control_flag` | V (0x49D810, 0x4F0B50, 0x54B480, 0x501190) |
| 0x104 | 4 | i32 | `rank` | = `UnitRes.Ranking` (0 infantry .. 4 general); set by `0x5089E0(rank)` which also re-registers the unit in the nation and, for rank > 2, makes it leave its team. `> 3` = generals: item pickup (0x49DA30), LOD tier C/H, boss invulnerability, longer path waits | V |
| 0x108 | 2 | i16 | `kingdom_id` | | R |
| 0x10A | 2 | i16 | `race_id` | | R |
| 0x114 | 4 | i32 | `death_state` | 0 normal. **-1** = invulnerable / not targetable (AI target filters skip it; control switch skips it). **-2** = corpse kept (not freed by the unit pass). **1** = "revive once" marker: on death the unit gets HP 1, `invuln_ms = 0x7FFFFFFF`, task := IDLE and the state becomes -1 | V 0x54B480, 0x432300, 0x51E280, 0x64F320 |
| 0x118 | 4 | u32 | `last_hp_regen_ms` | HP regen timer (reset to `now` while HP is full or `death_state == -1`) | V 0x52C490 |
| 0x11C | 4 | i32 | `lives_max` | `UnitRes.Life` | R |
| 0x120 | 4 | i32 | `lives` | extra lives left; `is_dead()` (0x42D0E0) = `hp < 1 && lives == 0` | V |
| 0x124 | 2 | u16 | `level` | current level (UnitRes.Level) | V |
| 0x126 | 4 | i32 | `experience` | total experience; init = `row(level-1).exp` or 0 for level 1 | V 0x4F8550, 0x5229A0, 0x522C40 |
| 0x12A | 2 | i16 | `skill_points` | | V 0x4949F0, 0x522C40 |
| 0x12C | 2 | i16 | `attr_points` | | V |
| 0x12E | 2 | u16 | `unitres_19b` | copy of the unparsed UnitRes+0x19B (0) | R |
| 0x130 | 1 | u8 | `levelup_flag` | set on level-up, cleared when `now - levelup_time >= LEVELARR` (5000 ms) | V 0x522C40, 0x52C490 |
| 0x131 | 4 | u32 | `levelup_time` | `now` at the last level-up | V |
| 0x135 | 4 | i32 | **`hp`** | current hit points. Setter `0x4FE510(v)` clamps to `[0, max_hp()]`; if the clamped value is 0 **and `lives != 0`** then `lives--`, `hp = hp_mark = max_hp()` (instant in-place respawn) | V |
| 0x139 | 4 | i32 | `hp_mark` | HP value at the last "big hit" check; `0x416680` (main char only): if `hp_mark - hp > CRI_DIFF (10)` -> HUD critical flash + `hp_mark = hp` | V |
| 0x13D | 4 | i32 | `base_max_hp` | `UnitRes.HP` (+ upgrades); `max_hp() = 0x523320 = base_max_hp + equipment_bonus(stat 1)` | V |
| 0x141 | 4 | i32 | `regen_hp` | `UnitRes.RegenHp` (no exported reader) | R |
| 0x145 | 4 | i32 | **`ep`** | current energy points; setter `0x4FE5D0` clamps `[0, max_ep()]` | V |
| 0x149 | 4 | i32 | `base_max_ep` | `UnitRes.EP`; `max_ep() = 0x523700 = base + bonus(stat 2)` | V |
| 0x14D | 4 | i32 | `regen_ep` | `UnitRes.RegenEp` | R |
| 0x151 | 4 | u32 | `last_ep_regen_ms` | EP regen timer | V |
| 0x155 | 2 | i16 | `strength` | (setter 0x4FE6E0) | V |
| 0x157 | 2 | i16 | `dexterity` | (0x4FE710) — drives walk/run speed (§Movement) | V |
| 0x159 | 2 | i16 | `defense` | (0x4FE740); `defense_total() = 0x5238C0 = defense + bonus(stat 5)` is also the HP-regen driver | V |
| 0x15B | 2 | i16 | `leadership` | (0x4FE770) | V |
| 0x15D | 2 | i16 | `armor_id` | UnitRes.EArmorId | R |
| 0x15F | 2 | i16 | `armor_item` | UnitRes.EArmorCode | R |
| 0x161 | 2 | i16 | `loyalty` | | R |
| 0x163 | 2 | i16 | `morale` | 0..100; setter `unit_set_morale` 0x4FE8C0 clamps (and a leader's equipment can set a floor, functions/unit.md §9.10.4); adjusted on deaths (MOR_CH_U 1.0 / MOR_CH_T 0.3); read by both damage functions as `(morale + 100 - MOR_NOR 50) / 100` | V 0x509950, 0x509E90, 0x4FE8C0 |
| 0x165 | 2 | i16 | `reputation` | | R |
| 0x167 | 4 | i32 | `mode` | **1 ATTACK, 2 ACTIVE, 3 IDLE** (`set_unit_mode`); `is_attack_mode` = ==1, `is_idle_mode` = >1. The AI flips ACTIVE<->ATTACK on enemy proximity | V |
| 0x16B | 4 | i32 | `target_select` | 1 NEAREST, 2 WEAKEST, 3 IMPORTANT, 4 NEAREST_ARCHER, 5 NEAREST_HIGH_PRIORITY (`set_unit_attack_behaviour`); consumed by 0x54B480 | V |
| 0x16F | 4 | i32 | `archery_skill` | (0x4FE7A0) | V |
| 0x173 | 4 | i32 | `horse_riding` | (0x4FE7D0) | V |
| 0x177 | 4 | i32 | `swimming` | (0x4FE800) | V |
| 0x17B | 4 | i32 | `elephant_riding` | | V |
| 0x17F | 4 | i32 | `wide_animal` | | V |
| 0x183 | 4 | i32 | `preaching` | | V |
| 0x187 | 4 | i32 | `power_jump` | | V |
| 0x18B | 4 | i32 | `disguise` | | V |
| 0x18F | 4 | i32 | `persuade` | | V |
| 0x193 | 4 | i32 | `lock_picking` | | V |
| 0x197 | 4 | i32 | `mountain` | | V |
| 0x1B5 | 2 | i16 | | zeroed at init | R |
| 0x1B7 | 2 | i16 | | zeroed at init | R |
| 0x1BD | 2 | i16 | `hand_item[0]` | item recno held in hand 0 (set by `0x50A480(item, level, 0)`; hand slot 0/1) | V 0x461F10, 0x50A480 |
| 0x1BF | 2 | i16 | `hand_item[1]` | hand 1 | V |
| 0x1C1 | 4 | Equipment* | `equip` | see "Equipment"; allocated for **every** unit by `unit_init` (all four weapon slots may be empty) | V |
| 0x1C5 | 2 | u16 | `drop_item` | item type dropped on death (rolled at creation, §A) | R |
| 0x1C7 | 4 | i32 | `drop_level` | its level | R |
| 0x1CB | 2 | i16 | `balance` | **knock-down meter 0..100** (init `UnitRes.Falling`). Setter `0x4FEA80(v)` clamps `[0,100]`; at 0 -> knock-down animation (`0x509000(0)`), or for a rider `0x4E` + dismount. Regenerates once per second while `< 100` (0x52C490; the added amount is an FPU expression not visible in the export — while knocked down it involves `defense_total()` (`DEF_FALL 0.2`), otherwise a fixed value; `TODO(re)`). Each melee hit lowers it by `(att_force + vert_force)/F_FALLR` (see functions/unit.md §9) | V (clamp/regen cadence), I (amounts) |
| 0x1CD | 4 | u32 | `balance_regen_ms` | | V |
| 0x1D1 | 4 | i32 | `fell_timer` | ms left on the ground after a knock-down; set in 0x501190 to `FELL_DUR/5 + rand` for actions 0x40..0x42/0x4E | R |
| 0x1DD | 4 | i32 | `invuln_ms` | **post-knock-down invulnerability countdown**; decremented by `DAT_00A500BE` each tick; `> 0` makes `0x5048B0`/`0x5051E0` ignore hits. Set to `max(cur, INVC_DUR=6000)` for rank > 3 on knock-down | V |
| 0x1E1 | 1 | u8 | `downed_hit` | set when a DOWN_ATTACK landed on a prone unit | R 0x75D540 |
| 0x1E2 | 4 | i32 | `mount` | recno of the horse this unit rides (0 = on foot). Written only by `unit_set_mount` 0x4FC220. `0x430190()` returns it | V |
| 0x1E6 | 4 | i32 | `rider` | on a horse unit: recno of its rider (written by `horse_set_rider` 0x482930). Units with a rider skip AI (0x49D810) | V |
| 0x1EA | 1 | u8 | `mounted_flag` | 0xFF while mounted, 0 after dismount (set by 0x4FC220) | V |
| 0x1EB | 1 | u8 | `dismounted_flag` | 0xFF after dismount, 0 while mounted (complement of 0x1EA) | V |
| 0x1EC | 1 | u8 | `mount_pending` | with `+0x1ED` deadline: delayed mount/dismount animation | R |
| 0x1ED | 4 | u32 | `mount_pending_ms` | | R |
| 0x1F5 | 4 | i32 | `link_a` | cart/escort pair link; written by `unit_set_link_a` 0x51A250 (partner side checked by 0x51A8A0); read by `unit_link_tick` 0x454B20; cleared in destroy | V |
| 0x1F9 | 4 | i32 | `link_b` | crew-to-weapon link: when the unit is IDLE and the link names a valid unit, `unit_link_tick` starts action 0x68 CA_IDLE; no exported setter | R |
| 0x213 | 2 | i16 | `anim_position` | animation-table key `POSITION`: 0 `STAND`, 1 `JUMP` (airborne); written only by `set_anim_position` 0x500D10 from `unit_set_action` (UP -> 1, DOWN / hits / death -> 0; functions/unit.md §6.3) | V 0x504010, 0x501190 |
| 0x215 | 2 | i16 | **`action`** | current action id = animation-table `MOVEMENT` code, enum in functions/unit.md §6. Init 1 (IDLE) | V |
| 0x217 | 2 | i16 | `action_alt` | alternative movement action when `+0x357 != 0` | R 0x4F0B50 |
| 0x219 | 2 | i16 | `prev_action` | previous action (set by 0x501190 before overwriting `+0x215`) | V |
| 0x21B | 2 | i16 | `anim_item_using` | animation-table `ITEM_USING` key (1SWORD, BOW...; init 1) | V |
| 0x21D | 2 | i16 | `anim_special` | animation-table `SPECIAL` key (init 0) | V |
| 0x21F | 4 | u32 | `anim_start_ms` | scheduled start (idle actions get `rand(5)*200` ms stagger, not for the main char) | R 0x4FF9F0 |
| 0x223 | 4 | i32 | `anim_loop` | loop flag chosen by the animation selector | R |
| 0x227 | 6+ | char[] | `combo_buf` | combo string being executed (`'A'`,`'B'`...; init 0xFF first byte) | V |
| 0x22D | 7 | char[] | `combo_pre` | pre-filled combo chars `'A'`; init: first `rand(0..3)` bytes = `'A'` and `+0x234 = n` | R 0x4F8550 |
| 0x233 | 1 | u8 | `combo_len` | combo chars consumed so far; 0xFF = combo closed by `unit_anim_tick` (the next letter restarts the count; `combo_reset` 0x50D0D0 keeps 0xFF) | V 0x7C7AB0, 0x50D0D0 |
| 0x234 | 1 | u8 | `combo_pre_len` | | R |
| 0x237 | 4 | i32 | `anim_aux0` | animation-table entry fields +0x24/+0x28 copied by 0x504010 | R |
| 0x23B | 4 | i32 | `anim_aux1` | | R |
| 0x23F | 4 | i32 | `combo_end_ms` | sim time the last combo was closed (`unit_anim_tick`); zeroed at init | V 0x7C7AB0 |
| 0x243 | 4 | i32 | `gravity_level` | `ftol(animscr.GravityLevel)` of the chosen entry (not a clip length; corrected 2026-10-22 from the animscr record layout) | R 0x504010 |
| 0x247 | 2 | i16 | `think_forced` | non-zero = think every call; `0x4FD630` (called after such a think) only writes 0 here | V 0x49D810, 0x4FD630 |
| 0x299 | 4 | Script* | `script` | AI script object (unitAI.res `rec+0x22`); in the shipped game the fall-back script is `{exit}` | V |
| 0x29D | 4 | u32 | `last_think_ms` | 0 -> first think scheduled at `now + rand(500)`; 1 forces an immediate think | V |
| 0x2A1 | 4 | Orders* | `orders` | order queue + current order (0x3E bytes), created lazily by `give_order` | V |
| 0x2A5 | 0x19 | Task | `task` | **current task**, see "Task" (id +0x2A5, priority +0x2A9, p1..p3 +0x2AA/2AE/2B2, deadline +0x2B6, state +0x2BA) | V |
| 0x2BE | 0x19 | Task | `task_backup` | task saved when the controllable hero is switched away; restored on switching back (0x432300) | V |
| 0x2D7 | 2 | i16 | `target` | current attack target (unit recno), cleared when invalid; filled by search type `'E'` and by hit reactions | V |
| 0x2D9 | 2 | i16 | | | R |
| 0x2DB | 2 | i16 | `shoot_target_unit` | ranged-attack target (unit) set by order SHOOT p1==1 | R 0x4BD090 |
| 0x2DD | 2 | i16 | `shoot_target_item` | ranged target (item/gate/wall) for SHOOT p1==2 | R |
| 0x2DF | 2 | u16 | `attack_chance` | **aggressiveness meter (percent)**. Reset to `0x511E70()` (base by rank, §Combat AI) when the unit attacks; `+10` when a melee hit lands on it, `+5` for an arrow, doubled when knocked down | V |
| 0x2E1 | 1 | u8 | `prisoner` | prisoner flag (`0x432B90()`); prisoners are skipped by AI and cannot be hit unless `+0x358` | V |
| 0x2E2 | 1 | u8 | `prisoner_pose` | selects action 7 (IDLE_T) vs 1 for prisoners (0x508B90) | R |
| 0x2E3 | 2 | u16 | `hit_by` | recno of the attacker whose TRIGGER_ATTACK event hit this tick; cleared at phase 0 | R 0x52C490, 0x75D540 |
| 0x2E5 | 2 | u16 | | cleared at phase 0 | R |
| 0x2E7 | 2 | i16 | | zeroed at init | R |
| 0x2ED | 1 | u8 | `hittable` | 0 -> hit tests return 0 (cleared by destroy / death); set to 1 with `+0x2EE` by `unit_instantiate` 0x442820 | R 0x5048B0, 0x461F10, 0x442820 |
| 0x2EE | 1 | u8 | `instantiated` | 1 once `unit_instantiate` 0x442820 spawned the actor (functions/unit.md §6.3) | V 0x442820 |
| 0x2F1 | 4 | AnimGroup* | `anim_group` | animation group of the current model (entry count at +0x2C, `0x4168D0`) | V |
| 0x2F5 | 4 | Actor* | `actor` | the en3d [Actor](Actor.md) (0x10C bytes); NULL while not instantiated; `actor+0x5C` = animation controller, `+0xB8` = collision shape type, `+0x30/+0x34/+0x38` fade rate/max/start | V |
| 0x2F9 | 4 | ptr | `fx_a` | attached effect/actor (zeroed with +0x301) | R |
| 0x309 | 4 | ptr | `fx_b` | (zeroed with +0x311) | R |
| 0x319 | 4 | Actor* | `shadow_actor` | shadow model actor | R 0x461F10, 0x64F320 |
| 0x31D | 4 | obj* | `fx_c` | | R |
| 0x321 | 4 | Actor* | `attach_actor` | weapon/attachment actor; recreated by 0x501190 per animation | R |
| 0x325 | 4 | Actor* | `armor_actor` | second attached actor | R |
| 0x329 | 4 | i32 | `item_ref` | item whose use-effects are released on destroy | R |
| 0x32D | 4 | obj* | `hand_fx[0]` | per-hand weapon trail/effect handle (updated by 0x687A60) | R |
| 0x331 | 4 | obj* | `hand_fx[1]` | | R |
| 0x335 | 4 | World* | `world` | owning [World](World.md) (`DAT_00A45F3E` while active); the unit pass skips units whose world differs | V |
| 0x339 | 4 | WorldBlock* | `block` | terrain block the unit stands in (spatial hash); doubly linked through `+0x33D prev` / `+0x341 next` in the block's unit list (block+0x8C head / +0x90 tail) — `0x4FEE60` relinks when the position changes block | V |
| 0x345 | 4 | Scene* | `scene` | en3d scene the actors are attached to (re-parented on block change) | V |
| 0x351 | 4 | u32 | `idle_resume_ms` | time the unit started waiting for the move animation (1000 ms timeout then re-issue idle) | R 0x52DAA0 |
| 0x355 | 1 | u8 | `anim_done` | movement-animation finished/stalled marker | R |
| 0x356 | 1 | u8 | `mount_busy` | cleared by `unit_set_mount` whenever a mount/dismount starts; other writers unknown | R 0x4FC220 |
| 0x357 | 1 | u8 | `use_action_alt` | if set, the movement step uses `+0x217` instead of `+0x215` | R |
| 0x358 | 1 | u8 | `prisoner_attackable` | `set_prisoner_can_be_attacked_flag` | V (BUILTINS) |
| 0x365 | 12 | f32[3] | `zero_vec` | init from the zero vector `DAT_00A69CD8` | R |
| 0x371 | 2 | u16 | `hit_counter` | set to 1 by the hit-reaction code | R |
| 0x373 | 1 | u8 | | cleared in 0x501190 | R |
| 0x378 | 4 | i32* | `crew_unit[n]` | on siege weapons: recno of the unit at each attach point (n = `UnitRes.AttachPointCount`), allocated at init (`n*4`, zeroed) | V |
| 0x37C | 4 | i32* | `crew_aux[n]` | second per-slot array | V |
| 0x380 | 4 | i32 | `weapon_slot_token` | `-1` none; set by LARGE_WEAPON_PUSH while a crew slot is reserved (cleared by 0x528300) | V |
| 0x384 | 4 | i32 | `weapon` | recno of the siege weapon this unit operates | V |
| 0x388 | 4 | i32 | `weapon_slot` | attach-point index on that weapon (`dist_to_attaching_point` builtin) | V |
| 0x38C | 2 | i16 | | 1 when the crew arrays were allocated | R |
| 0x38E | 2 | i16 | `mount_stick` | 1 while a mount transition waits for the rider | R 0x52C490 |
| 0x3A0 | 4 | u32 | `snd_last_ms_a` | last-played stamp of a sound slot (min-gap `rec+0x81` check in `unit_anim_start_sounds` 0x50DB80; used for the "above water" footstep variant) | R 0x50DB80 |
| 0x3A8 | 4 | u32 | `fx_last_ms` | last footstep/effect spawn (>2000 ms gap) | R 0x416680 |
| 0x3AC | 4 | u32 | `snd_last_ms_b` | last-played stamp (sound slot 1, ground variant), 0x50DB80 | R 0x50DB80 |
| 0x3B0 | 4 | u32 | `snd_last_ms_c` | last-played stamp (sound slot 2, ground variant), 0x50DB80 | R 0x50DB80 |
| 0x3B8 | 4 | i32 | `scripted_move` | non-zero -> the movement step moves straight to `+0x3CD` instead of following a path | R 0x4F0B50 |
| 0x3BC | 4 | u32 | `upg_timer` | `now` at the last level-up; AI units spend points after `UPG_TIME` | V |
| 0x3C0 | 1 | u8 | | init 1 | R |
| 0x3C1 | 4 | i32 | `vision_range` | init = `VISION_R` 2000 | R |
| 0x3C5 | 4 | u32 | `collision_id` | handle passed to 0x527690 on destroy | R |
| 0x3C9 | 4 | i32 | `move_group` | order parameter p4 of the current move order; inherited from the leader in `get_curr_task` | R |
| 0x3CD | 12 | f32[3] | `scripted_target` | target of `scripted_move` | R |
| 0x3D9 | 12 | f32[3] | `standoff_point` | APPROACH_ENEMY chosen standoff point (`0x3E6`-style cache, valid while order p4 != 0) | R 0x4BD420 |
| 0x3E5 | 1 | u8 | | init 1 | R |
| 0x3E6 | 4 | i32 | `path_index` | index of the next waypoint of the current path (0 = none) | V |
| 0x3EA | 4 | i32 | | zeroed with `path_index` by 0x422F10 | R |
| 0x3EE | 4 | i32 | `sidestep_phase` | 0..3 round-robin of the blocked-path sidestep (0x4F7BF0) | V |
| 0x3F2 | 4 | i32 | | set 1 after a sidestep; cleared by 0x422F10 | R |
| 0x3F6 | 4 | i32 | `wait_count` | frames/ticks left to wait while blocked; init `INIT_W_C` | V |
| 0x3FA | 4 | i32 | `wait_limit` | current wait threshold: init `INIT_W_C`(5), +1 per blocked retry up to `MAX_W_C`(30) then reset | V |
| 0x3FE | 2 | i16 | | set 20 when the "turn-on-spot" flag `+0x202` cannot be cleared | R |
| 0x400 | 2 | i16 | | | R |
| 0x402 | 4 | i32 | `stuck_count` | pushes counted while the next waypoint is not reached | R 0x52DAA0 |
| 0x406 | 4 | u32 | `stuck_ms` | time of the first push (500 ms window) | R |
| 0x40A | 4 | f32 | `leader_heading_ref` | reference heading for sidesteps (deg) | R 0x4F7BF0 |
| 0x40E | 4 | f32 | `path_remaining` | remaining path length; `<= WK_DIST (50)` selects WALK else RUN | R 0x52DAA0 |
| 0x412 | 4 | f32 | `form_off_x` | formation slot offset (rotated into world axes), written by 0x4FA0F0 | V |
| 0x416 | 4 | f32 | `form_off_y` | | V |
| 0x41E | 4 | f32 | `form_width` | formation width (`(cols-1)*100`) | V |
| 0x422 | 4 | f32 | `form_slot_x` | unrotated slot offset | V |
| 0x426 | 4 | f32 | `form_slot_y` | | V |
| 0x42E | 2 | i16 | `model_id` | resource id released with 0x4B16A0 on destroy | R |
| 0x432 | 0x41 | char[] | `save_str_a` | raw-save string buffer | R 0x4155E0 |
| 0x473 | 0x41 | char[] | `save_str_b` | raw-save string buffer | R |
| 0x4B4 | | | end | | |

Unlisted gaps (e.g. 0x0E1..0x101, 0x2EE..0x2F0, 0x3E7..) are not touched by any exported function: `TODO(re)`. The fields `+0x1F5/+0x1F9` and `+0x2F9..+0x32D` need a follow-up pass on the mount/prisoner/effect code (0x454B20, 0x51A250, 0x51A8A0, 0x4FC220, 0x482930).

## Task (0x19 bytes, at `Unit+0x2A5`; also the type of `0x4E4520` "this" and of the local temporaries in `give_task`)

`0x4E4520(id, priority, p1, p2, p3, deadline)` writes the struct and zeroes `state`.

| offset | size | type | name | meaning |
|---|---|---|---|---|
| +0x00 | 4 | i32 | `id` | task id (enum below); 0 = none |
| +0x04 | 1 | i8 | `priority` | signed; a new task replaces the current one only if `cur.priority < new.priority` or the current one expired (`deadline != 0 && deadline < now`) — **equal priority is refused** (0x527F10) |
| +0x05 | 4 | i32 | `p1` | e.g. place recno / target unit / x |
| +0x09 | 4 | i32 | `p2` | e.g. y |
| +0x0D | 4 | i32 | `p3` | e.g. z / radius |
| +0x11 | 4 | u32 | `deadline` | `now + ms`, 0 = never; `0x528440` (run at the start of every think) zeroes the task once `deadline < now` |
| +0x15 | 4 | i32 | `state` | `get_curr_task_state` (0x652280) returns it. **No exported writer other than the zeroing in 0x4E4520** -> always 0 in the shipped logic; `TODO(re)` |

Task ids (`0x4E4610` name ladder; case-insensitive): 1 ATTACK_PLACE, 2 ATTACK_UNIT, 3 RECRUIT_TROOP, 4 HELP_TROOP, 5 `<0x9D71DC>` (FLEE per unitAI.md), 6 DEFEND_PLACE, 7 `<0x9CF478>` (**IDLE**),
8 TOWER_ATTACK, 9 SCOUT, 10 FOLLOW_UNIT, 11 RETREAT, 12 RTS_MOVE_TO, 13 RTS_ATTACK_UNIT, 14 APPROACH_PLACE, 15 LARGE_WEAPON_PUSH, 17 RTS_ATTACK_ITEM, 18 DEFEND_UNIT, `SPECIAL<n>` = 100+n,
unknown = 0. Id 16 and **19 (0x13)** have no name: 19 is dispatched by every class handler but no exported code creates it.
Followers inherit the leader's task: `get_curr_task` (0x6520D0) and `get_curr_task_para` (0x652190) read the **leader's** fields when the unit's own task id is 0 (except leader task RTS_MOVE_TO -> 0).

## Orders object (0x3E bytes) and Order record (0x21 bytes)

`Unit+0x2A1 -> Orders` = `{ dynarr header (0x1D bytes, elem size 0x21, grow 10; count at +0x0C), current Order record at +0x1D (0x21 bytes) }` (ctor 0x4BAB50 memset).
Order record layout (queue elements and the current slot):

| record offset | in current slot (`Orders+`) | type | name | meaning |
|---|---|---|---|---|
| +0x00 | 0x1D | i32 | `id` | order id (0 = none) |
| +0x04 | 0x21 | i8 | `priority` | signed |
| +0x05 | 0x22 | i32 | `p1` | |
| +0x09 | 0x26 | i32 | `p2` | |
| +0x0D | 0x2A | i32 | `p3` | |
| +0x11 | 0x2E | i32 | `p4` | |
| +0x15 | 0x32 | i32 | `follower_flag` | 1 on copies broadcast to team members; passed to the path request as "follow leader" |
| +0x19 | 0x36 | i32 | `leader` | recno of the unit that broadcast it |
| +0x1D | 0x3A | u32 | `deadline` | `now + duration_ms` |

Order ids (name parser `0x4BBBC0`, strcmp ladder, case-insensitive; names confirmed in round 2): 1 COMBO_ATTACK, 2 SHOOT, 3 JUMP, 4 BLOCK, 5 MOVE, 6 SHIFT, 7 MOVE_TO_TARGET, 8 MOVE_TO_RELATIVE_TARGET, 9 MOVE_ON_TRACK,
10 TURN, 11 IDLE, 12 APPROACH_ENEMY, 13 APPROACH_ITEM, 14 APPROACH_PLACE, 15 FOLLOW_FORMATION, 16 FOLLOW_UNIT,
17 GATE_OPEN, 18 RECRUIT_TROOP, 19 DOWN_ATTACK, 20 ENERGY_ATTACK, 21 RUSH_ATTACK, 22 SPECIAL_MOVEMENT, unknown = 0. Handlers and the `order_can_interrupt` predicate table: [../functions/unit.md](../functions/unit.md) §5.3 / §5.3a.

Task ids (name parser `0x4E4610`, same compare helper `0x7C07B0`): 1 ATTACK_PLACE, 2 ATTACK_UNIT, 3 RECRUIT_TROOP, 4 HELP_TROOP, 5 FLEE, 6 DEFEND_PLACE, 7 IDLE, 8 TOWER_ATTACK, 9 SCOUT, 10 FOLLOW_UNIT, 11 RETREAT, 12 RTS_MOVE_TO, 13 RTS_ATTACK_UNIT, 14 APPROACH_PLACE, 15 LARGE_WEAPON_PUSH, 17 RTS_ATTACK_ITEM, 18 DEFEND_UNIT, `SPECIAL<n>` = 100 + n (suffix via atoi 0x7B15CB); ids 16 and 19 have no name (internal only); unknown name = 0.

## Team (allocated per leader, `Unit+0x33`)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | i16 | `leader` | recno of the leader (0x4FA0F0 reads `*team`) |
| 0x02 | 1 | i8 | `count` | members (**max 20**, 0x14: recruit loops stop at `< 0x14`) |
| 0x03 | 40 | i16[20] | `members` | member recnos (the leader included? 0x4216D0 treats the leader as separate; broadcast loops skip `== self`) |
| 0x2B | 12 | i16[6] | `rank_count[rank]` | members per `Unit.rank` (decremented by 0x5288F0) |
| 0x37 | 1 | u8 | `formation` | 1 = grid formation active |
| 0x38 | 12 | f32[3] | `last_pos` | leader position at the last formation recompute |
| 0x44 | 4 | f32 | `last_heading` | radians |
| 0x48 | 12 | f32[3] | `prev_pos` | snapshot used by FOLLOW_FORMATION (copied from 0x38..0x40 on first use) |
| 0x54 | 4 | f32 | `prev_heading` | |

Operations: join `0x438F80(member, leaderRecno)` (checks `count < 20` and nation compatibility, copies `team_id`, `0x5287D0`), leave `0x42FCB0`, disband `0x42DD70`, remove `0x5288F0(recno)`
(memmove, `count--`, `rank_count[rank]--`, recompute formation `0x4FA0F0(1,1)`).

## Equipment (`Unit+0x1C1`, 0x1E bytes, created for every unit by `unit_init`; `memset 0`)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 2 | u16 | `active_set` | 1 = melee set, 2 = ranged set (init: 1 if PWeapon, else 2 if PRangeWeapon) |
| 0x02 | 4 | Equipped* | `weapon_primary` | PWeaponId (0x4970C0) |
| 0x06 | 4 | Equipped* | `weapon_secondary` | SWeaponId |
| 0x0A | 4 | Equipped* | `ranged_primary` | PRangeWeaponId (0x497270; only item classes 0xB/0xC) |
| 0x0E | 4 | Equipped* | `ranged_secondary` | SRangeWeaponId (0x4973A0) |
| 0x12 | 4 | dynarr* | `inv_a` | 0x3E-byte arrays created only for rank >= 3 and the hero (0x4959B0 / 0x495B40): carried/active items; searched by the auto-potion code |
| 0x16 | 4 | dynarr* | `inv_b` | |
| 0x1A | 4 | dynarr* | `inv_c` | |

`Equipped` = 0x12 bytes `{u16 item_def, u16 level(item.+0x36), u16 item_recno, i32 a(+0x42), i32 b, i32 c}`.
Stat bonus from equipment/upgrades: `0x497E10(stat_id, unit_recno)` — see functions/unit.md §7.
