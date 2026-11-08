# EngineLight — en3d light object wrapping a D3DLIGHT8

Engine-level light (one per sun/ambient/point/spot), built by `0x5C5C30` and registered on the D3D8 device by `0x5C6110`.
Source file not named by an assert (`FUN_*` only); the code sits in the `0x5C5xxx..0x5C6xxx` block next to the scene code. Reference build: joan.exe.
Layout recovered from 0x5C5C30 (build), 0x5C6110 / 0x5C6250 (register / unregister), 0x5C65E0 (ambient sum), 0x5C6070 (validity), 0x5C63C0 (copy), 0x5C6560 (equality).
The scene family may describe the same object; if so, keep the offsets here in sync. Related: [LightStage](LightStage.md) (`.lgt` data), lights.md.

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | type | 0 = ambient, 1 = directional, 2 = spot, 3 = point (maps to D3DLIGHT type 3/3/2/1) | verified (0x5C5C30 switch, 0x5C6070 switch, 0x5C6110) |
| 0x04 | 4 | D3DMATRIX* | parent_matrix | optional 0x40-byte matrix (parent/attach); when set it is overwritten with the light's local matrix and light position/direction are taken from the parent's `0x60EF80` matrix | read |
| 0x0C | 12 | f32[3] | position | local translation | read |
| 0x18 | 4 | f32 | yaw | first angle passed to `0x60BD00` (orientation builder) | inferred |
| 0x1C | 4 | f32 | pitch | second angle | inferred |
| 0x20 | 4 | f32 | extra | fourth parameter of `0x60BD00` (offset along the facing axis) | inferred |
| 0x24 | 4 | f32 | theta | spot inner cone, copied to D3DLIGHT `Theta` | verified |
| 0x28 | 4 | f32 | phi | spot outer cone, copied to `Phi` | verified |
| 0x2C | 4 | f32 | range | copied to `Range` | verified |
| 0x30 | 4 | f32 | intensity | multiplier applied to the colour bytes (`0x5E9130`: `c = byte * intensity / 255`, alpha = a/255; 255 reciprocal at 0x9D97D8) | verified |
| 0x34 | 4 | u8[4] | color_rgba | R,G,B,A bytes (0x5C65E0 reads +0x34/+0x35/+0x36 as R,G,B) | verified |
| 0x48 | 4 | f32 | atten0 | `Attenuation0` | read |
| 0x4C | 4 | f32 | atten1 | | read |
| 0x50 | 4 | f32 | atten2 | | read |
| 0x58 | 1 | u8 | enabled | checked by 0x5C6070 | read |
| 0x59 | 1 | u8 | disabled_flag | `0x5C6790` returns it; light is invalid when set | read |
| 0x5A | 4 | i32 | dirty | non-zero -> rebuild D3DLIGHT8 on next `0x5C5C30`; cleared after build | verified |
| 0x5E | 0x68 | D3DLIGHT8 | d3d | built light: Type @+0x5E, Diffuse @+0x62, Specular @+0x72, Ambient @+0x82, Position @+0x92, Direction @+0x9E, Range @+0xAA, Falloff @+0xAE (= 1.0), Attenuation0-2 @+0xB2.., Theta @+0xBE, Phi @+0xC2 | verified |
| 0xC6 | 4 | i32 | d3d_index | device light index, -1 when not enabled | verified |

## Build rules (0x5C5C30)
- Type 0 (ambient): D3DLIGHT type = 3 (directional) with **only Ambient** = colour x intensity (Diffuse and Specular zero).
- Type 1 (directional): type 3, Diffuse and Specular = colour x intensity, Ambient zero.
- Type 2 (spot): D3DLIGHT type 2, same colours, plus Theta / Phi / Range / attenuation.
- Type 3 (point): D3DLIGHT type 1, same colours, Range / attenuation.
- Direction = second row (`+0x20` of the orientation matrix via `0x60C220`) normalised-as-stored; position = translation row (`0x60C310`).

## Register / unregister (0x5C6110 / 0x5C6250)
- Ambient lights (`type == 0`, i.e. `*this == 0`) are **not** sent to `SetLight`. They are pushed on a global array (`0xA6785C`, count `0xA6789C`);
  after every change the device ambient is set to `SetRenderState(D3DRS_AMBIENT = 0x8B, 0x5C65E0)`, which sums the R,G,B bytes of all registered
  ambient lights (clamped to 255, alpha 255; note the sum ignores `intensity`).
- Other lights: `index = 0x5F8030` (first device light slot for which `GetLightEnable` (vtable slot 47) is FALSE or fails; grows the table by one otherwise),
  `SetLight(index, &d3d)` (slot 44) then `LightEnable(index, TRUE)` (slot 46). On failure the index is released (`0x5F80F0`, empty) and `d3d_index` stays -1.
  Unregister disables the slot.
- Array capacity for ambient lights and the device's hardware light cap (`D3DCAPS8.MaxActiveLights`) are not checked here: `TODO(re)`.
