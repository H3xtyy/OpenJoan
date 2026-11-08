# PScriptSequence (0x162 bytes) - one `EventSequence { ... }` of a ParticleSystem

Heap object `operator new(0x162)`, ctor 0x67EF90, packed (unaligned dword fields). Offsets from the setter/getter functions 0x67BF00-0x67CA60 (each setter writes exactly one field) cross-checked with the spawn/update code (0x681070, 0x67F380, 0x662F60, 0x67FAF0, 0x675E90). Key names are now VERIFIED from the matchers 0x67A5C0-0x67AC00 (each compares the key text, case-insensitive, with a literal; matcher->setter pairing from 0x678320) - this corrects the earlier guesses for SelfRotation/Has_Rotation and the vec6 keys. `Initial Velocity`, `Initial Size`, `Initial Color` and all `Fade` lines are EVENTS (PScriptEvent), not sequence fields; the vec6 at +0x19 is the unused key `SpawnDir`. "range" = {a,b} floats, sampled uniformly (`rand_range`, see functions page) at use; "vec6" = {xa,ya,za,xb,yb,zb}, each axis sampled between its a and b.

| offset | size | type | key (inferred) | meaning / use | status |
|---|---|---|---|---|---|
| 0x00 | 1 | u8 | (active) | 1 after init 0x67F210, 0 after release 0x67F250 | verified |
| 0x01 | 8 | range | LifeTime | particle lifetime in seconds (spawn: `particle.life = rand(0x01,0x05)`) setter 0x67BF00 | verified |
| 0x09 | 8 | range | EmitRate | particles per second (`rand(0x09,0x0D)` each frame, 0x67F380) setter 0x67BF90 | verified |
| 0x11 | 8 | range | (copy of EmitRate) | `EmitRate` (matcher 0x67A600) calls both setters 0x67BF90 and 0x67C020 with the same range; no reader found in the 604+380 exported functions; treat as a redundant copy | verified (write) |
| 0x19 | 24 | vec6 | SpawnDir | spawn velocity per axis `rand(0x19+4i, 0x25+4i)` setter 0x67C0B0, matcher 0x67AB40. Never used in the 266 shipped scripts (they use `Initial Velocity`, a t=0 VELOCITY event) | verified |
| 0x31 | 24 | vec6 | EmitRadius | spawn offset per axis `rand(0x31+4i, 0x3D+4i)` (box, not sphere) setter 0x67C140 | verified |
| 0x49 | 24 | vec6 | Gravity | acceleration; resampled per axis every frame and added to every live particle's velocity (`v += dt*rand(0x49+4i,0x55+4i)`), 0x67F380 setter 0x67C1D0 | verified |
| 0x69 | 1 | u8 | FollowCamera | billboard mode 0..4 (see engine page); 0x67FAF0 switches on it | verified (use), name inferred |
| 0x6A | 12 | vec3 | Orientation | quad normal for modes 0/4 (normalised in draw); also the model orientation for IsSmo; setter 0x67C2A0 | verified |
| 0x76 | 4 | float | OriR | extra roll in degrees: `angle += OriR * DAT_009CF884` (deg->rad) for model particles (0x675E90) setter 0x67C300 | verified (use) |
| 0x7A | 12 | vec3 | Offset | added to every spawn position (`pos = rand(EmitRadius) + Offset`), setter 0x67CA60 | verified |
| 0x86 | 4 | float | TimeDelay | system start delay: emission only when `state.start_ms + TimeDelay <= now_ms` (0x67F380); setter 0x67C360 | verified |
| 0x8A | 1 | u8 | SelfRotation | spin of the billboard about its own axis (particle +0x40/+0x44); matcher 0x67A880, setter 0x67C4E0 | verified |
| 0x8B | 12 | vec3 | SelfRotAxis | axis for SelfRotation (mode 0 uses it directly; others use cross(camera right, up)); matcher 0x67A8C0, setter 0x67C550 | verified |
| 0x97 | 8 | range | SelfRotSpeed | `particle.rot_speed (+0x44) = rand(0x97,0x9B)`; matcher 0x67A900, setter 0x67C5E0 | verified |
| 0x9F | 1 | u8 | Has_Rotation | orbit the particle around Rotation_Center about Rotation_Axis (particle +0x38/+0x3C); matcher 0x67A940, setter 0x67C650 | verified |
| 0xA0 | 8 | range | Rotation_Speed | `particle.orbit_speed (+0x3C) = rand(0xA0,0xA4)`; matcher 0x67A980, setter 0x67C6D0 | verified |
| 0xA8 | 12 | vec3 | Rotation_Center | orbit pivot; matcher 0x67A9C0, setter 0x67C750 | verified |
| 0xB4 | 12 | vec3 | Rotation_Axis | orbit axis (matrix from axis+angle); matcher 0x67AA00, setter 0x67C7D0 | verified |
| 0xC0 | 0x21 | char[] | Model | model file name (`MODEL\<name>`), set by 0x681D10 | verified |
| 0xE1 | 0x21 | char[] | Texture | texture name (looked up with texture loader 0x6099C0) set by 0x681CD0 | verified |
| 0x102 | 0x21 | char[] | (sequence name) | 0x681E10 | verified |
| 0x123 | 1 | u8 | RChange | texture animation: 0 = advance frame sequentially, 1 = random frame, setter 0x67C480 | verified |
| 0x124 | 4 | float | TexturePeriod | seconds per texture frame, setter 0x67C3C0 | verified |
| 0x128 | 4 | i32 | TextureNumber | number of frames (atlas width split into N columns); N<=1 disables animation; setter 0x67C420 | verified |
| 0x12C | 4 | i32 | ColorOp (matcher 0x67AC00, key `COLOROP`; the alpha-op matcher 0x67AC40 is not exported) | ORed into mesh material flags: `flags34 = colorop \| 0x20000 \| alphaop` (0x67FAF0/0x681070); setter 0x67C900; probably a D3DTEXTUREOP | verified (use) |
| 0x130 | 4 | i32 | (alpha op mode, token kind 0x1B) | see above; setter 0x67C930 | verified (use) |
| 0x134 | 4 | i32 | NumParticles | max simultaneously alive particles (emission stops when pool count >= this) AND total emitted when Loop flag set | verified |
| 0x138 | 4 | i32 | Loop | nonzero = one-shot: count emitted particles, when emitted >= NumParticles set state.done. 0 = continuous (CampFire has Loop=0, cannon bursts Loop=1) | verified (use) |
| 0x13C | 1 | u8 | IsSmo | particles are 3D model actors instead of quads, setter 0x67C870 | verified |
| 0x141 | 4 | ptr | model actor template | `new(0x2D8)` object created by 0x681D10 | verified |
| 0x145 | 0x1D | dynarr | events | array (elem 4) of PScriptEvent*, sorted by time, see PScriptEvent.md | verified |

Total 0x162 = 0x145 + 0x1D. Unknown gaps: 0x61..0x68, 0x6A+12..0x75, 0x82..0x85, 0x87..0x89, 0xA4 is in range, 0xBC..0xBF, 0x12C.. fine.

## PScriptSystem / template (partial, from round 1)
`+0x03` name; `+0x2C` vec6 Position; `+0x44` evaluated position sample; `+0x60` u16 SourceBlending; `+0x62` u16 DestBlending; `+0x68` int refcount (incremented by instances that reference the template); `+0x6C` template link; `+0x70` sequence dynarr; `+0x74` PSeqState array. These offsets are the same class as the live instance, see PSystemInstance.md.
