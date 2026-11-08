# Animation / skeleton / skin / morph — joan.exe (reference build)

Pseudocode: `$JOAN_WORK_DIR/decomp/binfmt/anim/`. Formats: [am1](../../formats/am1.md), [am2](../../formats/am2.md), [am3](../../formats/am3.md),
[amt](../../formats/amt.md), [mda](../../formats/mda.md), [ma](../../formats/ma.md). Structs: [AnimDef](../structs/AnimDef.md), [AnimKey](../structs/AnimKey.md),
[BoneTree](../structs/BoneTree.md), [SkinVertex](../structs/SkinVertex.md).
I/O primitives (ilib filemem): 0x5893C0 `File::read_i32` (0 on failure), 0x588A80 `File::read_block(dst,n)`, 0x5892F0 `read_u16`, 0x5888C0 `skip(n)`,
0x588000 `FileMem::open(path)`, 0x5886D0 `close`; memory twin 0x587E20 / 0x5873A0 / 0x587650(read_string) / 0x587EA0 (skip); writers 0x587DA0 `write_i32`, 0x5872E0 `write_block`.

| address | proposed name | role | status |
|---|---|---|---|
| 0x4172A0 | `animres_load_tables` | reads `RESOURCE\ANIM_AM2.RES` + `ANIM_AM3.RES`, table `ANIMATE`; 0x3A-byte records `{i16 id, name[32], key[8]}` | read |
| 0x418F30 | `animscr_load` | `RESOURCE\animscr.res`; 0x21B-byte records, text-parsed | read |
| 0x417540 | `animres_acquire` | blocks until idle, ref-count++ (entry+0x128), first time creates AnimDef (0x5B2390) and loads `.AM2`+`.AM3` (0x5B2AA0) | read |
| 0x417B60 | `animres_update` | async step: state 1 -> create + start loading, state 2 -> pump 0x5B3100 until idle | read |
| 0x5B2390 | `AnimDef::AnimDef` | ctor (0x18B bytes) | read |
| 0x5B2F70 | `AnimDef::stat_files` | checks `<dir><name>.AM2` exists, stores dir/name, state 1 | verified |
| 0x5B23D0 | `AnimDef::load` | full/header load: picks `.AMT` cache if newer (state 4 path) else `.AM2` (0x5B4B90) + `.AM3` (0x5B6800) + 0x5B6A90 + optional cache write | read |
| 0x5B2AA0 | `AnimDef::load_ex` | same with explicit file handles (used by 0x417540) | read |
| 0x5B3100 | `AnimDef::async_step` | step machine on `this[1]` (0 idle/1 start/3,4,0xD,0xE); builds the three file names `.AM2/.AMT/.AM3` | read |
| 0x5B4140 | `AnimDef::is_idle` | `this[1]==0` | verified |
| 0x5B4170 | `AnimDef::unload` | frees tracks/tree/curves/labels (flag: keep header) | read |
| 0x5B4B90 | `AnimDef::read_am2_full` | header, clip table, all tracks (52-byte keys -> AnimKey), trailer | verified |
| 0x5B5830 | `AnimDef::read_am2_header` | header + clip table only | verified |
| 0x5B6DC0 | `AnimDef::load_clip` | lazy: loads keys of one clip window from the `.AM2` | verified |
| 0x5B5350 | `AnimDef::read_trailer` | section loop: plane colour/alpha curves, labels | read |
| 0x5B4FE0 | `AnimDef::write_trailer` | writer twin of 0x5B5350 | read |
| 0x5B7810 | `AnimDef::write_cache` | writes `ANIM_TMP\<name>.AMT` header + tracks + trailer | read |
| 0x5B7930 | `AnimDef::read_amt` | reads `.AMT` | read |
| 0x5B5A70 | `AnimDef::post_header` | called after header read (sort/ids of clips) — TODO(re) | inferred |
| 0x5B6800 / 0x5B69C0 / 0x5B9AF0 | `AnimDef::load_am3` / `BoneTree::create_load` / `BoneTree::parse_am3` | `.AM3` loading | verified |
| 0x5B6A90 / 0x5B6B30 | `AnimDef::make_relative` / `_node` | absolute -> parent-relative keys, post-order | read |
| 0x5B7BF0 | `AnimDef::grow_keys` | realloc key array by n x 0x20, returns old count | verified |
| 0x5B8770 | `AnimKey::set_from_matrix` | time + quaternion (0x60C840) + translation (0x60C310) | verified |
| 0x5B8840 | `AnimKey::lerp` | linear time/pos, quaternion blend 0x60C8B0 | read |
| 0x5B8910 | `AnimKey::to_matrix` | not exported | inferred |
| 0x5B83A0 | `AnimDef::reduce_keys` | exporter-side dead-key remover (thresholds 0.01 / 5e-6 / 250.1 ms) | read |
| 0x5B5E40 | `AnimDef::evaluate` | pose a model at (clip, time, loop?): per-bone key search 0x5B7C70, blend, apply; then plane colour/alpha curves | read |
| 0x5B9830 / 0x5B9990 / 0x5B9790 | `BoneTree::copy` / `init(n)` / `BoneNode::reset` | | verified |
| 0x5B9FE0 | `BoneTree::parent` | node+8 | verified |
| 0x5BA570 / 0x5BA660 / 0x5BA760 | name registry init / destroy / add name | | read |
| 0x5BA1D0 / 0x5BA300 | `BoneTree::compatible` / `build_remap` | by name id; -999 = no match | read |
| 0x5B0620 | `ActorDef::bind_am3` | opens `<dir><name>.AM3` (model skeleton), builds remap | read |
| 0x5B0000, 0x5AF5D0, 0x5AFDD0, 0x5AF3D0 | `ActorDef::load_model(skinned)` | `.AM1` if skinned else `.SMO`; error text `Error loading 3D Model : %1s` | read |
| 0x5C0EF0 / 0x5C2370 | `Model::load` / `Model::open_lods` | body parser / LOD+shadow file opener (belongs to smo family) | read |
| 0x5DA280 | `Plane::read_dispatch` | version 0/50/100/500 dispatch | verified |
| 0x5DC420 | `Plane::read_v500` | planes, strips, skin block, texture table, extensions | read |
| 0x5F46A0 / 0x5F47C0 | `SkinSet::clear` / `alloc` | arrays of 0x84-byte SkinVertex | verified |
| 0x5F4970 / 0x5F49C0 | `SkinVertex::add_bone` / `normalize` | thunks to v3d exports | inferred |
| 0x5F6C00 | `Device::init` | checks `MIXEDVP.SYS`, `SOFTVP.SYS`, `SOFTBONE.SYS` in the game dir (marker files force software vertex processing / software bone skinning) | read |
| 0x5C6820 / 0x5C9B10 | `Morph::load` / `Morph::load_append` | `.MDA` readers (see mda.md) | verified (part section) |
| 0x5C7370 / 0x5C7B80 | `Morph::copy` / `Morph::free` | | read |
| 0x5CA5D0 | `Morph::apply` | blend targets into vertex arrays — not yet read in detail | inferred |
| 0x5C90A0 | `Morph::find_target` | by name (strcmp) | inferred |

