// game/unknown_unit_xp.cpp: functions whose original source file is unknown (formerly game/_unplaced/unit_xp.cpp;
// re/attribution.csv: between ww/unit.cpp and ww/unitres.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

}  // namespace

// ---------------------------------------- from modules/game_b88_03.cpp (game/b88_03)
namespace {
namespace game_b88_03 {

using joan::hook::x87::Fpu;

using joan::img::str;

using joan::hook::x87::ftol;

void* const kUnitList = obj(d::g_units);
void* const kUnitTypes = obj(d::g_types);

std::uint8_t* unit_type(std::int16_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kUnitTypes)->get(joan::orig::arg::i16(id)));  // 0x0042d0a0 (reads the word)
}
std::int32_t unit_dead(void* u) {
    return static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(u)->dead()));  // 0x0042d0e0
}

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "layout");

JOAN_COUNTER(c_add_xp, "Unit::add_experience");

struct H {
    // ------------------------------------------------------------------------ 0x005229a0
    void add_experience(std::int32_t xp, std::int8_t share, std::int8_t flag) {
        c_add_xp.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 2) return;  // 0x004cd510
        if (sx16(jo::self<jo::UnitType>(unit_type(jo::self<jo::Unit>(this)->type_id))->unit_class) == 8) return;
        if (share != 0) {
            const joan::hook::x87::Fpu fpu{true};
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->leads()) != 0) {  // 0x0044a660 leads
                const std::int32_t part = joan::hook::x87::ftol(
                    fpu.mul(static_cast<double>(xp), static_cast<double>(global<float>(d::g_a3ccbc))));
                for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->count); ++i) {
                    if (joan::orig::as_dynarray(kUnitList)->empty_at(joan::orig::arg::i32(sx16(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i]))) != 0)  // 0x00428b70
                        continue;
                    void* const m = joan::orig::as_dynarray(kUnitList)->ptr_at(joan::orig::arg::i32(sx16(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i])));  // 0x0041b3c0
                    if (unit_dead(m) != 0) continue;
                    const std::int32_t count = static_cast<std::int32_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->count);
                    const std::int32_t v =
                        joan::hook::x87::ftol(fpu.div(static_cast<double>(part), static_cast<double>(count)));
                    joan::orig::as_unit(m)->add_experience(joan::orig::arg::i32(v), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(flag)));  // 0x005229a0
                }
            } else {
                void* const leader = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->leader());  // 0x004216d0
                if (leader != nullptr && unit_dead(leader) == 0) {
                    const std::int32_t v = joan::hook::x87::ftol(
                        fpu.mul(static_cast<double>(xp), static_cast<double>(global<float>(d::g_a3ccec))));
                    joan::orig::as_unit(leader)->add_experience(joan::orig::arg::i32(v), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(flag)));  // 0x005229a0
                }
            }
        }
        jo::self<jo::Unit>(this)->experience = static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::Unit>(this)->experience) +
                                                          static_cast<std::uint32_t>(xp));
        if (sx16(jo::self<jo::Unit>(this)->id) == global<std::int32_t>(d::g_player_unit_recno)) {
            joan::orig::as_name_list(obj(d::g_player_hud))->add_6fe520(joan::orig::arg::i32(xp));  // 0x006fe520
            if (global<std::int32_t>(d::g_control_mode) != 4) {
                global<std::int32_t>(d::g_command_points) = static_cast<std::int32_t>(
                    static_cast<std::uint32_t>(global<std::int32_t>(d::g_command_points)) + static_cast<std::uint32_t>(xp));
                if (!(global<std::int32_t>(d::g_command_points) < global<std::int32_t>(d::g_unit_max_com)))
                    global<std::int32_t>(d::g_command_points) = global<std::int32_t>(d::g_unit_max_com);
            }
        }
        joan::orig::as_unit(this)->level_up(static_cast<char>(joan::orig::arg::i32(flag)));  // 0x00522c40
        if (sx16(jo::self<jo::Unit>(this)->id) == global<std::int32_t>(d::g_player_unit_recno)) {
            Vec3 pos;
            std::memcpy(&pos, jo::self<jo::Unit>(this)->position_, sizeof pos);
            joan::orig::as_sound_list(obj(d::g_sfx_player))->play_effect(str(d::s_EXPUP), pos);  // 0x00494810 "EXPUP"
        }
    }
};

#define B88(name, addr, fn) JOAN_REPLACE("game/b88_03", name, addr, fn)
B88("Unit::add_experience", 0x005229a0, &H::add_experience);
#undef B88

}  // namespace game_b88_03
}  // namespace

