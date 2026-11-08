// game/unknown_cheats.cpp: functions whose original source file is unknown (formerly game/_unplaced/cheats.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::img::str;

using joan::hook::global;

using joan::img::obj;

using i32 = std::int32_t;

using i16 = std::int16_t;

void* const kUnitTable = obj(d::g_units);

using joan::call::cdecl_call;

using u16 = std::uint16_t;
using i8 = std::int8_t;

void* const kTypes = obj(d::g_types);

using joan::orig::helpers::stricmp_;  // _stricmp

using joan::hook::x87::ftol;

}  // namespace

// ---------------------------------------- from modules/game_b106_17.cpp (game/b106_17)
namespace {
namespace game_b106_17 {

struct H {
    // 0x00454210
    u32 dtor_454210() {
        joan::orig::as_mouse_cursor(this)->release_4941b0();                                       // 0x004941b0
        return joan::orig::arg::as<u32>(joan::orig::as_resource_db(&jo::self<jo::MouseCursor>(this)->res_cursor)->destruct());  // 0x00596380
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_17", name, addr, fn)
B("Obj::dtor_454210", 0x00454210, &H::dtor_454210);
#undef B

}  // namespace game_b106_17
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

struct H {
    // ------------------------------------------------------------------ tiny methods
    void nop() {}
    void* self() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::self_454270", 0x00454270, &H::self);
B("Obj::nop_454290", 0x00454290, &H::nop);
B("Obj::nop_4542b0", 0x004542b0, &H::nop);
B("Obj::nop_4542d0", 0x004542d0, &H::nop);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }

JOAN_COUNTER(c_next_word, "Text::next_word");

struct H {
    // 0x004542f0 (this unused)
    std::uint32_t next_word(const char* s, std::uint32_t pos, char* out) {
        c_next_word.hit();
        if (!(pos < strlen_(s))) return 0;
        const char* const p = s + pos;
        if (*p == 0) return 0;
        std::uint32_t i = 0;
        for (; i < strlen_(p); ++i) {
            out[i] = p[i];
            if (p[i] == ' ') {
                out[i] = 0;
                return pos + i;
            }
        }
        out[i] = 0;
        return pos + i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Text::next_word", 0x004542f0, &H::next_word);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

JOAN_COUNTER(c_parse_quoted, "parse_quoted");
JOAN_COUNTER(c_units_1dd, "units_set_1dd");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
using joan::orig::helpers::strlen_;

struct H {
    // ------------------------------------------------------------------ 0x004543c0
    u32 parse_quoted(const char* s, u32 off, char* out) {
        c_parse_quoted.hit();
        if (!(off < strlen_(s))) return 0;
        const char* p = s + off;
        if (strlen_(p) < 2u) return 0;
        if (*p != 0x22) return 0;
        ++p;
        for (u32 i = 0; i < strlen_(p); ++i) {
            if (p[i] == 0x22) {
                out[i] = 0;
                return off + i + 2u;
            }
            out[i] = p[i];
        }
        out[0] = 0;
        return 0;
    }

    // ------------------------------------------------------------------ 0x00455630
    void units_set_1dd(const char* s, const char* k) {
        c_units_1dd.hit();
        if (k != nullptr) {
            const i32 key = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(k));  // 0x004b91a0
            const i32 cnt = joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(key)));  // 0x004b67e0
            for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
                if (unit_missing(i) != 0) continue;
                if (static_cast<i32>(jo::as_unit(unit_get(i))->side) != cnt) continue;
                if (s != nullptr && *s == 0x31) {
                    jo::view<u32>(jo::as_unit(unit_get(i))->hit_value) = 0x3b9ac9ff;
                } else if (s != nullptr && *s == 0x30) {
                    jo::view<u32>(jo::as_unit(unit_get(i))->hit_value) = 0;
                }
            }
            return;
        }
        if (unit_missing(global<i32>(d::g_player_unit_recno)) != 0) return;
        if (s != nullptr && *s == 0x31) {
            jo::view<u32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->hit_value) = 0x3b9ac9ff;
        } else if (s != nullptr && *s == 0x30) {
            jo::view<u32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->hit_value) = 0;
        }
    }

    // ------------------------------------------------------------------ 0x00455d90
    u32 update_all(const char* a, const char* b) {
        joan::orig::as_console(this)->cheat_unit_add_hp(a, b);  // 0x00455e30
        joan::orig::as_console(this)->cheat_unit_add_ep(a, b);  // 0x00455f50
        joan::orig::as_console(this)->cheat_unit_add_str(a, b);  // 0x004561d0
        joan::orig::as_console(this)->cheat_unit_add_def(a, b);  // 0x00456330
        joan::orig::as_console(this)->cheat_unit_add_dex(a, b);  // 0x00456480
        joan::orig::as_console(this)->cheat_unit_add_lea(a, b);  // 0x00456590
        return 0;  // void callee; the result reaches Cheats::dispatch / Console::execute, whose callers drop it
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("parse_quoted", 0x004543c0, &H::parse_quoted);
B97("units_set_1dd", 0x00455630, &H::units_set_1dd);
B97("Obj::update_all", 0x00455d90, &H::update_all);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

JOAN_COUNTER(c_skip_spaces, "skip_spaces");
JOAN_COUNTER(c_add_gold, "add_gold");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
using joan::orig::helpers::strlen_;
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

struct H {
    // ------------------------------------------------------------------ 0x004544c0
    u32 skip_spaces(const char* s, u32 off) {
        c_skip_spaces.hit();
        if (!(off < strlen_(s))) return 0;
        const char* const p = s + off;
        if (*p == 0) return 0;
        for (u32 i = 0; i < strlen_(p); ++i) {
            if (p[i] != 0x20) return off + i;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x004555c0
    u32 player_unit_refresh() {
        const i32 m = unit_missing(global<i32>(d::g_player_unit_recno));
        if (m != 0) return static_cast<u32>(m);
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->hit_points_max());  // 0x00523320
        joan::orig::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->set_value_135(joan::orig::arg::i32(v));  // 0x004fe510
        return 0;  // void callee; result dropped by Cheats::dispatch callers
    }

    // ------------------------------------------------------------------ 0x004566a0
    i32 add_gold(const char* s, const char* k) {
        c_add_gold.hit();
        i32 n;
        if (k != nullptr) {
            joan::orig::as_list(kUnitTable)->find_name_51fdd0(joan::orig::arg::str(k));  // 0x0051fdd0 (result dead)
            n = atoi_(s);
        } else {
            n = atoi_(s);
        }
        const i32 result = n;
        i32& gold = global<i32>(d::g_get_a596c4);
        if (n <= 0 || gold > 99999999) return result;
        if (n > 99999999) n = 99999999;
        if (gold + n > 99999999 && gold < 99999999) {
            gold = 99999999;
            return result;
        }
        gold = gold + n;
        return result;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("skip_spaces", 0x004544c0, &H::skip_spaces);
B98("player_unit_refresh", 0x004555c0, &H::player_unit_refresh);
B98("add_gold", 0x004566a0, &H::add_gold);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b111_13.cpp (game/b111_13)
namespace {
namespace game_b111_13 {

const char* rstr(img::Addr a) { return &img::global<const char>(a); }

constexpr img::Addr kUtil(d::g_misc);

JOAN_COUNTER(c_console, "Console::execute");

struct H {
    // ------------------------------------------------------------------------ 0x00454560
    struct ConsoleFrame {  // original order: [ebp-0x104], [ebp-0xc4], [ebp-0x84], [ebp-0x44]
        char b104[64];
        char bc4[64];
        char b84[64];
        char b44[64];
    };
    u32 run(char* word, char* arg1, char* arg2) {
        return joan::orig::arg::as<u32>(joan::orig::as_console(this)->run_cheat(joan::orig::arg::str(word), joan::orig::arg::str(arg1), joan::orig::arg::str(arg2)));  // 0x00454c80
    }
    u32 tok_word(char* s, u32 pos, char* out) { return joan::orig::arg::as<u32>(joan::orig::as_text(this)->next_word(joan::orig::arg::str(s), joan::orig::arg::u32(pos), joan::orig::arg::str(out))); }  // 0x004542f0
    u32 tok_quoted(char* s, u32 pos, char* out) { return joan::orig::arg::as<u32>(joan::orig::as_console(this)->parse_quoted(joan::orig::arg::str(s), joan::orig::arg::u32(pos), joan::orig::arg::str(out))); }  // 0x004543c0
    u32 skip_spaces(char* s, u32 pos) { return joan::orig::arg::as<u32>(joan::orig::as_console(this)->skip_spaces(joan::orig::arg::str(s), joan::orig::arg::u32(pos))); }  // 0x004544c0

    u32 console_execute(const char* cmd) {
        // Inputs of 64+ chars would overflow the frame; they cannot occur (console TextBox max
        // length 0x40, see the header). Dead case: run on the first 63 characters.
        char clipped[64];
        if (std::strlen(cmd) >= 64) {
            std::memcpy(clipped, cmd, 63);
            clipped[63] = 0;
            cmd = clipped;
        }
        c_console.hit();
        ConsoleFrame fr;
        char* const b44 = fr.b44;
        char* const b84 = fr.b84;
        char* const bc4 = fr.bc4;
        char* const b104 = fr.b104;
        b84[0] = 0;
        bc4[0] = 0;
        b104[0] = 0;
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(b44)), joan::orig::arg::str(cmd));  // strcpy
        joan::orig::as_misc(img::obj(kUtil))->ltrim_58f680(joan::orig::arg::str(b44));  // 0x0058f680 ltrim
        joan::orig::as_misc(img::obj(kUtil))->rtrim_58f610(joan::orig::arg::str(b44));  // 0x0058f610 rtrim
        u32 i = 0;
        for (; i < joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(b44))); ++i) {  // strlen each pass
            b84[i] = b44[i];
            if (b44[i] == ' ') break;
            b44[i] = ' ';
        }
        b84[i] = 0;
        joan::orig::as_misc(img::obj(kUtil))->ltrim_58f680(joan::orig::arg::str(b44));  // ltrim
        auto cmp = [&](img::Addr s) { return joan::crt::stricmp(joan::orig::arg::str(b84), joan::orig::arg::str(rstr(s))); };  // _stricmp
        if (cmp(d::s_ep) == 0 || cmp(d::s_hp) == 0 || cmp(d::s_add_command_point) == 0 || cmp(d::s_toggle_fog_of_war) == 0) {
            return run(b84, nullptr, nullptr);
        }
        if (cmp(d::s_kill_team) == 0 || cmp(d::s_kill_unit_recno) == 0 || cmp(d::s_objective_finish) == 0) {
            const u32 r = tok_word(b44, 0, bc4);
            if (r == 0) return r;
            return run(b84, bc4, nullptr);
        }
        if (cmp(d::s_kill_all) == 0 || cmp(d::s_kill_unit_in_place) == 0 || cmp(d::s_capture_place) == 0 || cmp(d::s_del_item) == 0 ||
            cmp(d::s_kill_unit) == 0) {
            const u32 r = tok_quoted(b44, 0, bc4);
            if (r == 0) return r;
            return run(b84, bc4, nullptr);
        }
        if (cmp(d::s_unit_level_up) == 0 || cmp(d::s_unit_add_all_att) == 0 || cmp(d::s_unit_add_strength) == 0 || cmp(d::s_unit_add_hp) == 0 ||
            cmp(d::s_unit_add_ep) == 0 || cmp(d::s_unit_add_str) == 0 || cmp(d::s_unit_add_def) == 0 || cmp(d::s_unit_add_dex) == 0 ||
            cmp(d::s_unit_add_lea) == 0 || cmp(d::s_unit_add_gold) == 0 || cmp(d::s_invincible) == 0) {
            u32 r = tok_word(b44, 0, bc4);
            if (r == 0) return r;
            r = skip_spaces(b44, r);
            if (r == 0) return run(b84, bc4, nullptr);
            r = tok_quoted(b44, r, b104);
            if (r != 0) return run(b84, bc4, b104);
            return run(b84, bc4, nullptr);
        }
        if (cmp(d::s_unit_add_item) == 0) {  // "unit_add_item"
            u32 r = tok_quoted(b44, 0, bc4);
            if (r == 0) return r;
            r = skip_spaces(b44, r);
            if (r == 0) return run(b84, bc4, nullptr);
            r = tok_word(b44, r, b104);
            if (r != 0) return run(b84, bc4, b104);
            return run(b84, bc4, nullptr);
        }
        const i32 c = cmp(d::s_enable_fog_of_war);  // "enable_fog_of_war"
        if (c != 0) return static_cast<u32>(c);
        const u32 r = tok_word(b44, 0, bc4);
        if (r == 0) return r;
        return run(b84, bc4, nullptr);
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_13", name, addr, fn)
B111("Console::execute", 0x00454560, &H::console_execute);
#undef B111

}  // namespace game_b111_13
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_detach_rider, "Unit::detach_rider");
JOAN_COUNTER(c_reset_unit, "Obj::reset_unit");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x00454b20
    void detach_rider() {
        c_detach_rider.hit();
        (void)joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id));  // 0x0042d0a0 (result unused)
        if (unit_missing(jo::self<jo::Unit>(this)->mount_ref) == 0) {
            if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::i16(0));  // 0x006ad980
                jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode = 0;
                u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
                jo::as_shot(s)->hit = 0;
            }
            u8* const rider = unit_get(jo::self<jo::Unit>(this)->mount_ref);
            const float h = joan::orig::arg::as<float>(joan::orig::as_ground_object(rider)->heading());  // 0x004ff850
            joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(h));                       // 0x004ff870
            joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(h / global<float>(d::flt_0_017453292)));  // 0x0050abd0
        }
        if (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 1 && jo::view<u32>(jo::self<jo::Unit>(this)->rider_ref) != 0 && unit_missing(jo::self<jo::Unit>(this)->rider_ref) == 0) {
            u8* const other = unit_get(jo::self<jo::Unit>(this)->rider_ref);
            joan::orig::as_unit(other)->set_action(0x68, 0, 1);  // 0x00501190
        }
    }

    // ------------------------------------------------------------------ 0x00455a60
    void reset_unit(const char* key) {
        c_reset_unit.hit();
        const i32 id = joan::orig::arg::as<i32>(joan::orig::as_list(kUnits)->find_name_51fdd0(key));  // 0x0051fdd0
        if (unit_missing(id) != 0) return;
        u8* const unit = unit_get(id);
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->dead()) != 0) return;  // 0x0042d0e0
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
        if ((jo::as_unit_type(type)->trigger_flags & 1u) == 0) return;
        jo::view<u32>(jo::as_unit(unit)->state_120) = 0;
        joan::orig::as_unit(unit)->set_value_135(0);                          // 0x004fe510
        joan::orig::as_unit(unit)->die_7c6240(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));    // 0x007c6240
    }

    // ------------------------------------------------------------------ 0x00456960
    u32 copy_a3cc64() {
        const u32 v = global<u32>(d::g_unit_max_com);
        global<u32>(d::g_command_points) = v;
        return v;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Unit::detach_rider", 0x00454b20, &H::detach_rider);
B95("Obj::reset_unit", 0x00455a60, &H::reset_unit);
B95("Obj::copy_a3cc64", 0x00456960, &H::copy_a3cc64);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

JOAN_COUNTER(c_cheats, "Cheats::dispatch");

i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }                       // atoi

