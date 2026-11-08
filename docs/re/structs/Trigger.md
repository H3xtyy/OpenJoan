# TriggerEvent — the timed area-event queue ("trigger system", `trigger_system` array) — joan.exe

The loader list of `game_load_world_arrays` (0x470D40) ends with "Loading trigger_system" (`0x4EEDF0`, empty body: the queue starts empty and is filled at run time).
The queue is an `ilib::dynarr` of **pointers to 0x43-byte (67) records**; the update `0x4EF090` runs once per tick inside the world-update chain
(`game_update` 0x4E5FC0, after units/places chain, before `scenario_flow_update` 0x471620). Despite the name the records are **not** only the `.stg` `CreateTrigger` volumes: they are the engine's
generic *timed hit/effect events* — attack hit volumes created from animation-script timings (`0x501190` → `0x4EEF00`), gate triggers (`add_gate_trigger`), wall breaks (`break_wall`)
and positional sounds. Related: [AnimScript](AnimScript.md) (`StartTime`/`Range`), [docs/formats/animscr.md](../../formats/animscr.md), [functions/battle.md](../functions/battle.md) §7,
 Hit resolution (`0x75D540`, 6.6 KB) belongs to the unit family.

## 1. Record layout (`sizeof = 0x43`)

Allocated by `0x4EEF00` (`operator new(0x43)`, ctor `0x4EFF60` = `memset(this, 0, 0x43)`, filled by `0x47E200`, appended with the dynarr push `0x584620`).

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x00 | 4 | u32 | `flags` | event kind **mask** (bit meanings §3). Creation arg 2 of `0x47E200` | 0x47E200, 0x4EF090 | verified |
| 0x04 | 4 | u32 | `target_kind` | 1 = affects **units**, 2 = affects the **targeted object / gate** (or every unit when `target < 1`), 4 = **3D sound** emitter | 0x4EF090 (`== 1`, `== 2`, `== 4`) | verified |
| 0x08 | 2 | i16 | `target` | specific target recno (unit for kind 2 w/o gate, gate/item for gate events); `< 1` = "all". For kind 4: sound-definition index (`0x4D0820(target)`) | 0x4EF090, 0x4C0680 | read |
| 0x0A | 4 | i32 | `owner_kind` | 1 = owner is a **unit**, 2 = owner is an **item** (gate/wall/object) | 0x4EF090, 0x468A80 | verified |
| 0x0E | 2 | i16 | `owner` | owner recno (unit or item array, by `owner_kind`) | 0x468A80 | verified |
| 0x10 | 2 | i16 | `sub` | extra selector (creation arg 17). Used as "bypass the owner-animation guard" when non-zero | 0x47E200, 0x4EF090 | read |
| 0x12 | 1 | u8 | `b12` | creation arg 7; unused in the export | 0x47E200 | read |
| 0x13 | 4 | f32 | `x` | event centre (world units; z is height) | 0x4EF090, 0x4C0680 | verified |
| 0x17 | 4 | f32 | `y` | | | verified |
| 0x1B | 4 | f32 | `z` | | | verified |
| 0x1F | 4 | f32 | `radius` | reach (animscr `Range` + weapon reach for attacks, 100.0 / 50.0 literals for some kinds, script radius for stg triggers). `0.0` = unlimited for kind 4 | `script_res.md` §4.1, 0x4C0680 | read |
| 0x23 | 1 | u8 | `follow_owner` | if non-zero the centre is re-copied from the owner's position every tick (`0x468A80`: unit pos = unit `+0xE/+0x12/+0x16`; item pos = item `+0x56/+0x5A/+0x5E`) | 0x468A80 | verified |
| 0x24 | 2 | i16 | `value24` | read for `flags & 0x80` as `(float)(short)` and stored into the fog-distance global `DAT_00A48CDC`; **never written by the constructor** (0 unless a caller pokes it) — decompiler offset suspect, probably the `+0x3F` extra | 0x4EF090 | read |
| 0x26 | 4 | u32 | `start_ms` | game clock `DAT_00A500BA` value at which the event becomes active (`now + animscr.StartTime`) | 0x47E200, 0x4EF090 | verified |
| 0x2A | 4 | u32 | `end_ms` | expiry time (0 = none) | 0x43A530 | verified |
| 0x2E | 1 | u8 | `hold` | "not yet started" latch: while non-zero the event cannot expire; cleared the first tick `start_ms <= now` | 0x43A530, 0x4EF090 | verified |
| 0x2F..0x3E | 16 | | (unused) | zero | | |
| 0x3F | 4 | u32 | `extra` | stored by the constructor only when `flags` is **exactly** 4, 8 or 0x80 (equality, not mask): the value for the region events | 0x47E200 | read |

