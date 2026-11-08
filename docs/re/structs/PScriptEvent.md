# PScriptEvent (0x23 bytes)

Created by factory 0x677730 from the property keyword after `Initial` / `Fade So At t`. Packed layout. Vtable bodies are now exported and read , so the semantics below are verified unless marked.

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | vtable* | vtable | slot0 deleting dtor (e.g. 0x67AE90 for SIZE -> 0x67AEE0 -> base 0x67D970); slot1 (+4) `apply(PParticle*)`; slot2 (+8) `supports_fade() -> bool` (SIZE returns 1; every class that ramps does); slot3 (+0xC) `parse_value()` | verified |
| 0x04 | 4 | i32 | kind | -1 base; 0 SIZE, 1 COLOR | read |
| 0x08 | 1 | u8 | is_fade | `0x67EF70` returns it. 0 = `Initial` event (sets the value), 1 = `Fade So At` (does not set, only recomputes the ramp) | verified |
| 0x11 | 4 | float | time (s) | position on the particle timeline | verified |
| 0x15 | 4 | PScriptSequence* | owner | set by the parser | verified |
| 0x19 | 2 | i16 | next | 1-based index in the owner's event array of the next event of the **same class** (0 = last); filled by the parser (0x67AD70/0x67ADB0, not exported) | verified (use) |
| 0x1B.. | | | value | class specific: range `{lo(+0x1B), hi(+0x1F)}` floats (SIZE, VELOCITYX/Y/Z, COLORRED/GREEN/BLUE/ALPHA, EVENTTIMER); COLOR: two D3DCOLOR dwords `lo(+0x1B..0x1E), hi(+0x1F..0x22)` (bytes in particle order +0x24..+0x27); VELOCITY: vec6 `x:(+0x1B,+0x27) y:(+0x1F,+0x2B) z:(+0x23,+0x2F)` | verified |

## Apply semantics (all classes share the pattern)
Let `P` be the particle, `E` the event, `N` the event at index `E.next` (if any).
1. If `E.is_fade == 0`: set the property from a random sample of E's range.
2. If `E.next == 0`: rate = 0. Else `target = sample(N's range)`, `dt = N.time - E.time` (if exactly 0.0 use 1.0), `rate = (target - current_value) / dt`, where `current_value` is the property's value after step 1 (for fades: the value reached so far).
Per-frame integration (0x662F60) then does `value += dt * rate`. So a script is a chain of keyframes per property; each segment is a linear ramp whose end value is freshly sampled when the segment starts; the value is not clamped to the target (colour is clamped 0..255 per channel).

| class (ctor / vtable) | property set | rate field | sampling |
|---|---|---|---|
| SIZE 0x67ADE0 / G1 (apply 0x67D9A0, parse 0x67E730 expects `Size = <range>`) | `P.size +0x1C` | `+0x20` | `rand_range(lo,hi)` |
| COLOR 0x67AF60 / G2 (apply 0x67DAA0, parse 0x67E7F0 `Color = RGBA(...)`) | bytes `+0x24..+0x27` | floats `+0x28,+0x2C,+0x30,+0x34` | RGB: `r = rand15 % 256` (0x590540(0x100)); `ch = (lo*r + hi*(256-r)) >> 8` using ONE r for the three colour channels (so the colour moves along the line lo..hi); alpha sampled separately (0x590690). Rate per channel `(target_ch - current_byte)/dt` |
| COLORRED/GREEN/BLUE/ALPHA 0x67B130/0x67B2B0/0x67B430/0x67B5B0 / G3-G6 (apply 0x67DDD0 / 0x67DEE0 / 0x67DFF0 / 0x67E100) | single byte `+0x24 / +0x25 / +0x26 / +0x27` (ftol of `rand_range`) | `+0x28 / +0x2C / +0x30 / +0x34` | `rand_range(lo,hi)` |
| VELOCITY 0x67B730 / G7 (apply 0x67E210, parse 0x67EBB0 `Velocity = XYZ(...)`) | `P.velocity +0x60,+0x64,+0x68` | accel `+0x6C,+0x70,+0x74` | each axis `rand_range` |
| VELOCITYX/Y/Z 0x67B8B0/0x67BA30/0x67BBB0 / G8-G10 | `+0x60` / `+0x64` / `+0x68` | `+0x6C` / `+0x70` / `+0x74` | `rand_range` |
| EVENTTIMER 0x67BD30 / G11 (apply 0x67E6F0) | `P.event_time +0x10 = rand_range(lo,hi)` (a jump on the particle's event timeline; used for looping/rewinding keyframe chains, no rate) | - | |

Note on colour channel names: the stored bytes are copied to the particle verbatim and the particle dword is written to the vertex as the D3DCOLOR built by 0x42A260 (`RGBA(...)` is converted by four `__ftol` + `0x42A260(a,b,c,d)`); COLORRED therefore writes byte `+0x24` = byte 0 of the D3DCOLOR (blue in D3D byte order). The shipped scripts never use the single-channel classes, so this is only a porting caveat: in the port keep the script's RGBA order for the `Color`/`Initial Color` event and map to RGBA8 for the vertex.

## Parser side
0x67D6F0 is the token classifier: first char digit or `-` -> kind 1 (number); `"` -> 8 (string); prefix `COLOR_` -> 0x15; prefix `ALPHA_` -> 0x16 (the values of `ColorOp`/`AlphaOp`, names like D3DTOP constants, handled by 0x67D490/0x67D590); otherwise a binary search (0x38 entries of 8 bytes {name, kind} at 0xA21680) giving the keyword kind, default 0x1F (identifier). 0x67D690 appends such a token to the token array (empty text ignored). Alpha-op matcher 0x67AC40 compares the key with `ALPHAOP` (setter 0x67C930, field +0x130).

## Firing
At spawn (0x681070) all events with `time <= 0` fire. `0x681B10`: from `particle.next_event` while `particle.event_time >= event.time`: apply; if apply changed `event_time` (EVENTTIMER) the scan restarts from index 1 until an event with `time >= event_time`. Because every class of event is chained through `next`, each property has its own independent keyframe chain.