// ---------------------------------------- from modules/game_b89_03.cpp (game/b89_03)
namespace {
namespace game_b89_03 {

using joan::call::cdecl_call;

constexpr img::Addr kGameUnits(d::g_units);

const char* rdata(img::Addr a) { return &img::global<const char>(a); }

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12);

constexpr img::Addr kKinds(d::g_types);
constexpr img::Addr kFiles(d::g_pemi_res);
std::int32_t player_id() { return global<std::int32_t>(d::g_player_unit_recno); }
std::int32_t now() { return global<std::int32_t>(d::g_now); }

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* kind_get(std::int32_t k) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kKinds))->get(joan::orig::arg::i16(k))); }  // 0x0042d0a0

JOAN_COUNTER(c_level_up, "Unit::level_up");

struct H {
    // 0x00522c40
    std::int32_t level_up(char quiet) {
        c_level_up.hit();
        void* const table = img::obj(d::g_level_res);
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(this)->dead())) != 0) return 0;  // 0x0042d0e0
        std::uint8_t* row = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_level_res(table)->find(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->level)));  // 0x004a4e60
        std::uint8_t* const next = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_level_res(table)->find(joan::orig::arg::i32(jo::self<jo::Unit>(this)->level + 1)));
        if (row == nullptr) return 0;
        if (next == nullptr) return 0;
        if (jo::self<jo::Unit>(this)->experience < jo::self<jo::LevelRec>(row)->experience) return 0;
        while (jo::self<jo::Unit>(this)->experience >= jo::self<jo::LevelRec>(row)->experience) {
            joan::orig::as_unit(this)->apply_kind_links(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->level));  // 0x00523240
            jo::self<jo::Unit>(this)->level = static_cast<std::int16_t>(jo::self<jo::Unit>(this)->level + 1);
            if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id) == player_id()) {
                joan::orig::as_hud(img::obj(d::g_player_hud))->start_level_banner();  // 0x00703680
            }
            jo::self<jo::Unit>(this)->levelup_flag = 1;
            jo::self<jo::Unit>(this)->levelup_time = now();
            jo::self<jo::Unit>(this)->attr_points = static_cast<std::int16_t>(jo::self<jo::Unit>(this)->attr_points + static_cast<std::int16_t>(jo::self<jo::LevelRec>(row)->gain_12c));
            jo::self<jo::Unit>(this)->stat_points = static_cast<std::int16_t>(jo::self<jo::Unit>(this)->stat_points + static_cast<std::int16_t>(jo::self<jo::LevelRec>(row)->stat_points));
            row = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_level_res(table)->find(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->level)));
            if (row == nullptr) break;
        }
        jo::self<jo::Unit>(this)->upg_timer = now();
        if (unit_missing(player_id()) == 0 && static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id) != player_id()) {
            std::uint8_t* const p = unit_get(player_id());
            if (jo::self<jo::Unit>(this)->kills != 0) {
                void* const side = joan::orig::arg::as<void*>(joan::orig::as_unit(p)->nation());  // 0x00509400 (a Nation*)
                if (side == joan::orig::arg::as<void*>(joan::orig::as_unit(this)->nation()) && jo::self<jo::Unit>(this)->rank == 4) {
                    std::uint8_t* const k = kind_get(jo::self<jo::Unit>(this)->type_id);
                    char text[0x110];
                    const char* fmt;
                    if (static_cast<std::int32_t>(jo::self<jo::UnitType>(k)->gender) == 0x46) {
                        fmt = joan::orig::arg::as<const char*>(joan::orig::as_unit(img::obj(d::g_strings))->charmove3(static_cast<const char*>(jo::self<jo::Unit>(this)->name_), joan::orig::arg::u32(// 0x004de5c0
                                                    static_cast<std::int32_t>(jo::self<jo::Unit>(this)->level))));
                    } else {
                        fmt = joan::orig::arg::as<const char*>(joan::orig::as_unit(img::obj(d::g_strings))->charmove2(static_cast<const char*>(jo::self<jo::Unit>(this)->name_), joan::orig::arg::u32(// 0x004de580
                                                    static_cast<std::int32_t>(jo::self<jo::Unit>(this)->level))));
                    }
                    joan::orig::libc::sprintf(joan::orig::arg::str(static_cast<char*>(text)), joan::orig::arg::str(fmt));  // sprintf(text, fmt)
                    joan::orig::as_message_list(img::obj(d::g_names))->post(static_cast<char*>(text), nullptr, 0u);  // 0x004ad710
                }
            }
        }
        joan::orig::as_unit(this)->set_value_135(joan::orig::arg::i32(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hit_points_max())));  // 0x004fe510(0x00523320())
        jo::self<jo::Unit>(this)->hp_shown = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hit_points_max());    // 0x00523320
        joan::orig::as_unit(this)->set_145(joan::orig::arg::i32(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->value_149())));  // 0x004fe5d0(0x00523700())
        if (jo::self<jo::Unit>(this)->actor != nullptr && static_cast<std::int8_t>(quiet) == 0) {
            const std::int32_t handle = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_list(img::obj(kFiles))->find_4c2060(joan::orig::arg::str(rdata(d::s_flash_txt)), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060 "flash.txt"
            if (handle != 0) {
                std::uint8_t* const entry = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_name_list(img::obj(kFiles))->entry(handle));  // 0x0041b440

                void* const effects = img::obj(d::g_particle_effect_array);
                const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_model_list(effects)->add(// 0x0041baf0
                                                               jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene, jo::self<jo::PemiScript>(entry)->effect, img::obj(d::g_pos), img::obj(d::g_pos), static_cast<std::int8_t>(joan::orig::arg::i32(1)), 0, 0, 0x1388));
                void* const actor = jo::self<jo::Unit>(this)->actor;
                void* const effect = joan::orig::as_dynarray(effects)->ptr_at(joan::orig::arg::i32(id));  // 0x0041bd60
                joan::orig::as_effect(effect)->set_owner(joan::orig::arg::mptr(actor), joan::orig::arg::mptr(0));                         // 0x0041b0d0
            }
        }
        if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id) == player_id()) {
            V3 pos;
            std::memcpy(&pos, jo::self<jo::Unit>(this)->position_, 12);
            joan::orig::as_sound_list(img::obj(d::g_sfx_player))->play_effect(rdata(d::s_LEVELUP), pos);  // 0x00494810 "LEVELUP"
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->leads()) != 0 && jo::self<jo::Unit>(this)->group != nullptr) {  // 0x0044a660
            for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->count); ++i) {
                if (unit_missing(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i]) != 0) continue;
                std::uint8_t* const u = unit_get(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i]);
                joan::orig::as_unit(u)->set_level_523190(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->level));  // 0x00523190
            }
        }
        return 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_03", name, addr, fn)
