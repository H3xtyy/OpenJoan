# Wall (item class 4 `WALL`; `i_wall.cpp`) — joan.exe

Subclass of [Item](Item.md): allocation 0xEF bytes (ctor 0x432210 = base ctor 0x416A50, then `memset(+0xC2, 0, 0x2D)`; vtable 0x9D3C88). Template: [ItemRes](ItemRes.md) `class = 4`, `type = 4 (Castle Wall)` — 23 shipped
`.pro` files (`beaugency_w6de`, `msl_w3de`, `troyes_w1de`, `Lc_w01b_de` ...). A wall has two model *levels*: the intact wall `Model` and a *broken* model (`ModelName2: broken_*.AM1`, plus `AddModelName` debris `*.SMO`), see the wall-parse quirk in [ItemRes §2.2](ItemRes.md).
Script view = vtable slot `+0x58`. Functions: [../functions/items_facilities.md §3.4](../functions/items_facilities.md).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0xC2 | 4 | — | — | not read in the exported code | |
| 0xC6 | 4 | i32 | `hp` | `get_wall_hp` 0x650D50 / `set_wall_hp` 0x650DB0 (no class guard; invalid item returns 0) | (V) |
| 0xCA | 4 | i32 | `max_hp` | `get_wall_max_hp` 0x650E10 | (V) |
| 0xCE | 4 | — | — | unread (`TODO(re)`: break state / attack bookkeeping) | |
| 0xD2 | 4 | Actor* | `actor_a` | first extra scene object (destroyed in the dtor via 0x5EFDB0 + 0x420840); the dtor is the only reader | 0x487460 (R) |
| 0xD6 | 4 | i32 | `level_a` | model level of `actor_a`, released with `0x4A3E30(res, level_a)` | (R) |
| 0xDA | 4 | Actor* | `actor_b` | second extra scene object | (R) |
| 0xDE | 4 | i32 | `level_b` | model level of `actor_b` | (R) |
| 0xE2 | 4 | — | — | unread | |
| 0xE6 | 1 | u8 | `invincible` | `set_wall_invincible` 0x650F40 (returns the byte, or `-1` for an invalid item) | (V) |
| 0xE7 | 4 | i32 | `block_count` | number of collision block records | 0x487460 (R) |
| 0xEB | 4 | i32* | `block_ids` | indices into the world block array (freed in the dtor) | (R) |

## Behaviour that is visible in the export
* **dtor 0x487460**: base dtor 0x4251B0, then for each of `actor_a`, `actor_b`: scene unlink (0x5EFDB0 if linked), delete (0x420840), zero, release its model level; finally free `block_ids`.
* **break_wall(wall, unit)** (script id 39, handler 0x650E70): if the second argument is a valid unit and the wall item is valid, posts event type `0x40` (sub-id 2) with the wall's position `(+0x56,+0x5A,+0x5E)`, `time = now_ms (DAT_00A500BA)` and a 1000.0f parameter (`0x447A0000`) to the event queue 0x4EEF00 — the trigger system then switches the wall to its broken model/debris and removes its collision. The state transition itself is not in this export (`TODO(re)`).
* Hit points are plain fields; damage application and the "invincible" test are in the combat code (unit/battle family).
