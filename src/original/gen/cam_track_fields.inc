// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of CamTrack
// (r4_r1.json, confidence medium). Do not edit; change the spec.
    std::int16_t id;  // 0x0
    std::uint32_t kind;  // 0x2
    char name[64];  // 0x6
    std::uint8_t gap_46[0x2];
    float box[6];  // 0x48
    std::int16_t view_distance;  // 0x60
    float yaw_deg;  // 0x62
    float pitch_deg;  // 0x66
    std::int16_t per_key_angles;  // 0x6a
    std::int16_t scale_pitch;  // 0x6c
    std::int16_t face_unit;  // 0x6e
    std::int16_t free_pitch;  // 0x70
    std::int32_t key_count;  // 0x72
    CamKey keys[60];  // 0x76
    float rail_step;  // 0xac6
