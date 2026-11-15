// SPDX-License-Identifier: GPL-3.0-or-later
// coop_players.hpp. A player on another computer (docs/COOP.md): its game sends its keyboard and mouse items to the host
// (platform::input::forward, replay::coop_send); the host runs the game's own Input::poll on them with that player's Input
// object in place (platform::input::read_from), and the poll's device reads are recorded, so every game replays them.
// [Debug] CoopTestPlayer=1 (no network) gives a second hero to a scripted player on this PC instead (walk, turn, attack).
#include "coop_players.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <unordered_map>
#include <vector>

#include "../data_names.hpp"
#include "../original/direct.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../platform/ini.hpp"
#include "../platform/input.hpp"
#include "../platform/window.hpp"
#include "../gl/views.hpp"
#include "../registry.hpp"
#include "../runtime/replay/replay.hpp"

namespace jo = joan::orig;
using joan::hook::global;
using joan::img::obj;

namespace joan::coop {

namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
namespace pin = joan::platform::input;

/// A player other than the host: its own Player and Input objects, kept here while the game uses the host's.
struct OtherPlayer {
    std::vector<u8> player;  // a jo::Player
    std::vector<u8> input;   // a jo::Input
    std::vector<u8> camera;  // its camera: a jo::CameraFollow and the six jo::CameraMotion it points to
    std::vector<u8> node;    // its camera node's pose (jo::Camera prev_params and params)
    std::unordered_map<const void*, jo::CamZones> zones;  // its state of each loaded cell's camera zones
    std::vector<u8> hud;     // its HUD values (the jo::Hud block of player_hud_range())
    std::int16_t hero_id = 0;  // its copy of the player's hero id (0x00a47c36, Player::set_unit 0x0042f9c0)
    u8 side[sizeof(std::int32_t)] = {};  // its copy of the player's side (0x00a57fae, Player::switch_to)
    std::int16_t map_unit_id = 0;  // its minimap's tracked unit (MiniMap::unit_id / unit, MiniMap::track_unit 0x0070fb70)
    jo::Unit* map_unit = nullptr;
    std::int16_t map_rotation = 0;  // and its turn (MiniMap::rotation)
    float map_scroll[2] = {};       // and its scroll (MiniMap::scale_x / scale_y, set by Marker::update 0x00714040)
    std::uint32_t map_scroll_flag = 0;  // MiniMap::u32_25, set with them
    std::vector<u8> grass;  // its dynamic grass (a jo::DynGrass with its own patch arrays), empty until made
    u32 frames = 0;          // frames this player has run (the scripted test's clock)
    bool remote = false;     // on another computer (else the scripted test player)
    std::deque<pin::Item> mouse, keys;  // the host: its items received and not read by the game yet
};

std::vector<OtherPlayer> g_others;
bool g_tried = false;
bool g_polling = false;  // polling_other_player()
bool g_hidden = false;   // drawing_hidden_view()

JOAN_COUNTER(c_extra_view, "Coop::extra_view");
JOAN_COUNTER(c_view_frames, "Coop::view_frames");

// The items of the network: a device byte (pin::Device) and the item, as sent by a player.
constexpr std::size_t kWireItem = 1 + sizeof(pin::Item);
std::vector<u8> g_received;          // the host: bytes received, not a whole item yet
std::vector<u8> g_to_host;           // a player: items of this computer not sent yet
constexpr std::size_t kMaxUnsent = 1024 * kWireItem;
u32 g_sequence = 0;                  // the host: sequence numbers of the received items (one order for both devices)

bool test_player_wanted() {
    static const bool on = platform::ini::get_int("Debug", "CoopTestPlayer", 0, hook::ini_path().c_str()) != 0;
    return on;
}

void* player_object() { return obj(d::g_player_unit_recno); }
void* input_object() { return obj(d::g_mouse); }
void* g_camera = nullptr;  // the game's camera (CameraFollow), passed by the hooks
void* camera_object() { return g_camera; }

// The camera (CameraFollow at 0x00a48cc0) and the motions it steers (six, one block: CamCtl 0x00562f40 cam(0..5)) are one
// object of the game, which Player::on_key and Player::tick move after "the player's" hero: every player has a copy.
constexpr int kCameraMotions = 6;
constexpr std::size_t kCameraBytes = sizeof(jo::CameraFollow) + kCameraMotions * sizeof(jo::CameraMotion);

void save_camera(std::vector<u8>& to) {
    to.resize(kCameraBytes);
    const auto* const follow = static_cast<const jo::CameraFollow*>(camera_object());
    std::memcpy(to.data(), follow, sizeof(jo::CameraFollow));
    std::memcpy(to.data() + sizeof(jo::CameraFollow), follow->motions, kCameraMotions * sizeof(jo::CameraMotion));
}

// The game's camera becomes `from`. The pointers stay the game's: the camera node, the light, the motions block, and the
// path each motion tracks (a cutscene's: a copy must never keep one the game has freed).
// The camera node the views are drawn from (CameraFollow::camera): World::draw 0x0044c800 sets its pose from the active
// motion (Camera::set_frame 0x00614040: params, the previous ones kept in prev_params) and draws through it. Only the
// pose is a player's own: the draw ranges and fog are the game's (Camera::update_5634e0 sets them each frame), and
// the matrices are rebuilt from the pose (Camera::dirty 0x00614170).
void save_node(std::vector<u8>& to) {
    const jo::Camera* const node = static_cast<const jo::CameraFollow*>(camera_object())->camera;
    to.resize(2 * sizeof(jo::CamParams));
    std::memcpy(to.data(), &node->prev_params, sizeof(jo::CamParams));
    std::memcpy(to.data() + sizeof(jo::CamParams), &node->params, sizeof(jo::CamParams));
}

void load_node(const std::vector<u8>& from) {
    jo::Camera* const node = static_cast<jo::CameraFollow*>(camera_object())->camera;
    std::memcpy(&node->prev_params, from.data(), sizeof(jo::CamParams));
    std::memcpy(&node->params, from.data() + sizeof(jo::CamParams), sizeof(jo::CamParams));
    joan::orig::as_camera(node)->dirty();  // 0x00614170
}

// Every loaded cell has camera zones (CameraZone::update 0x00425fc0, run by World::draw for each cell) that move the
// zone camera after one unit, set from "the player's" hero when they are made (CamZones::init 0x00425e20), and keep
// which zone it is in and where it was. Player `p`'s copies (its hero, its zone) go in for its view and come out after.
template <class Each>
void for_camera_zones(void* world, Each each) {
    auto* const w = static_cast<jo::World*>(world);
    for (std::int32_t i = 0; i < w->cell_count; ++i) {
        auto* const c = reinterpret_cast<jo::Cell*>(w->cells[i].item);
        if (c != nullptr && c->loaded != 0 && c->camera_zones != nullptr) each(*c->camera_zones);
    }
}

void zones_in(OtherPlayer& p, void* world, std::unordered_map<const void*, jo::CamZones>& host) {
    const i32 hero = reinterpret_cast<const jo::Player*>(p.player.data())->unit_id_;
    std::unordered_map<const void*, jo::CamZones> mine;
    for_camera_zones(world, [&](jo::CamZones& z) {
        host[&z] = z;
        jo::CamZones own = z;
        if (const auto it = p.zones.find(&z); it != p.zones.end()) {
            own = it->second;
        } else {  // a cell this player's view has not had yet: its zones start afresh for the hero
            own.current_zone = 0;
            std::memset(own.last_position, 0, sizeof own.last_position);
        }
        own.unit_id = hero;
        own.tracks = z.tracks;  // the game's
        z = own;
        mine[&z] = own;
    });
    p.zones = std::move(mine);  // cells gone since: forgotten
}

void zones_out(OtherPlayer& p, void* world, const std::unordered_map<const void*, jo::CamZones>& host) {
    for_camera_zones(world, [&](jo::CamZones& z) {
        p.zones[&z] = z;
        if (const auto it = host.find(&z); it != host.end()) z = it->second;
    });
}

void load_camera(const std::vector<u8>& from) {
    auto* const follow = static_cast<jo::CameraFollow*>(camera_object());
    jo::CameraMotion* const motions = follow->motions;
    jo::Camera* const node = follow->camera;
    jo::Light* const light = follow->light;
    std::memcpy(follow, from.data(), sizeof(jo::CameraFollow));
    follow->motions = motions;
    follow->camera = node;
    follow->light = light;
    for (int i = 0; i < kCameraMotions; ++i) {
        jo::CamTrackNode* const tracked = motions[i].tracked;
        const std::int32_t count = motions[i].tracked_count;
        std::memcpy(&motions[i], from.data() + sizeof(jo::CameraFollow) + i * sizeof(jo::CameraMotion), sizeof(jo::CameraMotion));
        motions[i].tracked = tracked;
        motions[i].tracked_count = count;
    }
}

void* units() { return obj(d::g_units); }

// The game's window object (Game::run's Wnd): frame_ms / game_dt are the frame's elapsed time and step.
jo::Wnd* game_window() { return static_cast<jo::Wnd*>(obj(d::g_game_a50088)); }

u8* unit_of(i32 id) {
    void* const list = units();
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
    const i32 last = joan::orig::as_dynarray(units())->size();
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

// The HUD's values of "the player" (Hud 0x00a4d618): the shown health, mana and experience moving toward the hero's,
// the target, the experience / level / combo animations, the squad slots, the arrows and the objective fades - all
// value fields from shown_hp to the squad buttons. Its sprites, buttons and minimap stay the game's.
constexpr std::size_t kHudFirst = offsetof(jo::Hud, shown_hp);
constexpr std::size_t kHudEnd = offsetof(jo::Hud, squad_button0);
void* hud_object() { return obj(d::g_player_hud); }
// The HUD's minimap (Hud +0x1d69): the unit it follows, its turn and its scroll are the player's (the draw recomputes
// the scroll from the unit in layer 1 and draws the map with it in the others), its markers the mission's.
jo::MiniMap* minimap() { return &static_cast<jo::Hud*>(hud_object())->minimap; }
u8* hud_values() { return static_cast<u8*>(hud_object()) + kHudFirst; }

// The two globals Player::set_unit (0x0042f9c0) and switch_to set for "the player": its hero's id and side.
std::int16_t& hero_global() { return *static_cast<std::int16_t*>(obj(d::g_a47c36)); }
u8* side_global() { return static_cast<u8*>(obj(d::g_player_side)); }

OtherPlayer* g_current = nullptr;  // the player in the game's places (as_player), nullptr = the host
i32 g_host_unit = 0;               // the host's hero while another player is in its places

void start_player(bool remote) {
    g_tried = true;
    const char* const what = remote ? "the player on the other computer" : "CoopTestPlayer: a scripted second player";
    const i32 host_id = jo::self<jo::Player>(player_object())->unit_id_;
    const i32 hero = free_hero(host_id);
    if (hero == 0) {
        hook::log_line("[Coop] no other hero of the party for %s", what);
        return;
    }
    OtherPlayer p;
    const auto* const player = static_cast<const u8*>(player_object());
    const auto* const input = static_cast<const u8*>(input_object());
    p.player.assign(player, player + sizeof(jo::Player));
    p.input.assign(input, input + sizeof(jo::Input));
    save_camera(p.camera);
    save_node(p.node);
    p.hud.assign(hud_values(), hud_values() + (kHudEnd - kHudFirst));
    p.hero_id = static_cast<std::int16_t>(hero);
    std::memcpy(p.side, side_global(), sizeof p.side);
    p.map_unit_id = static_cast<std::int16_t>(hero);
    p.map_unit = jo::as_unit(unit_of(hero));
    p.map_rotation = minimap()->rotation;
    p.map_scroll[0] = minimap()->scale_x;
    p.map_scroll[1] = minimap()->scale_y;
    auto* const pl = reinterpret_cast<jo::Player*>(p.player.data());
    pl->unit_id_ = hero;
    auto* const in = reinterpret_cast<jo::Input*>(p.input.data());
    std::memset(in->key_state, 0, sizeof in->key_state);
    in->delta_x = in->delta_y = in->delta_z = 0;
    in->scan_code = in->key_code = 0;
    in->queue_head = in->queue_tail;  // no queued events
    jo::as_unit(unit_of(hero))->layer = 0;  // driven by a player, not by its AI
    take_orders_of(unit_of(hero));          // and no longer by its order (escorting the host, for instance)
    p.remote = remote;
    g_others.push_back(std::move(p));
    hook::log_line("[Coop] %s controls unit %d (the host controls %d)", what, hero, host_id);
}

// A player: an item of this computer for the host.
void keep_for_host(pin::Device device, const pin::Item& item) {
    if (g_to_host.size() >= kMaxUnsent) return;  // the host is not reading (not in a mission yet)
    g_to_host.push_back(static_cast<u8>(device));
    const auto* const b = reinterpret_cast<const u8*>(&item);
    g_to_host.insert(g_to_host.end(), b, b + sizeof item);
}

// A player, every frame of a mission: what this computer's keyboard and mouse did goes to the host.
void send_to_host() {
    static bool forwarding = false;
    if (!forwarding) {
        forwarding = true;
        g_to_host.clear();
        pin::forward(&keep_for_host);
        hook::log_line("[Coop] this computer's keyboard and mouse go to the host");
    }
    joan::replay::coop_send(g_to_host.data(), g_to_host.size());
    g_to_host.clear();
}

// The host: the items a player sent, into its queues, in the order they arrived and in the host's clock.
void take_received(OtherPlayer& p) {
    joan::replay::coop_receive(g_received);
    std::size_t at = 0;
    for (; at + kWireItem <= g_received.size(); at += kWireItem) {
        pin::Item item;
        std::memcpy(&item, g_received.data() + at + 1, sizeof item);
        item.time = static_cast<u32>(joan::platform::time_ms());
        item.sequence = ++g_sequence;
        (g_received[at] == static_cast<u8>(pin::Device::Mouse) ? p.mouse : p.keys).push_back(item);
    }
    g_received.erase(g_received.begin(), g_received.begin() + static_cast<std::ptrdiff_t>(at));
}

// Input::poll on the player's devices (its Input object in place): on the host the items it sent, in every other game
// the recorded reads.
void poll_player(OtherPlayer& p) {
    pin::read_from(&p.mouse, &p.keys);
    g_polling = true;
    joan::orig::as_input(input_object())->poll();  // 0x00594f90
    g_polling = false;
    pin::read_from(nullptr, nullptr);
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
    // What the mission's scripts set on "the player" holds for every player: blocked during a cutscene or a dialogue, and
    // the two flags next to it. They are only ever set on the host's object.
    auto* const mine = reinterpret_cast<jo::Player*>(p.player.data());
    const auto* const host = reinterpret_cast<const jo::Player*>(player);
    mine->blocked = host->blocked;
    mine->u8_79 = host->u8_79;
    mine->u8_7c = host->u8_7c;
    std::vector<u8> host_player(player, player + p.player.size()), host_input(input, input + p.input.size()), host_camera;
    save_camera(host_camera);
    u8* const hud = hud_values();
    std::vector<u8> host_hud(hud, hud + (kHudEnd - kHudFirst));
    const std::int16_t host_hero = hero_global();
    u8 host_side[sizeof p.side];
    std::memcpy(host_side, side_global(), sizeof host_side);
    std::memcpy(player, p.player.data(), p.player.size());
    std::memcpy(input, p.input.data(), p.input.size());
    load_camera(p.camera);
    std::memcpy(hud, p.hud.data(), p.hud.size());
    hero_global() = p.hero_id;
    std::memcpy(side_global(), p.side, sizeof p.side);
    jo::MiniMap* const map = minimap();
    const std::int16_t host_map_unit_id = map->unit_id, host_map_rotation = map->rotation;
    jo::Unit* const host_map_unit = map->unit;
    const float host_scroll[2] = {map->scale_x, map->scale_y};
    const std::uint32_t host_scroll_flag = map->u32_25;
    map->unit_id = p.map_unit_id;
    map->unit = p.map_unit;
    map->rotation = p.map_rotation;
    map->scale_x = p.map_scroll[0];
    map->scale_y = p.map_scroll[1];
    map->u32_25 = p.map_scroll_flag;
    g_current = &p;
    g_host_unit = reinterpret_cast<const jo::Player*>(host_player.data())->unit_id_;
    step();
    g_current = nullptr;
    std::memcpy(p.player.data(), player, p.player.size());
    std::memcpy(p.input.data(), input, p.input.size());
    save_camera(p.camera);
    std::memcpy(p.hud.data(), hud, p.hud.size());
    p.hero_id = hero_global();
    std::memcpy(p.side, side_global(), sizeof p.side);
    p.map_unit_id = map->unit_id;
    p.map_unit = map->unit;
    p.map_rotation = map->rotation;
    p.map_scroll[0] = map->scale_x;
    p.map_scroll[1] = map->scale_y;
    p.map_scroll_flag = map->u32_25;
    map->unit_id = host_map_unit_id;
    map->unit = host_map_unit;
    map->rotation = host_map_rotation;
    map->scale_x = host_scroll[0];
    map->scale_y = host_scroll[1];
    map->u32_25 = host_scroll_flag;
    std::memcpy(player, host_player.data(), host_player.size());
    std::memcpy(input, host_input.data(), host_input.size());
    load_camera(host_camera);
    std::memcpy(hud, host_hud.data(), host_hud.size());
    hero_global() = host_hero;
    std::memcpy(side_global(), host_side, sizeof host_side);
}

bool in_mission() {
    const i32 host_id = jo::self<jo::Player>(player_object())->unit_id_;
    return unit_of(host_id) != nullptr;
}

}  // namespace

bool polling_other_player() { return g_polling; }

namespace {

// Runs `step` with player `p`'s Input object in the game's place.
template <class Step>
void with_input(OtherPlayer& p, Step step) {
    auto* const input = static_cast<u8*>(input_object());
    std::vector<u8> host_input(input, input + p.input.size());
    std::memcpy(input, p.input.data(), p.input.size());
    step();
    std::memcpy(p.input.data(), input, p.input.size());
    std::memcpy(input, host_input.data(), host_input.size());
}

template <class Step>
void for_remote_players(Step step) {
    for (OtherPlayer& p : g_others)
        if (p.remote && unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) != nullptr) step(p);
}

}  // namespace

