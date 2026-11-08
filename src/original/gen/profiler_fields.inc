// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Profiler
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::uint32_t enabled;  // 0x0
    std::int32_t func_count;  // 0x4
    std::int32_t frames;  // 0x8
    std::int32_t frame_slot;  // 0xc
    std::uint32_t time_accum[2];  // 0x10
    std::uint32_t frame_avg[2];  // 0x18
    std::uint32_t clock[2];  // 0x20
    ArrU64* last;  // 0x28
    ArrU64* total;  // 0x2c
    ArrU64* start;  // 0x30
    ArrU32* depth;  // 0x34
    ArrU64* average;  // 0x38
    ArrU64* history;  // 0x3c
    ArrU32* history_hits;  // 0x40
    std::uint32_t cpu_hz[2];  // 0x44
    ProfFunc* funcs;  // 0x4c
    ProfNode* root;  // 0x50
    ProfNode* current;  // 0x54
    std::uint32_t last_report;  // 0x58
    std::uint32_t u32_5c;  // 0x5c
