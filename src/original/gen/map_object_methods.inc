// Generated (re/facades/gen3.py): typed members for the MapObject functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00415790 Obj::dtor_415790
std::uint32_t destruct();
/// 0x00416a50 Obj::ctor_416a50
void* construct();
/// 0x004178e0 Rec::set_kind
void* set_kind(std::int16_t a0);
/// 0x0041af80 Object::heading_to_62
float heading_to(void* a0);
/// 0x0044a5f0 Obj::drain
float drain_hp(std::int32_t a0, std::int32_t a1);
/// 0x00452b20 Obj::drain_item
float drain_item(std::int32_t a0, std::int32_t a1);
/// 0x00481ba0 Obj::save_481ba0
std::int32_t save(void* a0);
/// 0x00481cd0 Obj::load_481cd0
std::int32_t load(void* a0);
/// 0x00481ed0 ObjD::reset
void* reset_links();
/// 0x0048a040 Obj::base_48a040
std::uint32_t release();
/// 0x0048ba00 Obj::is_active_48ba00
std::uint32_t is_active();
/// 0x0048ec00 Obj::reset_48ec00
std::uint32_t on_remove();
/// 0x0048ed30 X::call_41d280
std::uint32_t time_left();
/// 0x0048f360 always_1_48f360
std::int32_t refresh_48f360(std::int32_t a0);
/// 0x0048f8a0 Obj::base_48f8a0
std::uint32_t release_48f8a0();
/// 0x00493060 Obj::spawn_effect
std::int32_t set_collision(std::int32_t a0);
/// 0x004991e0 SoundSlots::play
void play_sound(const void* a0, const void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5);
/// 0x004994b0 Obj::set_state
void set_world(World* a0);
/// 0x00499930 Obj::get_pos
std::uint32_t position(void* a0);
/// 0x00499e20 Obj::get_62
float yaw();
/// 0x00499fd0 Obj::set_62
void set_yaw(std::uint32_t a0);
/// 0x0049ad10 Obj::float_6a
float roll();
/// 0x0049adc0 Obj::set_6a
void set_roll(std::uint32_t a0);
/// 0x0049c1a0 Obj::release_92
void release_light();
/// 0x0049c920 Obj::release_slot
std::uint32_t release_marker();
/// 0x0049cac0 Obj::set_6e
void set_scale(std::uint32_t a0);
/// 0x00428600 Unit::can_target
bool can_target(std::int32_t a0);
/// 0x0041ff50 Unit::set_position
template <class T0>
void set_position(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::Unit_set_position, this, a0); }
/// 0x00444a20 Map::rect_hit_444a20
template <class T0, class T1>
std::uint32_t rect_hit(T0 a0, T1 a1) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Map_rect_hit_444a20, this, a0, a1); }
/// 0x00492370 Obj::forward_492370
std::uint32_t release_base() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Obj_forward_492370, this); }
/// 0x004633e0 Obj::set_c6_ca
void* set_target_unit(void* a0);