void draw_world_views(void* camera, void* world, void (*draw)(void* camera)) {
    g_camera = camera;
    std::vector<OtherPlayer*> views;
    for (OtherPlayer& p : g_others)
        if (p.remote && unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) != nullptr) views.push_back(&p);
    c_view_frames.hit();
    if (views.empty()) {
        draw(camera);
        return;
    }
    // The host's view, then each other player's in their order: the same in every game. A game shows only its own
    // player's view (a player's game: the one other player there is so far).
    const bool player_game = joan::replay::coop_player();
    const auto hide = [](bool on) {
        g_hidden = on;
        joan::gld::set_hidden_view(on);
    };
    hide(player_game);
    draw(camera);
    jo::Wnd* const wnd = game_window();
    for (OtherPlayer* p : views) {
        // The frame's time has passed in the first view: the others draw the same moment (grass, camera zones and the
        // like advance by the frame step while drawing).
        const u32 frame_ms = wnd->frame_ms;
        const i32 game_dt = wnd->game_dt;
        wnd->frame_ms = 0;
        wnd->game_dt = 0;
        hide(!player_game);
        c_extra_view.hit();
        as_player(*p, [p, camera, world, draw] {
            std::vector<u8> host_node;
            save_node(host_node);
            load_node(p->node);
            std::unordered_map<const void*, jo::CamZones> host_zones;
            zones_in(*p, world, host_zones);
            // The hero camera follows the unit at CameraFollow +8 (World::draw passes it to CameraFollow::start
            // 0x00563a20; hero selection sets it, 0x005639f0): this player's hero, whichever it is now.
            static_cast<jo::CameraFollow*>(camera)->i32_8 = jo::self<jo::Player>(player_object())->unit_id_;
            draw(camera);
            zones_out(*p, world, host_zones);
            save_node(p->node);
            load_node(host_node);
        });
        wnd->frame_ms = frame_ms;
        wnd->game_dt = game_dt;
    }
    hide(false);
}

