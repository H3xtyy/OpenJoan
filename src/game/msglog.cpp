// game/msglog.cpp: functions of the original d:\ww\msglog.cpp (re/attribution.csv).
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
#include "../original/records.hpp"
#include <cstdarg>
#include <cmath>
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

using joan::call::thiscall;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
//
//      0x004b2940 MsgEntry::init(...)           (thiscall, ret 0x14)
namespace {
namespace game_b98_02 {

using i8 = std::int8_t;

JOAN_COUNTER(c_msg_init, "MsgEntry::init");

u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

struct H {
    // ------------------------------------------------------------------ 0x004b2940
    i32 msg_init(i32 id, i32 kind, const char* text, i32 value, const char* extra) {
        c_msg_init.hit();
        jo::self<jo::MsgEntry>(this)->kind = static_cast<i16>(id);
        jo::self<jo::MsgEntry>(this)->type = static_cast<i8>(kind);
        jo::self<jo::MsgEntry>(this)->text_len = 0;
        jo::self<jo::MsgEntry>(this)->extra_len = 0;
        if (extra != nullptr && crt_strlen(extra) != 0) {
            jo::self<jo::MsgEntry>(this)->extra_len = static_cast<i16>(crt_strlen(extra));
            const i32 line = static_cast<i32>(global<i16>(d::g_a01750)) + 0xd;
            jo::self<jo::MsgEntry>(this)->extra = joan::orig::arg::as<char*>(joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::MsgEntry>(this)->extra_len) + 1), str(d::s_d_ww_msglog_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
            crt_strcpy(jo::self<jo::MsgEntry>(this)->extra, extra);
        } else {
            jo::self<jo::MsgEntry>(this)->extra = nullptr;
        }
        const i32 line = static_cast<i32>(global<i16>(d::g_a01750)) + 0x13;
        jo::self<jo::MsgEntry>(this)->text = joan::orig::arg::as<char*>(joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(static_cast<i32>(crt_strlen(text) + 1)), str(d::s_d_ww_msglog_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        crt_strcpy(jo::self<jo::MsgEntry>(this)->text, text);
        jo::self<jo::MsgEntry>(this)->text_len = static_cast<i16>(crt_strlen(text));
        jo::self<jo::MsgEntry>(this)->value = value;
        return value;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("MsgEntry::init", 0x004b2940, &H::msg_init);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
//
//      0x004b2a90 MsgLog::ctor                   (thiscall, ret 0)   - SEH frame normal path.
//
//  - SEH frames in 0x00492e20 / 0x004b0640 / 0x0046de90 only guard a constructor after
//    operator new (delete on throw); 0x004b0480 / 0x004b2a90 guard member dtors. Normal path only.
namespace {
namespace game_b95_02 {

struct H {
    // ------------------------------------------------------------------ 0x004b2a90
    void* msglog_ctor() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 5, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        joan::orig::as_msg_log(this)->clear_4b2b80();                   // 0x004b2b80
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("MsgLog::ctor", 0x004b2a90, &H::msglog_ctor);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x004b2b10 Obj::dtor_4b2b10        (thiscall, ret 0; SEH frame only - normal path kept)
//
//  - Functions whose eax result is just the last callee's eax (0x004883d0, 0x0047bc80,
//    0x004b2b10, 0x004b31c0, 0x004ba1f0) pass that value through.
namespace {
namespace game_b94_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004b2b10
    u32 dtor_4b2b10() {
        joan::orig::as_msg_log(this)->clear_4b2c00();         // 0x004b2c00
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::dtor_4b2b10", 0x004b2b10, &H::dtor_4b2b10);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x004b2b80 MsgLog::clear            (thiscall, ret 0)
namespace {
namespace game_b93_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004b2b80
    void msglog_clear() {
        jo::view<u32>(jo::self<jo::MsgLog>(this)->count) = 0;
        jo::self<jo::MsgLog>(this)->last = 0;
        for (i32 i = 0; i < 9; ++i) {
            jo::self<jo::MsgLog>(this)->first_of_kind[i] = 0;
            jo::view<u32>(jo::self<jo::MsgLog>(this)->count_of_kind[i]) = 0;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("MsgLog::clear", 0x004b2b80, &H::msglog_clear);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x004b2c00 MsgLog::clear()                     ret 0   frees every message (msglog.cpp)
namespace {
namespace game_b92_03 {

void* const kMemory = obj(d::g_mem);

using joan::orig::helpers::game_delete;                      // operator delete

using joan::orig::helpers::line_base;
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

JOAN_COUNTER(c_msglog_clear, "MsgLog::clear");

struct H {
    // 0x004b2c00
    void msglog_clear() {
        c_msglog_clear.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(i))) != 0) continue;              // 0x00486620
            std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(i)));         // 0x004865f0
            if (jo::view<void*>(jo::as_msg_entry(m)->text) != nullptr) {
                mem_free(jo::view<void*>(jo::as_msg_entry(m)->text), str(d::s_d_ww_msglog_cpp), line_base(d::g_a01754) + 0xa);
                jo::view<void*>(jo::as_msg_entry(m)->text) = nullptr;
            }
            if (jo::view<void*>(jo::as_msg_entry(m)->extra) != nullptr)
                mem_free(jo::view<void*>(jo::as_msg_entry(m)->extra), str(d::s_d_ww_msglog_cpp), line_base(d::g_a01754) + 0x10);
            game_delete(m);
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));  // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        jo::self<jo::MsgLog>(this)->count = 0;
        jo::view<std::int32_t>(jo::self<jo::MsgLog>(this)->last) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("MsgLog::clear", 0x004b2c00, &H::msglog_clear);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x004b2d30 MsgLog::add           (thiscall(word, byte, name, c, d), ret 0x14)
//
//      0x004b2f10 MsgLog::remove        (thiscall(id), ret 4)
namespace {
namespace game_b99_03 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;

JOAN_COUNTER(c_log_add, "MsgLog::add");
JOAN_COUNTER(c_log_remove, "MsgLog::remove");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }         // _stricmp

void mem_free(void* p, img::Addr file, i32 line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}

i32 count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------------ 0x004b2d30
    u32 log_add(u32 word_a, u32 byte_b, const char* name, u32 c, const char* d) {
        c_log_add.hit();
        const i32 a = static_cast<i16>(word_a);
        for (i32 i = 1; i <= count(this); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00486620
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(i)));   // 0x004865f0
            if (static_cast<i32>(jo::as_msg_entry(e)->kind) != a) continue;
            if (stricmp_(jo::view<void*>(jo::as_msg_entry(e)->text), name) == 0) return 0;
        }
        void* p = joan::crt::operator_new(sizeof(jo::MsgEntry));  // operator new (0x1f on x86)
        jo::as_msg_entry(p)->next = 0;
        jo::as_msg_entry(p)->prev = 0;
        joan::orig::as_msg_entry(p)->init(joan::orig::arg::i32(static_cast<u32>(static_cast<u16>(word_a))), joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(byte_b))), joan::orig::arg::str(name), joan::orig::arg::i32(c), d);  // 0x004b2940
        joan::orig::dynarray_b(this)->linkin(&p);                                       // 0x005857a0
        jo::as_msg_entry(p)->rec_no = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());                         // 0x00417520
        jo::as_msg_entry(p)->prev = jo::self<jo::MsgLog>(this)->last;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(jo::self<jo::MsgLog>(this)->last))) == 0) {  // 0x00486620
            const u32 n = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());
            void* const q = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(jo::self<jo::MsgLog>(this)->last)));
            jo::as_msg_entry(q)->next = n;
        }
        jo::self<jo::MsgLog>(this)->count = jo::self<jo::MsgLog>(this)->count + 1;
        if (jo::view<u32>(jo::self<jo::MsgLog>(this)->count_of_kind[a]) == 0) {
            jo::self<jo::MsgLog>(this)->first_of_kind[a] = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());
        }
        jo::self<jo::MsgLog>(this)->count_of_kind[a] = jo::self<jo::MsgLog>(this)->count_of_kind[a] + 1;
        jo::self<jo::MsgLog>(this)->last = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());
    }

    // ------------------------------------------------------------------------ 0x004b2f10
    void log_remove(u32 id) {
        c_log_remove.hit();
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(id)));  // 0x004865f0
        auto ki = [e]() { return static_cast<i32>(jo::as_msg_entry(e)->kind); };
        jo::self<jo::MsgLog>(this)->count_of_kind[ki()] = jo::self<jo::MsgLog>(this)->count_of_kind[ki()] - 1;
        if (jo::view<u32>(jo::self<jo::MsgLog>(this)->count_of_kind[ki()]) == 0) {
            jo::self<jo::MsgLog>(this)->first_of_kind[ki()] = 0;
        } else if (jo::self<jo::MsgLog>(this)->first_of_kind[ki()] == jo::as_msg_entry(e)->rec_no) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(jo::as_msg_entry(e)->next))) == 0) {      // 0x00486620
                void* const n = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(jo::as_msg_entry(e)->next)));
                if (static_cast<i32>(jo::as_msg_entry(n)->kind) == ki()) jo::self<jo::MsgLog>(this)->first_of_kind[ki()] = jo::as_msg_entry(e)->next;
            }
        }
        if (jo::self<jo::MsgLog>(this)->last == id) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(jo::as_msg_entry(e)->prev))) == 0) {
                jo::self<jo::MsgLog>(this)->last = jo::as_msg_entry(e)->prev;
            } else {
                jo::self<jo::MsgLog>(this)->last = 0;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(jo::as_msg_entry(e)->next))) == 0) {
            void* const n = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(jo::as_msg_entry(e)->next)));
            jo::as_msg_entry(n)->prev = jo::as_msg_entry(e)->prev;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(jo::as_msg_entry(e)->prev))) == 0) {
            void* const pv = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(jo::as_msg_entry(e)->prev)));
            jo::as_msg_entry(pv)->next = jo::as_msg_entry(e)->next;
        }
        if (jo::as_msg_entry(e)->text != nullptr) {
            mem_free(jo::view<void*>(jo::as_msg_entry(e)->text), d::s_d_ww_msglog_cpp, static_cast<i32>(global<i16>(d::g_a01758)) + 0x2a);
            jo::as_msg_entry(e)->text = nullptr;
        }
        if (jo::as_msg_entry(e)->extra != nullptr) {
            mem_free(jo::view<void*>(jo::as_msg_entry(e)->extra), d::s_d_ww_msglog_cpp, static_cast<i32>(global<i16>(d::g_a01758)) + 0x30);
        }
        joan::crt::operator_delete(joan::orig::arg::mptr(e));  // operator delete
        joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(id));  // 0x005858c0
        jo::self<jo::MsgLog>(this)->count = jo::self<jo::MsgLog>(this)->count - 1;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("MsgLog::add", 0x004b2d30, &H::log_add);
