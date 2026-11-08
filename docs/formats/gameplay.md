# `Resource/Gameplay.res` — tunable game constants (binary section index + text sections)

Reader: `gameplay.cpp` — section getters at **0x4DE530 / 0x4DE580 / 0x4DE5C0 / 0x4DE600** (reference `CHARMOVE`
0x9D014C), lookup in **0x454090**. Layout (file size 8,808 B):
```
u16 count (=6)
(count+1) × { char name[9] (NUL padded); u32 offset }  ; the extra entry has an empty name and offset = end of file (8808);
                                                       ; ECON 93, THROWI 380, UNIT 1941, CHARMOVE 7349, WEATHER 7851, GAME 8527
text sections at the offsets, each:
    // comments
    {GROUP}                 ; group header (BUILDING, CITY, …)
    [KEY]value   // comment ; KEY ≤ 8 chars, value int or float, inline // comments
```
Section names and keys are
ASCII; comments may hold any bytes (decode with the release page, never fail on them).

## Install findings
- Header is `2 + 7*13 = 93` bytes, so the first section (ECON) starts at 93 = the first offset. The index has **count+1**
  entries: the last has an all-zero name and offset 8808 = file size (= end of GAME). Sections are contiguous.
- Text is CR LF, comment-only lines start with `//`, blank lines allowed, `{GROUP}` headers, `[KEY]value` with optional
  tab/space then `// comment`. Every one of the 196 `[KEY]value` lines parses as int or float (`1.0`, `7000`, `-3`). Keys
  are 4-8 characters in the install (the reader's `+9` offset after the `[` suggests a 9-byte key slot); no duplicates.
  The first line of ECON is a `//` comment; case sensitivity of the original comparison: `TODO(re)` 0x47C180.
- Keys per section / group:

| section | offset | keys | groups (keys) |
|---|---|---|---|
| ECON | 93 | 5 | BUILDING 4, CITY 1 |
| THROWI | 380 | 60 | THROWI 60 |
| UNIT | 1941 | 98 | UNIT 78, CRITICAL 7, DANGER 1, MINIMAP 1, EXP 2, R_ATT 3, C_BOUNS 3, KILL 3 |
| CHARMOVE | 7349 | 8 | CHARMOVE 8 (AIM_R AIM_MXD AIM_HA TURN_ANG: float, JUMPMAXD JUMPMIND CHAINPRO CRI_TIME: int, read by 0x454090) |
| WEATHER | 7851 | 12 | WEFFECTS 9, CLOUDSYS 3 |
| GAME | 8527 | 13 | MUSIC 1, GAME 12 |

The file has a binary index, so it is not a ResLX bank like the other `.res` files ([res.md](res.md)).
