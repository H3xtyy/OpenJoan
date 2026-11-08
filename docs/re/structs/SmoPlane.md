# SmoPlane (on disk sub-mesh record, v500)

Verified on highpolytree1_large.SMO, alexander#v.smo, aaaafx_2nd.SMO (header part). See `docs/formats/smo.md` section 4.

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | tri_count | strip has tri_count+2 indices | 0x5E0530; tree 2, alexander 317, aaaafx 543 |
| 0x04 | 4 | i32 | vertex_count | | tree 4, alexander 92, aaaafx 202 |
| 0x08 | 16 | f32[4] | color0 | first float 1.0 in samples; __ftol to byte | 0x5D5570 |
| 0x18 | 16 | f32[4] | color1 | | 0x5D5540 |
| 0x28 | 16 | f32[4] | color2 | | 0x5D55A0 |
| 0x38 | 16 | f32[4] | color3 | | 0x5D55E0 |
| 0x48 | 16 | f32[4] | plane_bounds | read, unused; equals header bounds in samples | SAMPLES 0x68 |
| 0x58 | 4 | i32 | texture_index | -1 none | alexander#v |
| 0x5C | 2*(tri_count+2) | u16[] | strip_indices | plane-local | tree 0..3 |
| next | 32*vertex_count | SmoVertex[] | vertices | static; skinned uses the variable block in smo.md 4.4 | |

Size of the fixed part: 0x5C bytes. TODO(re): colour channel order.
