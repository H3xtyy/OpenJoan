# `animscr.res` — animation scripts (attack timing, hit windows, effect emitters)

`Resource/animscr.res`, 352,350 bytes, **347 records**. Container: ResLX bank, see [res.md](res.md) (`u32 count`, 38-byte index entries with a 34-byte name, sentinel).
Each payload is a CR LF text record. Reader (joan.exe): `animscr_load` **0x418F30**, record parser **0x419320**, emitter resolver 0x418C60; function table
[../re/functions/script_res.md](../re/functions/script_res.md) §4; in-memory record [../re/structs/AnimScript.md](../re/structs/AnimScript.md) (0x21B bytes).
Everything below is verified against the shipped file unless marked *inferred*.

## 1. Container and indexing

- The ResLX entry name (`SWORD`, `HUM_GEN_ATK_A`, `JOAN_ATK_AAAB`, ...) is **not used** by the engine. The reader numbers the records `1..347` in file order and
  the record's own name is the value of its `Code` key (copied into the record, 32 bytes max). The two usually agree, but not always (verified over the shipped file, 2026-10-22): 145 records differ only in letter case (`JOHN_POLE_ATK_B` / `Code : John_Pole_ATK_B`) and **two differ in
  spelling**: record 241 entry `BATTTERRAM_ATTACK` / `Code : BATTERRAM_ATTACK`, record 316 entry `WILLIAM_POLE_DOWN_ATK` / `Code : John_Pole_DOWN_ATK`. Lookups
  must use `Code`; the entry name of record 316 is unreachable.
- **Selection by animation name.** The animation DBF (`ANIMATE` table inside `Anim_am2.res`/`.SET`, see [dbf.md](dbf.md), [am2.md](am2.md)) has a 32-character column
  at row offset 0x9A (the previous 32-character column, at 0x7A, is a separate name that the reader copies to the group entry +0x4D). `animgrp::fn_00414A30` rtrims it and calls `0x41A0E0(name)`, which returns the 1-based
  animscr record number (0 = none) stored in the animation-group entry (+0x72). **Verified (0x41A0E0)**: linear scan, case-insensitive full-string match on the record's `Code`, first match wins
  (so record 316, the second `John_Pole_DOWN_ATK` and the only duplicated `Code`, is unreachable by name; record 124 wins). An empty/unknown name gives 0 and the animation simply has no script.
- Count / size limits: names <= 32 chars (longest shipped = 32), at most 6 emitter slots, `Range`/times are 32-bit signed integers.
- The reader reads **the whole file** into RAM once, parses all records, then discards the text; there is no lazy reload.

## 2. Lexical rules (the `FileTxt` token reader, 0x589C70)

- Encoding: single-byte; lines end with **CR LF**; the engine appends `0x1A` after the payload and stops there. A bare-LF file would not work.
- A *token* is a run of bytes not containing space, TAB, `,`, `:`, `=` or CR. Leading and trailing separators (space, TAB, `,`, `:`) are skipped. So
  `StartTime : 400`, `StartTime<TAB>:<TAB>400` and `StartTime 400` are all identical: a **key token followed by a value token**. `=` **ends** a token but is
  **never skipped** (0x589C70 / 0x5899B0 skip only the four separators): `StartTime=400` yields `StartTime`, then empty tokens forever at the `=` — the original
  never reaches `END` (no shipped record uses `=`).
- At a CR the reader moves to the next line (0x5899B0: to CR, skip CR, skip one LF, skip leading separators) and continues there; a bare LF is not a line end
  (it becomes part of the next token). A token that runs into the end marker `0x1A` is lost (the reader returns NULL), so the last token (`END`) must be followed
  by a separator or CR LF.
  Tokens longer than 128 bytes are truncated. There are no comments and no quoting; a value cannot contain a space.
- Key matching is case-insensitive (`strcmp` 0x7C07B0); *value* comparisons made by the parser (`none`) are also case-insensitive.
- Numbers: `atoi` (decimal, optional sign, trailing junk ignored, no hex) for all integer fields; `atof` for `GravityLevel` only.

## 3. Record grammar

```
record  := "VERSION" <version> NL  { pair NL }  "END" NL
pair    := key value
```

1. The first two tokens are read and ignored (`VERSION`, `1.0`). All shipped records have `VERSION 1.0`.
2. Then tokens are read **one at a time as keys** until a token equal to `END` is read (literal at 0x9CF644, inferred from the file). A key that is not in the table is
   skipped *without* consuming its value; the value is then examined as a key on the next iteration (harmless unless a value equals a key name). Reaching EOF before
   `END` makes the reader dereference NULL: every record must end with `END`.