struct H {
    // --------------------------------------------------------------------- 0x00454c80
    // Commands with (a, b-or-0) after atoi(a) > 0.
    u32 cheat_ab(std::uintptr_t thunk, const char* a, const char* b) {
        const i32 n = atoi_(a);
        if (n <= 0) return static_cast<u32>(n);
        if (b != nullptr) return thiscall<u32>(thunk, this, a, b);
        return thiscall<u32>(thunk, this, a, static_cast<const char*>(nullptr));
    }
    // Commands with (a) after atoi(a) > 0.
    u32 cheat_a_pos(std::uintptr_t thunk, const char* a) {
        const i32 n = atoi_(a);
        if (n <= 0) return static_cast<u32>(n);
        return thiscall<u32>(thunk, this, a);
    }
    u32 cheats(const char* cmd, const char* a, const char* b) {
        c_cheats.hit();
        i32 r;
        if ((r = stricmp_(cmd, str(d::s_ep))) == 0) return joan::orig::arg::as<u32>(joan::orig::as_console(this)->cheat_ep());  // 0x00455550
        if ((r = stricmp_(cmd, str(d::s_hp))) == 0) return joan::orig::arg::as<u32>(joan::orig::as_console(this)->cheat_hp());  // 0x004555c0
        if ((r = stricmp_(cmd, str(d::s_invincible))) == 0) { joan::orig::as_console(this)->cheat_invincible(a, b); return 0; }  // 0x00455630 invincible
        if ((r = stricmp_(cmd, str(d::s_unit_level_up))) == 0) return cheat_ab(fn::code_value(fn::set_unit_value), a, b);  // 0x00455cc0 unit_level_up
        if ((r = stricmp_(cmd, str(d::s_unit_add_hp))) == 0) return cheat_ab(fn::code_value(fn::Cheat_add_gold), a, b);  // 0x00455e30 unit_add_hp
        if ((r = stricmp_(cmd, str(d::s_unit_add_ep))) == 0) return cheat_ab(fn::code_value(fn::cmd_add_level), a, b);  // 0x00455f50 unit_add_ep
        if ((r = stricmp_(cmd, str(d::s_unit_add_str))) == 0) return cheat_ab(fn::code_value(fn::Cheat_add_food), a, b);  // 0x004561d0 unit_add_str
        if ((r = stricmp_(cmd, str(d::s_unit_add_def))) == 0) return cheat_ab(fn::code_value(fn::Cheat_add_wood), a, b);  // 0x00456330 unit_add_def
        if ((r = stricmp_(cmd, str(d::s_unit_add_dex))) == 0) return cheat_ab(fn::code_value(fn::Cheat_add_stone), a, b);  // 0x00456480 unit_add_dex
        if ((r = stricmp_(cmd, str(d::s_unit_add_lea))) == 0) return cheat_ab(fn::code_value(fn::Cheat_add_iron), a, b);  // 0x00456590 unit_add_lea
        if ((r = stricmp_(cmd, str(d::s_unit_add_gold))) == 0) return cheat_ab(fn::code_value(fn::add_gold), a, b);  // 0x004566a0 unit_add_gold
        if ((r = stricmp_(cmd, str(d::s_enable_fog_of_war))) == 0) { joan::orig::as_console(this)->cheat_enable_fog_of_war(joan::orig::arg::str(a)); return 0; }  // 0x00456990 enable_fog_of_war
        if ((r = stricmp_(cmd, str(d::s_toggle_fog_of_war))) == 0) { joan::orig::as_console(this)->cheat_toggle_fog_of_war(); return 0; }  // 0x00456a40 toggle_fog_of_war
        if ((r = stricmp_(cmd, str(d::s_unit_add_all_att))) == 0) return cheat_ab(fn::code_value(fn::Obj_update_all), a, b);  // 0x00455d90 unit_add_all_att
        if ((r = stricmp_(cmd, str(d::s_add_command_point))) == 0) return joan::orig::arg::as<u32>(joan::orig::as_console(this)->cheat_add_command_point());  // 0x00456960 add_command_point
        if ((r = stricmp_(cmd, str(d::s_kill_all))) == 0) { joan::orig::as_units(this)->hide_language(joan::orig::arg::str(a)); return 0; }  // 0x00455340 kill_all
        if ((r = stricmp_(cmd, str(d::s_kill_team))) == 0) return cheat_a_pos(fn::code_value(fn::cmd_reset_units_of_kind), a);  // 0x004557e0 kill_team
        if ((r = stricmp_(cmd, str(d::s_kill_unit_in_place))) == 0) { joan::orig::as_console(this)->cheat_kill_unit_in_place(a); return 0; }  // 0x004558f0 kill_unit_in_place
        if ((r = stricmp_(cmd, str(d::s_kill_unit))) == 0) { joan::orig::as_console(this)->cheat_kill_unit(a); return 0; }  // 0x00455a60 kill_unit
        if ((r = stricmp_(cmd, str(d::s_kill_unit_recno))) == 0) return cheat_a_pos(fn::code_value(fn::Units_hide_one), a);  // 0x00455b30 kill_unit_recno
        if ((r = stricmp_(cmd, str(d::s_objective_finish))) == 0) return cheat_a_pos(fn::code_value(fn::Obj_mark_table_done), a);  // 0x00455c00 objective_finish
        if ((r = stricmp_(cmd, str(d::s_unit_add_item))) == 0) {  // unit_add_item
            if (b != nullptr) {
                const i32 n = atoi_(b);
                if (n <= 0) return static_cast<u32>(n);
                return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->spawn_label(a, joan::orig::arg::str(b)));  // 0x00456780
            }
            return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->spawn_label(a, joan::orig::arg::str(static_cast<const char*>(nullptr))));  // 0x00456780
        }
        if ((r = stricmp_(cmd, str(d::s_capture_place))) == 0) { joan::orig::as_console(this)->cheat_capture_place(joan::orig::arg::str(a)); return 0; }  // 0x00456ae0 capture_place
        if ((r = stricmp_(cmd, str(d::s_del_item))) == 0) return joan::orig::arg::as<u32>(joan::orig::as_console(this)->cheat_del_item(joan::orig::arg::str(a)));  // 0x00456b90 del_item
        return static_cast<u32>(r);
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Cheats::dispatch", 0x00454c80, &H::cheats);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
namespace {
namespace game_world4 {

constexpr img::Addr kUnits(d::g_items);

JOAN_COUNTER(c_release, "Effect::release");

struct H {
    // 0x004551f0 Effect::release.
    void effect_release() {  // the object is a Missile (Obj::init_4e6fb0 calls it on one): its layout, not the Effect view
        c_release.hit();
        if (jo::view<void*>(jo::self<jo::Missile>(this)->trail) != nullptr) {
            joan::orig::as_trail(jo::view<void*>(jo::self<jo::Missile>(this)->trail))->release();  // 0x00687410
            void* const obj = jo::view<void*>(jo::self<jo::Missile>(this)->trail);
            if (obj != nullptr) joan::orig::as_trail(obj)->destroy(1u);  // 0x00420790 delete
            jo::view<void*>(jo::self<jo::Missile>(this)->trail) = nullptr;
        }
        if (joan::orig::as_units(img::obj(kUnits))->list.empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Missile>(this)->object_id))) == 0) {  // 0x00425d50
            std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnits))->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Missile>(this)->object_id))));  // 0x00425d20
            if (jo::view<void*>(jo::as_map_object(unit)->actor) != nullptr)
                joan::orig::as_scene(jo::as_map_object(unit)->actor->scene_params0)->nop(0);  // 0x005eb590 (empty; its argument, the actor pointer on x86, is ignored)
            joan::orig::as_units(img::obj(kUnits))->remove_49f370(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Missile>(this)->object_id)));  // 0x0049f370
        }
        if (jo::self<jo::Missile>(this)->sound_handle != 0) {
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(img::obj(d::g_sfx_player))->entry(joan::orig::arg::i16(jo::view<std::int16_t[2]>(jo::self<jo::Missile>(this)->sound_handle)[0])));  // 0x004cf690
            joan::orig::as_stream_slot(slot)->release();  // 0x004cea40
            jo::self<jo::Missile>(this)->sound_handle = 0;
        }
        jo::self<jo::Missile>(this)->model_id = 0;
        jo::self<jo::Missile>(this)->object_id = 0;
        jo::self<jo::Missile>(this)->sound_handle = 0;
        jo::self<jo::Missile>(this)->shooter_unit_id = 0;
        jo::self<jo::Missile>(this)->shooter_record = 0;
    }
};

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("Effect::release", 0x004551f0, &H::effect_release);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_hide_lang, "Units::hide_language");
JOAN_COUNTER(c_hide_one, "Units::hide_one");
JOAN_COUNTER(c_view_rect, "Obj::set_view_rect");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

