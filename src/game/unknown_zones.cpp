// game/unknown_zones.cpp: functions whose original source file is unknown (formerly game/_unplaced/zones.cpp;
// re/attribution.csv: between ww/uti3d/t_scrres.cpp and ww/trackres.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using u8 = std::uint8_t;

using i16 = std::int16_t;

}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

struct H {
    void* ctor_5843d0() {  // 0x004e48f0
        joan::orig::dynarray(this)->construct(static_cast<i32>(sizeof(void*)), i32{0x1e}, i32{0});  // 0x005843d0 (ZoneDef* records: 4 on x86)
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("X::ctor_5843d0", 0x004e48f0, &H::ctor_5843d0);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x004e4920
    void dtor_e49() {
        joan::orig::as_list(this)->delete_all_4e49b0();  // 0x004e49b0
        joan::orig::dynarray(this)->destruct();   // 0x005844b0
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("X::dtor_e49", 0x004e4920, &H::dtor_e49);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_4e4990", 0x004e4990, &H::nop);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::call::thiscall;
using joan::call::cdecl_call;

JOAN_COUNTER(c_delete_all, "List::delete_all");

using joan::orig::helpers::game_delete;                      // operator delete

struct H {
    // ------------------------------------------------------------------ 0x004e49b0
    std::uint32_t delete_all_4e49b0() {
        c_delete_all.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_dynarray(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;           // 0x004217e0
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get_420ff0(joan::orig::arg::i32(i)));                      // 0x00420ff0
            game_delete(p);
            joan::orig::dynarray(this)->linkout(joan::orig::arg::i32(i));  // 0x00584a10
        }
        joan::orig::as_dynarray(this)->zap_584580(1);  // 0x00584580
        return 0;  // void callee; callers ignore the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("List::delete_all_4e49b0", 0x004e49b0, &H::delete_all_4e49b0);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using joan::call::cdecl_call;

using joan::orig::helpers::memset_;  // memset

JOAN_COUNTER(c_new_e0, "X::new_e0");

struct H {
    // ------------------------------------------------------------------------ 0x004e4a50
    u32 new_e0() {
        c_new_e0.hit();
        u32* p = joan::orig::arg::as<u32*>(joan::crt::operator_new(static_cast<u32>(sizeof(jo::ZoneDef))));  // operator new (0xe0)
        memset_(p, 0, static_cast<u32>(sizeof(jo::ZoneDef)));
        joan::orig::dynarray(this)->linkin(&p);                   // 0x00584620
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        *p = v;
        return *p;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("X::new_e0", 0x004e4a50, &H::new_e0);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;

// ------------------------------------------------------------------------------- cdecl
i32 JOAN_CDECL cmp_name_4(char* const* a, char* const* b) {  // 0x004e59d0
    return joan::crt::stricmp(joan::orig::arg::str(*a + 4), joan::orig::arg::str(*b + 4));   // _stricmp
}

struct H {
    // ------------------------------------------------------------------------ 0x004e4ac0
    u32 remove_key(u32 key) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get_420ff0(joan::orig::arg::i32(key)));  // 0x00420ff0
        joan::crt::operator_delete(joan::orig::arg::mptr(p));                         // operator delete
        joan::orig::as_dynarray(this)->linkout_584a10(joan::orig::arg::i32(key));  // 0x00584a10
        return 0;  // void callee; 0x004e4ac0 is unreferenced
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("X::remove_key", 0x004e4ac0, &H::remove_key);
B("cmp_name_4", 0x004e59d0, &cmp_name_4);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

using joan::call::thiscall;
using joan::call::cdecl_call;

JOAN_COUNTER(c_find_by_name, "Table::find_by_name");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x004e4b20
    i32 find_by_name(const char* name) {
        c_find_by_name.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_dynarray(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;   // 0x004217e0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get_420ff0(joan::orig::arg::i32(i)));          // 0x00420ff0
            if (stricmp_(e + 4, name) == 0) return i;
        }
        return 0;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Table::find_by_name", 0x004e4b20, &H::find_by_name);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b106_11.cpp (game/b106_11)
namespace {
namespace game_b106_11 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kSms = obj(d::g_strings);

u32 now() { return global<u32>(d::g_now); }

JOAN_COUNTER(c_zones, "Zones::update");

// ------------------------------------------------------------------ 0x004e4bb0 helpers
// this: the territory list g_a52940 (a DynArray) followed by the round-robin cursor (+0x1d on x86) and the time of
// the last owner check (+0x21 on x86); the Zones facade names them at their x86 offsets, inside the larger x64 DynArray.
std::int32_t& zone_cursor(void* self) { return *reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(self) + sizeof(jo::DynArray)); }
std::uint32_t& zone_checked(void* self) { return *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(self) + sizeof(jo::DynArray) + 4); }
// One zone shape test (record rec, shape j) at position (x, y), x87 PC24 semantics.
bool zone_shape_hit(const u8* rec, i32 j, float x, float y) {
    const Fpu fpu{true};
    auto I = [rec, j](i32 k) { return jo::as_zone_def(rec)->coords[k * 4 + j]; };  // point k (x, y, ... radius) of shape j
    auto D = [&I](i32 a, i32 b) {  // int a - int b (wrapping), as fimul operand
        return static_cast<double>(static_cast<i32>(static_cast<u32>(I(a)) - static_cast<u32>(I(b))));
    };
    const double X = x, Y = y;
    switch (jo::as_zone_def(rec)->shape_kind[j]) {
        case 0: {
            if (static_cast<double>(I(0)) > X) return false;
            if (!(static_cast<double>(I(2)) >= X)) return false;
            if (static_cast<double>(I(1)) > Y) return false;
            if (!(static_cast<double>(I(3)) >= Y)) return false;
            return true;
        }
        case 1: {
            const double dx = fpu.sub(X, static_cast<double>(I(0)));
            const double dx2 = fpu.mul(dx, dx);
            const double dy = fpu.sub(Y, static_cast<double>(I(1)));
            const double dy2 = fpu.mul(dy, dy);
            const double d2 = fpu.add(dx2, dy2);
            const i32 r2 = static_cast<i32>(static_cast<u32>(I(8)) * static_cast<u32>(I(8)));
            return static_cast<double>(r2) > d2;
        }
        case 2: {
            // edge (a -> b): (X - ax) * (by - ay) - (Y - ay) * (bx - ax) < 0 (or unordered)
            auto edge = [&](i32 ax, i32 ay, i32 bx, i32 by) {
                const double l = fpu.mul(fpu.sub(X, static_cast<double>(I(ax))), D(by, ay));
                const double r = fpu.mul(fpu.sub(Y, static_cast<double>(I(ay))), D(bx, ax));
                return fpu.sub(l, r);
            };
            if (edge(0, 1, 2, 3) >= 0.0) return false;
            if (edge(2, 3, 4, 5) >= 0.0) return false;
            if (edge(4, 5, 6, 7) >= 0.0) return false;
            if (edge(6, 7, 0, 1) >= 0.0) return false;
            return true;
        }
        default:
            return false;
    }
}

// Shape loop of one zone (count re-read every iteration, stops at the first hit).
bool zone_hit(const u8* rec, float x, float y) {
    for (i32 j = 0; j < static_cast<i32>(jo::as_zone_def(rec)->shape_count); ++j) {
        if (zone_shape_hit(rec, j, x, y)) return true;
    }
    return false;
}

// SMS wrappers selected by (char)+0x26 (jump tables 0x4e566b / 0x4e5683, identical).
void zone_message(u8* rec) {
    const char* const arg = jo::as_zone_def(rec)->name;
    switch (static_cast<u32>(static_cast<i32>(jo::as_zone_def(rec)->kind))) {
        case 0: joan::orig::as_sms(kSms)->open_06(arg); break;  // 0x004dd760
        case 1: joan::orig::as_sms(kSms)->open_05(arg); break;  // 0x004dd720
        case 2: joan::orig::as_sms(kSms)->open_0a(arg); break;  // 0x004dd860
        case 3: joan::orig::as_menu(kSms)->sms_9(arg); break;  // 0x004dd820
        case 4: joan::orig::as_menu(kSms)->sms_7(arg); break;  // 0x004dd7a0
        case 5: joan::orig::as_sms(kSms)->open_08(arg); break;  // 0x004dd7e0
        default: break;
    }
}

struct H {

    // ------------------------------------------------------------------ 0x004e4bb0
    i32 count() { return joan::orig::as_dynarray(this)->size(); }  // 0x00417ad0

    void zones_update() {
        c_zones.hit();
        if (count() <= 0) return;
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))) == 0) {  // 0x00428b70
            u8* const player = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))));  // 0x0041b3c0
            float pos[3];
            joan::orig::as_unit(player)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
            for (i32 i = count(); i > 0; --i) {
                if (joan::orig::arg::as<i32>(joan::orig::as_dynarray(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x004217e0
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get_420ff0(joan::orig::arg::i32(i)));   // 0x00420ff0
                i8 hit = 0;
                if (jo::as_zone_def(rec)->shape_count != 0) hit = zone_hit(rec, pos[0], pos[1]) ? 1 : 0;
                if (now() - zone_checked(this) >= 0xfa0u) {
                    if (hit != 0 && jo::as_zone_def(rec)->flag_47 == 0) {
                        zone_message(rec);
                        zone_checked(this) = now();
                    } else if (hit == 0 && jo::as_zone_def(rec)->flag_47 != 0) {
                        zone_message(rec);
                        zone_checked(this) = now();
                    }
                }
                jo::as_zone_def(rec)->flag_47 = hit;
                if (hit != 0) jo::as_unit(player)->zone = static_cast<i16>(i);
            }
        }
        // 0x004e5111: round-robin owner zones
        if (zone_cursor(this) > count()) {
            zone_cursor(this) = count();
        } else if (zone_cursor(this) == 0) {
            zone_cursor(this) = 1;
        }
        i32 k = zone_cursor(this);
        for (;;) {
            if (joan::orig::arg::as<i32>(joan::orig::as_dynarray(this)->is_empty_at(joan::orig::arg::i32(k))) == 0) {           // 0x004217e0
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(this)->get_420ff0(joan::orig::arg::i32(k)));  // 0x00420ff0
                i8 hit = 0;
                if (jo::as_zone_def(rec)->owner != -1 && jo::view<i8>(jo::as_zone_def(rec)->disable) == 0) {
                    if (jo::as_zone_def(rec)->shape_count != 0) hit = zone_scan_units(rec);
                    if (hit != 0 && jo::as_zone_def(rec)->flag_46 == 0) jo::as_zone_def(rec)->time = now();
                    jo::as_zone_def(rec)->flag_46 = hit;
                    zone_cursor(this) = zone_cursor(this) + 1;
                    if (zone_cursor(this) > count()) zone_cursor(this) = 1;
                    return;
                }
            }
            ++k;
            if (k > count()) k = 1;
            if (k == zone_cursor(this)) return;
        }
    }

    static i8 zone_scan_units(u8* rec) {
        for (i32 u = joan::orig::as_dynarray(kUnits)->size(); u > 0; --u) {  // 0x00417ad0
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(u)));  // 0x0041b3f0
            if (unit == nullptr) continue;
            if (jo::as_unit(unit)->move_mode > 3) continue;
            if (static_cast<i32>(jo::as_unit(unit)->side) == jo::as_zone_def(rec)->owner) continue;
            u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
            if (jo::as_unit_type(type)->kind == 2) continue;
            if (jo::as_unit_type(type)->kind == 7) continue;
            if (jo::as_unit_type(type)->unit_class == 0) continue;
            if (jo::as_unit_type(type)->unit_class == 8) continue;
            if (joan::orig::arg::as<u8>(joan::orig::as_game(unit)->byte_2e1()) != 0) continue;  // 0x00432b90
            if (jo::as_unit_type(type)->unit_class == 9) continue;
            if (joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->dead()) != 0) continue;  // 0x0042d0e0
            if (jo::as_unit(unit)->side != 0) {
                void* const owner = joan::orig::arg::as<void*>(joan::orig::as_unit(unit)->nation());                  // 0x00509400
                u8* const rel = joan::orig::arg::as<u8*>(joan::orig::as_player(owner)->relation(joan::orig::arg::i32(jo::as_zone_def(rec)->owner)));  // 0x004b42e0
                if (rel != nullptr && jo::as_rel(rel)->attitude == 4) continue;
            }
            float pos[3];
            joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
            if (zone_hit(rec, pos[0], pos[1])) return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_11", name, addr, fn)
