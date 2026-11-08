// game/help.cpp: functions of the original d:\ww\help.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
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

void* const kHeap = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
//
//    - Help::load_488400 0x00488400 (name), ret 4 (only caller 0x004d7681): once (byte [this]):
//      +0x152 = alloc(0x8340), builds "RESOURCE\" + name in a 0x191-byte path buffer
//      (0x005a1170 / 0x005a1440 / 0x005a1550 / 0x00417d90), 0x00488590(path), byte [this] = 1.
//      Returns this on both paths.
namespace {
namespace game_b105_04 {

const char* const kHelp = str(d::s_d_ww_help_cpp);    // "d:\ww\help.cpp"

JOAN_COUNTER(c_help_load, "Help::load_488400");

struct alignas(4) PathBuf {  // 0x005a1440 / 0x005a1550 write the NUL at +0x190
    char s[0x194];
};

struct H {
    // ---------------------------------------------------------------- 0x00488400
    H* load_488400(const char* name) {
        c_help_load.hit();
        if (jo::self<jo::Help>(this)->loaded != 0) return this;
        {
            const i32 line = static_cast<i32>(global<i16>(d::g_a001c8)) + 4;
            jo::self<jo::Help>(this)->entries = joan::orig::arg::as<jo::HelpEntry*>(joan::orig::as_mem(kHeap)->add(300u * static_cast<u32>(sizeof(jo::HelpEntry)), joan::orig::arg::str(kHelp), joan::orig::arg::i32(line)));  // 0x8340: 300 entries of 0x70 on x86  // 0x0058e7b0
        }
        PathBuf path;
        joan::orig::as_string(&path)->construct();              // 0x005a1170
        joan::orig::as_string(&path)->assign(str(d::s_RESOURCE));            // 0x005a1440 "RESOURCE\"
        joan::orig::as_string(&path)->append(joan::orig::arg::str(name));      // 0x005a1550
        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        joan::orig::as_help_box(this)->parse(joan::orig::arg::str(s));                              // 0x00488590
        jo::self<jo::Help>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
B("Help::load_488400", 0x00488400, &H::load_488400);
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x004884c0 Help::clear()              frees +0x152 / +0x156 ("d:\\ww\\help.cpp")
namespace {
namespace game_b92_02 {

struct H {
    // 0x004884c0
    void help_clear() {
        if (jo::self<jo::Help>(this)->loaded == 0) return;
        if (jo::self<jo::Help>(this)->entries != nullptr) {
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a001cc)) + 6;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::Help>(this)->entries), str(d::s_d_ww_help_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::Help>(this)->entries = nullptr;
        }
        if (jo::self<jo::Help>(this)->text_buf != nullptr) {
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a001cc)) + 0xc;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::Help>(this)->text_buf), str(d::s_d_ww_help_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::Help>(this)->text_buf = nullptr;
        }
        jo::self<jo::Help>(this)->loaded = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Help::clear", 0x004884c0, &H::help_clear);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
//
//    - Help::load_488590 0x00488590 (name), ret 4: parses the help text file into 0x70-byte
//      entries. Caller (0x0048847d) ignores eax.
//
//  - 0x00488590: 0x00589ee0 returns atof's st0 (loaded from a qword, so exact as double).
namespace {
namespace game_b104_02 {

using joan::hook::x87::ftol;
using u16 = std::uint16_t;
using i8 = std::int8_t;

using joan::call::cdecl_call;
struct alignas(4) Reader {  // 0x00589550 text reader, [ebp-0x1ac] .. [ebp-0x10]
    u8 b[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger FileTxt
};

JOAN_COUNTER(c_help_load, "Help::load_488590");

struct H {
    // ------------------------------------------------------------------ 0x00488590
    void help_load_488590(const char* name) {
        c_help_load.hit();
        Reader rd;                                      // [ebp-0x1ac]
        joan::orig::as_file_txt(&rd)->construct(joan::orig::arg::str(name));         // 0x00589550
        const i32 size = joan::orig::arg::as<i32>(joan::orig::as_file_txt(&rd)->data_size_489e70());  // 0x00489e70
        if (size > jo::self<jo::Help>(this)->text_buf_size) {
            jo::view<void*>(jo::self<jo::Help>(this)->text_buf) = joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Help>(this)->text_buf)), joan::orig::arg::u32(size), str(d::s_d_ww_help_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a001d0)) + 0xa));  // 0x0058eb30
            jo::self<jo::Help>(this)->text_buf_size = size;
        }
        u8* entry = jo::view<u8*>(jo::self<jo::Help>(this)->entries);   // [ebp-0x1b4]
        u8* text = jo::view<u8*>(jo::self<jo::Help>(this)->text_buf);    // [ebp-0x1b8]
        i32 used = 0;                // [ebp-0x1c0]
        jo::view<u16>(jo::self<jo::Help>(this)->entry_count) = 0;
        jo::view<u16>(jo::self<jo::Help>(this)->num_first) = 0xffff;
        jo::view<u16>(jo::self<jo::Help>(this)->name_first) = 0xffff;
        jo::view<u16>(jo::self<jo::Help>(this)->num_end) = 0;
        jo::view<u16>(jo::self<jo::Help>(this)->name_end) = 0;
        while (joan::orig::arg::as<i32>(joan::orig::as_file_txt(&rd)->at_eof()) == 0) {  // 0x0046f060
            const char* const line = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(&rd)->get_token(0));  // 0x00589c70
            if (line == nullptr) break;
            entry[0] = 0;
            if (joan::orig::arg::as<i32>(joan::orig::libc::isalpha(static_cast<i32>(static_cast<i8>(line[0])))) != 0) {  // isalpha
                std::strncpy(static_cast<char*>(static_cast<void*>(entry)), joan::orig::arg::str(line), 8u);  // strncpy
                entry[8] = 0;
                if (jo::self<jo::Help>(this)->name_first == -1) jo::view<u16>(jo::self<jo::Help>(this)->name_first) = jo::view<u16>(jo::self<jo::Help>(this)->entry_count);
                jo::view<u16>(jo::self<jo::Help>(this)->name_end) = static_cast<u16>(static_cast<i32>(jo::self<jo::Help>(this)->entry_count) + 1);
            } else if (static_cast<i8>(line[0]) >= 0x30 && static_cast<i8>(line[0]) <= 0x39) {
                for (i32 k = 0; k < 18; ++k) {
                    const double v = joan::orig::arg::as<double>(joan::orig::as_file_txt(&rd)->get_num_589ee0());  // 0x00589ee0 (atof st0)
                    jo::as_help_entry(entry)->nums[k] = static_cast<u16>(ftol(v));
                }
                if (jo::self<jo::Help>(this)->num_first == -1) jo::view<u16>(jo::self<jo::Help>(this)->num_first) = jo::view<u16>(jo::self<jo::Help>(this)->entry_count);
                jo::view<u16>(jo::self<jo::Help>(this)->num_end) = static_cast<u16>(static_cast<i32>(jo::self<jo::Help>(this)->entry_count) + 1);
            }
            joan::orig::as_file_txt(&rd)->next_line();  // 0x005899b0
            if (joan::orig::arg::as<i32>(joan::orig::as_file_txt(&rd)->at_eof()) != 0) break;  // 0x0046f060
            joan::orig::as_file_txt(&rd)->read_line(jo::as_help_entry(entry)->title, 0x3c);  // +0x2d                         // 0x00589f10
            const i32 n = joan::orig::as_file_txt(&rd)->read_paragraph(static_cast<char*>(static_cast<void*>(text)), joan::orig::arg::i32(jo::self<jo::Help>(this)->text_buf_size - used));                         // 0x0058a060
            jo::view<u8*>(jo::as_help_entry(entry)->text) = text;
            jo::as_help_entry(entry)->text_len = static_cast<u16>(n);
            text += n;
            used += n;
            joan::orig::as_file_txt(&rd)->next_line();  // 0x005899b0
            jo::view<u16>(jo::self<jo::Help>(this)->entry_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Help>(this)->entry_count) + 1);
            entry += sizeof(jo::HelpEntry);  // 0x70 on x86
        }
        if (jo::self<jo::Help>(this)->name_first == -1) jo::view<u16>(jo::self<jo::Help>(this)->name_first) = 0;
        if (jo::self<jo::Help>(this)->num_first == -1) jo::view<u16>(jo::self<jo::Help>(this)->num_first) = 0;
        joan::orig::as_file_txt(&rd)->destruct();  // 0x00589890
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Help::load_488590", 0x00488590, &H::help_load_488590);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x004883d0 Obj::reset_4884c0       (thiscall, ret 0) - forwards to 0x004884c0 (eax passed).
//
//  - Functions whose eax result is just the last callee's eax (0x004883d0, 0x0047bc80,
//    0x004b2b10, 0x004b31c0, 0x004ba1f0) pass that value through.
namespace {
namespace game_b94_02 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004883d0
    u32 reset_4884c0() { joan::orig::as_help_box(this)->clear(); return 0; }  // 0x004884c0 (void; static dtor)
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::reset_4884c0", 0x004883d0, &H::reset_4884c0);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - Obj::field_196 0x00489e70 (ret 0).
namespace {
namespace game_b100_04 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00489e70
    u32 field_196() { return jo::view<u32>(jo::self<jo::FileTxt>(this)->data_size); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::field_196", 0x00489e70, &H::field_196);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::u;

void* crt_memset(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // ------------------------------------------------------------------ 0x00488390
    std::uintptr_t clear_160() {
        crt_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::Help)));  // 0x160 on x86
        return u(this);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::clear_160", 0x00488390, &H::clear_160);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

JOAN_COUNTER(c_extend, "Rect::extend");

// cdecl 0x004893c0
std::int32_t JOAN_CDECL int_max(std::int32_t a, std::int32_t b) { return a < b ? b : a; }

struct H {
    // ------------------------------------------------------------------ 0x00488ac0
    void extend(std::int32_t x, std::int32_t y) {
        c_extend.hit();
        if (jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->dirty_valid) == 0) {
            jo::self<jo::MouseCursor>(this)->dirty_x2 = x;
            jo::self<jo::MouseCursor>(this)->dirty_x1 = x;
            jo::self<jo::MouseCursor>(this)->dirty_last_x = x;
            jo::self<jo::MouseCursor>(this)->dirty_y2 = y;
            jo::self<jo::MouseCursor>(this)->dirty_y1 = y;
            jo::self<jo::MouseCursor>(this)->dirty_last_y = y;
            jo::view<std::int8_t>(jo::self<jo::MouseCursor>(this)->dirty_valid) = 1;
        } else if (x > jo::self<jo::MouseCursor>(this)->dirty_last_x) {
            if (y > jo::self<jo::MouseCursor>(this)->dirty_last_y) {
                jo::self<jo::MouseCursor>(this)->dirty_x1 = jo::self<jo::MouseCursor>(this)->dirty_last_x;
                jo::self<jo::MouseCursor>(this)->dirty_y1 = jo::self<jo::MouseCursor>(this)->dirty_last_y;
                jo::self<jo::MouseCursor>(this)->dirty_x2 = x;
                jo::self<jo::MouseCursor>(this)->dirty_y2 = y;
            } else {
                jo::self<jo::MouseCursor>(this)->dirty_x1 = jo::self<jo::MouseCursor>(this)->dirty_last_x;
                jo::self<jo::MouseCursor>(this)->dirty_y1 = y;
                jo::self<jo::MouseCursor>(this)->dirty_x2 = x;
                jo::self<jo::MouseCursor>(this)->dirty_y2 = jo::self<jo::MouseCursor>(this)->dirty_last_y;
            }
        } else if (y > jo::self<jo::MouseCursor>(this)->dirty_last_y) {
            jo::self<jo::MouseCursor>(this)->dirty_x1 = x;
            jo::self<jo::MouseCursor>(this)->dirty_y1 = jo::self<jo::MouseCursor>(this)->dirty_last_y;
            jo::self<jo::MouseCursor>(this)->dirty_x2 = jo::self<jo::MouseCursor>(this)->dirty_last_x;
            jo::self<jo::MouseCursor>(this)->dirty_y2 = y;
        } else {
            jo::self<jo::MouseCursor>(this)->dirty_x1 = x;
            jo::self<jo::MouseCursor>(this)->dirty_y1 = y;
            jo::self<jo::MouseCursor>(this)->dirty_x2 = jo::self<jo::MouseCursor>(this)->dirty_last_x;
            jo::self<jo::MouseCursor>(this)->dirty_y2 = jo::self<jo::MouseCursor>(this)->dirty_last_y;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("Rect::extend", 0x00488ac0, &H::extend);
B("int_max", 0x004893c0, &int_max);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b101_03.cpp (game/b101_03)
namespace {
namespace game_b101_03 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

void* const kMath = obj(d::g_misc);

JOAN_COUNTER(c_check_wait, "Obj::check_wait");

struct H {
    // ------------------------------------------------------------------ 0x00488c00
    u8 check_wait() {
        c_check_wait.hit();
        if (static_cast<i32>(global<i8>(d::g_a57fef)) == 0) return 0;
        bool timed;
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Help>(this)->label[0])) == 0 && static_cast<i32>(jo::view<i8>(jo::self<jo::Help>(this)->title[0])) == 0 &&
            static_cast<i32>(jo::self<jo::Help>(this)->box_cur[6]) == global<i32>(d::g_mouse_x) &&
            static_cast<i32>(jo::self<jo::Help>(this)->box_cur[7]) == global<i32>(d::g_mouse_y)) {
            timed = true;
        } else if (static_cast<i32>(jo::view<i8>(jo::self<jo::Help>(this)->label[0])) == 0 && static_cast<i32>(jo::view<i8>(jo::self<jo::Help>(this)->title[0])) == 0) {
            timed = false;
        } else {
            timed = jo::self<jo::Help>(this)->box_cur[0] == jo::self<jo::Help>(this)->box[2] && jo::self<jo::Help>(this)->box_cur[1] == jo::self<jo::Help>(this)->box[3] &&
                    jo::self<jo::Help>(this)->box_cur[2] == jo::self<jo::Help>(this)->box[4] && jo::self<jo::Help>(this)->box_cur[3] == jo::self<jo::Help>(this)->box[5] &&
                    jo::self<jo::Help>(this)->box_cur[4] == jo::self<jo::Help>(this)->box[6] && jo::self<jo::Help>(this)->box_cur[5] == jo::self<jo::Help>(this)->box[7] &&
                    global<u32>(d::g_mouse_left_down) == 0 && global<u32>(d::g_a516d4) == 0;
        }
        if (timed) {
            const u32 t = joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->get_time_591050());  // 0x00591050
            if (t >= jo::self<jo::Help>(this)->box_time + 0x320u && static_cast<i32>(global<i8>(d::g_a57fef)) == 2) {
                jo::view<u16>(jo::self<jo::Help>(this)->tooltip_style) = 2;
                return 1;
            }
            if (t >= jo::self<jo::Help>(this)->box_time + 0xc8u) {
                jo::view<u16>(jo::self<jo::Help>(this)->tooltip_style) = 1;
                return 1;
            }
        } else {
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[0]) = jo::view<u16>(jo::self<jo::Help>(this)->box[2]);
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[1]) = jo::view<u16>(jo::self<jo::Help>(this)->box[3]);
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[2]) = jo::view<u16>(jo::self<jo::Help>(this)->box[4]);
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[3]) = jo::view<u16>(jo::self<jo::Help>(this)->box[5]);
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[4]) = jo::view<u16>(jo::self<jo::Help>(this)->box[6]);
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[5]) = jo::view<u16>(jo::self<jo::Help>(this)->box[7]);
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[6]) = global<u16>(d::g_mouse_x);
            jo::view<u16>(jo::self<jo::Help>(this)->box_cur[7]) = global<u16>(d::g_mouse_y);
            jo::self<jo::Help>(this)->box_time = joan::orig::arg::as<u32>(joan::orig::as_misc(kMath)->get_time_591050());  // 0x00591050
        }
        jo::view<u16>(jo::self<jo::Help>(this)->tooltip_style) = 0;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_03", name, addr, fn)
