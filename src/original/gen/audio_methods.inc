// Generated (re/facades/gen3.py): typed members for the Audio functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0057a340 Obj::ctor_57a340
void* construct();
/// 0x0057a440 Audio::init_57a440
std::uint32_t init();
/// 0x0057a6c0 Audio::stop_57a6c0
std::uintptr_t shutdown();
/// 0x0057a740 DSound::create
std::int32_t create_dsound();
/// 0x0057a920 SoundSys::shutdown
void release_dsound();
/// 0x0057ab20 CdAudio::stop
std::uint32_t cd_stop_57ab20();
/// 0x0057b0d0 Audio::play_resource_57b0d0
std::uint32_t play_resource(std::uint32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x0057b580 Audio::play_memory
std::uint32_t play_memory(void* a0, std::uint32_t a1, std::uint32_t a2);
/// 0x0057b9c0 SoundSys::reap_buffers_57b9c0
std::int32_t reap_buffers();
/// 0x0057dd30 SoundSys::release_buffers
void release_buffers();
/// 0x0057e1a0 Cd::stop
void cd_stop();
