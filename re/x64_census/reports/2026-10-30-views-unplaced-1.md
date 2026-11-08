# Views audit: group unplaced-1 (2026-10-30)

Fixed (objcmp on the x86 reference build against build/objcmp-base):
- fixed: src/game/_unplaced/animscr_tex.cpp:43 Texture::release ScriptTexture -> Plane (vertex_buffer/index_buffer/resource at +0x90/+0x94/+0x98; caller uinput.cpp:2931 passes Planes) (SAME)
- fixed: src/game/_unplaced/i_obj0e.cpp:52 Res13::free_all_491770 Res13 -> Panels (source_a/b/c/d at +4/+0x10/+8/+0xc, now `Sound3D*`), and the slot loop `u32* e` -> `StreamSlot* e->source` (pointers were held in u32: truncation on x64) (DIFF reviewed: same operations and constants, only loop block order differs from pointer-typed null tests). Likely explains the dialogue-sound symptom (sndplay.cpp Sounds::shutdown calls it).
- fixed: src/game/_unplaced/i_unitatt.cpp:~695 Screen::open_level UnitAttrScreen -> UnitAttScreen (u8_6ae, u32_14, level -> u32_6af; the view hid u32_8 ptr before +0xc) (code identical; only __LINE__ names)
- fixed: src/game/_unplaced/i_qsave.cpp:197 QuitPopup::load_image QuitPopup -> QuickBar (0x6e4540 sits between QuickBar::reset_6e44b0 and Res1::free_6e45b0, loads "QSAVE" into the QuickBar texture at +0) (SAME)
- fixed: src/game/_unplaced/i_endscr.cpp:140 Overlay::tick Overlay -> EndScreen (+0x107e is EndScreen::open; EndScreen has 8 pointers and 5 Buttons before it, the view had none) (SAME)
- fixed: src/game/_unplaced/gamedraw.cpp:~412 PathGrid::find_path_4d3790 PathGrid -> PathGraph (node_count/nodes/u32_4e at +0x46/+0x4a/+0x4e, the 0x52-byte TrackRes row) (SAME)
- fixed: src/game/_unplaced/dlgtable.cpp:468 Pool::sweep Pool -> SortedNames (count i16 at +4 after a pointer; the sweep indexes b_a59a72 SortedNames; the Pool view put count at x64 offset 4 instead of 8; no ported caller) (SAME)

Requests:
- request unplaced-1-1: CombatScreen +0x0 tex_iattcomb ptr:Texture, +0x4 tex_commele ptr:Texture (replace u32_0/u32_4) - combat_load_sprites 0x6ef8d0 / free2 0x6ef970 store texture handles there; real object b_a48ff0 is CombatScreen, whose u32_0/u32_4 widen differently from the CombatPanel view; i_attcmb.cpp:73,75 store 0 and :91 passes u32_4 as i32 to ScrollBar::build (retype with view) (used by i_attcmb.cpp:211 CombatPanel::load_sprites, :235 free2)
- request unplaced-1-2: UnitAttScreen +0x0 tex_iunitatt ptr:Texture, +0x4 tex_commele ptr:Texture - Ui view (load_shared_textures 0x6f6120, hud_release 0x6f61e0) is called on b_a56398 UnitAttScreen (open_level calls it on this); AddScreen has the same two fields (texture_0/4) as a competing view of that object, as do UnitAttPanel and AddScreen at +0x8/+0x19/+0x1d (not in my list) (used by i_unitatt.cpp:217,252)

Self-consistent (bss-typed as themselves, or scalar-only, or pointer at +0 in every candidate):
- Credits (b_a52968), CloudLayers (b_a411a8), HelpBox (b_a59780 tooltip), NamedTable (b_a5cd6a), Music (b_a3ce50), QuitPopup (b_a5cec8), SlideMenu (b_a55ad0; the IngameFrame view agrees on +0/+1/+0x20..+0x2c, its f32_31d/f32_3d4 are unused), MgList (b_a50530), Talk (b_a48f08), FxNameTable (same shape as the typed AnimBuf at b_a5d0f8: ptr, then i16 at +4)
- ScriptLexer (escomp, escomp_hd, eslexer): prefix of Tokenizer (vtable, StringPool at +4, tokens at +0xc); every user (also uti3d/esstate2.cpp, tokenize.cpp) goes through ScriptLexer/Tokenizer
- Blink (gameover, i_vict): scalars only, reached by address of a typed member; ErrorLog (globals.cpp:1527): pointer at +0, same as Error.on_fatal; WaypointList (ctrl.cpp:1011): scalars only and Place has no pointer before +0x1da (Place's slot_pos/slot_yaw/slot_unit disagree with the function's 39-float + 13-u32 layout at +0xc6/+0x162: spec round, no x64 effect); Units (itemres_head): hand-written {vtable, DynArray} with sizeof assert

Unresolved:
- camcmd.cpp:1031 List::remove_at_42b9a0 CamGroupSet: no ported caller and no class with Scene*/group_count at +0x6e/+0x86/+0x8e; the object class is unknown
- ctrl.cpp:1210 Obj::pos_4384a0 SoundSlot, gameover.cpp:424 Obj6b7d30::set_4 WidgetChild: no ported callers (dead in the port), class unknown; WidgetChild flag_4 would be Widget::child (+4 after the vtable, +8 on x64) if it is a widget
- views with 0 jo::self uses (Action, Army, Cursor, Sound, Table, Screen, ...): no use sites in these files, nothing to audit

Hazard meter (ptr_u32 / raw columns, my files): i_obj0e ptr_u32 2 -> 0, i_qsave 1 -> 0, dlgtable 1 -> 0; others unchanged.
Symptoms: dialogue sound (Sounds::shutdown / SoundList::free_all pointer-in-u32 and wrong x64 offsets); Plane::release (animscr_tex) and EndScreen::tick corrupted neighbours on x64.
x64 build: clean (the 8 changed translation units compile with one pre-existing -Wunused-const-variable `kFiles` at i_obj0e.cpp:103)
objcmp: 6 SAME, 1 code identical (i_unitatt), 1 DIFF (i_obj0e, reviewed: loop block order only)