## 2. Per-tick update `0x4EF090` (verified structure, hidden ECX arguments marked)

```
if (trigger_count != 0) sort(triggers)                   // 0x5850B0 -> CRT qsort(data, count, stride, cmp); comparator not visible. TODO(re)
for i = trigger_count down to 1:                         // back to front, so removal is safe
    if triggers[i] == NULL: continue                     // 0x4F0290
    t = triggers[i]
    if   (t.flags & 4)  : if main_unit exists and dist(t.pos, main_unit.pos)^2 <= radius^2: 0x5F0340(...)   // region flag ON  (byte +0xDF of the target object)
    elif (t.flags & 8)  : same with the opposite argument                                                    // region flag OFF
    elif (t.flags & 0x80): if main_unit exists and within radius: DAT_00A48CDC = (float)(short)t.value24     // fog distance (set_fog_distance writes the same global)
    elif expired(t):  remove(i)                          // 0x43A530 -> 0x4EF020: free record (virtual dtor 0x4EEEB0), dynarr erase 0x584A10
    else:
        follow_owner_update(t)                           // 0x468A80
        if t.start_ms <= now:
            t.hold = 0
            switch t.target_kind: 1 / 2 / 4  (below)
```
`expired(t)` (0x43A530) = `hold == 0 && end_ms != 0 && now >= end_ms`. `now` = `DAT_00A500BA` (game ms). An event with `end_ms == 0` never expires by time; it is removed by its handler (kind 4 when the owner died) or lives forever.

### 2.1 `target_kind == 2` (object/gate or every unit)
* `target < 1`: loop over **all units** (count down); for each existing unit: if `((flags & 1) == 0 && (flags & 0x400) == 0) || owner_kind != 1` → test `0x41C5E0` (point-in-footprint of the *item* at hand: translates by the item position `+0x56/+0x5A`, rotates by `−yaw (+0x62)`, compares with half-extents `def.+0x24 / 2 + margin`, `def.+0x26 / 2 + margin`, `margin = ftol(...)`) and, on hit, calls the **virtual method at vtable +0x48** of the object (`on_trigger`). Otherwise (owner is a unit and `flags & 0x401`): the owner must exist and be in an *attack-type action state* (`0x510F80(state)` true for states `{0x08, 0x19, 0x1C..0x22, 0x46, 0x49, 0x53}`; state lives at unit `+0x215`) and not (`state == 0x49` and `now − owner.anim_obj(+0x2F5)->+0x4C <= 0x4F (79 ms)` and `sub == 0`).
* `target >= 1`: same logic for the single object `0x4853E0(target)` (array unidentified: gate/facility), vcall `+0x48`.
* Gate triggers (`add_gate_trigger`, event kind `flags = 0x20`) fall in this branch with `owner_kind` 2: the first tick after `start_ms` calls the gate object's `+0x48` method (opens/activates it).

