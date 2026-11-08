# Lights / sky / dynamic grass / lens flare / frame draw — joan.exe (reference build)

Pseudocode: `$JOAN_WORK_DIR/decomp/engine/lights/` (plus `engine/scene/` for the engine light object and `textfmt_loaders/0052f930` for the debug key handler).
Formats: [lgt](../../formats/lgt.md), [sca](../../formats/sca.md), [day](../../formats/day.md), [pro](../../formats/pro.md), [stg](../../formats/stg.md).
Structs: [LightSys](../structs/LightSys.md), [LightStage](../structs/LightStage.md), [ScatterParams](../structs/ScatterParams.md),
[EngineLight](../structs/EngineLight.md), [DynGrass](../structs/DynGrass.md), [Plasma](../structs/Plasma.md), [LensFlareList](../structs/LensFlareList.md).
Design input / algorithms: engine/lights.md.

Status: **verified** = read consistently in >= 2 places or confirmed by file census; **read** = one function read in full; **inferred** = from sizes/usage/strings.
The module-attribution names come from assert strings (`d:\ww\lightsys.cpp`, `plasma.cpp`, `dyngrass.cpp`, `uti3d\lflare.cpp`).
**Important correction:** `0x4D48F0` is *not* a scattering routine and not the 3D draw. It is the per-frame **screen/UI compositor**; the sky scattering
colour is computed in `0x614760` (scene camera render). `docs/re/functions/text_formats.md` row "0x4D48F0 Scatter::apply" should be read as
"references the Rayleigh label strings (debug overlay)".

## lightsys.cpp

| address | proposed name | purpose | evidence | status |
|---|---|---|---|---|
| 0x4A5970 | `lightsys_unload` | frees `stages` (+2) / `special_stages` (+8) / `day_schedule` (+0x4C8), zeroes counts, `lflare_clear`, memsets blocks at +0x198/+0x32C (0x194 bytes each), `0x689D00(0)` (global effect array reset), removes + deletes `sky_actor` (+0x5AC), zeroes weather state +0x594/+0x598/+0x5A4/+0x5A8 | assert strings `lightsys.cpp` | verified |
| 0x4A5BF0 | `lightsys_load_common(base)` | opens `<DAT_009CF818><base>.lgt` and `<...>.sca` (literals 0x9D46E4 / 0x9D46DC); header `LightNum`->+0x5B0, `StageNum`->+0; per stage: `light_count`, lights, `0x617EE0` defaults, `.sca` 11 fields, normalise `sun_dir` | formats lgt/sca | verified |
| 0x4A6840 | `lightsys_load_special` | identical, fixed base `"special"`; stores count at +6 and array at +8 | | verified |
| 0x4A7450 | `lightsys_load_day(world_name)` | `<world>.day`: `StageCount`->+0x4C4, rain/snow floats (+0x58C.. , percentages divided by 100.0 at 0x9D03C0), three strings (+0x54C SkyMesh, +0x4CC, +0x50C), schedule of `{i16 stage, f32 start, f32 duration}` (10 bytes) | format day | verified |
| 0x4A9120 | `lightsys_find_stage(name)` | 1-based index in `stages` by case-insensitive compare (0x7C07B0), 0 if absent | | verified |
| 0x4A9530 | `lightsys_find_special(name)` | same in `special_stages` (+6/+8), returns -1 if absent; used with `"Lightning"` | | verified |
| 0x617EE0 | `scatter_params_set_defaults(ScatterParams*)` | writes 24 default dwords (see ScatterParams.md); `this` is lost by the decompiler, size 0x60 matches the stage tail | | inferred (this) |
| 0x4A5020 | `lightsys_init(world_name)` | world start: unload; load `common`, `special`, `<world>.day`; find the schedule entry with `start <= DAT_00A47BB4` (game minute) -> `day_current`, `0x42E160(stage)` (snaps the stage in); create one [EngineLight](../structs/EngineLight.md) per light (`0x5EB5D0` alloc, `0x5C58A0(type)` ctor, `yaw <- h_dir (+0x64)`, `pitch <- v_dir (+0x60)`, colour `<- 0x42A260(R,G,B,A)` truncated, `enabled <- is_on`, `0x5C5C00` dirty; directional lights also get `+0x54 = 1` and `0x5ED9D0` scene attach); roll rain/snow start times (`rand(100) < probability`); set up the lens-flare list | verified |
| 0x4A7F70 | `lightsys_update` | per-frame stage blend (algorithm below): advance delta block A and weather block B into the current stage, detect schedule transitions, start/stop rain and snow | verified |
| 0x4B21A0 | `lightsys_begin_transition(stage_index, duration)` | clear delta block A, defaults, `remaining = duration`, `factor = 1 - weather_intensity`, delta = (target - current) / duration for light RGBA/v_dir/h_dir and all scatter fields | verified |
| 0x4C02B0 | `lightsys_begin_weather(special_index, minutes, intensity)` | same for the weather overlay block B from a special stage (colours + scatter, no directions) | verified |
| 0x4A9940 | `lightsys_end_weather(kind)` | `0x4A9AA0(kind)` + `0x68AB40(kind)` (stop the particle/sound effect of that weather) | read |
| 0x4956A0 | `clock_elapsed_since(prev)` | `now - prev`, plus the day length when negative (midnight wrap) | verified |
| 0x4A7980 | `lightsys_draw_sky_night` | per-frame before the world render: if `DAT_00A48CCC && DAT_00A5B9F8 != 4`, compute the moon position `camera + normalize(sun_dir) * [0x9D3778]`; when `sun_dir.z >= 0` create the "moon01" billboard (100 x 100) once, then move it | read |
| 0x4A7BA0 | `lightsys_draw_sky_post` | after the world render: same moon logic; when `sun_dir.z < 0` (day) delete the moon and run the **sun lens-flare test and draw** (engine/lights.md section 9) | read |
| 0x61D200 | `scatter_params_apply(sky, ScatterParams*)` | `memcpy(sky + 0x20, params, 0x60)`; called by `0x5EBAC0` with the live block 0xA47F40 or special stage "Lightning" | verified |
| 0x6351A0 / 0x635260 / 0x635D40 | `Billboard::ctor` / `create(x,y,z,w,h,texname,...)` / `set_position` | 0x37-byte world-space quad sprite (plane type 4); used for the moon | read |
| 0x4A8C20, 0x4A65D0, 0x4A8F20, 0x4A71F0, 0x4A9180 | (not lightsys) | in the address range but unrelated unit/AI helpers (unit-level lookups, relation score, idle-animation upkeep); no relation to lighting | read |