bool drawing_hidden_view() { return g_hidden; }

namespace {

// The dynamic grass (DynGrass 0x00a45d30): patches around one camera, rebuilt each frame by DynGrass::update
// (0x00464740, in the game tick). Every player has its own (its patch arrays, counts, resources and wind), so that
// the grass is around every hero; all patches are in the scene, so every view has them.
void* grass_object() { return obj(d::g_dyn_grass); }
bool g_grass_busy = false;  // a player's grass is in the game's place (no passing on to the players again)

template <class Step>
void with_grass(OtherPlayer& p, Step step) {
    auto* const g = static_cast<u8*>(grass_object());
    std::vector<u8> host(g, g + sizeof(jo::DynGrass));
    std::memcpy(g, p.grass.data(), sizeof(jo::DynGrass));
    g_grass_busy = true;
    step();
    g_grass_busy = false;
    std::memcpy(p.grass.data(), g, sizeof(jo::DynGrass));
    std::memcpy(g, host.data(), sizeof(jo::DynGrass));
}

// A player's grass, as the host's is now but without patches or resources; its arrays from the game's memory as
// DynGrass::reserve (0x004644d0) makes them.
void make_grass(OtherPlayer& p) {
    const auto* const host = static_cast<const jo::DynGrass*>(grass_object());
    p.grass.assign(reinterpret_cast<const u8*>(host), reinterpret_cast<const u8*>(host) + sizeof(jo::DynGrass));
    auto* const own = reinterpret_cast<jo::DynGrass*>(p.grass.data());
    own->patches = nullptr;
    own->patches2 = nullptr;
    own->patch_count = own->patch2_count = own->res_count = 0;
    own->density = 0.0f;  // reserve allocates when the density grows
    const auto density = static_cast<i32>(host->density);
    with_grass(p, [density] { joan::orig::as_grass(grass_object())->reserve(density); });  // 0x004644d0
}

}  // namespace