B("Zones::update", 0x004e4bb0, &H::zones_update);
#undef B

}  // namespace game_b106_11
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using joan::call::thiscall;

JOAN_COUNTER(c_init_index, "List::init_index");

struct H {
    // ------------------------------------------------------------------------ 0x004e5960
    void init_index(std::uintptr_t a) {  // the comparator's code address (pointer-sized through the facade)
        c_init_index.hit();
        joan::orig::dynarray(this)->quick_sort(joan::orig::arg::cmp(a));  // 0x005850b0
        for (i16 i = static_cast<i16>(joan::orig::as_dynarray(this)->size()); static_cast<i32>(i) >= 1;  // 0x00417ad0
             i = static_cast<i16>(i - 1)) {
            const i32 v = i;
            *joan::orig::arg::as<i32*>(joan::orig::as_dynarray(this)->get_420ff0(static_cast<i32>(i))) = v;  // 0x00420ff0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("List::init_index", 0x004e5960, &H::init_index);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::List::delete_all_4e49b0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_03::H::delete_all_4e49b0>(this)); }  // 0x4e49b0
auto joan::orig::Zones::add_new() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_04::H::new_e0>(this)); }  // 0x4e4a50
auto joan::orig::Table::find_by_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::find_by_name>(this, a0)); }  // 0x4e4b20
auto joan::orig::List::init_index(std::uintptr_t a0) -> void { joan::bind::member<&::game_b96_04::H::init_index>(this, a0); }  // 0x4e5960
