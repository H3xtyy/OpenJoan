# flocksys family (flocksys.cpp, physis_sys init, attention-call pop-ups)

Reference build: `joan.exe`. Export: `$JOAN_WORK_DIR/decomp/game/flocksys/` (106 functions, depth <= 3).
Confidence: V = read in >= 2 consistent places, R = read once, I = inferred. `this` is frequently lost by Ghidra
(MSVC `thiscall`/`fastcall` helpers show up as `thunk_FUN_x()` with no argument); where the receiver is deduced it is said so.

## 0. What this family really contains (read first)

The brief assumed `flocksys.cpp` + `physis_sys` = unit movement, steering, terrain following, knock-down physics.
**The exported code contains none of that.** The three kinds of code that were exported are:

1. **`flocksys.cpp`** (0x46DFA0..0x46EED0, 6 functions + helpers): a tiny *ambient flock* object manager. A "flock" is
   a pool of 5-ish groups of decorative actors that spawn at random points inside an AABB and get a random drift
   vector. No per-tick movement code is in the export (the per-tick update of flock members is not reachable from the
   depth-3 roots). Struct pages: [Flock](../structs/Flock.md).
2. **`0x472EB0` + `0x4D6A10` + `0x46E8D0`** are *not* movement roots:
   * `0x472EB0` is the setter called right after `physis_sys` init: `*(float*)(physis_obj+0xB16) = 3.0f`
     (the `0x40400000` argument in the init sequence of `0x46F3A0`, see [Physics](../structs/Physics.md)).
   * `0x4D6A10` is a predicate (`return *(byte*)(this+0x3F)`) gating `0x46E8D0` in the per-frame update (`0x4E5FC0`).
   * `0x46E8D0` is the per-frame update of the **"attention call" message pop-up list** (sliding panel + animated
     icon `M0001..M0009` + voice line + `ATTENTION_CALL` sound). It is HUD/UI, not physics. Details in section 3.
3. Generic helpers (`ilib::dynarr`, `dynarrb`, tracked malloc/free, OpenAL source wrappers, CRT string calls).

`physis_sys` itself (init `0x6AD020`) and the movement builtins were exported in **round 2**
(`decomp/game/movement/`, plus the `unit/`, `hud/`, `battle/`, `items_facilities/` exports for the physics step).
They are documented in sections 7-12 below; the physis object and bodies in [Physics](../structs/Physics.md).
Key correction to the brief: `physis_sys` is a **small rigid-body/projectile integrator with terrain collision**
(bodies for units, arrows, cannon balls, thrown stones, ...), **not** a steering/flocking system; ordinary unit walking
is a **numeric per-class speed from unit.res** pushed into a body as a velocity (section 9.3; only skill/attack animations use root motion), and "charmove" is the
direct-control tuning section `[CHARMOVE]` of `Gameplay.res` (section 11), not a movement module.

## 1. flocksys.cpp (source path `d:\ww\flocksys.cpp`, attribution V: assert/alloc file strings)

All allocation sites pass the file string to the tracked allocator `0x58E8D0`/`0x58E7B0` (size, file, line) and the
tracked free `0x58ED70`. Global flock registry = a `dynarrb` (see section 4) of `Flock*`, 1-based, null slot = free.

| Address | Proposed name | Purpose | Evidence | Conf |
|---|---|---|---|---|
| 0x46EDE0 | flocksys_create_flock(a1..a5 : i16, b1,b2,b3 : u32) -> i16 id | alloc `Flock` (0x5B bytes), zero it (0x46DE30), run ctor 0x46DE90 with the 8 args, push it into the global registry (0x5857A0), `flock.id = registry cursor` (0x417520 = dynarr `+8`), then call `add_group(flock.members_per_group)` `flock.group_count` (`+0x35`) times; returns the id | 0x5B alloc with flocksys.cpp string; loop bound `+0x35`, arg `+0x37` | V |
| 0x46DE30 | flock_zero | `memset(flock,0,0x5B)` | 0x46DE30 | V |
| 0x46DE90 | flock_ctor | stores arg8 at `+6`; creates two `dynarr`s (`operator_new(0x1D)` + `dynarr(elem=4, grow=0x1E)`) into `+0x57` (groups) and `+0x53` (members). Other ctor args are copied by code Ghidra did not show (8-arg prototype missing) | 0x46DE90 | R |
| 0x46E2D0 | flocksys_add_group(this=Flock*, member_count) | `++flock.group_counter(+0x39)`; **only if the counter is now < 6** (i.e. at most 5 groups): alloc `FlockGroup` (0xE bytes), init (`0x46DBA0`: `{flock_id, links=0}`), append to `flock+0x57`, `group.recno(+2)` = new array index; then `member_count` times: alloc `FlockMember` (0x7F), `flock_member_ctor`, `flock_member_init(flock_id, group_recno)`, append to `flock+0x53`, `member.recno(+0xC)` = its array index (as i16), link into the group list (0x46DC80) | 0xE / 0x7F allocs, `< 6` test | V |
| 0x46E430 | flocksys_remove_group(this=Flock*, group_recno) | `--group_counter`; **only if the counter is now < 5**: take the group, read its `+2` (recno), scan `flock+0x53` members from the back and for every member whose `+0xA` (group id, i16) equals the group recno: unlink from the group list (0x46DD70), release actor (0x4207D0), free, remove from array (0x584A10); finally clear + free the group and remove it from `flock+0x57`. The two thresholds (<6 on add, <5 on remove) are asymmetric: removal silently does nothing while 5 groups exist (likely an original off-by-one; keep as is) | 0x46E430 | R |
| 0x46DFA0 | flock_destroy_contents (a.k.a. dtor body) | for `flock+0x57` (groups) back to front: clear + free group, delete array entry; delete the array object (`0x463FC0`); same for `flock+0x53` (members: release actor, free); if `flock.kind(+2) == 1` and `+0xA` and `+6` non-null: remove `+0xA` from the list at `+6` via `0x5EA5E0` and null it | 0x46DFA0 | V |
| 0x46ECC0 | flocksys_free_all | for registry index count..1: if slot used: get ptr (0x4231F0), `flock_destroy_contents`, run scalar dtor (0x46ED90, flag 0), tracked-free, `dynarrb::remove` (0x5858C0); then reset the registry (0x585D20: `dynarr::reset(keep_capacity)` + free-list count = 0) | 0x46ECC0 | V |
| 0x46EED0 | flocksys_free_flock(id : i16) | same as one iteration of `free_all` for a single id | 0x46EED0 | V |
| 0x46ED90 | flock_scalar_dtor(flag) | calls 0x46DE60 (-> `flock_destroy_contents`); `delete this` if `flag & 1` | | V |
| 0x46DE60 | flock_dtor_thunk | tail-calls 0x46DFA0 | | V |
| 0x46DB60 | flock_group_ctor_a | returns ecx (no-op ctor of the base part) | | R |
| 0x46DBA0 | flock_group_init(flock_id) | `group.flock_id(+0)=arg; group.count(+6)=0; group.head(+0xA)=0` (writes `+6`,`+0xA` as i32 via the i16*3/5 indexing) | | V |
| 0x46DBE0 | flock_group_clear | `+2 = +6 = +0xA = 0` | | V |
| 0x46DC80 | flock_group_push_member(member*) | if `head==0`: `head=member; member.next=0; member.prev=0` else walk `next` to the tail, `tail.next=member`, `member.prev=tail`; `++count` | helper setters 0x422F70/0x422FC0 write `+0x66`/`+0x6A` | V |
| 0x46DD70 | flock_group_remove_member(member*) | find in list; if it is the head, `head = (count==1) ? 0 : head.next`; detach via 0x423030; clear its links; `--count` | | V |
| 0x422F70 / 0x422FA0 / 0x422FC0 | member_set_next / member_get_next / member_set_prev | `*(this+0x66)`, `*(this+0x6A)` | | V |
| 0x423030 | member_unlink | neighbour re-linking; `this` of the inner setters is lost, behaviour = standard doubly-linked unlink (both links 0: reset both) | | I |
| 0x4203E0 | flock_member_ctor | `vtable = 0x9CF828; memset(this,0,0x7F)` (vtable is overwritten to 0 by the memset only when called after; the order in the binary is vtable first, then memset covering offset 0 -> vtable slot = 0; treat `+0` as unused/zero) | 0x4203E0 | R |
| 0x420420 | flock_member_init(flock_id, group_recno) | see section 1.1 | 0x420420 | V |
| 0x4207D0 | flock_member_release_actor | `if (this+4) { actor_dtor_and_delete(this+4); this+4 = 0 }` (0x420840 -> `en3d::actor::fn_005AAF10`, i.e. actor dtor, then `delete` when bit0 set) | | V |
| 0x420840 / 0x4230F0 | actor_scalar_dtor / actor_dtor_thunk | -> `en3d::actor` destructor 0x5AAF10 (see [actor](actor.md)) | | V |
| 0x417AD0 / 0x417520 | dynarr_count / dynarr_cursor | `+0xC` / `+8` | 0x417AD0 | V |
| 0x4231C0 / 0x46EFC0 | dynarr_count thunks (members / groups) | -> 0x417AD0 | | V |
| 0x4231F0 / 0x46E590 / 0x46E5D0 / 0x4AF2D0 | dynarr_get thunks | -> 0x41B3F0 (1-based get, null when out of range) | | V |
| 0x46EFF0 / 0x4AF300 | dynarr_slot_empty | `get(i) == 0` | | V |