## Engine light objects (D3D8 lights)

| address | proposed name | purpose | status |
|---|---|---|---|
| 0x5C5C30 | `EngineLight::rebuild_d3d` | fills D3DLIGHT8 (+0x5E) from type/colour/intensity/orientation; clears dirty | verified |
| 0x5C6110 | `EngineLight::enable` | ambient -> registry + `SetRenderState(AMBIENT)`; else alloc slot, `SetLight`, `LightEnable(TRUE)` | verified |
| 0x5C6250 | `EngineLight::disable` | inverse | verified |
| 0x5C65E0 | `ambient_registry_sum` | sums R,G,B bytes of registered ambient lights, clamp 255, alpha 255 | verified |
| 0x5C6070 / 0x5C6790 | `EngineLight::is_usable` / `is_disabled` | `+0x58 != 0 && +0x59 == 0` and type-specific checks (spot/point need parent matrix) | read |
| 0x5C63C0 / 0x5C6560 | `EngineLight::assign` / `equals` | copies type + 0x4E bytes at +0x0C; equality = type + memcmp | read |
| 0x5F8030 / 0x5F80F0 | `d3d_light_alloc_index` / `_free` | first free device light index (`GetLightEnable`, vtable slot 47); free is empty | read |
| 0x5E9130 | `color_bytes_to_colorvalue(bytes, out, intensity)` | `out = {r,g,b} * intensity/255, a/255` (1/255 at 0x9D97D8) | verified |
| 0x5E91E0 | `color_bytes_to_d3dcolor` | repack `{b0,b1,b2,b3}` -> `b3<<24 | b0<<16 | b1<<8 | b2` | verified |
| 0x5E93B0 | `color_bytes_to_u32` | little-endian repack of 4 bytes | verified |
| 0x5C0E50 | `scene_fog_enable` | sets flag +0x44 and `SetRenderState(D3DRS_FOGENABLE=0x1C, obj+0x45)` | read |
| 0x616C10 | `scene_set_sky_fog_color(rgba[4])` | stores colour in camera +0x74..0x77 (BGRA), calls `0x5E9610(argb)` (stores the colour in the scene object), `0x5C0C30(fog, +0x60, +0x64)` and `0x5C0A70(fog, +0x68..+0x70, camera pos)` | verified |
| 0x5C0C30 | `Fog::set_range(colour, start, end)` | `this+0x48 = start`, `+0x4C = end`; sets dirty `+0x44` when they or the mode (`*this`, 0x5C0980) changed; on a mode change resets `+0x3C = 0`, `+0x40 = colour` | read |
| 0x5C0A70 | `Fog::set_table(colour, x, y, z, near, camera xyz, far...)` | caches per-camera fog parameters (`+0x14/+0x18/+0x1C`, `+0x20/+0x24` near/far, `+0x28` = distance camera->target); invalidates (`+0x0C = 0`) when the camera moved by a relative step `> _DAT_009D9560` or the range changed | read |
| 0x5E9610 | `Scene::set_fog_color(d3dcolor)` | `*this = colour` | verified |
| 0x5C6710 / 0x61C250 | `EngineLight::d3d_index` / `Plane::set_light_index` | returns `+0xC6`; stores it into `+0x18` of the plane (used per sector so the plane is lit by that D3D light) | verified |

