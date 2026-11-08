// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of TrackNode
// (r9f_x64.json, confidence medium). Do not edit; change the spec.
    std::int32_t id;  // 0x0
    std::int32_t i32_4;  // 0x4
    float pos[3];  // 0x8
    std::int32_t link_count;  // 0x14
    TrackNodePtr* links;  // 0x18
    std::uint32_t search_stamp;  // 0x1c
    TrackNode* parent;  // 0x20
    TrackNodePtr* children;  // 0x24
    TrackNode* next_open;  // 0x28
    std::uint32_t grid_x;  // 0x2c
    std::uint32_t grid_y;  // 0x30
    std::int16_t i16_34;  // 0x34
    std::int16_t child_count;  // 0x36
    std::int16_t child_cap;  // 0x38
    std::uint8_t gap_3a[0x1];
    std::int8_t closed;  // 0x3b
    std::int32_t cost_so_far;  // 0x3c
    std::int32_t cost_total;  // 0x40
    std::int32_t cost_heuristic;  // 0x44
    std::int16_t weight;  // 0x48
    std::int16_t weight_max;  // 0x4a
    std::int32_t weight_sum;  // 0x4c
    PathGraph* owner;  // 0x50
