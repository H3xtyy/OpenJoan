# GameplayTable / GameplayCategory / GameplayEntry (`gameplay.cpp`) — joan.exe

In-memory form of `RESOURCE\GAMEPLAY.RES` as built by the *text* parser 0x47C6C0 (file layout: [docs/formats/gameplay.md](../../formats/gameplay.md)).
Functions: [gameloop.md §3](../functions/gameloop.md).

Loading (resolved): `0x47C520` opens `GAMEPLAY.RES` as a ResX container and calls the text parser once per record (section); all sections fill this one flat table, keyed by the `{GROUP}` names (not by the ResX section names).

## GameplayTable (global)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 4 | GameplayCategory** | `cats` | 1-based array, sorted by name (qsort 0x47CF20, comparator 0x47CC60), bsearch on lookup |
| 0x04 | 2 | u16 | `count` | |

## GameplayCategory (alloc 0xF bytes)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 9 | char[9] | `name` | category name from `{NAME}` (<= 8 chars + NUL) |
| 0x09 | 4 | GameplayEntry** | `entries` | 1-based, sorted by key (comparator 0x47C210) |
| 0x0D | 2 | u16 | `count` | |

## GameplayEntry (0x109 bytes)

| offset | size | type | name | meaning |
|---|---|---|---|---|
| 0x00 | 9 | char[9] | `key` | text between `[` and `]` (<= 8 chars) |
| 0x09 | 0x100 | char[256] | `value` | rest of the line (255 chars + NUL); numeric conversion is done by the getters (not exported) |

Parser rules (0x47C6C0): a line is read up to CR/LF/NUL, max 0x109 chars; first char `NUL`, TAB or space = ignored; `/` = comment; `[` = entry (`key` = text up to the first `]`, `value` = remainder
copied with `strncpy(...,0xFF)`); `{` = category (text up to the last `}`; created if missing); `~` = end of data; any other first character aborts the parse.
