// Generated (re/facades/gen2.py): typed members for the Camera functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0042bcd0 Camera::move_42bcd0
std::uint32_t move_42bcd0(std::uint32_t a0, std::int32_t a1);
/// 0x00438720 Camera::step_height
void step_height(std::int32_t a0);
/// 0x0043f000 Camera::terrain_clamp_43f000
void terrain_clamp_43f000(std::uint32_t a0);
/// 0x00445990 Camera::update_follow_445990
void update_follow_445990(std::int8_t a0);
/// 0x005c58a0 Camera::init
std::uint32_t init(std::uint32_t a0);
/// 0x005c67f0 Camera::wrap_5c7b80
std::uint32_t wrap_5c7b80();
/// 0x005d80a0 Camera::type
void* type();
/// 0x0060fa50 Camera::get_1c
std::uint32_t get_1c();
/// 0x0060fa70 Camera::get_20
std::uint32_t get_20();
/// 0x00613bf0 Camera::set_viewport
std::uint32_t set_viewport(Surface* a0, void* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::View3D_set_view_area, this, a0, a1, a2, a3, a4, a5); }
/// 0x00613ca0 Camera::set_ranges
void set_ranges(float a0, float a1, float a2);
/// 0x00613d30 Camera::set_cf
void set_cf(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00613db0 Camera::set_scene
void set_scene(Scene* a0);
/// 0x00613de0 Camera::bone
void* bone();
/// 0x00614040 Camera::set_frame
void set_frame(void* a0);
/// 0x006140a0 Camera::frame
void* frame(void* a0);
/// 0x00614170 Camera::dirty
void dirty();
/// 0x006141a0 Camera::update_matrices
std::int32_t update_matrices();
/// 0x00614630 Camera::begin_frame
std::int32_t begin_frame();
/// 0x006146e0 Camera::release_scene
std::uint32_t release_scene();
/// 0x006153c0 Camera::project
std::int32_t project(void* a0, void* a1, void* a2, void* a3);
/// 0x006154d0 Camera::screen_ray
void* screen_ray(std::int32_t a0, std::int32_t a1, void* a2);
/// 0x006156b0 Camera::box_in_view
std::int32_t box_in_view(void* a0);
/// 0x00615840 Camera::sphere_visible
std::int32_t sphere_visible(void* a0, float a1);
/// 0x006159f0 Camera::sphere_in_view
std::int32_t sphere_in_view(void* a0, float a1);
/// 0x00615ba0 Camera::occluder_visible
std::int32_t occluder_visible(std::int32_t a0, void* a1, std::int32_t a2);
/// 0x00615d70 Camera::box_near
std::int32_t box_near(void* a0);
/// 0x00615e10 Camera::sphere_near
std::int32_t sphere_near(void* a0);
/// 0x00615e80 Camera::near_distance
float near_distance(void* a0);
/// 0x00615ed0 Camera::sphere_occluded
std::int32_t sphere_occluded(void* a0, float a1);
/// 0x00616030 Camera::box_occluded
std::int32_t box_occluded(void* a0);
/// 0x00616150 Camera::occluder_planes
std::int32_t occluder_planes(void* a0);
/// 0x00616460 Camera::build_frustum
void build_frustum();
/// 0x006169d0 Camera::set_light
void* set_light(void* a0);
/// 0x00616b40 Camera::light
void* light();
/// 0x00616b80 Camera::apply_fog
std::uint32_t apply_fog(std::uint32_t a0);
/// 0x00616c10 Camera::set_fog_colour
void set_fog_colour(void* a0);
/// 0x00616e90 Camera::matrix
void* matrix();
/// 0x00616ec0 Camera::matrix_ee
void* matrix_ee();
/// 0x00616f20 Camera::view
void* view();
/// 0x00616f50 Camera::matrix_22e
void* matrix_22e();
/// 0x00616fe0 Camera::flags
std::uint32_t flags();
/// 0x006170a0 Camera::push_plane
void push_plane(void* a0);
/// 0x00617120 Camera::clear_planes
void* clear_planes();
/// 0x00617150 Camera::plane
void plane_617150(std::int32_t a0, void* a1);
/// 0x006171a0 Camera::plane
void* plane_6171a0(std::int32_t a0, void* a1);
/// 0x006171f0 Camera::plane_count
std::int32_t plane_count();
/// 0x00617220 Camera::box_visible
std::int32_t box_visible(void* a0);
/// 0x00617370 Camera::water_receivers
void water_receivers();
/// 0x00617af0 Camera::apply_clip_planes
void apply_clip_planes();
/// 0x00617cf0 Camera::end_clip
void end_clip();
/// 0x00617d50 Camera::617d50
std::uint32_t m_617d50();
/// 0x00631850 Camera::build_631850
std::uint32_t build_631850();
/// 0x0068a170 Camera::track_68a170
void track_68a170(std::uint32_t a0, void* a1, std::int32_t a2);
/// 0x006b0d10 Camera::reset_state
void reset_state(void* a0);
/// 0x00562910 Obj562910::scalar_deleting_dtor
void* destroy(std::uint32_t a0);
/// 0x00613650 Obj::ctor_613650
void* construct();
/// 0x00613700 dtor_613700
std::uint32_t destruct();
/// 0x00613ba0 Obj613::is_ready
std::int32_t is_ready();
/// 0x00616a90 Sound3d::set
std::uint32_t set_fog_range(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x00616b60 get58
std::uint32_t get_58();
/// 0x006179a0 Lights::set
void set_clip_plane(std::uint32_t a0, std::uint32_t a1, void* a2, void* a3);
/// 0x00614760 Camera::render
void render() { ::joan::call::thiscall<void>(::joan::fn::View3D_render, this); }
