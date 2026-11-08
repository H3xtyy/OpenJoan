// game/unknown_mbtext.cpp: functions whose original source file is unknown (formerly game/_unplaced/mbtext.cpp;
// re/attribution.csv: between ww/ilib/gameset.cpp and ww/ilib/mem.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../platform/win32_types.hpp"
#include <cstring>
#include <cstdarg>
#include <cstdint>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

#include "../en2d/cjk_glyphs.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

// char* iterator of the text library (one pointer, trivially copyable).
struct Iter {
    char* p;
};

using u16 = std::uint16_t;

constexpr fn::Code kCurInit = fn::TextCursor_TextCursor;    // 0x0058b9c0 TextCursor(const char*)
constexpr fn::Code kCurCopy = fn::TextIter_TextIter_copy;    // 0x0058b9f0 TextCursor(const TextCursor&)

}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

using joan::img::str;

struct Store {
    img::Addr addr;
    u32 value;                     // stored as it is, unless `code` names a code address
    fn::Code code = fn::kNoCode;   // stored as its resolved pointer (fn::code_value at the store)
};
// Stores of the four switch cases of 0x0058b2d0, in the original order.
constexpr Store kCase0[26] = {{d::g_a157d4, 0x00000001}, {d::fns_StrIt_a157d8, 0, fn::StrIt_cur_58baa0}, {d::fns_StrIt_a157dc, 0, fn::StrIt_inc_58bc40}, {d::fns_StrIt_a157e0, 0, fn::StrIt_put_58bd70}, {d::fns_StrIt_a157e4, 0, fn::StrIt_len_58c1c0}, {d::fns_StrIt_a157e8, 0, fn::StrIt_len_58c310}, {d::fns_StrIt_a157ec, 0, fn::StrIt_strcpy_58c440}, {d::fns_TextCursor_a157f4, 0, fn::TextCursor_f4_strncpy}, {d::fns_TextCursor_a157f0, 0, fn::TextCursor_f0_strncpy}, {d::fns_TextCursor_a157f8, 0, fn::TextCursor_f8_strcat}, {d::fns_TextCursor_a15800, 0, fn::TextCursor_800_strncat}, {d::fns_TextCursor_a157fc, 0, fn::TextCursor_fc_strncat}, {d::fns_TextCursor_a15804, 0, fn::TextCursor_804_strchr}, {d::fns_TextCursor_a15808, 0, fn::TextCursor_808_strrchr}, {d::fns_TextCursor_a1580c, 0, fn::TextCursor_80c_strstr}, {d::fns_TextCursor_a15810, 0, fn::TextCursor_810_strcmp}, {d::fns_TextCursor_a15814, 0, fn::TextCursor_814_strncmp}, {d::fns_TextCursor_a15818, 0, fn::TextCursor_818_strncmp}, {d::fns_TextCursor_a1581c, 0, fn::TextCursor_81c_a}, {d::fns_TextCursor_a15820, 0, fn::TextCursor_820_strnicmp}, {d::fns_TextCursor_a15824, 0, fn::TextCursor_824_a}, {d::fns_TextCursor_a15828, 0, fn::TextCursor_828_a}, {d::fns_crt_a1582c, 0, fn::crt_tolower}, {d::fns_crt_a15830, 0, fn::crt_toupper}, {d::fns_Str, 0, fn::Str_lwr_a}, {d::g_a15838, 0, fn::Str_upr_a}};
constexpr Store kCase1[26] = {{d::g_a157d4, 0x00000001}, {d::fns_StrIt_a157d8, 0, fn::StrIt_nextc_58bad0}, {d::fns_StrIt_a157dc, 0, fn::StrIt_inc_58bc70}, {d::fns_StrIt_a157e0, 0, fn::TextCursor_mb_put_58bdc0}, {d::fns_StrIt_a157e4, 0, fn::StrIt_mbslen_58c1f0}, {d::fns_StrIt_a157e8, 0, fn::StrIt_len_58c340}, {d::fns_StrIt_a157ec, 0, fn::StrIt_strcpy_58c440}, {d::fns_TextCursor_a157f4, 0, fn::TextCursor_f4_mb}, {d::fns_TextCursor_a157f0, 0, fn::TextCursor_f0_mb}, {d::fns_TextCursor_a157f8, 0, fn::TextCursor_f8_strcat}, {d::fns_TextCursor_a15800, 0, fn::TextCursor_800_mb}, {d::fns_TextCursor_a157fc, 0, fn::TextCursor_fc_mb}, {d::fns_TextCursor_a15804, 0, fn::TextCursor_804_mb}, {d::fns_TextCursor_a15808, 0, fn::TextCursor_808_mb}, {d::fns_TextCursor_a1580c, 0, fn::TextCursor_80c_mb}, {d::fns_TextCursor_a15810, 0, fn::TextCursor_810_mb}, {d::fns_TextCursor_a15814, 0, fn::TextCursor_814_mb}, {d::fns_TextCursor_a15818, 0, fn::TextCursor_818_mb}, {d::fns_TextCursor_a1581c, 0, fn::TextCursor_81c_mb}, {d::fns_TextCursor_a15820, 0, fn::TextCursor_820_mb}, {d::fns_TextCursor_a15824, 0, fn::TextCursor_824_mb}, {d::fns_TextCursor_a15828, 0, fn::TextCursor_828_mb}, {d::fns_crt_a1582c, 0, fn::crt_tolower}, {d::fns_crt_a15830, 0, fn::crt_toupper}, {d::fns_Str, 0, fn::Str_lwr_b}, {d::g_a15838, 0, fn::Str_upr_b}};
constexpr Store kCase2[26] = {{d::g_a157d4, 0x00000002}, {d::fns_StrIt_a157d8, 0, fn::StrIt_cur16_58bb00}, {d::fns_StrIt_a157dc, 0, fn::StrIt_inc2_58bca0}, {d::fns_StrIt_a157e0, 0, fn::StrIt_put16_58be80}, {d::fns_StrIt_a157e4, 0, fn::StrIt_wcslen_58c220}, {d::fns_StrIt_a157e8, 0, fn::StrIt_wbytes_58c370}, {d::fns_StrIt_a157ec, 0, fn::StrIt_wcscpy_58c4a0}, {d::fns_TextCursor_a157f4, 0, fn::TextCursor_f4_w}, {d::fns_TextCursor_a157f0, 0, fn::TextCursor_f0_w}, {d::fns_TextCursor_a157f8, 0, fn::TextCursor_f8_w}, {d::fns_TextCursor_a15800, 0, fn::TextCursor_800_w}, {d::fns_TextCursor_a157fc, 0, fn::TextCursor_fc_w}, {d::fns_TextCursor_a15804, 0, fn::TextCursor_804_w}, {d::fns_TextCursor_a15808, 0, fn::TextCursor_808_w}, {d::fns_TextCursor_a1580c, 0, fn::TextCursor_80c_w}, {d::fns_TextCursor_a15810, 0, fn::TextCursor_810_w}, {d::fns_TextCursor_a15814, 0, fn::TextCursor_814_w}, {d::fns_TextCursor_a15818, 0, fn::TextCursor_818_w}, {d::fns_TextCursor_a1581c, 0, fn::TextCursor_81c_w}, {d::fns_TextCursor_a15820, 0, fn::TextCursor_820_w}, {d::fns_TextCursor_a15824, 0, fn::TextCursor_824_w}, {d::fns_TextCursor_a15828, 0, fn::TextCursor_828_w}, {d::fns_crt_a1582c, 0, fn::crt_tolower}, {d::fns_crt_a15830, 0, fn::crt_toupper}, {d::fns_Str, 0, fn::Str_lwr_c}, {d::g_a15838, 0, fn::Str_upr_c}};
constexpr Store kCase3[26] = {{d::g_a157d4, 0x00000001}, {d::fns_StrIt_a157d8, 0, fn::TextCursor_mb_get}, {d::fns_StrIt_a157dc, 0, fn::StrIt_inc_58bcd0}, {d::fns_StrIt_a157e0, 0, fn::TextCursor_mb_put}, {d::fns_StrIt_a157e4, 0, fn::TextCursor_mb_length}, {d::fns_StrIt_a157e8, 0, fn::StrIt_len_58c3a0}, {d::fns_StrIt_a157ec, 0, fn::StrIt_strcpy_58c4d0}, {d::fns_TextCursor_a157f4, 0, fn::TextCursor_f4_strncpy2}, {d::fns_TextCursor_a157f0, 0, fn::TextCursor_mb_copy_n}, {d::fns_TextCursor_a157f8, 0, fn::TextCursor_f8_strcat2}, {d::fns_TextCursor_a15800, 0, fn::TextCursor_800_strncat2}, {d::fns_TextCursor_a157fc, 0, fn::TextCursor_mb_cat_n}, {d::fns_TextCursor_a15804, 0, fn::TextCursor_mb_find}, {d::fns_TextCursor_a15808, 0, fn::TextCursor_mb_rfind}, {d::fns_TextCursor_a1580c, 0, fn::TextCursor_80c_strstr2}, {d::fns_TextCursor_a15810, 0, fn::TextCursor_810_strcmp2}, {d::fns_TextCursor_a15814, 0, fn::TextCursor_mb_ncmp}, {d::fns_TextCursor_a15818, 0, fn::TextCursor_818_strncmp2}, {d::fns_TextCursor_a1581c, 0, fn::TextCursor_mb_cmp}, {d::fns_TextCursor_a15820, 0, fn::TextCursor_mb_ncmp_58daa0}, {d::fns_TextCursor_a15824, 0, fn::TextCursor_mb_icmp}, {d::fns_TextCursor_a15828, 0, fn::TextCursor_mb_nicmp}, {d::fns_crt_a1582c, 0, fn::Mem_ptr_plus_20}, {d::fns_crt_a15830, 0, fn::Mem_ptr_minus_20}, {d::fns_Str, 0, fn::TextCursor_mb_lower}, {d::g_a15838, 0, fn::TextCursor_mb_upper}};

