# `.am3` — bone tree (skeleton hierarchy), 944 files

Reader: `FUN_005b69c0` (0x5B69C0, allocates a 12-byte `BoneTree`) -> `FUN_005b9af0` (0x5B9AF0, the parser);
`FUN_005b6800` (0x5B6800) is the "open `<dir><name>.AM3`, parse it" wrapper used after an `.am2` is loaded
(joan.exe; see [../re/functions/anim.md](../re/functions/anim.md), struct [BoneTree](../re/structs/BoneTree.md)).
The same extension is also opened for every model next to its `.am1`/`.smo` (actordef 0x5B0620 -> 0x5C5400, parser not
analysed, assumed identical): `unit/<model>.AM3` is the skeleton **of the model**, `anim/<name>.AM3` is the skeleton **of an
animation set**; both are matched by bone *name* at run time (see "Name matching").

All integers little-endian, no compression, no magic, no version, no padding. Strings are fixed 50-byte fields (cp1250/ASCII,
NUL-padded; bytes after the first NUL may be junk).

## Layout

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0 | 4 | u32 | `n_entries` | number of bone entries that follow | 0x5B9AF0: first `read_i32`; sample below |
| 4 | variable | `Entry[n_entries]` | | back-to-back, no alignment | |

`Entry`:

| rel. offset | size | type | name | meaning |
|---|---|---|---|---|
| 0 | 4 | i32 | `index` | bone index = track index in the paired `.am2` = palette index in `.am1` skin data. Indices need not be dense or sorted; the parser sets `bone_count = max(index)+1` (and at least `n_entries`) |
| 4 | 50 | char[50] | `name` | bone name (`Bip01`, `Bip01 Pelvis`, `Bip01 L Hand`, `Box02` ...). The reader forces `name[50]=0` in its copy |
| 54 | 4 | i32 | `n_children` | number of children |
| 58 | 4*n_children | i32[] | `children` | child bone indices |

Entry size = 58 + 4 * n_children. Parent links are derived from the child lists (parent of each child = this entry's `index`);
a bone that is nobody's child gets parent -1 (root; `Bip01` in every skeletal sample).

## Load algorithm (RE: 0x5B9AF0)

1. Read all entries into two temporary arrays (names: 0x38-byte `{index, name[50]+NUL}`; links: 8-byte `{parent, child}`).
2. `BoneTree::init(bone_count)` (0x5B9990): allocate `bone_count+1` 28-byte nodes, slot 0 is the virtual root (index -1).
3. Register every name in the **global bone-name registry** (0x5BA760, `strncpy(.., 50)`) -> a 1-based *name id*, stored in
   `node.name_id`; two trees with the same bone name get the same id.
4. Set all parents to -1, then apply the links (0x5B9DF0, not analysed).

## Name matching between trees (RE: 0x5BA300 / 0x5BA1D0 / 0x5B0620)

`actordef` builds an `int[bone_count]` remap (tree of the animation -> tree of the model) by comparing name ids: if either tree
has *all* name ids 0 the map is identity for `i < n_model_bones` and "none" (-999 = 0xFFFFFC19) otherwise; otherwise
`remap[i] = model bone whose name id equals anim bone i's name id` (via 0x5BA160), -999 when absent. So an `.am2` can drive
any model whose skeleton contains bones with the same names; extra animation tracks are ignored.
`0x5BA1D0` is the compatibility test (returns 1 = incompatible): incompatible when `model.count < anim.count`; else compatible when
every non-zero name id of the model-side tree matches the same slot, or either tree has only zero ids. TODO(re): read from decompiled branches only.

## Worked example — `anim/Alencon.am3` (1426 bytes)

```
0x00  17 00 00 00                n_entries = 23
0x04  00 00 00 00                entry0.index = 0
0x08  "Bip01" + zeros (50 B)     entry0.name  (0x08..0x39)
0x3A  01 00 00 00                n_children = 1
0x3E  01 00 00 00                children = [1]
0x42  01 00 00 00                entry1.index = 1
0x46  "Bip01 Pelvis"             entry1.name
0x78  01 00 00 00 / 02 00 00 00  n_children=1, children=[2]
0x80  02 00 00 00 "Bip01 Spine"  entry2 (index 2), name 0x84
0xB6  03 00 00 00 03,0F,13       entry2 has 3 children: 3, 15, 19   (entry3 starts at 0xC6, "Bip01 Spine1")
```
Verified: entry sizes 0x3E (62 = 58+4), 0x3E, 0x46 (58+12) tile exactly to the next index at 0x42, 0x80, 0xC6.
Inventory sample `item/Sentinelcbow.AM3`: `01 00 00 00 | 00 00 00 00 | "bone"` = one root bone named `bone` (a prop with a single node).
Shipped counts: first dwords `17`x253 (23 bones, humans), `01`x211 (props), `18`x111 (24 bones).

## Install facts (all 944 files)
- The entries of every file tile the file exactly (no trailing bytes).
- Indices are unique and dense `0..n-1`, entry 0 comes first, every child index exists, no bone has two parents, no cycles, everything reachable from a root.
- **Not always a single root**: 834 files have one root (`Bip01` or `bone`), 110 are forests (2 roots x31, 3 x13, 4 x22, 5 x28, 6 x8, 7 x5, 10, 100 and 101 roots - prop sets with many independent nodes).
  A reader must therefore treat the parent array as a forest (parent -1 for every root), not as one tree.
- 355 `.am3` have a sibling `.am2` of the same base name; the entry count equals the `.am2` `n_tracks` in all of them.
- `unit/alencon_p.am3` (1612 bytes) begins `1a 00 00 00 | 00 00 00 00 | "Bip01" ...` = 26 entries (the player model skeleton has 3 more bones than the 23-bone animation set `anim/Alencon.am3`).

## Confidence
Layout of header/entry: **verified** (hex of Alencon.am3 + code). Parent derivation and node-link roles: read from code, bone-link
setters 0x5B9DF0/0x5BA040/0x5BA0A0 not analysed (TODO(re)). `0x5C5400` (model-side AM3 loader) assumed to be the same parser.

## Bone names

There is one `.am3` format. Two kinds of name occur: Character Studio `Bip01 ...` names in character skeletons and `Box02`-style names in prop skeletons. The `id`
field of an entry is the bone index, and the entries follow the same order as the AM1 and AM2 bone indices.
