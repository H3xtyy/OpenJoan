# `.ma` — morph-animation curves (15 files, 496–938 bytes)

`.ma` is the **morph-animation** format: per-morph-target weight curves armed on an actor by `actor_set_morph_curve` (`0x5AE400`) and
loaded by `0x5CAE80` ("Error loading Morph Animation"). See [`docs/re/functions/actor.md`](../re/functions/actor.md) and
[`docs/re/structs/Morph.md`](../re/structs/Morph.md); the field meaning below follows that, the byte grammar was verified on the files.
Found beside flag/vegetation animations: `item/Object/Models_Items/Flags/*/anim/*.ma`, `Vegetation/Grass*/anim/*.MA`. The string `.MA` exists twice in
joan.exe (inside extension lists next to `.AM1`, `.AMT`, `.MDA`). Layout verified on all 15 files (records tile every file exactly).

| offset | size | type | meaning |
|---|---|---|---|
| 0 | 4 | i32 | `n_records` (11 in Fre_flag.ma) |
| 4 | 4 | f32 | `end_time` ms (366.667 = 0x43B75555; the Grass files have 0x45D05555 = 6826.67) |
| 8 | per record | `Rec[n_records]` | |

`Rec` = `char name[50]` (e.g. `english_flag2`; bytes after the NUL are uninitialised junk: `21 00 00 00 21 00 00 00` in Fre_flag, heap pointers in Grass_C.MA) +
`i32 n_keys` + `n_keys x { f32 time; f32 opacity }`. Record size = 54 + 8*n_keys: first record 0x08..0x55 (78 bytes, n=3: (0,0), (33.33,1.0), (66.67,0)), the second starts at 0x56 as predicted;
a 4-key record is 86 bytes (0xA4 -> 0xFA). Names are the model's plane names (`english_flag2`..`english_flag10` ...).

### `Grass_C.MA` (496 bytes, dumped whole)
```
0x00  04 00 00 00         n_records = 4
0x04  55 55 d0 45         end_time = 6666.67
0x08  "GrassC_A1" + junk  name[50] (junk = heap pointers after the NUL, e.g. 0x0cf67f80)
0x3A  0e 00 00 00         n_keys = 14
0x3E  00000000 9d44263f   key0 {time 0.0, opacity 0.6498}; next key 0x46 {300.0, 0.6498}; ... 8 bytes each, last key at 0xA6 {6666.67, 0.6498}
0xAE  "GrassC_A2"         record 1 (0xAE = 0x08 + 54 + 14*8), n_keys at 0xE0 = 3 -> next record "GrassC_B1" at 0xFA, then "GrassC_B2" at 0x14C (14 keys) ... ends at 0x1F0 = EOF
```
Install facts: 15 files = 4 flag copies (938 B, 11 records, end 366.67), `Grass01_B/_M` (638 B, 5 records, end 8000), `Grass_C` x2 (496 B, 4 records), `Grass01_M` family x8 (588 B, 6 records). Opacity values lie in [0,1], key
times are non-decreasing and <= `end_time`, records with 0 keys exist (`Grass01_Neu`, `04`). The record names are plane names (`GrassC_A1`) or plain indices (`01`..`06`) and
match the target names of the sibling `.mda` (`GrassC_A1` in `Grass_C.MDA`).

Evidence: `Fre_flag.ma` offsets 0x00, 0x08, 0x3A (n=3), 0x3E.. keys, 0x56 next name, 0xA4, 0xFA. Confidence: layout **verified on all 15 files**; consumer **unknown** (TODO(re): xref of the `.MA` string).
