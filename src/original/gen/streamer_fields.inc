// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Streamer
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::int32_t mode;  // 0x0
    std::int32_t thread;  // 0x4
    CriticalSection lock;  // 0x8
    FileLoader* tail;  // 0x20
    FileLoader* head;  // 0x24
    FileLoader* current;  // 0x28
    std::uint32_t u32_2c;  // 0x2c
    void* event_wake;  // 0x30
    void* event_done;  // 0x34
    void* event_38;  // 0x38
    void* event_3c;  // 0x3c
    void* event_finished;  // 0x40
    std::uint32_t u32_44;  // 0x44
    void* file;  // 0x48
    std::int32_t active_count;  // 0x4c
    std::uint32_t sector_size_cache[26];  // 0x50
