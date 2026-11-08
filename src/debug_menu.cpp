// In-game developer menu, see debug_menu.hpp.
//
// The menu lives in the game's per-frame input handler Game::process_input (0x0052f930,
// called once per frame by the main loop 0x00499960 right after Game::draw). That is
// where the game's own hidden developer keys run (super_cheat_mode.sys: 't' = next
// mission), so mission jumps and console commands issued from here run in the same
// context as the original developer code.
//
// Game data used (all from the original code):
//   game object 0x00a50088: +0x3a state (9 = in a mission, World::draw)
//   0x00a58fa7 current scenario number (1..8), 0x00a58f95 "jumped" flag set by the dev key
//   0x0041cc80 (thunk 0x00406b13) Scenarios::start(scenario, step, 0) on 0x00a51294,
//   0x004d7e70 (thunk 0x004027fc) Game::set_state(12 = loading) on 0x00a50088
//   hero unit id 0x00a5b9e8, unit list 0x00a47c08 (0x00428b70 deleted? / 0x0041b3c0 get)
//   unit +0x155 strength, +0x157 dexterity, +0x159 defense, +0x15b leadership (words),
//   +0x13d max HP, +0x149 EP; setters 0x004fe6e0 / 0x004fe710 / 0x004fe740 / 0x004fe770
//   hidden console 0x00454560 (thunk 0x0040a498) on 0x00a485a2: "unit_add_str 10", ...
//   "unit_level_up N" SETS the level to N (0x00455cc0 -> Unit::set_level 0x00523190, only
//   upwards, clamped 1..30); unit level = word +0x124
//   Scenario 6 continues scenario 5: Scenarios::start keeps the world when scenario 5 ends with
//   keep = 1 (0x0041c780) and the mission start 0x0041c810 skips the army setup for 6. Starting
//   6 from anywhere else crashes in its script (place lookup 0x00654280 returns null), so the
//   menu goes through 5 first and then advances like the game's victory path 0x00755d80
//   (Scenarios::start(cur + 1, 1, 1)).
//   Spawning: the script command add_unit (0x00655150): unit types 0x00a48d88 (+0 records of
//   0x203 bytes, name at +4, word +4 count; type = index + 1), Units::create 0x0051dfc0
//   (thunk 0x00405056), Unit::set_side 0x00509250 (thunk 0x004097be), +0x102 = 1; placed like
//   set_unit_pos (0x006554f0): 0x004fed00 sector (+0x335), 0x004fedf0 position, heading like
//   set_unit_dir (0x00655590), activate_unit 0x00442820 (thunk 0x0040786a), +0x3c0 can-ride.
#include "callconv.hpp"
#include "original/direct.hpp"
#include "data_names.hpp"
#include "debug_menu.hpp"
#include "freecam.hpp"
#include "fn_names.hpp"

#include "platform/win32_types.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#ifndef _WIN32
#include <filesystem>

#include <SDL3/SDL.h>
#endif

#include "registry.hpp"
#include "render/ambient_occlusion.hpp"
#include "render/shadow_map.hpp"
#include "render/pixel_lighting.hpp"
#include "platform/files.hpp"
#include "platform/window.hpp"
#include "platform/ini.hpp"

namespace joan::game_b117_04 {
void process_input(void* self);  // Game::process_input 0x0052f930 in C++ (game/_unplaced/after_unitres.cpp)
}

namespace joan::debugmenu {
namespace {
using joan::hook::global;

struct Obj {};
using joan::call::thiscall;

constexpr img::Addr kGame = d::g_game_a50088;
constexpr img::Addr kUnits = d::g_units;
constexpr img::Addr kConsole = d::g_console;
constexpr img::Addr kScenarios = d::g_scenarios;
constexpr img::Addr kInput = d::g_mouse;
constexpr int kMissions = 8;

JOAN_COUNTER(c_frames, "DebugMenu::process_input");
JOAN_COUNTER(c_actions, "DebugMenu::actions");

enum class Page { Root, Missions, Hero, Spawn, Cheats, Occlusion, Shadows };

struct Vec3 {
    float x, y, z;
};
struct Mat44 {
    float m[16];
};

struct State {
    bool open = false;
    Page page = Page::Root;
    int sel = 0;
    int menu_key = 0xC0;
    bool loaded = false;
    std::string status;
    std::string mission_names[kMissions];
    bool prev[256] = {};
    unsigned refresh = 0;
    bool dirty = true;
    int pending_mission = 0;   // mission to advance to once the current one is running
    int pending_wait = 0;      // frames in state 9 before the pending jump
    bool types_logged = false;
    std::vector<int> spawn_types;          // unit type numbers (1-based) offered on the Spawn page
    std::vector<std::string> spawn_names;
} g;

Overlay g_overlay;

// ---------------------------------------------------------------- game access
bool in_mission() { return global<std::int8_t>(kGame + 0x3a) == 9; }

std::uint8_t* hero() {
    if (!in_mission()) return nullptr;
    const int id = global<int>(d::g_player_unit_recno);
    if (id <= 0) return nullptr;
    if (joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)) != 0) return nullptr;  // 0x00428b70 deleted
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0 get
}

void console(const char* line) {
    char buf[48];
    std::snprintf(buf, sizeof buf, "%s", line);  // the console copies into a 64-byte local
    joan::orig::as_console(img::obj(kConsole))->execute(joan::orig::arg::str(static_cast<const char*>(buf)));  // 0x00454560
    c_actions.hit();
    hook::log_line("[DebugMenu] console: %s", buf);
}

