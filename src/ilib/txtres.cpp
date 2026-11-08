// ilib/txtres.cpp: functions of the original d:\ww\ilib\txtres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstddef>
#include <cmath>
#include <limits>

#include "../lib/crt/crt.hpp"
#include "../lib/crt/varargs.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../platform/win32_types.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b106_13.cpp (game/b106_13)
//
//    - TextRes::free_all 0x005a2070: frees the +5 entry array (count +1, 0x15-byte entries,
//      0x005a2190(0) each) and the 20 global buffers at 0x00a673b8 through Memory::free
//      0x0058ed70; byte +0 = 0. The original leaves eax = the last loop iteration's value
//      (19, or Memory::free's result when [0x00a673b8 + 19*4] was set); returned exactly.
namespace {
namespace game_b106_13 {

JOAN_COUNTER(c_txt_free, "TextRes::free_all");

constexpr img::Addr kMemory = d::g_mem;

// ======================= TextRes::free_all 0x005a2070 =======================
struct TextRes {
    std::uint32_t free_all() {
        c_txt_free.hit();
        void* const self = this;
        if (jo::as_text_res(self)->entries != nullptr) {
            for (std::int32_t i = 0; i < jo::as_text_res(self)->count; ++i)
                joan::orig::as_txt_entry(static_cast<std::uint8_t*>(jo::view<void*>(jo::as_text_res(self)->entries)) + i * static_cast<std::int32_t>(sizeof(jo::TxtEntry)))->destroy(0u);  // 0x005a2190
            joan::orig::as_mem(obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::as_text_res(self)->entries)), str(d::s_d_ww_ilib_txtres_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16938)) + 8));  // 0x0058ed70
            jo::as_text_res(self)->entries = nullptr;
        }
        std::uint32_t eax = 0;
        for (std::int32_t i = 0; i < 0x14; ++i) {
            eax = static_cast<std::uint32_t>(i);
            void*& buf = global<void*>(d::g_a673b8 + i * 4);
            if (buf != nullptr) {
                eax = thiscall<std::uint32_t>(fn::del, obj(kMemory), buf, str(d::s_d_ww_ilib_txtres_cpp),
                                              static_cast<std::int32_t>(global<std::int16_t>(d::g_a16938)) + 0x10);  // 0x0058ed70
                buf = nullptr;
            }
        }
        jo::as_text_res(self)->loaded = 0;
        return eax;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b106_13", name, addr, fn)
M("TextRes::free_all", 0x005a2070, &TextRes::free_all);
#undef M

}  // namespace game_b106_13
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
//
//    - Obj::scalar_dtor_5a2190 0x005a2190 (flags), ret 4: 0x005a1fe0, delete when flags & 1.
namespace {
namespace game_b105_16 {

using joan::orig::helpers::op_delete;

struct H {
    void* scalar_dtor_5a2190(u32 flags) {
        joan::orig::as_txt_entry(this)->destruct();  // 0x005a1fe0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::scalar_dtor_5a2190", 0x005a2190, &H::scalar_dtor_5a2190);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b111_08.cpp (fix/uninit)
//
//    - TxtRes::load 0x005a21e0 (thiscall (file name), ret 4, SEH frame for the text reader):
//      loads a .TXR text resource ("id|SECTION|subid|text" lines after a '#' header
//      separator; entries of 0x15 bytes: +0 id, +4 subid, +8 section (9 bytes), +0x11 text).
//      Original: the header-skip loop `while (line[0] != '#') read_line(line)` tests the line
//      buffer [ebp-0x228] before anything was written to it (stack garbage; '#' would skip the
//      header). Fix: the buffer's first byte starts at 0, i.e. the loop runs (the intent).
//
//  - 0x005a21e0: the stack buffers are kept contiguous and in the original order (header
//    line 12 bytes, line 0x200, subid field 12, id field 12) because the parser indexes them
//    by the line position and read_line writes its NUL at buf[n]. The field parser copies the
//    '|' separators too (they end the atoi conversions), the id field is written at the
//    absolute line position, the section field may run over into +0x11 of the entry - all as
//    in the original. Checked against the shipped t_basic.txr / t_interf.txr: every text line
//    has >= 3 '|', fields fit their buffers (id <= 4, section <= 9, subid <= 3 bytes with '|'),
//    lines < 0x1ff chars. The 0x005a11a0 message string is never destroyed (as original).
//    eax at return is not used by the callers (0x004da634 / 0x004de9d4 return it, and their
//    caller 0x004d7153 / 0x004d715d ignores it) - returns void.
namespace {
namespace game_b111_08 {


JOAN_COUNTER(c_txtres, "TxtRes::load_5a21e0");

// ---------------------------------------------------------------------- 0x005a21e0 (fix/uninit)
struct alignas(4) Reader {  // 0x00589550 text reader, [ebp-0x3dc] .. [ebp-0x234]
    u8 b[sizeof(jo::FileTxt) > 0x1a8 ? sizeof(jo::FileTxt) : 0x1a8];  // x64: a FileTxt is larger
};
struct alignas(4) Text {  // 0x005a11a0 string, [ebp-0x574] .. [ebp-0x3e0]
    u8 b[0x194];
};
struct TxrBufs {   // [ebp-0x234] .. [ebp-0x10], contiguous as in the original frame
    char hdr[12];  // [ebp-0x234] first header line (count line 1, unused)
    char line[0x200];  // [ebp-0x228]
    char subid[12];    // [ebp-0x28]
    char id[12];       // [ebp-0x1c] (count line, then the id field)
};
static_assert(sizeof(TxrBufs) == 0x224 && offsetof(TxrBufs, line) == 0xc && offsetof(TxrBufs, subid) == 0x20c &&
                  offsetof(TxrBufs, id) == 0x218,
              "TxrBufs");

i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
using joan::orig::helpers::strlen_;
char* strcpy_(void* d, const void* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void read_line(Reader& r, char* buf, i32 n) { joan::orig::as_file_txt(&r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); }  // 0x00589f10

struct HT {
    u8* entry(i32 i) { return reinterpret_cast<u8*>(&jo::self<jo::TextRes>(this)->entries[i]); }

    void load(const char* name) {
        c_txtres.hit();
        i32 state = 0;    // [ebp-0x238]
        i32 sec_len = 0;  // [ebp-0x23c]
        i32 sub_len = 0;  // [ebp-0x240]
        Reader reader;
        TxrBufs b;
        b.line[0] = 0;  // fix/uninit: the original tests this byte before any write
        joan::orig::as_file_txt(&reader)->construct(joan::orig::arg::str(name));  // 0x00589550
        read_line(reader, b.hdr, 0xa);
        read_line(reader, b.id, 0xa);
        jo::self<jo::TextRes>(this)->count = atoi_(b.id);
        jo::view<void*>(jo::self<jo::TextRes>(this)->entries) = joan::orig::as_mem(obj(d::g_mem))->add_clear(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::TextRes>(this)->count) * static_cast<u32>(sizeof(jo::TxtEntry))), str(d::s_d_ww_ilib_txtres_cpp), joan::orig::arg::i32(global<i16>(d::g_a1693c) + 0x10));  // 0x0058e8d0
        if (global<void*>(d::g_a673b8 + global<u32>(d::g_a6740c) * 4) == nullptr) {
            void* const pool = joan::orig::as_mem(obj(d::g_mem))->add_clear(0x1000u, str(d::s_d_ww_ilib_txtres_cpp), joan::orig::arg::i32(global<i16>(d::g_a1693c) + 0x16));  // 0x0058e8d0
            global<void*>(d::g_a673b8 + global<u32>(d::g_a6740c) * 4) = pool;
        }
        while (b.line[0] != '#') read_line(reader, b.line, 0x200);

        for (i32 i = 0; joan::orig::arg::as<i32>(joan::orig::as_file_txt(&reader)->at_eof()) == 0; ++i) {  // 0x0046f060 (eof)
            std::memset(b.line, 0, 0x200);
            read_line(reader, b.line, 0x200);
            if (b.line[0x1ff] != 0) {
                b.line[0x1ff] = 0;
                Text msg;
                joan::orig::as_string(&msg)->construct(str(d::s_Text_too_long));      // 0x005a11a0 "Text too long!\n"
                joan::orig::as_string(&msg)->append(str(d::s_Text_Resource_File_name));                // 0x005a1550 "Text Resource File name : "
                joan::orig::as_string(&msg)->append(joan::orig::arg::str(name));
                joan::orig::as_string(&msg)->append(str(d::kNl));                // "\n"
                joan::orig::as_string(&msg)->append(str(d::s_Text));                // "Text : "
                joan::orig::as_string(&msg)->append(joan::orig::arg::str(static_cast<const char*>(b.line)));
                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&msg)->c_str());  // 0x00417d90
                joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), joan::orig::arg::str(s));               // 0x00586cb0 log
            }
            if (b.line[0] == 0) break;
            void* const text = joan::orig::arg::as<void*>(joan::orig::txt_alloc(static_cast<i32>(strlen_(b.line) + 1)));  // 0x005a28e0
            jo::view<void*>(jo::as_txt_entry(entry(i))->text) = text;
            strcpy_(jo::view<void*>(jo::as_txt_entry(entry(i))->text), b.line);
        }