// Shared tail of 0x00455340 / 0x00455b30: hide a unit.
void hide_unit(u8* unit) {
    jo::view<u32>(jo::as_unit(unit)->state_120) = 0;
    joan::orig::as_unit(unit)->set_value_135(0);                          // 0x004fe510
    joan::orig::as_unit(unit)->die_7c6240(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));    // 0x007c6240
}
bool type_flag1(u8* unit) {
    u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
    return (jo::as_unit_type(t)->trigger_flags & 1u) != 0;
}

struct H {
    // ------------------------------------------------------------------ 0x00455340
    void hide_language(const char* name) {
        c_hide_lang.hit();
        void* const lookup = obj(d::g_nation_res);
        void* const groups = obj(d::g_nations);
        const i32 g = joan::orig::arg::as<i32>(joan::orig::as_list(groups)->find_by_word2(joan::orig::as_name_table(lookup)->find_4b91a0(joan::orig::arg::str(name))));  // 0x004b67e0(0x004b91a0)
        const i32 en = joan::orig::arg::as<i32>(joan::orig::as_list(groups)->find_by_word2(joan::orig::as_name_table(lookup)->find_4b91a0(str(d::s_english))));  // "english"
        const i32 fr = joan::orig::arg::as<i32>(joan::orig::as_list(groups)->find_by_word2(joan::orig::as_name_table(lookup)->find_4b91a0(str(d::s_french))));  // "french"
        bool others = false;
        if (stricmp_(name, str(d::s_others)) == 0) {  // "others"
            others = true;
        } else if (g == 0) {
            return;
        }
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            if (unit_missing(i) != 0) continue;
            u8* const unit = unit_get(i);
            if (!others && static_cast<i32>(jo::as_unit(unit)->side) == g) {
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->dead()) != 0) continue;  // 0x0042d0e0
                if (!type_flag1(unit)) continue;
                hide_unit(unit);
            } else if (others) {
                if (static_cast<i32>(jo::as_unit(unit)->side) == en) continue;
                if (static_cast<i32>(jo::as_unit(unit)->side) == fr) continue;
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->dead()) != 0) continue;
                hide_unit(unit);
            }
        }
    }

    // ------------------------------------------------------------------ 0x00455b30
    void hide_one(const char* text) {
        c_hide_one.hit();
        const i32 id = atoi_(text);
        if (unit_missing(id) != 0) return;
        u8* const unit = unit_get(id);
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->dead()) != 0) return;  // 0x0042d0e0
        if (!type_flag1(unit)) return;
        hide_unit(unit);
    }

    // ------------------------------------------------------------------ 0x00456990
    void set_view_rect(const char* text) {
        c_view_rect.hit();
        i32 v = 1;
        if (text != nullptr) v = atoi_(text);
        if (v != 0) {
            joan::orig::as_mini_map(obj(d::g_mini_map))->clear_reveal(0xa);  // 0x007198d0
            return;
        }
        const float* const src = jo::as_world(global<void*>(d::g_world))->bounds;
        float r[6];
        std::memcpy(r, src, sizeof r);
        const i32 a4 = joan::hook::x87::ftol(static_cast<double>(r[4]));
        const i32 a3 = joan::hook::x87::ftol(static_cast<double>(r[3]));
        const i32 a1 = joan::hook::x87::ftol(static_cast<double>(r[1]));
        const i32 a0 = joan::hook::x87::ftol(static_cast<double>(r[0]));
        joan::orig::as_mini_map(obj(d::g_mini_map))->set_reveal_rect(0xa, joan::orig::arg::u32(a0), joan::orig::arg::u32(a1), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4));  // 0x007196e0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Units::hide_language", 0x00455340, &H::hide_language);
