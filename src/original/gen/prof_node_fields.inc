// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of ProfNode
// (r3_C.json, confidence medium). Do not edit; change the spec.
    std::uint32_t last[2];  // 0x0
    std::uint32_t total[2];  // 0x8
    std::uint32_t start_[2];  // 0x10
    std::int32_t nesting;  // 0x18
    std::uint32_t average[2];  // 0x1c
    ProfNode* parent;  // 0x24
    ProfNode* sibling_prev;  // 0x28
    ProfNode* sibling_next;  // 0x2c
    ProfNode* children_tail;  // 0x30
    ProfNode* children_head;  // 0x34
    ProfFunc* func;  // 0x38
    ProfNode* func_prev;  // 0x3c
    ProfNode* func_next;  // 0x40
    ProfEdge* edge;  // 0x44
    ProfNode* edge_prev;  // 0x48
    ProfNode* edge_next;  // 0x4c
