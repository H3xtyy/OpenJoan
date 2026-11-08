# UnitAi — the native unit AI (`uscr_run_native_ai` 0x5467B0 and its 15 handlers)

Everything the shipped game uses as "unit AI" is **C++**: `unitAI.res` only contributes the 18 names and the class number (see [../../formats/unitai.md](../../formats/unitai.md), [UnitScriptRes.md](UnitScriptRes.md)); the script payloads are inert.
Cadence and entry are in [../functions/unit.md](../functions/unit.md) §12.1 (`unit_ai_think` 0x49D810: every `UnitRes.ScriptDuration` ms, skipped while the unit has queued orders, the first think at `now + rand(500)`).
Orders, tasks and fields: [Unit.md](Unit.md). Status: **V** verified (read in full), **R** read (structure), **I** inferred from sibling functions (not read line by line).

## 1. Data flow of one AI think

```
unit_ai_think(u)
  expire_task(u)                         // task.deadline < now -> task := none
  rec = uscr_record(UnitRes[u.type].ScriptFile index)    // AI class = rec.ai_class
  handled = handler of class, called with u            // always returns 1 for classes 1..15
  if !handled && u.script: u.script->run_step(u)         // inert { exit } in the shipped data
```

Every handler is a **reactive rule set that (a) reads the current task (own or the leader's), (b) searches for enemies, (c) emits orders with `give_order`** (0x661340). No handler keeps private state: all state lives in the unit
(`mode +0x167`, `target +0x2D7`, `attack_chance +0x2DF`, `task`, `orders`, `target_select +0x16B`). Orders' lifetimes (ms) are the AI's timers; the AI is re-entered each think and simply re-issues.
`give_order(unit, id, prio, team_flag, p1..p3, p4, ms)` is abbreviated `GO(id, prio, team, p1,p2,p3, ms)` below.

### Search type strings (first argument of `search_for_unit`, 0x54B480; unreadable literals identified by use)

`'T'` (0x9D626C) own current target (or, when the searcher is *another* unit — as in FOLLOW_UNIT — that unit's target), `'L'` (0x9D7DC8) the leader if within `range`, `'E'` (0x9CFC2C) hostile unit, `"EP"` (0x9D7DCC) hostile unit inside place `p`,
`"ET"`-like (0x9D7E34) = hostile *siege* target for siege crews, `'S'`/(0x9D7E30) siege/item targets for `6519D0` (`search_for_item`), `'M'` (0x9D006C) / `'F'` (0x9D0068) the siege-weapon seat usage letters used with `siege_slots_manned`.
Details of the search in functions/unit.md §11.1. Range defaults: 0 -> 1600.

### Per-class range table (`get_unit_range`, 0x65B280) — see functions/unit.md §11.2 (e.g. light infantry 800 / 1600 / 2500, archer 250 / 1200 / 1600).

### Target priority for `NEAREST_HIGH_PRIORITY` (0x527790): matrix `prio[searcher UnitClass][target UnitClass]` (higher wins; 0 = ignore for cavalry/citizen/siege targets)

| searcher \ target | 1 LIGHT | 2 HEAVY | 3 POLE | 4 MACE | 5 ARCHER | 6 CROSSBOW | 8 SIEGE |
|---|---|---|---|---|---|---|---|
| 1 LIGHT_INF | 4 | 2 | 1 | 3 | 5 | 6 | 0 |
| 2 HEAVY_INF | 6 | 3 | 4 | 5 | 2 | 1 | 0 |
| 3 POLEMAN | 5 | 4 | 3 | 6 | 2 | 1 | 0 |
| 4 MACEMAN | 4 | 1 | 2 | 3 | 6 | 5 | 0 |
| 5 ARCHER | 2 | 5 | 6 | 1 | 4 | 3 | 0 |
| 6 CROSSBOW | 2 | 5 | 6 | 1 | 4 | 3 | 0 |
| 7 CAVALRY | – (always 0) |
| 8 SIEGE | 1 | 2 | 3 | 4 | 5 | 6 | 7 |

(Searcher classes 0 CITIZEN / 9 -> 0.) Light infantry therefore hunts crossbowmen and archers first; heavy infantry hunts light infantry; archers shoot polemen first.

## 2. Classes and handlers

Name/ordinal rules and the ResLX table: unitai.md §3. `ai_class` -> handler -> `decide_attack` (0x546970):

| class | unitAI.res entries | unit.res `ScriptFile` that selects it | handler | target mode | decide_attack |
|---|---|---|---|---|---|
| 1 | HUMAN_LIGHT_INFANTRY | `human_light_infantry.scr` | 0x531F90 | NEAREST | 0x535C20 |
| 2 | HUMAN_LIGHT_GENERAL | `human_light_general.scr` | 0x5322C0 | NEAREST | 0x538340 |
| 3 | SPECIAL_GENERAL | `special_general.scr` (heroes, named captains) | 0x532520 | NEAREST | 0x53ABD0 |
| 4 | HUMAN_HEAVY_INFANTRY | `human_heavy_infantry.scr` | 0x532780 | NEAREST | 0x53CB10 |
| 5 | HUMAN_HEAVY_GENERAL | `human_heavy_general.scr` | 0x532A00 | NEAREST | 0x53F670 |
| 6 | HUMAN_ARCHER, HUMAN_ARCHER_EX | `human_archer.scr`, `human_archer_ex.scr` | 0x532C60 | WEAKEST | 0x541270 (shoot) |
| 7 | ARCHER_GEN, ARCHER_EX_GEN | `archer_gen.scr`, `archer_ex_gen.scr` | 0x532EF0 | WEAKEST | 0x5432A0 (shoot) |
| 8 | HUMAN_CANNON | `human_cannon.scr` | 0x533170 | WEAKEST | 0x544C40 (siege shoot) |
| 9 | AUTOBOW | `autobow.scr` | 0x533370 | WEAKEST | 0x544E00 |
| 10 | BATTERRAM | `batterram.scr` | 0x533440 | WEAKEST | 0x545380 |
| 11 | CAMP_HORSE | `camp_horse.scr` | 0x5335E0 | NEAREST | – |
| 12 | HUMAN_HORSE | `human_horse.scr` | 0x5336E0 | NEAREST | – |
| 13 | CITIZEN | `citizen.scr` | 0x533810 | NEAREST | – |
| 14 | HUMAN_TREBUCHET | `human_trebuchet.scr` | 0x533920 | WEAKEST | 0x545D00 (siege shoot) |
| 15 | CART | `cart.scr` | 0x533B10 | NEAREST | – |
| 0 | PIG | `pig.scr` | – (no native handler) | | |

(`ScriptFile NONE` -> AI recno 0 -> the unit has no AI at all.) The "decide_attack" function is both used inside the combat tick below and re-run by the BLOCK order when the block ends (0x4BF240 via 0x546970).

### 2.1 Common dispatcher skeleton for classes 1–7 (V: 0x531F90, 0x5322C0, 0x532520, 0x532780, 0x532A00, 0x532C60, 0x532EF0)

```
h(u):
  if is_dead(u) return 1
  set_attack_behaviour(u, NEAREST | WEAKEST)
  task = get_curr_task(u)                     // own task or the leader's
  if !is_attacked(u):                         // not in a hit-reaction / knock-down action
      switch task:                            // see tables
  else:                                       // in a reaction
      if task == IDLE(7) && get_curr_task_para(u,1) == 0:
           stop_curr_task(u); L = get_leader_recno(u); if L && get_curr_task(L) == IDLE: stop_curr_task(L)
  return 1
```
Task -> function, transcribed from the dispatchers (`common` = RETREAT 11 -> 0x545D50, RTS_MOVE_TO 12 -> 0x545EA0, APPROACH_PLACE 14 -> 0x546050; "-" = returns 1 without doing anything; "leaderless" = `0x4216D0() == 0`, i.e. neither a leader nor a follower):

| class | handler | tasks -> function |
|---|---|---|
| 1 light inf | 0x531F90 | 0, 1, 9, 0x12, default -> **0x534090**; 3, 4, 5, 7 -> -; 6 -> 0x534880; 10 -> (leaderless ? 0x5377B0 : 0x534090); 13 -> 0x535870; 15 -> 0x535020; 17 -> 0x535A20; 19 -> 0x535E30; 101..105 -> return 0 (script fallback); common |
| 2 light general | 0x5322C0 | 0, 3, 4 -> **0x536390**; 1 -> 0x536A40; 5, 7, default -> -; 6 -> 0x5370D0; 10, 0x12 -> 0x5377B0; 13 -> 0x537F90; 17 -> 0x538140; 19 -> 0x538730; common |
| 3 special general | 0x532520 | 0, 3, 4 -> **0x538C90**; 1 -> 0x539A40; 5, 7, default -> -; 6 -> 0x53A060; 10, 0x12 -> 0x5392D0; 13 -> 0x53A6D0; 17 -> 0x53A9D0; 19 -> 0x53B1B0; common |
| 4 heavy inf | 0x532780 | 0, 1, 9, 0x12, default -> **0x53B610**; 3, 4, 5, 7 -> -; 6 -> 0x53BEB0; 10 -> (leaderless ? 0x53E9D0 : 0x53B610); 13 -> 0x53C700; 17 -> 0x53C960; 19 -> 0x53CDA0; common |
| 5 heavy general | 0x532A00 | 0, 3, 4 -> **0x53D3B0**; 1 -> 0x53DB10; 5, 7, default -> -; 6 -> 0x53E240; 10, 0x12 -> 0x53E9D0; 13 -> 0x53F260; 17 -> 0x53F4C0; 19 -> 0x53FA60; common |
| 6 archer | 0x532C60 | 0, 1, 9, 0x12, default -> **0x540070**; 3, 4, 5, 7 -> -; 6 -> 0x540640; 8 -> 0x540C10; 10 -> (leaderless ? 0x542820 : 0x540070); 13 -> 0x540E80; 17 -> 0x5410B0; 19 -> 0x541300; common |
| 7 archer general | 0x532EF0 | 0, 9, 0x12, default -> **0x541600**; 1 -> 0x5420A0; 3, 4, 5, 7 -> -; 6 -> 0x541B10; 8 -> 0x5425D0; 10 -> 0x542820; 13 -> 0x542E60; 17 -> 0x543090; 19 -> 0x543330; common |

Task 15 (LARGE_WEAPON_PUSH) is handled only by class 1 (0x535020); task 19 (0x13) is dispatched by every combat class but no exported code creates it.
Cell values follow the dispatchers 0x5322C0.. exactly; the sub-functions themselves were read for classes 1 (all), 2 (0x536390, 0x538340), 3 (0x53ABD0), 4 (0x53CB10), 5 (0x53F670), 6 (0x540070, 0x540C10, 0x540E80, 0x5410B0, 0x541300) — the remaining ones (R/I) are structurally identical clones with different search ranges, combo tables and the class's `decide_attack`.

## 3. Shared task functions (V)

* **RETREAT (task 11) `0x545D50`**: `GO(APPROACH_PLACE 14, prio 99, team 1, p1 = task.p1 (place), 0,0,0, 30000)`. Arrival test `0x653F30(place, u)` (unit inside the place region). If arrived **or** `task.p3 < 1`: `0x656410(u)` (**vanish**: collision type 0, actor fade-out `rate -0.1`, hand items faded, HP := 0 via the silent setter -> the unit is deleted by the unit pass after the fade, no DIE action and no death bookkeeping), `unit_stop`, `stop_curr_task`, then `give_task(u, RETREAT, prio 99, 0,0,0, 0)` (re-armed with zero parameters).
* **RTS_MOVE_TO (12) `0x545EA0`**: if in a reaction action: `stop_curr_task`. Else `GO(MOVE_TO_TARGET 7, prio 99, team 0, task.p1, task.p2, task.p3, 0, 20000)` every think; `r = REACH_DT(15)`, `x5` (75) when the unit is mounted, class 8 (siege) or `lod > 1`; **completion only when `|r| < 31`** (i.e. only for r = 15; the x5 case never completes — a quirk of the original), and `dist²(xy to p1,p2) <= r²` -> `stop_curr_task`.
* **APPROACH_PLACE (14) `0x546050`**: `GO(APPROACH_PLACE 14, prio 1, team 0, p1 = place, p2 = task.p2 (area), ...)` (the decompiler lost the exact arguments; completion is by the order, then the task is cleared by the script `stop_curr_task`/deadline).
* **Combat tick (default / ATTACK_PLACE / SCOUT / DEFEND_UNIT) — class 1 `0x534090` (V)**, the template of classes 1..7:
  1. `T = search('T')`; `Lr = get_leader_recno`.
  2. If the unit has a leader: `l = search('L', 1000)`; if `l == 0`: `GO(FOLLOW_FORMATION 15, prio 2, team 1, ..., 10000)` + `set_target(0)` and **return**; else if `T == 0` (no own target) and `l`: `GO(FOLLOW_FORMATION, prio -2, team 1, ..., 10000)`.
  3. `e = search('E', 150)`; if found -> `mode := ATTACK`; else `mode := ACTIVE`, `e = search('E', 200)` and with `rand(50) == 0` -> `mode := ATTACK`.
  4. **mode ATTACK** (`is_attack_mode`): if `e == 0` return; else `GO(TURN 10, prio 3, team 0, p1 = get_rotation(u, e))` then `decide_attack(u)` (§4).
  5. **mode ACTIVE/IDLE** (`is_idle_mode`, mode > 1): `target_select := NEAREST_HIGH_PRIORITY`; `e1 = search('E', range1(u, use_leader=1))` (half range `range1/2` when the task has a place parameter); `near = search('E', range1/2)`.
     If `near` exists and is not an archer/crossbow (UnitClass 5/6), pick a defensive manoeuvre with `r = rand(10)`: `r < 1` -> `GO(APPROACH_ENEMY 12, prio 1, team 0, e1, 60, 60, 30000)`; `r < 5` -> `GO(TURN 10, prio 1, ...)`+`GO(SHIFT 6, prio 1, p1 = 1 (left), 1500)`; `r < 9` -> TURN + SHIFT p1 = 2 (right); `r == 9` -> TURN + SHIFT p1 = 3 (back). Return.
     Else if `e1 > 0`: `GO(APPROACH_ENEMY, prio 1, p1 = e1, 60, 60, 30000)`.
     Else if the leader has no target (or the unit has no leader) and `T` is nonzero... a second search `search('E', range2(u))` -> APPROACH_ENEMY; else if a target `T` exists and `dist(u,T) <= range3(u)`: `GO(APPROACH_ENEMY, prio 1, team 1, p1 = T, 60, 60, 30000)`; else `mode := IDLE`.
  Classes 2/3/5 (generals) use `p2 = p3 = 70` (0x46) for the team-flagged approach and a search window `0x87` for `use_leader`; heavy infantry/maceman variants add the same manoeuvre set. (R/I)
* **DEFEND_PLACE (6) class 1 `0x534880` (V)**: same as the combat tick but all enemy searches are `"EP"` bound to the task's place `p1` (`search("EP", range, place=p1)`), and in attack mode it writes `place+0x1D2 := 0` through `0x6547E0(place, degrees=0)` (a per-place float, probably an alert/guard angle; semantics `TODO(re)`). With no enemy in the place it re-issues `GO(APPROACH_PLACE 14, prio -3, team 0, place, 0,0,0, 30000)` and `mode := IDLE`. A leader that has lost its leader link (`T == 0` while having a follower) behaves like step 2.
* **RTS_ATTACK_UNIT (13) `0x535870` (V)**: `u1 = task.p1`; stop the task if in a reaction, the target is invalid/dead. If `get_unit_dist(u, u1) >= 200`: `GO(APPROACH_ENEMY 12, prio 1, team 0, u1, 100, 100, 30000)`; else `mode := ATTACK`, if attack mode: `GO(TURN, prio 4, angle to target)` + `decide_attack`; if only active mode: approach again.
* **RTS_ATTACK_ITEM (17) `0x535A20` (V)**: the same for an item target `p1` (only item sub-type 10 = box accepted via 0x65B6A0; distance via `get_item_dist`, threshold 0x95 = 149, `APPROACH_ITEM 13` with 100/50).
* **LARGE_WEAPON_PUSH (15) `0x535020` (V, long)**: crew logic for pushing/loading a siege weapon `p1`: if the unit is not yet attached: choose a free seat with `656DC0(unit, weapon, 'M'|'F')` (M seats first when `p2 == 0`, F seats first otherwise), reserve it (`+0x380`, weapon arrays), `GO(MOVE_TO_TARGET 7, prio 10, ..., seat x,y,z, 10000)` and register the attach request `0x657670`; once attached (`0x656AF0 > 0`) it waits at the seat; if the weapon is destroyed or the distance to the seat is `<= REACH_DT` it releases (`+0x384.. = 0`, `stop_curr_task`, `0x6618F0`).
* **Task 0x13 (19)**: dispatched to `0x535E30` (class 1) and clones — the combat tick with a MOVE_TO_TARGET fallback toward `task.p1,p2,p3` (`GO(MOVE_TO_TARGET 7, prio 0, ..., 30000)`; `stop_curr_task` when `dist < 201`; mode := IDLE): "attack-move".
* **FOLLOW_UNIT (10) `0x5377B0` (V)**: `f = task.p1` (followed unit), `bearing = task.p2` (default 180), `d = dist(u, f)`; if `d > 0x6592C0() + 200` (the formation distance of `f`): `GO(FOLLOW_UNIT 16, prio 0, team 1, f, bearing, dist, 0, 5000)` and clear the target; else behaves like the combat tick **with `T = target of f`** (defend the followed unit): the ATTACK/ACTIVE decision is the same; `APPROACH_ENEMY` uses `p2 = p3 = 70`.

## 4. `decide_attack` (the "attack decision") per class

All draw one `r = rand(100)` and use `attack_chance` (`Unit+0x2DF`, scaled by difficulty `GAME/*_A`, 0x511F80) as the threshold; after a decision the meter is **reset to the rank base** `0x511E70()`:
`base = 90 (0x5A)` for `Ranking` 4 and 5 (generals); `Ranking 3` (captain) and others: `30 (0x1E)` if UnitClass is ARCHER/CROSSBOW (5/6) else `50 (0x32)`. The meter then rises +10 per melee hit taken (+5 arrow).

**Class 1 `0x535C20` (V)** (light infantry): skip if the unit is mid-attack (`0x510F80`). `r = rand(100)`; if `r < chance`: reset meter; `k = rand(3)`: `k == 0` -> `GO(COMBO_ATTACK 1, prio 2, p1 = combo_code(<0x9D7DD4>, 1), 1500)` then with 90 % (`rand(100) < 90`) `GO(SHIFT 6, prio 1, p1 = 3 (back), 1500)`; `k == 1,2` -> combo `<0x9D7DD0>` + the same shift; (k's last branch `else` returns). Else (`r >= chance`): if `r < 90` nothing; if `r >= 90` -> `GO(SHIFT 6, prio 2, p1 = rand(3)+1, 1500)`.
**Class 2/5 `0x538340` / `0x53F670` (V)** (generals; 5 uses 1000 ms durations): `r = rand(100)`, `canDown = can_attack_down(u)` and `kd = is_facing_knock_down_enemy(u)` (only evaluated when `canDown`): if `r < 60 && canDown && kd` -> `GO(DOWN_ATTACK 19, prio 2, 1500)` (return). Else if `r < chance`: reset meter; `k = rand(14)`: `<3` combo A (`<0x9D7DD4>`), `<5` combo B (`<0x9D7DD0>`), `<6` combo C (`<0x9CFC70>`), `<13` combo D (`<0x9D7DD8>` for class 2; `<0x9CFC30>` for class 5), `==13` and `can_energy_attack` -> `GO(ENERGY_ATTACK 20, prio 2, 1500)`; each combo: `GO(COMBO_ATTACK 1, prio 2, p1 = combo_code(name, 1), 1500)` followed by a 90 % `GO(SHIFT 6, prio 1, p1 = 3, 1500)` (in all combo calls the word index is 1: the varying trailing numbers in the decompiler output are stale stack slots). Else if `r >= 90`: `GO(SHIFT 6, prio 2, p1 = rand(3)+1, 1500)`.
**Class 3 `0x53ABD0` (V)** (heroes): `r = rand(100)`; `r < 60` with a downable knocked-down enemy -> DOWN_ATTACK (1500). Else if `r < 60`, scenario id `DAT_00A58FA7 == 7` and the unit type is `MARTINGOUGH`: refill EP to max and, if `can_rush_attack` (0x652B00: needs RUSH animation + EP), `GO(RUSH_ATTACK 21, prio 2, 5000)` (the S7 boss). Else if `r < chance`: `hostile = main character exists, u != main, enemy(main, u), death_state in {0,-2}`; if `attack_chance < (hostile ? 100 : 120)` then `k = rand(24)`; otherwise a second roll `rand(200) <= chance_scaled * (hostile ? 1.3 : 1.0)` gives `k = 0x15` and priority 3 (else `k` stays the first roll `r`). Meter reset. Combos by `k`: `<5` `<0x9D7DD4>`, `<9` `<0x9D7DD0>`, `<13` `<0x9CFC70>`, `<17` `<0x9CFC30>`, `<19` `<0x9D7DE4>`, `<21` `<0x9D7DDC>`, `==21` ENERGY_ATTACK (5000 ms) if `can_energy_attack` else `<0x9CFC30>`, `<24` RUSH_ATTACK (5000 ms) if `can_rush_attack` else `<0x9D7DD4>`. All combos: `GO(COMBO_ATTACK, prio, p1 = combo_code(name,1), 1500)`.
**Class 4 `0x53CB10` (V)** (heavy infantry): skip if mid-attack. `r = rand(100) < chance`: reset; `k = rand(5)`: `0` -> combo `<0x9D7DD4>` + 90 % SHIFT(3); `1,2` -> `<0x9D7DD0>` + shift; else `<0x9CFC30>` + shift. `r >= 90` -> `GO(SHIFT 6, prio 2, rand(3)+1, 1500)`.
**Class 6 / 7 `0x541270` / `0x5432A0` (V)** (archers): `shoot(u, kind, target)` = `if rand(10) < chance_scaled(0x511F80)` { reset meter; `GO(SHOOT 2, prio 2, team 0, kind (1 unit / 2 item), target, 0, 0, 1500)` }. Because `chance_scaled` is a percentage (>= 15 for any rank at any difficulty: base 30 x 0.5) and `rand(10) <= 9`, **the test is always true in practice**: archers shoot on every think in which they have a clear target. Callers (0x540070 etc.) first `GO(TURN 10, prio 3, angle)` toward the target when the line of fire is clear (`0x5130A0(target) == 0`), else mode := ACTIVE.
**Class 8/9/14 `0x544C40` / `0x544E00` / `0x545D00`**: siege shoot: class 8: `if rand(10) < 4`: `GO(SHOOT 2, prio 2, kind, target, 1500)`; class 9 (autobow): always; class 14 (trebuchet): always with 5000 ms.
**Class 10 `0x545380`**: `if crew-manned(F) && rand(10) < 5`: `GO(COMBO_ATTACK 1, prio 2, combo_code(<0x9D7DD4>,1), 1500)` (ram swing).

Combo name literals (unreadable) are three-word strings of `A/B/C…` hits; `combo_code(name, idx)` returns word `idx` (1..3) of the packed name.

## 5. Other classes

* **Class 8 cannon `0x533170` / class 14 trebuchet `0x533920` (R)**: `A = siege_slots_manned('M')`, `B = siege_slots_manned('F')`. If neither: `stop_all_order`. Task 7 (IDLE): nothing. If `A`: task RTS_MOVE_TO (12) -> 0x545EA0, APPROACH_PLACE (14) -> 0x546050, 0x13 -> `0x543C10` (fallback: `GO(MOVE_TO_TARGET, prio 0, task xyz, 30000)`; stop when `dist < 201`; mode IDLE; plus the fire logic below when `B`). If `B` (shooters manned): task 0/0xB/1 -> `0x543800` (cannon) / `0x543FD0` (trebuchet) (fire at `search("ET")` with a `6519D0` item search up to 3000/4000; maximum search range 0x640, or 0x9C4 if the weapon's attach actor is in state 0xB; if target found: `mode := ATTACK`, `GO(TURN 10, prio 3, ...)` and `decide_attack(u, kind, target)`; otherwise if only active `GO(APPROACH_ITEM 13, prio 1, item, 800, 400, 30000)` or `mode := IDLE`); task 13 -> `0x5442D0`/`0x544500`; task 17 -> `0x544690`/`0x544AA0` (RTS attack on unit/item). Trebuchet task 1 -> `0x545B80`.
* **Class 9 autobow `0x533370` (V)**: only task 0: `0x544CB0`: `target_select := NEAREST_ARCHER`; `e = search('E', 3000, 0,0, max_xy=10 (angle window?), max_z=0x41)`; mode ATTACK if found else ACTIVE; if attack mode and the aiming error `get_angle_diff(u,e)` is within +-10 deg -> shoot; else clear the target.
* **Class 10 battering ram `0x533440` (V)**: ignores everything while `is_died`; same M/F logic; task 12/0x13 with M manned -> RTS_MOVE_TO; task 17 with F manned -> `0x5450B0`; task 0x65 (SPECIAL1) -> `0x544E50` ("attack gate"): approach the target item (`p3`) via `MOVE_ON_TRACK 9` (track from `p1/p2`), when within 300 and the F seats are manned face it and swing.
* **Class 11 camp horse `0x5335E0` (V)**: `target_select := NEAREST`; if not dying/attacked and a hostile is within 100 and `rand(100) >= 70`: `GO(SPECIAL_MOVEMENT 22, prio 7, team 0, p1 = 2, p2 = 0, p3 = 1, 10000)` (rear up / bolt).
* **Class 12 horse `0x5336E0` (V)**: sets `change_animation_special("DEFAULT")`; riderless horses do nothing unless `task = FOLLOW_UNIT` -> `0x545430` (`GO(FOLLOW_UNIT 16, prio -1 or 5, p1 = followed, p2 = bearing (default 180), p3 = 200, 5000)`; if `dist > 400` it also clears the target and uses priority 5), `APPROACH_PLACE`, `RTS_MOVE_TO`.
* **Class 13 citizen `0x533810` (V)**: `change_animation_special(DEFAULT)`; task 0 (none) -> `0x545590` (wander, below); IDLE -> nothing; FOLLOW_UNIT -> `0x545900` (as horse, 20000 ms); APPROACH_PLACE; task 0x65 (SPECIAL1) -> `0x545A60`: `rand(100) > 89` -> `GO(MOVE_TO_TARGET 7, prio -1, p1 + rand(2*p3) - p3, p2 + rand(2*p3) - p3, 0, 30000)` (loiter around a point).
* **Wander `0x545590` (V)**: if no hostile within 600: `if rand(100) > 89` pick a random offset `d = rand(n) - 100` per axis (negative offsets pushed by `-80`, positive by `-20`: the result is biased away from 0) and `GO(MOVE_TO_RELATIVE_TARGET 8, prio -1, dx, dy, dz, 0, 5000)`. With a hostile within 600 (flee): `ang = atan2(...)`; `GO` flee `MOVE_TO_RELATIVE_TARGET 8` away from it with `rand(400) - 400`, 2000 ms; with `rand(...) < 4` it first `stop_all_order` and then `GO(SPECIAL_MOVEMENT 22, prio 20, p1 = 1, p3 = 1, 2000)` (cry / panic).
* **Class 15 cart `0x533B10` (V)**: `change_animation_special("DEFAULT")`; if not attacked: if a hostile is within 100 and `rand(100) < 81` (`< 0x51`): scatter; elif `search(0x9D0068 'F' ...)` friendly within 100 and `rand(100) < 96`: scatter; else `rand(100) > 79` (20 %): random wander. "Scatter" = `GO(MOVE_TO_RELATIVE_TARGET 8, prio -1, dx, dy, dz, 0, 5000)` with `d = rand(200) - 100` per axis then `+-50` push away from 0, also `change_animation_special(0x9CF538)` for the breaking-wheel variant.
* **Class 0 PIG**: no handler -> nothing (the inert script exits).

## 6. Constants and priorities seen in AI orders

Durations (ms): combos/shifts 1500 (0x5DC), generals' class 5 1000, approach 30000, follow formation 10000, follow unit 5000/20000, wander 5000, scatter 5000, shoot 1500, trebuchet 5000.
Priorities: `-3` background APPROACH_PLACE, `-2` background FOLLOW_FORMATION, `-1` wander, `0` follow-unit / routine move, `1` approach & shift, `2` combos / attack decisions / formation follow when the leader is lost, `3` TURN before attack, `4` TURN for RTS commands, `5` follower catch-up, `7` camp-horse bolt, `10` crew seating, `20` panic, `99` RTS_MOVE_TO / RETREAT.
RNG sites: see functions/unit.md §14.

## 7. Port notes

* Reimplement the handlers as data-driven rule sets over the same unit fields; keep the *order/task* interface — `give_order`/`give_task` are what scripts call and the scenarios assume the AI's use of priorities (a script `TURN` with priority 1 must not be pre-empted by a priority-1 AI shift but is by priority 2 combos).
* `rand(...)` consumption per think matters for replays; the original draws in the exact order listed above.
* The AI is gated by `ScriptDuration` and by "no queued orders"; do not tick it every frame.
