// Generated (re/facades/gen3.py): typed members for the LensFlares functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x006625f0 Struct14::zero
std::uint32_t construct();
/// 0x00662620 Obj::nop_662620
void destruct();
/// 0x00662640 LensFlares::init
std::uint32_t init(std::int16_t a0, std::uint32_t a1, std::uint32_t a2, std::int16_t a3, std::int16_t a4);
/// 0x006626a0 LensFlares::clear
std::uint32_t clear();
/// 0x00662710 LensFlares::add
void* add(std::uint32_t a0, std::int16_t a1, std::uint32_t a2, const char* a3);
/// 0x006628a0 LensFlare::draw
void draw(std::int16_t a0, std::int16_t a1, float a2);
/// 0x00662d20 LensFlares::acquire_texture
std::int32_t acquire_texture() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::LensFlares_acquire_texture, this); }
/// 0x00662db0 LensFlares::release_texture
std::int32_t release_texture() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::LensFlares_release_texture, this); }
