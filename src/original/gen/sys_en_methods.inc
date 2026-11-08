// Generated (re/facades/gen2.py): typed members for the SysEn functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x005d2b40 SysEn::count
std::uint32_t count(std::uint32_t a0, std::uint32_t a1);
/// 0x005f7ca0 SysEn::create_buffers
void create_buffers();
/// 0x005f8030 SysEn::light_slot
std::uint32_t light_slot();
/// 0x005f80f0 SysEn::light_slot_free
void light_slot_free(std::int32_t a0);
/// 0x005f8110 SysEn::set_default_states
void set_default_states();
/// 0x005f8220 SysEn::vs_version
std::uint32_t vs_version();
/// 0x005ff180 SysEn::flush_extra
void flush_extra();
/// 0x005ff210 SysEn::flush_all
void flush_all();
/// 0x005ff290 SysEn::link_resource
void link_resource(void* a0);
/// 0x005ff380 SysEn::unlink_resource
void unlink_resource(void* a0);
/// 0x005ff9d0 SysEn::find_depth_format
std::uint32_t find_depth_format(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x005ffc90 SysEn::pick_depth_format
std::uint32_t pick_depth_format(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x006002b0 SysEn::error_string
const char* error_string(std::int32_t a0);
/// 0x00600310 SysEn::user_vertices
std::uint32_t user_vertices(void* a0, std::uint32_t a1);
/// 0x00600490 SysEn::fill_vertices
std::uint32_t fill_vertices(char a0, void* a1, std::uint32_t a2);
/// 0x00600510 SysEn::fill_indices
std::uint32_t fill_indices(void* a0, std::uint32_t a1, std::uint32_t a2);
/// 0x006005b0 SysEn::vertex_lock
void* vertex_lock(std::int32_t a0, std::uint32_t a1);
/// 0x00600740 SysEn::vertex_unlock
std::uint32_t vertex_unlock();
/// 0x00600820 SysEn::index_lock
void* index_lock(void* a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00600980 SysEn::index_unlock
std::uint32_t index_unlock();
/// 0x00600a00 SysEn::draw
std::uint32_t draw(std::int32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00600a90 SysEn::draw_indexed
std::uint32_t draw_indexed(std::int32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00600b30 SysEn::pass_filter
std::uint32_t pass_filter(std::uint32_t a0, std::uint32_t a1);
/// 0x00603140 SysEn::set_cull_flip
void set_cull_flip(std::int32_t a0);
/// 0x006035c0 SysEn::ctor_6035c0
void* ctor_6035c0();
/// 0x00603670 SysEn::dtor
std::uintptr_t dtor();
/// 0x00613730 SysEn::init_613730
std::uint32_t init_613730();
/// 0x0061aa50 SysEn::vs_material_constants
void vs_material_constants();
/// 0x005f6620 Obj::create_5f6620
std::int32_t create();
/// 0x005f6770 Renderer::shutdown
void shutdown();
/// 0x005f7f60 Sound3d::release
void release_buffers();
/// 0x005ff060 Device::buffer_count
std::uint32_t buffer_count();
/// 0x005ff0e0 Device::present
void present();
/// 0x005ff4a0 Display::reset_device
std::uint32_t reset_device();
/// 0x00600c20 Obj::reset_600c20
void reset_stats();
/// 0x00601140 RenderLog::dump
void dump_log(const char* a0);
/// 0x00602540 Device::probe_bump_602540
std::uint8_t probe_bump();
/// 0x00602a50 Obj::set_clamp_2ef
void set_brightness(float a0);
/// 0x00602ad0 Obj::set_clamp_2f3
void set_contrast(float a0);
/// 0x00602bd0 Obj::set_rgb_offsets
void set_rgb_offsets(std::int32_t a0, std::int32_t a1, std::int32_t a2);
/// 0x006034b0 Obj::release_323
void release_323();
/// 0x005fa250 set_texture_mode
void set_texture_mode(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::set_texture_mode, this, a0); }
/// 0x005f8250 set_blend
void set_blend(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::set_blend, this, a0); }
/// 0x006014d0 apply
void apply(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::apply, this, a0); }
/// 0x006024e0 set_vertex_type
void set_vertex_type(char a0) { ::joan::call::thiscall<void>(::joan::fn::set_vertex_type, this, a0); }
/// 0x00602080 set_flags
std::uint32_t set_flags(std::uint32_t a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::set_flags, this, a0); }
/// 0x006026a0 set_raster
void set_raster(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::set_raster, this, a0); }
/// 0x00601470 overlay_stage
std::int32_t overlay_stage() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::overlay_stage, this); }
/// 0x005fec10 set_filter
void set_filter(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::set_filter, this, a0); }
/// 0x006020f0 flags
std::uint32_t flags() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::flags, this); }
/// 0x005feb50 set_texture_factor
void set_texture_factor(std::uint32_t a0) { ::joan::call::thiscall<void>(::joan::fn::set_texture_factor, this, a0); }
/// 0x00602610 Tex::is_mipmapped
bool supports_mipmaps();
/// 0x005f6c00 Renderer::start_d3d
std::int32_t start_d3d() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::Sys3D_init_driver, this); }
