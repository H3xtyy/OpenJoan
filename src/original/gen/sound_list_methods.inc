// Generated (re/facades/gen2.py): typed members for the SoundList functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00463cb0 SoundList::read
void read(void* a0, void* a1);
/// 0x004cf690 SoundList::entry
void* entry(std::int16_t a0);
/// 0x00491770 Res13::free_all_491770
std::uint32_t free_all();
/// 0x004c0680 Obj::attach_4c0680
std::uint8_t play_zone_sound(std::int16_t a0);
/// 0x00494810 Unit::emit_particles
template <class T1>
std::int32_t play_effect(const char* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Unit_emit_particles, this, a0, a1); }
/// 0x004950a0 Sound::play_at
template <class T1>
std::int32_t play_at(std::int32_t a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Sound_play_at, this, a0, a1); }
/// 0x00493340 Obj::farthest
template <class T0>
std::uint32_t release_farthest(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Obj_farthest, this, a0); }
