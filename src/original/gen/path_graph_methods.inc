// Generated (re/facades/gen3.py): typed members for the PathGraph functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004ec3d0 TrackRes::free
void free();
/// 0x004ec5f0 PathGraph::route_4ec5f0
std::uint8_t route(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, void* a6, std::uint32_t a7);
/// 0x004ec480 Points::nearest_two
template <class T2, class T3>
void* nearest_two(void* a0, void* a1, T2 a2, T3 a3) { static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); static_assert(!std::is_pointer_v<T3> && std::is_trivially_copyable_v<T3>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::Points_nearest_two, this, a0, a1, a2, a3); }
/// 0x004d3790 PathGrid::find_path_4d3790
template <class T0, class T1>
std::uint8_t find_path(T0 a0, T1 a1, void* a2, std::uint32_t a3) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<std::uint8_t>(::joan::fn::PathGrid_find_path_4d3790, this, a0, a1, a2, a3); }
