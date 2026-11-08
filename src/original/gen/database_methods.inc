// Generated (re/facades/gen3.py): typed members for the Database functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00415280 get_dword_112
std::uint32_t rec_count();
/// 0x00580730 Stream::Stream
void* construct(const char* a0, std::int32_t a1, std::int32_t a2);
/// 0x00581df0 DataBase::add_field
template <class T0>
std::int32_t add_field(T0 a0, const char* a1) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::DataBase_add_field, this, a0, a1); }
