// Generated (re/facades/gen2.py): typed members for the PathSpace functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x006670e0 PathSpace::clear_children
void clear_children();
/// 0x0066b7a0 PathSpace::get_2c
std::int32_t get_2c();
/// 0x0066b9f0 PathSpace::zone_count
std::int32_t zone_count();
/// 0x00670980 PathSpace::load
std::int32_t load(const char* a0);
/// 0x00671cb0 PathSpace::flood
std::uint8_t flood(float a0, float a1, std::int32_t a2, float a3);
/// 0x00664fd0 PfdData::PfdData
void* construct();
/// 0x00665000 PfdData::~PfdData
std::uint32_t destruct();
/// 0x00665290 PfdData::clear_grid
std::uint32_t clear_grid();
/// 0x00665940 Obj665::index_of
std::int32_t point_index(Triple* a0);
/// 0x00666d80 PfdData::free_10
std::uint32_t clear_groups();
/// 0x00666fc0 X::call_66b7a0
std::uint32_t group_count();
/// 0x00667000 Obj::poly_first
std::int32_t group_size(std::int32_t a0);
/// 0x0066b7c0 PolySet::link_edges
void link_edges(std::int32_t a0);
/// 0x0066bb40 Nav::find_region_66bb40
std::int32_t find_region(float a0, float a1, float a2);
/// 0x0066c020 Region::pick_66c020
std::uint32_t pick(float a0, float a1, float a2, std::uint32_t a3);
/// 0x0066c710 PathTree::nearest_66c710
std::uint32_t nearest_zone(std::int32_t a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x0066cb40 Nav::find_path_66cb40
std::int32_t find_path(float a0, float a1, float a2, float a3, float a4, float a5, void* a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::uint32_t a10, std::uint32_t a11, std::uint32_t a12);
/// 0x006736e0 Polygon::segment_cut
std::int32_t segment_cut(void* a0, void* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6);
/// 0x00674590 PathSp::flush
void flush_bvh();
/// 0x006745e0 PathSp::build_6745e0
std::uint32_t build_bvh();
/// 0x006747a0 Bvh::build_6747a0
BoxTree* build_bvh_range(BoxTree* a0, std::int32_t a1, std::int32_t a2);
/// 0x00675660 PathZones::group
std::uint32_t group_zones();
/// 0x00673d00 PathSpace::find_zone
template <class T2>
std::int32_t find_zone(void* a0, std::int32_t a1, T2 a2, std::uint32_t a3, std::int32_t a4, std::int32_t a5) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::PathSpace_find_polygon_from_cur, this, a0, a1, a2, a3, a4, a5); }
/// 0x00673e20 PathSpace::find_best_673e20
template <class T2>
void find_best(void* a0, std::int32_t a1, T2 a2, std::int32_t a3, std::int32_t a4, void* a5) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::PathSpace_find_nearest_polygon_from_cur, this, a0, a1, a2, a3, a4, a5); }
