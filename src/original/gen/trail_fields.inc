// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Trail
// (g10.json, confidence high). Do not edit; change the spec.
    Actor* actor;  // 0x0
    ActorDef* actor_def;  // 0x4
    Scene* scene;  // 0x8
    std::int32_t unit_id;  // 0xc
    std::uint32_t vtx_base;  // 0x10
    std::uint8_t attached;  // 0x14
    std::uint8_t enabled;  // 0x15
    float points[30];  // 0x16
    std::int16_t point_count;  // 0x8e
    std::int16_t node_count;  // 0x90
    std::int16_t rate_hz;  // 0x92
    std::int16_t max_nodes;  // 0x94
    std::uint8_t dirty;  // 0x96
    std::uint32_t hold_start;  // 0x97
    std::uint32_t hold_ms;  // 0x9b
    std::uint32_t last_add;  // 0x9f
    TrailNodes nodes;  // 0xa3
