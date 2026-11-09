// game/unknown_unitupd.cpp: functions whose original source file is unknown (formerly game/_unplaced/unitupd.cpp;
// re/attribution.csv: between ww/unitres.cpp and ww/unitures.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "unit_stuck.hpp"
#include "unit_steer.hpp"
#include "animal_pen.hpp"
#include "../runtime/hitch.hpp"

#include "../x87.hpp"
#include "../original/dynarray.hpp"
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

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::img::str;

using u8 = std::uint8_t;

using joan::hook::x87::ftol;

using i8 = std::int8_t;
using u16 = std::uint16_t;

using joan::hook::x87::Fpu;

void* const kForms = obj(d::g_physic_sys);

using u32 = std::uint32_t;


u32 now() { return global<u32>(d::g_now); }

}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

void JOAN_CDECL init_a3cda4() { global<float>(d::g_a3cda4) = global<float>(d::g_a3cda0) / 2.0f; }  // 0x0052b940

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("init_a3cda4", 0x0052b940, &init_a3cda4);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_worlds2.cpp (game/worlds2)
namespace {
namespace game_worlds2 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

constexpr img::Addr kTypes(d::g_types);
constexpr img::Addr kDefs(d::g_model_res_mgr);
constexpr img::Addr kMeshes(d::g_anim_res_mgr);
constexpr img::Addr kUnits(d::g_items);
constexpr img::Addr kGameUnits(d::g_units);
constexpr img::Addr kUnitKinds(d::g_item_res);
constexpr img::Addr kAnim(d::s_ANIM);  // "ANIM\"

JOAN_COUNTER(c_create, "CellObject::create_scene_objects");

std::uint8_t* type_of(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kTypes))->get(joan::orig::arg::i16(id))); }  // 0x0042d0a0
std::uint8_t* def_of(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(img::obj(kDefs))->entry(joan::orig::arg::u32(id))); }     // 0x0041a180
// new(0x10c) + 0x005aab20 (mesh object).
void* new_mesh_object() {
    void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // operator new (0x10c on x86)
    return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
}
std::int32_t units_missing(std::int32_t id) { return joan::orig::as_units(img::obj(kUnits))->list.empty_at(joan::orig::arg::i32(id)); }  // 0x00425d50
void* units_get(std::int32_t id) { return joan::orig::as_units(img::obj(kUnits))->list.ptr_at(joan::orig::arg::i32(id)); }  // 0x00425d20
std::int32_t units_resolve(std::int32_t id) { return joan::orig::arg::as<std::int32_t>(joan::orig::as_units(img::obj(kUnits))->create_49ea90(joan::orig::arg::u32(id))); }  // 0x0049ea90

