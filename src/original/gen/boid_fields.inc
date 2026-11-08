// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Boid
// (r5_image.json, confidence high). Do not edit; change the spec.
    void* vtable;  // 0x0
    Actor* actor;  // 0x4
    std::int16_t kind;  // 0x8
    std::uint16_t u16_a;  // 0xa
    std::uint16_t member_index;  // 0xc
    float radius;  // 0xe
    float pos[3];  // 0x12
    float vel[3];  // 0x1e
    float dir[3];  // 0x2a
    float speed;  // 0x36
    std::int16_t neighbour_count;  // 0x3a
    std::uint16_t u16_3c;  // 0x3c
    Boid* nearest;  // 0x3e
    Boid* u32_42;  // 0x42
    float nearest_dist;  // 0x46
    std::uint32_t u32_4a;  // 0x4a
    float prev_pos[3];  // 0x4e
    float prev_vel[3];  // 0x5a
    Boid* next_;  // 0x66
    Boid* prev;  // 0x6a
    std::int8_t avoiding;  // 0x6e
    float avoid_blend;  // 0x6f
    float avoid_dir[3];  // 0x73
