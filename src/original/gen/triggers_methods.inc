// Generated (re/facades/gen3.py): typed members for the Triggers functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004eed50 Obj::ctor_4eed50
void* construct();
/// 0x004f0290 X::lacks
std::uint32_t is_empty_at(std::uint32_t a0);
/// 0x004eef00 List43::add
template <class T6>
std::int32_t add(std::uint32_t a0, std::uint32_t a1, std::int16_t a2, std::uint32_t a3, std::int16_t a4, std::int8_t a5, T6 a6, std::uint32_t a7, std::int8_t a8, std::uint32_t a9, std::uint32_t a10, std::int8_t a11, void* a12, std::int16_t a13) { static_assert(!std::is_pointer_v<T6> && std::is_trivially_copyable_v<T6>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::List43_add, this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13); }
