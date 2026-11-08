// Generated (re/facades/gen3.py): typed members for the ScrollBar functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00761720 ModeList::ctor_761720
void* construct();
/// 0x007617c0 ModeList::dtor_7617c0
std::uint32_t destruct();
/// 0x00761ae0 ScrollBar::build
std::int32_t build(BmpRes* a0, void* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8);
/// 0x007620c0 Obj::init_7620c0
std::int32_t init(BmpRes* a0, void* a1, std::uint16_t a2);
/// 0x00762600 Obj::set_label_762600
std::int32_t set_thumb(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, const char* a4);
/// 0x00762690 Obj::set_label_762690
std::int32_t set_arrows(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, const char* a4, const char* a5, const char* a6, std::uint32_t a7, std::uint32_t a8);
/// 0x00762770 Obj::setup_762770
std::int32_t setup_track(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7);
/// 0x00762880 Obj::reset_762880
std::int32_t reset();
/// 0x00762a00 Scroller::step_762a00
std::int32_t update();
/// 0x00763530 HudSlots::draw_763530
std::uint32_t draw(std::int32_t a0, std::int32_t a1) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::HudSlots_draw_763530, this, a0, a1); }
/// 0x00761990 X::set_27
template <class T0>
std::uint32_t set_layout(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::X_set_27, this, a0); }
