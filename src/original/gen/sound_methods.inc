// Generated (re/facades/gen2.py): typed members for the Sound functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00454000 Sound::set_distance_model
void set_distance_model(char a0);
/// 0x00492c40 Sound::play_kind
std::int32_t play_kind(const char* a0);
/// 0x004ceef0 Sound::play_def
std::int32_t play_def(const char* a0);
/// 0x00578390 Sound::set_11c
std::uint32_t set_11c(std::uint32_t a0);
/// 0x0057c960 Sound::attenuation
std::int32_t attenuation(std::int32_t a0);
/// 0x00585d20 Sound::reset_585d20
void* reset_585d20();
/// 0x00592ae0 Sound::pause_all
void pause_all();
/// 0x0059b150 Sound::set_volume
void set_volume_59b150(std::int32_t a0);
/// 0x0059f620 Sound::play
void play();
/// 0x0059f650 Sound::pause
void pause();
/// 0x00603fd0 Sound::restart
std::uint32_t restart();
/// 0x00755d80 Sound::play_755d80
std::uint32_t play_755d80() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Sound_play_755d80, this); }
/// 0x00473140 Obj::has_slot_473140
std::uint8_t is_playing(const char* a0);
