// Generated (re/layout/gen_types.py) from re/layout/layouts.json: the data members of WavHeader
// (r3_D.json, confidence medium). Do not edit; change the spec.
    char riff_id[4];  // 0x0
    std::uint32_t riff_size;  // 0x4
    char wave_id[4];  // 0x8
    char fmt_id[4];  // 0xc
    std::uint32_t fmt_size;  // 0x10
    std::uint16_t format_tag;  // 0x14
    std::uint16_t channels;  // 0x16
    std::uint32_t sample_rate;  // 0x18
    std::uint32_t byte_rate;  // 0x1c
    std::uint16_t block_align;  // 0x20
    std::uint16_t bits_per_sample;  // 0x22
    char data_id[4];  // 0x24
    std::uint32_t data_size;  // 0x28