void load_mission_names() {
    for (int i = 0; i < kMissions; ++i) {
        char path[MAX_PATH];
        std::snprintf(path, sizeof path, "%sScenario\\scenario_%02d.scr", hook::data_dir().c_str(), i + 1);
        std::string script;
        // The game's path form (backslashes, any case); off Windows resolved the way the game's own opens resolve it.
        if (FILE* f = std::fopen(platform::files::native_path(path).c_str(), "rb")) {
            char line[256];
            while (std::fgets(line, sizeof line, f)) {
                if (std::strncmp(line, "ScriptFileName", 14) != 0) continue;
                const char* p = std::strchr(line, ':');
                if (!p) continue;
                ++p;
                while (*p == ' ' || *p == '\t') ++p;
                script.assign(p);
                while (!script.empty() && (script.back() == '\r' || script.back() == '\n' || script.back() == ' '))
                    script.pop_back();
                break;
            }
            std::fclose(f);
        }
        g.mission_names[i] = script.empty() ? "?" : script;
    }
}

void start_mission(int n, int keep) {
    hook::log_line("[DebugMenu] starting mission %d (%s)%s", n, g.mission_names[n - 1].c_str(),
                   keep ? " continuing the current world" : "");
    global<std::uint8_t>(d::g_a58f95) = 1;
    joan::orig::as_battle(img::obj(kScenarios))->load_scenario_41cc80(joan::orig::arg::i32(n), 1, joan::orig::arg::i32(keep));  // 0x0041cc80
    joan::orig::as_game(img::obj(kGame))->set_state(0xcu);              // 0x004d7e70 state 12 (loading)
    c_actions.hit();
}

void jump_to_mission(int n) {
    if (!in_mission()) {
        g.status = "Start or load a mission first.";
        return;
    }
    const int cur = global<int>(d::g_mission);
    g.pending_mission = 0;
    if (n == 6 && cur == 5) {
        start_mission(6, 1);  // like winning mission 5
    } else if (n == 6) {
        // Mission 6 continues mission 5's world: load 5, then advance (see process_input).
        hook::log_line("[DebugMenu] mission 6 needs mission 5's world - loading 5 first");
        g.pending_mission = 6;
        g.pending_wait = 0;
        start_mission(5, 0);
    } else {
        start_mission(n, 0);
    }
    g.open = false;
    g.dirty = true;
}

void poll_pending_mission() {
    if (!g.pending_mission) return;
    if (!in_mission() || global<int>(d::g_mission) != g.pending_mission - 1) {
        g.pending_wait = 0;
        if (global<std::int8_t>(kGame + 0x3a) == 9) g.pending_mission = 0;  // ended up elsewhere
        return;
    }
    if (++g.pending_wait < 60) return;  // let mission 5 run for a second first
    const int n = g.pending_mission;
    g.pending_mission = 0;
    start_mission(n, 1);
}

// ---------------------------------------------------------------- pages
struct Stat {
    const char* label;
    int (*get)(const joan::orig::Unit& u);  // the value (Unit +0x155.. words, +0x13d / +0x149 dwords)
    bool settable;    // false: add only
    fn::Code setter;  // thunk of the set_* method (when settable)
    const char* command;
    int step;
};
constexpr Stat kStats[] = {
    {"Strength", [](const joan::orig::Unit& u) -> int { return u.stats[0]; }, true, fn::Obj_set_155, "unit_add_str", 1},    // +0x155, set 0x004fe6e0
    {"Dexterity", [](const joan::orig::Unit& u) -> int { return u.stats[1]; }, true, fn::Obj_set_157, "unit_add_dex", 1},   // +0x157, set 0x004fe710
    {"Defense", [](const joan::orig::Unit& u) -> int { return u.stats[2]; }, true, fn::Obj_set_159, "unit_add_def", 1},     // +0x159, set 0x004fe740
    {"Leadership", [](const joan::orig::Unit& u) -> int { return u.stats[3]; }, true, fn::Obj_set_15b, "unit_add_lea", 1},  // +0x15b, set 0x004fe770
    {"Max HP", [](const joan::orig::Unit& u) -> int { return u.hit_points_base; }, false, fn::kNoCode, "unit_add_hp", 10},  // +0x13d
    {"EP", [](const joan::orig::Unit& u) -> int { return u.base_max_ep; }, false, fn::kNoCode, "unit_add_ep", 10},              // +0x149
};
constexpr int kStatCount = static_cast<int>(sizeof kStats / sizeof kStats[0]);
// Hero page rows: the stats, then "Level up", "Gold +1000", "Back".
constexpr int kMaxLevel = 30;       // console clamp (0x00590aa0(v, 1, 30))
constexpr int kHeroRows = kStatCount + 3;

const char* const kRoot[] = {"Missions", "Hero stats", "Spawn", "Cheats", "Ambient occlusion", "Shadows", "Close"};
constexpr int kRootCount = 7;
// Ambient occlusion page rows: mode, radius, strength, distance, back.
constexpr int kAoRows = 5;
// Shadows page rows: mode, distance, map size, strength, softness, back.
constexpr int kShadowRows = 11;
const char* const kCheats[] = {"Invincible on", "Invincible off", "Toggle fog of war", "Back"};

