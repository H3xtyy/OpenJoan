// Generated (re/facades/gen2.py): typed members for the Place functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004233c0 Place::set_angle
std::uint32_t set_angle(std::uint32_t a0);
/// 0x00425a40 Place::detach
std::uint32_t detach();
/// 0x004366f0 Place::set_owner
void* set_owner(World* a0);
/// 0x00437e20 Place::update_station
std::uint32_t update_station();
/// 0x0043edb0 Place::unlink_owner
std::uint32_t unlink_owner(void* a0);
/// 0x0044f5c0 Place::set_nation
std::uint32_t set_nation(std::int32_t a0);
/// 0x004c22b0 Place::add_entrance
void add_entrance(void* a0);
/// 0x00422b50 Obj1f6::Obj1f6
void* construct();
/// 0x0042acd0 Obj::join_group
void join_group(const char* a0);
/// 0x00438420 Selection::append
void* add_parking(void* a0, std::uint32_t a1);
/// 0x00469360 get_52
std::int32_t recruit_pool();
/// 0x004693b0 get_56
std::int32_t recruit_cap();
/// 0x0047fc80 ObjC::save
std::int32_t save(void* a0);
/// 0x0047fdf0 Obj::clear_1d6
std::uint32_t clear_pointers();
/// 0x0049c240 Obj::find_unit
std::int32_t find_unit(const char* a0);
/// 0x0049c470 Town::count_units
std::int32_t count_hostiles(std::uint32_t a0);
/// 0x0049e500 Obj::count_49e500
std::int32_t count_enemy_units(std::uint32_t a0);
/// 0x004a8c20 Region::has_enemy_of
std::int32_t has_enemy_of(std::int32_t a0);
/// 0x004ab020 Region::has_unit_of
std::int32_t has_unit_of(const char* a0);
/// 0x004c21c0 X::get_pos
std::uint32_t position(void* a0);
/// 0x004c2470 Rec::at_c
void* slot_pos(std::int32_t a0);
/// 0x004c24b0 Rec::float_at
float slot_angle(std::int32_t a0);
/// 0x004c2540 X::set_slot
std::int32_t set_slot(std::int32_t a0, std::uint32_t a1);
/// 0x004c36c0 Obj::nop_4c36c0
void nop_4c36c0();
/// 0x004c39d0 Faction::link_place
void link_facility(void* a0);
/// 0x004c46f0 X::set_1ce
void* set_1ce(std::uint32_t a0);
/// 0x004c48b0 Obj::get_1ee
void* facility_head();
/// 0x004c5320 Obj4c5320::sdd
void* destroy(std::uint32_t a0);
/// 0x0042b350 Place::set_pose
template <class T0>
std::uint32_t set_pose(T0 a0, void* a1) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Place_set_pose, this, a0, a1); }
