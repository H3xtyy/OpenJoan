// Generated (re/facades/gen3.py): typed members for the ThrowItems functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0044cdd0 Effects::cleanup
void cleanup_in_cell(void* a0);
/// 0x00431480 Units::apply_kind
template <class T2, class T3>
std::int32_t apply_kind(std::int32_t a0, std::uint32_t a1, T2 a2, T3 a3) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); static_assert(!std::is_pointer_v<T3> && std::is_trivially_copyable_v<T3>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Units_apply_kind, this, a0, a1, a2, a3); }