int row_count() {
    switch (g.page) {
        case Page::Root: return kRootCount;
        case Page::Missions: return kMissions + 1;
        case Page::Hero: return kHeroRows;
        case Page::Spawn: return static_cast<int>(g.spawn_types.size()) + 1;
        case Page::Cheats: return 4;
        case Page::Occlusion: return kAoRows;
        case Page::Shadows: return kShadowRows;
    }
    return 1;
}

void go(Page p) {
    g.page = p;
    g.sel = 0;
    g.status.clear();
    g.dirty = true;
}

void change_stat(int row, int amount) {
    std::uint8_t* const u = hero();
    if (!u) {
        g.status = "No hero (start or load a mission).";
        return;
    }
    const Stat& s = kStats[row];
    if (amount > 0) {
        char cmd[48];
        std::snprintf(cmd, sizeof cmd, "%s %d", s.command, amount);
        console(cmd);
        g.status = cmd;
    } else if (s.settable) {
        const int cur = s.get(*joan::orig::as_unit(u));
        int v = cur + amount;
        if (v < 1) v = 1;
        thiscall(s.setter, u, v);
        c_actions.hit();
        char msg[64];
        std::snprintf(msg, sizeof msg, "%s %d -> %d", s.label, cur, v);
        g.status = msg;
        hook::log_line("[DebugMenu] %s", msg);
    } else {
        g.status = "This value can only be raised.";
    }
}

void level_up(int step) {
    std::uint8_t* const u = hero();
    if (!u) {
        g.status = "No hero (start or load a mission).";
        return;
    }
    const int cur = joan::orig::as_unit(u)->level;  // +0x124
    if (cur >= kMaxLevel) {
        g.status = "Already at the maximum level.";
        return;
    }
    int target = cur + step;
    if (target > kMaxLevel) target = kMaxLevel;
    char cmd[48];
    std::snprintf(cmd, sizeof cmd, "unit_level_up %d", target);  // sets the level (see the header)
    console(cmd);
    const int now = joan::orig::as_unit(u)->level;
    char msg[64];
    std::snprintf(msg, sizeof msg, "Level %d -> %d", cur, now);
    g.status = msg;
    hook::log_line("[DebugMenu] %s", msg);
}

// ---------------------------------------------------------------- spawning
constexpr img::Addr kUnitTypes = d::g_types;
// The type records (joan::orig::UnitType, 0x203 bytes on x86): +4 name, +0x25 body model, +0x46 second model.
const joan::orig::UnitType* types() { return global<const joan::orig::UnitType*>(kUnitTypes); }

int type_count() { return global<std::int16_t>(kUnitTypes + 4); }
const char* type_name(int index) {  // 0-based
    const joan::orig::UnitType* const recs = types();
    return recs ? recs[index].name : "";
}

// The type record names its model files (UNIT\<name.am1>, loaded when a unit is created,
// 0x00524e30 / 0x00525173): +0x25 body, +0x46 second model ("none" = no model). Some types
// in unit.res have no model files (JOAN_HORSE: the game shows "Error loading 3D Model" and
// quits), so only types whose files exist are offered.
bool model_exists(const char* name) {
    if (!name[0]) return false;
    char path[MAX_PATH];
    // the record stores the file name with its extension ("enghorseA.am1")
    std::snprintf(path, sizeof path, "%sUNIT\\%.32s%s", hook::data_dir().c_str(), name,
                  std::strchr(name, '.') ? "" : ".AM1");
#ifdef _WIN32
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#else
    // The name is the game's (backslashes, any case: "enghorseA.am1" is unit/enghorseA.AM1 on disk): resolved the
    // way the game's own opens resolve it.
    std::error_code ec;
    return std::filesystem::exists(platform::files::native_path(path), ec);
#endif
}

bool type_has_models(int index) {
    const joan::orig::UnitType& rec = types()[index];
    const char* const body_src = rec.anim_name;      // +0x25
    const char* const second_src = rec.anim_name_b;  // +0x46
    char body[0x22] = {}, second[0x22] = {};
    std::memcpy(body, body_src, 0x21);  // 0x21 bytes as before: the 32 characters and the byte after them
    std::memcpy(second, second_src, 0x21);
    if (!model_exists(body)) return false;
    const bool none = (second[0] | 0x20) == 'n' && (second[1] | 0x20) == 'o' && (second[2] | 0x20) == 'n' &&
                      (second[3] | 0x20) == 'e' && second[4] == 0;
    return none || model_exists(second);
}

bool contains_nocase(const char* s, const char* what) {  // `what` in upper case
    for (; *s; ++s) {
        std::size_t i = 0;
        while (what[i] && s[i] && (s[i] >= 'a' && s[i] <= 'z' ? s[i] - 32 : s[i]) == what[i]) ++i;
        if (!what[i]) return true;
    }
    return false;
}

void build_spawn_list() {
    g.spawn_types.clear();
    g.spawn_names.clear();
    const int n = global<const void*>(kUnitTypes) ? type_count() : 0;
    std::string all;
    for (int i = 0; i < n; ++i) {
        const char* name = type_name(i);
        if (!g.types_logged) {
            if (!all.empty()) all += ", ";
            all += name;
        }
        if (contains_nocase(name, "HORSE")) {
            if (!type_has_models(i)) {
                hook::log_line("[DebugMenu] %s skipped: model files missing (%.32s / %.32s)", name,
                               types()[i].anim_name, types()[i].anim_name_b);
                continue;
            }
            g.spawn_types.push_back(i + 1);
            g.spawn_names.push_back(name);
        }
    }
    if (!g.types_logged && n > 0) {
        g.types_logged = true;
        hook::log_line("[DebugMenu] %d unit types: %s", n, all.c_str());
    }
}

