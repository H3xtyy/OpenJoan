// game/unknown_unit_effects.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_004944c0.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using joan::orig::helpers::at;
std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

struct V3 {
    float x, y, z;  // floats: passed by value to aim_velocity (System V passes float members in XMM registers)
};
static_assert(sizeof(V3) == 12);

JOAN_COUNTER(c_spawn, "spawn_effect_kind");

float gi(img::Addr g) { return static_cast<float>(global<i32>(g)); }  // fild dword / fstp dword
u32 gd(img::Addr g) { return global<u32>(g); }

// 0x004945c0
std::uintptr_t JOAN_CDECL spawn_effect_kind(void* p, V3 a, V3 b, u32 kind) {
    c_spawn.hit();
    switch (kind) {
        case 0:
            return joan::orig::arg::as<u32>(joan::orig::aim_velocity(joan::orig::arg::mptr(p), a, b, joan::orig::arg::f32(gd(d::g_throw_arw_m)), joan::orig::arg::f32(gi(d::g_a39d74)), joan::orig::arg::bits(gi(d::g_a39d84)), joan::orig::arg::f32(gd(d::g_a39d5c)), joan::orig::arg::f32(gd(d::g_throw_arw_gl)), joan::orig::arg::f32(0u)));  // 0x00496d40
        case 1:
        case 2:
            return joan::orig::arg::as<u32>(joan::orig::aim_velocity(joan::orig::arg::mptr(p), a, b, joan::orig::arg::f32(gd(d::g_throw_proj_m)), joan::orig::arg::f32(gi(d::g_a39dc0)), joan::orig::arg::bits(gi(d::g_a39da4)), joan::orig::arg::f32(gd(d::g_a39cd8)), joan::orig::arg::f32(gd(d::g_throw_proj_gl)), joan::orig::arg::f32(0x3e800000u)));
        case 3:
            return joan::orig::arg::as<u32>(joan::orig::aim_velocity(joan::orig::arg::mptr(p), a, b, joan::orig::arg::f32(gd(d::g_throw_stm_m)), joan::orig::arg::f32(gi(d::g_a39d94)), joan::orig::arg::bits(gi(d::g_a39d10)), joan::orig::arg::f32(gd(d::g_a39d20)), joan::orig::arg::f32(gd(d::g_a39ce0)), joan::orig::arg::f32(0u)));
        case 4:
            return joan::orig::arg::as<u32>(joan::orig::aim_velocity(joan::orig::arg::mptr(p), a, b, joan::orig::arg::f32(gd(d::g_throw_stc_m)), joan::orig::arg::f32(gi(d::g_a39d58)), joan::orig::arg::bits(gi(d::g_a39ce4)), joan::orig::arg::f32(gd(d::g_a39cec)), joan::orig::arg::f32(gd(d::g_throw_stc_gl)), joan::orig::arg::f32(0x3e800000u)));
        case 5:
            return joan::orig::arg::as<u32>(joan::orig::aim_velocity(joan::orig::arg::mptr(p), a, b, joan::orig::arg::f32(gd(d::g_throw_can_m)), joan::orig::arg::f32(gi(d::g_a39d00)), joan::orig::arg::bits(gi(d::g_a39d64)), joan::orig::arg::f32(gd(d::g_a39cc8)), joan::orig::arg::f32(gd(d::g_throw_can_gl)), joan::orig::arg::f32(0x3dcccccdu)));
        default:
            return kind;
    }
}

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("spawn_effect_kind", 0x004945c0, &spawn_effect_kind);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
namespace {
namespace game_b89_04 {

using joan::hook::x87::Fpu;

constexpr Fpu kFpu{true};

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "layout");

void* const kEmitters = obj(d::g_sound_def_table);

JOAN_COUNTER(c_emit, "Unit::emit_particles");

struct H {
    // ------------------------------------------------------------------------ 0x00494810
    std::int32_t emit_particles(const char* kind, Vec3 pos) {
        c_emit.hit();
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_game_mode)) == 0xc) return 0;
        std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(kEmitters)->find_select(kind));  // 0x004d0d80, ebp-0x8
        if (global<void*>(d::g_sound3d_listener) == nullptr) return 0;
        if (rec == nullptr) return 0;
        jo::Sound3DBuffer* const tex = joan::orig::as_wave_res(rec)->buffer();  // 0x004d09c0, ebp-0xc
        if (tex == nullptr) return 0;
        const std::uint32_t slot_eax = joan::orig::arg::as<std::uint32_t>(joan::orig::as_party(this)->first_free());  // 0x0045f450 (only ax is used)
        const std::int16_t slot = static_cast<std::int16_t>(slot_eax);           // ebp-0x10
        if (!(sx16(slot) > 0)) return 0;
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(slot_eax)));  // 0x004cf690 (ax pushed as eax)
        joan::orig::as_sound_slot_entry(e)->set_key(joan::orig::arg::u32(std::int32_t{0}));                                     // 0x004d06b0
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->queue(tex);                                   // 0x0059f460
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_position(static_cast<void*>(&pos));  // 0x0059fb40
        // set_looping reads the low byte only (the old eax's upper bits were dropped)
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_looping(static_cast<std::uint8_t>(jo::as_wave_res(rec)->flags & 3u));  // 0x0059fa80
        const double scaled = kFpu.mul(kFpu.mul(static_cast<double>(jo::self<jo::Panels>(this)->volume), jo::as_wave_res(rec)->volume),
                                       global<float>(d::flt_0_01));  // fild * float * 0.01f
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_gain(joan::orig::arg::bits(static_cast<float>(scaled)));  // 0x0059f700
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_max_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(rec)->max_dist)));  // 0x0059f920
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_reference_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(rec)->min_dist)));  // 0x0059f970
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(std::int32_t{0})));               // 0x0059fae0
        joan::orig::as_sound3d(jo::as_sound_slot_entry(e)->sound)->set_rolloff(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(rec)->rolloff)));  // 0x0059f9c0
        joan::orig::as_sound_slot_entry(e)->set_id(joan::orig::arg::u32(sx16(jo::view<std::int16_t>(jo::as_wave_res(rec)->index))));            // 0x004d0550
        joan::orig::as_sound(jo::as_sound_slot_entry(e)->sound)->play();                                // 0x0059f620
        return sx16(slot);
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("Unit::emit_particles", 0x00494810, &H::emit_particles);
#undef B89

}  // namespace game_b89_04
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

