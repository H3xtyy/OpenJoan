// game/unknown_unit_core.cpp: functions whose original source file is unknown (formerly game/_unplaced/unit_core.cpp;
// re/attribution.csv: between ww/unit.cpp and ww/unitres.cpp). They move to their original file when evidence names it
// (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../en3d/scene_lists.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::cdecl_call;
using joan::call::import_call;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::hook::x87::ftol;

constexpr img::Addr kGameUnits(d::g_units);

using joan::img::str;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using i8 = std::int8_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;

std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

void* const kObjects = obj(d::g_items);

void* const kBlood = obj(d::g_a5ccd0);

void* const kItemTypes = obj(d::g_item_res);
void* const kTracks = obj(d::g_water_array);

using joan::orig::helpers::add32;
std::int32_t time_now() { return global<std::int32_t>(d::g_now); }

std::int16_t busy(void* unit) { return joan::orig::arg::as<std::int16_t>(joan::orig::as_unit(unit)->is_kind_special()); }  // 0x00508ab0

void* const kAnims = obj(d::g_model_refs);

void* const kSoldiers = obj(d::g_units);

}  // namespace

// ---------------------------------------- from modules/game_b87_01.cpp (game/b87_01)
namespace {
namespace game_b87_01 {


struct H {
    // ---- 0x004fed00 Unit::set_group(v) ret 4 ------------------------------------------------
    void set_group(jo::World* v) {  // the facade passes the World pointer: as i32 it was truncated above 4 GB (live x64 heap)
        if (jo::self<jo::Unit>(this)->world == v) return;
        if (jo::self<jo::Unit>(this)->cell != nullptr) joan::orig::as_unit(jo::self<jo::Unit>(this)->cell)->unlink_owner(joan::orig::arg::mptr(this));  // 0x0043eb10
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene) != nullptr) {
            joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene))->detach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor));  // 0x005efdb0
        }
        jo::self<jo::Unit>(this)->world = v;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_01", name, addr, fn)
B("Unit::set_group", 0x004fed00, &H::set_group);
#undef B

}  // namespace game_b87_01
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    void* unit_position(void* out) { return joan::orig::as_unit(this)->position(out); }  // 0x004fedb0
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Unit::position", 0x004fedb0, &H::unit_position);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_objects1.cpp (game/objects1)
namespace {
namespace game_objects1 {

struct V3 {
    float x, y, z;
};

std::uint32_t bits(float v) {
    std::uint32_t u;
    std::memcpy(&u, &v, 4);
    return u;
}

constexpr img::Addr kUnits(d::g_items);
constexpr img::Addr kTypes(d::g_types);
constexpr img::Addr kPlayers(d::g_physic_sys);

std::int32_t units_empty_at(std::int32_t id) { return joan::orig::as_units(img::obj(kUnits))->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
std::uint8_t* units_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnits))->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
void scene_remove(void* scene, void* node) { joan::orig::as_scene(scene)->detach(joan::orig::arg::mptr(node)); }                       // 0x005efdb0
void scene_add(void* scene, void* node) { joan::orig::as_scene(scene)->attach(joan::orig::arg::mptr(node)); }                          // 0x005efc50
void trail_set_scene(void* trail, void* scene) { joan::orig::as_trail(trail)->set_scene(joan::orig::arg::mptr(scene)); }                  // 0x006891d0

JOAN_COUNTER(c_update, "GroundObject::update_position");
JOAN_COUNTER(c_set_position, "GroundObject::set_position");

struct H {
    std::uint8_t* type() { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kTypes))->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id))); }  // 0x0042d0a0

    // 0x004ff830 reads the dword at +0: id with side in the upper half (both i16 members at +0 / +2 on x86 and x64)
    std::int32_t id() {
        static_assert(offsetof(jo::Unit, side) == offsetof(jo::Unit, id) + sizeof(std::int16_t), "id, side adjacent");
        std::int32_t v;
        std::memcpy(&v, &jo::self<jo::Unit>(this)->id, sizeof v);
        return v;
    }
    float heading() { return jo::self<jo::Unit>(this)->heading; }        // 0x004ff850

    // 0x004fedf0 GroundObject::set_position(pos): through the player list when the object has
    // an actor, else +0xe and update_position.
    void set_position(V3 pos) {
        c_set_position.hit();
        if (jo::self<jo::Unit>(this)->actor != nullptr) {
            joan::orig::as_players(img::obj(kPlayers))->place(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), &pos);  // 0x006aee40
            return;
        }
        std::memcpy(jo::self<jo::Unit>(this)->position_, &pos, 12);
        joan::orig::as_ground_object(this)->update_position();  // 0x004fee60
    }

    // Detaches / attaches the effect nodes of one attached unit (+0x8e and +0x86).
    void remove_unit_nodes(std::int16_t unit_id) {
        if (units_empty_at(unit_id) != 0) return;
        std::uint8_t* const u = units_get(unit_id);
        if (jo::view<void*>(jo::as_map_object(u)->actor_b) != nullptr) scene_remove(jo::self<jo::Unit>(this)->scene, jo::view<void*>(jo::as_map_object(u)->actor_b));
        if (jo::view<void*>(jo::as_map_object(u)->actor) != nullptr) scene_remove(jo::self<jo::Unit>(this)->scene, jo::view<void*>(jo::as_map_object(u)->actor));
    }
    void add_unit_nodes(std::int16_t unit_id) {
        if (units_empty_at(unit_id) != 0) return;
        std::uint8_t* const u = units_get(unit_id);
        if (jo::view<void*>(jo::as_map_object(u)->actor) == nullptr) return;
        scene_add(jo::self<jo::Unit>(this)->scene, jo::view<void*>(jo::as_map_object(u)->actor));
        if (jo::view<void*>(jo::as_map_object(u)->actor_b) != nullptr) scene_add(jo::self<jo::Unit>(this)->scene, jo::view<void*>(jo::as_map_object(u)->actor_b));
    }

    // 0x004fee60 GroundObject::update_position.
    void update_position() {
        c_update.hit();
        // 0x004fee72: fld [+0x16] / fcomp 10000.0 with the flags never read (dead).
        void* cell = jo::self<jo::Unit>(this)->cell;
        void* scene = jo::self<jo::Unit>(this)->scene;
        if (jo::self<jo::Unit>(this)->world != nullptr) joan::orig::as_map(jo::self<jo::Unit>(this)->world)->find_cell(&cell, &scene, joan::orig::arg::mptr(jo::self<jo::Unit>(this)->position_));  // 0x0054a2c0
        if (scene != jo::self<jo::Unit>(this)->scene && jo::self<jo::Unit>(this)->scene != nullptr) {
            if (jo::self<jo::Unit>(this)->trail_b != nullptr) trail_set_scene(jo::self<jo::Unit>(this)->trail_b, nullptr);
            remove_unit_nodes(jo::self<jo::Unit>(this)->carried_b);
            if (jo::self<jo::Unit>(this)->trail_a != nullptr) trail_set_scene(jo::self<jo::Unit>(this)->trail_a, nullptr);
            remove_unit_nodes(jo::self<jo::Unit>(this)->carried_a);
            if (jo::self<jo::Unit>(this)->weapon_model != nullptr) scene_remove(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->weapon_model);
            if (jo::self<jo::Unit>(this)->model != nullptr) scene_remove(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->model);
            if (jo::self<jo::Unit>(this)->actor != nullptr) scene_remove(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->actor);
            jo::self<jo::Unit>(this)->scene = nullptr;
        }
        if (cell != jo::self<jo::Unit>(this)->cell) {
            if (jo::self<jo::Unit>(this)->cell != nullptr && jo::view<void*>(jo::as_cell(jo::self<jo::Unit>(this)->cell)->camera_zones) != nullptr &&
                joan::orig::arg::as<std::int32_t>(joan::orig::as_ground_object(jo::view<void*>(jo::as_cell(jo::self<jo::Unit>(this)->cell)->camera_zones))->id()) == static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id) &&  // 0x004ff830
                cell != nullptr && jo::view<void*>(jo::as_cell(cell)->camera_zones) != nullptr) {
                joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(cell)->camera_zones))->set_unit(joan::orig::arg::u32(joan::orig::arg::as<std::int32_t>(joan::orig::as_ground_object(jo::view<void*>(jo::as_cell(jo::self<jo::Unit>(this)->cell)->camera_zones))->id())));  // 0x00428b00
            }
            if (cell != nullptr) {
                joan::orig::as_cell(cell)->attach_object(joan::orig::arg::mptr(this));  // 0x00437770
                if (units_empty_at(jo::self<jo::Unit>(this)->carried_a) == 0) joan::orig::as_cell(cell)->attach_unit(joan::orig::arg::mptr(units_get(jo::self<jo::Unit>(this)->carried_a)));  // 0x0043b190
                if (units_empty_at(jo::self<jo::Unit>(this)->carried_b) == 0) joan::orig::as_cell(cell)->attach_unit(joan::orig::arg::mptr(units_get(jo::self<jo::Unit>(this)->carried_b)));
            }
        }
        if (jo::self<jo::Unit>(this)->cell != nullptr && static_cast<std::int8_t>(jo::as_cell(jo::self<jo::Unit>(this)->cell)->loaded) != 0 &&
            static_cast<std::int8_t>(jo::self<jo::Unit>(this)->in_scene) == 0) {
            joan::orig::as_cell_object(this)->attach();  // 0x00442820
        } else if (jo::self<jo::Unit>(this)->cell != nullptr && static_cast<std::int8_t>(jo::as_cell(jo::self<jo::Unit>(this)->cell)->loaded) == 0 &&
                   static_cast<std::int8_t>(jo::self<jo::Unit>(this)->in_scene) != 0) {
            joan::orig::as_unit(this)->detach_scene();  // 0x0041fcc0
        } else if (scene != jo::self<jo::Unit>(this)->scene) {
            jo::self<jo::Unit>(this)->scene = static_cast<jo::Scene*>(scene);
            if (jo::self<jo::Unit>(this)->actor != nullptr) scene_add(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->actor);
            if (jo::self<jo::Unit>(this)->model != nullptr) scene_add(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->model);
            if (jo::self<jo::Unit>(this)->weapon_model != nullptr) scene_add(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->weapon_model);
            add_unit_nodes(jo::self<jo::Unit>(this)->carried_a);
            if (jo::self<jo::Unit>(this)->trail_a != nullptr) trail_set_scene(jo::self<jo::Unit>(this)->trail_a, jo::self<jo::Unit>(this)->scene);
            add_unit_nodes(jo::self<jo::Unit>(this)->carried_b);
            if (jo::self<jo::Unit>(this)->trail_b != nullptr) trail_set_scene(jo::self<jo::Unit>(this)->trail_b, jo::self<jo::Unit>(this)->scene);
        }

        // Child units of the type record: offsets +0x1b1 (V3 each), headings +0x1b5 (degrees).
        const std::int32_t n = jo::self<jo::UnitType>(type())->slot_count;
        if (n <= 0) return;
        const joan::hook::x87::Fpu fpu{true};
        for (std::int32_t i = 0; i < n; ++i) {
            if (!(jo::view<std::int32_t*>(jo::self<jo::Unit>(this)->mask_slots)[i] > 0)) continue;
            if (joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(jo::view<std::int32_t*>(jo::self<jo::Unit>(this)->mask_slots)[i])) != 0) continue;  // 0x00428b70
            V3 offset;
            std::memcpy(&offset, jo::view<std::uint8_t*>(jo::as_unit_type(type())->buffers[0]) + i * 0xc, 12);
            if (jo::self<jo::Unit>(this)->actor == nullptr) continue;
            V3 world;
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone))->matrix());  // 0x0060ef80
            import_call<void*>(d::iat_v3d_87_point_transform_3, &world, &offset, m);                               // point_transform
            std::uint8_t* const child = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(jo::view<std::int32_t*>(jo::self<jo::Unit>(this)->mask_slots)[i])));  // 0x0041b3c0
            const float h1 = joan::orig::arg::as<float>(joan::orig::as_ground_object(this)->heading());  // 0x004ff850
            float a1 = jo::view<float*>(jo::as_unit_type(type())->buffers[1])[i] * global<float>(d::flt_0_017453292);
            a1 = a1 + h1;
            joan::orig::as_unit(child)->set_target(joan::orig::arg::f32(a1));  // 0x004ff870
            const float h2 = joan::orig::arg::as<float>(joan::orig::as_ground_object(this)->heading());
            float a2 = jo::view<float*>(jo::as_unit_type(type())->buffers[1])[i] * global<float>(d::flt_0_017453292);
            a2 = a2 + h2;
            jo::as_unit(child)->heading_goal = a2;
            void* cell_id = nullptr;
            std::int32_t block = 0, layer = 0;
            if (jo::as_unit(child)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::as_unit(child)->actor)->record) != 0) {
                std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(img::obj(kPlayers))->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(child)->actor)->record))));  // 0x006af310
                cell_id = jo::as_shot(p)->state.sub;
                block = jo::view<std::int32_t>(jo::as_shot(p)->state.zone);
                layer = joan::hook::x87::ftol(static_cast<double>(jo::view<float>(jo::as_shot(p)->state.cell)));
            }
            const std::int16_t found = joan::orig::arg::as<std::int16_t>(joan::orig::as_players(img::obj(kPlayers))->find_slot(&cell_id, &block, joan::orig::arg::i16(0), joan::orig::arg::f32(bits(world.x)), joan::orig::arg::f32(bits(world.y)), joan::orig::arg::f32(bits(world.z)), 0, joan::orig::arg::f32(static_cast<float>(layer)), 0));  // 0x006ae8e0
            if (static_cast<std::int32_t>(found) != 0) {
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::placements::find(joan::orig::arg::mptr(img::obj(d::g_placements)), joan::orig::arg::i32(global<std::int32_t>(d::g_a57bb2)), joan::orig::as_path_space(cell_id)));  // 0x004a2b20
                const float ly = static_cast<float>(fpu.sub(world.y, static_cast<double>(jo::as_rworld_cell(e)->y)));
                const float lx = static_cast<float>(fpu.sub(world.x, static_cast<double>(jo::as_rworld_cell(e)->x)));
                const double h = joan::orig::arg::as<double>(joan::orig::as_terrain(cell_id)->height_66cae0(joan::orig::arg::i32(block), joan::orig::arg::f32(lx), joan::orig::arg::f32(ly)));  // 0x0066cae0
                world.z = static_cast<float>(fpu.add(h, offset.z));
            }
            joan::orig::as_ground_object(child)->set_position(world);  // 0x004fedf0
        }
    }
};

#define OB1(name, addr, fn) JOAN_REPLACE("game/objects1", name, addr, fn)
OB1("GroundObject::update_position", 0x004fee60, &H::update_position);
OB1("GroundObject::set_position", 0x004fedf0, &H::set_position);
OB1("GroundObject::id", 0x004ff830, &H::id);
OB1("GroundObject::heading", 0x004ff850, &H::heading);
#undef OB1

}  // namespace game_objects1
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
namespace {
namespace game_units5 {


constexpr img::Addr kPlayers(d::g_physic_sys);

JOAN_COUNTER(c_set_target, "Unit::set_target");

struct H {
    // 0x004ff870 (ret 4)
    std::int32_t set_target(float target) {  // a float argument (XMM on x64; the facade declares it float): heading bits
        c_set_target.hit();
        if (jo::self<jo::Unit>(this)->actor == nullptr) {
            jo::self<jo::Unit>(this)->heading = target;
            return 1;
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->mounted()) != 0 &&                                    // 0x00430190
            joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->mount_id)) == 0 &&  // 0x00428b70
            jo::as_unit(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->mount_id)))->actor != nullptr &&  // 0x0041b3c0
            static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) != 0x4f && static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) != 0x50 &&
            static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) != 0x51 && static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) != 0x52) {
            void* const owner = joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->mount_id));  // 0x0041b3c0
            if (static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_players(img::obj(kPlayers))->move_actor(joan::orig::arg::mptr(jo::as_unit(owner)->actor), joan::orig::arg::u32(joan::orig::arg::bits(target)), 0u, 0u))) == 0)  // 0x006af190
                return 0;
        }
        if (static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_players(img::obj(kPlayers))->move_actor(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), joan::orig::arg::u32(joan::orig::arg::bits(target)), 0u, 0u))) != 0) {  // 0x006af190
            jo::self<jo::Unit>(this)->heading = target;
            return 1;
        }
        return 0;
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("Unit::set_target", 0x004ff870, &H::set_target);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_b82_08.cpp (game/b82_08)
namespace {
namespace game_b82_08 {


constexpr img::Addr kTypes(d::g_types);
constexpr img::Addr kDefs(d::g_model_res_mgr);
constexpr img::Addr kMeshes(d::g_anim_res_mgr);
constexpr img::Addr kUnits(d::g_items);
constexpr img::Addr kRandom(d::g_misc);
constexpr img::Addr kAnimSounds(d::g_player_hud);
constexpr img::Addr kSoundRecords(d::g_model_refs);
constexpr img::Addr kSounds(d::g_physic_sys);
constexpr img::Addr kAnim(d::s_ANIM);  // "ANIM\"

JOAN_COUNTER(c_set_anim, "CellObject::set_anim_state");
JOAN_COUNTER(c_find_anim, "CellObject::find_anim_state");
JOAN_COUNTER(c_attach_unit, "CellObject::attach_unit");
JOAN_COUNTER(c_alt_mesh, "CellObject::create_alt_mesh");

std::uint8_t* type_of(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kTypes))->get(joan::orig::arg::i16(id))); }  // 0x0042d0a0
std::uint8_t* def_of(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(img::obj(kDefs))->entry(joan::orig::arg::u32(id))); }     // 0x0041a180
// 0x004168d0 AnimSet::entry (game/animgrp.cpp), inline: groups + (state - 1) * 0x99 in 32-bit unsigned arithmetic.
std::uint8_t* anim_of(void* table, std::int32_t state) {
    return jo::view<std::uint8_t*>(jo::as_anim_set(table)->groups) + (static_cast<std::uint32_t>(state) * 0x99u - 0x99u);
}
std::int32_t units_missing(std::int32_t id) { return joan::orig::as_units(img::obj(kUnits))->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
std::uint8_t* units_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnits))->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
// new(0x10c) + 0x005aab20 (mesh object).
void* new_mesh_object() {
    void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // operator new (0x10c on x86)
    return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
}
// new(0xaf) + 0x00687130 (unit holder).
void* new_holder() {
    void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Trail)));  // operator new (0xaf on x86)
    return mem != nullptr ? joan::orig::as_trail(mem)->construct() : nullptr;  // 0x00687130
}

struct H {

    // ---- 0x004ff9f0 helpers -------------------------------------------------------------
    // Duration argument: fild dword [anim(state) + 0x20], fstp float (lookup done first).
    float anim_duration(std::int32_t state) { return static_cast<float>(jo::as_anim_group_rec(anim_of(jo::self<jo::Unit>(this)->anim_set, state))->duration); }
    // Start on the main mesh object +0x2f5.
    void start_main(std::int32_t state, void* mesh, std::uint8_t* anim, std::int32_t delay, std::int32_t loop,
                    std::int32_t arg) {
        const float duration = anim_duration(state);
        joan::orig::as_actor(jo::self<jo::Unit>(this)->actor)->start_anim(mesh, reinterpret_cast<const char*>(anim + 0x4d), joan::orig::arg::u32(global<std::int32_t>(d::g_now) + delay), joan::orig::arg::u32(loop), joan::orig::arg::u32(arg), joan::orig::arg::bits(duration));  // 0x005abe30
    }
    // Start on an attached unit's mesh object (unit + 0x86).
    void start_unit(std::uint8_t* unit, std::int32_t state, std::uint8_t* anim, std::int32_t delay, std::int32_t loop,
                    std::int32_t arg) {
        const float duration = anim_duration(state);
        void* const mesh_object = jo::view<void*>(jo::as_map_object(unit)->actor);
        joan::orig::as_actor(mesh_object)->start_anim(jo::as_actor(mesh_object)->anim, reinterpret_cast<const char*>(anim + 0x4d), joan::orig::arg::u32(global<std::int32_t>(d::g_now) + delay), joan::orig::arg::u32(loop), joan::orig::arg::u32(arg), joan::orig::arg::bits(duration));  // 0x005abe30
    }
    void notify_local(std::uint8_t* anim, std::int32_t flag) {
        if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id) == global<std::int32_t>(d::g_player_unit_recno))
            joan::orig::as_hud(img::obj(kAnimSounds))->nop_6fe500(anim + 0x4d, joan::orig::arg::u32(flag));  // 0x006fe500
    }
    // The attached unit of a slot when its mesh has the animation, else null.
    std::uint8_t* unit_with_anim(std::int16_t jo::Unit::*slot, std::uint8_t* anim) {
        std::uint8_t* unit = nullptr;
        if (units_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->*slot)) == 0) {
            unit = units_get(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->*slot));
            if (jo::view<void*>(jo::as_map_object(unit)->actor) == nullptr || jo::view<void*>(jo::as_map_object(unit)->actor->anim) == nullptr ||
                joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_def(jo::view<void*>(jo::as_map_object(unit)->actor->anim))->find_action(joan::orig::arg::str(static_cast<void*>(anim + 0x4d)))) == 0)  // 0x005b5be0
                unit = nullptr;
        }
        return unit;
    }

    // 0x004ff9f0.
    void set_anim_state(std::int32_t state, std::int32_t /*time, unused*/, std::int32_t arg) {
        c_set_anim.hit();
        if (jo::self<jo::Unit>(this)->actor == nullptr) return;
        if (jo::self<jo::Unit>(this)->anim_entry == state) {
            const std::int32_t kind = jo::self<jo::Unit>(this)->anim_kind;
            if (kind != 0x23 && kind != 0x24 && kind != 0x25 && kind != 0x26 && kind != 0x27 && kind != 0x28 && kind != 0x4a &&
                kind != 0x4b && kind != 0x4c && kind != 0x4d) {
                const double position = joan::orig::arg::as<double>(joan::orig::as_unit(this)->anim_phase());  // 0x005045f0
                if (!(position == 0.0 || position != position)) return;   // fcomp 0.0f, test ah,0x40 jnz
            }
        }
        jo::self<jo::Unit>(this)->anim_entry = state;
        std::uint8_t* const anim = anim_of(jo::self<jo::Unit>(this)->anim_set, jo::self<jo::Unit>(this)->anim_entry);                         // ebp-0x8
        void* const mesh = *joan::orig::arg::as<void**>(joan::orig::as_anim_res_mgr(img::obj(kMeshes))->slot_4174f0(joan::orig::arg::u32(jo::as_anim_group_rec(anim)->anim_id)));  // 0x004174f0, ebp-0xc
        std::uint8_t* const unit0 = unit_with_anim(&jo::Unit::carried_a, anim);                                             // ebp-0x10
        std::uint8_t* const unit1 = unit_with_anim(&jo::Unit::carried_b, anim);                                             // ebp-0x14
        jo::self<jo::Unit>(this)->anim_start_ms = global<std::int32_t>(d::g_now);

        std::int32_t main_loop;   // main mesh: 4th argument of 0x005abe30 and the 0x006fe500 flag
        std::int32_t unit_loop;   // units: 4th argument
        std::int32_t unit_arg;    // units: 5th argument
        std::int32_t delay = 0;   // ebp-0x18
        switch (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind)) {
            case 1:
            case 4:
            case 7: {  // 0x00500098
                delay = static_cast<std::int32_t>(static_cast<std::uint32_t>(joan::orig::as_misc(img::obj(kRandom))->random(5)) * 0xc8u);  // 0x00590540
                if (global<std::int32_t>(d::g_player_unit_recno) == static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)) delay = 0;
                jo::self<jo::Unit>(this)->anim_start_ms = jo::self<jo::Unit>(this)->anim_start_ms + delay;
                jo::self<jo::Unit>(this)->anim_loop = 1;
                const std::int32_t sub = jo::self<jo::Unit>(this)->prev_action;
                if (sub == 0x3a || sub == 0x4f || sub == 0x50 || sub == 0x51 || sub == 0x52 || sub == 0x65) {
                    if (arg == -1) arg = 0;
                } else {
                    if (arg == -1) arg = 1;
                }
                main_loop = 1;
                unit_loop = 1;
                unit_arg = 1;
                break;
            }
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 16:  // 0x004ffc3c
                if (arg == -1) arg = 0;
                jo::self<jo::Unit>(this)->anim_loop = 1;
                main_loop = 1;
                unit_loop = 1;
                unit_arg = 0;
                break;
            case 67:
            case 68:
            case 69:
            case 70:
            case 89:
            case 90:
            case 104:
            case 105:
            case 106: {  // 0x00500421
                jo::self<jo::Unit>(this)->anim_loop = 1;
                const std::int32_t sub = jo::self<jo::Unit>(this)->prev_action;
                if (sub == 0x4f || sub == 0x50 || sub == 0x51 || sub == 0x52) {
                    if (arg == -1) arg = 0;
                } else {
                    if (arg == -1) arg = 1;
                }
                main_loop = 1;
                unit_loop = 1;
                unit_arg = 1;
                break;
            }
            case 79:
            case 80:  // 0x004fff81
            case 81:
            case 82:
            case 96:  // 0x004ffe6a (same code)
                if (arg == -1) arg = 0;
                jo::self<jo::Unit>(this)->anim_loop = 0;
                main_loop = 0;
                unit_loop = 0;
                unit_arg = 0;
                break;
            case 84:
            case 85:
            case 86:
            case 87:  // 0x004ffd53
                if (arg == -1) arg = 1;
                jo::self<jo::Unit>(this)->anim_loop = 1;
                main_loop = 1;
                unit_loop = 1;
                unit_arg = 0;
                break;
            default:  // 0x00500652
                if (arg == -1) arg = 1;
                jo::self<jo::Unit>(this)->anim_loop = 0;
                main_loop = 0;
                unit_loop = 0;
                unit_arg = 1;
                break;
        }
        start_main(state, mesh, anim, delay, main_loop, arg);
        notify_local(anim, main_loop);
        if (unit0 != nullptr) start_unit(unit0, state, anim, delay, unit_loop, unit_arg);
        if (unit1 != nullptr) start_unit(unit1, state, anim, delay, unit_loop, unit_arg);
    }

    // ---- 0x00504010 ------------------------------------------------------------------------
    void copy_anim_params(std::uint8_t* entry) {
        jo::view<std::int32_t>(jo::self<jo::Unit>(this)->combo_min_time) = jo::as_anim_group_rec(entry)->combo_min;
        jo::view<std::int32_t>(jo::self<jo::Unit>(this)->combo_max_time) = jo::as_anim_group_rec(entry)->combo_max;
    }
    // Pattern test: the first `count` bytes of the entry's name_8 equal armour_flags[pattern..], the rest
    // of the six bytes zero (the second loop continues from where the first one stopped). Indexed through
    // plain pointers: `count` is not bounded by the arrays (the original reads on past them).
    static std::int32_t pattern_ok(std::uint8_t* entry, H* self, std::int8_t jo::Unit::*count, std::int32_t pattern, bool clear) {
        std::int32_t ok = 1;
        std::int32_t j = 0;
        for (; j < static_cast<std::int32_t>(jo::self<jo::Unit>(self)->*count); ++j) {
            if (static_cast<std::int32_t>(static_cast<const std::int8_t*>(static_cast<const void*>(jo::as_anim_group_rec(entry)->name_8))[j]) != static_cast<std::int32_t>(static_cast<const std::int8_t*>(static_cast<const void*>(jo::self<jo::Unit>(self)->armour_flags))[pattern + j])) {
                ok = 0;
                if (clear) jo::self<jo::Unit>(self)->*count = 0;
                break;
            }
        }
        for (; j < 6; ++j) {
            if (static_cast<const std::int8_t*>(static_cast<const void*>(jo::as_anim_group_rec(entry)->name_8))[j] != 0) {
                ok = 0;
                break;
            }
        }
        return ok;
    }

    // 0x00504010.
    std::int32_t find_anim_state() {
        c_find_anim.hit();
        void* const table = jo::self<jo::Unit>(this)->anim_set;  // ebp-0x8
        if (jo::self<jo::Unit>(this)->anim_set == nullptr) return 0;
        std::int32_t direction = jo::self<jo::Unit>(this)->anim_special;  // ebp-0xc
        if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_special) == 4 && static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) != 0xa &&
            static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) != 0xb)
            direction = 0;
        std::int32_t fallback = 0;  // ebp-0x10
        for (std::int32_t i = jo::as_anim_set(table)->group_count; i > 0; --i) {
            std::uint8_t* const entry = anim_of(table, i);  // ebp-0x1c
            if (jo::view<std::int32_t>(jo::as_anim_group_rec(entry)->is_jump) != static_cast<std::int32_t>(jo::self<jo::Unit>(this)->action)) continue;
            if (jo::as_anim_group_rec(entry)->kind != static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind)) continue;
            if (jo::as_anim_group_rec(entry)->weapon_kind != static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_item_using)) continue;
            if (jo::as_anim_group_rec(entry)->direction != direction && jo::as_anim_group_rec(entry)->direction != 0) continue;
            {
                std::uint8_t* const record = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_refs(img::obj(kSoundRecords))->get(joan::orig::arg::i16(jo::view<std::int16_t[2]>(jo::as_anim_group_rec(entry)->model_ref)[0])));  // 0x00416f20
                jo::self<jo::Unit>(this)->gravity_level = joan::hook::x87::ftol(static_cast<double>(jo::view<float>(jo::as_model_slot(record)->gravity_level)));
            }
            // (ebp-0x20 = 1 when +0x243 > 3: never read)
            const std::int32_t kind = jo::self<jo::Unit>(this)->anim_kind;
            if (kind == 1 && jo::self<jo::Unit>(this)->combo_pre_len != 0) {
                const std::int32_t ok = pattern_ok(entry, this, &jo::Unit::combo_pre_len, 6, true);
                if (ok != 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set_entry(entry)->is_yes()) != 0) return i;  // 0x00416ed0
                if (jo::self<jo::Unit>(this)->combo_pre_len != 0) continue;
            }
            if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) == 0x1a) {
                copy_anim_params(entry);
                if (jo::as_anim_group_rec(entry)->direction == direction) return i;
                fallback = i;
                continue;
            }
            if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) == 0x1b) {
                copy_anim_params(entry);
                return i;
            }
            if (jo::self<jo::Unit>(this)->combo_state == 0) return i;
            if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) == 0x1c || static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) == 0x49) {
                if (pattern_ok(entry, this, &jo::Unit::combo_state, 0, false) != 0) {
                    if (jo::self<jo::Unit>(this)->layer != 0 || joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set_entry(entry)->is_yes()) != 0 ||          // 0x00416ed0
                        joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->has_item_kind9(joan::orig::arg::str(static_cast<void*>(entry + 8)))) != 0) {           // 0x0050d170
                        copy_anim_params(entry);
                        if (jo::as_anim_group_rec(entry)->direction == direction) return i;
                        fallback = i;
                    }
                }
                continue;
            }
            if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) == 0x22) {
                if (jo::self<jo::Unit>(this)->layer != 0 || joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set_entry(entry)->is_yes()) != 0 ||  // 0x00416ed0
                    joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->has_item(0xb)) != 0) {                            // 0x0050d320
                    copy_anim_params(entry);
                    if (jo::as_anim_group_rec(entry)->direction == direction) return i;
                    fallback = i;
                }
                continue;
            }
            if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) == 0x53) {
                if (jo::self<jo::Unit>(this)->layer != 0 || joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set_entry(entry)->is_yes()) != 0 ||  // 0x00416ed0
                    joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->has_item(0xc)) != 0) {                            // 0x0050d320
                    copy_anim_params(entry);
                    if (jo::as_anim_group_rec(entry)->direction == direction) return i;
                    fallback = i;
                }
                continue;
            }
            if (jo::as_anim_group_rec(entry)->direction == direction) return i;
            fallback = i;
        }
        return fallback;
    }

    // ---- 0x0050a480 ------------------------------------------------------------------------
    // Release the unit of a slot (+0x1bd / +0x1bf) with its holder (+0x32d / +0x331).
    void release_slot(std::int16_t id, std::int16_t jo::Unit::*slot, jo::Trail* jo::Unit::*holder_field, std::int16_t* old_out) {
        if (units_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->*slot)) == 0 &&
            static_cast<std::int32_t>(jo::self<jo::Unit>(this)->*slot) != static_cast<std::int32_t>(id)) {
            if (jo::view<void*>(jo::as_map_object(units_get(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->*slot)))->actor) != nullptr) {
                std::uint8_t* const unit = units_get(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->*slot));
                joan::orig::as_bone(jo::view<void*>(jo::as_map_object(unit)->actor->bone))->set_parent(joan::orig::arg::mptr(0));  // 0x0060ec70
            }
            if (jo::view<void*>(jo::self<jo::Unit>(this)->*holder_field) != nullptr) {
                void* const holder = jo::view<void*>(jo::self<jo::Unit>(this)->*holder_field);
                if (holder != nullptr) joan::orig::as_trail(holder)->destroy(1u);  // 0x00420790 (deleting destructor)
                jo::self<jo::Unit>(this)->*holder_field = nullptr;
            }
            if (old_out != nullptr)
                *old_out = jo::self<jo::Unit>(this)->*slot;
            else
                joan::orig::as_units(img::obj(kUnits))->remove_49f370(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->*slot));  // 0x0049f370
        }
        jo::self<jo::Unit>(this)->*slot = id;
    }
    void link_holder(jo::Trail* jo::Unit::*holder_field, std::uint8_t* unit) {
        if (jo::view<void*>(jo::self<jo::Unit>(this)->*holder_field) == nullptr) jo::view<void*>(jo::self<jo::Unit>(this)->*holder_field) = new_holder();
        joan::orig::as_trail(jo::view<void*>(jo::self<jo::Unit>(this)->*holder_field))->init(joan::orig::arg::mptr(static_cast<void*>(unit)));  // 0x006871f0
    }

    // 0x0050a480.
    void attach_unit(std::int16_t id, std::int32_t anim, std::int8_t slot, std::int16_t* old_out) {
        c_attach_unit.hit();
        if (slot == 0)
            release_slot(id, &jo::Unit::carried_a, &jo::Unit::trail_a, old_out);
        else
            release_slot(id, &jo::Unit::carried_b, &jo::Unit::trail_b, old_out);
        if (units_missing(static_cast<std::int32_t>(id)) != 0) return;
        std::uint8_t* const unit = units_get(static_cast<std::int32_t>(id));  // ebp-0x14
        if (jo::self<jo::Unit>(this)->actor == nullptr) return;
        virtual_call(unit, 0x24);
        if (anim != 0) {
            std::int32_t key = 0;  // ebp-0x18
            void* const anims = jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->def->model->bones);
            if (anims != nullptr) key = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_table(anims)->find(anim));  // 0x005ba160
            void* const value = joan::orig::as_plane(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->plane))->bone_5cf8f0(joan::orig::arg::i32(key));  // 0x005cf8f0
            if (jo::view<void*>(jo::as_map_object(unit)->actor) != nullptr) {
                joan::orig::as_bone(jo::view<void*>(jo::as_map_object(unit)->actor->bone))->set_parent(value);  // 0x0060ec70
                if (slot == 0)
                    link_holder(&jo::Unit::trail_a, unit);
                else
                    link_holder(&jo::Unit::trail_b, unit);
            }
        }
        joan::orig::as_map_object(unit)->set_world(jo::self<jo::Unit>(this)->world);  // 0x004994b0
        if (jo::self<jo::Unit>(this)->cell != nullptr) joan::orig::as_cell(jo::self<jo::Unit>(this)->cell)->attach_unit(joan::orig::arg::mptr(static_cast<void*>(unit)));  // 0x0043b190
        if (jo::view<void*>(jo::as_map_object(unit)->actor) != nullptr && jo::self<jo::Unit>(this)->scene != nullptr)
            joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::view<void*>(jo::as_map_object(unit)->actor)));  // 0x005efc50
    }

    // ---- 0x0050cc70 ------------------------------------------------------------------------
    void create_alt_mesh(std::int32_t mode) {
        c_alt_mesh.hit();
        if (mode != 1) return;  // unreachable (uninitialised locals in the original), see header
        if (jo::self<jo::Unit>(this)->actor_b != nullptr) return;
        std::uint8_t* const def = def_of(jo::self<jo::UnitType>(type_of(jo::self<jo::Unit>(this)->type_id))->model_b);  // ebp-0x14
        {
            const std::int32_t mesh_id = jo::view<std::int32_t>(jo::self<jo::UnitType>(type_of(jo::self<jo::Unit>(this)->type_id))->anim_id_b);
            jo::self<jo::Unit>(this)->anim_set_b = joan::orig::arg::as<jo::AnimSet*>(joan::orig::as_anim_res_mgr(img::obj(kMeshes))->entry(joan::orig::arg::u32(mesh_id)));  // 0x004b2730
        }
        jo::self<jo::Unit>(this)->actor_def_b = jo::as_model_entry(def)->actor_def;
        jo::self<jo::Unit>(this)->actor_b = static_cast<jo::Actor*>(new_mesh_object());
        jo::as_actor(jo::self<jo::Unit>(this)->actor_b)->def = jo::self<jo::Unit>(this)->actor_def_b;
        void* const object = jo::self<jo::Unit>(this)->actor_b;  // ebp-0x18
        jo::view<std::int32_t>(jo::self<jo::Unit>(this)->alt_mesh_state) = 2;

        joan::orig::as_scene_object(object)->init();                                        // 0x005aab70
        joan::orig::as_actor(object)->set_skin(joan::orig::arg::u32(jo::as_model_entry(def)->anim_db_id));           // 0x005add40
        joan::orig::as_actor(object)->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Unit>(this)->position_)));  // 0x005ac310
        jo::as_actor(object)->kind = 0xb;
        joan::en3d::unit_index_actor_kind(object);  // [Debug] UnitIndex
        jo::as_actor(object)->owner = this;
        joan::orig::as_scene_object(object)->set_yaw(joan::orig::arg::u32(jo::view<std::int32_t>(jo::self<jo::Unit>(this)->heading)));  // 0x005ac6a0
        for (std::int32_t i = 0; i < jo::as_model_entry(def)->morph_count; ++i) {
            joan::orig::as_actor(object)->add_part(joan::orig::arg::str(static_cast<void*>(jo::view<std::uint8_t*>(jo::as_model_entry(def)->morphs) + i * 0xa4)), jo::as_model_entry(def)->dir, joan::orig::arg::str(static_cast<void*>(jo::view<std::uint8_t*>(jo::as_model_entry(def)->morphs) + i * 0xa4 + 0x20)), nullptr);  // 0x005adf50
            if (joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(static_cast<void*>(jo::view<std::uint8_t*>(jo::as_model_entry(def)->morphs) + i * 0xa4 + 0x60)))) != 0) {  // strlen
                joan::orig::as_actor(object)->attach(joan::orig::arg::str(static_cast<void*>(jo::view<std::uint8_t*>(jo::as_model_entry(def)->morphs) + i * 0xa4)), img::str(kAnim), joan::orig::arg::str(static_cast<void*>(jo::view<std::uint8_t*>(jo::as_model_entry(def)->morphs) + i * 0xa4 + 0x60)));  // 0x005ae400
            }
        }
        jo::view<std::int32_t>(jo::as_actor(object)->aim_mode) = jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode);
        switch (jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode)) {
            case 1:
            case 3:
                std::memcpy(jo::as_actor(object)->aim_sphere, jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_sphere, 0x10);
                break;
            case 2:
                std::memcpy(jo::as_actor(object)->aim_box, jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_box, 0x18);
                break;
            default:
                break;
        }
        jo::view<std::int32_t>(jo::as_actor(object)->record) = joan::orig::arg::as<std::int32_t>(joan::orig::as_army(img::obj(kSounds))->fire_shot(joan::orig::arg::mptr(object), joan::orig::arg::bits(60.0f), joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->layer)), 0u, 2u, 1u));  // 0x006adbf0
        joan::orig::as_players(img::obj(kSounds))->set_active(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(object)->record)), joan::orig::arg::i16(0));  // 0x006ad980
    }
};

