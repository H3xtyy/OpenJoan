// game/unknown_unit_c.cpp: functions whose original source file is unknown (formerly game/_unplaced/unit_c.cpp;
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
using joan::hook::x87::Fpu;

using joan::hook::x87::ftol;

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::str;

using i8 = std::int8_t;

using joan::call::cdecl_call;
using joan::call::import_call;

using u16 = std::uint16_t;

void* const kObjects = obj(d::g_items);

}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

// _ftol(int * float) at PC24.
std::int32_t scaled(std::int32_t v, float k) {
    const joan::hook::x87::Fpu fpu{true};
    return joan::hook::x87::ftol(fpu.mul(static_cast<double>(v), static_cast<double>(k)));
}

struct H {
    // 0x00511f80
    std::int16_t scaled_2df() {
        const std::int32_t v = jo::self<jo::Unit>(this)->attack_chance;
        switch (static_cast<std::int32_t>(global<std::int8_t>(d::g_difficulty))) {
            case 0: return static_cast<std::int16_t>(scaled(v, global<float>(d::g_a39184)));
            case 1: return static_cast<std::int16_t>(scaled(v, global<float>(d::g_a3919c)));
            case 2: return static_cast<std::int16_t>(scaled(v, global<float>(d::g_a391a8)));
            case 3: return static_cast<std::int16_t>(scaled(v, global<float>(d::g_a39190)));
            default: return jo::self<jo::Unit>(this)->attack_chance;
        }
    }

    // 0x00512070
    std::int16_t scaled_a3cc90() {
        switch (static_cast<std::int32_t>(global<std::int8_t>(d::g_difficulty))) {
            case 0: return static_cast<std::int16_t>(scaled(global<std::int32_t>(d::g_unit_blockper), global<float>(d::g_a391a4)));
            case 1: return static_cast<std::int16_t>(scaled(global<std::int32_t>(d::g_unit_blockper), global<float>(d::g_a39188)));
            case 2: return static_cast<std::int16_t>(scaled(global<std::int32_t>(d::g_unit_blockper), global<float>(d::g_a391a0)));
            case 3: return static_cast<std::int16_t>(scaled(global<std::int32_t>(d::g_unit_blockper), global<float>(d::g_a39198)));
            default: return global<std::int16_t>(d::g_unit_blockper);
        }
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Unit::scaled_2df", 0x00511f80, &H::scaled_2df);
M("Game::scaled_a3cc90", 0x00512070, &H::scaled_a3cc90);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

JOAN_COUNTER(c_facing_sign, "Unit::facing_sign");

struct H {
    // ------------------------------------------------------------------------ 0x00512130
    i16 facing_sign() {
        c_facing_sign.hit();
        if (global<i32>(d::g_player_unit_recno) == static_cast<i32>(jo::self<jo::Unit>(this)->id)) return 0;
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
        if (static_cast<i32>(jo::self<jo::UnitType>(e)->unit_class) == 8) return 0;
        const i32 v = joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(global<i32>(d::g_a3cca4)));  // 0x00590540
        const i32 s = joan::orig::as_misc(obj(d::g_misc))->random(2) != 0 ? -1 : 1;  // 0x00590540
        return static_cast<i16>(static_cast<u32>(v) * static_cast<u32>(s));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Unit::facing_sign", 0x00512130, &H::facing_sign);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::orig::helpers::addr;

struct alignas(4) Text {  // 0x005a11a0 path string, [ebp-0x198] .. [ebp-0x4]
    u8 b[0x194];
};

void* const kUnits = obj(d::g_units);
void* const kSfx = obj(d::g_sfx_player);

JOAN_COUNTER(c_select_speech, "Unit::select_speech");

struct H {
    // ------------------------------------------------------------------ 0x005121c0
    std::uintptr_t select_speech(u32 group) {
        if (jo::self<jo::Unit>(this)->speech_off == 1) return addr(this);
        c_select_speech.hit();
        std::uintptr_t r = 0;
        bool speak = true;
        if (global<i32>(d::g_control_mode) != 4) {
            r = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());  // 0x004203c0
            if (static_cast<i32>(r) == static_cast<i32>(jo::self<jo::Unit>(this)->id)) speak = false;
        }
        if (speak) {
            Text path;  // [ebp-0x198]
            joan::orig::as_string(&path)->construct(str(d::s_RTS));      // 0x005a11a0 "RTS"
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_player(obj(d::g_player_unit_recno))->name_tag(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->id))));  // 0x00453da0
            joan::orig::as_string(&path)->append(joan::orig::arg::str(name));  // 0x005a1550
            if (jo::view<u32>(jo::self<jo::Unit>(this)->task.kind) == 0 && jo::self<jo::Unit>(this)->task.target != 0) {
                std::memcpy(&jo::self<jo::Unit>(this)->task, &jo::self<jo::Unit>(this)->task_next, sizeof(jo::self<jo::Unit>(this)->task));
                joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task_next)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 1u, 0u, 0u, 0u);  // 0x004e4520
                r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_string(&path)->append(str(d::s_007)));  // "007"
            } else {
                std::memcpy(&jo::self<jo::Unit>(this)->task_next, &jo::self<jo::Unit>(this)->task, sizeof(jo::self<jo::Unit>(this)->task));
                joan::orig::as_unit_task(&jo::self<jo::Unit>(this)->task)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 1u, 0u, 0u, 0u);  // 0x004e4520
                jo::self<jo::Unit>(this)->task.target = 1;
                r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_string(&path)->append(str(d::s_006)));  // "006"
            }
            if (jo::self<jo::Unit>(this)->rank >= 4) {
                const char* s = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
                r = joan::orig::arg::as<u32>(joan::orig::as_sound(kSfx)->play_kind(s));                    // 0x00492c40
            }
        }
        if (jo::self<jo::Unit>(this)->orders != nullptr) {
            if (jo::view<u32>(jo::as_order(jo::self<jo::Unit>(this)->orders)->current.kind) != 0) joan::orig::as_action(jo::self<jo::Unit>(this)->orders)->dispatch(joan::orig::arg::mptr(this));  // 0x004bb660
            void* const p = jo::self<jo::Unit>(this)->orders;
            if (p != nullptr) joan::orig::as_ai(p)->destroy(1u);  // 0x00415220 deleting dtor
            jo::self<jo::Unit>(this)->orders = nullptr;
            r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_squad(this)->clear());  // 0x00422f10
        }
        if (group == 0) return r;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->leads()) == 0) return 0;  // 0x0044a660
        r = addr(jo::view<u8*>(jo::self<jo::Unit>(this)->group));
        for (i32 i = static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->count) - 1; i >= 0; --i) {
            const i32 id = static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->members[i]);
            const u32 dead = joan::orig::arg::as<u32>(joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)));  // 0x00428b70
            if (dead != 0) {
                r = dead;
                continue;
            }
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
            r = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->select_speech(0u));                     // 0x005121c0
        }
        return r;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Unit::select_speech", 0x005121c0, &H::select_speech);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (fix/uninit)
namespace {
namespace game_b111_16 {

JOAN_COUNTER(c_slot, "Unit::set_formation_slot_5124b0");

struct H {
    // -------------------------------------------------------------- 0x005124b0 (fix/uninit)
    void set_formation_slot(i16 code) {
        c_slot.hit();
        if (code != 0xb) joan::orig::as_player(obj(d::g_player_unit_recno))->set_a8(0u);  // 0x00503fe0
        jo::self<jo::Unit>(this)->u8_373 = 0;
        if ((code == 0xa || code == 0xb) && jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0 &&
            jo::self<jo::Unit>(this)->scripted_move == 0) {
            joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::i16(1));  // 0x006ad980
        }
        if (jo::self<jo::Unit>(this)->anim_kind == 0x2a && jo::self<jo::Unit>(this)->formation_slot != 1) return;
        // Fix: [ebp-0x8] is never initialised; with no list (+0x2f1 == 0) or no match the
        // original passes stack garbage to 0x0050f570 and tests it before 0x0050db80. 0 = "none"
        // (the original's own `!= 0` test).
        i32 idx = 0;
        void* const list = jo::self<jo::Unit>(this)->anim_set;
        if (jo::self<jo::Unit>(this)->anim_set != nullptr) {
            const i32 key = jo::self<jo::Unit>(this)->anim_special;
            for (i32 i = jo::as_anim_set(list)->group_count; i > 0; --i) {
                u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(list)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
                if (jo::view<i32>(jo::as_anim_group_rec(r)->is_jump) == jo::self<jo::Unit>(this)->action && jo::as_anim_group_rec(r)->kind == jo::self<jo::Unit>(this)->anim_kind_alt &&
                    jo::as_anim_group_rec(r)->weapon_kind == jo::self<jo::Unit>(this)->anim_item_using && jo::as_anim_group_rec(r)->direction == key) {
                    idx = i;
                }
            }
        }
        if (jo::self<jo::Unit>(this)->anim_kind_alt != code) joan::orig::as_unit(this)->play_step_sounds(joan::orig::arg::i32(idx), 1u);  // 0x0050f570
        jo::self<jo::Unit>(this)->prev_action = jo::self<jo::Unit>(this)->anim_kind_alt;
        jo::self<jo::Unit>(this)->anim_kind_alt = code;
        if (jo::self<jo::Unit>(this)->anim_set != nullptr) {
            const i32 key = jo::self<jo::Unit>(this)->anim_special;
            for (i32 i = jo::as_anim_set(list)->group_count; i > 0; --i) {
                u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(list)->entry(joan::orig::arg::u32(i)));
                if (jo::view<i32>(jo::as_anim_group_rec(r)->is_jump) == jo::self<jo::Unit>(this)->action && jo::as_anim_group_rec(r)->kind == static_cast<i32>(code) &&
                    jo::as_anim_group_rec(r)->weapon_kind == jo::self<jo::Unit>(this)->anim_item_using && jo::as_anim_group_rec(r)->direction == key) {
                    idx = i;
                }
            }
        }
        if (idx != 0) joan::orig::as_unit(this)->play_anim_sounds(joan::orig::arg::i32(idx), 1);  // 0x0050db80
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: matched slot index [ebp-0x8] starts at 0 ("none") instead of stack garbage.
F("Unit::set_formation_slot_5124b0", 0x005124b0, &H::set_formation_slot);
#undef F

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x00512750
    std::uint16_t is_special_kind() {
        switch (static_cast<std::int32_t>(jo::self<jo::Unit>(this)->anim_kind_alt)) {
        case 1: case 2: case 3: case 4: case 5: case 6: case 8: case 67: case 89: case 90:
            return 1;
        default:
            return 0;
        }
    }