### 1.1 flock_member_init (0x420420) — what a spawned member looks like

Inputs: `flock_id`, `group_recno` -> stored at `member+8` and `member+0xA`. Let `F` be the flock record (registry
lookup 0x4231F0). Output state (offsets in [Flock.md](../structs/Flock.md), `FlockMember`):

* `member+0xE = F+0x2B` (u32; model/actor type id, copied only; consumer not in the export).
* Position `(+0x12,+0x16,+0x1A)` = uniform random in the flock AABB: `x = rand_range(F.min.x, F.max.x)`,
  `y = rand_range(F.min.y, F.max.y)`, `z = rand_range(F.min.z, F.max.z)` where the 6 floats are at `F+0x3B..F+0x52`
  as `[min.x, min.y, min.z, max.x, max.y, max.z]` (pairs read as `(f[0],f[3])`, `(f[1],f[4])`, `(f[2],f[5])`).
* Drift vector: `+0x1E = rand_range(0.1, 3.0)`, `+0x22 = rand_range(0.1, 3.0)`; each is negated with probability 1/2
  (`rand01 > 0.5` then `*= _DAT_009CF834`, presumed -1.0); `+0x26` stays 0 (from the memset). `+0x36 = length(+0x1E)`.
  (Only two of three vector components are random; which world axis they are is not decided by this function.)
* Zeroed: `+0x2A,+0x2E,+0x32` (second vector), `+0x3A`(i16), `+0x3C`(i16), `+0x3E`, `+0x42`, `+0x66`, `+0x6A`, `+0x6E`(byte).
* `+0x46 = +0x4A = 0x497423F0` = **999999.0f** (two "best distance so far" sentinels).
* RNG calls: 0x5906F0 `rand_range(a,b)` x5, 0x590630 `rand01()` x2 (global 15-bit LCG, see Constants).

## 2. Registry / array helpers (ilib, shared by the whole game)

| Address | Name | Purpose | Conf |
|---|---|---|---|
| 0x5843D0 | dynarr_ctor(elem_size, grow, initial_cap=0, cap_override=0) | `+0x10 = elem`, `+4 = grow`, `+0 = capacity (= initial or override)`, data = tracked alloc(elem*cap) at `+0x19`, `+8 = +0xC = 0`, `+0x14 = -1` | V |
| 0x584510 | dynarr_resize(cap) | realloc data to `cap*elem`; `+0 = cap` | V |
| 0x584580 | dynarr_reset(shrink) | if shrink and cap != grow: realloc to `grow` elements; `+8 = +0xC = 0` | V |
| 0x584620 | dynarr_push(src) | `++count; cursor = count`; grow by `+4` when `count > capacity`; copy `elem` bytes from `src` (or zero first byte if null) | V |
| 0x584A10 | dynarr_remove(i or -1 = cursor) | memmove tail down, `--count`, `cursor=min(cursor,count)`, shrink if `count < cap - 2*grow` | V |
| 0x5844B0 | dynarr_free_data | tracked free of the data block | V |
| 0x41B3F0 | dynarr_get(i) | `i` in `1..count` -> `*(void**)(data + (i-1)*elem)` else 0 (for pointer arrays) | V |
| 0x5857A0 | dynarrb_push(src) | like push but re-uses a freed index first: pops the last entry of the free list (`+0x1D` ptr, 8-byte entries, `+0x25` count) and sets `cursor` to it; else `++count` | V |
| 0x5858C0 | dynarrb_remove(i or -1) | marks slot `i` free: appends `i` to the free list (grow by `+4`, entries are 8 bytes), memset the slot to 0 | V |
| 0x585D20 | dynarrb_reset | `dynarr_reset(1)` then free-list count = 0 | V |

Layout: `dynarr` = 0x1D bytes `{+0 cap, +4 grow, +8 cursor(1-based), +0xC count, +0x10 elem_size, +0x14 -1, +0x19 data*}`;
`dynarrb` adds `{+0x1D freelist*, +0x21 freelist_cap, +0x25 freelist_count}` (total 0x29).
The flock registry is a `dynarrb`: ids are stable because removed slots become 0 and are recycled LIFO.

## 3. Attention-call pop-up list (0x46E8D0 and callees) — not part of flocksys.cpp, but exported with it

`0x4D6A10(this = GameApp?) -> byte at +0x3F` is the enable predicate; `0x4E5FC0` calls `0x46E8D0` after
`0x4AF590` when it is true (see [gameloop.md](gameloop.md)). The attention-call records are **HUD sliding
panels that announce a message with an animated icon and a spoken line**.

| Address | Proposed name | Purpose | Conf |
|---|---|---|---|
| 0x46E8D0 | attncall_update_all(this) | if `this+0x3A == 0` (`this` = the GameApp-like object passed by `0x4E5FC0`; `+0x3A` is its state byte, 0 = none; the `+0x36` count / `+0x44` flag used by the callees belong to the pop-up manager, a different object): for i = count..1 (reverse): skip empty slot; if `rec.state(+4) == 6` -> `attncall_free(rec.id)` else `attncall_update(rec)` | V |
| 0x463120 | attncall_free(mgr, id) | delete the record (dtor 0x4AE160 -> 0x4AA310 string-member dtor, `delete`), `dynarrb::remove(id)`, `--mgr.count(+0x36)` | R |
| 0x4ACE90 | attncall_update(rec) | the state machine, section 3.1 | V |
| 0x4AE710 | attncall_try_acquire_front(mgr, id) | returns 1 and clears `mgr+0x44` when no other live record has a smaller sort key `rec+0x3F` (ties broken by smaller `rec+0xD` y); returns 0 if `mgr+0x44 == 0` already or any other record is ahead | V |
| 0x499E40 | attncall_release_front(mgr) | if `mgr+0x44 == 0`: set 1 ("slot is free"); find the smallest key; then find the second-smallest; if that record has `flag(+8)==0`, then every live record with `flag==0` gets `target_y(+0x15) += 0x28` (40 px shift down the stack) | R |
| 0x4AF2B0 | attncall_front_free | `return mgr->byte(+0x44)` | V |
| 0x4AEF10 | attncall_sort_key | `return *(u32*)(rec+0x3F)` | R |
| 0x4CED90 | sepl_resume(voice_id) | for slots 1..12: if slot has a source, its id (0x4D0440) == voice_id and `source_get_state == 3 (PAUSED)` -> `source_play` | V |
| 0x470C90 | sepl_pause(voice_id) | same scan, state != PAUSED -> `alSourcePause` (0x59F650) | V |
| 0x4CFEE0 | sepl_is_busy(voice_id) | 1 if any slot with that id has state != STOPPED(4) | V |
| 0x4CF0F0 | sepl_play_id(id) | see [audio.md](audio.md) (`sepl_play_id`) | V |
| 0x4CEEF0 | sepl_play_def_by_name(name) | see audio.md (`sepl_play_def`); used here with `"ATTENTION_CALL"` | V |
| 0x4D6A10 | gameapp_attention_enabled | `return byte(+0x3F)` | R |
| 0x4D69A0 | string_from_number | -> 0x58FCA0 (int-to-string); used to build `"M000"+n` frame names | I |
| 0x5A11A0 / 0x5A15D0 | str401_assign / str401_append_number | `strncpy(dst,src,400); dst[400]=0`; `strncat(dst, number_string(n), 400-strlen)` | V |
| 0x590270 | str_copy_limited(max 0x78) | `memcpy(dst,src,min(n,0x78)); dst[n]=0` | R |
| 0x5EA5E0 | ptrlist_remove(ptr) | linear search in a pointer array (`+0x24` count, `+0x2C` data), shift down (0x591070 memmove), `--count`; used to detach a kind-1 flock from its owner list | V |
| 0x472EB0 | physis_set_gravity(float) | `*(float*)(this+0xB16) = arg` (init passes 3.0) | V |