void spawn(int type, const char* name) {
    std::uint8_t* const h = hero();
    if (!h) {
        g.status = "No hero (start or load a mission).";
        return;
    }
    void* const units = img::obj(kUnits);
    const int id = joan::orig::arg::as<int>(joan::orig::as_units(units)->create_51dfc0(joan::orig::arg::i32(type)));  // 0x0051dfc0 Units::create
    if (id <= 0 || joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(id)) != 0) {
        g.status = "Spawn failed.";
        hook::log_line("[DebugMenu] spawn %s failed (id %d)", name, id);
        return;
    }
    std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id)));
    // as add_unit (0x00655150), on the hero's side
    joan::orig::as_unit(u)->set_side(joan::orig::arg::i32(static_cast<int>(joan::orig::as_unit(h)->side)));  // 0x00509250 set_side
    joan::orig::as_unit(u)->layer = 1;  // +0x102

    // full HP / EP like load_unit (0x00655200): set_hp(max_hp), set_ep(max_ep)
    joan::orig::as_unit(u)->set_value_135(joan::orig::arg::i32(joan::orig::arg::as<int>(joan::orig::as_unit(u)->hit_points_max())));  // 0x004fe510(0x00523320())
    joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(joan::orig::arg::as<int>(joan::orig::as_unit(u)->value_149())));  // 0x004fe5d0(0x00523700())

    // in front of the hero (row 0 of the yaw matrix; row 1 is the hero's left - tested), same heading
    Vec3 pos;
    joan::orig::as_unit(h)->position_slot(joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x004fedb0 get_pos
    Mat44 m;
    const float yaw_f = joan::orig::as_unit(h)->heading;  // +0x1a
    // 0x0060bf40 (yaw, pitch, roll): its replacement (game/unknown_mathlib.cpp) takes the angles' bit patterns as
    // u32 parameters, so the call passes them the same way (integer registers on x64, as the callee reads them)
    reinterpret_cast<void*(JOAN_CDECL*)(void*, std::uint32_t, std::uint32_t, std::uint32_t)>(fn::code_value(fn::init_rotation_actor_hvr))(
        &m, joan::orig::arg::bits(yaw_f), 0u, 0u);
    // carts are long (the hero ended up stuck inside HORSE_CART at 130)
    const float kDist = contains_nocase(name, "CART") ? 400.0f : 130.0f;
    const Vec3 spot{pos.x + m.m[0] * kDist, pos.y + m.m[1] * kDist, pos.z + m.m[2] * kDist};
    auto place = [&] {
        joan::orig::as_unit(u)->set_group(joan::orig::as_unit(h)->world);  // 0x004fed00 sector (+0x335)
        joan::orig::as_ground_object(u)->set_position(spot);                                         // 0x004fedf0 position
    };
    place();
    const float k = global<float>(d::flt_0_017453292);  // degrees -> radians
    const float deg = yaw_f / k;
    joan::orig::as_unit(u)->set_target(joan::orig::arg::f32(deg * k));  // 0x004ff870 (as set_unit_dir)
    joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(deg));      // 0x0050abd0
    if (joan::orig::as_unit(u)->in_scene == 0) {  // +0x2ed
        joan::orig::as_cell_object(u)->attach();  // 0x00442820 activate_unit
        place();
    }
    joan::orig::as_unit(u)->can_be_ridden = 1;  // +0x3c0: set_unit_can_rided_flag(id, 1)
    c_actions.hit();
    char msg[96];
    std::snprintf(msg, sizeof msg, "Spawned %s (unit %d)", name, id);
    g.status = msg;
    hook::log_line("[DebugMenu] %s at %.0f %.0f %.0f (hero at %.0f %.0f %.0f)", msg, spot.x, spot.y, spot.z, pos.x,
                   pos.y, pos.z);
}