    // 0x0051c4b0
    void* set_byte_c4(std::uint32_t v) {
        jo::self<jo::Player>(this)->mounted = static_cast<std::uint8_t>(v);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Unit::is_special_kind", 0x00512750, &H::is_special_kind);
B("set_byte_c4", 0x0051c4b0, &H::set_byte_c4);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b106_16.cpp (game/b106_16)
namespace {
namespace game_b106_16 {

float K(img::Addr a) { return global<float>(a); }    // float constant in .rdata

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "by-value D3DVECTOR");
struct Mat {
    u32 m[16];
};
static_assert(sizeof(Mat) == 64, "D3DMATRIX");

JOAN_COUNTER(c_fire_512830, "Spawner::fire_512830");

struct H {
    // 0x00512830
    i32 fire_512830(i16 a8, i16 ac, i16 a10, u32 a14, u32 a18, u32 a1c) {
        c_fire_512830.hit();
        const Fpu fpu{true};
        (void)joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(a8));             // 0x00425ce0
        const i32 id = joan::orig::arg::as<i32>(joan::orig::as_units(obj(d::g_items))->create_49ea90(joan::orig::arg::u32(static_cast<i32>(a8))));  // 0x0049ea90
        void* const o18 = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        joan::orig::as_map_object(o18)->set_world(jo::self<jo::Unit>(this)->world);                                        // 0x004994b0
        joan::orig::as_map_object(o18)->set_position(jo::view<V3>(jo::self<jo::Unit>(this)->position_));                                           // 0x0041ff50
        void* const v60 = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id)));  // 0x0042d0a0
        V3 v{};
        bool found = false;
        Mat m;
        for (i32 i = 0; i < 10; ++i) {
            v = global<V3>(d::g_pos);
            switch (jo::view<i32>(jo::self<jo::UnitType>(v60)->shape_kind)) {
                case 1: v.x = -jo::self<jo::UnitType>(v60)->shape_sphere[3]; break;
                case 2: std::memcpy(&v.x, &jo::self<jo::UnitType>(v60)->shape_box[0], 4); break;
                case 3: v.x = -jo::self<jo::UnitType>(v60)->shape_cylinder[3]; break;
                default: break;
            }
            if (jo::view<void*>(jo::as_map_object(o18)->actor) != nullptr) {
                switch (jo::view<i32>(jo::as_map_object(o18)->actor->aim_mode)) {
                    case 1: v.x = v.x - jo::as_map_object(o18)->actor->aim_sphere[3]; break;
                    case 2: {
                        const float a = static_cast<float>(
                            joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(jo::as_map_object(o18)->actor->aim_box[0]))));  // 0x00427d60
                        const double b = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(jo::as_map_object(o18)->actor->aim_box[3])));
                        float c;
                        if (!(b >= static_cast<double>(a)))  // < or unordered
                            c = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(jo::as_map_object(o18)->actor->aim_box[0]))));
                        else
                            c = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(jo::as_map_object(o18)->actor->aim_box[3]))));
                        v.x = v.x - c;
                        break;
                    }
                    case 3: v.x = v.x - jo::as_map_object(o18)->actor->collide_radius; break;
                    default: break;
                }
            }
            v.x = v.x - K(d::flt_15);  // 15
            const i32 rnd = joan::orig::as_misc(obj(d::g_misc))->random(0x168);  // 0x00590540
            const float ang = static_cast<float>(fpu.mul(static_cast<double>(rnd), static_cast<double>(K(d::flt_0_017453292))));
            {
                const Mat* const r = joan::orig::arg::as<const Mat*>(joan::orig::matrix_rotation(&m, joan::orig::arg::f32(ang)));  // 0x0060bc50
                Mat t;
                std::memmove(&t, r, sizeof t);
                m = t;
            }
            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&v), static_cast<void*>(&m));  // point_transform
            if (jo::self<jo::Unit>(this)->actor != nullptr) {
                const Mat* const r = joan::orig::arg::as<const Mat*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone))->matrix());  // 0x0060ef80
                Mat t;
                std::memmove(&t, r, sizeof t);
                m = t;
            } else {
                joan::orig::matrix_rotation(&m, joan::orig::arg::f32(jo::self<jo::Unit>(this)->heading));  // 0x0060bc50
                import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(&m),
                                   static_cast<void*>(jo::self<jo::Unit>(this)->position_));  // multiply_translation
            }
            import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&v), static_cast<void*>(&m));  // point_transform
            void* v7c = nullptr;
            i32 v80 = 0, v84 = 0;
            if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                void* const p88 = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)));  // 0x006af310
                v7c = jo::as_shot(p88)->state.sub;
                v80 = jo::view<i32>(jo::as_shot(p88)->state.zone);
                v84 = ftol(static_cast<double>(jo::view<float>(jo::as_shot(p88)->state.cell)));
            }
            u32 vx, vy, vz;
            std::memcpy(&vx, &v.x, 4);
            std::memcpy(&vy, &v.y, 4);
            std::memcpy(&vz, &v.z, 4);
            const i32 ok = joan::orig::arg::as<i16>(joan::orig::as_players(obj(d::g_physic_sys))->find_slot(&v7c, &v80, joan::orig::arg::i16(0u), joan::orig::arg::f32(vx), joan::orig::arg::f32(vy), joan::orig::arg::f32(vz), 0, joan::orig::arg::f32(static_cast<float>(v84)), 0));  // 0x006ae8e0
            if (ok != 0) {
                const u8* const r = joan::orig::arg::as<const u8*>(joan::orig::placements::find(joan::orig::arg::mptr(obj(d::g_placements)), joan::orig::arg::i32(global<u32>(d::g_a57bb2)), static_cast<jo::PathSpace*>(v7c)));  // 0x004a2b20
                const float py = static_cast<float>(fpu.sub(static_cast<double>(v.y), static_cast<double>(jo::as_rworld_cell(r)->y)));
                const float px = static_cast<float>(fpu.sub(static_cast<double>(v.x), static_cast<double>(jo::as_rworld_cell(r)->x)));
                const double h = joan::orig::arg::as<double>(joan::orig::as_terrain(v7c)->height_66cae0(joan::orig::arg::i32(v80), joan::orig::arg::f32(px), joan::orig::arg::f32(py)));  // 0x0066cae0
                v.z = static_cast<float>(fpu.add(h, static_cast<double>(K(d::flt_0_2))));  // + 0.2
                found = true;
                break;
            }
        }
        if (!found) {
            joan::orig::as_units(obj(d::g_items))->remove_49f370(joan::orig::arg::i32(id));  // 0x0049f370
            return 0;
        }
        joan::orig::as_map_object(o18)->set_position(v);               // 0x0041ff50
        joan::orig::as_map_object(o18)->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)));    // 0x00499fd0
        jo::view<u16>(jo::as_map_object(o18)->side) = jo::view<u16>(jo::self<jo::Unit>(this)->side);
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::WorldItem)));  // operator new (0x52 on x86)
        void* const p = mem != nullptr ? joan::orig::as_world_item(mem)->construct() : nullptr;  // 0x00456eb0
        joan::orig::as_item_stack(p)->init(joan::orig::arg::i16(a8), joan::orig::arg::i16(ac), joan::orig::arg::i16(a10), joan::orig::arg::u32(a14), joan::orig::arg::u32(a18), joan::orig::arg::u32(a1c));  // 0x00456cd0
        jo::view<u16>(jo::as_world_item(p)->object_id) = static_cast<u16>(id);
        jo::as_world_item(p)->side = jo::view<u16>(jo::self<jo::Unit>(this)->side);
        joan::orig::as_world_item(p)->set_state(jo::self<jo::Unit>(this)->world);  // 0x00457120
        joan::orig::as_world_item(p)->set_pos(joan::orig::arg::bits(v.x), joan::orig::arg::bits(v.y), joan::orig::arg::bits(v.z));              // 0x00456fe0
        joan::orig::as_world_item(p)->set_heading(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)));   // 0x004571b0
        jo::view<void*>(jo::as_map_object(o18)->world_item) = p;
        if (jo::view<void*>(jo::as_map_object(o18)->actor) != nullptr) jo::as_map_object(o18)->actor->aim_mode = 0;
        return id;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_16", name, addr, fn)
B("Spawner::fire_512830", 0x00512830, &H::fire_512830);
#undef B

}  // namespace game_b106_16
}  // namespace

// ---------------------------------------- from modules/game_b111_18.cpp (fix/uninit)
namespace {
namespace game_b111_18 {


struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_los, "Unit::line_of_sight_5130a0");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------ 0x005130a0
    // One grid walk: returns 1 when a cell is above the ray; 0 when i reaches n or the cell
    // index leaves the grid (i / x / y / z then stay where the walk stopped).
    static i32 walk(u8* owner, u8* grid, i32& i, i32 n, float& x, float& y, float& z, float dx, float dy,
                    float dz) {
        const Fpu fpu{true};
        for (; i < n; ++i) {
            const i32 cx = ftol(fpu.div(fpu.sub(x, static_cast<double>(jo::as_cell(jo::as_unit(owner)->cell)->origin_x)), 64.0));
            const i32 cy = ftol(fpu.div(fpu.sub(y, static_cast<double>(jo::as_cell(jo::as_unit(owner)->cell)->origin_y)), 64.0));
            if (cx > static_cast<i32>(jo::as_cell(jo::as_unit(owner)->cell)->sector_rows)) return 0;
            if (cy > static_cast<i32>(jo::as_cell(jo::as_unit(owner)->cell)->sector_cols)) return 0;
            if (cx < 0) return 0;
            if (cy < 0) return 0;
            const i16 h = joan::orig::arg::as<i16>(joan::orig::as_height_grid(jo::as_path_space(grid)->grid)->at(joan::orig::arg::i32(static_cast<i16>(cx)), joan::orig::arg::i32(static_cast<i16>(cy))));  // 0x004c59d0
            if (static_cast<double>(h) > static_cast<double>(z)) return 1;  // test ah,0x41 / jne
            x = static_cast<float>(fpu.add(x, dx));
            y = static_cast<float>(fpu.add(y, dy));
            z = static_cast<float>(fpu.add(z, dz));
        }
        return 0;
    }

    i32 line_of_sight(i32 target_id) {
        c_los.hit();
        void* const units = obj(d::g_units);
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(target_id)) != 0) return 1;  // 0x00428b70
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(target_id)));  // 0x0041b3c0
        V3 tmp;
        const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_unit(t)->position_), joan::orig::arg::ptr(jo::self<jo::Unit>(this)->position_)));  // 0x0060cc90
        const float len = v3d_float(d::iat_v3d_87_length, &d);                       // length
        const Fpu fpu{true};
        const i32 n = ftol(fpu.div(len, 64.0));
        if (n == 0) return 0;
        const float dx = static_cast<float>(fpu.div(fpu.sub(jo::view<float[3]>(jo::as_unit(t)->position_)[0], jo::view<float[3]>(jo::as_unit(self())->position_)[0]), static_cast<double>(n)));
        const float dy = static_cast<float>(fpu.div(fpu.sub(jo::view<float[3]>(jo::as_unit(t)->position_)[1], jo::view<float[3]>(jo::as_unit(self())->position_)[1]), static_cast<double>(n)));
        const float dz = static_cast<float>(fpu.div(fpu.sub(jo::view<float[3]>(jo::as_unit(t)->position_)[2], jo::view<float[3]>(jo::as_unit(self())->position_)[2]), static_cast<double>(n)));
        float x = jo::view<float[3]>(jo::as_unit(self())->position_)[0];
        float y = jo::view<float[3]>(jo::as_unit(self())->position_)[1];
        float z = static_cast<float>(fpu.add(jo::view<float[3]>(jo::as_unit(self())->position_)[2], 50.0));  // + [0x9cfc18]
        // fix/uninit: the original's counter [ebp-0x24] is only zeroed in the first block; 0 here.
        i32 i = 0;
        u8* const g1 = jo::view<u8*>(jo::as_unit(self())->cell);
        if (g1 != nullptr && jo::view<i8>(jo::as_cell(g1)->loaded) != 0) {
            u8* const grid = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(g1)->grid(joan::orig::arg::i16(1)));  // 0x005600c0
            if (grid != nullptr) {
                i = 0;
                if (walk(self(), grid, i, n, x, y, z, dx, dy, dz) != 0) return 1;
            }
        }
        u8* const g2 = jo::view<u8*>(jo::as_unit(t)->cell);
        if (g2 != nullptr && jo::view<i8>(jo::as_cell(g2)->loaded) != 0) {
            u8* const grid = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(g2)->grid(joan::orig::arg::i16(1)));  // 0x005600c0
            if (grid != nullptr) {
                if (walk(t, grid, i, n, x, y, z, dx, dy, dz) != 0) return 1;
            }
        }
        return 0;
    }
};

// fix/uninit: step counter [ebp-0x24] read uninitialised by the second (target grid) loop when the
// first block is skipped; 0 used (start of the ray, where the position still is).
JOAN_REPLACE("fix/uninit", "Unit::line_of_sight", 0x005130a0, &H::line_of_sight);

}  // namespace game_b111_18
}  // namespace

// ---------------------------------------- from modules/game_b110_11.cpp (game/b110_11)
namespace {
namespace game_b110_11 {

void* const kObjs = obj(d::g_items);

// Plain dword read at an integer address (the original may read through a null pointer here).
u32 rd32(std::uintptr_t address) { return *reinterpret_cast<volatile u32*>(address); }
i16 rd16(std::uintptr_t address) { return *reinterpret_cast<volatile i16*>(address); }

struct Vec {
    float x, y, z;
};
static_assert(sizeof(Vec) == 12, "vec3");

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kForms = obj(d::g_physic_sys);
void* const kObjTypes = obj(d::g_item_res);

u8* unit_type(const void* u) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id)))); }  // 0x0042d0a0
i32 unit_missing(i32 k) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)); }  // 0x00428b70
u8* unit_rec(i32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k))); }  // 0x0041b3c0
u8* form_get(u32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_players(kForms)->list.ptr_at(joan::orig::arg::i32(k))); }  // 0x006af310
void unit_pos(const void* u, Vec* out) { joan::orig::as_unit(u)->position(joan::orig::arg::mptr(out)); }  // 0x004fedb0
void turn_to(const void* u, float deg) { joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(deg)); }   // 0x0050abd0

