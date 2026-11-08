// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of EffectKey
// (r5_image.json, confidence high). Do not edit; change the spec.
    std::uint8_t initialised;  // 0x0
    float lifetime[2];  // 0x1
    float emit_rate[2];  // 0x9
    float emit_rate2[2];  // 0x11
    float spawn_dir[6];  // 0x19
    float emit_radius[6];  // 0x31
    float gravity[6];  // 0x49
    FloatRange range_61;  // 0x61
    std::uint8_t follow_camera;  // 0x69
    float orientation[3];  // 0x6a
    float orir;  // 0x76
    float offset[3];  // 0x7a
    float time_delay;  // 0x86
    std::uint8_t self_rotation;  // 0x8a
    float self_rot_axis[3];  // 0x8b
    float self_rot_speed[2];  // 0x97
    std::uint8_t has_rotation;  // 0x9f
    float rotation_speed[2];  // 0xa0
    float rotation_center[3];  // 0xa8
    float rotation_axis[3];  // 0xb4
    char model_name[33];  // 0xc0
    char texture_name[33];  // 0xe1
    char name_102[33];  // 0x102
    std::uint8_t rchange;  // 0x123
    float texture_period;  // 0x124
    std::int32_t texture_number;  // 0x128
    std::uint32_t u32_12c;  // 0x12c
    std::uint32_t u32_130;  // 0x130
    std::int32_t num_particles;  // 0x134
    std::int32_t loop;  // 0x138
    std::uint8_t is_smo;  // 0x13c
    std::uint8_t gap_13d[0x4];
    ActorDef* u32_141;  // 0x141
    DynArray keys;  // 0x145