B99("MsgLog::remove", 0x004b2f10, &H::log_remove);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x004b31c0 Obj::clear_4b31c0       (thiscall, ret 0) - 0x004b2c00 then 0x004b2b80.
//
//  - Functions whose eax result is just the last callee's eax (0x004883d0, 0x0047bc80,
//    0x004b2b10, 0x004b31c0, 0x004ba1f0) pass that value through.
namespace {
namespace game_b94_02_2 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004b31c0
    u32 clear_4b31c0() {
        joan::orig::as_msg_log(this)->clear_4b2c00();         // 0x004b2c00
        joan::orig::as_msg_log(this)->clear_4b2b80();  // 0x004b2b80
        return 0;  // every caller discards the result; MsgLog::clear returns nothing
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::clear_4b31c0", 0x004b31c0, &H::clear_4b31c0);
#undef B94

}  // namespace game_b94_02_2
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - Holder::remove_all_of 0x004b31f0 (k), ret 4.
namespace {
namespace game_b100_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
JOAN_COUNTER(c_remove_all, "Holder::remove_all_of");

struct H {
    // ------------------------------------------------------------------ 0x004b31f0
    i32 remove_all_of(i32 k) {
        c_remove_all.hit();
        for (i32 i = 1;; ++i) {
            const i32 n = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
            if (i > n) return n;
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00486620
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_4865f0(joan::orig::arg::i32(i)));         // 0x004865f0
            if (static_cast<i32>(jo::as_msg_entry(e)->kind) == k) joan::orig::as_msg_log(this)->remove(joan::orig::arg::u32(i));  // 0x004b2f10
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Holder::remove_all_of", 0x004b31f0, &H::remove_all_of);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b116_12.cpp (game/b116_12)
namespace {
namespace game_b116_12 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::hook::global;

using i32 = std::int32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;

using u16 = std::uint16_t;

u32 fbits(float f) {
    u32 b;
    std::memcpy(&b, &f, sizeof b);
    return b;
}

// _ftol 0x7b26cc: fistp qword with truncation; eax = low dword, edx = high dword.
i32 ftol(float v) { return joan::hook::x87::ftol(v); }
u32 ftol_hi(float v) {
    const double d = v;
    if (!(std::fabs(d) < 9.2233720368547758e18)) return 0x80000000u;  // integer indefinite
    return static_cast<u32>(static_cast<std::uint64_t>(static_cast<std::int64_t>(d)) >> 32);
}

void* const kSprites = obj(d::g_sprites_a46128);
void* const kText = obj(d::g_font_goudy14_22_white);

i32 strncmp_(const char* a, const char* b, u32 n) { return joan::orig::arg::as<i32>(joan::orig::libc::strncmp(joan::orig::arg::str(a), joan::orig::arg::str(b), joan::orig::arg::u32(n))); }

// 0x00633500 (thunk 0x00401bbd, ret 0x44) with the constant tail of every call here.
void sprite(i32 x, i32 y, const char* name, u32 color) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(global<joan::orig::BmpRes*>(d::g_a55ab6)), joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);
}
// Color(0xff, 0xff, 0xff, a) (0x0042a260 thunk 0x004098b8, ret 0x10).
u32 white(float alpha_field) {
    u32 c = 0;
    const i32 a = ftol(alpha_field);
    joan::orig::as_colour(&c)->construct(i32{0xff}, i32{0xff}, i32{0xff}, a);
    return c;
}

JOAN_COUNTER(c_draw, "Message::draw_4d8c10");

struct H {
    // 0x004d8c10
    void draw() {
        joan::canvas::ScreenScope canvas_scope(0x004d8c10);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_draw.hit();
        if (joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0) return;  // 0x004301c0
        if (joan::orig::arg::as<i8>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) != 0) return;  // 0x0042ced0
        if (global<i8>(d::g_fade_title) != 0) return;
        if (joan::orig::arg::as<i8>(joan::orig::as_glory_screen(obj(d::g_glory_screen))->is_open()) != 0) return;  // 0x006e7b50
        if (joan::orig::arg::as<i8>(joan::orig::as_screen(obj(d::g_game_over_screen))->flag_9ec()) != 0) return;  // 0x004af410
        const u32 c8 = white(jo::self<jo::Message>(this)->colour[4]);
        const char* const name = jo::self<jo::Message>(this)->pic;
        if (jo::view<i32>(jo::self<jo::Message>(this)->state) == 3) {
            const u32 c0c = white(jo::self<jo::Message>(this)->colour[0]);
            const u32 c10 = white(jo::self<jo::Message>(this)->colour[1]);
            const u32 c14 = white(jo::self<jo::Message>(this)->colour[2]);
            const u32 c18 = white(jo::self<jo::Message>(this)->colour[3]);
            sprite(0xc, 0x224, str(d::s_PANEL), c18);    // 'PANEL'
            sprite(4, 0x21c, str(d::s_CORN_TL), c0c);      // 'CORN_TL'
            sprite(0x3d5, 0x21c, str(d::s_CORN_TR), c0c);  // 'CORN_TR'
            sprite(4, 0x26d, str(d::s_CORN_BL), c10);      // 'CORN_BL'
            sprite(0x3d5, 0x26d, str(d::s_CORN_BR), c10);  // 'CORN_BR'
            sprite(0x1c, 0x234, str(d::s_ICON_1), c14);   // 'ICON_1'
            sprite(0x1c, 0x234, str(d::s_ICON_2), c14);   // 'ICON_2'
            if (jo::self<jo::Message>(this)->has_pic != 0 && strncmp_(jo::self<jo::Message>(this)->pic, str(d::s_M000), 4u) != 0) {  // 'M000'
                sprite(0x1c, 0x234, name, c14);
            } else {
                const i32 y = ftol(jo::self<jo::Message>(this)->y);
                const i32 x = ftol(jo::self<jo::Message>(this)->x);
                sprite(x, y, name, c8);
            }
            return;
        }
        if (jo::view<i32>(jo::self<jo::Message>(this)->state) == 4 || jo::view<i32>(jo::self<jo::Message>(this)->state) == 5) {
            sprite(0xc, 0x224, str(d::s_PANEL), c8);    // 'PANEL'
            sprite(4, 0x21c, str(d::s_CORN_TL), c8);      // 'CORN_TL'
            sprite(0x3d5, 0x21c, str(d::s_CORN_TR), c8);  // 'CORN_TR'
            sprite(4, 0x26d, str(d::s_CORN_BL), c8);      // 'CORN_BL'
            sprite(0x3d5, 0x26d, str(d::s_CORN_BR), c8);  // 'CORN_BR'
            sprite(0x1c, 0x234, str(d::s_ICON_1), c8);   // 'ICON_1'
            if (jo::self<jo::Message>(this)->has_pic != 0 && strncmp_(jo::self<jo::Message>(this)->pic, str(d::s_M000), 4u) != 0) {  // 'M000'
                sprite(0x1c, 0x234, name, c8);
            } else {
                const i32 y = ftol(jo::self<jo::Message>(this)->y);
                const i32 x = ftol(jo::self<jo::Message>(this)->x);
                sprite(x, y, name, c8);
            }
            sprite(0x1c, 0x234, str(d::s_ICON_2), c8);  // 'ICON_2'

            // Measure the typed part: cut the text at ftol(+0x3b), restore afterwards.
            const char saved = jo::self<jo::Message>(this)->text[ftol(jo::self<jo::Message>(this)->revealed_chars)];
            jo::self<jo::Message>(this)->text[ftol(jo::self<jo::Message>(this)->revealed_chars)] = 0;
            char* const text = jo::self<jo::Message>(this)->text;
            i16 lines = 0;     // [ebp-0x1c], written by 0x00627d80
            i16 first = 0;     // [ebp-0x20], written by 0x00627d80
            {
                const i32 y2 = joan::canvas::size_read(0x004d928b) - 0x84;
                const i32 x2 = joan::canvas::size_read(0x004d9298) - 0x1c;
                const i32 y1 = joan::canvas::size_read(0x004d92a2) - 0xcc;
                joan::orig::as_text_box(kText)->measure_lines(i32{0x68}, joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::str(text), i32{2}, i32{0}, &lines, &first, joan::orig::arg::u32(fbits(1.0f)), joan::orig::arg::u32(fbits(1.0f)));  // 0x00627d80
            }
            jo::self<jo::Message>(this)->text[ftol(jo::self<jo::Message>(this)->revealed_chars)] = saved;
            if (static_cast<i32>(lines) > 4)
                first = static_cast<i16>(static_cast<i32>(lines) - 3);
            else
                first = 1;
            const float typed = jo::self<jo::Message>(this)->revealed_chars;
            const i32 chars = static_cast<i32>(static_cast<u32>(ftol(typed)) -
                                               static_cast<u32>(static_cast<i32>(first) - 1) * 0x5eu);  // wrapping sub
            const u32 first_arg = (ftol_hi(typed) & 0xffff0000u) | static_cast<u16>(first);  // mov dx, word; push edx
            const i32 y2 = joan::canvas::size_read(0x004d9332) - 0x84;
            const i32 x2 = joan::canvas::size_read(0x004d933f) - 0x1c;
            const i32 y1 = joan::canvas::size_read(0x004d9349) - 0xcc;
            joan::orig::as_text(kText)->draw(i32{0x68}, joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{9}), jo::self<jo::Message>(this)->text, joan::orig::arg::u32(c8), i32{2}, joan::orig::arg::i32(first_arg), i32{1}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(fbits(1.0f)), joan::orig::arg::f32(fbits(1.0f)), i32{1}, joan::orig::arg::i32(chars));  // 0x00626320
            return;
        }
        if (jo::self<jo::Message>(this)->has_pic == 0) {
            const i32 y = ftol(jo::self<jo::Message>(this)->y);
            const i32 x = ftol(jo::self<jo::Message>(this)->x);
            sprite(x, y, name, c8);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_12", name, addr, fn)
B("Message::draw_4d8c10", 0x004d8c10, &H::draw);
JOAN_CANVAS_SELF(0x004d8c10);  // ScreenScope in the body (no thunk wrapper)
#undef B

}  // namespace game_b116_12
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x004b3160
    i32 clear_slot(i32 k) {
        if (jo::view<u32>(jo::self<jo::MsgLog>(this)->count_of_kind[k]) != 0) joan::orig::as_msg_log(this)->remove_all_of(joan::orig::arg::i32(k));  // 0x004b31f0
        jo::view<u32>(jo::self<jo::MsgLog>(this)->count_of_kind[k]) = 0;
        jo::self<jo::MsgLog>(this)->cur_kind = k;
        return k;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Obj::clear_slot", 0x004b3160, &H::clear_slot);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MsgEntry::init(std::int32_t a0, std::int32_t a1, const char* a2, std::int32_t a3, const char* a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_02::H::msg_init>(this, a0, a1, a2, a3, a4)); }  // 0x4b2940
auto joan::orig::MsgLog::ctor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_02::H::msglog_ctor>(this)); }  // 0x4b2a90
auto joan::orig::MsgLog::clear_4b2b80() -> void { joan::bind::member<&::game_b93_02::H::msglog_clear>(this); }  // 0x4b2b80
auto joan::orig::MsgLog::clear_4b2c00() -> void { joan::bind::member<&::game_b92_03::H::msglog_clear>(this); }  // 0x4b2c00
auto joan::orig::MsgLog::add(std::uint32_t a0, std::uint32_t a1, const char* a2, std::uint32_t a3, const char* a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_03::H::log_add>(this, a0, a1, a2, a3, a4)); }  // 0x4b2d30
auto joan::orig::MsgLog::remove(std::uint32_t a0) -> void { joan::bind::member<&::game_b99_03::H::log_remove>(this, a0); }  // 0x4b2f10
auto joan::orig::MsgLog::clear_slot(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_04::H::clear_slot>(this, a0)); }  // 0x4b3160
auto joan::orig::MsgLog::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_02_2::H::clear_4b31c0>(this)); }  // 0x4b31c0
auto joan::orig::MsgLog::remove_all_of(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_04::H::remove_all_of>(this, a0)); }  // 0x4b31f0
