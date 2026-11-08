// Generated (re/facades/gen2.py): typed members for the World functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00429090 World::cells_in_box
void* cells_in_box(void* a0, void* a1) { return ::joan::call::thiscall<void*>(::joan::fn::World_cells_in_box, this, a0, a1); }
/// 0x0042c6b0 World::read_sectors
std::int32_t read_sectors(void* a0);
/// 0x0042ce10 World::add_entry
std::int32_t add_entry();
/// 0x0042d690 World::flag_1fd
std::uint32_t flag_1fd();
/// 0x0042df50 World::leave_mode
void* leave_mode(std::int8_t a0);
/// 0x0043d340 World::release
void* release() { return ::joan::call::thiscall<void*>(::joan::fn::World_release, this); }
/// 0x00440320 World::cleanup
void* cleanup();
/// 0x004426a0 World::unload
void unload();
/// 0x00442d30 World::load_pfd
std::int32_t load_pfd();
/// 0x00444e70 World::load_water
void load_water();
/// 0x004457e0 World::free_far_cells
void free_far_cells(void* a0, std::int32_t a1);
/// 0x00447860 World::finish_level
std::int8_t finish_level();
/// 0x00447900 World::state
std::uint32_t state();
/// 0x00447930 World::lerp
float lerp(float a0, float a1, float a2);
/// 0x00447ab0 World::pick_object_447ab0
void* pick_object_447ab0(std::int32_t a0, std::int32_t a1, void* a2);
/// 0x0044a260 World::build_tree
void build_tree();
/// 0x0044a690 World::siege_release_44a690
std::uint32_t siege_release_44a690();
/// 0x0044c800 World::draw
void draw();
/// 0x0044e8c0 World::init
void init(std::uint16_t a0, const char* a1, const char* a2, void* a3);
/// 0x0044eb80 World::siege_board_44eb80
std::uint32_t siege_board_44eb80(std::int32_t a0);
/// 0x0044f1f0 World::siege_attack_44f1f0
std::uint32_t siege_attack_44f1f0(std::int32_t a0);
/// 0x00452d20 World::cull_lights_452d20
std::uint32_t cull_lights_452d20();
/// 0x0049ae00 World::setup_sky
void setup_sky();
/// 0x00547630 World::stream_cells
void stream_cells(void* a0, void* a1, std::int32_t a2);
/// 0x005479a0 World::cells_pending
std::int32_t cells_pending(void* a0, void* a1, std::int32_t a2);
/// 0x005482c0 World::remove_entry
void remove_entry(std::int32_t a0) { ::joan::call::thiscall<void>(::joan::fn::World_remove_entry, this, a0); }
/// 0x0054a6c0 World::read_nodes
std::int32_t read_nodes(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_read_nodes, this, a0); }
/// 0x0054e1b0 World::sector
void* sector(std::int32_t a0) { return ::joan::call::thiscall<void*>(::joan::fn::World_sector, this, a0); }
/// 0x0054e220 World::sector_name
const char* sector_name(std::int32_t a0) { return ::joan::call::thiscall<const char*>(::joan::fn::World_sector_name, this, a0); }
/// 0x0054e6a0 World::find_sector
void* find_sector(const char* a0) { return ::joan::call::thiscall<void*>(::joan::fn::World_find_sector, this, a0); }
/// 0x00551440 World::read_scene_sector
std::int32_t read_scene_sector(void* a0, std::int32_t a1) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_read_scene_sector, this, a0, a1); }
/// 0x00551930 World::read_scene_object
std::int32_t read_scene_object(void* a0, std::int32_t a1) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_read_scene_object, this, a0, a1); }
/// 0x00553aa0 World::create_water
std::int32_t create_water(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_create_water, this, a0); }
/// 0x00558890 World::load_units
std::int32_t load_units(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_load_units, this, a0); }
/// 0x00558a60 World::create_unit
std::int32_t create_unit(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_create_unit, this, a0); }
/// 0x005593e0 World::read_portals
std::int32_t read_portals(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_read_portals, this, a0); }
/// 0x00559470 World::read_anti_portal
std::int32_t read_anti_portal(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_read_anti_portal, this, a0); }
/// 0x00559ff0 World::load_places
std::int32_t load_places(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_load_places, this, a0); }
/// 0x0055a090 World::create_place
std::int32_t create_place(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_create_place, this, a0); }
/// 0x0055ae40 World::create_facility
std::int32_t create_facility(void* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_create_facility, this, a0); }
/// 0x0055f320 World::load_pln
std::int32_t load_pln(const char* a0) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_load_pln, this, a0); }
/// 0x0055f9b0 World::write_sector_mesh
std::int32_t write_sector_mesh(void* a0, std::int32_t a1) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::World_write_sector_mesh, this, a0, a1); }
/// 0x0055fa40 World::merge_static
void merge_static(void* a0, void* a1, void* a2) { ::joan::call::thiscall<void>(::joan::fn::World_merge_static, this, a0, a1, a2); }
/// 0x00560120 World::merge_animated
void merge_animated(void* a0, void* a1, void* a2) { ::joan::call::thiscall<void>(::joan::fn::World_merge_animated, this, a0, a1, a2); }
/// 0x004214b0 Obj4214b0::dtor
std::uint32_t destruct();
/// 0x00485070 Obj::save_485070
std::int32_t save(void* a0);
/// 0x00485130 Rec92::load
std::int32_t load(void* a0);
/// 0x00485450 Obj::clear_6a
std::uint32_t clear_pointers();
/// 0x004f9330 Settlement::load
std::int32_t load_settlement(void* a0);
/// 0x004f6eb0 World::build_grid_4f6eb0
std::uint32_t build_grid(void* a0, void* a1, void* a2, void* a3) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::World_build_grid_4f6eb0, this, a0, a1, a2, a3); }