struct H {
    // The unit spawn sequence (inlined three times in the original with slot 0 / 1).
    void spawn_unit(std::int32_t id, std::int32_t slot) {
        virtual_call(units_get(id), 0x18, 1);
        virtual_call(units_get(id), 0x1c);
        void* const unit = units_get(id);
        std::uint8_t* const kind = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(img::obj(kUnitKinds))->get(joan::orig::arg::i16(jo::as_map_object(unit)->type_id)));  // 0x00425ce0
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(static_cast<std::int16_t>(id)), static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_item_def(kind)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(slot)), joan::orig::arg::mptr(0));  // 0x0050a480
    }

    // 0x0052b970.
    void create_scene_objects() {
        joan::hitch::Span hitch_span("create_scene_objects");  // [Debug] HitchLog
        c_create.hit();
        if (jo::self<jo::Unit>(this)->actor_c == nullptr) {
            std::uint8_t* def;  // ebp-0x14
            {
                std::uint8_t* const type = type_of(jo::self<jo::Unit>(this)->type_id);
                def = def_of(jo::as_unit_type(type)->model_ids[jo::self<jo::Unit>(this)->collision_id]);
            }
            {
                const std::int32_t index = jo::self<jo::Unit>(this)->collision_id;  // pushed before the type lookup
                std::uint8_t* const type = type_of(jo::self<jo::Unit>(this)->type_id);
                joan::orig::as_panel(type)->open(joan::orig::arg::i32(index));  // 0x00527520
            }
            {
                std::uint8_t* const type = type_of(jo::self<jo::Unit>(this)->type_id);
                jo::AnimSet* const mesh = joan::orig::arg::as<jo::AnimSet*>(joan::orig::as_anim_res_mgr(img::obj(kMeshes))->entry(joan::orig::arg::u32(jo::as_unit_type(type)->model_flags[jo::self<jo::Unit>(this)->collision_id])));  // 0x004b2730
                jo::self<jo::Unit>(this)->anim_set_c = mesh;
            }
            jo::self<jo::Unit>(this)->actor_def_c = jo::self<jo::ModelEntry>(def)->actor_def;
            jo::self<jo::Unit>(this)->actor_c = static_cast<jo::Actor*>(new_mesh_object());
            jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->def = jo::self<jo::Unit>(this)->actor_def_c;
            joan::orig::as_scene_object(jo::self<jo::Unit>(this)->actor_c)->init();                                        // 0x005aab70
            joan::orig::as_actor(jo::self<jo::Unit>(this)->actor_c)->set_skin(joan::orig::arg::u32(jo::self<jo::ModelEntry>(def)->anim_db_id));           // 0x005add40
            joan::orig::as_actor(jo::self<jo::Unit>(this)->actor_c)->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Unit>(this)->position_)));  // 0x005ac310
            jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->kind = 0xb;
            joan::en3d::unit_index_actor_kind(jo::self<jo::Unit>(this)->actor_c);  // [Debug] UnitIndex
            jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->owner = this;
            joan::orig::as_scene_object(jo::self<jo::Unit>(this)->actor_c)->set_yaw(joan::orig::arg::u32(jo::view<std::int32_t>(jo::self<jo::Unit>(this)->heading)));  // 0x005ac6a0
            for (std::int32_t i = 0; i < jo::self<jo::ModelEntry>(def)->morph_count; ++i) {
                joan::orig::as_actor(jo::self<jo::Unit>(this)->actor_c)->add_part(joan::orig::arg::str(static_cast<void*>(jo::self<jo::ModelEntry>(def)->morphs[i].name)), jo::self<jo::ModelEntry>(def)->dir, joan::orig::arg::str(static_cast<void*>(jo::self<jo::ModelEntry>(def)->morphs[i].file)), nullptr);  // 0x005adf50
                if (joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(static_cast<void*>(jo::self<jo::ModelEntry>(def)->morphs[i].anim)))) != 0) {  // strlen
                    joan::orig::as_actor(jo::self<jo::Unit>(this)->actor_c)->attach(joan::orig::arg::str(static_cast<void*>(jo::self<jo::ModelEntry>(def)->morphs[i].name)), img::str(kAnim), joan::orig::arg::str(static_cast<void*>(jo::self<jo::ModelEntry>(def)->morphs[i].anim)));  // 0x005ae400
                }
            }
        }
        jo::self<jo::Unit>(this)->actor = jo::self<jo::Unit>(this)->actor_c;
        jo::self<jo::Unit>(this)->anim_set = jo::self<jo::Unit>(this)->anim_set_c;

        if (jo::self<jo::Unit>(this)->weapon_model == nullptr) {
            switch (static_cast<std::int32_t>(jo::self<jo::UnitType>(type_of(jo::self<jo::Unit>(this)->type_id))->kind)) {
            case 4:
            case 5:
            case 6:
            case 9:
                jo::self<jo::Unit>(this)->weapon_model_id = global<std::int32_t>(d::g_a47c7c);
                break;
            case 8:
            case 11:
                jo::self<jo::Unit>(this)->weapon_model_id = global<std::int32_t>(d::g_a47c80);
                break;
            case 10:
                jo::self<jo::Unit>(this)->weapon_model_id = global<std::int32_t>(d::g_a47c84);
                break;
            default:  // 7 and outside 4..11
                jo::self<jo::Unit>(this)->weapon_model_id = global<std::int32_t>(d::g_a47c78);
                break;
            }
            jo::self<jo::Unit>(this)->weapon_model = static_cast<jo::SceneObject*>(new_mesh_object());
            joan::orig::as_model_res(img::obj(kDefs))->acquire(joan::orig::arg::i16(static_cast<std::int16_t>(jo::self<jo::Unit>(this)->weapon_model_id)));  // 0x004b0f40
            std::uint8_t* const def = def_of(jo::self<jo::Unit>(this)->weapon_model_id);
            jo::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->def = jo::self<jo::ModelEntry>(def)->actor_def;
            joan::orig::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->init();  // 0x005aab70
            {
                void* const source = jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone);
                joan::orig::as_bone(jo::view<void*>(jo::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->bone))->set_parent(joan::orig::arg::mptr(source));  // 0x0060ec70
            }
            joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->weapon_model));  // 0x005efc50
            joan::orig::as_model_instance(jo::self<jo::Unit>(this)->weapon_model)->set_10a(0u);                              // 0x0042cf20
        }

        if (joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->weapon)) == 0) {  // 0x00428b70
            std::int32_t id;  // ebp-0x1c
            if (units_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->carried_a)) != 0) {
                std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->weapon)));  // 0x0041b3c0
                std::uint8_t* const type = type_of(jo::as_unit(unit)->type_id);  // ebp-0x24
                if (jo::view<std::int16_t*>(jo::as_unit_type(type)->buffers[3])[jo::self<jo::Unit>(this)->weapon_slot] != 0)
                    id = units_resolve(static_cast<std::int32_t>(jo::view<std::int16_t*>(jo::as_unit_type(type)->buffers[3])[jo::self<jo::Unit>(this)->weapon_slot]));
                else
                    id = 0;
            } else {
                id = jo::self<jo::Unit>(this)->carried_a;
            }
            if (id != 0) spawn_unit(id, 0);
        } else if (jo::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->hand_mode != 0) {
            std::int32_t id;  // ebp-0x1c
            if (static_cast<std::int32_t>(jo::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->hand_mode) <= 1) {
                if (units_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->carried_a)) != 0) {
                    std::int16_t* const p = joan::orig::arg::as<std::int16_t*>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->slot(joan::orig::arg::i32(static_cast<std::int16_t>(0))));  // 0x004974f0
                    id = p != nullptr ? units_resolve(static_cast<std::int32_t>(*p)) : 0;
                } else {
                    id = jo::self<jo::Unit>(this)->carried_a;
                }
                if (id != 0) spawn_unit(id, 0);
                if (units_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->carried_b)) != 0) {
                    std::int16_t* const p = joan::orig::arg::as<std::int16_t*>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->slot(joan::orig::arg::i32(static_cast<std::int16_t>(1))));  // 0x004974f0
                    id = p != nullptr ? units_resolve(static_cast<std::int32_t>(*p)) : 0;
                } else {
                    id = jo::self<jo::Unit>(this)->carried_b;
                }
                if (id != 0) spawn_unit(id, 1);
            } else {
                if (units_missing(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->carried_b)) != 0) {
                    std::int16_t* const p = joan::orig::arg::as<std::int16_t*>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->slot_a());  // 0x00497560
                    id = p != nullptr ? units_resolve(static_cast<std::int32_t>(*p)) : 0;
                } else {
                    id = jo::self<jo::Unit>(this)->carried_b;
                }
                if (id != 0) spawn_unit(id, 1);
            }
        }

        // The third test (time - start, unsigned, jb after test) never jumps.
        if (jo::self<jo::Unit>(this)->anim_entry > 0 && jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->anim_start_ms) != 0) {
            std::uint8_t* const anim = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(jo::self<jo::Unit>(this)->anim_entry)));  // 0x004168d0, ebp-0x30
            void* const mesh = *joan::orig::arg::as<void**>(joan::orig::as_anim_res_mgr(img::obj(kMeshes))->slot_4174f0(joan::orig::arg::u32(jo::as_anim_group_rec(anim)->anim_id)));      // 0x004174f0, ebp-0x34
            const std::uint32_t elapsed = global<std::uint32_t>(d::g_now) - jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->anim_start_ms);
            const float elapsed_f = static_cast<float>(static_cast<double>(elapsed));  // fild qword (zero-extended) + fstp
            joan::orig::as_actor(jo::self<jo::Unit>(this)->actor)->start_anim(mesh, joan::orig::arg::str(static_cast<void*>(jo::as_anim_group_rec(anim)->bip_name)), joan::orig::arg::u32(jo::self<jo::Unit>(this)->anim_start_ms), joan::orig::arg::u32(jo::self<jo::Unit>(this)->anim_loop), 0u, joan::orig::arg::bits(elapsed_f));  // 0x005abe30
        } else {
            jo::self<jo::Unit>(this)->anim_entry = 0;
            const std::int32_t now = global<std::int32_t>(d::g_now);
            const std::int32_t state = joan::orig::arg::as<std::int32_t>(joan::orig::as_cell_object(this)->find_anim_state());  // 0x00504010
            joan::orig::as_cell_object(this)->set_anim_state(joan::orig::arg::i32(state), joan::orig::arg::i32(now), 0);              // 0x004ff9f0
        }

        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(this)->dead())) != 0) {  // 0x0042d0e0
            jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode) = 0;
        } else {
            std::uint8_t* const type = type_of(jo::self<jo::Unit>(this)->type_id);  // ebp-0x44
            switch (jo::self<jo::UnitType>(type)->shape_kind) {
            case 0:
                if (jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->def) != nullptr && jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->def->model) != nullptr) {
                    jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode) = 1;
                    std::memcpy(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_sphere, jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->def->model->sphere, 0x10);
                } else {
                    jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode) = 0;
                }
                break;
            case 1:
                jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode) = jo::view<std::int32_t>(jo::self<jo::UnitType>(type)->shape_kind);
                std::memcpy(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_sphere, jo::self<jo::UnitType>(type)->shape_sphere, 0x10);
                break;
            case 2:
                jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode) = jo::view<std::int32_t>(jo::self<jo::UnitType>(type)->shape_kind);
                std::memcpy(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_box, jo::self<jo::UnitType>(type)->shape_box, 0x18);
                break;
            case 3:
                jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->aim_mode) = jo::view<std::int32_t>(jo::self<jo::UnitType>(type)->shape_kind);
                std::memcpy(&jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->collide_kind, jo::self<jo::UnitType>(type)->shape_cylinder, 0x14);
                break;
            default:
                break;
            }
        }

        bool create;
        if (jo::self<jo::Unit>(this)->layer != 0 && jo::self<jo::Unit>(this)->draw == nullptr)
            create = true;
        else
            create = static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id) == static_cast<std::int32_t>(global<std::int16_t>(d::g_a47c36)) &&
                     jo::self<jo::Unit>(this)->draw == nullptr;
        if (create) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::UnitDraw)));  // operator new (0x53 on x86)
            jo::self<jo::Unit>(this)->draw = static_cast<jo::UnitDraw*>(mem != nullptr ? joan::orig::as_unit_draw(mem)->construct() : nullptr);  // 0x00520170
            joan::orig::as_unit_draw(jo::self<jo::Unit>(this)->draw)->set_unit(joan::orig::arg::i16(jo::self<jo::Unit>(this)->id));  // 0x005201d0
        }
    }
};

JOAN_REPLACE("game/worlds2", "CellObject::create_scene_objects", 0x0052b970, &H::create_scene_objects);

}  // namespace game_worlds2
}  // namespace

// ---------------------------------------- from modules/game_b109_06.cpp (game/b109_06)
namespace {
namespace game_b109_06 {

using joan::call::import_call;

struct V3 {
    float x, y;
    float z;
};
static_assert(sizeof(V3) == 12, "vec3");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);

u8* unit_type(void* u) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))); }  // 0x0042d0a0
i32 unit_missing(u32 k) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)); }  // 0x00428b70
u8* unit_rec(u32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k))); }  // 0x0041b3c0