3. Keys may appear in any order and are optional, except the emitter sub-block (§3.2), which is positional.

### 3.1 Scalar keys

Defaults apply when the key is absent (the record is zero-filled first, then 0x48/0x4C/0x50 are preset).

| key | type | default | record offset | meaning |
|---|---|---|---|---|
| `Code` | name (<= 32) | "" | +0x02 | record name = animation script name |
| `StartTime` | int, ms | 0 | +0x28 | delay from animation start to the **attack/hit moment** (event time = `now + StartTime`) |
| `EndTime` | int, ms | 0 | +0x2C | end of the action window; **not an absolute time** (shipped values range 0..1200 and are often < `StartTime`, e.g. `HUM_GEN_ATK_A` 267/233, `HUM_GEN_HOR_ATK_A` 267/0). Consumer not located: TODO(re) |
| `Range` | int | 0 | +0x30 | reach of the attack in world units; the engine adds the held weapon's reach (item field +0x32) |
| `StartForceTime` | int, ms | 0 | +0x34 | when the push/knock-back impulse starts (165..180 in the data) |
| `StartVerticalAngle` | int, degrees | 0 | +0x38 | lower edge of the vertical hit cone (-45 .. -90) |
| `EndVerticalAngle` | int, degrees | 0 | +0x3C | upper edge (45 .. 90) |
| `StartHorizontalAngle` | int, degrees | 0 | +0x40 | left edge of the horizontal arc (e.g. -45, -150); sign convention relative to the facing direction |
| `EndHorizontalAngle` | int, degrees | 0 | +0x44 | right edge (e.g. 45, 160) |
| `GravityLevel` | float | **3.0** | +0x48 | gravity multiplier for jump/air animations (`JUMP_HUM` 6; 8, 10, 30 on landing/knock-down records) |
| `AttackForce` | int | **3000** | +0x4C | horizontal knock-back strength (3000..8000 seen) |
| `VerticalAttackForce` | int | **500** | +0x50 | vertical knock-back strength (0..2000 seen) |
| `SoundScriptName` | string | "" | +0x54 | name of a sound script, `none` in every shipped record (no consumer found: TODO(re)) |
| `EmitterFlag` | int | 0 | +0x75 | `1` = an emitter sub-block follows (§3.2); `0` = none. Other values are stored but the block is *not* read |
| `AttachModel` | model name | – | +0x1D9 | **unused in the shipped file** (0 occurrences). Five tokens: `AttachModel <model>`, `<key2> <bone-name-to-end-of-line>`, `<key3> <one character>`. The model is loaded from `ITEM\<model>` (modelres 0x4B18D0), the second value is stored as bone name (+0x1E1, <= 32 chars, may contain spaces), the first char of the third value (+0x202) is a side tag |

### 3.2 Emitter sub-block (positional, only when `EmitterFlag : 1`)

Immediately after the `EmitterFlag` pair the parser stops matching keys and reads **fixed token pairs** (key text ignored, order fixed):

```
EmitterFlag        : 1
TerrainEmitterFlag : t                       ; t = 0, 1 or 2
repeat 6 times (slot 0..5):
    EmitterFileName        : <name>          ; pscript stem, or none/None
    EmitterStartTime       : ms
    EmitterEndTime         : ms
    EmitterMinSeparateDist : n               ; ONLY if t != 0
    EmitterX               : int             ; stored as float, fraction not allowed
    EmitterY               : int
    EmitterZ               : int
    EmitterLockType        : int             ; byte
    EmitterWeaponType      : int             ; byte
```

- Exactly 6 slots are always present when `EmitterFlag : 1` (shipped file: 197 of 347 records have `EmitterFlag : 1`, 150 have `0` and no further lines).
- Slot semantics: name = particle script in `script\pscript\<name>.txt` (see [pscript.md](pscript.md)) resolved at load
  time through `pemires_find`; `none` (any case) leaves the slot empty. Times are ms relative to animation start (`EmitterEndTime 40000` = for the duration of
  the loop, used by `RUN`). `EmitterX/Y/Z` is the attachment offset in model units. `EmitterLockType`: 0 = world position, 1 = follow the actor, 3 = other (semantics
  *inferred*; values 0, 1, 2, 3 occur — 2 in 92 slots, e.g. `ORLEAN_ATK_B`). `EmitterWeaponType` is 0 everywhere in the shipped file (0 = use the actor, 1 = use the held weapon's attachment, per 0x501190:
  *inferred*).
