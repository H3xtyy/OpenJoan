// Generated (re/facades/gen3.py): typed members for the CameraZones functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00420150 sdd_00420150
void* destroy(std::uint32_t a0);
/// 0x00425dc0 CamZones::CamZones
void* construct();
/// 0x00425df0 fwd_00425df0
std::uint32_t destruct();
/// 0x00425e20 CamZones::init
std::uint32_t init();
/// 0x00425ef0 Obj::destroy_12
std::uint32_t free_tracks();
/// 0x00425fc0 CameraZone::update
std::int32_t update();
/// 0x00426330 CamZones::load
std::uint32_t load(const char* a0);
/// 0x00427bb0 lerp_b
float lerp(float a0, float a1, float a2);
/// 0x00427cc0 Obj427cc0::set_id
void set_track_set(std::uint32_t a0);
/// 0x00427d10 Obj427::slot_free
std::int16_t zone_free();
/// 0x00427d90 CameraZone::scripted
void scripted(void* a0);
/// 0x00428850 CamMotion::start
std::int16_t start_track(const char* a0, std::uint32_t a1);
/// 0x00428a70 Obj::set_46
void* set_speed(std::uint32_t a0);
/// 0x00428b00 set_first
void set_unit(std::uint32_t a0);
/// 0x004279b0 hermite_b
template <class T1, class T2, class T3, class T4>
void* hermite(void* a0, T1 a1, T2 a2, T3 a3, T4 a4, float a5) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); static_assert(!std::is_pointer_v<T3> && std::is_trivially_copyable_v<T3>, "a struct by value"); static_assert(!std::is_pointer_v<T4> && std::is_trivially_copyable_v<T4>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::hermite_b, this, a0, a1, a2, a3, a4, a5); }
/// 0x00426450 CameraZone::inside
template <class T1>
void inside(void* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::CameraZone_inside, this, a0, a1); }
/// 0x004271f0 CameraZone::enter
template <class T1>
void enter(void* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::CameraZone_enter, this, a0, a1); }