Audio slot calls (0x59F4xx..0x59FCxx) are the OpenAL wrappers listed in [audio.md](audio.md)/[video.md](video.md);
0x4CF690 = `sepl_slot(n)` (`this-4+n*0x18`), 0x4D0440 = slot's voice id, 0x45F450 = `sepl_find_free_voice`.

### 3.1 Record state machine (0x4ACE90) — fields in [Flock.md](../structs/Flock.md) section "AttentionCall"

Every call first applies the voice pause rule: if the voice id `rec+0x19 != 0`: when `DAT_00A500CC == 0.0` (game speed
word is zero = paused) `sepl_pause(id)` else `sepl_resume(id)`. `dt = DAT_00A500BE` (ms per frame, see TIMING.md),
`now = DAT_00A500BA` (absolute ms). Then by `state` (`rec+4`):

| State | Meaning | Action |
|---|---|---|
| 0 | new | state = 1, `last_ms(+0x43) = now` |
| 1 | slide to rest | rest line `Y = DAT_00A48860 - 0xC2` (screen height - 194, px). If `y(+0xD) < Y`: if `target(+0x15) <= y` then (needs `front_free` and `try_acquire_front(id)`, else just stamp `last_ms` and return) set `target = Y`. Then `y += dt * _DAT_009CFC4C` (x `_DAT_009CFC20` when `flag(+8) != 0`), clamp to `<= target` and `<= Y`. If `y >= Y` -> state 2 |
| 2 | announce | state = 3; `sepl_play_def_by_name("ATTENTION_CALL")` |
| 3 | icon fade-in/animation | if `alpha_d(+0x4B) >= 255.0` -> state 4. Else four channel floats are raised in a chain: `+0x4F += dt*0.5` (cap 255.0); once `+0x4F >= _DAT_009CFCA0`, `+0x53 += dt*_DAT_009CFC7C` (cap 255); once `+0x53 >= _DAT_009D51EC`, `+0x47 += dt*_DAT_009D51E8` (cap 255); once `+0x47 >= 255.0`, `+0x4B += dt*_DAT_009D51E8` (cap 255). In parallel the frame name `rec+0x2E` (str401, e.g. `M0001`) advances: if it is not `"M0009"` (0x7C07B0 compare) and (`flag==0` or name starts with `"M000"`) and `now - last_ms > 30 ms (0x1E)`: take the last character run after the final `'M'` of the current name string (0x417D90), parse it (0x590270 + atoi), build `"M000" + (n+1)` and copy 8 chars back into the name; `last_ms = now`. So the icon is a 9-frame flipbook `M0001..M0009` at 30 ms/frame |
| 4 | speech | once: start voice `sepl_play_id(rec+0x19)` and set `started(+0x1D)=1`. Ramp `+0x3B += dt*_DAT_009D51D4` up to `hold(+0x17)` while stamping `last_ms`; when ramp done and `now > last_ms + hold*0x21 (33)` and the voice is no longer playing (`!sepl_is_busy`) -> state 5 |
| 5 | fade-out | `timer(+0x57) -= dt*1.0`; when `<= 0.0` -> state 6 and `attncall_release_front` (the old tick keeps writing `timer = max(timer,0)`) |
| 6 | dead | removed by `attncall_update_all` the same frame |

Order check: the `timer <= 0` test is made **before** the subtraction (so the panel lives one extra frame).

## 4. Constants (this family)

| Value | Where | Meaning |
|---|---|---|
| 0x5B / 0xE / 0x7F | 0x46EDE0 / 0x46E2D0 | `sizeof(Flock)`, `sizeof(FlockGroup)`, `sizeof(FlockMember)` |
| 5 groups | 0x46E2D0 (`counter < 6`), 0x46E430 (`< 5`) | max groups per flock |
| dynarr(elem 4, grow 0x1E) | 0x46DE90 | per-flock group/member pointer arrays, grow step 30 |
| 0.1 .. 3.0 (0x3DCCCCCD, 0x40400000) | 0x420420 | random drift component range (magnitude) |
| 999999.0 (0x497423F0) | 0x420420 | initial distance sentinels `+0x46`, `+0x4A` |
| LCG | 0x5905A0 | `s = s*0x15A4E35 + 1`; output `(s >> 16) & 0x7FFF` (15 bits); state at `+0x7A` of the global RNG object; `rand01 = r * _DAT_009D8C0C` (0x590630, presumably 1/32767), `rand_range(a,b) = a + (b-a)*r/_DAT_009D8C14` (0x5906F0, presumably 32767.0; see engine/particles.md) |
| 3.0 | 0x46F3A0 -> 0x472EB0 | gravity field of the physis object (units not determined; used by the unknown physics code) |
| 194 (0xC2) px | 0x4ACE90 | pop-up rest line above the bottom of the screen (`DAT_00A48860` = screen height) |
| 40 (0x28) px | 0x499E40 | stack shift of waiting pop-ups when one leaves |
| 30 ms (0x1E) / 33 ms x hold (0x21) | 0x4ACE90 | icon frame period / hold time per `hold` unit |
| 255.0 (0x437F0000, `_DAT_009CFB68`) | 0x4ACE90 | alpha channel cap |
| 12 voice slots | 0x4CED90 | `sepl_slot(1..12)` scan limit |
| 400 (0x190) | 0x5A11A0 | string buffer capacity (401 bytes incl. NUL) |
| Unread rodata | `_DAT_009CFC4C`, `_DAT_009CFC20`, `_DAT_009CFCA0`, `_DAT_009CFC7C`, `_DAT_009D51EC`, `_DAT_009D51E8`, `_DAT_009D51D4`, `_DAT_009CF834`, `_DAT_009D8C0C`, `_DAT_009D8C14` | TODO(re): read the float32 values from `.rdata` (not retrievable from the pseudocode) |
| Known from other pages | `_DAT_009CF5C4 = 0.0`, `_DAT_009CF784 = 1.0`, `_DAT_009CF77C = 0.5`, `_DAT_009D5344 = 0.01` | |

## 5. RNG call sites in this family (determinism audit)

All use the single global LCG (0x5905A0): 0x420420 `flock_member_init` x7 (3 positions, 2 drift magnitudes, 2 sign
flips). Attention-call code uses none. The flock spawn happens when the flock is created, so replays are deterministic
as long as the RNG state at that moment is identical.

## 7. physis_sys lifecycle and body registry (confidence R unless noted)

