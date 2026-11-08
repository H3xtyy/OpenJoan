# ModelSet (0x69 bytes) - mesh LOD container loaded by `0x5C0EF0` / `0x5C2370`

Constructor `0x5C0EC0`.

| offset | size | type | name | meaning / evidence |
|---|---|---|---|---|
| 0x00 | 1 | u8 | loaded | set 1 at end of `0x5C0EF0`, `0x5C2370` |
| 0x02 | 2 | i16 | skinned | `param_2`: 0 static (.SMO), else skinned (.AM1) |
| 0x04 | 0x20 | char[32] | name | first 32 chars of the full path (+NUL at 0x24) |
| 0x28 | 4 | Model* | model | main mesh (0x11C), sync path |
| 0x2C | 4 | Skin* | skin | 0x3E-byte skin object, skinned only |
| 0x30 | 4 | i32 | = 1 static, 0 skinned | |
| 0x4C..0x5C | 4x5 | ptr | async file[0..4] | async path: main + LODs 1..4 (0x164-byte async file objects) |
| 0x60 | 4 | ptr | async file | `#V`-class proxy |
| 0x64 | 4 | ptr | async file | shadow `#s` |
Local sync-path temporaries hold additional Model* for the proxy and shadow meshes (not stored at fixed ModelSet offsets in the pseudocode: they are attached through `0x5D4470` (LOD list) and `0x5C51B0`). TODO(re): 0x5C51B0, 0x5C53B0 exact destination fields.
