# ResLX (ilib\reslx.cpp, sizeof = 0x135) — joan.exe

Identical field layout to [ResX](ResX.md) (all offsets 0x10E..0x131 the same), with these differences:

| item | ResX | ResLX | evidence |
|---|---|---|---|
| count field in file | u16 | **u32** | 0x59B920 uses `File::read_u32` |
| index entry | 13 bytes: name[9] + u32 | **38 bytes (0x26): name[34] + u32** | 0x59B920 (`(count+1)*0x26`) |
| offset position | entry+9 | entry+0x22 | 0x59BF70, 0x59C440 |
| sort comparator | 0x59D160 | 0x4070FE | 0x59C440 |
| `count` at +0x121 | sign-extended u16 | u32 | |

Allocated by `new(0x135)` in animscr (0x418F30). Extra accessors: `payload_size(i)` 0x59BDD0, `entry(i)` 0x59C3B0.
