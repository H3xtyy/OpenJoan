// Generated (re/facades/gen2.py): typed members for the Input functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00592380 Input::init_592380
void* init_592380(void* a0, void* a1, void* a2);
/// 0x00592830 Input::release
void release();
/// 0x005929f0 Input::unacquire_all
std::uint32_t unacquire_all();
/// 0x00592d00 Input::push_event
void push_event(void* a0);
/// 0x00594d30 Input::double_click
std::int32_t double_click(std::int32_t a0);
/// 0x00594f90 Input::poll
std::int32_t poll();
/// 0x00595a70 Input::scale_axis
std::int32_t scale_axis(std::int32_t a0);
/// 0x00598040 Input::direction_598040
std::int32_t direction_598040();
/// 0x00598730 Input::poll_joysticks
std::uint32_t poll_joysticks(std::int32_t a0);
/// 0x00598ac0 Input::reset_keys
void reset_keys();
/// 0x00598bd0 Input::key_event
std::int32_t key_event(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00592c90 Obj::forward_5973d0
std::uint32_t lock_area(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
/// 0x00592dc0 EventQueue::push2
std::uint32_t push_event2(std::int32_t a0, std::int32_t a1);
/// 0x00592f00 EventQueue::push6
std::uint32_t push_event6(std::int32_t a0, std::int32_t a1);
/// 0x00593040 Log::push
void push_event3(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x005932e0 InputQueue::pop_5932e0
std::int32_t pop_event() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::InputQueue_pop_5932e0, this); }
/// 0x005939f0 Stream::forward
std::uint32_t cursor_in_triangle(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5);
/// 0x00593f60 Mouse::reset_boundary
std::uint32_t reset_boundary() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Mouse_reset_boundary, this); }
/// 0x00594490 Obj::try_slots
std::int32_t double_click_in(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::int32_t a6);
/// 0x005947c0 Obj::play_5947c0
std::int32_t click_count_in(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::int32_t a6);
/// 0x00594ab0 Region::hit
std::int32_t region_hit(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6);
/// 0x00595aa0 Buttons::state
std::int32_t button_down_state(std::int32_t a0);
/// 0x00598a90 Obj::call_598ac0
void reset_keys_thunk();
/// 0x00631770 get6aa
std::uint32_t flag_6aa();
/// 0x0063e420 Obj::flag_66d
std::int32_t flag_66d();
/// 0x00592cd0 Input::release_592cd0
void release_592cd0() { ::joan::call::thiscall<void>(::joan::fn::Input_release_592cd0, this); }
/// 0x005973d0 Obj::stub_5973d0
std::uint32_t lock_area_nop(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3);
