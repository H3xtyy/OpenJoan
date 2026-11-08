// Generated (re/facades/gen3.py): typed members for the Console functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00454270 Obj::self_454270
void* construct();
/// 0x004543c0 parse_quoted
std::uint32_t parse_quoted(const char* a0, std::uint32_t a1, const char* a2);
/// 0x004544c0 skip_spaces
std::uint32_t skip_spaces(const char* a0, std::uint32_t a1);
/// 0x00454560 Console::execute
std::uint32_t execute(const char* a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Console_execute, this, a0); }
/// 0x00454c80 Cheats::dispatch
std::uint32_t run_cheat(const char* a0, const char* a1, const char* a2);
/// 0x00455550 Obj::unit_call_4fe5d0
std::uint32_t cheat_ep();
/// 0x004555c0 player_unit_refresh
std::uint32_t cheat_hp();
/// 0x00455630 units_set_1dd
void cheat_invincible(const char* a0, const char* a1);
/// 0x004558f0 convert_units_in_zone
void cheat_kill_unit_in_place(const char* a0);
/// 0x00455a60 Obj::reset_unit
void cheat_kill_unit(const char* a0);
/// 0x00455e30 Cheat::add_gold
void cheat_unit_add_hp(const char* a0, const char* a1);
/// 0x00455f50 cmd_add_level
void cheat_unit_add_ep(const char* a0, const char* a1);
/// 0x004561d0 Cheat::add_food
void cheat_unit_add_str(const char* a0, const char* a1);
/// 0x00456330 Cheat::add_wood
void cheat_unit_add_def(const char* a0, const char* a1);
/// 0x00456480 Cheat::add_stone
void cheat_unit_add_dex(const char* a0, const char* a1);
/// 0x00456590 Cheat::add_iron
void cheat_unit_add_lea(const char* a0, const char* a1);
/// 0x00456960 Obj::copy_a3cc64
std::uint32_t cheat_add_command_point();
/// 0x00456990 Obj::set_view_rect
void cheat_enable_fog_of_war(const char* a0);
/// 0x00456a40 Obj::toggle_a4f3ae
void cheat_toggle_fog_of_war();
/// 0x00456ae0 Missions::flag_by_name
void cheat_capture_place(const char* a0);
/// 0x00456b90 remove_named
std::uint32_t cheat_del_item(const char* a0);
