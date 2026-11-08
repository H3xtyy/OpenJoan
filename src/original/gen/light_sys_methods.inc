// Generated (re/facades/gen2.py): typed members for the LightSys functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004a5020 LightSys::init
void init(const char* a0, void* a1);
/// 0x004a5970 LightSys::clear
void clear();
/// 0x004a5bf0 LightSys::load_lights
void load_lights(const char* a0);
/// 0x004a65d0 LightSys::lit_by
std::int32_t lit_by(std::uint32_t a0);
/// 0x004a6840 LightSys::load_special
void load_special();
/// 0x004a7450 LightSys::load_day
void load_day(const char* a0);
/// 0x004a9120 LightSys::find_set
std::int32_t find_set(const char* a0);
/// 0x0045b160 Obj::roll_timers
void roll_timers();
/// 0x0046a560 Obj::sync_colours
void sync_lights();
/// 0x004a7980 Sky::moon
void update_moon();
/// 0x004a7ba0 Sky::after_frame
void after_frame();
/// 0x004a7f70 Weather::tick_4a7f70
void tick_weather(Scene* a0);
/// 0x004d30f0 Sky::ray_clear
template <class T1>
std::uint8_t ray_clear(void* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<std::uint8_t>(::joan::fn::Sky_ray_clear, this, a0, a1); }
