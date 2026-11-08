// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of CameraMotion
// (r3_C.json, confidence medium). Do not edit; change the spec.
    CamPose from;  // 0x0
    CamPose to;  // 0x28
    CamPose current_;  // 0x50
    std::uint32_t time_mode;  // 0x78
    std::uint32_t duration_ms;  // 0x7c
    float ease_in;  // 0x80
    float ease_out;  // 0x84
    std::uint32_t blend_mode;  // 0x88
    std::uint32_t blend_callback;  // 0x8c
    std::uint8_t gap_90[0x8];
    float t;  // 0x98
    std::uint32_t start_ms;  // 0x9c
    std::uint8_t gap_a0[0x4];
    std::uint32_t skip_collision;  // 0xa4
    float collide_[6];  // 0xa8
    CamTrackNode* tracked;  // 0xc0
    std::int32_t tracked_count;  // 0xc4
