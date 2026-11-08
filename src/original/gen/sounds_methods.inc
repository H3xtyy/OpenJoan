// Generated (re/facades/gen2.py): typed members for the Sounds functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00470c90 Sounds::resume
void resume(std::int32_t a0);
/// 0x004959b0 Sounds::create
void create(std::uint32_t a0);
/// 0x00495b40 Sounds::create_missing
void create_missing();
/// 0x004ced90 Sounds::pause
void pause(std::int32_t a0);
/// 0x0068ab40 Sounds::stop
void stop(std::int32_t a0);
/// 0x004ceab0 Struct134::zero
void* zero();
/// 0x004ceaf0 Obj::call_4ceaf0
std::uint32_t destruct();
/// 0x004ceb60 X::reset_4ceb60
std::uint32_t shutdown();
/// 0x004ceb90 Panels::reset_4ceb90
void stop_all(std::uint32_t a0);
/// 0x004cf900 Cache::refresh_4cf900
void alloc_sources();
/// 0x004cfee0 Slots::has_kind
bool has_kind(std::int32_t a0);
/// 0x004d00e0 Obj::any_slot_at_4d00e0
template <class T0>
std::uint8_t any_slot_at(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint8_t>(::joan::fn::Obj_any_slot_at_4d00e0, this, a0); }
