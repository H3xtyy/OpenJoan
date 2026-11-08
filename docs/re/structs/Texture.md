# In-memory texture records (`en3d/texturea.cpp`, `en3d/texdir.cpp`)

Reference build joan.exe. Functions: docs/re/functions/dds.md. Offsets are byte offsets; layouts are partial
(only fields the exported code touches).

## Texture (polymorphic object, refcounted)
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | void** | vtable | slot 0 = deleting destructor `(flag)` | 0x60A550 |
| 0x04..0x0B | 8 | ? | ? | TODO(re) (probably D3D texture pointer / name) | - |
| 0x0C | 4 | int32 | refcount | ++ in 0x60A500, -- in 0x60A550; destroyed at <1 | 0x60A500/0x60A550 |
| 0x10 | 4 | int32 | cache_stamp | set from manager+0x38 when (re)referenced; compared with `age` in the purge | 0x60A500, 0x60A870 |
| 0x18 | 1 | uint8 | kind | 0 -> remove from list B, nonzero -> list A | 0x60A550 |

## TextureManager (`texturea`, sizeof not known; `this` of 0x609260)
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 / 0x04 / 0x08 | 4 each | int,int,Texture** | list_a count/cap/ptr | sorted pointer array, grows by 0x40 | 0x60AC20 |
| 0x0C / 0x10 / 0x14 | 4 each | int,int,Texture** | list_b | second sorted array | 0x609260 |
| 0x18 / 0x1C / 0x20 | 4 each | int,int,Texture** | cache | textures parked with their last reference | 0x60B1B0, 0x60A870 |
| 0x24 / 0x28 / 0x2C | 4 each | int,int,Texture** | list_c | third sorted array | 0x609260, 0x60B2E0 |
| 0x30 | 4 | int32 | ? | set 0 by 0x608E10 | 0x608E10 |
| 0x34 | 4 | int32 | cache_enabled | 1 = park last-ref textures in the cache | 0x60A550 |
| 0x38 | 4 | int32 | age | current stamp (initialised 1) | 0x608E10, 0x60A500 |

(The decompiler indexes the manager as `int*` in 0x609260, so list fields appear as `param_1[0..11]`; offsets above
are `index*4`. The list pairing {0,1,2},{3,4,5},{9,10,11} there equals offsets 0x00,0x0C,0x24, with `[6..8]` = cache.)

## TextureDirectory (`texdir`, this of 0x604150..0x604440)
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | int32 | initialised | | 0x604150 |
| 0x04 / 0x08 / 0x0C | 4 each | int,int,Group* | groups count/cap/ptr | stride 0x3D | 0x604440 |
| 0x10 / 0x14 / 0x18 | 4 each | int,int,Ext* | extensions count/cap/ptr | stride 8 | 0x604310 |

## Ext (8 bytes)
`char name[6]; uint16 mask` (mask = `1 << index`) — 0x604310.

## Group (stride 0x3D = 61, packed)
| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | int32 | index | |
| 0x04 | 33 | char[0x21] | name | |
| 0x25 | 4 | int32 | entry_count | |
| 0x29 | 4 | int32 | entry_cap | |
| 0x2D | 4 | Entry* | entries | stride 0x28, kept sorted by name |
| 0x31 | 4 | int32 | dir_count | |
| 0x35 | 4 | int32 | dir_cap | |
| 0x39 | 4 | Dir* | dirs | stride 0x10C |
Evidence: 0x604440 (memset 0x3D), 0x604150, 0x604650, 0x604960.

## Dir (stride 0x10C = 268)
`char path[0x105]` (+0x00), `u8 recurse` (+0x105), `u8 files_state` (+0x106, 0/1/2), `u8 subdirs_state` (+0x107),
`u8 noscan` (+0x108); remaining bytes padding — 0x604650.

## Entry (stride 0x28 = 40)
`char name[0x22]` (+0x00, base name <= 32 chars), `u16 ext_mask` (+0x22), `i32 dir_index` (+0x24) — 0x604960.
