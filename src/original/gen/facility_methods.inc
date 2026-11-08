// Generated (re/facades/gen3.py): typed members for the Facility functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0046d520 Record::Record
void* construct();
/// 0x0046d5b0 Obj::dtor_46d5b0
std::uint32_t destruct();
/// 0x0046d5e0 Entry::set
const char* set_kind_name(std::int16_t a0, const char* a1);
/// 0x0046d630 Obj::notify_owner
void* detach();
/// 0x0046d670 Obj::get_29
std::uint32_t get_position(void* a0);
/// 0x0046d720 Facility::set_angle
void set_angle(std::uint32_t a0);
/// 0x0046d750 Facility::add_entrance
void add_entrance(void* a0);
/// 0x0046da90 Area::is_free
std::int32_t is_free();
/// 0x004800a0 Record::load
std::int32_t save_base(void* a0);
/// 0x004801d0 Obj::clear_71_75
void* clear_pointers();
/// 0x0046d6b0 Facility::set_placement
template <class T0>
void set_placement(T0 a0, void* a1) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::Facility_set_placement, this, a0, a1); }