        // The parser indexes the frame block by line position (past the line into subid / id when
        // a line has fewer than three '|' - not the case in the shipped data), so index raw bytes.
        char* const raw = reinterpret_cast<char*>(&b);
        char* const base = raw + 0xc;      // [ebp-0x228]
        char* const subid = raw + 0x20c;   // [ebp-0x28]
        char* const idf = raw + 0x218;     // [ebp-0x1c]
        for (i32 i = 0; i < jo::self<jo::TextRes>(this)->count; ++i) {
            strlen_(jo::as_txt_entry(entry(i))->text);  // result unused ([ebp-0x57c])
            strcpy_(base, jo::as_txt_entry(entry(i))->text);
            i32 k = 0;
            for (; state < 3; ++k) {
                const char c = base[k];
                if (state == 0) {
                    idf[k] = c;
                } else if (state == 1) {
                    jo::as_txt_entry(entry(i))->name[sec_len] = c;
                    ++sec_len;
                } else if (state == 2) {
                    subid[sub_len] = c;
                    ++sub_len;
                }
                if (base[k] == '|') ++state;
            }
            strcpy_(jo::view<void*>(jo::as_txt_entry(entry(i))->text), base + k);
            strlen_(jo::as_txt_entry(entry(i))->text);  // result unused ([ebp-0x584])
            state = 0;
            jo::as_txt_entry(entry(i))->name[sec_len - 1] = 0;  // [entry + 7 + len]: the last name character (key's top byte for an empty name)
            sec_len = 0;
            sub_len = 0;
            const i32 idv = atoi_(idf);
            jo::as_txt_entry(entry(i))->id = idv;
            const i32 sub = atoi_(subid);
            jo::view<i32>(jo::as_txt_entry(entry(i))->key) = sub;
        }
        jo::self<jo::TextRes>(this)->loaded = 1;
        joan::orig::as_file_txt(&reader)->destruct();  // 0x00589890
    }
};

static_assert(sizeof(i16) == 2 && sizeof(float) == 4, "widths");

// fix/uninit: 0x005a21e0 tests the uninitialised line byte [ebp-0x228] against '#' before the
// first read; the fix starts it at 0 so the header lines are always skipped.
JOAN_REPLACE("fix/uninit", "TxtRes::load_5a21e0", 0x005a21e0, &HT::load);

}  // namespace game_b111_08
}  // namespace

// ---------------------------------------- from modules/game_stream2.cpp (game/stream2)
//
//      0x005a27d0 text_buffer(src)        (cdecl, txtres.cpp) - next of 8 rotating 256-byte
//                 buffers at 0xa66ab0 (index 0xa673b0), optional strncpy of src (255 chars)
namespace {
namespace game_stream2 {

// 0x005a27d0
char* JOAN_CDECL text_buffer(const char* src) {
    char* const buffer = &img::global<char>(d::g_a66ab0 + (global<std::uint32_t>(d::g_a673b0) << 8));
    global<std::int32_t>(d::g_a673b0) = global<std::int32_t>(d::g_a673b0) + 1;
    if (global<std::int32_t>(d::g_a673b0) >= 8) global<std::int32_t>(d::g_a673b0) = 0;
    if (src != nullptr) {
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(buffer)), joan::orig::arg::str(src), 0xffu);  // strncpy
        buffer[0xff] = 0;
    }
    return buffer;
}

#define STREAM2(name, addr, fn) JOAN_REPLACE("game/stream2", name, addr, fn)
STREAM2("text_buffer", 0x005a27d0, &text_buffer);
#undef STREAM2

}  // namespace game_stream2
}  // namespace

// ---------------------------------------- from modules/game_b82_11.cpp (game/b82_11)
//
//      - TextRes::small_buffer 0x005a2860 (cdecl): next of the eight 32-byte rotating buffers
//        0x00a669b0 (index 0x00a673b4), strncpy(buf, src, 31) when src != 0.
namespace {
namespace game_b82_11 {

// ---------------------------------------------------------------- 0x005a2860 (cdecl)
char* JOAN_CDECL small_buffer(const char* src) {
    char* const buf = &img::global<char>(d::g_a669b0 + (static_cast<std::uint32_t>(global<std::int32_t>(d::g_a673b4)) << 5));
    global<std::int32_t>(d::g_a673b4) = global<std::int32_t>(d::g_a673b4) + 1;
    if (global<std::int32_t>(d::g_a673b4) >= 8) global<std::int32_t>(d::g_a673b4) = 0;
    if (src != nullptr) {
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(src), 0x1fu);  // strncpy
        buf[0x1f] = 0;
    }
    return buf;
}

#define B11(name, addr, fn) JOAN_REPLACE("game/b82_11", name, addr, fn)
B11("TextRes::small_buffer", 0x005a2860, &small_buffer);
#undef B11

}  // namespace game_b82_11
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - txt_alloc 0x005a28e0 (cdecl short n): 4 KB arena blocks (0x58e8d0 "txtres.cpp").
namespace {
namespace game_b99_07 {

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_txt_alloc, "txt_alloc");

// ---------------------------------------------------------------------------- 0x005a28e0
static u8* JOAN_CDECL txt_alloc(i32 n32) {
    c_txt_alloc.hit();
    const i32 n = static_cast<i16>(n32);
    i32& used = global<i32>(d::g_a67408);
    i32& block = global<i32>(d::g_a6740c);
    if (used + n >= 0x1000) {
        block = block + 1;
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->add_clear(0x1000u, str(d::s_d_ww_ilib_txtres_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a16940)) + 7)));  // 0x0058e8d0
        global<u8*>(d::g_a673b8 + block * 4) = p;
        used = 0;
    }
    u8* const r = global<u8*>(d::g_a673b8 + block * 4) + used;
    used = used + n;
    return r;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("txt_alloc", 0x005a28e0, &txt_alloc);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b107_12.cpp (game/b107_12)
//
//    - Obj::forward_5a2070 0x005a2040 (thiscall, ret 0): TextRes::free_all 0x005a2070 on this,
//      its eax (callers 0x00566c40 / 0x00566d20 ignore it).
namespace {
namespace game_b107_12 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // 0x005a2040
    u32 forward_5a2070() { return joan::orig::arg::as<u32>(joan::orig::as_txt_res(this)->free_all()); }  // 0x005a2070
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_12", name, addr, fn)
B("Obj::forward_5a2070", 0x005a2040, &H::forward_5a2070);
#undef B

}  // namespace game_b107_12
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

