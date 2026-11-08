# Actor (per-object instance, `d:\ww\en3d\actor.cpp`)

Instance of a model in the world: points to a shared [ActorDef](ActorDef.md), owns its animation playback state and morph list. Packed (`#pragma pack(1)`) like the rest of en3d.
Offsets below come from the destructor/reset `0x5AAF10` (this in ECX), the morph adders `0x5ADF50` / `0x5AE0C0`, the batcher `0x5B1180` (`param_2`) and the
pose/LOD selector `0x5AD060` (`param_1`, listed under the ui2d export; context only). **sizeof(Actor) = 0x10C** (ctor `0x5AAB20`: `memset(0x10C)`, `scale = 1.0`, `enabled(+0x10A) = 1`; then `actor_init_model` `0x5AAB70` builds the instance, see functions/actor.md). The owner is a game-side object
holding the Actor at its `+0x28` (`0x675B30` calls `0x5AAF10` then the deleting dtor `0x420840`).

| offset | size | type | name | meaning | status |
|---|---|---|---|---|---|
| 0x00 | 4 | ActorDef* | `def` | shared definition; for `kind` 1/2/3 it is released by the deleting dtor 0x4B0C20 -> 0x5B03D0 | verified (0x5AAF10, 0x5AD060 `*param_1`) |
| 0x04 | 4 | AnimDef* | `anim` | current animation set; set by `actor_play` (0x5ABE30), NULL = static; tested at 0x5AD060 entry | verified (0x5ABE30) |
| 0x08 | 4 | i32 | `owner_type` | type code of the owning game object (seen: 1, 6..9, 10, 0xB, 0x27; spawn code stores 0xB); **-1 = no owner (UI preview etc.) -> time source `DAT_00A500B2`** (speed-scaled game time), otherwise `DAT_00A500BA` (real elapsed ms) - per [TIMING.md](../TIMING.md). Also gates `== 10` (far-distance cull) and `== 0x27` special cases | verified (0x52B970, 0x5AC310, 0x5B1180) |
| 0x0C | 4 | void* | `owner` | owning game object (position updates are mirrored into it by `actor_set_position`) | verified (0x52B970, 0x5AC310) |
| 0x28 | 4 | f32 | `scale` | uniform scale, ctor default 1.0; multiplies the rotation matrix (0x5AC6A0) and the bounding-sphere radius (0x5ACC80) | verified |
| 0x2C | 4 | u32 | `skin_variant` | texture/team variant id set by 0x5ADD40 (per-plane texture override, blend field `= id | 0x1000000 | slot<<16`) | read |
| 0xFD | 1 | u8 | `flag_fd` | set 1 by init; meaning TODO(re) | read |
| 0xFE / 0x102 / 0x106 | 4+4+4 | f32 x3 | `yaw` / `pitch?` / `roll?` | Euler angles fed to `0x60BF40` by `actor_set_rotation` (0x5AC6A0; `+0xFE` = the argument); packed floats | read |
| 0x138 | 4 | i32 | `fx_node` | zeroed by init (`param_1[0x3E]`; also written by 0x5AE590 as a mode flag) | inferred |
| 0x10 | 4 | i32 | `kind` | 0 none, 1 = owns `def` and the polymorphic object at `+0x14`, 2 and 3 = owns `def` only | read (0x5AAF10) |
| 0x14 | 4 | obj* (virtual dtor) | `extra` | kind 1 only; destroyed through vtable slot 0, also via 0x6356C0 | read |
| 0x18 | 4 | List* | `owner_list` | intrusive list head `{.. +0xCA first, +0xCE last}` this actor sits in (unlinked by 0x5EFDB0); `+0x20` prev, `+0x24` next | read |
| 0x30 | 4 | f32 | `fade_rate` | 0.0 = no fade (the batcher only accepts actors with `== 0.0`, `_DAT_009CF5C4 = 0`). Non-zero = fade-in (>0) / fade-out (<0): `0x5AB6E0` forces all sub-meshes of the model to blended materials (`+0x34 1->0x20010`, `+0x38 -> 1 or 9`, `+0x40 = 2`, `+0x20 |= 0x100`) and writes a diffuse alpha = `ftol(...)` clamped to `[0, fade_max]`. Only active while `DAT_00A500BE != 0` and `elapsed_ms <= 1000.0 / |fade_rate|` (`0x9D0338 = 1000.0`; the `0x7B1F6B` call is taken as `fabs`): i.e. the fade lasts `1000/|rate|` ms. Alpha (inferred, the FPU value is not shown): `a = elapsed * rate * fade_max / 1000`; fade-in (rate>0): `min(a, fade_max)`; fade-out (rate<0): `max(fade_max + a, 0)`; written as diffuse alpha `(255,255,255,a)` | read (0x5AB6E0, 0x5B1180) |
| 0x34 | 4 | i32 | `fade_max` | alpha ceiling (0..255) for the fade | read (0x5AB6E0) |
| 0x38 | 4 | u32 | `fade_start` | clock when the fade started | read |
| 0x3C | 4 | i32 | `slot` | 1-based handle into a `dynarrb` table (0x41BD60 get, 0x41BD90 "slot free"); that entry's `+0x12` back-points to this actor and is cleared on destroy | read |
| 0x44 | 4 | i32 | `clip` | **1-based clip index** in the AnimDef (clip record = `clips + (clip-1)*0x41`) | verified (0x5B5DD0, 0x5B1180) |
| 0x48 | 2 | i16 | `anim_set` | 0 = use the ActorDef's own AnimDef; else resource id of an override AnimDef (0x5B7650) | verified (0x5B5E40 `param_4`) |
| 0x4C | 4 | i32 | `start_time` | clock value (ms) at which the clip started; pose time = `clock - start_time` | verified (0x5B1180, 0x5AD060) |
| 0x50 | 4 | i32 | `loop` | 0 = clamp to clip end, non-zero = wrap with `fmod` | verified (0x5B5E40 `param_6`) |
| 0x54 | 4 | i32 | `last_eval` | clock of the last pose evaluation (skip re-evaluation when equal) | read (0x5AD060 `[0x15]`) |
| 0x5C | 4 | obj* | `aux` | destroyed by 0x6ADF70 (dynarrb-backed container) | read |
| 0x78 | 4 | Key*[n_bones] | `blend_from` | **start-pose cache**, `n_bones x 0x20` bytes ([AnimKey](AnimKey.md) layout), freed with `delete[]` | verified |
| 0x7C | 4 | i32 | `blend_on` | non-zero when `blend_from` holds a pose to cross-fade from; (0x5B1180 passes `+0x78` only if this is non-zero) | read |
| 0x80 | 4 | f32 | `blend_ms` | cross-fade duration (`param_9` of 0x5B5E40) | read |
| 0x84 | 4 | AnimDef* | `cursor_owner` | AnimDef the `cursors` array belongs to (cursors are only reused when equal) | verified (0x5B5E40 `*param_10 == this`) |
| 0x88 | 4 | i32[n_bones] | `cursors` | per-bone key-search cursor (hint for 0x5B7C70), freed with `delete[]` | verified |
| 0x8C | 4 | Frame* | `world_frame` | the actor's **root [FrameNode](FrameNode.md)** (position/orientation in the world; deleting dtor 0x5AB400 = unparent). For baked-frame (vertex animation) draws `0x5ABA20` parents the model's frame 0 under it for the draw and unparents afterwards; `0x5C5C30` shows how a frame-owning object builds its matrix from yaw/pitch/roll (`0x60BD00`) + translation. There is **no attachment-point table**; attaching one thing to a bone = `set_parent(bone_frame)` | verified (0x5AAF10, 0x5ABA20) |
| 0xA7 | 1 | u8 | `visible` | result of the last culling test for this frame (`0x5AB470` pass 1) | verified |
| 0xA8 | 4 | f32 | `view_depth` | distance of the actor's bounding sphere to the camera plane (`0x615E80` -> `0x60DA30`), used to **sort transparent actors** (pass 2 comparator `0x5F0780`) | read |
| 0x10A | 1 | u8 | `enabled` | actor takes part in rendering (0 -> culled/never drawn) | verified (0x5AB470, 0x5AB620) |
| 0x90 | 4 | Model* | `model` | the actor's own 0x11C [Model](ModelMesh.md) (LOD 0); its `+0x0A` byte must be 0 for batching; deleting dtor 0x459C40 | verified |
| 0x94 | 4 | Model* | `last_model` | model chosen by the last 0x5AD060 call | read |
| 0x9C | 4 | i32 | `last_lod` | LOD used at the last evaluation | read |
| 0xAC | 4 | i32 | `morph_cap` | capacity of `morphs` (grows by 2) | verified (0x5ADF50) |
| 0xB0 | 4 | i32 | `morph_count` | number of [Morph](Morph.md) objects; the batcher refuses actors with `morph_count >= 1` | verified |
| 0xB4 | 4 | Morph*[] | `morphs` | array of 0x84-byte Morph objects (tracked realloc); each freed by 0x4B0C70 -> 0x5C67F0 -> 0x5C7B80 | verified |
| 0xF8 | 4 | i32 | `hidden` | non-zero -> not drawn / not batched | read (0x5B1180) |

The position is **not** stored in the Actor: `actor_set_position` (0x5AC310) writes `world_frame.local` translation (`frame+0x30..0x38`) and mirrors it into the owner object. Other bytes (+0x1C, +0x58..+0x77, +0x98, +0xA0..+0xAB, +0xB8..+0xFC, +0x100..+0x137) are untouched by the exported functions: TODO(re).
Bounding sphere source: `ActorDef.modelset +0x34..+0x43` = `{cx, cy, cz, radius}` (see 0x5ACC80 in functions/actor.md).
