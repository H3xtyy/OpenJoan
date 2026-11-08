# `SceneCamera` — renderer-side camera (class around 0x614000-0x616F00)

Source: `FUN_00614040` (apply CamParams), `FUN_006140A0` (get), `FUN_006141A0` (rebuild matrices), `FUN_00616460` (frustum),
`FUN_00616D50` (reset), `FUN_00616DD0` (pan). The gameplay `CamMan.cur` block is copied here every frame by 0x44C800
(see [CamMan](CamMan.md), engine/camera.md). Byte offsets are from `this`; matrices are D3D8 row-major
`D3DMATRIX` (64 bytes). Status: verified = used consistently in >= 2 of these functions.

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | `SceneNode*` | node | camera node; its local matrix (node+0) is rebuilt from yaw/pitch/roll/dist/focus, world matrix at node+0x58 (via 0x60EE60, parent at node+0x40, dirty flag node+0x54) | read |
| 0x10 | 4 | int | vp_x | viewport X (copied into the D3DVIEWPORT8 at 0x2AE) | read |
| 0x14 | 4 | int | vp_y | viewport Y | read |
| 0x20 | 4 | int | vp_w | viewport width in pixels | verified |
| 0x24 | 4 | int | vp_h | viewport height; **aspect = vp_w / vp_h** (float division) | verified |
| 0x44 | 4 | float | near | near plane (frustum corners 0x616460, perspective) | verified |
| 0x48 | 4 | float | far_active | copy of 0x4C (perspective) or 0x50 (ortho), chosen in 0x6141A0 | verified |
| 0x4C | 4 | float | far_persp | | read |
| 0x50 | 4 | float | far_ortho | | read |
| 0x78 | 0x28 | CamParams | prev | previous block (0x614040 copies cur -> prev) | verified |
| 0xA0 | 0x28 | CamParams | cur | current block: +0xA0 proj_type, +0xA4..AC focus, +0xB0 yaw, +0xB4 pitch, +0xB8 roll, +0xBC dist, +0xC0 fov_param, +0xC4 ortho_scale | verified |
| 0xEA | 1 | u8 | locked | non-zero => 0x6141A0 does nothing | read |
| 0xEB | 1 | u8 | dirty | set by 0x614040/0x614170; cleared after rebuild | verified |
| 0xEE | 0x40 | D3DMATRIX | view | inverse of the camera node's world matrix | verified |
| 0x12E | 0x40 | D3DMATRIX | proj | projection (see engine page); also copied to 0x26E | verified |
| 0x16E | 0x40 | D3DMATRIX | view_proj | `view * proj` | verified |
| 0x1AE | 0x40 | D3DMATRIX | dev_view_proj | `dev_view(0x1EE) * dev_proj(0x26E)` | verified |
| 0x1EE | 0x40 | D3DMATRIX | **dev_view** | the matrix passed to `IDirect3DDevice8::SetTransform(D3DTS_VIEW=2)` (vtable slot 0x94) by 0x614760 and others; = `inverse(0x60C000(world))` via 0x766B4B (D3DXMatrixInverse) | verified |
| 0x22E | 0x40 | D3DMATRIX | dev_world | `0x60C000(world)` (the camera world matrix as seen by the device) | read |
| 0x26E | 0x40 | D3DMATRIX | **dev_proj** | copy of `proj`; passed to `SetTransform(D3DTS_PROJECTION=3)` | verified |
| 0x2AE | 0x18 | D3DVIEWPORT8 | viewport | X, Y, Width, Height, MinZ = 0.0, MaxZ = 1.0 | verified |
| 0x2C6 | 8 x 12 | D3DVECTOR[8] | frustum_corners | world-space frustum corners (near 4, far 4) from 0x616460 | verified |
| 0x326 | 6 x 16 | plane[6] | frustum_planes | built with 0x60D920 from the corner triples | read |

### Defaults (constructor 0x613730) and setters
sizeof >= 0x3B6. Constructor: node = new 0xA0 bytes; `cur` = `camparams_reset` (0x616D50); `near = 1.0`; `far_persp = 8193.0` (0x46000400);
`far_ortho = 16385.0` (0x46800200); far_active = far_persp for proj_type 0; flags +0xCE/+0xCF = 1; creates a default white D3DMATERIAL8
(`SetMaterial`, slot 0xA8, all 1.0); fields 0x58..0x70 (renderer state: 0.0 ..., 10000.0, 0.005, ...) belong to fog/clear settings (not camera). The scene
loader 0x5E45A0 then overrides from the stage/scene record: viewport corners via 0x613BF0 (`left, top, right, bottom` -> `vp_x, vp_y`, `vp_w = right-left+1`,
`vp_h = bottom-top+1`), near/far via 0x613CA0 (each only if > 0.0), +0xCE/CF/D0 via 0x613D30, +4 via 0x613DB0, six clip floats (+0x2C..+0x40) via 0x614110.
Getters that first call the lazy rebuild: 0x616EC0 -> view(0xEE), 0x616EF0 -> proj(0x12E), 0x616F20 -> **dev_view(0x1EE)**, 0x616F50 -> 0x22E, 0x616F80 -> **dev_proj(0x26E)**,
0x616FB0 -> 0x1AE, 0x613DE0 -> node. Consumers of SetTransform: 0x614760 (main scene render: VIEW/PROJECTION/`SetViewport` slot 0xA0 with the struct at 0x2AE),
0x5D6050 and 0x5D8650 (secondary passes, through the getters).

After the frustum step `view_proj` (0x16E) is multiplied **in place** by a screen matrix built from the 64-byte constant at `DAT_00A18A78`
(identity, inferred) with `[0] = w/2`, `[5] = h/2`, `[10] = maxz - minz`, `[12] = w/2`, `[13] = h/2`, `[14] = minz`; so 0x16E maps world to pixel
coordinates, while the D3D device itself gets `view` and `proj` separately (SetTransform not visible in this export).
