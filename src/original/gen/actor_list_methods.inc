// Generated (re/facades/gen3.py): typed members for the ActorList functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0041ba00 NodeList::delete_all
void delete_all();
/// 0x005af1b0 Obj::remove_5af1b0
std::uint32_t remove(std::uint32_t a0);
/// 0x0041a650 Actors::add_default
template <class T0>
std::int32_t add_default(T0 a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Actors_add_default, this, a0, a1, a2, a3); }
