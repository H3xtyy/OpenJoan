// Generated (re/facades/gen2.py): typed members for the Cell functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004329e0 Cell::load
void load(void* a0, std::int32_t a1);
/// 0x00437770 Cell::attach_object
void attach_object(void* a0);
/// 0x0043b190 Cell::attach_unit
void attach_unit(void* a0);
/// 0x0043f4e0 Cell::free_textures
void free_textures(void* a0, std::int32_t a1);
/// 0x00449780 Cell::unload_files
void unload_files();
/// 0x004f2f80 Cell::load_files
std::int32_t load_files(void* a0, std::int32_t a1) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::Cell_load_files, this, a0, a1); }
/// 0x004f3df0 Cell::create_object
std::int32_t create_object(void* a0, std::int32_t a1, void* a2) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::Cell_create_object, this, a0, a1, a2); }
/// 0x004f5210 Cell::create_water
std::int32_t create_water(void* a0, std::int32_t a1) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::Cell_create_water, this, a0, a1); }
/// 0x0050c430 Cell::release_slot
void release_slot(std::int16_t a0, std::int16_t a1);
/// 0x0042ac40 MapCell::unload
void unload();
/// 0x0042b270 WorldS::add_entry
std::int32_t add_scene_node() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::WorldS_add_entry, this); }
/// 0x0043ec80 Station::enter
void* attach_place(void* a0);
