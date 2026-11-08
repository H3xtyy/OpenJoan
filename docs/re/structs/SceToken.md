# SceToken — `.sce` token record, keyword table, token stream

Reference build `joan.exe`. Language description: [scripting/LANGUAGE.md](../../scripting/LANGUAGE.md); functions:
[functions/script_vm.md](../functions/script_vm.md). Related: [SceStatement](SceStatement.md), [SceScript](SceScript.md), [SceDynArr](SceDynArr.md).

## 1. Token record (12 bytes)

Elements of the token array (a [SceDynArr](SceDynArr.md) with element size `0xC`, grow step `0x1E`, ctor `0x661EA0`).
Built by `sce_token_push` 0x6622D0 (`{text, kind, line}` on the stack, then `sce_token_classify` 0x662330, then `dynarr_push` 0x584620).

| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 4 | char* | text | NUL-terminated lexeme, a copy owned by the compile-time string pool (0x684590 -> 0x5A18D0). For strings the classifier advances it past the opening quote and overwrites the closing quote with NUL | 0x662330 (`*param_1 += 1`, terminator write) |
| 0x04 | 4 | i32 | kind | token kind, table in section 2; initial value 0x22 | 0x662330 first statement `param_1[1] = 0x22` |
| 0x08 | 4 | i32 | line | 1-based source line of the lexeme (number of `\n` seen so far + 1) | 0x6840D0 `local_14`; read by every error report (`token+8`) |

Empty lexemes (`strlen == 0`) are dropped (0x6622D0).

## 2. Token kinds

The table at 0xA214C8 was dumped from the data section (`$JOAN_WORK_DIR/decomp/script/keyword_table_0xa214c8.tsv`); every kind below is now
**verified** by it, except that a few spellings are stated by the table and the earlier "inferred" marks in this page are superseded.

| kind | token | status |
|---|---|---|
| 1 | integer literal (`atoi`, may start with `-`) | verified 0x661FF0, 0x662330 |
| 2 | float literal (contains `.`; `atof` cast to f32) | verified 0x662060 |
| 3 | string literal (quotes stripped) | verified 0x662330, 0x649140 |
| 4 / 5 | `{` / `}` | verified 0x649140 |
| 6 | `=` | verified 0x644680 |
| 7 | `,` | verified 0x643C00, 0x640BD0 |
| 8 | `:` | verified 0x640060 (dialog grammar) |
| 9 | table entry `"\n"`; skipped (and counted) by `sce_tok_peek` 0x642370; the lexer never emits a bare newline lexeme | verified table + 0x642370 |
| 0x0A | `EVENTSYSTEM` keyword; no parse function or factory case uses it (reserved word, a statement starting with it is the fatal "Token type does not correspond to a statement!") | table |
| 0x0B | `ID` | verified 0x63F8A0 |
| 0x0C | `INIT` | verified 0x63FCA0 |
| 0x0D | `RESET` | verified 0x63FF00 |
| 0x0E | `WAIT` | verified 0x6404E0 |
| 0x0F | `CHOICE` | verified 0x640BD0 |
| 0x10 | `IF` | verified 0x645A40 |
| 0x11 | `ELSE` | verified 0x645A40 / 0x648630 |
| 0x12 | `SPEAKER` | verified 0x640060 |
| 0x13 | `UNIT` | verified 0x640170 |
| 0x14 | `OBJECT` | verified 0x640280 |
| 0x15 | `GOTO` | verified 0x6408F0 |
| 0x16 | `EXIT` | verified 0x6409F0 |
| 0x17 | `FUNC` | order from "Expecting FUNC/DIALOG/CAMERA/ACTION!" 0x644190 + pool order (inferred) |
| 0x18 | `DIALOG` | inferred (same) |
| 0x19 | `CAMERA` | inferred (same) |
| 0x1A | `ACTION` | inferred (same); 0x643D40 special parse belongs to this call class |
| 0x1B | `XYZ` (vector literal `XYZ(x, y, z)`) | verified 0x6620D0 |
| 0x1C | `SCRIPT` | verified 0x645DE0 ("Expecting Script!") |
| 0x1D / 0x1E / 0x1F / 0x20 | `INT` / `FLOAT` / `STRING` / `VECTOR` (variable declarations) | 0x6439D0 maps them to value types 1 / 2 / 3 / 0x1B; spellings inferred from the pool order |
| 0x21 | identifier (`[A-Za-z_][A-Za-z0-9_]*`) | verified 0x662330 |
| 0x22 | unrecognised word (never valid; a later parse error) | verified 0x662330 |
| -0x32 | `+` | operator codes verified by the evaluator 0x6463F0 and precedence helper 0x6484B0; spellings of -0x2E unknown |
| -0x31 | `-` | |
| -0x30 | `*` | |
| -0x2F | `/` | |
| -0x2E | `^` = power: evaluator calls `pow` (0x7B2719) and truncates the integer case with `__ftol`; there is no modulo | verified (table spelling + evaluator) |
| -0x2D / -0x2C / -0x2B / -0x2A | `<` / `>` / `<=` / `>=` | verified 0x6463F0, 0x644D80 |
| -0x29 / -0x28 | `==` / `!=` | verified |
| -0x27 / -0x26 | `&&` / `\|\|` | verified (precedences 2 / 1) |
| -0x25 / -0x24 | `(` / `)` | verified 0x647C90, 0x644190 |

