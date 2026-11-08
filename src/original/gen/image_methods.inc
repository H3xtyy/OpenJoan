// Generated (re/facades/gen2.py): typed members for the Image functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0060fad0 Image::Image
void* construct();
/// 0x0060fb10 Image::~Image
std::uint32_t dtor();
/// 0x0060fb60 Image::create
std::uint32_t create(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::int32_t a3);
/// 0x0060fd20 Image::release
std::uint32_t release();
/// 0x004d84e0 X::get_c
void* bits();
/// 0x004d8520 X::get_10
std::uint32_t pitch();
/// 0x00596760 Obj::scalar_dtor_596760
void* destroy(std::uint32_t a0);
/// 0x0060fc20 Obj::attach_60fc20
void* attach_surface(void* a0);
/// 0x0060fd70 ComPtr::set_60fd70
std::uint32_t set_surface(void* a0);
/// 0x0060ff20 Obj::release_60ff20
std::uint32_t unlock();
/// 0x0060ffd0 BitQueue::shift
std::uint32_t pop_lock();
/// 0x00610160 rgb_to_pixel
std::uint32_t rgb_to_pixel(void* a0);
/// 0x00610550 Surface::read_pixel
std::int32_t read_pixel(void* a0);
/// 0x00610e10 Surface::blit_sprite
void blit_sprite(std::int32_t a0, std::int32_t a1, void* a2, void* a3, std::int32_t a4, void* a5, void* a6);
/// 0x00612f30 Blit::draw_612f30
void fill_rect(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4);
/// 0x00613090 Palette::channel
std::int32_t ramp_channel(void* a0, std::int32_t a1, std::int32_t a2);
