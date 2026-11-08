# BlockArray (uti3d `blkarrb.cpp`) — handle-based block pool

Allocator used for the engine's global caches (material-state cache record 0x5C B, light record 0xD6 B, light-set record
0x8D B: all accessed through `0x417200(handle)` get, `0x417ad0()` high-water count, `0x417520()` allocate). Handles are
**1-based**; handle 0 is "none". Elements never move (stable addresses), the block table can be reallocated.

| off | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | u8** | blocks | table of block pointers | 0x637700 |
| 0x04 | 4 | i32 | block_table_cap | grows `used_blocks + 15` | 0x637700 |
| 0x08 | 4 | i32 | elem_size | bytes per element | 0x637700 |
| 0x0C | 4 | i32 | per_block | elements per block (block alloc = `per_block*elem_size`); also the growth quantum of the lists below | 0x637700 |
| 0x10 | 4 | i32 | high_water | number of handles ever handed out | 0x637700 |
| 0x14 | 4 | i32 | block_count | `ceil(high_water / per_block)` | 0x637700 |
| 0x18 | 4 | i32* | free_stack | recycled handles (LIFO) | 0x637700, 0x6379F0 |
| 0x1C | 4 | i32 | free_count | | 0x637700 |
| 0x20 | 4 | i32 | free_cap | grows `count + 1 + per_block` | 0x6379F0 |

Derived list class (adds an ordered handle list): `+0x24` i32* handles, `+0x28` cap (`+= count + per_block`), `+0x2C` count.

| addr | name | behaviour |
|---|---|---|
| 0x637700 | blkarr_alloc(init) | pop a free handle, else `++high_water` (allocating whole blocks as needed); element = copy of `init` (elem_size bytes) or zero; returns handle. Address = `blocks[(h-1)/per_block] + ((h-1)%per_block)*elem_size` |
| 0x6379F0 | blkarr_free_handle | push handle on `free_stack` |
| 0x6375B0 | blkarr_clear | free all blocks, table and free stack, zero counters |
| 0x637B00 | blkarrlist_clear | free the handle list, then `blkarr_clear` |
| 0x637BE0 | blkarrlist_add | allocate element (copy of arg), append its handle, return new count (1-based position) |
| 0x637CB0 | blkarrlist_add_n | allocate `n` zeroed elements, append handles, return the 1-based position of the first |
