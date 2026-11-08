// Reimplementation of Misc (d:\ww\ilib\misc.cpp): 31 integer, string, file
// and random-number helpers of the global `misc` object (0x00a45fa8).
// Decompiled 1:1 including quirks. CRT routines whose exact behaviour matters
// (_itoa, _fcvt, atoi, atof, time) are the game's own copies.
#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../platform/win32_types.hpp"

#include "../platform/files.hpp"

#include <cstring>
#include <emmintrin.h>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/misc.hpp"
#include "../registry.hpp"
#include <cstdint>
#include "../x87.hpp"
#include <cstdarg>
#include <vector>
#include "../original/gameset.hpp"
#include "../original/dynarray.hpp"
#include "dinput_formats.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
using joan::hook::original;

JOAN_COUNTER(c_str_cut, "Misc::str_cut");
JOAN_COUNTER(c_str_chr, "Misc::str_chr");
JOAN_COUNTER(c_rtrim_len, "Misc::rtrim_len");
JOAN_COUNTER(c_rtrim, "Misc::rtrim");
JOAN_COUNTER(c_ltrim, "Misc::ltrim");
JOAN_COUNTER(c_str_cmp, "Misc::str_cmp");
JOAN_COUNTER(c_format_int, "Misc::format(int)");
JOAN_COUNTER(c_format_dbl, "Misc::format(double)");
JOAN_COUNTER(c_num_to_str, "Misc::num_to_str");
JOAN_COUNTER(c_rtrim_field, "Misc::rtrim_field");
JOAN_COUNTER(c_copy_field, "Misc::copy_field");
JOAN_COUNTER(c_atoi_field, "Misc::atoi_field");
JOAN_COUNTER(c_atof_field, "Misc::atof_field");
JOAN_COUNTER(c_sqrt, "Misc::sqrt_int");
JOAN_COUNTER(c_diag, "Misc::diagonal_distance");
JOAN_COUNTER(c_points, "Misc::points_distance");
JOAN_COUNTER(c_randomize, "Misc::randomize");
JOAN_COUNTER(c_set_seed, "Misc::set_random_seed");
JOAN_COUNTER(c_random, "Misc::random");
JOAN_COUNTER(c_rand, "Misc::rand");
JOAN_COUNTER(c_limit, "Misc::limit(int)");
JOAN_COUNTER(c_limit_d, "Misc::limit(double)");
JOAN_COUNTER(c_file_exist, "Misc::is_file_exist");
JOAN_COUNTER(c_dir_exist, "Misc::is_dir_exist");
JOAN_COUNTER(c_file_time, "Misc::compare_file_time");
JOAN_COUNTER(c_get_time, "Misc::get_time");
JOAN_COUNTER(c_del_rec, "Misc::del_array_rec");
JOAN_COUNTER(c_ins_rec, "Misc::insert_array_rec");
JOAN_COUNTER(c_bin_search, "Misc::bin_search");
JOAN_COUNTER(c_fill_word, "Misc::fill_word");
JOAN_COUNTER(c_area_dist, "Misc::area_distance");

// Static result buffers of the original.
char* format_int_buf() { return &joan::hook::global<char>(d::g_a649f0); }
char* format_dbl_buf() { return &joan::hook::global<char>(d::g_a64a14); }
char* itoa_buf() { return &joan::hook::global<char>(d::g_a64660); }
char* num_str_buf() { return &joan::hook::global<char>(d::g_a649d4); }

// The game's CRT.
char* crt_itoa(std::int32_t v, char* buf, int radix) {
    return original<char*(JOAN_CDECL*)(std::int32_t, char*, int)>(fn::crt_ltoa)(v, buf, radix);
}
char* crt_fcvt(double v, int digits, int* dec, int* sign) {
    return original<char*(JOAN_CDECL*)(double, int, int*, int*)>(fn::crt_fcvt)(v, digits, dec, sign);
}
int crt_atoi(const char* s) { return original<int(JOAN_CDECL*)(const char*)>(fn::crt_atoi)(s); }
double crt_atof(const char* s) { return original<double(JOAN_CDECL*)(const char*)>(fn::crt_atof)(s); }
std::int32_t crt_time(void* t) { return original<std::int32_t(JOAN_CDECL*)(void*)>(fn::crt_time)(t); }

constexpr std::uint32_t kFieldMax = 0x78;
}  // namespace

namespace joan::orig {

// 0x0058f050 - copies up to `len` chars (len < 0: all) from 1-based position `start`.
std::int32_t Misc::str_cut(char* dst, const char* src, std::int32_t start, std::int32_t len) {
    c_str_cut.hit();
    const bool unlimited = len < 0;
    std::int32_t s = start - 1, d = 0;
    for (; (d < len || unlimited) && src[s]; ++s, ++d) dst[d] = src[s];
    dst[d] = 0;
    return 1;
}

// 0x0058f0f0 - 1-based position of `ch` between `start` and `end` (end -1 = to the end), 0 if absent.
std::int32_t Misc::str_chr(const char* s, char ch, std::int32_t start, std::int32_t end) {
    c_str_chr.hit();
    --end;
    for (std::int32_t i = start - 1; s[i] && (i <= end || end == -2); ++i)
        if (s[i] == ch) return i + 1;
    return 0;
}

// 0x0058f390 - length of s[start-1 .. start-1+len) without trailing spaces (len -1 = strlen).
std::int32_t Misc::rtrim_len(const char* s, std::int32_t start, std::int32_t len) {
    c_rtrim_len.hit();
    if (len == -1) len = static_cast<std::int32_t>(std::strlen(s));
    std::int32_t spaces = 0;
    for (std::int32_t i = start + len - 2; spaces < len && s[i] == ' '; --i) ++spaces;
    return len - spaces;
}

// 0x0058f610 - strips trailing spaces in place.
// Quirk kept: an all-space string makes the loop read s[-1] before stopping.
char* Misc::rtrim(char* s) {
    c_rtrim.hit();
    auto i = static_cast<std::int32_t>(std::strlen(s)) - 1;
    while (s[i] == ' ' && i >= 0) --i;
    s[i + 1] = 0;
    return s;
}

// 0x0058f680 - strips leading spaces in place.
char* Misc::ltrim(char* s) {
    c_ltrim.hit();
    std::int32_t i = 0;
    while (s[i] == ' ' && s[i]) ++i;
    std::int32_t d = 0;
    for (; s[i]; ++i, ++d) s[d] = s[i];
    s[d] = 0;
    return s;
}

// 0x0058f9f0 - 1 if `b` equals `a`, or `b` ends where `a` continues with a space.
std::int32_t Misc::str_cmp(const char* a, const char* b) {
    c_str_cmp.hit();
    std::int32_t i = 0;
    for (; a[i] && b[i]; ++i)
        if (a[i] != b[i]) return 0;
    return b[i] == 0 && (a[i] == 0 || a[i] == ' ') ? 1 : 0;
}

// 0x0058fca0 - integer with thousands separators. type 2: "$" prefix, 3: "%" suffix,
// 4: no separators; negative numbers in parentheses. Quirk kept: INT_MIN keeps its "-".
char* Misc::format(std::int32_t value, std::int32_t type) {
    c_format_int.hit();
    char* out = format_int_buf();
    const bool negative = value < 0;
    if (negative) value = static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(value));
    const char* digits = crt_itoa(value, itoa_buf(), 10);
    const auto len = static_cast<std::int32_t>(std::strlen(digits));
    if (negative) *out++ = '(';
    if (type == 2) *out++ = '$';
    for (std::int32_t i = len; i > 0; --i) {
        if (type != 4 && i % 3 == 0 && i < len) *out++ = ',';
        *out++ = *digits++;
    }
    if (type == 3) *out++ = '%';
    if (negative) *out++ = ')';
    *out = 0;
    return format_int_buf();
}

