# `Name { key:value; }` block files (common grammar)

Used by the world-editor outputs `.cm .trk .bm .uni .stg .pla .wor .ter .pth .wav` (`.por` is bracketed and has
its own page). Same `FileTxt` tokenizer as [keyvalue.md](keyvalue.md); the readers loop
`while (tok = get_token()) if (!stricmp(tok, "CreateX")) read_block()` — e.g. the `.uni` loop at
joan.exe **0x558890** and `.stg` dispatcher **0x44E5F0**.

## Grammar
```
file  := (block | comment)*
block := Name ws* '{' field* '}'            ; '{' and '}' on their own lines in every shipped file
field := key ':' value (';' | EOL)          ; .pth omits ';'; value may be empty ("Treasure:;") or hold spaces
```
- Keys may repeat (camera track points: `frame_time,x,y,z,h_dir,v_dir,rot_a,cam_dist` × N).
- Values are read by the original with `read_line` (strings: trailing `;` stripped by `buf[len-1]=0`) or
  `get_token` (numbers: the `;` becomes part of the token for the *last* number on a line, and `atof/atoi` stop
  at it). Float suffix `f` (`25.0f`) is accepted by `parse_float`.
- Unknown block names are skipped by the original (token loop); no unknown block name appears in the shipped files.

Reader: the ported `FileTxt` tokenizer in `src/ilib/filetxt.cpp`; the per-file block loops are in `src/game/` (see each page).
