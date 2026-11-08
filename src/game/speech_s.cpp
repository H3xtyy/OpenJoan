// game/speech_s.cpp: functions of the original d:\ww\speech_s.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstdarg>
#include <cmath>
#include "../canvas_api.hpp"
#include "../original/records.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::cdecl_call;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

using joan::orig::helpers::memset_;  // memset
using joan::orig::helpers::strcpy_;  // strcpy
using joan::orig::helpers::line_of;

using joan::call::thiscall;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x004d1770 Speech::load_header      (thiscall(file), ret 4)
//
//  - 0x004d1770 keeps the 0x104-byte line buffer (read_line writes its NUL at buf[0xff]);
//    the tokenizer 0x004d14f0 ends with its global (0xa3a290) cleared, as in the original.
namespace {
namespace game_b93_02 {

JOAN_COUNTER(c_speech_load, "Speech::load_header");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x004d1770
    void speech_load(void* file) {
        c_speech_load.hit();
        jo::view<void*>(jo::self<jo::Speech>(this)->script) = file;
        char buf[0x104];
        joan::orig::as_file_txt(jo::view<void*>(jo::self<jo::Speech>(this)->script))->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), i32{0xff});  // 0x00589f10 read_line
        joan::orig::str_token(joan::orig::arg::str(buf), static_cast<char>(joan::orig::arg::i32(i32{0x2c})));             // 0x004d14f0 tokenizer
        i32 cap = 0;
        jo::self<jo::Speech>(this)->text_column = -1;
        jo::self<jo::Speech>(this)->column_count = 0;
        for (;;) {
            char* const tok = joan::orig::arg::as<char*>(joan::orig::str_token(joan::orig::arg::str(static_cast<char*>(nullptr)), static_cast<char>(joan::orig::arg::i32(i32{0x2c}))));  // 0x004d14f0
            if (tok == nullptr) break;
            if (jo::self<jo::Speech>(this)->column_count == cap) {
                cap += 5;
                jo::self<jo::Speech>(this)->columns = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::self<jo::Speech>(this)->columns), joan::orig::arg::u32(cap * 0x50), joan::orig::arg::str(// 0x0058eb30
                                                 str(d::s_d_ww_speech_s_cpp)), joan::orig::arg::i32(line_of(d::g_a03390, 0x17)));  // "d:\ww\speech_s.cpp"
            }
            strcpy_(jo::view<char*>(jo::self<jo::Speech>(this)->columns) + jo::self<jo::Speech>(this)->column_count * 0x50, tok);
            if (stricmp_(tok, str(d::s_text)) == 0) jo::self<jo::Speech>(this)->text_column = jo::self<jo::Speech>(this)->column_count;  // "text"
            jo::self<jo::Speech>(this)->column_count = jo::self<jo::Speech>(this)->column_count + 1;
        }
        jo::view<void*>(jo::self<jo::Speech>(this)->text_) = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(i32{0x190}), str(d::s_d_ww_speech_s_cpp), joan::orig::arg::i32(// 0x0058e7b0
                                      line_of(d::g_a03390, 0x23)));
        memset_(jo::view<void*>(jo::self<jo::Speech>(this)->text_), 0, 0x190);
        jo::view<u32>(jo::self<jo::Speech>(this)->text_len) = 0;
        jo::view<u32>(jo::self<jo::Speech>(this)->text_cap) = 0x190;
        jo::view<u32>(jo::self<jo::Speech>(this)->speed) = 0x3f800000u;
        jo::view<u32>(jo::self<jo::Speech>(this)->state) = 1;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Speech::load_header", 0x004d1770, &H::speech_load);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x004d1980 Speech::reset         (thiscall, ret 0)
