# Model (0x11C bytes, en3d `plane.cpp`) and SubMesh (0x54 bytes)

Constructor `0x5CBAF0` (memset 0x11C). Offsets from `0x5CD470`, `0x5CD7F0`, `0x5CFA10`, `0x5E0530`, `0x5CC7A0`.

## Model

| offset | size | type | name | meaning / evidence |
|---|---|---|---|---|
| 0x04 | 1 | u8 | vertex_type | 1..8, selects vertex array at 0x1C..0x38 (0x5CD7F0) |
| 0x05 | 1 | u8 | flag_cpu | set to 1 for non-type-7 skinned (`0x5C0EF0`); 0 = GPU path |
| 0x06 | 1 | u8 | slot_count | `quality` slots (0xA0 each) 0x5CC7A0 |
| 0x07 | 1 | u8 | has_slots | |
| 0x09 | 1 | u8 | = 0xFF | ctor |
| 0x0A | 1 | u8 | dirty | cleared after load |
| 0x0B | 1 | u8 | gpu_ready | |
| 0x10 | 4 | i32 | vertex_count | |
| 0x14 | 4 | i32 | vertex_capacity | grow: count+0x1F+n |
| 0x1C | 4 | ptr | verts_type1 | 0x24 stride |
| 0x20/0x24/0x28/0x2C/0x30/0x34/0x38 | 4 | ptr | verts_type2..8 | strides 0x3C,0x1C,0x18,0x48,0x14,0x48,0x28 |
| 0x40 | 4 | i32 | tri_count | after strip expansion |
| 0x48 | 4 | ptr | tri_list | 6 bytes (3 x u16) per triangle |
| 0x5C | 4 | i32 | strip_index_count | |
| 0x60 | 4 | ptr | strip_indices | u16 |
| 0x64 | 4 | i32 | plane_count | |
| 0x68 | 4 | i32 | plane_capacity | grow: count+7+n |
| 0x6C | 4 | ptr | planes | SubMesh[], stride 0x54 |
| 0x70 | 4 | i32 | = 4 | set by `0x5DDD70`/`0x5DA280` |
| 0x84 | 4 | ptr | plane_matrices | 0x40 bytes per plane (init from `DAT_00A18A78`) |
| 0x88 | 4 | ptr | plane_aux | i32 per plane, init -1 |
| 0x90/0x94 | 4 | ptr | D3D vertex buffer / index buffer (0x24,0x25 as int[] in 0x5CFA10) |
| 0xE0 | 4 | f32 | lod_distance | `0x5D4720` |
| 0xE4 | 1 | u8 | lod_count | `0x5D4720` |
| 0xE8 | 4 | ptr | parent/LOD list (param_1[0x3A] in 0x5CC7A0) | inferred |

## SubMesh (0x54; init `0x5D52A0`)

| offset | size | name | meaning |
|---|---|---|---|
| 0x00 | 4 | color_a | default `DAT_00A69CD0` |
| 0x04 | 4 | color_b | = color_a |
| 0x10 | 4 | f32 specular_power | 50.0 (0x42480000) when trailer tag 2 C != -1 |
| 0x20 | 4 | flags | low nibble 2 when tag 2 E != 0 |
| 0x24 | 4 | texture0 | `0x5D5650(0,tex)`; virtual +0xC returns texture type |
| 0x34 | 4 | blend_a | 0x20000 or 8/9/10/11 combos (trailer tag 1) |
| 0x38 | 4 | blend_b | 1, 8, 9, 10, 0xD (shadow), 0x14 |
| 0x3C | 4 | = 0 (init 0xF) |
| 0x40 | 4 | alpha_mode | 1 or 2 (tag 1) |
Rest unknown: TODO(re) 0x5D5570 etc. (setters not exported).
