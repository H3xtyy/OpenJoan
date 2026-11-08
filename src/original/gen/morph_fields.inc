// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Morph
// (r3_A.json, confidence high). Do not edit; change the spec.
    std::uint8_t loaded;  // 0x0
    char name[51];  // 0x1
    std::int32_t target_count;  // 0x34
    MorphTarget* targets;  // 0x38
    std::int32_t vertex_base;  // 0x3c
    std::int32_t vertex_count;  // 0x40
    ArrI32* vertex_indices;  // 0x44
    std::uint32_t blend_start;  // 0x48
    std::uint32_t blend_end;  // 0x4c
    std::int32_t blend_duration;  // 0x50
    std::uint32_t last_update;  // 0x54
    Vec3f* positions;  // 0x58
    Vec3f* normals;  // 0x5c
    Vec3f* position_step;  // 0x60
    Vec3f* normal_step;  // 0x64
    Vec3f* base_positions;  // 0x68
    Vec3f* base_normals;  // 0x6c
    std::int32_t current_target;  // 0x70
    std::int32_t next_target;  // 0x74
    std::uint32_t anim_playing;  // 0x78
    std::uint32_t anim_start;  // 0x7c
    MorphAnim* anim;  // 0x80