B96("Units::hide_one", 0x00455b30, &H::hide_one);
B96("Obj::set_view_rect", 0x00456990, &H::set_view_rect);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

void* const kRandom = obj(d::g_misc);

using joan::orig::helpers::at;

void* const kItems = obj(d::g_items);

JOAN_COUNTER(c_argmax, "Table::argmax");
JOAN_COUNTER(c_convert_zone, "convert_units_in_zone");
JOAN_COUNTER(c_set_unit_value, "set_unit_value");
JOAN_COUNTER(c_remove_named, "remove_named");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------ 0x004554e0
    i32 argmax() {
        c_argmax.hit();
        i32 best = 0;                  // ebp-0x8
        // this = a Se / LodQueue entry (Top4: levels e0..e3 at +8 on x86, after the sample pointer)
        jo::Top4Entry* const levels = &jo::self<jo::Top4>(this)->e0;
        i32* bp = &levels[0].key;        // ebp-0xc
        for (i32 i = 1; i < static_cast<i32>(jo::self<jo::Top4>(this)->count); ++i) {
            if (*bp < levels[i].key) {
                bp = &levels[i].key;
                best = i;
            }
        }
        return best;
    }

    // ------------------------------------------------------------------ 0x004558f0
    void convert_units_in_zone(const char* arg) {
        c_convert_zone.hit();
        const i32 zone = joan::orig::arg::as<i32>(joan::orig::as_place_list(obj(d::g_place_array))->find(arg));  // 0x004c54a0
        if (zone == 0) return;
        for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
            if (unit_missing(i) != 0) continue;
            u8* const z = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(joan::orig::arg::i32(zone)));  // 0x00456c70
            u8* const u = unit_get(i);
            if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::as_place(z)->box)->contains(joan::orig::arg::ptr(jo::as_unit(u)->position_))) & 0xffu) == 0) continue;  // 0x0060d5f0
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead())) != 0) continue;         // 0x0042d0e0
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(sx16(jo::view<u16>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
            if ((jo::as_unit_type(t)->trigger_flags & 1u) == 0) continue;
            const i32 side = static_cast<i32>(jo::as_unit(u)->side);
            if (side == static_cast<i32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->side)) continue;
            jo::view<u32>(jo::as_unit(u)->state_120) = 0;
            joan::orig::as_unit(u)->set_value_135(0);                        // 0x004fe510
            joan::orig::as_unit(u)->die_7c6240(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno)));  // 0x007c6240
        }
    }

    // ------------------------------------------------------------------ 0x00455cc0
    u32 set_unit_value(const char* s, const char* k) {
        c_set_unit_value.hit();
        i32 id;  // ebp-0x8
        i32 n;   // ebp-0xc
        if (k != nullptr) {
            id = joan::orig::arg::as<i32>(joan::orig::as_list(kUnitTable)->find_name_51fdd0(joan::orig::arg::str(k)));  // 0x0051fdd0
            n = atoi_(s);
        } else {
            id = global<i32>(d::g_player_unit_recno);
            n = atoi_(s);
        }
        n = joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->limit_int(joan::orig::arg::i32(n), 1, 0x1e));  // 0x00590aa0 (clamp)
        const i32 m = unit_missing(id);
        if (m != 0) return static_cast<u32>(m);
        return joan::orig::arg::as<u32>(joan::orig::as_unit(unit_get(id))->set_level_523190(joan::orig::arg::i32(n)));  // 0x00523190
    }

    // ------------------------------------------------------------------ 0x00456b90
    u32 remove_named(const char* name) {
        c_remove_named.hit();
        for (i32 i = 1;; ++i) {
            const i32 n = joan::orig::as_dynarray(obj(d::g_unitList))->size();  // 0x00417ad0
            if (!(i < n)) return static_cast<u32>(n);
            if (joan::orig::as_units(kItems)->list.empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00425d50
            u8* const it = joan::orig::arg::as<u8*>(joan::orig::as_units(kItems)->list.ptr_at(joan::orig::arg::i32(i)));  // 0x00425d20
            if (stricmp_(jo::as_map_object(it)->name, name) != 0) continue;
            if (jo::as_map_object(it)->actor != nullptr) {
                joan::orig::as_scene(jo::as_map_object(it)->actor->scene_params0)->nop(0);  // 0x005eb590 (empty; its argument, the actor pointer on x86, is ignored)
            }
            joan::orig::as_units(kItems)->remove_49f370(joan::orig::arg::i32(i));  // 0x0049f370
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Table::argmax", 0x004554e0, &H::argmax);
B99("convert_units_in_zone", 0x004558f0, &H::convert_units_in_zone);
B99("set_unit_value", 0x00455cc0, &H::set_unit_value);
B99("remove_named", 0x00456b90, &H::remove_named);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

JOAN_COUNTER(c_mark_table, "Obj::mark_table_done");

struct H {
    // ------------------------------------------------------------------ 0x00455550
    u32 unit_call_4fe5d0() {
        const i32 id = global<i32>(d::g_player_unit_recno);
        const u32 missing = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)));  // 0x00428b70
        if (missing != 0) return missing;
        void* const unit = joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno)));  // 0x0041b3c0
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->value_149());                                         // 0x00523700
        void* const unit2 = joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno)));
        return joan::orig::arg::as<u32>(joan::orig::as_unit(unit2)->set_145(joan::orig::arg::i32(v)));  // 0x004fe5d0
    }

    // ------------------------------------------------------------------ 0x00455c00
    void mark_table_done(const char* s) {
        c_mark_table.hit();
        const i32 n = joan::crt::atoi(joan::orig::arg::str(s));  // atoi
        if (n <= static_cast<i32>(global<i16>(d::g_a59299))) {
            const u32 off = (static_cast<u32>(n) - 1u) * 0x1au;
            *reinterpret_cast<i16*>(img::addr(d::g_a592a4 + off)) = 1;
        }
        i32 all = 1;
        for (i32 i = 0; i < static_cast<i32>(global<i16>(d::g_a59299)); ++i) {
            const u32 off = static_cast<u32>(i) * 0x1au;
            if (static_cast<i32>(*reinterpret_cast<i16*>(img::addr(d::g_a592a4 + off))) != 1) {
                all = 0;
                break;
            }
        }
        if (all == 1) global<u8>(d::g_flag_a59298) = 1;
    }

    // ------------------------------------------------------------------ 0x00456a40
    void toggle_a4f3ae() {
        if (static_cast<i32>(global<i8>(d::g_a4f3ae)) != 0) {
            global<u8>(d::g_a4f3ae) = 0;
            float v[6];
            std::memcpy(v, jo::as_world(global<void*>(d::g_world))->bounds, sizeof(v));
            const i32 a3 = joan::hook::x87::ftol(static_cast<double>(v[4]));  // [ebp-0xc]
            const i32 a2 = joan::hook::x87::ftol(static_cast<double>(v[3]));  // [ebp-0x10]
            const i32 a1 = joan::hook::x87::ftol(static_cast<double>(v[1]));  // [ebp-0x18]
            const i32 a0 = joan::hook::x87::ftol(static_cast<double>(v[0]));  // [ebp-0x1c]
            joan::orig::as_mini_map(obj(d::g_mini_map))->set_reveal_rect(0xa, joan::orig::arg::u32(a0), joan::orig::arg::u32(a1), joan::orig::arg::u32(a2), joan::orig::arg::u32(a3));  // 0x007196e0
        } else {
            global<u8>(d::g_a4f3ae) = 1;
            joan::orig::as_mini_map(obj(d::g_mini_map))->clear_reveal(0xa);  // 0x007198d0
        }
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Obj::unit_call_4fe5d0", 0x00455550, &H::unit_call_4fe5d0);
B100("Obj::mark_table_done", 0x00455c00, &H::mark_table_done);
B100("Obj::toggle_a4f3ae", 0x00456a40, &H::toggle_a4f3ae);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_reset_kind, "cmd_reset_units_of_kind");
JOAN_COUNTER(c_add_level, "cmd_add_level");
JOAN_COUNTER(c_flag_by_name, "Missions::flag_by_name");

