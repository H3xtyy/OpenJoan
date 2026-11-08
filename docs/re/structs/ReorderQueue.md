# ReorderQueue (en3d `reorder.cpp`) — draw-call sorting queue

A singleton-like object embedded in the renderer (the code reaches it with `this` = renderer; `0x5E7B80()` returns
`this + 0x9E`, the *current key*). Models do not issue draw calls in queued mode; they **register batches** which the frame
flush later walks in bucket order. Reference counted for the shared node pools (`DAT_00A69A84`).
Offsets in the queue are relative to the renderer object. Status V/R/I as in [Plane.md](Plane.md).

## Queue (fields at `renderer + off`)

| offset | size | type | name | meaning | evidence | st |
|---|---|---|---|---|---|---|
| 0x8A | 4 | i32 | bucket_count | highest bucket index used + 1 | 0x5E7BB0 | V |
| 0x8E | 4 | i32 | bucket_cap | grows to `index + 0x10` | 0x5E7BB0 | V |
| 0x92 | 4 | i32 | cur_bucket | selected by `reorder_select_bucket` | 0x5E7BB0, 0x5E7CE0 | V |
| 0x96 | 4 | Bucket* | buckets | array of 16-byte buckets | 0x5E7BB0 | V |
| 0x9A | 4 | Item* | cur_item | item found/created by the last `reorder_acquire_item` | 0x5E7CE0 | V |
| 0x9E | 0x28 | Item | cur_key | scratch key being filled by callers before acquiring (+0 light-set handle, +4 vertex type byte, +8 effect flags, +0xC material-cache handle, +0x10 primitive kind) | 0x5D35B0, 0x5F03B0 | V |

## Bucket (16 bytes)

| off | type | meaning |
|---|---|---|
| 0x00 | i32 | item count |
| 0x04 | i32 | item capacity (grows `count + 0x80`) |
| 0x08 | i32 | unused / TODO(re) |
| 0x0C | Item* | items, kept **sorted by key** (binary search `0x5911A0`, insert with shift `0x5910F0`; comparator at 0x403B61, not exported — presumably lexicographic compare of the first 0x14 key bytes) |

Bucket index = draw order. Models use `draw_category` (1..7) as the bucket; actor-attached models (`category 4`) with
`*(actor+8) > 0` use `*(actor+8) + 9`. Index is clamped to >= 0 and grown on demand.

## Item (0x28 bytes)

| off | size | name | meaning |
|---|---|---|---|
| 0x00 | 4 | light_set | handle into the global light-set cache (record 0x8D bytes), assigned by `0x5E7870` from the sector |
| 0x04 | 1 | vertex_type | |
| 0x08 | 4 | fx_flags | `0x5D5F60`: bit0 bump, 0x40, 0x80 (see Plane `+0xF8/+0x100/+0x104`) |
| 0x0C | 4 | material | handle into the global material-state cache (record 0x5C bytes: `+0` handle, `+4` last-use frame, `+8` copy of a 0x54-byte [PlaneMaterial](PlaneMaterial.md)); found by `memcmp` of the 0x54 bytes, searched newest-first, else a new record is allocated (`0x5E79F0`) |
| 0x10 | 4 | prim_kind | 4 = indexed triangle list, 5 = indexed triangle strip (strip mode), 1 = point list, 2 = line list |
| 0x14 | 4 | last_frame | stamp = `DAT_00A500AE` (frame counter) written on acquire; lets the caches evict unused records |
| 0x18 | 4 | vpkt_head | linked list of 0x20-byte *vertex-stream packets* |
| 0x1C | 4 | vpkt_cur | cursor: packets are **recycled across frames** (reset cursor = reuse, list is only freed at shutdown by `0x5E8250`) |
| 0x20 | 4 | ipkt_head | linked list of 0x30-byte *indexed packets* |
| 0x24 | 4 | ipkt_cur | cursor |

Key copy for a new item: `0x5E8210` copies 5 dwords (0x14 bytes) from the current key, zeroing the item first.
Two items with equal key are the same render-state batch, so state changes are minimised.

## Packets

| packet | size | linked via | fields |
|---|---|---|---|
| vertex packet (points/lines, `0x5E8400`, wrapper `0x5E7E70`) | 0x20 | `+0x1C` | `+0` count, `+4` first vertex (0), `+8` system vertex pointer (`base + first*stride`), `+0xC` vb object (0), `+0x10` primitive count, `+0x14` PlanePart array (null = identity world), `+0x18` part count |
| indexed packet, system memory (`0x5E8540`, wrapper `0x5E7EC0`) | 0x30 | `+0x2C` | `+0x00` vertex count (`maxIndex-minIndex`), `+0x04` min vertex, `+0x08` vertex array pointer, `+0x0C` vb (0), `+0x10` index count, `+0x14` first index (0), `+0x18` index pointer (`indices + first*2`), `+0x1C` ib (0), `+0x20` primitive count, `+0x24` PlanePart array, `+0x28` part count. If the caller passes no valid vertex range it scans the index range to derive min/max |
| indexed packet, GPU buffers (`0x5E88C0`, wrapper `0x5E7F70`) | 0x30 | `+0x2C` | `+0x00` vertex count, `+0x04` min vertex, `+0x08` 0, `+0x0C` vertex buffer object, `+0x10` index count, `+0x14` first index, `+0x18` 0, `+0x1C` index buffer object, `+0x20..0x28` as above |

Flush (0x5E67B0): global order is **light-set handle first, then bucket, then key** — see
[../functions/scene.md](../functions/scene.md) "Reorder-queue additions". An item is only drawn when its `last_frame` equals the current frame
counter and its material pass equals the requested pass.

Pools: free lists `DAT_00A69A74` (0x20-byte nodes, count `DAT_00A69A80`) and `DAT_00A69A78` (0x30-byte nodes, count
`DAT_00A69A7C`); at most 0x28 (40) nodes are retained per pool (`0x5E8D60`, `0x5E8F10`), extra nodes are freed.
`0x5E8B20` drops one reference and frees both pools when the count hits 0. `0x5E61C0` is the queue shutdown (frees every item,
the buckets, the material and light-set caches via `0x5D5360`/`0x5C5B50`).