JOAN_COUNTER(c_launch, "Missile::launch_5133e0");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // ------------------------------------------------------------------ 0x005133e0 helpers
    // 0x00496d40 (cdecl, 13 dwords): aim the body at `target`.
    void aim(void* r6c, const Vec& p24, const Vec& target, u32 k) {
        joan::orig::aim_velocity(joan::orig::arg::mptr(jo::as_actor(jo::as_map_object(r6c)->actor)->velocity), p24, target, joan::orig::arg::f32(jo::self<jo::Missile>(this)->u32_3a), joan::orig::arg::f32(jo::self<jo::Missile>(this)->u32_3e), joan::orig::arg::u32(jo::self<jo::Missile>(this)->u32_42), joan::orig::arg::f32(jo::self<jo::Missile>(this)->u32_4e), joan::orig::arg::f32(jo::self<jo::Missile>(this)->u32_52), joan::orig::arg::f32(k));  // 0x00496d40
    }
    // Bow offset of the shooter model (+0x118 type 4 / 5).
    static void bow_offset(const u8* r18, Vec& p24) {
        auto mdl = [&]() { return jo::as_actor(jo::as_unit(r18)->actor)->bone; };
        Vec a{0.0f, 0.0f, 0.0f};
        jo::view<u32>(a.x) = jo::view<u32>(jo::as_bone(mdl())->local[0]);
        jo::view<u32>(a.y) = jo::view<u32>(jo::as_bone(mdl())->local[1]);
        jo::view<u32>(a.z) = jo::view<u32>(jo::as_bone(mdl())->local[2]);
        joan::orig::vec_scale_in_place(&a, joan::orig::arg::f32(u32{0x428963d7}));  // 0x0060ce80
        Vec b{0.0f, 0.0f, 0.0f};
        jo::view<u32>(b.x) = jo::view<u32>(jo::as_bone(mdl())->local[4]);
        jo::view<u32>(b.y) = jo::view<u32>(jo::as_bone(mdl())->local[5]);
        jo::view<u32>(b.z) = jo::view<u32>(jo::as_bone(mdl())->local[6]);
        joan::orig::vec_scale_in_place(&b, joan::orig::arg::f32(u32{0x41400000}));  // 0x0060ce80
        p24.x = p24.x + a.x;
        p24.y = p24.y + a.y;
        p24.x = p24.x + b.x;
        p24.y = p24.y + b.y;
        p24.z = p24.z + global<float>(d::flt_10_122);
    }
    static Vec vsub(const Vec* a, const Vec* b) {  // 0x0060cc90
        Vec t{0.0f, 0.0f, 0.0f};
        return *joan::orig::arg::as<Vec*>(joan::orig::vec_sub(&t, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
    }
    static Vec vadd(const Vec* a, const Vec* b) {  // 0x0060cc20
        Vec t{0.0f, 0.0f, 0.0f};
        return *joan::orig::arg::as<Vec*>(joan::orig::vec_add(&t, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
    }
    static Vec vscale(float s, const Vec* v) {  // 0x0060cd60
        Vec t{0.0f, 0.0f, 0.0f};
        return *joan::orig::arg::as<Vec*>(joan::orig::vec_scaled(&t, joan::orig::arg::f32(s), joan::orig::arg::ptr(v)));
    }
    // (i16) 0x00512130 added to a float with one x87 rounding.
    static float plus_jitter(const void* r18, float v) {
        const Fpu fpu{true};
        const i16 j = joan::orig::arg::as<i16>(joan::orig::as_unit(r18)->facing_sign());  // 0x00512130
        return static_cast<float>(fpu.add(static_cast<double>(j), static_cast<double>(v)));
    }
    // fild (i16) type+0x109 * 0.8f + v.
    static float plus_height(const u8* t, float v) {
        const Fpu fpu{true};
        const double h = fpu.mul(static_cast<double>(jo::self<jo::UnitType>(t)->height), static_cast<double>(global<float>(d::flt_0_8)));
        return static_cast<float>(fpu.add(h, static_cast<double>(v)));
    }
    // Copy of body +0x60 with z = 0 and the shooter-model axis (+0x58..+0x60) with z = 0, normalized.
    void axes(void* r6c, const u8* r18, Vec& body, Vec& axis) {
        body = jo::view<Vec>(jo::as_actor(jo::as_map_object(r6c)->actor)->velocity);
        jo::view<u32>(body.z) = 0;
        void* const m = jo::as_actor(jo::as_unit(r18)->actor)->bone;
        jo::view<u32>(axis.x) = jo::view<u32[16]>(jo::as_bone(m)->matrix_)[0];
        jo::view<u32>(axis.y) = jo::view<u32[16]>(jo::as_bone(m)->matrix_)[1];
        jo::view<u32>(axis.z) = jo::view<u32[16]>(jo::as_bone(m)->matrix_)[2];
        jo::view<u32>(axis.z) = 0;
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, &body);  // vector_normalize
        import_call<void*>(d::iat_v3d_87_vector_normalize_1, &axis);
    }
    // body +0x60 = dir * [+0x4a] (0x0060cd00).
    void straight(void* r6c, const Vec* dir) {
        Vec t{0.0f, 0.0f, 0.0f};
        const Vec v = *joan::orig::arg::as<Vec*>(joan::orig::vec_scale(&t, joan::orig::arg::ptr(dir), joan::orig::arg::f32(jo::self<jo::Missile>(this)->straight_speed)));  // 0x0060cd00
        jo::view<Vec>(jo::as_actor(jo::as_map_object(r6c)->actor)->velocity) = v;
    }
    void trail(void* r6c, const u8* r18) {
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Trail)));  // 0xaf on x86; operator new
        void* o = nullptr;
        if (mem != nullptr) o = joan::orig::as_trail(mem)->construct();  // 0x00687130
        jo::self<jo::Missile>(this)->trail = static_cast<jo::Trail*>(o);
        joan::orig::as_trail(jo::self<jo::Missile>(this)->trail)->init(joan::orig::arg::mptr(r6c));           // 0x006871f0
        joan::orig::as_trail(jo::self<jo::Missile>(this)->trail)->deform_update(joan::orig::arg::i32(jo::self<jo::Missile>(this)->lifetime_ms));  // 0x00687500
        if (global<u32>(d::g_player_unit_recno) == jo::self<jo::Missile>(this)->shooter_unit_id) joan::orig::as_unit_data(jo::as_unit(r18)->modifiers)->release_count(i32{1});  // 0x00497490
    }
    void form_order(u8* form, void* r6c, u32 k) {
        joan::orig::as_anim_table(kForms)->restart_6af370(joan::orig::arg::u32(jo::self<jo::Missile>(this)->model_id), joan::orig::arg::u32(jo::self<jo::Missile>(this)->u32_3a), joan::orig::arg::u32(static_cast<i32>(jo::view<i16[2]>(jo::as_shot(form)->type)[0])), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{1}));  // 0x006af370
        joan::orig::as_body(form)->init_target(joan::orig::arg::mptr(jo::as_actor(jo::as_map_object(r6c)->actor)->velocity), joan::orig::arg::u32(global<u32>(d::g_now)), joan::orig::arg::u32(k));  // 0x006b11a0
    }
    void store_origin(const Vec& p24) {
        jo::view<Vec>(jo::self<jo::Missile>(this)->origin) = p24;
        jo::view<u32>(jo::self<jo::Missile>(this)->origin[2]) = 0;
    }

    // ------------------------------------------------------------------ 0x005133e0
    void launch(u32 a8, Vec dir, Vec off) {
        c_launch.hit();
        const Fpu fpu{true};
        if (jo::self<jo::Missile>(this)->launch_time != 0) return;
        if (joan::orig::as_units(kObjs)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id)) != 0 || unit_missing(jo::view<i32>(jo::self<jo::Missile>(this)->shooter_unit_id)) != 0) {  // 0x00425d50
            joan::orig::as_missile(this)->expire();  // 0x004e6fb0
            return;
        }
        jo::self<jo::Missile>(this)->launch_time = global<u32>(d::g_now);
        std::uintptr_t s14 = 0;  // the MapObject (0 when a8 is none: the reads below then fault as the original's do)
        u8* const r18 = unit_rec(jo::view<i32>(jo::self<jo::Missile>(this)->shooter_unit_id));
        if (a8 != 0 && joan::orig::as_units(kObjs)->list.empty_at(joan::orig::arg::i32(a8)) == 0) s14 = reinterpret_cast<std::uintptr_t>(joan::orig::as_units(kObjs)->list.ptr_at(joan::orig::arg::i32(a8)));  // 0x00425d20
        Vec p24 = global<Vec>(d::g_pos);
        const float f28 = joan::orig::arg::as<float>(joan::orig::as_scene_object(jo::as_unit(r18)->actor)->yaw());  // 0x005ac7e0
        i32 s2c = static_cast<i32>(rd32(s14 + offsetof(jo::MapObject, sound_slots) + 8));
        if (s2c == 0) {
            const i16 w = rd16(s14 + offsetof(jo::MapObject, type_id));
            const u8* const ot = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kObjTypes)->get(joan::orig::arg::i16(static_cast<i32>(w))));  // 0x00425ce0
            s2c = jo::view<i16>(jo::as_item_def(ot)->bone);
        }
        {
            void* const mdl = jo::as_unit(r18)->actor;
            void* const res = jo::as_actor(mdl)->def->model->bones;
            const u32 s30 = joan::orig::arg::as<u32>(joan::orig::as_bone_table(res)->find(s2c));  // 0x005ba160
            void* const s34 = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::as_actor(jo::as_unit(r18)->actor)->plane)->bone_5cf8f0(joan::orig::arg::i32(s30)));  // 0x005cf8f0
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(s34)->matrix());  // 0x0060ef80
            import_call<void*>(d::iat_v3d_87_point_transform_2, &p24, m);           // point_transform
        }
        p24.x = p24.x + off.x;
        p24.y = p24.y + off.y;
        p24.z = p24.z + off.z;
        {
            const i16 kind = jo::self<jo::UnitType>(unit_type(r18))->kind;
            if (kind == 4 || kind == 5) bow_offset(r18, p24);
        }
        void* const r6c = joan::orig::as_units(kObjs)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Missile>(this)->object_id));  // 0x00425d20
        joan::orig::as_map_object(r6c)->set_world(jo::as_unit(r18)->world);  // 0x004994b0
        joan::orig::as_map_object(r6c)->set_position(p24);  // 0x0041ff50
        joan::orig::as_map_object(r6c)->set_yaw(joan::orig::arg::bits(f28));  // 0x00499fd0
        if (jo::view<i8>(jo::as_map_object(r6c)->active) == 0) {
            joan::orig::as_missile(this)->expire();  // 0x004e6fb0
            return;
        }
        joan::orig::as_bone(jo::as_actor(jo::as_map_object(r6c)->actor)->bone)->update();                          // 0x0060ee60
        joan::orig::as_scene(jo::as_actor(jo::as_unit(r18)->actor)->scene)->attach(joan::orig::arg::mptr(jo::as_map_object(r6c)->actor));       // 0x005efc50
        jo::as_actor(jo::as_map_object(r6c)->actor)->scene = jo::as_actor(jo::as_unit(r18)->actor)->scene;
        u8* const form = form_get(jo::self<jo::Missile>(this)->model_id);
        if (form == nullptr) {
            joan::orig::as_missile(this)->expire();  // 0x004e6fb0
            return;
        }
        const u32 kind = jo::self<jo::Missile>(this)->state;
        if (kind > 5) return;
        const double c884 = static_cast<double>(global<float>(d::flt_0_017453292));
        if (kind == 0) {
            // 0x00513839
            straight(r6c, &dir);
            Vec n{0.0f, 0.0f, 0.0f};
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, &n, jo::as_actor(jo::as_map_object(r6c)->actor)->velocity);  // vector_normalize(out, in)
            turn_to(r18, static_cast<float>(fpu.div(joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(n.y), joan::orig::arg::f32(n.x))), c884)));  // 0x00421020
            joan::orig::as_map_object(r6c)->set_yaw(joan::orig::arg::bits(joan::orig::arg::as<float>(joan::orig::libc::atan2_f(joan::orig::arg::f32(dir.y), joan::orig::arg::f32(dir.x)))));  // 0x00421020 -> 0x00499fd0
            joan::orig::as_unit(r6c)->set_66(joan::orig::arg::f32(joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(-dir.z)))));      // 0x00425610 -> 0x0049ab00
            form_order(form, r6c, 0x64);
            store_origin(p24);
            trail(r6c, r18);
            if (jo::self<jo::Missile>(this)->sound_handle == 0 && global<i8>(d::g_flag2) != 0) {
                const Vec at56 = jo::view<Vec>(jo::as_map_object(r6c)->pos);
                jo::self<jo::Missile>(this)->sound_handle = joan::orig::arg::as<u32>(joan::orig::as_sound_list(obj(d::g_sfx_player))->play_effect(str(d::s_ARROW), at56));  // 0x00494810 'ARROW'
            }
            return;
        }
        if (kind == 1 || kind == 2) {
            // 0x00513a4f
            if (unit_missing(static_cast<i32>(jo::as_unit(r18)->look_a)) == 0) {
                const u8* const tgt = unit_rec(static_cast<i32>(jo::as_unit(r18)->look_a));
                Vec p88{0.0f, 0.0f, 0.0f}, p98{0.0f, 0.0f, 0.0f};
                unit_pos(tgt, &p88);
                unit_pos(r18, &p98);
                const Vec d160 = vsub(&p88, &p98);
                const float f9c = v3d_float(d::iat_v3d_87_length, &d160);  // length
                if (f9c > global<float>(d::flt_300)) {
                    p88.x = plus_jitter(r18, p88.x);
                    p88.y = plus_jitter(r18, p88.y);
                }
                p88.z = plus_height(unit_type(tgt), p88.z);
                aim(r6c, p24, p88, 0x3e800000);
                Vec va8{0.0f, 0.0f, 0.0f}, vb4{0.0f, 0.0f, 0.0f};
                axes(r6c, r18, va8, vb4);
                if (f9c > global<float>(d::flt_300)) {
                    if (!(v3d_float(d::iat_v3d_87_dot_product, &vb4, &va8) >= global<float>(d::flt_0_9))) {  // dot_product
                        p98.z = plus_height(unit_type(tgt), p98.z);
                        const Vec d190 = vsub(&p88, &p98);
                        const float len = v3d_float(d::iat_v3d_87_length, &d190);  // length
                        const Vec d184 = vscale(len, &vb4);
                        Vec vc0 = vadd(&p98, &d184);
                        vc0.x = plus_jitter(r18, vc0.x);
                        vc0.y = plus_jitter(r18, vc0.y);
                        aim(r6c, p24, vc0, 0x3e800000);
                    }
                }
            } else if (joan::orig::as_units(kObjs)->list.empty_at(static_cast<i32>(jo::as_unit(r18)->look_b)) == 0) {  // 0x00425d50
                u8* const od0 = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjs)->list.ptr_at(static_cast<i32>(jo::as_unit(r18)->look_b)));  // 0x00425d20
                Vec vcc{0.0f, 0.0f, 0.0f};
                joan::orig::as_map_object(od0)->position(&vcc);  // 0x00499930
                Vec pdc{0.0f, 0.0f, 0.0f};
                unit_pos(r18, &pdc);
                const Vec d1c0 = vsub(&vcc, &pdc);
                const float fe0 = v3d_float(d::iat_v3d_87_length, &d1c0);  // length
                float h = 0.0f;
                if (jo::as_map_object(od0)->actor != nullptr) {
                    h = jo::as_model(jo::as_actor(jo::as_map_object(od0)->actor)->def->model)->sphere[3];
                } else {
                    const u8* const ot = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kObjTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(od0)->type_id))));  // 0x00425ce0
                    h = static_cast<float>(static_cast<i32>(jo::view<i16>(jo::as_item_def(ot)->stats[1])));
                }
                vcc.z = vcc.z + h;
                if (fe0 > global<float>(d::flt_300)) {
                    vcc.x = plus_jitter(r18, vcc.x);
                    vcc.y = plus_jitter(r18, vcc.y);
                }
                aim(r6c, p24, vcc, 0x3e800000);
                Vec vec{0.0f, 0.0f, 0.0f}, vf8{0.0f, 0.0f, 0.0f};
                axes(r6c, r18, vec, vf8);
                if (fe0 > global<float>(d::flt_300)) {
                    if (!(v3d_float(d::iat_v3d_87_dot_product, &vf8, &vec) >= global<float>(d::flt_0_9))) {  // dot_product
                        pdc.z = pdc.z + jo::as_model(jo::as_actor(jo::as_unit(r18)->actor)->def->model)->sphere[3];
                        const Vec d1f0 = vsub(&vcc, &pdc);
                        const float len = v3d_float(d::iat_v3d_87_length, &d1f0);  // length
                        const Vec d1e4 = vscale(len, &vf8);
                        Vec v104 = vadd(&pdc, &d1e4);
                        v104.x = plus_jitter(r18, v104.x);
                        v104.y = plus_jitter(r18, v104.y);
                        aim(r6c, p24, v104, 0x3e800000);
                    }
                }
            } else {
                dir.z = static_cast<float>(fpu.add(static_cast<double>(dir.z), global<double>(d::dbl_0_1)));
                straight(r6c, &dir);
            }
            // 0x00514364
            Vec n{0.0f, 0.0f, 0.0f};
            import_call<void*>(d::iat_v3d_87_vector_normalize_2, &n, jo::as_actor(jo::as_map_object(r6c)->actor)->velocity);  // vector_normalize(out, in)
            if (joan::crt::stricmp(joan::orig::arg::str(jo::self<jo::UnitType>(unit_type(r18))->name), str(d::s_AUTOBOW)) != 0 &&     // 'AUTOBOW'
                joan::crt::stricmp(joan::orig::arg::str(jo::self<jo::UnitType>(unit_type(r18))->name), str(d::s_AUTOBOW2)) != 0) {  // 'AUTOBOW2'
                turn_to(r18, static_cast<float>(fpu.div(joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(n.y), joan::orig::arg::f32(n.x))), c884)));  // 0x00421020
            }
            form_order(form, r6c, 1);
            store_origin(p24);
            joan::orig::as_map_object(r6c)->set_yaw(joan::orig::arg::bits(joan::orig::arg::as<float>(joan::orig::as_ground_object(r18)->heading())));  // 0x004ff850 -> 0x00499fd0
            joan::orig::as_unit(r6c)->set_66(joan::orig::arg::f32(joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(-n.z)))));  // 0x00425610 -> 0x0049ab00
            joan::orig::as_bone(jo::as_actor(jo::as_map_object(r6c)->actor)->bone)->update();            // 0x0060ee60
            trail(r6c, r18);
            return;
        }
        // kind 3..5: 0x00514542
        if (unit_missing(static_cast<i32>(jo::as_unit(r18)->look_a)) == 0) {
            const u8* const tgt = unit_rec(static_cast<i32>(jo::as_unit(r18)->look_a));
            Vec p{0.0f, 0.0f, 0.0f};
            unit_pos(tgt, &p);
            p.z = plus_height(unit_type(tgt), p.z);
            aim(r6c, p24, p, 0x3dcccccd);
        } else if (joan::orig::as_units(kObjs)->list.empty_at(static_cast<i32>(jo::as_unit(r18)->look_b)) == 0) {  // 0x00425d50
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjs)->list.ptr_at(static_cast<i32>(jo::as_unit(r18)->look_b)));  // 0x00425d20
            Vec v{0.0f, 0.0f, 0.0f};
            joan::orig::as_map_object(o)->position(&v);  // 0x00499930
            const u8* const ot = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kObjTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(o)->type_id))));  // 0x00425ce0
            const i32 sel = jo::view<i32>(jo::as_item_def(ot)->collision_type);
            const float c794 = global<float>(d::flt_2);
            if (sel == 2) {
                v.z = (jo::as_item_def(ot)->collision_params[9] - jo::as_item_def(ot)->collision_params[6]) / c794 + v.z;
            } else if (sel == 3) {
                v.z = jo::as_item_def(ot)->collision_params[14] / c794 + v.z;
            } else if (sel == 1) {
                v.z = v.z + jo::as_item_def(ot)->collision_params[3];
            } else {
                float h = 0.0f;
                if (jo::as_map_object(o)->actor != nullptr) {
                    h = jo::as_model(jo::as_actor(jo::as_map_object(o)->actor)->def->model)->sphere[3] / global<float>(d::flt_4);
                } else {
                    h = static_cast<float>(static_cast<i32>(jo::view<i16>(jo::as_item_def(ot)->stats[1])));
                }
                v.z = v.z + h;
            }
            aim(r6c, p24, v, 0x3dcccccd);
        } else {
            straight(r6c, &dir);
        }
        // 0x00514867
        joan::orig::as_map_object(r6c)->set_yaw(joan::orig::arg::bits(joan::orig::arg::as<float>(joan::orig::libc::atan2_f(joan::orig::arg::f32(dir.y), joan::orig::arg::f32(dir.x)))));  // 0x00421020 -> 0x00499fd0
        joan::orig::as_unit(r6c)->set_66(joan::orig::arg::f32(joan::orig::arg::as<float>(joan::orig::libc::asin_f(joan::orig::arg::f32(-dir.z)))));      // 0x00425610 -> 0x0049ab00
        form_order(form, r6c, 1);
        store_origin(p24);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_11", name, addr, fn)