JOAN_COUNTER(c_update, "Unit::update_52c490");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // ------------------------------------------------------------------ 0x0052c490
    u32 update(i32 phase) {
        c_update.hit();
        if (jo::self<jo::Unit>(this)->world != global<jo::World*>(d::g_world)) return 0;
        if (phase == 0) {
            phase0();
        } else if (phase == 1) {
            phase1();
        } else if (phase == 2) {
            if (joan::orig::arg::as<i8>(joan::orig::as_glory_screen(obj(d::g_glory_screen))->is_open()) != 0) return 1;  // 0x006e7b50
            phase2();
        } else if (phase == 3) {
            if (joan::orig::arg::as<i8>(joan::orig::as_glory_screen(obj(d::g_glory_screen))->is_open()) != 0) return 1;  // 0x006e7b50
            joan::orig::as_unit(this)->idle_tick();                                  // 0x0049d810
        }
        return 1;
    }
    void* form() { return jo::view<void*>(jo::self<jo::Unit>(this)->actor); }
    bool form_ok() {  // type's formation kind into +0x2f5+0xb8, then 0x00693170
        u8* const t = unit_type(this);
        jo::as_actor(form())->aim_mode = jo::self<jo::UnitType>(t)->shape_kind;
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->is_free(joan::orig::arg::mptr(form()), joan::orig::arg::u32(jo::self<jo::Unit>(this)->layer)));  // 0x00693170
        return (r & 0xff) != 0;
    }
    void phase0() {
        const Fpu fpu{true};
        jo::view<u16>(jo::self<jo::Unit>(this)->hit_by) = 0;
        jo::view<u16>(jo::self<jo::Unit>(this)->interact_item) = 0;
        joan::orig::as_unit(this)->check_pickups_49da30();  // 0x0049da30
        if (static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno)) joan::orig::as_unit(this)->tick_status_416680();  // 0x00416680
        if (jo::self<jo::Unit>(this)->trail_a != nullptr) joan::orig::as_trail(jo::view<void*>(jo::self<jo::Unit>(this)->trail_a))->attach_update();  // 0x00687a60
        if (jo::self<jo::Unit>(this)->trail_b != nullptr) joan::orig::as_trail(jo::view<void*>(jo::self<jo::Unit>(this)->trail_b))->attach_update();
        joan::orig::as_unit(this)->expire_20f();  // 0x00518b90
        if (joan::orig::arg::as<i8>(joan::orig::as_game(this)->byte_2e1()) != 0 && jo::self<jo::Unit>(this)->anim_kind != 7 && jo::self<jo::Unit>(this)->anim_set != nullptr) {  // 0x00432b90
            joan::orig::as_unit(this)->play_idle(i32{0}, i32{1});  // 0x00508b90
            if (jo::self<jo::Unit>(this)->prisoner_pose != 0) {
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(this)->has_order_for(joan::orig::arg::u32(i32{7}))) != 0) joan::orig::as_unit(this)->set_action(i32{7}, i32{0}, i32{1});  // 0x00500d40 / 0x00501190
            } else {
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(this)->has_order_for(joan::orig::arg::u32(i32{1}))) != 0) joan::orig::as_unit(this)->set_action(i32{1}, i32{0}, i32{1});
            }
        }
        if (jo::self<jo::UnitType>(unit_type(this))->kind == 0xb && jo::self<jo::Unit>(this)->cell != nullptr && jo::as_cell(jo::self<jo::Unit>(this)->cell)->tracks != nullptr)
            joan::orig::as_unit(this)->set_name_index(joan::orig::arg::mptr(str(d::s_BAT_TRK)));  // 0x00427b50 "BAT_TRK"
        if (jo::self<jo::Unit>(this)->task.kind == 0xf && unit_missing(jo::self<jo::Unit>(this)->task.target) == 0) {
            if (jo::self<jo::Unit>(this)->actor == nullptr) return;
            V3 me{0.0f, 0.0f, 0u}, him{0.0f, 0.0f, 0u};
            joan::orig::as_unit(this)->position(&me);  // 0x004fedb0
            u8* const target = unit_rec(jo::self<jo::Unit>(this)->task.target);
            joan::orig::as_unit(target)->position(&him);
            float range = 200.0f;
            const float c20 = global<float>(d::flt_20);
            if (jo::as_unit(target)->actor != nullptr) {
                const i32 kind = jo::view<i32>(jo::as_actor(jo::as_unit(target)->actor)->aim_mode);
                if (kind == 1) {
                    const float v = jo::as_actor(jo::as_unit(target)->actor)->aim_sphere[3] + c20;
                    if (v >= range) range = jo::as_actor(jo::as_unit(target)->actor)->aim_sphere[3] + c20;
                } else if (kind == 2) {
                    u8* const q = static_cast<u8*>(jo::view<void*>(jo::as_unit(target)->actor));
                    const float a1 = jo::as_actor(q)->aim_box[0] * jo::as_actor(q)->aim_box[0];
                    const float a2 = jo::as_actor(q)->aim_box[1] * jo::as_actor(q)->aim_box[1];
                    const float a28 = a1 + a2;
                    const float b1 = jo::as_actor(q)->aim_box[3] * jo::as_actor(q)->aim_box[3];
                    const float b2 = jo::as_actor(q)->aim_box[4] * jo::as_actor(q)->aim_box[4];
                    const float a2c = b1 + b2;
                    const float pick = !(a28 >= a2c) ? a2c : a28;
                    auto root20 = [&]() {
                        return fpu.add(joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(pick))), static_cast<double>(c20));  // 0x004256d0
                    };
                    const double v = root20();
                    if (v >= static_cast<double>(range)) range = static_cast<float>(root20());
                } else if (kind == 3) {
                    const float v = jo::as_actor(jo::as_unit(target)->actor)->collide_radius + c20;
                    if (v >= range) range = jo::as_actor(jo::as_unit(target)->actor)->collide_radius + c20;
                }
            }
            auto dist = [&]() {
                V3 tmp{0.0f, 0.0f, 0u};
                const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &me, &him));  // 0x0060cc90
                return v3d_float(d::iat_v3d_87_length, &d);                    // v3d length
            };
            if (!(dist() >= range)) {
                jo::as_actor(form())->aim_mode = 0;
                jo::view<u16>(jo::self<jo::Unit>(this)->mount_stick) = 1;
            } else if (dist() >= range) {
                if (!form_ok()) jo::as_actor(form())->aim_mode = 0;
            }
            if (unit_missing(jo::view<u32>(jo::self<jo::Unit>(this)->weapon)) == 0)
                joan::orig::as_players(kForms)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(form())->record)), joan::orig::arg::i16(i32{0}));  // 0x006ad980
            return;
        }
        if (jo::self<jo::Unit>(this)->mount_stick == 1 && jo::self<jo::Unit>(this)->actor != nullptr) {
            if (!form_ok()) {
                jo::as_actor(form())->aim_mode = 0;
            } else {
                u8* const fo = joan::orig::arg::as<u8*>(joan::orig::as_players(kForms)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(form())->record))));  // 0x006af310
                joan::orig::as_anim_table(kForms)->restart_6af370(joan::orig::arg::u32(jo::view<u32>(jo::as_actor(form())->record)), joan::orig::arg::u32(jo::as_shot(fo)->u32_27a), joan::orig::arg::u32(jo::view<i16[2]>(jo::as_shot(fo)->type)[0]), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{1}));  // 0x006af370
                jo::view<u16>(jo::self<jo::Unit>(this)->mount_stick) = 0;
            }
        }
        if (jo::self<jo::Unit>(this)->task.kind == 0xb && jo::view<i32>(jo::self<jo::Unit>(this)->task.arg_d) > 0)
            jo::self<jo::Unit>(this)->task.arg_d = jo::self<jo::Unit>(this)->task.arg_d - global<u32>(d::g_frame_step_ms);
    }
    void phase1() {
        const Fpu fpu{true};
        joan::orig::as_unit(this)->update_links_5174e0();  // 0x005174e0
        joan::orig::as_unit(this)->detach_rider();  // 0x00454b20
        if (jo::view<i8>(jo::self<jo::Unit>(this)->levelup_flag) != 0 && now() - jo::view<u32>(jo::self<jo::Unit>(this)->levelup_time) > global<u32>(d::g_a3cc58)) jo::self<jo::Unit>(this)->levelup_flag = 0;
        if (jo::self<jo::Unit>(this)->actor != nullptr) {
            joan::orig::as_unit(this)->type_behaviour_7c7ab0();                   // 0x007c7ab0
            const u32 grp = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->turn_step());   // 0x004f2190
            bool release = grp == 0;
            if (!release && jo::self<jo::Unit>(this)->layer == 0) release = joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) == 0;  // 0x004301c0
            if (release) joan::orig::as_unit(this)->move_update_52daa0();  // 0x0052daa0
            if (grp != 0 && jo::view<u32>(jo::as_actor(form())->record) != 0) {
                u8* const fo = joan::orig::arg::as<u8*>(joan::orig::as_players(kForms)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(form())->record))));  // 0x006af310
                jo::as_shot(fo)->state.vec0_x = 0;
                jo::view<u32>(jo::as_shot(fo)->state.vec0_y) = 0;
                jo::as_shot(fo)->state.vel_len = 0;
            }
        }
        if (jo::self<jo::Unit>(this)->weapon_model != nullptr) {
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(this)->dead()) != 0) joan::orig::as_model_instance(jo::view<void*>(jo::self<jo::Unit>(this)->weapon_model))->set_10a(joan::orig::arg::u32(i32{0}));  // 0x0042d0e0 / 0x0042cf20
            if (jo::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->scene != jo::self<jo::Unit>(this)->scene) {
                if (jo::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->scene != nullptr)
                    joan::orig::as_scene(jo::view<void*>(jo::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Unit>(this)->weapon_model)));  // 0x005efdb0
                joan::orig::as_scene(jo::view<void*>(jo::self<jo::Unit>(this)->scene))->attach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Unit>(this)->weapon_model)));  // 0x005efc50
            }
        }
        if (jo::view<i8>(jo::self<jo::Unit>(this)->u8_202) != 0) {
            if (!form_ok()) {
                jo::self<jo::Unit>(this)->u32_3fe = 0x14;
                jo::as_actor(form())->aim_mode = 0;
            } else {
                jo::self<jo::Unit>(this)->u8_202 = 0;
            }
        }
        if (jo::self<jo::Unit>(this)->model_b != nullptr) joan::orig::as_unit(this)->release_anim_321();  // 0x004956f0
        bool reset151 = true;
        if (jo::view<u32>(jo::self<jo::Unit>(this)->hit_points) != 0) {
            const i32 cap = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->value_149());  // 0x00523700
            if (jo::self<jo::Unit>(this)->mana < cap) {
                reset151 = false;
                if (now() - jo::view<u32>(jo::self<jo::Unit>(this)->last_ep_regen_ms) >= global<u32>(d::g_a3ccac)) {
                    jo::view<u32>(jo::self<jo::Unit>(this)->last_ep_regen_ms) = now();
                    if (global<i8>(d::g_a5d384) == 0)
                        joan::orig::as_unit(this)->set_145(static_cast<i32>(jo::view<u32>(jo::self<jo::Unit>(this)->mana) + global<u32>(d::g_a3cd20)));  // 0x004fe5d0
                }
            }
        }
        if (reset151) jo::view<u32>(jo::self<jo::Unit>(this)->last_ep_regen_ms) = now();
        if (jo::self<jo::Unit>(this)->morale < 0x64 && now() - jo::view<u32>(jo::self<jo::Unit>(this)->morale_time) >= 0x3e8u) {
            jo::view<u32>(jo::self<jo::Unit>(this)->morale_time) = now();
            const i16 kind = jo::self<jo::Unit>(this)->anim_kind;
            if (kind == 0x40 || kind == 0x41 || kind == 0x42 || kind == 0x29) {
                const float cur = static_cast<float>(static_cast<i32>(jo::self<jo::Unit>(this)->morale));
                const i32 n = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->level_sum());  // 0x005238c0
                double x = fpu.mul(static_cast<double>(n), global<float>(d::g_a3ccd0));
                x = fpu.add(x, global<float>(d::flt_1));
                x = fpu.mul(x, global<float>(d::flt_10));
                x = fpu.add(x, cur);
                joan::orig::as_unit(this)->set_morale(joan::orig::arg::i32(ftol(x)));  // 0x004fea80
            } else {
                const i32 next = static_cast<i32>(jo::self<jo::Unit>(this)->morale) + 1;
                const double y = fpu.mul(global<float>(d::flt_10), global<float>(d::g_a3ccd0));
                joan::orig::as_unit(this)->set_morale(joan::orig::arg::i32(ftol(fpu.add(static_cast<double>(next), y))));  // 0x004fea80
            }
        }
        bool reset118 = true;
        if (jo::view<u32>(jo::self<jo::Unit>(this)->hit_points) != 0) {
            const i32 cap = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max());  // 0x00523320
            if (jo::self<jo::Unit>(this)->hit_points < cap && jo::self<jo::Unit>(this)->markers != -1) {
                reset118 = false;
                if (now() - jo::view<u32>(jo::self<jo::Unit>(this)->last_hp_regen_ms) >= global<u32>(d::g_a3cc04)) {
                    jo::view<u32>(jo::self<jo::Unit>(this)->last_hp_regen_ms) = now();
                    if (global<i8>(d::g_a5d384) == 0) {
                        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->level_sum());  // 0x005238c0
                        const double v = fpu.mul(static_cast<double>(n), global<float>(d::g_a3cbf0));
                        float step = 1.0f;
                        if (v > static_cast<double>(global<float>(d::flt_1))) {
                            const i32 n2 = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->level_sum());
                            step = static_cast<float>(fpu.mul(static_cast<double>(n2), global<float>(d::g_a3cbf0)));
                        }
                        const double x = fpu.add(static_cast<double>(jo::self<jo::Unit>(this)->hit_points), step);
                        joan::orig::as_unit(this)->set_value_135(joan::orig::arg::i32(ftol(x)));  // 0x004fe510
                    }
                }
            }
        }
        if (reset118) jo::view<u32>(jo::self<jo::Unit>(this)->last_hp_regen_ms) = now();
        if (jo::self<jo::Unit>(this)->hit_value > 0) jo::view<u32>(jo::self<jo::Unit>(this)->hit_value) = jo::view<u32>(jo::self<jo::Unit>(this)->hit_value) - global<u32>(d::g_frame_step_ms);
        if (jo::view<u32>(jo::self<jo::Unit>(this)->upg_timer) != 0 && now() - jo::view<u32>(jo::self<jo::Unit>(this)->upg_timer) >= global<u32>(d::g_a3cc24)) {
            if (jo::self<jo::UnitType>(unit_type(this))->is_hero == 0) {
                joan::orig::as_unit(this)->distribute_levels();  // 0x004a95a0
                jo::view<u32>(jo::self<jo::Unit>(this)->upg_timer) = 0;
            }
        }
        jo::self<jo::Unit>(this)->last_update_ms = now();
    }
    void phase2() {
        joan::orig::as_unit(this)->recruit_nearby_4655a0();  // 0x004655a0
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->leads()) != 0) joan::orig::as_group(jo::view<void*>(jo::self<jo::Unit>(this)->group))->arrange(i32{0}, i32{1});  // 0x0044a660 / 0x004fa0f0
        if (jo::self<jo::Unit>(this)->orders != nullptr) joan::orig::as_ai(jo::view<void*>(jo::self<jo::Unit>(this)->orders))->update_4baf80(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004baf80
        const i32 s = jo::self<jo::Unit>(this)->move_mode;
        if (s == 1) {
            if (jo::self<jo::Unit>(this)->actor == nullptr || jo::view<u32>(jo::as_actor(form())->record) == 0) return;
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0) return;  // 0x00430190
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->get_1f5()) != 0) return;  // 0x004f2f60
            if (unit_missing(jo::view<u32>(jo::self<jo::Unit>(this)->weapon)) == 0) return;
            if (jo::self<jo::Unit>(this)->anim_kind == 0x51 || jo::self<jo::Unit>(this)->anim_kind == 0x52) return;
            if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return;  // 0x0042d690
            if (jo::self<jo::Unit>(this)->scripted_move != 0) return;
            joan::orig::as_players(kForms)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(form())->record)), joan::orig::arg::i16(i32{1}));  // 0x006ad980
            return;
        }
        if (s <= 1 || s > 4) return;
        static const i16 kSkip[] = {0xa,  0xb,  0x44, 0x46, 0x45, 0x1c, 0x1d, 0x1b, 0x23, 0x24, 0x25, 0x1e,
                                    0x1f, 0x20, 0x21, 0x3a, 0x3b, 0x3c, 0x53, 0x22, 0x15, 0x16, 0x17};
        const i16 kind = jo::self<jo::Unit>(this)->anim_kind;
        for (const i16 k : kSkip)
            if (kind == k) return;
        if (jo::self<jo::Unit>(this)->actor == nullptr || jo::view<u32>(jo::as_actor(form())->record) == 0) return;
        u8* const fo = joan::orig::arg::as<u8*>(joan::orig::as_players(kForms)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(form())->record))));  // 0x006af310
        if (jo::as_shot(fo)->state.goal_len > global<float>(d::k_9cf5c4)) return;
        if (jo::view<i16>(jo::as_shot(fo)->state.goal_flag) != 0) return;
        joan::orig::as_players(kForms)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(form())->record)), joan::orig::arg::i16(i32{0}));  // 0x006ad980
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_06", name, addr, fn)
B("Unit::update_52c490", 0x0052c490, &H::update);
#undef B

}  // namespace game_b109_06
}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
namespace {
namespace game_b102_05 {

void JOAN_CDECL init_52d940() { joan::orig::init_a3cda8(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("init_52d940", 0x0052d940, &init_52d940);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

// 0x0052da00
void JOAN_CDECL init_a3cda8() { global<float>(d::g_a3cda8) = global<float>(d::g_a3cda0) / global<float>(d::flt_3); }

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("init_a3cda8", 0x0052da00, &init_a3cda8);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b105_07.cpp (game/b105_07)
namespace {
namespace game_b105_07 {

// CRT initialiser forwarders (direct calls)
template <fn::Code Target>
u32 JOAN_CDECL forward() {
    return cdecl_call<u32>(Target);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("init_52da80", 0x0052da80, &forward<fn::div_a3cdac>);
#undef B

}  // namespace game_b105_07
}  // namespace

// ---------------------------------------- from modules/game_b110_14.cpp (game/b110_14)
namespace {
namespace game_b110_14 {

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "vec3");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);

u8* unit_rec(u32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k))); }  // 0x0041b3c0
i32 unit_missing(u32 k) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)); }  // 0x00428b70
u8* form_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_players(kForms)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x006af310