JOAN_REPLACE("game/b82_08", "CellObject::set_anim_state", 0x004ff9f0, &H::set_anim_state);
JOAN_REPLACE("game/b82_08", "CellObject::find_anim_state", 0x00504010, &H::find_anim_state);
JOAN_REPLACE("game/b82_08", "CellObject::attach_unit", 0x0050a480, &H::attach_unit);
JOAN_REPLACE("game/b82_08", "CellObject::create_alt_mesh", 0x0050cc70, &H::create_alt_mesh);

}  // namespace game_b82_08
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

using u32 = std::uint32_t;

using joan::orig::helpers::addr_of;

struct Vec3 {
    float v[3];
};
static_assert(sizeof(Vec3) == 12, "by-value 12-byte argument");

JOAN_COUNTER(c_hit_5051e0, "Unit::hit_value_5051e0");

u32 JOAN_CDECL dtor_508840() { joan::orig::as_wrap(obj(d::g_a3cae0))->f_4f0500(); return 0; }  // 0x004f0500 (void; atexit callback)
u32 JOAN_CDECL dtor_508860() { joan::orig::as_wrap(obj(d::g_a3cb28))->f_4f0500(); return 0; }  // 0x004f0500 (void; atexit callback)

struct H {

    std::uintptr_t set1b() {
        // this: the CriticalHud 0x00a4f820 (the only caller, 0x00509cbb; facade CriticalHud::mark_crit_kill)
        jo::self<jo::CriticalHud>(this)->i8_1b = 1;
        return addr_of(this);
    }
    void nop() {}

    // ------------------------------------------------------------------------ 0x005051e0
    float hit_value_5051e0(i32 attacker, u32 arg, u32 flag_arg) {
        c_hit_5051e0.hit();
        const float zero = global<float>(d::k_9cf5c4);
        u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(attacker)));  // 0x0041b3c0
        const auto type = [this]() { return static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind); };
        if (type() == 0x2a) return zero;
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(this)->dead())) != 0) return zero;  // 0x0042d0e0
        if (attacker == static_cast<i32>(jo::self<jo::Unit>(this)->id)) return zero;
        if (type() == 0x1e || type() == 0x1f || type() == 0x20 || type() == 0x21 || type() == 0x53)
            return zero;
        if (static_cast<i8>(flag_arg) == 0) {
            if (type() == 0x40 || type() == 0x41 || type() == 0x42 || type() == 0x4e) return zero;
            if (type() == 0x29) return zero;
        }
        if (jo::self<jo::Unit>(this)->hit_value > 0) return zero;
        if (static_cast<i8>(jo::as_unit(rec)->in_scene) == 0) return zero;
        const u32 word_arg = static_cast<u16>(attacker);  // x64: the original's upper word was `this` (eax leftover); hostile_to reads a word
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hostile_to(joan::orig::arg::i16(word_arg))) == 0) return zero;  // 0x005094d0
        const float a = joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to_unit(joan::orig::arg::i32(attacker)));       // 0x00505960

        if (type() == 0x2b || type() == 0x2d) {
            const i32 g = global<i32>(d::g_a3ccb0);
            const i32 ng = static_cast<i32>(static_cast<u32>(g) * 0xffffffffu);
            if (static_cast<double>(g) >= static_cast<double>(a) &&
                !(static_cast<double>(ng) > static_cast<double>(a))) {
                const i32 id = joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_model_refs))->find_41a0e0(str(d::s_DEFEND)));  // 0x0041a0e0
                if (id != 0) {
                    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(id)))));  // 0x00416f20
                    if (jo::as_model_slot(p)->emitter_flag != 0 &&
                        joan::crt::stricmp(joan::orig::arg::str(&jo::as_model_slot(p)->emitter_names[5 * 33]), str(d::s_none)) != 0) {  // stricmp
                        jo::Scene* const x18 = jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene;
                        joan::orig::as_spawner(obj(d::g_particle_effect_array))->try_spawn(joan::orig::arg::i32(id), x18, 5, static_cast<i32>(jo::self<jo::Unit>(this)->id));  // 0x0041b640
                    }
                }
                Vec3 pos;
                std::memcpy(&pos, jo::self<jo::Unit>(this)->position_, sizeof pos);
                joan::orig::as_sound_list(obj(d::g_sfx_player))->play_effect(str(d::s_GUARDARR), pos);  // 0x00494810
                if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(0x2d)) != 0) {         // 0x00416dc0
                    if (type() != 0x2d) {
                        joan::orig::as_unit(this)->set_action(0x2d, 0, 1);  // 0x00501190
                    } else {
                        const double s = joan::orig::arg::as<double>(joan::orig::as_unit(this)->anim_phase());  // 0x005045f0
                        const double z = static_cast<double>(global<float>(d::k_9cf5c4));
                        if (!(s < z) && !(s > z)) joan::orig::as_unit(this)->set_action(0x2d, 0, 1);
                    }
                }
                return global<float>(d::k_9cf5c4);
            }
        }

        i32 k;
        if (joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(joan::hook::x87::ftol(a)))) <= 0x2d) {
            k = 0;
        } else if (joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(joan::hook::x87::ftol(a)))) <= 0x87) {
            k = 0x14;
        } else {
            k = 0x28;
        }
        const float r = joan::orig::arg::as<float>(joan::orig::as_unit(this)->damage_509e90(joan::orig::arg::i32(attacker), joan::orig::arg::i32(arg), joan::orig::arg::i32(k)));  // 0x00509e90
        if (attacker == global<i32>(d::g_player_unit_recno))
            joan::orig::as_hud(obj(d::g_player_hud))->set_item_portrait(static_cast<i32>(jo::self<jo::Unit>(this)->id));  // 0x006fe280
        if ((r < global<float>(d::k_9cf5c4) || r > global<float>(d::k_9cf5c4)) && jo::self<jo::Unit>(this)->layer != 0)
            jo::view<u16>(jo::self<jo::Unit>(this)->attack_chance) = static_cast<u16>(jo::view<u16>(jo::self<jo::Unit>(this)->attack_chance) + 5);
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_500b80", 0x00500b80, &H::nop);
B("Unit::hit_value_5051e0", 0x005051e0, &H::hit_value_5051e0);
B("dtor_508840", 0x00508840, &dtor_508840);
B("dtor_508860", 0x00508860, &dtor_508860);
B("Obj::set1b_509d70", 0x00509d70, &H::set1b);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {


JOAN_COUNTER(c_can_act, "Unit::can_act");
JOAN_COUNTER(c_find_link, "Unit::find_link");
JOAN_COUNTER(c_mask_ready, "Unit::has_mask_ready");

struct H {
    // 0x00500ba0
    bool can_act() {
        c_can_act.hit();
        if (jo::self<jo::Unit>(this)->act_time + global<std::uint32_t>(d::g_a3cce8) > global<std::uint32_t>(d::g_now)) return false;
        switch (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind)) {
            case -1: case 0: case 1: case 2: case 3: case 4: case 5: case 6:
            case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
            case 19: case 20: case 43: case 44: case 45: case 54: case 55: case 57: case 67: case 68:
            case 69: case 70: case 84: case 85: case 86: case 87: case 89: case 90:
                return true;
            default:
                return false;
        }
    }

    // 0x0050ac00
    float float_1e() { return jo::self<jo::Unit>(this)->heading_goal; }

    // 0x0050d3d0
    std::int32_t find_link(std::int32_t kind, const char* name) {
        c_find_link.hit();
        void* const list = &jo::self<jo::Unit>(this)->upgrades;
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(list)->count()); i > 0; --i) {  // 0x00531360
            std::int16_t* const e = joan::orig::arg::as<std::int16_t*>(joan::orig::as_upgrade_list(list)->get(joan::orig::arg::i32(i)));         // 0x005313a0
            std::int16_t* const r =
                joan::orig::arg::as<std::int16_t*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(e[0]))));  // 0x0050d240
            if (static_cast<std::int32_t>(r[0]) != kind) continue;
            if (name != nullptr && stricmp_(reinterpret_cast<std::uint8_t*>(r) + 0x46, name) != 0) continue;
            if (static_cast<std::int32_t>(e[2]) != static_cast<std::int32_t>(e[1])) continue;
            return e[3];
        }
        return 0;
    }

    // 0x0050d550
    std::int32_t has_mask_ready(std::int8_t mask) {
        c_mask_ready.hit();
        if (jo::self<jo::Unit>(this)->crew_slots_ready == 0) return 0;
        std::int32_t count = 0;
        void* const rec = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id)));  // 0x0042d0a0
        if (jo::as_unit_type(rec)->slot_count <= 0) return 0;
        for (std::int32_t i = jo::as_unit_type(rec)->slot_count - 1; i >= 0; --i) {
            const std::int32_t bits = static_cast<std::int32_t>(jo::view<std::int8_t*>(jo::as_unit_type(rec)->buffers[2])[i]);
            if ((bits & static_cast<std::int32_t>(mask)) == 0) continue;
            ++count;
            if (!(jo::view<std::int32_t*>(jo::self<jo::Unit>(this)->mask_slots)[i] > 0)) return 0;
        }
        return count != 0 ? 1 : 0;
    }

    // 0x00510f80
    std::int32_t is_kind_510f80() {
        switch (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind)) {
            case 8: case 25: case 28: case 29: case 30: case 31: case 32: case 33: case 34:
            case 70: case 73: case 83:
                return 1;
            default:
                return 0;
        }
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Unit::can_act", 0x00500ba0, &H::can_act);
M("Unit::float_1e", 0x0050ac00, &H::float_1e);
M("Unit::find_link", 0x0050d3d0, &H::find_link);
M("Unit::has_mask_ready", 0x0050d550, &H::has_mask_ready);
M("Unit::is_kind_510f80", 0x00510f80, &H::is_kind_510f80);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {


JOAN_COUNTER(c_link_value, "Unit::link_value");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00504800
    float link_value(std::int32_t index) {
        c_link_value.hit();
        if (jo::self<jo::Unit>(this)->actor == nullptr) return global<float>(d::k_9cf5c4);
        if (index == 0) return global<float>(d::k_9cf5c4);
        void* const e1 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(index)));  // 0x004168d0
        void* const entry = *joan::orig::arg::as<void**>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_anim_group_rec(e1)->anim_id))));  // 0x004174f0
        void* const e2 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(index)));  // 0x004168d0
        const std::uint32_t key = joan::orig::arg::as<std::uint32_t>(joan::orig::as_anim_def(entry)->find_action(joan::orig::arg::str(static_cast<std::uint8_t*>(e2) + 0x4d)));  // 0x005b5be0
        void* const r = joan::orig::arg::as<void*>(joan::orig::as_anim_set(entry)->frame(joan::orig::arg::i32(key), 0u));  // 0x005b5dd0
        return joan::orig::arg::as<float>(joan::orig::as_range(r)->span());                       // 0x0042d130
    }

    // 0x0050d0d0
    void* clear_flags() {
        jo::self<jo::Unit>(this)->armour_flags[0] = 0;
        jo::self<jo::Unit>(this)->armour_flags[1] = 0;
        jo::self<jo::Unit>(this)->armour_flags[2] = 0;
        jo::self<jo::Unit>(this)->armour_flags[3] = 0;
        jo::self<jo::Unit>(this)->armour_flags[4] = 0;
        jo::self<jo::Unit>(this)->armour_flags[5] = 0;
        if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->combo_state) != -1) jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->combo_state) = 0;
        jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->combo_queued) = 0;
        return self();
    }

    void* set_a8(std::uint32_t v) {  // 0x00503fe0
        jo::self<jo::Player>(this)->u32_a8 = v;
        return self();
    }
    void* set_213(std::uint32_t v) {  // 0x00500d10
        jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->action) = static_cast<std::uint16_t>(v);
        return self();
    }
    void* set_9d() {                                              // 0x00503fb0
        jo::view<std::uint8_t>(jo::self<jo::Player>(this)->combo_count) = 1;
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Unit::link_value", 0x00504800, &H::link_value);
M("Unit::clear_flags", 0x0050d0d0, &H::clear_flags);
M("Obj::set_a8", 0x00503fe0, &H::set_a8);
M("Unit::set_213", 0x00500d10, &H::set_213);
M("Obj::set_9d", 0x00503fb0, &H::set_9d);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05 {

void* const kFx = obj(d::g_sfx_player);
void* const kSpells = obj(d::g_item_res);


using u32 = std::uint32_t;

// 12-byte vector passed by value: float members like the callee's (System V passes them in XMM registers).
struct V3 {
    float a, b, c;
};
static_assert(sizeof(V3) == 12, "V3");
V3 v3_at(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

void* const kModels = obj(d::g_physic_sys);
void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_has_order, "Unit::has_order_for");
JOAN_COUNTER(c_spawn, "Unit::spawn_projectile");
JOAN_COUNTER(c_heal, "Unit::heal");
JOAN_COUNTER(c_heal_mana, "Unit::heal_and_mana");
JOAN_COUNTER(c_buy_stat, "Unit::buy_stat");

// Unit helpers (thunks of the original calls).
bool unit_dead(void* u) { return joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0; }      // 0x0042d0e0 (movsx al)
i32 unit_max_hp(void* u) { return joan::orig::arg::as<i32>(joan::orig::as_unit(u)->hit_points_max()); }         // 0x00523320
i32 unit_max_mana(void* u) { return joan::orig::arg::as<i32>(joan::orig::as_unit(u)->value_149()); }       // 0x00523700
void unit_set_hp(void* u, i32 v) { joan::orig::as_unit(u)->set_value_135(joan::orig::arg::i32(v)); }    // 0x004fe510
void unit_set_mana(void* u, i32 v) { joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(v)); }  // 0x004fe5d0
void* spell_rec(u32 spell) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kSpells)->get(joan::orig::arg::i16(spell))); }  // 0x00425ce0

// Ally list (byte count at +2, word ids at +1 + 2 * i) of the group [unit + 0x33].
i32 group_count(void* g) { return static_cast<i32>(jo::as_group(g)->count); }
i32 group_id(void* g, i32 i) { return static_cast<i32>(jo::as_group(g)->members[i - 1]); }

// Ratio hp / max at PC24: fild hp; fstp float; fild max; fdivr float.
float hp_ratio(void* u, i32* max_out) {
    const float hp = static_cast<float>(jo::as_unit(u)->hit_points);  // fild + fstp float
    const i32 max = unit_max_hp(u);
    if (max_out) *max_out = max;
    return static_cast<float>(Fpu{true}.div(static_cast<double>(hp), static_cast<double>(max)));
}

// 0x0050b84b / 0x0050c0eb tail shared by both heal functions.
void heal_effect(void* self, u32 spell, i32 flag) {
    if (global<i32>(d::g_player_unit_recno) != static_cast<i32>(jo::as_unit(self)->id) || flag == 0) return;
    void* const rec = spell_rec(spell);
    if (jo::view<u32>(jo::as_item_def(rec)->sounds[0]) == 0) return;
    if (global<float>(d::g_game_speed) > global<float>(d::k_9cf5c4)) {  // test ah,0x41 / jne
        joan::orig::as_sound_list(kFx)->play_at(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(rec)->sounds[0])), v3_at(jo::as_unit(self)->position_));  // 0x004950a0
    } else {
        const char* const s = static_cast<const char*>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16[2]>(jo::as_item_def(rec)->sounds[0])[0]))));  // 0x004d0820
        joan::orig::as_sound(kFx)->play_def(jo::as_wave_res(s)->name);  // 0x004ceef0 (+6 on x86: the WaveRes name behind its vtable pointer)
    }
}

// 0x0050b6be / 0x0050bf5e: spread the remaining amount over the wounded allies of the
// group 0x004216d0(self), lowest hp ratio first.
void heal_spread(void* self, i32 rem) {
    void* const grp = joan::orig::arg::as<void*>(joan::orig::as_unit(self)->leader());  // 0x004216d0
    if (grp == nullptr || jo::as_unit(grp)->group == nullptr) return;
    while (rem > 0) {
        void* best = nullptr;
        float best_ratio = 100.0f;  // 0x42c80000
        for (i32 i = group_count(jo::as_unit(grp)->group); i > 0; --i) {
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(group_id(jo::as_unit(grp)->group, i))) != 0) continue;  // 0x00428b70
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(group_id(jo::as_unit(grp)->group, i)));  // 0x0041b3c0
            if (unit_dead(u)) continue;
            const i32 max = unit_max_hp(u);
            if (!(jo::as_unit(u)->hit_points < max)) continue;
            if (best != nullptr) {
                const float r = hp_ratio(u, nullptr);
                if (!(r < best_ratio || r != r)) continue;  // test ah,1 / je: skip unless < or NaN
            }
            best = u;
            best_ratio = hp_ratio(u, nullptr);
        }
        if (best == nullptr) return;
        const i32 max = unit_max_hp(best);
        const i32 amount = rem;
        const i32 sum = amount + jo::as_unit(best)->hit_points;
        const i32 nv = sum < max ? sum : max;
        rem = amount - (nv - jo::as_unit(best)->hit_points);
        unit_set_hp(best, nv);
        jo::as_unit(best)->hp_shown = nv;
    }
}

struct H {

    // ------------------------------------------------------------------------ 0x00500d40
    // Search the order list [+0x2f1] for an entry of `type` addressed to this unit's cell.
    u8 order_scan(void* list, i32 type, bool check_level, i32 fallback_arg, bool use_fallback) {
        for (i32 i = jo::as_anim_set(list)->group_count; i > 0; --i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_anim_set(list)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
            if (jo::view<i32>(jo::as_anim_group_rec(e)->is_jump) != static_cast<i32>(jo::self<jo::Unit>(this)->action)) continue;
            if (jo::as_anim_group_rec(e)->kind != type) continue;
            if (jo::as_anim_group_rec(e)->weapon_kind != static_cast<i32>(jo::self<jo::Unit>(this)->anim_item_using)) continue;
            if (jo::as_anim_group_rec(e)->direction != static_cast<i32>(jo::self<jo::Unit>(this)->anim_special)) continue;
            if (check_level && !(jo::self<jo::Unit>(this)->mana > jo::as_anim_group_rec(e)->min_mana)) continue;
            if (jo::self<jo::Unit>(this)->layer != 0) return 1;
            if (joan::orig::arg::as<i32>(joan::orig::as_anim_set_entry(e)->is_yes()) != 0) return 1;  // 0x00416ed0
            if (use_fallback && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->has_item(joan::orig::arg::i32(fallback_arg))) != 0) return 1;  // 0x0050d320
            return 0;
        }
        return 0;
    }
    u8 has_order_for(u32 kind_arg) {
        c_has_order.hit();
        if (jo::self<jo::Unit>(this)->anim_set == nullptr) return 0;
        void* const list = jo::self<jo::Unit>(this)->anim_set;
        switch (static_cast<i32>(static_cast<i16>(kind_arg))) {
            case 7: return order_scan(list, 7, false, 0, false);
            case 30:
                if (joan::orig::arg::as<i8>(joan::orig::as_game(this)->byte_2e1()) != 0) return 0;  // 0x00432b90
                return order_scan(list, 0x1e, false, 0, false);
            case 34: return order_scan(list, 0x22, false, 0xb, true);
            case 83: return order_scan(list, 0x53, true, 0xc, true);
            default: return 1;
        }
    }

    // ------------------------------------------------------------------------ 0x0050afe0
    void spawn_projectile() {
        c_spawn.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 2) return;  // 0x004cd510
        if (jo::self<jo::Unit>(this)->drop_item == 0) return;
        void* const rec = spell_rec(jo::view<u16>(jo::self<jo::Unit>(this)->drop_item));
        if (jo::view<i8>(jo::as_item_def(rec)->unique_item) != 0 &&
            (joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->has_word(joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Unit>(this)->drop_item)))) & 0xffu) != 0) {  // 0x00472740
            const u16 list = joan::orig::arg::as<u16>(joan::orig::as_names(obj(d::g_randList))->find_463ed0(str(d::s_unique_random_list)));  // 0x00463ed0 "unique_random_list"
            void* const l = joan::orig::arg::as<void*>(joan::orig::as_names(obj(d::g_randList))->at(joan::orig::arg::i16(static_cast<u32>(list))));  // 0x00464100
            joan::orig::as_grass(l)->pick(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Unit>(this)->drop_item)), joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Unit>(this)->treasure_level)));  // 0x00464030
        }
        const i32 id = joan::orig::arg::as<i32>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->drop_item))));  // 0x0049ea90
        void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        joan::orig::as_map_object(o)->set_world(jo::self<jo::Unit>(this)->world);                                         // 0x004994b0
        V3 pos = v3_at(jo::self<jo::Unit>(this)->position_);
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
            void* const m = joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)));  // 0x006af310
            if (jo::as_shot(m)->state.zone != 0) {
                const float z = static_cast<float>(
                    Fpu{true}.add(static_cast<double>(jo::view<float>(jo::as_shot(m)->state.cell)), global<double>(d::dbl_0_2)));
                std::memcpy(&pos.c, &z, 4);
            }
        }
        joan::orig::as_map_object(o)->set_position(pos);            // 0x0041ff50
        joan::orig::as_map_object(o)->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)));   // 0x00499fd0
        jo::view<u16>(jo::as_map_object(o)->side) = jo::view<u16>(jo::self<jo::Unit>(this)->side);
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::WorldItem)));  // operator new (0x52 on x86)
        void* const p = mem != nullptr ? joan::orig::as_world_item(mem)->construct() : nullptr;  // 0x00456eb0
        const u16 w2a = jo::as_item_def(spell_rec(jo::view<u16>(jo::self<jo::Unit>(this)->drop_item)))->stats[3];
        joan::orig::as_item_stack(p)->init(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::self<jo::Unit>(this)->drop_item))), joan::orig::arg::i16(static_cast<u32>(jo::view<u16[2]>(jo::self<jo::Unit>(this)->treasure_level)[0])), joan::orig::arg::i16(static_cast<u32>(w2a)), 0u, 0u, 0u);  // 0x00456cd0
        jo::view<u16>(jo::as_world_item(p)->object_id) = jo::view<u16>(jo::as_map_object(o)->index);
        jo::as_world_item(p)->side = jo::view<u16>(jo::self<jo::Unit>(this)->side);
        joan::orig::as_world_item(p)->set_state(jo::self<jo::Unit>(this)->world);            // 0x00457120
        {
            const V3 here = v3_at(jo::self<jo::Unit>(this)->position_);
            joan::orig::as_world_item(p)->set_pos(joan::orig::arg::bits(here.a), joan::orig::arg::bits(here.b), joan::orig::arg::bits(here.c));      // 0x00456fe0
        }
        joan::orig::as_world_item(p)->set_heading(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)));             // 0x004571b0
        jo::view<void*>(jo::as_map_object(o)->world_item) = p;
        if (jo::as_map_object(o)->actor != nullptr) {
            jo::as_map_object(o)->actor->aim_mode = 0;
            if (jo::self<jo::Unit>(this)->markers == 1 || jo::self<jo::Unit>(this)->markers == -2) joan::orig::as_model_instance(jo::view<void*>(jo::as_map_object(o)->actor))->set_10a(0u);  // 0x0042cf20
        }
    }

    // ------------------------------------------------------------------------ 0x0050b4f0
    i32 heal(u32 spell, i32 even_dead, i32 force) {
        c_heal.hit();
        if (unit_dead(this) && even_dead == 0) return 0;
        i32 flag = 0;
        const i32 max = unit_max_hp(this);
        if (force != 0)
            flag = 1;
        else if (jo::self<jo::Unit>(this)->hit_points < max)
            flag = 1;
        else
            return 0;
        const i32 amount = jo::as_item_def(spell_rec(spell))->damage;
        const i32 sum = amount + jo::self<jo::Unit>(this)->hit_points;
        const i32 nv = sum < max ? sum : max;
        const i32 rem = amount - (nv - jo::self<jo::Unit>(this)->hit_points);
        unit_set_hp(this, nv);
        jo::self<jo::Unit>(this)->hp_shown = nv;
        if (jo::self<jo::Unit>(this)->group != nullptr) {
            for (i32 i = group_count(jo::self<jo::Unit>(this)->group); i > 0; --i) {
                if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(group_id(jo::self<jo::Unit>(this)->group, i))) != 0) continue;  // 0x00428b70
                void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(group_id(jo::self<jo::Unit>(this)->group, i)));  // 0x0041b3c0
                if (unit_dead(u)) continue;
                const i32 umax = unit_max_hp(u);
                if (jo::as_unit(u)->hit_points < umax) flag = 1;
                const i32 s = amount / 2 + jo::as_unit(u)->hit_points;
                const i32 v = s < umax ? s : umax;
                unit_set_hp(u, v);
                jo::as_unit(u)->hp_shown = v;
            }
        } else {
            heal_spread(this, rem);
        }
        heal_effect(this, spell, flag);
        return flag;
    }

    // ------------------------------------------------------------------------ 0x0050bcd0
    i32 heal_and_mana(u32 spell, i32 even_dead, i32 force) {
        c_heal_mana.hit();
        if (unit_dead(this) && even_dead == 0) return 0;
        i32 flag = 0;
        const i32 max = unit_max_hp(this);
        const i32 max_mana = unit_max_mana(this);
        if (force != 0)
            flag = 1;
        else if (jo::self<jo::Unit>(this)->hit_points < max)
            flag = 1;
        else
            return 0;
        const i32 amount = jo::as_item_def(spell_rec(spell))->damage;
        const i32 sum = amount + jo::self<jo::Unit>(this)->hit_points;
        const i32 nv = sum < max ? sum : max;
        const i32 rem = amount - (nv - jo::self<jo::Unit>(this)->hit_points);
        const i32 amount2 = jo::as_item_def(spell_rec(spell))->damage;
        const i32 sum2 = amount2 + jo::self<jo::Unit>(this)->mana;
        const i32 nm = sum2 < max_mana ? sum2 : max_mana;
        unit_set_hp(this, nv);
        jo::self<jo::Unit>(this)->hp_shown = nv;
        unit_set_mana(this, nm);
        if (jo::self<jo::Unit>(this)->group != nullptr) {
            for (i32 i = group_count(jo::self<jo::Unit>(this)->group); i > 0; --i) {
                if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(group_id(jo::self<jo::Unit>(this)->group, i))) != 0) continue;  // 0x00428b70
                void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(group_id(jo::self<jo::Unit>(this)->group, i)));  // 0x0041b3c0
                if (unit_dead(u)) continue;
                const i32 umax = unit_max_hp(u);
                const i32 umana = unit_max_mana(u);
                if (jo::as_unit(u)->hit_points < umax)
                    flag = 1;
                else if (jo::as_unit(u)->mana < umana)
                    flag = 1;
                const i32 s = amount / 2 + jo::as_unit(u)->hit_points;
                const i32 v = s < umax ? s : umax;
                const i32 s2 = amount2 / 2 + jo::as_unit(u)->mana;
                (void)(s2 < umana ? s2 : umana);  // computed into a dead local by the original
                unit_set_hp(u, v);
                jo::as_unit(u)->hp_shown = v;
                unit_set_mana(u, v);  // original passes the hp value here
            }
        } else {
            heal_spread(this, rem);
        }
        heal_effect(this, spell, flag);
        return flag;
    }

    // ------------------------------------------------------------------------ 0x0050d640
    void bump_12c(bool undo) { jo::view<u16>(jo::self<jo::Unit>(this)->attr_points) = static_cast<u16>(jo::view<u16>(jo::self<jo::Unit>(this)->attr_points) + (undo ? 1 : -1)); }
    i32 buy_stat(i32 stat, i32 undo, u32 item) {
        c_buy_stat.hit();
        const bool u = undo != 0;
        const auto g16 = [](img::Addr a) { return static_cast<i32>(global<i16>(a)); };
        const auto addsub = [u](i32 a, i32 b) {
            return u ? static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b))
                     : static_cast<i32>(static_cast<u32>(a) + static_cast<u32>(b));
        };
        const auto mul = [](i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(b)); };
        switch (stat) {
            case 1:
                joan::orig::as_unit(this)->set_strength(joan::orig::arg::u32(addsub(static_cast<i32>(jo::self<jo::Unit>(this)->stats[0]), g16(d::g_a3cd1a))));  // 0x004fe6e0
                joan::orig::as_unit(this)->set_level_45b720(joan::orig::arg::i32(addsub(jo::self<jo::Unit>(this)->hit_points_base, mul(g16(d::g_a3cd1a), g16(d::g_unit_str_hp)))));  // 0x0045b720
                bump_12c(u);
                break;
            case 2:
                joan::orig::as_unit(this)->set_dexterity(joan::orig::arg::u32(addsub(static_cast<i32>(jo::self<jo::Unit>(this)->stats[1]), g16(d::g_a3cc18))));  // 0x004fe710
                bump_12c(u);
                break;
            case 3:
                joan::orig::as_unit(this)->set_defense(joan::orig::arg::u32(addsub(static_cast<i32>(jo::self<jo::Unit>(this)->stats[2]), g16(d::g_a3cc1e))));  // 0x004fe740
                joan::orig::as_unit(this)->set_level_45b720(joan::orig::arg::i32(addsub(jo::self<jo::Unit>(this)->hit_points_base, mul(g16(d::g_a3cc1e), g16(d::g_unit_def_hp)))));  // 0x0045b720
                bump_12c(u);
                break;
            case 5:
                joan::orig::as_unit(this)->set_leadership(joan::orig::arg::u32(addsub(static_cast<i32>(jo::self<jo::Unit>(this)->stats[3]), g16(d::g_a3cc10))));  // 0x004fe770
                bump_12c(u);
                break;
            case 6:
                joan::orig::as_unit(this)->set_level_45b720(joan::orig::arg::i32(addsub(jo::self<jo::Unit>(this)->hit_points_base, g16(d::g_a3cc7e))));  // 0x0045b720
                bump_12c(u);
                if (u) {
                    const i32 mx = unit_max_hp(this);
                    if (jo::self<jo::Unit>(this)->hit_points > mx) jo::self<jo::Unit>(this)->hit_points = unit_max_hp(this);
                }
                break;
            case 7:
                joan::orig::as_unit(this)->set_level_45bba0(joan::orig::arg::i32(addsub(jo::self<jo::Unit>(this)->base_max_ep, g16(d::g_a3cc4c))));  // 0x0045bba0
                bump_12c(u);
                if (u) {
                    const i32 mx = unit_max_mana(this);
                    if (jo::self<jo::Unit>(this)->mana > mx) jo::self<jo::Unit>(this)->mana = unit_max_mana(this);
                }
                break;
            case 8: {
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->get(joan::orig::arg::i32(item)));  // 0x005313a0
                jo::as_upgrade_list_entry(e)->level = static_cast<u16>(jo::as_upgrade_list_entry(e)->level + 1);
                jo::view<u16>(jo::self<jo::Unit>(this)->stat_points) = static_cast<u16>(jo::view<u16>(jo::self<jo::Unit>(this)->stat_points) - 1);
                break;
            }
            default: break;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("Unit::has_order_for", 0x00500d40, &H::has_order_for);
