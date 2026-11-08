# particles - `pemires.cpp` + particle-script parser (joan.exe)

Source: `$JOAN_WORK_DIR/decomp/engine/particles/` (call tree depth <= 2 from 0x4C1B40 and 0x677390).
Text grammar: `docs/formats/pscript.md` (not repeated).
Structs: `docs/re/structs/PEmiResEntry.md`, `PScriptToken.md`, `PScriptSequence.md`.

**Scope.** Round 1 covered the enumerator and parser; round 2 (`decomp/engine/particles2/`, 604 functions) added the
runtime: "Runtime" section below. Still not exported: the event vtable methods (anonymous `LAB_` at 0x9DC580..0x9DCAA0
data), the numeric/vector key *matchers* (0x67A5C0..0x67AC40; keys are inferred from setter offsets + data files), the
token classifier 0x5A18D0, the mesh/plane layer (`en3d::plane`), and the float constants (see engine page "Unresolved constants").
In round 2 the INDEX depth-0 rows 0x689000-0x68C800 are the **weather** module (rain/snow/ripple/thunder, uses pscript
templates) and are summarised separately; rows 0x6891D0-0x689B10 (array helpers) are generic containers.

## Resource table (`pemires.cpp`)

| address | proposed name | purpose | evidence | conf. |
|---|---|---|---|---|
| 0x4C1B40 | pemires_scan_scripts | `this` = table {i32 count; PEmiResEntry* array}. Pass 1: `_findfirst/_findnext` over `SCRIPT\PSCRIPT\*.txt` counting files (also tracks max file size, unused). Allocates `count*0x28` via tracked malloc (file `d:\ww\pemires.cpp`). Pass 2: for each file `count++`, memset entry 0x28, store `u16 +0x22 = 1-based index`, strcpy file name to entry+0. No parsing happens here: scripts are only enumerated, the parse is lazy/elsewhere. | strings `SCRIPT\\PSCRIPT\\*.txt`, `d:\\ww\\pemires.cpp` | high |
| 0x7B3719 / 0x7B37E6 / 0x7B38AE | crt_findfirst / findnext / findclose | CRT `_finddata_t` iteration (size at +0x10, name at +0x14) | layout | high |

## Parser (`0x677390` family)

