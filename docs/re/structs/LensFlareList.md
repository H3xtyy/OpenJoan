# LensFlareList (`uti3d/lflare.cpp`) — lens-flare element list

Reference build: joan.exe. Container: `0x6626A0` clear, `0x662710` add element, `0x662640` set-up, `0x6628A0` draw. The elements are created once per world
by `lightsys_init` (0x4A5020) and drawn from the sun branch of 0x4A7BA0; see engine/lights.md section 9 for the behaviour.
The list is a member of the [LightSys](LightSys.md) singleton (released from 0x4A5970); its offset inside it is not visible in the export.

## List header (this)
| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | Element* | items | tracked array, `count * 0x19B` bytes (realloc'd by `0x58EB30` on every append) |
| 0x04 | 2 | i16 | count | number of elements |
| 0x06 | 2 | i16 | param_a | first argument of `0x662640` (0x4A5020 passes 100) |
| 0x08 | 4 | f32 | param_b | second argument (0.0) |
| 0x0C | 4 | f32 | param_c | third argument (1.0), probably master intensity |
| 0x10 | 2 | i16 | centre_x | screen centre X (`width / 2`), overwritten with the viewport offset by the draw call |
| 0x12 | 2 | i16 | centre_y | screen centre Y (`height / 2`) |

`0x662640` also takes a reference on the shared texture set `"LFlare"` (`0x662D20`, refcount `DAT_00A6A270`, handle `DAT_00A6A26C`).

## Element (stride 0x19B = 411 bytes, packed)
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | u8[4] | colour | RGBA from `0x42A260(r,g,b,a)`; all five shipped elements use (170,170,170,85) | 0x4A5020 |
| 0x04 | 2 | i16 | size | sprite size in pixels (square); 220, 198, 352, 462, 330 | 0x4A5020, 0x6628A0 |
| 0x06 | 4 | f32 | position | position along the screen-centre -> sun axis: 1.0 = on the sun, 0 = centre, negative = opposite side | 0x4A5020 (values), inferred |
| 0x0A | 401 | String | name | sprite name inside the `LFlare` texture set: `sun01a`, `len03a`, `len02a`, `len03a`, `len01a` | 0x4A5020 |

## Shared resource
`0x662DB0` (`lflare_release_shared`): `DAT_00A6A270` is a reference count and `DAT_00A6A26C` a texture handle; when the count reaches 0 the handle is
freed with `0x60A550` (texture release). `0x6626A0` calls it, then frees `items` and zeroes `count`.