namespace {
namespace game_b99_03 {

JOAN_COUNTER(c_speech_reset, "Speech::reset");

void mem_free(void* p, img::Addr file, i32 line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------------ 0x004d1980
    void speech_reset() {
        c_speech_reset.hit();
        if (jo::self<jo::Speech>(this)->script != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Speech>(this)->script);
            if (p != nullptr) joan::orig::as_file_txt(p)->destroy(1u);  // 0x00419280
            jo::self<jo::Speech>(this)->script = nullptr;
        }
        jo::self<jo::Speech>(this)->target = nullptr;
        if (jo::self<jo::Speech>(this)->text_ != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Speech>(this)->text_), d::s_d_ww_speech_s_cpp, static_cast<i32>(global<i16>(d::g_a03394)) + 0xb);  // speech_s.cpp
            jo::self<jo::Speech>(this)->text_ = nullptr;
        }
        if (jo::self<jo::Speech>(this)->columns != nullptr) {
            mem_free(jo::self<jo::Speech>(this)->columns, d::s_d_ww_speech_s_cpp, static_cast<i32>(global<i16>(d::g_a03394)) + 0x11);
            jo::self<jo::Speech>(this)->columns = nullptr;
        }
        jo::view<u32>(jo::self<jo::Speech>(this)->text_len) = 0;
        jo::view<u32>(jo::self<jo::Speech>(this)->text_cap) = 0;
        jo::view<u32>(jo::self<jo::Speech>(this)->state) = 0;
        jo::view<u32>(jo::self<jo::Speech>(this)->time_f) = 0;
        jo::view<u32>(jo::self<jo::Speech>(this)->time) = 0;
        jo::view<u32>(jo::self<jo::Speech>(this)->next_time) = 0xffffffffu;
        jo::self<jo::Speech>(this)->last_tick = 0;
        jo::view<u32>(jo::self<jo::Speech>(this)->text_column) = 0xffffffffu;
        jo::view<u32>(jo::self<jo::Speech>(this)->column_count) = 0;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Speech::reset", 0x004d1980, &H::speech_reset);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
//
//      0x004d1af0 X::set_4                 (thiscall(v), ret 4)
namespace {
namespace game_b94_03 {

struct H {
    void set_4(jo::Actor* v) { jo::self<jo::Speech>(this)->target = v; }     // 0x004d1af0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("X::set_4", 0x004d1af0, &H::set_4);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
//
//      0x004d1b20 X::set_130_2             (thiscall, ret 0) - +0x130 = 2 unless it is 4.
//
//  - eax kept where cheap: 0x004c15a0 / 0x004d1b20 return this, 0x004cd570 returns v (owner
//    path) or this.
namespace {
namespace game_b95_03 {

struct H {
    void* set_130_2() {  // 0x004d1b20
        if (jo::view<u32>(jo::self<jo::Speech>(this)->state) != 4) jo::view<u32>(jo::self<jo::Speech>(this)->state) = 2;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("X::set_130_2", 0x004d1b20, &H::set_130_2);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
//
//      0x004d1b60 Speech::mode_2_or_3          (thiscall, ret 0) - al.
namespace {
namespace game_b98_03 {

struct H {
    // ------------------------------------------------------------------------ 0x004d1b60
    u8 mode_2_or_3() { return (jo::self<jo::Speech>(this)->state == 2 || jo::self<jo::Speech>(this)->state == 3) ? 1 : 0; }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Speech::mode_2_or_3", 0x004d1b60, &H::mode_2_or_3);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - Obj::is_state4 0x004d1bb0 -> bool: dword +0x130 == 4.
namespace {
namespace game_b100_05 {

struct H {
    // ------------------------------------------------------------------ 0x004d1bb0
    bool is_state4() { return jo::self<jo::Speech>(this)->state == 4; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::is_state4", 0x004d1bb0, &H::is_state4);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b106_04.cpp (game/b106_04)
//
//      0x004d1be0 Speech::update_4d1be0()           thiscall, ret 0 (called from 0x0063b5e0,
//                 eax ignored). Advances the timed script: reads "time,col0,col1,..." lines
//                 (time * 1000 via atof), fills two 256-byte-per-column tables, handles the
//                 "<comma>" escape, then turns "a[50]|b|c" weight lists into float arrays for
//                 0x005c9120 / 0x005c9350.
//
//  - 0x004d1be0: (now - [+0x18]) is loaded with fild qword {diff, 0} (unsigned) and multiplied
//    by a float at PC24, and atof * 1000.0 (qword) also at PC24: both via Fpu{true}.
//    weight = atoi / 100.0f is fild + fdiv -> Fpu{true}; the sums are float-only.
namespace {
namespace game_b106_04 {

using joan::hook::x87::Fpu;

// CRT (direct calls in the original).
void* crt_new(u32 n) { return joan::crt::operator_new(joan::orig::arg::u32(n)); }        // operator new
void crt_delete(void* p) { joan::crt::operator_delete(joan::orig::arg::mptr(p)); }            // operator delete
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }  // strlen
void* crt_memset(void* d, i32 c, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }
using joan::orig::helpers::crt_memcpy;
char* crt_strstr(const char* s, const char* k) { return joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(s), joan::orig::arg::str(k))); }
char* crt_strncpy(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
char* crt_strtok(char* s, const char* d) { return joan::orig::arg::as<char*>(joan::orig::libc::strtok(joan::orig::arg::str(s), joan::orig::arg::str(d))); }
char* crt_strchr(const char* s, i32 c) { return joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(s), joan::orig::arg::i32(c))); }
using joan::orig::helpers::crt_atoi;
double crt_atof(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }  // st0 (qword)

JOAN_COUNTER(c_speech_update, "Speech::update_4d1be0");

// "<comma>" escape: first occurrence becomes ',' and the rest of the string moves down.
void unescape_comma(char* tk) {
    char* const p = crt_strstr(tk, str(d::s_comma_9d5b88));  // "<comma>"
    if (p != nullptr) {
        *p = ',';
        const u32 n = crt_strlen(p + 7);
        crt_memcpy(p + 1, p + 7, n + 1);
    }
}

// Parses "name[pct]|name|..." of one column into a weight array for the object of column i.
// Returns the array and the object.
float* parse_weights(u8* self, i32 i, char* text, void** out_obj) {
    void* const o = joan::orig::arg::as<void*>(joan::orig::as_actor(jo::view<void*>(jo::as_speech(self)->target))->find_morph(joan::orig::arg::str(jo::view<u8*>(jo::as_speech(self)->columns) + i * 0x50)));  // 0x005aec90
    const u32 sz = jo::view<u32>(jo::as_morph(o)->target_count) << 2;
    float* const arr = static_cast<float*>(crt_new(sz));
    crt_memset(arr, 0, jo::view<u32>(jo::as_morph(o)->target_count) << 2);
    float sum = 0.0f;
    char* tok = crt_strtok(text, str(d::s_bar_9d5b84));  // "|"
    while (tok != nullptr) {
        float w = 1.0f;
        if (crt_strchr(tok, 0x5b) != nullptr) {
            char* const q = crt_strchr(tok, 0x5b) + 1;
            *crt_strchr(q, 0x5d) = 0;
            const i32 pct = crt_atoi(q);
            w = static_cast<float>(Fpu{true}.div(static_cast<double>(pct),
                                                 static_cast<double>(global<float>(d::flt_100))));
            *crt_strchr(tok, 0x5b) = 0;
        }
        const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_morph(o)->find(joan::orig::arg::str(tok)));  // 0x005c90a0
        arr[idx - 1] = w;
        sum = sum + w;
        tok = crt_strtok(nullptr, str(d::s_bar_9d5b84));
    }
    const float rest = global<float>(d::flt_1) - sum;  // 1.0f - sum
    arr[0] = rest + arr[0];
    *out_obj = o;
    return arr;
}

// Reads the columns of the current line (after the time) into `table` (col i at i*256), the
// speech column (+0x28) going to `speech(tk)`.
template <class F>
void read_columns(u8* self, char* table, F speech) {
    for (i32 i = 0; i < jo::as_speech(self)->column_count; ++i) {
        char* const tk = joan::orig::arg::as<char*>(joan::orig::str_token(joan::orig::arg::str(static_cast<char*>(nullptr)), static_cast<char>(joan::orig::arg::i32(0x2c))));  // 0x004d14f0
        if (crt_strlen(tk) == 0) continue;
        if (i == jo::as_speech(self)->text_column) {
            unescape_comma(tk);
            speech(tk);
        } else {
            crt_strncpy(table + (i << 8), tk, 0xff);
        }
    }
}

struct H {
    // -----------------------------------------------------------------------------------
    // 0x004d1be0
    void speech_update() {
        u8* const self = reinterpret_cast<u8*>(this);
        if (jo::as_speech(self)->state != 2 && jo::as_speech(self)->state != 3) return;
        c_speech_update.hit();
        if (jo::as_speech(self)->last_tick != 0) {
            const u32 diff = global<u32>(d::g_now) - jo::as_speech(self)->last_tick;
            const Fpu fpu{true};
            const double p = fpu.mul(static_cast<double>(diff), static_cast<double>(jo::as_speech(self)->speed));
            jo::as_speech(self)->time_f = static_cast<float>(fpu.add(p, static_cast<double>(jo::as_speech(self)->time_f)));
            jo::as_speech(self)->time = joan::hook::x87::ftol(static_cast<double>(jo::as_speech(self)->time_f));
            jo::as_speech(self)->last_tick = global<u32>(d::g_now);
        } else {
            jo::view<u32>(jo::as_speech(self)->time_f) = 0;
            jo::as_speech(self)->time = 0;
            jo::as_speech(self)->last_tick = global<u32>(d::g_now);
        }
        if (jo::as_speech(self)->time < jo::as_speech(self)->next_time && jo::as_speech(self)->next_time != -1 &&
            jo::as_speech(self)->state == 2)
            return;
        char* const speech = jo::as_speech(self)->pending;
        if (crt_strlen(speech) != 0) {
            joan::orig::as_speech(self)->append_word(joan::orig::arg::str(speech));  // 0x004d27e0
            jo::as_speech(self)->pending[0] = 0;
        }
        i32 last = jo::as_speech(self)->next_time;  // ebp-0x8
        char* const tab_a = static_cast<char*>(crt_new(jo::view<u32>(jo::as_speech(self)->column_count) << 8));
        char* const tab_b = static_cast<char*>(crt_new(jo::view<u32>(jo::as_speech(self)->column_count) << 8));
        crt_memset(tab_a, 0, jo::view<u32>(jo::as_speech(self)->column_count) << 8);
        crt_memset(tab_b, 0, jo::view<u32>(jo::as_speech(self)->column_count) << 8);
        i32 lines = 0;
        jo::as_speech(self)->next_time = -1;
        char line[0x400];
        do {
            joan::orig::as_file_txt(jo::view<void*>(jo::as_speech(self)->script))->read_line(static_cast<char*>(joan::orig::arg::mptr(line)), 0x3ff);  // 0x00589f10 read_line
            char* const tm = joan::orig::arg::as<char*>(joan::orig::str_token(joan::orig::arg::str(line), static_cast<char>(joan::orig::arg::i32(0x2c))));  // 0x004d14f0
            if (tm == nullptr || crt_strlen(tm) == 0) {
                jo::as_speech(self)->state = 4;
                break;
            }
            tm[crt_strlen(tm) - 1] = 0;
            const double ms = Fpu{true}.mul(crt_atof(tm), 1000.0);
            const i32 t = joan::hook::x87::ftol(ms);
            if (jo::as_speech(self)->state == 3) {
                jo::as_speech(self)->time = t + 1;
                jo::as_speech(self)->time_f = static_cast<float>(t + 1);
            }
            if (t <= jo::as_speech(self)->time) {
                last = t;
                read_columns(self, tab_a, [self](char* tk) { joan::orig::as_speech(self)->append_word(joan::orig::arg::str(tk)); });  // 0x004d27e0
            } else {
                jo::as_speech(self)->next_time = t;
                read_columns(self, tab_b, [self](char* tk) { crt_strcpy(jo::as_speech(self)->pending, tk); });
            }
            ++lines;
        } while (jo::as_speech(self)->next_time == -1 && last != jo::as_speech(self)->time);

        if (jo::as_speech(self)->target != nullptr && lines != 0) {
            for (i32 i = 0; i < jo::as_speech(self)->column_count; ++i) {
                if (i == jo::as_speech(self)->text_column) continue;
                if (crt_strlen(tab_a + (i << 8)) == 0) continue;
                void* o = nullptr;
                float* const arr = parse_weights(self, i, tab_a + (i << 8), &o);
                joan::orig::as_morph(o)->set_weights(joan::orig::arg::mptr(arr));  // 0x005c9120
            }
            if (jo::as_speech(self)->next_time != -1) {
                const u32 start = jo::as_speech(self)->last_tick + static_cast<u32>(last - jo::as_speech(self)->time);
                const i32 span = jo::as_speech(self)->next_time - last;
                for (i32 i = 0; i < jo::as_speech(self)->column_count; ++i) {
                    if (i == jo::as_speech(self)->text_column) continue;
                    if (crt_strlen(tab_b + (i << 8)) == 0) continue;
                    void* o = nullptr;
                    float* const arr = parse_weights(self, i, tab_b + (i << 8), &o);
                    joan::orig::as_morph(o)->blend_to_5c9350(joan::orig::arg::mptr(arr), joan::orig::arg::u32(start), joan::orig::arg::u32(span));  // 0x005c9350
                }
            }
        }
        crt_delete(tab_a);
        crt_delete(tab_b);
    }

    // -----------------------------------------------------------------------------------
    // 0x00525080
    struct Frame525 {         // ebp-0x16c .. ebp-0xc of the original
        char b16c[0x20];      // ebp-0x16c
        u8 pad14c[4];         // ebp-0x14c (NUL of the 0x20 read)
        char b148[0x28];      // ebp-0x148
        u8 pad120[4];         // ebp-0x120 (NUL of the 0x28 read)
        char b11c[8];         // ebp-0x11c
        u8 pad114[4];         // ebp-0x114 (NUL of the 8 read)
        char b110[0x100];     // ebp-0x110
        i32 i;                // ebp-0x10 (NUL of the 0x100 read lands in its low byte)
    };
    static_assert(sizeof(Frame525) == 0x160 && offsetof(Frame525, i) == 0x15c, "frame");
};

JOAN_REPLACE("game/b106_04", "Speech::update_4d1be0", 0x004d1be0, &H::speech_update);

}  // namespace game_b106_04
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
//
//    - empty thiscall functions (ret 0): 0x004c5240, 0x004d2a50, 0x004e1ea0, 0x004e4990,
//      0x004eedf0 (callers 0x470e14 / 0x530398 / 0x4e0162 / 0x470e42 / 0x470f3f never read eax),
//      0x004cadf0, 0x004cc830, 0x004ce9f0, 0x004d27c0 (speech_s.cpp), 0x00500b80, 0x00527500
//      (no references: dead code); Obj::nop_4feb60 0x004feb60 (2 args), ret 8 (dead code).
namespace {
namespace game_b105_06 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_4d27c0", 0x004d27c0, &H::nop);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
//
//      0x004d27e0 SpeechStr::append_word     (thiscall(s), ret 4) - grows buffer by 0x190
//                 (realloc, speech_s.cpp), adds ' ' separator, strcpy, length += strlen.
namespace {
namespace game_b97_03 {

JOAN_COUNTER(c_append_word, "SpeechStr::append_word");

using joan::orig::helpers::strlen_;  // strlen

struct H {
    // ------------------------------------------------------------------------ 0x004d27e0
    void* append_word(const char* s) {
        c_append_word.hit();
        const i32 len = static_cast<i32>(strlen_(s));
        if (jo::self<jo::Speech>(this)->text_cap < jo::self<jo::Speech>(this)->text_len + len + 2) {
            jo::view<void*>(jo::self<jo::Speech>(this)->text_) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Speech>(this)->text_)), joan::orig::arg::u32(jo::self<jo::Speech>(this)->text_cap + 0x190), joan::orig::arg::str(// 0x0058eb30
                                          str(d::s_d_ww_speech_s_cpp)), joan::orig::arg::i32(line_of(d::g_a03398, 4)));  // "d:\ww\speech_s.cpp"
            memset_(jo::view<u8*>(jo::self<jo::Speech>(this)->text_) + jo::self<jo::Speech>(this)->text_cap, 0, 0x190);
            jo::self<jo::Speech>(this)->text_cap = jo::self<jo::Speech>(this)->text_cap + 0x190;
        }
        if (jo::self<jo::Speech>(this)->text_len != 0) {
            jo::view<u8*>(jo::self<jo::Speech>(this)->text_)[jo::self<jo::Speech>(this)->text_len] = 0x20;
            jo::self<jo::Speech>(this)->text_len = jo::self<jo::Speech>(this)->text_len + 1;
        }
        strcpy_(reinterpret_cast<char*>(jo::view<u8*>(jo::self<jo::Speech>(this)->text_) + jo::self<jo::Speech>(this)->text_len), s);
        jo::self<jo::Speech>(this)->text_len = jo::self<jo::Speech>(this)->text_len + len;
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("SpeechStr::append_word", 0x004d27e0, &H::append_word);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x004d14f0 str_token(char* s, char sep)        cdecl   strtok-like, static 0xa3a290
namespace {
namespace game_b92_03 {

using joan::hook::global;

JOAN_COUNTER(c_str_token, "str_token");

// 0x004d14f0
char* JOAN_CDECL str_token(char* s, char sep) {
    c_str_token.hit();
    char*& next = global<char*>(d::g_a3a290);
    if (s != nullptr) next = s;
    char* const p = next;
    if (p == nullptr) return nullptr;
    std::int32_t i = 0;
    while (p[i] != sep && p[i] != 0) ++i;
    if (p[i] == 0) {
        next = nullptr;
    } else {
        next = p + i + 1;
        p[i] = 0;
    }
    return p;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("str_token", 0x004d14f0, &str_token);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - Obj::call_4d0bc0 0x004d0bc0 / Obj::call_4d15f0 0x004d15f0: forward to 0x004d0c20 /
//      0x004d1980.
namespace {
namespace game_b100_05_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    u32 call_4d15f0() {
        joan::orig::as_speech(this)->reset();  // 0x004d1980
        return 0;  // callers (0x0063b590 and its replacement) discard the result; Speech::reset returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::call_4d15f0", 0x004d15f0, &H::call_4d15f0);
#undef B

}  // namespace game_b100_05_2
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
//
//    - Speech::load 0x004d1620 (dir, file), ret 8 (dead code): 0x590b80 exists check (error
//      "Error loading Speech Script : %1s" via 0x586cb0), else new(0x19a) 0x589550(name) and
//      0x4d1770.
//
//  - 0x004c9530 / 0x004d1620: SEH frames only guard `new` + constructor (delete on throw);
//    normal path reproduced, the constructors do not throw.
namespace {
namespace game_b102_04 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::op_new;

JOAN_COUNTER(c_speech_load, "Speech::load");

struct H {
    // ------------------------------------------------------------------ 0x004d1620
    u32 speech_load(const char* dir, const char* file) {
        c_speech_load.hit();
        alignas(8) u8 path[0x194];                    // [ebp-0x1a4]
        joan::orig::as_string(path)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
        joan::orig::as_string(path)->append(joan::orig::arg::str(file));  // 0x005a1550
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(name)) == 0) {  // 0x00590b80
            void* const n2 = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
            return cdecl_call<u32>(fn::Error_fatal, obj(d::g_err), str(d::s_Error_loading_Speech_Script), n2);  // 0x00586cb0
        }
        void* const m = op_new(sizeof(jo::FileTxt));  // 0x19a on x86
        void* script = nullptr;
        if (m != nullptr) {
            void* const n3 = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
            script = joan::orig::as_file_txt(m)->construct(joan::orig::arg::str(n3));  // 0x00589550
        }
        joan::orig::as_speech(this)->load_header(script);  // 0x004d1770
        return 0;  // 0x004d1620 has no caller (dead code); Speech::load_header returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("Speech::load", 0x004d1620, &H::speech_load);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b105_06_2.cpp (game/b105_06)
namespace {
namespace game_b105_06_2 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {
    char* get8() { return jo::self<jo::Speech>(this)->text_; }
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::get8_4d2910", 0x004d2910, &H::get8);
B("Obj::nop_4d2a50", 0x004d2a50, &H::nop);
#undef B

}  // namespace game_b105_06_2
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------------ 0x004d2930
    void set_130_3() {
        if (jo::view<u32>(jo::self<jo::Speech>(this)->state) != 4) jo::view<u32>(jo::self<jo::Speech>(this)->state) = 3;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("X::set_130_3", 0x004d2930, &H::set_130_3);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {
    void* set_134(u32 v) {  // 0x004d2970
        jo::view<u32>(jo::self<jo::Speech>(this)->speed) = v;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Obj::set_134", 0x004d2970, &H::set_134);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03_2 {

using u32 = std::uint32_t;

struct H {
    u32 get_68() { return jo::self<jo::Scene>(this)->u32_68; }    // 0x004d29c0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("X::get_68", 0x004d29c0, &H::get_68);
#undef B94

}  // namespace game_b94_03_2
}  // namespace

// ---------------------------------------- from modules/game_b92_03_2.cpp (game/b92_03)
namespace {
namespace game_b92_03_2 {

struct H {
    // 0x004d2a00
    std::uint32_t dword_6c() { return jo::self<jo::Scene>(this)->u32_6c; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Obj::dword_6c", 0x004d2a00, &H::dword_6c);
#undef B

}  // namespace game_b92_03_2
}  // namespace

// ---------------------------------------- from modules/game_b100_05_3.cpp (game/b100_05)
namespace {
namespace game_b100_05_3 {

struct H {
    // ------------------------------------------------------------------ 0x004d2a20
    void nop_4d2a20() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::nop_4d2a20", 0x004d2a20, &H::nop_4d2a20);
#undef B

}  // namespace game_b100_05_3
}  // namespace

// ---------------------------------------- from modules/game_b95_03_2.cpp (game/b95_03)
namespace {
namespace game_b95_03_2 {

using u8 = std::uint8_t;

struct H {
    u8 bit_e() { return jo::self<jo::PopUpWin>(this)->flags & 1u; }  // 0x004d2a70 (on the PopUpWin 0x00a50808)
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("X::bit_e", 0x004d2a70, &H::bit_e);
#undef B95

}  // namespace game_b95_03_2
}  // namespace

// ---------------------------------------- from modules/game_b93_02_2.cpp (game/b93_02)
namespace {
namespace game_b93_02_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_138() {  // 0x004d15b0
        memset_(this, 0, sizeof(jo::Speech));  // 0x138 on x86
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Struct138::zero", 0x004d15b0, &H::zero_138);
#undef B93

}  // namespace game_b93_02_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::str_token(const char* a0, char a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::game_b92_03::str_token>(a0, a1)); }  // 0x4d14f0
auto joan::orig::Speech::zero() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_02_2::H::zero_138>(this)); }  // 0x4d15b0
auto joan::orig::Speech::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_05_2::H::call_4d15f0>(this)); }  // 0x4d15f0
auto joan::orig::Speech::load_header(void* a0) -> void { joan::bind::member<&::game_b93_02::H::speech_load>(this, a0); }  // 0x4d1770
auto joan::orig::Speech::reset() -> void { joan::bind::member<&::game_b99_03::H::speech_reset>(this); }  // 0x4d1980
auto joan::orig::Speech::set_4(Actor* a0) -> void { joan::bind::member<&::game_b94_03::H::set_4>(this, a0); }  // 0x4d1af0
auto joan::orig::Speech::set_state_2() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_03::H::set_130_2>(this)); }  // 0x4d1b20
auto joan::orig::Speech::mode_2_or_3() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b98_03::H::mode_2_or_3>(this)); }  // 0x4d1b60
auto joan::orig::Speech::is_state4() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b100_05::H::is_state4>(this)); }  // 0x4d1bb0
auto joan::orig::Speech::update() -> void { joan::bind::member<&::game_b106_04::H::speech_update>(this); }  // 0x4d1be0
auto joan::orig::Speech::append_word(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_03::H::append_word>(this, a0)); }  // 0x4d27e0
auto joan::orig::Speech::text() -> char* { return joan::bind::conv<char*>(joan::bind::member<&::game_b105_06_2::H::get8>(this)); }  // 0x4d2910
auto joan::orig::Speech::set_state_3() -> void { joan::bind::member<&::game_b96_03::H::set_130_3>(this); }  // 0x4d2930
auto joan::orig::Scene::flag_68() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_03_2::H::get_68>(this)); }  // 0x4d29c0
auto joan::orig::Scene::flag_6c() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_03_2::H::dword_6c>(this)); }  // 0x4d2a00
auto joan::orig::DebugOverlay::on_key() -> void { joan::bind::member<&::game_b100_05_3::H::nop_4d2a20>(this); }  // 0x4d2a20
auto joan::orig::DebugOverlay::update() -> void { joan::bind::member<&::game_b105_06_2::H::nop>(this); }  // 0x4d2a50
auto joan::orig::Popup::has_answer() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b95_03_2::H::bit_e>(this)); }  // 0x4d2a70