void activate(bool shift, int direction) {  // direction: 0 = Enter, +1 / -1 = Right / Left
    switch (g.page) {
        case Page::Root:
            if (direction < 0) return;
            if (g.sel == 0) go(Page::Missions);
            else if (g.sel == 1) go(Page::Hero);
            else if (g.sel == 2) { build_spawn_list(); go(Page::Spawn); }
            else if (g.sel == 3) go(Page::Cheats);
            else if (g.sel == 4) go(Page::Occlusion);
            else if (g.sel == 5) go(Page::Shadows);
            else g.open = false;
            break;
        case Page::Missions:
            if (direction < 0) return;
            if (g.sel == kMissions) go(Page::Root);
            else jump_to_mission(g.sel + 1);
            break;
        case Page::Hero:
            if (g.sel < kStatCount) {
                const int step = kStats[g.sel].step * (shift ? 10 : 1);
                change_stat(g.sel, direction < 0 ? -step : step);
            } else if (direction < 0) {
                return;
            } else if (g.sel == kStatCount) {
                level_up(shift ? 5 : 1);
            } else if (g.sel == kStatCount + 1) {
                if (hero()) { console("unit_add_gold 1000"); g.status = "Gold +1000"; }
                else g.status = "No hero (start or load a mission).";
            } else {
                go(Page::Root);
            }
            break;
        case Page::Spawn:
            if (direction < 0) return;
            if (g.sel >= static_cast<int>(g.spawn_types.size())) { go(Page::Root); break; }
            if (!in_mission()) { g.status = "Start or load a mission first."; break; }
            spawn(g.spawn_types[g.sel], g.spawn_names[g.sel].c_str());
            break;
        case Page::Cheats:
            if (direction < 0) return;
            if (g.sel == 3) { go(Page::Root); break; }
            if (!in_mission()) { g.status = "Start or load a mission first."; break; }
            console(g.sel == 0 ? "invincible 1" : g.sel == 1 ? "invincible 0" : "toggle_fog_of_war");
            g.status = kCheats[g.sel];
            break;
        case Page::Occlusion: {
            // [Graphics] AmbientOcclusion, live (render/ambient_occlusion.hpp); the values are logged so
            // they can be copied into openjoan.ini.
            auto& a = render::ao_params();
            const int dir = direction == 0 ? 1 : direction;
            if (g.sel == kAoRows - 1) {
                if (direction >= 0) go(Page::Root);
                break;
            }
            if (g.sel == 0) a.mode = (a.mode + dir + 3) % 3;
            else if (g.sel == 1) a.radius = std::max(2.0f, a.radius + static_cast<float>(dir) * (shift ? 25.0f : 5.0f));
            else if (g.sel == 2) a.strength = std::max(0.0f, std::min(4.0f, a.strength + static_cast<float>(dir) * (shift ? 0.5f : 0.1f)));
            else if (g.sel == 3) {
                a.fade_start = std::max(250.0f, a.fade_start + static_cast<float>(dir) * (shift ? 2500.0f : 250.0f));
                a.fade_end = a.fade_start * 2.0f;
            }
            char line[160];
            std::snprintf(line, sizeof line, "AmbientOcclusion=%d  Radius=%g  Strength=%.1f  Distance=%g", a.mode,
                          static_cast<double>(a.radius), static_cast<double>(a.strength), static_cast<double>(a.fade_start));
            g.status = line;
            hook::log_line("[DebugMenu] %s", line);
            break;
        }
        case Page::Shadows: {
            // [Graphics] Shadows, live (render/shadow_map.hpp); logged in openjoan.ini's terms.
            auto& s = render::shadow_params();
            const int dir = direction == 0 ? 1 : direction;
            if (g.sel == kShadowRows - 1) {
                if (direction >= 0) go(Page::Root);
                break;
            }
            if (g.sel == 0) {
                s.mode = (s.mode + dir + 3) % 3;
            } else if (g.sel == 1) {
                s.distance = std::max(500.0f, std::min(30000.0f, s.distance + static_cast<float>(dir) * (shift ? 2500.0f : 500.0f)));
            } else if (g.sel == 2) {
                static const int kSizes[] = {1024, 2048, 4096, 8192};
                int i = 0;
                while (i < 3 && kSizes[i] < s.map_size) ++i;
                s.map_size = kSizes[std::max(0, std::min(3, i + dir))];
            } else if (g.sel == 3) {
                s.strength = std::max(0.0f, std::min(1.0f, s.strength + static_cast<float>(dir) * (shift ? 0.2f : 0.05f)));
            } else if (g.sel == 4) {
                s.softness = std::max(0.0f, std::min(32.0f, s.softness + static_cast<float>(dir) * (shift ? 2.0f : 0.5f)));
            } else if (g.sel == 5) {
                s.foliage = !s.foliage;
            } else if (g.sel == 6) {
                s.baked = std::max(0.0f, std::min(1.0f, s.baked + static_cast<float>(dir) * (shift ? 0.25f : 0.05f)));
            } else if (g.sel == 7) {
                s.caster_margin = std::max(0.0f, std::min(20000.0f, s.caster_margin + static_cast<float>(dir) * (shift ? 2500.0f : 500.0f)));
            } else if (g.sel == 8) {
                render::surface_params().pixel_lighting = !render::surface_params().pixel_lighting;
            } else if (g.sel == 9) {
                s.debug = s.debug ? 0 : 1;
            }
            const auto& sp = render::surface_params();
            char line[400];
            std::snprintf(line, sizeof line,
                          "Shadows=%d  ShadowDistance=%.0f  ShadowMapSize=%d  ShadowStrength=%.0f  ShadowSoftness=%g  ShadowFoliage=%d  "
                          "BakedShadows=%.0f  ShadowCasterMargin=%.0f  PixelLighting=%d",
                          s.mode, static_cast<double>(s.distance), s.map_size, static_cast<double>(s.strength * 100.0f),
                          static_cast<double>(s.softness), s.foliage ? 1 : 0, static_cast<double>(s.baked * 100.0f),
                          static_cast<double>(s.caster_margin), sp.pixel_lighting ? 1 : 0);
            g.status = line;
            hook::log_line("[DebugMenu] %s", line);
            break;
        }
    }
    g.dirty = true;
}

// ---------------------------------------------------------------- drawing (GDI into a DIB; elsewhere SDL)
constexpr int kWidth = 520, kLineH = 20, kPad = 12;
constexpr COLORREF kBg = RGB(14, 18, 26), kSelBg = RGB(52, 74, 116);