B("Unit::spawn_projectile", 0x0050afe0, &H::spawn_projectile);
B("Unit::heal", 0x0050b4f0, &H::heal);
B("Unit::heal_and_mana", 0x0050bcd0, &H::heal_and_mana);
B("Unit::buy_stat", 0x0050d640, &H::buy_stat);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_b88_01.cpp (game/b88_01)
namespace {
namespace game_b88_01 {

using joan::orig::helpers::at;
struct Vec3 {
    float x, y, z;  // raw float bits, copied only
};
static_assert(sizeof(Vec3) == 12, "layout");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kPlayers = obj(d::g_physic_sys);
void* const kCamera = obj(d::g_player_unit_recno);
void* const kRandom = obj(d::g_misc);
void* const kItems = obj(d::g_items);
void* const kSounds = obj(d::g_particle_effect_array);
void* const kEffects = obj(d::g_pemi_res);
void* const kModels = obj(d::g_model_res_mgr);
void* const kTextures = obj(d::g_anim_res_mgr);
constexpr img::Addr kNone(d::s_none);  // "none"

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

bool unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* type_of(const void* unit) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(sx16(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
}
void set_action(void* unit, std::int32_t code, std::int32_t a, std::int32_t b) {
    joan::orig::as_unit(unit)->set_action(joan::orig::arg::i32(code), joan::orig::arg::i32(a), joan::orig::arg::i32(b));  // 0x00501190
}
void play_idle(void* unit) { joan::orig::as_unit(unit)->play_idle(0, 1); }  // 0x00508b90
void reset_state(void* unit, std::int32_t v) { joan::orig::as_unit(unit)->set_213(joan::orig::arg::u32(v)); }  // 0x00500d10
std::int32_t rnd(std::int32_t n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
bool item_invalid(std::int32_t id) { return joan::orig::as_units(kItems)->list.empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00425d50
std::uint8_t* item_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(kItems)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
std::uint8_t* item_type(std::int16_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(sx16(id))));  // 0x00425ce0 (reads the word)
}
std::uint8_t* anim_rec(std::int32_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_refs(kAnims)->get(joan::orig::arg::i16(sx16(static_cast<std::int16_t>(id)))));  // 0x00416f20 (word)
}
std::uint8_t* anim_entry(const void* unit, std::int32_t anim) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(jo::as_unit(unit)->anim_set)->entry(joan::orig::arg::u32(anim)));  // 0x004168d0
}
std::uint8_t* effect_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_name_list(kEffects)->entry(joan::orig::arg::i32(id))); }  // 0x0041b440
std::uint8_t* model_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(kModels)->entry(joan::orig::arg::u32(id))); }  // 0x0041a180
std::uint8_t* texture_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_res_mgr(kTextures)->slot_4174f0(joan::orig::arg::u32(id))); }  // 0x004174f0
void get_pos(void* unit, Vec3* p) { joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(static_cast<void*>(p))); }  // 0x004fedb0
void blood(std::uint32_t a8, std::int32_t ac, std::int32_t a10w, std::int32_t a14, std::int32_t a18w,
           std::int32_t a1cb, Vec3 at_, float size, std::int32_t a30b, std::int32_t a34, std::int32_t a38,
           std::int32_t a3cb, void* a40, std::int32_t a44w) {
    joan::orig::as_triggers(kBlood)->add(joan::orig::arg::u32(a8), joan::orig::arg::u32(ac), joan::orig::arg::i16(a10w), joan::orig::arg::u32(a14), joan::orig::arg::i16(a18w), static_cast<std::int8_t>(joan::orig::arg::i32(a1cb)), at_, joan::orig::arg::bits(size), static_cast<std::int8_t>(joan::orig::arg::i32(a30b)), joan::orig::arg::u32(a34), joan::orig::arg::u32(a38), static_cast<std::int8_t>(joan::orig::arg::i32(a3cb)), a40, joan::orig::arg::i16(a44w));  // 0x004eef00 (ret 0x40)
}
// Sum of the attack-range values of both carried items (0x00425ce0 +0x32), ebp-0x5c pattern.
std::int32_t item_range(const void* unit, std::int16_t jo::Unit::*slot) {
    const std::int16_t id = jo::as_unit(unit)->*slot;
    return jo::as_item_def(item_type(jo::as_map_object(item_get(sx16(id)))->type_id))->i32_32;
}

JOAN_COUNTER(c_set_action, "Unit::set_action");

struct H {
    std::int16_t action() { return jo::self<jo::Unit>(this)->anim_kind; }
    std::int32_t slot(std::int32_t i) { return jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots)[i].value; }
    std::int32_t slot_count_minus_1() {
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::UnitType>(type_of(this))->slot_count) - 1u);
    }
    bool slot_flag2(std::int32_t i) {
        return (static_cast<std::int8_t>(jo::view<std::uint8_t*>(jo::as_unit_type(type_of(this))->buffers[2])[i]) & 2) != 0;
    }
    // Children: forward an action to each flagged child not already in it.
    void children_action(std::int32_t code) {
        for (std::int32_t i = slot_count_minus_1(); i >= 0; --i) {
            if (slot(i) <= 0) continue;
            if (!slot_flag2(i)) continue;
            if (sx16(jo::as_unit(unit_get(slot(i)))->anim_kind) == code) continue;
            set_action(unit_get(slot(i)), code, 0, 1);
        }
    }
    // Children: idle every child that is not busy.
    void children_idle() {
        for (std::int32_t i = slot_count_minus_1(); i >= 0; --i) {
            if (slot(i) <= 0) continue;
            if (sx16(busy(unit_get(slot(i)))) != 0) continue;
            play_idle(unit_get(slot(i)));
        }
    }
    // Rebuild of the +0x321 model object (0x005024ec.. / 0x00503468..); pos = 0x005ac310 arg; rec = anim_rec().
    void build_model(std::uint8_t* rec, void* pos, bool fill7) {
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // operator new (0x10c on x86)
        void* const m = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::self<jo::Unit>(this)->model_b = static_cast<jo::SceneObject*>(m);
        {
            jo::ActorDef* const v = jo::as_model_entry(model_get(jo::as_model_slot(rec)->attach_model))->actor_def;
            jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->def = v;
        }
        if (jo::as_model_slot(rec)->attach_anim != 0) {
            jo::AnimDef* const v = jo::self<jo::AnimSlot>(texture_get(jo::as_model_slot(rec)->attach_anim))->anim;
            jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->anim = v;
        }
        joan::orig::as_scene_object(jo::self<jo::Unit>(this)->model_b)->init();  // 0x005aab70
        {
            const std::int32_t v = jo::as_model_entry(model_get(jo::as_model_slot(rec)->attach_model))->anim_db_id;
            joan::orig::as_actor(jo::self<jo::Unit>(this)->model_b)->set_skin(joan::orig::arg::u32(v));  // 0x005add40
        }
        joan::orig::as_actor(jo::self<jo::Unit>(this)->model_b)->set_position_5ac310(joan::orig::arg::mptr(pos));                    // 0x005ac310
        joan::orig::as_scene_object(jo::self<jo::Unit>(this)->model_b)->set_yaw(joan::orig::arg::u32(jo::view<std::int32_t>(jo::self<jo::Unit>(this)->heading)));  // 0x005ac6a0
        jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->kind = 0x10;
        jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->owner = this;
        if (fill7) {
            for (std::int32_t i = 0; i < jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->plane->material_count; ++i) {
                std::uint8_t* const arr = jo::view<std::uint8_t*>(jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->plane->materials);
                jo::view<std::int32_t>(jo::as_material(arr)[i].blend) = 7;
            }
        }
        if (jo::self<jo::Unit>(this)->model_b != nullptr && jo::self<jo::Unit>(this)->scene != nullptr) {
            joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->model_b));  // 0x005efc50
        }
        if (jo::as_model_slot(rec)->attach_anim != 0) {
            joan::orig::as_actor(jo::self<jo::Unit>(this)->model_b)->start_anim(joan::orig::arg::mptr(jo::as_scene_object(jo::self<jo::Unit>(this)->model_b)->anim), joan::orig::arg::str(static_cast<void*>(jo::as_model_slot(rec)->anim_name)), joan::orig::arg::u32(time_now()), 0u, 0u, 0u);  // 0x005abe30
        }
    }
    // Attached effect for a weapon hand (0x0050225a.. / 0x0050235e..): emitter slot e (2 / 3) of rec = anim_rec().
    void hand_effect(std::uint8_t* rec, std::int32_t e_slot) {
        jo::ModelSlot* const r = jo::as_model_slot(rec);
        std::uint8_t* item = nullptr;  // ebp-0x44 / ebp-0x48
        if (r->emitter_sel[e_slot] == 0 && !item_invalid(sx16(jo::self<jo::Unit>(this)->carried_a))) {
            item = item_get(sx16(jo::self<jo::Unit>(this)->carried_a));
        } else if (r->emitter_sel[e_slot] == 1 && !item_invalid(sx16(jo::self<jo::Unit>(this)->carried_b))) {
            item = item_get(sx16(jo::self<jo::Unit>(this)->carried_b));
        }
        if (item == nullptr || jo::as_map_object(item)->actor == nullptr) return;
        const std::int32_t b = r->emitter_bytes[e_slot];
        const std::int32_t v15d = r->emitter_b[e_slot];
        const std::int32_t v145 = r->emitter_a[e_slot];
        const void* const e = jo::as_pemi_script(effect_get(r->emitter_txt_ids[e_slot]))->effect;  // PemiScript::effect
        joan::orig::as_map_object(item)->play_sound(e, static_cast<const void*>(&r->emitter_pos[e_slot * 3]), 0u, joan::orig::arg::u32(v145), joan::orig::arg::u32(v15d), joan::orig::arg::u32(b));  // 0x004991e0 (ret 0x18)
    }
    void fade_timers(std::int32_t anim) {
        if (jo::self<jo::Unit>(this)->trail_a != nullptr) {
            const std::int32_t t = ftol(joan::orig::arg::as<double>(joan::orig::as_unit(this)->link_value(joan::orig::arg::i32(anim))));  // 0x00504800
            joan::orig::as_trail(jo::self<jo::Unit>(this)->trail_a)->deform_update(joan::orig::arg::i32(t));                // 0x00687500
        }
        if (jo::self<jo::Unit>(this)->trail_b != nullptr) {
            const std::int32_t t = ftol(joan::orig::arg::as<double>(joan::orig::as_unit(this)->link_value(joan::orig::arg::i32(anim))));  // 0x00504800
            joan::orig::as_trail(jo::self<jo::Unit>(this)->trail_b)->deform_update(joan::orig::arg::i32(t));                // 0x00687500
        }
    }

    // ------------------------------------------------------------------------ 0x00501190
    void set_action_(std::int32_t code_arg, std::int32_t push_arg, std::int32_t /*unused*/) {
        c_set_action.hit();
        // The original passes &arg (its word) to the three filters; they only read / write a word.
        std::int16_t w = static_cast<std::int16_t>(code_arg);
        auto code = [&]() -> std::int32_t { return sx16(w); };
        const std::int8_t push = static_cast<std::int8_t>(push_arg);

        joan::orig::as_unit(this)->filter_command(joan::orig::arg::mptr(static_cast<void*>(&w)));  // 0x0048b460
        joan::orig::as_unit(this)->on_event(joan::orig::arg::mptr(static_cast<void*>(&w)));  // 0x00459970
        joan::orig::as_unit(this)->on_command_nop(static_cast<void*>(&w));  // 0x00518d10

        if (code() != 0xb && global<std::int32_t>(d::g_player_unit_recno) == sx16(jo::self<jo::Unit>(this)->id) &&
            sx16(jo::self<jo::Unit>(this)->layer) == 0) {
            joan::orig::as_player(kCamera)->set_a8(0u);  // 0x00503fe0
        }
        {
            const std::int32_t c = code();
            if (c == 0x23 || c == 0x24 || c == 0x25 || c == 0x26 || c == 0x29 || c == 0x27 || c == 0x28 ||
                c == 0x2a || c == 0x3d || c == 0x40 || c == 0x41 || c == 0x42 || c == 0x4a || c == 0x4b ||
                c == 0x4c || c == 0x4d) {
                if (!unit_invalid(jo::self<jo::Unit>(this)->weapon)) {
                    {
                        std::uint8_t* const carrier = unit_get(jo::self<jo::Unit>(this)->weapon);
                        jo::as_arr_i32(jo::as_unit(carrier)->mask_slots)[jo::self<jo::Unit>(this)->weapon_slot].value = 0;
                    }
                    {
                        std::uint8_t* const carrier = unit_get(jo::self<jo::Unit>(this)->weapon);
                        jo::as_arr_i32(jo::as_unit(carrier)->mask_slots_b)[jo::self<jo::Unit>(this)->weapon_slot].value = 0;
                    }
                    play_idle(unit_get(jo::self<jo::Unit>(this)->weapon));
                    jo::self<jo::Unit>(this)->weapon = 0;
                    jo::self<jo::Unit>(this)->weapon_slot = 0;
                    jo::self<jo::Unit>(this)->slot_index = -1;
                }
                reset_state(this, 0);
            }
        }

        switch (sx16(jo::self<jo::UnitType>(type_of(this))->kind)) {
            case 4:
            case 5:
            case 6:
            case 9:
            case 10:
                if (code() == 0x30) {
                    children_action(0x31);
                } else if (code() == 0x36 || code() == 0x39) {
                    children_action(0x37);
                } else if (code() == 0x33 || code() == 0x38) {
                    children_action(0x35);
                } else if (code() == 1 || code() == 2 || code() == 3 || code() == 4) {
                    children_idle();
                }
                break;
            case 11:
                if (code() == 0x1c) {
                    for (std::int32_t i = slot_count_minus_1(); i >= 0; --i) {
                        if (slot(i) <= 0) continue;
                        if (slot_flag2(i)) {
                            if (sx16(jo::as_unit(unit_get(slot(i)))->anim_kind) != 0x58) {
                                set_action(unit_get(slot(i)), 0x58, 0, 1);
                            }
                        } else {
                            if (sx16(busy(unit_get(slot(i)))) == 0) play_idle(unit_get(slot(i)));
                        }
                    }
                } else if (code() == 1 || code() == 2 || code() == 3 || code() == 4) {
                    children_idle();
                }
                break;
            default:
                break;
        }

        // 0x005018c3
        jo::self<jo::Unit>(this)->u8_373 = 0;
        if (code() != 0x1c && code() != 0x49) joan::orig::as_unit(this)->clear_flags();  // 0x0050d0d0
        if (sx16(jo::self<jo::Unit>(this)->id) == global<std::int32_t>(d::g_player_unit_recno) && code() != 0x1c && code() != 0x49 &&
            code() != 0x1b) {
            joan::orig::as_player(kCamera)->set_9d();  // 0x00503fb0
        }
        {
            bool hit = false;
            const std::int32_t c = code();
            if (c == 0xa || c == 0xb || c == 0x44 || c == 0x46 || c == 0x45) {
                hit = true;
            } else {
                const std::int32_t a = sx16(action());
                hit = a == 0x1c || a == 0x1d || a == 0x1b || a == 0x23 || a == 0x24 || a == 0x25 || a == 0x1e ||
                      a == 0x1f || a == 0x20 || a == 0x21 || a == 0x3a || a == 0x3b || a == 0x3c || a == 0x53 ||
                      a == 0x22 || a == 0x15 || a == 0x16 || a == 0x17;
            }
            if (hit && jo::self<jo::Unit>(this)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0 &&
                joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->mounted()) == 0 &&  // 0x00430190
                joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->get_1f5()) == 0 &&  // 0x004f2f60
                unit_invalid(jo::self<jo::Unit>(this)->weapon) && sx16(action()) != 0x51 && sx16(action()) != 0x52 &&
                joan::orig::arg::as<std::int32_t>(joan::orig::as_world(this)->flag_1fd()) == 0 &&  // 0x0042d690
                jo::view<std::int32_t>(jo::self<jo::Unit>(this)->scripted_move) == 0) {
                joan::orig::as_players(kPlayers)->set_active(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::i16(1));  // 0x006ad980
            }
        }
        // 0x00501b2f
        if (sx16(action()) == 0x2a && jo::self<jo::Unit>(this)->formation_slot != 1) return;
        auto busy_return = [&]() -> bool {  // 0x00501be4 / 0x00501c8b
            if (code() != 0x2a && jo::self<jo::Unit>(this)->fell_timer > 0 &&
                global<std::int32_t>(d::g_player_unit_recno) != sx16(jo::self<jo::Unit>(this)->id) && sx16(jo::self<jo::Unit>(this)->layer) != 0) {
                return true;
            }
            return code() == 0x2a;
        };
        {
            const std::int32_t a = sx16(action());
            if (a == 0x40 || a == 0x41 || a == 0x42 || a == 0x4e) {
                const std::int32_t c = code();
                if (c == 0x29) {
                } else if (c == 0x23 || c == 0x27 || c == 0x28 || c == 0x26 || c == 0x40 || c == 0x41 || c == 0x42) {
                    w = 0x29;
                } else if (busy_return()) {
                    return;
                }
            }
        }
        // 0x00501c28
        if (sx16(action()) == 0x29) {
            const std::int32_t c = code();
            if (c == 0x23 || c == 0x27 || c == 0x28 || c == 0x26 || c == 0x40 || c == 0x41 || c == 0x42 || c == 0x29) {
                w = 0x29;
            } else if (busy_return()) {
                return;
            }
        }
        // 0x00501ccf
        {
            const std::int32_t a = sx16(action());
            if (a == 0x23 || a == 0x24 || a == 0x25 || a == 0x26 || a == 0x27 || a == 0x28) {
                const std::int32_t c = code();
                if (c != 0x2a && c != 0x23 && c != 0x24 && c != 0x25 && c != 0x27 && c != 0x28 && c != 0x26 &&
                    c != 0x40 && c != 0x41 && c != 0x42 &&
                    sx16(jo::self<jo::UnitType>(type_of(this))->unit_class) != 8) {
                    const double pos = joan::orig::arg::as<double>(joan::orig::as_unit(this)->anim_phase());  // 0x005045f0
                    if (pos > static_cast<double>(global<float>(d::k_9cf5c4))) return;
                }
            }
        }
        // 0x00501dc0
        if (code() == 0x23) {
            if (sx16(action()) == 0x23) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(0x24)) != 0) w = 0x24;  // 0x00416dc0
            } else if (sx16(action()) == 0x24) {
                w = 0x23;
            } else if (sx16(action()) == 0x25) {
                w = static_cast<std::int16_t>(rnd(2) != 0 ? 0x23 : 0x24);
            } else if (jo::self<jo::Unit>(this)->anim_set != nullptr &&
                       joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(0x24)) != 0) {  // 0x00416dc0
                w = static_cast<std::int16_t>(rnd(2) != 0 ? 0x23 : 0x24);
            }
        }
        // 0x00501e6e
        if (sx16(action()) != code()) joan::orig::as_unit(this)->play_step_sounds(joan::orig::arg::i32(jo::self<jo::Unit>(this)->anim_entry), 0u);  // 0x0050f570
        jo::self<jo::Unit>(this)->prev_action = jo::self<jo::Unit>(this)->anim_kind;
        jo::self<jo::Unit>(this)->anim_kind = w;
        {
            const std::int32_t idx = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->combo_state));
            jo::view<std::int8_t>(jo::self<jo::Unit>(this)->armour_flags[idx]) = push;
            if (push != 0) jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->combo_state) = static_cast<std::uint8_t>(jo::view<std::uint8_t>(jo::self<jo::Unit>(this)->combo_state) + 1);
        }
        // 0x00501ee6
        {
            const std::int32_t c = code();
            if (c != 0x30 && c != 0x32 && c != 0x33 && c != 0x36 && c != 0x38 && c != 0x39 &&
                !item_invalid(sx16(jo::self<jo::Unit>(this)->carried_a)) &&
                joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->slot_e()) != nullptr) {  // 0x00497580
                const std::int32_t a = sx16(jo::as_map_object(item_get(sx16(jo::self<jo::Unit>(this)->carried_a)))->type_id);
                const std::int16_t* const b = joan::orig::arg::as<std::int16_t*>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->slot_e());  // 0x00497580
                if (a == sx16(*b)) {
                    const std::int16_t item = jo::self<jo::Unit>(this)->carried_a;  // ebp-0x2c
                    joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));            // 0x0050a480
                    if (!item_invalid(sx16(item))) joan::orig::as_units(kItems)->remove_49f370(joan::orig::arg::i32(sx16(item)));  // 0x0049f370
                }
            }
        }
        // 0x00501fd1
        if (code() == 0x1a && sx16(jo::self<jo::Unit>(this)->action) == 0) reset_state(this, 1);
        if (code() == 0x18) {
            const std::int32_t s = sx16(jo::self<jo::Unit>(this)->action);
            if (s >= 0 && s <= 1) reset_state(this, 0);
            if (sx16(jo::self<jo::Unit>(this)->prev_action) == 0x1d || sx16(jo::self<jo::Unit>(this)->prev_action) == 8) {
                jo::self<jo::Unit>(this)->anim_kind = 0x19;
                w = 0x19;
            }
        }
        if (code() == 1 && sx16(jo::self<jo::Unit>(this)->prev_action) == 0x1d) {
            jo::self<jo::Unit>(this)->anim_kind = 8;
            w = 8;
        }
        if (code() == 0x40 || code() == 0x41 || code() == 0x42 || code() == 0x4e) {
            const std::int32_t q = global<std::int32_t>(d::g_a3cc88) / 5;
            const std::int32_t r = rnd(global<std::int32_t>(d::g_a3cc88));
            jo::self<jo::Unit>(this)->fell_timer = add32(q, r);
            if (jo::self<jo::Unit>(this)->rank >= 4 && jo::self<jo::Unit>(this)->hit_value < global<std::int32_t>(d::g_a3cbe4)) {
                jo::self<jo::Unit>(this)->hit_value = global<std::int32_t>(d::g_a3cbe4);
            }
        }
        if (sx16(jo::self<jo::Unit>(this)->prev_action) == 0x3b) jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = 3;

        // 0x0050212d
        const std::int32_t anim = joan::orig::arg::as<std::int32_t>(joan::orig::as_cell_object(this)->find_anim_state());  // 0x00504010, ebp-0x30
        if (anim == 0) return;
        joan::orig::as_unit(this)->play_anim_sounds(joan::orig::arg::i32(anim), 0);  // 0x0050db80
        const std::int32_t sound = jo::as_anim_group_rec(anim_entry(this, anim))->model_ref;  // ebp-0x34
        if (sound != 0) {
            std::uint8_t* const rec = anim_rec(sound);  // ebp-0x38
            if (jo::as_model_slot(rec)->emitter_flag != 0) {
                if (stricmp_(&jo::as_model_slot(rec)->emitter_names[0 * 33], img::str(kNone)) != 0) {
                    (void)joan::orig::arg::as<std::int32_t>(joan::orig::as_spawner(kSounds)->try_spawn(joan::orig::arg::i32(sound), jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene, 0, joan::orig::arg::i32(sx16(jo::self<jo::Unit>(this)->id))));  // 0x0041b640
                }
                if (stricmp_(&jo::as_model_slot(rec)->emitter_names[1 * 33], img::str(kNone)) != 0) {
                    const std::int32_t s = joan::orig::arg::as<std::int32_t>(joan::orig::as_spawner(kSounds)->try_spawn(joan::orig::arg::i32(sound), jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene, 1, joan::orig::arg::i32(sx16(jo::self<jo::Unit>(this)->id))));  // 0x0041b640
                    if (s != 0) {
                        void* const snd = joan::orig::as_dynarray(kSounds)->ptr_at(joan::orig::arg::i32(s));  // 0x0041bd60
                        joan::orig::as_effect(snd)->set_owner(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), joan::orig::arg::mptr(0));              // 0x0041b0d0
                    }
                }
                if (stricmp_(&jo::as_model_slot(rec)->emitter_names[2 * 33], img::str(kNone)) != 0) hand_effect(rec, 2);
                if (stricmp_(&jo::as_model_slot(rec)->emitter_names[3 * 33], img::str(kNone)) != 0) hand_effect(rec, 3);
            }
            // 0x00502443
            if (jo::as_model_slot(rec)->attach_model != 0 && static_cast<std::int8_t>(jo::as_model_slot(rec)->u8_202) != 0x57) {
                if (jo::self<jo::Unit>(this)->model_b != nullptr) {
                    void* const m = jo::self<jo::Unit>(this)->model_b;
                    if (jo::view<void*>(jo::as_scene_object(m)->scene) != nullptr) joan::orig::as_scene(jo::view<void*>(jo::as_scene_object(m)->scene))->detach(joan::orig::arg::mptr(m));  // 0x005efdb0
                    void* const d = jo::self<jo::Unit>(this)->model_b;
                    if (d != nullptr) (void)joan::orig::arg::as<void*>(joan::orig::as_scene_object(d)->destroy(1u));  // 0x00420840
                    jo::self<jo::Unit>(this)->model_b = nullptr;
                }
                build_model(rec, jo::self<jo::Unit>(this)->position_, true);
            }
        }

        // 0x005026d3
        {
            const std::int32_t c = code();
            if (c == 0x1c || c == 0x1d || c == 8 || c == 0x19 || c == 0x1e || c == 0x1f || c == 0x20 || c == 0x21 ||
                c == 0x49 || c == 0x53 || c == 0x46 || c == 0x44) {
                Vec3 pos;  // ebp-0x58
                get_pos(this, &pos);
                std::int32_t range = 0;   // ebp-0x5c
                std::int32_t range2 = 0;  // ebp-0x60
                if (!item_invalid(sx16(jo::self<jo::Unit>(this)->carried_a))) range = item_range(this, &jo::Unit::carried_a);
                if (!item_invalid(sx16(jo::self<jo::Unit>(this)->carried_b))) range2 = item_range(this, &jo::Unit::carried_b);
                if (range < range2) range = range2;
                if (code() == 0x49 && !unit_invalid(jo::self<jo::Unit>(this)->mount_id)) {
                    std::uint8_t* const target = unit_get(jo::self<jo::Unit>(this)->mount_id);  // ebp-0x64
                    if (target != nullptr && sx16(jo::as_unit(target)->anim_kind) == 0xb) {
                        const std::int32_t saved = sx16(action());  // ebp-0x68
                        jo::self<jo::Unit>(this)->anim_kind = 0x46;
                        const std::int32_t anim2 = joan::orig::arg::as<std::int32_t>(joan::orig::as_cell_object(this)->find_anim_state());  // 0x00504010
                        jo::self<jo::Unit>(this)->anim_kind = static_cast<std::int16_t>(saved);
                        if (!item_invalid(sx16(jo::self<jo::Unit>(this)->carried_a))) range = item_range(this, &jo::Unit::carried_a);
                        if (!item_invalid(sx16(jo::self<jo::Unit>(this)->carried_b))) range2 = item_range(this, &jo::Unit::carried_b);
                        if (range < range2) range = range2;
                        if (jo::self<jo::Unit>(this)->actor != nullptr) {
                            const std::int32_t id = jo::as_anim_group_rec(anim_entry(this, anim2))->model_ref;  // ebp-0x70
                            const std::int32_t idw = sx16(static_cast<std::int16_t>(id));
                            for (std::int32_t k = 1; k <= 2; ++k) {
                                const std::int32_t r1 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                                const std::int32_t r2 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                                const std::int32_t r3 = add32(jo::as_model_slot(anim_rec(id))->times[2], range);
                                blood(1, k, 0, 1, sx16(jo::self<jo::Unit>(this)->id), 2, pos, static_cast<float>(r3), 1, r2, r1,
                                      1, 0, idw);
                            }
                        }
                    }
                }
                // 0x00502a69
                if (jo::self<jo::Unit>(this)->actor != nullptr) {
                    const std::int32_t id = jo::as_anim_group_rec(anim_entry(this, anim))->model_ref;  // ebp-0x74
                    for (std::int32_t k = 1; k <= 2; ++k) {
                        const std::int32_t r1 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                        const std::int32_t r2 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                        const std::int32_t r3 = add32(jo::as_model_slot(anim_rec(id))->times[2], range);
                        blood(1, k, 0, 1, sx16(jo::self<jo::Unit>(this)->id), 1, pos, static_cast<float>(r3), 1, r2, r1, 1, 0,
                              0);
                    }
                } else {
                    for (std::int32_t k = 1; k <= 2; ++k) {
                        const std::int32_t g1 = time_now();
                        const std::int32_t g2 = time_now();
                        const float size = static_cast<float>(add32(range, 0x32));
                        blood(1, k, 0, 1, sx16(jo::self<jo::Unit>(this)->id), 1, pos, size, 1, g2, g1, 1, 0, 0);
                    }
                    const std::int32_t g1 = time_now();
                    const std::int32_t g2 = time_now();
                    blood(0x200, 1, 0, 1, sx16(jo::self<jo::Unit>(this)->id), 1, pos, 100.0f, 0, g2, g1, 1, 0, 0);
                }
                fade_timers(anim);
            } else if (code() == 0x22) {
                Vec3 pos;  // ebp-0x80
                get_pos(this, &pos);
                std::int32_t range = 0;         // ebp-0x84
                const std::int32_t range2 = 0;  // ebp-0x88 (never written: original bug)
                if (!item_invalid(sx16(jo::self<jo::Unit>(this)->carried_a))) range = item_range(this, &jo::Unit::carried_a);
                if (!item_invalid(sx16(jo::self<jo::Unit>(this)->carried_b))) range = item_range(this, &jo::Unit::carried_b);
                if (range < range2) range = range2;
                if (jo::self<jo::Unit>(this)->actor != nullptr) {
                    const std::int32_t id = jo::as_anim_group_rec(anim_entry(this, anim))->model_ref;  // ebp-0x8c
                    const std::int32_t r1 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                    const std::int32_t r2 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                    const float size = static_cast<float>(jo::as_model_slot(anim_rec(id))->times[2]);
                    blood(0x400, 1, 0, 1, sx16(jo::self<jo::Unit>(this)->id), 1, pos, size, 0, r2, r1, 1, 0, 0);
                } else {
                    const std::int32_t g1 = time_now();
                    const std::int32_t g2 = time_now();
                    const float size = static_cast<float>(add32(range, 0x32));
                    blood(0x400, 1, 0, 1, sx16(jo::self<jo::Unit>(this)->id), 1, pos, size, 0, g2, g1, 1, 0, 0);
                }
                fade_timers(anim);
            }
        }
        // 0x00502f62
        if (code() == 0x3a) {
            Vec3 pos;  // ebp-0x98
            get_pos(this, &pos);
            if (jo::self<jo::Unit>(this)->actor != nullptr) {
                const std::int32_t id = jo::as_anim_group_rec(anim_entry(this, anim))->model_ref;  // ebp-0x9c
                const std::int32_t r1 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                const std::int32_t r2 = add32(jo::as_model_slot(anim_rec(id))->times[0], time_now());
                const float size = static_cast<float>(jo::as_model_slot(anim_rec(id))->times[2]);
                blood(0x10, 1, sx16(jo::self<jo::Unit>(this)->target_building), 1, sx16(jo::self<jo::Unit>(this)->id), 1, pos, size, 0, r2, r1, 1,
                      0, 0);
            } else {
                const std::int32_t g1 = time_now();
                const std::int32_t g2 = time_now();
                blood(0x10, 1, sx16(jo::self<jo::Unit>(this)->target_building), 1, sx16(jo::self<jo::Unit>(this)->id), 1, pos, 50.0f, 0, g2, g1,
                      1, 0, 0);
            }
        }
        // 0x005030a5
        if (code() == 0x1a && sx16(jo::self<jo::Unit>(this)->action) == 1) {
            if (jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) {
                const std::int32_t h = joan::orig::arg::as<std::int32_t>(joan::orig::as_army(kPlayers)->fire_shot(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), joan::orig::arg::bits(60.0f), joan::orig::arg::u32(sx16(jo::self<jo::Unit>(this)->layer)), 0u, 2u, 1u));  // 0x006adbf0
                jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) = h;
            }
            if (jo::self<jo::Unit>(this)->mount_id == 0) {
                void* const p = joan::orig::as_players(kPlayers)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)));  // 0x006af310
                const std::int32_t t = add32(time_now(), jo::view<std::int32_t>(jo::self<jo::Unit>(this)->combo_min_time));
                joan::orig::as_body(p)->init_target(joan::orig::arg::mptr(static_cast<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->velocity)), joan::orig::arg::u32(t), 0x50u);  // 0x006b11a0
            } else if (!unit_invalid(jo::self<jo::Unit>(this)->mount_id)) {
                std::uint8_t* const other = unit_get(jo::self<jo::Unit>(this)->mount_id);  // ebp-0xa4
                void* const p =
                    joan::orig::as_players(kPlayers)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(other)->actor)->record)));  // 0x006af310
                const std::int32_t t = add32(time_now(), jo::view<std::int32_t>(jo::self<jo::Unit>(this)->combo_min_time));
                joan::orig::as_body(p)->init_target(joan::orig::arg::mptr(static_cast<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->velocity)), joan::orig::arg::u32(t), 0x50u);  // 0x006b11a0
            }
        }
        // 0x005031db
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(this)->dead())) == 0 &&  // 0x0042d0e0
            sx16(busy(this)) == 0) {
            std::uint8_t* const track =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_waters(kTracks)->find_at(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Unit>(this)->position_)), 0));  // 0x00469d10
            // `track`: a Water (Waters::find_at 0x00469d10); box[2] = the surface height
            if (track == nullptr || jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[2] >= jo::as_water(track)->box[2]) {
                jo::self<jo::Unit>(this)->in_water = 0;
            } else {
                std::uint8_t* const type = type_of(this);  // ebp-0xb0
                const float scale = code() == 0xb ? jo::self<jo::UnitType>(type)->speed_run : jo::self<jo::UnitType>(type)->speed_walk;
                const float limit = static_cast<float>(global<std::int16_t>(d::g_a3ccc8)) * scale;  // ebp-0xb4
                bool skip = false;
                if (sx16(jo::view<std::int16_t[2]>(jo::as_water(track)->bytes_0)[0]) == jo::self<jo::Unit>(this)->in_water) {
                    Vec3 tmp;  // ebp-0xf4
                    Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(static_cast<void*>(&tmp), static_cast<void*>(jo::self<jo::Unit>(this)->position_), static_cast<void*>(jo::self<jo::Unit>(this)->splash_pos)));  // 0x0060cc90
                    const double len = import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&d));  // length
                    if (!(len >= static_cast<double>(limit)) && code() != 0x1a && code() != 0x18) skip = true;
                }
                if (!skip) {
                    // 0x00503328
                    Vec3 at_;  // ebp-0xc0
                    at_.x = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0];
                    at_.y = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1];
                    at_.z = jo::view<float>(jo::as_water(track)->box[2]);
                    std::int32_t n = jo::view<std::int32_t>(jo::as_water(track)->u32_2c);  // ebp-0xc4
                    if (jo::view<std::int32_t>(jo::as_water(track)->u32_2c) > 1) n = jo::view<std::int32_t>(jo::as_water(track)->u32_2c) - 1;
                    // arr_24[i]: i <= 1 while count <= 3 (Water::add_sound 0x00546bf0 appends at +0x24 + count * 4)
                    for (std::int32_t i = 0; i < n; ++i) {
                        void* const e = jo::self<jo::PemiScript>(effect_get(jo::view<std::int32_t>(jo::as_water(track)->arr_24[i])))->effect;
                        joan::orig::as_model_list(kSounds)->add(jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene, e, static_cast<const void*>(&at_), static_cast<const void*>(obj(d::g_pos)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), 0, 1, 0x32);  // 0x0041baf0
                    }
                    std::memcpy(jo::self<jo::Unit>(this)->splash_pos, jo::self<jo::Unit>(this)->position_, 12);
                    if (jo::self<jo::Unit>(this)->model_b == nullptr && sound != 0) {
                        std::uint8_t* const rec = anim_rec(sound);  // ebp-0xcc
                        if (jo::as_model_slot(rec)->attach_model != 0 && static_cast<std::int8_t>(jo::as_model_slot(rec)->u8_202) == 0x57) {
                            build_model(rec, &at_, false);
                        }
                    }
                }
                jo::self<jo::Unit>(this)->in_water = sx16(jo::view<std::int16_t[2]>(jo::as_water(track)->bytes_0)[0]);
            }
        }
        jo::self<jo::Unit>(this)->mount_busy = 1;
    }
};

JOAN_REPLACE("game/b88_01", "Unit::set_action", 0x00501190, &H::set_action_);

}  // namespace game_b88_01
}  // namespace

