// Generated (re/facades/gen2.py): typed members for the ObjMenu functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004dbd30 ObjMenu::open_0f
const char* open_0f();
/// 0x004dbf40 ObjMenu::open_1a
const char* open_1a();
/// 0x004dbfa0 ObjMenu::open_1c
const char* open_1c(std::uint32_t a0);
/// 0x004dc1c0 ObjMenu::open_27
const char* open_27();
/// 0x004dc2f0 ObjMenu::open_n
const char* open_n(std::int32_t a0);
/// 0x004dc3f0 ObjMenu::open_43
const char* open_43();
/// 0x004dc490 ObjMenu::open_46
const char* open_46();
/// 0x004dc4f0 ObjMenu::open_48
const char* open_48();
/// 0x004dc520 ObjMenu::open_49
const char* open_49();
/// 0x0071e800 ObjMenu::release_71e800
std::uint32_t release_71e800();
/// 0x007246f0 ObjMenu::open_brief
std::uint32_t open_brief();
/// 0x004303d0 Byte::get_4303d0
std::uint32_t is_open();
/// 0x0056e6b0 Obj::fwd_56e6b0
std::uint32_t release();
/// 0x0071d290 Obj71d290::Obj
void* zero();
/// 0x0071efc0 Mgr::reload_masks_71efc0
std::uint32_t load_masks();
/// 0x0071f690 ObjectiveMenu::draw
void draw(std::int32_t a0, std::int32_t a1) { ::joan::call::thiscall<void>(::joan::fn::ObjectiveMenu_draw, this, a0, a1); }
/// 0x00720bc0 ObjectiveMenu::draw_camera
void draw_camera();
/// 0x00723c80 Scroller::init_723c80
void build_briefing();
/// 0x00723f90 Obj::release_723f90
void free_briefing();
/// 0x00724210 Scroller::buttons_724210
std::uint32_t update_briefing();
/// 0x00724620 Dlg::release3
std::uint32_t update_briefing_widgets();
/// 0x00724670 Obj::reset_buttons_724670
std::uint32_t reset_briefing_widgets();
/// 0x00724bf0 Briefing::start
void reset_briefing();
/// 0x00724e00 Panel724e00::init
void build_army();
/// 0x00725870 GroupPanel::draw_725870
void draw_army(std::uint8_t a0);
/// 0x00727740 ObjectiveMenu::draw_extra
void draw_army_title();
/// 0x007277a0 Bar::update_7277a0
std::uint8_t update_army();
/// 0x00728290 X::layout_4
std::uint32_t reset_army_widgets();
/// 0x00728390 Roster::build_728390
void build_roster(std::int32_t a0);
/// 0x00728d20 Roster::hide_728d20
void leave_army();
/// 0x00729200 Cine::idle_camera_729200
std::uint32_t play_idle_anim();
/// 0x00729590 ItemView::refresh
void refresh_preview();
/// 0x0072a470 Obj::free_panels
void free_debrief();
/// 0x0072a620 ObjectiveMenu::draw_results_72a620
void draw_debrief(std::uint32_t a0);
/// 0x0072c740 Hero::pick_portrait
std::uint8_t update_debrief();
/// 0x0072ca30 Obj::reset_72ca30
std::uint32_t enter_debrief();
/// 0x0072ca90 Obj72::reset
void leave_debrief();
/// 0x0072cad0 Debrief::setup_72cad0
std::uint32_t collect_debrief_heroes();
/// 0x0072d060 Images4::release
void free_debrief_images();
/// 0x0072d380 ObjectiveMap::create_camera_72d380
std::uint32_t build_map() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::ObjectiveMap_create_camera_72d380, this); }
/// 0x0072d550 Obj::free_lists
void free_map() { ::joan::call::thiscall<void>(::joan::fn::Obj_free_lists, this); }
/// 0x0072d6f0 ObjectiveMenu::init_map_72d6f0
void init_map() { ::joan::call::thiscall<void>(::joan::fn::ObjectiveMenu_init_map_72d6f0, this); }
/// 0x0072e370 X::clear_list
void clear_map_models() { ::joan::call::thiscall<void>(::joan::fn::X_clear_list, this); }
/// 0x0072e480 ObjectiveMap::draw_72e480
void draw_map(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::ObjectiveMap_draw_72e480, this, a0); }
/// 0x00730150 MapView::pan_zoom_730150
std::uint8_t update_map() { return ::joan::call::thiscall<std::uint8_t>(::joan::fn::MapView_pan_zoom_730150, this); }
/// 0x00730be0 HeroModels::spawn_730be0
std::uint32_t spawn_hero_models();
/// 0x00731a60 Loader::parse_map_731a60
std::uint32_t parse_map_script(void* a0);
/// 0x00732940 Obj::release_732940
void free_reward();
/// 0x00732af0 Obj::set_camera_732af0
std::uint32_t init_reward_camera();
/// 0x00732c70 RewardScreen::draw
void draw_reward();
/// 0x007349b0 Obj::reset_buttons_7349b0
std::uint32_t reset_reward_widgets();
/// 0x00735850 Obj::release_735850
std::uint32_t free_strategy();
/// 0x00735930 Panel8::draw_735930
void draw_strategy(std::uint32_t a0);
/// 0x00736120 StrategicDlg::update
std::uint8_t update_strategy();
/// 0x00736480 Pair::update_736480
std::uint32_t update_strategy_arrows();
/// 0x007364c0 Obj::reset_two
std::int32_t reset_strategy_arrows();
/// 0x00736520 X::set_page
std::uint32_t set_strategy_page(std::int32_t a0);
/// 0x007365e0 X::reset_items
std::uint32_t enter_strategy();
/// 0x00736880 Obj::load_strategy
void load_strategy_image();
/// 0x00736900 Obj::free_strategy
void free_strategy_image();
/// 0x007369c0 ShopScreen::build_7369c0
std::uint32_t build_shop();
/// 0x00736d80 Obj::release_736d80
void free_shop();
/// 0x00736f30 Obj::set_camera_736f30
std::uint32_t init_shop_camera();
/// 0x00737030 ShopScreen::draw_bg_737030
std::uint32_t draw_shop_bg(std::uint32_t a0);
/// 0x007370b0 MapPanel::draw_7370b0
void draw_shop();
/// 0x007380b0 Scroller::handle_buttons
std::uint32_t update_shop();
/// 0x00738340 MapView::update_738340
void update_shop_widgets();
/// 0x007388a0 Obj::reset_buttons_7388a0
std::uint32_t reset_shop_widgets();
/// 0x00738b50 Panel2::layout
std::int32_t enter_shop();
/// 0x00738e30 CannonScreen::build_738e30
std::uint32_t build_siege();
/// 0x00739290 Obj::free_scene
void free_siege();
/// 0x00739540 Panel9::draw_739540
void draw_siege(std::uint8_t a0);
/// 0x0073b040 Spinner::tick
void spin_siege_model();
/// 0x0073b1c0 X::reset_model
void leave_siege();
/// 0x0073b2e0 SiegeDebrief::init_73b2e0
void init_siege();
/// 0x0073b6e0 Res2::free_1b3
void free_siege_images();
/// 0x0073a320 ObjMenu::siege_help
bool update_siege();
