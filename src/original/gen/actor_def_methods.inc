// Generated (re/facades/gen2.py): typed members for the ActorDef functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x005af390 ActorDef::ActorDef
void* construct();
/// 0x005af3d0 ActorDef::load2
void load2(const char* a0, const char* a1, std::uint8_t a2, const char* a3, void* a4);
/// 0x005af5d0 ActorDef::load(anim)
void load_anim(const char* a0, const char* a1, std::uint32_t a2, const char* a3, void* a4);
/// 0x005af890 ActorDef::poll_load
void poll_load();
/// 0x005afda0 ActorDef::loaded
std::int32_t loaded();
/// 0x005afdd0 ActorDef::load
void load(const char* a0, const char* a1, std::int16_t a2, std::uint8_t a3);
/// 0x005b0000 ActorDef::load(kind)
void load_kind(const char* a0, const char* a1, std::uint32_t a2, std::uint32_t a3);
/// 0x005b02f0 ActorDef::create
void* create(Plane* a0, const void* a1);
/// 0x005b03d0 ActorDef::release
std::uint32_t release();
/// 0x005b05d0 ActorDef::scalar_deleting_destructor
void* scalar_deleting_destructor(std::uint32_t a0);
/// 0x005b0620 ActorDef::load_bones
std::int32_t load_bones(const char* a0, const char* a1, void* a2);
/// 0x005b0850 ActorDef::setup_bones
std::int32_t setup_bones(void* a0, void* a1);
/// 0x005b0a50 ActorDef::~ActorDef
std::uint32_t dtor();
/// 0x005b0ab0 ActorDef::create_lods
void create_lods();
/// 0x005b1180 ActorDef::queue_instance
std::int32_t queue_instance(void* a0, void* a1, std::int32_t a2, void* a3);
/// 0x0047b5b0 wrap_0047b5b0
std::uint32_t destruct();
/// 0x004b0c20 Object::sdd_4b0c20
void* destroy(std::uint32_t a0);
/// 0x005b1cb0 ActorDef::draw_instances
void draw_instances(void* a0, std::int32_t a1, std::int32_t a2) { ::joan::call::thiscall<void>(::joan::fn::ActorDef_draw_instances, this, a0, a1, a2); }
