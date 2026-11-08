// Generated (re/facades/gen2.py): typed members for the EffectState functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0041b170 EffectState::start
void start();
/// 0x0041b190 EffectState::stop
void stop();
/// 0x0041b1c0 EffectState::clear
void clear();
/// 0x0041b1e0 EffectState::started
std::uint8_t started();
/// 0x0041b280 EffectState::object
void* object();
/// 0x00675e90 EffectState::update_parts
void update_parts(float a0, void* a1);
/// 0x00677120 EffectState::parts_stop
void parts_stop();
/// 0x006771c0 EffectState::parts_hide
void parts_hide();
/// 0x00677250 EffectState::parts_start
std::uint32_t parts_start();
/// 0x006772f0 EffectState::parts_alive
std::int32_t parts_alive();
/// 0x0067a240 EffectState::parts_alloc
void parts_alloc();
/// 0x0041b200 Obj41b::set_24
void* set_model(Plane* a0);
/// 0x0041b230 Obj41b::get_24
Plane* model();
/// 0x0041b250 Obj41b::set_28
void* set_actor(Actor* a0);
/// 0x0041b300 Obj41b::get_60
std::int16_t source_blend();
/// 0x0041b320 Obj41b::set_60
void* set_source_blend(std::int16_t a0);
/// 0x0041b350 Obj41b::get_62
std::int16_t dest_blend();
/// 0x0041b370 Obj41b::set_62
void* set_dest_blend(std::int16_t a0);
/// 0x0041b3a0 Obj41b::ptr_3
void* name();
/// 0x006758f0 Obj::ctor_6758f0
void* construct();
/// 0x006759a0 ParticleHost::init
void init();
/// 0x00677730 ParticleProp::create_677730
void* create_key(const char* a0);
/// 0x00677e00 Container::clear
std::uint32_t clear_sequences();
/// 0x00677f00 ParticleSystem::parse_block
void parse_block(void* a0);
/// 0x00678320 ParticleSystem::parse_sequence
void parse_sequence(void* a0, void* a1);
/// 0x0067a1c0 Named::set_name
const char* set_name(const char* a0);
/// 0x0067a200 set_name_a6a28c
void set_error_text(const char* a0);
/// 0x0068c940 Entry::flag
std::uint32_t initialized();
/// 0x00677390 ParticleSystem::compile
bool compile(const char* a0);
/// 0x0041b2a0 EffectState::set_position
template <class T0>
void set_position(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); ::joan::call::thiscall<void>(::joan::fn::EffectState_set_position, this, a0); }
/// 0x0067ad10 X::set_2c
template <class T0>
std::uint32_t set_position_67ad10(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::X_set_2c, this, a0); }
/// 0x005aee80 Attachment::position_44
void* position(void* a0);