void apply(const Store* s) {
    for (int i = 0; i < 26; ++i) {
        if (s[i].code != fn::kNoCode)
            *reinterpret_cast<volatile std::uintptr_t*>(img::addr(s[i].addr)) = fn::code_value(s[i].code);  // code pointer slot
        else
            *reinterpret_cast<volatile u32*>(img::addr(s[i].addr)) = s[i].value;
    }
}

JOAN_COUNTER(c_cp_setup, "CodePage::setup");

// ---------------------------------------------------------------------------- 0x0058b2d0
static u32 JOAN_CDECL codepage_setup(const char* locale, u32 cp, u32 mode, char* ranges) {
    c_cp_setup.hit();
    if (locale != nullptr && joan::orig::arg::as<u32>(joan::orig::libc::setlocale(0, joan::orig::arg::str(locale))) != 0) {  // setlocale
        mode = 1;
    } else if (joan::orig::arg::as<i32>(joan::orig::libc::_setmbcp(joan::orig::arg::i32(cp))) == 0) {  // _setmbcp
        mode = 1;
    } else {
        joan::orig::warning_box(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Code_Page_u_not_supported), cp);  // 0x00586bc0 'Code Page %u not supported'
    }
    // Ours: the double-byte modes (1 the _mbs* functions, 3 the file's lead-byte table) hand the fonts 16-bit codes;
    // the CJK glyphs draw the ones the fonts lack (en2d/cjk_glyphs.hpp).
    joan::cjk::set_code_page(mode == 1 || mode == 3 ? cp : 0u);
    switch (mode) {
        case 0: apply(kCase0); return mode;
        case 1: apply(kCase1); return mode;
        case 2: apply(kCase2); return mode;
        case 3: break;
        default: return mode;
    }
    apply(kCase3);
    std::memset(joan::orig::arg::mptr(obj(d::g_text_lead_byte_table)), 0, 0x100u);  // memset
    ranges = joan::orig::arg::as<char*>(joan::orig::_strdup(joan::orig::arg::str(ranges)));           // _strdup
    char* tok = joan::orig::arg::as<char*>(joan::orig::libc::strtok(joan::orig::arg::str(ranges), str(d::s_comma_9d0728)));  // strtok
    u32 lo = 0;
    u32 hi = 0;
    while (tok != nullptr) {
        const i32 n = joan::orig::arg::as<i32>(joan::orig::libc::sscanf(joan::orig::arg::str(tok), str(d::s_x_x), &lo, &hi));  // sscanf '%x-%x'
        if (n >= 1) {
            if (n == 1) hi = lo;
            for (; lo <= hi && lo <= 0xff; ++lo) global<u8>(d::g_text_lead_byte_table + lo) = 1;
        }
        tok = joan::orig::arg::as<char*>(joan::orig::libc::strtok(joan::orig::arg::str(static_cast<char*>(nullptr)), str(d::s_comma_9d0728)));  // strtok
    }
    joan::orig::free_(joan::orig::arg::mptr(ranges));  // free
    return 0;  // eax of free; the only caller (0x004d7134, mainwnd.cpp code_page::setup) ignores it
}

struct H {
    u8* p0() { return jo::view<u8*>(jo::self<jo::StrIt>(this)->p); }