| address | proposed name | purpose | evidence | conf. |
|---|---|---|---|---|
| 0x677390 | pscript_parse | `(const char* text)`: reset state, tokenise, validate header, parse system props + sequences. Throws C++ `const char*` exceptions (`__CxxThrowException`, type 0x9EFBF8) for every error. | strings | high |
| 0x677E00 | pscript_reset_all | iterate global dynarr of loaded systems (count via 0x417AD0, element via 0x41B3F0), release each (0x67F250) and delete (0x677EB0), then `dynarr.clear(free)` | read | med |
| 0x67A200 | pscript_set_status | copy message into global `DAT_00A6A28C` (255 chars + NUL at 0xA6A38B); first set to "0 error(s), you're good to go!" | read | high |
| 0x67A1C0 | pscript_set_name | strcpy name into `this+3` (system name) | read | med |
| 0x67CAA0 | token_array_ctor | token dynarr: elem size 0xC, initial capacity 0x1E; vtable PTR_LAB_009DC6C0 | read | high |
| 0x67CB70 | token_array_dtor | | read | high |
| 0x6840D0 | pscript_tokenize | char-by-char 4-state lexer (below); emits tokens through vtable slot 1 -> `0x684590` -> classifier `0x5A18D0` | read | high |
| 0x4B9D40 | cursor_reset | `cursor(+8) = min(count(+0xC), 1)` (1-based) | read | high |
| 0x67AC80 | tok_peek | current token (`4B9C30`: NULL if cursor 0 else `data + (cursor-1)*elem`) | read | high |
| 0x67ACB0 | tok_next | throws "Unexpected end-of-file." if cursor >= count; else cursor++ (0x63EE20) and returns the new current token | read | high |
| 0x642440 / 0x63EE20 / 0x417AD0 / 0x41B3F0 | dynarr_at_end / advance / count / get(1-based, bounds-checked) | | read | high |
| 0x5843D0 | dynarr_init(elem,grow,cap) | fields: +0 cap, +4 grow, +8 cursor, +0xC count, +0x10 elem size, +0x14 = -1, data pointer at +0x19 (packed/unaligned) | read | high |
| 0x584580 | dynarr_clear(shrink) | shrink to grow-size when shrink!=0; cursor=count=0 | read | high |
| 0x584620 | dynarr_append(ptr/elem) | ++count, cursor=count, grow if count>cap, copy elem (or zero) | read | high |
| 0x677F00 | pscript_parse_system_body | loop until `}`(kind 7): kind 0x17 -> `key = XYZ(...)` (only `POSITION` accepted: 0x67A580 -> 0x67AD10 stores 6 floats at `system+0x2C`); kind 0x18 -> `key = number` (`SOURCEBLENDING` 0x67A700 -> 0x41B320 store u16 `system+0x60`; `DESTBLENDING` 0x67A740 -> 0x41B370 u16 `system+0x62`; values via `__ftol` = truncation); kind 0xA (EventSequence) -> new 0x162-byte object, ctor 0x67EF90, init 0x67F210, append, parse 0x678320. Errors: "Cannot specify any particle system properties after specifying sequences.", "Expected equals sign!", "Unknown particle system property!". | read | high |
| 0x678320 | pscript_parse_sequence | one `EventSequence` block (see below) | read | high |
| 0x677628 | pscript_finish | epilogue of 0x677390: evaluates the system Position range to 3 floats (0x67A400 -> 0x67A500) into `system+0x44/0x48/0x4C`; returns 1 | read | med |
| 0x67CDF0 | parse_xyz | `XYZ ( a , b , c )`; each component by 0x67CBF0; output 6 floats = {a.lo,b.lo,c.lo,a.hi,b.hi,c.hi}. Errors "Expecting XYZ(...)!", "Expecting ( after XYZ!", "Vector components must be seperated by a comma.", "Expecting ) to close vector." | read | high |
| 0x67CBF0 | parse_range | number -> {v,v}; `Random ( a , b )` -> {a,b} (not ordered). Numbers converted by 0x7B2110 (atof). | read | high |
| 0x67EF90 | sequence_ctor | constructs members (3 x range(8 B), 3 x vec6(24 B) in order range,range,range,vec6,vec6,vec6,range,range,range), dynarr `elem 4, cap 0x10` for events, registers object with manager `DAT_00A45FA8` (`0x5919A0(mgr, this, 0, 0x162, 1, this+0x145, 0x1D)`), sets `+0x12C=0`, `+0x130=0` | read | med |
| 0x67F210 / 0x67F250 | sequence_init / sequence_release | byte `+0` is an "active" flag; release calls the virtual deleting destructor (slot 0, arg 1) of every event (reverse order), clears event array, frees object at `+0x141` via 0x4B0C20 | read | high |
| 0x67A3E0 / 0x67A4E0 / 0x67A380 / 0x67A4A0 | vec6_dtor / range_dtor / vec6_ctor / range_ctor | trivial; range_ctor zeroes 2 floats | read | high |
| 0x58E7B0 | tracked_malloc | allocates size+0x11, header {next? prev, flags}, memset 0xEE, links into global alloc list | read | high |
| 0x58F050 | substr(dst,src,start1,len) | 1-based substring; used to strip padding from quoted names (`start=3`; spec says trim 2 chars each end) | read | high |
| 0x5A1550 | strcat_limited | append to buffer capped at 400 chars (used building error messages) | read | high |

### Sequence block dispatch (0x678320), by token kind
Name token (kind 8) -> name via 0x58F050 + 0x681E10 (set sequence name); `{` (6) required. Then until `}`:

