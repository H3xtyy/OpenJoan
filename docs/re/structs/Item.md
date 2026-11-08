# Item (world item *instance*; `itemres.cpp` factory, `i_gate.cpp`, `i_wall.cpp`, base dtor 0x4251B0) — joan.exe

An `Item` is one placed object of the running scenario: a door, a wall, a treasure box, a torch, a pick-up lying on the floor, a held weapon. Template data lives in
[ItemRes](ItemRes.md) (`ItemRes.recno` is stored at `+0x0A`). Instances live in the *world item array* (a `dynarrb` of object pointers, 1-based, `count` at `+0x0C`, element
stride at `+0x10`, data pointer at `+0x19` of the container; getters `0x425D20(recno) = ptr`, `0x425D50(recno) = (ptr == 0)` "invalid"). Script `item` arguments are these recnos.
Subclasses: [Gate](Gate.md) (class 1), [Wall](Wall.md) (class 4); box (class 10) fields are in §3. Functions: [../functions/items_facilities.md §3](../functions/items_facilities.md).

## 1. Common header (all classes, base object = 0xC2 bytes, ctor 0x416A50: `vptr`, then `memset(+4, 0, 0xBE)`)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | vptr | `vtbl` | per-class vtable (base 0x9D3D78, gate 0x9D388C, wall 0x9D3C88); slots used: `+0x00` deleting dtor(flag), `+0x08` init(res_id), `+0x18`/`+0x1C` copy-state hooks (0x65BCE0), `+0x4C` as-gate, `+0x50` as-box, `+0x58` as-wall, `+0x64` set-path-owner | 0x49EA90, 0x49F370, script handlers (V) |
| 0x04 | 2 | i16 | `recno` | own array index, written by the factory after registration (0x417520 = "last allocated recno") | 0x49EA90 (V) |
| 0x06 | 2 | i16 | `modify_id` | handle in the per-tick *modify_item* list (registered by 0x4AFA80 for classes accepted by 0x4A4050, released by 0x4AFAF0 on delete); also the target id of the delayed event posted by `add_gate_trigger` | 0x49EA90, 0x49F370, 0x65C530 (R) |
| 0x08 | 2 | i16 | `nation` | owner nation (set by `set_item_nation`, `add_item`; 0 = neutral); units inherit it in the floor-holder | 0x651AD0, 0x652010 (V) |
| 0x0A | 2 | i16 | `res` | `ItemRes.recno` | 0x49EA90 (V) |
| 0x0C | 64 | char[64] | `name` | instance name (≤ 0x3F + NUL): `add_item_box` copies the script-supplied box name; MISSION (15) and CROSS (19) instances copy `ItemRes.name`. `search_item_by_name` compares this field (case-insens., scans from the **last** item down to 1) | 0x651C90, 0x49EA90, 0x49F420 (V) |
| 0x4E | 2 | i16 | `class` | copy of `ItemRes.class`; script predicates `is_item_gate` (==1), `is_item_wall` (==4), `is_item_box` (==10) test this | 0x65B6A0/0x65B700/0x65B760 (V) |
| 0x52 | 4 | i32 | `level` | currently shown model level (index into `ItemRes.level_*`; 0 = base `model`); released with `0x4A3E30(res, level)` on delete | 0x4251B0 (R) |
| 0x56 | 4 | f32 | `x` | world position | `get_item_pos_x` 0x65EA50 (V) |
| 0x5A | 4 | f32 | `y` | | (V) |
| 0x5E | 4 | f32 | `z` | | (V) |
| 0x7E | 4 | u32 | `interact_flags` | `0x200` = can be used with the Interact key; `set_item_can_interact_flag(item, on)` stores `on ? 0x200 : 0`; LADDER (class 11) items are created with `0x200` | 0x65E930, 0x49EA90 (V) |
| 0x82 | 1 | u8 | — | cleared by the base dtor | 0x4251B0 (R) |
| 0x86 | 4 | Actor* | `actor` | the scene object (../../engine/actor.md); `actor+0x18` = scene link, `actor+0xB8` = collision-type override (gate: set to `ItemRes.collision_type` while closed, 0 while open) | 0x48D4B0 (V) |
| 0x8E | 4 | Actor* | `actor2` | secondary actor (shadow/second model); freed by the base dtor | 0x4251B0 (R) |
| 0x92 | 2 | i16 | `light` | engine-light handle (`HasLight`) freed by 0x49C1A0 | (R) |
| 0x94 | 2 | i16 | `light2d` | 2-D light handle (`HasLight2d`) freed by 0x49C920 | (R) |
| 0x96 | 4 | i16[2] | `emitter_a` | particle-emitter instance recnos (emitter slots 0,1) | 0x4251B0 (R) |
| 0x9A | 4 | i16[2] | `emitter_b` | emitter slots 2,3 | (R) |
| 0xA2 | 4 | FloorItem* | `floor` | non-NULL while the item lies on the floor as a pick-up (script `is_item_on_floor` returns it); position then comes from `floor+0x12/0x16/0x1A`, else from `+0x56..` | 0x65EA00/0x65EA50 (V) |
| 0xA6 | 4 | ptr | `scene` | world/scene context pointer (set by `0x4994B0(DAT_00A45F3E)` when placed; clearing it also unregisters the collision at `+0xAA`) | 0x4994B0 (R) |
| 0xAA | 4 | ptr | `collision` | collision registration (non-NULL while registered) | 0x48D4B0, 0x4994B0 (R) |
| 0xAE..0xC1 | 20 | — | — | not read in the exported code | |

