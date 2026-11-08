# Views audit: group views-interfac-en (2026-10-30)

Priority item (shutdown crash, Popup::free_gpopup 0x006d3c60): object = .bss `b_a52df8 obj:MessageBox` (0xe2a bytes). The `Popup` view's `image` (void*, 0x448) is 8 bytes on x64 and overlaps MessageBox `x`/`y` (0x44c, set to 0xffff at init) -> texture=0xffff01c9`00000000. MessageBox has no field there (`text[1032]` at 0x44 swallows 0x444..0x44b).
- fixed: src/interfac/i_buy.cpp:2865-2885 Popup::set_text / Popup::set_title Popup(text,title) -> MessageBox(text_box,font) (0x454/0x450 Font2d*, same pointer-ness; objcmp code identical)
- request views-interfac-en-1: MessageBox: shrink `text` to char[0x400] at 0x44; add `u32_444` u32 @0x444 (choice/result), `image` ptr:void @0x448 (TextureManager::load result), keep x/y @0x44c/0x44e - evidence: set_text strncpy 0x3ff/memset 0x400 (so text ends at 0x444), get_444 0x006d5370, load_gpopup 0x006d3be0 (used by i_buy.cpp load_gpopup/free_gpopup/get_444, TODO(x64) left at each). Then retype those three to jo::MessageBox and drop Popup.
- request views-interfac-en-1 (same round): ChoiceDialog view (i_buy.cpp Weather::start_any_6d5120 0x006d5120) is the same MessageBox object: button_mask = MessageBox.state@0, choice = u32_444, buttons 0x458/0x79e/0xae4 which become 0x460.. on x64 in MessageBox but stay 0x458.. in the view. After the request: rewrite as jo::MessageBox (state, u32_444, button0..2). TODO(x64) left.
- Other Popup-like users: Popup::set_first/reset_6d3b80/ctor-ish already use jo::MessageBox. Popup:: facade labels (poll_buttons etc.) are method calls only.

Per-site results
- fixed: src/en3d/actor.cpp:603 Part::set 0x005acf60 Part -> Actor (scene=nullptr, scene_params0=v; was 4-byte stores into pointers; objcmp SAME)
- fixed: src/en3d/actor.cpp:1880 Attachment::position_9 0x005aee40 Attachment -> Billboard::pos (pos@0x9 sat behind the hidden `plane_` ptr; x64 offset 0x11 not 0xd; SAME)
- fixed: src/en3d/actor.cpp:1884 Attachment::position_44 0x005aee80 Attachment -> EffectState::pos (SAME)
- fixed: src/interfac/i_buy.cpp:2627 Buy::load 0x006c9060 Buy -> BuyScreen::tex (caller set_open runs it on b_a4fc98 BuyScreen; code identical)
- fixed: src/interfac/i_buy.cpp:2718 Res1::free_6d66f0 Res1 -> InventoryScreen::tex (b_a488e0; code identical). Other Res1 uses (i_sms/i_quit/i_qsave) are not in my files.
- fixed: src/interfac/i_letter.cpp:946 Obj6e::get_6af 0x006e0df0 UnitAttrScreen -> UnitAttScreen::u32_6af (b_a56398 obj:UnitAttScreen; SAME)
- fixed: src/interfac/objmenub.cpp:181 SortedNames(snd)->count -> WaveRes::index (snd is the WaveRes entry find_select returns; +4 is behind the vtable ptr, 8 on x64; i16 cast kept; SAME)
- self-consistent: Jpeg - b_a69b40 is typed `Jpeg` in bss.inc (+4 tail = 0xc), ctor/free/uses all in jpeg.cpp
- self-consistent: Track - real element class of AnimDef::tracks (Track*), sizeof(jo::Track) used for alloc/stride in animdef.cpp
- self-consistent: CloudLayers - b_a411a8 obj:CloudLayers (bss-typed)
- self-consistent: ScriptLexer - stack object sized sizeof(jo::ScriptLexer) (esstate2.cpp:589), members StringPool/DynArray declared
- self-consistent: WidgetChild (0x62b6a0) and Flag3 (0x6ca000/20) - `this` = &Widget::child; Widget is pack(1) so child+4 = i8_8 and child+3 = enabled on both x86 and x64
- self-consistent: Bitmap (mousecr word0/word2) - i16 header at +0/+2, nothing wider before them
- self-consistent: PtrList (widget2d.cpp add/remove/free 0x00634be0/cf0/dd0) - no caller anywhere in src (no fn:: use outside fn_names), only layout applied to it
- Views with 0 jo::self uses (Table, Pool, Counter, Reader, Sound, Cursor, Screen, Units, Textures, ...): only used as method-facade receivers (as_xxx(p)->fn()), no field read in my files.
- unresolved: none

x64 build: clean (only pre-existing platform/win32_types.hpp anonymous-struct warnings, not my files)
objcmp: 4 SAME (actor, i_letter, objmenub; i_buy is "code identical", 0 data diffs) -> 3 SAME, 1 code identical, 0 DIFF
