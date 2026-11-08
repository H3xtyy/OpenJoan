// Generated (re/facades/gen3.py): typed members for the Markers functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004876d0 List487790::delete_all
void delete_all();
/// 0x00487b00 Markers::draw
std::int32_t draw(void* a0);
/// 0x004877e0 List::add_new_4877e0
template <class T1, class T2>
std::uint32_t add(std::int32_t a0, T1 a1, T2 a2, std::int32_t a3, const char* a4) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::List_add_new_4877e0, this, a0, a1, a2, a3, a4); }