B("Obj::check_wait", 0x00488c00, &H::check_wait);
#undef B

}  // namespace game_b101_03
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::call::cdecl_call;
using joan::img::str;

JOAN_COUNTER(c_menu_488f00, "Menu::update_488f00");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x00488f00
    void menu_488f00(u32 flags_arg) {
        c_menu_488f00.hit();
        const u32 flags = flags_arg & 0xff;
        auto finish = [&]() {
            if ((flags & 4) != 0) {
                jo::view<u8>(jo::self<jo::Help>(this)->label[0]) = 0;
                jo::view<u8>(jo::self<jo::Help>(this)->title[0]) = 0;
            }
        };
        if (joan::orig::arg::as<u8>(joan::orig::as_help_box(me())->check_wait()) == 0) {  // 0x00488c00
            finish();
            return;
        }
        void* const hover = obj(d::g_mouse);
        u8* rec = nullptr;
        i32 i = 0;
        if (jo::view<i8>(jo::self<jo::Help>(this)->label[0]) != 0) {
            i = jo::self<jo::Help>(this)->name_first;
            rec = reinterpret_cast<u8*>(jo::self<jo::Help>(this)->entries + i);
            for (; i < jo::self<jo::Help>(this)->name_end; ++i, rec += sizeof(jo::HelpEntry)) {
                if (static_cast<i8>(*rec) == jo::view<i8>(jo::self<jo::Help>(this)->label[0]) &&
                    std::strcmp(joan::orig::arg::str(rec), joan::orig::arg::str(jo::self<jo::Help>(this)->label)) == 0)  // strcmp
                    break;
            }
            if (i >= jo::self<jo::Help>(this)->name_end) rec = nullptr;
        } else if (jo::view<i8>(jo::self<jo::Help>(this)->title[0]) == 0) {
            // [0xa48858] != 0: the loop is skipped and the (uninitialised) index compare can only
            // leave the record pointer at 0 - same result as here.
            if (global<u32>(d::g_a48858) == 0) {
                i = jo::self<jo::Help>(this)->num_first;
                rec = reinterpret_cast<u8*>(jo::self<jo::Help>(this)->entries + i);
                for (; i < jo::self<jo::Help>(this)->num_end; ++i, rec += sizeof(jo::HelpEntry)) {
                    if (static_cast<i8>(*rec) == 0 && jo::view<i16>(jo::as_help_entry(rec)->nums[16]) == -1 &&
                        joan::orig::arg::as<i32>(joan::orig::as_point(hover)->inside(static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[12])), static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[13])), static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[14])), static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[15])))) != 0)  // 0x00593980
                        break;
                    if (joan::orig::arg::as<i32>(joan::orig::as_input(hover)->cursor_in_triangle(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[12]))), joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[13]))), joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[14]))), joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[15]))), joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[16]))), joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_help_entry(rec)->nums[17]))))) != 0)  // 0x005939f0
                        break;
                }
                if (i >= jo::self<jo::Help>(this)->num_end) rec = nullptr;
            } else {
                rec = nullptr;
            }
        }
        const u8 fl = static_cast<u8>(flags);
        if (jo::view<i8>(jo::self<jo::Help>(this)->label[0]) != 0 && rec != nullptr) {
            joan::orig::as_help_box(me())->draw_tooltip(joan::orig::arg::i32(global<u32>(d::g_mouse_x)), joan::orig::arg::i32(global<u32>(d::g_mouse_y)), joan::orig::arg::str(jo::as_help_entry(rec)->title), joan::orig::arg::str(jo::as_help_entry(rec)->text), joan::orig::arg::u32(fl));  // 0x00489400
        } else if (jo::view<i8>(jo::self<jo::Help>(this)->title[0]) != 0) {
            joan::orig::as_help_box(me())->draw_tooltip(joan::orig::arg::i32(global<u32>(d::g_mouse_x)), joan::orig::arg::i32(global<u32>(d::g_mouse_y)), joan::orig::arg::str(jo::self<jo::Help>(this)->title), joan::orig::arg::str(jo::self<jo::Help>(this)->text), joan::orig::arg::u32(fl));  // 0x00489400
        } else if (rec != nullptr) {
            // [0xa48858] cannot have changed since the read above; zero bounds are a documented
            // deterministic stand-in for the original's uninitialised locals.
            i32 x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
            if (global<u32>(d::g_a48858) == 0) {
                x1 = jo::view<i16>(jo::as_help_entry(rec)->nums[12]);
                y1 = jo::view<i16>(jo::as_help_entry(rec)->nums[13]);
                x2 = jo::view<i16>(jo::as_help_entry(rec)->nums[14]);
                y2 = jo::view<i16>(jo::as_help_entry(rec)->nums[15]);
                x3 = jo::view<i16>(jo::as_help_entry(rec)->nums[16]);
                y3 = jo::view<i16>(jo::as_help_entry(rec)->nums[17]);
            }
            i32 lo_x, lo_y, hi_x, hi_y;
            if (x3 == -1) {
                lo_x = x1;
                lo_y = y1;
                hi_x = x2;
                hi_y = y2;
            } else {
                lo_x = joan::orig::arg::as<i32>(joan::orig::min_int(joan::orig::arg::i32(joan::orig::arg::as<i32>(joan::orig::min_int(joan::orig::arg::i32(x1), joan::orig::arg::i32(x2)))), joan::orig::arg::i32(x3)));  // 0x0046f020 min
                lo_y = joan::orig::arg::as<i32>(joan::orig::min_int(joan::orig::arg::i32(joan::orig::arg::as<i32>(joan::orig::min_int(joan::orig::arg::i32(y1), joan::orig::arg::i32(y2)))), joan::orig::arg::i32(y3)));
                hi_x = joan::orig::arg::as<i32>(joan::orig::int_max(joan::orig::arg::i32(joan::orig::arg::as<i32>(joan::orig::int_max(joan::orig::arg::i32(x1), joan::orig::arg::i32(x2)))), joan::orig::arg::i32(x3)));  // 0x004893c0 max
                hi_y = joan::orig::arg::as<i32>(joan::orig::int_max(joan::orig::arg::i32(joan::orig::arg::as<i32>(joan::orig::int_max(joan::orig::arg::i32(y1), joan::orig::arg::i32(y2)))), joan::orig::arg::i32(y3)));
            }
            const i32 cx = static_cast<i32>(static_cast<u32>(lo_x) + static_cast<u32>(hi_x)) / 2;
            const i32 cy = static_cast<i32>(static_cast<u32>(lo_y) + static_cast<u32>(hi_y)) / 2;
            joan::orig::as_help_box(me())->draw_tooltip(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy), joan::orig::arg::str(jo::as_help_entry(rec)->title), joan::orig::arg::str(jo::as_help_entry(rec)->text), joan::orig::arg::u32(fl));  // 0x00489400
        }
        finish();
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Menu::update_488f00", 0x00488f00, &H::menu_488f00);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::wadd;

