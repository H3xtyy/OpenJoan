// game/unknown_t_basic.cpp: functions whose original source file is unknown (formerly game/_unplaced/t_basic.cpp;
// re/attribution.csv: between ww/speech_s.cpp and ww/t_scrres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/records.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::img::str;

using i32 = std::int32_t;

using joan::hook::global;

using joan::hook::x87::Fpu;

using joan::call::thiscall;

}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

using joan::call::cdecl_call;

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
const char* open_menu(void* self, const char* name, i32 n, u32 a) {
    return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), a));  // 0x005a3090
}

void JOAN_CDECL init_4d96e0() { joan::orig::init_pi_a3a328(); }
void JOAN_CDECL init_4d9740() { joan::orig::init_a3a32c(); }
void JOAN_CDECL init_4d9b00() { joan::orig::init_a3a334(); }

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* date(u32 a, u32 flag) {  // 0x004d9dc0
        if ((flag & 0xff) != 0) return open_menu(this, str(d::s_DATE), 3, a);
        return open_menu(this, str(d::s_DATE), 4, a);
    }
    const char* year_1() { return menu(d::s_YEAR, 1); }    // 0x004d9f00
    const char* name_1() { return menu(d::s_NAME, 1); }    // 0x004da470
    const char* name_2() { return menu(d::s_NAME, 2); }    // 0x004da4a0
    const char* name_3() { return menu(d::s_NAME, 3); }    // 0x004da4d0
    const char* popup_2() { return menu(d::s_POPUP, 2); }   // 0x004da530
    const char* popup_3() { return menu(d::s_POPUP, 3); }   // 0x004da560
    const char* popup_4() { return menu(d::s_POPUP, 4); }   // 0x004da590
    const char* popup_5() { return menu(d::s_POPUP, 5); }   // 0x004da5c0
    const char* popup_6() { return menu(d::s_POPUP, 6); }   // 0x004da5f0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("init_4d96e0", 0x004d96e0, &init_4d96e0);
B("init_4d9740", 0x004d9740, &init_4d9740);
B("init_4d9b00", 0x004d9b00, &init_4d9b00);
B("Date::open", 0x004d9dc0, &H::date);
B("Year::open_1", 0x004d9f00, &H::year_1);
B("Name::open_1", 0x004da470, &H::name_1);
B("Name::open_2", 0x004da4a0, &H::name_2);
B("Name::open_3", 0x004da4d0, &H::name_3);
B("Popup::open_2", 0x004da530, &H::popup_2);
B("Popup::open_3", 0x004da560, &H::popup_3);
B("Popup::open_4", 0x004da590, &H::popup_4);
B("Popup::open_5", 0x004da5c0, &H::popup_5);
B("Popup::open_6", 0x004da5f0, &H::popup_6);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {

using joan::call::cdecl_call;

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090

// ------------------------------------------------------------------------------- cdecl
float pi_from_atan() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(Fpu{true}.mul(a, 4.0));       // qword [0x9cf788] = 4.0
}
void JOAN_CDECL init_pi_a3a328() { global<float>(d::g_a3a328) = pi_from_atan(); }  // 0x004d9710

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* ampm() { return menu(d::s_AMPM, 1); }                                         // 0x004d9ea0 "AMPM"
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("init_pi_a3a328", 0x004d9710, &init_pi_a3a328);
B98("Menu::ampm", 0x004d9ea0, &H::ampm);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b100_06.cpp (game/b100_06)
namespace {
namespace game_b100_06 {

using joan::call::cdecl_call;

char* text1(void* self, const char* section, i32 n, u32 a) {
    return joan::orig::arg::as<char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(section), joan::orig::arg::i32(n), a));
}

const char* const kDate = str(d::s_DATE);      // "DATE"

struct H {
    // ------------------------------------------------------------------ 0x004d9e30
    char* weekday(u32 dow, u32 abbreviated) {
        if ((abbreviated & 0xffu) != 0) return text1(this, kDate, 5, dow);
        return text1(this, kDate, 6, dow);
    }
};