## Scene camera render (sky colour part) — `0x614760`, `0x5EBAC0`, `0x6141A0`

| address | proposed name | purpose | status |
|---|---|---|---|
| 0x614760 | `camera_render` | one 3D view: matrices, **sky/fog colour from scattering**, Clear, BeginScene, `0x5EBAC0` passes, EndScene | read (full) |
| 0x6141A0 | `camera_update_matrices` | if dirty (+0xEB): view = inverse of world, projection (perspective if `+0x28==0`: FOV `+0xC0`, aspect `+0x20/+0x24`, near `+0x44`, far `+0x48`; orthographic if `+0x28==1`), viewport (0x18 bytes at +0x2AE), clip matrices | read |
| 0x5EBAC0 | `scene_draw_passes(scene, camera)` | sky follows camera, `0x61D200(scatter)`, sector sort/portal visibility, stencil states, opaque pass 1, effects, transparent pass 2 back-to-front | read (summary only) |
| 0x6140A0 | `camera_get_viewport(out10)` | copies 10 dwords at camera +0xA0 (saved viewport/transform block) | verified |
| 0x631D50 | `camera_ready` | thunk of `0x614630`: viewport and buffers valid and the device is not lost | read |
| 0x631D80 | `scene_render_and_present_end` | `0x614760` then `0x631DC0` | verified |
| 0x631DC0 | `scene_frame_end` | for layers 1..10 (`0x5EA6E0`) runs per-layer cleanup (`0x5CD2D0`, `0x5EFF10`), clears +0x4CE | read |
| 0x6BBC50, 0x720BC0 | `render_viewport_widget` | wrappers that call `0x614760` for secondary 3D viewports (portrait / mode 2,3,5,9 panels) | read |

## dyngrass.cpp

