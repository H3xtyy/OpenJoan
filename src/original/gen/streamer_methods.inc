// Generated (re/facades/gen2.py): typed members for the Streamer functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00579040 Streamer::shutdown
void* shutdown();
/// 0x00579220 Streamer::enqueue_signal
void enqueue_signal(void* a0);
/// 0x00579440 Streamer::cancel
void cancel(void* a0);
/// 0x00579e10 Streamer::sector_size
std::uint32_t sector_size(const char* a0);
/// 0x00579f10 Streamer::can_start
std::int32_t can_start();
/// 0x00579f40 Streamer::add_active
void add_active();
/// 0x0057a040 Streamer::enqueue
void enqueue(void* a0);
/// 0x0057a220 Streamer::remove
void remove(void* a0);
/// 0x00578d70 PalStream::init
void init();
/// 0x005795b0 Stream::requeue
std::int32_t requeue(void* a0, std::int32_t a1);
/// 0x00579bc0 SoundThread::finish
void finish_active(std::uint32_t a0, std::uint32_t a1);
/// 0x00579ce0 Stream::take_pending
std::int32_t take_pending();
/// 0x00579f70 Obj::dec_4c
void* dec_count_4c();
/// 0x00579fa0 PalStream::pump
std::int32_t pump();
