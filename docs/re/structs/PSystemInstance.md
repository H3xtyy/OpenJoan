# PSystemInstance (0x78 bytes) and PSeqState (0x3E bytes)

One class is both the *parsed template* and the *live instance* (ctor 0x6758F0 memsets 0x78; templates are shared by reference through 0x679300).

## PSystemInstance
| offset | size | type | name | meaning | evidence |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | inited | 1 after 0x6759A0 | 0x6759A0/0x675B30 |
| 0x02 | 1 | u8 | place_mode | 1 default; passed as `mode` (param_9) into 0x67F380/0x662F60: 1 = world, 2 = emitter-relative (inverse matrix applied), 3/4 = trail interpolation | 0x675E90, 0x67F380 |
| 0x03 | var | char[] | name | 0x67A1C0 | round 1 |
| 0x20 | 2 | i16 | weather index | used by `snow%d.txt` | 0x68B600 |
| 0x22 | 4 | float? | scale / z base | copied from unit field +0x339 in 0x68A170 | 0x68A170 |
| 0x24 | 4 | Mesh* | plane | dynamic mesh (`en3d::plane`, `new 0x11C`) | 0x6759A0 |
| 0x28 | 4 | Actor* | actor | model actor used for IsSmo particles | 0x675B30 |
| 0x2C | 24 | vec6 | position | `Position` | round 1 |
| 0x44 | 12 | vec3 | position_sample | evaluated emitter position (min/max resolved) | 0x677628 |
| 0x50 | 12 | vec3 | emitter_target | alternative emitter position (transformed) | 0x675E90 |
| 0x5C | 4 | float | | passed to draw (0x67FAF0 param_3) | 0x675E90 |
| 0x60 | 2 | u16 | src_blend | D3DBLEND | verified |
| 0x62 | 2 | u16 | dst_blend | D3DBLEND | verified |
| 0x64 | 1 | u8 | owns_sequences | 1 = owns list (template), 0 = references `+0x6C` | 0x679300 / 0x675B30 |
| 0x68 | 4 | i32 | refcount | on the template | 0x679300 |
| 0x6C | 4 | PSystemInstance* | template | | 0x679300 |
| 0x70 | 4 | dynarr* | sequences | list of PScriptSequence* (elem 4, grow 0x1E) | 0x6759A0 |
| 0x74 | 4 | PSeqState* | states | array [n] of 0x3E, count header at -4 | 0x67A240 |

## PSeqState (0x3E bytes, one per sequence per instance; indexes `states + (i-1)*0x3E`)
| offset | size | name | meaning |
|---|---|---|---|
| 0x00 | 0x2E | particle pool | dynarrb of PParticle* (alloc/free 0x663800/0x663850, `get(i)` 0x663960, `is_free(i)` 0x663910 = slot ptr null or `*slot==0`, count 0x6638B0/0x6638E0) |
| 0x2E | 4 | done | 1 = no more emission |
| 0x32 | 4 | emitted | total emitted count (only counted when sequence.Loop != 0) |
| 0x36 | 4 | start_ms | `DAT_00A500BA` at restart (0x677250) |
| 0x3A | 4 | float | fractional emission accumulator |
Layout of the first 0x2E bytes is a container (not exported); sizes verified by the `0x3E` multiplier in 0x67A240/0x675E90.

## PParticle (about 0x80 bytes)
| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | ptr/u32 | alloc tag | nonzero = slot used (pool alloc 0x663800 sets `*slot = 1`; freed by 0x663850 -> 0). Preserved across init | verified |
| 0x04 | 4 | float | lifetime (s) | `rand(seq.LifeTime)`; init default 1.0 (0x662E20 memsets 0x80 then sets +4=1.0, +0x14=1.0, +0x1C=1.0, +0x24=0xFFFFFFFF white, +0x18=0, +0x38=0) | verified |
| 0x08 | 4 | float | age (s) | += dt; particle dies when age >= lifetime | verified |
| 0x0C | 4 | float | carry (s) | initial partial time = `fmod(rand(0,dt), lifetime)`; added to the first dt then zeroed | verified |
| 0x10 | 4 | float | event time (s) | += dt; event timeline cursor, modifiable by EVENTTIMER events | verified |
| 0x18 | 4 | Actor* | model | IsSmo particle actor | verified |
| 0x1C | 4 | float | size | += dt*size_rate; quad half-extent = size * 0.5 (DAT_009CF77C, presumed 0.5) | verified |
| 0x20 | 4 | float | size_rate | per second | verified |
| 0x24 | 4 | u8[4] | colour | D3DCOLOR dword written verbatim into the vertex (byte0=B, 1=G, 2=R, 3=A) | verified |
| 0x28 0x2C 0x30 0x34 | 4 each | float | colour_rate[4] | per second, for bytes 0x24..0x27 in order | verified |
| 0x38 | 4 | float | orbit_angle (rad) | Has_Rotation orbit around Rotation_Center/Axis | verified |
| 0x3C | 4 | float | orbit_speed | from Rotation_Speed | verified |
| 0x40 | 4 | float | spin_angle (rad) | SelfRotation (billboard spin) | verified |
| 0x44 | 4 | float | spin_speed | from SelfRotSpeed | verified |
| 0x48 | 12 | vec3 | local_pos | integrated position relative to the spawn origin | verified |
| 0x54 | 12 | vec3 | world_pos | emitter origin + local (or integrated directly in mode 0) | verified |
| 0x60 | 12 | vec3 | velocity | integrated; gravity added after update | verified |
| 0x6C | 12 | vec3 | accel | per-particle acceleration set by VELOCITY fade events: `v += dt*accel` | verified |
| 0x78 | 2 | u16 | next event | 1-based index into sequence.events | verified |
| 0x7A | 4 | float | frame_timer (s) | texture animation | verified |
| 0x7E | 2 | i16 | frame | current texture frame | verified |
