// Generated (re/facades/gen2.py): typed members for the TextureManager functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x006094c0 TextureManager::create_2d
void* create_2d();
/// 0x006096d0 TextureManager::new_render_target
void* new_render_target();
/// 0x0060a500 TextureManager::use
std::uint32_t use(void* a0);
/// 0x0060a670 TextureManager::set_name
std::uint32_t set_name(void* a0, const char* a1);
/// 0x0060a870 TextureManager::purge
void purge(std::int32_t a0);
/// 0x0060a990 TextureManager::trim_loaders
void trim_loaders(std::int32_t a0);
/// 0x0060ae70 TextureManager::find
std::int32_t find(void* a0);
/// 0x0060b0d0 TextureManager::remove_a
std::int32_t remove_a(void* a0);
/// 0x0060b140 TextureManager::remove_b
std::int32_t remove_b(void* a0);
/// 0x0060b2e0 TextureManager::add
void* add(void* a0);
/// 0x0060b3e0 TextureManager::remove_c
void* remove_c(void* a0);
/// 0x00608db0 Obj::clear_3c
void* construct();
/// 0x00608e10 TexPaths::build
std::uint32_t build_paths();
/// 0x00609260 TextureArray::clear
void clear();
/// 0x00609780 find_file_609780
std::uint8_t find_file(const char* a0, const char* a1, const char* a2);
/// 0x0060a770 TextureLoader::flush
void flush_loaders();
/// 0x0060ac20 SortedArray::insert
std::int32_t insert_named(std::uintptr_t a0, std::uintptr_t a1);
/// 0x0060ad40 SortedArray::add
std::int32_t add_sorted(std::uintptr_t a0);
/// 0x0060b000 SortedArray::find
std::int32_t find_sorted(std::uintptr_t a0);
/// 0x0060b1b0 TextureList::insert
std::int32_t insert_unused(void* a0);
/// 0x0060a740 Textures::set_mode
void* set_mode_60a740(std::uint32_t a0) { return ::joan::call::thiscall<void*>(::joan::fn::Textures_set_mode, this, a0); }