// 0x0058fe20 - like format(int) with two decimals when -1000 < value < 1000 (via _fcvt).
char* Misc::format(double value, std::int32_t type) {
    c_format_dbl.hit();
    char* out = format_dbl_buf();
    int dec = 0, sign = 0;
    const char* digits = crt_fcvt(value, 2, &dec, &sign);
    // x87 compares: "less" is also true for NaN (unordered), as in the original.
    const bool less_than_zero = !(value >= 0.0);
    if (less_than_zero) *out++ = '(';
    if (type == 2) *out++ = '$';
    for (int i = dec; i > 0; --i) {
        if (i % 3 == 0 && i < dec) *out++ = ',';
        *out++ = *digits++;
    }
    if (dec <= 0) *out++ = '0';
    if (value > -1000.0 && !(value >= 1000.0)) {
        *out++ = '.';
        if (*digits && dec >= 0) *out++ = *digits++;
        else *out++ = '0';
        if (*digits) *out++ = *digits++;
        else *out++ = '0';
    }
    if (type == 3) *out++ = '%';
    if (less_than_zero) *out++ = ')';
    *out = 0;
    return format_dbl_buf();
}

// 0x00590130
char* Misc::num_to_str(std::int32_t value) {
    c_num_to_str.hit();
    return crt_itoa(value, num_str_buf(), 10);
}

// 0x00590170 - first `len` (<= 120) chars of a space-padded field, without trailing spaces.
char* Misc::rtrim_field(const char* src, std::int32_t len) {
    c_rtrim_field.hit();
    if (len > static_cast<std::int32_t>(kFieldMax)) len = kFieldMax;
    std::memcpy(str_buf, src, static_cast<std::size_t>(len));
    std::int32_t i = len - 1;
    for (; i >= 0; --i) {
        if (str_buf[i] != ' ') {
            str_buf[i + 1] = 0;
            break;
        }
    }
    if (i < 0) str_buf[0] = 0;
    return str_buf;
}

// 0x00590210 - copies a space-padded field without its trailing spaces.
void Misc::copy_field(char* dst, const char* src, std::int32_t len) {
    c_copy_field.hit();
    const std::int32_t n = rtrim_len(src, 1, len);
    std::memcpy(dst, src, static_cast<std::size_t>(n));
    dst[n] = 0;
}

// 0x00590270 - atoi of the first `len` (<= 120, unsigned compare) chars.
std::int32_t Misc::atoi_field(const char* src, std::uint32_t len) {
    c_atoi_field.hit();
    if (len >= kFieldMax + 1) len = kFieldMax;
    std::memcpy(str_buf, src, len);
    str_buf[len] = 0;
    return crt_atoi(str_buf);
}

// 0x005902d0 - atof of the first `len` chars; the double is returned in ST0 unrounded.
double Misc::atof_field(const char* src, std::uint32_t len) {
    c_atof_field.hit();
    if (len >= kFieldMax + 1) len = kFieldMax;
    std::memcpy(str_buf, src, len);
    str_buf[len] = 0;
    return crt_atof(str_buf);
}

// 0x00590340 - integer square root (floor), hand-written assembly in the original.
std::int32_t Misc::sqrt_int(std::int32_t n) {
    c_sqrt.hit();
    if (n < 16) {
        std::int32_t root = 1, sum = 1;  // sum of odd numbers
        while (sum <= n) {
            sum += root;
            ++root;
            sum += root;
        }
        return root - 1;
    }
    // The original's bit-by-bit loop gives floor(sqrt(n)); for 16 <= n < 2^31 so does sqrtsd truncated (the
    // double root is within an ulp, far from the next integer): equal for every such n (checked exhaustively).
    return _mm_cvttsd_si32(_mm_sqrt_sd(_mm_setzero_pd(), _mm_set_sd(static_cast<double>(n))));
}

// 0x005903d0
std::int32_t Misc::diagonal_distance(std::int32_t dx, std::int32_t dy) {
    c_diag.hit();
    const auto ux = static_cast<std::uint32_t>(dx), uy = static_cast<std::uint32_t>(dy);
    return sqrt_int(static_cast<std::int32_t>(ux * ux + uy * uy));  // wraps like the original imul
}

// 0x00590410
std::int32_t Misc::points_distance(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2) {
    c_points.hit();
    return diagonal_distance(x2 - x1, y2 - y1);
}

// 0x005904e0
void Misc::randomize() {
    c_randomize.hit();
    set_random_seed(crt_time(nullptr));
}

// 0x00590510
void Misc::set_random_seed(std::int32_t seed) {
    c_set_seed.hit();
    random_seed = seed;
}

// 0x00590540 - 0 .. max-1. Kept exactly: signed 32-bit LCG, 15-bit output, unsigned scaling.
std::int32_t Misc::random(std::int32_t max) {
    c_random.hit();
    random_seed = static_cast<std::int32_t>(static_cast<std::uint32_t>(random_seed) * 0x015a4e35u + 1u);
    const std::int32_t r = (random_seed >> 16) & 0x7fff;
    return static_cast<std::int32_t>((static_cast<std::uint32_t>(max) * static_cast<std::uint32_t>(r)) >> 15);
}

// 0x005905a0 - 0 .. 32767
std::int32_t Misc::rand() {
    c_rand.hit();
    random_seed = static_cast<std::int32_t>(static_cast<std::uint32_t>(random_seed) * 0x015a4e35u + 1u);
    return (random_seed >> 16) & 0x7fff;
}

// 0x00590aa0
std::int32_t Misc::limit(std::int32_t x, std::int32_t lo, std::int32_t hi) {
    c_limit.hit();
    if (x < lo) return lo;
    return x > hi ? hi : x;
}

// 0x00590b00 - returns one of the arguments unchanged (no arithmetic).
double Misc::limit(double x, double lo, double hi) {
    c_limit_d.hit();
    if (!(x >= lo)) return lo;  // x87 "less" includes unordered (NaN)
    return x > hi ? hi : x;
}

