# esstate2 script structures (`uti3d/esstate2.cpp`)

Reference build joan.exe. Functions: [video.md](../functions/video.md) section "esstate2"; design notes in
engine/video.md. Layouts are partial and mostly inferred (many `this` registers are lost by the decompiler).

## Token (stride 0x10, array `tokens` in the parser object)
Parser object fields (packed, unaligned): `tokens` base pointer at +0x19, `token_stride` at +0x10 (=0x10), `cur_index` at +0x08 (1-based, 0 = none),
`count` at +0x0C, `line_counter` at +0x29. Current token = `*(this+0x19) + (cur_index-1)*stride` (0x4B9C30, 0x417200).
| offset | size | name | meaning | evidence |
|---|---|---|---|---|
| 0x00 | 4 | text | char* (NUL-terminated lexeme) | 0x645DE0 passes `*tok` to the name lookup |
| 0x04 | 4 | type | token type; observed: 3 = word/identifier or string, 9 = newline (skipped, bumps `line_counter`), 0x1C = keyword `Script`, -0x25 (0xFFFFFFDB) = `(`, -0x24 = `)` | 0x645DE0, 0x642370 |
| 0x08 | 4 | line | source line (for error reports) | 0x645DE0 -> 0x63EB80 |

## ScriptSlot (stride 0x10C = 268, array lookup via 0x417200(index) = `base + (idx-1)*stride`, 1-based, cache of already-parsed scripts)
| offset | size | name | meaning | evidence |
|---|---|---|---|---|
| 0x000 | 4 | refcount | `InterlockedIncrement` (0x648D20) / `InterlockedDecrement` (0x648D60); slot is free when 0 (0x648E40) | verified |
| 0x004 | 260 | name | script file name (strcpy, case-sensitive compare `FUN_007c07b0` = strcmp/_stricmp) | verified 0x648BD0, 0x648C50 |
| 0x108 | 4 | parser | owning pointer to the parsed script object (vtable slot 0 = deleting dtor, called with flag 1 when refcount reaches 0) | verified 0x648BD0, 0x648D60 |

## EsScript object (this is the compiled-script container; 0x70 bytes, ctor 0x63F190, base ctor 0x63E6E0 (vtable 0x9DA980, `memset(+0x14,0,0xC)`); vtable 0x9DA9C8)
Added fields (verified in 0x63F2C0, 0x63E7F0, 0x641A60, 0x641C30): `+0x04 owner` (set by slot 1 = 0x63E630), `+0x14 / +0x18 / +0x1C` = three dynamic arrays
(pointer to header `{count@+0, ..., stride@+0x10}`, items are polymorphic objects with a virtual dtor (slot 0) and `memory_size()` (slot 11, +0x2C)), `+0x64 dlgres*`,
`+0x68 arena*`, `+0x6C arena counter`. Table below keeps the earlier (round-1) field notes; the "vtable slot 1 = on_token" remark there is **wrong** (slot 1 is the owner setter).

Round 14: this "EsScript" object is the `.sce` **Script** class; field `+0x20` = script name (char[], 0x649860); `+0x14` statement array (items have `kind` at +8, `Id` value at +0x21), `+0x18` named blocks (name at item+0x14); the tokenizer/token-list object is a different class (vtable 0x9DBB54: `cur` index at +8, `count` at +0xC, stride 0x10 at +0x10, data pointer at +0x19). Slot list: functions/video.md section 8.

## dlgres object (`uti3d/dlgres.cpp`, 0x637EC0; `<base>EX.res` string table)
`+0 u8 initialised`, `+1 i16 count`, `+3 Entry* entries` (buffer of 0x12C0 bytes = 300 x 16), `+7 char* pool`, `+0xB u32 pool_size`; entry: text pointer at `+4`
(pointers inside `pool` are not counted twice by 0x638D50). Loader 0x6381A0 (TODO(re), not exported).

## (old) ScriptParser object notes (vtable 0x9DA9C8)
| offset | size | name | meaning | evidence |
|---|---|---|---|---|
| 0x00 | 4 | vtable | slot 1 (`+4`) = `on_token(text, line)` — the lexer pushes each token through it; slot 2 (`+8`) = reset; slot 4 (`+0x10`) = finish | 0x649280, 0x6840D0 |
| 0x08 | 4 | (int) | set to 1 by ctor | 0x63F190 |
| 0x64 | 4 | dlgres* | companion dialog resource (`<base>EX.res`), created by 0x63F580, freed by 0x63F4E0 | 0x63F580 |
| 0x68 | 4 | arena | allocator arena pointer (`param_1[0x1a]`) created on first compile (0x649280) | 0x649280 |
| 0x6C | 4 | arena_count | allocation counter (`param_1+0x1b`) incremented per arena allocation (0x63E450) | 0x649280, 0x63E450 |

Arena: globals `DAT_00a6a0bc` (arena object) and `DAT_00a6a0c0` (counter ptr) redirect `0x63E450` (the parser's `operator new`) to a bump allocator
(0x5A18D0, 4-byte aligned) while a script is being parsed; they are saved/zeroed/restored around nested `Script(...)` parses so that a nested
parser object is allocated from the normal heap.

## Lexer file reader (0x586DA0 / 0x5873A0 / 0x587F30 / 0x587260)
`File` helper: name char[0x105] at +0, `HANDLE` at +0x105 (-1 = closed; set by 0x4179C0), `u32 flags` at +0x109, `u8 flag` at +0x10D; open =
`CreateFileA(prefix + name, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL)` where the prefix is the global base path string
`PTR_DAT_00a1579c`; error strings "Error opening file %s." / "File : file name is too long." / "Error reading file %s, Retry ?". See also
[File.md](File.md).
