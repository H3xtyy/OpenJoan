// Generated (re/facades/gen2.py): typed members for the Fog functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x005bff40 Fog::create_texture
std::int32_t create_texture(void* a0, std::int32_t a1, std::int32_t a2);
/// 0x005bff60 Fog::fill_range_texture
std::int32_t fill_range_texture();
/// 0x005c00b0 Fog::fill_height_texture
std::int32_t fill_height_texture(float a0);
/// 0x005c0460 Fog::apply
std::uint32_t apply(std::int32_t a0, std::int32_t a1, float a2, float a3, float a4);
/// 0x005c09a0 Fog::off
void off();
/// 0x005c09f0 Fog::kinds
std::uint32_t kinds();
/// 0x005c0c30 Fog::set
void set(std::uint32_t a0, float a1, float a2);
/// 0x005c0cf0 Fog::range_texture
std::int32_t range_texture(std::int32_t a0, std::int32_t a1);
/// 0x005c0da0 Fog::height_texture
std::int32_t height_texture(std::int32_t a0, std::int32_t a1);
/// 0x005c0e50 Fog::begin
void begin();
/// 0x005bfd80 Obj50::zero
void* construct();
/// 0x005bfdb0 Obj5bf::~Obj5bf
void destruct();
/// 0x005bfde0 TexCache::init
std::int32_t init();
/// 0x005bfed0 Device::release
std::uint32_t release_textures();
/// 0x00613b50 Object::scalar_delete_613b50
void* destroy(std::uint32_t a0);
/// 0x005c0a70 Fog::set_range
template <class T4, class T5>
void set_range(std::uint32_t a0, float a1, float a2, float a3, T4 a4, T5 a5) { static_assert(!std::is_pointer_v<T4> && std::is_trivially_copyable_v<T4>, "a struct by value"); static_assert(!std::is_pointer_v<T5> && std::is_trivially_copyable_v<T5>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::Fog_set_range, this, a0, a1, a2, a3, a4, a5); }