// ---------------------------------------- from modules/game_b106_01.cpp (game/b106_01)
namespace {
namespace game_b106_01 {

using u32 = std::uint32_t;

using joan::orig::helpers::fbits;
u32 bitsf(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}

JOAN_COUNTER(c_pick_target, "Unit::pick_target_503680");

// ---- 0x00503680 helpers ----
void* const kUnits = obj(d::g_items);
i32 unit_gone(i32 id) { return joan::orig::as_units(kUnits)->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
i32 random100() { return joan::orig::as_misc(obj(d::g_misc))->random(i32{100}); }  // 0x00590540

// 0x0060cc90 (cdecl out, a, b) into a temporary, copied to a second local, then length_sq.
double length_sq_of(const void* a, const void* b) {
    float tmp[3];
    float* const r = joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(tmp), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));  // 0x0060cc90
    float d[3];
    std::memcpy(d, r, 12);
    return import_call<double>(d::iat_v3d_87_length_sq, static_cast<const void*>(d));  // length_sq (st0)
}
float length_sq_f(const void* a, const void* b) {
    float tmp[3];
    float* const r = joan::orig::arg::as<float*>(joan::orig::vec_sub(joan::orig::arg::mptr(tmp), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));  // 0x0060cc90
    float d[3];
    std::memcpy(d, r, 12);
    return v3d_float(d::iat_v3d_87_length_sq, static_cast<const void*>(d));  // fstp dword
}

struct H {
    // ------------------------------------------------------------------ 0x00503680
    // Keep-current-target checks for mode[0] == 'E' (returns true with *out set when the
    // original returns the current id from there).
    bool keep_current(const char* mode, i32 nn, const float* pos, i32* out) {
        if (mode[1] == 'G') {
            u8* const t = unit_get(static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id));
            const double ls = length_sq_of(pos, jo::as_map_object(t)->pos);
            if (static_cast<double>(nn) > ls && static_cast<i32>(jo::as_map_object(t)->class_id) == 1) {
                u8* const r = virtual_call<u8*>(t, 0x4c);
                // MapObject virtual +0x4c returns the Gate; x64: class unknown for the +0x58 result (+0xc6 i32)
                if (mode[2] == 'C' && jo::as_gate(r)->hp > 0) {
                    if (static_cast<i32>(jo::as_gate(r)->state) == 0) {
                        *out = static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id);
                        return true;
                    }
                } else if (mode[2] == 'O' && static_cast<i32>(jo::as_gate(r)->state) == 1) {
                    *out = static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id);
                    return true;
                }
            }
        } else if (mode[1] == 'C') {
            u8* const t = unit_get(static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id));
            if (static_cast<i32>(jo::as_map_object(t)->class_id) == 1) {
                const double ls = length_sq_of(pos, jo::as_map_object(t)->pos);
                if (static_cast<double>(nn) > ls) {
                    u8* const r = virtual_call<u8*>(t, 0x4c);
                    if (static_cast<i32>(jo::as_gate(r)->state) == 0 && jo::as_gate(r)->hp > 0) {
                        *out = static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id);
                        return true;
                    }
                }
            } else if (static_cast<i32>(jo::as_map_object(t)->class_id) == 4) {
                const double ls = length_sq_of(pos, jo::as_map_object(t)->pos);
                if (static_cast<double>(nn) > ls) {
                    u8* const r = virtual_call<u8*>(t, 0x58);
                    if (jo::view<i32>(jo::as_wall(r)->u32_c6) > 0) {
                        *out = static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id);
                        return true;
                    }
                }
            }
        } else {
            u8* const t = unit_get(static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id));
            const double ls = length_sq_of(pos, jo::as_map_object(t)->pos);
            if (static_cast<double>(nn) > ls && random100() <= 0x5f) {
                if (static_cast<i32>(jo::as_map_object(t)->class_id) == 4) {
                    u8* const r = virtual_call<u8*>(t, 0x58);
                    if (jo::view<i32>(jo::as_wall(r)->u32_c6) > 0) {
                        *out = static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id);
                        return true;
                    }
                } else {
                    *out = static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id);
                    return true;
                }
            }
        }
        return false;
    }

    // Filter of one candidate t (kind record k) for the scan; false = skip it.
    bool candidate_ok(const char* mode, u8* t, const i16* k) {
        if (mode[0] == 'E') {
            if (static_cast<i32>(*k) != 1 && static_cast<i32>(*k) != 4 && static_cast<i32>(*k) != 0x12)
                return false;
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->relation_to_side(joan::orig::arg::i16(jo::as_map_object(t)->side))) == 0) return false;  // 0x00509590
            if (mode[1] == 'G') {
                if (static_cast<i32>(*k) != 1) return false;
                u8* const r = virtual_call<u8*>(t, 0x4c);
                if (mode[2] == 'C') {
                    if (static_cast<i32>(jo::as_gate(r)->state) != 0) return false;
                    if (!(jo::as_gate(r)->hp > 0)) return false;
                } else if (mode[2] == 'O') {
                    if (static_cast<i32>(jo::as_gate(r)->state) != 1) return false;
                }
            }
            if (mode[1] == 'C') {
                if (static_cast<i32>(*k) != 1 && static_cast<i32>(*k) != 4) return false;
                if (static_cast<i32>(*k) == 1) {
                    u8* const r = virtual_call<u8*>(t, 0x4c);
                    if (static_cast<i32>(jo::as_gate(r)->state) != 0) return false;
                    if (!(jo::as_gate(r)->hp > 0)) return false;
                } else if (static_cast<i32>(*k) == 4) {
                    u8* const r = virtual_call<u8*>(t, 0x58);
                    if (jo::view<i32>(jo::as_wall(r)->u32_c6) == 0) return false;
                }
            }
            if (mode[1] == 'W') {
                if (static_cast<i32>(*k) != 4) return false;
                u8* const r = virtual_call<u8*>(t, 0x58);
                if (jo::view<i32>(jo::as_wall(r)->u32_c6) == 0) return false;
            }
            if (mode[1] == 'K') {
                if (static_cast<i32>(*k) != 0x12) return false;
            }
        } else if (mode[0] == 'F') {
            if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(this)->is_ally_of(joan::orig::arg::i16(jo::as_map_object(t)->side)))) == 0) return false;  // 0x00509670
            if (mode[1] == 'G' && static_cast<i32>(*k) != 1) return false;
            if (mode[1] == 'W' && static_cast<i32>(*k) != 4) return false;
        }
        return true;
    }

    i32 pick_target_503680(const char* mode, i32 range, i32 from_leader) {
        c_pick_target.hit();
        if (jo::self<jo::Unit>(this)->world == nullptr) return 0;
        if (mode[0] == 'T') {
            if (unit_gone(static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id)) == 0) return static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id);
            return 0;
        }
        void* const leader = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->leader());  // 0x004216d0
        float pos[3];
        if (from_leader != 0 && leader != nullptr && leader != static_cast<void*>(this)) {
            std::memcpy(pos, jo::as_unit(leader)->position_, 12);
        } else {
            std::memcpy(pos, jo::self<jo::Unit>(this)->position_, 12);
            from_leader = 0;
        }
        const i32 nn = static_cast<i32>(static_cast<u32>(range) * static_cast<u32>(range));
        if (unit_gone(static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id)) == 0 && mode[0] == 'E') {
            u8* const cur = unit_get(static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id));
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->relation_to_side(joan::orig::arg::i16(jo::as_map_object(cur)->side))) != 0) {  // 0x00509590
                i32 id = 0;
                if (keep_current(mode, nn, pos, &id)) return id;
            }
        }

        // Grid scan.
        u32 best_bits = 0xbf800000u;  // -1.0f
        i32 result = 0;
        const Fpu fpu{true};
        const double r = static_cast<double>(range);
        float box[6];
        box[0] = static_cast<float>(fpu.sub(pos[0], r));
        box[1] = static_cast<float>(fpu.sub(pos[1], r));
        box[2] = static_cast<float>(fpu.sub(pos[2], r));
        box[3] = static_cast<float>(fpu.add(r, pos[0]));
        box[4] = static_cast<float>(fpu.add(r, pos[1]));
        box[5] = static_cast<float>(fpu.add(r, pos[2]));
        i32 count = 0;
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(jo::self<jo::Unit>(this)->world)->cells_in_box(joan::orig::arg::mptr(box), &count));  // 0x00429090
        for (i32 i = 0; i < count; ++i) {
            void* const first = joan::orig::arg::as<void*>(joan::orig::as_world_cell(cells[i])->get_9c());  // 0x0054f890
            u8* e = static_cast<u8*>(first);
            if (e == nullptr) continue;
            do {
                u8* const t = unit_get(static_cast<i32>(jo::as_world_item(e)->object_id));
                const i16* const k = joan::orig::arg::as<i16*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(t)->type_id)));  // 0x00425ce0
                if (candidate_ok(mode, t, k)) {
                    float d = length_sq_f(pos, jo::as_map_object(t)->pos);
                    if (static_cast<double>(nn) >= static_cast<double>(d)) {  // C0 clear, ordered
                        if (from_leader != 0) d = length_sq_f(jo::self<jo::Unit>(this)->position_, jo::as_map_object(t)->pos);
                        if (best_bits == 0xbf800000u) {
                            best_bits = bitsf(d);
                            result = static_cast<i32>(jo::as_map_object(t)->index);
                        }
                        if (fbits(best_bits) > d) {
                            if (random100() <= 0x32) {
                                best_bits = bitsf(d);
                                result = static_cast<i32>(jo::as_map_object(t)->index);
                            }
                        }
                    }
                }
                e = jo::view<u8*>(jo::as_world_item(e)->cell_prev);
            } while (e != nullptr && e != first);
        }
        if (mode[0] == 'E' && random100() <= 0x32) {
            if (unit_gone(static_cast<i32>(jo::self<jo::Unit>(this)->target_object_id)) != 0)
                jo::self<jo::Unit>(this)->target_object_id = static_cast<i16>(result);
            else if (result != 0)
                jo::self<jo::Unit>(this)->target_object_id = static_cast<i16>(result);
        }
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_01", name, addr, fn)
B("Unit::pick_target_503680", 0x00503680, &H::pick_target_503680);
#undef B

}  // namespace game_b106_01
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {


JOAN_COUNTER(c_phase, "Unit::anim_phase");
JOAN_COUNTER(c_has_item, "Unit::has_item");

[[maybe_unused]] constexpr img::Addr kAnimRes = d::g_anim_res_mgr, kModelRes = d::g_model_res_mgr, kModelRefs = d::g_model_refs;

struct H {
    // ------------------------------------------------------------------ 0x005045f0
    float anim_phase() {
        c_phase.hit();
        if (jo::self<jo::Unit>(this)->actor == nullptr) return global<float>(d::k_9cf5c4);
        if (jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->anim_entry) == 0) return global<float>(d::k_9cf5c4);
        std::uint32_t t = global<std::uint32_t>(d::g_now) - jo::as_actor(jo::self<jo::Unit>(this)->actor)->start_time;
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(jo::self<jo::Unit>(this)->anim_entry)));  // 0x004168d0
        std::uint8_t* const a =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_res_mgr(obj(kAnimRes))->slot_4174f0(joan::orig::arg::u32(jo::as_anim_group_rec(e)->anim_id)));  // 0x004174f0
        void* const anim = jo::self<jo::AnimSlot>(a)->anim;
        auto length = [&]() {
            void* const seq = joan::orig::arg::as<void*>(joan::orig::as_anim_set(anim)->frame(joan::orig::arg::i32(jo::as_actor(jo::self<jo::Unit>(this)->actor)->action), 0u));  // 0x005b5dd0
            return joan::orig::arg::as<float>(joan::orig::as_range(seq)->span());  // 0x0042d130
        };
        if (jo::as_actor(jo::self<jo::Unit>(this)->actor)->anim_flags != 0) {
            const std::uint32_t len = static_cast<std::uint32_t>(joan::hook::x87::ftol(length()));
            t = t % len;
        }
        const float l1 = length();
        if (!(static_cast<double>(t) >= static_cast<double>(l1))) {  // fild qword t; fcompp; C0
            const float l2 = length();
            return static_cast<float>(Fpu{true}.sub(l2, static_cast<double>(static_cast<std::int32_t>(t))));  // fisub
        }
        return global<float>(d::k_9cf5c4);
    }

    // ------------------------------------------------------------------ 0x0050d170
    std::int32_t has_item_kind9(const char* name) {
        c_has_item.hit();
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->count()); i > 0; --i) {  // 0x00531360
            std::uint8_t* const it = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->get(joan::orig::arg::i32(i)));        // 0x005313a0
            std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_upgrade_list_entry(it)->id))));  // 0x0050d240
            if (jo::self<jo::UpgradeItem>(def)->type != 9) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::libc::_strnicmp(joan::orig::arg::str(jo::self<jo::UpgradeItem>(def)->bonus_text), joan::orig::arg::str(name), 6)) != 0) continue;
            if (jo::as_upgrade_list_entry(it)->cur == jo::as_upgrade_list_entry(it)->max) return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x0050d320
    std::int32_t has_item(std::int32_t kind) {
        c_has_item.hit();
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->count()); i > 0; --i) {
            std::uint8_t* const it = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->get(joan::orig::arg::i32(i)));
            std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_upgrade_list_entry(it)->id))));
            if (static_cast<std::int32_t>(jo::self<jo::UpgradeItem>(def)->type) != kind) continue;
            if (jo::as_upgrade_list_entry(it)->cur > 0) return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Unit::anim_phase", 0x005045f0, &H::anim_phase);
B("Unit::has_item_kind9", 0x0050d170, &H::has_item_kind9);
B("Unit::has_item", 0x0050d320, &H::has_item);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {


using u32 = std::uint32_t;

JOAN_COUNTER(c_progress, "Building::progress");
JOAN_COUNTER(c_total, "Group::total");
JOAN_COUNTER(c_drop, "Building::drop_2f9");

struct H {
    // ------------------------------------------------------------------------ 0x00504760
    float progress() {
        c_progress.hit();
        if (jo::self<jo::Unit>(this)->actor == nullptr) return 0.0f;
        if (jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry) == 0) return 0.0f;
        void* const a = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry))));    // 0x004168d0
        void** const b = joan::orig::arg::as<void**>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_anim_group_rec(a)->anim_id))));  // 0x004174f0
        void* const c = *b;
        void* const d = joan::orig::arg::as<void*>(joan::orig::as_anim_set(c)->frame(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->action)), 0u));  // 0x005b5dd0
        return joan::orig::arg::as<float>(joan::orig::as_range(d)->span());  // 0x0042d130
    }

    // ------------------------------------------------------------------------ 0x00509710
    float total() {
        c_total.hit();
        float sum = joan::orig::arg::as<float>(joan::orig::as_unit(this)->stat_pct());  // 0x005097d0
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->leads()) != 0) {     // 0x0044a660
            for (i32 i = 0; i < static_cast<i32>(static_cast<i8>(jo::view<u8>(jo::as_group(jo::self<jo::Unit>(this)->group)->count))); ++i) {
                const i32 id = static_cast<i16>(jo::view<u16>(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i]));
                if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)) != 0) continue;  // 0x00428b70
                void* const m = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
                sum = joan::orig::arg::as<float>(joan::orig::as_unit(m)->stat_pct()) + sum;                         // 0x005097d0
            }
        }
        return sum;
    }

    // ------------------------------------------------------------------------ 0x0050d050
    void drop_2f9(i32 flag) {
        c_drop.hit();
        if (flag != 1) return;
        void* const p = jo::self<jo::Unit>(this)->actor_b;
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
        jo::self<jo::Unit>(this)->alt_mesh_state = 0;
        jo::self<jo::Unit>(this)->actor_b = nullptr;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("Building::progress", 0x00504760, &H::progress);
B98("Group::total", 0x00509710, &H::total);
B98("Building::drop_2f9", 0x0050d050, &H::drop_2f9);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b107_14.cpp (game/b107_14)
namespace {
namespace game_b107_14 {

using u32 = std::uint32_t;

const char* rstr(std::uintptr_t a) { return &img::global<const char>(a); }

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12);

JOAN_COUNTER(c_attack_value, "Actor::attack_value");

// stricmp-like CRT helper (0x007c07b0)
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
using joan::orig::helpers::crt_abs;

void* const kSkills = obj(d::g_model_refs);

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x005048b0
    float attack_value(i32 target_id) {
        c_attack_value.hit();
        u8* const s = self();
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(target_id)));  // 0x0041b3c0
        auto act = [s]() -> i32 { return jo::as_unit(s)->anim_kind; };
        auto tact = [t]() -> i32 { return jo::as_unit(t)->anim_kind; };

        if (act() == 0x2a || joan::orig::arg::as<char>(joan::orig::as_unit(s)->dead()) != 0)    // 0x0042d0e0
            return 0.0f;
        if (target_id == static_cast<i32>(jo::as_unit(s)->id))
            return 0.0f;
        {
            const i32 a = act();
            if (a == 0x1e || a == 0x1f || a == 0x20 || a == 0x21 || a == 0x53 || a == 0x4f ||
                a == 0x50 || a == 0x52 || a == 0x51)
                return 0.0f;
        }
        auto ranged = [&]() {
            const i32 a = act();
            return a == 0x40 || a == 0x41 || a == 0x42 || a == 0x4e || a == 0x29;
        };
        if (ranged()) {
            if (tact() != 0x22) {
                if (jo::as_unit(s)->actor == nullptr || jo::view<u32>(jo::as_actor(jo::as_unit(s)->actor)->record) == 0)
                    return 0.0f;
                const i32 c = jo::as_unit(t)->combo_state;
                if (c != 0 && jo::view<signed char>(jo::as_unit(t)->armour_flags[c - 1]) == 0x41)
                    return 0.0f;
            }
        }
        if (tact() == 0x22 && !ranged())
            return 0.0f;
        if (jo::as_unit(s)->hit_value > 0 && tact() != 0x22)
            return 0.0f;
        if (tact() == 0x22 && jo::view<signed char>(jo::as_unit(s)->downed_hit) != 0)
            return 0.0f;
        if (jo::view<signed char>(jo::as_unit(t)->in_scene) == 0)
            return 0.0f;

        // weapon / skill id of the target
        u8* const w = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(t)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(t)->anim_entry))));  // 0x004168d0
        const u32 skill = jo::view<u32>(jo::as_anim_group_rec(w)->model_ref);
        i32 bonus = 0;
        const i32 zero = 0;
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(static_cast<i32>(jo::as_unit(t)->carried_a)) == 0) {  // 0x00425d50
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(static_cast<i32>(jo::as_unit(t)->carried_a)));  // 0x00425d20
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::view<u16>(jo::as_map_object(e)->type_id))));  // 0x00425ce0
            bonus = jo::as_item_def(q)->i32_32;
        }
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(static_cast<i32>(jo::as_unit(t)->carried_b)) == 0) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(static_cast<i32>(jo::as_unit(t)->carried_b)));
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::view<u16>(jo::as_map_object(e)->type_id))));
            bonus = jo::as_item_def(q)->i32_32;
        }
        if (bonus < zero)
            bonus = zero;
        if (joan::orig::arg::as<i32>(joan::orig::as_players(obj(d::g_nations))->hostile(static_cast<i32>(jo::as_unit(s)->side), static_cast<i32>(jo::as_unit(t)->side))) == 0)    // 0x004b6860
            return 0.0f;

        V3 tp{0.0f, 0.0f, 0.0f};
        V3 sp{0.0f, 0.0f, 0.0f};
        V3 tmp{0.0f, 0.0f, 0.0f};
        joan::orig::as_unit(t)->position(&tp);                            // 0x004fedb0
        joan::orig::as_unit(s)->position(&sp);
        V3 diff = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &tp, &sp));       // 0x0060cc90
        const float len_sq = v3d_float(d::iat_v3d_87_length_sq, static_cast<void*>(&diff));  // length_sq

        auto skill_rec = [skill]() {
            return joan::orig::arg::as<u8*>(joan::orig::as_model_refs(kSkills)->get(joan::orig::arg::i16(static_cast<u16>(skill))));  // 0x00416f20
        };
        const u32 r1 = static_cast<u32>(jo::as_model_slot(skill_rec())->times[2]) + static_cast<u32>(bonus);
        const u32 r2 = static_cast<u32>(jo::as_model_slot(skill_rec())->times[2]) + static_cast<u32>(bonus);
        const i32 range_sq = static_cast<i32>(r1 * r2);
        if (!(static_cast<double>(range_sq) >= static_cast<double>(len_sq)))
            return 0.0f;

        // facing angle of the target towards us vs. the weapon's arc
        float ang = joan::orig::arg::as<float>(joan::orig::as_unit(t)->heading_to_unit(static_cast<i32>(jo::as_unit(s)->id)));  // 0x00505960
        float hi = static_cast<float>(jo::as_model_slot(skill_rec())->times[6]);
        float lo = static_cast<float>(jo::as_model_slot(skill_rec())->times[7]);
        if (!(hi >= lo)) {
            const float x = hi;
            hi = lo;
            lo = x;
        }
        if (!(ang >= lo) || ang > hi) {
            if (!(ang >= 0.0f)) {
                ang = ang + 360.0f;  // 0x9cf9c8
                if (ang > hi)
                    return 0.0f;
            } else {
                ang = ang - 360.0f;
                if (!(ang >= lo))
                    return 0.0f;
            }
        }

        const float dir = joan::orig::arg::as<float>(joan::orig::as_unit(s)->heading_to_unit(joan::orig::arg::i32(target_id)));  // 0x00505960
        if (act() == 0x2b || act() == 0x2d) {
            const i32 g = global<i32>(d::g_a3ccb0);
            if (static_cast<double>(g) >= static_cast<double>(dir)) {
                const i32 ng = static_cast<i32>(static_cast<u32>(g) * 0xffffffffu);
                if (!(static_cast<double>(ng) > static_cast<double>(dir))) {
                    const u32 defend = joan::orig::arg::as<u32>(joan::orig::as_name_table(kSkills)->find_41a0e0(joan::orig::arg::str(rstr(img::addr(d::s_DEFEND)))));  // 0x0041a0e0 "DEFEND"
                    if (defend != 0) {
                        u8* const d = joan::orig::arg::as<u8*>(joan::orig::as_model_refs(kSkills)->get(joan::orig::arg::i16(static_cast<u16>(defend))));
                        if (jo::as_model_slot(d)->emitter_flag != 0 && crt_stricmp(rstr(reinterpret_cast<std::uintptr_t>(&jo::as_model_slot(d)->emitter_names[5 * 33])), rstr(img::addr(d::s_none))) != 0)  // "none"
                            joan::orig::as_spawner(obj(d::g_particle_effect_array))->try_spawn(joan::orig::arg::i32(defend), jo::as_actor(jo::as_unit(s)->actor)->scene, 5, static_cast<i32>(jo::as_unit(s)->id));  // 0x0041b640
                    }
                    V3 pos{0.0f, 0.0f, 0.0f};
                    std::memcpy(&pos, jo::as_unit(s)->position_, sizeof pos);
                    joan::orig::as_sound_list(obj(d::g_sfx_player))->play_effect(rstr(img::addr(d::s_GUARD)), pos);  // 0x00494810 "GUARD"
                    if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::as_unit(s)->anim_set)->index_of_416dc0(0x2d)) != 0) {  // 0x00416dc0
                        if (act() != 0x2d) {
                            joan::orig::as_unit(s)->set_action(0x2d, 0, 1);  // 0x00501190
                        } else {
                            const double v = joan::orig::arg::as<double>(joan::orig::as_unit(s)->anim_phase());  // 0x005045f0 (st0)
                            if (!(v < 0.0 || v > 0.0))  // C3: equal or unordered
                                joan::orig::as_unit(s)->set_action(0x2d, 0, 1);
                        }
                    }
                    return 0.0f;
                }
            }
        }

        if (skill != 0) {
            u8* const k = skill_rec();
            if (jo::as_model_slot(k)->emitter_flag != 0 && crt_stricmp(rstr(reinterpret_cast<std::uintptr_t>(&jo::as_model_slot(k)->emitter_names[4 * 33])), rstr(img::addr(d::s_none))) != 0)
                joan::orig::as_spawner(obj(d::g_particle_effect_array))->try_spawn(joan::orig::arg::i32(skill), jo::as_actor(jo::as_unit(s)->actor)->scene, 4, static_cast<i32>(jo::as_unit(s)->id));
        }
        i32 roll = 0;
        if (tact() == 0x22) {
            roll = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->random_range(0x32, 0x64));  // 0x00590690
        } else if (crt_abs(joan::hook::x87::ftol(dir)) <= 0x2d) {
            roll = 0;
        } else if (crt_abs(joan::hook::x87::ftol(dir)) <= 0x87) {
            roll = 0x14;
        } else {
            roll = 0x28;
        }
        const float dmg = joan::orig::arg::as<float>(joan::orig::as_unit(s)->melee_damage_509950(joan::orig::arg::u32(target_id), joan::orig::arg::i32(roll)));  // 0x00509950
        if (target_id == global<i32>(d::g_player_unit_recno))
            joan::orig::as_hud(obj(d::g_player_hud))->set_item_portrait(static_cast<i32>(jo::as_unit(s)->id));  // 0x006fe280
        if ((dmg < 0.0f || dmg > 0.0f) && jo::as_unit(s)->layer != 0)
            jo::view<u16>(jo::as_unit(s)->attack_chance) = static_cast<u16>(jo::view<u16>(jo::as_unit(s)->attack_chance) + 10);
        return dmg;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_14", name, addr, fn)
B("Actor::attack_value", 0x005048b0, &H::attack_value);
#undef B

}  // namespace game_b107_14
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {


using u32 = std::uint32_t;

JOAN_COUNTER(c_attack_value, "Unit::attack_value");
JOAN_COUNTER(c_set_view, "Unit::set_view");

bool nonzero_ordered(float v) { return v < 0.0f || v > 0.0f; }  // !(ZF) after fcomp 0.0

struct H {
    // ------------------------------------------------------------------------ 0x00505650
    float attack_value(u32 target) {
        c_attack_value.hit();
        void* const t = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(target));  // 0x00425d20
        const i32 kind = static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind);
        if (kind == 0x2a) return global<float>(d::k_9cf5c4);
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(this)->dead())) != 0)  // 0x0042d0e0
            return global<float>(d::k_9cf5c4);
        if (kind == 0x1e || kind == 0x1f || kind == 0x20 || kind == 0x21 || kind == 0x53)
            return global<float>(d::k_9cf5c4);
        if (kind == 0x29) return global<float>(d::k_9cf5c4);
        if (kind == 0x40 || kind == 0x41 || kind == 0x42 || kind == 0x4e)
            return global<float>(d::k_9cf5c4);
        if (jo::self<jo::Unit>(this)->hit_value > 0) return global<float>(d::k_9cf5c4);
        if (static_cast<i32>(jo::view<i8>(jo::as_map_object(t)->active)) == 0) return global<float>(d::k_9cf5c4);
        const float v = joan::orig::arg::as<float>(joan::orig::as_unit(this)->trade_amount(joan::orig::arg::i32(target), 0));  // 0x0049d5f0
        if (nonzero_ordered(v)) jo::view<u16>(jo::self<jo::Unit>(this)->attack_chance) = static_cast<u16>(jo::view<u16>(jo::self<jo::Unit>(this)->attack_chance) + 10);
        return v;
    }

    // ------------------------------------------------------------------------ 0x00505b50
    float value_of_type(u32 type) {
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(type)));  // 0x0041b3c0
        return joan::orig::arg::as<float>(joan::orig::as_unit(this)->bearing_to(joan::orig::arg::mptr(jo::as_unit(p)->position_)));              // 0x00505ba0
    }

    // ------------------------------------------------------------------------ 0x00505cc0
    void set_view(u32 x, u32 y, u32 z) {
        c_set_view.hit();
        float zf;
        std::memcpy(&zf, &z, 4);
        if (nonzero_ordered(zf)) jo::self<jo::Unit>(this)->view = global<u32>(d::g_now);
        joan::orig::as_players(obj(d::g_physic_sys))->set_b16(joan::orig::arg::bits(static_cast<float>(jo::self<jo::Unit>(this)->gravity_level)));  // 0x00472eb0
        joan::orig::as_players(obj(d::g_physic_sys))->set_b1a(joan::orig::arg::i16(1));                                   // 0x00505d60
        float* const v = jo::as_actor(jo::self<jo::Unit>(this)->actor)->velocity;
        jo::view<u32>(v[0]) = x;
        jo::view<u32>(v[1]) = y;
        jo::view<u32>(v[2]) = z;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("Unit::attack_value", 0x00505650, &H::attack_value);
B100("Unit::value_of_type", 0x00505b50, &H::value_of_type);
B100("Unit::set_view", 0x00505cc0, &H::set_view);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {


using u32 = std::uint32_t;

struct Vec3 {
    float x, y, z;
};

JOAN_COUNTER(c_can_face, "Unit::can_face_505820");
JOAN_COUNTER(c_find_kind_d, "Obj::find_kind_d");

struct H {

    // ------------------------------------------------------------------------ 0x00505820
    i32 can_face(u32 id) {
        c_can_face.hit();
        u8* const other = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        if (jo::view<i8>(jo::as_unit(other)->in_scene) == 0) return 0;
        if (id != global<u32>(d::g_player_unit_recno)) return 0;
        Vec3 a{};
        Vec3 b{};
        Vec3 tmp{};
        joan::orig::as_unit(other)->position(&a);  // 0x004fedb0
        joan::orig::as_unit(this)->position(&b);   // 0x004fedb0
        const Vec3* const r = joan::orig::arg::as<const Vec3*>(joan::orig::vec_sub(&tmp, &a, &b));  // 0x0060cc90
        Vec3 d;
        std::memcpy(&d, r, sizeof d);
        const float ls = v3d_float(d::iat_v3d_87_length_sq, static_cast<const void*>(&d));  // length_sq
        if (ls > global<float>(d::flt_10000)) return 0;  // 10000.0f
        const float ang = joan::orig::arg::as<float>(joan::orig::as_unit(other)->heading_to_unit(static_cast<i32>(jo::self<jo::Unit>(this)->id)));  // 0x00505960
        if (!(ang >= -90.0f)) return 0;
        if (ang > 90.0f) return 0;
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00509800
    void empty_509800() {}

    // ------------------------------------------------------------------------ 0x0050d4b0
    i32 find_kind_d() {
        c_find_kind_d.hit();
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->count()); i > 0; --i) {  // 0x00531360
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->get(joan::orig::arg::i32(i)));       // 0x005313a0
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_list_entry(e)->id))));  // 0x0050d240
            if (jo::self<jo::UpgradeItem>(rec)->type == 0xd) return jo::as_upgrade_list_entry(e)->cur;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("Unit::can_face_505820", 0x00505820, &H::can_face);
B("Obj::empty_509800", 0x00509800, &H::empty_509800);
B("Obj::find_kind_d", 0x0050d4b0, &H::find_kind_d);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {


void* const kTable57 = obj(d::g_nations);
void* const kUnits = obj(d::g_units);

struct H {
    // 0x00509400
    void* lookup_509400() {
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(kTable57)->is_null(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side))) != 0)  // 0x00484870
            return nullptr;
        return joan::orig::arg::as<void*>(joan::orig::as_dynarray(kTable57)->ptr_at(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side)));  // 0x0047b600
    }

    // 0x00505960
    float set_from(std::int32_t id) {  // float in st0 (0x00505a00 heading)
        std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        return joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to(joan::orig::arg::mptr(jo::as_unit(u)->position_)));                // 0x00505a00
    }

    // 0x00508f80 / 0x004f2f60
    std::int32_t get_1f9() { return jo::self<jo::Unit>(this)->rider_ref; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj509::lookup", 0x00509400, &H::lookup_509400);
M("Obj505::set_from", 0x00505960, &H::set_from);
M("Obj::get_1f9", 0x00508f80, &H::get_1f9);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
namespace {
namespace game_b89_05 {

// x87 "fcomp; test ah,0x40; je" falls through for equal or unordered.
bool equal_or_unordered(float a, float b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_heading, "Object::heading_to");

struct H {
    // ------------------------------------------------------------ 0x00505a00 (ret 4)
    float heading_to(const float* point) {
        c_heading.hit();
        const joan::hook::x87::Fpu fpu{true};
        const float facing = joan::orig::arg::as<float>(joan::orig::as_ground_object(this)->heading());  // 0x004ff850 (fld [this+0x1a])
        const float degrees = static_cast<float>(fpu.div(fpu.mul(facing, 180.0f), 3.141592653589793));
        float pos[3];  // ebp-0x18
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
        if (equal_or_unordered(pos[0], point[0]) && equal_or_unordered(pos[1], point[1])) return 0.0f;
        std::uint32_t dir = 0;  // ebp-0x1c (default ctor 0x00577190 leaves it)
        joan::orig::as_angle(&dir)->construct_577190();  // 0x00577190
        const double dy = static_cast<double>(point[1] - pos[1]);
        const double dx = static_cast<double>(point[0] - pos[0]);
        joan::orig::as_angle(&dir)->from_xy(joan::orig::arg::f64(dx), joan::orig::arg::f64(dy));  // 0x00577b90
        const float unused = joan::orig::arg::as<float>(joan::orig::as_angle(&dir)->degrees());  // 0x00577450 (stored to ebp-0x20)
        (void)unused;
        std::uint32_t face = 0;  // ebp-0x24
        joan::orig::as_angle(&face)->construct_577190();          // 0x00577190
        joan::orig::as_angle(&face)->set_degrees(joan::orig::arg::f32(degrees));  // 0x005773e0
        std::uint32_t tmp = 0;                       // ebp-0x30
        std::uint32_t diff = *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_turn(&face)->sub(&tmp, &dir));  // 0x005775b0
        float r = joan::orig::arg::as<float>(joan::orig::as_angle(&diff)->degrees());  // 0x00577450
        if (r > 180.0f) r = r - 360.0f;               // test ah,0x41 jne
        if (!(r >= -180.0f)) r = r + 360.0f;          // test ah,1 je (less or unordered adds)
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("Object::heading_to", 0x00505a00, &H::heading_to);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {


using u32 = std::uint32_t;

JOAN_COUNTER(c_bearing, "Unit::bearing_to");
JOAN_COUNTER(c_damage5, "Unit::apply_damage_5");

struct H {
    // ------------------------------------------------------------------------ 0x00505ba0
    float bearing_to(const float* point) {
        c_bearing.hit();
        float pos[3];  // ebp-0x10
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
        const float dx = point[0] - pos[0];
        const float dy = point[1] - pos[1];
        const float dx2 = dx * dx;  // x87 at PC24: each op rounded to float (no contraction)
        const float dy2 = dy * dy;
        const float sum = dx2 + dy2;
        const float dist = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum)))));  // sqrt
        if (!(dist < global<float>(d::k_9cf5c4) || dist > global<float>(d::k_9cf5c4))) {
            return global<float>(d::flt_90);  // test ah,0x40 jne: equal or unordered -> 90.0
        }
        u32 dir = 0;  // ebp-0x18 (default ctor 0x00577190 leaves it)
        joan::orig::as_angle(&dir)->construct_577190();  // 0x00577190
        const double dz = static_cast<double>(point[2] - pos[2]);
        joan::orig::as_angle(&dir)->from_xy(joan::orig::arg::f64(static_cast<double>(dist)), joan::orig::arg::f64(dz));  // 0x00577b90
        float r = joan::orig::arg::as<float>(joan::orig::as_angle(&dir)->degrees());  // 0x00577450
        if (r > global<float>(d::flt_180)) r = r - global<float>(d::flt_360);     // test ah,0x41 jne
        if (!(r >= global<float>(d::flt_m180))) r = r + global<float>(d::flt_360);  // test ah,1 je
        return r;
    }

    // ------------------------------------------------------------------------ 0x00509d90
    u32 apply_damage_5(u32 id) {
        c_damage5.hit();
        const Fpu fpu{true};
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(unit)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(unit)->anim_entry))));  // 0x004168d0
        const float base = static_cast<float>(jo::as_anim_group_rec(rec)->i32_1c);  // ebp-0xc
        const i32 pct = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->stat_3()) + 0x64;      // 0x005237e0
        const float amount = static_cast<float>(
            fpu.div(fpu.mul(static_cast<double>(pct), base), global<float>(d::flt_100)));  // ebp-0x10
        const float zero = 0.0f;                                     // ebp-0x14
        const float half0 = zero / global<float>(d::flt_2);        // ebp-0x18
        float extra;                                                 // ebp-0x24
        if (!((amount - half0) / global<float>(d::flt_2) >= global<float>(d::k_9cf5c4))) {
            extra = 0.0f;  // test ah,1 je: less or unordered
        } else {
            extra = (amount - half0) / global<float>(d::flt_2);
        }
        const float total = amount / global<float>(d::flt_2) + extra;  // ebp-0x1c
        const i32 hp = joan::hook::x87::ftol(fpu.sub(static_cast<double>(jo::self<jo::Unit>(this)->hit_points), total));
        joan::orig::as_unit(this)->set_value_135(hp);  // 0x004fe510
        return 0;  // void callee; every caller ignores the result
    }

    // ------------------------------------------------------------------------ 0x00510e40
    float stat_e_scaled() {
        const Fpu fpu{true};
        const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->find_link(i32{0xe}, joan::orig::arg::str(i32{0})));  // 0x0050d3d0
        return static_cast<float>(
            fpu.div(fpu.add(static_cast<double>(v), global<float>(d::flt_10)), global<float>(d::flt_11)));
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Unit::bearing_to", 0x00505ba0, &H::bearing_to);
B95("Unit::apply_damage_5", 0x00509d90, &H::apply_damage_5);
B95("Unit::stat_e_scaled", 0x00510e40, &H::stat_e_scaled);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

// Virtual thiscall: vtable slot at x86 byte offset `off` (slot off / 4, pointer-sized entries).
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>((*static_cast<const std::uintptr_t* const*>(o))[joan::call::x86_vslot(off)], o, args...);  // x86 byte offset -> slot
}

using u32 = std::uint32_t;

JOAN_COUNTER(c_slot_event, "Obj::slot_event");
JOAN_COUNTER(c_set_10c, "Obj::set_10c");