JOAN_COUNTER(c_layout, "Tree::layout_4892d0");

struct H {
    // 0x004892d0
    void layout_4892d0(u8* node) {
        c_layout.hit();
        jo::Anim* const n = jo::as_anim(node);  // the path-search node (Anim layout: links 0x24, costs 0x3c..0x4c)
        for (i32 i = 0; i < static_cast<i32>(jo::view<i16>(n->member_count)); ++i) {
            jo::Anim* const c = n->members[i].anim;
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_path(this)->random_range(joan::orig::arg::u32(jo::view<u32>(c->grid_x)), joan::orig::arg::u32(jo::view<u32>(c->grid_y)), joan::orig::arg::u32(jo::view<u32>(n->grid_x)), joan::orig::arg::u32(jo::view<u32>(n->grid_y))));  // 0x004ee2e0
            jo::view<i32>(c->cost_so_far) = wadd(jo::view<i32>(n->cost_so_far), r);
            jo::view<i32>(c->cost_total) = wadd(jo::view<i32>(c->cost_so_far), c->cost_to_goal);
            jo::view<i32>(c->weight_sum) = wadd(jo::view<i32>(n->weight_sum), static_cast<i32>(c->weight));
            i16 v = 0;
            if (static_cast<i32>(n->weight_max) > static_cast<i32>(c->weight))
                v = n->weight_max;
            else
                v = c->weight;
            c->weight_max = v;
            joan::orig::as_path(this)->propagate_costs(joan::orig::arg::mptr(c));  // 0x004892d0
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Tree::layout_4892d0", 0x004892d0, &H::layout_4892d0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::img::str;

constexpr u32 kOne = 0x3f800000u;  // 1.0f
constexpr u32 M1 = 0xffffffffu;

JOAN_COUNTER(c_tooltip, "Tooltip::draw_489400");

struct H {
    // ------------------------------------------------------------------ 0x00489400
    static u32 color(u32 a1, u32 a2, u32 a3, u32 a4) {
        alignas(4) u8 tmp[4] = {0, 0, 0, 0};
        const u8* const c = joan::orig::arg::as<const u8*>(joan::orig::as_colour(static_cast<void*>(tmp))->construct(a1, a2, a3, a4));  // 0x0042a260
        u32 v;
        std::memcpy(&v, c, 4);
        return v;
    }
    static u32 s16(i32 v) { return static_cast<u32>(static_cast<i32>(static_cast<i16>(v))); }

    void tooltip_draw(i32 x, i32 y, const char* title, const char* text, u32 flags_arg) {
        c_tooltip.hit();
        const u32 flags = flags_arg & 0xffu;
        if (flags != 7 && (flags & 1u) != 0) return;
        if (global<i8>(d::g_a57fef) == 0) return;
        i32 w1 = 0, h1 = 0, w2 = 0, h2 = 0;
        void* const font1 = obj(d::g_font_a53f28);
        void* const font2 = obj(d::g_font_verdana_10);
        if (title == nullptr || *title == 0) return;
        w1 = static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_font(font1)->measure(joan::orig::arg::str(title), joan::orig::arg::i16(M1), 0x122, 0, joan::orig::arg::f32(kOne)))) + 10;  // 0x00624f00
        h1 = static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_font(font1)->scaled_round(joan::orig::arg::f32(kOne)))) + 9;                          // 0x00624eb0
        if (text != nullptr && *text != 0) {
            w2 = static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_font(font2)->measure(joan::orig::arg::str(text), joan::orig::arg::i16(M1), 0x122, 0, joan::orig::arg::f32(kOne)))) + 10;
            alignas(4) u8 o24[4] = {0, 0, 0, 0};
            alignas(4) u8 o20[4] = {0, 0, 0, 0};
            joan::orig::as_text_box(font2)->measure_lines(0, 0, joan::orig::arg::i32(static_cast<u32>(w2 - 0xb)), joan::orig::arg::i32(global<u32>(d::g_screen_h) - 1u), joan::orig::arg::str(text), 4, 0, joan::orig::arg::mptr(static_cast<void*>(o24)), joan::orig::arg::mptr(static_cast<void*>(o20)), joan::orig::arg::u32(kOne), joan::orig::arg::u32(kOne));  // 0x00627d80 (writes words)
            i16 lines;
            std::memcpy(&lines, o24, 2);
            h2 = static_cast<i32>(static_cast<u32>(lines) * static_cast<u32>(static_cast<i32>(jo::as_font2d(font2)->line_height)) +
                                  static_cast<u32>(lines) * 4u + 5u);
            if (jo::self<jo::Help>(this)->tooltip_style == 2) {
                const i32 m = w2 > w1 ? w2 : w1;
                w1 = m;
                w2 = m;
            }
        }
        if (jo::self<jo::Help>(this)->tooltip_style == 0) return;
        if (title == nullptr || *title == 0) return;
        i32 x1 = x;
        i32 y1 = static_cast<i32>(static_cast<u32>(y) + global<u32>(d::g_a4863a));
        i32 yb;
        if (static_cast<i32>(static_cast<u32>(y1) + static_cast<u32>(h1) + static_cast<u32>(h2) - 1u) >=
            global<i32>(d::g_screen_h)) {
            y1 = static_cast<i32>(static_cast<u32>(y) - static_cast<u32>(h1));
            yb = static_cast<i32>(static_cast<u32>(y1) - static_cast<u32>(h2));
        } else {
            yb = static_cast<i32>(static_cast<u32>(y1) + static_cast<u32>(h1));
        }
        i32 x2 = static_cast<i32>(static_cast<u32>(x1) + static_cast<u32>(w1) - 1u);
        const i32 y2 = static_cast<i32>(static_cast<u32>(y1) + static_cast<u32>(h1) - 1u);
        if (x2 >= global<i32>(d::g_screen_w)) {
            x2 = static_cast<i32>(global<u32>(d::g_screen_w) - 1u);
            x1 = static_cast<i32>(static_cast<u32>(x2) - static_cast<u32>(w1) + 1u);
        }
        if ((flags & 2u) != 0) {
            const u32 c = color(0x66u, 0x66u, 0x66u, 0xccu);
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(joan::orig::arg::i32(s16(x1)), joan::orig::arg::i32(s16(y1)), joan::orig::arg::i32(s16(x2)), joan::orig::arg::i32(s16(y2)), 2u, joan::orig::arg::u32(c), joan::orig::arg::mptr(0u), 3, 0, 0, 0, joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), 0);  // 0x00632a70
        }
        if ((flags & 4u) != 0) {
            const u32 c = color(0xffu, 0xffu, 0xffu, 0xffu);
            joan::orig::as_text(font1)->draw_line(joan::orig::arg::i32(static_cast<u32>(x1 + 5)), joan::orig::arg::i32(static_cast<u32>(y1 + 4)), 2u, title, joan::orig::arg::u32(c), joan::orig::arg::i32(M1), 0, 1, 2, 0, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
        }
        if (jo::self<jo::Help>(this)->tooltip_style != 2) return;
        if (text == nullptr || *text == 0) return;
        const i32 xb = x1;
        const i32 xb2 = static_cast<i32>(static_cast<u32>(xb) + static_cast<u32>(w2) - 1u);
        const i32 yb2 = static_cast<i32>(static_cast<u32>(yb) + static_cast<u32>(h2) - 1u);
        if ((flags & 2u) != 0) {
            const u32 c = color(0xccu, 0xccu, 0xccu, 0xccu);
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(joan::orig::arg::i32(s16(xb)), joan::orig::arg::i32(s16(yb)), joan::orig::arg::i32(s16(xb2)), joan::orig::arg::i32(s16(yb2)), 2u, joan::orig::arg::u32(c), joan::orig::arg::mptr(0u), 3, 0, 0, 0, joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), 0);
        }
        if ((flags & 4u) != 0) {
            const u32 c = color(0u, 0u, 0u, 0xffu);
            joan::orig::as_text(font2)->draw(joan::orig::arg::i32(static_cast<u32>(xb + 5)), joan::orig::arg::i32(static_cast<u32>(yb + 4)), joan::orig::arg::i32(static_cast<u32>(xb2 - 5)), joan::orig::arg::i32(static_cast<u32>(yb2 - 2)), 2u, text, joan::orig::arg::u32(c), 4, 1, 1, 1, 2, 0, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), 0, joan::orig::arg::i32(M1));  // 0x00626320
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Tooltip::draw_489400", 0x00489400, &H::tooltip_draw);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::at;

