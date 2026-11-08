// Generated (re/facades/gen2.py): typed members for the Track functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004289e0 Track::restart
void* restart();
/// 0x00428aa0 Track::is_type1
std::int16_t is_type1();
/// 0x004956a0 Track::wrap_delta
float wrap_delta(float a0);
/// 0x004a9ac0 Track::add_point
std::uint32_t add_point(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x005b7bf0 Track::allocate
std::int32_t allocate(std::int32_t a0);
/// 0x005b83a0 Track::compress
void compress();
/// 0x00429f30 Obj::nop_429f30
void destruct();
/// 0x0045cc20 TrackRes::free_45cc20
std::uint8_t free_points();
/// 0x00654a80 Obj::set_word_44
void* set_word_44(std::uint32_t a0);
/// 0x00654ab0 Obj::set_4a
void* set_4a(std::int16_t a0);
/// 0x005b7c70 Track::sample
float sample(float a0, void* a1, void* a2, void* a3);
/// 0x00429a60 Spline::point
template <class T1, class T2, class T3>
std::uint32_t spline_point(void* a0, T1 a1, T2 a2, T3 a3, float a4) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); static_assert(!std::is_pointer_v<T3> && std::is_trivially_copyable_v<T3>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Spline_point, this, a0, a1, a2, a3, a4); }
