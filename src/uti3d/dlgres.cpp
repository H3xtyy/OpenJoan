// uti3d/dlgres.cpp: functions of the original d:\ww\uti3d\dlgres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::call::cdecl_call;

using i8 = std::int8_t;
using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b105_18.cpp (game/b105_18)
//
//    - DlgRes::init 0x00637ec0 (arg), ret 4: optional 0x00637f60 (byte [this]), alloc 0x12c0
//      bytes (0x0058e7b0 on 0xa48838, "dlgres.cpp", line = word 0xa197dc + 4) -> +3, +7 = +0xb =
//      0, optional 0x006381a0(arg), byte [this] = 1. eax = this, or 0x006381a0's eax.
namespace {
namespace game_b105_18 {


JOAN_COUNTER(c_dlg_init, "DlgRes::init");

struct H {
    // ------------------------------------------------------------------------ 0x00637ec0
    std::uintptr_t dlg_init(const char* arg) {
        c_dlg_init.hit();
        if (jo::self<jo::DlgRes>(this)->loaded != 0) joan::orig::as_dlg_res(this)->release();  // 0x00637f60
        const i32 line = static_cast<i32>(global<i16>(d::g_a197dc)) + 4;
        jo::self<jo::DlgRes>(this)->entries = joan::orig::arg::as<jo::DlgResEntry*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(300 * sizeof(jo::DlgResEntry)), str(d::s_d_ww_uti3d_dlgres_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        jo::self<jo::DlgRes>(this)->data = nullptr;
        jo::self<jo::DlgRes>(this)->data_size = 0;
        std::uintptr_t eax = reinterpret_cast<std::uintptr_t>(this);
        if (arg != nullptr) eax = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dlg_res(this)->load(arg));  // 0x006381a0
        jo::self<jo::DlgRes>(this)->loaded = 1;
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_18", name, addr, fn)
B("DlgRes::init", 0x00637ec0, &H::dlg_init);
#undef B

}  // namespace game_b105_18
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00637f60 DlgRes::release()                             thiscall
namespace {
namespace game_b92_05 {


void* const kMem = obj(d::g_mem);

// Memory manager free / realloc with source file and line (0x0058ed70 / 0x0058eb30).
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}
using joan::orig::helpers::line_base;

JOAN_COUNTER(c_dlgres_release, "DlgRes::release");

struct H {
    // 0x00637f60
    void dlgres_release() {
        c_dlgres_release.hit();
        if (jo::self<jo::DlgRes>(this)->loaded == 0) return;
        for (std::int32_t i = jo::self<jo::DlgRes>(this)->count_; i > 0; --i) {
            const std::uintptr_t p = reinterpret_cast<std::uintptr_t>(jo::self<jo::DlgRes>(this)->entries[i - 1].text);
            if (jo::self<jo::DlgRes>(this)->data != nullptr && !(p < reinterpret_cast<std::uintptr_t>(jo::self<jo::DlgRes>(this)->data)) && !(p > reinterpret_cast<std::uintptr_t>(jo::self<jo::DlgRes>(this)->data) + jo::self<jo::DlgRes>(this)->data_size)) continue;
            mem_free(reinterpret_cast<void*>(p), d::s_d_ww_uti3d_dlgres_cpp, line_base(d::g_a197e0) + 8);  // dlgres.cpp
        }
        if (jo::self<jo::DlgRes>(this)->entries != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::DlgRes>(this)->entries), d::s_d_ww_uti3d_dlgres_cpp, line_base(d::g_a197e0) + 0xd);
            jo::self<jo::DlgRes>(this)->entries = nullptr;
        }
        if (jo::self<jo::DlgRes>(this)->data != nullptr) {
            mem_free(jo::self<jo::DlgRes>(this)->data, d::s_d_ww_uti3d_dlgres_cpp, line_base(d::g_a197e0) + 0x13);
            jo::self<jo::DlgRes>(this)->data = nullptr;
        }
        jo::self<jo::DlgRes>(this)->loaded = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("DlgRes::release", 0x00637f60, &H::dlgres_release);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
//
//      0x006380b0 Table::index_of_ref   (thiscall(str), ret 4) - "[n" lookup, returns ax (the
//                 upper half of eax is replicated: arg / atoi / this, as in the original).
namespace {
namespace game_b93_04 {


JOAN_COUNTER(c_index_of_ref, "Table::index_of_ref");

i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }  // atoi

struct H {
    // ------------------------------------------------------------------ 0x006380b0
    u32 index_of_ref(const char* s) {
        c_index_of_ref.hit();
        if (static_cast<i32>(static_cast<i8>(s[0])) != 0x5b) return joan::call::register_low_bits(s) & 0xffff0000u;  // every caller reads only the low 16 bits (as<i16>)
        for (i32 i = 1;; ++i) {
            if (i > static_cast<i32>(jo::self<jo::DlgRes>(this)->count_)) break;
            const u16* const e = joan::orig::arg::as<const u16*>(joan::orig::as_dlg_res(this)->entry(joan::orig::arg::i32(i)));  // 0x00638150
            const i32 v = static_cast<i32>(*e);
            const i32 n = atoi_(s + 1);
            if (v == n) return (static_cast<u32>(n) & 0xffff0000u) | (static_cast<u32>(i) & 0xffffu);
        }
        return joan::call::register_low_bits(this) & 0xffff0000u;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("Table::index_of_ref", 0x006380b0, &H::index_of_ref);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03 {


struct H {
    // 0x00638150 1-based 16-byte entry.
    jo::DlgResEntry* entry16(std::int32_t i) { return &jo::self<jo::DlgRes>(this)->entries[i - 1]; }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Table16::entry", 0x00638150, &H::entry16);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
//
//    - DlgRes::count 0x00638180: word +1 (callers movsx ax).
//
//    - DlgRes::load 0x006381a0 (name), ret 4: reads the dialog text file (dlgres.cpp lines
//      +0x12 / +0x14 / +0x9e), parses the '~' separated entries (16-byte records at +3).
namespace {
namespace game_b104_11 {


void* const kMemory = obj(d::g_mem);

struct alignas(alignof(void*)) File {  // a File (0x004179c0): 0x110 in the x86 frames, sizeof(File) = 0x112 on x64
    u8 bytes[((sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

void* mem_alloc(u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}
void* mem_realloc(void* p, u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}

JOAN_COUNTER(c_dlg_load, "DlgRes::load");

struct H {
    i16 count() { return jo::self<jo::DlgRes>(this)->count_; }

    // ------------------------------------------------------------------ 0x006381a0
    i32 next_line(const char* p, i32 n) { return joan::orig::arg::as<i32>(joan::orig::as_dlg_res(this)->line_length(joan::orig::arg::str(p), joan::orig::arg::i32(n))); }  // 0x00638820
    static bool is_sep(char ch) { return ch == 0xd || ch == 0xa || ch == 0x7e; }

    u32 dlg_load(const char* name) {
        c_dlg_load.hit();
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(name), 1, static_cast<char>(0));  // 0x00586da0 open
        const i32 size = joan::orig::as_file(&file)->file_size();   // 0x00587f30
        const char* const src = str(d::s_d_ww_uti3d_dlgres_cpp);  // dlgres.cpp
        if (jo::self<jo::DlgRes>(this)->data != nullptr) {
            jo::self<jo::DlgRes>(this)->data = mem_realloc(jo::self<jo::DlgRes>(this)->data, static_cast<u32>(size + 1), src,
                                      static_cast<i32>(global<i16>(d::g_a197e4)) + 0x12);
        } else {
            jo::self<jo::DlgRes>(this)->data = mem_alloc(static_cast<u32>(size + 1), src, static_cast<i32>(global<i16>(d::g_a197e4)) + 0x14);
        }
        jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size) = size;
        joan::orig::as_file(&file)->file_read(joan::orig::arg::mptr(jo::self<jo::DlgRes>(this)->data), joan::orig::arg::u32(jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size)));  // 0x005873a0
        joan::orig::as_file(&file)->file_close();                        // 0x00587260 close
        jo::DlgResEntry* e = jo::self<jo::DlgRes>(this)->entries;
        char* p = jo::view<char*>(jo::self<jo::DlgRes>(this)->data);
        i32 n = 0;
        i32 pos = 0;
        jo::self<jo::DlgRes>(this)->count_ = 0;
        while (pos < jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size)) {
            do {
                n = next_line(p, jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size) - pos);
                jo::view<i16>(e->id) = static_cast<i16>(joan::crt::atoi(joan::orig::arg::str(p + 1)));  // atoi
                const char ch = p[n - 3];
                e->kind = (ch == 'M' || ch == 'm') ? 1 : 0;
                pos += n;
                p += n;
            } while (n <= 1);
            while (is_sep(*p)) {
                ++p;
                ++pos;
            }
            e->text = p;
            i32 len = 0;
            while (*p != '~' && *p != 0) {
                ++p;
                ++len;
                ++pos;
            }
            e->len = static_cast<i16>(len);
            do {
                n = next_line(p, jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size) - pos);
                pos += n;
                p += n;
            } while (pos < jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size) && n <= 1);
            e->text2 = nullptr;
            e->len2 = 0;
            if (jo::view<i8>(e->kind) == 1) {
                pos -= n;
                p -= n;
                while (is_sep(*p)) {
                    ++p;
                    ++pos;
                }
                e->text2 = p;
                i32 len2 = 0;
                while (*p != '~' && *p != 0) {
                    ++p;
                    ++len2;
                    ++pos;
                }
                e->len2 = static_cast<i16>(len2);
                do {
                    n = next_line(p, jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size) - pos);
                    pos += n;
                    p += n;
                } while (pos < jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size) && n <= 1);
            }
            jo::self<jo::DlgRes>(this)->count_ = static_cast<i16>(jo::self<jo::DlgRes>(this)->count_ + 1);
            if (pos >= jo::view<i32>(jo::self<jo::DlgRes>(this)->data_size)) break;
            pos -= n;
            p -= n;
            ++e;
        }
        jo::view<void*>(jo::self<jo::DlgRes>(this)->entries) = mem_realloc(jo::view<void*>(jo::self<jo::DlgRes>(this)->entries), static_cast<u32>(static_cast<i32>(jo::self<jo::DlgRes>(this)->count_)) * static_cast<u32>(sizeof(jo::DlgResEntry)), src,
                                  static_cast<i32>(global<i16>(d::g_a197e4)) + 0x9e);
        joan::orig::as_file(&file)->destruct_5872b0();  // 0x005872b0 ~File
        return 0;  // callers (DlgRes::init, whose callers discard it) do not use the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("DlgRes::count", 0x00638180, &H::count);
B("DlgRes::load", 0x006381a0, &H::dlg_load);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - text_span 0x00638820 (s, n), ret 8.
namespace {
namespace game_b97_06 {

JOAN_COUNTER(c_text_span, "text_span");

struct H {
    // 0x00638820
    std::int32_t text_span(const char* s, std::int32_t n) {
        c_text_span.hit();
        std::int32_t i = 0;
        for (; i < n; ++i) {
            const char c = s[i];
            if (c == 0 || c == 0xd || c == 0xa || c == 0x7e) break;
        }
        return i + 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("text_span", 0x00638820, &H::text_span);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b107_11.cpp (game/b107_11)
//
//    - DlgRes::add_string_6388b0 0x006388b0 (const char* "[xxxxx]text"), ret 4 (uti3d/dlgres.cpp;
//      no references at all: dead code): appends a 16-byte string record unless an equal one
//      exists.
//
//  - 0x006388b0: eax reproduced on every path (s itself when s[0] != '[', movsx s[6] when
//    s[6] != ']', strncmp's 0 on a duplicate, memcpy's dest after appending).
namespace {
namespace game_b107_11 {


void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_add_string, "DlgRes::add_string_6388b0");

struct H {
    // ------------------------------------------------------------------ 0x006388b0
    std::intptr_t add_string_6388b0(const char* s) {
        c_add_string.hit();
        if (static_cast<i32>(static_cast<i8>(s[0])) != 0x5b)
            return reinterpret_cast<std::intptr_t>(s);  // eax still holds s
        if (static_cast<i32>(static_cast<i8>(s[6])) != 0x5d) return static_cast<i8>(s[6]);
        if (jo::self<jo::DlgRes>(this)->loaded == 0) joan::orig::as_dlg_res(this)->init(nullptr);  // 0x00637ec0
        for (i32 i = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_dlg_res(this)->count())); i > 0; --i) {  // 0x00638180
            void* e = joan::orig::arg::as<void*>(joan::orig::as_dlg_res(this)->entry(joan::orig::arg::i32(i)));                               // 0x00638150
            const i32 r = joan::orig::arg::as<i32>(joan::orig::libc::strncmp(joan::orig::arg::str(s + 7), joan::orig::arg::str(jo::as_dlg_res_entry(e)->text), joan::orig::arg::u32(static_cast<i32>(jo::as_dlg_res_entry(e)->len))));  // strncmp
            if (r == 0) return r;
        }
        const char* const file = str(d::s_d_ww_uti3d_dlgres_cpp);  // "d:\\ww\\uti3d\\dlgres.cpp"
        jo::view<void*>(jo::self<jo::DlgRes>(this)->entries) = joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::DlgRes>(this)->entries)), joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(jo::self<jo::DlgRes>(this)->count_) + 1) * sizeof(jo::DlgResEntry)), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a197e8)) + 0x10));  // 0x0058eb30
        jo::DlgResEntry* e = jo::self<jo::DlgRes>(this)->entries + static_cast<i32>(jo::self<jo::DlgRes>(this)->count_);
        jo::view<u16>(jo::self<jo::DlgRes>(this)->count_) = static_cast<u16>(jo::view<u16>(jo::self<jo::DlgRes>(this)->count_) + 1);
        std::memset(static_cast<void*>(e), 0, sizeof(jo::DlgResEntry));    // memset (0x10 on x86)
        e->id = jo::view<u16>(jo::self<jo::DlgRes>(this)->count_);
        e->kind = static_cast<u8>(s[5]);
        const i32 len = joan::orig::arg::as<i32>(std::strlen(joan::orig::arg::str(s))) - 7;  // strlen
        jo::view<void*>(e->text) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(len + 1), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a197e8)) + 0x1b));  // 0x0058e7b0
        jo::view<u16>(e->len) = static_cast<u16>(len);
        return joan::orig::arg::as<std::intptr_t>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::view<void*>(e->text))), joan::orig::arg::str(s + 7), joan::orig::arg::u32(len + 1)));  // memcpy
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_11", name, addr, fn)
B("DlgRes::add_string_6388b0", 0x006388b0, &H::add_string_6388b0);
#undef B

}  // namespace game_b107_11
}  // namespace

