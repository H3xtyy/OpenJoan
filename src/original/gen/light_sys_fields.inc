// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of LightSys
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::int16_t set_count;  // 0x0
    LightPreset* sets;  // 0x2
    std::int16_t special_count;  // 0x6
    LightPreset* specials;  // 0x8
    LightPreset current;  // 0xc
    LightState state_a;  // 0x198
    LightState state_b;  // 0x32c
    std::uint32_t time;  // 0x4c0
    std::int16_t day_count;  // 0x4c4
    std::int16_t day_index;  // 0x4c6
    LightDay* days;  // 0x4c8
    char sky_name_b[64];  // 0x4cc
    char sky_name_c[64];  // 0x50c
    char sky_name_a[64];  // 0x54c
    float event_a_chance;  // 0x58c
    float event_a_rate;  // 0x590
    float event_a_time;  // 0x594
    std::uint32_t event_a_state;  // 0x598
    float event_b_chance;  // 0x59c
    float event_b_rate;  // 0x5a0
    float event_b_time;  // 0x5a4
    std::uint32_t event_b_state;  // 0x5a8
    Billboard* moon;  // 0x5ac
    std::int16_t light_count;  // 0x5b0
    Light* lights[9];  // 0x5b2
    LensFlares flares;  // 0x5d6
