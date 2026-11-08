// Generated (re/facades/gen3.py): typed members for the Particle functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00662e20 Particle::reset
void reset();
/// 0x00662ed0 PartSlot::clear
void clear_model();
/// 0x00662f60 Particle::update
template <class T1, class T2>
std::uint8_t update(float a0, T1 a1, T2 a2, char a3) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<std::uint8_t>(::joan::fn::Particle_update, this, a0, a1, a2, a3); }
