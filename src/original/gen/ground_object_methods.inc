// Generated (re/facades/gen2.py): typed members for the GroundObject functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004fee60 GroundObject::update_position
void update_position();
/// 0x004ff830 GroundObject::id
std::int32_t id();
/// 0x004ff850 GroundObject::heading
float heading();
/// 0x004fedf0 GroundObject::set_position
template <class T0>
void set_position(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::GroundObject_set_position, this, a0); }