B("Missile::launch_5133e0", 0x005133e0, &H::launch);
#undef B

}  // namespace game_b110_11
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

struct Obj {};

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_find_zone, "Unit::find_zone");

struct H {
    // ------------------------------------------------------------------------ 0x00514b80
    i32 find_zone() {
        c_find_zone.hit();
        V3 box;
        joan::orig::as_unit(this)->position(&box);  // 0x004fedb0
        void* const objects = obj(d::g_places);
        for (i32 i = joan::orig::as_dynarray(objects)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(objects)->empty_at_e(joan::orig::arg::i32(i))) != 0) continue;     // 0x0044cf80
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_list(objects)->get_e(joan::orig::arg::i32(i)));           // 0x0043c450
            if (static_cast<i32>(jo::as_facility(o)->kind) != 3) continue;
            using Virtual = u8* (Obj::*)();
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(o)[0x24 / 4], sizeof(void*));
            u8* const r = (reinterpret_cast<Obj*>(o)->*method)();
            if (jo::as_facility(r)->owner->nation != static_cast<i32>(jo::self<jo::Unit>(this)->side)) continue;
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_facility(r)->owner->box)->contains(&box)) != 0) return i;  // 0x0060d5f0
        }
        return 0;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Unit::find_zone", 0x00514b80, &H::find_zone);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b106_11.cpp (game/b106_11)
namespace {
namespace game_b106_11 {

void* const kFx = obj(d::g_sfx_player);
void* const kSpells = obj(d::g_item_res);

// 12-byte vector passed by value (raw bits).
struct V3 {
    float a, b, c;
};
static_assert(sizeof(V3) == 12, "V3");
V3 v3_at(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);
void* const kRandom = obj(d::g_misc);
void* const kModels = obj(d::g_physic_sys);
void* const kTriggers = obj(d::g_a5ccd0);

u32 now() { return global<u32>(d::g_now); }
i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_impact, "Missile::impact");
JOAN_COUNTER(c_unit_ini, "load_unit_constants");

// ------------------------------------------------------------------ 0x00514c40 helpers
// TriggerList::add 0x004eef00 (16 dwords, ret 0x40) with the impact pattern.
void impact_event(u32 kind, u32 sub, u32 w, u32 unit_id, V3 pos, u32 owner) {
    joan::orig::as_triggers(kTriggers)->add(joan::orig::arg::u32(kind), joan::orig::arg::u32(sub), joan::orig::arg::i16(w), 1u, joan::orig::arg::i16(unit_id), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), pos, joan::orig::arg::u32(owner), static_cast<std::int8_t>(joan::orig::arg::i32(0u)), joan::orig::arg::u32(now()), joan::orig::arg::u32(now()), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), 0u, joan::orig::arg::i16(0u));  // 0x004eef00
}
void fx(img::Addr name, V3 pos) { joan::orig::as_sound_list(kFx)->play_effect(str(name), pos); }  // 0x00494810

void explosive_fx(V3 pos) {
    switch (random(3)) {
        case 0: fx(d::s_EXPLOSIVEHIT1, pos); break;  // EXPLOSIVEHIT1
        case 1: fx(d::s_EXPLOSIVEHIT2, pos); break;  // EXPLOSIVEHIT2
        case 2: fx(d::s_EXPLOSIVEHIT3, pos); break;  // EXPLOSIVEHIT3
        default: break;
    }
}