void other_players_grass(void (*update)(void* grass)) {
    if (g_camera == nullptr || g_grass_busy) return;
    for (OtherPlayer& p : g_others) {
        if (!p.remote || unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) == nullptr) continue;
        if (p.grass.empty()) make_grass(p);
        std::vector<u8> host_node;
        save_node(host_node);
        load_node(p.node);  // DynGrass::update places the grass around the camera's pose
        with_grass(p, [update] { update(grass_object()); });  // DynGrass::update 0x00464740
        load_node(host_node);
    }
}

void players_grass_enabled(char on) {
    if (g_grass_busy) return;
    for (OtherPlayer& p : g_others)
        if (!p.grass.empty()) with_grass(p, [on] { joan::orig::as_grass(grass_object())->set_enabled(on); });  // 0x00464600
}

void release_players_grass() {
    if (g_grass_busy) return;
    for (OtherPlayer& p : g_others) {
        if (p.grass.empty()) continue;
        with_grass(p, [] { joan::orig::as_grass(grass_object())->release(); });  // 0x00464390
        p.grass.clear();
    }
}

bool is_player_unit(std::int32_t unit_id) {
    if (unit_id == 0) return false;
    if (unit_id == jo::self<jo::Player>(player_object())->unit_id_) return true;  // the player in the game's places
    return taken_by_other_player(unit_id);
}

