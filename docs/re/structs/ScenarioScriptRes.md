# ScenarioScriptRes = TextureScript (t_scrres.cpp record, sizeof = 0xD51 = 3409) — joan.exe

**Naming note.** The brief called this struct "ScenarioScriptRes" because the source file is `t_scrres.cpp`; the code behind 0x4DEB70 is the **texture-script
bank `Resource/texteff.res`** (51 records: animated/scrolling/bump/cube-map material definitions per model), not the scenario `.sce` scripts
(see [functions/script_res.md](../functions/script_res.md) §0). Proposed type name for Ghidra: `TextureScript`; page file kept under the requested name.

Table header object: `+0 TextureScript* records`, `+4 i16 count`, `+6 u8 initialised`. Records are 1-based, `base + (i-1)*0xD51` (accessor 0x4B27B0), zero-filled
first, then parsed by `texscr_parse_record` 0x4DED90. The text grammar is purely **positional** (see §2); keys are cosmetic.

## 1. Layout

M = material index 0..5, L = layer index 0..3 (array extents prove 6 materials x 4 layers; `NumMaterial` > 6 would overlap the next array).
"per-layer" arrays are indexed `base + M*0x10 + L*4` (dwords) unless stated.

| offset | size | type | name | text key (file order) | notes / evidence |
|---|---|---|---|---|---|
| 0x00 | 2 | u16 | recno | – | = ResLX index |
| 0x02 | 0x21 | char[33] | model_name | `ModelName` | `strcpy`; matched case-insensitively against the model table at the end of the parse |
| 0x23 | 2 | i16 | num_material | `NumMaterial` | atoi; loop bound |
| 0x25 | 6 x 2 | i16[6] | material_id | `MaterialID` | atoi (the id stored is the *file's* id, e.g. 3 in `S_TURNR90`) |
| 0x31 | 6 x 2 | i16[6] | num_layers | `NumLayers` | loop bound for the layer block (<= 4) |
| 0x3D | 6 x 2 | i16[6] | alpha_value | `AlphaValue` | percent (100 = opaque, 80 in water) |
| 0x49 | 6 x 4 | f32[6] | alpha_per_second | `AlphaValuePerSecond` | atof; the array ends exactly at 0x61 |
| 0x61 | 0x60 | u32[6][4] | alpha_arg1 | `AlphaArg1` | `texscr_parse_d3d_enum` (D3DTA value) |
| 0xC1 | 0x60 | u32[6][4] | alpha_arg2 | `AlphaArg2` | D3DTA |
| 0x121 | 0x60 | u32[6][4] | alpha_op | `AlphaOp` | D3DTOP |
| 0x181 | 0x60 | u32[6][4] | color_arg1 | `ColorArg1` | D3DTA |
| 0x1E1 | 0x60 | u32[6][4] | color_arg2 | `ColorArg2` | D3DTA |
| 0x241 | 0x60 | u32[6][4] | color_op | `ColorOp` | D3DTOP |
| 0x2A1 | 0x60 | u32[6][4] | layer_num_frames | `LayerNumFrames` | **passed through the enum mapper**, so any numeric text yields 0; effectively unused (same token position as the 7th enum read) |
| 0x301 | 0x60 | f32[6][4] | seconds_per_frame | `LayerSecondsPerFrame` | atof; 0 = static, 100 / 2000 seen (ms per frame despite the name, TODO(re) unit unconfirmed) |
| 0x361 | 0x60 | i32[6][4] | num_texture | `LayerNumTexture` | atoi; number of numbered textures in the animation (0, 2, 65 ...) |
| 0x3C1 | 6 x 4 x 0x21 | char[6][4][33] | texture_name | `LayerTextureName` | stride `M*0x84 + L*0x21`, ends at 0x6D9 |
| 0x6D9 | 6 x 4 | u8[6][4] | tex_coord_mode | `TextureCoordinate` (last field of a layer) | `NORMAL` -> 0, `OBJECT` -> 1 (case-insensitive), any other text leaves the byte 0; index `M*4 + L` |
| 0x6F1 | 0x60 | f32[6][4] | u_shift_per_second | `UShiftPerSecond` | atof |
| 0x751 | 0x60 | i32[6][4] | u_address | `UWrap` | `"true"` (case-sensitive `strcmp`) -> **1** (D3DTADDRESS_WRAP) else **3** (CLAMP) |
| 0x7B1 | 0x60 | f32[6][4] | v_shift_per_second | `VShiftPerSecond` | atof |
| 0x811 | 0x60 | i32[6][4] | v_address | `VWrap` | as `u_address` |
| 0x871 | 0x60 | i32[6][4] | is_bump | `IsBump` | `"true"` -> 1 else 0 |
| 0x8D1 | 0x60 | i32[6][4] | is_cubemap | `IsCubeMap` | 1/0 |
| 0x931 | 0x60 | i32[6][4] | is_reflective | `IsReflective` | 1/0 |
| 0x991 | 0x60 | f32[6][4] | bump_mat00 | `BumpEnvMat00` | present in the file **only when `IsBump` is true**, directly after the `IsBump` line |
| 0x9F1 | 0x60 | f32[6][4] | bump_mat01 | `BumpEnvMat01` | " |
| 0xA51 | 0x60 | f32[6][4] | bump_mat10 | `BumpEnvMat10` | " |
| 0xAB1 | 0x60 | f32[6][4] | bump_mat11 | `BumpEnvMat11` | " |
| 0xB11 | 0x60 | f32[6][4] | bump_lscale | `BumpEnvLScale` | " |
| 0xB71 | 0x60 | f32[6][4] | bump_loffset | `BumpEnvLOffset` | " |
| 0xBD1 | 0x60 | f32[6][4] | initial_u_shift | `InitialUShift` | atof |
| 0xC31 | 0x60 | f32[6][4] | initial_v_shift | `InitialVShift` | |
| 0xC91 | 0x60 | f32[6][4] | initial_u_scale | `InitialUScale` | |
| 0xCF1 | 0x60 | f32[6][4] | initial_v_scale | `InitialVScale` | ends at 0xD51 = sizeof |

Link to the model table: after parsing, the first model entry (scanning from the last, `DAT_00A50568` down to 1, accessor 0x41A180, stride 0x2B, name at entry+10)
whose name equals `model_name` (case-insensitive) gets `*(i32*)(entry+0x16) = recno`. No match -> nothing stored, no error.

## 2. Text grammar (verified from 0x4DED90 and `texteff.res`)

Tokenised by the `FileTxt` reader ([script_res.md](../functions/script_res.md) §1). For every field the reader consumes **two tokens: a key and a value**; the key
text is ignored. Order is fixed:

```
Version : 1.00                  (pair 1, value discarded)
NumMaterial : N
ModelName : <name>
repeat N times:                 (material M)
    MaterialID : id
    NumLayers : n
    AlphaValue : percent
    AlphaValuePerSecond : f
    repeat n times:             (layer L, 22 pairs; bump adds 6)
        AlphaArg1 AlphaArg2 AlphaOp ColorArg1 ColorArg2 ColorOp   (enum words)
        LayerNumFrames  LayerSecondsPerFrame  LayerNumTexture  LayerTextureName
        UShiftPerSecond UWrap VShiftPerSecond VWrap
        IsBump [BumpEnvMat00 BumpEnvMat01 BumpEnvMat10 BumpEnvMat11 BumpEnvLScale BumpEnvLOffset]
        IsCubeMap IsReflective
        InitialUShift InitialVShift InitialUScale InitialVScale
        TextureCoordinate
```

Blank lines are free. A record without `NumMaterial` blocks as declared is not detected (the reader would hit `0x1A` and dereference NULL).
Example (`BIGRED`, record 1, `NumMaterial: 4`, `ModelName: Bigred`): materials 1..4 each have 1 layer with `modulate`/`texture`/`diffuse`, texture `bigred`,
`UShiftPerSecond` 0 / 0.3 / -0.3 / -0.3. `BumpEnvWater` (record at file line 175): 1 material, 2 layers, layer 0 `ColorOp: bumpenvmap`, `IsBump: true` plus the six
`BumpEnv*` lines, layer 1 `IsCubeMap: true`, texture `waters`.

## 3. Index (51 records, file order)

`BIGRED, F_MODEL, N_WATER, BE_WATER, S_TURNR90, BE_WATERSEA, S_TURNRMID, S_TURNREND, S_TURNRBEGIN, S_TURNLMID, S_TURNLEND, S_TURNLBEGIN, S_TURNL90, S_STRAIGHT_SHORT,
S_STRAIGHT, L_TURN*, L_STRAIGHT, BRIDGE, BIGGREEN, MEDIUMGREEN, SMALLGREEN, BIGBLUE, BIGYELLOW, SMALLBLUE, SMALLYELLOW, MEDIUMBLUE, MEDIUMYELLOW, BE_WATERRIVER, MEDIUMRED,
SMALLRED, CLOUD_LAYER1/2, BE_WATERRIVERR, BE_WATERCAUST, WATERFALL01, WATERFALL_MOD, RIVER_SECTOR*, ICE, SP_CLIFF`. The ResLX entry name and the record's `model_name` are different
strings (e.g. entry `BE_WATER` holds `ModelName: BumpEnvWater`); only `model_name` is used for the model link.

Status: layout **verified** (stores at every offset read in 0x4DED90; extents tile to 0xD51); semantics of layer fields beyond the D3D names are **inferred**.
