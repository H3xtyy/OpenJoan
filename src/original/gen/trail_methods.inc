// Generated (re/facades/gen2.py): typed members for the Trail functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00687850 Trail::release_hold
void release_hold();
/// 0x00687880 Trail::update
void update();
/// 0x00687cc0 Trail::build
void build_687cc0(void* a0);
/// 0x006883d0 Trail::add_node
void add_node();
/// 0x00688550 Trail::age
void age(std::int16_t a0);
/// 0x006889c0 Trail::build
void build_6889c0();
/// 0x006891d0 Trail::set_scene
void set_scene(void* a0);
/// 0x00420790 sdd_00420790
void* destroy(std::uint32_t a0);
/// 0x00687130 Obj687130::constructor
void* construct();
/// 0x006871f0 UnitTask::init
void init(void* a0);
/// 0x00687410 SoundSource::release
void release();
/// 0x00687500 MeshDeform::update
void deform_update(std::int32_t a0);
/// 0x00687a60 Attach::update
void attach_update();
/// 0x00688fd0 Trail::hermite
template <class T1, class T2, class T3, class T4>
void* hermite(void* a0, T1 a1, T2 a2, T3 a3, T4 a4, float a5) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); static_assert(!std::is_pointer_v<T3> && std::is_trivially_copyable_v<T3>, "a struct by value"); static_assert(!std::is_pointer_v<T4> && std::is_trivially_copyable_v<T4>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::Trail_hermite, this, a0, a1, a2, a3, a4, a5); }
