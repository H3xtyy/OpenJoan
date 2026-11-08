// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of MusicStream
// (r4_r1.json, confidence medium). Do not edit; change the spec.
    std::int16_t source;  // 0x0
    void* file;  // 0x2
    std::int32_t file_start;  // 0x6
    std::uint8_t* data;  // 0xa
    std::uint32_t data_pos;  // 0xe
    std::int16_t width;  // 0x12
    std::int16_t height;  // 0x14
    std::uint16_t frame_count;  // 0x16
    std::uint16_t frame;  // 0x18
    std::uint8_t user_buffers;  // 0x1a
    std::uint8_t* pixels;  // 0x1b
    std::uint8_t* palette;  // 0x1f
    MusicStream* prev;  // 0x23
    MusicStream* next;  // 0x27
    std::uint8_t loop;  // 0x2b
    void* palette_cb;  // 0x2c