struct H {
    // ------------------------------------------------------------------------ 0x00505d60
    void* set_b1a(i16 v) {
        jo::self<jo::Players>(this)->i16_b1a = v;
        return this;
    }

    // ------------------------------------------------------------------------ 0x0050c4f0
    void slot_event(i16 a, i16 b, i16 c, i16 d, i16 e, u32 x, u32 y, u32 z) {
        c_slot_event.hit();
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return;
        joan::orig::as_names(jo::self<jo::Unit>(this)->modifiers)->set_4970c0(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::u32(z));  // 0x004970c0
        if (static_cast<i32>(jo::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->hand_mode) != static_cast<i32>(a)) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return;  // 0x0042d690
        const i32 o = joan::orig::arg::as<i32>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(c))));  // 0x0049ea90
        if (o == 0) return;
        void* const p1 = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(o));  // 0x00425d20
        vcall<i32>(p1, 0x18, 1);  // the `push 1` before Objects::get is this vcall's argument
        void* const p2 = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(o));  // 0x00425d20
        vcall<i32>(p2, 0x1c);
        if (static_cast<i32>(b) == 0) {
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(c)));  // 0x00425ce0
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(static_cast<i16>(o)), static_cast<i32>(jo::view<i16>(jo::as_item_def(r)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
        } else if (static_cast<i32>(b) == 1) {
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(c)));  // 0x00425ce0
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(static_cast<i16>(o)), static_cast<i32>(jo::view<i16>(jo::as_item_def(r)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));  // 0x0050a480
        }
    }

    // ------------------------------------------------------------------------ 0x00510030
    std::uintptr_t set_10c(i32 v) {
        c_set_10c.hit();
        if (jo::self<jo::Unit>(this)->formation_slot == v) return reinterpret_cast<std::uintptr_t>(this);
        jo::self<jo::Unit>(this)->formation_slot = v;
        if (jo::self<jo::Unit>(this)->formation_slot != 0) {
            jo::self<jo::Unit>(this)->state_120_saved = jo::self<jo::Unit>(this)->state_120;
            jo::self<jo::Unit>(this)->state_120 = 1;
        } else {
            jo::self<jo::Unit>(this)->state_120 = jo::self<jo::Unit>(this)->state_120_saved;
        }
        jo::self<jo::Unit>(this)->hit_points = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max());  // 0x00523320
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->value_149());    // 0x00523700
        jo::self<jo::Unit>(this)->mana = r;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::set_b1a", 0x00505d60, &H::set_b1a);
B("Obj::slot_event", 0x0050c4f0, &H::slot_event);
B("Obj::set_10c", 0x00510030, &H::set_10c);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {


using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kPlayers = obj(d::g_nations);

JOAN_COUNTER(c_is_ally, "Unit::is_ally_of");
JOAN_COUNTER(c_stat16, "Unit::stat16_factor");

struct H {
    // ------------------------------------------------------------------------ 0x00505d90
    void set_vec_6c(V3 v) {
        jo::view<V3>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->impulse) = v;
    }

    // ------------------------------------------------------------------------ 0x00509670
    bool is_ally_of(i16 player) {
        c_is_ally.hit();
        void* const mine = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->nation());  // 0x00509400
        if (mine == nullptr) return false;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlayers)->is_null(static_cast<i32>(player))) != 0) return false;  // 0x00484870
        u8* const other = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kPlayers)->ptr_at(static_cast<i32>(player)));  // 0x0047b600
        if (other == mine) return true;
        const i8 r = joan::orig::arg::as<i8>(joan::orig::as_table(mine)->byte_58(joan::orig::arg::i16(jo::as_nation(other)->id)));  // 0x004b4360
        return static_cast<i32>(r) >= 4;
    }

    // ------------------------------------------------------------------------ 0x00510f10
    float stat16_factor() {
        c_stat16.hit();
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            const Fpu fpu{true};
            const i32 s = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(i32{0x16}, joan::orig::arg::i32(// 0x00497e10
                                        static_cast<i32>(jo::self<jo::Unit>(this)->id))));
            const i32 n = 0x64 - s;
            return static_cast<float>(fpu.mul(static_cast<double>(n), global<float>(d::flt_0_01)));
        }
        return global<float>(d::flt_1);
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Unit::set_vec_6c", 0x00505d90, &H::set_vec_6c);
B93("Unit::is_ally_of", 0x00509670, &H::is_ally_of);
B93("Unit::stat16_factor", 0x00510f10, &H::stat16_factor);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b110_11.cpp (game/b110_11)
namespace {
namespace game_b110_11 {

using u32 = std::uint32_t;

// Plain dword read at an integer address (the original may read through a null pointer here).
// the original reads a dword at a possibly null address here (see the comment above)
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
void* rd_ptr(std::uintptr_t address) { return *reinterpret_cast<void* volatile*>(address); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

struct Vec {
    float x, y, z;
};
static_assert(sizeof(Vec) == 12, "vec3");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kForms = obj(d::g_physic_sys);

u8* unit_type(const void* u) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id)))); }  // 0x0042d0a0
i32 unit_missing(i32 k) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)); }  // 0x00428b70
u8* unit_rec(i32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k))); }  // 0x0041b3c0
u8* form_get(u32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_players(kForms)->list.ptr_at(joan::orig::arg::i32(k))); }  // 0x006af310
void unit_pos(const void* u, Vec* out) { joan::orig::as_unit(u)->position(joan::orig::arg::mptr(out)); }  // 0x004fedb0
void turn_to(const void* u, float deg) { joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(deg)); }   // 0x0050abd0

// Path lists (0x0066e4b0 class).
i32 count(const void* list) { return joan::orig::arg::as<i32>(joan::orig::as_list(list)->count_66e8a0()); }               // 0x0066e8a0
float* item(const void* list, i32 i) { return joan::orig::arg::as<float*>(joan::orig::as_list(list)->from_end(joan::orig::arg::i32(i))); }  // 0x0066e8d0
float* back(const void* list) { return joan::orig::arg::as<float*>(joan::orig::as_path(list)->first()); }          // 0x0066e940
void push(const void* list, const void* p) { joan::orig::as_path(list)->push(joan::orig::arg::mptr(p)); }     // 0x0066e840
void clear(const void* list) { joan::orig::as_path(list)->clear(); }                     // 0x004eeca0

JOAN_COUNTER(c_move, "Unit::move_to_505dd0");

// Radius used for the fine path (switch on type +0x1c3), before the * [0x9cfc20] scaling.
float path_radius(const u8* t) {
    float v = static_cast<float>(static_cast<i32>(global<u32>(d::g_unit_body_r) << 1));
    const i32 kind = jo::view<i32>(jo::self<jo::UnitType>(t)->shape_kind);
    if (kind == 1) {
        v = jo::self<jo::UnitType>(t)->shape_sphere[3];
    } else if (kind == 2) {
        auto fabs_ = [&](i32 i) {
            return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::self<jo::UnitType>(t)->shape_box[i]))));  // CRT fabs
        };
        const double a = fabs_(0);
        const double b = fabs_(3);
        const double m1 = !(b >= a) ? fabs_(0) : fabs_(3);
        v = static_cast<float>(m1);
        const double q1 = m1;
        const double c = fabs_(1);
        const double m2 = !(c >= q1) ? static_cast<double>(v) : fabs_(1);
        v = static_cast<float>(m2);
        const double q2 = m2;
        const double d = fabs_(4);
        const double m3 = !(d >= q2) ? static_cast<double>(v) : fabs_(4);
        v = static_cast<float>(m3);
    } else if (kind == 3) {
        v = jo::self<jo::UnitType>(t)->shape_cylinder[3];
    }
    return v;
}

// ------------------------------------------------------------------ thiscall functions
struct H {
    void* zone() { return jo::self<jo::Unit>(this)->cell; }
    void* p432() { return &jo::self<jo::Unit>(this)->path; }
    void* p473() { return &jo::self<jo::Unit>(this)->path_b; }
    // v - (float)int [+0x339 + off], one x87 rounding.
    float rel(float v, i32 jo::Cell::*origin) {
        const Fpu fpu{true};
        return static_cast<float>(fpu.sub(static_cast<double>(v), static_cast<double>(jo::as_cell(zone())->*origin)));
    }
    bool zone_ok(const Vec* p) {  // 0x0060d5f0 on +0x339 + 8, then (i8) zone +0x43
        if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_cell(zone())->box)->contains(joan::orig::arg::ptr(p))) == 0) return false;
        return jo::view<i8>(jo::as_cell(zone())->loaded) != 0;
    }
    // +0x40e += segment lengths of the +0x432 path (caller has zeroed +0x40e).
    void sum_length() {
        const Fpu fpu{true};
        for (i32 i = 1; i <= count(p432()); ++i) {
            const float* a = item(p432(), i - 1);
            const float* b = item(p432(), i);
            const float dx = a[0] - b[0];
            const float dy = a[1] - b[1];
            const float dz = a[2] - b[2];
            const float s = (dx * dx + dy * dy) + dz * dz;
            const double r = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(s)));  // 0x004256d0 sqrt
            jo::view<float>(jo::self<jo::Unit>(this)->path_remaining) = static_cast<float>(fpu.add(r, static_cast<double>(jo::view<float>(jo::self<jo::Unit>(this)->path_remaining))));
        }
    }
    // Slot mover 0x004eb810 / 0x004eb760 (on +0x339 ->5c, ret 0x24).
    void slot_move(bool b9, const Vec& from, const Vec& pos, u32 a14) {
        joan::orig::as_squad(this)->clear();  // 0x00422f10
        void* const mover = jo::view<void*>(jo::as_cell(zone())->tracks);
        thiscall<u32>(b9 ? fn::Slots_call_4d3790 : fn::Path_test_segment_4eb760, mover, p432(), jo::self<jo::Unit>(this)->name_index, from, pos, a14);  // 0x004eb810 / 0x004eb760
    }
    bool type_9() {
        if (jo::self<jo::UnitType>(unit_type(this))->unit_class == 9) return true;
        return jo::self<jo::UnitType>(unit_type(this))->kind == 0xb;
    }

    // Coarse path from `st` to `pos` into +0x473, spliced with a fine path (static list `fine`)
    // into +0x432. 0x00506240..0x00506b0f and 0x005075b5..0x00507fb0.
    u8 replan(const Vec& st, const Vec& pos, u32 a14, img::Addr fine_addr, u8 bit, const void* dtor, bool chk118) {
        const Fpu fpu{true};
        const float c = global<float>(d::flt_32);
        {
            const float f41e = jo::view<float>(jo::self<jo::Unit>(this)->form_width) / c;
            const float f426 = jo::view<float>(jo::self<jo::Unit>(this)->form_slot_y) / c;
            const float lim = static_cast<float>(fpu.div(static_cast<double>(global<i32>(d::g_unit_body_r)), c));
            const float py = pos.y / c, px = pos.x / c;
            const float sy = st.y / c, sx = st.x / c;
            void* const finder = jo::view<void*>(jo::as_world(jo::self<jo::Unit>(this)->world)->path_space);
            joan::orig::as_path_space(finder)->find_path(joan::orig::arg::f32(sx), joan::orig::arg::f32(sy), joan::orig::arg::f32(u32{0}), joan::orig::arg::f32(px), joan::orig::arg::f32(py), joan::orig::arg::f32(u32{0}), joan::orig::arg::mptr(p473()), joan::orig::arg::bits(lim), i32{0}, joan::orig::arg::i32(a14), joan::orig::arg::bits(f426), joan::orig::arg::bits(f41e), joan::orig::arg::u32(u32{0xbf800000}));  // 0x0066cb40
        }
        joan::orig::as_path(p473())->scale(32.0f, 1.0f);              // 0x0066faa0
        joan::orig::as_path(p473())->translate(16.0f, 16.0f, joan::orig::arg::f32(i32{0}));     // 0x0066fa00
        jo::view<Vec>(jo::as_point_list(p473())->start.pos) = st;  // +0x490
        if (count(p473()) != 0) {
            i32 first = -1, last = 0;
            for (i32 i = 0; i <= count(p473()); ++i) {
                const Vec v = *reinterpret_cast<const Vec*>(item(p473(), i));
                if (!zone_ok(&v)) break;
                if (first == -1) first = i;
                last = i;
            }
            if (!(last > first) && first != -1 && !(first + 1 > count(p473()))) last = first + 1;
            if (first != -1 && last > first) {
                const float* e1 = item(p473(), first);
                const float* e2 = item(p473(), last);
                void* const fine = obj(fine_addr);
                if ((global<u8>(d::g_a3cb70) & bit) == 0) {
                    global<u8>(d::g_a3cb70) = static_cast<u8>(global<u8>(d::g_a3cb70) | bit);
                    joan::orig::as_path(fine)->construct();   // 0x0066e4b0
                    joan::crt::atexit(dtor);  // atexit
                }
                clear(fine);
                bool special = jo::self<jo::UnitType>(unit_type(this))->unit_class == 8;
                if (!special && chk118) special = jo::self<jo::UnitType>(unit_type(this))->kind == 2;
                if (!special) special = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0;  // 0x00430190
                if (special) {
                    const u8* const t = unit_type(this);
                    const float r = path_radius(t) * global<float>(d::flt_10);
                    const u32 z2 = jo::view<u32>(e2[2]);
                    const float y2 = rel(e2[1], &jo::Cell::origin_y), x2 = rel(e2[0], &jo::Cell::origin_x);
                    const u32 z1 = jo::view<u32>(e1[2]);
                    const float y1 = rel(e1[1], &jo::Cell::origin_y), x1 = rel(e1[0], &jo::Cell::origin_x);
                    void* const finder = joan::orig::arg::as<void*>(joan::orig::as_world_cell(zone())->grid(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->layer))));  // 0x005600c0
                    joan::orig::as_path_space(finder)->find_path(joan::orig::arg::f32(x1), joan::orig::arg::f32(y1), joan::orig::arg::f32(z1), joan::orig::arg::f32(x2), joan::orig::arg::f32(y2), joan::orig::arg::f32(z2), joan::orig::arg::mptr(fine), joan::orig::arg::bits(r), i32{0}, joan::orig::arg::i32(a14), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_slot_y), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_width), joan::orig::arg::u32(u32{0x3f000000}));  // 0x0066cb40
                } else {
                    const float lim = static_cast<float>(static_cast<i32>(global<u32>(d::g_unit_body_r) << 1));
                    const u32 z2 = jo::view<u32>(e2[2]);
                    const float y2 = rel(e2[1], &jo::Cell::origin_y), x2 = rel(e2[0], &jo::Cell::origin_x);
                    const u32 z1 = jo::view<u32>(e1[2]);
                    const float y1 = rel(e1[1], &jo::Cell::origin_y), x1 = rel(e1[0], &jo::Cell::origin_x);
                    void* const finder = joan::orig::arg::as<void*>(joan::orig::as_world_cell(zone())->grid(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->layer))));  // 0x005600c0
                    joan::orig::as_path_space(finder)->find_path(joan::orig::arg::f32(x1), joan::orig::arg::f32(y1), joan::orig::arg::f32(z1), joan::orig::arg::f32(x2), joan::orig::arg::f32(y2), joan::orig::arg::f32(z2), joan::orig::arg::mptr(fine), joan::orig::arg::bits(lim), i32{0}, joan::orig::arg::i32(a14), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_slot_y), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_width), joan::orig::arg::u32(u32{0xbf800000}));  // 0x0066cb40
                }
                {
                    const float zy = static_cast<float>(jo::as_cell(zone())->origin_y);
                    const float zx = static_cast<float>(jo::as_cell(zone())->origin_x);
                    joan::orig::as_path(fine)->translate(joan::orig::arg::f32(zx), joan::orig::arg::f32(zy), joan::orig::arg::f32(i32{0}));  // 0x0066fa00
                }
                const float* a = item(p473(), 0);
                const float* b = back(p473());
                if (last == count(p473())) b = back(fine);
                {
                    const i32 bz = ftol(b[2]), by = ftol(b[1]), bx = ftol(b[0]);
                    const i32 az = ftol(a[2]), ay = ftol(a[1]), ax = ftol(a[0]);
                    joan::orig::as_path_box(p432())->set(joan::orig::arg::i32(ax), joan::orig::arg::i32(ay), joan::orig::arg::i32(az), joan::orig::arg::i32(bx), joan::orig::arg::i32(by), joan::orig::arg::i32(bz));  // 0x0066e640
                }
                for (i32 i = count(p473()) - 1; i > last; --i) push(p432(), item(p473(), i));
                for (i32 i = count(fine); i > 0; --i) push(p432(), item(fine, i));
                for (i32 i = first; i > 0; --i) push(p432(), item(p473(), i));
            } else {
                joan::orig::as_path(p432())->assign(joan::orig::arg::mptr(p473()));  // 0x0066e5b0
            }
        } else {
            joan::orig::as_path(p432())->assign(joan::orig::arg::mptr(p473()));  // 0x0066e5b0
        }
        clear(p473());
        jo::view<u32>(jo::self<jo::Unit>(this)->i32_3ea) = 0;
        jo::self<jo::Unit>(this)->path_remaining = 0;
        sum_length();
        return count(p432()) != 0 ? 1 : 2;
    }

    // ------------------------------------------------------------------ 0x00505dd0
    u8 move_to(Vec pos, u32 a14, u32 a18, u32 a1c) {
        c_move.hit();
        if (jo::self<jo::Unit>(this)->scripted_move != 0) {
            jo::view<Vec>(jo::self<jo::Unit>(this)->move_target) = pos;
            Vec p{0.0f, 0.0f, 0.0f};
            unit_pos(this, &p);
            u32 angle = 0;
            joan::orig::as_angle(&angle)->construct_577190();  // 0x00577190
            const double dy = pos.y - p.y;
            const double dx = pos.x - p.x;
            joan::orig::as_angle(&angle)->from_xy(joan::orig::arg::f64(dx), joan::orig::arg::f64(dy));                      // 0x00577b90
            turn_to(this, joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->degrees()));               // 0x00577450
            return 1;
        }
        const u32 s18 = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->link_20e());  // 0x0050ac20
        u8* rec = nullptr;
        const i32 id = static_cast<i32>(static_cast<i16>(a1c & 0xffff));
        if (unit_missing(id) == 0) rec = unit_rec(id);
        float d30 = 0.0f;
        Vec d{0.0f, 0.0f, 0.0f};
        if (rec != nullptr) {
            d.x = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0] - jo::view<float[3]>(jo::as_unit(rec)->position_)[0];
            d.y = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1] - jo::view<float[3]>(jo::as_unit(rec)->position_)[1];
            d.z = jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[2] - jo::view<float[3]>(jo::as_unit(rec)->position_)[2];
            d30 = v3d_float(d::iat_v3d_87_length_sq, &d);  // length_sq
        }
        if ((a18 & 0xff) != 0 && s18 != 0 && rec != nullptr && joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->dead()) == 0 &&  // 0x0042d0e0
            !(d30 > global<float>(d::flt_102400))) {
            // ---- follow the leader's path (0x00505f28)
            if (count(&jo::as_unit(rec)->path) == 0) return 3;
            const float* last = back(&jo::as_unit(rec)->path);
            pos.x = d.x + last[0];
            pos.y = d.y + last[1];
            jo::view<u32>(pos.z) = jo::view<u32>(last[2]);
            if (!(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0] < pos.x) && !(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0] > pos.x) && !(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1] < pos.y) &&
                !(jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1] > pos.y))
                return 0;
            if (jo::self<jo::Unit>(this)->cell == nullptr) return 3;
            bool keep = false;
            if (jo::self<jo::Unit>(this)->name_index != 0 && jo::self<jo::Unit>(this)->task.kind == 0xe && count(p432()) != 0 && jo::self<jo::Unit>(this)->cell != nullptr &&
                jo::as_cell(zone())->tracks != nullptr)
                keep = true;
            if (!keep) joan::orig::as_squad(this)->clear();  // 0x00422f10
            jo::self<jo::Unit>(this)->wait_count = jo::self<jo::Unit>(this)->wait_limit;
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_cell(zone())->box)->contains(&pos)) != 0 && jo::self<jo::Unit>(this)->name_index == 0 &&
                jo::view<i8>(jo::as_cell(zone())->loaded) != 0) {  // 0x0060d5f0
                if (s18 == 0) return 0;
                const u8* const form = form_get(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
                void* const v38 = jo::as_shot(form)->state.sub;
                const u8* const slot = joan::orig::arg::as<const u8*>(joan::orig::placements::find(img::obj(d::g_placements), joan::orig::arg::i32(global<u32>(d::g_a57bb2)), joan::orig::as_path_space(v38)));  // 0x004a2b20
                const i32 dz = ftol(d.z), dy = ftol(d.y), dx = ftol(d.x);
                const i32 idx = static_cast<i32>(slot - static_cast<const u8*>(img::obj(d::g_placements))) / 0x34;
                joan::orig::route_around_6ac050(img::obj(d::g_placements), joan::orig::arg::i32(global<u32>(d::g_a57bb2)), idx,
                                                joan::orig::arg::i32(s18), joan::orig::arg::mptr(p432()), joan::orig::arg::mptr(&jo::as_unit(rec)->path), dx, dy, dz,
                                                jo::view<Vec>(jo::self<jo::Unit>(this)->position_).x, jo::view<Vec>(jo::self<jo::Unit>(this)->position_).y, jo::view<Vec>(jo::self<jo::Unit>(this)->position_).z,
                                                joan::orig::arg::i32(global<u32>(d::g_unit_body_r)));  // 0x006ac050
                jo::self<jo::Unit>(this)->path_remaining = 0;
                sum_length();
                return count(p432()) != 0 ? 1 : 3;
            }
            // 0x00506206
            Vec p54{0.0f, 0.0f, 0.0f};
            unit_pos(this, &p54);
            if (jo::self<jo::Unit>(this)->name_index != 0 && jo::self<jo::Unit>(this)->cell == nullptr && rd_ptr(reinterpret_cast<std::uintptr_t>(jo::self<jo::Unit>(this)->cell) + offsetof(jo::Cell, tracks)) == nullptr) return 0;
            return replan(p54, pos, a14, d::g_a3cb28, 1, fn::code_ptr(fn::dtor_508860), false);
        }
        // ---- 0x00506b14
        if (count(p432()) != 0) return 0;
        Vec pa0{0.0f, 0.0f, 0.0f};
        unit_pos(this, &pa0);
        if (joan::orig::arg::as<u8>(joan::orig::vec_equal(&pa0, &pos)) != 0) return 0;  // 0x0060ced0
        if (jo::self<jo::Unit>(this)->cell == nullptr) return 2;
        if (!(jo::self<jo::Unit>(this)->name_index != 0 && jo::self<jo::Unit>(this)->cell != nullptr && jo::as_cell(zone())->tracks != nullptr))
            joan::orig::as_squad(this)->clear();  // 0x00422f10
        jo::self<jo::Unit>(this)->wait_count = jo::self<jo::Unit>(this)->wait_limit;
        jo::view<Vec>(jo::self<jo::Unit>(this)->move_target) = pos;
        if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_cell(zone())->box)->contains(joan::orig::arg::ptr(jo::self<jo::Unit>(this)->move_target))) != 0 && jo::view<i8>(jo::as_cell(zone())->loaded) != 0) {
            // ---- direct (0x00506c09)
            u32 a4 = 0;
            if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                const u8* const fm = form_get(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
                void* const r = joan::orig::arg::as<void*>(joan::orig::as_world_cell(zone())->grid(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->layer))));  // 0x005600c0
                if (jo::as_shot(fm)->state.sub == r) a4 = jo::as_shot(form_get(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)))->state.zone;
            }
            if (jo::self<jo::Unit>(this)->name_index != 0 && jo::self<jo::Unit>(this)->cell != nullptr && jo::as_cell(zone())->tracks != nullptr) {
                // 0x005070b3
                if (jo::self<jo::Unit>(this)->name_index != 0 && jo::self<jo::Unit>(this)->cell != nullptr && jo::as_cell(zone())->tracks != nullptr) {
                    const bool b9 = type_9();
                    if (jo::self<jo::Unit>(this)->task.kind != 0xe && count(p432()) != 0) return 0;
                    if (count(p432()) > 1 && jo::self<jo::Unit>(this)->path_index < 1) return 0;
                    if (jo::as_cell(zone())->tracks == nullptr) return 0;
                    slot_move(b9, pa0, pos, a14);
                }
            } else {
                bool special = jo::self<jo::UnitType>(unit_type(this))->unit_class == 8;
                if (!special) special = jo::self<jo::UnitType>(unit_type(this))->kind == 2;
                if (!special) special = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0;  // 0x00430190
                if (special) {
                    const u8* const t = unit_type(this);
                    const float r = path_radius(t) * global<float>(d::flt_10);
                    const float y2 = rel(pos.y, &jo::Cell::origin_y), x2 = rel(pos.x, &jo::Cell::origin_x);
                    const float y1 = rel(pa0.y, &jo::Cell::origin_y), x1 = rel(pa0.x, &jo::Cell::origin_x);
                    void* const finder = joan::orig::arg::as<void*>(joan::orig::as_world_cell(zone())->grid(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->layer))));  // 0x005600c0
                    joan::orig::as_path_space(finder)->find_path(joan::orig::arg::f32(x1), joan::orig::arg::f32(y1), joan::orig::arg::f32(jo::view<u32>(pa0.z)), joan::orig::arg::f32(x2), joan::orig::arg::f32(y2), joan::orig::arg::f32(jo::view<u32>(pos.z)), joan::orig::arg::mptr(p432()), joan::orig::arg::bits(r), joan::orig::arg::i32(a4), joan::orig::arg::i32(a14), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_slot_y), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_width), joan::orig::arg::u32(u32{0x3f000000}));  // 0x0066cb40
                } else {
                    const float lim = static_cast<float>(static_cast<i32>(global<u32>(d::g_unit_body_r) << 1));
                    const float y2 = rel(pos.y, &jo::Cell::origin_y), x2 = rel(pos.x, &jo::Cell::origin_x);
                    const float y1 = rel(pa0.y, &jo::Cell::origin_y), x1 = rel(pa0.x, &jo::Cell::origin_x);
                    void* const finder = joan::orig::arg::as<void*>(joan::orig::as_world_cell(zone())->grid(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->layer))));  // 0x005600c0
                    joan::orig::as_path_space(finder)->find_path(joan::orig::arg::f32(x1), joan::orig::arg::f32(y1), joan::orig::arg::f32(jo::view<u32>(pa0.z)), joan::orig::arg::f32(x2), joan::orig::arg::f32(y2), joan::orig::arg::f32(jo::view<u32>(pos.z)), joan::orig::arg::mptr(p432()), joan::orig::arg::bits(lim), joan::orig::arg::i32(a4), joan::orig::arg::i32(a14), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_slot_y), joan::orig::arg::u32(jo::self<jo::Unit>(this)->form_width), joan::orig::arg::u32(u32{0xbf800000}));  // 0x0066cb40
                }
                const float zy = static_cast<float>(jo::as_cell(zone())->origin_y);
                const float zx = static_cast<float>(jo::as_cell(zone())->origin_x);
                joan::orig::as_path(p432())->translate(joan::orig::arg::f32(zx), joan::orig::arg::f32(zy), joan::orig::arg::f32(i32{0}));  // 0x0066fa00
            }
            // 0x005072ab
            jo::self<jo::Unit>(this)->path_remaining = 0;
            sum_length();
            return count(p432()) != 0 ? 1 : 2;
        }
        // ---- 0x005073d5
        if (jo::self<jo::Unit>(this)->name_index != 0 && jo::self<jo::Unit>(this)->cell != nullptr && jo::as_cell(zone())->tracks != nullptr) {
            const bool b9 = type_9();
            if (jo::self<jo::Unit>(this)->task.kind != 0xe && count(p432()) != 0) return 0;
            if (count(p432()) > 1 && jo::self<jo::Unit>(this)->path_index < 1) return 0;
            slot_move(b9, pa0, pos, a14);
            return 1;
        }
        return replan(pa0, pos, a14, d::g_a3cae0, 2, fn::code_ptr(fn::dtor_508840), true);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_11", name, addr, fn)
B("Unit::move_to_505dd0", 0x00505dd0, &H::move_to);
#undef B

}  // namespace game_b110_11
}  // namespace

// ---------------------------------------- from modules/game_b109_09.cpp (game/b109_09)
namespace {
namespace game_b109_09 {

using u32 = std::uint32_t;

// Game virtual (thiscall) through the object's vtable.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}

JOAN_COUNTER(c_launch, "Missile::launch_507fc0");

void* const kUnits = obj(d::g_items);
void* const kEmitters = obj(d::g_physic_sys);

// ---------------------------------------------------------------------------------------------
// 0x00507fc0 helper: effect slot + emitter for the model's +0x86 block.
void missile_emitter(void* self, void* model, u32 kind, img::Addr g_new, u32 mode, img::Addr g_reuse) {
    jo::view<u32>(jo::as_map_object(model)->actor->kind) = kind;
    joan::en3d::unit_index_actor_kind(jo::as_map_object(model)->actor);  // [Debug] UnitIndex
    jo::as_map_object(model)->actor->owner = self;
    if (jo::view<u32>(jo::as_map_object(model)->actor->record) == 0) {
        jo::as_missile(self)->model_id =
            joan::orig::arg::as<u32>(joan::orig::as_army(kEmitters)->fire_shot(joan::orig::arg::mptr(jo::view<void*>(jo::as_map_object(model)->actor)), joan::orig::arg::u32(global<u32>(g_new)), joan::orig::arg::u32(mode), 0u, 2u, 0u));  // 0x006adbf0
        (void)joan::orig::as_players(kEmitters)->list.ptr_at(joan::orig::arg::i32(jo::as_missile(self)->model_id));                 // 0x006af310
    } else {
        joan::orig::as_anim_table(kEmitters)->restart_6af370(joan::orig::arg::u32(jo::view<u32>(jo::as_map_object(model)->actor->record)), joan::orig::arg::u32(global<u32>(g_reuse)), joan::orig::arg::u32(mode), 0u, 2u, 0u);  // 0x006af370
        jo::as_missile(self)->model_id = jo::view<u32>(jo::as_map_object(model)->actor->record);
        (void)joan::orig::as_players(kEmitters)->list.ptr_at(joan::orig::arg::i32(jo::as_missile(self)->model_id));  // 0x006af310
    }
}
void* model_prepare(void* self) {
    void* const model = joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(jo::as_missile(self)->object_id));  // 0x00425d20
    vcall<void>(model, 0x18, 2);
    vcall<void>(model, 0x1c);
    return model;
}
i32 owner_from_source(void* source) {
    const i16* const id = joan::orig::arg::as<i16*>(joan::orig::as_unit_data(jo::as_unit(source)->modifiers)->slot_e());  // 0x00497580
    return joan::orig::arg::as<i32>(joan::orig::as_units(kUnits)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(*id))));         // 0x0049ea90
}
i32 owner_from_kind(void* source) {
    u8* const k = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(source)->type_id))));  // 0x0042d0a0
    return joan::orig::arg::as<i32>(joan::orig::as_units(kUnits)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::UnitType>(k)->item_stack))));                     // 0x0049ea90
}

struct H {
    // 0x00507fc0
    void launch_507fc0(u8* source, i32 kind_arg) {
        c_launch.hit();
        void* const self = this;
        jo::view<i32>(jo::self<jo::Missile>(this)->shooter_unit_id) = jo::as_unit(source)->id;
        jo::self<jo::Missile>(this)->shooter_record = jo::view<u32>(jo::as_actor(jo::as_unit(source)->actor)->record);
        switch (static_cast<i32>(static_cast<i16>(kind_arg)) - 13) {
            case 0:
            case 3:
                jo::self<jo::Missile>(this)->state = 0;
                break;
            case 1:
            case 4:
                jo::self<jo::Missile>(this)->state = 1;
                break;
            case 2:
            case 5:
                jo::self<jo::Missile>(this)->state = 2;
                break;
            case 6:
                jo::self<jo::Missile>(this)->state = 5;
                break;
            case 7:
                jo::self<jo::Missile>(this)->state = 4;
                break;
            default:
                break;
        }
        joan::orig::as_missile(self)->load_stats();  // 0x00498e90
        void* model = nullptr;
        switch (jo::self<jo::Missile>(this)->state) {
            case 0: {
                jo::view<i32>(jo::self<jo::Missile>(this)->object_id) = owner_from_source(source);
                const i32 r = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::as_unit(source)->modifiers)->sum(0x10, static_cast<i32>(jo::as_unit(source)->id)));  // 0x00497e10
                jo::view<i32>(jo::self<jo::Missile>(this)->max_range) = static_cast<i32>(static_cast<u32>(r) * static_cast<u32>(jo::view<i32>(jo::self<jo::Missile>(this)->max_range))) / 100;
                model = model_prepare(self);
                missile_emitter(self, model, 6, d::g_throw_arw_m, 2, d::g_throw_arw_m);
                break;
            }
            case 1:
                jo::view<i32>(jo::self<jo::Missile>(this)->object_id) = owner_from_source(source);
                model = model_prepare(self);
                missile_emitter(self, model, 7, d::g_throw_proj_m, 3, d::g_throw_proj_m);
                break;
            case 2:
                jo::view<i32>(jo::self<jo::Missile>(this)->object_id) = owner_from_source(source);
                model = model_prepare(self);
                missile_emitter(self, model, 8, d::g_a39d50, 4, d::g_throw_proj_m);
                break;
            case 3:
                jo::view<i32>(jo::self<jo::Missile>(this)->object_id) = owner_from_kind(source);
                model = model_prepare(self);
                missile_emitter(self, model, 9, d::g_throw_stm_m, 5, d::g_throw_stm_m);
                break;
            case 4:
                jo::view<i32>(jo::self<jo::Missile>(this)->object_id) = owner_from_kind(source);
                model = model_prepare(self);
                missile_emitter(self, model, 9, d::g_throw_stc_m, 5, d::g_throw_stc_m);
                break;
            case 5:
                jo::view<i32>(jo::self<jo::Missile>(this)->object_id) = owner_from_kind(source);
                model = model_prepare(self);
                missile_emitter(self, model, 9, d::g_throw_can_m, 0x12, d::g_throw_can_m);
                break;
            default:
                return;  // unreachable (see header): the original would use an uninitialised local
        }
        jo::as_map_object(model)->actor->owner = self;
        jo::view<u32>(jo::as_map_object(model)->actor->record) = jo::self<jo::Missile>(this)->model_id;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_09", name, addr, fn)
B("Missile::launch_507fc0", 0x00507fc0, &H::launch_507fc0);
#undef B

}  // namespace game_b109_09
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {


using u32 = std::uint32_t;

JOAN_COUNTER(c_distance_band, "Unit::distance_band");
JOAN_COUNTER(c_release_slot, "Cell::release_slot");

struct H {

    // ------------------------------------------------------------------------ 0x00508880
    i32 distance_band() {
        c_distance_band.hit();
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))) != 0) return 4;  // 0x00428b70
        if (global<i32>(d::g_player_unit_recno) == static_cast<i32>(jo::self<jo::Unit>(this)->id)) return 1;
        u8* const sel = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))));  // 0x0041b3c0
        float x, y;
        if (global<i32>(d::g_control_mode) == 4) {
            u32 buf[10];
            joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(buf)));  // 0x006140a0
            std::memcpy(&x, &buf[1], 4);
            std::memcpy(&y, &buf[2], 4);
        } else {
            x = jo::view<float[3]>(jo::as_unit(sel)->position_)[0];
            y = jo::view<float[3]>(jo::as_unit(sel)->position_)[1];
        }
        const float dx = x - jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[0];
        const float dx2 = dx * dx;
        const float dy = y - jo::view<float[3]>(jo::self<jo::Unit>(this)->position_)[1];
        const float dy2 = dy * dy;
        const float d = dx2 + dy2;
        if (!(d > global<float>(d::flt_4000000))) return 1;  // 4e6
        if (!(d > global<float>(d::flt_16000000))) return 2;  // 16e6
        if (!(d > global<float>(d::flt_64000000))) return 3;  // 64e6
        return 4;
    }

    // ------------------------------------------------------------------------ 0x0050c430
    void release_slot(i16 a, i16 b) {
        c_release_slot.hit();
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return;
        joan::orig::as_slots(jo::self<jo::Unit>(this)->modifiers)->free(static_cast<i32>(a), static_cast<i32>(b));  // 0x004971e0
        if (static_cast<i32>(*jo::view<i16*>(jo::self<jo::Unit>(this)->modifiers)) != static_cast<i32>(a)) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return;  // 0x0042d690
        if (static_cast<i32>(b) == 0) {
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
        } else if (static_cast<i32>(b) == 1) {
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));  // 0x0050a480
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Unit::distance_band", 0x00508880, &H::distance_band);
B("Cell::release_slot", 0x0050c430, &H::release_slot);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b87_03.cpp (game/b87_03)
namespace {
namespace game_b87_03 {


constexpr img::Addr kUnits(d::g_units);
constexpr img::Addr kSides(d::g_nations);

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }
bool unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
void play(void* unit, std::int32_t code, std::int32_t a, std::int32_t b) {
    joan::orig::as_unit(unit)->set_action(joan::orig::arg::i32(code), joan::orig::arg::i32(a), joan::orig::arg::i32(b));  // 0x00501190
}
void* side_get(std::int32_t side) { return joan::orig::as_dynarray(img::obj(kSides))->ptr_at(joan::orig::arg::i32(side)); }  // 0x0047b600
bool side_invalid(std::int32_t side) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kSides))->is_null(joan::orig::arg::i32(side))) != 0; }  // 0x00484870

