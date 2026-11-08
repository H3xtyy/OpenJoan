// Generated (re/facades/gen2.py): typed members for the Sound3D functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0048d120 Sound3D::create_buffer_48d120
void* create_buffer_48d120();
/// 0x00490bc0 Sound3D::release_buffer
void release_buffer(Sound3DBuffer* a0);
/// 0x0059ede0 Sound3D::cleanup
void cleanup();
/// 0x0059f460 Sound3D::queue
void* queue(void* a0);
/// 0x0059f4b0 Sound3D::unqueue
void unqueue(std::int32_t a0);
/// 0x0059f680 Sound3D::stop
void stop();
/// 0x0059f700 Sound3D::set_gain
void set_gain(std::uint32_t a0);
/// 0x0059f920 Sound3D::set_max_distance
void set_max_distance(std::uint32_t a0);
/// 0x0059f970 Sound3D::set_reference_distance
void set_reference_distance(std::uint32_t a0);
/// 0x0059f9c0 Sound3D::set_rolloff
void set_rolloff(std::uint32_t a0);
/// 0x0059fa80 Sound3D::set_looping
void set_looping(std::uint8_t a0);
/// 0x0059fae0 Sound3D::set_relative
void set_relative(std::uint8_t a0);
/// 0x0059fb40 Sound3D::set_position
void set_position(void* a0);
/// 0x0059fc90 Sound3D::state
std::uint32_t state();
/// 0x0059fd20 Sound3D::processed
std::int32_t processed();
/// 0x0059fd60 Sound3D::queued_59fd60
std::int32_t queued_59fd60();
/// 0x005a0330 Sound3D::init_5a0330
std::uint32_t init_5a0330();
/// 0x005a04e0 Sound3D::release_5a04e0
void* release_5a04e0();
/// 0x00689d00 Sound3D::release
void release(char a0);
/// 0x0068aa10 Sound3D::detach_channels
void detach_channels();
/// 0x004907e0 Sound3d::alloc_source
Sound3D* alloc_source();
/// 0x0059ed90 Sound3DBuffer::~Sound3DBuffer
void destruct();
/// 0x0059f2e0 Obj::clear_53
void* construct();
/// 0x0059f310 Obj59f::dtor_59f310
std::uint32_t destruct_59f310();
/// 0x0059f400 ListA::step
std::uint32_t stop_and_unqueue();
/// 0x0059f6b0 Sound3d::set_pitch
void set_pitch(std::uint32_t a0);
/// 0x0059f770 AlSource::set_min_gain
void set_min_gain(float a0);
/// 0x0059f7c0 AlSource::set_max_gain
void set_max_gain(float a0);
/// 0x0059f810 Sound3d::set_rolloff
void set_cone_outer_gain(std::uint32_t a0);
/// 0x0059f860 AlSource::set_angle
void set_cone_inner_angle(std::uint32_t a0);
/// 0x0059f8c0 Sound3d::set_distance
void set_cone_outer_angle(std::uint32_t a0);
/// 0x005a0e90 Obj5a0e90::deleting_dtor
void* destroy(std::uint32_t a0);