JOAN_COUNTER(c_anim_start, "Anim::start");

struct H {
    // ------------------------------------------------------------------------ 0x00489820
    // Starts a route search at this TrackNode (named Anim::start; its only callers are Path::find_4ed040's start and
    // neighbour nodes): stamp, no parent, rounded position, the child table grown to the graph's node count.
    u32 anim_start(u32 a, void* b) {
        c_anim_start.hit();
        jo::TrackNode* const n = jo::self<jo::TrackNode>(this);
        n->search_stamp = a;
        n->parent = nullptr;
        n->next_open = nullptr;
        n->grid_x = static_cast<u32>(joan::hook::x87::ftol(static_cast<double>(n->pos[0])));
        n->grid_y = static_cast<u32>(joan::hook::x87::ftol(static_cast<double>(n->pos[1])));
        n->i16_34 = static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(n->pos[2])));
        if (n->owner->node_count > static_cast<i32>(n->child_cap)) {  // PathGraph +0x46
            n->child_cap = static_cast<i16>(n->owner->node_count);
            if (n->children != nullptr) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(n->children)));  // operator delete
            n->children = static_cast<jo::TrackNodePtr*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(static_cast<i32>(n->child_cap)) * static_cast<u32>(sizeof(jo::TrackNodePtr)))));  // new (pointer table: << 2 on x86)
        }
        n->child_count = 0;
        n->closed = 0;
        joan::orig::as_member(this)->set_group_48be30(b);  // 0x0048be30
        return 0;  // void callee; every caller ignores the result
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Anim::start", 0x00489820, &H::anim_start);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b92_02_2.cpp (game/b92_02)
namespace {
namespace game_b92_02_2 {

using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::call::cdecl_call;
using joan::img::str;

JOAN_COUNTER(c_helpbox_set, "HelpBox::set");
JOAN_COUNTER(c_helpbox_set2, "HelpBox::set2");

struct H {
    static std::int32_t half_sum(std::int32_t a, std::int32_t b) {
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)) / 2;
    }
    static std::int32_t imin(std::int32_t a, std::int32_t b) { return joan::orig::arg::as<std::int32_t>(joan::orig::min_int(joan::orig::arg::i32(a), joan::orig::arg::i32(b))); }  // 0x0046f020
    static std::int32_t imax(std::int32_t a, std::int32_t b) { return joan::orig::arg::as<std::int32_t>(joan::orig::int_max(joan::orig::arg::i32(a), joan::orig::arg::i32(b))); }  // 0x004893c0
    // Shared box/anchor computation; false when the rectangle test rejects it.
    static bool anchor(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, std::int32_t ax,
                       std::int32_t ay, std::int32_t* cx, std::int32_t* cy) {
        if (ax == -1) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1))) == 0) return false;  // 0x00593980
            *cx = half_sum(x0, x1);
            *cy = half_sum(y0, y1);
        } else {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_input(obj(d::g_mouse))->cursor_in_triangle(joan::orig::arg::u32(x0), joan::orig::arg::u32(y0), joan::orig::arg::u32(x1), joan::orig::arg::u32(y1), joan::orig::arg::u32(ax), joan::orig::arg::u32(ay))) == 0) return false;  // 0x005939f0
            const std::int32_t mn = imin(imin(x0, x1), ax);
            const std::int32_t mx = imax(imax(x0, x1), ax);
            *cx = half_sum(mx, mn);
            const std::int32_t mny = imin(imin(y0, y1), ay);
            const std::int32_t mxy = imax(imax(y0, y1), ay);
            *cy = half_sum(mxy, mny);
        }
        return true;
    }
    void store_box(std::int32_t cx, std::int32_t cy, std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1,
                   std::int32_t ax, std::int32_t ay) {
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[0]) = static_cast<std::uint16_t>(cx);
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[1]) = static_cast<std::uint16_t>(cy);
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[2]) = static_cast<std::uint16_t>(x0);
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[3]) = static_cast<std::uint16_t>(y0);
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[4]) = static_cast<std::uint16_t>(x1);
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[5]) = static_cast<std::uint16_t>(y1);
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[6]) = static_cast<std::uint16_t>(ax);
        jo::view<std::uint16_t>(jo::self<jo::Help>(this)->box[7]) = static_cast<std::uint16_t>(ay);
    }

    // 0x00489920
    void helpbox_set(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, const char* text, std::int32_t ax,
                     std::int32_t ay) {
        c_helpbox_set.hit();
        std::int32_t cx, cy;
        if (!anchor(x0, y0, x1, y1, ax, ay, &cx, &cy)) return;
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Help>(this)->label)), joan::orig::arg::str(text));  // strcpy
        store_box(cx, cy, x0, y0, x1, y1, ax, ay);
    }

    // 0x00489b30
    void helpbox_set2(std::int32_t x0, std::int32_t y0, std::int32_t x1, std::int32_t y1, const char* title,
                      const char* text, std::int32_t ax, std::int32_t ay) {
        c_helpbox_set2.hit();
        std::int32_t cx, cy;
        if (!anchor(x0, y0, x1, y1, ax, ay, &cx, &cy)) return;
        jo::view<std::uint8_t>(jo::self<jo::Help>(this)->label[0]) = 0;
        store_box(cx, cy, x0, y0, x1, y1, ax, ay);
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Help>(this)->title)), joan::orig::arg::str(title), 0x50u);  // strncpy
        jo::view<std::uint8_t>(jo::self<jo::Help>(this)->title[80]) = 0;
        if (text != nullptr) {
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Help>(this)->text)), joan::orig::arg::str(text), 0xc8u);  // strncpy
            jo::view<std::uint8_t>(jo::self<jo::Help>(this)->text[200]) = 0;
        } else {
            jo::view<std::uint8_t>(jo::self<jo::Help>(this)->text[0]) = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("HelpBox::set", 0x00489920, &H::helpbox_set);
B("HelpBox::set2", 0x00489b30, &H::helpbox_set2);
#undef B

}  // namespace game_b92_02_2
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::str;