// ------------------------------------------------------------------ 0x0051c680
enum : u8 { kInt, kShort, kFloat };
struct IniEntry {
    u8 kind;
    img::Addr global, section, key;
};
const IniEntry kUnitIni[] = {
    {kInt, d::g_unit_body_r, d::s_UNIT_9d7a44, d::s_BODY_R},  // UNIT BODY_R
    {kInt, d::g_a3ccdc, d::s_UNIT_9d7a44, d::s_MAX_W_C},  // UNIT MAX_W_C
    {kInt, d::g_unit_init_w_c, d::s_UNIT_9d7a44, d::s_INIT_W_C},  // UNIT INIT_W_C
    {kInt, d::g_a3ccd8, d::s_UNIT_9d7a44, d::s_WK_DIST},  // UNIT WK_DIST
    {kInt, d::g_a3cbe0, d::s_UNIT_9d7a44, d::s_HORSE_WD},  // UNIT HORSE_WD
    {kInt, d::g_a3cc74, d::s_UNIT_9d7a44, d::s_LD_FM_DT},  // UNIT LD_FM_DT
    {kInt, d::g_waypoint_reach_dist, d::s_UNIT_9d7a44, d::s_REACH_DT},  // UNIT REACH_DT
    {kInt, d::g_a3cc88, d::s_UNIT_9d7a44, d::s_FELL_DUR},  // UNIT FELL_DUR
    {kInt, d::g_a3cbe4, d::s_UNIT_9d7a44, d::s_INVC_DUR},  // UNIT INVC_DUR
    {kShort, d::g_unit_str_hp, d::s_UNIT_9d7a44, d::s_STR_HP},  // UNIT STR_HP
    {kShort, d::g_unit_def_hp, d::s_UNIT_9d7a44, d::s_DEF_HP},  // UNIT DEF_HP
    {kFloat, d::g_a3ccd0, d::s_UNIT_9d7a44, d::s_DEF_FALL},  // UNIT DEF_FALL
    {kFloat, d::g_a3cbf0, d::s_UNIT_9d7a44, d::s_DEF_HP_R},  // UNIT DEF_HP_R
    {kInt, d::g_a3cc04, d::s_UNIT_9d7a44, d::s_RECOV_HP},  // UNIT RECOV_HP
    {kInt, d::g_unit_egatk_mp, d::s_UNIT_9d7a44, d::s_EGATK_MP},  // UNIT EGATK_MP
    {kInt, d::g_a3cd1c, d::s_UNIT_9d7a44, d::s_EGATK_HP},  // UNIT EGATK_HP
    {kInt, d::g_a3cbd0, d::s_UNIT_9d7a44, d::s_HATK_MP},  // UNIT HATK_MP
    {kInt, d::g_a3cd20, d::s_UNIT_9d7a44, d::s_RECOV_MP},  // UNIT RECOV_MP
    {kInt, d::g_a3ccac, d::s_UNIT_9d7a44, d::s_RECOV_T},  // UNIT RECOV_T
    {kInt, d::g_a3cc40, d::s_UNIT_9d7a44, d::s_ARW_ATH},  // UNIT ARW_ATH
    {kInt, d::g_a3cc68, d::s_UNIT_9d7a44, d::s_ARW_TIME},  // UNIT ARW_TIME
    {kInt, d::g_a3cce8, d::s_UNIT_9d7a44, d::s_CLB_IDLE},  // UNIT CLB_IDLE
    {kInt, d::g_unit_max_com, d::s_UNIT_9d7a44, d::s_MAX_COM},  // UNIT MAX_COM
    {kInt, d::g_a3cbc8, d::s_UNIT_9d7a44, d::s_COM_LIM},  // UNIT COM_LIM
    {kInt, d::g_a3ccc4, d::s_UNIT_9d7a44, d::s_COM_TIME},  // UNIT COM_TIME
    {kInt, d::g_a3cbd8, d::s_UNIT_9d7a44, d::s_COM_CONS},  // UNIT COM_CONS
    {kInt, d::g_a3cc78, d::s_UNIT_9d7a44, d::s_CRI_DIFF},  // UNIT CRI_DIFF
    {kFloat, d::g_a3cbd4, d::s_UNIT_9d7a44, d::s_CRITEXP},  // UNIT CRITEXP
    {kInt, d::g_a3cc58, d::s_UNIT_9d7a44, d::s_LEVELARR},  // UNIT LEVELARR
    {kInt, d::g_a3cc14, d::s_UNIT_9d7a44, d::s_LEVSTEP},  // UNIT LEVSTEP
    {kFloat, d::g_a3cc70, d::s_UNIT_9d7a44, d::s_AI_MOVE},  // UNIT AI_MOVE
    {kInt, d::g_a3cd00, d::s_UNIT_9d7a44, d::s_MINDEXS},  // UNIT MINDEXS
    {kInt, d::g_a3cbdc, d::s_UNIT_9d7a44, d::s_MAXDEXS},  // UNIT MAXDEXS
    {kInt, d::g_unit_mor_nor, d::s_UNIT_9d7a44, d::s_MOR_NOR},  // UNIT MOR_NOR
    {kFloat, d::g_a3cc80, d::s_UNIT_9d7a44, d::s_MOR_CH_U},  // UNIT MOR_CH_U
    {kFloat, d::g_a3cd08, d::s_UNIT_9d7a44, d::s_MOR_CH_T},  // UNIT MOR_CH_T
    {kInt, d::g_a3cd14, d::s_UNIT_9d7a44, d::s_WP_DAMG},  // UNIT WP_DAMG
    {kInt, d::g_a3cc00, d::s_UNIT_9d7a44, d::s_AMR_DEF},  // UNIT AMR_DEF
    {kFloat, d::g_a3cd0c, d::s_UNIT_9d7a44, d::s_AMREXDEF},  // UNIT AMREXDEF
    {kFloat, d::g_a3cbec, d::s_UNIT_9d7a44, d::s_SIE_UDAM},  // UNIT SIE_UDAM
    {kInt, d::g_unit_f_attack, d::s_UNIT_9d7a44, d::s_F_ATTACK},  // UNIT F_ATTACK
    {kInt, d::g_unit_f_defend, d::s_UNIT_9d7a44, d::s_F_DEFEND},  // UNIT F_DEFEND
    {kFloat, d::g_a3cc94, d::s_UNIT_9d7a44, d::s_IDLE2_CH},  // UNIT IDLE2_CH
    {kFloat, d::g_a3cc30, d::s_UNIT_9d7a44, d::s_IDLE3_CH},  // UNIT IDLE3_CH
    {kFloat, d::g_a3ccf0, d::s_UNIT_9d7a44, d::s_IDLE_CC},  // UNIT IDLE_CC
    {kFloat, d::g_a3cce4, d::s_UNIT_9d7a44, d::s_STOP_CC},  // UNIT STOP_CC
    {kInt, d::g_a3cca4, d::s_UNIT_9d7a44, d::s_ARR_ACCU},  // UNIT ARR_ACCU
    {kInt, d::g_unit_blockper, d::s_UNIT_9d7a44, d::s_BLOCKPER},  // UNIT BLOCKPER
    {kInt, d::g_a3ccb0, d::s_UNIT_9d7a44, d::s_BLOCKANG},  // UNIT BLOCKANG
    {kInt, d::g_a3cc08, d::s_UNIT_9d7a44, d::s_ARR_FALL},  // UNIT ARR_FALL
    {kInt, d::g_a3cc84, d::s_UNIT_9d7a44, d::s_T_FALLF},  // UNIT T_FALLF
    {kInt, d::g_a3ccc0, d::s_UNIT_9d7a44, d::s_V_FALLF},  // UNIT V_FALLF
    {kInt, d::g_a3cc34, d::s_UNIT_9d7a44, d::s_F_FALLR},  // UNIT F_FALLR
    {kShort, d::g_a3ccd4, d::s_UNIT_9d7a44, d::s_LINF_DI},  // UNIT LINF_DI
    {kShort, d::g_a3cc1a, d::s_UNIT_9d7a44, d::s_LCAP_DI},  // UNIT LCAP_DI
    {kShort, d::g_a3cc5c, d::s_UNIT_9d7a44, d::s_HINF_DI},  // UNIT HINF_DI
    {kShort, d::g_a3cbe8, d::s_UNIT_9d7a44, d::s_HCAP_DI},  // UNIT HCAP_DI
    {kShort, d::g_a3cd18, d::s_UNIT_9d7a44, d::s_ARC_DI},  // UNIT ARC_DI
    {kShort, d::g_a3ccca, d::s_UNIT_9d7a44, d::s_ACAP_DI},  // UNIT ACAP_DI
    {kShort, d::g_a3cc7c, d::s_UNIT_9d7a44, d::s_CRO_DI},  // UNIT CRO_DI
    {kShort, d::g_a3cbcc, d::s_UNIT_9d7a44, d::s_CCAP_DI},  // UNIT CCAP_DI
    {kShort, d::g_a3ccb8, d::s_UNIT_9d7a44, d::s_POLE_DI},  // UNIT POLE_DI
    {kShort, d::g_a3cca0, d::s_UNIT_9d7a44, d::s_PCAP_DI},  // UNIT PCAP_DI
    {kShort, d::g_a3cca2, d::s_UNIT_9d7a44, d::s_MACE_DI},  // UNIT MACE_DI
    {kShort, d::g_a3cc38, d::s_UNIT_9d7a44, d::s_MCAP_DI},  // UNIT MCAP_DI
    {kShort, d::g_a3ccc8, d::s_UNIT_9d7a44, d::s_PEFF_R},  // UNIT PEFF_R
    {kShort, d::g_a3cc7e, d::s_UNIT_9d7a44, d::s_HP_PER},  // UNIT HP_PER
    {kShort, d::g_a3cc4c, d::s_UNIT_9d7a44, d::s_EP_PER},  // UNIT EP_PER
    {kShort, d::g_a3cd1a, d::s_UNIT_9d7a44, d::s_STR_PER},  // UNIT STR_PER
    {kShort, d::g_a3cc1e, d::s_UNIT_9d7a44, d::s_DEF_PER},  // UNIT DEF_PER
    {kShort, d::g_a3cc18, d::s_UNIT_9d7a44, d::s_DEX_PER},  // UNIT DEX_PER
    {kShort, d::g_a3cc10, d::s_UNIT_9d7a44, d::s_LDS_PER},  // UNIT LDS_PER
    {kShort, d::g_a3cce2, d::s_UNIT_9d7a44, d::s_STR_DAM},  // UNIT STR_DAM
    {kShort, d::g_a3cca8, d::s_UNIT_9d7a44, d::s_DEX_DAM},  // UNIT DEX_DAM
    {kShort, d::g_a3cce0, d::s_UNIT_9d7a44, d::s_DEF_RAT},  // UNIT DEF_RAT
    {kInt, d::g_critical_fade_in1, d::s_CRITICAL, d::s_FADE_IN1},  // CRITICAL FADE_IN1
    {kInt, d::g_a3ccf8, d::s_CRITICAL, d::s_FADE_IN2},  // CRITICAL FADE_IN2
    {kInt, d::g_a3cc98, d::s_CRITICAL, d::s_FADE_DUR},  // CRITICAL FADE_DUR
    {kInt, d::g_a3cd2c, d::s_CRITICAL, d::s_FADE_OUT},  // CRITICAL FADE_OUT
    {kInt, d::g_a3cbfc, d::s_CRITICAL, d::s_GLOW_IN1},  // CRITICAL GLOW_IN1
    {kInt, d::g_critical_glow_in2, d::s_CRITICAL, d::s_GLOW_IN2},  // CRITICAL GLOW_IN2
    {kInt, d::g_critical_glow_in3, d::s_CRITICAL, d::s_GLOW_IN3},  // CRITICAL GLOW_IN3
    {kFloat, d::g_a3cd24, d::s_DANGER, d::s_INDANGER},  // DANGER INDANGER
    {kInt, d::g_a3cc24, d::s_UNIT_9d7a44, d::s_UPG_TIME},  // UNIT UPG_TIME
    {kInt, d::g_a3cc44, d::s_MINIMAP, d::s_VISION_R},  // MINIMAP VISION_R
    {kFloat, d::g_a3ccec, d::s_EXP, d::s_LD_EXP},  // EXP LD_EXP
    {kFloat, d::g_a3ccbc, d::s_EXP, d::s_MEM_EXP},  // EXP MEM_EXP
    {kInt, d::g_a3cd10, d::s_UNIT_9d7a44, d::s_BRI_COMM},  // UNIT BRI_COMM
    {kInt, d::g_a3cc54, d::s_UNIT_9d7a44, d::s_BRI_CAPT},  // UNIT BRI_CAPT
    {kInt, d::g_a3cc0c, d::s_R_ATT, d::s_FALL_S},  // R_ATT FALL_S
    {kInt, d::g_a3cc6c, d::s_R_ATT, d::s_FALL_E},  // R_ATT FALL_E
    {kFloat, d::g_a3cc20, d::s_R_ATT, d::s_DIST_P},  // R_ATT DIST_P
    {kInt, d::g_a3cccc, d::s_C_BOUNS, d::s_C_BONUSL},  // C_BOUNS C_BONUSL
    {kInt, d::g_a3ccb4, d::s_C_BOUNS, d::s_E_BONUSL},  // C_BOUNS E_BONUSL
    {kInt, d::g_a3cc3c, d::s_C_BOUNS, d::s_M_BONUSL},  // C_BOUNS M_BONUSL
    {kInt, d::g_a3cc60, d::s_KILL, d::s_EP_NOR},  // KILL EP_NOR
    {kInt, d::g_a3cc48, d::s_KILL, d::s_EP_CAP},  // KILL EP_CAP
    {kInt, d::g_a3cc50, d::s_KILL, d::s_EP_GEN},  // KILL EP_GEN
};
static_assert(sizeof(kUnitIni) / sizeof(kUnitIni[0]) == 98, "98 entries");

void JOAN_CDECL load_unit_constants() {
    c_unit_ini.hit();
    void* const ini = obj(d::g_steer);
    for (const IniEntry& e : kUnitIni) {
        void* const sec = joan::orig::arg::as<void*>(joan::orig::as_ini(ini)->section(joan::orig::arg::str(str(e.section))));  // 0x0047cbf0
        char* const v = joan::orig::arg::as<char*>(joan::orig::as_ini(sec)->value(joan::orig::arg::str(str(e.key)))) + 9;     // 0x0047c180
        switch (e.kind) {
            case kInt: global<i32>(e.global) = joan::crt::atoi(joan::orig::arg::str(v)); break;  // atoi
            case kShort: global<i16>(e.global) = static_cast<i16>(joan::crt::atoi(joan::orig::arg::str(v))); break;
            default: global<float>(e.global) = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(v)))); break;  // atof
        }
    }
}

