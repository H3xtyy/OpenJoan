# world — binary level files (functions)

`joan.exe`. Export: `decomp/binfmt/world/`. Formats: [pln](../../formats/pln.md), [gri](../../formats/gri.md),
[map](../../formats/map.md), [pfd](../../formats/pfd.md), [nod](../../formats/nod.md), [scenario_res](../../formats/scenario_res.md).
Structs: [PlnSectorEntry](../structs/PlnSectorEntry.md), [PlaneChunks](../structs/PlaneChunks.md), [WorldGridCell](../structs/WorldGridCell.md).

## Level / terrain (world_s.cpp)
| address | proposed name | role | status |
|---|---|---|---|
| 0x4F2F80 | world_load_level | loads `.cm/.trk/.stg/.wor/.por`, then tries `.pln` (if `DAT_00A0B6AC`), else `.gri` + terrain build | read |
| 0x4F6EB0 | world_gri_load | reads `.gri` into the cell grid; creates one scene sector per `cell[0]!=0` | read (layer decoder 0x59DC10 documented from disassembly in docs/formats/gri.md, status: disassembled, not decompiled) |
| 0x558140 | world_gri_load_texmap | reduced `.gri` reader (header, names, 16 layers; keeps tex-id layers) used after a successful `.pln` load | read |
| 0x55F320 | world_pln_load | `.pln` reader (hw-variant flag, version 3, table, per-sector plane) | verified vs S1l1.pln header/table |
| 0x55B720 | world_terrain_build | builds sector meshes from cells (64-unit quads, checkerboard diagonals, subdivision, lightmaps `"lightmap_default"`) and, if `DAT_00A0B6B0`, writes the `.pln` | read |
| 0x55F9B0 | world_pln_write_sector | writes `u32 1`, 0x21-byte name, plane | read |
| 0x558890 / 0x558A60 | (text, see uni.md) | `.uni` | — |

## Plane (en3d/plane.cpp)
| address | proposed name | role | status |
|---|---|---|---|
| 0x5E26C0 | plane_read | reads chunked mesh PMAT/PVTX/PTRI/PSTP/PEX1/PEX2/PEND | verified (writer mirrors it) |
| 0x5E2380 | plane_write | writer of the same layout | read |
| 0x5E2B70 | plane_read_material | 0x54-byte material + 4 texture slots | read |
| 0x5CC7A0 | plane_ctor | `new 0x11C`, part array (0xA0 each) | read |
| 0x5CCAA0 | plane_clear | frees arrays/D3D buffers | read |
| 0x5CDC70 | plane_set_vertex_type | converts/allocs vertex array for type 1..8 (strides 0x24,0x3C,0x1C,0x18,0x48,0x14,0x48,0x28); confirmed against pseudocode (also in smo.md) | verified |
| 0x5CDAE0 | plane_get_vertices | returns vertex array + stride by type | read |
| 0x5CD7F0 / 0x5CE6F0 / 0x5CD470 | plane_add_vertices / add_triangles / add_materials | grow arrays, return old count | read |
| 0x5CF1D0 | plane_build_vertex_tri_index | per-vertex triangle lists and min/max index ranges (post-load) | read |
| 0x5CFA10 | plane_upload_buffers | creates/locks D3D vertex+index buffers ("Cannot create vertex buffer (%s)") | read |
| 0x5D6EE0 / 0x5D7270 | plane_set_material_tri_count / plane_set_sort_mode | PSTP data | read |

## File class (ilib/filemem.cpp, file.cpp)
| address | proposed name | role |
|---|---|---|
| 0x588000 | filemem_open | load whole file; `+0x106` = report/retry errors, `+0x105` = packed (u16-chunk) mode, `+0x108` buffer ptr |
| 0x5893C0 / 0x589220 / 0x5892F0 | file_read_u32 / read_u16 / read_u16(chunk header) | |
| 0x588A80 | file_read_bytes(dst, n) | zero-fills short reads; retry prompt "Error reading file %s, Retry ?" |
| 0x588C60 | file_read_array(dst, count, elem) | packed mode resizes element size |
| 0x5888C0 / 0x5889F0 | file_seek(pos, whence) / file_tell | |
| 0x586F00 / 0x587DA0 / 0x587BA0 / 0x5872E0 / 0x587510 | file_create / write_u32 / write_u16 / write_bytes / write_array | |

## Others in this export (not world binary formats)
| address | role | status |
|---|---|---|
| 0x47DE30, 0x47F800, 0x4820D0, 0x483430, 0x483CA0, 0x483F10, 0x4842C0, 0x4848A0, 0x484E40, 0x485130, 0x4864B0 (`gfile3.cpp`) | typed-record reader: `read_u16` type id then class dispatch (types 1,2,3,4,10,13,14,18,default; object sizes 0xDB,0xCE,0xCE,0xEF,0xC2,0xCD,0x107,0xCE,0xC2; unit-like block 0x4B4 in 0x483F10). Looks like game-state/save serialisation (to be assigned to the `sav` family), **not** `.map/.pfd/.gri/.pln/.nod` | inferred |
| 0x4A4CA0 / 0x4A4C30 (`levelres.cpp`) | loads res table `"LEVEL"` rows of 14 text chars -> 7-byte records (u8 [0..1], u32 [2..10], u8 [11..12], u8 [13..14]) via `atoi` | read |
| 0x5EF900.. (`en3d/scene_s.cpp`) | runtime scene sector render/visibility, no file format | read |

## Cross-module requests
- Rename `FUN_005893c0`->`file_read_u32`, `FUN_00588a80`->`file_read_bytes`, `FUN_00588c60`->`file_read_array`, `FUN_005888c0`->`file_seek`, `FUN_00587da0`->`file_write_u32`, `FUN_005872e0`->`file_write_bytes`; `FUN_005e26c0`->`plane_read`.
- Export needed: 0x59DC10/0x59D190/0x59D1B0 (gri layer decoder), 0x54E6A0/0x54E930/0x54EB20 (sector table), 0x5E2A90 (material writer), `.map/.pfd/.nod` readers (search for loops reading `u16 0x40 / 0x80` buffers; none reference the extension strings), data at `DAT_00A17D88` (vertex-type table), `DAT_009D81B0..9D81DC` (extension literals of 0x4F2F80), `DAT_00A52508/00A524FC/00A52623` (device caps).
