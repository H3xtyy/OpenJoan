// Generated (re/facades/gen2.py): typed members for the Actor functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0041a930 Actor::attach_model
void* attach_model(void* a0, void* a1, void* a2);
/// 0x005048b0 Actor::attack_value
float attack_value(std::int32_t a0);
/// 0x005aaf10 Actor::release
void release_5aaf10();
/// 0x005ab470 Actor::visible
std::int32_t visible_5ab470(void* a0, std::int32_t a1);
/// 0x005ab620 Actor::visible
std::uint32_t visible_5ab620();
/// 0x005ab6e0 Actor::fade_update
void fade_update(void* a0);
/// 0x005abc50 Actor::draw_entry
void draw_entry(void* a0, void* a1, std::int32_t a2);
/// 0x005abe30 Actor::start_anim
void start_anim(void* a0, const char* a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5);
/// 0x005ac270 Actor::position
void* position(void* a0);
/// 0x005ac310 Actor::set_position
void set_position_5ac310(void* a0);
/// 0x005ac4b0 Actor::set_position
void set_position_5ac4b0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2);
/// 0x005acc30 Actor::get_xyz
void* get_xyz(void* a0, void* a1, void* a2);
/// 0x005acc80 Actor::bounding_sphere
void* bounding_sphere(void* a0, std::uint32_t a1);
/// 0x005ace10 Actor::matrix
void* matrix(void* a0);
/// 0x005acfa0 Actor::set_byte_fc
void* set_byte_fc(std::int8_t a0);
/// 0x005ad000 Actor::set_byte_fd
void set_byte_fd(std::uint8_t a0);
/// 0x005ad030 Actor::flag_fd
std::int32_t flag_fd();
/// 0x005add40 Actor::set_skin
void set_skin(std::uint32_t a0);
/// 0x005adf50 Actor::add_part
void* add_part(const char* a0, const char* a1, const char* a2, void* a3);
/// 0x005ae0c0 Actor::add_child
void* add_child(void* a0, std::uint32_t a1);
/// 0x005ae400 Actor::attach
void attach(const char* a0, const char* a1, const char* a2);
/// 0x005ae590 Actor::set_faded
void set_faded(char a0);
/// 0x005aed50 Actor::layer
std::uint32_t layer();
/// 0x005aede0 Actor::sub_count
std::int32_t sub_count();
/// 0x00675b30 Actor::release
std::uint32_t release_675b30(std::uint32_t a0);
/// 0x0044cee0 get_10a
std::uint8_t byte_10a();
/// 0x005ab450 Obj::stub_5ab450
void on_release();
/// 0x005acf60 Part::set
void* set_scene(Scene* a0);
/// 0x005aec90 ActorDefs::find
void* find_morph(const char* a0);
/// 0x005aba20 Actor::draw
void draw(void* a0, std::int32_t a1) { ::joan::call::thiscall<void>(::joan::fn::Actor_render, this, a0, a1); }
/// 0x005ab6a0 Actor::choose_mesh
std::uint32_t choose_mesh(void* a0, std::int32_t a1, void* a2, void* a3);
/// 0x005ad060 Actor::select_mesh
void* select_mesh(void* a0, std::int32_t a1, void* a2, void* a3);
/// 0x005aee10 Actor::sub_planes
void* sub_planes();