struct H {
    // ------------------------------------------------------------------ 0x00514c40
    bool missile_impact(i32 model_id) {
        c_impact.hit();
        if (model_id == 0) return true;
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::Missile>(this)->shooter_unit_id))) != 0) {  // 0x00428b70
            joan::orig::as_missile(this)->expire();                               // 0x004e6fb0
            return true;
        }
        if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::Missile>(this)->object_id))) != 0) {  // 0x00425d50
            joan::orig::as_missile(this)->expire();                                // 0x004e6fb0
            return true;
        }
        u8* const shooter = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::Missile>(this)->shooter_unit_id))));  // 0x0041b3c0
        u8* const model = joan::orig::arg::as<u8*>(joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(model_id)));  // 0x006af310
        u8* const info = jo::view<u8*>(jo::as_shot(model)->actor);
        if (jo::as_actor(info)->kind == 0xb || jo::as_actor(info)->kind == 0xc) {
            u8* const target = jo::view<u8*>(jo::as_actor(info)->owner);
            jo::view<i32>(jo::self<jo::Missile>(this)->expired) = static_cast<i32>(jo::as_unit(target)->id);
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::Missile>(this)->object_id))));  // 0x00425d20
            if (jo::as_map_object(o)->actor == nullptr) return true;
            float v[3];
            joan::orig::as_actor(jo::view<void*>(jo::as_map_object(o)->actor))->position(joan::orig::arg::mptr(v));  // 0x005ac270
            u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(target)->type_id))));  // 0x0042d0a0
            const Fpu fpu{true};
            const double top = fpu.add(static_cast<double>(static_cast<i32>(jo::self<jo::UnitType>(type)->height)),
                                       static_cast<double>(jo::view<float[3]>(jo::as_unit(target)->position_)[2]));
            if (!(top > static_cast<double>(v[2]))) return true;  // 0x005150fc: no 0x004e6fb0
            if (jo::as_unit(target)->rider_id != 0) jo::view<i32>(jo::self<jo::Missile>(this)->expired) = jo::as_unit(target)->rider_id;
            const u32 sid = static_cast<u32>(jo::view<u16>(jo::as_unit(shooter)->id));
            switch (jo::self<jo::Missile>(this)->state) {
                case 0:
                case 1:
                    impact_event(2, 1, static_cast<u32>(jo::view<u16[2]>(jo::self<jo::Missile>(this)->expired)[0]), sid, v3_at(jo::as_unit(target)->position_), jo::self<jo::Missile>(this)->u32_46);
                    break;
                case 2: {
                    V3 out;
                    const V3 p = *joan::orig::arg::as<V3*>(joan::orig::as_marker(this)->position(&out, joan::orig::arg::mptr(jo::as_shot(model)->actor)));  // 0x004e5d90
                    impact_event(0x2000, 1, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                    impact_event(0x2000, 2, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                    impact_event(0x2000, 2, static_cast<u32>(jo::view<u16[2]>(jo::self<jo::Missile>(this)->expired)[0]), sid, p, jo::self<jo::Missile>(this)->u32_46);
                    explosive_fx(p);
                    break;
                }
                case 3:
                case 4:
                case 5: {
                    V3 out;
                    const V3 p = *joan::orig::arg::as<V3*>(joan::orig::as_marker(this)->position(&out, joan::orig::arg::mptr(jo::as_shot(model)->actor)));  // 0x004e5d90
                    fx(d::s_CANNONHL, p);  // CANNONHL
                    impact_event(0x40, 1, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                    impact_event(0x40, 2, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                    break;
                }
                default:
                    break;
            }
            joan::orig::as_missile(this)->expire();  // 0x004e6fb0
            return true;
        }
        if (jo::as_actor(info)->kind != 1) return true;
        u8* const target = jo::view<u8*>(jo::as_actor(info)->owner);
        jo::view<i32>(jo::self<jo::Missile>(this)->expired) = static_cast<i32>(jo::as_map_object(target)->index);
        const u32 sid = static_cast<u32>(jo::view<u16>(jo::as_unit(shooter)->id));
        switch (jo::self<jo::Missile>(this)->state) {
            case 0:
            case 1:
                impact_event(2, 2, static_cast<u32>(jo::view<u16>(jo::as_map_object(target)->portrait_unit)), sid, v3_at(jo::as_map_object(target)->pos), jo::self<jo::Missile>(this)->u32_46);
                break;
            case 2: {
                V3 out;
                const V3 p = *joan::orig::arg::as<V3*>(joan::orig::as_marker(this)->position(&out, joan::orig::arg::mptr(jo::as_shot(model)->actor)));  // 0x004e5d90
                impact_event(0x2000, 2, static_cast<u32>(jo::view<u16>(jo::as_map_object(target)->portrait_unit)), sid, p, jo::self<jo::Missile>(this)->u32_46);
                impact_event(0x2000, 2, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                impact_event(0x2000, 1, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                explosive_fx(p);
                break;
            }
            case 3:
            case 4:
            case 5: {
                V3 out;
                const V3 p = *joan::orig::arg::as<V3*>(joan::orig::as_marker(this)->position(&out, joan::orig::arg::mptr(jo::as_shot(model)->actor)));  // 0x004e5d90
                u8* const spell = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kSpells)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(target)->type_id))));  // 0x00425ce0
                if (jo::view<i32>(jo::as_item_def(spell)->struction_type) == 1 || jo::view<i32>(jo::as_item_def(spell)->struction_type) == 2) {
                    fx(d::s_CANNONHH, p);  // CANNONHH
                } else {
                    fx(d::s_CANNONHM, p);  // CANNONHM
                }
                impact_event(0x40, 2, static_cast<u32>(jo::view<u16>(jo::as_map_object(target)->portrait_unit)), sid, p, jo::self<jo::Missile>(this)->u32_46);
                impact_event(0x40, 1, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                impact_event(0x40, 2, 0, sid, p, jo::self<jo::Missile>(this)->u32_46);
                break;
            }
            default:
                break;
        }
        joan::orig::as_missile(this)->expire();  // 0x004e6fb0
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_11", name, addr, fn)
B("Missile::impact", 0x00514c40, &H::missile_impact);
B("load_unit_constants", 0x0051c680, &load_unit_constants);
#undef B

}  // namespace game_b106_11
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
namespace {
namespace game_b101_07 {

struct Vec3 {
    float v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kKinds = obj(d::g_types);
void* const kTypes = obj(d::g_item_res);

using joan::orig::helpers::crt_strcmp;

u32 object_missing(i32 id) { return joan::orig::arg::as<u32>(joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id))); }  // 0x00425d50
u8* object_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id))); }  // 0x00425d20
u8* type_get(i16 t) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(t)))); }  // 0x00425ce0
u8* kind_get(i16 k) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<i32>(k)))); }  // 0x0042d0a0
void own_pos(void* self, Vec3* out) { joan::orig::as_unit(self)->position(joan::orig::arg::mptr(out)); }          // 0x004fedb0
void object_pos(void* o, Vec3* out) { joan::orig::as_map_object(o)->position(joan::orig::arg::mptr(out)); }             // 0x00499930

JOAN_COUNTER(c_hero_height, "Unit::hero_height_check");

struct H {
    bool kind_named(const char* name) {
        return crt_strcmp(reinterpret_cast<const char*>(kind_get(jo::self<jo::Unit>(this)->type_id) + 4), name) == 0;
    }

    // ------------------------------------------------------------------ 0x00518890
    void hero_height_check() {
        c_hero_height.hit();
        if (object_missing(jo::self<jo::Unit>(this)->held_item_id) != 0) return;
        u8* const o = object_get(jo::self<jo::Unit>(this)->held_item_id);
        float p[3];
        object_pos(o, reinterpret_cast<Vec3*>(p));
        u8* const t = type_get(jo::as_map_object(o)->type_id);
        const Fpu fpu{true};
        p[2] = static_cast<float>(fpu.add(static_cast<double>(jo::view<i16>(jo::as_item_def(t)->stats[1])), p[2]));  // fild + fadd
        float q[3];
        own_pos(this, reinterpret_cast<Vec3*>(q));
        std::memcpy(jo::self<jo::Unit>(this)->height_point, q, 12);
        float k = 5.0f;  // 0x40a00000
        if (kind_named(str(d::s_LAHIRE_9cfb3c))) {         // "LAHIRE"
            k = 4.5f;                              // 0x40900000
        } else if (kind_named(str(d::s_ALENCON_9cfb30))) {  // "ALENCON"
            k = 4.5f;
        } else if (kind_named(str(d::s_JEAN_DE_METZ))) {  // "JEAN_DE_METZ"
            k = 4.5f;
        }
        const double lim = fpu.add(fpu.mul(global<float>(d::flt_22), k), q[2]);  // 22.0f * k + z
        const i32 kind = jo::self<jo::Unit>(this)->anim_kind;
        if (!(lim >= static_cast<double>(p[2]))) {  // fcomp, test ah,1: less or unordered
            if (kind == 0x59) {
                joan::orig::as_unit(this)->set_action(0x61, 0, 1);  // 0x00501190
            } else if (kind == 0x5a) {
                joan::orig::as_unit(this)->set_action(0x63, 0, 1);
            }
        } else {
            if (kind == 0x59) {
                joan::orig::as_unit(this)->set_action(0x62, 0, 1);
            } else if (kind == 0x5a) {
                joan::orig::as_unit(this)->set_action(0x64, 0, 1);
            }
        }
    }
};

static void JOAN_CDECL init_a3cba0() { joan::orig::init_pi_515c90(); }  // 0x00515aa0
static void JOAN_CDECL init_a3cbc0() { joan::orig::div_a3cbc0(); }  // 0x0051c160

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("init_a3cba0", 0x00515aa0, &init_a3cba0);
B("Unit::hero_height_check", 0x00518890, &H::hero_height_check);
B("init_a3cbc0", 0x0051c160, &init_a3cbc0);
#undef B

}  // namespace game_b101_07
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

void JOAN_CDECL init_pi_515c90() {  // 0x00515c90
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));      // atan
    global<float>(d::g_a3cba0) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("init_pi_515c90", 0x00515c90, &init_pi_515c90);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

u32 JOAN_CDECL init_515e60() { joan::orig::init_5164a0(); return 0u; }  // eax dead: _initterm reloads it
u32 JOAN_CDECL init_516fc0() { joan::orig::init_5170b0(); return 0u; }  // eax dead: _initterm reloads it
u32 JOAN_CDECL init_51ac40() { joan::orig::init_pi_a3cbb4(); return 0u; }  // eax dead: _initterm reloads it
u32 JOAN_CDECL init_51b8f0() { joan::orig::init_51bab0(); return 0u; }  // eax dead: _initterm reloads it

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("init_515e60", 0x00515e60, &init_515e60);
B("init_516fc0", 0x00516fc0, &init_516fc0);
B("init_51ac40", 0x0051ac40, &init_51ac40);
B("init_51b8f0", 0x0051b8f0, &init_51b8f0);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b104_06.cpp (game/b104_06)
namespace {
namespace game_b104_06 {

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct Vec3 {
    float v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_find_near, "Unit::find_near_structure");
JOAN_COUNTER(c_set_stance, "Unit::set_stance");

void* kind_get(i32 w) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(sx16(w)))); }  // 0x00425ce0 (word)

struct H {
    // ------------------------------------------------------------------------ 0x005183d0
    i16 find_near_structure(i16* out) {
        c_find_near.hit();
        const float limit = 100.0f;  // 0x42c80000
        *out = -1;
        Vec3 me{};
        joan::orig::as_unit(this)->position(&me);  // 0x004fedb0
        for (i32 i = joan::orig::as_dynarray(obj(d::g_map_object_list))->size(); i > 0; --i) {  // 0x00417ad0
            void* const s = joan::orig::as_dynarray(obj(d::g_map_object_list))->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (s == nullptr) continue;
            void* const k = kind_get(jo::as_map_object(s)->type_id);
            if (static_cast<i32>(*static_cast<i16*>(k)) != 0xb) continue;
            Vec3 sp{};
            joan::orig::as_map_object(s)->position(&sp);  // 0x00499930
            {
                Vec3 tmp{};
                const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &sp, &me));  // 0x0060cc90
                const float len = v3d_float(d::iat_v3d_87_length, &d);
                if (!(len > limit)) {
                    *out = 1;
                    return static_cast<i16>(i);
                }
            }
            {
                float z;
                std::memcpy(&z, &sp.v[2], 4);
                z = static_cast<float>(static_cast<i32>(jo::view<i16>(jo::as_item_def(k)->stats[1]))) + z;
                std::memcpy(&sp.v[2], &z, 4);
            }
            {
                Vec3 tmp{};
                const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &sp, &me));  // 0x0060cc90
                const float len = v3d_float(d::iat_v3d_87_length, &d);
                if (!(len > limit)) {
                    *out = 0;
                    return static_cast<i16>(i);
                }
            }
        }
        return 0;
    }

    // ------------------------------------------------------------------------ 0x00519800
    void set_stance(u32 a, u32 b) {
        c_set_stance.hit();
        if (static_cast<i32>(jo::self<jo::Unit>(this)->stance) != 0 && static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 7) return;
        joan::orig::as_unit(this)->play_idle(0, 1);  // 0x00508b90
        if (jo::self<jo::Unit>(this)->anim_set != nullptr && static_cast<i32>(static_cast<i16>(b)) != 0) {
            if (static_cast<i32>(static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(this)->has_order_for(7u)))) != 0)  // 0x00500d40
                joan::orig::as_unit(this)->set_action(7, 0, 1);                                    // 0x00501190
        } else {
            if (static_cast<i32>(static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(this)->has_order_for(1u)))) != 0)  // 0x00500d40
                joan::orig::as_unit(this)->set_action(1, 0, 1);                                    // 0x00501190
        }
        jo::view<u8>(jo::self<jo::Unit>(this)->stance) = static_cast<u8>(a);
        jo::view<u8>(jo::self<jo::Unit>(this)->prisoner_pose) = static_cast<u8>(b);
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));  // 0x0050a480
    }
};

// ---- static initialisers (CRT table; eax ignored) ----
void JOAN_CDECL init_5164a0() { global<float>(d::g_a3cba4) = global<float>(d::g_a3cba0) / global<float>(d::flt_2); }
void JOAN_CDECL init_516ac0() { joan::orig::div_a3cba0(); }
void JOAN_CDECL init_5170b0() { global<float>(d::g_a3cbac) = global<float>(d::g_a3cba0) / global<float>(d::flt_4); }
void JOAN_CDECL init_51bab0() { global<float>(d::g_a3cbbc) = global<float>(d::g_pi_a3cbb4) / global<float>(d::flt_3); }

