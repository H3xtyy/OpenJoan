// Generated (re/facades/gen3.py): typed members for the Occluder functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00530eb0 Occluder::set_70
void set_enabled();
/// 0x00530ee0 Occluder::set_corner
void set_corner(std::int32_t a0, void* a1);
/// 0x00530f20 Quad::ray_hit
template <class T0, class T1>
bool ray_hit(T0 a0, T1 a1) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<bool>(::joan::fn::Quad_ray_hit, this, a0, a1); }
