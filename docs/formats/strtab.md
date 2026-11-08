# String tables: `*EX.res` / `*_des.sce`-companions and `help.res` (`~`-separated records)

## A. `script/scenario/<name>EX.res` (28 files: `S?L?_desEX.res`, `_finishEX.res`, `_tacticEX.res`, `S?L?EX.res`)
Reader: **0x638AA0** / **0x63F580** (`EX.res` literal 0x9DA868; `_des` 0x9CF820 used by `battle.cpp` 0x41DFE0 to
build `<script>_des` names). Localised text referenced by scripts through ids.
```
file   := record ( "~" EOL record )*
record := id EOL text-lines            ; id = "[" 4 digits letter "]"  e.g. [0001N], [0002M]
```
- Confirmed by 0x638AA0: this function is the *writer* of the format (it emits `[%s%c]\n`, the text, then the `~` separator
  for each of `count` records, ids zero-padded to 4 digits), so the syntax is exactly what the file shows.
- All 29 files (not 28) in the install: ids are 1-based and **contiguous** in every file (1161 records). Kind letters found: `N`
  (1161x) and `M` (once, `S2L0_cut_scene_1EX.res` id `0001M`, a cut-scene line); the meaning of `M` is `TODO(re)`.
- Terminator: a line that is exactly `~`. `S3L1_tacticEX.res` has **no** `~` after its last record (id 0068).
- Line ends are CR LF; text may span several lines (kept joined with `\n`); no record is empty; no U+FFFD after cp1250 decoding.
- text may span several lines; keep line breaks (UI wraps). Decode through the release code page.

## B. `Resource/help.res`
Reader: `help.cpp` **0x488400/0x4884C0/0x488590**.
```
record := key EOL title EOL body-lines   ; key ≤ 8 chars (`CompCol`, `BUGphicD`), title = caption, body = help text
file   := record ("~" EOL record)*
```
Confirmed in the install: 15 records, keys 6-8 characters, titles <= 60 characters (0x488590 copies the key with
`strncpy(...,8)` and the title with a 60-byte read), bodies one line, the file ends `~\r\n\r\n`. 0x488590 also has a numeric
branch (a line starting with a digit is read as 20 numbers into the record); no shipped record uses it.
Keys are referenced by UI elements (`Interfac.set`). The English text in the Polish install is original
(unlocalised help) — do not assume the language of the content.

## C. Telling the layouts apart
The `.res` extension is shared with the binary ResLX banks ([res.md](res.md)). By content: control bytes in the first 512 bytes
-> binary bank; first bytes `[dddd?]` -> EX table; a line `~` -> help table.

## Open questions
- Meaning of kind `M` (vs `N`) - maybe "monologue/cut-scene"; the reader of the `[%s%c]` kind byte is not yet found.
