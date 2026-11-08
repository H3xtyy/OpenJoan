// Generated (re/facades/gen3.py): typed members for the SpriteQueue functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00631d10 Obj::true_631d10
std::uint32_t build_hook();
/// 0x00631d30 Obj::true_631d30
std::uint32_t close_hook();
/// 0x00631d50 0x00631d50
std::uint32_t begin_frame();
/// 0x00631d80 0x00631d80
std::int32_t end_frame();
/// 0x00631dc0 0x00631dc0
std::int32_t release_list();
/// 0x006332b0 SpriteQueue::blit_image
std::int32_t blit_image(std::int16_t a0, std::int16_t a1, std::uint32_t a2, void* a3, std::uint32_t a4, std::int16_t a5, std::int16_t a6, std::int16_t a7, std::int16_t a8, std::int16_t a9, std::int16_t a10, std::int16_t a11, std::int16_t a12, std::int16_t a13, std::int16_t a14);
/// 0x006349b0 Layer2D::place_model
std::uint32_t place_model(std::int32_t a0, std::int32_t a1, std::uint32_t a2, void* a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Layer2D_place_model, this, a0, a1, a2, a3, a4, a5, a6, a7); }