44 entries = 15 operators + `{ } = , :` + `"\n"` + `EVENTSYSTEM` + 22 keywords.

## 3. Keyword / punctuation table (0xA214C8)

`0x2C` (44) entries of 8 bytes `{ const char* name; i32 kind }`. Sorted once with `qsort` on first use (flag `DAT_00A6A268`, comparator 0x662590 calling
`FUN_007C07B0(a,b)`), then every lexeme is looked up with `bsearch` (comparator 0x6625C0, same function). `FUN_007C07B0` is treated as `_stricmp`
(inferred by use: it is the case-insensitive comparator everywhere else, and the table spellings are upper-case while the scripts write `if`, `else`, `init`, `exit` in lower case). The same function compares builtin names (0x661E10) and variable names (0x63EE70).

Classification order (0x662330) after a table miss:
1. first char is a digit or `-` (isdigit/`'-'`) -> number: kind 2 if the text contains `.` (`strchr`), else kind 1. A trailing garbage tail is ignored because the value is read with `atoi`/`atof` (e.g. `2000*aliveCount` is the integer 2000, `0;` is 0);
2. first char `"` -> kind 3; a text that does not end in `"` throws the token pointer ("Bad token", see [SceScript](SceScript.md) section 5);
3. first char alphabetic or `_` and every following char alphanumeric or `_` -> kind 0x21; otherwise the kind stays 0x22.

## 4. Token stream object

Created by 0x661EA0 for each compile (stack object, 48 bytes). It is a [SceDynArr](SceDynArr.md) of tokens plus a source-line counter:

| field (relative to the array base used by 0x4B9C30 / 0x417AD0) | meaning |
|---|---|
| `+0x08` cur | 1-based cursor; the "current token" is `data[(cur-1)*stride]`; 0 = none |
| `+0x0C` count | number of tokens |
| `+0x10` stride | 0x0C |
| `+0x19` data | base pointer (unaligned) |

MSVC places the vtable (0x9DBB54, base 0x9DCD98 built by 0x684010, which also owns the string pool 0x5A1730) in front of the array, and the parse functions address the
stream through its vtable-first pointer; the line counter used by `sce_tok_peek` (0x642370) and reset by 0x6498E0 is at `stream+0x29`. The exact relation
between the two pointer bases is **inferred** (the decompiler hides the implicit `this`). Slot 1 of 0x9DBB54 is `on_token(text, line)` = 0x6622D0.

Primitive operations (all on the current-token cursor):

| function | behaviour |
|---|---|
| `sce_tok_peek` 0x642370 | returns the current token; while its kind is 9, increments the line counter and advances |
| `sce_tok_next` 0x6423E0 | if `count <= cur` (already on the last token) throws the C string `"Unexpected end-of-file."`; otherwise `cur++` then `sce_tok_peek`; returns the new current token |
| `cursor_advance` 0x63EE20 | `if (cur < count) { cur++; return true } return false` (shared with the statement lists) |
| `rewind` 0x6498E0 | `cur = min(count,1)`, line counter = 1 |

The parser never consumes the last token of the file (`}` closing the script is left unread).
