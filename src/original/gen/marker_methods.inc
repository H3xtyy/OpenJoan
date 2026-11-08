// Generated (re/facades/gen2.py): typed members for the Marker functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00486d10 Marker::draw
void draw(void* a0);
/// 0x00486de0 Marker::line_of_sight
char line_of_sight(void* a0);
/// 0x004e5d90 Marker::position
void* position(void* a0, void* a1);
/// 0x00714040 Marker::update
std::uint32_t update();
/// 0x00717650 Marker::visible
std::uint8_t visible();
/// 0x0044cf10 Obj::get_8
std::uint16_t word_8();
/// 0x0044cf30 Obj::get_e
std::uint8_t byte_e();
/// 0x0044cf50 View::set_word_f
void* set_word_f(std::uint16_t a0);
/// 0x00486650 Block55::clear
void* construct();
/// 0x00486680 fwd_00486680
std::uint32_t destruct();
/// 0x004866b0 Obj::destroy_51
std::uint32_t release_billboard();
/// 0x00486910 Obj::update_fade
std::int32_t update_fade(std::int8_t a0);
/// 0x00487790 sdd_00487790
void* destroy(std::uint32_t a0);
/// 0x004fbc90 Projectile::hit_effects
std::uint32_t hit_effects(std::int32_t a0);
/// 0x004fcc00 Missile::update
void update_missile();
/// 0x00507fc0 Missile::launch_507fc0
void launch_from_unit(void* a0, std::int32_t a1);
/// 0x00514c40 Missile::impact
bool impact(std::int32_t a0);
/// 0x00486720 Obj::init_486720
template <class T1, class T2>
std::uint32_t init(std::int32_t a0, T1 a1, T2 a2, std::int32_t a3, const char* a4) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Obj_init_486720, this, a0, a1, a2, a3, a4); }
/// 0x005133e0 Missile::launch_5133e0
template <class T1, class T2>
void launch(std::uint32_t a0, T1 a1, T2 a2) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::Missile_launch_5133e0, this, a0, a1, a2); }
