# PathRoute — the "path object" a unit follows (`pathsp.cpp` neighbourhood) — joan.exe

The result of a path request and the object `unit_follow_path` (`0x52DAA0`) reads. **0x41 bytes**, embedded in the unit: two instances `unit+0x432` and
`unit+0x473` (0x41 apart; `0x473` is passed to the search as the "owner tag"). It is a packed `ilib::dynarr` (header 0x1D bytes, unaligned data pointer at
`+0x19`) followed by one inline "head" record. Functions: [pathfind.md](../functions/pathfind.md) §Route.

## Layout
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `cap` | allocated elements | 0x584580, 0x584620 |
| 0x04 | 4 | i32 | `quantum` | growth step = 0x10 | 0x66E4B0 (`dynarr(0x24, 0x10, 0)`) |
| 0x08 | 4 | i32 | `cursor` | last appended index (= `count` after an append) | 0x584620 |
| 0x0C | 4 | i32 | `count` | number of stored records `n` | 0x584620, 0x417200 |
| 0x10 | 4 | i32 | `elem_size` | 0x24 | 0x66E4B0 |
| 0x14 | 5 | bytes | (header) | unused here | |
| 0x19 | 4 | RouteRec* | `data` | record array, 1-based storage `S[1..n]` | 0x417200, 0x584620 |
| 0x1D | 0x24 | RouteRec | `head` | logical point 0 = the start | 0x66E8D0 (index 0), 0x66E640 |

## RouteRec (0x24 bytes = 9 dwords)
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 12 | f32[3] | `pos` | the waypoint x, y, z (world frame after `0x66FA00` shifts it by the block origin) — **this is what the unit follows** | 0x66ECE0 writes it, 0x505DD0 reads the first 3 floats |
| 0x0C | 12 | i32[3] | `left` | portal end 1 (path-space ints; for the start/goal record equal to the point) | 0x66E790, 0x66E720 |
| 0x18 | 12 | i32[3] | `right` | portal end 2 | 0x66E790 |

## Logical order (important)
`0x66E8D0(route, i)`: `i == 0` -> `head`; `1 <= i <= n` -> `S[n + 1 - i]` (the **last stored** record is logical point 1); out of range -> NULL. The builder
appends the portals while walking from the goal back to the start, so `S[1]` is the goal and `S[n]` the portal next to the start; reading in logical order
therefore gives `start (0), portal 1, ..., portal n-1, goal (n)`. `0x66E8A0` returns `n` (so a route has `n + 1` points, indices `0..n`; `n == 0` = no path).
Stored-order access: `0x66E940` = `S[1]` (the goal / partial-path end).

## Serialised form (`0x66EB90` write, `0x66E970` read; used for sync/save)
`u16 count` (= `n`), `u16 0x24`, the `head` record (0x24 B), then — only when the predicate `0x43C420` (TODO(re): not understood, 36-byte wrapper over the count getter) returns 0 — `S[1]`, then `S[2..n]` in stored order, 0x24 B each (a trailing-record count of `n` records). The reader rebuilds with `0x66E640` + `0x66E790` + copy-over of each record. Returns the number of bytes used.

## Operations (all in pathfind.md)
`0x66E4B0` construct (empty), `0x66E4E0`/`0x66E5B0` copy (head 36 B + all records), `0x66E640(a, b)` reset: clear, `head = a`, append `b` (the builder passes the
start and the goal), `0x66E720(x,y,z)` overwrite `S[1]` (used to set / correct the end point), `0x66E790` append a portal record (float mid = average of both ends),
`0x66E840` append a prepared record, `0x66E870` insert a record at storage index 1, `0x66FA00(dx,dy,dz)` translate all points (incl. head), `0x66FAA0(sxy, sz)` scale all points.