// ---------------------------------------------------------------------- static initialisers
template <fn::Code Worker>
void JOAN_CDECL static_init() { cdecl_call<void>(Worker); }

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("init_4949a0", 0x004949a0, &static_init<fn::init_a39c8c>);
B("init_494da0", 0x00494da0, &static_init<fn::init_a39c90>);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

void JOAN_CDECL init_a39c8c() { global<float>(d::g_a39c8c) = global<float>(d::g_a39c84) / global<float>(d::flt_3); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a39c8c", 0x004949c0, &init_a39c8c);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b88_03.cpp (game/b88_03)
namespace {
namespace game_b88_03 {

std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

void* const kItems = obj(d::g_upgrade_res);

std::uint8_t* item_def(std::int32_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_table(kItems)->entry(joan::orig::arg::u32(id)));  // 0x0050d240
}

// atoi(item def + 0x46): the bonus value of an item.
std::int32_t bonus(const std::int16_t* item) {
    return atoi_(jo::as_upgrade_item(item_def(sx16(item[0])))->bonus_text);
}
using joan::orig::helpers::add32;
std::int32_t mul32(std::int32_t a, std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
}

JOAN_COUNTER(c_use_item, "Unit::use_item");

struct H {
    // ------------------------------------------------------------------------ 0x004949f0
    void use_item(std::int16_t* item) {
        c_use_item.hit();
        const std::int8_t kind =
            static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_upgrade_res(kItems)->is_special(joan::orig::arg::i32(sx16(item[0])))));  // 0x00531ed0
        if (kind == 0) {
            const std::int32_t have = sx16(jo::self<jo::Unit>(this)->attr_points);
            if (have < sx16(jo::as_upgrade_item(item_def(sx16(item[0])))->cost)) return;
        } else {
            if (kind != 1) return;
            const std::int32_t have = sx16(jo::self<jo::Unit>(this)->stat_points);
            if (have < sx16(jo::as_upgrade_item(item_def(sx16(item[0])))->cost)) return;
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_counter(item)->step()) == 0) return;  // 0x005315a0
        if (kind == 1) {
            const std::uint8_t* d = item_def(sx16(item[0]));
            jo::self<jo::Unit>(this)->stat_points = static_cast<std::int16_t>(static_cast<std::uint16_t>(jo::self<jo::Unit>(this)->stat_points) -
                                                               static_cast<std::uint16_t>(jo::as_upgrade_item(d)->cost));
        } else {
            const std::uint8_t* d = item_def(sx16(item[0]));
            jo::self<jo::Unit>(this)->attr_points = static_cast<std::int16_t>(static_cast<std::uint16_t>(jo::self<jo::Unit>(this)->attr_points) -
                                                               static_cast<std::uint16_t>(jo::as_upgrade_item(d)->cost));
        }
        const std::uint32_t type = static_cast<std::uint32_t>(sx16(jo::as_upgrade_item(item_def(sx16(item[0])))->type)) - 1u;
        if (type > 5u) return;
        switch (type) {
            case 0: {
                const std::int32_t n = bonus(item);
                joan::orig::as_unit(this)->set_strength(joan::orig::arg::u32(add32(sx16(jo::self<jo::Unit>(this)->stats[0]), n)));  // 0x004fe6e0
                joan::orig::as_unit(this)->set_level_45b720(joan::orig::arg::i32(add32(jo::self<jo::Unit>(this)->hit_points_base, mul32(n, sx16(global<std::int16_t>(d::g_unit_str_hp))))));  // 0x0045b720
                break;
            }
            case 1: {
                const std::int32_t n = bonus(item);
                joan::orig::as_unit(this)->set_dexterity(joan::orig::arg::u32(add32(sx16(jo::self<jo::Unit>(this)->stats[1]), n)));  // 0x004fe710
                break;
            }
            case 2: {
                const std::int32_t n = bonus(item);
                joan::orig::as_unit(this)->set_defense(joan::orig::arg::u32(add32(sx16(jo::self<jo::Unit>(this)->stats[2]), n)));  // 0x004fe740
                joan::orig::as_unit(this)->set_level_45b720(joan::orig::arg::i32(add32(jo::self<jo::Unit>(this)->hit_points_base, mul32(n, sx16(global<std::int16_t>(d::g_unit_def_hp))))));  // 0x0045b720
                break;
            }
            case 3:
                break;  // type 4: nothing
            case 4: {
                const std::int32_t n = bonus(item);
                joan::orig::as_unit(this)->set_leadership(joan::orig::arg::u32(add32(sx16(jo::self<jo::Unit>(this)->stats[3]), n)));  // 0x004fe770
                break;
            }
            case 5: {
                const std::int32_t n = bonus(item);
                joan::orig::as_unit(this)->set_level_45b720(joan::orig::arg::i32(add32(jo::self<jo::Unit>(this)->hit_points_base, n)));       // 0x0045b720
                const std::int32_t hp = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hit_points_max());  // 0x00523320
                joan::orig::as_unit(this)->set_value_135(joan::orig::arg::i32(hp));                      // 0x004fe510
                jo::self<jo::Unit>(this)->hp_shown = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hit_points_max());  // 0x00523320
                break;
            }
            default:
                break;
        }
    }
};