void* unit(i32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

struct H {
    // 0x004557e0
    void reset_units_of_kind(const char* s) {
        c_reset_kind.hit();
        const i32 kind = atoi_(s);
        for (i32 i = list_count(kUnits); i > 0; --i) {
            if (unit_missing(i) != 0) continue;
            void* const u = unit(i);
            if (static_cast<i32>(jo::as_unit(u)->group_id) != kind) continue;
            if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead())) != 0) continue;  // 0x0042d0e0
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));  // 0x0042d0a0
            if ((jo::as_unit_type(t)->trigger_flags & 1) == 0) continue;
            jo::as_unit(u)->state_120 = 0;
            joan::orig::as_unit(u)->set_value_135(0);                        // 0x004fe510
            joan::orig::as_unit(u)->die_7c6240(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno)));  // 0x007c6240
        }
    }

    // 0x00455f50
    void add_level(const char* s, const char* id) {
        c_add_level.hit();
        i32 uid, n;
        if (id != nullptr) {
            uid = joan::orig::arg::as<i32>(joan::orig::as_list(kUnits)->find_name_51fdd0(joan::orig::arg::str(id)));  // 0x0051fdd0
            n = atoi_(s);
        } else {
            uid = global<i32>(d::g_player_unit_recno);
            n = atoi_(s);
        }
        if (unit_missing(uid) != 0) return;
        void* const u = unit(uid);
        i32 level = jo::as_unit(u)->base_max_ep;
        if (n <= 0 || level > 999) return;
        if (n > 999) n = 999;
        level = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->limit_int(joan::orig::arg::i32(level + n), 1, 999));  // 0x00590aa0
        joan::orig::as_unit(u)->set_level_45bba0(joan::orig::arg::i32(level));                  // 0x0045bba0
        joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(jo::as_unit(u)->base_max_ep));      // 0x004fe5d0
    }

    // 0x00456ae0
    void flag_by_name(const char* name) {
        c_flag_by_name.hit();
        void* const list = obj(d::g_place_array);
        for (i32 i = 1; i <= list_count(list); ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(list)->is_missing_ca0(joan::orig::arg::i32(i))) != 0) continue;  // 0x00456ca0
            const char* const e = joan::orig::arg::as<const char*>(joan::orig::as_list(list)->get_c70(joan::orig::arg::i32(i)));  // 0x00456c70
            if (joan::crt::stricmp(joan::orig::arg::str(e + 5), joan::orig::arg::str(name)) == 0) {  // _stricmp
                jo::view<i32>(jo::as_place(joan::orig::arg::as<void*>(joan::orig::as_list(list)->get_c70(joan::orig::arg::i32(i))))->captured) = 1;
                return;
            }
        }
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("cmd_reset_units_of_kind", 0x004557e0, &H::reset_units_of_kind);
B94("cmd_add_level", 0x00455f50, &H::add_level);
B94("Missions::flag_by_name", 0x00456ae0, &H::flag_by_name);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