bool taken_by_other_player(std::int32_t unit_id) {
    if (unit_id == 0) return false;
    if (g_current != nullptr && unit_id == g_host_unit) return true;  // the host's, while another player is in its places
    for (const OtherPlayer& p : g_others)
        if (&p != g_current && reinterpret_cast<const jo::Player*>(p.player.data())->unit_id_ == unit_id) return true;
    return false;
}

void level_ends() {
    if (!g_others.empty()) hook::log_line("[Coop] the level ends: the other players start again on the next one");
    g_others.clear();
    g_tried = false;
    g_received.clear();
}

void other_player_hero_died(std::int32_t unit_id) {
    for (OtherPlayer& p : g_others)
        if (p.remote && reinterpret_cast<const jo::Player*>(p.player.data())->unit_id_ == unit_id)
            as_player(p, [] { joan::orig::as_player(player_object())->select_hero(0); });  // 0x00432300 the next hero
}

void other_players_hud_tick(void (*track)(void* hud)) {
    for (OtherPlayer& p : g_others)
        if (p.remote && unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) != nullptr)
            as_player(p, [track] { track(hud_object()); });
}

void draw_huds(void (*draw)(void* context), void* context) {
    std::vector<OtherPlayer*> views;
    for (OtherPlayer& p : g_others)
        if (p.remote && unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) != nullptr) views.push_back(&p);
    if (views.empty() || g_camera == nullptr) {
        draw(context);
        return;
    }
    // As draw_world_views: the host's HUD, then each other player's with its Player (and camera) in place and the frame
    // step at zero, the same in every game; a game shows its own player's.
    const bool player_game = joan::replay::coop_player();
    const auto hide = [](bool on) {
        g_hidden = on;
        joan::gld::set_hidden_view(on);
    };
    hide(player_game);
    draw(context);
    jo::Wnd* const wnd = game_window();
    for (OtherPlayer* p : views) {
        const u32 frame_ms = wnd->frame_ms;
        const i32 game_dt = wnd->game_dt;
        wnd->frame_ms = 0;
        wnd->game_dt = 0;
        hide(!player_game);
        as_player(*p, [draw, context] { draw(context); });
        wnd->frame_ms = frame_ms;
        wnd->game_dt = game_dt;
    }
    hide(false);
}