    // ------------------------------------------------------------------------ string iterator
    u32 cur_58baa0() { return p0()[0]; }
    u32 nextc_58bad0() { return joan::orig::arg::as<u32>(joan::orig::libc::_mbsnextc(joan::orig::arg::mptr(p0()))); }  // _mbsnextc
    u32 cur16_58bb00() { return ((static_cast<u32>(p0()[0]) << 8) | p0()[1]) & 0xffff; }
    H* inc_58bc40() {
        jo::self<jo::StrIt>(this)->p = jo::self<jo::StrIt>(this)->p + 1;
        return this;
    }
    char* inc_58bc70() {
        char* const r = joan::orig::arg::as<char*>(joan::orig::libc::_mbsinc(joan::orig::arg::mptr(p0())));  // _mbsinc
        jo::self<jo::StrIt>(this)->p = r;
        return r;
    }
    H* inc2_58bca0() {
        jo::self<jo::StrIt>(this)->p = jo::self<jo::StrIt>(this)->p + 2;
        return this;
    }
    H* inc_58bcd0() {
        if (global<i8>(d::g_text_lead_byte_table + static_cast<u32>(p0()[0])) != 0) jo::self<jo::StrIt>(this)->p = jo::self<jo::StrIt>(this)->p + 1;
        jo::self<jo::StrIt>(this)->p = jo::self<jo::StrIt>(this)->p + 1;
        return this;
    }
    void* put_58bd70(void* ret, u32 c) {
        p0()[0] = static_cast<u8>(c);
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const t = joan::orig::as_text_cursor(&tmp)->construct(joan::orig::arg::str(jo::self<jo::StrIt>(this)->p + 1));  // 0x0058b9c0
        joan::orig::as_text_iter(ret)->TextIter_copy(joan::orig::arg::mptr(t));                             // 0x0058b9f0
        return ret;
    }
    void* put16_58be80(void* ret, u32 c) {
        p0()[0] = static_cast<u8>(((c & 0xffff) >> 8) & 0xff);
        p0()[1] = static_cast<u8>(c);
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const t = joan::orig::as_text_cursor(&tmp)->construct(joan::orig::arg::str(jo::self<jo::StrIt>(this)->p + 2));  // 0x0058b9c0
        joan::orig::as_text_iter(ret)->TextIter_copy(joan::orig::arg::mptr(t));                             // 0x0058b9f0
        return ret;
    }
    u32 eq_58bfb0(char* p) { return jo::self<jo::StrIt>(this)->p == p ? 1u : 0u; }
    std::uintptr_t cmp_58c010(char* p) {
        if (jo::self<jo::StrIt>(this)->p == nullptr || p == nullptr) {
            const u32 flag = (jo::self<jo::StrIt>(this)->p == nullptr && p == nullptr) ? 1u : 0u;
            return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffu}) | flag;
        }
        return p >= jo::self<jo::StrIt>(this)->p ? 1u : 0u;
    }
    u32 lt_58c080(char* p) { return p < jo::self<jo::StrIt>(this)->p ? 1u : 0u; }
    std::uintptr_t cmp_58c0b0(char* p) {
        if (jo::self<jo::StrIt>(this)->p == nullptr || p == nullptr) {
            const u32 flag = (jo::self<jo::StrIt>(this)->p == nullptr && p == nullptr) ? 1u : 0u;
            return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffu}) | flag;
        }
        return jo::self<jo::StrIt>(this)->p >= p ? 1u : 0u;
    }
    u32 strlen_() { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(p0()))); }
    u32 len_58c1c0() { return strlen_(); }
    u32 len_58c310() { return strlen_(); }
    u32 len_58c340() { return strlen_(); }
    u32 len_58c3a0() { return strlen_(); }
    u32 mbslen_58c1f0() { return joan::orig::arg::as<u32>(joan::orig::libc::_mbslen(joan::orig::arg::mptr(p0()))); }
    u32 wcslen_58c220() { return joan::orig::arg::as<u32>(joan::orig::libc::wcslen(joan::orig::arg::mptr(p0()))); }
    u32 wbytes_58c370() { return joan::orig::arg::as<u32>(joan::orig::libc::wcslen(joan::orig::arg::mptr(p0()))) << 1; }
    u32 size_58c3d0() { return joan::orig::arg::as<u32>(joan::orig::as_text_cursor(this)->call_a157e8()) + global<u32>(d::g_a157d4); }  // 0x0058c2e0
    std::uintptr_t strcpy_58c440(const void* s) { return joan::orig::arg::as<std::uintptr_t>(std::strcpy(static_cast<char*>(joan::orig::arg::mptr(p0())), joan::orig::arg::str(s))); }
    std::uintptr_t op_58c470(const void* s) { return joan::orig::arg::as<std::uintptr_t>(joan::orig::libc::_mbscpy(joan::orig::arg::mptr(p0()), joan::orig::arg::mptr(s))); }
    std::uintptr_t wcscpy_58c4a0(const void* s) { return joan::orig::arg::as<std::uintptr_t>(joan::orig::libc::wcscpy(joan::orig::arg::mptr(p0()), joan::orig::arg::mptr(s))); }
    std::uintptr_t strcpy_58c4d0(const void* s) { return joan::orig::arg::as<std::uintptr_t>(std::strcpy(static_cast<char*>(joan::orig::arg::mptr(p0())), joan::orig::arg::str(s))); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("CodePage::setup", 0x0058b2d0, &codepage_setup);
B("StrIt::cur_58baa0", 0x0058baa0, &H::cur_58baa0);
B("StrIt::nextc_58bad0", 0x0058bad0, &H::nextc_58bad0);
B("StrIt::cur16_58bb00", 0x0058bb00, &H::cur16_58bb00);
B("StrIt::inc_58bc40", 0x0058bc40, &H::inc_58bc40);
B("StrIt::inc_58bc70", 0x0058bc70, &H::inc_58bc70);
B("StrIt::inc2_58bca0", 0x0058bca0, &H::inc2_58bca0);
B("StrIt::inc_58bcd0", 0x0058bcd0, &H::inc_58bcd0);
B("StrIt::put_58bd70", 0x0058bd70, &H::put_58bd70);
B("StrIt::put16_58be80", 0x0058be80, &H::put16_58be80);
B("StrIt::eq_58bfb0", 0x0058bfb0, &H::eq_58bfb0);
B("StrIt::cmp_58c010", 0x0058c010, &H::cmp_58c010);
B("StrIt::lt_58c080", 0x0058c080, &H::lt_58c080);
B("StrIt::cmp_58c0b0", 0x0058c0b0, &H::cmp_58c0b0);
B("StrIt::len_58c1c0", 0x0058c1c0, &H::len_58c1c0);
B("StrIt::mbslen_58c1f0", 0x0058c1f0, &H::mbslen_58c1f0);
B("StrIt::wcslen_58c220", 0x0058c220, &H::wcslen_58c220);
B("StrIt::len_58c310", 0x0058c310, &H::len_58c310);
B("StrIt::len_58c340", 0x0058c340, &H::len_58c340);
B("StrIt::wbytes_58c370", 0x0058c370, &H::wbytes_58c370);
B("StrIt::len_58c3a0", 0x0058c3a0, &H::len_58c3a0);
B("StrIt::size_58c3d0", 0x0058c3d0, &H::size_58c3d0);
B("StrIt::strcpy_58c440", 0x0058c440, &H::strcpy_58c440);
B("StrIt::op_58c470", 0x0058c470, &H::op_58c470);
B("StrIt::wcscpy_58c4a0", 0x0058c4a0, &H::wcscpy_58c4a0);
B("StrIt::strcpy_58c4d0", 0x0058c4d0, &H::strcpy_58c4d0);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_strings1.cpp (game/strings1)
namespace {
namespace game_strings1 {

// 0x0058e380
std::int32_t JOAN_CDECL text_toupper(std::int32_t character) {
    return cdecl_call<std::int32_t>(global<std::uintptr_t>(d::fns_crt_a15830), character);
}

struct H {
    H* cursor_init(const char* p) {  // 0x0058b9c0
        *reinterpret_cast<const char**>(this) = p;
        return this;
    }
    H* cursor_next() {  // 0x0058bbc0
        thiscall(global<std::uintptr_t>(d::fns_StrIt_a157dc), this);
        return this;
    }
    std::int32_t cursor_length() { return thiscall<std::int32_t>(global<std::uintptr_t>(d::fns_StrIt_a157e4), this); }  // 0x0058c190
    std::int32_t cursor_char() { return thiscall<std::int32_t>(global<std::uintptr_t>(d::fns_StrIt_a157d8), this); }    // 0x0058ba70
};

JOAN_REPLACE("game/strings1", "TextCursor::TextCursor", 0x0058b9c0, &H::cursor_init);
JOAN_REPLACE("game/strings1", "TextCursor::next", 0x0058bbc0, &H::cursor_next);
JOAN_REPLACE("game/strings1", "TextCursor::length", 0x0058c190, &H::cursor_length);
JOAN_REPLACE("game/strings1", "TextCursor::get", 0x0058ba70, &H::cursor_char);
JOAN_REPLACE("game/strings1", "text_toupper", 0x0058e380, &text_toupper);

}  // namespace game_strings1
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (game/b83_08)
namespace {
namespace en3d_b83_08 {

static_assert(sizeof(Iter) == sizeof(void*));

JOAN_COUNTER(c_text_assign, "TextBuffer::assign");

struct H {
    // 0x0058cdb0 (this = buffer holder, *this = char buffer)
    Iter* text_assign(Iter* ret, Iter first, Iter last) {
        c_text_assign.hit();
        if ((joan::orig::arg::as<std::uint32_t>(joan::orig::as_text_iter(&last)->valid()) & 0xffu) == 0) {  // 0x0058ce80
            Iter arg;
            joan::orig::as_text_iter(&arg)->TextIter_copy(&first);  // 0x0058b9f0
            Iter tmp;
            Iter* const r = joan::orig::arg::as<Iter*>(joan::orig::as_text_cursor(this)->copy_end(&tmp, arg));  // 0x0058cd40
            joan::orig::as_text_iter(ret)->TextIter_copy(joan::orig::arg::mptr(r));  // 0x0058b9f0
            return ret;
        }
        const std::uint32_t n = static_cast<std::uint32_t>(last.p - first.p);  // a 32-bit length in the original
        std::memcpy(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::TextBuffer>(this)->p)), joan::orig::arg::ptr(first.p), joan::orig::arg::u32(n));  // memcpy
        std::memset(joan::orig::arg::mptr(jo::self<jo::TextBuffer>(this)->p + n), 0, joan::orig::arg::u32(global<std::uint32_t>(d::g_a157d4)));  // memset
        Iter tmp;
        Iter* const r = joan::orig::arg::as<Iter*>(joan::orig::as_text_cursor(&tmp)->construct(joan::orig::arg::str(jo::self<jo::TextBuffer>(this)->p + n)));  // 0x0058b9c0
        joan::orig::as_text_iter(ret)->TextIter_copy(joan::orig::arg::mptr(r));  // 0x0058b9f0
        return ret;
    }

    // 0x0058ce80
    std::uint8_t iter_valid() { return jo::self<jo::TextIter>(this)->p != nullptr ? 1 : 0; }
    // 0x0058b9f0
    void* iter_copy(char* const* src) {
        jo::self<jo::TextIter>(this)->p = *src;
        return this;
    }
    // 0x0058ba20
    void* iter_set(char* v) {
        jo::self<jo::TextIter>(this)->p = v;
        return this;
    }
    // 0x0058ba50
    char* iter_get() { return jo::self<jo::TextIter>(this)->p; }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b83_08", name, addr, fn)
GAME("TextBuffer::assign", 0x0058cdb0, &H::text_assign);
GAME("TextIter::valid", 0x0058ce80, &H::iter_valid);
GAME("TextIter::TextIter(copy)", 0x0058b9f0, &H::iter_copy);
GAME("TextIter::operator=", 0x0058ba20, &H::iter_set);
GAME("TextIter::get", 0x0058ba50, &H::iter_get);
#undef GAME

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- from modules/game_b102_08.cpp (game/b102_08)
namespace {
namespace game_b102_08 {

// Byte distance a - b, 32-bit as in the original.
u32 dist(const void* a, const void* b) { return static_cast<u32>(static_cast<const char*>(a) - static_cast<const char*>(b)); }

char* crt_strncat(char* d, const char* s, u32 n) { return joan::orig::arg::as<char*>(joan::orig::libc::strncat(joan::orig::arg::str(d), joan::orig::arg::str(s), joan::orig::arg::u32(n))); }

// ---- text cursor --------------------------------------------------------------------------------
struct Cur {
    const char* p;
};
constexpr fn::Code kCurAssign = fn::TextIter_op_assign;  // 0x0058ba20 operator=(TextCursor)
constexpr fn::Code kCurChar = fn::TextCursor_get;    // 0x0058ba70 current character
constexpr fn::Code kCurNext = fn::TextCursor_next;    // 0x0058bbc0 ++cursor
constexpr fn::Code kCurPut = fn::TextCursor_put;     // 0x0058bd30 put(ret, ch)
constexpr fn::Code kCurValid = fn::TextIter_valid;   // 0x0058ce80 pointer != 0
constexpr fn::Code kCurFind = fn::TextCursor_find;    // 0x0058ceb0 find(ret, ch)
constexpr fn::Code kCurFindNot = fn::TextIter_find_not; // 0x0058e130 first char not in set (ret, set)
constexpr fn::Code kCurFindIf = fn::List_find_if;  // 0x0058df70 first char in set (ret, set)
constexpr fn::Code kCurCopyN = fn::TextCursor_call_7f4;   // 0x0058c6d0 copy n chars (src, n)

Cur* cur_init(Cur* c, const char* p) { return thiscall<Cur*>(kCurInit, c, p); }
Cur* cur_copy(Cur* c, const void* src) { return thiscall<Cur*>(kCurCopy, c, src); }
Cur* assign(void* c, const char* value) { return thiscall<Cur*>(kCurAssign, c, value); }
i32 ch(const void* c) { return thiscall<i32>(kCurChar, c); }
Cur* next(Cur* c) { return thiscall<Cur*>(kCurNext, c); }
bool valid(const void* c) { return thiscall<bool>(kCurValid, c); }
Cur* put(Cur* c, Cur* ret, i32 character) { return thiscall<Cur*>(kCurPut, c, ret, character); }
// A by-value TextCursor argument, built with the copy constructor like the original does.
const char* by_value(const void* src) {
    Cur arg;
    cur_copy(&arg, src);
    return arg.p;
}
const char* null_arg() {
    Cur arg;
    cur_init(&arg, nullptr);
    return arg.p;
}
i32 to_lower(i32 c) { return joan::orig::arg::as<i32>(joan::orig::to_lower(joan::orig::arg::i32(c))); }  // 0x0058e330
i32 to_upper(i32 c) { return joan::orig::arg::as<i32>(joan::orig::text_toupper(joan::orig::arg::i32(c))); }  // 0x0058e380

bool lead(u8 c) { return global<i8>(d::g_text_lead_byte_table + c) != 0; }

JOAN_COUNTER(c_copy_trim_n, "TextCursor::copy_trim_n");
JOAN_COUNTER(c_mb_find, "TextCursor::mb_find");
JOAN_COUNTER(c_mb_rfind, "TextCursor::mb_rfind");
JOAN_COUNTER(c_mb_ncmp, "TextCursor::mb_ncmp");
JOAN_COUNTER(c_mb_cmp, "TextCursor::mb_cmp");
JOAN_COUNTER(c_mb_icmp, "TextCursor::mb_icmp");
JOAN_COUNTER(c_mb_nicmp, "TextCursor::mb_nicmp");
JOAN_COUNTER(c_cspn, "TextCursor::cspn");
JOAN_COUNTER(c_spn, "TextCursor::spn");
JOAN_COUNTER(c_token, "TextCursor::token");
JOAN_COUNTER(c_mb_lower, "TextCursor::mb_lower");
JOAN_COUNTER(c_mb_upper, "TextCursor::mb_upper");

struct H {
    u8* p() { return jo::view<u8*>(jo::self<jo::TextCursor>(this)->p); }  // the cursor's pointer

    // ------------------------------------------------------------------ 0x0058bb40
    i32 mb_get() {
        if (lead(p()[0]) && p()[1] != 0) return static_cast<i32>((static_cast<u32>(p()[1]) | (static_cast<u32>(p()[0]) << 8)) & 0xffffu);
        return p()[0];
    }

    // ------------------------------------------------------------------ 0x0058bdc0 / 0x0058bef0
    Cur* mb_put(Cur* ret, u32 c) {
        Cur t;
        if ((((c & 0xffffu) >> 8) & 0xffu) == 0) {
            p()[0] = static_cast<u8>(c);
            cur_copy(ret, cur_init(&t, reinterpret_cast<const char*>(p() + 1)));
        } else {
            p()[0] = static_cast<u8>(((c & 0xffffu) >> 8) & 0xffu);
            p()[1] = static_cast<u8>(c);
            cur_copy(ret, cur_init(&t, reinterpret_cast<const char*>(p() + 2)));
        }
        return ret;
    }

    // ------------------------------------------------------------------ 0x0058c250
    i32 mb_length() {
        i32 n = 0;
        for (const u8* q = p(); *q != 0; ++q, ++n)
            if (lead(*q) && q[1] != 0) ++q;
        return n;
    }

    // ------------------------------------------------------------------ 0x0058c600
    u8* mb_copy_n(const u8* src, u32 n) {
        u8* d = p();
        for (; n > 0 && *src != 0; --n) {
            if (lead(*src) && src[1] != 0) {
                u16 w;
                std::memcpy(&w, src, 2);
                std::memcpy(d, &w, 2);
                src += 2;
                d += 2;
            } else {
                *d = *src;
                ++src;
                ++d;
            }
        }
        *d = 0;
        return d;
    }

    // ------------------------------------------------------------------ 0x0058ca10
    char* mb_cat_n(const u8* src, u32 n) {
        const u8* q = src;
        u32 i = 0;
        for (; i < n && *q != 0; ++i) {
            if (lead(*q) && q[1] != 0)
                q += 2;
            else
                ++q;
        }
        if (i < n) return crt_strncat(reinterpret_cast<char*>(p()), reinterpret_cast<const char*>(src), dist(q, p()) + 1u);
        return crt_strncat(reinterpret_cast<char*>(p()), reinterpret_cast<const char*>(src), dist(q, p()));
    }

    // ------------------------------------------------------------------ 0x0058cc40
    std::uintptr_t copy_trim_n(const char* src, u32 n) {  // eax: a pointer (the copy's, or the put temporary's)
        c_copy_trim_n.hit();
        Cur c;
        cur_copy(&c, &src);
        i32 pos = -1;
        while (dist(c.p, src) < n) {
            const i32 k = ch(&c);
            if (k == 0) break;
            if (k == 0x20) {
                if (pos == -1) pos = static_cast<i32>(dist(c.p, src));
            } else {
                pos = -1;
            }
            next(&c);
        }
        if (pos == -1) return thiscall<std::uintptr_t>(kCurCopyN, this, by_value(&src), n);
        thiscall<std::uintptr_t>(kCurCopyN, this, by_value(&src), static_cast<u32>(pos));
        Cur t;
        cur_init(&t, reinterpret_cast<const char*>(p() + pos));
        Cur tmp;
        return reinterpret_cast<std::uintptr_t>(put(&t, &tmp, 0));
    }

    // ------------------------------------------------------------------ 0x0058cfe0
    Cur* mb_find(Cur* ret, i32 c) {
        c_mb_find.hit();
        Cur t;
        cur_copy(&t, this);
        while (ch(&t) != 0 && ch(&t) != c) next(&t);
        if (ch(&t) == c) {
            cur_copy(ret, &t);
        } else {
            Cur z;
            cur_copy(ret, cur_init(&z, nullptr));
        }
        return ret;
    }

    // ------------------------------------------------------------------ 0x0058d1b0
    Cur* mb_rfind(Cur* ret, i32 c) {
        c_mb_rfind.hit();
        Cur t;
        cur_copy(&t, this);
        Cur last;
        cur_init(&last, nullptr);
        for (;;) {
            if (ch(&t) == c) assign(&last, by_value(&t));
            if (ch(&t) == 0) break;
            next(&t);
        }
        cur_copy(ret, &last);
        return ret;
    }

    // ------------------------------------------------------------------ 0x0058d5e0 / 0x0058daa0
    i32 mb_ncmp(const char* other, u32 n) {
        c_mb_ncmp.hit();
        Cur a, b;
        cur_copy(&a, this);
        cur_copy(&b, &other);
        u32 i = 0;
        for (; i < n; ++i) {
            const i32 x = ch(&a);
            if (x != ch(&b)) break;
            if (ch(&a) == 0) break;
            if (ch(&b) == 0) break;
            next(&a);
            next(&b);
        }
        if (i < n) {
            const i32 x = ch(&a);
            if (static_cast<u32>(x) - static_cast<u32>(ch(&b)) != 0) return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x0058d8e0
    i32 mb_cmp(const char* other) {
        c_mb_cmp.hit();
        Cur a, b;
        cur_copy(&a, this);
        cur_copy(&b, &other);
        for (;;) {
            const i32 x = ch(&a);
            if (x != ch(&b)) break;
            if (ch(&a) == 0) break;
            if (ch(&b) == 0) break;
            next(&a);
            next(&b);
        }
        const i32 x = ch(&a);
        return static_cast<i32>(static_cast<u32>(x) - static_cast<u32>(ch(&b)));
    }

    // ------------------------------------------------------------------ 0x0058dc60
    i32 mb_icmp(const char* other) {
        c_mb_icmp.hit();
        Cur a, b;
        cur_copy(&a, this);
        cur_copy(&b, &other);
        for (;;) {
            const i32 x = to_lower(ch(&a));
            if (x != to_lower(ch(&b))) break;
            if (ch(&a) == 0) break;
            if (ch(&b) == 0) break;
            next(&a);
            next(&b);
        }
        const i32 x = to_lower(ch(&a));
        return static_cast<i32>(static_cast<u32>(x) - static_cast<u32>(to_lower(ch(&b))));
    }

    // ------------------------------------------------------------------ 0x0058de50
    i32 mb_nicmp(const char* other, u32 n) {
        c_mb_nicmp.hit();
        Cur a, b;
        cur_copy(&a, this);
        cur_copy(&b, &other);
        u32 i = 0;
        for (; i < n; ++i) {
            const i32 x = to_lower(ch(&a));
            if (x != to_lower(ch(&b))) break;
            if (ch(&a) == 0) break;
            if (ch(&b) == 0) break;
            next(&a);
            next(&b);
        }
        if (i < n) {
            const i32 x = to_lower(ch(&a));
            if (static_cast<u32>(x) - static_cast<u32>(to_lower(ch(&b))) != 0) return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x0058e010
    i32 cspn(const char* set) {
        c_cspn.hit();
        Cur t;
        cur_copy(&t, this);
        i32 n = 0;
        for (; ch(&t) != 0; ++n, next(&t)) {
            const i32 k = ch(&t);
            Cur tmp;
            if (valid(thiscall<Cur*>(kCurFind, &set, &tmp, k))) break;
        }
        return n;
    }

    // ------------------------------------------------------------------ 0x0058e0a0
    i32 spn(const char* set) {
        c_spn.hit();
        Cur t;
        cur_copy(&t, this);
        i32 n = 0;
        for (; ch(&t) != 0; ++n, next(&t)) {
            const i32 k = ch(&t);
            Cur tmp;
            if (!valid(thiscall<Cur*>(kCurFind, &set, &tmp, k))) break;
        }
        return n;
    }

    // ------------------------------------------------------------------ 0x0058e1e0
    Cur* token(Cur* ret, const char* delims) {
        c_token.hit();
        if (!valid(this)) {
            cur_init(ret, nullptr);
            return ret;
        }
        Cur start;
        {
            const char* arg = by_value(&delims);
            Cur tmp;
            cur_copy(&start, thiscall<Cur*>(kCurFindNot, this, &tmp, arg));
        }
        if (valid(&start)) {
            Cur end;
            {
                const char* arg = by_value(&delims);
                Cur tmp;
                cur_copy(&end, thiscall<Cur*>(kCurFindIf, &start, &tmp, arg));
            }
            if (valid(&end)) {
                const char* arg = by_value(&delims);
                Cur tmp;
                Cur* const r = thiscall<Cur*>(kCurFindNot, &end, &tmp, arg);
                assign(this, by_value(r));
                Cur tmp2;
                put(&end, &tmp2, 0);
            } else {
                assign(this, null_arg());
            }
        } else {
            assign(this, null_arg());
        }
        cur_copy(ret, &start);
        return ret;
    }

    // ------------------------------------------------------------------ 0x0058e490 / 0x0058e5d0
    template <i32 (*Conv)(i32)>
    i32 mb_convert() {
        Cur t;
        cur_copy(&t, this);
        for (;; next(&t)) {
            const i32 c = ch(&t);
            if (c == 0) break;
            const i32 v = Conv(c);
            if (v != c) {
                Cur tmp;
                put(&t, &tmp, v);
            }
        }
        return 0;  // eax: the 0 character that ended the loop
    }
    i32 mb_lower() {
        c_mb_lower.hit();
        return mb_convert<&to_lower>();
    }
    i32 mb_upper() {
        c_mb_upper.hit();
        return mb_convert<&to_upper>();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_08", name, addr, fn)
B("TextCursor::mb_get", 0x0058bb40, &H::mb_get);
B("TextCursor::mb_put_58bdc0", 0x0058bdc0, &H::mb_put);
B("TextCursor::mb_put", 0x0058bef0, &H::mb_put);
B("TextCursor::mb_length", 0x0058c250, &H::mb_length);
B("TextCursor::mb_copy_n", 0x0058c600, &H::mb_copy_n);
B("TextCursor::mb_cat_n", 0x0058ca10, &H::mb_cat_n);
B("TextCursor::copy_trim_n", 0x0058cc40, &H::copy_trim_n);
B("TextCursor::mb_find", 0x0058cfe0, &H::mb_find);
B("TextCursor::mb_rfind", 0x0058d1b0, &H::mb_rfind);
B("TextCursor::mb_ncmp", 0x0058d5e0, &H::mb_ncmp);
B("TextCursor::mb_cmp", 0x0058d8e0, &H::mb_cmp);
B("TextCursor::mb_ncmp_58daa0", 0x0058daa0, &H::mb_ncmp);
B("TextCursor::mb_icmp", 0x0058dc60, &H::mb_icmp);
B("TextCursor::mb_nicmp", 0x0058de50, &H::mb_nicmp);
B("TextCursor::cspn", 0x0058e010, &H::cspn);
B("TextCursor::spn", 0x0058e0a0, &H::spn);
B("TextCursor::token", 0x0058e1e0, &H::token);
B("TextCursor::mb_lower", 0x0058e490, &H::mb_lower);
B("TextCursor::mb_upper", 0x0058e5d0, &H::mb_upper);
#undef B

}  // namespace game_b102_08
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x0058bd30
    void* cursor_put(void* out, std::uint32_t arg) {
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const r = thiscall<void*>(global<std::uintptr_t>(d::fns_StrIt_a157e0), this, &tmp, arg);
        joan::orig::as_text_iter(out)->TextIter_copy(joan::orig::arg::mptr(r));  // 0x0058b9f0
        return out;
    }

    // 0x0058ceb0
    void* cursor_find(void* out, std::uint32_t arg) {
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const r = thiscall<void*>(global<std::uintptr_t>(d::fns_TextCursor_a15804), this, &tmp, arg);
        joan::orig::as_text_iter(out)->TextIter_copy(joan::orig::arg::mptr(r));  // 0x0058b9f0
        return out;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("TextCursor::put", 0x0058bd30, &H::cursor_put);
M("TextCursor::find", 0x0058ceb0, &H::cursor_find);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

struct H {
    // 0x0058bfe0
    std::int32_t below(char* v) { return jo::self<jo::StrIt>(this)->p < v ? 1 : 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Obj::below", 0x0058bfe0, &H::below);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03 {

// 0x0058e330 tolower through the CRT pointer at 0xa1582c.
std::int32_t JOAN_CDECL to_lower(std::int32_t c) { return cdecl_call<std::int32_t>(global<std::uintptr_t>(d::fns_crt_a1582c), c); }

struct H {
    // 0x0058c120 copy of this cursor advanced n characters (hidden return).
    void* advanced(void* out, std::int32_t n) {
        std::uintptr_t cursor;  // a TextCursor (one pointer)
        joan::orig::as_text_iter(&cursor)->TextIter_copy(joan::orig::arg::mptr(this));  // 0x0058b9f0 copy
        if (n >= 0) {
            for (; n > 0; --n) joan::orig::as_text_cursor(&cursor)->next();  // 0x0058bbc0 next
        }
        joan::orig::as_text_iter(out)->TextIter_copy(&cursor);
        return out;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("TextCursor::advanced", 0x0058c120, &H::advanced);
B91("to_lower", 0x0058e330, &to_lower);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (game/b85_04)
namespace {
namespace en3d_b85_04 {

static_assert(sizeof(Iter) == sizeof(void*));

struct H {
    // 0x0058c400
    std::uintptr_t text_fn_ec(Iter it) {  // the strcpy result (a pointer)
        Iter arg;
        joan::orig::as_text_iter(&arg)->TextIter_copy(&it);  // 0x0058b9f0
        return thiscall<std::uintptr_t>(global<std::uintptr_t>(d::fns_StrIt_a157ec), this, arg);
    }

    // 0x0058c2e0
    std::uint32_t text_fn_e8() { return thiscall<std::uint32_t>(global<std::uintptr_t>(d::fns_StrIt_a157e8), this); }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b85_04", name, addr, fn)
GAME("TextCursor::call_a157ec", 0x0058c400, &H::text_fn_ec);
GAME("TextCursor::call_a157e8", 0x0058c2e0, &H::text_fn_e8);
#undef GAME

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b103_09.cpp (game/b103_09)
namespace {
namespace game_b103_09 {

// (the called constructors fill `arg` / `tmp`: Clang cannot see that)
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#pragma clang diagnostic ignored "-Wunknown-warning-option"  // (the next one is Clang 21+)
#pragma clang diagnostic ignored "-Wuninitialized-const-pointer"
#endif
// A by-value cursor argument (one pointer), built with the copy constructor like the original does.
std::uintptr_t by_value(const std::uintptr_t* pos) {
    std::uintptr_t arg;
    thiscall<void*>(kCurCopy, &arg, pos);
    return arg;
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
std::uintptr_t slot(img::Addr table_entry) { return global<std::uintptr_t>(table_entry); }

struct H {
    char* p() { return jo::self<jo::TextCursor>(this)->p; }

    // CRT forwarders: crt(this->p, a [, b]) -> eax. `a` is the other string (a pointer) and the result a pointer
    // (copy / cat) or an int (compare), so both are pointer-size.
    template <fn::Code Crt>
    std::uintptr_t crt2(std::uintptr_t a, u32 b) {
        return cdecl_call<std::uintptr_t>(Crt, p(), a, b);
    }
    template <fn::Code Crt>
    std::uintptr_t crt2_half(std::uintptr_t a, u32 b) {
        return cdecl_call<std::uintptr_t>(Crt, p(), a, b >> 1);
    }
    template <fn::Code Crt>
    std::uintptr_t crt1(std::uintptr_t a) {
        return cdecl_call<std::uintptr_t>(Crt, p(), a);
    }
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option"  // (the next one is Clang 21+)
#pragma clang diagnostic ignored "-Wuninitialized-const-pointer"  // kCurInit constructs `tmp`
#endif
    // ret = TextCursor(crt(this->p, a)); `a` is a character (strchr) or a string (strstr).
    template <fn::Code Crt>
    void* crt_cursor(void* out, std::uintptr_t a) {
        const std::uintptr_t r = cdecl_call<std::uintptr_t>(Crt, p(), a);
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const c = thiscall<void*>(kCurInit, &tmp, r);
        thiscall<void*>(kCurCopy, out, c);
        return out;
    }
    template <fn::Code Crt>
    void* crt_cursor_w(void* out, u32 a) {
        const std::uintptr_t r = cdecl_call<std::uintptr_t>(Crt, p(), static_cast<u32>(static_cast<u16>(a)));
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const c = thiscall<void*>(kCurInit, &tmp, r);
        thiscall<void*>(kCurCopy, out, c);
        return out;
    }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

    // Front-ends through the cursor function table.
    template <auto Slot>
    std::uintptr_t call_pos_n(std::uintptr_t pos, u32 n) {
        const std::uintptr_t arg = by_value(&pos);
        return thiscall<std::uintptr_t>(slot(Slot), this, arg, n);
    }
    template <auto Slot>
    std::uintptr_t call_pos(std::uintptr_t pos) {
        const std::uintptr_t arg = by_value(&pos);
        return thiscall<std::uintptr_t>(slot(Slot), this, arg);
    }
    // 0x0058d080
    void* call_808(void* out, u32 a) {
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const r = thiscall<void*>(slot(d::fns_TextCursor_a15808), this, &tmp, a);
        thiscall<void*>(kCurCopy, out, r);
        return out;
    }
    // 0x0058d250
    void* call_80c(void* out, std::uintptr_t pos) {
        const std::uintptr_t arg = by_value(&pos);
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        void* const r = thiscall<void*>(slot(d::fns_TextCursor_a1580c), this, &tmp, arg);
        thiscall<void*>(kCurCopy, out, r);
        return out;
    }
    // 0x0058e3d0
    u32 call_834() { return thiscall<u32>(slot(d::fns_Str), this); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_09", name, addr, fn)
// Front-ends.
B("TextCursor::call_f0", 0x0058c500, &H::call_pos_n<d::fns_TextCursor_a157f0>);
B("TextCursor::call_f8", 0x0058c810, &H::call_pos<d::fns_TextCursor_a157f8>);
B("TextCursor::call_fc", 0x0058c910, &H::call_pos_n<d::fns_TextCursor_a157fc>);
B("TextCursor::call_800", 0x0058cb00, &H::call_pos_n<d::fns_TextCursor_a15800>);
B("TextCursor::call_808", 0x0058d080, &H::call_808);
B("TextCursor::call_80c", 0x0058d250, &H::call_80c);
B("TextCursor::call_810", 0x0058d3e0, &H::call_pos<d::fns_TextCursor_a15810>);
B("TextCursor::call_814", 0x0058d4e0, &H::call_pos_n<d::fns_TextCursor_a15814>);
B("TextCursor::call_818", 0x0058d6d0, &H::call_pos_n<d::fns_TextCursor_a15818>);
B("TextCursor::call_81c", 0x0058d810, &H::call_pos<d::fns_TextCursor_a1581c>);
B("TextCursor::call_820", 0x0058d9a0, &H::call_pos_n<d::fns_TextCursor_a15820>);
B("TextCursor::call_824", 0x0058db90, &H::call_pos<d::fns_TextCursor_a15824>);
B("TextCursor::call_828", 0x0058dd50, &H::call_pos_n<d::fns_TextCursor_a15828>);
B("TextCursor::call_834", 0x0058e3d0, &H::call_834);
// (a, b), ret 8.
B("TextCursor::f0_strncpy", 0x0058c540, &H::crt2<fn::crt_strncpy>);
B("TextCursor::f0_mb", 0x0058c580, &H::crt2<fn::crt_mbsncpy>);
B("TextCursor::f0_w", 0x0058c5c0, &H::crt2<fn::crt_wcsncpy>);
B("TextCursor::f4_strncpy", 0x0058c710, &H::crt2<fn::crt_strncpy>);
B("TextCursor::f4_mb", 0x0058c750, &H::crt2<fn::crt_mbsnbcpy>);
B("TextCursor::f4_w", 0x0058c790, &H::crt2_half<fn::crt_wcsncpy>);
B("TextCursor::f4_strncpy2", 0x0058c7d0, &H::crt2<fn::crt_strncpy>);
B("TextCursor::fc_strncat", 0x0058c950, &H::crt2<fn::crt_strncat>);
B("TextCursor::fc_mb", 0x0058c990, &H::crt2<fn::crt_mbsncat>);
B("TextCursor::fc_w", 0x0058c9d0, &H::crt2<fn::crt_wcsncat>);
B("TextCursor::800_strncat", 0x0058cb40, &H::crt2<fn::crt_strncat>);
B("TextCursor::800_mb", 0x0058cb80, &H::crt2<fn::crt_mbsnbcat>);
B("TextCursor::800_w", 0x0058cbc0, &H::crt2_half<fn::crt_wcsncat>);
B("TextCursor::800_strncat2", 0x0058cc00, &H::crt2<fn::crt_strncat>);
B("TextCursor::814_strncmp", 0x0058d520, &H::crt2<fn::crt_strncmp>);
B("TextCursor::814_mb", 0x0058d560, &H::crt2<fn::crt_mbsncmp>);
B("TextCursor::814_w", 0x0058d5a0, &H::crt2<fn::crt_wcsncmp>);
B("TextCursor::818_strncmp", 0x0058d710, &H::crt2<fn::crt_strncmp>);
B("TextCursor::818_mb", 0x0058d750, &H::crt2<fn::crt_strncmp>);
B("TextCursor::818_w", 0x0058d790, &H::crt2_half<fn::crt_wcsncmp>);
B("TextCursor::818_strncmp2", 0x0058d7d0, &H::crt2<fn::crt_strncmp>);
B("TextCursor::820_strnicmp", 0x0058d9e0, &H::crt2<fn::crt_strncoll>);
B("TextCursor::820_mb", 0x0058da20, &H::crt2<fn::crt_mbsncoll>);
B("TextCursor::820_w", 0x0058da60, &H::crt2<fn::crt_wcsncoll>);
B("TextCursor::828_a", 0x0058dd90, &H::crt2<fn::crt_strnicoll>);
B("TextCursor::828_mb", 0x0058ddd0, &H::crt2<fn::crt_mbsnicoll>);
B("TextCursor::828_w", 0x0058de10, &H::crt2<fn::crt_wcsnicoll>);
// (a), ret 4.
B("TextCursor::f8_strcat", 0x0058c850, &H::crt1<fn::crt_strcat>);
B("TextCursor::f8_mb", 0x0058c880, &H::crt1<fn::crt_mbscat>);
B("TextCursor::f8_w", 0x0058c8b0, &H::crt1<fn::crt_wcscat>);
B("TextCursor::f8_strcat2", 0x0058c8e0, &H::crt1<fn::crt_strcat>);
B("TextCursor::810_strcmp", 0x0058d420, &H::crt1<fn::crt_strcmp>);
B("TextCursor::810_mb", 0x0058d450, &H::crt1<fn::crt_mbscmp>);
B("TextCursor::810_w", 0x0058d480, &H::crt1<fn::crt_wcscmp>);
B("TextCursor::810_strcmp2", 0x0058d4b0, &H::crt1<fn::crt_strcmp>);
B("TextCursor::81c_a", 0x0058d850, &H::crt1<fn::crt_strcoll>);
B("TextCursor::81c_mb", 0x0058d880, &H::crt1<fn::crt_mbscoll>);
B("TextCursor::81c_w", 0x0058d8b0, &H::crt1<fn::crt_wcscoll>);
B("TextCursor::824_a", 0x0058dbd0, &H::crt1<fn::crt_stricoll>);
B("TextCursor::824_mb", 0x0058dc00, &H::crt1<fn::crt_mbsicoll>);
B("TextCursor::824_w", 0x0058dc30, &H::crt1<fn::crt_wcsicoll>);
// Cursor results (ret, a), ret 8.
B("TextCursor::804_strchr", 0x0058cef0, &H::crt_cursor<fn::crt_strchr>);
B("TextCursor::804_mb", 0x0058cf40, &H::crt_cursor<fn::crt_mbschr>);
B("TextCursor::804_w", 0x0058cf90, &H::crt_cursor_w<fn::crt_wcschr>);
B("TextCursor::808_strrchr", 0x0058d0c0, &H::crt_cursor<fn::crt_strrchr>);
B("TextCursor::808_mb", 0x0058d110, &H::crt_cursor<fn::crt_mbsrchr>);
B("TextCursor::808_w", 0x0058d160, &H::crt_cursor_w<fn::crt_wcsrchr>);
B("TextCursor::80c_strstr", 0x0058d2a0, &H::crt_cursor<fn::crt_strstr>);
B("TextCursor::80c_mb", 0x0058d2f0, &H::crt_cursor<fn::crt_mbsstr>);
B("TextCursor::80c_w", 0x0058d340, &H::crt_cursor<fn::crt_wcsstr>);
B("TextCursor::80c_strstr2", 0x0058d390, &H::crt_cursor<fn::crt_strstr>);
#undef B

}  // namespace game_b103_09
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x0058c6d0: the position (a TextCursor, one pointer) is copy-constructed (0x0058b9f0) into the argument slot.
    std::uintptr_t call_7f4(std::uintptr_t pos, std::uint32_t arg) {
        std::uintptr_t copy = 0;
        joan::orig::as_text_iter(&copy)->TextIter_copy(&pos);  // 0x0058b9f0
        return thiscall<std::uintptr_t>(global<std::uintptr_t>(d::fns_TextCursor_a157f4), this, copy, arg);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("TextCursor::call_7f4", 0x0058c6d0, &H::call_7f4);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (game/b84_01)
namespace {
namespace en3d_b84_01 {

static_assert(sizeof(Iter) == sizeof(void*));

JOAN_COUNTER(c_text_end, "TextBuffer::end_of");

struct H {
    // 0x0058cd40 (this = text buffer, *this = char buffer)
    Iter* text_end_of(Iter* ret, Iter it) {
        c_text_end.hit();
        const std::uint32_t n = joan::orig::arg::as<std::uint32_t>(joan::orig::as_text_cursor(&it)->call_a157e8());  // 0x0058c2e0
        Iter arg;
        joan::orig::as_text_iter(&arg)->TextIter_copy(&it);  // 0x0058b9f0
        joan::orig::as_text_cursor(this)->copy_dispatch(arg);         // 0x0058c400
        Iter tmp;
        Iter* const r = joan::orig::arg::as<Iter*>(joan::orig::as_text_cursor(&tmp)->construct(joan::orig::arg::str(jo::self<jo::TextBuffer>(this)->p + n)));  // 0x0058b9c0
        joan::orig::as_text_iter(ret)->TextIter_copy(joan::orig::arg::mptr(r));                                        // 0x0058b9f0
        return ret;
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b84_01", name, addr, fn)
GAME("TextBuffer::end_of", 0x0058cd40, &H::text_end_of);
#undef GAME

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

JOAN_COUNTER(c_find_if, "List::find_if");

struct H {
    // ------------------------------------------------------------------ 0x0058df70
    std::uintptr_t* find_if(std::uintptr_t* out, std::uintptr_t pred) {  // TextCursors (one pointer each)
        c_find_if.hit();
        std::uintptr_t it;
        joan::orig::as_text_iter(&it)->TextIter_copy(joan::orig::arg::mptr(this));  // 0x0058b9f0
        while (joan::orig::arg::as<u32>(joan::orig::as_text_cursor(&it)->get()) != 0) {  // 0x0058ba70
            const u32 cur = joan::orig::arg::as<u32>(joan::orig::as_text_cursor(&it)->get());  // 0x0058ba70
            std::uintptr_t tmp;
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_text_cursor(&pred)->find(&tmp, joan::orig::arg::u32(cur)));  // 0x0058ceb0
            if (static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_text_iter(r)->valid())) != 0) {     // 0x0058ce80
                joan::orig::as_text_iter(out)->TextIter_copy(&it);                        // 0x0058b9f0
                return out;
            }
            joan::orig::as_text_cursor(&it)->next();  // 0x0058bbc0
        }
        joan::orig::as_text_cursor(out)->construct(joan::orig::arg::str(0));  // 0x0058b9c0
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("List::find_if", 0x0058df70, &H::find_if);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

JOAN_COUNTER(c_find_not, "TextIter::find_not");

struct H {
    // 0x0058e130
    std::uintptr_t* find_not(std::uintptr_t* out, std::uintptr_t key) {  // TextCursors (one pointer each)
        c_find_not.hit();
        std::uintptr_t it;
        joan::orig::as_text_iter(&it)->TextIter_copy(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0058b9f0
        while (joan::orig::arg::as<std::uint32_t>(joan::orig::as_text_cursor(&it)->get()) != 0) {  // 0x0058ba70
            const std::uint32_t v = joan::orig::arg::as<std::uint32_t>(joan::orig::as_text_cursor(&it)->get());  // 0x0058ba70
            std::uintptr_t tmp;
            void* r = joan::orig::arg::as<void*>(joan::orig::as_text_cursor(&key)->find(joan::orig::arg::mptr(static_cast<void*>(&tmp)), joan::orig::arg::u32(v)));  // 0x0058ceb0
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_text_iter(r)->valid()) == 0) {  // 0x0058ce80
                joan::orig::as_text_iter(out)->TextIter_copy(joan::orig::arg::mptr(static_cast<void*>(&it)));  // 0x0058b9f0
                return out;
            }
            joan::orig::as_text_cursor(&it)->next();  // 0x0058bbc0
        }
        joan::orig::as_text_cursor(out)->construct(joan::orig::arg::str(std::uint32_t{0}));  // 0x0058b9c0
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("TextIter::find_not", 0x0058e130, &H::find_not);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

// 0x0058e360 / 0x0058e3b0
u32 JOAN_CDECL ptr_plus_20(u32 p) { return p + 0x20; }
u32 JOAN_CDECL ptr_minus_20(u32 p) { return p - 0x20; }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Mem::ptr_plus_20", 0x0058e360, &ptr_plus_20);
B("Mem::ptr_minus_20", 0x0058e3b0, &ptr_minus_20);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

struct H {
    // ------------------------------------------------------------------ 0x0058e400..0x0058e5a0
    std::uintptr_t lwr_a() { return joan::orig::arg::as<u32>(joan::orig::libc::_strlwr(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Str>(this)->p)))); }
    std::uintptr_t lwr_b() { return joan::orig::arg::as<u32>(joan::orig::libc::_mbslwr(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Str>(this)->p)))); }
    std::uintptr_t lwr_c() { return joan::orig::arg::as<u32>(joan::orig::libc::_wcslwr(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Str>(this)->p)))); }
    std::uintptr_t upr_a() { return joan::orig::arg::as<u32>(joan::orig::libc::_strupr(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Str>(this)->p)))); }
    std::uintptr_t upr_b() { return joan::orig::arg::as<u32>(joan::orig::libc::_mbsupr(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Str>(this)->p)))); }
    std::uintptr_t upr_c() { return joan::orig::arg::as<u32>(joan::orig::libc::_wcsupr(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Str>(this)->p)))); }

    // ------------------------------------------------------------------ 0x0058e510 (dead code)
    u32 upr() { return thiscall<u32>(global<std::uintptr_t>(d::g_a15838), this); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Str::lwr_a", 0x0058e400, &H::lwr_a);
B("Str::lwr_b", 0x0058e430, &H::lwr_b);
B("Str::lwr_c", 0x0058e460, &H::lwr_c);
B("Str::upr", 0x0058e510, &H::upr);
B("Str::upr_a", 0x0058e540, &H::upr_a);
B("Str::upr_b", 0x0058e570, &H::upr_b);
B("Str::upr_c", 0x0058e5a0, &H::upr_c);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::hook::global;

using joan::call::thiscall;

using joan::hook::global;

using joan::call::thiscall;
struct H {
    // 0x0058b990
    H* ptr_ctor() {
        jo::self<jo::StrIt>(this)->p = nullptr;
        return this;
    }

    // 0x0058bbf0
    void* cursor_postinc(void* out, std::int32_t) {
        std::uintptr_t tmp;  // a TextCursor (one pointer)
        joan::orig::as_text_iter(&tmp)->TextIter_copy(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0058b9f0 copy
        thiscall(global<std::uintptr_t>(d::fns_StrIt_a157dc), this);          // next
        joan::orig::as_text_iter(out)->TextIter_copy(joan::orig::arg::mptr(static_cast<void*>(&tmp)));
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Ptr::Ptr", 0x0058b990, &H::ptr_ctor);
B("TextCursor::postinc", 0x0058bbf0, &H::cursor_postinc);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::code_page::setup(const char* a0, std::uint32_t a1, std::uint32_t a2, const char* a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b103_08::codepage_setup>(a0, a1, a2, a3)); }  // 0x58b2d0
auto joan::orig::TextCursor::construct_null() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::ptr_ctor>(this)); }  // 0x58b990
auto joan::orig::TextCursor::construct(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_strings1::H::cursor_init>(this, a0)); }  // 0x58b9c0
auto joan::orig::TextIter::TextIter_copy(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08::H::iter_copy>(this, a0)); }  // 0x58b9f0
auto joan::orig::TextIter::op_assign(char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08::H::iter_set>(this, a0)); }  // 0x58ba20
auto joan::orig::TextIter::get() -> char* { return joan::bind::conv<char*>(joan::bind::member<&::en3d_b83_08::H::iter_get>(this)); }  // 0x58ba50
auto joan::orig::TextCursor::get() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_strings1::H::cursor_char>(this)); }  // 0x58ba70
auto joan::orig::TextCursor::next() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_strings1::H::cursor_next>(this)); }  // 0x58bbc0
auto joan::orig::TextCursor::postinc(void* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::cursor_postinc>(this, a0, a1)); }  // 0x58bbf0
auto joan::orig::TextCursor::put(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_04::H::cursor_put>(this, a0, a1)); }  // 0x58bd30
auto joan::orig::TextIter::less(char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_05::H::below>(this, a0)); }  // 0x58bfe0
auto joan::orig::TextCursor::advanced(void* a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::advanced>(this, a0, a1)); }  // 0x58c120
auto joan::orig::TextCursor::length() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_strings1::H::cursor_length>(this)); }  // 0x58c190
auto joan::orig::TextCursor::call_a157e8() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::text_fn_e8>(this)); }  // 0x58c2e0
auto joan::orig::TextIter::valid() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::en3d_b83_08::H::iter_valid>(this)); }  // 0x58ce80
auto joan::orig::TextCursor::find(void* a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_04::H::cursor_find>(this, a0, a1)); }  // 0x58ceb0
auto joan::orig::to_lower(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b91_03::to_lower>(a0)); }  // 0x58e330
auto joan::orig::text_toupper(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_strings1::text_toupper>(a0)); }  // 0x58e380