#define B(name, addr, fn) JOAN_REPLACE("game/b104_06", name, addr, fn)
B("init_5164a0", 0x005164a0, &init_5164a0);
B("init_516ac0", 0x00516ac0, &init_516ac0);
B("init_5170b0", 0x005170b0, &init_5170b0);
B("Unit::find_near_structure", 0x005183d0, &H::find_near_structure);
B("Unit::set_stance", 0x00519800, &H::set_stance);
B("init_51bab0", 0x0051bab0, &init_51bab0);
#undef B

}  // namespace game_b104_06
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL div_a3cba0() {  // 0x00516ed0
    global<float>(d::g_a3cba8) = global<float>(d::g_a3cba0) / global<float>(d::flt_3);
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Globals::div_a3cba0", 0x00516ed0, &div_a3cba0);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b106_20.cpp (game/b106_20)
namespace {
namespace game_b106_20 {

using joan::orig::helpers::as_float;
float kf(img::Addr a) { return global<float>(a); }

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_links_5174e0, "Unit::update_links_5174e0");

struct H {
    // ------------------------------------------------------------------ 0x005174e0
    std::uintptr_t update_links_5174e0() {
        c_links_5174e0.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
        auto vol = [this]() { return jo::view<u8*>(jo::self<jo::Unit>(this)->actor); };
        if ((jo::self<jo::Unit>(this)->anim_kind == 0x51 || jo::self<jo::Unit>(this)->anim_kind == 0x52) && vol() != nullptr && jo::view<u32>(jo::as_actor(vol())->record) != 0) {
            joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(vol())->record)), joan::orig::arg::i16(u32{0}));  // 0x006ad980
        }
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->mount_id))) == 0) {  // 0x00428b70
            if (vol() != nullptr && jo::view<u32>(jo::as_actor(vol())->record) != 0) {
                joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(vol())->record)), joan::orig::arg::i16(u32{0}));  // 0x006ad980
                jo::as_actor(vol())->aim_mode = 0;
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(vol())->record))));  // 0x006af310
                jo::as_shot(e)->hit = 0;
            }
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->mount_id))));  // 0x0041b3c0
            if (vol() != nullptr) {
                if (joan::orig::arg::as<u8>(joan::orig::as_unit(this)->dead()) != 0 || joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead()) != 0) {  // 0x0042d0e0
                    joan::orig::as_unit(this)->set_action(i32{0x4e}, i32{0}, i32{1});  // 0x00501190
                    u32 v[3];
                    const u8* const src = jo::view<u8*>(jo::as_actor(vol())->bone);
                    v[0] = jo::view<u32>(jo::as_bone(src)->local[0]);
                    v[1] = jo::view<u32>(jo::as_actor(vol())->bone->local[1]);
                    v[2] = jo::view<u32>(jo::as_actor(vol())->bone->local[2]);
                    joan::orig::vec_scale_in_place(static_cast<void*>(v), joan::orig::arg::f32(u32{0xc61c4000}));  // 0x0060ce80 (-10000.0f)
                    if (vol() != nullptr && jo::view<u32>(jo::as_actor(vol())->record) != 0) {
                        void* const fx = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(vol())->record)));  // 0x006af310
                        joan::orig::as_body(fx)->init_target(joan::orig::arg::mptr(static_cast<void*>(v)), joan::orig::arg::u32(global<u32>(d::g_now)), joan::orig::arg::u32(i32{0x64}));  // 0x006b11a0
                    }
                }
            }
        }
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->rider_id))) == 0) {  // 0x00428b70
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Unit>(this)->rider_id))));  // 0x0041b3c0
            if (jo::view<i8>(jo::as_unit(u)->mounted_flag) != 0) {
                const i32 w = jo::as_unit(u)->anim_kind;
                if (w != 0x4f && w != 0x50 && w != 0x51 && w != 0x52) {
                    float t = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());  // 0x004ff850 (fld [u+0x1a])
                    u32 tb;
                    std::memcpy(&tb, &t, 4);
                    joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(tb));  // 0x004ff870
                    // fld [this+0x1a]; fcomp t; test ah,0x40; jne: skip when equal or unordered
                    if (jo::self<jo::Unit>(this)->heading < as_float(tb) || jo::self<jo::Unit>(this)->heading > as_float(tb)) {
                        tb = jo::view<u32>(jo::self<jo::Unit>(this)->heading);
                        joan::orig::as_unit(u)->set_target(joan::orig::arg::f32(tb));  // 0x004ff870
                        joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(as_float(tb) / kf(d::flt_0_017453292)));  // 0x0050abd0
                    }
                    joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(as_float(tb) / kf(d::flt_0_017453292)));  // 0x0050abd0
                }
            }
        }
        if (vol() != nullptr && jo::self<jo::UnitType>(rec)->kind == 2 && jo::view<i8>(jo::self<jo::Unit>(this)->mount_pending) != 0 &&
            jo::self<jo::Unit>(this)->mount_pending_ms < global<u32>(d::g_now)) {
            jo::as_actor(vol())->aim_mode = jo::self<jo::UnitType>(rec)->shape_kind;
            const u8 ok = joan::orig::arg::as<u8>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->is_free(joan::orig::arg::mptr(static_cast<void*>(vol())), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->layer))));  // 0x00693170
            if (ok == 0) {
                u32 c30 = 0, c2c = 0;  // fully written by 0x0042a260 / 0x005e95b0
                void* const c = joan::orig::as_colour(&c30)->construct(u32{0xff}, u32{0xff}, u32{0xff}, u32{0xaa});  // 0x0042a260
                joan::orig::as_colour(&c2c)->from_rgba(joan::orig::arg::mptr(c));           // 0x005e95b0
                joan::orig::as_actor(vol())->set_faded(static_cast<char>(joan::orig::arg::i32(u32{1})));        // 0x005ae590
                joan::orig::as_plane(jo::view<void*>(jo::as_actor(vol())->plane))->set_fade(joan::orig::arg::u32(c2c));  // 0x005d5e80
                jo::as_actor(vol())->aim_mode = 0;
            } else {
                joan::orig::as_actor(vol())->set_faded(static_cast<char>(joan::orig::arg::i32(u32{0})));  // 0x005ae590
                jo::self<jo::Unit>(this)->mount_pending_ms = 0;
                jo::self<jo::Unit>(this)->mount_pending = 0;
            }
        }
        const i32 w = jo::self<jo::Unit>(this)->prev_action;
        if (w == 0x51 || w == 0x52) {
            if (jo::view<i8>(jo::self<jo::Unit>(this)->dismounted_flag) == 0) jo::self<jo::Unit>(this)->dismounted_flag = 1;
            return reinterpret_cast<std::uintptr_t>(this);
        }
        return static_cast<u32>(w);
    }
};

JOAN_REPLACE("game/b106_20", "Unit::update_links_5174e0", 0x005174e0, &H::update_links_5174e0);

}  // namespace game_b106_20
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

struct H {
    // ------------------------------------------------------------------------ 0x00518b90
    void expire_20f() {
        if (jo::self<jo::Unit>(this)->act_time != 0 && jo::self<jo::Unit>(this)->act_time + global<u32>(d::g_a3cce8) < global<u32>(d::g_now))
            jo::self<jo::Unit>(this)->act_time = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("Obj::expire_20f", 0x00518b90, &H::expire_20f);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

struct H {
    void nop_518d10(void*) {}  // 0x00518d10
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::nop_518d10", 0x00518d10, &H::nop_518d10);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b87_03.cpp (game/b87_03)
namespace {
namespace game_b87_03 {

constexpr Fpu kFpu{true};

struct Mat44 {
    float m[16];
};

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12 && sizeof(Mat44) == 64, "layout");

constexpr img::Addr kUnits(d::g_units);
constexpr img::Addr kTypes(d::g_types);
constexpr img::Addr kPlayers(d::g_physic_sys);

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }
bool unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* type_of(std::int16_t id) {
    return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(kTypes))->get(joan::orig::arg::i16(sx16(id))));  // 0x0042d0a0 (reads the word)
}
void set_pos(void* unit, Vec3 p) { joan::orig::as_ground_object(unit)->set_position(p); }  // 0x004fedf0
void get_pos(void* unit, Vec3* p) { joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(static_cast<void*>(p))); }  // 0x004fedb0
float get_yaw(void* unit) { return joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading()); }  // 0x004ff850
void set_heading_(void* unit, float v) { joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(v)); }  // 0x0050abd0
void rotation(Mat44* m, std::uint32_t a) {
    joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::u32(a), 0u, 0u);  // 0x0060bf40
}
Vec3 scaled(const Vec3* v, float s) {  // 0x0060cd00 into a temporary, copied through the result
    Vec3 tmp;
    return *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(static_cast<void*>(&tmp), static_cast<const void*>(v), joan::orig::arg::f32(s)));
}
void anim(std::int32_t player, std::int32_t id, std::int32_t code) {  // 0x006af370
    joan::orig::as_anim_table(img::obj(kPlayers))->restart_6af370(joan::orig::arg::u32(player), joan::orig::arg::u32(id), joan::orig::arg::u32(code), 0u, 2u, 1u);
}

JOAN_COUNTER(c_set_mount, "Unit::set_mount");
JOAN_COUNTER(c_set_rider, "Unit::set_rider");
JOAN_COUNTER(c_weapon, "Unit::attach_weapon_model");

