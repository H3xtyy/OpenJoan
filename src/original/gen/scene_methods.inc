// Generated (re/facades/gen2.py): typed members for the Scene functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0044e5f0 Scene::load_script
std::int32_t load_script(void* a0, std::int32_t a1) { return ::joan::call::thiscall<std::int32_t>(::joan::fn::Scene_load_script, this, a0, a1); }
/// 0x00450ea0 Scene::create_trigger
std::int32_t create_trigger(void* a0, std::int32_t a1);
/// 0x004749c0 Scene::create_projective_shadow
std::int32_t create_projective_shadow(void* a0, std::int32_t a1);
/// 0x0047ad40 Scene::create_node
std::int32_t create_node(void* a0);
/// 0x0049b970 Scene::create_light_49b970
std::uint32_t create_light_49b970(void* a0, std::int32_t a1) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Scene_create_light_49b970, this, a0, a1); }
/// 0x005e8fc0 Scene::lit
std::uint8_t lit();
/// 0x005e8fe0 Scene::set_lit
void* set_lit(std::uint32_t a0);
/// 0x005e9a00 Scene::Scene
void* construct();
/// 0x005e9a90 Scene::~Scene
std::uint32_t dtor();
/// 0x005e9ac0 Scene::reset
void* reset();
/// 0x005e9b10 Scene::clear
std::uintptr_t clear();
/// 0x005e9e50 Scene::setup
std::uintptr_t setup(void* a0);
/// 0x005ea060 Scene::set_sky
void* set_sky(void* a0);
/// 0x005ea0d0 Scene::set_1c
std::uintptr_t set_1c(void* a0);
/// 0x005ea180 Scene::free_71
std::uintptr_t free_71();
/// 0x005ea1f0 Scene::add_actor
void add_actor(void* a0);
/// 0x005ea390 Scene::release_7d
void* release_7d();
/// 0x005ea460 Scene::add_node
void* add_node();
/// 0x005ea5e0 Scene::remove_group
std::int32_t remove_group(void* a0);
/// 0x005ea6e0 Scene::group
void* group(std::int32_t a0);
/// 0x005ea730 Scene::build_tree
void build_tree();
/// 0x005ea9d0 Scene::build_node
void* build_node(void* a0, std::int32_t a1, std::int32_t a2);
/// 0x005eb030 Scene::delete_30
void* delete_30();
/// 0x005eb150 Scene::clear_34_4
void* clear_34_4();
/// 0x005eb590 Scene::nop
void nop(void* a0);
/// 0x005eb5d0 Scene::add_light
void* add_light();
/// 0x005eb700 Scene::remove_light
std::int32_t remove_light(void* a0);
/// 0x005eb820 Scene::delete_lights
void* delete_lights();
/// 0x005eb8b0 Scene::active
std::uint32_t active();
/// 0x005eb8d0 Scene::draw
void draw(void* a0, void* a1);
/// 0x005ed9d0 Scene::set_camera
std::uintptr_t set_camera(void* a0);
/// 0x005eddc0 Scene::register_material
std::int32_t register_material(void* a0);
/// 0x005eded0 Scene::remove_sorted
void remove_sorted(void* a0);
/// 0x005edfd0 Scene::free_c5
void* free_c5();
/// 0x005ee0d0 Scene::shadows_on
std::uint32_t shadows_on();
/// 0x005ee140 Scene::set_fog_flag
void* set_fog_flag(std::int32_t a0);
/// 0x005eeb50 Scene::get_5eeb50
std::uintptr_t get_5eeb50(std::int32_t a0);
/// 0x005eebc0 Scene::lights_off
void lights_off();
/// 0x005eec30 Scene::lights_on
void lights_on();
/// 0x005eeca0 Scene::add_entry
std::int32_t add_entry();
/// 0x005eed70 Scene::count_5c
std::int32_t count_5c();
/// 0x005eed90 Scene::occluder
void* occluder(std::int32_t a0);
/// 0x005eedc0 Scene::remove_5c
std::uint32_t remove_5c(std::int32_t a0);
/// 0x005ef380 Scene::end_frame
void* end_frame(void* a0);
/// 0x005ef3b0 Scene::occluder_count
std::int32_t occluder_count();
/// 0x005ef610 Scene::nodes_in_box
void** nodes_in_box(void* a0, void* a1);
/// 0x005ef690 Scene::regions
void* regions(void* a0, void* a1);
/// 0x005ef730 Scene::set_mode
void* set_mode(std::uint32_t a0);
/// 0x005efc50 Scene::attach
void attach(void* a0);
/// 0x005efdb0 Scene::detach
std::uint32_t detach(void* a0);
/// 0x005f0040 Scene::new_child
void* new_child();
/// 0x005f31c0 Scene::update_5f31c0
std::uint32_t update_5f31c0(void* a0);
/// 0x005f35f0 Scene::push
void push(void* a0);
/// 0x005f37c0 Scene::update_5f37c0
void update_5f37c0(void* a0, std::int32_t a1);
/// 0x00692c10 Scene::segment_blocked
std::int32_t segment_blocked(void* a0, void* a1, void* a2);
/// 0x00731b00 Scene::parse_light
std::uint32_t parse_light(void* a0);
/// 0x00731ff0 Scene::parse_model
std::uint32_t parse_model(void* a0);
/// 0x004201f0 sdd_004201f0
void* destroy(std::uint32_t a0);
/// 0x00484ad0 Recd5::save
std::int32_t save(void* a0);
/// 0x00484b60 Obj::clear_484b60
void* clear_pointers();
/// 0x004d29c0 X::get_68
std::uint32_t flag_68();
/// 0x004d2a00 Obj::dword_6c
std::uint32_t flag_6c();
/// 0x005ea410 Array::at1
void* actor_at(std::int32_t a0);
/// 0x005ea6c0 Obj::get_24_5ea6c0
std::uint32_t get_24();
/// 0x005edd90 Obj5ed::name
void* pick();
/// 0x005eeba0 Obj::get4c
std::uint32_t get_4c();
/// 0x005ef3d0 Obj::set_68
void set_68(std::uint32_t a0);
/// 0x005ef400 Object::set_6c
void set_6c(std::uint32_t a0);
/// 0x005ebac0 Scene::render
std::uint32_t render(void* a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::Scene_render, this, a0); }
/// 0x005ee1d0 Scene::draw_shadows
void draw_shadows(void* a0, void* a1, std::int32_t a2, std::int32_t a3) { ::joan::call::thiscall<void>(::joan::fn::Scene_draw_shadows, this, a0, a1, a2, a3); }
