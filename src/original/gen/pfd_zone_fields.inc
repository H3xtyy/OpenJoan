// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of PfdZone
// (r4_r2.json, confidence medium). Do not edit; change the spec.
    std::int32_t id;  // 0x0
    std::int32_t kind;  // 0x4
    std::int32_t bbox_min[3];  // 0x8
    std::int32_t bbox_max[3];  // 0x14
    std::int32_t blocked;  // 0x20
    std::int32_t name_id;  // 0x24
    std::int32_t point_count;  // 0x28
    std::int32_t visit_stamp;  // 0x2c
    PfdPointPtrArray* points;  // 0x30
    PfdZonePtrArray* links;  // 0x34
    std::int32_t search_stamp;  // 0x38
    PfdZone* came_from;  // 0x3c
    PfdZonePtrArray* children;  // 0x40
    PfdZone* open_next;  // 0x44
    std::int32_t centre_x;  // 0x48
    std::int32_t centre_y;  // 0x4c
    std::int16_t centre_z;  // 0x50
    std::int16_t child_count;  // 0x52
    std::uint8_t gap_54[0x3];
    std::uint8_t closed;  // 0x57
    std::uint32_t cost_g;  // 0x58
    std::int32_t cost_f;  // 0x5c
    std::uint32_t cost_h;  // 0x60
    std::uint16_t flags;  // 0x64
    std::int16_t max_flags;  // 0x66
    std::uint32_t flags_sum;  // 0x68
    PathSpace* path_space;  // 0x6c
    std::int32_t group;  // 0x70
