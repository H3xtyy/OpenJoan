// Generated (re/facades/gen2.py): typed members for the Hud functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x006f28c0 Hud::init_sliders
void init_sliders();
/// 0x006f61e0 Hud::release
void release();
/// 0x006f97d0 Hud::load_resources
std::uint32_t load_resources();
/// 0x006f98a0 Hud::free_resources
void* free_resources();
/// 0x006f9970 Hud::draw_corner_icon
void draw_corner_icon(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::Hud_draw_corner_icon, this, a0); }
/// 0x006f9a60 Hud::draw_pause_caption
void draw_pause_caption(char a0);
/// 0x00704260 Hud::draw_arrows
void draw_arrows();
/// 0x00704810 Hud::draw_objectives
void draw_objectives() { ::joan::call::thiscall<void>(::joan::fn::Hud_draw_objectives, this); }
/// 0x007076c0 Hud::blink
void blink(std::int32_t a0);
/// 0x00707b20 Hud::blink_all
void blink_all(std::int32_t a0);
/// 0x00707ce0 Hud::blink_restart
void blink_restart();
/// 0x0070e1d0 Hud::init_70e1d0
std::uintptr_t init_70e1d0();
/// 0x004303f0 Byte::get_4303f0
std::uint32_t is_open();
/// 0x0065ee00 Obj::set_word_11
void set_word_11(std::int32_t a0);
/// 0x006fe240 Obj::reset_6fe240
std::uint32_t clear_portrait();
/// 0x006fe280 ItemPortrait::set_item
void set_item_portrait(std::int32_t a0);
/// 0x006fe340 UnitPortrait::set_unit
void set_unit_portrait(std::int32_t a0);
/// 0x006fe480 Obj6fe::nop2
void nop_6fe480(void* a0, void* a1);
/// 0x006fe4a0 Obj6fe::set_30
void set_health_state(std::int8_t a0);
/// 0x006fe500 Portrait::nothing
void nop_6fe500(void* a0, std::uint32_t a1);
/// 0x006fe660 ExpCounter::draw_6fe660
void draw_exp_counter();
/// 0x006ffd90 Obj6ff::hit
std::int32_t hit_portrait(std::int32_t a0, std::int32_t a1);
/// 0x006ffed0 LevelBanner::update
void update_level_banner();
/// 0x00702740 ComboCounter::draw_702740
void draw_combo_counter() { ::joan::call::thiscall<void>(::joan::fn::ComboCounter_draw_702740, this); }
/// 0x00703680 State::reset_time
void start_level_banner();
/// 0x007036f0 Obj::mark
std::uint32_t start_combo();
/// 0x00703750 UnitPanel::update_health_state
void update_health_state(std::int32_t a0);
/// 0x007072c0 SlotList::remove
void remove_slot(std::uint32_t a0);
/// 0x00707400 Log::push
void push_log_entry(std::int32_t a0);
/// 0x007078c0 Fade::push
void append_log_entry(std::int32_t a0);
/// 0x00707960 Fade::restart
void restart_log_entry(std::int32_t a0);
/// 0x00707df0 Obj707df0::reset
void reset_be();
/// 0x006f9b60 Hud::draw
void draw(char a0) { ::joan::call::thiscall<void>(::joan::fn::Hud_draw, this, a0); }
/// 0x00703880 Unit::siege_model_prefix
const char* siege_model_prefix(std::uint32_t a0);