Subclass tails start at `+0xC2` (see below). Allocation sizes by class (factory `0x49EA90(res_id)`, returns the new recno; V):

| class | sizes | ctor | note |
|---|---|---|---|
| 1 GATE | 0xDB | 0x48C250 | [Gate](Gate.md) |
| 2 OBSTACLE, 14 PATH | 0xCE | 0x434380, 0x4352B0 | tail unknown |
| 3 HOUSE | 0xCE | 0x48EA70 | |
| 4 WALL | 0xEF | 0x432210 | [Wall](Wall.md) |
| 10 BOX | 0xCD | 0x489EA0 | §3 |
| 13 SWITCH | 0x107 | 0x434040 | |
| 18 KEG | 0xC2 | 0x48F740 | |
| 15 MISSION, 19 CROSS | 0xC2 | 0x416A50 | + copy `ItemRes.name` into `+0x0C` |
| all others (5 TERRAIN, 6 WEAPON, 7 POTION, 8 GOLD, 9 ENHANCER, 11 LADDER, 12 WAVE, 16 ARMOR, 17 MISC, 20 PTERRAIN) | 0xC2 | 0x416A50 | |

After construction: `vtbl slot +8 (res_id)` initialises from the template; classes {1,2,3,4,6,7,8,9,10,11,13,14,15,17,18,19,20} (predicate 0x4A4050) additionally register in the modify list (0x4AFA80).

## 2. FloorItem holder (0x52 bytes, pointed to by `Item.floor`)

Built by `add_item` (0x651AD0, only when `ItemRes.pickable != 0`) and by the floor spawn 0x512830. Ctor 0x456EB0 zeroes `+0x12..+0x51`; the first 0x12 bytes are an [ItemStack](Inventory.md) (`0x456CD0(item, level=min_level, quantity=ItemRes.quantity)`).
`+0x32 i16` = nation of the owner, `+0x34 i16` = back-pointer to `Item.recno`; position/scene are set with 0x457120 / 0x456FE0 / 0x4571B0 (position, angle). Picking it up hands the stack to the unit inventory (0x495F40).

## 3. Box (class 10, 0xCD bytes)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0xC2 | 2 | i16 | `content_item` | item recno inside the box (0 = empty) | 0x651C90, `.uni` loader 0x4F3DF0 (V) |
| 0xC4 | 4 | i32 | `content_level` | item level of the contents (default `ItemRes[content].min_level`, or `TreasureLevel` from the `.uni`, or the level rolled from a drop list) | (V) |
| 0xC8..0xCC | 5 | — | — | unread | |

`add_item_box(code, name, x, y, z, rot_deg, contents)` (0x651C90): `contents` is looked up **first as a DROPITEM list name** (0x463ED0) and only then as an item code (0x4A36C0). A list is rolled once, at creation, with the weighted pick of [Inventory §6](Inventory.md).
The `.uni` object loader (0x4F3DF0, world family) does the same for placed boxes, plus an optional `TreasureLevel n`.
The vtable `+0x50` accessor returns the box view; `+0x4C` = gate view, `+0x58` = wall view (all return the same object).

## 4. Lifecycle

* **create**: `item_create(res)` 0x49EA90 → new subclass object → `vtbl slot +8 (res)` → append to the world array (0x5857A0) → `recno = 0x417520()` → `modify` list registration. Script `add_item`/`add_item_box` then set nation (`+0x08`), position (`0x4994B0(scene); 0x41FF50(x,y,z)`), yaw (`deg * π/180` via 0x499FD0), name (`+0x0C`), and create the floor holder for pickables.
* **delete** `item_delete(recno)` 0x49F370: unregister from the modify list (`0x4AFAF0(+0x06)`), `vtbl slot 0 with arg 1` (deleting dtor: subclass part, then base 0x4251B0), remove the array slot (0x5858C0). `del_item_by_name` deletes **all** matches (loops `search_item_by_name` until 0).
* **base dtor 0x4251B0** order: clear `scene` (0x4994B0(0)) → detach emitter instances whose attach target is this item's actor (0x41B0D0) → free `actor2` → release lights (0x49C1A0, 0x49C920) → free emitter slots 0,1 → free `actor` and release the model level (`0x4A3E30(res, level)`) → free the floor holder → clear `+0x82`.
