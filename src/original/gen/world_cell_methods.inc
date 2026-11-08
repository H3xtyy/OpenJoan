// Generated (re/facades/gen2.py): typed members for the WorldCell functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x005600c0 WorldCell::grid
void* grid(std::int16_t a0) { return ::joan::call::thiscall<void*>(::joan::fn::WorldCell_grid, this, a0); }
/// 0x00484e00 Recac::save
std::int32_t save(void* a0);
/// 0x00484f50 Obj::clear_484f50
void* clear_pointers();
/// 0x00548430 Obj::scalar_delete
void* destroy(std::uint32_t a0);
/// 0x0054ee10 Obj::get_8c
void* get_8c() { return ::joan::call::thiscall<void*>(::joan::fn::Obj_get_8c, this); }
/// 0x0054eec0 Obj::get_90
void* get_90() { return ::joan::call::thiscall<void*>(::joan::fn::Obj_get_90, this); }
/// 0x0054f510 Obj::get_98
void* get_98() { return ::joan::call::thiscall<void*>(::joan::fn::Obj_get_98, this); }
/// 0x0054f580 List54f580::unlink
void unlink_object(void* a0) { ::joan::call::thiscall<void>(::joan::fn::List54f580_unlink, this, a0); }
/// 0x0054f890 X::get_9c
std::uintptr_t get_9c() { return ::joan::call::thiscall<std::uintptr_t>(::joan::fn::X_get_9c, this); }
/// 0x0054f960 Obj::get_a0
std::uintptr_t get_a0() { return ::joan::call::thiscall<std::uintptr_t>(::joan::fn::Obj_get_a0, this); }
/// 0x00558a30 Obj::forward_558a30
std::uint32_t load_units_thunk(void* a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Obj_forward_558a30, this, a0); }
/// 0x00560060 X::flags_54_58
std::uint32_t path_data_flags() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::X_flags_54_58, this); }
/// 0x00560c70 Occluders::add
std::int32_t add_occluder() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::Occluders_add, this); }
/// 0x00560d30 Occluders::get
void* occluder(std::int32_t a0) { return ::joan::call::thiscall<void*>(::joan::fn::Occluders_get, this, a0); }
/// 0x0054e080 forward_43d340
void* destruct() { return ::joan::call::thiscall<void*>(::joan::fn::forward_43d340, this); }
/// 0x00558140 WorldS::load_grid_558140
std::uint32_t load_grid(void* a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::WorldS_load_grid_558140, this, a0); }
/// 0x0055b720 Cell::build_ground
void build_ground(void* a0, void* a1, void* a2, const char* a3) { ::joan::call::thiscall<void>(::joan::fn::Cell_build_ground, this, a0, a1, a2, a3); }
/// 0x00560ee0 Terrain::name_at
template <class T0>
void* terrain_at(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::Terrain_name_at, this, a0); }
/// 0x00560d80 Zones::any_hit
template <class T0, class T1>
bool occluders_hit(T0 a0, T1 a1, std::int32_t a2) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<bool>(::joan::fn::Zones_any_hit, this, a0, a1, a2); }
/// 0x00560e30 Terrain::cell_of
template <class T2>
void grid_cell_of(void* a0, void* a1, T2 a2) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::Terrain_cell_of, this, a0, a1, a2); }
