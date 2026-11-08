// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Trigger
// (r4_r2.json, confidence low). Do not edit; change the spec.
    std::uint32_t flags;  // 0x0
    std::uint32_t target_kind;  // 0x4
    std::int16_t target_id;  // 0x8
    std::uint32_t ref_kind;  // 0xa
    std::int16_t ref_id;  // 0xe
    std::int16_t sub;  // 0x10
    std::uint8_t gap_12[0x1];
    float center[3];  // 0x13
    float range;  // 0x1f
    std::uint8_t gap_23[0x1];
    std::int16_t value24;  // 0x24
    std::uint32_t next_time;  // 0x26
    std::uint32_t expire_time;  // 0x2a
    std::int8_t active;  // 0x2e
    std::int32_t radius;  // 0x2f
    std::uint8_t gap_33[0xc];
    SceneS* scene;  // 0x3f
