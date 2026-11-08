// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of ActorDef
// (r5_image.json, confidence medium). Do not edit; change the spec.
    std::uint8_t state;  // 0x0
    std::uint8_t gap_1[0x2];
    std::int8_t loaded_;  // 0x3
    void* anim_file;  // 0x4
    void* user;  // 0x8
    Model* model;  // 0xc
    std::int16_t bone_count;  // 0x10
    std::uint8_t gap_12[0x2];
    void* bone_map;  // 0x14
    void* mesh_list_18;  // 0x18
    void* mesh_list_1c;  // 0x1c
    std::uint32_t lods_created;  // 0x20
    std::uint8_t lod_limit[5];  // 0x24
    std::uint8_t lod_merged[5];  // 0x29
    std::uint8_t lod_count[5];  // 0x2e
    std::uint8_t merged_mask;  // 0x33
    void* matrices;  // 0x34
    Actor* instances[80];  // 0x38
    Plane* batch[5];  // 0x178
    Bone* batch_planes[80];  // 0x18c
    InstanceList* instance_list;  // 0x2cc
    ActorDef* prev;  // 0x2d0
    ActorDef* next;  // 0x2d4
