# Physis (physis_sys), Body, Volume and the movement-related Unit fields

Reference build `joan.exe`. Behaviour: [functions/flocksys.md](../functions/flocksys.md) sections 7-12.
Status: V = read in >= 2 functions, R = one function, I = inferred. All structs are byte-packed (odd offsets are normal).

## Physis (singleton, created by 0x6AD020; sizeof >= 0xB20)

| Offset | Size | Type | Name | Meaning | Evidence | St |
|---|---|---|---|---|---|---|
| 0x000 | 4 | ptr | table1_owner | set by 0x68DB80 (`this[0] = arg`) | 0x68DB80 | R |
| 0x004 | 27 x 0x34 | PlaneEntry[] | table1 | first plane-pair table (entries `{A at +4, B at +8}` relative to the object) | 0x68DD80 | R |
| 0x580 | 4 | i32 | table1_count | 0 at reset (0x68DBC0) | 0x68DBC0, 0x68DD80 | R |
| 0x586 | 4 | u32 | last_step_ms | `DAT_00A500BA` at the previous step; init = now | 0x6AD660, 0x6AD020 | V |
| 0x58A | 4 | i32 | step_flag | cleared every step | 0x6AD660 | R |
| 0x58E | n x 0x34 | PlaneEntry[] | planes | registered terrain plane pairs | 0x6AD450, 0x6AAE70 | V |
| 0xB0A | 4 | i32 | plane_count | | 0x6AD5D0 | V |
| 0xB0E | 4 | dynarrb* | active_bodies | list of `Body*` currently simulated (`dynarrb(elem 4, grow 5)`) | 0x6AD020, 0x6AD980 | V |
| 0xB12 | 2 | i16 | enabled | 0 at init; the step returns early while 0. Writer: `0x6AD900(on)`, called with 1 at every world/scenario start | 0x6AD660, 0x6AD020, 0x6AD900 | V |
| 0xB14 | 2 | i16 | field_B14 | init 1 | 0x6AD020 | R |
| 0xB16 | 4 | f32 | gravity | init 1.0, `0x472EB0(3.0)` at game start. Multiplies the vertical acceleration (`_DAT_009DD75C * gravity`) of every body except riders (which use the unit's `+0x243`) | 0x6AD020, 0x472EB0, 0x6A45A0, 0x6AABB0 | V |
| 0xB1A | 2 | i16 | substep_mul | init 1; factor in the friction term of the step | 0x6AABE0, 0x6A45A0 | R |
| 0xB1C | 2 | i16 | void_mode | 0: no ground -> height 0; != 0: no ground -> -5000.0 | 0x6ADFD0, 0x6AE2A0 | V |
| 0xB1E | 2 | i16 | field_B1E | init 1 | 0x6AD020 | R |
| 0xB20 | ? | ? | sub_object | initialised by `0x68DB80` (`this[0]=owner, this[0x160]=0`) | 0x6AD020 | R |

## PlaneEntry (0x34 = 52 bytes)

| Offset | Size | Type | Name | Meaning | Evidence | St |
|---|---|---|---|---|---|---|
| 0x00 | 4 | handle | plane_a | first surface layer (terrain plane id for `0x637DA0/0x66B9F0`) | 0x6AD450, 0x6AAC10 | R |
| 0x04 | 4 | handle | plane_b | second layer | same | R |
| 0x08 | 4 | i32 | origin_x | integer origin subtracted from world X in height/edge queries | 0x6ADFD0, 0x6AE8E0, 0x6AB4E0 | V |
| 0x0C | 4 | i32 | origin_y | same for Y | same | V |
| 0x10 | 4 | f32 | local_x | origin used by the ground search | 0x6AAE70 | R |
| 0x14 | 4 | f32 | local_y | | 0x6AAE70 | R |
| 0x1C | 4 | f32 | min_x | AABB | 0x6AAE70 | R |
| 0x20 | 4 | f32 | min_y | | | R |
| 0x24 | 4 | f32 | min_z | | | R |
| 0x28 | 4 | f32 | max_x | | | R |
| 0x2C | 4 | f32 | max_y | | | R |
| 0x30 | 4 | f32 | max_z | | | R |

(`0x18` and `0x34`-boundary padding unused by the readers.)

## Body (0x2B2 = 690 bytes, vtable-less; ctor 0x6B0830, init 0x6B08E0, global `dynarrb` of `Body*`, 1-based; id stored in `actor+0x5C`)

| Offset | Size | Type | Name | Meaning | Evidence | St |
|---|---|---|---|---|---|---|
| 0x000 | 4 | i32 | id | its registry index | 0x6B2040 | V |
| 0x004 | 4 | i32 | kind | body kind. **0** inert/static; **1** character; **2,3,4,5,0x12** projectile/thrown (skipped as obstacles; step 0x6A8060); **6,7** big thrown (extent/2.0); **9,10,0xB,0xE,0xF,0x11,0x13** unit-like (extent/1.0); **0xB** hero/mount with riders; **0x14** special. Names inferred from the filters, not from data | 0x6AD660, 0x68DE40, 0x6AEE40 | R |
| 0x008 | 4 | Actor* | actor | owner; `actor+8` = category (0xB = unit), `actor+0xC` = Unit* | 0x68DE40 | V |
| 0x00C | 0x136 | State | current | state block A (0x4D dwords + one i16) | 0x6B08E0, 0x6A45A0 | V |
| 0x142 | 0x136 | State | next | state block B (the integrator works on this one, then copies it over `current`) | same | V |
| 0x1DA | 12 | vec3 | prev_pos | position before the step (physics units) | 0x6B18A0, 0x6AF190 | R |
| 0x1F6 | 12 | vec3 | facing | forward unit vector * 0.02169 (const 0x3CB1B35E) | 0x6B18A0, 0x6B1030 | R |
| 0x1FE | 4 | f32 | z | height in physics units (world z = z * K) | 0x6AD980, 0x6B1030 | R |
| 0x202/0x206/0x20A | 12 | i32[3] | pos_i | integer world position copy (`ftol`) | 0x6ADBF0, 0x6AEE40 | V |
| 0x20E | 4 | handle | ground_layer | current plane layer ref | 0x6A45A0, 0x6AD450 | V |
| 0x212 | 4 | handle | ground_plane | current plane ref | same | V |
| 0x216 | 2 | i16 | air_frames | steps since last ground contact; set to 1 on ground | 0x6AD980, 0x6A45A0 | R |
| 0x218 | 4 | f32 | ground_z | last ground height (physics units) | 0x6AD980, 0x6A45A0 | R |
| 0x21C | 12 | vec3 | knock_vec | knock-back **force** (= `next+0xDA` of the integrator, `local_c = body+0x142`): `a = vec / mass` per sub-step, all axes | 0x6B11A0, 0x6A45A0 | V |
| 0x228 | 4 | u32 | impulse_c_start | start ms | 0x6B11A0 | V |
| 0x22C | 4 | f32 | impulse_c_dur | duration ms | 0x6B11A0 | V |
| 0x230 | 4 | u32 | impulse_c_end | start+dur | 0x6B11A0 | V |
| 0x234 | 2 | i16 | impulse_c_done | | 0x6B11A0 | V |
| 0x236 | 12 | vec3 | impulse_d_vec | push-sideways velocity target (`next+0xF4`; `a = (vec - v)/dt`, z = 0; set by 0x6B12A0 on a collided victim) | 0x6B12A0, 0x6A45A0 | R |
| 0x242 | 4 | u32 | impulse_d_start | | 0x6B12A0 | R |
| 0x246 | 4 | f32 | impulse_d_dur | | 0x6B12A0 | R |
| 0x24A | 4 | u32 | impulse_d_end | | 0x6B12A0 | R |
| 0x24E | 2 | i16 | impulse_d_done | | 0x6B12A0 | R |
| 0x250 | 12 | vec3 | walk_vec | root-motion velocity target (`next+0x10E`; `a = (vec - v)/dt`, z = 0; applied only when the push did not run this sub-step) | 0x6B10A0, 0x6A45A0 | V |
| 0x25C | 4 | u32 | walk_start | | 0x6B10A0 | V |
| 0x260 | 4 | f32 | walk_dur | | 0x6B10A0 | V |
| 0x264 | 4 | u32 | walk_end | | 0x6B10A0 | V |
| 0x268 | 2 | i16 | walk_done | | 0x6B10A0 | V |
| 0x27A | 4 | f32 | mass | ctor arg (50.0 for units) | 0x5102A0, 0x6B08E0 | V |
| 0x27E/0x282/0x286 | 12 | f32[3] | extents | from the volume (sphere radius x3, box per-axis max abs, capsule (r, r, h/2)) | 0x6ADBF0, 0x6B08E0 | V |
| 0x28A | 2 | i16 | layer | 1 or 2 selects plane layer preference (`0x6AAC10`, `0x6AA9E0`) | 0x6ADBF0 | R |
| 0x278 | 2 | i16 | sleep_ticks | steps with nothing to do; > 999 -> deactivate | 0x6A45A0 | V |
| 0x298 | 2 | i16 | blocked | set on collision, cleared by walk-vector application | 0x68DE40, 0x6A45A0 | V |
| 0x29A | 4 | i32 | blocker_id | body id of the blocker (kind-0xB blockers only) | 0x68DE40 | V |
| 0x29E | 4 | i32 | moving_flag | cleared at the start of each step | 0x6A45A0 | R |
| 0x2A2 | 2 | i16 | landing_mode | | 0x6A45A0 | I |
| 0x2AC | 2 | i16 | need_ground | | 0x6A45A0 | I |
| 0x2AE | 4 | i32 | active_index | index in `Physis.active_bodies` (0 = inactive) | 0x6AD980, 0x6AD660 | V |

State block fields used inside "next" (offsets relative to the block start `+0x142` in `0x6A45A0`'s `local_c`): `+0x00` velocity xyz, `+0x0C` angular/vel2, `+0xA4..0xB0` quaternion-ish,
`+0xB4` position xyz, `+0xC0/C4/C8` last integer pos, `+0xCC/D0` ground normal flags, `+0xD4` on_ground, `+0xD6` ground z,
`+0xDA` impulse A vec, `+0xE6` start, `+0xEA` duration, `+0xF2` active, `+0xF4` impulse B vec, `+0x104` duration, `+0x10C` active,
`+0x10E` impulse C vec, `+0x11E` duration, `+0x126` active. (R; layouts of A/B/C identical to the standalone `0x21C/0x250` blocks.)

## Volume (fields on the Actor wrapper `unit+0x2F5`)

| Offset | Type | Meaning | Evidence |
|---|---|---|---|
| 0x18 | ptr | model instance (null = nothing to do) | 0x68DE40, 0x4FED00 |
| 0x28 | f32 | uniform scale | 0x6A31D0 |
| 0x5C | i32 | physis body id | everywhere |
| 0x8C | D3DMATRIX* | world matrix; translation at +0x30/+0x34/+0x38 | 0x68EE50, 0x6B1350 |
| 0xB8 | i32 | volume type: 0 none, 1 sphere, 2 box, 3 capsule | V |
| 0xBC/0xC0/0xC4 | f32[3] | sphere centre | 0x65D0D0 |
| 0xC8 | f32 | sphere radius | 0x65D0D0 |
| 0xCC..0xE0 | f32[6] | box `min.xyz, max.xyz` | 0x65D0D0, 0x6A31D0 |
| 0xE4/0xE8/0xEC | f32[3] | capsule base | 0x6A31D0 |
| 0xF0 | f32 | capsule radius | |
| 0xF4 | f32 | capsule height | |

Source of the volume for units: unit-res record fields `+0x1C3` (type), `+0x1C7..0x1D3`, `+0x1D7..0x1EE`, `+0x1EF..0x202` (see unit.res docs); `enable_unit_bounding_volume` copies them.

## Constants (read from the executable)

`K = 46.1` world units per physics unit (0x9CF780); `1/K = 0.02169197` (0x9DD768, also literal 0x3CB1B35E); `1000/K = 21.69197` (0x9D7490, float); gravity constant -9.81 (0x9DD75C) times
`Physis.gravity` 3.0; epsilons 0.00015 (0x9D748C), 0.0015 (0x9DD760); search margins 250.0 (0x9DD6D0), 20.0 (0x9D5334); stuck-hero volume shrink 0.005 (0x9CFC78);
turn-rate divisor 33.0 (0x9D74D0); in-place-turn threshold 3.0 deg (double, 0x9D0758); min tested move 0.01 (double, 0x9D8C38); `-1.0` at 0x9CF834 (flock drift sign flip).

## GroupRecord (formation group, `unit+0x33`; used by 0x4FA0F0, 0x658810, 0x65D350)

| Offset | Type | Meaning |
|---|---|---|
| 0x00 | i16 | leader unit id |
| 0x02 | u8 | member count |
| 0x03 | i16[count] | member unit ids |
| 0x37 | u8 | formation type (1 = block grid) |
| 0x38/0x3C/0x40 | f32[3] | anchor (leader position at last recompute; set by `set_formation_pos`) |
| 0x44 | f32 | anchor heading (radians) |
| 0x48/0x4C/0x50 | f32[3] | copy of the anchor (set by `set_formation_pos`) |
| 0x54 | f32 | copy of heading |

Per-unit formation slot fields: `unit+0x412/0x416` rotated slot offset, `+0x41E` depth, `+0x422/0x426` unrotated slot.

## Unit movement fields (byte offsets on the Unit record; this export only)

| Offset | Type | Meaning | Evidence |
|---|---|---|---|
| 0x00 | i16 | unit id (recno) | 0x4F2190 |
| 0x0E/0x12/0x16 | f32[3] | world position | 0x4FEDF0, 0x4FEDB0 |
| 0x1A | f32 | heading, radians | 0x4FF850/0x4FF870 |
| 0x1E | f32 | heading, degrees (as set by scripts) | 0x50ABD0 |
| 0x22 | i32 | update priority class (0 closest ... 4); from 0x508880 | 0x51E280 |
| 0x2A | u32 | last update ms (`unit+0x2A`) used as turn dt origin | 0x4F2190 (short index 0x15) |
| 0x33 | ptr | formation/group record (`+2` member count, members i16 at `+3...`, `+0x38/3C/40` anchor xyz, `+0x44` heading rad, `+0x48..0x50` copy, `+0x54` heading copy) | 0x658810, 0x65D350 |
| 0x37 | i16 | leader recno | 0x4B62B0, 0x4216D0 |
| 0x3B | i16 | unit type (unit.res index) | 0x5102A0 |
| 0x102 | i16 | role flag passed as body kind (and rider test in 0x6B1DD0) | 0x5102A0 |
| 0x1E2 | i32 | linked front rider recno | 0x6B1350 |
| 0x1E6 | i32 | linked rear rider recno | 0x6B1350 |
| 0x215 | i16 | current action/animation class | 0x52C490, 0x52DAA0 |
| 0x2F5 | Actor* | actor wrapper with the Volume fields | |
| 0x335 | i32 | team tag compared with `DAT_00A45F3E` | 0x4FED00, 0x51E280 |
| 0x339 | obj* | attached/linked object (`+0x5C` = its body id) used by tracks | 0x653360 |
| 0x384 | i32 | linked object id (>= 1 skips path following) | 0x52DAA0 |
| 0x3B8 | i32 | physics disabled flag | 0x5102A0 |
| 0x3C9 | i32 | current track id | 0x427B50 |
| 0x3CD/0x3D1/0x3D5 | f32[3] | position saved when physics was disabled | 0x5102A0 |
| 0x3E6/0x3E8 | i16 | route index / sub-index | 0x52DAA0 |
| 0x402/0x406 | i16 / u32 | blocked counter / its timestamp | 0x52DAA0 |
