# Sound3D manager, Sound3DSource, Sound3DBuffer, Sound3DListener (ilib\sound3d.cpp) — joan.exe

Thin OpenAL 1.0 (Creative) object layer. Offsets of the manager are partly inferred because `this` arrives in ECX through thunks.
Functions: [audio.md](../functions/audio.md).

## Sound3D (manager)

| offset | size | type | name | meaning | evidence | status |
|---|---|---|---|---|---|---|
| 0x00 | 1 | u8 | `distance_model` | 0 none, 1 inverse, 2 inverse-clamped | 0x454000 (`*param_1 = m`) | R |
| 0x0A | 4 | f32 | `doppler_factor` | init 0.0 | 0x5A06F0 | R |
| 0x0E | 4 | f32 | `doppler_velocity` | init 343.0 | 0x5A0790 | R |
| 0x12 | 4 | void* | `sentinel` | non-null = initialised (10-byte tracked alloc) | 0x5A1110, 0x5A0330 | V |
| 0x16 | 2 | i16 | | zeroed on shutdown | 0x5A04E0 | I |
| 0x18 | 4 | Sound3DBuffer** | `buffers` | array sorted by ALuint, 1-based binary search | 0x48D120, 0x490BC0 | V |
| 0x1C | 2 | i16 | `n_buffers` | | 0x48D120 | V |
| 0x1E | 2 | i16 | `cap_buffers` | grows/shrinks by 0x20 | 0x48D120, 0x490BC0 | V |
| 0x20 | 4 | Sound3DSource** | `sources` | sorted by ALuint | 0x4907E0 | V |
| 0x24 | 2 | i16 | `n_sources` | | 0x4907E0 | V |
| 0x26 | 2 | i16 | `cap_sources` | grows/shrinks by 0x20 | 0x4907E0, 0x5A0CF0 | V |
| 0x28 | 4 | Sound3DListener* | `listener` | 0x35-byte object | 0x5A0330 | V |

## Sound3DSource (sizeof 0x53; `+0` = ALuint)

| offset | type | name | default | AL parameter | status |
|---|---|---|---|---|---|
| 0x00 | u32 | `al_source` | from `alGenSources` | | V |
| 0x04 | f32 | `pitch` | 1.0 | AL_PITCH 0x1003 | V |
| 0x08 | f32 | `gain` | 1.0 | AL_GAIN 0x100A (sent as 0 if `muted`) | V |
| 0x0C | f32 | `min_gain` | 0.0 | AL_MIN_GAIN 0x100D | V |
| 0x10 | f32 | `max_gain` | 1.0 | AL_MAX_GAIN 0x100E | V |
| 0x14 | f32 | `cone_outer_gain` | 1.0 | 0x1022 | V |
| 0x18 | f32 | `cone_inner_angle` | 2*pi rad | 0x1001, degrees = rad / (pi/180) | R |
| 0x1C | f32 | `cone_outer_angle` | 2*pi rad | 0x1002 | R |
| 0x20 | f32 | `max_distance` | 1.0e7 | AL_MAX_DISTANCE 0x1023 | V |
| 0x24 | f32 | `reference_distance` | 1000.0 | AL_REFERENCE_DISTANCE 0x1020 | V |
| 0x28 | f32 | `rolloff_factor` | 1.0 | AL_ROLLOFF_FACTOR 0x1021 | V |
| 0x2C | u8 | `muted` | 0 | forces gain 0 | R |
| 0x2D | u8 | `looping` | 0 | AL_LOOPING 0x1007 (0x59FA80) | V |
| 0x2E | u8 | `relative` | 0 | AL_SOURCE_RELATIVE 0x202 (0x59FAE0) | V |
| 0x2F | 12 | vec3 | `position` | 0 | AL_POSITION 0x1004 (0x59FB40) | V |
| 0x3B..0x52 | | | | velocity/direction (setters not seen) | I |

## Sound3DBuffer (sizeof 0x11A)

| offset | size | name | meaning | evidence |
|---|---|---|---|---|
| 0x000 | 4 | `al_buffer` | ALuint | 0x48D120 |
| 0x004 | 2 | `refcount` | number of source queues holding it; decremented in 0x59F4B0 | V |
| 0x006 | 1 | `loaded` | set by `alBufferData` upload (0x59F1A0); +7 cleared | |
| 0x10C | 4 | `al_format` | 0x1100 mono8, 0x1101 mono16, 0x1102 stereo8, 0x1103 stereo16 | 0x59F1A0 |
| 0x110 | 4 | `size` | PCM bytes | 0x59F1A0 |
| 0x114 | 4 | `freq` | Hz | 0x59F1A0 |
| 0x119 | 1 | `auto_delete` | destroy when refcount hits 0 | 0x59F4B0 |

## Sound3DListener (sizeof 0x35)

| offset | type | name | meaning | status |
|---|---|---|---|---|
| 0x00 | f32 | `gain` | init 1.0 | R |
| 0x04 | u8 | `muted` | gain forced to 0 | R |
| 0x05 | 12 | vec3 | `position` | AL_POSITION (0x59FF70) | V |
| 0x1D | 12 | vec3 | `forward` | AL_ORIENTATION first 3 floats (0x5A0050) | V |
| 0x29 | 12 | vec3 | `up` | AL_ORIENTATION last 3 floats | V |
No listener velocity is ever set (doppler is off). Pose struct used by 0x5A0110 (0x28 B, init 0x616D50): `+4 vec3 origin`, `+0x10 yaw`, `+0x14 pitch`, `+0x1C distance (init 100.0)`, `+0x20/+0x24 = 1.0`.