// 0x00590b80
std::int32_t Misc::is_file_exist(const char* path) {
    c_file_exist.hit();
    WIN32_FIND_DATAA fd;
    const HANDLE h = joan::platform::files::FindFirstFileA(path, &fd);
    const std::int32_t found = h != INVALID_HANDLE_VALUE;
    if (h != INVALID_HANDLE_VALUE) joan::platform::files::FindClose(h);
    return found;
}

// 0x00590c00 - a trailing backslash is removed before the lookup.
std::int32_t Misc::is_dir_exist(const char* path) {
    c_dir_exist.hit();
    if (!path || !*path) return 0;
    char buf[0x104];
    std::strcpy(buf, path);  // as unbounded as the original
    if (buf[std::strlen(buf) - 1] == '\\') buf[std::strlen(buf) - 1] = 0;
    WIN32_FIND_DATAA fd;
    const HANDLE h = joan::platform::files::FindFirstFileA(buf, &fd);
    const std::int32_t dir = h != INVALID_HANDLE_VALUE && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
    if (h != INVALID_HANDLE_VALUE) joan::platform::files::FindClose(h);
    return dir;
}

// 0x00590e20 - CompareFileTime of the last-write times; 2 / -2 if only the first / second exists.
std::int32_t Misc::compare_file_time(const char* a, const char* b) {
    c_file_time.hit();
    WIN32_FIND_DATAA fa, fb;
    const HANDLE ha = joan::platform::files::FindFirstFileA(a, &fa);
    if (ha != INVALID_HANDLE_VALUE) joan::platform::files::FindClose(ha);
    const HANDLE hb = joan::platform::files::FindFirstFileA(b, &fb);
    if (hb != INVALID_HANDLE_VALUE) joan::platform::files::FindClose(hb);
    const bool ea = ha != INVALID_HANDLE_VALUE, eb = hb != INVALID_HANDLE_VALUE;
    if (ea && eb) return joan::platform::files::CompareFileTime(&fa.ftLastWriteTime, &fb.ftLastWriteTime);
    if (ea) return 2;
    if (eb) return -2;
    return 0;
}

// 0x00591050
std::uint32_t Misc::get_time() {
    c_get_time.hit();
    // timeGetTime through the game's own import (no winmm link dependency).
    return (*reinterpret_cast<DWORD(JOAN_STDCALL**)()>(img::addr(d::iat_WINMM_timeGetTime)))();
}

// 0x00591070 - removes record `pos` (1-based) of a plain array.
void Misc::del_array_rec(char* arr, std::int32_t count, std::int32_t size, std::int32_t pos) {
    c_del_rec.hit();
    const std::int32_t i = pos - 1;
    std::memmove(arr + size * i, arr + size * (i + 1), static_cast<std::size_t>(size * (count - i - 1)));
}

// 0x005910f0 - inserts at `pos` (1-based); rec == nullptr inserts a zeroed record.
char* Misc::insert_array_rec(char* arr, std::int32_t count, std::int32_t size, std::int32_t pos, const void* rec) {
    c_ins_rec.hit();
    char* const p = arr + (pos - 1) * size;
    if (pos <= count) std::memmove(p + size, p, static_cast<std::size_t>((count - pos + 1) * size));
    if (rec) std::memcpy(p, rec, static_cast<std::size_t>(size));
    else std::memset(p, 0, static_cast<std::size_t>(size));
    return p;
}

// 0x005911a0 - binary search in a sorted array; returns the 1-based record or 0.
// With insert_pos it also reports where the key would be inserted.
std::int32_t Misc::bin_search(const void* key, const char* arr, std::int32_t count, std::int32_t size,
                              int(JOAN_CDECL* cmp)(const void*, const void*), std::int32_t* insert_pos) {
    c_bin_search.hit();
    auto rec = [&](std::int32_t n) { return arr + (n - 1) * size; };
    if (count <= 0 || cmp(key, arr) < 0) {
        if (insert_pos) *insert_pos = 1;
        return 0;
    }
    if (cmp(key, rec(count)) > 0) {
        if (insert_pos) *insert_pos = count + 1;
        return 0;
    }
    std::int32_t lo = 1, hi = count;
    if (insert_pos) {
        while (hi - lo > 1) {
            const std::int32_t mid = (hi + lo) / 2;
            if (cmp(key, rec(mid)) < 0) hi = mid;
            else lo = mid;
        }
        *insert_pos = cmp(key, rec(hi)) == 0 ? hi + 1 : lo + 1;
    }
    lo = 1;
    hi = count;
    while (hi - lo > 1) {
        const std::int32_t mid = (hi + lo) / 2;
        if (cmp(key, rec(mid)) > 0) lo = mid;
        else hi = mid;
    }
    if (cmp(key, rec(lo)) == 0) return lo;
    if (cmp(key, rec(hi)) == 0) return hi;
    return 0;
}

// 0x00591960 - fills `count` 16-bit words (rep stosd + stosw in the original).
void Misc::fill_word(std::uint16_t* dst, std::int32_t value, std::int32_t count) {
    c_fill_word.hit();
    const auto v = static_cast<std::uint16_t>(value);
    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(count); ++i) dst[i] = v;
}

// 0x00591e70 - gap between two rectangles: the larger of the x and y gaps (0 if overlapping).
std::int32_t Misc::area_distance(std::int32_t ax1, std::int32_t ay1, std::int32_t ax2, std::int32_t ay2,
                                 std::int32_t bx1, std::int32_t by1, std::int32_t bx2, std::int32_t by2) {
    c_area_dist.hit();
    const std::int32_t gy = by2 < ay1 ? ay1 - by2 : (by1 > ay2 ? by1 - ay2 : 0);
    const std::int32_t gx = bx2 < ax1 ? ax1 - bx2 : (bx1 > ax2 ? bx1 - ax2 : 0);
    return gx > gy ? gx : gy;
}

}  // namespace joan::orig

namespace {
using joan::orig::Misc;
}  // namespace

