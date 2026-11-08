// Generated (re/facades/gen3.py): typed members for the SoundEffects functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00457ad0 Obj::init_457ad0
void* construct(void* a0);
/// 0x00457e00 Lookup::find
std::int32_t find(const char* a0);
/// 0x0045a350 Wave::load_45a350
void load();
/// 0x00490d30 Se::build_tables
void build_tables();
/// 0x00490eb0 LodQueue::update
std::uint32_t flush();
/// 0x0059e0c0 Se::free
void free();
/// 0x0059e220 Array28::each_59ded0
void clear_recent();
/// 0x0059ebd0 Sound::play_59ebd0
template <class T1>
std::uint32_t play(const char* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Sound_play_59ebd0, this, a0, a1); }
