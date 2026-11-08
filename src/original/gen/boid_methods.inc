// Generated (re/facades/gen3.py): typed members for the Boid functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x004203e0 Obj::ctor_4203e0
void* construct();
/// 0x00420420 Particle::init
void* init(std::int32_t a0, std::int32_t a1);
/// 0x004207d0 Holder::release
void release_actor();
/// 0x00420890 Projectile::update_420890
void update(std::uint32_t a0, Boid* a1);
/// 0x00421ba0 Cam::pick_nearest
std::int32_t pick_nearest(void* a0);
/// 0x00421dd0 Mover::hits_box_421dd0
std::uint32_t hits_box();
/// 0x00422bd0 Obj::push_a33650
void push_neighbour(Boid* a0);
/// 0x00422c30 Particle::reset_group
void reset_neighbours();
/// 0x00422f70 Obj::set_66
void set_next(Boid* a0);
/// 0x00422fa0 Obj::get_66
Boid* next();
/// 0x00422fc0 Obj::set_6a
void set_prev(Boid* a0);
/// 0x00423030 Link::relink
std::uint32_t unlink();
/// 0x00423190 Obj::dtor_423190
void destruct();
/// 0x00422a40 Obj::segment_hit_in_range
bool segment_hit_in_range(void* a0, void* a1, void* a2, void* a3, void* a4);
/// 0x00422b90 Vec::add_length
template <class T1>
double add_length(void* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<double>(::joan::fn::Vec_add_length, this, a0, a1); }
