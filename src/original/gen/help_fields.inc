// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of Help
// (g11.json, confidence medium). Do not edit; change the spec.
    std::uint8_t loaded;  // 0x0
    char title[81];  // 0x1
    char text[201];  // 0x52
    char label[9];  // 0x11b
    std::int16_t box[8];  // 0x124
    std::int16_t box_cur[8];  // 0x134
    std::uint32_t box_time;  // 0x144
    std::int16_t entry_count;  // 0x148
    std::int16_t name_first;  // 0x14a
    std::int16_t name_end;  // 0x14c
    std::int16_t num_first;  // 0x14e
    std::int16_t num_end;  // 0x150
    HelpEntry* entries;  // 0x152
    char* text_buf;  // 0x156
    std::int32_t text_buf_size;  // 0x15a
    std::int16_t tooltip_style;  // 0x15e