JOAN_COUNTER(c_find_entry, "Obj::find_entry");

struct H {
    // ------------------------------------------------------------------ 0x00489d90
    u8* find_entry(const char* name) {
        c_find_entry.hit();
        if (name == nullptr || static_cast<i32>(static_cast<i8>(name[0])) == 0) return nullptr;
        i32 i = jo::self<jo::Help>(this)->name_first;
        jo::HelpEntry* p = jo::self<jo::Help>(this)->entries + i;  // 0x70-byte records on x86
        for (; i < static_cast<i32>(jo::self<jo::Help>(this)->name_end); ++i, ++p) {
            if (static_cast<i32>(static_cast<i8>(p->key[0])) == static_cast<i32>(static_cast<i8>(name[0])) &&
                std::strcmp(joan::orig::arg::str(static_cast<const void*>(p->key)), joan::orig::arg::str(name)) == 0) {
                break;
            }
        }
        if (i < static_cast<i32>(jo::self<jo::Help>(this)->name_end)) return reinterpret_cast<u8*>(p);
        return nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::find_entry", 0x00489d90, &H::find_entry);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::HelpBox::load(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_04::H::load_488400>(this, a0)); }  // 0x488400
auto joan::orig::HelpBox::clear() -> void { joan::bind::member<&::game_b92_02::H::help_clear>(this); }  // 0x4884c0
auto joan::orig::HelpBox::parse(const char* a0) -> void { joan::bind::member<&::game_b104_02::H::help_load_488590>(this, a0); }  // 0x488590
auto joan::orig::Rect::extend(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b84_06::H::extend>(this, a0, a1); }  // 0x488ac0
auto joan::orig::HelpBox::check_wait() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b101_03::H::check_wait>(this)); }  // 0x488c00
auto joan::orig::Path::propagate_costs(void* a0) -> void { joan::bind::member<&::game_b108_02::H::layout_4892d0>(this, a0); }  // 0x4892d0
auto joan::orig::int_max(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b84_06::int_max>(a0, a1)); }  // 0x4893c0
auto joan::orig::HelpBox::draw_tooltip(std::int32_t a0, std::int32_t a1, const char* a2, const char* a3, std::uint32_t a4) -> void { joan::bind::member<&::game_b108_15::H::tooltip_draw>(this, a0, a1, a2, a3, a4); }  // 0x489400
auto joan::orig::Anim::start(std::uint32_t a0, void* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_02::H::anim_start>(this, a0, a1)); }  // 0x489820
auto joan::orig::HelpBox::set(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, const char* a4, std::int32_t a5, std::int32_t a6) -> void { joan::bind::member<&::game_b92_02_2::H::helpbox_set>(this, a0, a1, a2, a3, a4, a5, a6); }  // 0x489920
auto joan::orig::HelpBox::set2(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, const char* a4, const char* a5, std::int32_t a6, std::int32_t a7) -> void { joan::bind::member<&::game_b92_02_2::H::helpbox_set2>(this, a0, a1, a2, a3, a4, a5, a6, a7); }  // 0x489b30
auto joan::orig::FileTxt::data_size_489e70() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::field_196>(this)); }  // 0x489e70