- Terrain-dependent emitters (`TerrainEmitterFlag` 1 = foot/walk dust, 2 = landing): at load time (0x418C60) the engine resolves `<name>_default<suffix>`
  (suffix literal 0x9CF5F4, presumably `.txt`) and caches the handle in the record; at run time it presumably swaps `default` for the terrain class
  (`_other`, snow, water ... see `walk_s_other.txt`, `landdust_other.txt`, `snowsplash_*.txt`). The terrain variant selection is in 0x41A2A0/0x41B640
  ([../re/functions/particles.md](../re/functions/particles.md)).

## 4. How the engine consumes a record

1. At load: the 347 records are parsed once and all emitter templates preloaded (0x418C60). Memory: 0x21B bytes per record.
2. When an animation with a script starts (0x501190, read for action ids 0x49, 0x22, 0x3A): a timed event is created with `time = now_ms + StartTime` and
   `radius = Range + weapon_reach`; when the timer fires the hit test runs over the arc/cone given by the four angle fields, and targets receive an impulse built from
   `AttackForce`/`VerticalAttackForce` after `StartForceTime`. (The first half is read from 0x501190/0x4EEF00; the angle-cone test and the impulse are *inferred* from the field
   names; the code is behind 0x47E200 and not analysed - TODO(re).)
3. The six emitters are started per slot at `EmitterStartTime` and stopped at `EmitterEndTime`, positioned by `EmitterX/Y/Z` (+ lock type).
4. Preload/unload (0x415300 / 0x4156D0): scenarios preload all records of the animation groups of the units present; unloading decrements the per-record refcount (+0x24).

Units: all times are milliseconds on the game clock (`DAT_00A500BA`, `timeGetTime`-based, see [../re/TIMING.md](../re/TIMING.md)); angles are degrees; distances are
world units (same unit as unit positions and weapon reach).

## 5. Worked examples (from the shipped file)

**`SWORD`** (record 1; the file starts `5B 01 00 00` = count 0x15B = 347, then the entry `SWORD` with payload offset 0x33AC; the payload starts `VERSION 1.0`):

```
VERSION 1.0 / Code : SWORD / StartTime : 400 / EndTime : 500 / Range : 50 / StartForceTime : 165
StartVerticalAngle : -45 / EndVerticalAngle : 45 / StartHorizontalAngle : -45 / EndHorizontalAngle : 45
SoundScriptName : none / EmitterFlag : 0 / END
```
Parsed: start_time 400, end_time 500, range 50, force time 165, ±45° cone both ways, gravity 3.0, attack_force 3000, vertical 500 (defaults), no emitters.

**`HUM_GEN_HOR_ATK_A`** (generic human horse-less light attack): `StartTime 267, EndTime 0, Range 49, StartForceTime 167, vertical -45..45, horizontal -70..30,
AttackForce 5000, VerticalAttackForce 900, EmitterFlag 0` - the hit lands 267 ms (8 frames at 30 fps) into the animation, in a 100-degree arc biased to the right.

**`JUMP_HUM`**: `GravityLevel : 6` (twice the default), `StartTime 40`, `EmitterFlag 0`.

**`RUN`** (terrain emitter): `EmitterFlag 1, TerrainEmitterFlag 1`; slot 0 `EmitterFileName : walk_s, EmitterStartTime 100, EmitterEndTime 40000, EmitterMinSeparateDist 80,
X/Y/Z 0 0 0, LockType 1, WeaponType 0`; slots 1..5 `None`, `100..400`, `MinSeparateDist 80`, `Z 100`. Because `TerrainEmitterFlag` is non-zero every slot has the extra
`EmitterMinSeparateDist` line; the reader resolves `walk_s_default.txt` for slot 0.

**`JOAN_ATK_A`**: `StartTime 267, EndTime 500, Range 74, horizontal -50..30, AttackForce 3000, VerticalAttackForce 0`, `EmitterFlag 1, TerrainEmitterFlag 0`, six slots
(`none`, with `LockType 1`, `3`, `3`, ... on some).

## 6. Round trip

The record text is parsed into `{ int32 time/force fields; float gravity; Emitter[6] }`; to repack the bank keep the text byte-exact, whitespace included ([res.md](res.md) "Reading notes").

## 7. Open points

- TODO(re): consumers of `EndTime`, `StartForceTime`, angles, `GravityLevel`, `SoundScriptName` (see script_res.md §7).
- Closed: 0x41A0E0 name -> recno rules and the duplicate `Code` (§1).
- `AttachModel` path (`ITEM\` models) is unexercised by the shipped data.

## Implementation

Ported in `src/game/animscr.cpp` (`AnimScr::load` 0x00418f30, record parser 0x00419320); the name lookup 0x41A0E0 is called from
`src/game/animgrp.cpp` (animation-group table, row `SCRIPT` -> record number).
