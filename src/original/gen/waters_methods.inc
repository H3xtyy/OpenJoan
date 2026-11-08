// Generated (re/facades/gen3.py): typed members for the Waters functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00469d10 Regions::find
void* find_at(void* a0, std::int32_t a1);
/// 0x00546d80 Waters::clear_all
void clear_all();
/// 0x00474df0 List::add_546af0
template <class T2>
std::int32_t add(char a0, char a1, T2 a2, std::uint32_t a3, std::uint32_t a4, void* a5) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::List_add_546af0, this, a0, a1, a2, a3, a4, a5); }