| Address | Proposed name | Purpose | Conf |
|---|---|---|---|
| 0x46F3A0 (call site) | game_init step | log `"Loading physis_sys"`, `physis_init(0x6AD020)` then `physis_set_gravity(3.0f)` (0x472EB0) | V |
| 0x6AD020 | physis_init(this=Physis) | `+0xB12=0` (disabled), `+0xB14=1`, `+0xB16=1.0f` (gravity factor, overwritten with 3.0 by the caller), `+0xB1A=1`, `+0xB1C=0`, `+0xB1E=1`, `+0x586 = now_ms` (`DAT_00A500BA`), clear plane tables (0x6AD5D0), `0x68DB80(this+0xB20)`, allocate the active-body list `+0xB0E` = `dynarrb(elem 4, grow 5)` (0x2E-byte object) | V |
| 0x6AD170 | physis_shutdown | clear tables, 0x6B1F30 (free all bodies), `0x68DBC0` (table-1 count `+0x580 = 0`), free the active list | R |
| 0x6AD950 | physis_reset | `shutdown` then `init` | R |
| 0x6AD5D0 | physis_clear_planes | for i = count..1: remove each registered plane pair (0x68DD80), `count(+0xB0A)=0` | R |
| 0x6AD450 | physis_remove_plane_pair(A,B) | find entry `(A,B)` in the `+0x58E` table (stride 0x34); every body whose ground refs (`+0x212`/`+0x20E`) equal A or B has them cleared; remove from both tables (0x68DD80 for table 1 at `+4`, memmove for table 2) | R |
| 0x6B1F30 | physis_free_all_bodies | for each live slot of the global body array (a `dynarrb` of `Body*`): delete body (0x6B1FF0 -> dtor 0x6B08C0), `dynarrb::remove`; then reset | R |
| 0x6ADBF0 | physis_create_body(actor, mass, kind:i16, skip:i16, layer:i16, enable:i16) -> id | extents by the actor's volume type (`actor+0xB8`: 0 none -> (0,0,0); 1 sphere -> radius x3; 2 box -> per-axis max(abs(min),abs(max)); 3 capsule -> (radius, radius, height/2.0 (`_DAT_009CF794`))); `skip != 0` returns -2 (0xFFFFFFFE); else allocate `Body` (0x2B2 bytes, 0x6B0830 ctor), init (0x6B08E0), push into the global body array, `body.id = cursor`; copy the actor position (x,y,z `ftol`) into `+0x202/+0x206/+0x20A`; `layer` -> `+0x28A`; if `enable` -> `physis_enable_body(id,1)`. Returns the body id (stored by callers in `actor+0x5C`) | V |
| 0x6AF370 | physis_recreate_body(id, mass, kind, skip, layer, enable) | like create but re-initialises an existing body (disable, re-run 0x6B08E0 with the new extents, re-enable); used when the volume type changes | R |
| 0x6B08E0 | body_init(actor, mass, extents*, kind, enable) | sets owner `+8`, mass `+0x27A`, extents `+0x27E/+0x282/+0x286`, kind `+4`, clears flags (`+0x298`, `+0x2AC`, `+0x2AE`, `+0x2A2`, `+0x278`), resets both 0x136-byte state blocks (`+0x0C` "current", `+0x142` "next"), seeds position from the actor, computes the ground (0x6ADFD0) when `enable`, then copies "next" over "current" | R |
| 0x6AD980 | physis_enable_body(id, on) | `on`: if the body is not already in the active list (`+0x2AE == 0`) push it into the list `+0xB0E` and store the list index at `body+0x2AE`. `off`: clear velocity-impulse blocks (`+0x29E`, `+0x22C`, `+0x234`, `+0x260`, `+0x268`), `+0x216 = 1`, `+0x1FE = ground*1.0/K`, reset the saved positions `+0x142..+0x156` to the neutral vector `DAT_00A69CD8..E0`, remove from the list | V |
| 0x6AF310 / 0x6B0760 / 0x6B0790 | physis_get_body / body_ptr / body_slot_empty | `dynarr_get` on the global body array (1-based) | V |
| 0x6AABB0 / 0x6AABE0 | physis_get_gravity / physis_get_substep_mode | `+0xB16` / `+0xB1A` | V |
| 0x6AAC10 | physis_pick_layer(plane_pair, layer) | layer 0: prefer `B` else `A`; layer != 0: prefer `A` else `B` | R |

`K = _DAT_009CF780`: conversion between physics-space and world units; every body position is `world / K`
(multiplied back by K when looking up terrain). `camera.md` lists `0x9CF780 = 46.1`, unverified for this use.
TODO(re): read `.rdata` 0x9CF780.

## 8. Physics step (0x6AD660) — per frame, only in game state 9 (0x4E5FC0)

Called from the update root right after `0x5634E0` in state 9 (see [gameloop.md](gameloop.md)). Not fixed-step:
the step length is the real elapsed time.

1. `+0x58A = 0`. `now = DAT_00A500BA` (ms). If `+0x586 == 0` or `+0xB12 == 0` (physics disabled): `+0x586 = now`, return.
2. For every body with an actor model: reset a per-frame field (`model+0x50 = 0`).
3. For `i = count..1` over the **global body array** (reverse order) and only bodies with `+0x2AE != 0` (active list member)
   and `id != 0`: `0x6B18A0` (record the previous position `+0x1DA..`, build the facing vector `+0x1F6.. = unit_x * 0.02169` — scale
   constant `0x3CB1B35E`), then
   * kinds **2, 3, 4, 5, 0x12** -> `0x6A8060` (projectiles / thrown items), else -> `0x6A45A0` (characters and everything else);
   * `elapsed = float(now - last_ms)`, `0x6B1350` post-processing (copies position back to the actor, and for kind 0xB drags
     the two attached "riders" (see below)).
4. `+0x586 = now`.

### 8.1 Character body integrator `0x6A45A0` (this = Physis, args: body, physis, now, elapsed_ms, +2)

Key behaviour (read from the 1100-line pseudocode; sub-formulae marked `I` are inferred):

* **Sleep**: if the body has no pending impulse (`+0x234`, `+0x22C`, `+0x268`, `+0x260`, `+0x24E`, `+0x246` all quiet) a
  counter `+0x278` increments each step; at **> 999** the body deactivates itself (`+0x298 = 0`, return).
* **Sub-stepping**: `max_steps = 1.0` for bodies whose helper `0x6B1DD0` returns non-zero, **20.0** when it returns 0
  (`kind 0`, or kind 7 whose rider has `+0x102 == 0`). If `elapsed > max_steps`: `dt_s = elapsed * 0.001 / max_steps`,
  `n = max_steps` iterations, `dt_s` clamped to 1.0; otherwise one iteration with `dt_s = elapsed * 0.001`.
  So non-rider bodies integrate once per frame with `dt = frame_ms/1000` s; type-0 bodies are split into up to 20 sub-steps.
* **Forces per sub-step** (`dt_s` seconds):
  * gravity: z-acceleration `+= _DAT_009DD75C * g` with `_DAT_009DD75C = -9.81` and `g = Physis+0xB16` (3.0), i.e. **-29.43 physics-units/s^2**,
    or the rider unit's `+0x243` (int) as `g` for kind 0xB; the static vector `0xC11CF5C3` = -9.81 is used for the slope term
    `dot(gravity_vec, ground_normal)`.
  * **Unit system**: `K = _DAT_009CF780 = 46.1` world units per physics unit (1 physics unit ~ 1 m). `_DAT_009DD768 = 0.02169197 = 1/K`
    (also the literal `0x3CB1B35E`), `_DAT_009D7490 = 21.69197f = 1000/K` (float; read as float in 0x52DAA0/0x4F0B50). Body positions/heights are stored
    divided by K; terrain lookups multiply by K.
  * The integrator's state pointer is `local_c = body+0x142` (the "next" block), so `next+X` = `body+0x142+X`.
    *Correction 2026-10-23:* earlier readings named the blocks by `body+0xC` offsets and swapped two of them.
  * **knock-back force** (`next+0xDA` = `body+0x21C`, set by 0x6B11A0; duration `next+0xEA` = `body+0x22C`): while the
    duration is above `_DAT_009D748C` the sub-step `dt` is cut to the time left and the acceleration is
    `vec * (1 / mass)` (`body+0x27A`, 50.0 for units) on all three axes; `+0xEA -= dt_s*1000`; sets `next+0xF2 = 1`.
    0x6B11A0 also clears the walk block, 0x6B10A0 clears this one.
  * **push** (`next+0xF4` = `body+0x236`, 0x6B12A0; duration `next+0x104`): velocity target
    `a = (vec - v) / dt` with `a.z = 0`; sets `next+0x10C` ("ran"), which clears on the first sub-step after the
    duration ran out.
  * **walk** (`next+0x10E` = `body+0x250`, 0x6B10A0; duration `next+0x11E`): velocity target like the push, applied
    only when the push did not run this sub-step and the slope factor `local_f0 > 0.9`; sets `next+0x126`. On the
    first sub-step after it ran out (`+0x11E <= 0` and `+0x126 != 0`) with `body+0x2A2 == 0` the horizontal velocity
    is zeroed (`local_120 = local_11c = 0`). With `+0x2A2 != 0` the knock-back direction is blended instead (TODO(re)).
  * **friction**: `d = 0.5 * mu * N * dt * grounded` (capped at 1) with `N = |dot((0,0,-9.81), ground_normal)|`,
    `mu = 0x6AA9E0(body, 0) = 1.0` when every velocity component is within `_DAT_009D748C`, else
    `0x6AA9E0(body, 1) = 0.9` (layers 1/2; 0 for other layers) and `grounded` = within `_DAT_009DD760` of the ground;
    then `v = ((1 - d) * v + a * dt) / (1 + d)` and `p += v_old * dt + 0.5 * a * dt^2` (0x6A5E42..0x6A5F53).
  * `local_d4 = 0.5 * damping_factor * speed * dt * substep_mul` is a stop-blend factor (capped 1.0) where `damping_factor` is 0x6AA9E0
    (`body+0x28A` layer 1 or 2: 1.0 when not landing, `_DAT_009CF83C` when landing; layer else 0.0); `0x6AE2A0`'s result is scaled by `1/K` (`_DAT_009DD768`).
    Epsilons: `_DAT_009D748C = 0.00015` (impulse/ground epsilon), `_DAT_009DD760 = 0.0015` (grounded band above the surface), `_DAT_009D8C38 = 0.01` (double; minimum move to test).