M("Unit::level_up", 0x00522c40, &H::level_up);
#undef M

}  // namespace game_b89_03
}  // namespace

// ---------------------------------------- from modules/game_b87_03.cpp (game/b87_03)
namespace {
namespace game_b87_03 {

JOAN_COUNTER(c_set_level, "Unit::set_level");

struct H {
    // ------------------------------------------------------------------------ 0x00523190
    std::int32_t set_level(std::int32_t level) {
        c_set_level.hit();
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(this)->dead())) != 0) return 0;  // 0x0042d0e0
        if (!(level > sx16(jo::self<jo::Unit>(this)->level))) return 0;
        std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_level_res(obj(d::g_level_res))->find(joan::orig::arg::i32(level - 1)));  // 0x004a4e60
        if (rec == nullptr) return 0;
        joan::orig::as_unit(this)->add_experience(static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::LevelRec>(rec)->experience) -
                                           static_cast<std::uint32_t>(jo::self<jo::Unit>(this)->experience)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), static_cast<std::int8_t>(joan::orig::arg::i32(1)));              // 0x005229a0
        joan::orig::as_unit(this)->distribute_levels();  // 0x004a95a0
        return 1;
    }
};

#define B87(name, addr, fn) JOAN_REPLACE("game/b87_03", name, addr, fn)
B87("Unit::set_level", 0x00523190, &H::set_level);
#undef B87

}  // namespace game_b87_03
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using joan::hook::global;

using joan::hook::global;
using u32 = std::uint32_t;

struct H {
    // 0x00522c10 (this unused)
    u32 add_global(u32 v) {
        global<u32>(d::g_get_a596c4) += v;
        return global<u32>(d::g_get_a596c4);
    }
};

JOAN_REPLACE(M, "add_global_a596c4", 0x00522c10, &H::add_global);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::add_experience(std::int32_t a0, std::int8_t a1, std::int8_t a2) -> void { joan::bind::member<&::game_b88_03::H::add_experience>(this, a0, a1, a2); }  // 0x5229a0
auto joan::orig::Unit::level_up(char a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_03::H::level_up>(this, a0)); }  // 0x522c40
