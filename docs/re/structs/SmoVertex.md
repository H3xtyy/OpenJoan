# SmoVertex (disk) and in-memory Model vertex

## Disk, static (32 bytes) - verified (highpolytree, alexander#v)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 12 | f32[3] | normal | unit length |
| 0x0C | 12 | f32[3] | position | model units; copied to memory +0x00 |
| 0x18 | 8 | f32[2] | uv | u, v |

## Memory vertex type 1 (stride 0x24 = D3DFVF_XYZ|NORMAL|DIFFUSE|TEX1) - `RE: 0x5CD7F0`, `0x5E0530`

| offset | size | type | name |
|---|---|---|---|
| 0x00 | 12 | f32[3] | position |
| 0x0C | 12 | f32[3] | normal |
| 0x18 | 4 | D3DCOLOR | diffuse (default `DAT_00A69CD0`) |
| 0x1C | 8 | f32[2] | u, v |

Other vertex types selected by Model+4 (byte), strides from `0x5CD7F0`: 1:0x24, 2:0x3C, 3:0x1C, 4:0x18, 5:0x48, 6:0x14, 7:0x48 (skinned: position x2 +0x00/+0x0C, normal x2 +0x18/+0x24, three u16 bone ids +0x30, two weights +0x34, diffuse +0x3C, uv +0x40), 8:0x28. TODO(re): layouts of 2,3,4,6,8 (other families; not produced by the smo readers).

## Skinned disk vertex
Variable length: uv(8) + i32 A + i32 B + 4*A skipped + B x 32-byte influence (`i32 bone; f32 weight; f32[3] pos_off; f32[3] nrm_off`). TODO(re) verification, see smo.md 4.4.