// ---------------------------------------- from modules/game_b106_11.cpp (game/b106_11)
//
//    - Widget::load_ex_res 0x0063f580 (thiscall, (const char* dir), ret 4, void - all 12
//      call sites overwrite eax): deletes +0x64 (0x0063f4e0, 1; the field is not cleared),
//      path dir + "EX.res"; when 0x00590b80 finds it: +0x64 = new 0xf-byte 0x00637e90,
//      0x00637ec0(path); the name +0x20 up to its '.' (strchr / strncpy into a 0x104
//      buffer, NUL) -> 0x00639560 on 0xa5cd6a.
//
//  - 0x0063f580: the SEH frame only guards `new` + ctor; normal path only. The 0x194-byte
//    path object and the 0x104-byte name buffer keep the original's sizes.
namespace {
namespace game_b106_11 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_ex_res, "Widget::load_ex_res");

struct H {
    // ------------------------------------------------------------------ 0x0063f580
    void load_ex_res(const char* dir) {
        c_ex_res.hit();
        if (jo::self<jo::Script>(this)->strings != nullptr) {
            void* const old = jo::view<void*>(jo::self<jo::Script>(this)->strings);
            if (old != nullptr) joan::orig::as_widget(old)->scalar_dtor_63f4e0(joan::orig::arg::u32(i32{1}));  // 0x0063f4e0 scalar delete
        }
        alignas(4) char path[0x194];
        joan::orig::as_string(path)->construct();               // 0x005a1170
        joan::orig::as_string(path)->assign(joan::orig::arg::str(dir));     // 0x005a1440
        joan::orig::as_string(path)->append(str(d::s_EX_res));                   // 0x005a1550 "EX.res"
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(name)) == 0) return;  // 0x00590b80
        void* res = nullptr;
        if (void* mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::DlgRes))))  // operator new (0xf on x86)
            res = joan::orig::as_dlg_res(mem)->construct();               // 0x00637e90
        jo::view<void*>(jo::self<jo::Script>(this)->strings) = res;
        void* const name2 = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_dlg_res(jo::view<void*>(jo::self<jo::Script>(this)->strings))->init(joan::orig::arg::str(name2));       // 0x00637ec0
        char* const base = jo::self<jo::Script>(this)->name_;
        char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(base), i32{0x2e}));  // strchr '.'
        const i32 len = static_cast<i32>(reinterpret_cast<std::uintptr_t>(dot) - reinterpret_cast<std::uintptr_t>(base));
        char buf[0x104];
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(static_cast<const char*>(base)), joan::orig::arg::u32(static_cast<u32>(len)));  // strncpy
        buf[len] = 0;
        joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->load(joan::orig::arg::str(buf));  // 0x00639560
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_11", name, addr, fn)
B("Widget::load_ex_res", 0x0063f580, &H::load_ex_res);
#undef B

}  // namespace game_b106_11
}  // namespace