| address | proposed name | purpose | status |
|---|---|---|---|
| 0x4641A0 | `DynGrass::init(capacity, enabled)` | clamp capacity to >= 100, allocate two `capacity*0x66` arrays, init three vectors | verified |
| 0x464390 | `DynGrass::shutdown` | `set_enabled(0)`, free arrays, release the model slots (`0x425CE0`/`0x4A3E30`), zero counts | verified |
| 0x4644D0 | `DynGrass::resize(capacity)` | if `capacity >= current`: realloc both arrays (`0x58EB30`); else just lower the stored value | read |
| 0x464600 | `DynGrass::set_enabled(flag)` | `flag != 0`: store; `0`: clear +4, free every built mesh (+0x28) and reset `active_count` | verified |
| 0x466560 | `DynGrass::append_instance(actor, entry, world_matrix)` | builds/extends a merged `Plane` (type 8, 0x28-byte vertices) with one grass model instance, per-vertex weight, budget check | read |
| 0x425CE0 | `itemres_record(id)` | `*(table+0x14) - 0x2BF + id*0x2BF` (1-based 703-byte record) | verified |
| 0x4A3E30 | `itemres_release(rec, lod)` | refcount-- on record (+0xC) and per-LOD (+0x9B+lod*4), unload at 0 | read |
| 0x60F4B0 | `vertex36_to_vertex40` | converts a 0x24-byte plane vertex to the 0x28-byte type-8 layout: position, `w = 1.0` at +0x0C, then normal/uv words shifted by one | verified |
| 0x5CD7F0 / 0x5CE6F0 | `Plane::reserve_vertices(n)` / `reserve_faces(n)` | grow arrays (vertex stride by type: 1:0x24, 2:0x3C, 3:0x1C, 4:0x18, 5:0x48, 6:0x14, 7:0x48, 8:0x28; faces 6 bytes + 2 bytes material), return old count | verified (see [Plane](../structs/Plane.md)) |
| 0x5CF920 | `Plane::mark_dirty(flags)` | `|= flags` into +0x8C; flags 4 frees cached buffers (+0xB0/+0xB4); sets +0x0A | read |

## plasma.cpp (height grid) — see [Plasma](../structs/Plasma.md)

| address | proposed name | purpose | status |
|---|---|---|---|
| 0x4C56F0 / 0x4C57B0 | `Plasma::Plasma(const&)` / `operator=` | deep copy | verified |
| 0x4C58A0 | `Plasma::create(w,h,fill)` | allocate `(w+1)(h+1)` i16 and fill | verified |
| 0x4C5960 (0x4C5870 thunk) | `Plasma::free` | | verified |
| 0x4C5A20 | `Plasma::set(x,y,v)` | | verified |
| 0x4C7B70 / 0x4C7100 | `Plasma::sample_flat` / `sample_smooth` | terrain height sampling | read |

## uti3d/lflare.cpp — see [LensFlareList](../structs/LensFlareList.md)

| address | proposed name | purpose | status |
|---|---|---|---|
| 0x6626A0 | `lflare_clear` | release shared texture + free element array | verified |
| 0x662640 | `lflare_setup(a, b, c, cx, cy)` | takes the `"LFlare"` texture-set reference (0x662D20), stores header fields | verified |
| 0x6628A0 | `lflare_draw(x, y, intensity)` | per element: copy (0x662CB0), compute alpha/size/offset, reject if the quad is off-screen, `0x633500` draw at layer 9 | read |
| 0x662710 | `lflare_add(colour, size, position, name)` | append one 0x19B-byte element | verified |
| 0x662800 / 0x662830 | `String::init` / `Element::copy` | element helpers | read |
| 0x662DB0 | `lflare_release_shared` | refcount (`DAT_00A6A270`) / texture handle (`DAT_00A6A26C`) | read |

## Per-frame draw / debug overlay — `0x4D48F0` and neighbours

| address | proposed name | purpose | status |
|---|---|---|---|
| 0x4D48F0 | `frame_draw` | screen compositor: dispatch on screen mode (obj +0x3A), 3D viewports via `0x614760`, video playback overlay, HUD panels, debug text, scattering debug panel. Pass order in engine/lights.md | read (full) |
| 0x600DF0 | `frame_stats_reset` | memset 0x2C bytes at +0x21B (11 u32 counters), clear dyn array | read |
| 0x600E50 | `frame_stats_roll` | copy counters to +0x247, accumulate into +0x273, ++frame count +0x2CB; every 3000 ms (`DAT_00A500B2` = ms clock) average into +0x29F, reset | read |
| 0x45B9D0 | `draw_message_log(layer)` | translucent panel (colour 0x5A000000) plus text lines | read |
| 0x70D670 | `ui_layout_init` | per-frame UI animation/keyframe constants (0x61E020 / 0x61F1B0 calls), clears flags +0x3412..0x341D | read |
| 0x6F97D0 / 0x6F98A0 | `maingame_ui_load` / `_release` | load or free the `MAINGAME`, `MGEXP` UI sprite sets (`0x6099C0(name,1,0)`, `0x60A550`) | read |
| 0x60A870(age) / 0x60A990(budget) | `uiset_cache_age` / `texture_cache_trim` | resource-cache housekeeping each frame | read |
| 0x685A80(ms) | `video_pump` | returns 0 none, 1 running no frame, 2 frame ready, 3 finished, 4 frame + finished (see engine/video.md) | read |
| 0x686160 | `video_stop` | release audio/texture/COM objects, clear active flag | read |
| 0x472D10 | `autosave_request` | string `AUTOSAVE`; called at the end of the frame when `DAT_00A58F96` is set in screen mode 9 | inferred |
| 0x52F930 | `debug_key_handler` | (in `textfmt_loaders/`) debug hot-keys incl. the scattering sliders; key 0x23 toggles `DAT_00A39A14`, 0x3D toggles stats `DAT_00A3A2AC`, 0x3B calls `DynGrass::set_enabled(DAT_00A45D34 == 0)`; slider values copied to the live scatter globals each frame | read |