#define B88(name, addr, fn) JOAN_REPLACE("game/b88_03", name, addr, fn)
B88("Unit::use_item", 0x004949f0, &H::use_item);
#undef B88

}  // namespace game_b88_03
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;

JOAN_COUNTER(c_morale_494cc0, "Army::set_morale_494cc0");

struct H {
    // -------------------------------------------------------------- 0x00494cc0
    u32 set_morale(u32 id, u32 level) {
        c_morale_494cc0.hit();
        void* const o = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(id));  // 0x0047b600
        if (jo::as_nation(o)->mood > global<float>(d::k_9cf5c4)) {
            const i8 k = static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_table(this)->byte_58(joan::orig::arg::i16(static_cast<u16>(id)))) & 0xffu);  // 0x004b4360
            if (k == 5) {
                const float v = -jo::as_nation(o)->mood * global<float>(d::flt_10) / global<float>(d::flt_100);
                joan::orig::as_player(this)->add_mood(joan::orig::arg::f32(v));  // 0x004b5420
            } else {
                const float v = -jo::as_nation(o)->mood * global<float>(d::flt_20) / global<float>(d::flt_100);
                joan::orig::as_player(this)->add_mood(joan::orig::arg::f32(v));
            }
        }
        if (static_cast<i32>(level) <= 2) {
            jo::view<u32>(jo::as_relation(joan::orig::arg::as<void*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(id))))->f32_c) = 0;                                    // 0x004b42e0
            jo::view<u32>(jo::as_relation(joan::orig::arg::as<void*>(joan::orig::as_player(o)->relation(static_cast<i32>(jo::self<jo::Nation>(this)->id))))->f32_c) = 0;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_nation(this)->set_relation_state(joan::orig::arg::i32(static_cast<u16>(id)), joan::orig::arg::i32(static_cast<u8>(level)), i32{0}));  // 0x00499770
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Army::set_morale_494cc0", 0x00494cc0, &H::set_morale);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

using joan::img::str;

JOAN_COUNTER(c_trigname, "trigger_type_from_name");

using joan::orig::helpers::stricmp_;

