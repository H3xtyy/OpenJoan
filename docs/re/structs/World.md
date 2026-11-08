# World (`world.cpp`) container, WorldBlock and BVH node — joan.exe

`World` owns the terrain/object *blocks* of a scenario map, a bounding-volume hierarchy over them, and the en3d scenes. Scene/block internals
belong to the scene family (`docs/re/functions/scene.md`, [WorldGridCell](WorldGridCell.md), [PlaneChunks](PlaneChunks.md)); this page only records the container layout.
Functions: [gameloop.md §7](../functions/gameloop.md). Pointer arrays are **1-based** (`arr[i-1]`).

## World (this of 0x547BF0 / 0x440320 / 0x44A260 / 0x42CE10 / 0x4426A0)

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `used` | cleared by `destroy` | 0x440320: read |
| 0x01 | 1 | u8 | `loaded` | `unload_contents` runs only when set; cleared at its end | 0x4426A0: read |
| 0x60 | 1 | u8 | `flag60` | cleared by `destroy` | read |
| 0x6A | 4 | obj* | `obj6a` | deleted with 0x4201A0(1) on unload | read |
| 0x6E | 4 | obj* | `obj6e` | deleted with 0x4201F0(1) on unload | read |
| 0x72 | 4 | i32 | `block_count` | number of `WorldBlock` | verified (4 fns) |
| 0x76 | 4 | i32 | `block_cap` | capacity; grows to `count + 0x10 + n` | 0x547BF0: read |
| 0x7A | 4 | WorldBlock** | `blocks` | pointer array | verified |
| 0x7E | 4 | BvhNode* | `bvh` | `2*block_count-1` nodes (0x38 bytes) allocated with an element-count cookie; null = not built | 0x44A260, 0x54A040: verified |
| 0x82 | 4 | ptr* | `block_aux` | second array, same capacity (grown together) | 0x547BF0, 0x440320: read |
| 0x86 | 4 | i32 | `scene_count` | en3d scenes | 0x42CE10: read |
| 0x8A | 4 | i32 | `scene_cap` | grows by 10 | read |
| 0x8E | 4 | Scene** | `scenes` | `0xE4`-byte scene objects created by 0x5EA460 (en3d `scene.cpp`), each holding a back pointer to the scene container | read |

## WorldBlock (0xAC bytes, ctor 0x41B5C0)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | World* | `world` | back pointer |
| 0x04 | 4 | BvhNode* | `leaf` | set by the BVH builder |
| 0x08 | 24 | f32[6] | `aabb` | min xyz, max xyz (copied into leaf nodes) |
| 0x20.. | | | | `TODO(re)` — remainder is scene-family data |

## BvhNode (0x38 bytes)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | BvhNode* | `parent` | |
| 0x04 / 0x08 | 4+4 | BvhNode* | `left`, `right` | null on leaves |
| 0x0C | 4 | i32 | `axis` | split axis 0=X, 1=Y, 2=Z (longest extent; ties prefer lower axis: X unless Y or Z strictly larger) |
| 0x10 | 24 | f32[6] | `aabb` | union of children / the block's own box |
| 0x28 | 12 | f32[3] | `center` | box centre used as split key |
| 0x34 | 4 | WorldBlock* | `block` | leaf only |

Build (0x548880): box = union of blocks `[lo,hi)`; if all three extents equal `_DAT_009CF5C4` (0.0) split at `(lo+hi)/2`, else partition the pointer slice around the centre
on the chosen axis (0x5499B0 + comparator 0x549C30/0x549DC0/0x549F90). Used for view culling / picking (consumers are in the scene family).
