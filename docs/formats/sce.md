# `.sce` — scenario script language (lexical grammar)

Parser in the original: `t_scrres.cpp` **0x63F8A0 / 0x63FCA0** (error strings `Expecting '}' after Init.`,
`Expecting Id!`, `Expecting ) for the ending script!` …), script resource table `0x4DEB70` (`texteff.res`),
unit scripts `uscrres.cpp` 0x5462B0 (`unitAI.res`). Census: docs/scripting/BUILTINS.md.

## Lexical grammar (full token table in docs/scripting/LANGUAGE.md)
```
file      := '"' name '"' '{' body '}'              ; the script starts with its quoted file name (all 42 files do)
identifier:= [A-Za-z_][A-Za-z0-9_]*
integer   := '-'? [0-9]+   float := '-'? [0-9]+ '.' [0-9]+     ; the sign is part of the literal (original: 0x662330 + atoi/atof); a lone '-' is subtraction
string    := '"' chars-without-quote-or-newline '"'       ; no escapes; code-page text (display names, [0001N] ids)
punct     := { } ( ) , = : == != < > <= >= && || + - * / ^    ; original table 0xA214C8 (no '!', no '%'; '^' = pow); ';' is not a token (glued to a preceding number it is ignored by atoi)
keywords  := ID INIT RESET WAIT CHOICE IF ELSE SPEAKER UNIT OBJECT GOTO EXIT FUNC DIALOG CAMERA ACTION XYZ SCRIPT INT FLOAT STRING VECTOR EVENTSYSTEM   ; case-insensitive, reserved; code uses INT init exit if else WAIT FUNC ACTION DIALOG SCRIPT
comments  := '//' to end of line   (no block comments in any of the 42 files)
```
Verified findings (2026-10-22): the BUILTINS.md "keywords" `for and or not while switch case break continue return main`
occur **only inside string literals**; the census regex counted them there. The census numbers are
(code keywords + in-string regex hits). Whitespace is CRLF/TAB; no
block comments; no `--`; two stray `;` (`S2L1_tactic.sce:1234`); 42 files lex without error, 0 invalid bytes,
6902 comments, 35071 strings, 30059 integers, 626 floats.

Original parser reference: the full token-kind table (dumped from 0xA214C8) and the grammar are in docs/scripting/LANGUAGE.md and
docs/re/structs/SceToken.md. `0x63F8A0` / `0x63FCA0` are the parse functions of the `ID n { }` (unused dialog grammar) and `INIT { }` statements, not the scenario-file parser; the file parser is `0x649140`.

Language semantics: docs/scripting/LANGUAGE.md.

Lexical facts over the 42 files: no lexeme is classified as unknown; the two
`;` are glued to numbers (`0;` = 0); `-` counts only the operator uses (signed literals are one token); every file is accepted by the grammar
(`S8L1_tactic.sce` only with the lenient unknown-name policy of BUILTINS.md §4.1); `S8L1_finish.sce` and `S9L1_finish.sce`
carry the quoted name `"S5L1_finish.sce"` (shipped data); there is no `S1L1EX.res`, the other `<base>EX.res` companions exist.

## Open questions
- Resolved: `!` and `%` are not tokens, `/` and `^` are; `STRING`/`FLOAT`/`VECTOR` are keywords; the sign belongs to the number token.
