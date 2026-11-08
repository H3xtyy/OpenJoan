// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of EffectState
// (r3_A.json, confidence medium). Do not edit; change the spec.
    std::uint8_t initialised;  // 0x0
    std::uint8_t started_;  // 0x1
    std::uint8_t visible;  // 0x2
    char name_[33];  // 0x3
    Plane* parts;  // 0x24
    Actor* object_;  // 0x28
    Range range;  // 0x2c
    float pos[3];  // 0x44
    float prev_pos[3];  // 0x50
    std::uint32_t yaw;  // 0x5c
    std::int16_t i16_60;  // 0x60
    std::int16_t i16_62;  // 0x62
    std::uint32_t owns_emitters;  // 0x64
    std::uint32_t share_refs;  // 0x68
    EffectState* shared_from;  // 0x6c
    DynArray* emitters;  // 0x70
    ParticlePool* pools;  // 0x74
