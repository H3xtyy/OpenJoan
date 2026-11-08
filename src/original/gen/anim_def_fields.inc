// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of AnimDef
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::int8_t loaded;  // 0x0
    std::uint8_t source_set;  // 0x1
    std::uint16_t u16_02;  // 0x2
    std::uint8_t gap_4[0x2];
    char dir[257];  // 0x6
    char name[33];  // 0x107
    std::int32_t track_count;  // 0x128
    std::int32_t action_count;  // 0x12c
    std::uint32_t start;  // 0x130
    std::uint32_t end;  // 0x134
    AnimAction* actions;  // 0x138
    ArrI32* sort_index;  // 0x13c
    Track* tracks;  // 0x140
    BoneTree* bones;  // 0x144
    std::uint32_t bones_resolved;  // 0x148
    std::int16_t child_count;  // 0x14c
    std::uint8_t quiet;  // 0x14e
    std::int32_t extra_count;  // 0x14f
    ColourTrack* colour_tracks;  // 0x153
    AlphaTrack* alpha_tracks;  // 0x157
    EventList* events;  // 0x15b
    AnimDef* children[8];  // 0x15f
    void* loader_17f;  // 0x17f
    void* loader_183;  // 0x183
    void* loader_187;  // 0x187
