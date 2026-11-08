# Views audit: group unplaced-2 (2026-10-30)

Scope: every view class of the list with a `jo::self` use (all others have 0 uses in these files). Build `build/agent-unplaced-b-x64` (Clang x64) compiles clean for my files.

fixed: src/game/_unplaced/musicstr.cpp:178 Music::play (0x586210) Music -> SoundStreams3 (`loader==nullptr` is `open==0`; same body already uses `jo::self<SoundStreams3>(this)->videos[slot]`, so `this` is the stream set, not the DirectMusic wrapper b_a3ce50) (objcmp: DIFF, reviewed: same instructions, gcc splits a `.part.0` out of play_music and reallocates registers)
fixed: partslot.cpp:195 PartSlot::clear PartSlot -> Particle (Particle +0x18 node Actor*; facade file particle_methods.inc; Particle::update uses the same field) (SAME)
fixed: pemitter.cpp:148 Owner::release EmitterSlot -> EffectKey (active->initialised +0, def->u32_141 +0x141, children->keys +0x145; facade effect_key_methods.inc, sibling set_name_e1/102 use EffectKey) (SAME)
fixed: psystem.cpp:244 Owner::release_24 PlaneHolder -> EffectState (+0x24 parts Plane*; neighbour Actor::release 0x675b30 / EffectState::update_parts 0x675e90 use EffectState.parts) (SAME)
fixed: playerctl.cpp:70 Obj::get_6ae UnitAttrScreen -> UnitAttScreen (bss `b_a56398 obj:UnitAttScreen`, callers `as_inventory(obj(d::g_a56398))->is_open()`; the real class has a BmpRes* at +8 that shifts +0x6ae on x64) (SAME)
fixed: vshader.cpp:195,1074 Effects::release_all / release_61c280 Effects (+EffectShader) -> VsCache/VsEntry (shaders=entries +8, shader_count=i32_10 +0x10, iface=VsEntry.constant_table +0xc, handle=VsEntry.u32_10 +0x10; VsCache.entries is VsEntryPtr*, iterated up to +0x10) (SAME)
fixed: vshader.cpp:420 VsConstants::set VsConstants -> VsCache (+0x14 current; facade vs_cache_methods.inc) (SAME)
fixed: sndplay.cpp:747 Defs::at Defs -> NamedTable (bss `b_a5cd6a obj:NamedTable`, +0 table WaveRes*; callers `as_sound_res_table(obj(d::g_a5cd6a))->at`) (SAME)
fixed: squadcmd.cpp:63 Records::get Records -> UnitRes (bss `kTypes_a48d88 obj:UnitRes`, +0 types UnitType*) (SAME)
request unplaced-2-1: News +0x1d u8 flag_1d, +0x1e u8 flag_1e (replace gap_1d[2]; rename freely) - News is DynArray (0x1d bytes, widens on x64) + gap_1d[2]; Action is a flat view at +0x1d/+0x1e; Action::reset_bytes 0x4b9310 is called from News dtor 0x4b92c0 (used by news.cpp:149 Action::reset_bytes)
request unplaced-2-2: Display +0x8 colours u8[0x400] (replaces gap_8[0x400]; flag@+1 is the Palette `loaded`) - Palette::load 0x60f760 `this` = g_a4f888 (bss obj:Display, same object as Display::state/device); on x64 Display.device (+4) is 8 bytes so the view's +8 overwrites it (used by surface.cpp Palette::load; also a "device pointer gets clobbered" candidate for the x64 render failures)
request unplaced-2-3: VsCache +0x18 u32_18 -> void* tint_source - Tint::set 0x61c250 stores a pointer there (facade set_tint_source on VsCache, callers en3d/scene*.cpp via g_a52597 = VsCache); u32 would truncate on x64 (used by vshader.cpp:1040 Tint::set; Tint view is flat, +0x18 sits inside `current` on x64)
self-consistent: Mgr - complete layout (vtable + faders), bss `b_a506e0 obj:Mgr`, only mlfader.cpp uses it (spec notes: fader manager)
self-consistent: NextObjectivesScreen - single byte at +0 of a 1-byte object, no gap
self-consistent: SortedNames - bss `b_a59a72 obj:SortedNames` (6 bytes: items ptr, count i16); it is the real class, used by soundres.cpp and sndplay.cpp name_get
self-consistent: Tokenizer / TokenList - Tokenizer is the vtable+strings prefix of TokenList (TokenList ctor 0x67caa0 calls Tokenizer ctor 0x684010 on this); same field order, so same x64 offsets; TokenList (vtable, strings, tokens +0xc) is a complete layout, no bss typing, frame object in psystem.cpp
self-consistent: Talk - bss `b_a48f08 obj:Talk`, u32 at +0 then msgs at +4, this = Talk (g_queue) in talk.cpp
self-consistent: Cursor (vgalock_mb.cpp, 3 uses) - 4-byte stack guard object created by the message-box callers (`u8 guard[8]`), u32 at +0 only
unresolved: none (0 jo::self uses for the remaining listed views: Order0c_a/b, SaveLoad and a template `C` are not on my list)
Hazard meter: not rerun (no raw reads added or removed); views on x64 removed: 6 classes (Music, PartSlot, EmitterSlot, PlaneHolder, UnitAttrScreen, Effects/VsConstants/Defs/Records), 3 left as requests.
Symptoms plausibly explained: Palette (request 2) clobbers Display.device on x64 (render/device failures after the palette load at start-up); Music::play read a pointer where the stream-set holds a u32 (menu music / ambient streams not starting); Effects::release_all freed garbage entries at shutdown (shutdown crash).
x64 build: clean (my files; the only warning in the build is i_obj0e.cpp:103, not mine)
objcmp: 8 SAME, 1 code identical (news.cpp), 1 DIFF (musicstr.cpp, reviewed: inliner split only)
