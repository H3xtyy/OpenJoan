// Generated (re/facades/gen2.py): typed members for the MiniMap functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0070ec50 MiniMap::update_70ec50
void update_70ec50();
/// 0x00716be0 MiniMap::draw_view_716be0
void draw_view_716be0();
/// 0x0071a3b0 MiniMap::draw_marker_71a3b0
std::uint32_t draw_marker_71a3b0(std::int32_t a0);
/// 0x0071aba0 MiniMap::draw_gold_71aba0
void draw_gold_71aba0(void* a0);
/// 0x0070e190 Struct480::zero
void* zero();
/// 0x0070e500 Minimap::draw_70e500
std::uintptr_t draw(std::uint32_t a0);
/// 0x0070eb20 MapButton::click_70eb20
std::uint8_t handle_zoom_key();
/// 0x0070f9e0 MapView::to_screen
void* unit_to_map(void* a0, void* a1);
/// 0x0070fae0 Obj::random_offset
void* pos_to_map(void* a0, void* a1);
/// 0x0070fb70 UnitMarker::set_unit
void track_unit(std::int16_t a0);
/// 0x0070fbe0 MapOverlay::draw_unit_70fbe0
void draw_unit(void* a0);
/// 0x007141a0 Minimap::draw_tiles_7141a0
std::uint32_t draw_tiles();
/// 0x00716010 Minimap::draw_markers_716010
void draw_markers(std::int32_t a0);
/// 0x00717220 Obj717::get_f
std::uint16_t zoom_mode();
/// 0x00717240 MapButton::click_717240
std::uint8_t handle_click();
/// 0x00717700 MapMarker::to_screen
std::uint8_t to_screen(void* a0);
/// 0x00717f40 Obj::draw_clipped
void draw_clipped_line(float a0, float a1, std::uint32_t a2, float a3, float a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7);
/// 0x007180f0 Obj::set_slot_7180f0
void set_waypoint_style(std::int16_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x007190b0 MapOverlay::update_unit_7190b0
void update_unit(void* a0);
/// 0x007196e0 Obj719::set_slot
void* set_reveal_rect(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4);
/// 0x00719770 X::set_slot
void set_reveal_circle(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x007197f0 Obj::set_slot_7197f0
void* set_reveal_quad(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8);
/// 0x007198d0 Obj719::clear_slot
void clear_reveal(std::int32_t a0);
/// 0x00719900 MapView::fill_rect_719900
void reveal_rect(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3);
/// 0x00719b40 Minimap::reveal_circle
void reveal_circle(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x00719e60 MapOverlay::reveal_quad_719e60
void reveal_quad(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7);
/// 0x00717a90 UnitMarker::in_range
bool near_tracked_unit(float a0, float a1, std::uint32_t a2);
/// 0x00718540 MiniMap::draw_waypoint_718540
template <class T1>
void draw_waypoint(std::uint32_t a0, T1 a1, void* a2) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::MiniMap_draw_waypoint_718540, this, a0, a1, a2); }
/// 0x00718010 Panel::set_slot
template <class T2>
void set_waypoint(std::int16_t a0, char a1, T2 a2, char a3) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::Panel_set_slot, this, a0, a1, a2, a3); }
/// 0x00717910 Cursor::can_drop_at
template <class T0>
std::uint8_t can_drop_at(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint8_t>(::joan::fn::Cursor_can_drop_at, this, a0); }