// fcomp + `test ah,0x40` taken: equal or unordered.
bool fcom_eq(float a, float b) { return !(a < b) && !(a > b); }
u32 bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

JOAN_COUNTER(c_move, "Unit::move_update_52daa0");

u8* unit_type(const void* u) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))); }  // 0x0042d0a0
void order(void* u, i32 o) { joan::orig::as_unit(u)->set_action(joan::orig::arg::i32(o), i32{0}, i32{1}); }              // 0x00501190

// Formation member i gets order `want` unless it already has it.
void member_order(void* self, i32 i, i32 want) {
    if (jo::as_unit(unit_rec(jo::view<u32>(jo::as_arr_i32(jo::as_unit(self)->mask_slots)[i].value)))->anim_kind != want) {
        order(unit_rec(jo::view<u32>(jo::as_arr_i32(jo::as_unit(self)->mask_slots)[i].value)), want);
    }
}
// Order fall-back used at several path ends (0x52eaa9 has no +0x213 test, 0x52eb79 / 0x52f0f7 do).
void idle_order(void* self, bool check213) {
    if (jo::view<u32>(jo::as_unit(self)->mount_id) == 0 && (!check213 || jo::as_unit(self)->action == 0)) {
        if (!(static_cast<double>(global<i32>(d::g_a3ccd8)) >= static_cast<double>(jo::view<float>(jo::as_unit(self)->path_remaining)))) {
            order(self, 0xb);
        } else {
            order(self, 0xa);
        }
    } else if (jo::view<u32>(jo::as_unit(self)->mount_id) != 0 && jo::as_unit(self)->anim_kind != 0x46) {
        joan::orig::as_unit(self)->hit_effect_466d50();  // 0x00466d50
    }
}
void clear_form_motion(void* self) {
    if (jo::as_unit(self)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::as_unit(self)->actor)->record) != 0) {
        u8* const f = form_rec(jo::view<u32>(jo::as_actor(jo::as_unit(self)->actor)->record));
        jo::as_shot(f)->state.vec0_x = 0;
        jo::view<u32>(jo::as_shot(f)->state.vec0_y) = 0;
        jo::as_shot(f)->state.vel_len = 0;
    }
}
void path_done(void* self) {
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(self)->mounted()) != 0) {  // 0x00430190
        joan::orig::as_unit(self)->start_paired_anim();         // 0x004533e0
    } else if (unit_missing(jo::view<u32>(jo::as_unit(self)->rider_id)) != 0) {
        joan::orig::as_unit(self)->play_idle(i32{0}, i32{1});  // 0x00508b90
    }
}
// The end of a route (follow_path, 0x0052daa0): the route cleared, the retry delay set (Gameplay.res INIT_W_C,
// 0x00a3cd04) and the end-of-route actions.
void end_route(void* self) {
    joan::orig::as_path(&jo::as_unit(self)->path)->clear();  // 0x004eeca0
    jo::view<u32>(jo::as_unit(self)->path_index) = 0;
    jo::as_unit(self)->wait_limit = global<u32>(d::g_unit_init_w_c);
    jo::as_unit(self)->wait_count = jo::as_unit(self)->wait_limit;
    path_done(self);
}
void own_or_leader_pos(void* self, V3* out) {
    if (joan::orig::arg::as<i32>(joan::orig::as_unit(self)->mounted()) != 0) {                               // 0x00430190
        joan::orig::as_unit(unit_rec(jo::view<u32>(jo::as_unit(self)->mount_id)))->position(joan::orig::arg::mptr(out));     // 0x004fedb0
    } else {
        joan::orig::as_unit(self)->position(joan::orig::arg::mptr(out));
    }
}
float sqr_dist(float ax, float bx, float ay, float by) {
    const float dx = ax - bx;
    const float dy = ay - by;
    return dx * dx + dy * dy;
}