#ifdef _WIN32
struct Canvas {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HFONT font = nullptr, bold = nullptr;
    std::uint32_t* bits = nullptr;
    int w = 0, h = 0;
} cv;

bool ensure_canvas(int h) {
    if (cv.bits && cv.h == h) return true;
    if (!cv.dc) {
        cv.dc = CreateCompatibleDC(nullptr);
        cv.font = CreateFontA(-15, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
        cv.bold = CreateFontA(-16, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              ANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
    }
    if (cv.bmp) DeleteObject(cv.bmp);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof bi.bmiHeader;
    bi.bmiHeader.biWidth = kWidth;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    cv.bmp = CreateDIBSection(cv.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!cv.bmp) return false;
    SelectObject(cv.dc, cv.bmp);
    cv.bits = static_cast<std::uint32_t*>(bits);
    cv.w = kWidth;
    cv.h = h;
    return true;
}
#else
// No GDI elsewhere: SDL's software renderer on an ARGB surface (the same 0xAARRGGBB pixels as the DIB) and its
// built-in 8x8 font, drawn 8x16.
struct Canvas {
    SDL_Surface* surface = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::uint32_t* bits = nullptr;
    int w = 0, h = 0;
} cv;

bool ensure_canvas(int h) {
    if (cv.bits && cv.h == h) return true;
    if (cv.renderer) SDL_DestroyRenderer(cv.renderer);
    if (cv.surface) SDL_DestroySurface(cv.surface);
    cv.renderer = nullptr;
    cv.bits = nullptr;
    cv.surface = SDL_CreateSurface(kWidth, h, SDL_PIXELFORMAT_ARGB8888);
    if (!cv.surface || cv.surface->pitch != kWidth * 4) return false;
    cv.renderer = SDL_CreateSoftwareRenderer(cv.surface);
    if (!cv.renderer) return false;
    cv.bits = static_cast<std::uint32_t*>(cv.surface->pixels);
    cv.w = kWidth;
    cv.h = h;
    return true;
}

void set_color(COLORREF c) {
    SDL_SetRenderDrawColor(cv.renderer, static_cast<Uint8>(c & 0xffu), static_cast<Uint8>((c >> 8) & 0xffu),
                           static_cast<Uint8>((c >> 16) & 0xffu), 0xff);
}
#endif

struct Line {
    std::string text;
    COLORREF color;
    bool selected;
    bool title;
};

void render() {
    std::vector<Line> lines;
    auto add = [&](std::string t, COLORREF c, bool sel = false, bool title = false) {
        lines.push_back({std::move(t), c, sel, title});
    };
    const COLORREF white = RGB(230, 230, 230), grey = RGB(140, 150, 165), gold = RGB(255, 210, 90),
                   green = RGB(140, 230, 140);
    char buf[160];
    switch (g.page) {
        case Page::Root:
            add("JOAN-RE DEBUG MENU", gold, false, true);
            for (int i = 0; i < kRootCount; ++i)
                add(std::string(kRoot[i]) + (i < kRootCount - 1 ? "  >" : ""), white, g.sel == i);
            break;
        case Page::Missions: {
            add("MISSIONS", gold, false, true);
            const int cur = in_mission() ? global<int>(d::g_mission) : 0;
            for (int i = 0; i < kMissions; ++i) {
                std::snprintf(buf, sizeof buf, "Mission %d   %-8s%s", i + 1, g.mission_names[i].c_str(),
                              cur == i + 1 ? "  (current)" : i == 5 && cur != 5 ? "  (via mission 5)" : "");
                add(buf, cur == i + 1 ? green : white, g.sel == i);
            }
            add("< Back", white, g.sel == kMissions);
            add(in_mission() ? "Enter: start the mission" : "Start or load a mission to enable jumping", grey);
            break;
        }
        case Page::Hero: {
            add("HERO STATS", gold, false, true);
            std::uint8_t* const u = hero();
            for (int i = 0; i < kStatCount; ++i) {
                const Stat& s = kStats[i];
                if (u) {
                    const int v = s.get(*joan::orig::as_unit(u));
                    std::snprintf(buf, sizeof buf, "%-11s %6d   %s", s.label, v, s.settable ? "< - / + >" : "    + >");
                } else {
                    std::snprintf(buf, sizeof buf, "%-11s      -", s.label);
                }
                add(buf, white, g.sel == i);
            }
            if (u) std::snprintf(buf, sizeof buf, "Level up    %6d       + >", joan::orig::as_unit(u)->level);
            else std::snprintf(buf, sizeof buf, "Level up");
            add(buf, white, g.sel == kStatCount);
            add("Gold +1000", white, g.sel == kStatCount + 1);
            add("< Back", white, g.sel == kStatCount + 2);
            add(u ? "Left/Right: -/+  (Shift: x10)" : "No hero: start or load a mission", grey);
            break;
        }
        case Page::Spawn: {
            add("SPAWN (in front of the hero)", gold, false, true);
            for (std::size_t i = 0; i < g.spawn_types.size(); ++i)
                add("Spawn " + g.spawn_names[i], white, g.sel == static_cast<int>(i));
            if (g.spawn_types.empty()) add("No horse unit types found (list in openjoan.log)", grey);
            add("< Back", white, g.sel == static_cast<int>(g.spawn_types.size()));
            break;
        }
        case Page::Cheats:
            add("CHEATS", gold, false, true);
            for (int i = 0; i < 4; ++i) add(kCheats[i], white, g.sel == i);
            break;
        case Page::Occlusion: {
            add("AMBIENT OCCLUSION", gold, false, true);
            const auto& a = render::ao_params();
            static const char* const kModes[] = {"off", "on", "occlusion only"};
            std::snprintf(buf, sizeof buf, "Mode        %-15s < >", kModes[a.mode < 0 || a.mode > 2 ? 0 : a.mode]);
            add(buf, white, g.sel == 0);
            std::snprintf(buf, sizeof buf, "Radius      %6.0f units    < - / + >", static_cast<double>(a.radius));
            add(buf, white, g.sel == 1);
            std::snprintf(buf, sizeof buf, "Strength    %6.1f          < - / + >", static_cast<double>(a.strength));
            add(buf, white, g.sel == 2);
            std::snprintf(buf, sizeof buf, "Distance    %6.0f units    < - / + >", static_cast<double>(a.fade_start));
            add(buf, white, g.sel == 3);
            add("< Back", white, g.sel == 4);
            add("Left/Right: -/+  (Shift: x5)  Joan is ~79 units tall", grey);
            break;
        }
        case Page::Shadows: {
            add("SHADOWS", gold, false, true);
            const auto& s = render::shadow_params();
            static const char* const kModes[] = {"off", "original", "shadow map"};
            std::snprintf(buf, sizeof buf, "Mode        %-15s < >", kModes[s.mode < 0 || s.mode > 2 ? 0 : s.mode]);
            add(buf, white, g.sel == 0);
            std::snprintf(buf, sizeof buf, "Distance    %6.0f units    < - / + >", static_cast<double>(s.distance));
            add(buf, white, g.sel == 1);
            std::snprintf(buf, sizeof buf, "Map size    %6d texels   < - / + >", s.map_size);
            add(buf, white, g.sel == 2);
            std::snprintf(buf, sizeof buf, "Strength    %6.0f %%        < - / + >", static_cast<double>(s.strength * 100.0f));
            add(buf, white, g.sel == 3);
            std::snprintf(buf, sizeof buf, "Softness    %6.1f texels   < - / + >", static_cast<double>(s.softness));
            add(buf, white, g.sel == 4);
            std::snprintf(buf, sizeof buf, "Foliage     %-15s < >", s.foliage ? "casts" : "does not cast");
            add(buf, white, g.sel == 5);
            std::snprintf(buf, sizeof buf, "Baked kept  %6.0f %%        < - / + >", static_cast<double>(s.baked * 100.0f));
            add(buf, white, g.sel == 6);
            std::snprintf(buf, sizeof buf, "Margin      %6.0f units    < - / + >", static_cast<double>(s.caster_margin));
            add(buf, white, g.sel == 7);
            const auto& sp = render::surface_params();
            std::snprintf(buf, sizeof buf, "Pixel light %-15s < >", sp.pixel_lighting ? "on" : "off");
            add(buf, white, g.sel == 8);
            static const char* const kDebug[] = {"off", "shadow map"};
            std::snprintf(buf, sizeof buf, "Debug view  %-15s < >", kDebug[s.debug ? 1 : 0]);
            add(buf, white, g.sel == 9);
            add("< Back", white, g.sel == 10);
            add(s.mode == 2 ? "Distance..Baked: shadow map only  (Shift: bigger steps)" : "Shadows need the game's own shadow option on", grey);
            break;
        }
    }
    if (!g.status.empty()) add(g.status, green);
    add("Up/Down  Enter/Right  Esc/Backspace: back", grey);

    const int h = kPad * 2 + static_cast<int>(lines.size()) * kLineH;
    if (!ensure_canvas(h)) return;
#ifdef _WIN32
    RECT all = {0, 0, cv.w, cv.h};
    HBRUSH bg = CreateSolidBrush(kBg), sel = CreateSolidBrush(kSelBg);
    FillRect(cv.dc, &all, bg);
    SetBkMode(cv.dc, TRANSPARENT);
    int y = kPad;
    for (const Line& l : lines) {
        if (l.selected) {
            RECT r = {4, y - 1, cv.w - 4, y + kLineH - 1};
            FillRect(cv.dc, &r, sel);
        }
        SelectObject(cv.dc, l.title ? cv.bold : cv.font);
        SetTextColor(cv.dc, l.color);
        const std::string t = (l.selected ? "> " : "  ") + l.text;
        TextOutA(cv.dc, kPad, y, t.c_str(), static_cast<int>(t.size()));
        y += kLineH;
    }
    DeleteObject(bg);
    DeleteObject(sel);
    GdiFlush();
#else
    SDL_SetRenderScale(cv.renderer, 1.0f, 1.0f);
    set_color(kBg);
    SDL_RenderClear(cv.renderer);
    int y = kPad;
    for (const Line& l : lines) {
        if (l.selected) {
            const SDL_FRect r = {4.0f, static_cast<float>(y - 1), static_cast<float>(cv.w - 8), static_cast<float>(kLineH)};
            set_color(kSelBg);
            SDL_RenderFillRect(cv.renderer, &r);
        }
        set_color(l.color);
        const std::string t = (l.selected ? "> " : "  ") + l.text;
        SDL_SetRenderScale(cv.renderer, 1.0f, 2.0f);  // 8x16 glyphs, centred in the line
        const float ty = static_cast<float>(y + (kLineH - 16) / 2 - 1) / 2.0f;
        SDL_RenderDebugText(cv.renderer, static_cast<float>(kPad), ty, t.c_str());
        if (l.title) SDL_RenderDebugText(cv.renderer, static_cast<float>(kPad + 1), ty, t.c_str());  // bold
        SDL_SetRenderScale(cv.renderer, 1.0f, 1.0f);
        y += kLineH;
    }
    SDL_FlushRenderer(cv.renderer);
#endif
    // Alpha: background translucent, everything drawn on it opaque (GDI leaves alpha at 0, SDL at 0xff).
    // DIB pixels are 0x00RRGGBB, COLORREF is 0x00BBGGRR. (GetRValue etc. on a constexpr value trip MSVC C4310.)
    const std::uint32_t bg_rgb = ((kBg & 0xffu) << 16) | (kBg & 0xff00u) | ((kBg >> 16) & 0xffu);
    for (int i = 0; i < cv.w * cv.h; ++i) {
        const std::uint32_t p = cv.bits[i] & 0x00ffffffu;
        cv.bits[i] = p | (p == bg_rgb ? 0xd8000000u : 0xff000000u);
    }
    g_overlay.width = cv.w;
    g_overlay.height = cv.h;
    g_overlay.pixels = cv.bits;
    ++g_overlay.version;
}

// ---------------------------------------------------------------- input
bool key_pressed(int vk) {
    const bool down = platform::key_down(vk);
    const bool edge = down && !g.prev[vk];
    g.prev[vk] = down;
    return edge;
}

void handle_keys() {
    const bool shift = platform::key_down(VK_SHIFT);
    const int n = row_count();
    if (key_pressed(VK_UP)) { g.sel = (g.sel + n - 1) % n; g.dirty = true; }
    if (key_pressed(VK_DOWN)) { g.sel = (g.sel + 1) % n; g.dirty = true; }
    if (key_pressed(VK_RETURN)) activate(shift, 0);
    if (key_pressed(VK_RIGHT)) activate(shift, +1);
    if (key_pressed(VK_LEFT)) {
        if (g.page == Page::Hero && g.sel < kStatCount) activate(shift, -1);
        else if (g.page == Page::Occlusion && g.sel < kAoRows - 1) activate(shift, -1);
        else if (g.page == Page::Shadows && g.sel < kShadowRows - 1) activate(shift, -1);
        else if (g.page != Page::Root) go(Page::Root);
    }
    const bool esc = key_pressed(VK_ESCAPE), back = key_pressed(VK_BACK);
    if (esc || back) {
        if (g.page == Page::Root) g.open = false;
        else go(Page::Root);
        g.dirty = true;
    }
}

// ---------------------------------------------------------------- the per-frame hook
struct H {
    void process_input() {  // replaces Game::process_input 0x0052f930 (thiscall, ret)
        c_frames.hit();
        if (!g.loaded) {
            g.loaded = true;
            g.menu_key = static_cast<int>(
                joan::platform::ini::get_int("Debug", "MenuKey", 0xC0, hook::ini_path().c_str()));
            if (g.menu_key <= 0 || g.menu_key > 255) g.menu_key = 0xC0;
            load_mission_names();
            hook::log_line("[DebugMenu] ready (menu key 0x%02X)", g.menu_key);
        }
        const bool focused = platform::window_focused(global<HWND>(d::g_main_hwnd));
        const bool toggle = focused && key_pressed(g.menu_key);
        if (toggle) {
            g.open = !g.open;
            if (g.open) {
                go(Page::Root);
                // Swallow the keys that are already down so they don't act on the menu.
                static const int kNav[] = {VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_RETURN, VK_ESCAPE, VK_BACK};
                for (int vk : kNav) g.prev[vk] = platform::key_down(vk);
            }
            g.dirty = true;
        }
        // [Debug] FreecamKey (freecam.hpp): while it flies, the game gets no input either.
        freecam::frame(focused, g.open);
        const bool was_open = g.open || toggle || freecam::takes_input();
        if (g.open && focused && !toggle) handle_keys();
        poll_pending_mission();
        if (g.open && (g.dirty || ++g.refresh >= 15)) {  // values change while the game runs
            g.refresh = 0;
            g.dirty = false;
            render();
        }
        g_overlay.visible = g.open;
        if (was_open) {
            // The game ignores input while the menu is open (and on the frame it closes): drop the
            // queued key / mouse events of the input object 0x00a516b0 (ring read index +0xd65 =
            // write index +0xd61, the queue Game::process_input consumes through 0x005932e0).
            global<std::int32_t>(kInput + 0xd65) = global<std::int32_t>(kInput + 0xd61);
            return;
        }
        // The game's own input handling: our port (module game/b117_04, which then yields 0x0052f930 to
        // this hook) when it is enabled, else the original body (only the thunk 0x00407815 is redirected).
        static const bool port = hook::module_enabled("game/b117_04");
        if (port)
            game_b117_04::process_input(this);
        else
            thiscall(fn::Game_process_input_debug_menu_body, this);
    }
};

}  // namespace

const Overlay& overlay() { return g_overlay; }

JOAN_REPLACE("debug/menu", "Game::process_input (debug menu)", 0x0052f930, &H::process_input);

}  // namespace joan::debugmenu