// cdecl 0x00494dc0
std::int32_t JOAN_CDECL trigger_type_from_name(const char* name) {
    c_trigname.hit();
    if (stricmp_(name, str(d::s_TRIGGER_ATTACK)) == 0) return 1;       // TRIGGER_ATTACK
    if (stricmp_(name, str(d::s_TRIGGER_ARROW_ATTACK)) == 0) return 2;       // TRIGGER_ARROW_ATTACK
    if (stricmp_(name, str(d::s_TRIGGER_FADEOUT)) == 0) return 4;       // TRIGGER_FADEOUT
    if (stricmp_(name, str(d::s_TRIGGER_FADEIN)) == 0) return 8;       // TRIGGER_FADEIN
    if (stricmp_(name, str(d::s_TRIGGER_THROW)) == 0) return 0x10;    // TRIGGER_THROW
    if (stricmp_(name, str(d::s_TRIGGER_GATE)) == 0) return 0x20;    // TRIGGER_GATE
    if (stricmp_(name, str(d::s_TRIGGER_STONE_ATTACK)) == 0) return 0x40;    // TRIGGER_STONE_ATTACK
    if (stricmp_(name, str(d::s_TRIGGER_EXPLOSIVE_ARROW_ATTA)) == 0) return 0x2000;  // TRIGGER_EXPLOSIVE_ARROW_ATTACK
    if (stricmp_(name, str(d::s_TRIGGER_EXPLOSIVE_KEG_ATTACK)) == 0) return 0x4000;  // TRIGGER_EXPLOSIVE_KEG_ATTACK
    if (stricmp_(name, str(d::s_TRIGGER_FOG)) == 0) return 0x80;    // TRIGGER_FOG
    if (stricmp_(name, str(d::s_TRIGGER_PLAY_ANIMATION)) == 0) return 0x100;   // TRIGGER_PLAY_ANIMATION
    if (stricmp_(name, str(d::s_TRIGGER_INTERACT)) == 0) return 0x200;   // TRIGGER_INTERACT
    if (stricmp_(name, str(d::s_TRIGGER_DOWN_ATTACK)) == 0) return 0x400;   // TRIGGER_DOWN_ATTACK
    if (stricmp_(name, str(d::s_TRIGGER_PLAY_SOUND)) == 0) return 0x800;   // TRIGGER_PLAY_SOUND
    if (stricmp_(name, str(d::s_TRIGGER_PLAY_AMB_SOUND)) == 0) return 0x1000;  // TRIGGER_PLAY_AMB_SOUND
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("trigger_type_from_name", 0x00494dc0, &trigger_type_from_name);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

void JOAN_CDECL init_a39c90() { global<float>(d::g_a39c90) = global<float>(d::g_a39c84) / global<float>(d::flt_4); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_a39c90", 0x00495070, &init_a39c90);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

void* const kUnits = obj(d::g_nations);

JOAN_COUNTER(c_time_score, "Obj::time_score");

struct H {
    // ------------------------------------------------------------------ 0x004944c0
    float time_score(u32 id, float t) {
        c_time_score.hit();
        const float twice = t + t;
        if (twice >= jo::self<jo::Rules>(this)->f32_1371) return global<float>(d::k_9cf5c4);
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0047b600
        float a = jo::as_nation(unit)->strength - jo::self<jo::Rules>(this)->f32_14dd;
        a = a * global<float>(d::g_a01778);
        a = a * global<float>(d::flt_0_005);
        float b = global<float>(d::g_a0177c) * jo::self<jo::Rules>(this)->f32_14e1;
        b = b * global<float>(d::flt_0_002);
        float c = t / jo::self<jo::Rules>(this)->f32_1371;
        c = c * global<float>(d::flt_0_5);
        float d = 0.0f;
        const float r = joan::orig::arg::as<float>(joan::orig::as_economy(this)->projected());  // 0x004b4e30
        if (!(r >= t)) {
            d = 0.5f;
        } else {
            const float r2 = joan::orig::arg::as<float>(joan::orig::as_economy(this)->projected());
            d = t / r2;
            d = d * global<float>(d::flt_0_5);
        }
        float s = a + b;
        s = s - c;
        s = s - d;
        return s;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::time_score", 0x004944c0, &H::time_score);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Nation::time_score(std::uint32_t a0, float a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_04::H::time_score>(this, a0, a1)); }  // 0x4944c0
auto joan::orig::Unit::use_item(void* a0) -> void { joan::bind::member<&::game_b88_03::H::use_item>(this, a0); }  // 0x4949f0
auto joan::orig::Army::set_morale_494cc0(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::set_morale>(this, a0, a1)); }  // 0x494cc0
auto joan::orig::trigger_type_from_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b84_06::trigger_type_from_name>(a0)); }  // 0x494dc0
