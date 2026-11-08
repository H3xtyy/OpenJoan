# Flock, FlockGroup, FlockMember (flocksys.cpp) and AttentionCall

Functions: [flocksys.md](../functions/flocksys.md). Reference build `joan.exe`. Status: V = verified (>= 2 reads),
R = one read, I = inferred. All offsets are byte offsets; fields are packed (MSVC `#pragma pack(1)`-like: 4-byte
fields sit at odd offsets such as `+0x19`).

## Flock (91 = 0x5B bytes, `new` at 0x46EDE0, zeroed by 0x46DE30)

Stored in the global registry (a `dynarrb` of `Flock*`, 1-based ids; slot freed -> 0).

| Offset | Size | Type | Name | Meaning | Evidence | St |
|---|---|---|---|---|---|---|
| 0x00 | 2 | i16 | id | registry index (`dynarr` cursor after push) | 0x46EDE0 | V |
| 0x02 | 2 | i16 | kind | `1` = flock is also listed in an owner list (`+0xA` in list `+6`); removed from it on destroy | 0x46DFA0 | R |
| 0x06 | 4 | ptr | owner_list | ctor arg 8 (`in_stack_00000020`); a pointer-array object (`+0x24` count, `+0x2C` data) | 0x46DE90, 0x5EA5E0 | R |
| 0x0A | 4 | ptr | owner_entry | pointer stored in `owner_list`; nulled after removal | 0x46DFA0 | R |
| 0x2B | 4 | u32 | member_type | copied into every member `+0xE` | 0x420420 | R |
| 0x35 | 2 | i16 | group_count | number of `add_group` calls made by create_flock | 0x46EDE0 | R |
| 0x37 | 2 | i16 | members_per_group | argument of `add_group` | 0x46EDE0 | R |
| 0x39 | 2 | i16 | group_counter | `++` on add_group (spawns only if `< 6`), `--` on remove_group (deletes only if `< 5`) | 0x46E2D0, 0x46E430 | V |
| 0x3B | 24 | f32[6] | box | `min.x, min.y, min.z, max.x, max.y, max.z` spawn AABB | 0x420420 (copied as 6 dwords) | V |
| 0x53 | 4 | dynarr* | members | `dynarr(elem 4, grow 0x1E)` of `FlockMember*` | 0x46DE90, 0x46DFA0 | V |
| 0x57 | 4 | dynarr* | groups | `dynarr(elem 4, grow 0x1E)` of `FlockGroup*` | 0x46DE90, 0x46DFA0 | V |

Gaps `0x0E..0x2A`, `0x2F..0x34` are written by ctor arguments Ghidra did not show (TODO(re) 0x46DE90 with an 8-arg
prototype: args are `i16 x5` then `u32 x3`, only arg 8 -> `+6` is visible).

## FlockGroup (14 = 0xE bytes)

| Offset | Size | Type | Name | Meaning | Evidence | St |
|---|---|---|---|---|---|---|
| 0x00 | 2 | i16 | flock_id | owner | 0x46DBA0 | V |
| 0x02 | 4 | i32 | recno | index in `Flock.groups` (written after push; `member+0xA` refers to it) | 0x46E2D0 | V |
| 0x06 | 4 | i32 | count | number of linked members | 0x46DC80/0x46DD70 | V |
| 0x0A | 4 | ptr | head | first `FlockMember` of the intrusive list (0 when empty) | same | V |

## FlockMember (127 = 0x7F bytes, vtable `0x9CF828`, ctor 0x4203E0 which memsets the whole object)