struct H {
    // ------------------------------------------------------------------------ 0x0051a250
    std::uint8_t set_mount(std::int32_t id) {
        c_set_mount.hit();
        if (id == sx16(jo::self<jo::Unit>(this)->id)) return 0;
        if (id == jo::self<jo::Unit>(this)->mount_ref) return 1;
        std::uint8_t* const def = type_of(jo::self<jo::Unit>(this)->type_id);  // ebp-0x8
        if (id == 0 && jo::self<jo::Unit>(this)->mount_ref != 0) {
            Vec3 pos;  // ebp-0x14
            get_pos(this, &pos);
            Mat44 m;  // ebp-0x54
            rotation(&m, jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->heading));
            const Vec3 row1{m.m[4], m.m[5], m.m[6]};  // ebp-0x60
            const Vec3 off = scaled(&row1, 80.0f);   // ebp-0xfc
            Vec3 tmp;
            const Vec3 spot = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(static_cast<void*>(&tmp), static_cast<const void*>(&pos), static_cast<const void*>(&off)));  // 0x0060cc20, ebp-0xf0
            set_pos(this, spot);
            jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = jo::view<std::int32_t>(jo::self<jo::UnitType>(def)->shape_kind);
            const std::uint8_t ok = joan::orig::arg::as<std::uint8_t>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->is_free(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), joan::orig::arg::u32(sx16(jo::self<jo::Unit>(this)->layer))));  // 0x00693170
            if (ok == 0) return 0;
            jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = 0;
            set_pos(this, pos);
            set_heading_(this, get_yaw(this) / global<float>(d::flt_0_017453292));
            if (!unit_invalid(jo::self<jo::Unit>(this)->mount_ref)) {
                std::uint8_t* const mount = unit_get(jo::self<jo::Unit>(this)->mount_ref);  // ebp-0x68
                joan::orig::as_unit(mount)->set_rider(0);                         // 0x0051a8a0
                set_heading_(mount, get_yaw(mount) / global<float>(d::flt_0_017453292));
            }
            if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                std::uint8_t* const rec =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(img::obj(kPlayers))->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
                anim(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record), jo::view<std::int32_t>(jo::as_shot(rec)->u32_27a),
                     sx16(jo::view<std::int16_t[2]>(jo::as_shot(rec)->type)[0]));
            }
            jo::self<jo::Unit>(this)->mount_ref = 0;
            return 1;
        }

        if (unit_invalid(id)) return 2;
        std::uint8_t* const mount = unit_get(id);           // ebp-0x70
        type_of(jo::as_unit(mount)->type_id);             // ebp-0x74 (unused)
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(mount)->set_rider(joan::orig::arg::i32(sx16(jo::self<jo::Unit>(this)->id))))) ==
            0)  // 0x0051a8a0
            return 0;
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0 &&
            jo::self<jo::Unit>(this)->mount_ref == 0)
            joan::orig::as_players(img::obj(kPlayers))->set_active(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::i16(0));  // 0x006ad980
        Mat44 m;  // ebp-0xc0
        rotation(&m, jo::view<std::uint32_t>(jo::as_unit(mount)->heading));
        const Vec3 row0{m.m[0], m.m[1], m.m[2]};  // ebp-0xcc (row 1 copied to ebp-0xd8, unused)
        const float side = joan::orig::arg::as<float>(joan::orig::as_unit(mount)->heading_to_unit(joan::orig::arg::i32(sx16(jo::self<jo::Unit>(this)->id))));  // 0x00505960
        Vec3 p;  // ebp-0x80
        float yaw;
        if (!(side >= global<float>(d::k_9cf5c4))) {  // fcomp; test ah,1 (below or unordered)
            get_pos(mount, &p);
            const Vec3 off = scaled(&row0, 80.0f);  // ebp-0x120
            joan::orig::vec3_sub_assign(joan::orig::arg::mptr(static_cast<void*>(&p)), joan::orig::arg::mptr(static_cast<const void*>(&off)));  // 0x0060ce20
            set_pos(this, p);
            const double a = joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(row0.y), joan::orig::arg::f32(row0.x)));  // 0x00421020 atan2
            const float q = global<float>(d::g_pi_a3cbb4) / global<float>(d::flt_2);
            yaw = static_cast<float>(kFpu.sub(a, q));  // ebp-0xe0
        } else {
            get_pos(mount, &p);
            const Vec3 off = scaled(&row0, 80.0f);  // ebp-0x138
            joan::orig::vec_add_in_place(static_cast<void*>(&p), static_cast<const void*>(&off));  // 0x0060cdc0
            set_pos(this, p);
            const double a = joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(row0.y), joan::orig::arg::f32(row0.x)));
            const float q = global<float>(d::g_pi_a3cbb4) / global<float>(d::flt_2);
            yaw = static_cast<float>(kFpu.add(a, q));  // ebp-0xe4
        }
        joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(yaw));  // 0x004ff870
        set_heading_(this, yaw / global<float>(d::flt_0_017453292));
        jo::self<jo::Unit>(this)->mount_ref = id;
        return 1;
    }

    // ------------------------------------------------------------------------ 0x0051a8a0
    std::uint8_t set_rider(std::int32_t id) {
        c_set_rider.hit();
        if (id == 0 && jo::self<jo::Unit>(this)->rider_ref != 0) {
            jo::self<jo::Unit>(this)->rider_ref = 0;
            if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0) {
                std::uint8_t* const rec =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(img::obj(kPlayers))->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record))));  // 0x006af310
                joan::orig::as_unit(this)->play_idle(0, 1);  // 0x00508b90
                std::uint8_t* const def = type_of(jo::self<jo::Unit>(this)->type_id);
                jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->aim_mode) = jo::view<std::int32_t>(jo::self<jo::UnitType>(def)->shape_kind);
                anim(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record), jo::view<std::int32_t>(jo::as_shot(rec)->u32_27a), 6);
            }
            jo::self<jo::Unit>(this)->layer = 1;
            return 1;
        }
        if (unit_invalid(id)) return 0;
        if (jo::self<jo::Unit>(this)->rider_ref != 0) return 0;
        jo::self<jo::Unit>(this)->rider_ref = id;
        joan::orig::as_unit(this)->play_idle(0, 1);  // 0x00508b90
        jo::self<jo::Unit>(this)->layer = jo::as_unit(unit_get(id))->layer;
        jo::view<std::int32_t>(jo::self<jo::Unit>(this)->wait_count) = jo::view<std::int32_t>(jo::self<jo::Unit>(this)->wait_limit);
        return 1;
    }

    // ------------------------------------------------------------------------ 0x00519250
    void attach_weapon_model(std::int32_t flag) {
        c_weapon.hit();
        if (jo::self<jo::Unit>(this)->weapon_model == nullptr) return;
        if (jo::self<jo::UnitType>(type_of(jo::self<jo::Unit>(this)->type_id))->kind == 2 ||
            jo::self<jo::UnitType>(type_of(jo::self<jo::Unit>(this)->type_id))->kind == 7) {
            joan::orig::as_model_instance(jo::self<jo::Unit>(this)->weapon_model)->set_10a(0u);  // 0x0042cf20
            return;
        }
        if (jo::self<jo::Unit>(this)->actor == nullptr) {
            joan::orig::as_model_instance(jo::self<jo::Unit>(this)->weapon_model)->set_10a(0u);
            return;
        }
        if (static_cast<std::int8_t>(flag) != 0) {
            joan::orig::as_bone(jo::view<void*>(jo::as_scene_object(jo::self<jo::Unit>(this)->weapon_model)->bone))->set_parent(joan::orig::arg::mptr(jo::as_actor(jo::self<jo::Unit>(this)->actor)->bone));  // 0x0060ec70
            joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->weapon_model));  // 0x005efc50
            joan::orig::as_model_instance(jo::self<jo::Unit>(this)->weapon_model)->set_10a(1u);
            return;
        }
        if (jo::self<jo::Unit>(this)->weapon_model != nullptr) joan::orig::as_model_instance(jo::self<jo::Unit>(this)->weapon_model)->set_10a(0u);
    }
};

#define B87(name, addr, fn) JOAN_REPLACE("game/b87_03", name, addr, fn)
B87("Unit::set_mount", 0x0051a250, &H::set_mount);
B87("Unit::set_rider", 0x0051a8a0, &H::set_rider);
B87("Unit::attach_weapon_model", 0x00519250, &H::attach_weapon_model);
#undef B87

}  // namespace game_b87_03
}  // namespace

// ---------------------------------------- from modules/game_b106_19.cpp (game/b106_19)
namespace {
namespace game_b106_19 {

// Virtual thiscall: vtable slot at x86 byte offset `off` (slot off / 4, pointer-sized entries).
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>((*static_cast<const std::uintptr_t* const*>(o))[joan::call::x86_vslot(off)], o, args...);  // x86 byte offset -> slot
}

void* const kUnitTypes = obj(d::g_item_res);

JOAN_COUNTER(c_drop, "Unit::drop_carried");

struct H {
    // ------------------------------------------------------------------------ 0x005199b0
    void drop_carried() {
        c_drop.hit();
        if (static_cast<i32>(jo::self<jo::Unit>(this)->stance) == 0) return;
        for (i32 slot = 0; slot < 2; ++slot) {
            i16* const c = joan::orig::arg::as<i16*>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->slot(joan::orig::arg::i32(slot)));  // 0x004974f0
            if (c == nullptr) continue;
            const i32 o = joan::orig::arg::as<i32>(joan::orig::as_units(kObjects)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(*c))));  // 0x0049ea90
            void* const p1 = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(o));  // 0x00425d20
            vcall<i32>(p1, 0x18, 1);  // the `push 1` before Objects::get is this vcall's argument
            void* const p2 = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(o));
            vcall<i32>(p2, 0x1c);
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(*c))));  // 0x00425ce0
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(o))), joan::orig::arg::i32(// 0x0050a480
                          static_cast<i32>(jo::view<i16>(jo::as_item_def(r)->bone))), static_cast<std::int8_t>(joan::orig::arg::i32(slot)), joan::orig::arg::mptr(i32{0}));
        }
        jo::view<u8>(jo::self<jo::Unit>(this)->stance) = 0;
        jo::view<u8>(jo::self<jo::Unit>(this)->prisoner_pose) = 0;
        joan::orig::as_unit(this)->play_idle(i32{0}, i32{1});  // 0x00508b90
    }
};

#define B106(name, addr, fn) JOAN_REPLACE("game/b106_19", name, addr, fn)
B106("Unit::drop_carried", 0x005199b0, &H::drop_carried);
#undef B106

}  // namespace game_b106_19
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

void JOAN_CDECL init_pi_a3cbb4() {  // 0x0051ac60
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_pi_a3cbb4) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("init_pi_a3cbb4", 0x0051ac60, &init_pi_a3cbb4);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b107_12.cpp (game/b107_12)
namespace {
namespace game_b107_12 {

// Direct call (cdecl, no args), its eax.
template <fn::Code A>
u32 JOAN_CDECL init_one() {
    return cdecl_call<u32>(A);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b107_12", name, addr, fn)
B("StaticInit::init_51adb0", 0x0051adb0, (&init_one<fn::Globals_div_a3cbb4>));
#undef B

}  // namespace game_b107_12
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

void JOAN_CDECL div_a3cbb4() {  // 0x0051b7e0
    global<float>(d::g_a3cbb8) = global<float>(d::g_pi_a3cbb4) / global<float>(d::flt_2);
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Globals::div_a3cbb4", 0x0051b7e0, &div_a3cbb4);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL div_a3cbc0() {  // 0x0051c310
    global<float>(d::g_a3cbc0) = global<float>(d::g_pi_a3cbb4) / global<float>(d::flt_4);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("div_a3cbc0", 0x0051c310, &div_a3cbc0);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

struct Obj {};
using joan::call::thiscall;
using joan::orig::helpers::virtual_call;
using joan::img::obj;

JOAN_COUNTER(c_find_index, "ZoneRef::find_index");

struct Vec3 {
    float x, y, z;
};

struct H {
    // 0x005156d0
    i32 find_index() {
        c_find_index.hit();
        Vec3 pos;
        joan::orig::as_unit(this)->position(&pos);  // 0x004fedb0
        void* const zones = obj(d::g_places);
        for (i32 n = joan::orig::as_dynarray(zones)->size(); n >= 1; --n) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(zones)->empty_at_e(joan::orig::arg::i32(n))) != 0) continue;      // 0x0044cf80
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(zones)->get_e(joan::orig::arg::i32(n)));            // 0x0043c450
            if (static_cast<i32>(jo::as_facility(e)->kind) != 0x10) continue;
            u8* const o = virtual_call<u8*>(e, 0x1c);
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::as_facility(o)->owner->box)->contains(&pos)) == 0) continue;  // 0x0060d5f0
            if (jo::as_facility(o)->owner->nation == static_cast<i32>(jo::self<jo::ZoneRef>(this)->side)) return n;
        }
        return 0;
    }
};

JOAN_REPLACE(M, "ZoneRef::find_index", 0x005156d0, &H::find_index);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Unit::scaled_2df() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b90_04::H::scaled_2df>(this)); }  // 0x511f80
auto joan::orig::Game::scaled_a3cc90() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b90_04::H::scaled_a3cc90>(this)); }  // 0x512070
auto joan::orig::Unit::facing_sign() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b97_04::H::facing_sign>(this)); }  // 0x512130
auto joan::orig::Unit::select_speech(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::select_speech>(this, a0)); }  // 0x5121c0
auto joan::orig::Unit::set_formation_slot_5124b0(std::int16_t a0) -> void { joan::bind::member<&::game_b111_16::H::set_formation_slot>(this, a0); }  // 0x5124b0
auto joan::orig::Unit::is_special_kind() -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b90_02::H::is_special_kind>(this)); }  // 0x512750
auto joan::orig::Spawner::fire_512830(std::int16_t a0, std::int16_t a1, std::int16_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_16::H::fire_512830>(this, a0, a1, a2, a3, a4, a5)); }  // 0x512830
auto joan::orig::Unit::line_of_sight(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_18::H::line_of_sight>(this, a0)); }  // 0x5130a0
auto joan::orig::Unit::find_zone() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::find_zone>(this)); }  // 0x514b80
auto joan::orig::Marker::impact(std::int32_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b106_11::H::missile_impact>(this, a0)); }  // 0x514c40
auto joan::orig::Unit::find_zone_index() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_06::H::find_index>(this)); }  // 0x5156d0
auto joan::orig::init_pi_515c90() -> void { joan::bind::free<&::game_b99_05::init_pi_515c90>(); }  // 0x515c90
auto joan::orig::init_5164a0() -> void { joan::bind::free<&::game_b104_06::init_5164a0>(); }  // 0x5164a0
auto joan::orig::div_a3cba0() -> void { joan::bind::free<&::game_b94_04::div_a3cba0>(); }  // 0x516ed0
auto joan::orig::init_5170b0() -> void { joan::bind::free<&::game_b104_06::init_5170b0>(); }  // 0x5170b0
auto joan::orig::Unit::update_links_5174e0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_20::H::update_links_5174e0>(this)); }  // 0x5174e0
auto joan::orig::Unit::find_near_structure(void* a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b104_06::H::find_near_structure>(this, a0)); }  // 0x5183d0
auto joan::orig::Unit::hero_height_check() -> void { joan::bind::member<&::game_b101_07::H::hero_height_check>(this); }  // 0x518890
auto joan::orig::Unit::expire_20f() -> void { joan::bind::member<&::game_b96_04::H::expire_20f>(this); }  // 0x518b90
auto joan::orig::Unit::on_command_nop(void* a0) -> void { joan::bind::member<&::game_b89_06::H::nop_518d10>(this, a0); }  // 0x518d10
auto joan::orig::Unit::attach_weapon_model(std::int32_t a0) -> void { joan::bind::member<&::game_b87_03::H::attach_weapon_model>(this, a0); }  // 0x519250
auto joan::orig::Unit::set_stance(std::uint32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b104_06::H::set_stance>(this, a0, a1); }  // 0x519800
auto joan::orig::Unit::drop_carried() -> void { joan::bind::member<&::game_b106_19::H::drop_carried>(this); }  // 0x5199b0
auto joan::orig::Unit::set_mount(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b87_03::H::set_mount>(this, a0)); }  // 0x51a250
auto joan::orig::Unit::set_rider(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b87_03::H::set_rider>(this, a0)); }  // 0x51a8a0
auto joan::orig::init_pi_a3cbb4() -> void { joan::bind::free<&::game_b98_04::init_pi_a3cbb4>(); }  // 0x51ac60
auto joan::orig::init_51bab0() -> void { joan::bind::free<&::game_b104_06::init_51bab0>(); }  // 0x51bab0
auto joan::orig::div_a3cbc0() -> void { joan::bind::free<&::game_b100_08::div_a3cbc0>(); }  // 0x51c310
auto joan::orig::Player::set_byte_c4(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_02::H::set_byte_c4>(this, a0)); }  // 0x51c4b0
auto joan::orig::load_unit_constants() -> void { joan::bind::free<&::game_b106_11::load_unit_constants>(); }  // 0x51c680