// ------------------------------------------------------------------ thiscall functions
struct H {
    void member_orders() {
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})))) == 0 || jo::self<jo::UnitType>(unit_type(this))->kind == 9) {  // 0x0050d550
            if (jo::self<jo::Unit>(this)->anim_kind == 10 || jo::self<jo::Unit>(this)->anim_kind == 11) joan::orig::as_unit(this)->play_idle(i32{0}, i32{1});
            return;
        }
        if (jo::self<jo::Unit>(this)->anim_kind != 10 && jo::self<jo::Unit>(this)->anim_kind != 11) return;
        for (i32 i = static_cast<i32>(jo::self<jo::UnitType>(unit_type(this))->slot_count - 1u); i >= 0; --i) {
            if (jo::as_arr_i32(jo::self<jo::Unit>(this)->mask_slots)[i].value <= 0) continue;
            const u32 kind = static_cast<u32>(static_cast<i32>(jo::self<jo::UnitType>(unit_type(this))->kind) - 4);
            switch (kind) {
                case 0:  // type 4
                    if ((jo::view<u8*>(jo::as_unit_type(unit_type(this))->buffers[2])[i] & 1) != 0) {
                        member_order(this, i, 0x56);
                    } else {
                        member_order(this, i, 0xa);
                    }
                    break;
                case 1:  // types 5, 8, 9
                case 4:
                case 5:
                    if ((jo::view<u8*>(jo::as_unit_type(unit_type(this))->buffers[2])[i] & 1) != 0) {
                        member_order(this, i, 0x54);
                    } else {
                        member_order(this, i, 0xa);
                    }
                    break;
                case 7:  // type 11
                    if ((jo::view<u8*>(jo::as_unit_type(unit_type(this))->buffers[2])[i] & 1) != 0) {
                        member_order(this, i, 0x54);
                    } else {
                        member_order(this, i, 1);
                    }
                    break;
                default: break;
            }
        }
    }

    static bool fight_order(i32 o) {
        return o == 0xb || o == 0xa || o == 9 || o == 0x46 || o == 0x44 || o == 0x45 || o == 0xe || o == 0xf ||
               o == 0x10 || o == 0x15 || o == 0x17 || o == 0x16 || o == 0x11 || o == 0x12 || o == 0x13 || o == 0x14;
    }
    static bool fight_order2(i32 o) {
        return o == 0xb || o == 0xa || o == 9 || o == 0xe || o == 0xf || o == 0x10 || o == 0x11 || o == 0x12 ||
               o == 0x13 || o == 0x14;
    }
    static bool anchor_order(i32 o) {
        return o == 0x1c || o == 0x1d || o == 0x1b || o == 0x23 || o == 0x24 || o == 0x25 || o == 0x1e ||
               o == 0x1f || o == 0x20 || o == 0x21 || o == 0x3a || o == 0x3b || o == 0x3c || o == 0x53 ||
               o == 0x22 || o == 0x15 || o == 0x16 || o == 0x17;
    }

    // Formation anchor follows its path (0x52e1cd .. 0x52e4a0).
    void anchor_motion() {
        u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry))));       // 0x004168d0
        u8* const b = joan::orig::arg::as<u8*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_anim_group_rec(a)->anim_id))));  // 0x004174f0
        const std::uintptr_t c = reinterpret_cast<std::uintptr_t>(jo::self<jo::AnimSlot>(b)->buffer);
        if (c == 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_path(obj(c + (jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->action) - 1u) * 0x25cu))->has_points()) == 0) return;  // 0x00418400
        void* const path = obj(c + (jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->action) - 1u) * 0x25cu);
        const i32 d1c = static_cast<i32>(now() - jo::as_actor(jo::self<jo::Unit>(this)->actor)->start_time);
        i32 d20 = static_cast<i32>(static_cast<u32>(d1c) - global<u32>(d::g_frame_step_ms));
        if (d20 < 0) d20 = 0;
        float x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
        joan::orig::as_path(path)->lookup(joan::orig::arg::f32(static_cast<float>(d20)), &x0, &y0);  // 0x00418a00
        joan::orig::as_path(path)->lookup(joan::orig::arg::f32(static_cast<float>(d1c)), &x1, &y1);
        const float heading = joan::orig::arg::as<float>(joan::orig::as_ground_object(this)->heading());  // 0x004ff850
        u32 ang = 0;                                               // angle object [ebp-0x30]
        joan::orig::as_angle(static_cast<void*>(&ang))->construct_577190();     // 0x00577190
        joan::orig::as_angle(static_cast<void*>(&ang))->set_wrapped(joan::orig::arg::f32(heading * global<float>(d::flt_1)));  // 0x00577340
        if (global<u32>(d::g_frame_elapsed_ms) == 0) return;
        const float c40 = joan::orig::arg::as<float>(joan::orig::as_turns(static_cast<void*>(&ang))->cos_577b50());  // 0x00577b50 (cos)
        const float c44 = joan::orig::arg::as<float>(joan::orig::as_turns(static_cast<void*>(&ang))->sin_577b10());  // 0x00577b10 (sin)
        V3 v{};
        v.x = (x1 - x0) * c40 - (y1 - y0) * c44;
        v.y = (x1 - x0) * c44 + (y1 - y0) * c40;
        v.z = 0.0f;
        const Fpu fpu{true};
        const float k = static_cast<float>(fpu.div(static_cast<double>(global<float>(d::flt_21_691975)),
                                                   static_cast<double>(global<u32>(d::g_frame_step_ms))));
        joan::orig::vec_scale_in_place(&v, joan::orig::arg::f32(k));  // 0x0060ce80 (scale)
        if (jo::view<i8>(jo::self<jo::Unit>(this)->anim_done) != 0) return;
        joan::orig::as_unit(this)->set_vec_6c(v);  // 0x00505d90 (vector by value)
        if (jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) {
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_army(kForms)->fire_shot(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), joan::orig::arg::u32(u32{0x42700000}), joan::orig::arg::u32(jo::view<u16>(jo::self<jo::Unit>(this)->layer)), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{1})));  // 0x006adbf0 (60.0f)
            jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) = id;
        }
        if (jo::self<jo::UnitType>(unit_type(this))->kind == 2) {
            joan::orig::vec_scale_in_place(joan::orig::arg::mptr(jo::as_actor(jo::self<jo::Unit>(this)->actor)->impulse), joan::orig::arg::f32(u32{0x40000000}));  // 0x0060ce80 (* 2)
        }
        u8* const form = form_rec(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
        if (jo::self<jo::Unit>(this)->last_update_ms != 0) {
            u32 dt = 0;
            if (now() - jo::self<jo::Unit>(this)->last_update_ms > global<u32>(d::g_frame_step_ms)) {
                dt = now() - jo::self<jo::Unit>(this)->last_update_ms;
            } else {
                dt = global<u32>(d::g_frame_step_ms);
            }
            joan::orig::as_body(form)->start(joan::orig::arg::mptr(jo::as_actor(jo::self<jo::Unit>(this)->actor)->impulse), joan::orig::arg::i32(now()), joan::orig::arg::i32(dt));  // 0x006b10a0
        } else {
            joan::orig::as_body(form)->start(joan::orig::arg::mptr(jo::as_actor(jo::self<jo::Unit>(this)->actor)->impulse), joan::orig::arg::i32(now()), joan::orig::arg::i32(global<u32>(d::g_frame_step_ms)));
        }
    }

    // [Fixes] AnimalPen (game/animal_pen.hpp): an animal (UnitType::kind 7, unit.res Special ANIMAL) whose steps the
    // physics refuses for leaving the navigation mesh (the pigs in their pens) gets no wander route, and a route it has
    // ends: it keeps its idle animations instead of walking on the spot.
    bool penned_animal() {
        if (!joan::hook::settings().fix_animal_pen) return false;
        if (jo::self<jo::Unit>(this)->actor == nullptr || jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) == 0) return false;
        if (jo::self<jo::UnitType>(unit_type(this))->kind != 7) return false;
        const u8* const body = form_rec(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
        return body != nullptr && joan::animal_pen::off_mesh(body, this, jo::as_shot(body)->state.zone != 0);
    }

    // 0x52e842 .. 0x52eb0e (delay counting down). Always returns from the function afterwards.
    void delay_step(const V3& me) {
        jo::view<i32>(jo::self<jo::Unit>(this)->wait_count) = static_cast<i32>(jo::self<jo::Unit>(this)->wait_count - 1u);
        if (jo::view<i32>(jo::self<jo::Unit>(this)->wait_count) > 0) return;
        if (jo::view<u32>(jo::self<jo::Unit>(this)->i32_3f2) == 0) {
            joan::orig::as_squad(this)->clear();  // 0x00422f10
            if (!penned_animal()) joan::orig::as_unit(this)->wander_4f7bf0();  // 0x004f7bf0
        } else {
            if (jo::self<jo::Unit>(this)->name_index == 0 || joan::orig::arg::as<i32>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->count_66e8a0()) == 0 || jo::self<jo::Unit>(this)->cell == nullptr ||
                jo::as_cell(jo::self<jo::Unit>(this)->cell)->tracks == nullptr) {
                joan::orig::as_squad(this)->clear();
            }
        }
        jo::self<jo::Unit>(this)->wait_limit = jo::self<jo::Unit>(this)->wait_limit + 1u;
        if (jo::view<i32>(jo::self<jo::Unit>(this)->wait_limit) >= global<i32>(d::g_a3ccdc)) jo::self<jo::Unit>(this)->wait_limit = global<u32>(d::g_unit_init_w_c);
        jo::self<jo::Unit>(this)->wait_count = jo::self<jo::Unit>(this)->wait_limit;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->count_66e8a0()) == 0) return;  // 0x0066e8a0
        const float* const wp = joan::orig::arg::as<float*>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->from_end(i32{1}));  // 0x0066e8d0
        if (fcom_eq(wp[0], me.x) && fcom_eq(wp[1], me.y)) {
            joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(u32{0}));  // 0x0050abd0
        } else {
            u32 ang = 0;
            joan::orig::as_angle(static_cast<void*>(&ang))->construct_577190();  // 0x00577190
            const double dx = static_cast<double>(wp[0] - me.x);
            const double dy = static_cast<double>(wp[1] - me.y);
            joan::orig::as_angle(static_cast<void*>(&ang))->from_xy(joan::orig::arg::f64(dx), joan::orig::arg::f64(dy));       // 0x00577b90
            const float a = joan::orig::arg::as<float>(joan::orig::as_angle(static_cast<void*>(&ang))->degrees());  // 0x00577450
            if (!fcom_eq(joan::orig::arg::as<float>(joan::orig::as_unit(this)->float_1e()), a)) {                 // 0x0050ac00
                joan::orig::as_unit(this)->float_1e();                                // (difference unused)
                joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(bits(a)));
                clear_form_motion(this);
                return;
            }
        }
        idle_order(this, false);
    }

    // 0x52eda4 .. 0x52f1fe: walk the +0x432 waypoint list.
    void follow_path() {
        V3 me{};
        own_or_leader_pos(this, &me);  // [ebp-0xa0]
        joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->from_end(static_cast<i32>(jo::view<u32>(jo::self<jo::Unit>(this)->path_index) + 1u));  // 0x0066e8d0
        u32 reach = global<u32>(d::g_waypoint_reach_dist);
        u8* const type = unit_type(this);
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0 || jo::self<jo::UnitType>(type)->unit_class == 8) {
            reach = jo::self<jo::Unit>(this)->move_mode > 1 ? global<u32>(d::g_waypoint_reach_dist) * 10u : global<u32>(d::g_waypoint_reach_dist) * 5u;
        } else if (jo::self<jo::Unit>(this)->move_mode > 1) {
            reach = global<u32>(d::g_waypoint_reach_dist) * 5u;
        }
        for (;;) {
            if (jo::self<jo::Unit>(this)->path_index >= joan::orig::arg::as<i32>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->count_66e8a0())) break;
            const float* const wp =
                joan::orig::arg::as<float*>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->from_end(static_cast<i32>(jo::view<u32>(jo::self<jo::Unit>(this)->path_index) + 1u)));
            const float d = sqr_dist(me.x, wp[0], me.y, wp[1]);
            const i32 next = static_cast<i32>(jo::view<u32>(jo::self<jo::Unit>(this)->path_index) + 1u);
            if (next == joan::orig::arg::as<i32>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->count_66e8a0())) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0 || jo::self<jo::UnitType>(type)->unit_class == 8 || jo::self<jo::Unit>(this)->move_mode > 1) {
                    reach = global<u32>(d::g_waypoint_reach_dist) << 1;
                }
                if (jo::view<u32>(jo::self<jo::Unit>(this)->task.kind) == 0xf) reach = global<u32>(d::g_waypoint_reach_dist);
            }
            const i32 sq = static_cast<i32>(reach * reach);
            if (static_cast<double>(sq) >= static_cast<double>(d)) {
                jo::view<u32>(jo::self<jo::Unit>(this)->path_index) = jo::view<u32>(jo::self<jo::Unit>(this)->path_index) + 1u;
                continue;
            }
            float aim_x = wp[0], aim_y = wp[1];
            joan::unit_steer::aim(this, me.x, me.y, aim_x, aim_y);  // around a unit standing in the way
            if (fcom_eq(aim_x, me.x) && fcom_eq(aim_y, me.y)) {
                joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(u32{0}));  // 0x0050abd0
            } else {
                u32 ang = 0;
                joan::orig::as_angle(static_cast<void*>(&ang))->construct_577190();  // 0x00577190
                const double dx = static_cast<double>(aim_x - me.x);
                const double dy = static_cast<double>(aim_y - me.y);
                joan::orig::as_angle(static_cast<void*>(&ang))->from_xy(joan::orig::arg::f64(dx), joan::orig::arg::f64(dy));          // 0x00577b90
                const float a = joan::orig::arg::as<float>(joan::orig::as_angle(static_cast<void*>(&ang))->degrees());  // 0x00577450
                const float diff = joan::orig::arg::as<float>(joan::orig::as_unit(this)->float_1e()) - a;               // 0x0050ac00
                const double ad = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(diff))));  // CRT fabs
                if (ad > global<double>(d::dbl_3)) {                                         // 3.0
                    joan::orig::as_unit(this)->float_1e();  // (difference unused)
                    joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(bits(a)));
                    clear_form_motion(this);
                    return;
                }
            }
            idle_order(this, true);
            break;
        }
        if (jo::self<jo::Unit>(this)->path_index == joan::orig::arg::as<i32>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->count_66e8a0())) end_route(this);
    }

    // ------------------------------------------------------------------ 0x0052daa0
    void move_update() {
        c_move.hit();
        if (jo::self<jo::Unit>(this)->weapon > 0) return;
        if (jo::view<i32>(jo::self<jo::UnitType>(unit_type(this))->slot_count) > 0) member_orders();
        // 0x52de42
        if (jo::self<jo::Unit>(this)->anim_kind == 0x2a || joan::orig::arg::as<i8>(joan::orig::as_unit(this)->dead()) != 0) {  // 0x0042d0e0
            if (jo::self<jo::Unit>(this)->anim_kind != 0x3b) return;
        }
        if (jo::view<i32>(jo::self<jo::Unit>(this)->wait_count) < jo::view<i32>(jo::self<jo::Unit>(this)->wait_limit) && jo::view<u32>(jo::self<jo::Unit>(this)->rider_id) == 0) {
            jo::view<i32>(jo::self<jo::Unit>(this)->wait_count) = static_cast<i32>(jo::self<jo::Unit>(this)->wait_count - 1u);
            if (jo::view<i32>(jo::self<jo::Unit>(this)->wait_count) > 0) return;
        }
        if (fight_order(jo::self<jo::Unit>(this)->anim_kind)) {
            joan::orig::as_unit(this)->steer_4f0b50();  // 0x004f0b50
        } else if (jo::view<i8>(jo::self<jo::Unit>(this)->use_action_alt) != 0 && fight_order2(jo::self<jo::Unit>(this)->anim_kind_alt)) {
            joan::orig::as_unit(this)->steer_4f0b50();
        }
        if (jo::self<jo::Unit>(this)->actor != nullptr && anchor_order(jo::self<jo::Unit>(this)->anim_kind)) anchor_motion();
        // 0x52e4a5
        if (jo::view<i8>(jo::self<jo::Unit>(this)->anim_done) == 0 && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
            u8* const form = form_rec(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
            if (jo::view<i16>(jo::as_shot(form)->state.on_ground) == 0) {
                if (static_cast<i32>(jo::self<jo::Unit>(this)->id) != global<i32>(d::g_player_unit_recno) || jo::self<jo::Unit>(this)->layer != 0) {
                    joan::orig::as_squad(this)->clear();  // 0x00422f10
                    jo::self<jo::Unit>(this)->wait_count = jo::self<jo::Unit>(this)->wait_limit;
                    joan::orig::as_unit(this)->play_idle(i32{0}, i32{1});  // 0x00508b90
                }
                jo::self<jo::Unit>(this)->anim_done = 1;
            } else if (!(jo::as_shot(form)->state.move_len > global<float>(d::flt_0_00015)) &&
                       !(jo::as_shot(form)->state.vec0_z > global<float>(d::k_9cf5c4))) {
                if (jo::self<jo::Unit>(this)->anim_kind == 5 || jo::self<jo::Unit>(this)->anim_kind == 6 || jo::self<jo::Unit>(this)->anim_kind == 8) {
                    order(this, 0x18);
                    jo::self<jo::Unit>(this)->anim_done = 0;
                    jo::self<jo::Unit>(this)->view = 0;
                }
            }
        }
        // 0x52e5e8
        if (jo::view<i8>(jo::self<jo::Unit>(this)->anim_done) != 0 && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
            u8* const form = form_rec(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
            bool release = true;
            if (jo::view<i16>(jo::as_shot(form)->state.on_ground) == 0) {
                const float lim = (jo::view<float>(jo::as_shot(form)->state.cell) * global<float>(d::flt_1)) / global<float>(d::flt_46_1) +
                                  global<float>(d::flt_0_5);
                if (!(lim > jo::as_shot(form)->state.pos_z)) {
                    release = false;
                } else if (jo::as_shot(form)->state.move_len > global<float>(d::flt_0_00015) && jo::as_shot(form)->state.move_end >= now()) {
                    release = false;
                } else if (jo::as_shot(form)->state.vec0_z > global<float>(d::k_9cf5c4)) {
                    release = false;
                }
            }
            if (release) {
                u8* const t = unit_type(this);
                if (jo::self<jo::UnitType>(t)->kind != 2 && jo::self<jo::Unit>(this)->action == 1 && jo::self<jo::Unit>(this)->anim_kind != 0x1a) order(this, 0x18);
                jo::self<jo::Unit>(this)->anim_done = 0;
                jo::self<jo::Unit>(this)->view = 0;
                return;
            }
        }
        // 0x52e711
        if (jo::view<i8>(jo::self<jo::Unit>(this)->anim_done) == 0 && jo::self<jo::Unit>(this)->view != 0 && now() - jo::self<jo::Unit>(this)->view > 1000u) {
            order(this, 0x18);
            jo::self<jo::Unit>(this)->view = 0;
        }
        if (joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) == 0 &&  // 0x004301c0
            static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno)) {
            if (static_cast<i32>(jo::self<jo::Unit>(this)->id) != global<i32>(d::g_player_unit_recno) || global<u32>(d::g_control_mode) != 2) {
                if (jo::self<jo::Unit>(this)->layer == 0) return;
            }
        }
        if (unit_missing(global<u32>(d::g_player_unit_recno)) != 0) return;
        V3 hero{};  // [ebp-0x68], only feeds a dead distance
        joan::orig::as_unit(unit_rec(global<u32>(d::g_player_unit_recno)))->position(&hero);  // 0x004fedb0
        V3 me{};    // [ebp-0x74]
        own_or_leader_pos(this, &me);
        // `if (1)`: the else block at 0x52f203 is unreachable.
        if (jo::view<i32>(jo::self<jo::Unit>(this)->wait_count) < jo::view<i32>(jo::self<jo::Unit>(this)->wait_limit)) {
            delay_step(me);
            return;
        }
        if (jo::self<jo::Unit>(this)->scripted_move != 0) {
            const float s = sqr_dist(me.x, jo::self<jo::Unit>(this)->move_target[0], me.y, jo::self<jo::Unit>(this)->move_target[1]);
            const i32 n = ftol(static_cast<double>(s));
            if (!(static_cast<double>(n) == static_cast<double>(global<float>(d::k_9cf5c4)))) idle_order(this, true);
            return;
        }
        const i32 route_points = joan::orig::arg::as<i32>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->count_66e8a0());  // 0x0066e8a0 (a count: the original reads it twice)
        if (route_points == 0) return;
        if (jo::self<jo::Unit>(this)->path_index == route_points) return;
        if (jo::self<jo::Unit>(this)->actor != nullptr) {
            u8* const form = form_rec(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record));
            if (form != nullptr && joan::orig::arg::as<i16>(joan::orig::as_body(form)->bump_pending()) == 1) {  // 0x004fd3d0
                if (jo::self<jo::Unit>(this)->stuck_count == 0) jo::self<jo::Unit>(this)->stuck_ms = now();
                jo::self<jo::Unit>(this)->stuck_count = jo::self<jo::Unit>(this)->stuck_count + 1u;
            } else {
                jo::self<jo::Unit>(this)->stuck_count = static_cast<i32>(jo::self<jo::Unit>(this)->stuck_count - 1u) > 0 ? jo::self<jo::Unit>(this)->stuck_count - 1u : 0u;
                if (jo::self<jo::Unit>(this)->stuck_count == 0) jo::self<jo::Unit>(this)->stuck_ms = now();
            }
            if (now() - jo::self<jo::Unit>(this)->stuck_ms > 500u && jo::self<jo::Unit>(this)->stuck_count != 0) {
                jo::self<jo::Unit>(this)->stuck_count = 0;
                jo::self<jo::Unit>(this)->stuck_ms = now();
                jo::self<jo::Unit>(this)->wait_count = jo::self<jo::Unit>(this)->wait_count - 1u;
                joan::orig::as_unit(this)->aim_at_player(joan::orig::arg::i32(jo::as_shot(form)->hit_obj));  // 0x004360d0
                path_done(this);
                return;
            }
        }
        // [Fixes] AnimalPen: a penned animal's route (one it had before, e.g. from a save) ends as at its end.
        if (penned_animal()) {
            end_route(this);
            return;
        }
        // [Fixes] UnitStuck (game/unit_stuck.hpp): a unit pushing against an obstacle the path data does not know
        // (no unit bump, no progress) ends its route as at its end (follow_path's end-of-route block).
        if (joan::hook::settings().fix_unit_stuck && joan::hook::settings().fix_unit_stuck_watch &&
            [&] {
                const float* const waypoint = joan::orig::arg::as<const float*>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->from_end(  // 0x0066e8d0
                    static_cast<i32>(jo::view<u32>(jo::self<jo::Unit>(this)->path_index) + 1u)));  // the point follow_path walks to
                return waypoint != nullptr &&
                       joan::unit_stuck::watch(this, jo::self<jo::Unit>(this)->id, me.x, me.y, jo::self<jo::Unit>(this)->path_index, waypoint[0], waypoint[1],
                                               jo::self<jo::Unit>(this)->move_target[0], jo::self<jo::Unit>(this)->move_target[1], now());
            }()) {
            end_route(this);
            return;
        }
        follow_path();
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_14", name, addr, fn)
B("Unit::move_update_52daa0", 0x0052daa0, &H::move_update);
#undef B

}  // namespace game_b110_14
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

