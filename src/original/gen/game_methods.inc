// Generated (re/facades/gen2.py): typed members for the Game functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00421b60 Game::reset_three
std::uint32_t reset_three();
/// 0x0042ca90 Game::on_mode
std::uint32_t on_mode(std::int32_t a0);
/// 0x0042f620 Game::update_mode
void update_mode();
/// 0x0042f930 Game::set_speed
void set_speed(std::uint32_t a0);
/// 0x00432b90 Game::byte_2e1
std::uint32_t byte_2e1();
/// 0x004404c0 Game::start_rts
void start_rts(std::int8_t a0);
/// 0x00442ef0 Game::rts_mode_442ef0
void rts_mode_442ef0();
/// 0x0046f090 Game::Game_46f090
void* Game_46f090();
/// 0x0046f200 Game::shutdown_46f200
std::uint32_t shutdown_46f200();
/// 0x0046f350 Game::init_once_46f350
std::uint32_t init_once_46f350();
/// 0x0046f3a0 Game::load_resources_46f3a0
void* load_resources_46f3a0();
/// 0x0046fc30 Game::load_interface_res
void load_interface_res();
/// 0x00470630 Game::reset_all_470630
void reset_all_470630();
/// 0x004709d0 Game::reset_all
std::uint32_t reset_all();
/// 0x00470d40 Game::load_arrays
void load_arrays();
/// 0x00471010 Game::end_session
void end_session(std::int32_t a0);
/// 0x004727b0 Game::copy_words_to
void copy_words_to();
/// 0x004728f0 Game::copy_word_table
void copy_word_table();
/// 0x00479720 Game::reset_479720
std::uint32_t reset_479720();
/// 0x0047b830 Game::draw_ending
std::uint32_t draw_ending() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Game_draw_ending, this); }
/// 0x0047b8d0 Game::toggle_47b8d0
std::uint32_t toggle_47b8d0();
/// 0x0047bce0 Game::draw_loading_screen
std::uint32_t draw_loading_screen() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Game_draw_loading_screen, this); }
/// 0x0047bfd0 Game::draw_credits
std::uint32_t draw_credits() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Game_draw_credits, this); }
/// 0x00499960 Game::run
std::uint32_t run(std::int32_t a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Game_run, this, a0); }
/// 0x004d48f0 Game::draw
void draw();
/// 0x004d6fe0 Game::load_fonts_4d6fe0
std::uint32_t load_fonts_4d6fe0();
/// 0x004d7890 Game::release_globals
std::uint32_t release_globals();
/// 0x004e5fc0 Game::tick_4e5fc0
void tick_4e5fc0();
/// 0x00512070 Game::scaled_a3cc90
std::int16_t scaled_a3cc90();
/// 0x0052f930 Game::process_input
void process_input_debug_menu() { ::joan::call::thiscall<void>(::joan::fn::Game_process_input_debug_menu, this); }
/// 0x005a8be0 Game::message_box
std::int32_t message_box(const char* a0, void* a1, std::uint32_t a2);
/// 0x006037f0 Game::init_display_6037f0
std::int32_t init_display_6037f0();
/// 0x006039d0 Game::shutdown_managers
std::uint32_t shutdown_managers();
/// 0x00603af0 Game::flush_loader
void flush_loader();
/// 0x00603b20 Game::screenshot_603b20
void screenshot_603b20();
/// 0x004301c0 get_byte_744
std::uint8_t flag_744();
/// 0x0046f1c0 SlotOwner::reset
void reset();
/// 0x00471580 nop_471580
void log_progress(const char* a0);
/// 0x00471e20 SlotOwner::add_slot
std::int32_t add_end_condition();
/// 0x00471ef0 Obj::pop_entry
std::int32_t pop_end_condition();
/// 0x00472000 Obj::set_bit_472000
std::uint32_t set_flag_1b5(std::int32_t a0, std::int32_t a1);
/// 0x004722a0 Obj::set_bit
void set_flag_1bd(std::int32_t a0, std::int32_t a1);
/// 0x004723f0 Obj::set_bit_4723f0
std::uint32_t set_flag_1c1(std::int32_t a0, std::uint32_t a1);
/// 0x004724c0 Obj::get_bit_4724c0
std::uint32_t get_flag_1c1(std::int32_t a0);
/// 0x00472540 Obj::clear_mask_472540
std::uint32_t clear_flags_1b9();
/// 0x004725f0 Obj::set_744
void set_744(char a0);
/// 0x00472620 ShortSet::add
std::int32_t add_word(std::int16_t a0);
/// 0x004729c0 WordList::push
void push_word_728(std::int32_t a0);
/// 0x00472a90 ShortList::contains
std::uint8_t has_word_728(std::int16_t a0);
/// 0x00472b00 Obj::push_buffer
std::uint32_t push_buffer();
/// 0x00472c40 Obj::pull_buffer
std::uint32_t pull_buffer();
/// 0x0047b7c0 X::call_a50088_3
std::uint32_t update_mode1();
/// 0x0047b980 Obj::stub_47b980
void update_mode4();
/// 0x0047ba00 Obj::stub_47ba00
void update_mode5();
/// 0x0047baa0 Obj::stub_47baa0
void update_mode6();
/// 0x0047bb20 Obj::stub_47bb20
void update_mode7();
/// 0x0047bba0 Obj::stub_47bba0
void update_mode8();
/// 0x0047bc50 X::fwd_75fa50
std::uint32_t update_mode11();
/// 0x0047bd40 forward_47bd40
std::uint32_t update_mode12();
/// 0x0047be30 forward_755d80
std::uint32_t update_mode13();
/// 0x0047bf40 Obj::call_a46030
std::uint32_t update_mode14();
/// 0x0047c020 X::call_a52968
std::uint32_t update_mode15();
/// 0x004d6b90 Obj::ctor_4d6b90
void* construct();
/// 0x004d6be0 Obj::dtor_4d6be0
std::uint32_t destruct();
/// 0x004d6c50 Obj::init_4d6c50
std::uint32_t init(void* a0);
/// 0x004d7bf0 X::resolve_path
std::int32_t resolve_cd_path(const char* a0, const char* a1, const char* a2);
/// 0x004d7ca0 Cd::find_drive
void find_cd_drive();
/// 0x004d7e70 set_byte_3c
void* set_state(std::uint32_t a0) { return ::joan::call::thiscall<void*>(::joan::fn::set_byte_3c, this, a0); }
/// 0x004d7ef0 Wnd::on_message
std::int32_t on_message(void* a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x004e6fe0 Music::update_4e6fe0
void update_music();
/// 0x00472740 Obj::has_word
bool has_word(std::int16_t a0);
/// 0x00472d10 play_sound
std::uint32_t play_sound(std::uint32_t a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::play_sound, this, a0); }