struct H {
    // ------------------------------------------------------------------ 0x005a1fb0 (dead code)
    H* rec_clear() {
        std::memset(static_cast<void*>(this), 0, static_cast<std::size_t>(sizeof(jo::TxtEntry)));  // memset (0x15 on x86; a TxtEntry holds a char*)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Rec15::clear", 0x005a1fb0, &H::rec_clear);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

struct H {
    void stub_void() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::stub_5a1fe0", 0x005a1fe0, &H::stub_void);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* rec89_clear() {  // 0x005a2000
        memset_(this, 0, 0x89);
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Rec89::clear", 0x005a2000, &H::rec89_clear);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_stream2_2.cpp (game/stream2)
namespace {
namespace game_stream2_2 {

using joan::hook::global;

using joan::call::cdecl_call;

std::uint32_t crt_strlen(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
char* small_buffer() { return joan::orig::arg::as<char*>(joan::orig::text_res::small_buffer(joan::orig::arg::str(static_cast<const char*>(nullptr)))); }  // 0x005a2860

JOAN_COUNTER(c_fmt_double, "format_double");
JOAN_COUNTER(c_fmt_int, "format_int");
JOAN_COUNTER(c_fmt_roman, "format_roman");

// 0x005a29a0 - the double arrives as two raw dwords (lo, hi).
char* JOAN_CDECL format_double(std::uint32_t lo, std::uint32_t hi, std::int32_t style) {
    c_fmt_double.hit();
    char* const result = small_buffer();
    char* out = result;
    std::int32_t sign = 0;
    std::int32_t dec = 0;
    const char* digits = cdecl_call<const char*>(fn::crt_fcvt, lo, hi, 2, &dec, &sign);  // _fcvt
    double v;
    const std::uint32_t raw[2] = {lo, hi};
    std::memcpy(&v, raw, 8);
    if (!(v >= 0.0)) *out++ = 0x28;  // fcomp 0.0, test ah,1: less or unordered
    if (style == 2) *out++ = 0x24;
    for (std::int32_t i = dec; i > 0; --i) {
        if (i % 3 == 0 && i < dec) *out++ = 0x2c;
        *out++ = *digits++;
    }
    if (dec <= 0) *out++ = 0x30;
    if (v > -1000.0 && !(v >= 1000.0)) {  // test ah,0x41 (greater), then test ah,1 (less / NaN)
        *out++ = 0x2e;
        if (*digits != 0 && dec >= 0)
            *out++ = *digits++;
        else
            *out++ = 0x30;
        if (*digits != 0)
            *out++ = *digits++;
        else
            *out++ = 0x30;
    }
    if (style == 3) *out++ = 0x25;
    if (!(v >= 0.0)) *out++ = 0x29;
    *out++ = 0;
    return result;
}

// 0x005a2bf0
char* JOAN_CDECL format_int(std::int32_t value, std::int32_t style) {
    c_fmt_int.hit();
    char* const result = small_buffer();
    char* out = result;
    std::int32_t negative;
    if (value < 0) {
        negative = -1;
        value = static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(value));
    } else {
        negative = 0;
    }
    char text[0x1c];
    const char* digits = joan::orig::arg::as<const char*>(joan::orig::libc::_ltoa(joan::orig::arg::i32(value), joan::orig::arg::str(static_cast<char*>(text)), 10));  // _itoa
    const std::int32_t len = static_cast<std::int32_t>(crt_strlen(digits));
    if (negative < 0) *out++ = 0x28;
    if (style == 2) *out++ = 0x24;
    for (std::int32_t i = len; i > 0; --i) {
        if (style != 4 && i % 3 == 0 && i < len) *out++ = 0x2c;
        *out++ = *digits++;
    }
    if (style == 3) *out++ = 0x25;
    if (negative < 0) *out++ = 0x29;
    *out++ = 0;
    return result;
}

// 0x005a2d70
char* JOAN_CDECL format_roman(std::int32_t value) {
    c_fmt_roman.hit();
    char* const result = small_buffer();
    char* out = result;
    if (value <= 0) {
        *out = 0;
        return result;
    }
    char text[0x10];
    joan::orig::libc::_itoa(joan::orig::arg::i32(value), joan::orig::arg::str(static_cast<char*>(text)), 10);  // _itoa
    const std::int32_t len = static_cast<std::int32_t>(crt_strlen(text));
    for (std::int32_t i = 0; i < len; ++i) {
        const char* part = &img::global<const char>(d::kEmpty);  // ""
        const std::uint32_t c = static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(text[i])));
        switch (static_cast<std::uint32_t>(len - i - 1)) {
            case 0: part = global<const char*>(d::g_a167d4 + c * 4u); break;  // units
            case 1: part = global<const char*>(d::g_a167fc + c * 4u); break;  // tens
            case 2: part = global<const char*>(d::g_a16824 + c * 4u); break;  // hundreds
            case 3: part = global<const char*>(d::g_a1684c + c * 4u); break;  // thousands
            default: break;
        }
        for (; *part != 0; ++part) *out++ = *part;
    }
    *out = 0;
    return result;
}

#define STREAM2(name, addr, fn) JOAN_REPLACE("game/stream2", name, addr, fn)
STREAM2("format_double", 0x005a29a0, &format_double);
STREAM2("format_int", 0x005a2bf0, &format_int);
STREAM2("format_roman", 0x005a2d70, &format_roman);
#undef STREAM2

}  // namespace game_stream2_2
}  // namespace

// ---------------------------------------- from modules/game_b111_08_2.cpp (fix/uninit)
namespace {
namespace game_b111_08_2 {

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


JOAN_COUNTER(c_find, "Strings::find_sorted_5a2f00");

// ---------------------------------------------------------------------- 0x005a2f00 (fix/uninit)
void* JOAN_CDECL find_sorted(void* table, i32 id, ...) {
    c_find.hit();
    i32 lo = 0;
    i32 hi = jo::as_text_res(table)->count - 1;
    i32 mid = 0;  // fix/uninit: uninitialised in the original when the count is <= 0
    while (lo <= hi) {
        mid = static_cast<i32>(static_cast<u32>(lo) + static_cast<u32>(hi)) / 2;
        const i32 key = (&jo::as_text_res(table)->entries[mid])->id;
        if (key == id) break;
        if (id > key) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    va_list args;
    va_start(args, id);
    void* const text = joan::orig::arg::as<void*>(joan::orig::strings::format(joan::orig::arg::str(jo::view<void*>((&jo::as_text_res(table)->entries[mid])->text)), joan::orig::arg::mptr(static_cast<void*>(args))));  // 0x005a31e0
    va_end(args);
    return text;
}

// fix/uninit: 0x005a2f00 (dead code) uses an uninitialised mid index for a table count <= 0;
// the fix uses 0.
JOAN_REPLACE("fix/uninit", "Strings::find_sorted_5a2f00", 0x005a2f00, &find_sorted);

}  // namespace game_b111_08_2
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (fix/uninit)
namespace {
namespace game_b111_11 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


struct H {
    // ------------------------------------------------------------------ 0x005a2fd0 (fix/uninit)
    char* find_5a2fd0(i32 id) {
        i32 lo = 0;
        i32 hi = static_cast<i32>(jo::view<u32>(jo::self<jo::TextRes>(this)->count) - 1u);
        i32 mid = 0;  // fix/uninit: uninitialised in the original when the count is <= 0
        while (lo <= hi) {
            mid = static_cast<i32>(static_cast<u32>(lo) + static_cast<u32>(hi)) / 2;
            const i32 key = (&jo::self<jo::TextRes>(this)->entries[mid])->id;
            if (key == id) break;
            if (id > key)
                lo = mid + 1;
            else
                hi = mid - 1;
        }
        return (&jo::self<jo::TextRes>(this)->entries[mid])->text;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: mid index 0 instead of stack garbage for an empty table (dead code).
FIX("TxtTable::find_5a2fd0", 0x005a2fd0, &H::find_5a2fd0);
#undef FIX

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::call::cdecl_call;



JOAN_COUNTER(c_strings, "Strings::get");

// 0x005a3090 Strings::get(table, section, id, ...): the entry (0x15 bytes: +0 name, +4 id,
// +0x11 text) matching section and id - or the one past the end - formatted with the
// variable arguments (0x005a31e0).
void* JOAN_CDECL strings_get(void* table, const char* section, std::int32_t id, ...) {
    c_strings.hit();
    std::int32_t i = 0;
    for (; i < jo::as_text_res(table)->count; ++i) {
        const jo::TxtEntry* const entry = &jo::as_text_res(table)->entries[i];
        if (std::strcmp(joan::orig::arg::str(static_cast<const void*>(entry->name)), joan::orig::arg::str(section)) == 0 && jo::view<std::int32_t>(entry->key) == id) break;  // strcmp
    }
    va_list args;
    va_start(args, id);
    void* const text = joan::orig::arg::as<void*>(joan::orig::strings::format(joan::orig::arg::str(jo::view<void*>(jo::as_text_res(table)->entries[i].text)), joan::orig::arg::mptr(static_cast<void*>(args))));  // 0x005a31e0
    va_end(args);
    return text;
}

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Strings::get", 0x005a3090, &strings_get);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
namespace {
namespace game_b102_09 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


using joan::img::str;

JOAN_COUNTER(c_key_find, "KeyTable::find");

struct H {

    // ------------------------------------------------------------------------ 0x005a3140
    char* key_find(const char* name, u32 key) {
        c_key_find.hit();
        i32 i = 0;
        for (; i < jo::self<jo::TextRes>(this)->count; ++i) {
            u8* const e = jo::view<u8*>(jo::self<jo::TextRes>(this)->entries) + i * static_cast<i32>(sizeof(jo::TxtEntry));
            if (std::strcmp(joan::orig::arg::str(e + 8), joan::orig::arg::str(name)) == 0 && jo::as_txt_entry(e)->key == key) break;
        }
        return jo::as_txt_entry(jo::view<u8*>(jo::self<jo::TextRes>(this)->entries) + i * static_cast<i32>(sizeof(jo::TxtEntry)))->text;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("KeyTable::find", 0x005a3140, &H::key_find);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- from modules/game_strings1.cpp (game/strings1)
namespace {
namespace game_strings1 {

using joan::call::cdecl_call;

using joan::call::thiscall;

JOAN_COUNTER(c_format, "Strings::format");

// The 4-byte text cursor.
struct Cur {
    const char* p;
};

constexpr fn::Code kCurNull = fn::Ptr_Ptr;     // 0x0058b990 TextCursor()
constexpr fn::Code kCurInit = fn::TextCursor_TextCursor;     // 0x0058b9c0 TextCursor(const char*)
constexpr fn::Code kCurCopy = fn::TextIter_TextIter_copy;     // 0x0058b9f0 TextCursor(const TextCursor&)
constexpr fn::Code kCurAssign = fn::TextIter_op_assign;   // 0x0058ba20 operator=(TextCursor)
constexpr fn::Code kCurGet = fn::TextIter_get;      // 0x0058ba50 pointer
constexpr fn::Code kCurChar = fn::TextCursor_get;     // 0x0058ba70 current character
constexpr fn::Code kCurNext = fn::TextCursor_next;     // 0x0058bbc0 ++cursor
constexpr fn::Code kCurPostInc = fn::TextCursor_postinc;  // 0x0058bbf0 cursor++
constexpr fn::Code kCurPut = fn::TextCursor_put;      // 0x0058bd30 write a character, return cursor after it
constexpr fn::Code kCurAdvance = fn::TextCursor_advanced;  // 0x0058c120 cursor + n
constexpr fn::Code kCurCat = fn::TextBuffer_end_of;      // 0x0058cd40 copy a string, return cursor after it
constexpr fn::Code kCurValid = fn::TextIter_valid;    // 0x0058ce80 pointer != 0
constexpr fn::Code kCurFind = fn::TextCursor_find;     // 0x0058ceb0 find a character (null cursor if none)

void cur_null(Cur* c) { thiscall<Cur*>(kCurNull, c); }
void cur_init(Cur* c, const char* p) { thiscall<Cur*>(kCurInit, c, p); }
void cur_copy(Cur* c, const Cur* src) { thiscall<Cur*>(kCurCopy, c, src); }
std::int32_t ch(const Cur* c) { return thiscall<std::int32_t>(kCurChar, c); }
Cur* next(Cur* c) { return thiscall<Cur*>(kCurNext, c); }
bool valid(const Cur* c) { return thiscall<bool>(kCurValid, c); }

// A by-value TextCursor argument, built with the copy constructor like the original does.
const char* by_value(const Cur* src) {
    Cur arg;
    cur_copy(&arg, src);
    return arg.p;
}
const char* by_value_init(const char* p) {
    Cur arg;
    cur_init(&arg, p);
    return arg.p;
}
Cur* assign(Cur* c, const char* value) { return thiscall<Cur*>(kCurAssign, c, value); }

// out = out.put(character)
void put(Cur* out, std::int32_t character) {
    Cur tmp;
    Cur* const r = thiscall<Cur*>(kCurPut, out, &tmp, character);
    assign(out, by_value(r));
}
// out = out.cat(string)
void cat(Cur* out, const char* text) {
    const char* const src = by_value_init(text);
    Cur tmp;
    Cur* const r = thiscall<Cur*>(kCurCat, out, &tmp, src);
    assign(out, by_value(r));
}
// target = from.find(character); returns target.valid()
bool find_into(Cur* target, Cur* from, std::int32_t character) {
    Cur tmp;
    Cur* const r = thiscall<Cur*>(kCurFind, from, &tmp, character);
    return valid(assign(target, by_value(r)));
}
// t = (fresh or given cursor) = c.find('}'); if valid, c = t
void skip_to_close(Cur* c, Cur* t) {
    if (find_into(t, c, 0x7d)) assign(c, by_value(t));
}

bool is_digit(const Cur* c) { return !(ch(c) < 0x30 || ch(c) > 0x39); }
std::int32_t read_number(Cur* c) {
    std::int32_t n = 0;
    for (; is_digit(c); next(c)) n = static_cast<std::int32_t>(static_cast<std::uint32_t>(n) * 10u + static_cast<std::uint32_t>(ch(c)) - 0x30u);
    return n;
}

// "{a|...": writes a (up to '|'), then skips to '}'. c is at the type character.
void choose_first(Cur* c, Cur* out) {
    Cur t;
    cur_copy(&t, c);
    next(&t);
    if (ch(&t) != 0x7b) return;
    assign(c, by_value(next(&t)));
    for (; ch(c) != 0 && ch(c) != 0x7c; next(c)) put(out, ch(c));
    if (ch(c) == 0) return;
    skip_to_close(c, &t);
}
// "{a|b}": writes b (after the first '|', up to '}').
void choose_second(Cur* c, Cur* out) {
    Cur t;
    cur_copy(&t, c);
    next(&t);
    if (ch(&t) != 0x7b) return;
    if (!find_into(&t, next(&t), 0x7c)) return;
    Cur tmp;
    Cur* const r = thiscall<Cur*>(kCurAdvance, &t, &tmp, 1);
    assign(c, by_value(r));
    for (; ch(c) != 0 && ch(c) != 0x7d; next(c)) put(out, ch(c));
    ch(c);  // the original tests for the end of the text here; both ways end the case
}
// "{a|b|c}" with 1: writes b (between the first and the second '|'), then skips to '}'.
void choose_middle(Cur* c, Cur* out) {
    Cur t;
    cur_copy(&t, c);
    next(&t);
    if (ch(&t) != 0x7b) return;
    if (!find_into(&t, next(&t), 0x7c)) return;
    assign(c, by_value(next(&t)));
    for (; ch(c) != 0 && ch(c) != 0x7c; next(c)) put(out, ch(c));
    if (ch(c) == 0) return;
    skip_to_close(c, &t);
}
// "{a|b|c}" otherwise: writes c (after the second '|', up to '}').
void choose_third(Cur* c, Cur* out) {
    Cur t;
    cur_copy(&t, c);
    next(&t);
    bool found = false;
    if (ch(&t) == 0x7b && find_into(&t, next(&t), 0x7c)) found = true;
    if (!found) return;
    if (!find_into(&t, next(&t), 0x7c)) return;
    assign(c, by_value(next(&t)));
    for (; ch(c) != 0 && ch(c) != 0x7d; next(c)) put(out, ch(c));
    ch(c);  // the original tests for the end of the text here; both ways end the case
}
// "{a|b|c...}": writes the value-th alternative (1-based; the last one when out of range).
void choose_nth(Cur* c, Cur* out, std::int32_t value) {
    Cur t;
    cur_copy(&t, c);
    next(&t);
    if (ch(&t) != 0x7b) return;
    std::int32_t k = 1;
    next(&t);
    next(c);
    for (;;) {
        if (ch(c) == 0x7d) break;
        if (k == value) break;
        next(c);
        while (ch(c) != 0 && ch(c) != 0x7c && ch(c) != 0x7d) next(c);
        if (ch(c) == 0 || ch(c) == 0x7d) break;
        Cur tmp;
        Cur* const r = thiscall<Cur*>(kCurAdvance, c, &tmp, 1);
        assign(&t, by_value(r));
        ++k;
    }
    assign(c, by_value(&t));
    for (; ch(c) != 0 && ch(c) != 0x7c && ch(c) != 0x7d; next(c)) put(out, ch(c));
    if (ch(c) == 0) return;
    skip_to_close(c, &t);
}
void skip_block_null(Cur* c) {
    Cur t;
    cur_null(&t);
    skip_to_close(c, &t);
}

// 0x005a31e0
char* JOAN_CDECL strings_format(const char* fmt, std::uint8_t* va) {
    c_format.hit();
    Cur c, out;
    cur_null(&c);
    cur_null(&out);
    std::uint8_t types[10];
    std::memset(&types[1], 0, 9);
    std::int32_t ints[10] = {};
    const char* strs[10] = {};
    std::uint32_t dbls[10][2] = {};

    std::int32_t percents = 0;
    std::int32_t max_index = percents;

    // ---- pass 1: argument types and the highest argument number --------------------------
    assign(&c, by_value_init(fmt));
    for (;; next(&c)) {
        if (ch(&c) == 0) break;
        if (ch(&c) != 0x25) continue;
        Cur saved;
        cur_copy(&saved, &c);
        Cur old;
        thiscall<Cur*>(kCurPostInc, &c, &old, 0);
        if (ch(&c) == 0) break;
        if (ch(&c) == 0x25) {
            ++percents;
            continue;
        }
        if (!is_digit(&c)) continue;
        const std::int32_t index = read_number(&c);
        if (ch(&c) == 0) break;
        if (ch(&c) == 0x2c) {
            next(&c);
            if (ch(&c) == 0) break;
            read_number(&c);
            if (ch(&c) == 0) break;
        }
        const bool in_range = index > 0 && index <= 9;
        switch (ch(&c)) {
            case 0x73:  // s
                if (in_range) types[index] = 2;
                break;
            case 0x66:  // f
                if (in_range) types[index] = 3;
                break;
            case 0x64:  // d
            case 0x72:  // r
            case 0x7a:  // z
                if (in_range) types[index] = 1;
                break;
            case 0x3f:  // ?
            case 0x6a:  // j
            case 0x70:  // p
            case 0x71:  // q
                if (in_range) types[index] = 1;
                skip_block_null(&c);
                break;
            default:
                break;
        }
        if (index > max_index) max_index = index;
    }
    if (max_index == 0 && percents == 0) return const_cast<char*>(fmt);

    // ---- the arguments ----------------------------------------------------------------------
    // `va` is the callers' va_list (find_sorted / strings_get / 0x004d0b10 / 0x0065e060 pass their own): one
    // pointer-sized slot per int / pointer argument (4 bytes on x86, 8 on x64 MSVC / MinGW; an int is the low half),
    // 8 bytes per double on both.
#if JOAN_SYSV_VARARGS
    // System V x86-64 (lib/crt/varargs.hpp): the arguments travel in registers and `va` is the caller's va_list
    // itself (decayed to a pointer), read with va_arg: an int, a pointer, a double (an XMM register, not two slots).
    va_list& ap = *reinterpret_cast<va_list*>(va);
    for (std::int32_t i = 1; i <= max_index && i <= 9; ++i) {
        const std::uint8_t type = types[i];
        if (type == 1) {
            ints[i] = va_arg(ap, std::int32_t);
        } else if (type == 2) {
            strs[i] = va_arg(ap, const char*);
        } else if (type == 3) {
            const double d = va_arg(ap, double);
            std::uint32_t lo, hi;
            std::memcpy(&lo, &d, 4);
            std::memcpy(&hi, reinterpret_cast<const std::uint8_t*>(&d) + 4, 4);
            // fld qword / fstp qword: exact, except that a signalling NaN comes back quiet.
            if ((hi & 0x7ff00000u) == 0x7ff00000u && ((hi & 0x000fffffu) != 0 || lo != 0) && (hi & 0x00080000u) == 0) hi |= 0x00080000u;
            dbls[i][0] = lo;
            dbls[i][1] = hi;
        } else {
            (void)va_arg(ap, std::uintptr_t);
        }
    }
#else
    constexpr std::int32_t kSlot = static_cast<std::int32_t>(sizeof(void*));
    for (std::int32_t i = 1; i <= max_index && i <= 9; ++i) {
        const std::uint8_t type = types[i];
        if (type == 1) {
            va += kSlot;
            std::memcpy(&ints[i], va - kSlot, 4);
        } else if (type == 2) {
            va += kSlot;
            std::memcpy(&strs[i], va - kSlot, sizeof strs[i]);
        } else if (type == 3) {
            va += 8;
            std::uint32_t lo, hi;
            std::memcpy(&lo, va - 8, 4);
            std::memcpy(&hi, va - 4, 4);
            // fld qword / fstp qword: exact, except that a signalling NaN comes back quiet.
            if ((hi & 0x7ff00000u) == 0x7ff00000u && ((hi & 0x000fffffu) != 0 || lo != 0) && (hi & 0x00080000u) == 0) hi |= 0x00080000u;
            dbls[i][0] = lo;
            dbls[i][1] = hi;
        } else {
            va += kSlot;
        }
    }
#endif

    // ---- pass 2: the text -------------------------------------------------------------------
    Cur start;
    {
        char* const buffer = joan::orig::arg::as<char*>(joan::orig::text_buffer(joan::orig::arg::str(0)));  // 0x005a27d0 next rotating buffer
        Cur* const r = assign(&out, by_value_init(buffer));
        cur_copy(&start, r);
    }
    assign(&c, by_value_init(fmt));
    for (;; next(&c)) {
        if (ch(&c) == 0) break;
        if (ch(&c) != 0x25) {
            put(&out, ch(&c));
            continue;
        }
        Cur saved;
        cur_copy(&saved, &c);
        next(&c);
        if (ch(&c) == 0) break;
        if (ch(&c) == 0x25) {
            put(&out, 0x25);
            continue;
        }
        if (!is_digit(&c)) {
            put(&out, 0x25);
            continue;
        }
        const std::int32_t index = read_number(&c);
        if (ch(&c) == 0) break;
        std::int32_t param = 0;
        if (ch(&c) == 0x2c) {
            next(&c);
            if (ch(&c) == 0) break;
            param = read_number(&c);
            if (ch(&c) == 0) break;
        }
        const bool in_range = index > 0 && index <= 9;
        switch (ch(&c)) {
            case 0x73:  // s
                if (in_range) cat(&out, strs[index]);
                break;
            case 0x66:  // f
                if (in_range) {
                    const std::int32_t digits = param != 0 ? param : 1;
                    cat(&out, joan::orig::arg::as<const char*>(joan::orig::format_double(joan::orig::arg::u32(dbls[index][0]), joan::orig::arg::u32(dbls[index][1]), joan::orig::arg::i32(digits))));  // 0x005a29a0
                }
                break;
            case 0x64:  // d
                if (in_range) {
                    const std::int32_t digits = param != 0 ? param : 1;
                    cat(&out, joan::orig::arg::as<const char*>(joan::orig::format_int(joan::orig::arg::i32(ints[index]), joan::orig::arg::i32(digits))));  // 0x005a2bf0
                }
                break;
            case 0x72:  // r
                if (index > 0 && index < 9 && ints[index] != 0) cat(&out, joan::orig::arg::as<const char*>(joan::orig::format_roman(joan::orig::arg::i32(ints[index]))));  // 0x005a2d70
                break;
            case 0x7a:  // z
                if (in_range && ints[index] != 0) {
                    const std::int32_t digits = param != 0 ? param : 1;
                    cat(&out, joan::orig::arg::as<const char*>(joan::orig::format_int(joan::orig::arg::i32(ints[index]), joan::orig::arg::i32(digits))));  // 0x005a2bf0
                }
                break;
            case 0x70:  // p
                if (in_range) {
                    if (ints[index] == 0 || ints[index] == 1)
                        choose_first(&c, &out);
                    else
                        choose_second(&c, &out);
                } else {
                    skip_block_null(&c);
                }
                break;
            case 0x71:  // q
                if (in_range) {
                    if (ints[index] == 0)
                        choose_first(&c, &out);
                    else if (ints[index] == 1)
                        choose_middle(&c, &out);
                    else
                        choose_third(&c, &out);
                } else {
                    Cur t;
                    cur_copy(&t, &c);
                    skip_to_close(&c, &t);
                }
                break;
            case 0x3f:  // ?
                if (in_range) {
                    if (ints[index] != 0)
                        choose_first(&c, &out);
                    else
                        choose_second(&c, &out);
                } else {
                    skip_block_null(&c);
                }
                break;
            case 0x6a:  // j
                if (in_range)
                    choose_nth(&c, &out, ints[index]);
                else
                    skip_block_null(&c);
                break;
            default:
                break;
        }
    }
    {
        Cur tmp;
        thiscall<Cur*>(kCurPut, &out, &tmp, 0);  // terminator; out itself is not moved
    }
    return thiscall<char*>(kCurGet, &start);
}

JOAN_REPLACE("game/strings1", "Strings::format", 0x005a31e0, &strings_format);

}  // namespace game_strings1
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
namespace {
namespace game_b115_10 {

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

constexpr fn::Code kMemset = fn::crt_memset;  // CRT memset
const char kEmpty[] = "";

JOAN_COUNTER(c_format, "Text::format_5a4c30");

// =========================================================================================
// 0x005a4c30 helpers: String (a char pointer: 4 bytes on x86, 8 on x64) methods, all through their thunks.
// =========================================================================================
// A String of the original: one char pointer (the TextCursor / TextIter layout).
struct Str {
    const char* p = nullptr;
};
void s_ctor0(Str* s) { joan::orig::as_text_cursor(s)->construct_null(); }                     // 0x0058b990
void s_ctor(Str* s, const char* p) { joan::orig::as_text_cursor(s)->construct(p); }            // 0x0058b9c0
Str* s_copy(Str* s, const Str* src) { return joan::orig::arg::as<Str*>(joan::orig::as_text_iter(s)->TextIter_copy(joan::orig::arg::mptr(src))); }  // 0x0058b9f0
Str* s_assign(Str* s, Str v) { return joan::orig::arg::as<Str*>(joan::orig::as_text_iter(s)->op_assign(const_cast<char*>(v.p))); }  // 0x0058ba20 (by value)
const char* s_ptr(Str* s) { return joan::orig::as_text_iter(s)->get(); }                 // 0x0058ba50
i32 s_ch(Str* s) { return joan::orig::arg::as<i32>(joan::orig::as_text_cursor(s)->get()); }                  // 0x0058ba70
Str* s_adv(Str* s) { return joan::orig::arg::as<Str*>(joan::orig::as_text_cursor(s)->next()); }               // 0x0058bbc0
Str* s_postinc(Str* s, Str* out) { return joan::orig::arg::as<Str*>(joan::orig::as_text_cursor(s)->postinc(joan::orig::arg::mptr(out), 0)); }  // 0x0058bbf0
Str* s_plus_ch(Str* s, Str* out, u32 c) { return joan::orig::arg::as<Str*>(joan::orig::as_text_cursor(s)->put(joan::orig::arg::mptr(out), joan::orig::arg::u32(c))); }  // 0x0058bd30
Str* s_plus(Str* s, Str* out, Str v) { return joan::orig::arg::as<Str*>(joan::orig::as_text_cursor(s)->copy_end(joan::orig::arg::mptr(out), v)); }     // 0x0058cd40
bool s_valid(Str* s) { return joan::orig::arg::as<u8>(joan::orig::as_text_iter(s)->valid()) != 0; }          // 0x0058ce80
Str* s_find(Str* s, Str* out, u32 c) { return joan::orig::arg::as<Str*>(joan::orig::as_text_cursor(s)->find(joan::orig::arg::mptr(out), joan::orig::arg::u32(c))); }     // 0x0058ceb0
Str* s_skip(Str* s, Str* out, i32 n) { return joan::orig::arg::as<Str*>(joan::orig::as_text_cursor(s)->advanced(joan::orig::arg::mptr(out), joan::orig::arg::i32(n))); }     // 0x0058c120
// `push ecx; mov ecx,esp; push src; call copy; lea ecx,dst; call assign` -> returns dst
Str* s_set(Str* dst, const Str* src) {
    Str a;
    s_copy(&a, src);
    return s_assign(dst, a);
}

struct Fmt {
    Str sc;  // [ebp-0xc]  cursor
    Str so;  // [ebp-0x10] output
    Str out_start;  // [ebp-0xc4]
    i32 ints[10] = {};  // [ebp-0x44 + 4N]
    const char* ptrs[10] = {};  // [ebp-0x68 + 4N]
    double dbl[10] = {};  // [ebp-0xb4 + 8N]

    i32 ch() { return s_ch(&sc); }
    void adv() { s_adv(&sc); }
    void append(u32 c) {
        Str t;
        s_set(&so, s_plus_ch(&so, &t, c));
    }
    void append_text(const char* text) {
        Str a;
        s_ctor(&a, text);
        Str t;
        s_set(&so, s_plus(&so, &t, a));
    }
    // ctor0 tmp; find '}' from the cursor; jump past it when found (blocks 0x5a5692 etc.)
    void skip_close_new() {
        Str tmp;
        s_ctor0(&tmp);
        Str t;
        if (s_valid(s_set(&tmp, s_find(&sc, &t, 0x7d)))) s_set(&sc, &tmp);
    }
    // same with a tmp copy-constructed from the cursor (0x5a5aca)
    void skip_close_copy() {
        Str tmp;
        s_copy(&tmp, &sc);
        Str t;
        if (s_valid(s_set(&tmp, s_find(&sc, &t, 0x7d)))) s_set(&sc, &tmp);
    }
    // append until 0 or `stop`; false when the text ended (-> case end)
    bool copy_until(i32 stop) {
        while (ch() != 0 && ch() != stop) {
            append(static_cast<u32>(ch()));
            adv();
        }
        return ch() != 0;
    }
    // first alternative "{a|b}": print a, continue after '}' (0x5a5494 / 0x5a5719 / 0x5a5b55)
    void choice_first() {
        Str tmp;
        s_copy(&tmp, &sc);
        s_adv(&tmp);
        if (s_ch(&tmp) != 0x7b) return;
        s_set(&sc, s_adv(&tmp));
        if (!copy_until(0x7c)) return;
        Str t;
        if (s_valid(s_set(&tmp, s_find(&sc, &t, 0x7d)))) s_set(&sc, &tmp);
    }
    // second alternative "{a|b}": print b (0x5a558e / 0x5a5c4f)
    void choice_second() {
        Str tmp;
        s_copy(&tmp, &sc);
        s_adv(&tmp);
        if (s_ch(&tmp) != 0x7b) return;
        Str t;
        Str* const r = s_find(s_adv(&tmp), &t, 0x7c);
        if (!s_valid(s_set(&tmp, r))) return;
        Str t2;
        s_set(&sc, s_skip(&tmp, &t2, 1));
        copy_until(0x7d);
    }
};

// imul acc,10 / lea [acc*10 + c - 0x30]: wrapping 32-bit arithmetic (no signed-overflow UB)
i32 digit(i32 acc, i32 c) {
    return static_cast<i32>(static_cast<u32>(acc) * 10u + static_cast<u32>(c) - 0x30u);
}

// =========================================================================================
// 0x005a4c30 (cdecl)
// =========================================================================================
// args: one pointer-sized slot per argument (int / float bits in the low dword, %s a char pointer): 4 bytes on x86, 8 on x64.
const char* JOAN_CDECL format_5a4c30(const char* fmt, const std::uintptr_t* args) {
    c_format.hit();
    Fmt F;
    s_ctor0(&F.sc);
    s_ctor0(&F.so);
    u8 types[10] = {};
    cdecl_call<void*>(kMemset, &types[1], 0, 9u);
    i32 pct = 0;   // [ebp-0x8]
    i32 max_n = 0;  // [ebp-0x4]
    // fix: unfilled typed slots (format using one index with two types): 0 / 0.0 / ""
    for (i32 k = 0; k < 10; ++k) F.ptrs[k] = kEmpty;
    {
        Str a;
        s_ctor(&a, fmt);
        s_assign(&F.sc, a);
    }
    // ---------------------------------------------------------------- pass 1
    for (bool first = true;; first = false) {
        if (!first) F.adv();
        if (F.ch() == 0) break;
        if (F.ch() != 0x25) continue;
        {
            Str b0;
            s_copy(&b0, &F.sc);  // [ebp-0xb0] (unused)
            Str t;
            s_postinc(&F.sc, &t);
        }
        if (F.ch() == 0) break;
        if (F.ch() == 0x25) {
            ++pct;
            continue;
        }
        if (F.ch() < 0x30) continue;
        if (F.ch() > 0x39) continue;
        i32 n = 0;
        for (bool f2 = true;; f2 = false) {
            if (!f2) F.adv();
            if (!(F.ch() >= 0x30)) break;
            if (!(F.ch() <= 0x39)) break;
            n = digit(n, F.ch());
        }
        if (F.ch() == 0) break;
        i32 w = 0;  // [ebp-0xb8] (unused in pass 1)
        if (F.ch() == 0x2c) {
            F.adv();
            if (F.ch() == 0) break;
            for (bool f3 = true;; f3 = false) {
                if (!f3) F.adv();
                if (!(F.ch() >= 0x30)) break;
                if (!(F.ch() <= 0x39)) break;
                w = digit(w, F.ch());
            }
            if (F.ch() == 0) break;
        }
        (void)w;
        const u32 c = static_cast<u32>(F.ch() - 0x3f);
        const bool in_range = n > 0 && n <= 9;
        if (c <= 0x3b) {
            switch (c + 0x3f) {
            case 's':
                if (in_range) types[n] = 2;
                break;
            case 'f':
                if (in_range) types[n] = 3;
                break;
            case 'd':
            case 'r':
            case 'z':
                if (in_range) types[n] = 1;
                break;
            case '?':
            case 'j':
            case 'p':
            case 'q':
                if (in_range) types[n] = 1;
                F.skip_close_new();
                break;
            default:
                break;
            }
        }
        if (n > max_n) max_n = n;
    }
    if (max_n == 0 && pct == 0) return fmt;
    for (i32 n = 1; n <= max_n && n <= 9; ++n) {
        switch (types[n]) {
        case 1:
            F.ints[n] = static_cast<i32>(args[n - 1]);
            break;
        case 2:
            F.ptrs[n] = reinterpret_cast<const char*>(args[n - 1]);
            break;
        case 3: {
            float v = 0.0f;
            std::memcpy(&v, &args[n - 1], 4);
            F.dbl[n] = static_cast<double>(v);
            break;
        }
        default:
            break;
        }
    }
    // ---------------------------------------------------------------- pass 2
    {
        const char* const buf = joan::orig::text_buffer(nullptr);  // 0x005a27d0
        Str a;
        s_ctor(&a, buf);
        s_copy(&F.out_start, s_assign(&F.so, a));
        Str b;
        s_ctor(&b, fmt);
        s_assign(&F.sc, b);
    }
    for (bool first = true;; first = false) {
        if (!first) F.adv();
        if (F.ch() == 0) break;
        if (F.ch() != 0x25) {
            F.append(static_cast<u32>(F.ch()));
            continue;
        }
        {
            Str c8;
            s_copy(&c8, &F.sc);  // [ebp-0xc8] (unused)
        }
        F.adv();
        if (F.ch() == 0) break;
        if (F.ch() == 0x25) {
            F.append(0x25u);
            continue;
        }
        if (F.ch() < 0x30 || F.ch() > 0x39) {
            F.append(0x25u);
            continue;
        }
        i32 n = 0;
        for (bool f2 = true;; f2 = false) {
            if (!f2) F.adv();
            if (!(F.ch() >= 0x30)) break;
            if (!(F.ch() <= 0x39)) break;
            n = digit(n, F.ch());
        }
        if (F.ch() == 0) break;
        i32 w = 0;
        if (F.ch() == 0x2c) {
            F.adv();
            if (F.ch() == 0) break;
            for (bool f3 = true;; f3 = false) {
                if (!f3) F.adv();
                if (!(F.ch() >= 0x30)) break;
                if (!(F.ch() <= 0x39)) break;
                w = digit(w, F.ch());
            }
            if (F.ch() == 0) break;
        }
        const u32 c = static_cast<u32>(F.ch() - 0x3f);
        if (c > 0x3b) continue;
        const bool in_range = n > 0 && n <= 9;
        const i32 prec = w != 0 ? w : 1;
        switch (c + 0x3f) {
        case 's':
            if (in_range) F.append_text(F.ptrs[n]);
            break;
        case 'f':
            if (in_range) {
                std::uint32_t dbl_bits[2];
                std::memcpy(dbl_bits, &F.dbl[n], sizeof dbl_bits);
                F.append_text(joan::orig::format_double(dbl_bits[0], dbl_bits[1], joan::orig::arg::i32(prec)));  // 0x005a29a0
            }
            break;
        case 'd':
            if (in_range) F.append_text(joan::orig::format_int(joan::orig::arg::i32(F.ints[n]), joan::orig::arg::i32(prec)));  // 0x005a2bf0
            break;
        case 'r':
            if (n > 0 && n < 9 && F.ints[n] != 0) F.append_text(joan::orig::format_roman(joan::orig::arg::i32(F.ints[n])));  // 0x005a2d70
            break;
        case 'z':
            if (in_range && F.ints[n] != 0) F.append_text(joan::orig::format_int(joan::orig::arg::i32(F.ints[n]), joan::orig::arg::i32(prec)));  // 0x005a2bf0
            break;
        case 'p':
            if (!in_range) {
                F.skip_close_new();
            } else if (F.ints[n] == 0 || F.ints[n] == 1) {
                F.choice_first();
            } else {
                F.choice_second();
            }
            break;
        case 'q':
            if (!in_range) {
                F.skip_close_copy();
            } else if (F.ints[n] == 0) {
                F.choice_first();
            } else if (F.ints[n] == 1) {
                // 0x5a5824: middle alternative "{a|b|c}" -> b
                Str tmp;
                s_copy(&tmp, &F.sc);
                s_adv(&tmp);
                if (s_ch(&tmp) == 0x7b) {
                    Str t;
                    Str* const r = s_find(s_adv(&tmp), &t, 0x7c);
                    if (s_valid(s_set(&tmp, r))) {
                        s_set(&F.sc, s_adv(&tmp));
                        if (F.copy_until(0x7c)) {
                            Str t2;
                            if (s_valid(s_set(&tmp, s_find(&F.sc, &t2, 0x7d)))) s_set(&F.sc, &tmp);
                        }
                    }
                }
            } else {
                // 0x5a596e: third alternative -> c
                Str tmp;
                s_copy(&tmp, &F.sc);
                s_adv(&tmp);
                bool ok = false;
                if (s_ch(&tmp) == 0x7b) {
                    Str t;
                    Str* const r = s_find(s_adv(&tmp), &t, 0x7c);
                    ok = s_valid(s_set(&tmp, r));
                }
                if (ok) {
                    Str t;
                    Str* const r = s_find(s_adv(&tmp), &t, 0x7c);
                    if (s_valid(s_set(&tmp, r))) {
                        s_set(&F.sc, s_adv(&tmp));
                        F.copy_until(0x7d);
                    }
                }
            }
            break;
        case '?':
            if (!in_range) {
                F.skip_close_new();
            } else if (F.ints[n] != 0) {
                F.choice_first();
            } else {
                F.choice_second();
            }
            break;
        case 'j':
            if (!in_range) {
                F.skip_close_new();
            } else {
                // 0x5a5dc9: "{a|b|c...}" -> alternative number ints[n] (1-based)
                Str tmp;
                s_copy(&tmp, &F.sc);
                s_adv(&tmp);
                if (s_ch(&tmp) == 0x7b) {
                    i32 k = 1;
                    s_adv(&tmp);
                    F.adv();
                    while (F.ch() != 0x7d && k != F.ints[n]) {
                        F.adv();
                        while (F.ch() != 0 && F.ch() != 0x7c && F.ch() != 0x7d) F.adv();
                        if (F.ch() == 0 || F.ch() == 0x7d) break;
                        Str t;
                        s_set(&tmp, s_skip(&F.sc, &t, 1));
                        ++k;
                    }
                    s_set(&F.sc, &tmp);
                    while (F.ch() != 0 && F.ch() != 0x7c && F.ch() != 0x7d) {
                        F.append(static_cast<u32>(F.ch()));
                        F.adv();
                    }
                    if (F.ch() != 0) {
                        Str t;
                        if (s_valid(s_set(&tmp, s_find(&F.sc, &t, 0x7d)))) s_set(&F.sc, &tmp);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    {
        Str t;
        s_plus_ch(&F.so, &t, 0u);
    }
    return s_ptr(&F.out_start);
}

// 0x005a4c30: typed argument slot never filled -> 0 / 0.0 / "".
JOAN_REPLACE("fix/uninit", "Text::format_5a4c30", 0x005a4c30, &format_5a4c30);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b111_11_2.cpp (game/b111_11)
namespace {
namespace game_b111_11_2 {

using joan::hook::global;

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

// S7c: the real address 0x005a6660 as resolved by seal() (a trap stub: see format_positional).
JOAN_LATE_CODE_VA(kc_s7_5a6660, 0x005a6660);

using i8 = std::int8_t;
using u64 = std::uint64_t;

JOAN_COUNTER(c_fmt, "format_positional");
JOAN_COUNTER(c_fmt_orig, "format_positional (original: mixed classes / braces)");
JOAN_COUNTER(c_fmt_fix, "format_positional (fix/uninit: unwritten argument slot read)");

// ============================================================================ text cursor
// TextCursor (4-byte char pointer) through the original thunks; all of these are pure except
// put / cat, which write into the output buffer.
struct Cur {
    const char* p;
};
constexpr fn::Code kCurCtor = fn::Ptr_Ptr;     // 0x0058b990 TextCursor()
constexpr fn::Code kCurInit = fn::TextCursor_TextCursor;     // 0x0058b9c0 TextCursor(const char*)
constexpr fn::Code kCurCopy = fn::TextIter_TextIter_copy;     // 0x0058b9f0 TextCursor(const TextCursor&)
constexpr fn::Code kCurAssign = fn::TextIter_op_assign;   // 0x0058ba20 operator=(TextCursor)
constexpr fn::Code kCurPtr = fn::TextIter_get;      // 0x0058ba50 pointer
constexpr fn::Code kCurGet = fn::TextCursor_get;      // 0x0058ba70 current character
constexpr fn::Code kCurNext = fn::TextCursor_next;     // 0x0058bbc0 ++cursor
constexpr fn::Code kCurPostInc = fn::TextCursor_postinc;  // 0x0058bbf0 cursor++ (ret, int)
constexpr fn::Code kCurPut = fn::TextCursor_put;      // 0x0058bd30 put(ret, ch)
constexpr fn::Code kCurAdvance = fn::TextCursor_advanced;  // 0x0058c120 advance(ret, n)
constexpr fn::Code kCurCat = fn::TextBuffer_end_of;      // 0x0058cd40 cat(ret, TextCursor src)
constexpr fn::Code kCurValid = fn::TextIter_valid;    // 0x0058ce80 pointer != 0
constexpr fn::Code kCurFind = fn::TextCursor_find;     // 0x0058ceb0 find(ret, ch)

void cur_ctor(Cur* c) { thiscall<Cur*>(kCurCtor, c); }
void cur_copy(Cur* c, const Cur* src) { thiscall<Cur*>(kCurCopy, c, src); }
i32 get(const Cur* c) { return thiscall<i32>(kCurGet, c); }
Cur* next(Cur* c) { return thiscall<Cur*>(kCurNext, c); }
bool valid(const Cur* c) { return (thiscall<u32>(kCurValid, c) & 0xffu) != 0; }
// dst = src (the by-value argument is built with the copy constructor, as the original does).
Cur* assign(Cur* dst, const Cur* src) {
    Cur arg;
    cur_copy(&arg, src);
    return thiscall<Cur*>(kCurAssign, dst, arg.p);
}
// dst = TextCursor(p)
Cur* assign_init(Cur* dst, const char* p) {
    Cur arg;
    thiscall<Cur*>(kCurInit, &arg, p);
    return thiscall<Cur*>(kCurAssign, dst, arg.p);
}
// dst = from.find(ch); returns dst.valid()
bool find_into(Cur* dst, Cur* from, i32 ch) {
    Cur r;
    Cur* const res = thiscall<Cur*>(kCurFind, from, &r, ch);
    return valid(assign(dst, res));
}
// out = out.put(ch)
void put_assign(Cur* out, i32 ch) {
    Cur r;
    Cur* const res = thiscall<Cur*>(kCurPut, out, &r, ch);
    assign(out, res);
}
// out = out.cat(TextCursor(s))
void cat_assign(Cur* out, const char* s) {
    Cur arg;
    thiscall<Cur*>(kCurInit, &arg, s);
    Cur r;
    Cur* const res = thiscall<Cur*>(kCurCat, out, &r, arg.p);
    assign(out, res);
}
// cur = c.advance(1)
void assign_advance1(Cur* dst, Cur* c) {
    Cur r;
    Cur* const res = thiscall<Cur*>(kCurAdvance, c, &r, i32{1});
    assign(dst, res);
}
// digits loop: while ('0' <= ch <= '9') { v = v * 10 + ch - '0'; ++cur; } (MSVC int wrap)
i32 read_number(Cur* cur) {
    u32 v = 0;
    for (;;) {
        if (get(cur) < 0x30) break;
        if (get(cur) > 0x39) break;
        v = v * 10u + static_cast<u32>(get(cur)) - 0x30u;
        next(cur);
    }
    return static_cast<i32>(v);
}

// ============================================================================ 0x005a6660
// Pass 1: the class of every index 1..9 (last conversion seen wins), the highest index and the
// number of "%%". Identical to 0x005a666c..0x005a6953 (the caller has built `cur`).
void fmt_pass1(const char* fmt, Cur* cur, u8* types, i32* pct, i32* max) {
    assign_init(cur, fmt);
    bool first = true;
    for (;;) {
        if (!first) next(cur);
        first = false;
        if (get(cur) == 0) return;
        if (get(cur) != 0x25) continue;
        Cur save;  // [ebp-0xb4] (unused copy)
        cur_copy(&save, cur);
        {
            Cur old;
            thiscall<Cur*>(kCurPostInc, cur, &old, i32{0});
        }
        if (get(cur) == 0) return;
        if (get(cur) == 0x25) {
            ++*pct;
            continue;
        }
        if (get(cur) < 0x30) continue;
        if (get(cur) > 0x39) continue;
        const i32 idx = read_number(cur);
        if (get(cur) == 0) return;
        if (get(cur) == 0x2c) {
            next(cur);
            if (get(cur) == 0) return;
            read_number(cur);  // width (unused in pass 1)
            if (get(cur) == 0) return;
        }
        const u32 sel = static_cast<u32>(get(cur)) - 0x3fu;
        if (sel <= 0x3bu) {
            switch (sel + 0x3fu) {
                case 's':
                    if (idx > 0 && idx <= 9) types[idx - 1] = 2;
                    break;
                case 'f':
                    if (idx > 0 && idx <= 9) types[idx - 1] = 3;
                    break;
                case 'd':
                case 'r':
                case 'z':
                    if (idx > 0 && idx <= 9) types[idx - 1] = 1;
                    break;
                case '?':
                case 'j':
                case 'p':
                case 'q': {
                    if (idx > 0 && idx <= 9) types[idx - 1] = 1;
                    Cur c0;  // [ebp-0xc0]
                    cur_ctor(&c0);
                    if (find_into(&c0, cur, 0x7d)) assign(cur, &c0);
                    break;
                }
                default:
                    break;
            }
        }
        if (idx > *max) *max = idx;
    }
}

// Class of the slot a pass-3 conversion reads: 1 int, 2 string, 3 double, 0 none.
u8 read_class(u8 c) {
    switch (c) {
        case 's': return 2;
        case 'f': return 3;
        case 'd':
        case 'r':
        case 'z':
        case 'p':
        case 'q':
        case '?':
        case 'j': return 1;
        default: return 0;
    }
}

// True when the C++ version reads only argument slots that pass 2 wrote (see the header).
bool format_safe(const char* fmt) {
    Cur cur{nullptr};
    u8 types[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    i32 pct = 0, max = 0;
    fmt_pass1(fmt, &cur, types, &pct, &max);
    if (max == 0 && pct == 0) return true;  // returns fmt after pass 1
    const u8* const s = reinterpret_cast<const u8*>(fmt);
    std::size_t n = 0;
    while (s[n] != 0) ++n;
    // Text mode (0x0058b2d0, from RESOURCE\CODEPAGE.RES): the cursor's get / ++ / find go through
    // 0xa157d8 / 0xa157dc / 0xa15804. The byte scan below models the cursor only when it steps
    // byte by byte: SBCS mode always; the _mbs* mode (lead = _mbctype[b + 1] & 4, 0xa6cc21) and
    // the table mode (lead = [0xa6452c + b], ranges from the file, may be < 0x80) only when the
    // format has no lead byte (then get = byte, ++ = +1, find = strchr). The 16-bit mode (get =
    // two bytes, ++ = +2) and anything unknown run the original.
    const std::uintptr_t m_get = global<std::uintptr_t>(d::fns_StrIt_a157d8), m_next = global<std::uintptr_t>(d::fns_StrIt_a157dc), m_find = global<std::uintptr_t>(d::fns_TextCursor_a15804);
    bool bytewise = false;
    if (m_get == fn::code_value(fn::StrIt_cur_58baa0) && m_next == fn::code_value(fn::StrIt_inc_58bc40) && m_find == fn::code_value(fn::TextCursor_804_strchr)) {
        bytewise = true;  // SBCS: 0x0058baa0 / 0x0058bc40 / 0x0058cef0
    } else if (m_get == fn::code_value(fn::StrIt_nextc_58bad0) && m_next == fn::code_value(fn::StrIt_inc_58bc70) && m_find == fn::code_value(fn::TextCursor_804_mb)) {
        bytewise = true;  // _mbsnextc / _mbsinc / _mbschr (0x0058bad0 / 0x0058bc70 / 0x0058cf40)
        for (std::size_t k = 0; k < n; ++k)
            if ((global<u8>(d::g_a6cc21 + s[k]) & 4u) != 0) bytewise = false;
    } else if (m_get == fn::code_value(fn::TextCursor_mb_get) && m_next == fn::code_value(fn::StrIt_inc_58bcd0) && m_find == fn::code_value(fn::TextCursor_mb_find)) {
        bytewise = true;  // lead-byte table (0x0058bb40 / 0x0058bcd0 / 0x0058cfe0)
        for (std::size_t k = 0; k < n; ++k)
            if (global<i8>(d::g_text_lead_byte_table + s[k]) != 0) bytewise = false;
    }
    if (!bytewise) {
        c_fmt_orig.hit();
        return false;
    }
    // A format inside the 8 rotating result buffers (0xa66ab0, 8 x 0x100, index at 0xa673b0) can
    // be overwritten by pass 3's own output while it is parsed; the scan below would not see that.
    const std::uintptr_t lo = reinterpret_cast<std::uintptr_t>(fmt), hi = lo + n + 1;
    if (hi > img::addr(d::g_a66ab0) && lo < img::addr(d::g_a673b4)) {
        c_fmt_orig.hit();
        return false;
    }
    for (std::size_t k = 0; k < n; ++k) {
        if (s[k] == '{') {
            c_fmt_orig.hit();
            return false;
        }
    }
    for (std::size_t k = 0; k < n; ++k) {
        if (s[k] != '%') continue;
        std::size_t q = k + 1;
        if (s[q] < '0' || s[q] > '9') continue;
        u32 idx = 0;
        while (s[q] >= '0' && s[q] <= '9') idx = idx * 10u + (s[q++] - 0x30u);
        if (s[q] == ',') {
            ++q;
            while (s[q] >= '0' && s[q] <= '9') ++q;
        }
        const u8 cls = read_class(s[q]);
        const i32 i = static_cast<i32>(idx);
        if (cls != 0 && i > 0 && i <= 9 && types[i - 1] != cls) {
            c_fmt_orig.hit();
            return false;
        }
    }
    return true;
}

// fix/uninit string value: empty in every text mode (the 16-bit mode reads a 2-byte character).
const char kEmptyText[4] = {0, 0, 0, 0};

// x87 `fld qword; fstp qword`: a signalling NaN becomes quiet, everything else is unchanged.
void x87_copy_double(u32* dst, const u8* src) {
    u64 v;
    std::memcpy(&v, src, 8);
    const u64 exp = 0x7ff0000000000000ull, mant = 0x000fffffffffffffull, quiet = 0x0008000000000000ull;
    if ((v & exp) == exp && (v & mant) != 0 && (v & quiet) == 0) v |= quiet;
    std::memcpy(dst, &v, 8);
}

// Print loop: put characters until NUL or one of the stop characters.
void print_until(Cur* cur, Cur* out, i32 stop1, i32 stop2) {
    for (;;) {
        if (get(cur) == 0) return;
        if (get(cur) == stop1) return;
        if (stop2 != 0 && get(cur) == stop2) return;
        put_assign(out, get(cur));
        next(cur);
    }
}
// "{first|second}" -> first: p (value 0 / 1), q (value 0), ? (value != 0).
void brace_first(Cur* cur, Cur* out) {
    Cur e;
    cur_copy(&e, cur);
    next(&e);
    if (get(&e) != 0x7b) return;
    assign(cur, next(&e));
    print_until(cur, out, 0x7c, 0);
    if (get(cur) == 0) return;
    if (find_into(&e, cur, 0x7d)) assign(cur, &e);
}
// "{first|second}" -> second: p (other values), ? (value 0).
void brace_second(Cur* cur, Cur* out) {
    Cur e;
    cur_copy(&e, cur);
    next(&e);
    if (get(&e) != 0x7b) return;
    next(&e);
    if (!find_into(&e, &e, 0x7c)) return;
    assign_advance1(cur, &e);
    print_until(cur, out, 0x7d, 0);
}
// Not a valid index: skip to the closing brace when there is one.
void brace_skip(Cur* cur, bool copy_ctor) {
    Cur c0;
    if (copy_ctor)
        cur_copy(&c0, cur);
    else
        cur_ctor(&c0);
    if (find_into(&c0, cur, 0x7d)) assign(cur, &c0);
}

// The C++ body of format_positional: `args` = its variable arguments (MSVC 6 cdecl: 4-byte int / pointer slots,
// 8-byte double slots, read in order like pass 2 of the original).
const char* format_v(const char* fmt, va_list args) {
    c_fmt.hit();
    Cur cur, out;  // [ebp-0x10], [ebp-0x14]
    cur_ctor(&cur);
    cur_ctor(&out);
    u8 types[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::memset(static_cast<void*>(types), i32{0}, u32{9});  // memset
    i32 pct = 0, max = 0;
    fmt_pass1(fmt, &cur, types, &pct, &max);
    if (max == 0 && pct == 0) return fmt;

    // pass 2: the arguments by index (the variable arguments in order, as the original reads its stack)
    u32 ints[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const char* strs[10] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    u32 dbls[10][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    // Classes written by pass 2 per index (1 int, 2 string, 4 double). A pass-3 read of a slot not
    // written is the original's uninitialised read (fix/uninit, see the header): int / double read
    // as 0 (the zeroed arrays), string as "". Never happens for formats format_safe accepts.
    u8 written[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool unwritten = false;
    auto int_at = [&](i32 i) -> u32 {
        if ((written[i] & 1u) == 0) unwritten = true;
        return ints[i];
    };
    auto str_at = [&](i32 i) -> const char* {
        if ((written[i] & 2u) == 0) {
            unwritten = true;
            return kEmptyText;
        }
        return strs[i];
    };
    auto dbl_at = [&](i32 i) -> const u32* {
        if ((written[i] & 4u) == 0) unwritten = true;
        return dbls[i];
    };
    for (i32 i = 1; i <= max && i <= 9; ++i) {
        switch (types[i - 1]) {
            case 1:
                ints[i] = va_arg(args, u32);
                written[i] |= 1u;
                break;
            case 2:
                strs[i] = va_arg(args, const char*);
                written[i] |= 2u;
                break;
            case 3: {
                const double v = va_arg(args, double);
                x87_copy_double(dbls[i], reinterpret_cast<const u8*>(&v));
                written[i] |= 4u;
                break;
            }
            default:
                static_cast<void>(va_arg(args, u32));  // an index no conversion names: one 4-byte slot
                break;
        }
    }

    Cur start;  // [ebp-0xc8]
    {
        const char* const buffer = joan::orig::arg::as<const char*>(joan::orig::text_buffer(joan::orig::arg::str(0u)));  // 0x005a27d0 next result buffer
        Cur* const o = assign_init(&out, buffer);
        cur_copy(&start, o);
    }
    assign_init(&cur, fmt);
    bool first = true;
    for (;;) {
        if (!first) next(&cur);
        first = false;
        if (get(&cur) == 0) break;
        if (get(&cur) != 0x25) {
            put_assign(&out, get(&cur));
            continue;
        }
        Cur pc;  // [ebp-0xcc] (unused copy)
        cur_copy(&pc, &cur);
        next(&cur);
        if (get(&cur) == 0) break;
        if (get(&cur) == 0x25) {
            put_assign(&out, 0x25);
            continue;
        }
        if (get(&cur) < 0x30 || get(&cur) > 0x39) {
            put_assign(&out, 0x25);  // the character after '%' is skipped by the loop's ++cursor
            continue;
        }
        const i32 idx = read_number(&cur);
        if (get(&cur) == 0) break;
        i32 width = 0;
        if (get(&cur) == 0x2c) {
            next(&cur);
            if (get(&cur) == 0) break;
            width = read_number(&cur);
            if (get(&cur) == 0) break;
        }
        const u32 sel = static_cast<u32>(get(&cur)) - 0x3fu;
        if (sel > 0x3bu) continue;
        const bool in = idx > 0 && idx <= 9;
        const i32 w = width != 0 ? width : 1;
        switch (sel + 0x3fu) {
            case 's':
                if (in) cat_assign(&out, str_at(idx));
                break;
            case 'f':
                if (in) {
                    const u32* const d = dbl_at(idx);
                    cat_assign(&out, joan::orig::arg::as<const char*>(joan::orig::format_double(joan::orig::arg::u32(d[0]), joan::orig::arg::u32(d[1]), joan::orig::arg::i32(w))));  // 0x005a29a0
                }
                break;
            case 'd':
                if (in) cat_assign(&out, joan::orig::arg::as<const char*>(joan::orig::format_int(joan::orig::arg::i32(int_at(idx)), joan::orig::arg::i32(w))));  // 0x005a2bf0
                break;
            case 'r':
                if (idx > 0 && idx < 9 && int_at(idx) != 0)
                    cat_assign(&out, joan::orig::arg::as<const char*>(joan::orig::format_roman(joan::orig::arg::i32(int_at(idx)))));  // 0x005a2d70
                break;
            case 'z':
                if (in && int_at(idx) != 0) cat_assign(&out, joan::orig::arg::as<const char*>(joan::orig::format_int(joan::orig::arg::i32(int_at(idx)), joan::orig::arg::i32(w))));
                break;
            case 'p':
                if (!in)
                    brace_skip(&cur, false);
                else if (int_at(idx) == 0 || int_at(idx) == 1)
                    brace_first(&cur, &out);
                else
                    brace_second(&cur, &out);
                break;
            case 'q':
                if (!in) {
                    brace_skip(&cur, true);
                } else if (int_at(idx) == 0) {
                    brace_first(&cur, &out);
                } else if (int_at(idx) == 1) {
                    Cur e;  // [ebp-0xfc]
                    cur_copy(&e, &cur);
                    next(&e);
                    if (get(&e) == 0x7b) {
                        next(&e);
                        if (find_into(&e, &e, 0x7c)) {
                            assign(&cur, next(&e));
                            print_until(&cur, &out, 0x7c, 0);
                            if (get(&cur) != 0 && find_into(&e, &cur, 0x7d)) assign(&cur, &e);
                        }
                    }
                } else {
                    Cur e;  // [ebp-0x100]
                    cur_copy(&e, &cur);
                    next(&e);
                    bool second = false;
                    if (get(&e) == 0x7b) {
                        next(&e);
                        second = find_into(&e, &e, 0x7c);
                    }
                    if (second) {
                        next(&e);
                        if (find_into(&e, &e, 0x7c)) {
                            assign(&cur, next(&e));
                            print_until(&cur, &out, 0x7d, 0);
                        }
                    }
                }
                break;
            case '?':
                if (!in)
                    brace_skip(&cur, false);
                else if (int_at(idx) != 0)
                    brace_first(&cur, &out);
                else
                    brace_second(&cur, &out);
                break;
            case 'j':
                if (!in) {
                    brace_skip(&cur, false);
                } else {
                    Cur e;  // [ebp-0x114]
                    cur_copy(&e, &cur);
                    next(&e);
                    if (get(&e) == 0x7b) {
                        u32 k = 1;  // [ebp-0x118]
                        next(&e);
                        next(&cur);
                        for (;;) {
                            if (get(&cur) == 0x7d) break;
                            if (k == int_at(idx)) break;
                            next(&cur);
                            for (;;) {
                                if (get(&cur) == 0) break;
                                if (get(&cur) == 0x7c) break;
                                if (get(&cur) == 0x7d) break;
                                next(&cur);
                            }
                            if (get(&cur) == 0) break;
                            if (get(&cur) == 0x7d) break;
                            assign_advance1(&e, &cur);
                            ++k;
                        }
                        assign(&cur, &e);
                        print_until(&cur, &out, 0x7c, 0x7d);
                        if (get(&cur) != 0 && find_into(&e, &cur, 0x7d)) assign(&cur, &e);
                    }
                }
                break;
            default:
                break;
        }
    }
    {
        Cur r;
        thiscall<Cur*>(kCurPut, &out, &r, i32{0});
    }
    if (unwritten) c_fmt_fix.hit();
    return thiscall<const char*>(kCurPtr, &start);
}

// True: run format_v. With fix/uninit enabled always (unwritten slots read as 0 / 0.0 / "");
// otherwise only when format_safe proves every pass-3 read hits a written slot.
// The flag is read on the first call (settings are loaded before install_all); a plain int, not a
// function-local static (no thread-safe-init guard on this path; a racing first call computes the
// same value).
volatile i32 g_fmt_fix = -1;
bool format_use_cpp(const char* fmt) {
    if (g_fmt_fix < 0) g_fmt_fix = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fmt_fix != 0 || format_safe(fmt);
}

// 0x005a6660 entry (cdecl, variable arguments): the C++ version, or (fix/uninit off and a format whose pass 3 could
// read an unwritten slot) the original body, i.e. the real address 0x5a6660 through its code slot. That slot is a
// trap stub (the function is reached only through its thunk 0x4010eb: S5 entry rule, src/code_slots_core.hpp), so
// that case ends the game with "original code needed" ([Fixes] UninitReads=0 only). The trap never returns and
// reads no argument; `fmt` is passed so the call is the original's (cdecl, the caller's format first).
const char* JOAN_CDECL format_positional(const char* fmt, ...) {
    if (!format_use_cpp(fmt)) return cdecl_call<const char*>(std::uintptr_t{kc_s7_5a6660}, fmt);
    va_list args;
    va_start(args, fmt);
    const char* const r = format_v(fmt, args);
    va_end(args);
    return r;
}

#define G(name, addr, fn) JOAN_REPLACE("game/b111_11", name, addr, fn)
G("format_positional", 0x005a6660, &format_positional);
#undef G

}  // namespace game_b111_11_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::TxtEntry::destruct() -> void { joan::bind::member<&::game_b104_09::H::stub_void>(this); }  // 0x5a1fe0
auto joan::orig::TxtRes::construct_5a2000() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::rec89_clear>(this)); }  // 0x5a2000
auto joan::orig::TxtRes::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_12::H::forward_5a2070>(this)); }  // 0x5a2040
auto joan::orig::TxtEntry::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_16::H::scalar_dtor_5a2190>(this, a0)); }  // 0x5a2190
auto joan::orig::text_buffer(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_stream2::text_buffer>(a0)); }  // 0x5a27d0
auto joan::orig::text_res::small_buffer(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_b82_11::small_buffer>(a0)); }  // 0x5a2860
auto joan::orig::txt_alloc(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b99_07::txt_alloc>(a0)); }  // 0x5a28e0
auto joan::orig::format_double(std::uint32_t a0, std::uint32_t a1, std::int32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_stream2_2::format_double>(a0, a1, a2)); }  // 0x5a29a0
auto joan::orig::format_int(std::int32_t a0, std::int32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_stream2_2::format_int>(a0, a1)); }  // 0x5a2bf0
auto joan::orig::format_roman(std::int32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_stream2_2::format_roman>(a0)); }  // 0x5a2d70
auto joan::orig::strings::format(const char* a0, void* a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_strings1::strings_format>(a0, a1)); }  // 0x5a31e0
auto joan::orig::text::format_5a4c30(const char* a0, void* a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_b115_10::format_5a4c30>(a0, a1)); }  // 0x5a4c30