void JOAN_CDECL div_a3cdac() {  // 0x0052f900
    global<float>(d::g_a3cdac) = global<float>(d::g_a3cda0) / global<float>(d::flt_4);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("div_a3cdac", 0x0052f900, &div_a3cdac);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

void JOAN_CDECL init_52b8c0() { joan::orig::init_a3cda0(); }
void JOAN_CDECL init_52b910() { joan::orig::init_a3cda4(); }

JOAN_REPLACE(M, "init_52b8c0", 0x0052b8c0, &init_52b8c0);
JOAN_REPLACE(M, "init_52b910", 0x0052b910, &init_52b910);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_a3cda4() -> void { joan::bind::free<&::game_b98_04::init_a3cda4>(); }  // 0x52b940
auto joan::orig::CellObject::create_scene_objects() -> void { joan::bind::member<&::game_worlds2::H::create_scene_objects>(this); }  // 0x52b970
auto joan::orig::Unit::update_52c490(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_06::H::update>(this, a0)); }  // 0x52c490
auto joan::orig::init_a3cda8() -> void { joan::bind::free<&::game_b96_04::init_a3cda8>(); }  // 0x52da00
auto joan::orig::Unit::move_update_52daa0() -> void { joan::bind::member<&::game_b110_14::H::move_update>(this); }  // 0x52daa0