void* const kArrays = obj(d::g_misc);     // 0x00591070 remove, 0x005911a0 search, 0x00590660 random

using joan::orig::helpers::s16;

void* const kUnits = obj(d::g_units);   // 0x00428b70 / 0x0041b3c0 / 0x0041b3f0 / 0x0051fdd0 / 0x00417ad0

std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::int32_t clamp_(std::int32_t v, std::int32_t lo, std::int32_t hi) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kArrays)->limit_int(joan::orig::arg::i32(v), joan::orig::arg::i32(lo), joan::orig::arg::i32(hi)));  // 0x00590aa0
}

JOAN_COUNTER(c_gold, "Cheat::add_gold");
JOAN_COUNTER(c_food, "Cheat::add_food");
JOAN_COUNTER(c_wood, "Cheat::add_wood");
JOAN_COUNTER(c_stone, "Cheat::add_stone");
JOAN_COUNTER(c_iron, "Cheat::add_iron");

// Cheat helper shared shape (0x00455e30 / 0x004561d0 / 0x00456330 / 0x00456480 / 0x00456590):
// unit id from the key (0x0051fdd0) or 0xa5b9e8, amount = atoi.
std::int32_t cheat_unit(const char* key, const char* amount, std::int32_t& n) {
    std::int32_t id;
    if (key != nullptr) {
        id = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(kUnits)->find_name_51fdd0(joan::orig::arg::str(key)));  // 0x0051fdd0
        n = atoi_(amount);
    } else {
        id = global<std::int32_t>(d::g_player_unit_recno);
        n = atoi_(amount);
    }
    return id;
}

