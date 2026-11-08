// Generated (re/facades/gen2.py): typed members for the Billboard functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00522940 Billboard::set_plane
void set_plane(void* a0);
/// 0x006351a0 Billboard::Billboard
void* construct();
/// 0x00635260 Billboard::create
void* create(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, const char* a5, std::int16_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11, std::uint32_t a12, std::uint32_t a13, std::uint32_t a14);
/// 0x00635760 Billboard::place
void place(void* a0);
/// 0x00635d40 Billboard::set_position
void set_position(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00635d80 Billboard::set_15
void* set_15(std::uint32_t a0);
/// 0x00635db0 Billboard::set_19
void set_19(std::uint32_t a0);
/// 0x00635e80 Billboard::plane
void* plane();
/// 0x00635ea0 Billboard::centre
void* centre(void* a0, void* a1);
/// 0x006360f0 Billboard::blit
std::int32_t blit(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint32_t a4, void* a5, std::int16_t a6, std::int16_t a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::int16_t a11);
/// 0x00636640 Billboard::blit_more
std::int32_t blit_more(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::uint32_t a4, void* a5, std::int32_t a6, std::int16_t a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::int16_t a11);
/// 0x00636aa0 Billboard::normalise
void normalise();
/// 0x005ef780 Object::radius_a
float half_width();
/// 0x005ef7b0 Object::radius_b
float half_height();
/// 0x00635230 ObjA::~ObjA
std::int32_t destruct();
/// 0x006356c0 Holder::release
void* release(std::uint32_t a0);
/// 0x00635de0 Obj::set_colors
std::uint32_t set_colors(std::uint32_t a0);
/// 0x00636ff0 Overlay::quad_636ff0
std::uint32_t quad(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, void* a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::uint32_t a10, std::uint32_t a11);
/// 0x00636040 Billboard::text
template <class T0>
std::int32_t draw_text(T0 a0, std::int16_t a1, std::int16_t a2, const char* a3, std::uint32_t a4, std::int16_t a5, std::int16_t a6, std::int16_t a7) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::int32_t>(::joan::fn::Billboard_text, this, a0, a1, a2, a3, a4, a5, a6, a7); }
/// 0x005aee40 Attachment::position_9
void* position(void* a0);
