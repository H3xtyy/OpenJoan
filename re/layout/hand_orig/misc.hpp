// Original layout of Misc (d:\ww\ilib\misc.cpp), the engine's global helper
// object `misc` at 0x00a45fa8: string helpers, number formatting, the
// deterministic random number generator, small geometry and file helpers.
//
// The floating-point helpers (round 0x00590a20, random floats 0x00590630 /
// 660 / 690 / 6f0, round-to-decimals 0x00590740) are in src/ilib/misc_fp.cpp,
// which emulates the x87 rounding of the precision control Direct3D sets.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

namespace joan::orig {

#pragma pack(push, 1)

struct Misc {
    char str_buf[0x79];        // 0x00  scratch for atoi_n / atof_n / rtrim_field (<= 120 chars)
    char flag_79;              // 0x79
    std::int32_t random_seed;  // 0x7a  linear congruential generator state

    // --- reimplemented members (src/ilib/misc.cpp unless noted) ---
    std::int32_t str_cut(char* dst, const char* src, std::int32_t start, std::int32_t len);  // 0x0058f050
    std::int32_t str_chr(const char* s, char ch, std::int32_t start, std::int32_t end);      // 0x0058f0f0
    std::int32_t rtrim_len(const char* s, std::int32_t start, std::int32_t len);              // 0x0058f390
    char* rtrim(char* s);                                                                     // 0x0058f610
    char* ltrim(char* s);                                                                     // 0x0058f680
    std::int32_t str_cmp(const char* a, const char* b);                                       // 0x0058f9f0
    char* format(std::int32_t value, std::int32_t type);                                     // 0x0058fca0
    char* format(double value, std::int32_t type);                                           // 0x0058fe20
    char* num_to_str(std::int32_t value);                                                     // 0x00590130
    char* rtrim_field(const char* src, std::int32_t len);                                     // 0x00590170
    void copy_field(char* dst, const char* src, std::int32_t len);                            // 0x00590210
    std::int32_t atoi_field(const char* src, std::uint32_t len);                              // 0x00590270
    double atof_field(const char* src, std::uint32_t len);                                    // 0x005902d0 (ST0)
    std::int32_t sqrt_int(std::int32_t n);                                                    // 0x00590340
    std::int32_t diagonal_distance(std::int32_t dx, std::int32_t dy);                         // 0x005903d0
    std::int32_t points_distance(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2);  // 0x00590410
    void randomize();                                                                         // 0x005904e0
    void set_random_seed(std::int32_t seed);                                                  // 0x00590510
    std::int32_t random(std::int32_t max);                                                    // 0x00590540
    std::int32_t rand();                                                                      // 0x005905a0
    std::int32_t round(double x);  // 0x00590a20 half away from zero (misc_fp.cpp, x87 rounding emulated)
    std::int32_t limit(std::int32_t x, std::int32_t lo, std::int32_t hi);                     // 0x00590aa0
    double limit(double x, double lo, double hi);                                             // 0x00590b00 (ST0)
    std::int32_t is_file_exist(const char* path);                                             // 0x00590b80
    std::int32_t is_dir_exist(const char* path);                                              // 0x00590c00
    void extract_file_name(char* dst, const char* path);                                      // 0x00590d90 (gameset.cpp)
    std::int32_t compare_file_time(const char* a, const char* b);                             // 0x00590e20
    std::uint32_t get_time();                                                                 // 0x00591050
    void del_array_rec(char* arr, std::int32_t count, std::int32_t size, std::int32_t pos);   // 0x00591070
    char* insert_array_rec(char* arr, std::int32_t count, std::int32_t size, std::int32_t pos, const void* rec);  // 0x005910f0
    std::int32_t bin_search(const void* key, const char* arr, std::int32_t count, std::int32_t size,
                            int(__cdecl* cmp)(const void*, const void*), std::int32_t* insert_pos);  // 0x005911a0
    void fill_word(std::uint16_t* dst, std::int32_t value, std::int32_t count);               // 0x00591960
    std::int32_t area_distance(std::int32_t ax1, std::int32_t ay1, std::int32_t ax2, std::int32_t ay2,
                               std::int32_t bx1, std::int32_t by1, std::int32_t bx2, std::int32_t by2);  // 0x00591e70

    // --- src/ilib/misc_more.cpp ---
    // Offsets of the num-th position on a square spiral around a point (num 1 = centre),
    // within a (min(width, height) + 1)^2 area; the first 900 are cached in byte tables
    // at 0x00a64a3b (x) / 0x00a64dbf (y), filled once (count 0x00a65144 = 900).
    void move_around_a_point(std::int16_t num, std::int16_t width, std::int16_t height, std::int32_t* x,
                             std::int32_t* y);  // 0x00591480
    void move_around_a_point_cached(std::int16_t num, std::int16_t width, std::int16_t height, std::int32_t* x,
                                    std::int32_t* y);  // 0x005913b0
    void init_move_around_table();                     // 0x00591710
    Misc* construct();                                 // 0x00591cb0 Misc()
    // 0x005919a0 (varargs member, cdecl with `this` first): memset(dst, fill, size)
    // except `count` (pointer, length) regions given as the variadic arguments.

    // The other Misc functions the game code calls (typed, through their slots).
#include "gen/misc_methods.inc"
};

#pragma pack(pop)

static_assert(offsetof(Misc, random_seed) == 0x7a);

}  // namespace joan::orig