JOAN_COUNTER(c_play_idle, "Unit::play_idle");
JOAN_COUNTER(c_set_side, "Unit::set_side");
JOAN_COUNTER(c_level_state, "Unit::set_level_state");

struct H {
    // ------------------------------------------------------------------------ 0x00508b90
    void play_idle(std::int32_t a, std::int32_t b) {
        c_play_idle.hit();
        if (joan::orig::arg::as<std::int16_t>(joan::orig::as_unit(this)->is_kind_special()) != 0) return;  // 0x00508ab0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->mounted()) != 0) {       // 0x00430190
            play(this, 0x43, a, b);
            if (!unit_invalid(jo::self<jo::Unit>(this)->mount_id))
                joan::orig::as_unit(unit_get(jo::self<jo::Unit>(this)->mount_id))->random_stance(joan::orig::arg::u32(a), joan::orig::arg::u32(b));  // 0x00429ec0
            return;
        }
        if (jo::self<jo::Unit>(this)->rider_id != 0) {
            joan::orig::as_unit(this)->random_stance(joan::orig::arg::u32(a), joan::orig::arg::u32(b));
            return;
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->get_1f9()) != 0) {  // 0x00508f80
            if (jo::self<jo::Unit>(this)->anim_kind == 5 || jo::self<jo::Unit>(this)->anim_kind == 0x1a) play(this, 6, a, b);
            if (jo::self<jo::Unit>(this)->idle_time == 1 || jo::self<jo::Unit>(this)->idle_time == 2)
                play(this, 4, a, b);
            else
                play(this, 1, a, b);
            if (!unit_invalid(jo::self<jo::Unit>(this)->rider_ref)) play(unit_get(jo::self<jo::Unit>(this)->rider_ref), 0x68, 0, 1);
            return;
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->get_1f5()) != 0) {  // 0x004f2f60
            play(this, 0x68, a, b);
            if (!unit_invalid(jo::self<jo::Unit>(this)->mount_ref)) play(unit_get(jo::self<jo::Unit>(this)->mount_ref), 1, 0, 1);
            return;
        }
        if (jo::self<jo::Unit>(this)->held_item_id != 0) {
            switch (sx16(jo::self<jo::Unit>(this)->anim_kind)) {  // table 0x508ea6 / 0x508e9a
                case 0x59: case 0x5d: case 0x5e: case 0x63: case 0x66:
                    play(this, 0x59, a, b);
                    break;
                case 0x5a: case 0x5b: case 0x5f: case 0x61:
                    play(this, 0x5a, a, b);
                    break;
                default:
                    break;
            }
            return;
        }
        if (jo::self<jo::Unit>(this)->anim_kind == 5 || jo::self<jo::Unit>(this)->anim_kind == 0x1a) {
            play(this, 6, a, b);
            return;
        }
        if (jo::self<jo::Unit>(this)->anim_kind == 8 || jo::self<jo::Unit>(this)->anim_kind == 0x1d) return;
        if (jo::self<jo::Unit>(this)->anim_kind == 6) return;
        if (jo::self<jo::Unit>(this)->idle_time == 1 || jo::self<jo::Unit>(this)->idle_time == 2) {
            play(this, 4, a, b);
            return;
        }
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(this)->byte_2e1())) != 0 &&  // 0x00432b90
            jo::self<jo::Unit>(this)->prisoner_pose != 0) {
            play(this, 7, a, b);
            return;
        }
        play(this, 1, a, b);
    }

    // ------------------------------------------------------------------------ 0x00509250
    void set_side(std::int32_t side_arg) {
        c_set_side.hit();
        const std::int16_t side = static_cast<std::int16_t>(side_arg);
        if (jo::self<jo::Unit>(this)->side == side) return;
        if (!side_invalid(sx16(jo::self<jo::Unit>(this)->side))) {
            joan::orig::as_nation(side_get(sx16(jo::self<jo::Unit>(this)->side)))->remove_unit(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004b5cc0
            std::uint32_t v = 0u - static_cast<std::uint32_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hit_points_max()));   // 0x00523320
            v *= static_cast<std::uint32_t>(joan::orig::arg::as<std::int32_t>(joan::orig::unit_class(joan::orig::arg::i16(sx16(jo::self<jo::Unit>(this)->type_id)))));  // 0x00527dd0
            void* const st = side_get(sx16(jo::self<jo::Unit>(this)->side));
            joan::orig::as_nation(st)->add_1389(static_cast<std::int32_t>(v));  // 0x004b5520
            std::int32_t* const n = &jo::view<std::int32_t>(jo::as_nation(side_get(sx16(jo::self<jo::Unit>(this)->side)))->unit_count);
            *n = static_cast<std::int32_t>(static_cast<std::uint32_t>(*n) - 1u);
        }
        jo::self<jo::Unit>(this)->side = side;
        if (!side_invalid(sx16(jo::self<jo::Unit>(this)->side))) {
            joan::orig::as_nation(side_get(sx16(jo::self<jo::Unit>(this)->side)))->update_unit(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004b5e40
            std::uint32_t v = static_cast<std::uint32_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->hit_points_max()));
            v *= static_cast<std::uint32_t>(joan::orig::arg::as<std::int32_t>(joan::orig::unit_class(joan::orig::arg::i16(sx16(jo::self<jo::Unit>(this)->type_id)))));
            void* const st = side_get(sx16(jo::self<jo::Unit>(this)->side));
            joan::orig::as_nation(st)->add_1389(static_cast<std::int32_t>(v));
            std::int32_t* const n = &jo::view<std::int32_t>(jo::as_nation(side_get(sx16(jo::self<jo::Unit>(this)->side)))->unit_count);
            *n = static_cast<std::int32_t>(static_cast<std::uint32_t>(*n) + 1u);
        }
    }

    // ------------------------------------------------------------------------ 0x005089e0
    void set_level_state(std::int32_t v) {
        c_level_state.hit();
        if (v == jo::self<jo::Unit>(this)->rank) return;
        if (v >= 3 && jo::self<jo::Unit>(this)->leader_id != 0) joan::orig::as_member(this)->release();  // 0x0042fcb0
        jo::self<jo::Unit>(this)->rank = v;
        if (v >= 3 && joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->leads()) == 0) joan::orig::as_unit(this)->sync_shadow();  // 0x0044a660, 0x00447600
        if (!side_invalid(sx16(jo::self<jo::Unit>(this)->side)))
            joan::orig::as_nation(side_get(sx16(jo::self<jo::Unit>(this)->side)))->update_unit(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004b5e40
    }

    // ------------------------------------------------------------------------ 0x0050abd0
    void set_heading(float v) { jo::self<jo::Unit>(this)->heading_goal = v; }  // a float argument (XMM on x64; the facade declares it float)
};

#define B87(name, addr, fn) JOAN_REPLACE("game/b87_03", name, addr, fn)
B87("Unit::play_idle", 0x00508b90, &H::play_idle);
B87("Unit::set_side", 0x00509250, &H::set_side);
B87("Unit::set_level_state", 0x005089e0, &H::set_level_state);
B87("Unit::set_heading", 0x0050abd0, &H::set_heading);
#undef B87

}  // namespace game_b87_03
}  // namespace

// ---------------------------------------- from modules/game_b88_04.cpp (game/b88_04)
namespace {
namespace game_b88_04 {

using joan::orig::helpers::addr_of;

struct H {
    // ------------------------------------------------------------ 0x00508ab0 (ret 0)
    std::uintptr_t is_kind_special() {
        const std::uint32_t k = static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind)) - 1u;
        if (k > 0x59u) return addr_of(this) & 0xffff0000u;  // eax = this, xor ax,ax
        switch (k + 1u) {
            case 1: case 2: case 3: case 4: case 5: case 6: case 8: case 67: case 89: case 90:
                return 1;  // index byte 0: eax = 0, mov ax,1
            default:
                return 0;  // index byte 1: eax = 1, xor ax,ax
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_04", name, addr, fn)
B("Unit::is_kind_special", 0x00508ab0, &H::is_kind_special);
#undef B

}  // namespace game_b88_04
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {


using u32 = std::uint32_t;

void* const kRandom = obj(d::g_misc);
void* const kTypes = obj(d::g_item_res);
void* const kTerrain = obj(d::g_types);

JOAN_COUNTER(c_idle_anim, "Unit::pick_idle_anim");
JOAN_COUNTER(c_attack_value, "Unit::attack_value");
JOAN_COUNTER(c_defence_value, "Unit::defence_value");

i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540
i32 stat_497e10(void* army, i32 kind, i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_modifiers(army)->sum(joan::orig::arg::i32(kind), joan::orig::arg::i32(id))); }  // 0x00497e10
// type(word q[0]).value(short q[+4]) (0x00425ce0 / 0x004a41c0); the q[+4] push happens first.
i32 type_value(const void* q) {
    const i32 arg = static_cast<i32>(jo::as_inv_item(q)->level);
    void* const type = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::as_inv_item(q)->type)));  // 0x00425ce0
    return joan::orig::arg::as<i32>(joan::orig::as_mod_table(type)->value_own(joan::orig::arg::i32(arg)));                            // 0x004a41c0
}
// Difficulty scaling shared by 0x005104f0 / 0x00510bf0 (switch on (char)0xa57fc2, 0..3).
float difficulty_scale(void* unit, float v) {
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(global<i16>(d::g_player_unit_recno)))) != 0) {  // 0x005094d0
        const u32 level = static_cast<u32>(static_cast<i32>(global<std::int8_t>(d::g_difficulty)));
        switch (level) {
            case 0: v = v * global<float>(d::g_a391ac); break;
            case 1: v = v * global<float>(d::g_a3918c); break;
            case 2: v = v * global<float>(d::g_a391b0); break;
            case 3: v = v * global<float>(d::g_a39194); break;
            default: break;
        }
    }
    return v;
}

struct H {
    // ------------------------------------------------------------------------ 0x00509000
    void pick_idle_anim(i32) {
        c_idle_anim.hit();
        if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(this)->is_kind_40_42())) != 0) return;  // 0x00508fb0
        if (static_cast<i32>(jo::self<jo::UnitType>(joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTerrain)->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id))))->unit_class) == 8 ||  // 0x0042d0a0
            static_cast<i32>(jo::self<jo::UnitType>(joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTerrain)->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id))))->unit_class) == 9)
            return;
        if (jo::self<jo::Unit>(this)->anim_set == nullptr) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0) return;  // 0x00430190
        if (jo::view<u32>(jo::self<jo::Unit>(this)->rider_id) != 0) return;
        if (jo::view<u32>(jo::self<jo::Unit>(this)->held_item_id) != 0) return;
        i32 n = 1;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(i32{0x41})) != 0) ++n;  // 0x00416dc0
        if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(i32{0x42})) != 0) ++n;
        n = random(n);
        switch (n) {
            case 0: joan::orig::as_unit(this)->set_action(i32{0x40}, i32{0}, i32{1}); break;  // 0x00501190
            case 1: joan::orig::as_unit(this)->set_action(i32{0x41}, i32{0}, i32{1}); break;
            case 2: joan::orig::as_unit(this)->set_action(i32{0x42}, i32{0}, i32{1}); break;
            default: joan::orig::as_unit(this)->set_action(i32{0x40}, i32{0}, i32{1}); break;
        }
        jo::view<u16>(jo::self<jo::Unit>(this)->attack_chance) = static_cast<u16>(jo::view<u16>(jo::self<jo::Unit>(this)->attack_chance) << 1);
    }

    // ------------------------------------------------------------------------ 0x005104f0
    float attack_value() {
        c_attack_value.hit();
        const Fpu fpu{true};
        u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry))));  // 0x004168d0
        const float base = static_cast<float>(jo::as_anim_group_rec(a)->i32_1c);
        float c = 0.0f;
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            void* q = joan::orig::arg::as<void*>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->slot(i32{0}));  // 0x004974f0
            if (q != nullptr) c = static_cast<float>(fpu.add(type_value(q), c));
            q = joan::orig::arg::as<void*>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->slot(i32{1}));
            if (q != nullptr) c = static_cast<float>(fpu.add(type_value(q), c));
            const i32 t = stat_497e10(jo::self<jo::Unit>(this)->modifiers, 7, static_cast<i32>(jo::self<jo::Unit>(this)->id)) + 0x64;
            c = static_cast<float>(fpu.mul(fpu.mul(t, global<float>(d::flt_0_01)), c));
        }
        const float d = static_cast<float>(
            fpu.add(fpu.sub(c, static_cast<double>(global<i32>(d::g_a3cd14))), global<float>(d::flt_100)));
        const i32 m = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->stat_3()) * static_cast<i32>(global<i16>(d::g_a3cce2));  // 0x005237e0
        float v = static_cast<float>(fpu.div(fpu.mul(fpu.add(m, d), base), global<float>(d::flt_100)));
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            const i32 t = stat_497e10(jo::self<jo::Unit>(this)->modifiers, 8, static_cast<i32>(jo::self<jo::Unit>(this)->id)) + 0x64;
            v = static_cast<float>(fpu.mul(fpu.mul(t, global<float>(d::flt_0_01)), v));
        }
        return difficulty_scale(this, v);
    }

    // ------------------------------------------------------------------------ 0x00510bf0
    float defence_value() {
        c_defence_value.hit();
        const Fpu fpu{true};
        float c = 0.0f;
        i32 both = 0;
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            void* q = joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->slot_a());  // 0x00497560
            if (q != nullptr) c = static_cast<float>(type_value(q));
            q = joan::orig::arg::as<void*>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->slot_e());  // 0x00497580
            if (q != nullptr) {
                c = static_cast<float>(fpu.add(type_value(q), c));
                both = 1;
            }
            const i32 t = stat_497e10(jo::self<jo::Unit>(this)->modifiers, 7, static_cast<i32>(jo::self<jo::Unit>(this)->id)) + 0x64;
            c = static_cast<float>(fpu.mul(fpu.mul(t, global<float>(d::flt_0_01)), c));
        }
        const float div = both != 0 ? 200.0f : 100.0f;
        const i32 m = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->stat_4()) * static_cast<i32>(global<i16>(d::g_a3cca8)) + 0x64;  // 0x00523860
        float v = static_cast<float>(fpu.div(fpu.mul(m, c), div));
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            const i32 t = stat_497e10(jo::self<jo::Unit>(this)->modifiers, 8, static_cast<i32>(jo::self<jo::Unit>(this)->id)) + 0x64;
            v = static_cast<float>(fpu.mul(fpu.mul(t, global<float>(d::flt_0_01)), v));
        }
        return difficulty_scale(this, v);
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Unit::pick_idle_anim", 0x00509000, &H::pick_idle_anim);
B92("Unit::attack_value", 0x005104f0, &H::attack_value);
B92("Unit::defence_value", 0x00510bf0, &H::defence_value);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

using u32 = std::uint32_t;


void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_has_type_b, "Obj::has_type_b_50d270");

struct H {
    // ------------------------------------------------------------------ 0x00509450
    i16 lookup_509450(i16 id) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->nation());  // 0x00509400
        if (p == nullptr) return 0;
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(id));  // 0x0041b3c0
        void* const r = joan::orig::arg::as<void*>(joan::orig::as_unit(u)->nation());                             // 0x00509400
        if (r == nullptr) return 0;
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_player(p)->relation(static_cast<i32>(jo::as_nation(r)->id)));  // 0x004b42e0
        return jo::view<i16[2]>(jo::as_relation(e)->opinion)[0];
    }

    // ------------------------------------------------------------------ 0x0050d270
    i32 has_type_b_50d270() {
        c_has_type_b.hit();
        void* const list = &jo::self<jo::Unit>(this)->upgrades;
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(list)->count()); i > 0; --i) {     // 0x00531360
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_upgrade_list(list)->get(joan::orig::arg::i32(i)));            // 0x005313a0
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_list_entry(e)->id))));  // 0x0050d240
            if (static_cast<i32>(jo::self<jo::UpgradeItem>(t)->type) != 0xb) continue;
            if (static_cast<i32>(jo::as_upgrade_list_entry(e)->cur) > 0) return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x00510170
    u32 call_1c1_510170(u32 arg) {
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->count_of_kind(joan::orig::arg::i32(arg)));  // 0x004987d0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("Obj::lookup_509450", 0x00509450, &H::lookup_509450);
B("Obj::has_type_b_50d270", 0x0050d270, &H::has_type_b_50d270);
B("Obj::call_1c1_510170", 0x00510170, &H::call_1c1_510170);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {


using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");
struct S24 {
    u32 d[6];
};
static_assert(sizeof(S24) == 24, "S24");

void* const kUnitTypes = obj(d::g_item_res);
void* const kSfx = obj(d::g_sfx_player);
void* const kModels = obj(d::g_physic_sys);

JOAN_COUNTER(c_heal_group, "Unit::heal_group");
JOAN_COUNTER(c_emit, "Unit::emit_6afb10");
JOAN_COUNTER(c_link_20e, "Unit::link_20e");

struct H {
    // ------------------------------------------------------------------------ 0x005097d0
    float stat_pct() {
        const Fpu fpu{true};
        const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max());  // 0x00523320
        return static_cast<float>(fpu.div(static_cast<double>(v), global<float>(d::flt_100)));  // 100.0f
    }

    // ------------------------------------------------------------------------ 0x0050ac20
    u32 link_20e() {
        c_link_20e.hit();
        if (jo::self<jo::Unit>(this)->actor == nullptr) return 0;
        if (jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) return 0;
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
        if (p == nullptr) return 0;
        return jo::as_shot(p)->state.zone;
    }

    // ------------------------------------------------------------------------ 0x0050ba00
    i32 heal_group(u32 type_arg, i32 force_ok, i32 always) {
        c_heal_group.hit();
        const i32 type = static_cast<i32>(static_cast<i16>(type_arg));
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(this)->dead())) != 0 && force_ok == 0) return 0;  // 0x0042d0e0
        i32 result = 0;
        i32 lim = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->value_149());  // 0x00523700
        if (always != 0)
            result = 1;
        else if (jo::self<jo::Unit>(this)->mana < lim)
            result = 1;
        else
            return 0;
        const i32 amount = jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(type))))->damage;  // 0x00425ce0
        const i32 own = amount + jo::self<jo::Unit>(this)->mana < lim ? amount + jo::self<jo::Unit>(this)->mana : lim;
        joan::orig::as_unit(this)->set_145(joan::orig::arg::i32(own));  // 0x004fe5d0
        if (jo::self<jo::Unit>(this)->group != nullptr) {
            for (i32 i = static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->count); i > 0; --i) {
                const i32 id = static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i - 1]);
                if (joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(id)) != 0) continue;  // 0x00428b70
                const i32 id2 = static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i - 1]);
                u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id2)));  // 0x0041b3c0
                if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead())) != 0) continue;  // 0x0042d0e0
                lim = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->value_149());                              // 0x00523700
                if (jo::as_unit(u)->mana < lim) result = 1;
                const i32 half = amount / 2;
                const i32 x = half + jo::as_unit(u)->mana < lim ? half + jo::as_unit(u)->mana : lim;
                joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(x));  // 0x004fe5d0
            }
        }
        if (global<i32>(d::g_player_unit_recno) == static_cast<i32>(jo::self<jo::Unit>(this)->id)) {
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(type)));  // 0x00425ce0
            if (jo::view<u32>(jo::as_item_def(t)->sounds[0]) != 0) {
                if (global<float>(d::g_game_speed) > global<float>(d::k_9cf5c4)) {
                    const V3 pos = jo::view<V3>(jo::self<jo::Unit>(this)->position_);
                    joan::orig::as_sound_list(kSfx)->play_at(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(t)->sounds[0])), pos);  // 0x004950a0
                } else {
                    const char* const s = static_cast<const char*>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->get(joan::orig::arg::i16(// 0x004d0820
                                                static_cast<i32>(jo::view<i16[2]>(jo::as_item_def(t)->sounds[0])[0]))));
                    joan::orig::as_sound(kSfx)->play_def(jo::as_wave_res(s)->name);  // 0x004ceef0 (+6 on x86: the WaveRes name behind its vtable pointer)
                }
            }
        }
        return result;
    }

    // ------------------------------------------------------------------------ 0x00510220
    u32 emit_6afb10(i8 flag) {
        c_emit.hit();
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
        u8 buf[0x40];
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_actor(jo::self<jo::Unit>(this)->actor)->matrix(joan::orig::arg::mptr(static_cast<void*>(buf))));  // 0x005ace10
        const S24 s = jo::view<S24>(jo::self<jo::UnitType>(p)->shape_box);
        return joan::orig::arg::as<u32>(joan::orig::as_players(kModels)->assign_matches(s, m, i32{-1}, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i32>(flag))), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0}))));  // 0x006afb10
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Unit::stat_pct", 0x005097d0, &H::stat_pct);
B94("Unit::link_20e", 0x0050ac20, &H::link_20e);
B94("Unit::heal_group", 0x0050ba00, &H::heal_group);
B94("Unit::emit_6afb10", 0x00510220, &H::emit_6afb10);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b106_09.cpp (game/b106_09)
namespace {
namespace game_b106_09 {

using u32 = std::uint32_t;


JOAN_COUNTER(c_melee_damage, "Unit::melee_damage_509950");

struct H {
    // ------------------------------------------------------------------------ 0x00509950
    float melee_damage(u32 att_id, i32 pct) {
        c_melee_damage.hit();
        const Fpu fpu{true};
        void* const a = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(att_id));  // 0x0041b3c0
        float dmg = 0.0f;  // [ebp-0xc]; every path assigns it before use (init silences MSVC C4701)
        bool direct = false;
        if (jo::as_unit(a)->modifiers != nullptr) {
            const i32 roll = joan::orig::as_misc(obj(d::g_misc))->random(0x64);  // 0x00590540
            const i32 skill = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::as_unit(a)->modifiers)->sum(0x14, static_cast<i32>(jo::as_unit(a)->id)));  // 0x00497e10
            if (roll < skill) {
                dmg = static_cast<float>(jo::self<jo::Unit>(this)->hit_points);
                direct = true;
            }
        }
        if (!direct) {
            float t10 = joan::orig::arg::as<float>(joan::orig::as_unit(a)->attack_value_5104f0());  // 0x005104f0
            const i32 i20 = static_cast<i32>(jo::as_unit(a)->stats[7]) + 0x64 - global<i32>(d::g_unit_mor_nor);
            const float t24 = static_cast<float>(fpu.div(static_cast<double>(i20), 100.0));
            const float m = joan::orig::arg::as<float>(joan::orig::as_unit(a)->armour_factor());  // 0x00510750
            t10 = m * t24 * t10;
            const i32 i28 = static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->stats[4])) *
                                             static_cast<u32>(global<i32>(d::g_a3cc00)));
            const float t14 = static_cast<float>(i28);
            const float t2c = static_cast<float>(fpu.sub(100.0, static_cast<double>(pct)));
            const i32 i30 = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->level_sum());  // 0x005238c0
            double q = fpu.mul(static_cast<double>(i30), t2c);
            q = fpu.mul(q, static_cast<double>(static_cast<i32>(global<i16>(d::g_a3cce0))));
            q = fpu.div(q, 100.0);
            q = fpu.add(q, t14);
            float t18 = static_cast<float>(q);
            const i32 i38 = static_cast<i32>(jo::self<jo::Unit>(this)->stats[7]) + 0x64 - global<i32>(d::g_unit_mor_nor);
            t18 = static_cast<float>(fpu.div(fpu.mul(static_cast<double>(i38), t18), 100.0));
            const float v = (t10 - t18) / 2.0f;
            float t3c;
            if (!(v >= 0.0f)) {
                t3c = 0.0f;
            } else {
                t3c = (t10 - t18) / 2.0f;
            }
            dmg = t10 / 2.0f + t3c;
            dmg = joan::orig::arg::as<float>(joan::orig::as_unit(this)->health_factor()) * dmg;  // 0x00510e80
            if (static_cast<i32>(jo::as_unit(a)->id) == global<i32>(d::g_player_unit_recno)) {
                if (jo::as_unit(a)->crit_ready != 0) dmg = dmg + dmg;
                const i16 mul = joan::orig::arg::as<i16>(joan::orig::as_player(obj(d::g_player_unit_recno))->byte_9d());  // 0x00509d40
                dmg = static_cast<float>(fpu.mul(static_cast<double>(mul), dmg));
            }
        }
        // 0x509af4
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) == 0) {  // 0x00428b70
            if (global<i32>(d::g_mission) == 8) {
                {
                    const i32 s = jo::as_unit(a)->side;
                    void* const p = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));
                    if (s != static_cast<i32>(jo::as_unit(p)->side)) {
                        const float k = static_cast<float>(
                            fpu.div(static_cast<double>(global<i32>(d::g_unit_f_attack)), 250.0));  // 0x009dd6d0
                        dmg = k * dmg;
                    }
                }
                {
                    const i32 s = jo::self<jo::Unit>(this)->side;
                    void* const p = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));
                    if (s != static_cast<i32>(jo::as_unit(p)->side)) {
                        const float k = static_cast<float>(
                            fpu.div(static_cast<double>(global<i32>(d::g_unit_f_defend)), 100.0));
                        dmg = k * dmg;
                    }
                }
            }
            {
                const i32 s = jo::as_unit(a)->side;
                void* const p = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));
                if (s == static_cast<i32>(jo::as_unit(p)->side)) {
                    const float k =
                        static_cast<float>(fpu.div(static_cast<double>(global<i32>(d::g_unit_f_attack)), 100.0));
                    dmg = k * dmg;
                }
            }
            {
                const i32 s = jo::self<jo::Unit>(this)->side;
                void* const p = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));
                if (s == static_cast<i32>(jo::as_unit(p)->side)) {
                    const float k =
                        static_cast<float>(fpu.div(static_cast<double>(global<i32>(d::g_unit_f_defend)), 100.0));
                    dmg = k * dmg;
                }
            }
        }
        // 0x509bdd
        void* const leader = joan::orig::arg::as<void*>(joan::orig::as_unit(a)->leader());  // 0x004216d0
        if (leader != nullptr && leader != a) {
            i32 lv = jo::as_unit(leader)->stats[3];
            if (!(lv < 0x64)) lv = 0x64;
            const float k = static_cast<float>(fpu.div(static_cast<double>(lv + 0x64), 100.0));
            dmg = k * dmg;
        }
        const i32 hp = ftol(fpu.sub(static_cast<double>(jo::self<jo::Unit>(this)->hit_points), dmg));
        joan::orig::as_unit(this)->set_value_135(joan::orig::arg::i32(hp));  // 0x004fe510
        if (static_cast<i32>(jo::as_unit(a)->id) == global<i32>(d::g_player_unit_recno)) {
            if (jo::as_unit(a)->crit_ready != 0) jo::as_unit(a)->crit_hits = static_cast<u16>(jo::as_unit(a)->crit_hits + 1);
            if (jo::as_unit(a)->crit_ready != 0 && jo::self<jo::Unit>(this)->hit_points == 0 && jo::self<jo::Unit>(this)->state_120 == 0) {
                joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(str(d::s_CRITKILL));  // 0x00492c40 'CRITKILL'
                joan::orig::as_critical_hud(obj(d::g_critical_hud))->mark_crit_kill();                   // 0x00509d70
                jo::as_unit(a)->crit_kills = static_cast<u16>(jo::as_unit(a)->crit_kills + 1);
            }
        }
        return dmg;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_09", name, addr, fn)
B("Unit::melee_damage_509950", 0x00509950, &H::melee_damage);
#undef B

}  // namespace game_b106_09
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {


using u32 = std::uint32_t;

void* const kUnits = obj(d::g_items);

JOAN_COUNTER(c_set_unit_state, "Player::set_unit_state");

// ------------------------------------------------------------------------------- thiscall
struct H {
    // 0x00509d40
    i16 byte_9d() { return static_cast<i16>(jo::self<jo::Player>(this)->combo_count); }

    // 0x0050a9e0
    void set_unit_state(i16 id, i8 mode) {
        c_set_unit_state.hit();
        if (joan::orig::as_units(kUnits)->list.empty_at(static_cast<i32>(id)) != 0) return;  // 0x00425d50
        void* const unit = joan::orig::as_units(kUnits)->list.ptr_at(static_cast<i32>(id));  // 0x00425d20
        if (jo::view<void*>(jo::as_map_object(unit)->actor) != nullptr)
            joan::orig::as_bone(jo::view<void*>(jo::as_map_object(unit)->actor->bone))->set_parent(joan::orig::arg::mptr(i32{0}));  // 0x0060ec70
        i32 state = 0;
        if (static_cast<i32>(mode) == 0)
            state = 2;
        else if (static_cast<i32>(mode) == 1)
            state = 3;
        thiscall<u32>(static_cast<const std::uintptr_t*>(jo::as_map_object(unit)->vtable)[0x24 / 4], unit);  // virtual slot 0x24
        if (state != 0) {
            i32 colour = 0;
            void* const owner = jo::self<jo::Unit>(this)->actor;
            if (jo::view<void*>(jo::as_actor_def(*static_cast<void**>(owner))->model->bones) != nullptr) {
                colour = joan::orig::arg::as<i32>(joan::orig::as_bone_table(// 0x005ba160
                                       jo::view<void*>(jo::as_actor_def(*jo::view<void**>(jo::self<jo::Unit>(this)->actor))->model->bones))->find(state));
            }
            void* const material = joan::orig::as_plane(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->plane))->bone_5cf8f0(joan::orig::arg::i32(colour));  // 0x005cf8f0
            if (jo::view<void*>(jo::as_map_object(unit)->actor) != nullptr)
                joan::orig::as_bone(jo::view<void*>(jo::as_map_object(unit)->actor->bone))->set_parent(material);  // 0x0060ec70
            jo::view<i32[3]>(jo::as_map_object(unit)->sound_slots)[2] = state;  // +0x9e: sound_slots[4..5] as one dword
        }
        joan::orig::as_map_object(unit)->set_world(jo::self<jo::Unit>(this)->world);  // 0x004994b0
        if (jo::self<jo::Unit>(this)->cell != nullptr) joan::orig::as_cell(jo::self<jo::Unit>(this)->cell)->attach_unit(joan::orig::arg::mptr(unit));  // 0x0043b190
        if (jo::view<void*>(jo::as_map_object(unit)->actor) != nullptr && jo::self<jo::Unit>(this)->scene != nullptr)
            joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::view<void*>(jo::as_map_object(unit)->actor)));  // 0x005efc50
    }

    // 0x0050cbc0
    void on_flag(i32 what, u8 on) {
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Unit>(this)->in_scene)) != 0) {
            if (what == 1) {
                if (on != 0) {
                    if (jo::view<i32>(jo::self<jo::Unit>(this)->alt_mesh_state) != 2) joan::orig::as_cell_object(this)->create_alt_mesh(joan::orig::arg::i32(what));  // 0x0050cc70
                } else {
                    joan::orig::as_unit(this)->drop_alt_mesh(joan::orig::arg::i32(what));  // 0x0050d050
                }
            }
        } else if (what == 1) {
            jo::view<i32>(jo::self<jo::Unit>(this)->alt_mesh_state) = on != 0 ? 1 : 0;
        }
    }

    // 0x00510e80
    float health_factor() {
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return global<float>(d::flt_1);  // 1.0
        const i32 a = 100 - joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(i32{9}, static_cast<i32>(jo::self<jo::Unit>(this)->id)));     // 0x00497e10
        const i32 b = 100 - joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(i32{0x16}, static_cast<i32>(jo::self<jo::Unit>(this)->id)));  // 0x00497e10
        const i32 product = static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(b));
        const Fpu fpu{true};
        return static_cast<float>(fpu.mul(static_cast<double>(product), global<float>(d::flt_9d5360)));  // 1e-4f
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("Obj::byte_9d", 0x00509d40, &H::byte_9d);
B("Player::set_unit_state", 0x0050a9e0, &H::set_unit_state);
B("Obj::on_flag", 0x0050cbc0, &H::on_flag);
B("Unit::health_factor", 0x00510e80, &H::health_factor);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b104_06.cpp (game/b104_06)
namespace {
namespace game_b104_06 {

constexpr Fpu kFpu{true};

using u32 = std::uint32_t;

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }
using joan::orig::helpers::fd;

struct Vec3 {
    float v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);

JOAN_COUNTER(c_damage, "Unit::damage_509e90");
JOAN_COUNTER(c_clear_markers, "Unit::clear_markers");
JOAN_COUNTER(c_armour, "Unit::armour_factor");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(i32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
void* type_get(i32 w) { return joan::orig::arg::as<void*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(sx16(w)))); }  // 0x0042d0a0 (word)
void* kind_get(i32 w) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(sx16(w)))); }  // 0x00425ce0 (word)

// 0x00510750 helpers.
float base_factor(void* self, i32 k, void* p, float acc) {
    const i32 n = joan::orig::arg::as<i32>(joan::orig::as_unit(self)->find_link(joan::orig::arg::i32(k), joan::orig::arg::str(p)));  // 0x0050d3d0
    return static_cast<float>(
        kFpu.mul(kFpu.div(kFpu.add(static_cast<double>(n), fd(d::flt_10)), fd(d::flt_11)), acc));
}
float skill_factor(void* self, i32 k, float acc) {
    if (jo::as_unit(self)->modifiers == nullptr) return acc;
    const i32 n = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::as_unit(self)->modifiers)->sum(joan::orig::arg::i32(k), joan::orig::arg::i32(// 0x00497e10
                                static_cast<i32>(jo::as_unit(self)->id)))) + 100;
    return static_cast<float>(kFpu.mul(kFpu.mul(static_cast<double>(n), fd(d::flt_0_01)), acc));
}
bool weapon_is_a(void* self) {
    const i32 idx = static_cast<i32>(jo::as_unit(self)->combo_state);
    return static_cast<i32>(jo::view<i8>(jo::as_unit(self)->armour_flags[idx - 1])) == 0x41;
}

