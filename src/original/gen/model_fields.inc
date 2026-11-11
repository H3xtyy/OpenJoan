// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Model
// (r4_r1.json, confidence medium). Do not edit; change the spec.
    std::int8_t loaded;  // 0x0
    std::uint8_t gap_1[0x1];
    std::int16_t kind;  // 0x2
    char name[32];  // 0x4
    std::uint8_t u8_24;  // 0x24
    std::uint8_t gap_25[0x3];
    Plane* mesh;  // 0x28
    Skin* skin;  // 0x2c
    std::uint32_t has_sphere;  // 0x30
    float sphere[4];  // 0x34
    BoneTree* bones;  // 0x44
    std::uint32_t u32_48;  // 0x48
    FileLoader* stream;  // 0x4c
    FileLoader* lod_streams[4];  // 0x50
    FileLoader* shadow_stream;  // 0x60
    FileLoader* volume_stream;  // 0x64
    std::uint8_t load_flag;  // 0x68
