// Generated (re/facades/gen2.py): typed members for the Reorder functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x005e6000 Reorder::ctor
void* ctor();
/// 0x005e61c0 Reorder::clear
void clear();
/// 0x005e67b0 Reorder::draw
void draw(void* a0, void* a1, std::int32_t a2);
/// 0x005e79f0 Reorder::slot_for
std::int32_t slot_for(std::int32_t a0, void* a1);
/// 0x005e7bb0 Reorder::set_count
void set_count(std::int32_t a0);
/// 0x005e7ce0 Reorder::find_or_insert
void* find_or_insert(std::int8_t a0);
/// 0x005e7ec0 Reorder::forward
std::uint32_t forward(void* a0, std::uint32_t a1, std::uint16_t a2, std::uint16_t a3, void* a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9);
/// 0x005e7f70 Reorder::add_span_fwd
std::int32_t add_span_fwd(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9);
/// 0x005e8400 Reorder::push_span
std::int32_t push_span(void* a0, std::uint32_t a1, std::int32_t a2, std::int32_t a3, std::uint32_t a4, Bone* a5, std::uint32_t a6);
/// 0x005e8540 Reorder::add_batch
std::int32_t add_batch(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, std::int32_t a5, std::int32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9);
/// 0x005e88c0 Reorder::add_span
std::int32_t add_span(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, Bone* a8, std::uint32_t a9);
/// 0x005e6110 Obj5e::~Obj5e
void destruct();
/// 0x005e61a0 Obj::call_5e8ab0
std::uint32_t begin_frame();
/// 0x005e6430 Mirror::reset
void reset_batches();
/// 0x005e64e0 LightManager::purge
void purge();
/// 0x005e76f0 LightManager::light_slot
std::int32_t light_slot(std::int32_t a0, void* a1);
/// 0x005e7870 LightManager::sector_slot
std::int32_t sector_slot(std::int32_t a0, void* a1);
/// 0x005e7b80 LightManager::state
void* state();
/// 0x00613b00 Object::scalar_delete_613b00
void* destroy(std::uint32_t a0);
/// 0x005e8ab0 span_begin
std::int32_t span_begin() { return ::joan::call::thiscall<std::int32_t>(::joan::fn::RenderPrimitive_cache_add_ref, this); }