#define MISC(name, addr, fn) JOAN_REPLACE("ilib/misc.cpp", "Misc::" name, addr, fn)
MISC("str_cut", 0x0058f050, &Misc::str_cut);
MISC("str_chr", 0x0058f0f0, &Misc::str_chr);
MISC("rtrim_len", 0x0058f390, &Misc::rtrim_len);
MISC("rtrim", 0x0058f610, &Misc::rtrim);
MISC("ltrim", 0x0058f680, &Misc::ltrim);
MISC("str_cmp", 0x0058f9f0, &Misc::str_cmp);
MISC("format_58fca0", 0x0058fca0, static_cast<char* (Misc::*)(std::int32_t, std::int32_t)>(&Misc::format));
MISC("format", 0x0058fe20, static_cast<char* (Misc::*)(double, std::int32_t)>(&Misc::format));
MISC("num_to_str", 0x00590130, &Misc::num_to_str);
MISC("rtrim_field", 0x00590170, &Misc::rtrim_field);
MISC("copy_field", 0x00590210, &Misc::copy_field);
MISC("atoi_field", 0x00590270, &Misc::atoi_field);
MISC("atof_field", 0x005902d0, &Misc::atof_field);
MISC("sqrt_int", 0x00590340, &Misc::sqrt_int);
MISC("diagonal_distance", 0x005903d0, &Misc::diagonal_distance);
MISC("points_distance", 0x00590410, &Misc::points_distance);
MISC("randomize", 0x005904e0, &Misc::randomize);
MISC("set_random_seed", 0x00590510, &Misc::set_random_seed);
MISC("random", 0x00590540, &Misc::random);
MISC("rand", 0x005905a0, &Misc::rand);
MISC("limit(int)", 0x00590aa0, static_cast<std::int32_t (Misc::*)(std::int32_t, std::int32_t, std::int32_t)>(&Misc::limit));
MISC("bound", 0x00590b00, static_cast<double (Misc::*)(double, double, double)>(&Misc::limit));
MISC("is_file_exist", 0x00590b80, &Misc::is_file_exist);
MISC("is_dir_exist", 0x00590c00, &Misc::is_dir_exist);
MISC("extract_file_name", 0x00590d90, &Misc::extract_file_name);
MISC("compare_file_time", 0x00590e20, &Misc::compare_file_time);
MISC("get_time", 0x00591050, &Misc::get_time);
MISC("del_array_rec", 0x00591070, &Misc::del_array_rec);
MISC("insert_array_rec", 0x005910f0, &Misc::insert_array_rec);
MISC("bsearch_array_rec", 0x005911a0, &Misc::bin_search);
MISC("memsetw", 0x00591960, &Misc::fill_word);
MISC("area_distance", 0x00591e70, &Misc::area_distance);
#undef MISC

// ---------------------------------------- from ilib/misc_fp.cpp (ilib/misc.cpp)
namespace {
namespace ilib_misc_fp {

#define MISC(name, addr, fn) JOAN_REPLACE("ilib/misc.cpp", "Misc::" name, addr, fn)
MISC("round", 0x00590a20, &joan::orig::Misc::round);
#undef MISC

}  // namespace ilib_misc_fp
}  // namespace

// ---------------------------------------- from ilib/misc_more.cpp (ilib/misc.cpp)
namespace {
namespace ilib_misc_more {

using joan::orig::Misc;

JOAN_COUNTER(c_fill_except, "Misc::fill_except");

struct Region {
    char* ptr;
    std::uint32_t len;
};

void fill_except_impl(char* dst, int fill, std::uint32_t size, std::int32_t count, const Region* regions) {
    // Regions in ascending order: fill the gaps in one pass.
    char* p = dst;
    bool sorted = true;
    for (std::int32_t i = 0; i < count; ++i) {
        if (regions[i].ptr < p) sorted = false;
        if (sorted) p = regions[i].ptr + regions[i].len;
    }
    if (sorted) {
        p = dst;
        for (std::int32_t i = 0; i < count; ++i) {
            std::memset(p, fill, static_cast<std::size_t>(regions[i].ptr - p));
            p = regions[i].ptr + regions[i].len;
        }
        std::memset(p, fill, size - static_cast<std::uint32_t>(p - dst));
        return;
    }
    // Otherwise: repeatedly find the region nearest ahead of (or covering) p.
    p = dst;
    char* const end = dst + size;
    for (;;) {
        char* best = nullptr;
        char* best_end = nullptr;
        for (std::int32_t i = 0; i < count; ++i) {
            char* const a = regions[i].ptr;
            if (regions[i].len == 0) continue;
            char* const e = a + regions[i].len;
            if (!((p <= a && a < end) || (a <= p && p < e))) continue;
            if (best && (a > best || (a == best && e <= best_end))) continue;
            best = a;
            best_end = e;
        }
        if (!best) {
            std::memset(p, fill, static_cast<std::size_t>(end - p));
            return;
        }
        if (p < best) std::memset(p, fill, static_cast<std::size_t>(best - p));
        if (best_end >= end) return;
        p = best_end;
    }
}

void JOAN_CDECL fill_except_entry(Misc* /*self*/, char* dst, int fill, std::uint32_t size, std::int32_t count, ...) {
    c_fill_except.hit();
    std::vector<Region> regions(count > 0 ? static_cast<std::size_t>(count) : 0);
    va_list ap;
    va_start(ap, count);
    for (auto& r : regions) {
        r.ptr = va_arg(ap, char*);
        r.len = va_arg(ap, std::uint32_t);
    }
    va_end(ap);
    fill_except_impl(dst, fill, size, count, regions.data());
}

#define MISC(name, addr, fn) JOAN_REPLACE("ilib/misc.cpp", "Misc::" name, addr, fn)
MISC("Misc()", 0x00591cb0, &Misc::construct);
MISC("fill_except", 0x005919a0, &fill_except_entry);
#undef MISC

}  // namespace ilib_misc_more
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (game/b111_16)
namespace {
namespace game_b111_16 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::cdecl_call;

struct H {
    // -------------------------------------------------------------- 0x005908b0
    double round2(float v) {
        const Fpu fpu{true};
        const double x = fpu.mul(v, global<double>(d::dbl_100));  // * 100.0, fstp qword (exact)
        double ip = 0.0;
        const double frac = joan::orig::arg::as<double>(joan::orig::libc::modf(joan::orig::arg::f64(x), &ip));  // CRT modf
        if (frac > global<double>(d::dbl_0_5)) {                    // 0.5 (test ah,0x41 jne)
            return fpu.mul(fpu.add(ip, global<double>(d::dbl_1)), global<double>(d::dbl_0_01));
        }
        if (!(frac >= global<double>(d::dbl_9d8c18))) {  // -0.5: less or NaN
            return fpu.mul(fpu.sub(ip, global<double>(d::dbl_1)), global<double>(d::dbl_0_01));
        }
        return fpu.mul(ip, global<double>(d::dbl_0_01));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_16", name, addr, fn)
B("Misc::round2_5908b0", 0x005908b0, &H::round2);
#undef B

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- from modules/game_b111_13.cpp (game/b111_13)
namespace {
namespace game_b111_13 {

using joan::hook::global;

using joan::call::cdecl_call;

using i16 = std::int16_t;

const double* rdbl(img::Addr a) { return &img::global<const double>(a); }

JOAN_COUNTER(c_round_n, "Misc::round_n");

// x87 arithmetic at the game's precision control (PC24) with the x87 exponent range: src/fp/x87.hpp.
using joan::fp::pc24_mul;
using joan::fp::pc24_div;
using joan::fp::pc24_add;

// x87 operations of 0x00590960 (`fld qword; fmul / fadd / fdiv qword [c]` under PC24), stored with fstp qword.
double x87_fmul_q(double a, const double* c) { return pc24_mul(a, *c); }
double x87_fadd_q(double a, const double* c) { return pc24_add(a, *c); }
double x87_fdiv_q(double a, const double* c) { return pc24_div(a, *c); }

struct H {
    // ------------------------------------------------------------------------ 0x00590960
    double round_n(double x, i16 n) {
        c_round_n.hit();
        for (i16 i = 0; i < n; i = static_cast<i16>(i + 1)) x = x87_fmul_q(x, rdbl(d::dbl_9d8c28));  // * 10.0
        x = joan::orig::arg::as<double>(joan::orig::libc::floor(joan::orig::arg::f64(x87_fadd_q(x, rdbl(d::dbl_0_5)))));                     // floor(x + 0.5)
        for (i16 i = 0; i < n; i = static_cast<i16>(i + 1)) x = x87_fdiv_q(x, rdbl(d::dbl_9d8c28));  // / 10.0
        return x;
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_13", name, addr, fn)
B111("Misc::round_n", 0x00590960, &H::round_n);
#undef B111

}  // namespace game_b111_13
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

using i8 = std::int8_t;
using i32 = std::int32_t;

using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_replace_ext, "Path::replace_ext");

struct H {

    // ------------------------------------------------------------------ 0x00590d20 (dead code)
    std::uintptr_t replace_ext(char* dst, const char* src, const char* ext) {
        c_replace_ext.hit();
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->str_chr_58f0f0(joan::orig::arg::str(src), static_cast<char>(joan::orig::arg::i32(0x2e)), 1, joan::orig::arg::i32(-1)));  // 0x0058f0f0
        std::memcpy(static_cast<void*>(dst), static_cast<const void*>(src), joan::orig::arg::u32(n));         // memcpy
        return joan::orig::arg::as<std::uintptr_t>(std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst + n)), joan::orig::arg::str(ext)));  // strcpy
    }

