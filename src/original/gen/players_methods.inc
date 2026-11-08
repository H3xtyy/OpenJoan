// Generated (re/facades/gen2.py): typed members for the Players functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004b6860 Players::hostile
std::int32_t hostile(std::int32_t a0, std::int32_t a1);
/// 0x0068de40 Players::collide_68de40
std::uint32_t collide_68de40(void* a0, void* a1, void* a2, void* a3, void* a4);
/// 0x006ad270 Players::add_cell
std::uint32_t add_cell(PathSpace* a0, PathSpace* a1, std::int32_t a2, std::int32_t a3, void* a4);
/// 0x006ad450 Players::remove_cell
std::uint32_t remove_cell(PathSpace* a0, PathSpace* a1);
/// 0x006ad980 Players::set_active
std::uint32_t set_active(std::int32_t a0, std::int16_t a1);
/// 0x006adba0 Players::is_active
std::int16_t is_active(std::int32_t a0);
/// 0x006ae610 Players::find_slot_wrapper
std::uint32_t find_slot_wrapper(void* a0, void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11);
/// 0x006aee40 Players::place
std::int16_t place(void* a0, void* a1);
/// 0x006af190 Players::move_actor
std::int16_t move_actor(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x006af310 Players::get
void* get(std::int32_t a0);
/// 0x006b0800 Players::record_count
std::uint32_t record_count();
/// 0x00472eb0 Obj::set_b16
void set_b16(std::uint32_t a0);
/// 0x006aabb0 Obj::field_b16
float time_scale();
/// 0x006aabe0 Obj::word_b1a
std::uint32_t word_b1a();
/// 0x006aced0 Global::ctor_6aced0
void* construct();
/// 0x006ad020 Obj6ad020::init
void* init();
/// 0x006ad170 X::shutdown_b0e
std::uint32_t shutdown();
/// 0x006ad5d0 Obj6a::release_slots
void release_slots();
/// 0x006ad900 Obj::set_timer
void* set_timer(std::int16_t a0);
/// 0x006ad950 Global::reset_6ad950
std::uint32_t reset();
/// 0x006adf70 SoundSet::remove
std::uint32_t remove_record(std::int32_t a0);
/// 0x006ae2a0 X::pick_point
void pick_point(void* a0, void* a1, void* a2, std::uint32_t a3, std::uint32_t a4, float a5, float a6, std::uint32_t a7, std::int32_t a8);
/// 0x006b0720 Obj6b0720::self
void* construct_base();
/// 0x006b0740 empty_6b0740
void nop_6b0740();
/// 0x00505d60 Obj::set_b1a
void* set_b1a(std::int16_t a0);
/// 0x006ae8e0 Players::find_slot
std::int16_t find_slot(void* a0, void* a1, std::int16_t a2, float a3, float a4, float a5, std::int32_t a6, float a7, std::int32_t a8);
/// 0x006adfd0 Picker::pick
void pick(void* a0, void* a1, void* a2, void* a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::int32_t a9);
/// 0x006afb10 Formation::assign_matches
template <class T0>
std::int32_t assign_matches(T0 a0, void* a1, std::int32_t a2, std::int8_t a3, std::uint8_t a4) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Formation_assign_matches, this, a0, a1, a2, a3, a4); }