static void JOAN_CDECL init_a3a32c() {                              // 0x004d9770
    global<float>(d::g_a3a32c) = global<float>(d::g_a3a328) / global<float>(d::flt_2);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_06", name, addr, fn)
B("init_a3a32c", 0x004d9770, &init_a3a32c);
B("TxtRes::weekday", 0x004d9e30, &H::weekday);
#undef B

}  // namespace game_b100_06
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
namespace {
namespace game_b101_06 {

using joan::call::cdecl_call;

using joan::call::cdecl_call;

// ------------------------------------------------------------------------ 0x004d97a0
void JOAN_CDECL init_4d97a0() { joan::orig::update_third(); }  // direct call in the original

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("init_4d97a0", 0x004d97a0, &init_4d97a0);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

using joan::call::cdecl_call;

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL update_third() {  // 0x004d9a50
    global<float>(d::g_a3a330) = global<float>(d::g_a3a328) / 3.0f;  // dword 0x9cf798 = 3.0f
}

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* ampm_2() { return menu(d::s_AMPM, 2); }                               // 0x004d9ed0 "AMPM"
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("update_third", 0x004d9a50, &update_third);
B97("AmPm::open_2", 0x004d9ed0, &H::ampm_2);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_dsa_open, "Dsa::open");

const char* open_menu(void* self, const char* name, i32 n, i32 a, i32 b, u32 c) {
    return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), a, b, c));  // 0x005a3090
}

void JOAN_CDECL init_a3a334() {  // 0x004d9b50
    global<float>(d::g_a3a334) = global<float>(d::g_a3a328) / global<float>(d::flt_4);  // 4.0f
}

struct H {
    // ------------------------------------------------------------------------ 0x004da110
    const char* dsa_open(i32 a, i32 b, i32 c, u32 d, u32 e) {
        c_dsa_open.hit();
        const bool has_d = (d & 0xffu) != 0;
        const char* name;
        if ((e & 0xffu) == 0)
            name = has_d ? str(d::s_DSA) : str(d::s_D_A);  // "DSA" / "D-A"
        else
            name = has_d ? str(d::s_DS) : str(d::s_D_9d627c);  // "DS" / "D"
        i32 n = 1;
        if (has_d) {
            if (b <= 0 && c <= 0)
                n = 1;
            else if (c <= 0)
                n = 2;
            else
                n = 3;
        }
        return open_menu(this, name, n, c, b, static_cast<u32>(a));
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("init_a3a334", 0x004d9b50, &init_a3a334);
B96("Dsa::open", 0x004da110, &H::dsa_open);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b99_04.cpp (game/b99_04)
namespace {
namespace game_b99_04 {

using joan::call::cdecl_call;

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
const char* open_menu(void* self, const char* name, i32 n, u32 a, u32 b, u32 c) {
    return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), a, b, c));  // 0x005a3090
}

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* date_1(u32 a, u32 b, u32 c) { return open_menu(this, str(d::s_DATE), 1, a, b, c); }  // 0x004d9d20 "DATE"
    const char* popup_1() { return menu(d::s_POPUP, 0x01); }     // 0x004da500 "POPUP"
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_04", name, addr, fn)
B99("Menu::date_1", 0x004d9d20, &H::date_1);
B99("Menu::popup_1", 0x004da500, &H::popup_1);
#undef B99

}  // namespace game_b99_04
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
namespace {
namespace game_b95_03 {

using joan::call::cdecl_call;

const char* open_menu(void* self, const char* name, i32 n, u32 a, u32 b, u32 c) {
    return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), a, b, c));  // 0x005a3090
}

struct H {
    const char* date_2(u32 d, u32 m, u32 y) { return open_menu(this, str(d::s_DATE), 2, d, m, y); }  // 0x004d9d70 "DATE"
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("Date::open_2", 0x004d9d70, &H::date_2);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_tsa_open, "Tsa::open");

const char* open_menu(void* self, const char* name, i32 n, i32 a, i32 b, u32 c) {
    return joan::orig::arg::str(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), a, b, c));  // 0x005a3090
}

