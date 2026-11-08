# GameConfig — the `CONFIG.DAT` image (0xB05 bytes, packed, no alignment) — joan.exe

`CONFIG.DAT` in the game directory is a **raw memory dump of this object**: `config_load` (0x45AAF0) requires `GetFileSize == 0xB05` and reads 0xB05 bytes
straight over the object (0x45ADC0). There is no header, version or checksum. A CONFIG.DAT whose size differs is ignored and defaults are used
(0x459E30). Fields are packed (unaligned `u32` at odd offsets). The player profile name `Player` is stored at `+0x17` (the file in the install contains it).

Object address: **`0xA5801E`** (inferred: `DAT_00A58055 == 2` is tested for the shadow-depth surface in WinMain, and the default for `+0x37` is 2; `DAT_00A58057/58058`
= `+0x39/+0x3A` feed `dyngrass`; `DAT_00A58023/27` = `+0x05/+0x09` are read by the options menu). Role names are inferred from consumers and defaults; defaults are
exact. `CPU_MHz` = `tsc_hz / 1e6` (`DAT_00A485EC/F0`, measured by 0x45C8C0).

| offset | size | type | name | default | role / evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | i32 | `lang_code` | table value (i16 table 0x9FB1CA indexed by language) | set by `set_language` 0x45ACB0 (default call with 1); likely code page / LCID |
| 0x04 | 1 | u8 | `lang_flag` | byte table 0x9FB1DB[idx] | 0x45ACB0 |
| 0x0A | 1 | u8 | `language` | language index, default 1 | 0x45ACB0 |
| 0x05 | 1 | u8 | `opt05` | 1 | 0x459EC0; options menu value (0/1/3 compared in 0x7435E0) |
| 0x06 | 2 | u16 | `opt06` | 0x2D (45) | 0x459EC0 |
| 0x08 | 1 | u8 | `opt08` | 1 | 0x459EC0 |
| 0x09 | 1 | u8 | `opt09` | 2 | 0x459EC0; options menu passes it to 0x432250 |
| 0x0F | 1 | u8 | `opt0f` | 0 | 0x459EC0 |
| 0x10 | 4 | i32 | `opt10` | 20 | 0x459EC0 |
| 0x14..0x16 | 3 | u8[3] | `reserved14` | 0 | cleared after every successful load (0x45AAF0) and by defaults (0x459F30) |
| 0x17 | up to 0x15 | char[] | `player_name` | `"Player"` | 0x459FB0 (strcpy) |
| 0x2C | 1 | u8 | `b2c` | 0 | 0x459FB0 |
| 0x2D | 1 | u8 | `b2d` | 1 | 0x459FB0 |
| 0x2E | 4 | i32 | `profile_index` | 1 | 0x459FB0; used as `i16` by 0x4CA290 to select the current profile (1-based) |
| 0x32 | 1 | u8 | `b32` | 0 | 0x459FB0; `DAT_00A58050` (float far-clip display) lives here (inferred) |
| 0x33 | 1 | u8 | `b33` | 1 | 0x45A010 |
| 0x34 | 1 | u8 | `b34` | 5 | 0x45A010 |
| 0x35 | 2 | u16 | `w35` | 3 | 0x45A010 |
| 0x37 | 1 | u8 | `shadow_quality` | 2 | `==2` creates the 1024x1024 depth surface (0x420EA0); also gates shadow code in scene/lights (`DAT_00A58055 == 2`) |
| 0x38 | 1 | u8 | `b38` | 0 | |
| 0x39 | 1 | u8 | `grass_enable` | 1 | passed to dyngrass load/unload (`DAT_00A58057`) |
| 0x3A | 2 | u16 | `grass_density` | 0x4D (77) | passed with the above (`DAT_00A58058`) |
| 0x3C | 2 | u16 | `w3c` | 0x27 (39) | volume-like (0..100) |
| 0x3E | 2 | u16 | `w3e` | 100 | volume-like |
| 0x40 | 1 | u8 | `b40` | 1 | |
| 0x41 | 2 | u16 | `w41` | 100 | volume-like |
| 0x43..0x48 | 6 | u8[6] | `flags43` | 1 | |
| 0x49 | 1 | u8 | `b49` | 0 | |
| 0x4A..0x4C | 3 | u8[3] | `flags4a` | 1 | |
| 0x4D | 1 | u8 | `b4d` | 0 | |
| 0x4E | 1 | u8 | `b4e` | 1 | |
| 0x4F | 4 | i32 | `i4f` | 20 | |
| 0x53 | 4 | i32 | `i53` | 5, then **0** after 0x45A8B0/0x45A950 | 0x45A010 sets 5 first, calls two un-exported helpers, then forces 0 |
| 0x57 | 4 | i32 | `i57` | 100 | |
| 0x5B | 4 | i32 | `i5b` | 100 | |
| 0x5F | 4 | i32 | `i5f` | 90 | |
| 0x63 | 4 | i32 | `i63` | 100 | |
| 0x67 | 4 | i32 | `i67` | 70 | |
| 0x6B | 4 | i32 | `i6b` | 3 | |
| 0x6F | 4 | i32 | `i6f` | 0 | |
| 0x73 | 1 | u8 | `b73` | 0 | |
| 0x76 | 4 | f32 | `fov_deg?` | 90.0 | 0x42B40000 |
| 0x7A | 4 | i32 | `i7a` | 0 | |
| 0x7E | 4 | i32 | `fullscreen?` | 0 | |
| 0x82 | 4 | i32 | `res_w` | 1024 (0x400) | |
| 0x86 | 4 | i32 | `res_h` | 768 (0x300) | |
| 0x8A | 4 | i32 | `bpp` | 32 (0x20) | |
| 0x8E | 4 | i32 | `refresh_hz` | 60 (0x3C) | |
| 0x92 | 1 | u8 | `b92` | 1 | |
| 0x93 | 1 | u8 | `b93` | 2 | |
| 0x94 | 1 | u8 | `b94` | 3 | |
| 0x95 | 1 | u8 | `b95` | 5 | |
| 0x96 | 1 | u8 | `b96` | 1 | |
| 0x97 | 1 | u8 | `b97` | 1 | |
| 0x98 | 4 | i32 | `i98` | 0x2001 | |
| 0x9C | 1 | u8 | `detail_level` | CPU_MHz < 1001 -> 1; < 1501 -> 2; else 3 | clamped to 1 by 0x45B240 when `DAT_00A5272B < 4` |
| 0x9D | 1 | u8 | `effects_tier` | CPU_MHz < 1001 -> 0, else 1 | 0x45B240: tier 2 needs 64x64 texture+depth probes and not "geforce3"; tier 1 needs `DAT_00A52633`, else 0 |
| 0x9E | 1 | u8 | `rt_effects` | 1 | 0x45B240 forces 0 unless `DAT_00A52500 & 0x200000` and a 256x256 render-target probe (format 0x3C) succeeds; also 0 if tier 2 on GeForce3 |
| 0x9F | 1 | u8 | `b9f` | CPU_MHz < 701 -> 0 else 1 | |
| 0xA0 | 4 | i32 | `view_dist?` | CPU_MHz<701: 500; 701..1999: `((MHz-700)*11/1300 + 5)*100` (integer division); >=2000: 1600 | |
| 0xA4 | 4 | i32 | `ia4` | CPU_MHz<701: 30; 701..1999: `((MHz-700)*5/1300 + 3)*10`; >=2000: 80 | |
| 0xA8 | 4 | f32 | `fa8` | -0.2 (0xBE4CCCCD) | |
| 0xAC | 4 | f32 | `fac` | 1.1 (0x3F8CCCCD) | |
| 0xB0 | 4 | f32 | `fb0` | 1.0 | |
| 0xB4 | 1 | u8 | `bb4` | 0 | 0x459EC0 |
| 0xB5 | 0xA50 (2640) | u8[] | `block_b5` | reset by 0x45ACB0(1) (not exported) | preserved/restored as one blob by 0x45ADC0 — probably the key-binding table (`remapkey`) `TODO(re)` |

Preserved-on-load list (restored when `keep_prefs != 0`, 0x45ADC0): `+0x37, +0x39, +0x3A(w), +0x3C(w), +0x3E(w), +0x40, +0x41(w), +0x44, +0x6B, +0x6F, +0x73, +0x76, +0x7A,
+0x7E, +0x82, +0x86, +0x8A, +0x8E, +0x92..+0x98, +0x9C..+0x9F, +0xA0..+0xB0, +0xB4` and the block `+0xB5..+0xB04`.

Sizes: total **0xB05** (2821); `sizeof` verified by the file-size test.
Effective defaults after the helpers 0x45A8B0 / 0x45A950 run (they are called from `config_defaults` between the writes shown): `+0x45..+0x48 = 1`, `+0x49 = 0`, `+0x4A..+0x4E = 1` (so `+0x4D = 1`, overriding the 0 above), `+0x53 = 0` (final).
Open: the writer that saves CONFIG.DAT was not found in the 1145-function round-8 export (WinMain is the only referrer of the literal; 0x45ADC0 has no other caller there); the language tables at 0x9FB1CA/0x9FB1DB.
