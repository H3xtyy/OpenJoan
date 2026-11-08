// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Flock
// (r5_image.json, confidence high). Do not edit; change the spec.
    std::int16_t id;  // 0x0
    std::int16_t kind;  // 0x2
    std::uint8_t gap_4[0x2];
    Scene* scene;  // 0x6
    SceneS* group_node;  // 0xa
    std::uint8_t active;  // 0xe
    float max_speed;  // 0xf
    float max_urgency;  // 0x13
    float max_urgency2;  // 0x17
    float speed_urgency;  // 0x1b
    float half_speed;  // 0x1f
    float keep_away;  // 0x23
    float separate;  // 0x27
    float def_range;  // 0x2b
    std::uint8_t use_truth;  // 0x2f
    std::uint8_t react_enemy;  // 0x30
    std::int16_t width;  // 0x31
    std::int16_t height;  // 0x33
    std::int16_t num_flock;  // 0x35
    std::int16_t num_entity;  // 0x37
    std::int16_t spawned_groups;  // 0x39
    float box[6];  // 0x3b
    DynArray* members;  // 0x53
    DynArray* groups;  // 0x57
