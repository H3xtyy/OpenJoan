// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Game
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::uint8_t active;  // 0x0
    std::uint8_t init_done;  // 0x1
    std::uint8_t resources_loaded;  // 0x2
    std::uint8_t interface_loaded;  // 0x3
    std::uint8_t arrays_loaded;  // 0x4
    std::uint8_t u8_5;  // 0x5
    std::uint8_t u8_6;  // 0x6
    std::uint32_t u32_7;  // 0x7
    std::uint32_t u32_b;  // 0xb
    std::uint32_t u32_f;  // 0xf
    std::uint32_t u32_13;  // 0x13
    Scenario scenario;  // 0x17
    std::uint8_t gap_2a0[0x14];
    std::uint32_t mode_timers[20];  // 0x2b4
    std::uint32_t mode2_timer;  // 0x304
    std::uint8_t end_state;  // 0x308
    std::int16_t end_condition_count;  // 0x309
    EndCon end_conditions[40];  // 0x30b
    std::uint8_t u8_71b;  // 0x71b
    ArrI16* word_set;  // 0x71c
    std::int32_t word_set_count;  // 0x720
    std::int32_t word_set_cap;  // 0x724
    ArrI16* word_stack;  // 0x728
    std::int32_t word_stack_count;  // 0x72c
    std::int32_t word_stack_cap;  // 0x730
    std::uint32_t u32_734;  // 0x734
    std::uint32_t end_delay;  // 0x738
    std::uint32_t end_since;  // 0x73c
    std::uint32_t u32_740;  // 0x740
    std::uint8_t cutscene_flag;  // 0x744