| kind | meaning | handler |
|---|---|---|
| 0xE | `Texture = "name"` -> 0x681CD0 | kind 0xF `Model = "name"` -> 0x681D10 (texture/model name setters; both illegal after events) |
| 0x18 | numeric property (`= range-or-number`) | matcher chain of 15 name tests, each followed by a setter, see below |
| 0x19 | vector property (`= XYZ(...)`) | 8 matchers, see below |
| 0x1A | colour op mode, `= ...` parsed by 0x67D490, matcher 0x67AC00, store 0x67C900 | "Unknown sequence color op mode property!" |
| 0x1B | alpha op mode: parsed 0x67D590, matcher 0x67AC40, store 0x67C930 | "Unknown sequence alpha op mode property!" |
| 0x10,0x12,0x13,0x14 | time-specified event (`Fade So At t Key = ...` family): time via 0x67BEB0 and 0x67D340 (also yields a "fade" flag), next token must be kind 0x1C/0x1D/0x1E (Color/Size/Velocity-like), event object created by factory 0x677730; if fade flag set the event must answer true on virtual slot +8 ("Fading is not supported on this event."); virtual slot +0xC is called, `event+0x15 = owner sequence`, value parsed 0x67AD70, finished 0x67ADB0, appended to sequence event array | 
| other | "Unknown token" style error (message via 0x5A11A0/0x5A1550) | |

Properties and events are mutually ordered: any property after the first event throws "Cannot specify any sequence properties after specifying events.".

Numeric matchers -> setters (matcher address -> setter address; arg type): 0x67A5C0->0x67BF00 range; 0x67A600->0x67BF90 + 0x67C020 range (sets two fields); 0x67A640->0x67C990 int; 0x67A680->0x67C9F0 int; 0x67A840->0x67C230 int; 0x67A880->0x67C4E0 int; 0x67A900->0x67C5E0 range; 0x67A940->0x67C650 int; 0x67AA40->0x67C870 int; 0x67AA80->0x67C480 int; 0x67AAC0->0x67C420 int; 0x67AB00->0x67C3C0 float; 0x67A6C0->0x67C360 float; 0x67A980->0x67C6D0 range; 0x67A7C0->0x67C300 float. Ints are `__ftol` truncated doubles.
Vector matchers -> setters: 0x67AB40->0x67C0B0, 0x67AB80->0x67C140, 0x67ABC0->0x67C1D0, 0x67A780->0x67C2A0, 0x67A8C0->0x67C550, 0x67A9C0->0x67C750, 0x67AA00->0x67C7D0, 0x67A800->0x67CA60.
Which key name each matcher tests is **unexported** (each is a `FUN_007C07B0` case-insensitive compare against a literal, like `POSITION`). Mapping to the 22 keys of pscript.md is TODO(re).

### Tokenizer (0x6840D0)
Buffer 0x100 chars. States: 1 skip blanks (`FUN_007B3BDD` = isspace); first non-blank char starts a token; `//` -> state 4 (comment to LF); `"` -> state 3 (quoted string, copy up to closing quote or CR/LF, quotes kept, max 0x100); else state 2 (word). In state 2 these chars terminate the word and are themselves tokens: `( ) , { } :` and a `"` (starts a string); `/` begins a comment only if followed by `/`. **`=` is NOT a delimiter**, so `key=value` without blanks would be one word. Line counter increments on every `\n`; token carries its line number. Token kinds (from use): 1 number, 2 `=`, 3 `,`, 4 `(`, 5 `)`, 6 `{`, 7 `}`, 8 quoted string, 9 ParticleSystem, 0xA EventSequence, 0xB Random, 0xC XYZ, 0xE Texture, 0xF Model(?), 0x10/0x12/0x13/0x14 event-start keywords (Fade/At variants), 0x17 system vector key, 0x18 numeric key, 0x19 vector key, 0x1A colour-op key, 0x1B alpha-op key, 0x1C-0x1E event value keys. The classifier 0x5A18D0 decides; TODO(re).

## Runtime 