* **Terrain**: ground height + normal come from `0x6ADFD0` / `0x6AE2A0` / `0x6AE610 -> 0x6AE8E0` (section 10). The
  body is clamped: if `z <= ground + _DAT_009D748C` the body is on the ground, `vz = 0`, `z = ground`, `on_ground(next+0xD4) = 1`;
  within `_DAT_009DD760` above the ground it counts as grounded (`local_1b4 = 1`); a ground normal with
  `nz/|n_xy| > 0` (0x6ACCF0, steepness) resets the normal to `(0,0,1)` for kinds in the unit set {9,10,0xB,0xD,0xE,0xF,0x10,0x11,0x13}.
* **Unit-vs-unit collision** (0x68DE40, section 10): after the move a probe `p = pos + move` is tested; on a hit
  `body+0x298 = 1`, `body+0x29A = other_body_id`, and for kind 0xB (hero/horse rider) the **other** body gets
  `+0x298 = 1` too, so both stop. If both bodies are "unit" kinds and the hit unit is not already rolling
  (`+0x246 <= 0`, `+0x24E == 0`) the mover pushes it sideways: `push = normalize(cross-perp(move, delta_pos)) * 4 (...)`
  through `0x6B12A0` (impulse C of the victim for `DAT_00A500BE << 2` ms). Combat knock-down therefore is a physics
  impulse, not a script action.
* After integration the "next" state is copied to "current" (0x4D dwords + one short) and, if the position changed, the
  XY move is validated again with 0x68DE40 / 0x692C10 (slide along the blocker; reject if blocked).
* After a successful move `body+0x216++` (frames since landing) and, when colliding with a unit that is moving fast
  (`|v| >= _DAT_009DD748` or hit cooldown) the game triggers a hurt animation: `0x501190(0x23, 0, 1)` (anim id 0x23) or `0x509000`.

### 8.2 Projectile integrator `0x6A8060`
Same structure for kinds 2-5 and 0x12 (arrows, cannon shot, stones): gravity z-accel `_DAT_009DD75C * g_body`, drag through
`0x6B1DD0`-style scale, ground hit via 0x6A9570/0x6AE8E0, unit hit via 0x68DE40 and 0x693750 (+0x4FBC90 = damage on hit), landing
sets `actor+0x35 = 1/0` (visible-under-terrain test via the plasma sampler 0x4C7B70 with the checkerboard parity of `ftol(x/cell)+ftol(y/cell)`).
The tuning table for these bodies is the `[THROWI]` section of `Gameplay.res` (see [gameplay.md](../../formats/gameplay.md)):
per kind `_TL` (time limit ms), `_DL` (distance limit), `_M` (mass), `_MXD`/`_MND` (max/min distance), `_FC` (force constant), `_TR`, `_AV`
(average velocity), `_GL` (gravity level), `_ZE`: e.g. `ARW` TL 7000, DL 3000, M 1.0, FC 200, GL 0; `PROJ` TL 7000, DL 3000, MXD 4000, FC 30000, AV 1800, GL 0.5;
`EXPL` as PROJ plus TR 150; `STM` (stone man) TL 8000, DL 4000, MXD 200, MND 20, TR 100, FC 15000, AV 750, GL 3; `STC` TL 20000, DL 20000.
TODO(re): which handler maps these keys to the body `mass`/impulse at launch (not in the export).

## 9. Unit movement entry points

Unit fields: see [Physics.md](../structs/Physics.md) "Unit movement fields".

| Address | Proposed name | Purpose | Conf |
|---|---|---|---|
| 0x52C490 | unit_update_phase(unit, phase) | per-unit dispatcher called from the scheduler 0x51E280 (priority classes in `unit+0x22`, from 0x508880; closest/most important units updated every frame, far ones round-robin with per-class budgets `local_8+0x58..0x6E`). Phase 0 = pre (anim/path bookkeeping, restores the volume type), 1 = main (turn + path follow + root motion), 2 = late (enables/disables the body depending on the AI state `unit+0x215`), 3 = tail | R |
| 0x51E280 | unit_update_all (scheduler) | not documented here (unit page); the interesting detail: it calls phase 0 and phase 1 for every unit of the active team (`unit+0x335 == DAT_00A45F3E`) | R |
| 0x4F2190 | unit_turn_toward(unit) -> bool done | turn step toward the target heading `unit+0x1A` (see 9.1) | R |
| 0x52DAA0 | unit_follow_path(unit) | path following + root-motion velocity (9.2) | R |
| 0x4FEDF0 | unit_set_pos(x,y,z) | if the unit has no actor: store `+0xE/+0x12/+0x16` and refresh links (0x4FEE60); else `physis_move_body(actor, &pos)` (0x6AEE40) | V |
| 0x4FEE60 | unit_refresh_attachments | re-sync the attached object (`+0x339`), shadow/decal actors (`+0x2F5`, `+0x319`, `+0x325`, `+0x32D`, `+0x331`) and the rider offsets (unit-res `+0x1AD` count, `+0x1B1` offsets xyz*0xC, `+0x1B5` heading offsets) after a move; places riders on the ground with 0x6AE8E0 | R |
| 0x4FF870 | unit_set_heading(rad) | with an actor: also rotates the linked rider (`+0x1E2`) and calls `physis_set_facing` (0x6AF190); stores `+0x1A` on success, returns 0 when the facing change is blocked | R |
| 0x50ABD0 | unit_set_degree(deg) | `+0x1E = deg` | V |
| 0x4FF850 | unit_get_heading | `return +0x1A` (radians) | V |
| 0x4FEDB0 | unit_get_pos(out) | copies `+0xE,+0x12,+0x16` | V |
| 0x6AEE40 | physis_move_body(actor, pos*) | no body or body inactive -> just copy the pos to the actor (0x5AC310). Else compute the volume extents (kinds 9,10,0xB,0xE,0xF,0x11: `max(extent_x, extent_y)/1.0`; kinds 6,7: `/2.0`), derive the world delta from the actor (0x60CC90), run the ground probe (0x6AE610) and the unit collision check (0x68DE40); if ground exists and no unit blocks: store the new position `+0x202/+0x206/+0x20A` (ftol) and apply it to the actor; else the move is rejected (returns 0) | R |
| 0x6AF190 | physis_set_facing(body_owner, x,y,z) | no-op (returns 1) if unchanged; else check 0x68EE50 (oriented-box overlap of the rotated volume against all neighbours: spheres 0x69BD60, boxes 0x69C0F0, capsules 0x69B390) and, when free, write `+0x1DA/+0x1DE/+0x1E2` and sync (0x6B1350); returns 0 when rotation is blocked | R |
| 0x6B1350 | body_sync_to_actor | copies the pose to the actor; for kind 0xB the front rider (`rider+0x1E6`) is placed `30.0` ahead (`0x41F00000`) and the rear one (`rider+0x1F9`) `80.0` (`0x42A00000`)/(sign) behind along the body forward vector, copying ground refs and height | R |

### 9.1 Heading / turning (0x4F2190)
* heading is stored in **radians** at `unit+0x1A` and in degrees at `unit+0x1E` (set by `set_unit_dir`, `set_unit_degree`: handler
  0x655590 converts deg->rad with `_DAT_009CF884 = pi/180` and stores both; 0x655620 stores only degrees; 0x655680 returns the
  radians `ftol`).
