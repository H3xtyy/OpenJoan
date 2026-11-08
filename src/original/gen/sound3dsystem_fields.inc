// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Sound3DSystem
// (r3_D.json, confidence medium). Do not edit; change the spec.
    std::uint8_t distance_model;  // 0x0
    std::uint8_t gap_1[0x9];
    float doppler_factor;  // 0xa
    float doppler_velocity;  // 0xe
    void* device;  // 0x12
    std::uint16_t u16_16;  // 0x16
    Sound3DBufferPtrArray* buffers;  // 0x18
    std::int16_t buffer_count;  // 0x1c
    std::int16_t buffer_capacity;  // 0x1e
    Sound3DPtrArray* sources;  // 0x20
    std::int16_t source_count;  // 0x24
    std::int16_t source_capacity;  // 0x26
    Listener* listener;  // 0x28
