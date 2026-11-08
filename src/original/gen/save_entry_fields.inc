// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of SaveEntry
// (r5_image.json, confidence low). Do not edit; change the spec.
    std::int32_t type;  // 0x0
    std::uint32_t target_kind;  // 0x4
    std::uint16_t target_id;  // 0x8
    std::uint32_t ref_kind;  // 0xa
    std::uint16_t ref_id;  // 0xe
    std::uint16_t sub;  // 0x10
    std::uint8_t u8_12;  // 0x12
    std::uint32_t values[3];  // 0x13
    std::uint32_t range;  // 0x1f
    std::uint8_t follow_ref;  // 0x23
    std::uint8_t gap_24[0x2];
    std::uint32_t next_time;  // 0x26
    std::uint32_t expire_time;  // 0x2a
    std::uint8_t active;  // 0x2e
    std::uint8_t gap_2f[0x10];
    SceneS* scene;  // 0x3f