Naming: PSystem = parsed template (a "ParticleSystem" file) *and* the 0x78-byte live instance share one class (`PSystemInstance`, see struct page); PSequence = one EventSequence (0x162 bytes); PSeqState = per-sequence runtime state (0x3e bytes); PParticle = one particle (~0x80 bytes).

| address | proposed name | purpose | evidence | conf. |
|---|---|---|---|---|
| 0x68A170 | weather_draw_particles(dt_s, view_matrix, pass) | per-frame entry from 0x4E5FC0. Builds camera vectors, decides whether the local player's weather (rain/snow, "PSystem A") is visible, then calls 0x675E90. pass 1 = update+draw with two systems (precipitation at camera + ground ripples), pass 2 = draw/update only the first system with z lowered by a constant. Gated on `DAT_00A5B9F8 != 4` (not in some camera mode) and on a per-unit test 0x60D5F0 (point in region) for type-4 map entries | read | med |
| 0x675E90 | psystem_update_and_draw(this, dt_s, view_matrix) | for each sequence (reverse order i = n..1): if sequence.IsSmo==0 -> `psequence_update` 0x67F380(dt, emitter pos, ...); if view_matrix != 0 and not IsSmo -> `psequence_draw` 0x67FAF0; if IsSmo -> per-particle model actor orientation + rotation (see below) | read | high |
| 0x67F380 | psequence_update(this=PSequence, dt, emitter_pos xyz, prev/delta xyz, mode, blend src, blend dst, ..., PSeqState*) | updates all live particles then emits new ones (details in engine page) | read | high |
| 0x662F60 | pparticle_update(this=PParticle, dt, ...)->bool alive | integrates one particle: age, position, size, colour, velocity; returns 0 when age >= lifetime | read | high |
| 0x681070 | pparticle_spawn(this=PSequence, emitter xyz, dt, blend src, blend dst, mode, PSeqState*) | allocates a particle (0x663800/0x662E20), samples ranges, runs t=0 events, attaches model actor if IsSmo | read | high |
| 0x681B10 | pparticle_run_events(PParticle*) | fires due events (vtable slot 1) and re-scans if an event changed the particle's event timer | read | high |
| 0x67FAF0 | psequence_draw(PSystemInstance.mesh, view matrix, ...) | builds the camera-facing quad list into a dynamic mesh (4 verts + 6 indices per particle) | read | high |
| 0x677730 | pevent_create(keyword_token) | event factory: SIZE-kind (first compare against `DAT_009DC06C`, presumably "SIZE") -> 0x67ADE0; COLOR (RGBA, token kind 5) -> 0x67AF60; COLORRED 0x67B130; COLORGREEN 0x67B2B0; COLORBLUE 0x67B430; COLORALPHA 0x67B5B0; VELOCITY (vec, kind 8) 0x67B730; VELOCITYX 0x67B8B0; VELOCITYY 0x67BA30; VELOCITYZ 0x67BBB0; EVENTTIMER 0x67BD30. Each object is 0x23 bytes (operator new(0x23)) | read | high |
| 0x67D8C0 | pevent_base_ctor | vtable 0x9DCAA0, zeroes 0x17 bytes at +4, `+4 = -1` | read | high |
| 0x67A240 | psystem_alloc_states | lazily allocates the per-sequence PSeqState array at `this+0x74`: `new(count*0x3e+4)` with count header, per-element ctor 0x663680 | read | high |
| 0x677250 | psystem_restart | alloc states; for each sequence set `state.done(+0x2E)=0`, `state.start_ms(+0x36)=DAT_00A500BA` (current time ms) | read | high |
| 0x6771C0 | psystem_stop_emitting | sets `state.done=1` for all sequences (existing particles continue to live) | read | high |
| 0x677120 | psystem_kill | frees particles of all sequences (0x663740) and sets done=1 | read | high |
| 0x6772F0 | psystem_particle_count | sum of live particle counts over sequences | read | high |
| 0x6759A0 | psystem_init | on first use: creates the dynamic mesh object (`new 0x11C`, 0x5CBAF0(4,1)), sets `mesh+0x70 = 6`, creates sequence list dynarr (elem 4, grow 0x1E) at +0x70, flags `+0x64=1`, `+2=1`, `+0=1` | read | med |
| 0x675B30 | psystem_release(bool keep_mesh) | frees state array (0x675D70), releases template ref (decrement refcount at template+0x68) or the owned sequence list, destroys actor at +0x28 and mesh at +0x24 | read | high |
| 0x675930 | psystem_dtor | calls release(0) and destroys the name/position vec6 | read | high |
| 0x6758F0 | psystem_ctor | vec6 ctor + memset 0x78 | read | high |
| 0x679300 | psystem_assign_template(src) | makes this instance a *reference* to a loaded template (copy-on-reference): sets `+0x6C = template`, `+0x70 = template.sequences`, refcount at template+0x68++ | read | high |
| 0x4C2060 | pemires_find_by_name(name, flag) | resolves a script by name in the pemires table (string compare 0x7C07B0), used by weather | read | med |
| 0x5906F0 | rand_range(a, b) | `a + (b-a) * rand15 / C` where rand15 = RNG 0x5905A0 and C = `DAT_009D8C14` (RNG max, presumably 32767.0) | read | high |
| 0x590660 | rand_unit | `rand15 * DAT_009D8C10` (presumably 1/32767) | read | high |
| 0x590690 | rand_int_range(lo, span) | `lo + (int)(rand_unit * span)` (used as `(1,4)`, `(2,5)`, `(0,1)`) | read | med |
| 0x590540 | rand_int(n) | `[0,n)` (used for texture frame pick and weather jitter) | read | med |
| 0x681CD0 / 0x681D10 / 0x681E10 | psequence_set_texture / set_model / set_name | strcpy to +0xE1 / +0xC0 / +0x102; set_model also creates the model actor `new(0x2D8)` into +0x141 and loads `MODEL\<name>` via 0x5AF3D0 | read | high |
| 0x67BF00-0x67CA60 | PSequence setters/getters | field map in `docs/re/structs/PScriptSequence.md` | read | high |