struct H {
    // ------------------------------------------------------------ 0x00455e30 (ret 8)
    void add_gold(const char* amount, const char* key) {
        c_gold.hit();
        std::int32_t n = 0;
        const std::int32_t id = cheat_unit(key, amount, n);
        if (unit_missing(id) != 0) return;
        std::uint8_t* const u = unit_get(id);
        std::int32_t cur = jo::as_unit(u)->hit_points_base;
        if (n <= 0 || cur > 0x270f) return;
        if (n > 0x270f) n = 0x270f;
        cur = clamp_(cur + n, 1, 0x270f);
        joan::orig::as_unit(u)->set_level_45b720(joan::orig::arg::i32(cur));                         // 0x0045b720
        joan::orig::as_unit(u)->set_value_135(joan::orig::arg::i32(jo::as_unit(u)->hit_points_base));  // 0x004fe510
    }

    // shared tail of 0x004561d0 / 0x00456330 / 0x00456480 / 0x00456590
    template <std::int32_t Stat, fn::Code Setter>  // Stat: index into Unit::stats
    std::uint8_t* add_res(const char* amount, const char* key, std::int32_t& n) {
        const std::int32_t id = cheat_unit(key, amount, n);
        if (unit_missing(id) != 0) return nullptr;
        std::uint8_t* const u = unit_get(id);
        std::int32_t cur = s16(jo::view<std::uint16_t>(jo::as_unit(u)->stats[Stat]));
        if (n <= 0 || cur > 0x3e7) return nullptr;
        if (n > 0x3e7) n = 0x3e7;
        cur = clamp_(cur + n, 1, 0x3e7);
        thiscall<std::uint32_t>(Setter, u, cur);
        return u;
    }
    std::int32_t gold_plus(std::uint8_t* u, std::int32_t n, img::Addr factor) {
        const std::int32_t g = jo::as_unit(u)->hit_points_base;
        const std::int32_t add =
            static_cast<std::int32_t>(static_cast<std::uint32_t>(n) * static_cast<std::uint32_t>(s16(global<std::uint16_t>(factor))));
        return clamp_(static_cast<std::int32_t>(static_cast<std::uint32_t>(g) + static_cast<std::uint32_t>(add)), 1, 0x270f);
    }
    void add_food(const char* amount, const char* key) {  // 0x004561d0
        c_food.hit();
        std::int32_t n = 0;
        std::uint8_t* const u = add_res<0, fn::Obj_set_155>(amount, key, n);  // 0x004fe6e0
        if (u == nullptr) return;
        joan::orig::as_unit(u)->set_level_45b720(joan::orig::arg::i32(gold_plus(u, n, d::g_unit_str_hp)));  // 0x0045b720
    }
    void add_wood(const char* amount, const char* key) {  // 0x00456330
        c_wood.hit();
        std::int32_t n = 0;
        std::uint8_t* const u = add_res<2, fn::Obj_set_159>(amount, key, n);  // 0x004fe740
        if (u == nullptr) return;
        (void)gold_plus(u, n, d::g_unit_def_hp);  // result unused in the original
    }
    void add_stone(const char* amount, const char* key) {  // 0x00456480
        c_stone.hit();
        std::int32_t n = 0;
        (void)add_res<1, fn::Obj_set_157>(amount, key, n);  // 0x004fe710
    }
    void add_iron(const char* amount, const char* key) {  // 0x00456590
        c_iron.hit();
        std::int32_t n = 0;
        (void)add_res<3, fn::Obj_set_15b>(amount, key, n);  // 0x004fe770
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Cheat::add_gold", 0x00455e30, &H::add_gold);
B("Cheat::add_food", 0x004561d0, &H::add_food);
B("Cheat::add_wood", 0x00456330, &H::add_wood);
B("Cheat::add_stone", 0x00456480, &H::add_stone);
B("Cheat::add_iron", 0x00456590, &H::add_iron);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

void* const kUtil = obj(d::g_misc);
void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_add_456070, "Script::add_456070");

struct H {
    // ---------------------------------------------------------------- 0x00456070
    u32 add_456070(const char* text, u32 id) {
        c_add_456070.hit();
        u32 unit;
        i32 n;
        if (id != 0) {
            unit = joan::orig::arg::as<u32>(joan::orig::as_list(kUnits)->find_name_51fdd0(joan::orig::arg::str(id)));  // 0x0051fdd0
            n = joan::crt::atoi(joan::orig::arg::str(text));         // atoi
        } else {
            unit = global<u32>(d::g_player_unit_recno);
            n = joan::crt::atoi(joan::orig::arg::str(text));
        }
        const u32 gone = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(unit)));  // 0x00428b70
        if (gone != 0) return gone;
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(unit)));  // 0x0041b3c0
        const i32 v = jo::as_unit(p)->stats[0];
        if (n <= 0 || v > 0x3e7) return joan::call::register_low_bits(p);  // eax residue (the unit); no ported caller reads it
        if (n > 0x3e7) n = 0x3e7;
        const u32 nv = joan::orig::arg::as<u32>(joan::orig::as_misc(kUtil)->limit_int(joan::orig::arg::i32(static_cast<u32>(v) + static_cast<u32>(n)), 1, 0x3e7));  // 0x00590aa0
        joan::orig::as_unit(p)->set_strength(joan::orig::arg::u32(nv));  // 0x004fe6e0
        const u32 w = jo::view<u32>(jo::as_unit(p)->hit_points_base);
        const u32 add_w = static_cast<u32>(n) * static_cast<u32>(static_cast<i32>(global<i16>(d::g_unit_str_hp)));
        const u32 nw = joan::orig::arg::as<u32>(joan::orig::as_misc(kUtil)->limit_int(joan::orig::arg::i32(w + add_w), 1, 0x270f));
        return joan::orig::arg::as<u32>(joan::orig::as_unit(p)->set_level_45b720(joan::orig::arg::i32(nw)));  // 0x0045b720
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Script::add_456070", 0x00456070, &H::add_456070);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_spawn_label, "Unit::spawn_label_456780");

void* const kRes = obj(d::g_misc);
void* const kUnits = obj(d::g_units);