struct H {
    // ------------------------------------------------------------------------ 0x004d9f30
    const char* tsa_open(i32 a, i32 b, u32 c, u32 d, u32 e) {
        c_tsa_open.hit();
        const bool has_d = (d & 0xffu) != 0;
        const char* name;
        if ((e & 0xffu) == 0)
            name = has_d ? str(d::s_TSA) : str(d::s_T_A);  // "TSA" / "T-A"
        else
            name = has_d ? str(d::s_TS_9d6270) : str(d::s_T);  // "TS" / "T"
        i32 n = 1;
        if (has_d) {
            if (b <= 0 && a <= 0)
                n = 1;
            else if (a <= 0)
                n = 2;
            else
                n = 3;
        }
        return open_menu(this, name, n, a, b, c);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("Tsa::open", 0x004d9f30, &H::tsa_open);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
namespace {
namespace game_b102_04 {

using joan::call::cdecl_call;
using joan::img::obj;

JOAN_COUNTER(c_say_reaction, "Unit::say_reaction");

struct H {
    // ------------------------------------------------------------------ 0x004da2f0
    const char* say_reaction(i32 a, i32 b, u32 c, u32 d) {
        c_say_reaction.hit();
        void* const tbl = obj(d::g_date);
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_date(tbl)->julian(0x7d0, 1, 1));  // 0x0057f990
        const i32 ab = joan::orig::arg::as<i32>(joan::orig::libc::abs(static_cast<i32>(static_cast<u32>(b) - static_cast<u32>(a))));  // abs
        const i32 x = static_cast<i32>(static_cast<u32>(ab) + static_cast<u32>(r));
        const i32 v3 = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_date_info(tbl)->year_4957e0(joan::orig::arg::i32(x))) - 0x7d0u);  // 0x004957e0
        const i32 v2 = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_date_info(tbl)->month_495810(joan::orig::arg::i32(x))) - 1u);      // 0x00495810
        const i32 v1 = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_pool(tbl)->alloc44(joan::orig::arg::u32(x))) - 1u);      // 0x00495840
        return joan::orig::arg::as<const char*>(joan::orig::as_txt_res(this)->format_dsa(joan::orig::arg::i32(v1), joan::orig::arg::i32(v2), joan::orig::arg::i32(v3), joan::orig::arg::u32(c & 0xffu), joan::orig::arg::u32(d & 0xffu)));   // 0x004da110
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("Unit::say_reaction", 0x004da2f0, &H::say_reaction);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_show_diff, "Clock::show_diff_4da3c0");

struct H {
    // ------------------------------------------------------------------ 0x004da3c0
    const char* show_diff_4da3c0(double a, double b, u32 c, u32 e) {
        c_show_diff.hit();
        const Fpu fpu{true};
        const double diff = fpu.sub(a, b);
        const double d = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(diff)));  // CRT fabs
        const i32 m = joan::hook::x87::ftol(d) / 60;
        const i32 m60 = static_cast<i32>(static_cast<u32>(m) * 60u);
        const i32 s = joan::hook::x87::ftol(d) - m60;
        double t = fpu.sub(d, static_cast<double>(m60));  // fild + fsubr qword
        t = fpu.sub(t, static_cast<double>(s));           // fisub
        t = fpu.mul(t, 60.0);                             // fmul qword [0x9d6290]
        const i32 h = joan::hook::x87::ftol(t);
        return joan::orig::arg::as<const char*>(joan::orig::as_txt_res(this)->format_tsa(joan::orig::arg::i32(m), joan::orig::arg::i32(s), joan::orig::arg::u32(h), joan::orig::arg::u32(c & 0xffu), joan::orig::arg::u32(e & 0xffu)));  // 0x004d9f30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Clock::show_diff_4da3c0", 0x004da3c0, &H::show_diff_4da3c0);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

struct H {
    // ------------------------------------------------------------------ 0x004da620
    u32 load_basic_txr() { joan::orig::as_txt_res(this)->load(joan::orig::arg::str(str(d::s_RESOURCE_T_BASIC_TXR))); return 0; }  // 0x005a21e0 "RESOURCE\\T_BASIC.TXR" (sole caller discards the result)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Obj::load_basic_txr_4da620", 0x004da620, &H::load_basic_txr);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_pi_a3a328() -> void { joan::bind::free<&::game_b98_03::init_pi_a3a328>(); }  // 0x4d9710
auto joan::orig::init_a3a32c() -> void { joan::bind::free<&::game_b100_06::init_a3a32c>(); }  // 0x4d9770
auto joan::orig::update_third() -> void { joan::bind::free<&::game_b97_03::update_third>(); }  // 0x4d9a50
auto joan::orig::init_a3a334() -> void { joan::bind::free<&::game_b96_03::init_a3a334>(); }  // 0x4d9b50
auto joan::orig::TxtRes::format_tsa(std::int32_t a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::tsa_open>(this, a0, a1, a2, a3, a4)); }  // 0x4d9f30
auto joan::orig::TxtRes::format_dsa(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::uint32_t a3, std::uint32_t a4) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_03::H::dsa_open>(this, a0, a1, a2, a3, a4)); }  // 0x4da110
auto joan::orig::Menu::popup_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::popup_1>(this)); }  // 0x4da500
auto joan::orig::TxtRes::load_basic_txr() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::load_basic_txr>(this)); }  // 0x4da620