### Weather module (neighbours in 0x689000-0x68C800)
| address | purpose |
|---|---|
| 0x68B380 | spawn "rain" system from template `rain.txt`: PSystemInstance via 0x6759A0 + assign template, copies name/blend src/dst from template (0x41B3A0/0x41B300/0x41B350), makes model actor, positions at `DAT_00A69CD8/DC/E0` (+ radius 0x43480000 = 200.0) |
| 0x68B600 | same for snow: file name `snow.txt` patched to `snow%d.txt` with the current weather index (`this+0x20`, short) |
| 0x68B900 | same for `ripple.txt` (ground ripples) |
| 0x68BBB0 | thunder: picks random sound/effect index 1..3 (`thunder` + digits, 0x590690(1,4), re-rolls if equal to the previous one at this+0x1C), computes flash position near camera with random sign, creates a lightning plane object (0x37-byte + 0x1A-byte record) with schedule times `param_1`, `+ftol`, `+ftol` |
| 0x68C600 | schedule `param_1` thunders: each starts at the previous start + random time `rand_range(1.0 - K/L, 1.0 + K/L)` with K=`DAT_00A3CDE4`, L=`DAT_009CF794` |
| 0x68AF10 | random "undefine action" spawn: picks offset radius 100/200/300 (probability 5/10, 3/10, 2/10 via `rand_int(10)<5, <8, <10`) around the player with random sign per axis, requires ground at that spot (0x469D10) |
| 0x68C2A0 / 0x68C440 / 0x68C570 / 0x68C6C0 | per-frame bookkeeping of weather entries (dynarr of 0x1A-byte records) |
| 0x689D00 / 0x689F60 / 0x68AB40 / 0x68AC20 / 0x68A010 / 0x68AA10 / 0x68ACE0 / 0x68AE90 | weather shutdown / reset / list helpers (not particle logic; `0x68ACE0` loads `ANIM\`/`ITEM\` + `ripple.am1` models) |
| 0x6891D0-0x689BF0 | generic container helpers, unrelated |

## Round 7 additions (verified)

### Key matchers 0x67A5C0-0x67AC00 (each `strcmp-nocase(key, literal) == 0`) -> setter -> field
LIFETIME->0x67BF00 (+0x01); EMITRATE->0x67BF90+0x67C020 (+0x09,+0x11); NUMPARTICLES->0x67C990 (+0x134); `Loop` (literal at 0x9DC? not shown)->0x67C9F0 (+0x138); TIMEDELAY->0x67C360 (+0x86); SOURCEBLENDING/DESTBLENDING (system level, 0x41B320/0x41B370); ORIENTATION->0x67C2A0 (+0x6A); `OriR` (data literal 0x9DC474)->0x67C300 (+0x76); OFFSET->0x67CA60 (+0x7A); FOLLOWCAMERA->0x67C230 (+0x69); SELFROTATION->0x67C4E0 (+0x8A); SELFROTAXIS->0x67C550 (+0x8B); SELFROTSPEED->0x67C5E0 (+0x97); HAS_ROTATION->0x67C650 (+0x9F); ROTATION_SPEED->0x67C6D0 (+0xA0); ROTATION_CENTER->0x67C750 (+0xA8); ROTATION_AXIS->0x67C7D0 (+0xB4); ISSMO->0x67C870 (+0x13C); RCHANGE->0x67C480 (+0x123); TEXTURENUMBER->0x67C420 (+0x128); TEXTUREPERIOD->0x67C3C0 (+0x124); SPAWNDIR->0x67C0B0 (vec6 +0x19, unused by shipped scripts); EMITRADIUS->0x67C140 (+0x31); GRAVITY->0x67C1D0 (+0x49); COLOROP->0x67C900 (+0x12C). 0x67AC40 (alpha op -> 0x67C930, +0x130) not exported. This corrects an earlier reading: SelfRotation/SelfRotAxis/SelfRotSpeed are the +0x8A/+0x8B/+0x97 group and Has_Rotation/Rotation_* the orbit group.
`Initial Velocity/Size/Color` and `Fade` lines never reach these matchers: they are events (factory 0x677730).

| address | name | purpose | conf. |
|---|---|---|---|
| 0x590B00 | clamp_f64(x, lo, hi) | `x<lo -> lo; x>hi -> hi; else x`. Round 2 guessed pow(); in 0x662F60 it is `clamp(f(frame-rate)*10.0, 1.0, 40.0)` (args 1.0 = 0x3FF00000, 40.0 = 0x40440000) used as a divisor of the emitter motion delta (modes 1-4) | high |
| 0x662E20 | pparticle_init | memset 0x80, keep +0 tag, life=1.0, +0x14=1.0, size=1.0, colour=0xFFFFFFFF, actor=0 | high |
| 0x663800 | ppool_alloc | allocate slot (0x5857A0 grow, 0x417520), set slot tag `*p = 1`, return index | high |
| 0x662ED0 | pparticle_free_actor | destroys the attached model actor (0x5EFDB0 detach, 0x420840 delete) | high |
| 0x41BAF0 | effect_spawn(template, pos, ...) | **central "play effect" API**: `new 0xA3` effect record (ctor 0x41A1B0, args via 0x41A740), appended to the global effect list (0x5857A0), returns its index (0x417520). Callers pass the template from `pemires_find_by_name(...)->+0x24` | med |
| 0x41A930 | effect_instance_from_template | builds a PSystemInstance (0x6759A0 + 0x679300) with blend copied from the template and a model actor `new 0x2D8` at `DAT_00A69CD8/DC/E0` radius 200.0 | med |
| 0x4C2060 | pemires_find(name, load) | linear case-insensitive search of the 0x28-byte entries (`strcmp-nocase(entry+0, name)`), returns index+1 (0 = not found); `load=1` -> 0x4C1D80(index) parses the file once and caches it at entry+0x24 | high |
| 0x4C1D80 | pemires_load(index) | lazy load/parse (see PEmiResEntry) | high |

### Which effects the game requests (callers of 0x4C2060 with literal names or built names)
| site | effect |
|---|---|
| 0x4FD980, 0x43B990, 0x43B490 | `spiritual.txt` (unit spirit/special effect; duration arg 5000 ms into 0x41BAF0) |
| 0x522C40 | `flash.txt` (level-up / flash, with `LEVELUP` text; duration 5000) |
| 0x490110, 0x54FFD0 | `towerfire.txt`, `towerfiresmoke.txt` (burning towers/buildings) |
| 0x68B380 / 0x68B600 / 0x68B900 | `rain.txt`, `snow<N>.txt`, `ripple.txt` |
| 0x41A2A0, 0x41B640, 0x418C60 | weapon impact/hit effects: name = weapon/material prefix (`this+0x77 + i*0x21`, 6 names of 0x21 bytes) + optional `_default` + a fixed suffix literal (`DAT_009CF5F4` / `DAT_009CF758`, presumably `.txt`) resolved via 0x4C2060 and cached in the weapon record at `+0x203+i*4`; the terrain class is chosen from literals at 0x9CF770/0x9CF768 else `other` |
| 0x4A39F0 | per-unit-type effect names composed with the `ANIM\` path (cached at `unit+0x172+i*4`) |
| 0x5ABE30 | **animation-event effects**: while advancing an actor animation, the model's event table (0x3C-byte records at `anim+0x15B`: `float time; float bone_or_-NaN; char name[...] (+8); byte flag (+0x3B)`) triggers `pemires_find(name)` -> `effect_spawn`; duration `ftol(...)+5000` ms when the actor is not the camera target. So most combat particles are authored in the animation files (see docs/formats am3/amt), not in code |
| 0x546BF0, 0x551930 | other name-built effects (building/ability code) |

## Round 14 (event bodies)
| address | name | purpose |
|---|---|---|
| 0x67D9A0 / 0x67DAA0 / 0x67DDD0 / 0x67DEE0 / 0x67DFF0 / 0x67E100 / 0x67E210 / 0x67E3F0 / 0x67E4F0 / 0x67E5F0 / 0x67E6F0 | event apply: SIZE / COLOR / COLORRED / GREEN / BLUE / ALPHA / VELOCITY / VELOCITYX / Y / Z / EVENTTIMER | keyframe-chain semantics in PScriptEvent.md |
| 0x67E730 / 0x67E7F0 / 0x67E8B0 ... 0x67EEB0 | event parse_value (slot 3): checks the keyword (`Size`, `Color`, `ColorRed`, ..., `Velocity`) then `=` then parses a range (0x67CBF0), `RGBA(...)` (0x67D050) or `XYZ(...)` (0x67CDF0) into event+0x1B | errors "Expecting Size!/= after Size." etc. |
| 0x67AE70 ... | supports_fade (returns 1 for ramping classes) |
| 0x67AE90 ... | scalar deleting dtors; 0x67CB20 = token-array dtor |
| 0x67EF70 | event_is_fade (`event+8`) |
| 0x67D6F0 | token classifier (see PScriptEvent.md), 0x67D690 add token |
| 0x67AC40 | matcher for key `ALPHAOP` -> field +0x130 |
| 0x590340 | integer square root (floor), used by the emitter-drag divisor |

## Remaining unexported
Event vtable slot bodies: round 7 gave the *thunk* addresses (0x4049D0 ... 0x40A8B7) but the export does not contain files for those thunks or their callees (no 0x40xxxx files exist in particles3), so the fade-ramp formula is still INFERRED. The alpha-op matcher 0x67AC40 and the `Loop` literal text are also absent. `en3d::plane` mesh API 0x5CBAF0-0x5CE6F0 was exported but belongs to the scene page.
