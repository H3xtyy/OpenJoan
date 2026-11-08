# Key:value record files (common grammar)

Used by `.scr .lgt .sca .pro .day` and the model `.pro` property files. Read by Enlight's `FileTxt` tokenizer
(`ilib/filetxt.cpp`): `FileTxt::get_token` = joan.exe **0x589C70**, `FileTxt::read_line` = **0x589F10**,
`FileTxt::next_line` = **0x5899B0** (docs/re/functions/text_formats.md).

## Tokenizer facts (RE: 0x589C70)
- Token delimiters skipped before a token: space, `,`, `:`, tab. A token ends at space, `=`, `,`, `:`, tab or CR.
  `;` is **not** a delimiter (block files strip it by hand). Max token length 0x80.
- `0x1A` (DOS EOF) ends the file. CR starts the next line (`next_line`); LF is skipped as part of the line break.
- `read_line(buf, n)` copies the rest of the current line (up to n chars) — used for values that contain spaces.
- Consequence: the original readers are **positional** — they call `get_token` for the key and ignore it, then read
  the value. Key names are therefore not in the binary; a reader written from this page should match keys case-insensitively and keep
  the documented order.

## Grammar
```
file    := (record | comment | blank)*
record  := line+                      ; consecutive non-blank lines
line    := key ws* ':' ws* value      ; value = rest of line, trimmed; may be empty
        |  bare                       ; a line without ':' (model .pro: "VERSION 1.0", "END")
comment := ('#' | '//') any*          ; Scat.pro uses "# SunRise" section comments
```
Encoding: release code page (cp1250 here), as is. Line ends: CR LF.
Tokenizer: `FileTxt` in `src/ilib/filetxt.cpp`.
