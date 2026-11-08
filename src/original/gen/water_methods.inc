// Generated (re/facades/gen3.py): typed members for the Water functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00436780 Obj436780::sdd
void* destroy(std::uint32_t a0);
/// 0x00546af0 Obj546af0::ctor
void* construct();
/// 0x00546b20 Water::clear
std::uint32_t clear();
/// 0x00546bc0 0x00546bc0
void reset_sounds();
/// 0x00546bf0 Water::add_sound
void add_sound(const char* a0);
/// 0x00546b50 Water::init
template <class T2>
std::uint32_t init(std::uint8_t a0, std::uint32_t a1, T2 a2, std::uint32_t a3, std::uint32_t a4, void* a5) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Water_init, this, a0, a1, a2, a3, a4, a5); }
