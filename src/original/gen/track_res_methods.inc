// Generated (re/facades/gen3.py): typed members for the TrackRes functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004eb5f0 sdd_004eb5f0
void* destroy(std::uint32_t a0);
/// 0x004eb9a0 TrackRes::TrackRes
void* construct();
/// 0x004eba00 TrackRes::free
void free();
/// 0x004ebb40 Tracks::load
template <class T1>
void load_file(const char* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::Tracks_load_4ebb40, this, a0, a1); }
/// 0x004eba90 TrackRes::load
template <class T1>
void load(const char* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::TrackRes_load, this, a0, a1); }
/// 0x004ec240 Tracks::create_array
void* create_array(void* a0);
/// 0x004ebd50 Tracks::parse_track
template <class T2>
const char* parse_track(void* a0, void* a1, T2 a2) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<const char*>(::joan::fn::Tracks_parse_track, this, a0, a1, a2); }