// ---------------------------------------- from modules/game_b99_08.cpp (game/b99_08)
//
//      0x00643520 Obj::call_637f60             (thiscall, ret 0)
namespace {
namespace game_b99_08 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00643520
    u32 call_637f60() { joan::orig::as_dlg_res(this)->release(); return 0; }  // 0x00637f60 (result unused: a destructor)
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("Obj::call_637f60", 0x00643520, &H::call_637f60);
#undef B99

}  // namespace game_b99_08
}  // namespace

// ---------------------------------------- from modules/game_b99_08_2.cpp (game/b99_08)
namespace {
namespace game_b99_08_2 {

struct H {
    // ------------------------------------------------------------------ 0x00637e90
    void* clear15() {
        std::memset(this, 0, sizeof(jo::DlgRes));  // 0x007b1690 memset (0xf on x86)
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("Obj::clear15", 0x00637e90, &H::clear15);
#undef B99

}  // namespace game_b99_08_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DlgRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_08_2::H::clear15>(this)); }  // 0x637e90
auto joan::orig::DlgRes::init(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_18::H::dlg_init>(this, a0)); }  // 0x637ec0
auto joan::orig::DlgRes::release() -> void { joan::bind::member<&::game_b92_05::H::dlgres_release>(this); }  // 0x637f60
auto joan::orig::Table::index_of_ref(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b93_04::H::index_of_ref>(this, a0)); }  // 0x6380b0
auto joan::orig::DlgRes::entry(std::int32_t a0) -> DlgResEntry* { return joan::bind::conv<DlgResEntry*>(joan::bind::member<&::game_b91_03::H::entry16>(this, a0)); }  // 0x638150
auto joan::orig::DlgRes::count() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b104_11::H::count>(this)); }  // 0x638180
auto joan::orig::DlgRes::load(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11::H::dlg_load>(this, a0)); }  // 0x6381a0
auto joan::orig::DlgRes::line_length(const char* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_06::H::text_span>(this, a0, a1)); }  // 0x638820
auto joan::orig::Widget::load_ex_res(const char* a0) -> void { joan::bind::member<&::game_b106_11::H::load_ex_res>(this, a0); }  // 0x63f580
auto joan::orig::DlgRes::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_08::H::call_637f60>(this)); }  // 0x643520
