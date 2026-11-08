// Generated (re/facades/gen2.py): typed members for the CamTracks functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00429c70 CamTracks::find
std::int32_t find(const char* a0);
/// 0x00429d00 CamTracks::prepare
std::int32_t prepare(std::int32_t a0);
/// 0x00425f70 sdd_00425f70
void* destroy(std::uint32_t a0);
/// 0x00428cf0 CamRes::CamRes
void* construct();
/// 0x00428d50 CamRes::free
void free();
/// 0x00429bb0 Zones::find
template <class T1>
std::int32_t find_zone(const char* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Zones_find_429bb0, this, a0, a1); }
/// 0x00429110 CameraTracks::parse_track
template <class T2>
void* parse_track(void* a0, void* a1, T2 a2) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::CameraTracks_parse_track, this, a0, a1, a2); }
/// 0x00428e90 CameraTracks::load
template <class T1>
void load_file(const char* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::CameraTracks_load, this, a0, a1); }
/// 0x00428de0 CamRes::load
template <class T1>
void load(const char* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::CamRes_load, this, a0, a1); }
/// 0x00429980 CameraTracks::create_array
void* create_array(void* a0);
