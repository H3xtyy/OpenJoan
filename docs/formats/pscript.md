# pscript — particle system scripts (`script/pscript/*.txt`, 266 files)

Reader: directory scan **0x4C1B40** (`SCRIPT\PSCRIPT\*.txt` 0x9D5684); parser **0x677390**
(`PScript::parse`, proposed name, see docs/re/functions/text_formats.md). The decompiled function is only the header: it checks, in order,
script size >= 2 ("This script is too small to be valid."), first word `ParticleSystem` ("First word must be
ParticleSystem."), a quoted name token whose first two and last two characters (quote + pad blank) are stripped ("Must name
particle system."), a number-class token == `"1.00"` by `strcmp` ("Must have version number." / "Version number mismatched.
The program currently supports version ..."), and `{` ("Missing opening brace for ParticleSystem block."). Then it calls
0x677628 for the body (not used here: the body grammar below is derived from the 266 files, all of which parse).

## Grammar (verified against all 266 files)
```
file     := 'ParticleSystem' '"' name '"' version '{' psfield* sequence* '}'      ; version "1.00" in every file
psfield  := key '=' value
sequence := 'EventSequence' '"' name '"' '{' (assignment | fade)* '}'
assignment := key '=' value            ; key = words separated by blanks, up to '=' ("Initial Velocity")
fade     := 'Fade' 'So'? 'At' number key '=' value     ; "So" is present in all 2113 lines (kept as a flag)
value    := number | random(a,b) | XYZ(s,s,s) | RGBA(s,s,s,s) | '"' text '"'      ; s := number | random(a,b)
comment  := '//' … EOL                 ; whole-line comments (`//// Color 0`) are common
```
Keywords are matched case-insensitively. Whitespace (blanks, newlines) is free inside expressions
(`XYZ(random(10.00,-5.00), random(...), ...)`). Names and strings keep the padding blanks inside the quotes
(`" P_Glow_01 "`); `unpad()` trims them. `random(min,max)` bounds are not ordered (`random(10.00,-5.00)`).
Numbers are decimal `d`, `d.dd`, optionally signed; parsed as `double`. Fields stay generic
(`key -> number|Random|Vec3|Rgba|string`, original key spelling and order kept); emission semantics are not specified here.

## `.txt` ambiguity
`.txt` also names the localised text files in `des/` and `item_des/` ([descriptions.md](descriptions.md)). A file whose first
word is not `ParticleSystem` is not a particle script; the original rejects it with "First word must be ParticleSystem."
Tested on a sample of des/ and item_des/ files.

## Census
266 files, all version `1.00`; 567 event sequences (1..n per file); 2113 Fade lines. File start is always
`ParticleSystem " EnterSystemName " 1.00` or a custom name.

System-level keys (all 266 files, always this order): `Position` = XYZ, `SourceBlending` = int, `DestBlending` = int
(D3DBLEND: 5 SRCALPHA, 6 INVSRCALPHA, ...).

Sequence-level keys (count of sequences having it, of 567):

| key | n | value form |
|---|---|---|
| LifeTime, EmitRate, Initial Size, SelfRotSpeed | 567 | random(a,b) |
| EmitRadius, Gravity, Orientation, SelfRotAxis, Initial Velocity | 567 | XYZ (components number or random) |
| Initial Color | 567 | RGBA (0..255, random or number) |
| FollowCamera, Has_Rotation, SelfRotation, NumParticles | 567 | int |
| Loop | 562 | int |
| Texture | 496 | string (texture name, padded) |
| Model | 71 | string (smo name, padded) |
| IsSmo | 534 | int |
| Offset | 534 | XYZ |
| OriR | 534 | number |
| TimeDelay | 525 | number |
| RChange, TextureNumber, TexturePeriod | 457 | number (texture-animation) |
| Rotation_Axis, Rotation_Center | 30 | XYZ |
| Rotation_Speed | 30 | random(a,b) |

Fade forms (`Fade So At <t> <key> = <value>`):

| key | value form | n |
|---|---|---|
| Color | RGBA(...) | 1060 |
| Size | random(a,b) | 911 |
| Size | plain number | 3 |
| Velocity | XYZ(...) | 139 |

## Open questions
- Meaning of `OriR`, `RChange`, `TimeDelay`, `TexturePeriod` (body parser 0x677628 / emitter).
- Whether `Fade` times are fractions of lifetime (all observed in the data are numeric; range not yet tabulated).