### 2.2 `target_kind == 1` (units hit by a volume)
If `t.target` names an existing unit (`0x428B70(target) == 0`) only that unit is a candidate; otherwise every unit is (loop count down, skipping the owner itself when `owner_kind == 1`). (The index argument of `0x428B70` is not visible in the export; `t.target` is inferred from the later `0x41B3C0((short)t.target)` / `0x75D540` call.)
For each candidate unit `u` with unit-def `d = 0x42D0A0(u.def)` (stride 0x203, `def = base − 0x203 + idx*0x203`): require `(flags & d.+0x11C) != 0` (def hit-mask). Gate by the owner-state rule of §2.1 (flag bits 0x1/0x400/0x10: with `0x10` only when `owner_kind == 1` and owner state `== 0x3A`).
Volume test (first that applies):
* `flags` has none of `0x2000/0x4000/0x40` → **sphere**: `|t.pos − u.pos|² <= radius²`.
* else if `d.+0x1C3 == 2` → **oriented box** in the unit's local frame (inverse of the unit's matrix, or of a translation when the unit has no animation object `u.+0x2F5 == 0`), box = pos ± radius on all axes, tested with the AABB-overlap `0x60D6B0` against the unit's bounds.
* else **cylinder**: horizontal distance² `<= radius²` and `|dz| <= radius`.
If hit and not (`0x508FB0(owner) != 0` [owner state ∈ 0x40..0x42] and the owner-state exception): call **`0x75D540(u)`** = apply the hit to the unit; its return value `2` aborts the loop for this event.

### 2.3 `target_kind == 4` (3D sound, `0x4C0680`)
If the listener `DAT_00A57EF8` exists and `0x4D0820(target)` resolves a sound definition: unless `end_ms == 0`-style special case (`0x4D00E0` early-out when `end_ms == 0` and the position is already audible), skip when `radius != 0 && |listener − t.pos| > radius`; pick a sample (`0x45F450`, else `0x493340` positional), allocate an OpenAL source (`0x4D09C0`), set gain from `volume (*local_8 * def.+0x69 * DAT_009D5344)`, loop/pitch/min/max-distance from the def (`+0x75/+0x79/+0x7D/+0x85`), position, play. Port: map to OpenAL Soft source; `stg` `TRIGGER_PLAY_AMB_SOUND` objects are of this kind (sound `code` such as `CAMPFIRE`).
Kind-4 events with `owner_kind == 1` are removed when the owner unit is gone (`0x428B70` → `0x4EF020`), with `owner_kind == 2` when the item is gone, otherwise processed each tick (`0x4C0680`).

## 3. `flags` bits seen in the export (names inferred; semantics belong to the unit family)

| bit | evidence |
|---|---|
| 0x001 melee/weapon hit | owner-animation guard in kinds 1/2 |
| 0x002 | with 0x2000 selects the "all units" branch before `0x75D540` cases (`case 2`, `case 0x40` share code) |
| 0x004 / 0x008 | region flag on / off (`0x5F0340`) — near the main unit |
| 0x010 | owner must be in state `0x3A` (guard `LAB_004ef704`) |
| 0x020 | **gate trigger** (`add_gate_trigger`, BUILTINS 332) |
| 0x040 | **wall break** (`break_wall`, BUILTINS 39) / uses the box test |
| 0x080 | fog-distance region (`DAT_00A48CDC`) |
| 0x200 | special owner handling in `0x75D540 (case 0x200)` |
| 0x400 | like 0x001 (ranged?) in the guard |
| 0x2000, 0x4000 | select box/cylinder volume instead of the sphere (`0x4000` = item-target event handled in `0x75D540`) |

## 4. Open questions
* `TODO(re) 0x5850B0`: comparator of the per-tick sort (start time?).
* `TODO(re)`: identity of the arrays behind `0x4853E0/0x485410` (gate/facility) and the meaning of the virtual call `+0x48`.
* `TODO(re) 0x47E200`: which creation argument is which for every caller (`0x4EEF00` has 15 stack params); the export lost the ECX/stack mapping for `0x501190`'s calls.
* `TODO(re)`: `value24` vs `extra (0x3F)` for fog regions.
* `TODO(re) 0x450EA0`: `.stg` `CreateTrigger` → record mapping (`TRIGGER_PLAY_ANIMATION`, `TRIGGER_PLAY_AMB_SOUND`) — loader not in this export.
