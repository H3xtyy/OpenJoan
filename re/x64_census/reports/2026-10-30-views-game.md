# views-game audit (2026-10-30)

fixed (all objcmp SAME on linux-x86 unless noted; x64 Clang build clean):
- fixed: src/game/lightsys.cpp:354 Light::trigger LightTrigger -> Order (`current.kind/.target/.radius` = view +0x1d/+0x22/+0x26; Order derives DynArray so `current` moves on x64, the view did not; callers: ai_behav kTable, `this` is the Order) (SAME). Likely explains "scripts/AI orders not running" on x64.
- fixed: src/game/gfile3.cpp:1909,3956,4015 MsgEntry load/clear/save LogMessage -> MsgEntry (text_len/extra_len/text/extra; 0x1f-byte record, fieldwise save already uses MsgEntry) (SAME)
- fixed: src/game/world_s.cpp:5613 Owner::clear_54df60 ObjChain -> WorldSector (nodes_head +0x62, nodes_tail +0x5e; WorldItem::sector_next chain) (SAME)
- fixed: src/game/unitres.cpp:918 ModelRef::release ModelRef -> UnitType (model_a +0x68, model_b +0x6c, refcount +0xb0; same function already used UnitType) (code identical, only a CSWTCH.N label number differs)
- fixed: src/game/animgrp.cpp:702 ModelRefs::get ModelRefs -> AnimBuf (object 0xa5d0f8 is bss `obj:AnimBuf`); stride 0x21b constant -> sizeof(jo::ModelSlot) (ModelSlot holds pointers, so the array stride is 0x21b only on x86) (SAME)
- fixed: src/game/gamefil2.cpp:583 Calendar advance_476bb0 Clock -> Calendar (time_prev +0xc, time_of_day +0x1c; `this` calls as_calendar(this)->next_day(); object b_a47ba8 obj:Calendar) (SAME)
- fixed: src/game/unitures.cpp:303 Counter::step Counter -> UpgradeListEntry (cur +4, max +2; same function already used it) (SAME)
- fixed: src/game/itemres.cpp:1307 ItemDef::is_soldier UnitDef -> ItemDef (class_id +0, read as int16 as before) (SAME)
- fixed: src/game/t_scrres.cpp:417 TextureList::find TextureList -> AnimDb (records +0, count i16 +4; layouts.json AnimDb evidence "TextEffects/ScrRes same object", bss b_a47c01 obj:AnimDb) (SAME)
- fixed: src/game/sitemres.cpp:115 SellItemRes::load helper SellItemRes -> SItemRes (this = bss b_a501e0 obj:SItemRes; count +0, entries +4) (SAME)

requests: none.

self-consistent (bss-typed object, or the view is a base/element class of the real object; offsets are pointer-free or at 0):
- AnimRes (animres/animscr/modelres), Credits, RaceRes (raceres), IniFile (gameplay), Talk (nation2: `msgs`): all bss-typed with their own class (kAnimRes_a58cb8, b_a52968, b_a52758, b_a3ce5a, b_a48f08) and carry their full members; no second layout is applied.
- AnimBuf (animscr AnimBuf::release): bss kModelRefs_a5d0f8 obj:AnimBuf, 7-byte object.
- CamTracks (camres zones_find/prepare/find): CamRes (size 9, new(9)) and CameraTracks derive from CamTracks, so tracks +0/count +4 agree on both architectures.
- HeightGrid (plasma): base of HeightMap (new(0x14)); only base members (+0/+2/+4) are read through it.
- Units (camman list4_get/empty_at): hand-written struct in original/tables.hpp, bss kEffects_a460a0 obj:Units.
- Occluder (unitures set_70/set_corner): element class of Cell::occluders (ptr:Occluder), scalars only (floats, one i32), no pointer before +0x70.
- Counter (battle at_least_10 on g_scenarios): a plain 4-byte int in .bss, not a struct; Sound u32_0 (i_wall:442): scalar at +0.

unresolved: none. The zero-use views of the list (Table, Track, Screen, Res, Sound, ...) are only facade method calls (`as_x(p)->method()`), no field is read through them in these files.

Note: pre-existing -Wunused-const-variable warnings (animgrp.cpp:605-606, unitres.cpp:908) are not from this change.

x64 build: clean (0 errors; 10 touched files rebuilt)
objcmp: 9 SAME, 1 code identical (unitres.cpp), 0 DIFF
