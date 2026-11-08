// Generated (re/facades/gen2.py): typed members for the TrailNodes functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x006894f0 TrailNodes::pop_oldest
void pop_oldest();
/// 0x006895c0 TrailNodes::push
void push(void* a0);
/// 0x00689af0 TrailNodes::count
std::int32_t count();
/// 0x00689370 Obj689370::Obj
void* construct();
/// 0x006898d0 TrailNodes::tangent
template <class T1, class T2, class T3>
void* tangent(void* a0, T1 a1, T2 a2, T3 a3, float a4) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); static_assert(!std::is_pointer_v<T3> && std::is_trivially_copyable_v<T3>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::TrailNodes_tangent, this, a0, a1, a2, a3, a4); }
/// 0x00689a20 TrailNodes::lerp
template <class T1, class T2>
void* lerp(void* a0, T1 a1, T2 a2, float a3) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::TrailNodes_lerp, this, a0, a1, a2, a3); }