| Offset | Size | Type | Name | Meaning | Evidence | St |
|---|---|---|---|---|---|---|
| 0x00 | 4 | vptr | vtable | `0x9CF828` assigned, then overwritten by the memset (ctor order) -> 0 | 0x4203E0 | R |
| 0x04 | 4 | Actor* | actor | owned `en3d::actor`; deleted by 0x4207D0 | 0x4207D0 | V |
| 0x08 | 2 | i16 | flock_id | | 0x420420 | V |
| 0x0A | 2 | i16 | group_recno | (read by remove_group as i16 at `+0xA`) | 0x420420, 0x46E430 | V |
| 0x0C | 2 | i16 | recno | index in `Flock.members` | 0x46E2D0 | R |
| 0x0E | 4 | u32 | type | copy of `Flock+0x2B` | 0x420420 | R |
| 0x12 | 12 | vec3 | pos | uniform random in `Flock.box` | 0x420420 | V |
| 0x1E | 12 | vec3 | drift | `(+0x1E,+0x22)` = +-U(0.1,3.0), `+0x26` = 0 | 0x420420 | V |
| 0x2A | 12 | vec3 | aux_vec | zeroed (steer/heading target, user unknown) | 0x420420 | I |
| 0x36 | 4 | f32 | speed | `length(drift)` | 0x420420 | V |
| 0x3A | 2 | i16 | state_a | 0 | 0x420420 | R |
| 0x3C | 2 | i16 | state_b | 0 | 0x420420 | R |
| 0x3E | 4 | u32 | field_3E | 0 | 0x420420 | R |
| 0x42 | 4 | u32 | field_42 | 0 | 0x420420 | R |
| 0x46 | 4 | f32 | best_dist_a | 999999.0 | 0x420420 | R |
| 0x4A | 4 | f32 | best_dist_b | 999999.0 | 0x420420 | R |
| 0x66 | 4 | ptr | next | intrusive list link | 0x422F70/0x422FA0 | V |
| 0x6A | 4 | ptr | prev | intrusive list link | 0x422FC0 | V |
| 0x6E | 1 | u8 | flag | 0 | 0x420420 | R |

Unseen (no reader in the export): `0x4E..0x65`, `0x6F..0x7E`. The member update code (and with it the real meaning of
`aux_vec`, `state_*`, `best_dist_*`) was not reachable from the depth-3 roots.

## AttentionCall (HUD pop-up record; allocated by an unexported creator, freed by 0x463120)

Not a flock structure; documented here because it was exported with the family. Manager (`this` of 0x46E8D0): list
`dynarrb` of records, count `+0x36`, `+0x44` byte `front_free`. Size unknown (>= 0x5B).

| Offset | Size | Type | Name | Meaning | Evidence | St |
|---|---|---|---|---|---|---|
| 0x00 | 4 | i32 | id | dynarrb index | 0x46E8D0 | V |
| 0x04 | 4 | i32 | state | 0..6 (see functions page 3.1) | 0x4ACE90 | V |
| 0x08 | 1 | u8 | flag | alternate style: 0 = normal; != 0 = faster slide (x `_DAT_009CFC20`), only advances while name starts `"M000"` | 0x4ACE90, 0x499E40 | R |
| 0x0D | 4 | f32 | y | current vertical position (px) | 0x4ACE90 | V |
| 0x15 | 2 | i16 | target_y | slide target (px) | 0x4ACE90, 0x499E40 | V |
| 0x17 | 2 | i16 | hold | speech hold units (x33 ms) and ramp limit | 0x4ACE90 | R |
| 0x19 | 4 | u32 | voice_id | sound id for the spoken line (> 0x4000 selects the second sound table in 0x4CF0F0) | 0x4ACE90 | R |
| 0x1D | 4 | i32 | voice_started | | 0x4ACE90 | R |
| 0x2E | 401 | char[401] | name | flipbook frame name `M0001..M0009` (str401: `strncpy(.,400)`) | 0x5A11A0 | R |
| 0x3B | 4 | f32 | ramp | speech ramp 0 -> `hold` | 0x4ACE90 | R |
| 0x3F | 4 | u32 | sort_key | smaller = earlier in line (writer unknown) | 0x4AEF10 | R |
| 0x43 | 4 | u32 | last_ms | absolute ms stamp (`DAT_00A500BA`) | 0x4ACE90 | V |
| 0x47 / 0x4B / 0x4F / 0x53 | 4 each | f32 | alpha chain | four fade channels 0..255 raised in order `4F -> 53 -> 47 -> 4B` | 0x4ACE90 | R |
| 0x57 | 4 | f32 | timer | fade-out countdown (state 5) | 0x4ACE90 | R |

(`name` at `+0x2E` of 401 bytes would overlap `+0x3B..+0x57`; the string object therefore cannot be inline 401 bytes —
`+0x2E` must be a pointer-sized/`str` header and the 400-limit refers to its heap buffer. Layout above `+0x2E` is
verified, the string width is not: TODO(re) 0x5A11A0 receiver type.)
