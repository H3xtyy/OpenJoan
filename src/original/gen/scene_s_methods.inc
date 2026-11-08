// Generated (re/facades/gen2.py): typed members for the SceneS functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x005eee40 SceneS::occluders
void occluders(void* a0);
/// 0x005effa0 SceneS::first_actor
void* first_actor();
/// 0x005f0270 SceneS::clear
void clear();
/// 0x005f0370 SceneS::casts_shadows
std::uint32_t casts_shadows();
/// 0x005f1cc0 SceneS::shadow_volumes
char shadow_volumes(void* a0, void* a1, void* a2);
/// 0x005f39a0 SceneS::push
void push(void* a0);
/// 0x005f3a70 SceneS::draw_projected
void draw_projected(void* a0, std::int32_t a1);
/// 0x005f3fe0 SceneS::sector_fade
std::int32_t sector_fade(void* a0);
/// 0x005f41f0 SceneS::actor_fade
std::int32_t actor_fade(void* a0);
/// 0x005e8f90 Sector::lit
std::uint8_t lit();
/// 0x005ef850 SceneGroup::~SceneGroup
std::uint32_t destruct();
/// 0x005eff10 SceneGroup::release_ce
std::uint32_t release_actors();
/// 0x005f0340 Obj::set_flag_df
void* set_casts_shadows(std::uint8_t a0);
/// 0x005f2730 LightList::get
void* light_at(std::int32_t a0);
/// 0x005f03b0 SceneS::draw
void draw(void* a0, void* a1, std::int32_t a2) { ::joan::call::thiscall<void>(::joan::fn::SceneSector_render_ordered, this, a0, a1, a2); }
/// 0x005f07f0 SceneS::draw_lists
void draw_lists(void* a0, std::int32_t a1, std::int32_t a2) { ::joan::call::thiscall<void>(::joan::fn::SceneSector_render, this, a0, a1, a2); }
/// 0x005f0f80 SceneS::draw_receivers
void draw_receivers(void* a0, void* a1, std::int32_t a2, std::int32_t a3) { ::joan::call::thiscall<void>(::joan::fn::SceneS_draw_receivers, this, a0, a1, a2, a3); }