* `cur_deg = rad / pi * 180` (`_DAT_009CFBF8` = pi, `_DAT_009CFBE8` = 180). The signed difference to the desired heading is computed with
  the vector helpers at 0x577190..0x577450 (a 2D heading object) and wrapped to `[0, 360)` (`_DAT_009CF9C8 = 360.0`).
* Turn speed per update: `rate = (now - unit.last_update_ms(+0x2A)) / _DAT_009D74D0 * mult` with
  `mult = 1.0` for unit-res class (`unitres+0x118`) in {4, 8, 9, 10, 0xB, 0xD}, `2.0` for class 5, `5.0` for class 6, otherwise **20.0**; clamped to
  `25.0` degrees when the unit priority class (`+0x22`) is < 2, to `90.0` (`_DAT_009CFBA8`) otherwise. The turn goes the short way
  (if the CCW distance is >= 180 turn clockwise); new heading = `cur +/- min(rate, delta)`, converted back to radians and stored with `unit_set_heading`.
  Returns true when the target heading has been reached (`rate >= delta`).
* Group formations (`unitres+2 == 8`, riders with the unit-res list at `+0x1AD/+0x1B1/+0x1B5`): each rider is re-placed at
  `leader_pos + R(leader_heading) * offset_i` (offset table at unit-res `+0x1B1`, heading offset `+0x1B5` degrees), its ground z recomputed with 0x6AE8E0
  and its animation set to 0x56/0x54 (formation march) or 10 (idle); this is how an escorted cart/ladder crew keeps its shape.

