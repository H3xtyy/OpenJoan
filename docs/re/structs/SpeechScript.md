# SpeechScript (speech_s.cpp, sizeof 0x138) — joan.exe

Lip-sync / subtitle script player (corrected in round 3). Created by `new(0x138)` + zero-fill (0x4D15B0), opened with a FileTxt (0x4D1770), advanced by 0x4D1BE0,
closed by 0x4D1980. Row format and algorithm: [audio.md](../functions/audio.md) "Speech = lip-sync".

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x00 | 4 | FileTxt* | `file` | owned (0x19A-byte object) | 0x4D1770, 0x4D1980 | V |
| 0x04 | 4 | Actor* | `target` | model whose morph targets receive weights (set by 0x4D1AF0 from the talker's actor `+0x2F5`) | 0x63B5E0, 0x4D1BE0 | R |
| 0x08 | 4 | char* | `text` | subtitle accumulator (400-byte chunks) | 0x4D27E0 | V |
| 0x0C | 4 | i32 | `text_len` | | 0x4D27E0 | V |
| 0x10 | 4 | i32 | `text_cap` | initial 400 | 0x4D27E0 | V |
| 0x14 | 4 | i32 | `time_ms` | script time = `(int)time_f` | 0x4D1BE0 | V |
| 0x18 | 4 | u32 | `last_clock` | `DAT_00A500BA` at last update; 0 = not started | 0x4D1BE0 | V |
| 0x1C | 4 | f32 | `time_f` | accumulates `(clock - last_clock) * speed` | 0x4D1BE0 | V |
| 0x20 | 4 | i32 | `next_key_ms` | time of the row read ahead, -1 = none | 0x4D1BE0, 0x4D1980 | V |
| 0x24 | 4 | char(*)[0x50] | `names` | column names from header row (first line, comma separated), grown by 5 | 0x4D1770 | V |
| 0x28 | 4 | i32 | `text_col` | column index whose header equals `"text"` (`DAT_009D5B64`, the subtitle column); -1 none | 0x4D1770, 0x4D1BE0 | V |
| 0x2C | 4 | i32 | `n_cols` | | 0x4D1770 | V |
| 0x30 | 0x100? | char[] | `cur_text` | text of the row about to be shown (subtitle) | 0x4D1BE0 | R |
| 0x130 | 4 | i32 | `state` | 0 closed, 1 open, 2 playing, 3 seek-to-first-key, 4 finished | 0x4D1BE0, 0x4D1770 (writes 1) | V |
| 0x134 | 4 | f32 | `speed` | time multiplier, init 1.0 | 0x4D1770 | V |

Row: `time_ms, col_1, ..., col_n`. Non-text columns: `|`-separated (`DAT_009D5B84`) tokens `name` or `name[weight]`; `weight / 100` (const `_DAT_009CF9C4`), default 1.0; token
index = morph-target slot from the name table (0x5C90A0); weights are renormalised so that their total is 1.0 (the remainder is added to element 0).
