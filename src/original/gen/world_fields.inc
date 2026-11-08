// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of World
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::uint8_t active;  // 0x0
    std::uint8_t loaded;  // 0x1
    std::int16_t world_id;  // 0x2
    std::uint16_t id;  // 0x4
    char name[33];  // 0x6
    char title[33];  // 0x27
    float bounds[6];  // 0x48
    std::uint8_t u8_60;  // 0x60
    BmpRes bitmaps;  // 0x61
    PathSpace* path_space;  // 0x6a
    Scene* scene;  // 0x6e
    std::int32_t cell_count;  // 0x72
    std::int32_t cell_capacity;  // 0x76
    CellPtrArray* cells;  // 0x7a
    BoxTree* box_tree;  // 0x7e
    void* cell_query;  // 0x82
    std::int32_t entry_count;  // 0x86
    std::int32_t entry_cap;  // 0x8a
    SceneSPtr* entries;  // 0x8e
    std::uint8_t gap_92[0x63];
    std::uint8_t u8_f5;  // 0xf5
    std::uint8_t gap_f6[0x107];
    std::uint32_t u32_1fd;  // 0x1fd