### 9.2 Path following (0x52DAA0, `unit+0x384 <= 0`, i.e. not linked to a carried object)
* A route (list of 2D waypoints) lives in a path object returned by 0x66E8D0; `0x66E8A0` is its length; the current index is the short at
  `unit+0x3E6`, a second short at `+0x3E8`. `unit+0x3C9` = track record id set by `set_unit_track` (0x653300 -> 0x427B50: `track_find_by_name`
  0x4EC320 on the 0x52-byte [UnitTrack](../structs/UnitTrack.md) table, linear scan with `strcmp`, returns the track's `index`).
* Tuning globals come from the `Gameplay.res` UNIT section loader **0x51C680** (called at start): `DAT_00A3CC28 = REACH_DT = 15`, `_DAT_00A3CCD8 = WK_DIST = 50`
  (walk when the remaining path is shorter than this scale, else run; `HORSE_WD = 500` for mounts), `DAT_00A3CCDC = MAX_W_C = 30`, `DAT_00A3CD04 = INIT_W_C = 5`
  (blocked-wait retry counts), `_DAT_00A3CC74 = LD_FM_DT = 160000` (squared distance a leader must move before the formation is recomputed), `_DAT_00A3CC70 = AI_MOVE = 1.2`
  (speed multiplier for `unit+0x102 != 0` units), `DAT_00A3CD00 = MINDEXS = 30`, `DAT_00A3CBDC = MAXDEXS = 200`, `DAT_00A3CC9C = BODY_R = 32`, `DAT_00A3CC88 = FELL_DUR = 2500` ms, `DAT_00A3CBE4 = INVC_DUR = 6000`, falling-force
  keys `T_FALLF 6000`, `V_FALLF 3000`, `F_FALLR 250`, `ARR_FALL 25`, `DEF_FALL 0.2`, `BLOCKPER 50`, `BLOCKANG 70`. (Values read from the shipped `Gameplay.res`.)
* Waypoint reach radius `R = DAT_00A3CC28 = 15` (world units; compared squared); `R*5` when the unit's `+0x44`
  (action class) > 1 and the unit-res `+2 != 8`; `R*10` for riders/commanders when moving fast, `R<<1` on the last-but-one point; `R = DAT_00A3CC28`
  for unit type `+0x2A5 == 0xF`. A waypoint is reached when `dist^2 <= R^2`; then the index advances. The walk stops (`index == length`) with
  the end-of-route actions (`0x4EECA0`, reset indices, `unit+0x3FA = DAT_00A3CD04` retry delay, new order via 0x508B90 or 0x4533E0).
* Facing: if the heading error to the next waypoint exceeds `_DAT_009D0758` the unit first turns on the spot (physics velocity
  cleared: `body+0x142 = +0x146 = +0x260 = 0`). Move command animation: speed value `unit+0x40E` (float) `<= DAT_00A3CCD8` selects the walk
  animation, above it the run animation (0x501190 calls).
* **Stuck/blocked detection**: `unit+0x402` counts consecutive blocked updates (`+1` when the physics reports `+0x298` collided, `-1` otherwise, floor 0);
  after more than **500 ms** (`now - unit+0x406`) of a non-zero count the counter is reset, the retry counter `+0x3F6` is decremented and the unit
  re-plans (0x4360D0) or stops (0x508B90). The retry delay table is `DAT_00A3CCDC` (cap) / `DAT_00A3CD04` (reset value).
* **Two velocity sources** (CORRECTED in round 3):
  1. **Locomotion classes** (`unit+0x215` in {9, 0xA, 0xB, 0xE-0x14, 0x15-0x17, 0x44-0x46}) -> `0x4F0B50` (section 9.3): a **numeric speed** from the
     unit-res, not root motion.
  2. **Skill/attack animation classes** ({0x1B-0x25, 0x3A-0x3C, 0x53, 0x22, 0x15-0x17} in 0x52DAA0): displacement of the animation's root bone between the previous and
     current animation time (`0x418A00` x2 on the animation table entry), rotated by the heading (sin/cos 0x577B50/0x577B10), scaled by `_DAT_009D7490 / frame_ms`
     (= `/K * 1000 / dt`, i.e. physics-units per second), handed to the body with `0x6B10A0`; mounted classes (`unitres+0x118 == 2`) multiply the vector by 2.0.
     *Detail (2026-10-23):* the class test reads the **action** `unit+0x215`; the displacement comes from a precomputed
     **root-motion track** per (animation set, clip), not from the evaluated pose. The animation-group loader (0x4151DF / 0x415529)
     calls `0x417F20(anim_id = row+0x6E, clip = row+0x4D)` for every row whose `MOVEMENT` (row+4) is in the set above. 0x417F20 finds
     the `Bip01` bone, picks the base key bracketing the clip start (the next one when the bracket fraction > 0.99, 0x5B7C70), and
     for each key in `[start - 0.001, end + 0.001]` appends `(t - start, pos.xy - base.xy)` (positions through the `Mother` bone's
     matrix, 0x5B8910) to a 0x25C-byte record (50 × (t, dx, dy) + count at +0x258) with **0x418820**, which first drops the
     previous key when it lies within 1.0 unit (x and y) of the line from the key before it (an implicit zero key for the first)
     to the new one. It also **overwrites those `Bip01` keys' x/y with the base key's**, so the drawn pose stays in place.
     0x418A00 samples a track (before the first key: lerp from zero; after the last: the last value); 0x52DAA0 takes
     `sample(now - actor.start) - sample(now - actor.start - dt)`, rotates by the heading and passes it to 0x6B10A0 for one frame.
  Both end in `body_set_walk_velocity` (0x6B10A0: vector `+0x250`, start `+0x25C`, duration `+0x260`, end `+0x264`; clears the knock-back block `+0x21C..`).

### 9.3 Locomotion speed and velocity (0x4F0B50, called from 0x52DAA0 every update of a walking unit)

1. `class = unit+0x215` (or `+0x217` when byte `+0x357` set). Base speed `v` (physics units/s) by class: 9 -> 1.0; 10 -> `unitres+0x9C`; 0xB -> `+0xA0`;
   0xE, 0x11-0x14 -> `+0xA4`; 0xF -> `+0xA8`; 0x10 -> `+0xAC`; 0x44 -> 3.0; 0x45 -> -3.0 (backstep); 0x46 -> 5.0; 0x15-0x17 -> 0 (animation-driven).
2. Dexterity scaling for classes other than 0xE/0xF/0x10: `d = (unit.dex(+0x157)` — or the leader's, when the unit has a leader (0x4216D0) —`)`; if
   `d > DAT_00A3CD00 (MIN_DEXTERITY_AFFECT_SPEED = 30)`: `v *= ((min(d, DAT_00A3CBDC (MAXDEXS = 200)) - 30) / (200 - 30) + 1.0)` (range x1.0 .. x2.0; see gameplay.md keys `MINDEXS`/`MAXDEXS`).
3. `unit+0x102 != 0` (hero/player flag): `v *= _DAT_00A3CC70` (AI_MOVE 1.2); then `v *= _DAT_009D51E8` (= **1.3**, applies to all units).
4. Distant-unit scaling (units outside the team when `0x428B70` says "unit exists"): priority class `+0x22 >= 3`: `v *= _DAT_009D7498`; class 2 and `v < 20.0`: `v *= _DAT_009D7494` (compensate for infrequent updates).
5. Do not overshoot: for hero units with a route, `v = min(v, dist_to_next_waypoint / K * 1000 / dt_ms)`.
6. Velocity `(cos h * v, sin h * v, 0)` (h = heading); the body is created on demand (`0x6ADBF0`, mass 50.0, layer 2); for `unit+0x3B8 != 0` (physics disabled) the unit
   is moved kinematically instead: `pos += vel` toward the saved position `+0x3CD..` and stop when within `_DAT_009CF878`.
7. Otherwise `0x6B10A0(vel, now, duration)`: duration is one update for priority < 2 (near units, `max(frame_ms,...)`); for far units
   the duration is stretched `ms = frame_ms * dist / v * _DAT_009D727C` (capped to 300 ms for non-heroes, 1000 ms for heroes) so far units can be updated less often.

### 9.4 Formation placement (0x4FA0F0 formation_apply(group, force, param))
`this` = group record ([Physics.md](../structs/Physics.md) "GroupRecord"). Skipped for unit state `+0x2A5 == 0xC`. If the leader moved less than `sqrt(DAT_00A3CC74)`
since the stored anchor (`group+0x38/0x3C`) nothing happens (unless forced). Otherwise the anchor := leader position and heading, and when the formation
type byte `group+0x37 == 1` (block formation) the slots are computed: spacing 100 world units (`local_40`), first rank offset -100 behind the anchor;
columns by member count `n`: n<2 -> 1, n<5 -> 2, n<10 -> 3, n<17 -> 4, n<26 -> 5; slot `i`: `local_x = -100 - (i / cols) * 100`, `local_y = (cols-1)*100/2 - (i % cols) * 100`,
rotated by the anchor heading (`world = (cos*x - sin*y, sin*x + cos*y)`). Members are bucketed by unit-res class (`unitres+2`: 1,4 = front line; 2,3 = second;
5,6,8 = rear) and the slots are assigned front line first, then 2/3, then 5/6/8, each member taking the best free slot via `0x45DE50(anchor+slot, slot_local, ...)`; each unit stores its
slot offset at `unit+0x412/0x416` (rotated), `+0x41E` (formation depth `(cols-1)*100`) and `+0x422/0x426` (unrotated). The leader gets depth only. Formation types other than 1 are not handled here.

## 10. Terrain, height and collision

* **Planes table** (`Physis+0x58E`, stride 0x34, count `+0xB0A`): each entry registers one terrain block "plane pair" `(A,B)` (two layers of
  walkable surface) with its origin `(+8,+0xC)` ints and an AABB; `0x6AAE70` picks, for a world XY(Z), the entry whose box contains the point and whose
  surface under (and over) `z` is closest (layer height from `0x6731D0`; ties resolved by `|dz| < _DAT_009CF788`, then prefer the one under the foot).
  `0x6AB1B0` finds the first polygon edge hit inside a plane for the same query. Terrain polygon data comes from the `plane` objects
  (0x637DA0 get, 0x66B9F0 count, 0x664180/0x6645B0 sample height / slope, `0x66CAE0(plane, dx, dy)` = height at local offset).
* **`0x6AE8E0 physis_find_ground(&plane_ref, &layer_ref, layer, x, y, z, ?, lastZ, kind)`**: first re-tests the previous plane (`*param_2`) up to 4 times
  (the 4 probe offsets), accepting it if the ground is below the foot (`ground - lastZ <= 0` or `ground < z`); otherwise searches the table (`0x6AAE70`), then
  `0x6AB1B0`; if the best hit is within `_DAT_009DD790` of `z` it is kept as a fallback. Returns 1 when ground exists.
  `0x6ADFD0`/`0x6AE2A0` wrap it and produce `(height, normal)`; no ground -> `height = 0` (normal (0,0,1)) when `Physis+0xB1C == 0`, else the void value **-5000.0**.
  The ground normal comes from `0x6645B0` and is zeroed to (0,0,1) when `0x6ACCF0` (slope) says "walkable".
* The checkerboard-triangle height sampler used for object placement and the shadow test is the Plasma sampler (0x4C7B70, see
  [Plasma.md](../structs/Plasma.md)); the physics step uses the `plane` samplers above, not the height grid.
* **Unit-vs-unit / unit-vs-object test `0x68DE40(physis, body, delta, &moveVec, &dist)`**: skipped when the move is smaller than `_DAT_009D8C38`.
  Builds a search box = body volume expanded by `|move| + _DAT_009DD6D0 + _DAT_009D5334` (kind-1 sphere adds radius; kind 2 box adds half-diagonal
  0x4256D0; kind 3 capsule adds radius), gathers the sector list (0x429090) and iterates three entity lists: units (0x54EEC0/0x54EF40), items (0x54F510/0x54F530)
  and facilities/other objects (0x54F960/0x54F980). Exclusions: self; bodies of kind 2, 3, 4, 5, 0x12 (projectiles); items of kind 8-0xC; dead/invisible. A candidate within `2*extent` in X and Y is tested with
  0x6A1BB0 (swept shape test). On a hit it sets `body+0x298 = 1`, `body+0x29A = other unit id` (only when the other is a kind-0xB unit), and for kind-0xB movers shrinks the volume radius by `_DAT_009CFC78` when the
  clearance was 0 (so a stuck hero can slip through). Returns the other unit's id (0 = free).
* **Volume overlap (static pair test) `0x6A31D0(volA, volB)`**: dispatch on the two `actor+0xB8` types: sphere/sphere 0x69D540, sphere/box 0x69D5D0,
  sphere/capsule 0x69D210, box/box 0x69E4E0 (via local-space matrix product 0x6A30C0), box/capsule 0x69D390, capsule/capsule 0x69D160; every shape is
  transformed by the actor matrix (`actor+0x8C`) and scaled by `actor+0x28`. Type 0 never overlaps.
* **`0x68EE50 physis_rotation_blocked`**: for box volumes (type 2) builds the 8 corners of the rotated box (+ the swept corners when the turn is large: `0x68FC3F` branch), inflates by
  `|box| * _DAT_009D72A4 + _DAT_009CFCA0` and tests against units, items and objects exactly like the translation test; spheres and capsules never block a rotation.
* **Block flag**: `body+0x298` (blocked), `body+0x29A` (blocker id); `set_charmove_detect_block_flag` is **not** this (section 11).

## 11. Builtin handlers (docs/scripting/BUILTINS.md ids)

| Id | Name | Handler | Behaviour (all: first `0x428B70(unit)` == 0 means "unit exists", else return 0/false) |
|---|---|---|---|
| 70 | set_unit_track(unit, name) | 0x653300 | `unit_set_track` 0x427B50: requires the unit to have a linked object (`+0x339` and its `+0x5C`); `unit+0x3C9 = track_find_by_name(name)` (0 when unknown). Returns 1 if the unit exists |
| 71 | unit_track_to_id(unit, name) | 0x653360 | 0 if the unit has no linked object or `name == 0`; else `track_find_by_name(name)` (the track `index`) |
| 72 | is_unit_track_exist(name) | 0x653400 | true if **any** live team/group record (0x420240 list; record byte `+1` set) has an entry in its unit array (`+0x7A`, count `+0x72`) with flag `+0x43` and a linked object (`+0x5C`) for which `track_find_by_name(name)` succeeds |
| 73? | set_unit_pos(unit,x,y,z) | 0x6554F0 | `0x4FED00(DAT_00A45F3E)` (set the unit's "current team" tag `+0x335`; if it changed detach attachments `0x43EB10` and sync) then `unit_set_pos(x,y,z)`; returns the unit id. (id per BUILTINS.md) |
| - | set_unit_dir(unit, deg) | 0x655590 | `unit_set_heading(deg * pi/180)` and `unit_set_degree(deg)` |
| - | set_unit_degree(unit, deg) | 0x655620 | only `unit_set_degree` (stores `+0x1E`, no physics rotation) |
| - | get_unit_dir(unit) | 0x655680 | `ftol(heading_rad)` (truncated radians, as the original) |
| 149 | set_formation_pos(unit, x, y, z, deg) | 0x658810 | needs the unit to have a group (`+0x33`) or a leader (`+0x37`); `group = leader's +0x33`; writes into the group record: `+0x38..0x40 = (x,y,z)` target formation anchor, `+0x44 = deg*pi/180`, `+0x54 = +0x44`, `+0x48..0x50 = +0x38..0x40` (copy), then `0x4FA0F0(1, 0)` (formation recompute/apply, 2.5 KB). Returns the unit id or 0 |
| 348 | enable_unit_physic(unit, on) | 0x65D070 | 0x5102A0: `on == 0`: `unit+0x3B8 = 1`, disable the body, remember pos at `+0x3CD..`; `on != 0`: `+0x3B8 = 0`, create/enable the body (`mass 50.0 = 0x42700000`, kind `unit+0x102`, layer 2) |
| 349 | enable_unit_bounding_volume(unit, on) | 0x65D0D0 | `on == 0`: `actor+0xB8 = 0`. `on != 0`: copy the volume from the unit-res (`unitres+0x1C3` = type; type 1 sphere `+0x1C7..0x1D3`; type 2 box 6 floats `+0x1D7`; type 3 capsule 5 floats `+0x1EF`; type 0 = use the model's own bounding sphere from the actor def `def+0x34..0x40`) |
| 350 | enable_unit_team_bounding_volume(unit, on) | 0x65D350 | `enable_unit_bounding_volume` for the unit and every member of its group (`unit+0x33`: byte `+2` = count, shorts from `+3`) |
| 353 | unit_dist_to_track(unit, track, last) | 0x65D6F0 | 0.0 if the unit has no linked object; else Euclid XY distance (sqrt, 0x4256D0) between the unit position (`unit+0xE/+0x12`) and the first (`last==0`) or last (`last!=0`) point of track `track` (`points + (n-1)*0x54 + 8`) |
| 380 | get_charmove_mode | (not in the export) | see section 12 |
| 391 | set_charmove_detect_block_flag | (not in the export) | see section 12 |

## 12. CHARMOVE

`CHARMOVE` has exactly two users: the `Gameplay.res` section lookup `0x4DE530` (`gameplay_section_get("CHARMOVE", ...)`) and the loader `0x454090`,
called once at start from `0x46F350`. It reads 8 keys into globals (values from `Resource/Gameplay.res`, section `{CHARMOVE}`, verified against the shipped file):

| Key | Global | Type | Value | Use (RE or inferred) |
|---|---|---|---|---|
| `AIM_R` | `0xA3373C` | float | 25.0 | aiming radius (direct control) |
| `AIM_MXD` | `0xA33758` | float | 2500.0 | aiming max distance |
| `AIM_HA` | `0xA33754` | float | 60.0 | aiming half angle (deg) |
| `TURN_ANG` | `0xA33740` | float | 6.0 | max horse/mount turn angle per update (deg) |
| `JUMPMAXD` | `0xA3374C` | int | 100 | jump button max duration |
| `JUMPMIND` | `0xA33748` | int | 100 | jump button min duration |
| `CHAINPRO` | `0xA33744` | int | 200 | chain attack timing proportion |
| `CRI_TIME` | `0xA33750` | int | 10000 | critical-hit timeout (ms) |

So "charmove" is the **direct character control** (player drives one character with keyboard/mouse; vs RTS command mode, see GLOSSARY).
`get_charmove_mode` / `set_charmove_detect_block_flag` handlers (0x65F... ids 380/391) were not in this export: TODO(re). The collision
"blocked" state is `body+0x298` (section 10), **not** a charmove detect flag; the builtin presumably toggles whether the control code samples it.

### 10.1 Wall slide (0x6A9570)
`slide(body, ..., plane_ref, layer_ref, from*, to_x,to_y,to_z, impulse_out*, step_len)`: when the straight move was rejected by terrain: if the target equals the source it just
stores the plane refs. Otherwise both points are converted to plane-local physics-space (`*K - plane origin`) and tested against the plane's wall polygons/edges (`0x664C60` segment vs
edge for point-like volumes, `0x671FB0` swept circle of radius `extent` otherwise; kinds with extent 0 use polygon edges). The blocking wall's tangent gives the slide direction:
`slide = normalize(wall_tangent)`, flipped if `dot(desired, slide) < 0`; its magnitude is the current impulse length (knock-back `next+0x10E`, else walk `next+0xF4`), scaled by `|dot|`
(`FUN_007b1f6b` = fabs). The new target `from + slide*step_len` is re-validated with `physis_find_ground` (`0x6AE610`); the step is repeated with longer multiples (2..99)
while the displacement stays under `_DAT_009DD780`. If the direct slide is impossible the direction is rotated about Z by +-0.01 rad, alternating sign and growing by `_DAT_009CFBB4`
up to `_DAT_00A6A4E8 / 2` (a half-angle) until a ground-valid direction is found (returns 0 when none). On success: `next+0x10E = slide`, `body+0x2AC = 1`, the actor facing is turned toward the slide
when `|dot| > _DAT_009DD770` (the player unit also updates the camera yaw via 0x563DC0/0x563C80/0x423FE0), returns 1.

### 10.2 Other round-3 findings
* `0x6AD900(physis, short on)` = `physis_set_enabled`: `Physis+0xB12 = on`. Called with 1 from every scenario/world start path (battle `0x41BF40`, `0x41C810`, `0x41DFE0`, `0x41EA60`, hud `0x4797A0`), right after the world and units are created, so physics runs in all in-game states.
* `0x66E8D0(route, index)`: index 0 -> `route+0x1D` (the route's head/current point); 1..n -> point `n+1-index` of the point array (last-in first-out ordering); 0 when out of range. `0x66E8A0` = point count.
* `0x46DE90` ctor: still shows only `Flock+6 = arg8`; no callers of `0x46EDE0` exist in the exported code (flocks are probably not created by the shipped S1 scripts: TODO(re) check script builtin handlers).
* `0x6A1BB0 (swept shape test)` and `0x693750 (projectile hit test)` call the pair tests 0x69xxxx; geometry only, not described further.

## 13. Open questions

* TODO(re): handlers of ids 380 and 391 (`get_charmove_mode`, `set_charmove_detect_block_flag`).
* FINAL rodata (read from the executable): `_DAT_009D51E8 = 1.3` (global locomotion speed scale in 0x4F0B50; also the alpha rate in 0x4ACE90), `_DAT_009D7494 = 1.4` (class-2 distant boost when v < 20), `_DAT_009D7498 = 1.8` (class >= 3 distant boost), `_DAT_009D727C = 0.8` and `_DAT_009D7280 = 300.0` (far-unit velocity duration factor and non-hero cap in ms), `_DAT_009CF878 = 5.0` (kinematic arrive distance), `_DAT_009DD780 = 0.02169197` (double, = 1/K, slide displacement limit), `_DAT_009DD770 = 0.707` (double, slide turn-to-face threshold on |dot|), `_DAT_009CFBB4 = 0.2` (slide search angle growth), `_DAT_009CF83C = 0.9` (landing damping factor), `_DAT_009DD790 = 30.0` (double). `_DAT_00A6A4E8` is .bss (runtime, unwritten in the exported code). Locomotion speed therefore = class speed x dex factor x 1.2 (heroes) x 1.3.
* Pages final for the movement rounds.
* TODO(re): unit-res fields `+0x9C/+0xA0/+0xA4/+0xA8/+0xAC` (speed floats) — values per unit type from `unit.res` dump; `0x4D...` writer of `unit+0x157` (dexterity stat).
* TODO(re): flock vtable slots 0x423140, 0x423120 (not exported) and the callers of `0x46EDE0`.
* TODO(re): callers of `0x46EDE0` (who creates flocks and with which 8 arguments) and the per-tick user of
  `FlockMember` fields `+0x12..+0x4A` (the vtable `0x9CF828` of `FlockMember`).
* TODO(re): re-decompile `0x46DE90` with an 8-argument prototype to recover which args fill `Flock +2, +0x2B, +0x35,
  +0x37, +0x3B..+0x52`.
* TODO(re): `0x4AE390`, `0x4AF590`, `0x4AF5D0` (siblings of the attention-call code) and the writer of `rec+0x3F`.