struct H {
    // ------------------------------------------------------------------ 0x00456780
    u32 spawn_label(const char* key, const char* num) {
        c_spawn_label.hit();
        const u32 r0 = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x00428b70
        if (r0 != 0) return r0;
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
        const i32 id = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(key)));              // 0x004a36c0
        if (!(id > 0)) return static_cast<u32>(id);
        const i32 slot = joan::orig::arg::as<i32>(joan::orig::as_units(obj(d::g_items))->create_49ea90(joan::orig::arg::u32(id)));  // 0x0049ea90
        if (!(slot > 0)) return static_cast<u32>(slot);
        (void)joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(slot));                             // 0x00425d20
        u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u16>(id))));  // 0x00425ce0
        i32 pos = jo::as_item_def(def)->min_level;
        if (num != nullptr) {
            const i32 n = joan::crt::atoi(joan::orig::arg::str(num));  // atoi
            pos = static_cast<i32>(static_cast<u32>(n) + static_cast<u32>(jo::as_item_def(def)->min_level) - 1u);
        }
        alignas(4) u8 label[sizeof(jo::WorldItem) + 2];  // [ebp-0x7c], 0x52 bytes used (x86)
        joan::orig::as_world_item(label)->construct();  // 0x00456eb0
        joan::orig::as_item_stack(label)->init(joan::orig::arg::i16(static_cast<u16>(id)), joan::orig::arg::i16(jo::view<u16[2]>(jo::as_item_def(def)->min_level)[0]), joan::orig::arg::i16(jo::as_item_def(def)->stats[3]), 0u, 0u, 0u);  // 0x00456cd0
        const i32 clamped = joan::orig::arg::as<i32>(joan::orig::as_misc(kRes)->limit_int(joan::orig::arg::i32(pos), joan::orig::arg::i32(jo::as_item_def(def)->min_level), joan::orig::arg::i32(jo::as_item_def(def)->max_level)));  // 0x00590aa0
        joan::orig::as_inventory(jo::as_unit(unit)->modifiers)->add_495f40(joan::orig::arg::u32(static_cast<u16>(id)), joan::orig::arg::u32(clamped), joan::orig::arg::u32(jo::as_item_def(def)->stats[3]), joan::orig::arg::u32(global<u32>(d::g_player_unit_recno)), joan::orig::arg::u32(jo::view<u32>(jo::as_world_item(label)->stack.uses)), joan::orig::arg::u32(jo::view<u32>(jo::as_world_item(label)->stack.max_uses)), joan::orig::arg::u32(jo::as_world_item(label)->stack.u32_e), 0u);  // 0x00495f40
        return joan::orig::arg::as<u32>(joan::orig::as_world_item(label)->destruct());  // 0x00456ef0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Unit::spawn_label", 0x00456780, &H::spawn_label);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

struct H {
    // ------------------------------------------------------------------ 0x00456900
    u32 set_id(i32 id) {
        if (id == 0) {
            jo::self<jo::MouseCursor>(this)->cur_shape = 0;
            jo::self<jo::MouseCursor>(this)->icon_bitmap = nullptr;
            return joan::call::register_low_bits(this);  // eax residue; every caller ignores it (i_wall, i_gate, misc_004944c0, ai_behav)
        }
        const i32 cur = jo::self<jo::MouseCursor>(this)->cur_shape;
        if (cur != 0 && id == cur) return static_cast<u32>(cur);
        return joan::orig::arg::as<u32>(joan::orig::as_cursor(this)->set_596db0(joan::orig::arg::i32(id)));  // 0x00596db0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Sound::set_id", 0x00456900, &H::set_id);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

void* list_get(const void* list, std::int32_t i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    std::int32_t is_missing(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x00456ca0 / 0x004217e0
    void* get(std::int32_t i) { return list_get(this, i); }                                // 0x00456c70 / 0x00420ff0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("List456ca0::is_missing", 0x00456ca0, &H::is_missing);
B85("List456c70::get", 0x00456c70, &H::get);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Console::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_02::H::self>(this)); }  // 0x454270
auto joan::orig::Text::next_word(const char* a0, std::uint32_t a1, const char* a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_01::H::next_word>(this, a0, a1, a2)); }  // 0x4542f0
auto joan::orig::Console::parse_quoted(const char* a0, std::uint32_t a1, const char* a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_01::H::parse_quoted>(this, a0, a1, a2)); }  // 0x4543c0
auto joan::orig::Console::skip_spaces(const char* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::skip_spaces>(this, a0, a1)); }  // 0x4544c0
auto joan::orig::Unit::detach_rider() -> void { joan::bind::member<&::game_b95_01::H::detach_rider>(this); }  // 0x454b20
auto joan::orig::Console::run_cheat(const char* a0, const char* a1, const char* a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::cheats>(this, a0, a1, a2)); }  // 0x454c80
auto joan::orig::Effect::release() -> void { joan::bind::member<&::game_world4::H::effect_release>(this); }  // 0x4551f0
auto joan::orig::Units::hide_language(const char* a0) -> void { joan::bind::member<&::game_b96_01::H::hide_language>(this, a0); }  // 0x455340
auto joan::orig::Table::argmax() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_01::H::argmax>(this)); }  // 0x4554e0
auto joan::orig::Console::cheat_ep() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_02::H::unit_call_4fe5d0>(this)); }  // 0x455550
auto joan::orig::Console::cheat_hp() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_01::H::player_unit_refresh>(this)); }  // 0x4555c0
auto joan::orig::Console::cheat_invincible(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b97_01::H::units_set_1dd>(this, a0, a1); }  // 0x455630
auto joan::orig::Console::cheat_kill_unit_in_place(const char* a0) -> void { joan::bind::member<&::game_b99_01::H::convert_units_in_zone>(this, a0); }  // 0x4558f0
auto joan::orig::Console::cheat_kill_unit(const char* a0) -> void { joan::bind::member<&::game_b95_01::H::reset_unit>(this, a0); }  // 0x455a60
auto joan::orig::Console::cheat_unit_add_hp(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b92_01::H::add_gold>(this, a0, a1); }  // 0x455e30
auto joan::orig::Console::cheat_unit_add_ep(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b94_01::H::add_level>(this, a0, a1); }  // 0x455f50
auto joan::orig::Console::cheat_unit_add_str(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b92_01::H::add_food>(this, a0, a1); }  // 0x4561d0
auto joan::orig::Console::cheat_unit_add_def(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b92_01::H::add_wood>(this, a0, a1); }  // 0x456330
auto joan::orig::Console::cheat_unit_add_dex(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b92_01::H::add_stone>(this, a0, a1); }  // 0x456480
auto joan::orig::Console::cheat_unit_add_lea(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b92_01::H::add_iron>(this, a0, a1); }  // 0x456590
auto joan::orig::Unit::spawn_label(const char* a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_03::H::spawn_label>(this, a0, a1)); }  // 0x456780
auto joan::orig::Console::cheat_add_command_point() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_01::H::copy_a3cc64>(this)); }  // 0x456960
auto joan::orig::Console::cheat_enable_fog_of_war(const char* a0) -> void { joan::bind::member<&::game_b96_01::H::set_view_rect>(this, a0); }  // 0x456990
auto joan::orig::Console::cheat_toggle_fog_of_war() -> void { joan::bind::member<&::game_b100_02::H::toggle_a4f3ae>(this); }  // 0x456a40
auto joan::orig::Console::cheat_capture_place(const char* a0) -> void { joan::bind::member<&::game_b94_01::H::flag_by_name>(this, a0); }  // 0x456ae0
auto joan::orig::Console::cheat_del_item(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_01::H::remove_named>(this, a0)); }  // 0x456b90
auto joan::orig::List::get_c70(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::get>(this, a0)); }  // 0x456c70
auto joan::orig::List::is_missing_ca0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_06::H::is_missing>(this, a0)); }  // 0x456ca0