struct H {
    // ------------------------------------------------------------------------ 0x00509e90
    float damage_509e90(i32 unit_id, i32 kind, i32 pct) {
        c_damage.hit();
        void* const u = unit_get(unit_id);
        float c = joan::orig::arg::as<float>(joan::orig::as_unit(u)->defence_value());  // 0x00510bf0
        float l40;
        {
            const i32 k1 = static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::as_unit(u)->stats[7])) + 100u -
                                            global<u32>(d::g_unit_mor_nor));
            l40 = static_cast<float>(kFpu.div(kFpu.mul(static_cast<double>(k1), c), fd(d::flt_100)));
        }
        {
            const double v = joan::orig::arg::as<double>(joan::orig::as_unit(u)->stat_e_scaled());  // 0x00510e40 (st0)
            c = static_cast<float>(kFpu.mul(v, l40));
        }
        const float l10 = static_cast<float>(static_cast<i32>(
            static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->stats[4])) * global<u32>(d::g_a3cc00)));
        const float l48 = static_cast<float>(kFpu.sub(fd(d::flt_100), static_cast<double>(pct)));
        float l14;
        {
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->level_sum());  // 0x005238c0
            double t = kFpu.mul(static_cast<double>(r), l48);
            t = kFpu.mul(t, static_cast<double>(static_cast<i32>(global<i16>(d::g_a3cce0))));
            t = kFpu.div(t, fd(d::flt_100));
            l14 = static_cast<float>(kFpu.add(t, l10));
        }
        {
            const i32 k2 = static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->stats[7])) + 100u -
                                            global<u32>(d::g_unit_mor_nor));
            l14 = static_cast<float>(kFpu.div(kFpu.mul(static_cast<double>(k2), l14), fd(d::flt_100)));
        }
        float l58;
        {
            const float d = (c - l14) / global<float>(d::flt_2);
            if (!(d >= global<float>(d::k_9cf5c4)))
                l58 = 0.0f;
            else
                l58 = (c - l14) / global<float>(d::flt_2);
        }
        float l18 = c / global<float>(d::flt_2) + l58;
        {
            const double v = joan::orig::arg::as<double>(joan::orig::as_unit(this)->stat16_factor());  // 0x00510f10 (st0)
            l18 = static_cast<float>(kFpu.mul(v, l18));
        }
        if (static_cast<i32>(jo::self<jo::UnitType>(type_get(jo::self<jo::Unit>(this)->type_id))->unit_class) != 8) {
            Vec3 tmp{};
            const Vec3 dv = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::self<jo::Unit>(this)->position_), joan::orig::arg::ptr(jo::as_unit(u)->position_)));  // 0x0060cc90
            const double len = import_call<double>(d::iat_v3d_87_length, &dv);  // length
            const i32 dist = ftol(len);
            const i32 far_ = global<i32>(d::g_a3cc6c);
            const i32 near_ = global<i32>(d::g_a3cc0c);
            if (dist >= far_) {
                l18 = l18 * global<float>(d::g_a3cc20);
            } else if (dist > near_ && dist < far_) {
                const i32 num = static_cast<i32>(static_cast<u32>(dist) - static_cast<u32>(near_));
                const i32 den = static_cast<i32>(static_cast<u32>(far_) - static_cast<u32>(near_));
                double t = kFpu.mul(static_cast<double>(num), fd(d::g_a3cc20));
                t = kFpu.div(t, static_cast<double>(den));
                t = kFpu.sub(fd(d::flt_1), t);
                l18 = static_cast<float>(kFpu.mul(t, l18));
            }
        }
        if (static_cast<i32>(jo::self<jo::Unit>(this)->stats[4]) == 3 && (kind == 0x40 || kind == 0x2000))
            l18 = l18 * global<float>(d::g_a3cd0c);
        if (kind == 0x40) l18 = l18 * global<float>(d::g_a3cbec);
        if (unit_missing(global<i32>(d::g_player_unit_recno)) == 0) {
            const auto scale = [&](img::Addr g, img::Addr div) {
                const double t = kFpu.div(static_cast<double>(global<i32>(g)), fd(div));
                l18 = static_cast<float>(kFpu.mul(t, l18));
            };
            if (global<i32>(d::g_mission) == 8) {
                {
                    const i32 s = static_cast<i32>(jo::as_unit(u)->side);
                    if (s != static_cast<i32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->side))
                        scale(d::g_unit_f_attack, d::flt_250);
                }
                {
                    const i32 s = static_cast<i32>(jo::self<jo::Unit>(this)->side);
                    if (s != static_cast<i32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->side))
                        scale(d::g_unit_f_defend, d::flt_100);
                }
            }
            {
                const i32 s = static_cast<i32>(jo::as_unit(u)->side);
                if (s == static_cast<i32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->side))
                    scale(d::g_unit_f_attack, d::flt_100);
            }
            {
                const i32 s = static_cast<i32>(jo::self<jo::Unit>(this)->side);
                if (s == static_cast<i32>(jo::as_unit(unit_get(global<i32>(d::g_player_unit_recno)))->side))
                    scale(d::g_unit_f_defend, d::flt_100);
            }
        }
        {
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_unit(u)->leader());  // 0x004216d0
            if (p != nullptr && p != u) {
                i32 m = static_cast<i32>(jo::as_unit(p)->stats[3]);
                if (!(m < 100)) m = 100;
                const double t = kFpu.div(static_cast<double>(m + 100), fd(d::flt_100));
                l18 = static_cast<float>(kFpu.mul(t, l18));
            }
        }
        {
            const i32 n = ftol(kFpu.sub(static_cast<double>(jo::self<jo::Unit>(this)->hit_points), l18));
            joan::orig::as_unit(this)->set_value_135(joan::orig::arg::i32(n));  // 0x004fe510
        }
        return l18;
    }

    // ------------------------------------------------------------------------ 0x0050b390
    u32 clear_markers() {
        c_clear_markers.hit();
        bool found = false;
        for (i32 i = joan::orig::as_dynarray(obj(d::g_map_object_list))->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_types(obj(d::g_a50530))->empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00485410
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_a50530))->at_plus4(joan::orig::arg::u32(i)));  // 0x004853e0
            if (jo::as_map_object(m)->world_item == nullptr) continue;
            if (static_cast<i32>(jo::as_map_object(m)->type_id) == static_cast<i32>(jo::self<jo::Unit>(this)->drop_item)) {
                if (jo::as_map_object(m)->actor != nullptr) joan::orig::as_model_instance(jo::view<void*>(jo::as_map_object(m)->actor))->set_10a(1u);  // 0x0042cf20
                found = true;
            } else if (static_cast<i32>(*static_cast<i16*>(kind_get(jo::as_map_object(m)->type_id))) == 0xf) {
                if (jo::as_map_object(m)->actor != nullptr) joan::orig::as_model_instance(jo::view<void*>(jo::as_map_object(m)->actor))->set_10a(1u);  // 0x0042cf20
            }
        }
        if (found) return 1;
        const u32 saved = jo::view<u32>(jo::self<jo::Unit>(this)->markers);
        jo::view<u32>(jo::self<jo::Unit>(this)->markers) = 0;
        joan::orig::as_unit(this)->spawn_projectile();  // 0x0050afe0
        jo::view<u32>(jo::self<jo::Unit>(this)->markers) = saved;
        return 0;  // void callee; every caller ignores the result
    }

    // ------------------------------------------------------------------------ 0x00510750
    float armour_factor() {
        c_armour.hit();
        float l8 = 1.0f;
        void* const self = this;
        void* const p227 = jo::as_unit(self)->armour_flags;
        const i32 t = static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind);
        if (t == 0x1c) {
            l8 = base_factor(self, 9, p227, l8);
            if (jo::self<jo::Unit>(this)->modifiers != nullptr) l8 = skill_factor(self, weapon_is_a(self) ? 0xb : 0xc, l8);
        } else if (t == 0x22) {
            l8 = base_factor(self, 0xb, nullptr, l8);
            l8 = skill_factor(self, 0xf, l8);
        } else if (t == 0x1d || t == 8 || t == 0x19) {
            l8 = base_factor(self, 0xa, nullptr, l8);
            l8 = skill_factor(self, 0xd, l8);
        } else if (t == 0x53) {
            l8 = base_factor(self, 0xc, nullptr, l8);
            l8 = skill_factor(self, 0xe, l8);
        } else if (t == 0x1e || t == 0x1f || t == 0x20 || t == 0x21) {
            l8 = base_factor(self, 0xd, nullptr, l8);
        } else if (t == 0x49) {
            if (weapon_is_a(self)) {
                l8 = base_factor(self, 0xf, p227, l8);
                l8 = skill_factor(self, 0xb, l8);
            } else {
                l8 = base_factor(self, 0x10, p227, l8);
                l8 = skill_factor(self, 0xc, l8);
            }
        }
        return l8;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_06", name, addr, fn)
B("Unit::damage_509e90", 0x00509e90, &H::damage_509e90);
B("Unit::clear_markers", 0x0050b390, &H::clear_markers);
B("Unit::armour_factor", 0x00510750, &H::armour_factor);
#undef B

}  // namespace game_b104_06
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
namespace {
namespace game_b101_07 {

using u32 = std::uint32_t;


struct Vec3 {
    float v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);
void* const kKinds = obj(d::g_types);
void* const kTypes = obj(d::g_item_res);
void* const kSquads = obj(d::g_physic_sys);

using joan::orig::helpers::crt_memcpy;

u32 object_missing(i32 id) { return joan::orig::arg::as<u32>(joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id))); }  // 0x00425d50
u8* object_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
u8* type_get(i16 t) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(t)))); }  // 0x00425ce0
u8* kind_get(i16 k) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<i32>(k)))); }  // 0x0042d0a0
void own_pos(void* self, Vec3* out) { joan::orig::as_unit(self)->position(joan::orig::arg::mptr(out)); }          // 0x004fedb0
void object_pos(void* o, Vec3* out) { joan::orig::as_map_object(o)->position(joan::orig::arg::mptr(out)); }             // 0x00499930
Vec3* vec_sub(Vec3* out, const Vec3* a, const Vec3* b) {
    return joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));  // 0x0060cc90
}
void normalize(void* v) { import_call<void*>(d::iat_v3d_87_vector_normalize_1, v); }
// 0x0050a480 (short id, int, char, short* out)
void set_target(void* self, i16 id, i32 b, i32 c, i32 d) {
    joan::orig::as_cell_object(self)->attach_unit(joan::orig::arg::i16(static_cast<i32>(id)), joan::orig::arg::i32(b), static_cast<std::int8_t>(joan::orig::arg::i32(c)), joan::orig::arg::mptr(d));
}

JOAN_COUNTER(c_look_dir, "Unit::update_look_dir");
JOAN_COUNTER(c_set_leader, "Unit::set_leader");
JOAN_COUNTER(c_switch_mount, "Unit::switch_mount");

struct H {
    // ------------------------------------------------------------------ 0x0050aca0
    void update_look_dir() {
        c_look_dir.hit();
        if (joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnits)->empty_at(static_cast<i32>(jo::self<jo::Unit>(this)->look_a))) == 0) {  // 0x00428b70
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::self<jo::Unit>(this)->look_a));  // 0x0041b3c0
            Vec3 a = global<Vec3>(d::g_pos);
            own_pos(u, &a);
            Vec3 b = global<Vec3>(d::g_pos);
            own_pos(this, &b);
            Vec3 tmp;
            jo::view<Vec3>(jo::self<jo::Unit>(this)->look_dir) = *vec_sub(&tmp, &a, &b);
            normalize(jo::self<jo::Unit>(this)->look_dir);
            return;
        }
        if (object_missing(jo::self<jo::Unit>(this)->look_b) == 0) {
            u8* const o = object_get(jo::self<jo::Unit>(this)->look_b);
            Vec3 a = global<Vec3>(d::g_pos);
            object_pos(o, &a);
            Vec3 b = global<Vec3>(d::g_pos);
            own_pos(this, &b);
            Vec3 tmp;
            jo::view<Vec3>(jo::self<jo::Unit>(this)->look_dir) = *vec_sub(&tmp, &a, &b);
            normalize(jo::self<jo::Unit>(this)->look_dir);
            return;
        }
        if (jo::self<jo::Unit>(this)->actor == nullptr) return;
        jo::view<u32>(jo::self<jo::Unit>(this)->look_dir[0]) = jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone->local[0]);
        jo::view<u32>(jo::self<jo::Unit>(this)->look_dir[1]) = jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone->local[1]);
        jo::view<u32>(jo::self<jo::Unit>(this)->look_dir[2]) = jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone->local[2]);
        u8* const k = kind_get(jo::self<jo::Unit>(this)->type_id);
        switch (static_cast<i32>(jo::self<jo::UnitType>(k)->kind)) {
            case 4: jo::self<jo::Unit>(this)->look_dir[2] = jo::self<jo::Unit>(this)->look_dir[2] + global<float>(d::flt_0_15); break;  // 0.15f
            case 5: jo::self<jo::Unit>(this)->look_dir[2] = jo::self<jo::Unit>(this)->look_dir[2] + global<float>(d::flt_0_15); break;
            default: break;
        }
        normalize(jo::self<jo::Unit>(this)->look_dir);
    }

    // Leader slot (0 = first, 1 = second) of the squad: virtual +0x18(1) / +0x1c on the
    // leader object, then 0x50a480(leader, type +0xa, slot, 0).
    void apply_leader(i32 slot) {
        u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->slot(joan::orig::arg::i32(slot)));  // 0x004974f0
        u32 id = 0;
        if (s != nullptr) id = joan::orig::arg::as<u32>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::as_held_item(s)->item)))));  // 0x0049ea90
        if (id == 0) return;
        virtual_call<u32>(object_get(static_cast<i32>(id)), 0x18, 1);
        virtual_call<u32>(object_get(static_cast<i32>(id)), 0x1c);
        u8* const t = type_get(jo::view<i16>(jo::as_held_item(s)->item));
        set_target(this, static_cast<i16>(id), static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->bone)), slot, 0);
    }

    // ------------------------------------------------------------------ 0x0050c680
    void set_leader(i32 arg) {
        c_set_leader.hit();
        const i16 id = static_cast<i16>(arg);
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return;
        if (static_cast<i32>(jo::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->hand_mode) == static_cast<i32>(id)) return;
        set_target(this, 0, 0, 0, 0);
        set_target(this, 0, 0, 1, 0);
        joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->set_owner_id(joan::orig::arg::u16(static_cast<i32>(id)));  // 0x004975a0
        apply_leader(0);
        apply_leader(1);
    }

    // ------------------------------------------------------------------ 0x0050c8a0
    void switch_mount(i32 which) {
        c_switch_mount.hit();
        u8* const old = jo::view<u8*>(jo::self<jo::Unit>(this)->actor);
        u8* cur = nullptr;
        jo::AnimSet* extra = nullptr;
        if (which == 0) {
            cur = jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c);
            extra = jo::self<jo::Unit>(this)->anim_set_c;
        } else if (which == 1) {
            cur = jo::view<u8*>(jo::self<jo::Unit>(this)->actor_b);
            extra = jo::self<jo::Unit>(this)->anim_set_b;
        }
        if (old == cur) return;
        joan::orig::as_scene(jo::view<void*>(jo::as_actor(old)->scene))->attach(joan::orig::arg::mptr(cur));  // 0x005efc50
        joan::orig::as_scene(jo::view<void*>(jo::as_actor(old)->scene))->detach(joan::orig::arg::mptr(old));  // 0x005efdb0
        jo::view<u8*>(jo::self<jo::Unit>(this)->actor) = cur;
        jo::self<jo::Unit>(this)->anim_set = extra;
        if (jo::view<u32>(jo::as_actor(old)->record) != 0) {
            u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_players(kSquads)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(old)->record))));  // 0x006af310
            u8* const dst = joan::orig::arg::as<u8*>(joan::orig::as_players(kSquads)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(cur)->record))));  // 0x006af310
            const u32 keep = jo::as_shot(dst)->id;
            crt_memcpy(dst, src, static_cast<u32>(sizeof(jo::Shot)));  // the whole Shot (0x2b2 on x86)
            jo::as_shot(dst)->id = keep;
            jo::view<u8*>(jo::as_shot(dst)->actor) = cur;
            joan::orig::as_players(kSquads)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(old)->record)), joan::orig::arg::i16(0));  // 0x006ad980
            joan::orig::as_players(kSquads)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(cur)->record)), joan::orig::arg::i16(static_cast<i32>(jo::view<i16[2]>(jo::as_actor(cur)->record)[0])));        // 0x006ad980
        }
        joan::orig::as_actor(cur)->set_position_5ac310(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->position_));    // 0x005ac310
        joan::orig::as_scene_object(cur)->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)));      // 0x005ac6a0
        jo::as_actor(cur)->skin = jo::as_actor(old)->skin;
        jo::view<u32>(jo::as_actor(cur)->fade) = jo::view<u32>(jo::as_actor(old)->fade);
        jo::as_actor(cur)->fade_38 = jo::as_actor(old)->fade_38;
        const u32 v = joan::orig::arg::as<u32>(joan::orig::as_cell_object(this)->find_anim_state());  // 0x00504010
        jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry) = 0;
        joan::orig::as_cell_object(this)->set_anim_state(joan::orig::arg::i32(v), joan::orig::arg::i32(jo::as_actor(old)->start_time), joan::orig::arg::i32(-1));  // 0x004ff9f0
        jo::as_actor(cur)->start_time = jo::as_actor(old)->start_time;
        jo::as_actor(cur)->anim_flags = jo::as_actor(old)->anim_flags;
        if (object_missing(jo::self<jo::Unit>(this)->carried_a) == 0) {
            u8* const o = object_get(jo::self<jo::Unit>(this)->carried_a);
            u8* const t = type_get(jo::as_map_object(o)->type_id);
            set_target(this, jo::self<jo::Unit>(this)->carried_a, static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->bone)), 0, 0);
        }
        if (object_missing(jo::self<jo::Unit>(this)->carried_b) == 0) {
            u8* const o = object_get(jo::self<jo::Unit>(this)->carried_b);
            u8* const t = type_get(jo::as_map_object(o)->type_id);
            set_target(this, jo::self<jo::Unit>(this)->carried_b, static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->bone)), 1, 0);
        }
    }

    // ------------------------------------------------------------------ 0x00510120
    u32 squad_call_4985f0(u32 a) {
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->count_in_grids(joan::orig::arg::i32(a)));  // 0x004985f0
    }

    // ------------------------------------------------------------------ 0x005101c0
    u32 squad_call_498970(u32 a, u32 b) {
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return 0;
        return joan::orig::arg::as<u32>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->remove_type(joan::orig::arg::i32(a), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->id)), joan::orig::arg::u32(b)));  // 0x00498970
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("Unit::update_look_dir", 0x0050aca0, &H::update_look_dir);
B("Unit::set_leader", 0x0050c680, &H::set_leader);
B("Unit::switch_mount", 0x0050c8a0, &H::switch_mount);
B("Unit::squad_call_4985f0", 0x00510120, &H::squad_call_4985f0);
B("Unit::squad_call_498970", 0x005101c0, &H::squad_call_498970);
#undef B

}  // namespace game_b101_07
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using u32 = std::uint32_t;


JOAN_COUNTER(c_buy, "Shop::buy_50c2d0");

struct H {
    // ------------------------------------------------------------------------ 0x0050c2d0
    i32 buy(i16 item, i32 cost) {
        c_buy.hit();
        if (!(global<i32>(d::g_get_a596c4) >= cost)) return 0;
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return 0;
        alignas(4) u8 o[sizeof(jo::WorldItem) + 2];  // 0x54 on x86
        joan::orig::as_world_item(o)->construct();  // 0x00456eb0
        void* const tbl = obj(d::g_item_res);
        const i16 w2a = jo::view<i16>(jo::as_item_def(joan::orig::arg::as<void*>(joan::orig::as_records_2bf(tbl)->get(joan::orig::arg::i16(item))))->stats[3]);  // 0x00425ce0
        const i16 w36 = static_cast<i16>(jo::as_item_def(joan::orig::arg::as<void*>(joan::orig::as_records_2bf(tbl)->get(joan::orig::arg::i16(item))))->min_level);  // 0x00425ce0
        joan::orig::as_item_stack(o)->init(joan::orig::arg::i16(item), joan::orig::arg::i16(w36), joan::orig::arg::i16(w2a), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00456cd0
        void* const target = jo::view<void*>(jo::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->grid_16);
        const bool ok = 0 != (joan::orig::as_inventory(target)->add_stack(joan::orig::arg::i32(item), joan::orig::arg::i32(jo::view<i16>(jo::as_world_item(o)->stack.u16_4)), joan::orig::arg::mptr(&jo::as_world_item(o)->stack.count), joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(o)->stack.uses)), joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(o)->stack.max_uses)), joan::orig::arg::i32(jo::as_world_item(o)->stack.u32_e), i32{0}));  // 0x004577a0
        i32 r = 0;
        if (ok) {
            global<i32>(d::g_get_a596c4) = global<i32>(d::g_get_a596c4) - cost;
            r = 1;
        }
        joan::orig::as_world_item(o)->destruct();  // 0x00456ef0
        return r;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Shop::buy_50c2d0", 0x0050c2d0, &H::buy);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
namespace {
namespace en3d_b82_17 {


struct H {
    // 0x0050d240
    std::uint8_t* table_entry(std::uint32_t i) { return reinterpret_cast<std::uint8_t*>(jo::self<jo::UpgradeRes>(this)->items) + i * 0x64u - 0x64u; }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("Table::entry", 0x0050d240, &H::table_entry);
#undef B82_17G

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b89_01.cpp (game/b89_01)
namespace {
namespace game_b89_01 {

using joan::orig::helpers::at;

struct Vec3 {
    float x, y, z;  // raw float bits, copied only
};
static_assert(sizeof(Vec3) == 12, "layout");

// 0x5a1170-family fixed string (0x190 chars + NUL, no destructor).
struct alignas(4) String {
    std::uint8_t b[0x198];
};
const char* c_str(String& s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()); }  // 0x00417d90
void str_init(String& s, const char* t) { joan::orig::as_string(&s)->construct(joan::orig::arg::str(t)); }  // 0x005a11a0
void str_assign(String& s, const char* t) { joan::orig::as_string(&s)->assign(joan::orig::arg::str(t)); }  // 0x005a1440
void str_append(String& s, const char* t) { joan::orig::as_string(&s)->append(joan::orig::arg::str(t)); }  // 0x005a1550

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kPlayers = obj(d::g_physic_sys);
void* const kSfx = obj(d::g_sound_def_table);

std::int32_t sx16(std::int32_t v) { return static_cast<std::int32_t>(static_cast<std::int16_t>(v)); }

bool unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
bool sfx_is_named(std::int32_t s) {
    return static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_sorted_names(kSfx)->is_none(joan::orig::arg::i32(s)))) != 0;  // 0x004d14c0
}
std::uint8_t* sfx_rec(std::int32_t s) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sorted_names(kSfx)->get(joan::orig::arg::i16(sx16(s))));  // 0x004d0820 (reads the word)
}
std::int32_t sfx_find(const char* name) {
    return static_cast<std::int32_t>(joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(kSfx)->find(joan::orig::arg::str(name))));  // 0x004d12f0
}
void blood(std::int32_t sound, const void* unit, std::uint32_t size, std::int32_t t) {
    const Vec3 pos = jo::view<Vec3>(jo::as_unit(unit)->position_);
    joan::orig::as_triggers(kBlood)->add(0x800u, 4u, joan::orig::arg::i16(sx16(sound)), 1u, joan::orig::arg::i16(sx16(jo::as_unit(unit)->id)), static_cast<std::int8_t>(joan::orig::arg::i32(1)), pos, joan::orig::arg::u32(size), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::u32(t), joan::orig::arg::u32(t), static_cast<std::int8_t>(joan::orig::arg::i32(1)), 0u, joan::orig::arg::i16(0));  // 0x004eef00 (ret 0x40)
}

JOAN_COUNTER(c_anim_sounds, "Unit::play_anim_sounds");

struct H {
    std::int32_t id() { return static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id); }
    bool is_camera() { return global<std::int32_t>(d::g_player_unit_recno) == id(); }

    // ebp-0x1c4..0x1c8 pattern: the player's tile under the unit may turn the surface into "item".
    void item_surface(String& surface) {
        if (jo::self<jo::Unit>(this)->actor == nullptr || jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) return;
        std::uint8_t* p;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->mounted()) != 0) {  // 0x00430190
            std::uint8_t* const carrier = unit_get(jo::self<jo::Unit>(this)->mount_id);
            p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(kPlayers)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(carrier)->actor)->record))));  // 0x006af310
        } else {
            p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(kPlayers)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
        }
        if (jo::view<std::int32_t>(jo::as_shot(p)->state.zone) == 0 || jo::as_shot(p)->state.sub == nullptr) return;
        std::uint8_t* const tile =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_billboard(jo::as_path_space(jo::as_shot(p)->state.sub)->zones)->item(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_shot(p)->state.zone))));  // 0x00637da0
        if (jo::view<std::int32_t>(jo::as_terrain_block(tile)->u32_4) == 1) str_assign(surface, str(d::s_item));  // "item"
    }
    static bool in_water(void* self, const std::uint8_t* track) {
        return track != nullptr && !(jo::view<float[3]>(jo::as_unit(self)->position_)[2] >= jo::as_water(track)->box[2]);  // `track`: Waters::find_at 0x00469d10 result
    }

    // One sound record of the animation entry. Returns false when the whole function returns.
    bool play_record(std::int32_t s, std::int32_t delay, const char* name, bool first, std::int32_t flag,
                     String& surface, std::int32_t& f1b4, std::int32_t& f1b8, std::int32_t& f1bc) {
        if (s == 0) return true;
        if (sfx_is_named(s)) {
            if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(this)->dead())) != 0) return true;  // 0x0042d0e0
            bool idle;
            if (first && (flag & 0xff) != 0) {
                idle = joan::orig::arg::as<std::int16_t>(joan::orig::as_unit(this)->is_special_kind()) == 0;  // 0x00512750
            } else {
                idle = busy(this) == 0;
            }
            if (!idle) return true;
            if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) != 0) return true;  // 0x004301c0
            if (!unit_invalid(jo::self<jo::Unit>(this)->mount_id) &&
                static_cast<std::int32_t>(jo::as_unit(unit_get(jo::self<jo::Unit>(this)->mount_id))->anim_kind) == 1)
                return true;
            if (delay == -1 && (!is_camera() || global<std::int32_t>(d::g_control_mode) == 4)) return false;
            item_surface(surface);
            std::uint8_t* const track =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_waters(kTracks)->find_at(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Unit>(this)->position_)), 0));  // 0x00469d10
            String sname;
            str_init(sname, name);
            str_append(sname, str(d::s_us_9cf774));  // "_"
            auto append_ground = [&] {
                if (joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(c_str(surface)), str(d::s_grass))) != nullptr) {  // strstr "grass"
                    str_append(sname, str(d::s_GRASS));  // "GRASS"
                } else if (joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(c_str(surface)), str(d::s_snow))) != nullptr) {  // strstr "snow"
                    str_append(sname, str(d::s_SNOW));  // "SNOW"
                } else {
                    str_append(sname, str(d::s_GROUND));  // "GROUND"
                }
            };
            bool wet = false;
            if (in_water(this, track)) {
                str_append(sname, str(d::s_WATER));  // "WATER"
            } else if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_bytes(obj(d::g_weather_system))->at(1)) & 0xff) != 0) {  // 0x0041bf10
                str_append(sname, str(d::s_WET));  // "WET"
                wet = true;
            } else {
                append_ground();
            }
            std::int32_t snd = sfx_find(c_str(sname));
            if (snd == 0 && wet && joan::hook::settings().fix_wet_sounds) {
                // [Fixes] WetSounds: in rain the original asks for "<sound>_WET" only; the sounds recorded without a
                // wet version (the jump, the dash) went silent. Their ground version instead.
                str_init(sname, name);
                str_append(sname, str(d::s_us_9cf774));  // "_"
                append_ground();
                snd = sfx_find(c_str(sname));
            }
            if (snd == 0) return true;
            if (delay == -1) {
                if (in_water(this, track)) {
                    const std::uint32_t dt = static_cast<std::uint32_t>(time_now()) - jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->snd_last_ms_a);
                    if (dt > jo::view<std::uint32_t>(jo::self<jo::WaveRes>(sfx_rec(snd))->sound_time_ms)) {
                        blood(snd, this, 0, time_now());
                        f1bc = 1;
                    }
                } else {
                    const std::uint32_t dt =
                        static_cast<std::uint32_t>(time_now()) - jo::view<std::uint32_t>(first ? jo::self<jo::Unit>(this)->step_time : jo::self<jo::Unit>(this)->step_time_b);
                    if (dt > jo::view<std::uint32_t>(jo::self<jo::WaveRes>(sfx_rec(snd))->sound_time_ms)) {
                        blood(snd, this, 0, time_now());
                        (first ? f1b4 : f1b8) = 1;
                    }
                }
            } else {
                if (in_water(this, track)) {
                    blood(snd, this, 0, time_now());
                } else if (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind) != 0x1a) {
                    blood(snd, this, 0, time_now());
                }
            }
            return true;
        }
        if (sfx_is_named(s)) return true;
        std::uint8_t* const rec = sfx_rec(s);
        if (static_cast<std::int8_t>(jo::self<jo::WaveRes>(rec)->stream) != 0 && !is_camera()) return true;  // WaveRes +0x8a on x86 (behind a pointer on x64)
        const std::int32_t variants = static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::WaveRes>(rec)->max_play));
        if (variants > 1) s = add32(s, id() % variants);
        const std::int32_t action = static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind);
        if (action == 0x33 || action == 0x38) {
            if (first) {
                const std::int32_t kind = static_cast<std::int32_t>(jo::self<jo::UnitType>(
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(sx16(jo::self<jo::Unit>(this)->type_id)))))->kind);  // 0x0042d0a0
                if (kind == 4 || kind == 5 || kind == 6 || kind == 9 || kind == 0xc || kind == 0xa) {
                    blood(s, this, jo::view<std::uint32_t>(jo::self<jo::WaveRes>(rec)->max_dist), add32(time_now(), delay));
                    return true;
                }
            }
            if (jo::self<jo::Unit>(this)->modifiers == nullptr) return true;
            std::int16_t* const item = joan::orig::arg::as<std::int16_t*>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->slot_a());  // 0x00497560
            if (item == nullptr) return true;
            std::uint8_t* const it = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(sx16(*item))));  // 0x00425ce0
            if (static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_item_def(it)->type_id)) == (first ? 0xb : 0xc)) {
                blood(s, this, jo::view<std::uint32_t>(jo::self<jo::WaveRes>(rec)->max_dist), add32(time_now(), delay));
            }
            return true;
        }
        blood(s, this, jo::view<std::uint32_t>(jo::self<jo::WaveRes>(rec)->max_dist), add32(time_now(), delay));
        return true;
    }

    void play_anim_sounds_(std::int32_t anim, std::int32_t flag) {
        if (anim == 0 || jo::self<jo::Unit>(this)->actor == nullptr) return;
        c_anim_sounds.hit();
        if (busy(this) != 0 && static_cast<std::int8_t>(global<std::uint8_t>(d::g_a5d384)) != 0) return;
        std::uint8_t* const entry = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(anim)));  // 0x004168d0
        const std::int32_t v72 = jo::as_anim_group_rec(entry)->model_ref;
        if (v72 == 0) return;
        joan::orig::as_model_refs(kAnims)->get(joan::orig::arg::i16(sx16(v72)));  // 0x00416f20 (result unused)
        String surface;  // ebp-0x1a4
        const Vec3 pos = jo::view<Vec3>(jo::self<jo::Unit>(this)->position_);
        str_init(surface, joan::orig::arg::as<const char*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->terrain_at(pos)));  // 0x00560ee0, 0x005a11a0
        std::int32_t f1b4 = 0, f1b8 = 0, f1bc = 0;
        if (!play_record(jo::as_anim_group_rec(entry)->sound_a, jo::as_anim_group_rec(entry)->sound_a_delay,
                         static_cast<const char*>(jo::as_anim_group_rec(entry)->sound_a_name), true, flag, surface, f1b4, f1b8, f1bc))
            return;
        if (!play_record(jo::as_anim_group_rec(entry)->sound_b, jo::as_anim_group_rec(entry)->sound_b_delay,
                         static_cast<const char*>(jo::as_anim_group_rec(entry)->sound_b_name), false, flag, surface, f1b4, f1b8, f1bc))
            return;
        if (f1b4 != 0) jo::self<jo::Unit>(this)->step_time = time_now();
        if (f1b8 != 0) jo::self<jo::Unit>(this)->step_time_b = time_now();
        if (f1bc != 0) jo::self<jo::Unit>(this)->snd_last_ms_a = time_now();
    }
};

JOAN_REPLACE("game/b89_01", "Unit::play_anim_sounds", 0x0050db80, &H::play_anim_sounds_);

}  // namespace game_b89_01
}  // namespace

// ---------------------------------------- from modules/game_b89_02.cpp (game/b89_02)
namespace {
namespace game_b89_02 {


// ---------------------------------------------------------------------------------------------
// 0x0050f570
struct Path {
    char s[0x194];  // 0x005a11a0 / 0x005a1440 write up to +0x190
};
static_assert(sizeof(Path) == 0x194, "path");

struct Vec3Raw {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers
    float x, y, z;
};
static_assert(sizeof(Vec3Raw) == 12, "vec3");

void* const kSoundNames = obj(d::g_sound_def_table);
void* const kSoundPlayer = obj(d::g_sfx_player);
void* const kGameFlags = obj(d::g_game_state);
void* const kUnits = obj(d::g_units);
void* const kPlayers = obj(d::g_physic_sys);
void* const kWeather = obj(d::g_weather_system);

void path_init(Path& p, const char* s) { joan::orig::as_string(&p)->construct(joan::orig::arg::str(s)); }  // 0x005a11a0
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
char* crt_strstr(const char* s, const char* t) { return joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(s), joan::orig::arg::str(t))); }

JOAN_COUNTER(c_step_sounds, "Unit::play_step_sounds");

struct H {
    // One sound slot (0x0050f64f.. / 0x0050f931..). Returns false when the original jumps
    // straight to the function epilogue (camera test).
    bool step_sound(std::uint8_t alt, std::int32_t name_id, std::int32_t variant, const char* base,
                    Path& terrain, std::int32_t jo::Unit::*clear_field) {
        if (name_id == 0) return true;
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_sorted_names(kSoundNames)->is_none(joan::orig::arg::i32(name_id)))) == 0)
            return true;  // 0x004d14c0
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(this)->dead())) != 0) return true;  // 0x0042d0e0
        std::int32_t idle;
        if (alt != 0)
            idle = joan::orig::arg::as<std::int16_t>(joan::orig::as_unit(this)->is_special_kind()) == 0 ? 1 : 0;  // 0x00512750
        else
            idle = joan::orig::arg::as<std::int16_t>(joan::orig::as_unit(this)->is_kind_special()) == 0 ? 1 : 0;  // 0x00508ab0
        if (idle == 0) return true;
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(kGameFlags)->flag_744())) != 0) return true;  // 0x004301c0
        if (variant != -1) return true;
        if (global<std::int32_t>(d::g_player_unit_recno) != static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id)) return false;
        if (global<std::int32_t>(d::g_control_mode) == 4) return false;

        const std::uint8_t skip = 0;  // ebp-0x1b4 / ebp-0x358, never set
        void* const owner_ref = jo::self<jo::Unit>(this)->actor;
        if (owner_ref != nullptr && jo::view<std::int32_t>(jo::as_actor(owner_ref)->record) != 0) {
            std::uint8_t* player;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->mounted()) != 0) {  // 0x00430190
                std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->mount_id)));  // 0x0041b3c0
                player = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(kPlayers)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->record))));  // 0x006af310
            } else {
                player = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(kPlayers)->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));
            }
            if (jo::view<std::int32_t>(jo::as_shot(player)->state.zone) != 0 && jo::as_shot(player)->state.sub != nullptr) {
                std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_billboard(jo::as_path_space(jo::as_shot(player)->state.sub)->zones)->item(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_shot(player)->state.zone))));  // 0x00637da0
                if (jo::view<std::int32_t>(jo::as_terrain_block(r)->u32_4) == 1) path_assign(terrain, str(d::s_item));  // "item"
            }
        }
        if (skip != 0) return true;

        Path name;
        path_init(name, base);
        path_append(name, str(d::s_us_9cf774));  // "_"
        auto append_ground = [&] {
            if (crt_strstr(c_str(terrain), str(d::s_grass)) != nullptr) {  // "grass"
                path_append(name, str(d::s_GRASS));  // "GRASS"
            } else if (crt_strstr(c_str(terrain), str(d::s_snow)) != nullptr) {  // "snow"
                path_append(name, str(d::s_SNOW));  // "SNOW"
            } else {
                path_append(name, str(d::s_GROUND));  // "GROUND"
            }
        };
        const bool wet = (joan::orig::arg::as<std::uint32_t>(joan::orig::as_bytes(kWeather)->at(1)) & 0xff) != 0;  // 0x0041bf10
        if (wet)
            path_append(name, str(d::s_WET));  // "WET"
        else
            append_ground();
        std::int32_t sound = joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(kSoundNames)->find(joan::orig::arg::str(c_str(name))));  // 0x004d12f0
        if (sound == 0 && wet && joan::hook::settings().fix_wet_sounds) {  // [Fixes] WetSounds: no "_WET" recording -> the ground one
            path_init(name, base);
            path_append(name, str(d::s_us_9cf774));  // "_"
            append_ground();
            sound = joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(kSoundNames)->find(joan::orig::arg::str(c_str(name))));  // 0x004d12f0
        }
        if (sound != 0) {
            joan::orig::as_scenario(kSoundPlayer)->trigger_slot(joan::orig::arg::i32(sound));  // 0x004cecd0
            jo::self<jo::Unit>(this)->*clear_field = 0;
        }
        return true;
    }

    // 0x0050f570
    void play_step_sounds(std::int32_t anim, std::uint32_t alt_arg) {
        c_step_sounds.hit();
        if (anim == 0 || jo::self<jo::Unit>(this)->actor == nullptr) return;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::self<jo::Unit>(this)->anim_set)->index_of_416dc0(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind))) == 0)  // 0x00416dc0
            return;
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(anim)));  // 0x004168d0
        const std::int32_t rec = jo::as_anim_group_rec(e)->model_ref;
        if (rec == 0) return;
        // push edx after `mov dx, word [ebp-0xc]`: the upper half is leftover from 0x004168d0;
        // 0x00416f20 (and its replacement) only read the low word.
        joan::orig::as_model_refs(kAnims)->get(joan::orig::arg::i16(static_cast<std::int16_t>(rec)));  // 0x00416f20, result unused
        Vec3Raw pos;
        std::memcpy(&pos, jo::self<jo::Unit>(this)->position_, sizeof pos);
        const char* const t = joan::orig::arg::as<const char*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->terrain_at(pos));  // 0x00560ee0
        Path terrain;
        path_init(terrain, t);

        const std::uint8_t alt = static_cast<std::uint8_t>(alt_arg & 0xff);
        if (!step_sound(alt, jo::as_anim_group_rec(e)->sound_a, jo::as_anim_group_rec(e)->sound_a_delay,
                        static_cast<const char*>(jo::as_anim_group_rec(e)->sound_a_name), terrain, &jo::Unit::step_time))
            return;
        step_sound(alt, jo::as_anim_group_rec(e)->sound_b, jo::as_anim_group_rec(e)->sound_b_delay, static_cast<const char*>(jo::as_anim_group_rec(e)->sound_b_name),
                   terrain, &jo::Unit::step_time_b);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_02", name, addr, fn)
