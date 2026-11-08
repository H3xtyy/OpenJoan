// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of WorldSector
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::uint8_t gap_0[0x8];
    float box[6];  // 0x8
    std::int16_t grid_x;  // 0x20
    std::int16_t grid_y;  // 0x22
    SceneS* owner;  // 0x24
    std::uint8_t gap_28[0x4];
    std::int16_t level;  // 0x2c
    std::uint8_t gap_2e[0xa];
    void* texture_a;  // 0x38
    void* model;  // 0x3c
    void* texture_b;  // 0x40
    std::uint8_t gap_44[0xa];
    std::uint32_t u32_4e;  // 0x4e
    std::uint32_t u32_52;  // 0x52
    std::uint8_t gap_56[0x8];
    WorldItem* nodes_tail;  // 0x5e
    WorldItem* nodes_head;  // 0x62
