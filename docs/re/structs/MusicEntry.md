# MusicRes / MusicEntry (musicres.cpp, sizeof entry = 0x68) — joan.exe

`MusicRes`: `+0 i32 count`, `+4 MusicEntry* entries` (`count*0x68`, zeroed then filled by 0x4B3360), `+8 u8 loaded`. Source table: dBase `MUSIC` in the
gameplay bank, schema R of [gameset.md](../../formats/gameset.md) (42 records): `CODE C16, FILENAME C64, TYPE C1, STARTMIN N4, ENDMIN N4, STARTSCE N2, ENDSCE N2, E_TYPE C1`.
Field offsets inside the dBase record (after the delete flag): CODE 0x00, FILENAME 0x10, TYPE 0x50, STARTMIN 0x51, ENDMIN 0x55, STARTSCE 0x59, ENDSCE 0x5B, E_TYPE 0x5D.

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x00 | 2 | i16 | `id` | 1-based record number | 0x4B3360 | V |
| 0x02 | 17 | char[] | `code` | CODE, up to 16 chars, NUL-terminated | `copy_trimmed(+2, rec, 0x10)` | V |
| 0x13 | 65 | char[] | `filename` | FILENAME, up to 64 chars (the `music/*.wma` name; install has 40 files) | `copy_trimmed(+0x13, rec+0x10, 0x40)` | V |
| 0x54 | 4 | f32 | `start_min` | `(float)atoi(STARTMIN)` | psVar5+0x2A shorts | V |
| 0x58 | 4 | f32 | `end_min` | `(float)atoi(ENDMIN)` — units unknown (name says minutes; may be seconds) | psVar5+0x2C | V |
| 0x5C | 2 | i16 | `start_sce` | `atoi(STARTSCE)` (scenario/stage range start) | psVar5[0x2E] | V |
| 0x5E | 2 | i16 | `end_sce` | `atoi(ENDSCE)` | psVar5[0x2F] | V |
| 0x60 | 2 | i16 | `e_type` | E_TYPE letter -> M1 A2 N3 R4 S5 C6 I7 B8 W9 (case-insensitive), else 0 | 0x4B3360 | V |
| 0x62 | 2 | i16 | | always 0 | | R |
| 0x64 | 2 | i16 | `kind` | TYPE letter -> N1 B2 M3 E4 S5 G6 W7 C8, digit '1' -> 9, '0' -> 10; unmatched stays 0 | 0x4B3360 | V |
| 0x66 | 2 | i16 | | always 0 | | R |

Selection semantics (0x4B3940 / 0x4B3C70 / 0x4E6FE0): `kind` is the *situation*: 1 normal/explore, 2 battle, 3 menu/map, 4 environment (chosen by
time-of-day window `[start_min, end_min)`, wrapping midnight), 5 stinger (random one-shots keyed by `e_type`), 6 game-over, 7 victory, 8 special siege/castle, 9/10 result tracks.
Candidates = same kind and (scenario 0 or `start_sce <= scn <= end_sce`) and optional `e_type`; the pick is uniformly random.

Letter meanings are not named in the binary (candidates: N normal, B battle, M menu, E ending/environment, S sad/story, G ...). TODO(re): confirm against the
consumer and against the `MUSIC` rows (`TYPE`, `E_TYPE` columns) by dumping the gameplay bank (`tools/jdbf`).
