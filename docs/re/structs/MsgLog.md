# MsgLog and MsgRecord (`msglog.cpp`) — joan.exe

In-game message log (the "message_log_interface" / "message_array_interface" screens show it). Only three functions of `msglog.cpp` were exported: record init,
clear-all and remove-one; the add/prune policy (capacity, expiry timing) is in callers that are **not** in this export -> `TODO(re)`; no capacity constant or
timer appears in these three functions. Records live in a global `ilib::dynarrb` of `MsgRecord*` (element access 0x41B3F0 via wrappers 0x4865F0 "get" and 0x486620 "is deleted").
Functions: [gameloop.md §5](../functions/gameloop.md).

## MsgLog (this of 0x4B2C00 / 0x4B2F10)

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x2E | 4 | i32 | `count` | live records | verified |
| 0x32 | 4 | i32 | `cursor` | dynarr index of the "current/newest" record (0 = none); moved to `rec.link_a` when that record is removed | read |
| 0x3A | 9 x 4 | i32[9] | `cat_head[cat]` | per-category pointer (dynarr index) | read |
| 0x5E | 9 x 4 | i32[9] | `cat_count[cat]` | per-category record count (nine categories, ids 0..8; the arrays are 0x24 bytes each) | read |

## MsgRecord (packed)

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 2 | i16 | `category` | selects `cat_head/cat_count` | verified |
| 0x02 | 1 | u8 | `flags` | init argument 3 | read |
| 0x03 | 4 | i32 | `index` | the record's own dynarr index (compared with `cat_head` on removal; set by the caller, not by `init`) | read |
| 0x07 | 4 | i32 | `link_a` | neighbour index; on removal the global `cursor` moves here and the neighbour's `link_b` is patched | read |
| 0x0B | 4 | i32 | `link_b` | other neighbour; on removal `cat_head[cat]` moves here if it is the same category | read |
| 0x0F | 4 | i32 | `value` | init argument 5 (probably a timestamp or id) | read |
| 0x13 | 2 | u16 | `text_len` | `strlen(text)` | verified |
| 0x15 | 2 | u16 | `extra_len` | `strlen(extra)`, 0 when absent | verified |
| 0x17 | 4 | char* | `text` | heap copy (`len+1`), freed on removal | verified |
| 0x1B | 4 | char* | `extra` | heap copy, null when none | verified |

Removal (0x4B2F10): `cat_count[c]--`; if it reaches 0 `cat_head[c]=0`, else if `cat_head[c]==rec.index` and the `link_b` neighbour has the same category it becomes the head; unlink from both neighbours; free strings and the record; `count--`.