void other_players_poll() {
    for_remote_players([](OtherPlayer& p) {
        take_received(p);
        with_input(p, [&p] { poll_player(p); });
    });
}

void other_players_clear_input() {
    for_remote_players([](OtherPlayer& p) {
        with_input(p, [] {
            joan::orig::as_cursor(input_object())->clear_deltas();  // 0x004d8560
            auto* const in = static_cast<jo::Input*>(input_object());
            in->prev_skey_state = in->skey_state;
        });
    });
}

void other_players_end_input() {
    for_remote_players([](OtherPlayer& p) {
        auto* const in = reinterpret_cast<jo::Input*>(p.input.data());
        in->u32_38[0] = static_cast<u32>(in->delta_x);  // the frame's motion, kept as the previous one
        in->u32_38[1] = static_cast<u32>(in->delta_y);
        in->u32_38[2] = static_cast<u32>(in->delta_z);
        in->delta_x = in->delta_y = in->delta_z = 0;
    });
}

void other_players_input(void* camera) {
    g_camera = camera;
    const bool network = joan::replay::coop_remote_players() > 0;
    if (!network && !test_player_wanted()) return;
    if (!in_mission()) return;
    if (network && joan::replay::coop_player()) send_to_host();
    if (!g_tried) {
        start_player(network);
        // Its HUD starts from its hero (what Player::set_unit 0x0042f9c0 does for "the player").
        if (!g_others.empty())
            as_player(g_others.back(), [] {
                joan::orig::as_hud(hud_object())->update_health_state(joan::orig::arg::i32(jo::self<jo::Player>(player_object())->unit_id_));  // 0x00703750
            });
    }
    for (OtherPlayer& p : g_others) {
        if (unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) == nullptr) continue;
        if (p.remote) {
            as_player(p, [] { joan::orig::as_player(player_object())->on_key_42e1a0(); });  // 0x0042e1a0
        } else {
            script(p);
            as_player(p, [] { joan::orig::as_player(player_object())->on_key_42e1a0(); });  // 0x0042e1a0
        }
    }
}

void other_players_tick(void* camera) {
    g_camera = camera;
    for (OtherPlayer& p : g_others) {
        if (unit_of(reinterpret_cast<jo::Player*>(p.player.data())->unit_id_) == nullptr) continue;
        as_player(p, [] { joan::orig::as_player(player_object())->tick(); });  // 0x0042f700
    }
}

}  // namespace joan::coop
