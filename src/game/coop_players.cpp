// SPDX-License-Identifier: GPL-3.0-or-later
// coop_players.hpp. For now only a test: [Debug] CoopTestPlayer=1 gives a second hero of the party to a scripted player on
// this PC (walk, turn, attack), to see two heroes under player control at once before any network is involved.
#include "coop_players.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

#include "../data_names.hpp"
#include "../original/direct.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../platform/ini.hpp"
#include "../registry.hpp"

namespace jo = joan::orig;
using joan::hook::global;
using joan::img::obj;

namespace joan::coop {

namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

/// A player other than the host: its own Player and Input objects, kept here while the game uses the host's.
struct OtherPlayer {
    std::vector<u8> player;  // a jo::Player
    std::vector<u8> input;   // a jo::Input
    u32 frames = 0;          // frames this player has run (the scripted test's clock)
};

std::vector<OtherPlayer> g_others;
bool g_tried = false;

bool test_player_wanted() {
    static const bool on = platform::ini::get_int("Debug", "CoopTestPlayer", 0, hook::ini_path().c_str()) != 0;
    return on;
}

void* player_object() { return obj(d::g_player_unit_recno); }
void* input_object() { return obj(d::g_mouse); }

u8* unit_of(i32 id) {
    void* const list = obj(d::g_units);
    if (id <= 0 || joan::orig::as_dynarray(list)->empty_at(joan::orig::arg::i32(id)) != 0) return nullptr;  // 0x00428b70
    return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
}

const u8* type_of(const u8* unit) {
    return joan::orig::arg::as<const u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
}

// A hero of the host's side other than the host's, alive and present: the test player's hero.
i32 free_hero(i32 host_id) {
    const u8* const host = unit_of(host_id);
    if (host == nullptr) return 0;
    const i32 last = joan::orig::as_dynarray(obj(d::g_units))->size();
    for (i32 id = 1; id <= last; ++id) {
        if (id == host_id) continue;
        u8* const u = unit_of(id);
        if (u == nullptr || jo::as_unit(u)->actor == nullptr || jo::as_unit(u)->side != jo::as_unit(host)->side) continue;
        if (jo::as_unit_type(type_of(u))->is_hero == 0) continue;
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(u)->dead()) != 0) continue;  // 0x0042d0e0
        return id;
    }
    return 0;
}

void start_test_player() {
    g_tried = true;
    const i32 host_id = jo::self<jo::Player>(player_object())->unit_id_;
    const i32 hero = free_hero(host_id);
    if (hero == 0) {
        hook::log_line("[Coop] CoopTestPlayer: no other hero of the party to give to a second player");
        return;
    }
    OtherPlayer p;
    const auto* const player = static_cast<const u8*>(player_object());
    const auto* const input = static_cast<const u8*>(input_object());
    p.player.assign(player, player + sizeof(jo::Player));
    p.input.assign(input, input + sizeof(jo::Input));
    auto* const pl = reinterpret_cast<jo::Player*>(p.player.data());
    pl->unit_id_ = hero;
    auto* const in = reinterpret_cast<jo::Input*>(p.input.data());
    std::memset(in->key_state, 0, sizeof in->key_state);
    in->delta_x = in->delta_y = in->delta_z = 0;
    in->scan_code = in->key_code = 0;
    in->queue_head = in->queue_tail;  // no queued events
    jo::as_unit(unit_of(hero))->layer = 0;  // driven by a player, not by its AI
    g_others.push_back(std::move(p));
    hook::log_line("[Coop] CoopTestPlayer: a scripted second player controls unit %d (the host controls %d)", hero, host_id);
}

// The scripted test input of frame `f`: four seconds each of walking, walking and turning, standing and attacking.
void script(OtherPlayer& p) {
    auto* const in = reinterpret_cast<jo::Input*>(p.input.data());
    std::memset(in->key_state, 0, sizeof in->key_state);
    in->scan_code = 0;
    in->delta_x = in->delta_y = in->delta_z = 0;
    const auto* const bindings = jo::view<const u8*>(jo::as_remap_keys(obj(d::g_keys_a48820))->bindings);
    const auto key_of = [&](u32 action) { return jo::as_key_binding(bindings + (action - 1u) * 0x30u)->key_mouse; };
    const auto hold = [&](u32 action) {
        if (const u32 k = key_of(action); k != 0 && k < 256) in->key_state[k] = 0x80;
    };
    const u32 phase = (p.frames / 240u) % 3u;  // about four seconds at 60 frames a second
    if (phase == 0) hold(4);
    if (phase == 1) {
        hold(4);
        hold(6);
    }
    if (phase == 2 && p.frames % 45u == 0) in->scan_code = key_of(0xd);  // an attack every 0.75 s
    ++p.frames;
}

// Runs `step` with player `p` in the game's places.
template <class Step>
void as_player(OtherPlayer& p, Step step) {
    auto* const player = static_cast<u8*>(player_object());
    auto* const input = static_cast<u8*>(input_object());
    std::vector<u8> host_player(player, player + p.player.size()), host_input(input, input + p.input.size());
    std::memcpy(player, p.player.data(), p.player.size());
    std::memcpy(input, p.input.data(), p.input.size());
    step();
    std::memcpy(p.player.data(), player, p.player.size());
    std::memcpy(p.input.data(), input, p.input.size());
    std::memcpy(player, host_player.data(), host_player.size());
    std::memcpy(input, host_input.data(), host_input.size());
}

bool in_mission() {
    const i32 host_id = jo::self<jo::Player>(player_object())->unit_id_;
    return unit_of(host_id) != nullptr;
}

}  // namespace

void other_players_input() {
    if (!test_player_wanted()) return;
    if (!g_tried && in_mission()) start_test_player();
    for (OtherPlayer& p : g_others) {
        if (unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) == nullptr) continue;
        script(p);
        as_player(p, [] { joan::orig::as_player(player_object())->on_key_42e1a0(); });  // 0x0042e1a0
    }
}

void other_players_tick() {
    for (OtherPlayer& p : g_others) {
        if (unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) == nullptr) continue;
        as_player(p, [] { joan::orig::as_player(player_object())->tick(); });  // 0x0042f700
    }
}

}  // namespace joan::coop
