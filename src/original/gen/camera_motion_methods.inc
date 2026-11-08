// Generated (re/facades/gen2.py): typed members for the CameraMotion functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004237a0 CameraMotion::update
std::int32_t update();
/// 0x00423ec0 CameraMotion::start
void start(void* a0, void* a1);
/// 0x00423f40 CameraMotion::retarget
std::uint32_t retarget(void* a0) { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::CameraMotion_retarget, this, a0); }
/// 0x00423fb0 CameraMotion::current
void current(void* a0);
/// 0x00423fe0 CameraMotion::end
void* end(void* a0) { return ::joan::call::thiscall<void*>(::joan::fn::CameraMotion_end, this, a0); }
/// 0x00424010 CameraMotion::collide
void collide();
/// 0x004235d0 Obj4235d0::dtor
std::uint32_t destruct();
/// 0x00423600 CamMan::init_423600
void* init();
/// 0x004236d0 CamMan::free_list
std::uint32_t free_list();
/// 0x00423e70 Obj::set_flags_78_88_a4
void set_default_modes();
/// 0x00425a90 square
std::int32_t square(std::int32_t a0);
/// 0x00425ac0 vec_lerp
void* vec_lerp(void* a0, void* a1, void* a2, float a3);
/// 0x00425b90 angle_lerp
float angle_lerp(void* a0, void* a1, float a2, void* a3, void* a4);
/// 0x00425c80 lerp_float
float lerp(void* a0, void* a1, float a2);
/// 0x00562420 Obj562420::vector_deleting_dtor
void* vector_delete(std::uint32_t a0);