## Constants
- AM3 name field 50 bytes; clip name 50 bytes; label name 50 bytes; set name < 32, dir < 256.
- Key dedup thresholds (0x5B83A0): 0.01, 5e-6, 250.1 ms. Time unit ms, 30 fps grid.
- `0xFFFFFC19` (-999) = "no bone / no mapping" sentinel.
- Skin: max 4 influences, hw vertex uses 2; 3 constant registers per bone (`bone*3`).

## Cross-module requests
- `TODO(docs)`: reconcile the plane-record header table in am1.md with smo.md (same code 0x5DC420); `Plane::read_v500` name.
- Ghidra: name `0x5B3100 AnimDef::async_step`, `0x5B6DC0 AnimDef::load_clip`, `0x5B9AF0 BoneTree::parse_am3`, create structs AnimDef/BoneTree/SkinVertex per structs/.
- `TODO(re)`: `ANIMATE` table format of `ANIM_AM2.RES`/`ANIM_AM3.RES` (58-byte records) and `animscr.res`.

## Open questions
- 0x5B5DD0 (clip name -> index), 0x5B7C70 (key search), 0x5B9DF0 (set parent), 0x5BA040/0x5BA0A0, 0x5C5400 (model-side AM3 load), 0x5F4970/0x5F49C0 internals: not exported.
- `.MA` consumer; `zCripple` (0x9DD6D2) xref; clip `flags` dword; trailer id values other than 1; label record semantics.
- Verify with byte dumps: Alencon.am2 @0x142A len 256 and last 256 bytes (track block + trailer); alencon_p.am1 @0xEBA len 256; unit/alencon_p.am3 first 16 bytes; Grass_C.MA whole file; bigred.AMT tail 128.