UI drawing primitives used by `0x4D48F0` (documented by the ui2d family): `0x42A260` colour factory (r,g,b,a), `0x6255D0` draw_text
(x, y, font, text, colour, shadow..., scale x, scale y), `0x632A70` fill_rect, `0x6332B0` blit image, `0x629BD0` / `0x622CC0` widget draw.

## Cross-module requests
1. **scene**: `0x614760` camera_render and `0x5EBAC0` scene_draw_passes should be documented there; this page only covers the sky colour part. Name the globals
   `g_device` = `DAT_00A52428` (IDirect3DDevice8*), `g_scat_live` = `0xA47F40` (ScatterParams), `g_lightsys` = `0xA47E08`.
2. **ui2d**: confirm the primitive names above.
3. **gameloop**: `0x470D40` (loads dynamic grass) and `0x471010` (shutdown) feed `DynGrass`; screen-mode byte values (obj +0x3A: 9 = in-game, 2 = 3D menu scene) need an enum.
4. **video**: statuses of `0x685A80` as listed.
5. `docs/formats/day.md` quotes struct offsets (+0x1388, +0x1630, ...) that do not match the Steam build (+0x4C4 count, +0x58C/+0x590 rain, +0x59C/+0x5A0 snow, +0x54C/+0x4CC/+0x50C strings, schedule +0x4C8); `TODO(re)`: reconcile. Also `text_formats.md` row 0x4D48F0 (see top). Stage meaning: each `.day` entry's stage values are the **target at the end** of that entry (to be noted in day.md/lgt.md).

## Open questions (need another decompile / trace)
Resolved (`decomp/engine/lights2/`): stage selection/blend (0x4A7F70, 0x4B21A0, 0x4C02B0), engine-light creation (0x4A5020; `h_dir`->yaw, `v_dir`->pitch,
`alpha`->light colour alpha), fog set-up, scatter apply, lens-flare set-up/draw/occlusion, all `.rdata` constants (see engine/lights.md).
Still open:
- `TODO(re)` constants `0x9D3778` (moon distance), `0x9D9560` (fog cache tolerance), `0x9CF5E8`, `0x9D0338`, `DAT_00A47BC4`, the string literals at `0x9D4710` / `0x9D4708`
  (read as `"Rain"` / `"Snow"`, the special-stage names; confirm) and what the particle effect ids `0x689FB0(1|2)` do beyond rain/snow.
- `TODO(re)` the exact screen-position formula inside `0x6628A0` (float math lost in the decompile; read as `centre + position * (sun - centre)`), blend mode `0x633500(.., 4,3,3,1,..)`.
- `TODO(re)` grass: no placement, wind, billboard or distance-fade code exists in the exported range; the caller of `0x466560` (append_instance) is only reachable through scene code
  not in the export. The vertex program for type-8 planes (probably `hardware_skin.nvo`-like with the blend weight) is the next place to look.
- `TODO(re)` identity of the `0x68C570` array entries (special "Lightning" stage selector); which D3D light cap / ambient-array capacity (`0xA6785C`) applies.
- `TODO(re)` caller that sets `DAT_00A47BB4` (game minute) and the clock object used by `0x4956A0`.