    // ------------------------------------------------------------------ 0x005917d0 (dead code)
    u32* bits_set(u32* p, i32 i) {
        *p |= global<u32>(d::g_a15850 + i * 4);
        return p;
    }

    // ------------------------------------------------------------------ 0x00591d20 / 0x00591d50
    i32 has_79() { return jo::view<i8>(jo::self<jo::Misc>(this)->flag_79) > 0 ? 1 : 0; }
    u32 get_a158cc(i32 a, i32 b) {
        return global<u32>(d::g_a158cc + (static_cast<i32>(static_cast<u32>(a + 1) * 0xcu) + b * 4));
    }

    // ------------------------------------------------------------------ 0x00591f20 / 0x00591f70
    u32 start_random() {
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->random(100)) + 1u;  // 0x00590540
        jo::view<u32>(jo::self<jo::Misc>(this)->random_limit) = v;
        jo::view<u32>(jo::self<jo::Misc>(this)->random_count) = 0;
        return v;
    }
    i32 advance(i32 d) {
        jo::self<jo::Misc>(this)->random_count = static_cast<i32>(jo::view<u32>(jo::self<jo::Misc>(this)->random_count) + static_cast<u32>(d));
        return jo::self<jo::Misc>(this)->random_limit < jo::self<jo::Misc>(this)->random_count ? 1 : 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Misc::change_file_ext", 0x00590d20, &H::replace_ext);
B("Bits::set", 0x005917d0, &H::bits_set);
B("Obj::has_79", 0x00591d20, &H::has_79);
B("Table::get_a158cc", 0x00591d50, &H::get_a158cc);
B("Timer::start_random", 0x00591f20, &H::start_random);
B("Timer::advance", 0x00591f70, &H::advance);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b102_08.cpp (game/b102_08)
namespace {
namespace game_b102_08 {

using joan::hook::global;

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;
using joan::img::str;

using joan::call::thiscall;

void crt_atexit(const void* fn) { joan::crt::atexit(fn); }  // the code address (crt.hpp narrows it for the onexit table)

JOAN_COUNTER(c_ordinal, "Misc::ordinal");

struct H {
    // ------------------------------------------------------------------ 0x00590f00
    char* ordinal(i32 v) {
        c_ordinal.hit();
        void* const s = obj(d::g_a646a8);
        u8& guard = global<u8>(d::g_a64a38);
        if ((guard & 1) == 0) {
            guard = static_cast<u8>(guard | 1);
            joan::orig::as_string(s)->construct();  // 0x005a1170 ctor
            crt_atexit(fn::code_ptr(fn::StaticInit_atexit_empty_591030));
        }
        const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_misc(this)->format_int_58fca0(joan::orig::arg::i32(v), 1));  // 0x0058fca0 Misc::format(int)
        joan::orig::as_string(s)->assign(joan::orig::arg::str(text));                       // 0x005a1440 assign
        img::Addr suffix = d::s_th;                                     // "th"
        if (!(v >= 0xb && v <= 0xd)) {
            switch (v % 10) {
                case 1: suffix = d::s_st; break;
                case 2: suffix = d::s_nd; break;
                case 3: suffix = d::s_rd; break;
                default: break;
            }
        }
        joan::orig::as_string(s)->append(joan::orig::arg::str(str(suffix)));  // 0x005a1550 append
        return joan::orig::as_string(s)->c_str();  // 0x00417d90
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_08", name, addr, fn)
B("Misc::ordinal", 0x00590f00, &H::ordinal);
#undef B

}  // namespace game_b102_08
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

using u8 = std::uint8_t;


void JOAN_CDECL empty_cdecl() {}

struct H {
    // 0x00591ce0 / 0x00591d00
    void* set_79_on() {
        jo::view<u8>(jo::self<jo::Misc>(this)->flag_79) = 1;
        return this;
    }
    void* set_79_off() {
        jo::view<u8>(jo::self<jo::Misc>(this)->flag_79) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("StaticInit::atexit_empty_591030", 0x00591030, &empty_cdecl);
B("StaticInit::atexit_empty_591940", 0x00591940, &empty_cdecl);
B("Obj::set_79_on", 0x00591ce0, &H::set_79_on);
B("Obj::set_79_off", 0x00591d00, &H::set_79_off);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
namespace {
namespace game_b102_09 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::cdecl_call;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

JOAN_COUNTER(c_say_number, "Speech::say_number");
JOAN_COUNTER(c_directions, "Compass::directions");
JOAN_COUNTER(c_in_band, "Ratio::in_band");

struct H {
    // ------------------------------------------------------------------------ 0x00591810
    char* say_number(i32 n) {  // the static String's text (a pointer)
        c_say_number.hit();
        void* const s = obj(d::g_a64840);
        if ((global<u8>(d::g_a64a37) & 1u) == 0) {
            global<u8>(d::g_a64a37) = static_cast<u8>(global<u8>(d::g_a64a37) | 1u);
            joan::orig::as_string(s)->construct();         // 0x005a1170 ctor
            joan::crt::atexit(fn::code_ptr(fn::StaticInit_atexit_empty_591940));  // atexit
        }
        joan::orig::as_string(s)->assign(str(d::kEmpty));           // 0x005a1440
        if (n > 100) {
            joan::orig::as_string(s)->append(joan::orig::arg::str(global<const char*>(d::strs_a1589c + (n / 100) * 4)));  // 0x005a1550
            n = n - (n / 100) * 100;
        }
        if (n > 10) {
            joan::orig::as_string(s)->append(joan::orig::arg::str(global<const char*>(d::strs_a1589c + ((n - 1) / 10) * 4)));  // 0x005a1550
            n = n - ((n - 1) / 10) * 10;
        }
        joan::orig::as_string(s)->append(joan::orig::arg::str(global<const char*>(d::strs_a1589c + n * 4)));  // 0x005a1550
        return joan::orig::as_string(s)->c_str();    // 0x00417d90
    }

    // ------------------------------------------------------------------------ 0x00591d90
    std::uintptr_t directions(i32 deg, i32* x, i32* y) {
        c_directions.hit();
        while (deg < 0) deg += 0x168;
        const i32 q = deg / 0x168;
        deg = deg % 0x168;
        std::uintptr_t eax = static_cast<u32>(q);
        if (deg > 0x2c && deg < 0x88) {
            *x = 1;
        } else if (deg > 0xb9 && deg < 0x162) {
            eax = reinterpret_cast<std::uintptr_t>(x);
            *x = -1;
        } else {
            *x = 0;
        }
        if (deg > 0x5f && deg < 0x109) {
            *y = -1;
        } else if (deg < 0x55 || deg > 0x111) {
            eax = reinterpret_cast<std::uintptr_t>(y);
            *y = 1;
        } else {
            *y = 0;
        }
        return eax;
    }

    // ------------------------------------------------------------------------ 0x00591fc0
    u32 octant_delta(u32 a_, u32 b_) {
        const i32 a = static_cast<i8>(a_ & 0xffu);
        const i32 b = static_cast<i8>(b_ & 0xffu);
        if (a >= b) {
            const i32 d = static_cast<i8>(static_cast<u8>(a - b));
            if (d > 4) return static_cast<u32>(8 - d);
            return (static_cast<u32>(b) & 0xffffff00u) | static_cast<u8>(d);
        }
        const u32 diff = static_cast<u32>(b - a);
        const i32 d = static_cast<i8>(static_cast<u8>(diff));
        if (d > 4) return (diff & 0xffffff00u) | static_cast<u8>(8 - d);
        return diff;
    }

    // ------------------------------------------------------------------------ 0x00592050
    i32 in_band(float a, float b, float tol) {
        c_in_band.hit();
        const double fa = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(a))));
        const double fb = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(b))));
        if (!(fb >= fa)) {  // fb < fa or unordered
            const float t = b;
            b = a;
            a = t;
        }
        const Fpu fpu{true};
        const double one = static_cast<double>(global<float>(d::flt_1));  // 1.0f
        const double lo = fpu.mul(fpu.sub(one, tol), b);
        const double ad = a;
        // fcomp 0.0 / test ah,1 / jne: b < 0 or unordered takes the second branch.
        if (static_cast<double>(b) >= static_cast<double>(global<float>(d::k_9cf5c4))) {
            if (!(lo < ad || lo != lo || ad != ad)) return 0;
            const double hi = fpu.mul(fpu.add(one, tol), b);
            return hi > ad ? 1 : 0;
        }
        if (!(lo > ad)) return 0;
        const double hi = fpu.mul(fpu.add(one, tol), b);
        return (hi >= ad) ? 0 : 1;  // hi < a or unordered -> 1
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("Speech::say_number", 0x00591810, &H::say_number);
B("Compass::directions", 0x00591d90, &H::directions);
B("Compass::octant_delta", 0x00591fc0, &H::octant_delta);
B("Ratio::in_band", 0x00592050, &H::in_band);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

using i32 = std::int32_t;

using i64 = std::int64_t;

// 0x00592170
i32 JOAN_CDECL mul_div(i32 a, i32 b, i32 c) {
    return static_cast<i32>(static_cast<i64>(a) * static_cast<i64>(b) / static_cast<i64>(c));
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("mul_div", 0x00592170, &mul_div);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

using joan::call::cdecl_call;

using i32 = std::int32_t;

using u32 = std::uint32_t;

i32 neg(i32 v) { return static_cast<i32>(0u - static_cast<u32>(v)); }

// ---------------------------------------------------------------------------- 0x005921a0
static i32 JOAN_CDECL muldiv_signed(i32 a, i32 b, i32 c) {
    i32 sign = 1;
    i32 ua;
    i32 ub;
    i32 uc;
    if (a >= 0) {
        ua = a;
    } else {
        ua = neg(a);
        sign = neg(sign);
    }
    if (b >= 0) {
        ub = b;
    } else {
        ub = neg(b);
        sign = neg(sign);
    }
    if (c >= 0) {
        uc = c;
    } else {
        uc = neg(c);
        sign = neg(sign);
    }
    const i32 r = joan::orig::arg::as<i32>(joan::orig::mul_div_round(joan::orig::arg::u32(ua), joan::orig::arg::u32(ub), joan::orig::arg::u32(uc)));  // 0x00592290
    return sign > 0 ? r : neg(r);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("muldiv_signed", 0x005921a0, &muldiv_signed);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b111_12.cpp (game/b111_12)
namespace {
namespace game_b111_12 {

using u32 = std::uint32_t;

// ------------------------------------------------------------------------ 0x00592260
// `mul dword [b]; div dword [c]` (edx:eax / c). The original raises #DE when the quotient does not fit in 32
// bits (or c == 0, which faults here too); otherwise identical.
u32 JOAN_CDECL muldiv(u32 a, u32 b, u32 c) { return static_cast<u32>(static_cast<std::uint64_t>(a) * b / c); }

#define G(name, addr, fn) JOAN_REPLACE("game/b111_12", name, addr, fn)
G("muldiv_592260", 0x00592260, &muldiv);
#undef G

}  // namespace game_b111_12
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

// ---- free functions ----
// 0x00592290: mul / div through edx:eax. The original raises #DE when the quotient does not fit
// in 32 bits (c == 0 still faults here too); otherwise identical.
std::uint32_t JOAN_CDECL mul_div_round(std::uint32_t a, std::uint32_t b, std::uint32_t c) {
    const std::uint32_t half = (c - 1u) >> 1;
    const std::uint64_t p = static_cast<std::uint64_t>(a) * b;
    const std::uint32_t q = static_cast<std::uint32_t>(p / c);
    const std::uint32_t r = static_cast<std::uint32_t>(p % c);
    return q + (half < r ? 1u : 0u);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("mul_div_round", 0x00592290, &mul_div_round);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b103_10_2.cpp (game/b103_10)
namespace {
namespace game_b103_10_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;

using joan::hook::x87::Fpu;

JOAN_COUNTER(c_wait, "Timer::wait");

struct H {

    // ------------------------------------------------------------------ 0x0058ef30 (dead code)
    i32 wait(float seconds) {
        c_wait.hit();
        const i32 t0 = joan::orig::arg::as<i32>(joan::orig::libc::clock());  // clock
        const Fpu fpu{true};
        const double target = fpu.add(static_cast<double>(t0),
                                      fpu.mul(static_cast<double>(seconds), static_cast<double>(global<float>(d::flt_1000))));
        const i32 end = joan::hook::x87::ftol(target);
        i32 now;
        do {
            now = joan::orig::arg::as<i32>(joan::orig::libc::clock());
        } while (now < end);
        return now;
    }

    // ------------------------------------------------------------------ 0x0058f280 / 0x0058f2c0
    i32 upper(i32 c) {
        if (c >= 0x61 && c <= 0x7a) c -= 0x20;
        return c;
    }
    i32 lower(i32 c) {
        if (c >= 0x41 && c <= 0x5a) c += 0x20;
        return c;
    }

    // ------------------------------------------------------------------ 0x0058f7f0 (dead code)
    void* fill_spaces(char* p, u32 n) {
        void* const r = std::memset(static_cast<void*>(p), 0x20, joan::orig::arg::u32(n));  // memset
        p[n] = 0;
        return r;
    }

    // ------------------------------------------------------------------ 0x00590450 (dead code)
    i32 chebyshev(i32 x0, i32 y0, i32 x1, i32 y1) {
        const i32 dx = joan::orig::arg::as<i32>(joan::orig::libc::abs(static_cast<i32>(static_cast<u32>(x0) - static_cast<u32>(x1))));  // abs
        const i32 dy = joan::orig::arg::as<i32>(joan::orig::libc::abs(static_cast<i32>(static_cast<u32>(y0) - static_cast<u32>(y1))));  // abs
        return dx > dy ? dx : dy;
    }

    // ------------------------------------------------------------------ 0x005905f0 (dead code)
    u32 rand_next() {
        jo::view<u32>(jo::self<jo::Misc>(this)->random_seed) = jo::view<u32>(jo::self<jo::Misc>(this)->random_seed) * 0x015a4e35u + 1u;
        return jo::view<u32>(jo::self<jo::Misc>(this)->random_seed);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Timer::wait", 0x0058ef30, &H::wait);
B("Chr::upper", 0x0058f280, &H::upper);
B("Chr::lower", 0x0058f2c0, &H::lower);
B("Str::fill_spaces", 0x0058f7f0, &H::fill_spaces);
B("Grid::chebyshev", 0x00590450, &H::chebyshev);
B("Rand::next", 0x005905f0, &H::rand_next);
#undef B

}  // namespace game_b103_10_2
}  // namespace

// ---------------------------------------- from modules/game_b102_08_2.cpp (game/b102_08)
namespace {
namespace game_b102_08_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;

using i8 = std::int8_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;

using joan::call::thiscall;
using joan::img::str;
using joan::orig::helpers::addr;

// ---- CRT (game copies) ------------------------------------------------------------------------
i32 crt_strlen(const char* s) { return joan::orig::arg::as<i32>(std::strlen(joan::orig::arg::str(s))); }
char* crt_strncpy(char* d, const char* s, i32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
std::uintptr_t crt_memset(void* p, i32 v, u32 n) { return joan::orig::arg::as<std::uintptr_t>(std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n))); }

JOAN_COUNTER(c_word_cut, "Misc::word_cut");
JOAN_COUNTER(c_str_str, "Misc::str_str");
JOAN_COUNTER(c_first_word_copy, "Misc::first_word_copy");
JOAN_COUNTER(c_first_word, "Misc::first_word");
JOAN_COUNTER(c_ltrim_copy, "Misc::ltrim_copy");
JOAN_COUNTER(c_rtrim_copy, "Misc::rtrim_copy");
JOAN_COUNTER(c_pad, "Misc::pad");
JOAN_COUNTER(c_str_ieq, "Misc::str_ieq");
JOAN_COUNTER(c_hash, "Misc::hash");
JOAN_COUNTER(c_guid_str, "Misc::guid_str");

struct H {
    // ================================================================== Misc
    // ------------------------------------------------------------------ 0x0058ef80
    i32 word_cut(char* dst, const char* src, i32 n) {
        c_word_cut.hit();
        crt_strncpy(dst, src, n);
        dst[n] = 0;
        const i32 len = crt_strlen(src);
        if (len < n) return len;
        if (static_cast<i32>(static_cast<i8>(src[n])) == 0x20) return 0x20;
        const i32 k = joan::orig::arg::as<i32>(joan::orig::as_misc(this)->str_chr_58f0f0(joan::orig::arg::str(dst), static_cast<char>(joan::orig::arg::i32(static_cast<char>(0x20))), 1, joan::orig::arg::i32(-1)));  // 0x0058f0f0 str_chr
        if (k == 0) return 0;
        i32 eax = k;
        i32 i = n - 1;
        while (i > 0) {
            if (static_cast<i32>(static_cast<i8>(dst[i])) == 0x20) break;
            dst[i] = 0;
            --i;
            eax = i;
        }
        dst[i] = 0;
        return eax;
    }

    // ------------------------------------------------------------------ 0x0058f180
    i32 str_str(const char* s, const char* sub, i32 start, i32 end) {
        c_str_str.hit();
        if (end == -1) end = crt_strlen(s);
        const i32 sl = crt_strlen(sub);
        end -= sl;
        for (i32 i = start - 1;; ++i) {
            if (s[i] == 0) return 0;
            if (i > end) return 0;
            i32 j = 0;
            for (; j < sl; ++j) {
                if (s[i + j] == 0) break;
                if (static_cast<i8>(s[i + j]) != static_cast<i8>(sub[j])) break;
            }
            if (j == sl) return i + 1;
        }
    }

    // ------------------------------------------------------------------ 0x0058f300
    i32 ltrim_len(const char* s, i32 start, i32 n) {
        if (n == -1) n = crt_strlen(s);
        i32 i = start - 1, spaces = 0;
        for (; spaces < n && static_cast<i8>(s[i]) == 0x20; ++i) ++spaces;
        return n - spaces;
    }

    // ------------------------------------------------------------------ 0x0058f420
    i32 rtrim_copy(char* dst, const char* src) {
        c_rtrim_copy.hit();
        i32 i = crt_strlen(src) - 1;
        i32 c;
        for (;;) {
            c = static_cast<i8>(src[i]);
            if (c != 0x20) break;
            if (i < 0) break;
            dst[i] = src[i];
            --i;
        }
        dst[i + 1] = 0;
        return c;
    }

    // ------------------------------------------------------------------ 0x0058f4a0
    char* ltrim_copy(char* dst, const char* src) {
        c_ltrim_copy.hit();
        i32 i = 0;
        while (static_cast<i8>(src[i]) == 0x20 && src[i] != 0) ++i;
        i32 d = 0;
        for (; src[i] != 0; ++i, ++d) dst[d] = src[i];
        dst[d] = 0;
        return dst + d;
    }

    // ------------------------------------------------------------------ 0x0058f550
    std::uintptr_t first_word_copy(char* dst, const char* src) {
        c_first_word_copy.hit();
        i32 i = 0;
        while (static_cast<i8>(src[i]) == 0x20 && src[i] != 0) ++i;
        i32 d = 0;
        bool by_space = false;
        for (;; ++i, ++d) {
            if (src[i] == 0) break;
            if (static_cast<i8>(src[i]) == 0x20) {
                by_space = true;
                break;
            }
            dst[d] = src[i];
        }
        dst[d] = 0;
        // eax: src + i, except after copying when the NUL ended the loop (then the new d)
        return (!by_space && d > 0) ? static_cast<u32>(d) : addr(src + i);
    }

    // ------------------------------------------------------------------ 0x0058f730
    char* first_word(char* s) {
        c_first_word.hit();
        i32 i = 0;
        while (static_cast<i8>(s[i]) == 0x20 && s[i] != 0) ++i;
        i32 d = 0;
        for (; s[i] != 0 && static_cast<i8>(s[i]) != 0x20; ++i, ++d) s[d] = s[i];
        s[d] = 0;
        return s;
    }

    // ------------------------------------------------------------------ 0x0058f830
    i32 is_blank(const char* s, i32 n) {
        if (n == 0) n = crt_strlen(s);
        for (i32 i = 0; i < n; ++i)
            if (static_cast<i8>(s[i]) != 0x20) return 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x0058f8b0
    std::uintptr_t pad(char* s, i32 n, char c) {
        c_pad.hit();
        i32 len;
        if (c == 0)
            len = crt_strlen(s);
        else
            len = joan::orig::arg::as<i32>(joan::orig::as_misc(this)->str_chr_58f0f0(joan::orig::arg::str(s), static_cast<char>(joan::orig::arg::i32(c)), 1, joan::orig::arg::i32(-1))) - 1;  // 0x0058f0f0 str_chr
        std::uintptr_t eax = static_cast<u32>(len);
        if (n > len) eax = crt_memset(s + len, 0x20, static_cast<u32>(n - len));
        s[n] = 0;
        return eax;
    }

    // ------------------------------------------------------------------ 0x0058f940
    i32 is_name_char(char c) {
        const i32 v = static_cast<i8>(c);
        if (v >= 0x61 && v <= 0x7a) return 1;
        if (v >= 0x41 && v <= 0x5a) return 1;
        if (v >= 0x30 && v <= 0x39) return 1;
        if (v == 0x5c || v == 0x2e || v == 0x5f || v == 0x3a) return 1;
        return 0;
    }

    // ------------------------------------------------------------------ 0x0058fab0
    i32 str_eq(const char* a, const char* b) {
        i32 i = 0;
        for (; a[i] != 0 && b[i] != 0; ++i)
            if (static_cast<i8>(a[i]) != static_cast<i8>(b[i])) return 0;
        return b[i] == 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x0058fb40
    i32 str_ieq(const char* a, const char* b) {
        c_str_ieq.hit();
        i32 i = 0;
        for (;; ++i) {
            i32 x = static_cast<i8>(a[i]);
            if (x == 0) break;
            i32 y = static_cast<i8>(b[i]);
            if (y == 0) break;
            if (x >= 0x61 && x <= 0x7a) x -= 0x20;
            if (y >= 0x61 && y <= 0x7a) y -= 0x20;
            if (x != y) return 0;
        }
        return b[i] == 0 ? 1 : 0;
    }

    // ------------------------------------------------------------------ 0x0058fc00
    i32 hash(const char* s, i32 n) {
        c_hash.hit();
        if (n == -1) n = crt_strlen(s);
        u32 sum = 0;
        for (i32 i = 0; i < n; ++i, ++s)
            sum += static_cast<u32>(static_cast<i32>(static_cast<i8>(*s))) * (static_cast<u32>(i) + sum + 1u);
        return static_cast<i32>(sum);
    }

    // ------------------------------------------------------------------ 0x00590070
    struct Guid {  // a GUID's first 8 bytes (data4 = g[8..15])
        u32 data1;
        u16 data2;
        u16 data3;
    };
    char* guid_str(const u8* g) {
        c_guid_str.hit();
        char* const buf = reinterpret_cast<char*>(obj(d::g_a6467c));
        joan::orig::libc::sprintf(joan::orig::arg::str(buf), str(d::s_08LX_04X_04X_02X_02X_02X_02X), reinterpret_cast<const Guid*>(g)->data1, static_cast<u32>(reinterpret_cast<const Guid*>(g)->data2), static_cast<u32>(reinterpret_cast<const Guid*>(g)->data3), static_cast<u32>(g[8]), static_cast<u32>(g[9]), static_cast<u32>(g[10]), static_cast<u32>(g[11]), static_cast<u32>(g[12]), static_cast<u32>(g[13]), static_cast<u32>(g[14]), static_cast<u32>(g[15]));  // sprintf
        return buf;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_08", name, addr, fn)
B("Misc::word_cut", 0x0058ef80, &H::word_cut);
B("Misc::str_str", 0x0058f180, &H::str_str);
B("Misc::ltrim_len", 0x0058f300, &H::ltrim_len);
B("Misc::rtrim_copy", 0x0058f420, &H::rtrim_copy);
B("Misc::ltrim_copy", 0x0058f4a0, &H::ltrim_copy);
B("Misc::first_word_copy", 0x0058f550, &H::first_word_copy);
B("Misc::first_word", 0x0058f730, &H::first_word);
B("Misc::is_blank", 0x0058f830, &H::is_blank);
B("Misc::pad", 0x0058f8b0, &H::pad);
B("Misc::is_name_char", 0x0058f940, &H::is_name_char);
B("Misc::str_eq", 0x0058fab0, &H::str_eq);
B("Misc::str_ieq", 0x0058fb40, &H::str_ieq);
B("Misc::hash", 0x0058fc00, &H::hash);
B("Misc::format_590070", 0x00590070, &H::guid_str);
#undef B

}  // namespace game_b102_08_2
}  // namespace

// ---------------------------------------- from modules/game_b105_15_2.cpp (game/b105_15)
namespace {
namespace game_b105_15_2 {

using u32 = std::uint32_t;


struct H {
    // 0x005904c0
    u32 get_7a() { return jo::view<u32>(jo::self<jo::Misc>(this)->random_seed); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::get_7a", 0x005904c0, &H::get_7a);
#undef B

}  // namespace game_b105_15_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::mul_div(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_03::mul_div>(a0, a1, a2)); }  // 0x592170
auto joan::orig::muldiv_signed(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_07::muldiv_signed>(a0, a1, a2)); }  // 0x5921a0
auto joan::orig::mul_div_round(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b95_05::mul_div_round>(a0, a1, a2)); }  // 0x592290
