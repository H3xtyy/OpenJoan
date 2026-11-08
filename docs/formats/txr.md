# `.txr` — text resource tables (`Resource/t_basic.txr`, `Resource/t_interf.txr`)

Reader: `ilib::txtres::fn_005a21e0` at joan.exe 0x5A21E0 (takes the path; called with
`"RESOURCE\\T_BASIC.TXR"` from 0x4DA620 and `"RESOURCE\\T_INTERF.TXR"` from 0x4DE9C0). The body of 0x5A21E0
was not analysed, so everything below is derived from the two shipped files (all 2 instances).
Files are cp1250 (Polish install), CRLF line endings.

```
line 1 : N1          ; see "Counts" below
line 2 : N2          ; total number of records
K lines: KEY|count   ; key table (file order, not sorted): category name and its record count
"#"                  ; separator
records: index|KEY|n|text          ; index 1-based global (= line order); n 1-based within KEY
```
Only the first three `|` of a record separate fields; the text is the rest of the line and may contain `|`
inside `{a|b|c}` groups. Records of one KEY need not be contiguous (t_interf: `GMSG|2` is record 8,
`GMSG|6` record 238). Key names are ASCII letters, digits, `-` and `_` (`T-A`, `G_POPUP`).

## Counts (verified against both files)
| file | line 1 (N1) | line 2 (N2) | records | sum of key counts | keys |
|---|---|---|---|---|---|
| t_basic.txr | 37 | 38 | 38 | 38 | 14 |
| t_interf.txr | 367 | 367 | 367 | 367 | 31 |

* **N2 = number of records = sum of the key counts**, in both files. Both files agree.
* **N1** equals N2 in t_interf but is one less in t_basic (37 vs 38). No record, key or index in t_basic is
  special enough to explain it (the last record `38|POPUP|6|Włączono tryb RTS.` looks like any other), so it is
  most likely a stale count in the data that the original ignores or uses only as a lower bound (e.g. an
  allocation hint). The reader ignores it (see "Reader behaviour" below).

Invariants in both files: record `index` is sequential from 1; every KEY exists in the key table;
`1 <= n <= count`; `(KEY, n)` unique and every key has exactly `count` records; no U+FFFD after decoding.

## Format mini-language in the texts
Argument numbers `N` are 1-based indices into the caller's argument list. Code forms found (occurrences over
both files; 22 of 38 t_basic texts and 52 of 367 t_interf texts contain codes):

| form | meaning (inferred) | t_basic | t_interf |
|---|---|---|---|
| `%Nd` | decimal integer argument N | 18 | 29 |
| `%N,Wd` | integer argument N with field width W (`%3,4d` = year, `%1,4d`); arg/width order is a guess | 2 | 1 |
| `%Ns` | string argument N (names, hotkeys, items) | 0 | 25 |
| `%Nj{a\|b\|…}` | choose the alternative selected by argument N (1-based): months (12), weekdays (7), numbers 1..20 | 6 | 1 |
| `%Np{one\|many}` | plural form chosen by argument N (Polish: 1 vs other; some entries repeat the same form) | 16 | 4 |
| `%Nq{a\|b\|c}` | joiner chosen by argument N, always paired with `%Nz`; alternatives are `""`, separator, separator (`", "`, `" i "`, `" & "`, `" sek."`) | 32 | 0 |
| `%Nz` | zero-suppression marker for argument N: the group before it is dropped when that argument is 0 | 16 | 0 |
| `%N?{a\|b}` | two-way pick by argument N (`%5?{jej\|jego}`, grammatical gender) | 0 | 1 |
| `%%` | escape; occurs as `%%1s` in the "Naciśnij %%1s aby …" hotkey messages (9x), probably yields a literal `%1s` for a second substitution pass | 0 | 9 |

No other `%` sequence occurs; a bare `%` not followed by a digit or `%` is a scan error in `scan_format`.
Duration entries (`TSA`, `T-A`, `TS`, `T`, `DSA`, `D-A`, `DS`, `D`) combine `%Nd %Np{…}` units with `%Nq`/`%Nz`
separators for hours/minutes/seconds (args 1–3) and years/months/days. Groups never nest. Empty alternatives
are legal (`%3q{| i | i }` has three: `""`, `" i "`, `" i "`).
Exact selection rules (index semantics of `j`/`q`/`?`, evaluation order of `z`) are not specified here.

## Key tables
t_basic (14): DATE 6, AMPM 4, TSA 3, T-A 1, TS 3, T 1, DSA 3, D-A 1, DS 3, D 1, NAME 3, BOX 2, YEAR 1, POPUP 6.
t_interf (31): BUY 10, GMSG 12, UPGRADE 2, ATTCOMB 2, IGAMEMNU 15, OPTMENU 56, SAVEMENU 25, VICT 5, OBJMENU 77,
MAINGAME 7, QSAVE 6, INVENT 15, AUTOSAVE 2, GAMEOVER 3, UNITATT 21, MESSAGE 4, SMS 15, VICTS 8, BARRACK 15,
WORKSHOP 19, RTS 1, MSGLOG 2, QUIT 2, MAINMENU 11, ARMORY 4, SCELOAD 1, G_POPUP 3, CHARMOVE 4, ENDING 17,
CREDITS 2, SYSDET 1.

## Open questions
* Whether lookups in the original are by (KEY, n) or by global index only (both are provided here).
* `%%` and `%N?{}` / `%Nq{}` exact semantics (above) — needs 0x5A21E0 callers or the formatter.

## Reader behaviour (RE: joan.exe 0x5A21E0 `TxtRes::load`, ilib/txtres.cpp — verified 2026-10-22)
- Line 1 is read into a 10-byte buffer and **never used** (explains `t_basic.txr` 37 vs 38 records: a stale count).
- Line 2 = record count; the record table is allocated as `count × 0x15` bytes: `{ int32 index; int32 n; char key[9]; char* text }`.
- The key table lines are skipped: the reader discards lines until one starts with `#`.
- Each following line (≤ 0x200 bytes, "Text too long!" otherwise) is one record, split at the first **three** `|`:
  index (atoi), KEY (≤ 8 chars + NUL), n (atoi), text = everything after the third `|` (may contain more `|`).
- Reading stops at EOF or at an empty line. Nothing validates the key table against the records.