B("Unit::play_step_sounds", 0x0050f570, &H::play_step_sounds);
#undef B

}  // namespace game_b89_02
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using u32 = std::uint32_t;


JOAN_COUNTER(c_split_50fd90, "Unit::split_50fd90");

struct H {
    // -------------------------------------------------------------- 0x0050fd90
    u32 split_50fd90(u32 a, u32 b, u32 c, u32 d) {
        c_split_50fd90.hit();
        if (jo::self<jo::Unit>(this)->modifiers == nullptr) return 0;
        void* const node = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u16>(a))));  // 0x00425ce0
        const i32 sb = static_cast<i16>(b & 0xffffu);
        if (jo::view<i16>(jo::as_item_def(node)->class_id) == 7 && jo::view<u32>(jo::as_item_def(node)->i32_23d) != 0) {
            if (jo::view<i16>(jo::as_item_def(node)->type_id) == 0x15) {
                const i32 want = jo::self<jo::Unit>(this)->hit_points + jo::as_item_def(node)->damage * sb;
                if (want > joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max())) {  // 0x00523320
                    jo::self<jo::Unit>(this)->hit_points = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max());
                } else {
                    jo::self<jo::Unit>(this)->hit_points = jo::self<jo::Unit>(this)->hit_points + jo::as_item_def(node)->damage * sb;
                }
            }
            return 1;
        }
        const i32 sc = static_cast<i16>(c & 0xffffu);
        if (jo::as_item_def(node)->min_level > sc || jo::as_item_def(node)->max_level < sc) return 0;
        alignas(4) u8 cmd[sizeof(jo::WorldItem) + 2];  // 0x54 on x86
        std::memset(cmd, 0, sizeof cmd);
        joan::orig::as_world_item(cmd)->construct();  // 0x00456eb0
        joan::orig::as_item_stack(cmd)->init(joan::orig::arg::i16(static_cast<u16>(a)), joan::orig::arg::i16(static_cast<u16>(jo::as_item_def(node)->min_level)), joan::orig::arg::i16(jo::as_item_def(node)->stats[3]), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00456cd0
        i32 n = joan::orig::arg::as<i32>(joan::orig::as_inventory(jo::self<jo::Unit>(this)->modifiers)->add_495f40(joan::orig::arg::u32(static_cast<u16>(a)), joan::orig::arg::u32(static_cast<u16>(c)), joan::orig::arg::u32(static_cast<u16>(b)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->id)), joan::orig::arg::u32(jo::view<u32>(jo::as_world_item(cmd)->stack.uses)), joan::orig::arg::u32(jo::view<u32>(jo::as_world_item(cmd)->stack.max_uses)), joan::orig::arg::u32(jo::as_world_item(cmd)->stack.u32_e), joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(d & 0xffffu)))));  // 0x00495f40
        if (n > 0) {
            joan::orig::as_world_item(cmd)->destruct();  // 0x00456ef0
            return 1;
        }
        if (n < 0) n = -n;
        else n = sb;
        while (n > 0) {
            const i32 step = jo::view<i16>(jo::as_item_def(node)->stats[4]) < n ? jo::view<i16>(jo::as_item_def(node)->stats[4]) : n;
            if (joan::orig::arg::as<i32>(joan::orig::as_spawner(this)->fire_512830(joan::orig::arg::i16(static_cast<u16>(a)), joan::orig::arg::i16(static_cast<u16>(c)), joan::orig::arg::i16(static_cast<u16>(step)), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}))) == 0)  // 0x00512830
                break;
            n -= jo::view<i16>(jo::as_item_def(node)->stats[4]);
        }
        joan::orig::as_world_item(cmd)->destruct();
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Unit::split_50fd90", 0x0050fd90, &H::split_50fd90);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using u32 = std::uint32_t;


// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x005102a0
    void toggle_5102a0(i32 off) {
        if (off != 0) {
            jo::self<jo::Unit>(this)->scripted_move = 0;
            u8* const o = jo::view<u8*>(jo::self<jo::Unit>(this)->actor);
            if (jo::view<u32>(jo::as_actor(o)->record) == 0) {
                // `mov cx, [this+0x102]; push ecx` with ecx = this: the upper half is not reproduced on x64 (the pointer
                // does not fit; fire_shot reads the type as a word, i16).
                const u32 w = jo::view<u16>(jo::self<jo::Unit>(this)->layer);

                const u32 r = joan::orig::arg::as<u32>(joan::orig::as_army(obj(d::g_physic_sys))->fire_shot(joan::orig::arg::mptr(o), joan::orig::arg::u32(u32{0x42700000}), joan::orig::arg::u32(w), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{1})));  // 0x006adbf0
                jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->record) = r;
            } else {
                joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(o)->record)), joan::orig::arg::i16(i32{1}));  // 0x006ad980
            }
            return;
        }
        jo::self<jo::Unit>(this)->scripted_move = 1;
        if (jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->record) != 0) {
            joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->record)), joan::orig::arg::i16(i32{0}));
        }
        jo::view<u32>(jo::self<jo::Unit>(this)->move_target[0]) = jo::view<u32[3]>(jo::self<jo::Unit>(this)->position_)[0];
        jo::view<u32>(jo::self<jo::Unit>(this)->move_target[1]) = jo::view<u32[3]>(jo::self<jo::Unit>(this)->position_)[1];
        jo::view<u32>(jo::self<jo::Unit>(this)->move_target[2]) = jo::view<u32[3]>(jo::self<jo::Unit>(this)->position_)[2];
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::toggle_5102a0", 0x005102a0, &H::toggle_5102a0);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

using u32 = std::uint32_t;


using joan::orig::helpers::units;

JOAN_COUNTER(c_group_avg, "Unit::group_avg_5103c0");

struct H {
    // ------------------------------------------------------------------ 0x005103c0
    i32 group_avg() {
        c_group_avg.hit();
        if (jo::self<jo::Unit>(this)->group_id == 0) return jo::self<jo::Unit>(this)->stats[7];
        if (jo::self<jo::Unit>(this)->group == nullptr) {
            void* const leader = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->leader());  // 0x004216d0
            return joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->group_avg_5103c0());               // 0x005103c0
        }
        u32 sum = static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->stats[7]));
        i16 n = 1;
        if (jo::as_group(jo::self<jo::Unit>(this)->group)->count == 0) return static_cast<i32>(sum);
        for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->count); ++i) {
            const i32 id = jo::as_group(jo::self<jo::Unit>(this)->group)->members[i];
            if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(id)) != 0) continue;
            void* const m = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id));
            sum += static_cast<u32>(static_cast<i32>(jo::as_unit(m)->stats[7]));
            n = static_cast<i16>(n + 1);
        }
        return static_cast<i32>(sum) / static_cast<i32>(n);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Unit::group_avg_5103c0", 0x005103c0, &H::group_avg);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b106_15.cpp (game/b106_15)
namespace {
namespace game_b106_15 {

using u32 = std::uint32_t;


JOAN_COUNTER(c_anim_set, "Unit::set_anim_set");

u8* unit_type(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(type))); }  // 0x0042d0a0

struct H {

    // ---- 0x00511060 -------------------------------------------------------------------
    // Detach (arg 0 pushed) or re-attach the light of unit slot `id_field` (carried_a / carried_b).
    void light_detach(i16 jo::Unit::*id_field) {
        void* const units = obj(d::g_items);
        if (joan::orig::as_units(units)->list.empty_at(static_cast<i32>(jo::self<jo::Unit>(this)->*id_field)) != 0) return;  // 0x00425d50
        u8* u = joan::orig::arg::as<u8*>(joan::orig::as_units(units)->list.ptr_at(static_cast<i32>(jo::self<jo::Unit>(this)->*id_field)));  // 0x00425d20
        if (jo::as_map_object(u)->actor == nullptr) return;
        u = joan::orig::arg::as<u8*>(joan::orig::as_units(units)->list.ptr_at(static_cast<i32>(jo::self<jo::Unit>(this)->*id_field)));
        joan::orig::as_bone(jo::view<void*>(jo::as_map_object(u)->actor->bone))->set_parent(joan::orig::arg::mptr(0));  // 0x0060ec70
    }
    void light_attach(i16 jo::Unit::*id_field) {
        void* const units = obj(d::g_items);
        if (joan::orig::as_units(units)->list.empty_at(static_cast<i32>(jo::self<jo::Unit>(this)->*id_field)) != 0) return;
        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_units(units)->list.ptr_at(static_cast<i32>(jo::self<jo::Unit>(this)->*id_field)));
        if (jo::as_map_object(u)->actor == nullptr) return;
        i32 v = 0;
        if (jo::as_actor(jo::self<jo::Unit>(this)->actor)->def->model->bones != nullptr) {
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(u)->type_id)));  // 0x00425ce0
            const i32 key = jo::view<i16>(jo::as_item_def(t)->bone);
            v = joan::orig::arg::as<i32>(joan::orig::as_bone_table(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->def->model->bones))->find(key));  // 0x005ba160
        }
        void* const w = joan::orig::as_plane(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->plane))->bone_5cf8f0(joan::orig::arg::i32(v));  // 0x005cf8f0
        joan::orig::as_bone(jo::view<void*>(jo::as_map_object(u)->actor->bone))->set_parent(w);                    // 0x0060ec70
    }
    void light_325() {
        if (jo::self<jo::Unit>(this)->weapon_model != nullptr) joan::orig::as_bone(jo::view<void*>(jo::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->bone))->set_parent(joan::orig::arg::mptr(0));  // 0x0060ec70
    }

    void set_anim_set(i32 index) {
        c_anim_set.hit();
        if (jo::self<jo::Unit>(this)->collision_id == index) return;
        u8* const type = unit_type(jo::self<jo::Unit>(this)->type_id);
        const u32 old_set = jo::view<u32>(jo::as_unit_type(type)->model_ids[jo::self<jo::Unit>(this)->collision_id]);  // [ebp-0x18]
        const u32 new_set = jo::view<u32>(jo::as_unit_type(type)->model_ids[index]);         // [ebp-0x1c]
        const u8 shared = jo::self<jo::Unit>(this)->actor_c == jo::self<jo::Unit>(this)->actor ? 1 : 0;    // [ebp-0x20]
        jo::Actor* const had_anim = jo::self<jo::Unit>(this)->actor_c;                           // [ebp-0x94]
        jo::AnimSet* const old_311 = jo::self<jo::Unit>(this)->anim_set_c;                   // [ebp-0x98]
        if (jo::self<jo::Unit>(this)->actor_c != nullptr) joan::orig::as_panel(type)->open(joan::orig::arg::i32(index));  // 0x00527520

        u32 saved_5c = 0;                  // [ebp-0x24]
        u32 saved_60[3] = {}, saved_6c[3] = {};
        u32 saved_b8 = 0;                  // [ebp-0x40]
        u32 saved_bc[4] = {};              // [ebp-0x50]
        u32 saved_cc[6] = {};              // [ebp-0x68]
        u32 saved_e4[5] = {};              // [ebp-0x7c]
        u32 saved_44 = 0, saved_48 = 0, saved_4c = 0, saved_50 = 0;
        jo::AnimDef* saved_04 = nullptr;
        if (new_set != old_set && had_anim != nullptr) {
            if (shared != 0) {
                saved_5c = jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record);
                if (jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                    u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
                    jo::as_shot(r)->actor = nullptr;
                    jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) = 0;
                }
                std::memcpy(saved_60, jo::as_actor(jo::self<jo::Unit>(this)->actor)->velocity, 12);
                std::memcpy(saved_6c, jo::as_actor(jo::self<jo::Unit>(this)->actor)->impulse, 12);
                saved_b8 = jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode;
                const i32 kind = jo::view<i32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode);
                // kind 3: collide_kind .. collide_height, 20 bytes without a pointer in between (same on x64)
                static_assert(offsetof(jo::Actor, collide_height) + sizeof(float) - offsetof(jo::Actor, collide_kind) == 20, "collide block");
                if (kind == 1) std::memcpy(saved_bc, jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_sphere, 16);
                else if (kind == 2) std::memcpy(saved_cc, jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_box, 24);
                else if (kind == 3) std::memcpy(saved_e4, &jo::as_actor(jo::self<jo::Unit>(this)->actor)->collide_kind, 20);
                if (jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene != nullptr) joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene))->detach(joan::orig::arg::mptr(jo::view<u8*>(jo::self<jo::Unit>(this)->actor)));  // 0x005efdb0
                saved_44 = jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->action);
                saved_48 = jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->sub_action);
                saved_4c = jo::as_actor(jo::self<jo::Unit>(this)->actor)->start_time;
                saved_50 = jo::as_actor(jo::self<jo::Unit>(this)->actor)->anim_flags;
                saved_04 = jo::as_actor(jo::self<jo::Unit>(this)->actor)->anim;
                light_detach(&jo::Unit::carried_a);
                light_detach(&jo::Unit::carried_b);
                light_325();
                u8* const old = jo::view<u8*>(jo::self<jo::Unit>(this)->actor);
                if (old != nullptr) joan::orig::as_scene_object(old)->destroy(1u);  // 0x00420840 scalar deleting dtor
                jo::self<jo::Unit>(this)->actor = nullptr;
                jo::self<jo::Unit>(this)->actor_c = nullptr;
            } else {
                u8* const old = jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c);
                if (old != nullptr) joan::orig::as_scene_object(old)->destroy(1u);  // 0x00420840
                jo::self<jo::Unit>(this)->actor_c = nullptr;
            }
        }
        const i32 old_index = jo::self<jo::Unit>(this)->collision_id;  // [ebp-0xa0]
        jo::self<jo::Unit>(this)->collision_id = index;
        if (new_set != old_set && had_anim != nullptr) {
            u8* const set = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit_type(unit_type(jo::self<jo::Unit>(this)->type_id))->model_ids[jo::self<jo::Unit>(this)->collision_id]))));  // 0x0041a180
            jo::self<jo::Unit>(this)->anim_set_c = joan::orig::arg::as<jo::AnimSet*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit_type(unit_type(jo::self<jo::Unit>(this)->type_id))->model_flags[jo::self<jo::Unit>(this)->collision_id]))));  // 0x004b2730
            jo::self<jo::Unit>(this)->actor_def_c = jo::as_model_entry(set)->actor_def;
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // operator new (0x10c on x86)
            void* const anim = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
            jo::self<jo::Unit>(this)->actor_c = static_cast<jo::Actor*>(anim);
            jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->def = jo::self<jo::Unit>(this)->actor_def_c;
            joan::orig::as_scene_object(jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c))->init();                        // 0x005aab70
            joan::orig::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c))->set_skin(joan::orig::arg::u32(jo::view<u32>(jo::as_model_entry(set)->anim_db_id)));    // 0x005add40
            joan::orig::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c))->set_position_5ac310(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->position_));          // 0x005ac310
            jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->kind) = 0xb;
            joan::en3d::unit_index_actor_kind(jo::self<jo::Unit>(this)->actor_c);  // [Debug] UnitIndex
            jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->owner = this;
            joan::orig::as_scene_object(jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c))->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)));          // 0x005ac6a0
            for (i32 i = 0; i < jo::as_model_entry(set)->morph_count; ++i) {
                jo::ModelMorphRec* const entry = &jo::as_model_entry(set)->morphs[i];
                joan::orig::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c))->add_part(joan::orig::arg::str(entry->name), jo::as_model_entry(set)->dir, joan::orig::arg::str(entry->file), nullptr);  // 0x005adf50
                if (joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(jo::as_model_entry(set)->morphs[i].anim))) != 0) {  // strlen
                    joan::orig::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor_c))->attach(joan::orig::arg::str(jo::as_model_entry(set)->morphs[i].name), str(d::s_ANIM), joan::orig::arg::str(jo::as_model_entry(set)->morphs[i].anim));  // 0x005ae400, "ANIM\\"
                }
            }
            if (jo::self<jo::Unit>(this)->scene != nullptr && shared != 0) joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor_c));  // 0x005efc50
            if (shared != 0) {
                jo::self<jo::Unit>(this)->actor = jo::self<jo::Unit>(this)->actor_c;
                jo::self<jo::Unit>(this)->anim_set = jo::self<jo::Unit>(this)->anim_set_c;
                if (saved_5c != 0) {
                    jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) = saved_5c;
                    u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
                    jo::as_shot(r)->actor = jo::self<jo::Unit>(this)->actor;
                }
                std::memcpy(jo::as_actor(jo::self<jo::Unit>(this)->actor)->velocity, saved_60, 12);
                std::memcpy(jo::as_actor(jo::self<jo::Unit>(this)->actor)->impulse, saved_6c, 12);
                jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode = saved_b8;
                const i32 kind = jo::view<i32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode);
                if (kind == 1) std::memcpy(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_sphere, saved_bc, 16);
                else if (kind == 2) std::memcpy(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_box, saved_cc, 24);
                else if (kind == 3) std::memcpy(&jo::as_actor(jo::self<jo::Unit>(this)->actor)->collide_kind, saved_e4, 20);
                if (jo::self<jo::Unit>(this)->anim_set_c == old_311) {
                    jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->action) = saved_44;
                    jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->sub_action) = saved_48;
                    jo::as_actor(jo::self<jo::Unit>(this)->actor)->start_time = saved_4c;
                    jo::as_actor(jo::self<jo::Unit>(this)->actor)->anim_flags = saved_50;
                    jo::as_actor(jo::self<jo::Unit>(this)->actor)->anim = saved_04;
                } else if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::Unit>(this)->anim_set_c))->index_of_416dc0(static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind))) != 0) {  // 0x00416dc0
                    jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry) = 0;
                    const u32 a = joan::orig::arg::as<u32>(joan::orig::as_cell_object(this)->find_anim_state());  // 0x00504010
                    joan::orig::as_cell_object(this)->set_anim_state(joan::orig::arg::i32(a), joan::orig::arg::i32(saved_4c), joan::orig::arg::i32(-1));                   // 0x004ff9f0
                } else {
                    joan::orig::as_list(jo::view<void*>(jo::self<jo::Unit>(this)->anim_set_c))->index_of_416dc0(1);  // 0x00416dc0
                    joan::orig::as_unit(this)->set_action(1, 0, 1);             // 0x00501190
                    jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry) = 0;
                    const u32 now = global<u32>(d::g_now);  // pushed before the find call in the original
                    const u32 a = joan::orig::arg::as<u32>(joan::orig::as_cell_object(this)->find_anim_state());  // 0x00504010
                    joan::orig::as_cell_object(this)->set_anim_state(joan::orig::arg::i32(a), joan::orig::arg::i32(now), 0);                                       // 0x004ff9f0
                }
                light_attach(&jo::Unit::carried_a);
                light_attach(&jo::Unit::carried_b);
                light_325();
            }
        }
        if (had_anim != nullptr) joan::orig::as_unit_type(type)->release_model_slot(joan::orig::arg::i32(old_index));  // 0x00527690
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_15", name, addr, fn)
B("Unit::set_anim_set", 0x00511060, &H::set_anim_set);
#undef B

}  // namespace game_b106_15
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;

void* const kUnits = obj(d::g_units);
void* const kObjects = obj(d::g_items);
void* const kSides = obj(d::g_nations);

std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }   // 0x0041b3c0

struct H {
    std::uintptr_t self_bits() const { return reinterpret_cast<std::uintptr_t>(this); }

    // 0x005059b0
    float face_object(std::int32_t id) {  // float in st0 (0x00505a00 heading)
        std::uint8_t* const o = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
        return joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to(joan::orig::arg::mptr(static_cast<void*>(jo::as_map_object(o)->pos))));  // 0x00505a00
    }

    // 0x00508fb0
    std::uintptr_t is_kind_40_42() {
        const std::int32_t k = jo::self<jo::Unit>(this)->anim_kind;
        const std::uint32_t v = (k >= 0x40 && k <= 0x42) ? 1u : 0u;
        return (self_bits() & 0xffff0000u) | v;
    }

    // 0x005091d0
    std::uint32_t is_kind_e_14() {
        std::int16_t w = jo::self<jo::Unit>(this)->anim_kind;
        if (jo::view<std::int8_t>(jo::self<jo::Unit>(this)->use_action_alt) != 0) w = jo::self<jo::Unit>(this)->anim_kind_alt;
        const std::int32_t k = w;
        const std::uint32_t v = (k >= 0xe && k <= 0x14) ? 1u : 0u;
        return (static_cast<std::uint32_t>(k) & 0xffff0000u) | v;
    }

    // 0x00509590
    std::int32_t relation_to_side(std::int16_t side) {
        if (side == 0) return 0;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_players(kSides)->hostile(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side), static_cast<std::int32_t>(side)));  // 0x004b6860
    }

    // 0x005095e0
    std::uintptr_t is_allied_with(std::int16_t id) {
        void* const a = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->nation());  // 0x00509400
        if (a == nullptr) return 0;
        std::uint8_t* const b = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(unit_get(static_cast<std::int32_t>(id)))->nation());
        if (b == nullptr) return 0;
        if (b == a) return (reinterpret_cast<std::uintptr_t>(b) & 0xffffff00u) | 1u;
        const std::int8_t r = joan::orig::arg::as<std::int8_t>(joan::orig::as_table(a)->byte_58(joan::orig::arg::i16(jo::as_nation(b)->id)));  // 0x004b4360
        return r >= 4 ? 1u : 0u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Unit::face_object", 0x005059b0, &H::face_object);
B("Unit::is_kind_40_42", 0x00508fb0, &H::is_kind_40_42);
B("Unit::is_kind_e_14", 0x005091d0, &H::is_kind_e_14);
B("Unit::relation_to_side", 0x00509590, &H::relation_to_side);
B("Unit::is_allied_with", 0x005095e0, &H::is_allied_with);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_units_2.cpp (game/units1)
namespace {
namespace game_units_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::global;

using joan::call::thiscall;

constexpr img::Addr kGameUnits(d::g_units);

JOAN_COUNTER(c_relation, "Unit::hostile_to");

struct H {
    // 0x005094d0 Unit::hostile_to(id): 0 for gone / neutral-dead units, else the relation
    // table (0xa57f80) entry for the two players.
    std::int32_t hostile_to(std::int16_t id) {
        c_relation.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kGameUnits))->empty_at_b(static_cast<std::int32_t>(id))) != 0) return 0;  // 0x00428b70
        std::uint8_t* const other = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGameUnits))->get_b(static_cast<std::int32_t>(id)));  // 0x0041b3c0
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(other)->dead())) != 0 &&                     // 0x0042d0e0
            static_cast<std::int32_t>(jo::as_unit(other)->id) != global<std::int32_t>(d::g_player_unit_recno))
            return 0;
        if (jo::as_unit(other)->side == 0) return 0;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_players(img::obj(d::g_nations))->hostile(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side), static_cast<std::int32_t>(jo::as_unit(other)->side))) != 0  // 0x004b6860
                   ? 1
                   : 0;
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("Unit::hostile_to", 0x005094d0, &H::hostile_to);
#undef U1

}  // namespace game_units_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::GroundObject::update_position() -> void { joan::bind::member<&::game_objects1::H::update_position>(this); }  // 0x4fee60
auto joan::orig::GroundObject::id() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_objects1::H::id>(this)); }  // 0x4ff830
auto joan::orig::GroundObject::heading() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_objects1::H::heading>(this)); }  // 0x4ff850
auto joan::orig::CellObject::set_anim_state(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b82_08::H::set_anim_state>(this, a0, a1, a2); }  // 0x4ff9f0
auto joan::orig::Unit::set_213(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_213>(this, a0)); }  // 0x500d10
auto joan::orig::Unit::has_order_for(std::uint32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b103_05::H::has_order_for>(this, a0)); }  // 0x500d40
auto joan::orig::Unit::set_action(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::member<&::game_b88_01::H::set_action_>(this, a0, a1, a2); }  // 0x501190
auto joan::orig::Unit::pick_target_503680(const char* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_01::H::pick_target_503680>(this, a0, a1, a2)); }  // 0x503680
auto joan::orig::Player::set_9d() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_9d>(this)); }  // 0x503fb0
auto joan::orig::Player::set_a8(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::set_a8>(this, a0)); }  // 0x503fe0
auto joan::orig::CellObject::find_anim_state() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_08::H::find_anim_state>(this)); }  // 0x504010
auto joan::orig::Unit::anim_phase() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b83_07::H::anim_phase>(this)); }  // 0x5045f0
auto joan::orig::Unit::build_progress() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b98_04::H::progress>(this)); }  // 0x504760
auto joan::orig::Unit::link_value(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b89_06::H::link_value>(this, a0)); }  // 0x504800
auto joan::orig::Actor::attack_value(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b107_14::H::attack_value>(this, a0)); }  // 0x5048b0
auto joan::orig::Unit::hit_value_5051e0(std::int32_t a0, std::uint32_t a1, std::uint32_t a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b105_06::H::hit_value_5051e0>(this, a0, a1, a2)); }  // 0x5051e0
auto joan::orig::Unit::attack_value_505650(std::uint32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_07::H::attack_value>(this, a0)); }  // 0x505650
auto joan::orig::Unit::can_face_505820(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_04::H::can_face>(this, a0)); }  // 0x505820
auto joan::orig::Unit::heading_to_unit(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b88_06::H::set_from>(this, a0)); }  // 0x505960
auto joan::orig::Unit::face_object(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b91_02::H::face_object>(this, a0)); }  // 0x5059b0
auto joan::orig::Unit::heading_to(void* a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b89_05::H::heading_to>(this, a0)); }  // 0x505a00
auto joan::orig::Unit::value_of_type(std::uint32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_07::H::value_of_type>(this, a0)); }  // 0x505b50
auto joan::orig::Unit::bearing_to(void* a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b95_04::H::bearing_to>(this, a0)); }  // 0x505ba0
auto joan::orig::Unit::set_view(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b100_07::H::set_view>(this, a0, a1, a2); }  // 0x505cc0
auto joan::orig::Players::set_b1a(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_05::H::set_b1a>(this, a0)); }  // 0x505d60
auto joan::orig::Marker::launch_from_unit(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b109_09::H::launch_507fc0>(this, a0, a1); }  // 0x507fc0
auto joan::orig::Unit::distance_band() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_04::H::distance_band>(this)); }  // 0x508880
auto joan::orig::Unit::set_level_state(std::int32_t a0) -> void { joan::bind::member<&::game_b87_03::H::set_level_state>(this, a0); }  // 0x5089e0
auto joan::orig::Unit::is_kind_special() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_04::H::is_kind_special>(this)); }  // 0x508ab0
auto joan::orig::Unit::play_idle(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b87_03::H::play_idle>(this, a0, a1); }  // 0x508b90
auto joan::orig::Unit::get_1f9() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_06::H::get_1f9>(this)); }  // 0x508f80
auto joan::orig::Unit::is_kind_40_42() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_02::H::is_kind_40_42>(this)); }  // 0x508fb0
auto joan::orig::Unit::pick_idle_anim(std::int32_t a0) -> void { joan::bind::member<&::game_b92_04::H::pick_idle_anim>(this, a0); }  // 0x509000
auto joan::orig::Unit::is_kind_e_14() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_02::H::is_kind_e_14>(this)); }  // 0x5091d0
auto joan::orig::Unit::nation() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b88_06::H::lookup_509400>(this)); }  // 0x509400
auto joan::orig::Unit::hostile_to(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units_2::H::hostile_to>(this, a0)); }  // 0x5094d0
auto joan::orig::Unit::relation_to_side(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::relation_to_side>(this, a0)); }  // 0x509590
auto joan::orig::Unit::is_allied_with(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_02::H::is_allied_with>(this, a0)); }  // 0x5095e0
auto joan::orig::Unit::is_ally_of(std::int16_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b93_02::H::is_ally_of>(this, a0)); }  // 0x509670
auto joan::orig::Group::total() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b98_04::H::total>(this)); }  // 0x509710
auto joan::orig::Unit::stat_pct() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b94_04::H::stat_pct>(this)); }  // 0x5097d0
auto joan::orig::Unit::refresh() -> void { joan::bind::member<&::game_b96_04::H::empty_509800>(this); }  // 0x509800
auto joan::orig::Unit::melee_damage_509950(std::uint32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b106_09::H::melee_damage>(this, a0, a1)); }  // 0x509950
auto joan::orig::Player::byte_9d() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b100_08::H::byte_9d>(this)); }  // 0x509d40
auto joan::orig::CriticalHud::mark_crit_kill() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_06::H::set1b>(this)); }  // 0x509d70
auto joan::orig::Unit::apply_damage_5(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_04::H::apply_damage_5>(this, a0)); }  // 0x509d90
auto joan::orig::Unit::damage_509e90(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b104_06::H::damage_509e90>(this, a0, a1, a2)); }  // 0x509e90
auto joan::orig::CellObject::attach_unit(std::int16_t a0, std::int32_t a1, std::int8_t a2, void* a3) -> void { joan::bind::member<&::game_b82_08::H::attach_unit>(this, a0, a1, a2, a3); }  // 0x50a480
auto joan::orig::Player::set_unit_state(std::int16_t a0, std::int8_t a1) -> void { joan::bind::member<&::game_b100_08::H::set_unit_state>(this, a0, a1); }  // 0x50a9e0
auto joan::orig::Unit::link_20e() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_04::H::link_20e>(this)); }  // 0x50ac20
auto joan::orig::Unit::update_look_dir() -> void { joan::bind::member<&::game_b101_07::H::update_look_dir>(this); }  // 0x50aca0
auto joan::orig::Unit::spawn_projectile() -> void { joan::bind::member<&::game_b103_05::H::spawn_projectile>(this); }  // 0x50afe0
auto joan::orig::Unit::clear_markers() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_06::H::clear_markers>(this)); }  // 0x50b390
auto joan::orig::Unit::heal(std::uint32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_05::H::heal>(this, a0, a1, a2)); }  // 0x50b4f0
auto joan::orig::Unit::heal_group(std::uint32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_04::H::heal_group>(this, a0, a1, a2)); }  // 0x50ba00
auto joan::orig::Unit::heal_and_mana(std::uint32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_05::H::heal_and_mana>(this, a0, a1, a2)); }  // 0x50bcd0
auto joan::orig::Cell::release_slot(std::int16_t a0, std::int16_t a1) -> void { joan::bind::member<&::game_b97_04::H::release_slot>(this, a0, a1); }  // 0x50c430
auto joan::orig::Unit::slot_event(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3, std::int16_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7) -> void { joan::bind::member<&::game_b99_05::H::slot_event>(this, a0, a1, a2, a3, a4, a5, a6, a7); }  // 0x50c4f0
auto joan::orig::Unit::set_leader(std::int32_t a0) -> void { joan::bind::member<&::game_b101_07::H::set_leader>(this, a0); }  // 0x50c680
auto joan::orig::Unit::switch_mount(std::int32_t a0) -> void { joan::bind::member<&::game_b101_07::H::switch_mount>(this, a0); }  // 0x50c8a0
auto joan::orig::Unit::on_flag(std::int32_t a0, std::uint8_t a1) -> void { joan::bind::member<&::game_b100_08::H::on_flag>(this, a0, a1); }  // 0x50cbc0
auto joan::orig::CellObject::create_alt_mesh(std::int32_t a0) -> void { joan::bind::member<&::game_b82_08::H::create_alt_mesh>(this, a0); }  // 0x50cc70
auto joan::orig::Unit::drop_alt_mesh(std::int32_t a0) -> void { joan::bind::member<&::game_b98_04::H::drop_2f9>(this, a0); }  // 0x50d050
auto joan::orig::Unit::clear_flags() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::clear_flags>(this)); }  // 0x50d0d0
auto joan::orig::Unit::has_item_kind9(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::has_item_kind9>(this, a0)); }  // 0x50d170
auto joan::orig::Table::entry(std::uint32_t a0) -> std::uint8_t* { return joan::bind::conv<std::uint8_t*>(joan::bind::member<&::en3d_b82_17::H::table_entry>(this, a0)); }  // 0x50d240
auto joan::orig::Unit::has_item(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::has_item>(this, a0)); }  // 0x50d320
auto joan::orig::Unit::find_link(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::find_link>(this, a0, a1)); }  // 0x50d3d0
auto joan::orig::Unit::find_upgrade_kind_d() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_04::H::find_kind_d>(this)); }  // 0x50d4b0
auto joan::orig::Unit::has_mask_ready(std::int8_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::has_mask_ready>(this, a0)); }  // 0x50d550
auto joan::orig::Unit::buy_stat(std::int32_t a0, std::int32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_05::H::buy_stat>(this, a0, a1, a2)); }  // 0x50d640
auto joan::orig::Unit::play_anim_sounds(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b89_01::H::play_anim_sounds_>(this, a0, a1); }  // 0x50db80
auto joan::orig::Unit::play_step_sounds(std::int32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b89_02::H::play_step_sounds>(this, a0, a1); }  // 0x50f570
auto joan::orig::Unit::split_50fd90(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::split_50fd90>(this, a0, a1, a2, a3)); }  // 0x50fd90
auto joan::orig::Unit::set_10c(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_05::H::set_10c>(this, a0)); }  // 0x510030
auto joan::orig::Unit::squad_call_4985f0(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_07::H::squad_call_4985f0>(this, a0)); }  // 0x510120
auto joan::orig::Unit::count_modifiers_of_kind(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_05::H::call_1c1_510170>(this, a0)); }  // 0x510170
auto joan::orig::Unit::squad_call_498970(std::uint32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_07::H::squad_call_498970>(this, a0, a1)); }  // 0x5101c0
auto joan::orig::Unit::emit_6afb10(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_04::H::emit_6afb10>(this, a0)); }  // 0x510220
auto joan::orig::Unit::toggle_3b8(std::int32_t a0) -> void { joan::bind::member<&::game_b108_12::H::toggle_5102a0>(this, a0); }  // 0x5102a0
auto joan::orig::Unit::group_avg_5103c0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_15::H::group_avg>(this)); }  // 0x5103c0
auto joan::orig::Unit::attack_value_5104f0() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_04::H::attack_value>(this)); }  // 0x5104f0
auto joan::orig::Unit::armour_factor() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b104_06::H::armour_factor>(this)); }  // 0x510750
auto joan::orig::Unit::defence_value() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_04::H::defence_value>(this)); }  // 0x510bf0
auto joan::orig::Unit::stat_e_scaled() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b95_04::H::stat_e_scaled>(this)); }  // 0x510e40
auto joan::orig::Unit::health_factor() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_08::H::health_factor>(this)); }  // 0x510e80
auto joan::orig::Unit::stat16_factor() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b93_02::H::stat16_factor>(this)); }  // 0x510f10
auto joan::orig::Unit::is_kind_510f80() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::is_kind_510f80>(this)); }  // 0x510f80
auto joan::orig::Unit::set_anim_set(std::int32_t a0) -> void { joan::bind::member<&::game_b106_15::H::set_anim_set>(this, a0); }  // 0x511060
