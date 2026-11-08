# Bitmap, Bitmap32, Image2D, BmpResEntry (ilib\bitmap*.cpp, en2d\image2d.cpp, en2d\bmpres.cpp)

## Bitmap (8 bpp) — `ilib\bitmap.cpp`, creator 0x57F080

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0 | 2 | u16 | `width` | | 0x57EF20 (stores param 1) |
| 2 | 2 | u16 | `height` | | 0x57EF20 |
| 4 | w*h | u8[] | `pixels` | palette indices (`pal_std.res` palette; I), row-major | allocation size `w*h+4` (0x57F050) |

## Bitmap32 (32 bpp) — `ilib\bitmapd.cpp`, creator 0x57F230

Same header (u16 w, u16 h) then `w*h` x 4 bytes; allocation `w*h*4+4` (0x57F200). Byte order of the 32-bit pixel: TODO(re).

## Image2D (en2d\image2d.cpp)

Now fully documented in [Image2D.md](Image2D.md) (sizeof 0xB7). Corrections to the earlier partial table: `+0x94` is the **text**
pointer (kind 1), not an image name; the image name is the 9-byte string at `+0x76`; `+0x72` is the `BmpRes*`.

## BmpRes (en2d\bmpres.cpp) — table header and entries

Header (`this` of 0x620BC0): `+0` u8 loaded; `+1` i32 `count` (= row count of the DB table); `+5` BmpResEntry* `entries` (`count * 0x38`).

BmpResEntry (0x38 bytes), filled by 0x620BC0 from one fixed-width ASCII row of the `INTERFAC` resource DB
(source offsets are within the row; `trim` = copy and strip trailing spaces; `num4` = copy 4 chars then `atoi`):

| offset | size | type | source | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 9 | char[9] | row+0x00, 8 chars trim | entry name | 0x590210 |
| 0x09 | 0x21 | char[0x21] | row+0x08, 0x20 chars trim | image/file name (texture) | 0x590210 |
| 0x2A | 2 | i16 | row+0x28 num4 | `src_x0`: left texel of the sprite in the texture named at +0x09; -1 = 0 | verified 0x633500 |
| 0x2C | 2 | i16 | row+0x2C num4 | `src_y0`: top texel; -1 = 0 | verified 0x633500 |
| 0x2E | 2 | i16 | row+0x30 num4 | `src_x1`: right texel (inclusive); -1 = texture width-1 | verified 0x633500 |
| 0x30 | 2 | i16 | row+0x34 num4 | `src_y1`: bottom texel (inclusive); -1 = texture height-1 | verified 0x633500 |
| 0x32 | 2 | i16 | row+0x38 num1 | `rot`: 0..3 quarter-turn applied to the UVs (added to the draw-time rotation); odd values swap width/height of the default dest size | verified 0x633500, 0x632A70 |
| 0x34 | 2 | i16 | row+0x39 num4 | `w`: default on-screen width in pixels (used by `Button2D`/`Image2D` init: `x1 = x - 1 + w`) | verified 0x6211D0, 0x629660 |
| 0x36 | 2 | i16 | row+0x3D num4 | `h`: default on-screen height | verified 0x6211D0, 0x629660 |

The texture named at +0x09 is looked up with 0x60A4B0 (already-loaded texture by name) and drawn through the 2D quad primitive; see
docs/engine/ui2d.md. Entries are found by name with 0x620B40 (linear search, 0x38 stride).

Row stride = DB `+0x118` (header `+0x116` = u16 header length); numbering of rows is 1-based (0x580C70).
