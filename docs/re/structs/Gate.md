# Gate (item class 1 `GATE`; `i_gate.cpp`) — joan.exe

Subclass of [Item](Item.md): allocation 0xDB bytes (ctor 0x48C250 = base ctor 0x416A50, then `memset(+0xC2, 0, 0x19)`; vtable 0x9D388C). Template: [ItemRes](ItemRes.md) with `class = 1`,
`type = 1 (Castle Gate)`; the 22 shipped gate `.pro` files are castle gates, wooden doors, prison doors, a drawbridge (`Bridge.pro`), trap doors.
Script view = vtable slot `+0x4C` (returns the object itself). Functions: [../functions/items_facilities.md §3.3](../functions/items_facilities.md).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0xC2 | 4 | i32 | `hp` | current hit points (`get_gate_hp` 0x650B90 returns it only when `Item.class == 1`, else 0; `set_gate_hp` 0x650C90 writes it unguarded) | (V) |
| 0xC6 | 4 | i32 | `max_hp` | `get_gate_max_hp` 0x650C10 / `set_gate_max_hp` 0x650CF0 | (V) |
| 0xCA | 4 | i32 | — | third integer, never touched by the exported code (`TODO(re)`) | |
| 0xCE | 1 | u8 | `need_key` | `set_gate_need_key` 0x652520 / `get_gate_need_key` 0x652590; `-1` is returned for an invalid item | (V) |
| 0xCF | 1 | u8 | `invincible` | `set_gate_invincible` 0x6525F0 | (V) |
| 0xD0 | 1 | i8 | `attacked` | `get_gate_attacked` 0x652660 (`-1` when the view pointer is NULL) | (V) |
| 0xD1 | 1 | i8 | `state` | `get_gate_state` 0x6524A0 (`-1` for invalid item or `Item.class != 1`). Script usage (S3L1 1436-1509): `== 1` ⇒ open, `== 0` ⇒ closed (then `add_gate_trigger(gate, 0)` is sent to open it). Other values (broken?) and the writers are not in the export; `TODO(re)` | (R, I) |
| 0xD2 | 1 | u8 | `closed` | collision-applied flag toggled by 0x48D4B0's callers; the dtor 0x48C410 forces the opposite state before tearing down (`0 → apply(1)`, `1 → apply(0)`) | 0x48C410 (R) |
| 0xD3 | 4 | i32 | `block_count` | number of collision *block records* created for the gate (`= ItemRes-model block count at model+0x28`) | 0x48D4B0 (R) |
| 0xD7 | 4 | i32* | `block_ids` | `block_count` indices into the world block array (`world_s` records of 0x74 bytes, allocator 0x560C70, getter 0x560D30 `= base + i*0x74`); freed in the dtor | 0x48D4B0, 0x48C410 (R) |

## Open / close = collision toggling (`gate_apply(close)` 0x48D4B0, `__thiscall`, `char close`)

* Returns 0 = nothing changed, non-zero = applied. If `ItemRes.not_block` (`+0x3E`) is set, nothing is done (returns 0).
* **Closing** (`close != 0`) first checks that nobody stands in the doorway: for every scene-sector actor in an axis box of half-size `DAT_009CFCA0 + DAT_009CF9C4` around the gate (cells from 0x5EF610 / 0x5EFFA0) it looks at *unit* actors (actor kind 0xB) other than the gate's own actor; if the unit definition (unitres record, stride 0x203, getter 0x42D0A0) has `+0x02 == 8`, or its flag `+0x1E2 != 0` (0x430190), or `+0x118 == 2`, **and** its XY squared distance to the gate is `<= DAT_009D3968`, the call returns 0 and the door stays open. (Constants not yet read, `TODO(re)`.)
* Then it (un)registers the gate collision with the world (0x6AF710 for models without a bounding box, 0x6AFB10 with the model's `col_*` box transformed by the actor matrix; both selected by `ItemRes.has_bbox` and `item_usage`) and, on success:
  * *open* (`close == 0`): every block record `block_ids[i]` gets `+0x70 = 0` (disabled) and the actor's collision override `actor+0xB8 = 0` (+ 0x6AD980 refresh);
  * *close*: on first use it allocates `block_count` block records (0x560C70) and fills four vectors per record (`+0x30` matrix via 0x60D920, `+0x40/+0x4C/+0x58/+0x64` from 0x60CC90) from the model's block geometry (`ItemRes` model data `+0x28` = block count); later calls just set `+0x70 = 1`; `actor+0xB8 = ItemRes.collision_type` (+0x1E9), then 0x6AD980.
* The visual open/close is the actor's `AnimName` animation of the current model level (`ItemRes.level_anim`, e.g. `default`) — driven by the interact code (not exported). Sound ids: `sound` (open/close), `sound2`, `sound3` (break) from the template.

## Gate rules relevant for scenario scripts
* `add_gate_trigger(gate, delay_ms)` (0x65C530) posts a delayed trigger event `0x20` with sub-id 2 to the event queue (0x4EEF00), target = `Item.modify_id` (+0x06), `time = now_ms (DAT_00A500BA) + delay_ms`, source = the player unit (`DAT_00A5B9E8`). S1L1 passes 0 for the bridge and 1 for the gate — i.e. "fire at once"; S3L1 uses it to open a gate that `get_gate_state` reports as 0. The event handler (trigger system 0x4EF090, event type 0x20) performs the open/close; it is not exported (`TODO(re)`).
* `get_gate_hp/max_hp/state/attacked/need_key` are plain field reads/writes (above). Who subtracts damage from `hp`, honours `invincible` and sets `attacked`/`state` is in the combat/interact code (unit/battle family, not exported): `TODO(re)` — until then an implementation must treat `hp <= 0` as "destroyed" (the `Sound3` = break sound and the `Door_broke_dust` emitter of the template suggest the break transition).
