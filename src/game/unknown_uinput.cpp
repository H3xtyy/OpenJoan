// game/unknown_uinput.cpp: functions whose original source file is unknown (formerly game/_unplaced/uinput.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
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

namespace {
// A 12-byte vector argument by value as the callee declares it: float members (System V passes them in XMM registers).
struct F3 {
    float x, y, z;
};
template <class T>
F3 f3(const T& v) {
    static_assert(sizeof(T) == sizeof(F3), "12-byte vector");
    F3 f;
    std::memcpy(&f, &v, sizeof f);
    return f;
}
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

using i32 = std::int32_t;

using i16 = std::int16_t;

using joan::img::obj;

using joan::hook::global;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;

using joan::orig::helpers::units;

using joan::call::cdecl_call;
using joan::call::import_call;

void* const kObjects = obj(d::g_items);

using joan::img::str;

struct Obj {};

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;


using joan::hook::x87::Fpu;

using joan::orig::helpers::cf;
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // 0x00438ad0
    void reset_db() {
        jo::self<jo::Player>(this)->goto_active = 0;
        joan::orig::as_player(this)->reset_goto();  // 0x0043ac60
    }

    i32 not_kind_53(void* u) { return static_cast<i32>(jo::as_unit(u)->anim_kind) == 0x53 ? 0 : 1; }  // 0x0043bbc0

    i32 list_empty() { return list_count(this) <= 0 ? 1 : 0; }  // 0x0043c420
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::reset_db", 0x00438ad0, &H::reset_db);
B94("Obj::not_kind_53", 0x0043bbc0, &H::not_kind_53);
B94("List::empty", 0x0043c420, &H::list_empty);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

void* const kGameUnits = obj(d::g_units);  // unit manager

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_reset_43c500, "UnitCmd::reset_43c500");

struct H {
    // ---- unit command resets -------------------------------------------------------------
    void reset_43c500() {  // 0x0043c500
        c_reset_43c500.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return;
        std::uint8_t* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (jo::view<std::uint32_t>(jo::as_unit(unit)->mount_id) != 0) {
            if (unit_missing(jo::as_unit(unit)->mount_id) == 0) {
                joan::orig::as_slot(unit_get(jo::as_unit(unit)->mount_id))->select(0);  // 0x00482930
            }
            jo::view<std::uint32_t>(jo::as_unit(unit)->mount_id) = 0;
        }
        jo::self<jo::Player>(this)->mounted = 0;
    }

    std::uintptr_t stop_438b00() {  // 0x00438b00
        jo::view<std::uint16_t>(jo::self<jo::Player>(this)->goto_active) = 0;
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_player(this)->reset_goto());  // 0x0043ac60
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("UnitCmd::reset_43c500", 0x0043c500, &H::reset_43c500);
B85("Unit::stop_438b00", 0x00438b00, &H::stop_438b00);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using u32 = std::uint32_t;

i32 sx16(i32 v) { return static_cast<i16>(v); }

struct V3 {
    u32 a, b, c;
};
static_assert(sizeof(V3) == 12, "by-value vector");

JOAN_COUNTER(c_input_438b30, "Unit::input_438b30");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x00438b30
    i16 input_438b30() {
        c_input_438b30.hit();
        void* const units = obj(d::g_units);
        void* const hover = obj(d::g_mouse);
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return 0;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        V3 pos;
        joan::orig::as_unit(unit)->position(&pos);  // 0x004fedb0
        void* const& g = global<void*>(d::g_camera);
        const i32 click0 = joan::orig::arg::as<i32>(joan::orig::as_parser(hover)->rect_value(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_x1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_y1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_x2)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_y2)), i32{0}));  // 0x005945d0
        const i32 click1 = joan::orig::arg::as<i32>(joan::orig::as_parser(hover)->rect_value(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_x1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_y1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_x2)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(g)->view_y2)), i32{1}));  // 0x005945d0
        void* const keys = obj(d::g_keys_a48820);
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(keys)->test(i32{0}, i32{0x13})) != 0) {  // 0x004cb820
            joan::orig::as_order(me())->consume_43b490(joan::orig::arg::u32(i32{0}));  // 0x0043b490
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(keys)->test(i32{0}, i32{0x14})) != 0 && (global<u16>(d::g_key_modifiers) & 0x30) == 0) {
            joan::orig::as_unit(me())->use_spiritual_item_43b990(i32{0});  // 0x0043b990
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(keys)->test(i32{0}, i32{0x11})) != 0) {
            joan::orig::as_order(me())->fire_43bc50();  // 0x0043bc50
            return 1;
        }
        if (global<u8>(d::g_game_running) == 0) return 0;
        V3 v2c{0, 0, 0};
        if (click0 != 0) {
            const i32 x = joan::orig::arg::as<i32>(joan::orig::as_table(hover)->at_676(i32{0}));  // 0x0043c3f0
            const i32 y = joan::orig::arg::as<i32>(joan::orig::as_table(hover)->at_672(i32{0}));  // 0x0043c3c0
            void* const found = joan::orig::arg::as<void*>(joan::orig::as_player(me())->pick_object(joan::orig::arg::u32(y), joan::orig::arg::u32(x), &v2c));  // 0x00439110
            if (found == nullptr) {
                const V3 p = pos;
                const i32 x2 = joan::orig::arg::as<i32>(joan::orig::as_table(hover)->at_676(i32{0}));  // 0x0043c3f0
                const i32 y2 = joan::orig::arg::as<i32>(joan::orig::as_table(hover)->at_672(i32{0}));  // 0x0043c3c0
                if (joan::orig::as_player(me())->pick_ray(y2, x2, joan::orig::arg::f32(p.a), joan::orig::arg::f32(p.b), joan::orig::arg::f32(p.c), joan::orig::arg::mptr(&v2c)) != 0) {  // 0x0043a3b0
                    joan::orig::as_player(me())->reset_goto();                                // 0x0043ac60
                    return joan::orig::as_player(me())->move_to(f3(v2c));  // 0x0043a580
                }
                return 0;
            }
            const i32 kind = jo::as_actor(found)->kind;
            if (kind == 0xb) {
                void* const p = jo::as_actor(found)->owner;
                if (p == nullptr) return 0;
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(p)->is_allied_with(joan::orig::arg::i16(sx16(jo::self<jo::Player>(this)->unit_id_)))) != 0)  // 0x005095e0
                    return static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_player(me())->stub_zero_43a720(static_cast<i32>(jo::as_unit(p)->id))));  // 0x0043a720
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->hostile_to(joan::orig::arg::i16(sx16(jo::self<jo::Player>(this)->unit_id_)))) != 0)  // 0x005094d0
                    return joan::orig::as_ctrl(me())->set_target_43a7d0(static_cast<i32>(jo::as_unit(p)->id), i32{0},
                                                          click0);  // 0x0043a7d0
            } else if (kind == 1) {
                void* const q = jo::as_actor(found)->owner;
                if (q == nullptr) return 0;
                if (jo::as_map_object(q)->world_item != nullptr)
                    return static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_player(me())->goto_object_43abd0(static_cast<i32>(jo::as_map_object(q)->index))));  // 0x0043abd0
                if (jo::as_map_object(q)->side != 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->relation_to_side(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(q)->side)))) != 0)  // 0x00509590
                    return joan::orig::as_ctrl(me())->set_target_43a7d0(static_cast<i32>(jo::as_map_object(q)->index), i32{1},
                                                          click1);  // 0x0043a7d0
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->is_ally_of(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(q)->side)))) != 0)  // 0x00509670
                    return static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_player(me())->goto_object(joan::orig::arg::u32(static_cast<i32>(jo::as_map_object(q)->index)))));  // 0x0043a650
                if (joan::orig::arg::as<i16>(joan::orig::as_player(me())->try_kind_1e(joan::orig::arg::u32(static_cast<i32>(jo::as_map_object(q)->index)))) != 0) return 1;  // 0x0043a740
            }
        } else if (click1 != 0) {
            const i32 x = joan::orig::arg::as<i32>(joan::orig::as_table(hover)->at_676(i32{0}));  // 0x0043c3f0
            const i32 y = joan::orig::arg::as<i32>(joan::orig::as_table(hover)->at_672(i32{0}));  // 0x0043c3c0
            void* const found = joan::orig::arg::as<void*>(joan::orig::as_player(me())->pick_object(joan::orig::arg::u32(y), joan::orig::arg::u32(x), &v2c));  // 0x00439110
            if (found == nullptr) return 0;
            const i32 kind = jo::as_actor(found)->kind;
            if (kind == 0xb) {
                void* const p = jo::as_actor(found)->owner;
                if (p == nullptr) return 0;
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->hostile_to(joan::orig::arg::i16(sx16(jo::self<jo::Player>(this)->unit_id_)))) != 0)  // 0x005094d0
                    return joan::orig::as_player(me())->goto_target(static_cast<i32>(jo::as_unit(p)->id), i32{0});  // 0x0043a9e0
            } else if (kind == 1) {
                void* const q = jo::as_actor(found)->owner;
                if (q == nullptr) return 0;
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->relation_to_side(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(q)->side)))) != 0 && jo::as_map_object(q)->world_item == nullptr)  // 0x00509590
                    return joan::orig::as_player(me())->goto_target(static_cast<i32>(jo::as_map_object(q)->index), i32{1});  // 0x0043a9e0
            }
        }
        joan::orig::as_player(me())->order_update(i32{0}, i32{0}, i32{0});  // 0x004367c0
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Unit::input_438b30", 0x00438b30, &H::input_438b30);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
namespace {
namespace game_b86_05 {

void* const kStations = obj(d::g_nations);

void* const kGameUnits = obj(d::g_units);  // unit manager

std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_link_to, "Unit::link_to");

struct H {
    // ---- 0x00438f80 Unit::link_to(id) --------------------------------------------------------
    void link_to(std::int32_t id) {
        c_link_to.hit();
        if (id == 0) {
            joan::orig::as_member(this)->release();  // 0x0042fcb0
            return;
        }
        joan::orig::as_group(this)->release();  // 0x0042dd70
        std::uint8_t* const unit = unit_get(id);
        if (static_cast<std::int32_t>(jo::as_group(jo::as_unit(unit)->group)->count) >= 0x14) return;
        if (jo::as_unit(unit)->side != 0 && jo::self<jo::Unit>(this)->side != 0 && jo::self<jo::Unit>(this)->side != jo::as_unit(unit)->side) return;
        jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->leader_id) = static_cast<std::uint16_t>(id);
        jo::self<jo::Unit>(this)->team_flag = 0;
        jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->group_id) = jo::view<std::uint16_t>(jo::as_unit(unit)->group_id);
        joan::orig::as_group(jo::as_unit(unit)->group)->add_unit(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->id));  // 0x005287d0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(kStations)->is_null(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side))) == 0) {  // 0x00484870
            void* const st = joan::orig::as_dynarray(kStations)->ptr_at(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side));  // 0x0047b600
            joan::orig::as_nation(st)->update_unit(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004b5e40
        }
        joan::orig::as_unit(this)->set_level_523190(static_cast<std::int32_t>(jo::as_unit(unit)->level));  // 0x00523190
    }

    // ---- 0x0043ac60 reset: eax = this.
    H* reset_43ac60() {
        jo::view<std::uint32_t>(jo::self<jo::Player>(this)->goto_target_) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Player>(this)->goto_kind) = 0;
        jo::self<jo::Player>(this)->goto_time = 0;
        return this;
    }
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("Unit::link_to", 0x00438f80, &H::link_to);
B86("Obj43ac60::reset", 0x0043ac60, &H::reset_43ac60);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x00439090
    u32 update_439090() {
        const i32 s = jo::view<i32>(jo::self<jo::Player>(this)->goto_time);
        if (s > 0) {
            if (s <= 3)
                joan::orig::as_cmd(this)->follow_target_43acb0();  // 0x0043acb0
            else if (s == 4)
                joan::orig::as_cmd(this)->check_object_reach();  // 0x0043b2d0
        }
        return joan::orig::arg::as<u32>(joan::orig::as_player(this)->update_camera());  // 0x004384e0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Obj::update_439090", 0x00439090, &H::update_439090);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    void empty() {}
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("empty_0043c7b0", 0x0043c7b0, &H::empty);
B82("empty_004390f0", 0x004390f0, &H::empty);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b106_20.cpp (game/b106_20)
namespace {
namespace game_b106_20 {

using u32 = std::uint32_t;

using joan::orig::helpers::as_float;
float kf(img::Addr a) { return global<float>(a); }

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_pick_439110, "Picker::pick_439110");

// ================================================================== 0x00439110 picking
struct V3 {  // float members: passed by value to Shape::sphere_hit / box_hit / cylinder_hit, which take floats (System V x86-64: XMM registers, not integer ones)
    float v[3];
};
struct S24 {
    u32 v[6];
};
struct S20 {
    u32 v[5];
};
static_assert(sizeof(V3) == 12 && sizeof(S24) == 24 && sizeof(S20) == 20, "by-value args");
struct Ray {
    V3 a;  // [ebp-0x54]
    V3 b;  // [ebp-0x48]
};
static_assert(sizeof(Ray) == 24, "ray");

struct PickBest {
    u8* best = nullptr;      // [ebp-0x58]
    u32 t_bits = 0x40000000;  // [ebp-0x5c] 2.0f
    void offer(u32 t, u8* vol) {
        // cmp best,0; je take; fld best_t; fcomp t; test ah,0x41; jne skip -> take if best_t > t
        if (best != nullptr && !(as_float(t_bits) > as_float(t))) return;
        t_bits = t;
        best = vol;
    }
};

// One candidate (unit +0x2f5 / object +0x86). late_check: the object-ref loop tests the volume
// pointer after the transforms instead of before 0x0060ee60.
void pick_candidate(jo::Actor* const* field, bool late_check, Ray* ray, PickBest& pb) {
    auto vol = [field]() { return reinterpret_cast<u8*>(*field); };
    u8* const g = vol();
    if (g == nullptr) {
        // the original reads [g+0x90] here (faults for a null g)
#if defined(__GNUC__) && !defined(__clang__)
// TODO(fix): the original reads [0x90] when the volume is null and faults (real null read, kept as is)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
        if (*reinterpret_cast<volatile u32*>(static_cast<std::uintptr_t>(0x90)) == 0) return;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    }
    u8* const d = jo::view<u8*>(jo::as_actor(vol())->plane);
    if (d != nullptr && jo::as_plane(d)->vertex_format == 7) {
        u8 sphere[16];
        joan::orig::as_actor(vol())->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), joan::orig::arg::u32(u32{1}));  // 0x005acc80
        if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(joan::orig::arg::mptr(static_cast<void*>(ray)), joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) return;  // 0x0060e3f0
        u32 t = 0x40000000;  // 2.0f
        u8 hit = 0;
        if (!late_check && vol() == nullptr) return;
        joan::orig::as_bone(jo::view<void*>(jo::as_actor(vol())->bone))->update();  // 0x0060ee60
        u8* const m = jo::as_actor(vol())->bone->matrix_;
        u8 inv[64];
        import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), static_cast<void*>(m));  // matrix_inverse
        V3 p0, p1;
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&p0), static_cast<void*>(&ray->a),
                           static_cast<void*>(inv));  // point_transform(out, in, m)
        import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&p1), static_cast<void*>(&ray->b),
                           static_cast<void*>(inv));
        if (late_check && vol() == nullptr) return;
        const u32 kind = jo::as_actor(vol())->aim_mode;
        if (kind == 0) return;
        if (kind <= 3) {
            V3 tmp;
            const V3* const r = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &p1, &p0));  // 0x0060cc90 (p1 - p0)
            const V3 dir = *r;
            if (kind == 1) {
                hit = joan::orig::arg::as<u8>(joan::orig::shape::sphere_hit(&t, p0, dir, joan::orig::arg::mptr(static_cast<void*>(jo::as_actor(vol())->aim_sphere))));  // 0x006a0f90
            } else if (kind == 2) {
                S24 s;
                std::memcpy(&s, jo::as_actor(vol())->aim_box, sizeof s);
                hit = joan::orig::arg::as<u8>(joan::orig::shape::box_hit(&t, p0, dir, s));  // 0x00698970
            } else {
                S20 s;
                std::memcpy(&s, &jo::as_actor(vol())->collide_kind, sizeof s);
                hit = joan::orig::arg::as<u8>(joan::orig::shape::cylinder_hit(&t, p0, dir, s));  // 0x00699ec0
            }
        }
        if (hit == 0) return;
        pb.offer(t, vol());
        return;
    }
    if (d == nullptr) return;
    u8 sphere[16];
    joan::orig::as_actor(vol())->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), joan::orig::arg::u32(u32{1}));  // 0x005acc80
    if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(joan::orig::arg::mptr(static_cast<void*>(ray)), joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) return;  // 0x0060e3f0
    u32 t = 0;  // [ebp-0xf4]: written by 0x005d4a80 whenever it returns non-zero
    if (joan::orig::arg::as<i32>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(vol())->plane))->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(ray)))) == 0) return;  // 0x005d4a80
    pb.offer(t, vol());
}

struct H {
    // ------------------------------------------------------------------ 0x00439110
    void* pick_439110(u32 x, u32 y, void* out) {
        c_pick_439110.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return nullptr;  // 0x00428b70
        u8* const self = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x0041b3c0
        float pos[3];
        joan::orig::as_unit(self)->position(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x004fedb0
        float box[6];
        box[0] = pos[0] - kf(d::flt_5000);  // 5000
        box[1] = pos[1] - kf(d::flt_5000);
        box[2] = pos[2] - kf(d::flt_1000);  // 1000
        box[3] = pos[0] + kf(d::flt_5000);
        box[4] = pos[1] + kf(d::flt_5000);
        box[5] = pos[2] + kf(d::flt_1000);
        i32 count = 0;  // [ebp-0x30] (written by 0x00429090)
        u8** const cells = joan::orig::arg::as<u8**>(joan::orig::as_world(jo::as_unit(self)->world)->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x00429090
        i32 sx = 0, sy = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y), &sx, &sy)) == 0) return nullptr;  // 0x006152b0
        Ray ray;
        joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(sx), joan::orig::arg::i32(sy), joan::orig::arg::mptr(static_cast<void*>(&ray)));  // 0x006154d0
        PickBest pb;
        for (i32 i = count - 1; i >= 0; --i) {
            u8* const cell = cells[i];  // the cell pointer buffer (World +0x82)
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(static_cast<void*>(&ray), static_cast<void*>(jo::as_world_cell(cell)->box))) == 0) continue;  // 0x0060dfd0 (the cell's box, +0x8 on x86)
            for (u8* o = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cell)->get_90()); o != nullptr;
                 o = joan::orig::arg::as<u8*>(joan::orig::get_341(joan::orig::arg::mptr(o)))) {  // 0x0054eec0 / 0x0054ef40
                if (o == self) continue;
                pick_candidate(&jo::as_unit(o)->actor, false, &ray, pb);
            }
            for (u8* o = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cell)->get_98()); o != nullptr;
                 o = joan::orig::arg::as<u8*>(joan::orig::get_b2(joan::orig::arg::mptr(o)))) {  // 0x0054f510 / 0x0054f530
                pick_candidate(&jo::as_map_object(o)->actor, false, &ray, pb);
            }
            for (u8* e = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cell)->get_a0()); e != nullptr;
                 e = joan::orig::arg::as<u8*>(joan::orig::get_42(joan::orig::arg::mptr(e)))) {  // 0x0054f960 / 0x0054f980
                if (joan::orig::as_units(kObjects)->list.empty_at(static_cast<i32>(jo::as_world_item(e)->object_id)) != 0) continue;  // 0x00425d50
                u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(static_cast<i32>(jo::as_world_item(e)->object_id)));  // 0x00425d20
                pick_candidate(&jo::as_map_object(o)->actor, true, &ray, pb);
            }
        }
        if (pb.best == nullptr) return nullptr;
        joan::orig::segment_point_at(joan::orig::arg::mptr(out), static_cast<void*>(&ray), joan::orig::arg::f32(pb.t_bits));  // 0x0060de40
        return pb.best;
    }
};

JOAN_REPLACE("game/b106_20", "Picker::pick_439110", 0x00439110, &H::pick_439110);

}  // namespace game_b106_20
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

struct alignas(4) Six {  // 24-byte by-value argument
    std::uint32_t v[6];
};
struct alignas(4) Path {  // 0x005a1170 string, NUL at +0x190
    std::uint8_t b[0x194];
};
struct alignas(4) Archive {  // 0x0047cdf0
    std::uint8_t b[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger ResourceIdxL
};
struct alignas(4) TextReader {  // 0x00589780 (+0x196 dword)
    std::uint8_t b[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger FileTxt
};

void* memcpy_(void* d, const void* s, std::uint32_t n) { return joan::orig::arg::as<void*>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n))); }  // CRT 0x007b1db0 (as called)
char* strrchr_(const char* s, std::int32_t c) { return joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(s), joan::orig::arg::i32(c))); }

void path_init(Path& p) { joan::orig::as_string(&p)->construct(); }          // 0x005a1170
void path_set(Path& p, const void* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void path_add(Path& p, const void* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
const char* path_c(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

JOAN_COUNTER(c_map_res, "Map::load_res");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // ---- 0x00439ff0 Map::load_res(name, V6*, a, b) --------------------------------------------------
    void map_load_res(const char* name, const Six* v, std::uint32_t a, std::uint32_t b) {  // this: a Cell (the Map facade's +0 / +0x20 / +0x44 are Cell::world / name / origin)
        c_map_res.hit();
        if (name != nullptr) {
            memcpy_(jo::self<jo::Cell>(this)->name, name, 0x20);
            jo::self<jo::Cell>(this)->name[0x20] = 0;  // +0x40 on x86
        } else {
            jo::self<jo::Cell>(this)->name[0] = 0;
        }
        std::memcpy(jo::self<jo::Cell>(this)->box, v, sizeof(Six));  // +0x8 on x86
        jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->origin_x) = a;
        jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->origin_y) = b;
        char dir_slash[0x104];  // ebp-0x114
        char dir[0x104];        // ebp-0x218
        const char* const src = jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6;
        const char* const slash = strrchr_(src, 0x5c);
        if (slash != nullptr) {
            memcpy_(dir_slash, jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6, static_cast<std::uint32_t>(slash - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6)) + 1);
            memcpy_(dir, jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6, static_cast<std::uint32_t>(slash - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6)) + 1);
            dir_slash[(slash - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6)) + 1] = 0;
            dir[slash - (jo::view<char*>(jo::self<jo::Cell>(this)->world) + 6)] = 0;
        } else {
            dir_slash[0] = 0;
            dir[0] = 0;
        }
        Path res_path;
        path_init(res_path);
        path_set(res_path, str(d::s_MAP_9cf818));  // "MAP\"
        path_add(res_path, dir_slash);
        path_add(res_path, dir);
        path_add(res_path, str(d::s_res));  // ".res"
        Archive archive;
        joan::orig::as_archive(&archive)->construct(path_c(res_path), 0u, 0u);  // 0x0047cdf0
        Path path;
        path_init(path);

        path_set(path, jo::self<jo::Cell>(this)->name);
        path_add(path, str(d::s_STG_9d8188));  // "_STG"
        TextReader stg;
        open_reader(stg, archive, path);
        joan::orig::as_scene(this)->load_script(joan::orig::arg::mptr(static_cast<void*>(&stg)), 0);  // 0x0044e5f0

        path_set(path, jo::self<jo::Cell>(this)->name);
        path_add(path, str(d::s_UNI_9d8180));  // "_UNI"
        TextReader uni;
        open_reader(uni, archive, path);
        joan::orig::as_world(this)->load_units(joan::orig::arg::mptr(static_cast<void*>(&uni)));  // 0x00558890

        path_set(path, jo::self<jo::Cell>(this)->name);
        path_add(path, str(d::s_PLA));  // "_PLA"
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_res_idx_l(&archive)->get_index(joan::orig::arg::str(path_c(path)))) != 0) {  // 0x0059be20
            TextReader pla;
            open_reader(pla, archive, path);
            joan::orig::as_world(this)->load_places(joan::orig::arg::mptr(static_cast<void*>(&pla)));  // 0x00559ff0
            joan::orig::as_file_txt(&pla)->destruct();                            // 0x00589890
        }
        joan::orig::as_file_txt(&uni)->destruct();      // 0x00589890
        joan::orig::as_file_txt(&stg)->destruct();      // 0x00589890
        joan::orig::as_archive(&archive)->dtor();  // 0x00417a30
    }
    // reader(archive.data(archive.index(name)), archive.size(name)) in the original's call order.
    static void open_reader(TextReader& reader, Archive& archive, Path& path) {
        const std::uint32_t size = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx_l(&archive)->get_data_size_by_name(joan::orig::arg::str(path_c(path))));  // 0x0059bd60
        const std::uint32_t index = joan::orig::arg::as<std::uint32_t>(joan::orig::as_res_idx_l(&archive)->get_index(joan::orig::arg::str(path_c(path))));  // 0x0059be20
        const char* const data = joan::orig::arg::as<const char*>(joan::orig::as_res_idx_l(&archive)->read_rec(joan::orig::arg::i32(index)));  // 0x0059bf70
        joan::orig::as_file_txt(&reader)->construct_mem(joan::orig::arg::str(data), joan::orig::arg::i32(size));                                          // 0x00589780
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Map::load_res", 0x00439ff0, &H::map_load_res);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_pick_ray, "Obj::pick_ray");
JOAN_COUNTER(c_try_attack, "Pair::try_attack");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x0043a3b0
    i16 pick_ray(i32 sx, i32 sy, float px, float py, float pz, V3* out) {
        c_pick_ray.hit();
        float box[6];
        box[0] = px - global<float>(d::flt_5000);
        box[1] = py - global<float>(d::flt_5000);
        box[2] = pz - global<float>(d::flt_1000);
        box[3] = px + global<float>(d::flt_5000);
        box[4] = py + global<float>(d::flt_5000);
        box[5] = pz + global<float>(d::flt_1000);
        i32 count = 0;
        u8** const list = joan::orig::arg::as<u8**>(joan::orig::as_scene(jo::view<void*>(jo::as_world(global<u8*>(d::g_world))->scene))->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        i32 p28 = 0;
        i32 p2c = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(sx), joan::orig::arg::i32(sy), &p28, &p2c)) == 0) return 0;  // 0x006152b0
        V3 seg[2];
        joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(p28), joan::orig::arg::i32(p2c), joan::orig::arg::mptr(static_cast<void*>(seg)));  // 0x006154d0
        void* best = nullptr;
        float best_t = 0.0f;  // only read once best != 0
        for (i32 i = count - 1; i >= 0; --i) {
            u8* const e = list[i];
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(static_cast<void*>(seg), static_cast<void*>(jo::as_scene_s(e)->desc))) == 0) continue;  // 0x0060dfd0 (x86: e + 8)
            void* const mesh = jo::view<void*>(jo::as_scene_s(e)->plane);
            if (mesh == nullptr) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->visible()) == 0) continue;  // 0x005d0480
            float t;
            if (joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(seg)))) == 0) continue;  // 0x005d4a80
            if (best == nullptr || best_t > t) {
                best_t = t;
                best = mesh;
            }
        }
        if (best == nullptr) return 0;
        joan::orig::segment_point_at(static_cast<void*>(out), static_cast<void*>(seg), joan::orig::arg::f32(best_t));  // 0x0060de40
        return 1;
    }

    // ------------------------------------------------------------------ 0x0043c7d0
    i16 try_attack(i32 flag) {
        c_try_attack.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (flag == 0) return 0;
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->can_act()) == 0) return 0;  // 0x00500ba0
        if (static_cast<i32>(jo::as_unit(unit)->action) == 1) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0) return 0;  // 0x00430190
        const i32 target = joan::orig::arg::as<i16>(joan::orig::as_unit(unit)->nearest_enemy_of(2));  // 0x0044f0b0
        if (unit_missing(target) != 0) return 0;
        if (jo::view<u32>(jo::as_unit(unit_get(target))->rider_id) != 0) return 0;
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->mount(joan::orig::arg::i32(target)))) != 1) return 0;  // 0x004fc220
        jo::self<jo::Player>(this)->mounted = 1;
        jo::view<u32>(jo::self<jo::Player>(this)->jump_hold_ms) = 0;
        joan::orig::as_player(this)->reset_command();  // 0x0042a770
        joan::orig::as_player(this)->store_heading();  // 0x0043c480
        return 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::pick_ray", 0x0043a3b0, &H::pick_ray);
B96("Pair::try_attack", 0x0043c7d0, &H::try_attack);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0043a530
    u8 timer_expired() {
        if (jo::self<jo::Trigger>(this)->active != 0) return 0;
        if (jo::self<jo::Trigger>(this)->expire_time == 0) return 0;
        if (global<u32>(d::g_now) < jo::self<jo::Trigger>(this)->expire_time) return 0;
        return 1;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::timer_expired", 0x0043a530, &H::timer_expired);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);

struct Vec3 {
    float v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* unit(u32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_missing(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

JOAN_COUNTER(c_move_to, "Order::move_to_43a580");

struct H {
    // ------------------------------------------------------------------ 0x0043a580
    i16 move_to(Vec3 pos) {
        c_move_to.hit();
        if (unit_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) != 0) return 0;
        void* const u = unit(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        joan::orig::as_squad(u)->clear();  // 0x00422f10
        if (static_cast<i32>(jo::as_unit(u)->anim_kind) == 0x3d || static_cast<i32>(jo::as_unit(u)->anim_kind) == 0x3b) return 0;
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->move_to(pos, 0x64u, 0u, 0u)) != 0) return 1;  // 0x00505dd0
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Order::move_to_43a580", 0x0043a580, &H::move_to);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using u32 = std::uint32_t;

struct V3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers (Order::move_to_43a580 0x0043a580)
    float a, b, c;
};
static_assert(sizeof(V3) == 12, "vec3 by value");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x0043a650
    u32 try_set_43a650(u32 id) {
        u32 r = joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)));  // 0x00425d50
        if (r != 0) return r & 0xffff0000u;
        void* const o = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        V3 v{0, 0, 0};
        joan::orig::as_map_object(o)->position(&v);  // 0x00499930 (writes all 12 bytes)
        r = thiscall<u32>(fn::Order_move_to_43a580, this, v);  // 0x0043a580
        if (static_cast<i16>(r) == 0) return r & 0xffff0000u;
        jo::self<jo::Player>(this)->goto_time = 4;
        jo::view<u32>(jo::self<jo::Player>(this)->goto_kind) = 1;
        jo::view<u32>(jo::self<jo::Player>(this)->goto_target_) = id;
        return (id & 0xffff0000u) | 1u;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::try_set_43a650", 0x0043a650, &H::try_set_43a650);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

using u32 = std::uint32_t;

struct H {
    u32 zero16_43a720(i32) { return 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Obj::zero16_43a720", 0x0043a720, &H::zero16_43a720);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_dispatch, "Obj::dispatch_43c5c0");

struct H {
    // ------------------------------------------------------------------ 0x0043a740
    i16 try_1e(u32 a) {
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(a)) != 0) return 0;  // 0x00425d50
        void* const p = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(a));  // 0x00425d20
        if (static_cast<i32>(jo::as_map_object(p)->class_id) != 0x1e) return 0;
        joan::orig::as_player(this)->reset_goto();                     // 0x0043ac60
        joan::orig::as_ctrl(this)->set_target_43a7d0(joan::orig::arg::i32(a), i32{1}, i32{1});  // 0x0043a7d0
        return 1;
    }

    // ------------------------------------------------------------------ 0x0043c5c0
    i16 dispatch(u32 a8, u32 ac, u32 a10, u32 a14, u32 a18, u32 a1c, u32 a20) {
        c_dispatch.hit();
        const float f8 = joan::orig::arg::as<float>(joan::orig::as_unit(this)->link_value_of());  // 0x0042fd60
        void* const keys = obj(d::g_keys_a48820);
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(keys)->test(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->mode)), i32{0x13})) != 0) {  // 0x004cb820
            joan::orig::as_order(this)->consume_43b490(joan::orig::arg::u32(i32{0}));  // 0x0043b490
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(keys)->test(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->mode)), i32{0x14})) != 0 &&
            (static_cast<u32>(global<u16>(d::g_key_modifiers)) & 0x30u) == 0) {
            joan::orig::as_unit(this)->use_spiritual_item_43b990(i32{0});  // 0x0043b990
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(keys)->test(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->mode)), i32{0x11})) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->recharge_43bef0()) != 0 ||  // 0x0043bef0
                joan::orig::arg::as<i32>(joan::orig::as_order(this)->spawn_effect()) != 0 ||  // 0x0043bd40
                joan::orig::arg::as<i32>(joan::orig::as_unit(this)->try_link_43be40()) != 0 ||  // 0x0043be40
                joan::orig::arg::as<i32>(joan::orig::as_player(this)->link_nearest_enemy()) != 0 ||  // 0x0043c210
                joan::orig::arg::as<i32>(joan::orig::as_order(this)->fire_43bc50()) != 0 ||  // 0x0043bc50
                joan::orig::arg::as<i32>(joan::orig::as_unit(this)->start_6c0310_43bfe0()) != 0 ||  // 0x0043bfe0
                joan::orig::arg::as<i32>(joan::orig::as_order(this)->alencon_43c080()) != 0) {  // 0x0043c080
                return 1;
            }
        }
        if (!(jo::self<jo::Player>(this)->mode == 3 && global<i8>(d::g_a5802c) != 0)) {
            joan::orig::as_player(this)->set_follow(joan::orig::arg::i32(jo::self<jo::Player>(this)->mode != 3 ? a14 : 0u));  // 0x00436190
        }
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->try_dismount(joan::orig::arg::i32(a8))) != 0) return 1;  // 0x0043c920
        joan::orig::as_unit(this)->promotion_update_43ca90(joan::orig::arg::i32(ac), joan::orig::arg::i32(a10));                // 0x0043ca90
        const u32 m = jo::self<jo::Player>(this)->mode != 3 ? a14 : 0u;
        joan::orig::as_player(this)->steer_update(joan::orig::arg::u32(a18), joan::orig::arg::i32(a20), joan::orig::arg::f32(f8), joan::orig::arg::i32(m));  // 0x0043dab0
        joan::orig::as_order(this)->apply_43d7a0(joan::orig::arg::i32(a1c), joan::orig::arg::i32(a14));         // 0x0043d7a0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Obj::try_1e_43a740", 0x0043a740, &H::try_1e);
B("Obj::dispatch_43c5c0", 0x0043c5c0, &H::dispatch);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b106_15.cpp (game/b106_15)
namespace {
namespace game_b106_15 {

using u32 = std::uint32_t;

struct H {
    // ---- 0x0043a980 -------------------------------------------------------------------
    void dtor_43a980() {
        jo::self<jo::Order45>(this)->vtable = obj(d::vt_Obj_9d3bec);
        joan::orig::as_map_object(this)->release_base();  // 0x00492370
        joan::orig::as_map_object(this)->destruct();  // 0x00415790
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_15", name, addr, fn)
B("Obj43a980::dtor", 0x0043a980, &H::dtor_43a980);
#undef B

}  // namespace game_b106_15
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using u32 = std::uint32_t;

struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_goto, "Obj::goto_target_43a9e0");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0043a9e0
    i16 goto_target(i32 id, i32 mode) {
        c_goto.hit();
        void* const units = obj(d::g_units);
        if (id == 0) return 0;
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return 0;  // 0x00428b70
        (void)joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0 (dead local)
        Vec3 pos{0, 0, 0};
        if (mode == 0) {
            if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id));
            if (joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead()) != 0) return 0;  // 0x0042d0e0
            void* u2 = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id));
            joan::orig::as_unit(u2)->position(&pos);  // 0x004fedb0
        } else if (mode == 1) {
            if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
            void* b = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
            joan::orig::as_map_object(b)->position(&pos);                                 // 0x00499930
        }
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->move_to(f3(pos))) == 0) return 0;  // 0x0043a580
        jo::self<jo::Player>(this)->goto_target_ = id;
        jo::self<jo::Player>(this)->goto_kind = mode;
        jo::view<i32>(jo::self<jo::Player>(this)->goto_time) = 3;
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::goto_target_43a9e0", 0x0043a9e0, &H::goto_target);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

void* const kUnits = obj(d::g_units);

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_dismount, "Mount::try_dismount");

struct H {
    // 0x0043ab30
    void release_43ab30() {
        if (jo::view<void*>(jo::self<jo::TrackNode>(this)->links) != nullptr) {
            joan::crt::operator_delete(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::TrackNode>(this)->links)));  // operator delete
            jo::view<void*>(jo::self<jo::TrackNode>(this)->links) = nullptr;
        }
        // 0x004ee530 on the search state at +0x1c: its +0x8 buffer is the child table (+0x24)
        if (jo::self<jo::TrackNode>(this)->children != nullptr) {
            joan::orig::operator_(jo::self<jo::TrackNode>(this)->children);  // operator delete
            jo::self<jo::TrackNode>(this)->children = nullptr;
        }
    }

    // 0x0043c920
    std::int16_t try_dismount(std::int32_t flag) {
        c_dismount.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        std::uint8_t* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (unit_missing(jo::as_unit(u)->mount_id) != 0) return 0;
        std::uint8_t* const t = unit_get(jo::as_unit(u)->mount_id);
        if (flag == 0) return 0;
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(u)->can_act()) == 0) return 0;  // 0x00500ba0
        if (jo::as_unit(u)->anim_kind != 0x43) return 0;
        const std::int16_t tk = jo::as_unit(t)->anim_kind;
        if (tk != 1 && tk != 2 && tk != 3) return 0;
        if (jo::as_unit(u)->action == 1) return 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(u)->mounted()) == 0) return 0;  // 0x00430190
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(u)->mount(0)) != 1) return 0;  // 0x004fc220
        jo::self<jo::Player>(this)->mounted = 0;
        jo::self<jo::Player>(this)->jump_hold_ms = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Obj::release_43ab30", 0x0043ab30, &H::release_43ab30);
B("Mount::try_dismount", 0x0043c920, &H::try_dismount);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

void* const kUnits = obj(d::g_units);

i32 word_arg(u32 raw) { return static_cast<i32>(static_cast<i16>(raw)); }

struct H {
    // ------------------------------------------------------------------ 0x0043ab80
    u32 unit_50c680_43ab80(u32 w) {
        void* u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        const u32 r = thiscall<u32>(fn::Unit_set_leader, u, word_arg(w));       // 0x0050c680
        return r & 0xffff0000u;                                        // xor ax, ax
    }
    // ------------------------------------------------------------------ 0x0043c1c0
    u32 dtor_43c1c0() {
        if (jo::self<jo::Order0c_b>(this)->u32_c2 == 0) joan::orig::as_unit(this)->play_effect(static_cast<char>(joan::orig::arg::i32(1)));  // 0x00477360
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_4251b0());                     // 0x004251b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::unit_50c680_43ab80", 0x0043ab80, &H::unit_50c680_43ab80);
// 0x0043bbf0 (loads the MILIBAL texture into a Texture* kept in 4 bytes at +0x5 of an object of unknown class) is not
// replaced: neither it nor its thunk 0x004036b6 is referenced by joan.exe or by our code, and a 4-byte pointer slot
// cannot be right on x64. A call would trap as "original code needed".
B("Unit::dtor_43c1c0", 0x0043c1c0, &H::dtor_43c1c0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using u32 = std::uint32_t;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_goto_object, "Mover::goto_object");

struct H {
    // ------------------------------------------------------------------ 0x0043abd0
    u32 goto_object(i32 id) {
        c_goto_object.hit();
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50 (1 -> xor ax,ax)
        void* const o = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        Vec3 pos{0.0f, 0.0f, 0.0f};
        joan::orig::as_map_object(o)->position(&pos);  // 0x00499930 (writes all 12 bytes)
        joan::orig::as_player(this)->reset_goto();       // 0x0043ac60
        return thiscall<u32>(fn::Order_move_to_43a580, this, pos);  // 0x0043a580
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Mover::goto_object", 0x0043abd0, &H::goto_object);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b111_09.cpp (game/b111_09)
namespace {
namespace game_b111_09 {

using joan::hook::x87::Fpu;

using u32 = std::uint32_t;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_follow, "Cmd::follow_target_43acb0");

void* const kUnits = obj(d::g_units);

struct H {
    // ------------------------------------------------------------------ 0x0043acb0
    void reset() { joan::orig::as_player(this)->reset_goto(); }  // 0x0043ac60
    i32 order(i32 a, i32 b, i32 c) {                     // 0x004367c0, short result
        return static_cast<i32>(joan::orig::arg::as<std::int16_t>(joan::orig::as_player(this)->order_update(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c))));
    }
    static i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
    static void* unit_get(i32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
    static i32 unit_flag(void* u) { return static_cast<i32>(joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(u)->can_act())); }  // 0x00500ba0

    void follow_target() {
        c_follow.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) {
            reset();
            return;
        }
        void* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);  // ebp-0x8
        Vec3 my_pos;                             // ebp-0x14
        joan::orig::as_unit(unit)->position(&my_pos);  // 0x004fedb0
        Vec3 tgt_pos = {0.0f, 0.0f, 0.0f};  // ebp-0x20 (kind other than 0 / 1 is unreachable, see header)
        float range = 0.0f;                 // ebp-0x24
        const i32 kind = jo::self<jo::Player>(this)->goto_kind;
        if (kind == 0) {
            if (unit_missing(jo::self<jo::Player>(this)->goto_target_) != 0 ||
                joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(unit_get(jo::self<jo::Player>(this)->goto_target_))->dead()) != 0) {  // 0x0042d0e0
                reset();
                return;
            }
            void* const tgt = unit_get(jo::self<jo::Player>(this)->goto_target_);  // ebp-0x28
            joan::orig::as_unit(tgt)->position(&tgt_pos);
            float box[4];  // ebp-0x38 (filled completely by 0x005acc80)
            joan::orig::as_actor(jo::as_unit(tgt)->actor)->bounding_sphere(joan::orig::arg::mptr(box), joan::orig::arg::u32(i32{1}));  // 0x005acc80
            range = box[3];
        } else if (kind == 1) {
            if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->goto_target_)) != 0) {  // 0x00425d50
                reset();
                return;
            }
            void* const tgt = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->goto_target_));  // 0x00425d20
            joan::orig::as_map_object(tgt)->position(&tgt_pos);                            // 0x00499930
            float box[4];  // ebp-0x4c
            joan::orig::as_actor(jo::view<void*>(jo::as_map_object(tgt)->actor))->bounding_sphere(joan::orig::arg::mptr(box), joan::orig::arg::u32(i32{1}));
            range = box[3];
        }
        Vec3 tmp;  // ebp-0x8c
        Vec3 diff = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &tgt_pos, &my_pos));  // 0x0060cc90, ebp-0x80
        const float dist = v3d_float(d::iat_v3d_87_length, static_cast<const void*>(&diff));  // length
        if (!(range + global<float>(d::flt_10) > dist)) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(&jo::as_unit(unit)->path_b)->empty()) == 0) return;  // 0x0043c420
            if (joan::orig::arg::as<i32>(joan::orig::as_list(&jo::as_unit(unit)->path)->empty()) == 0) return;
            const i32 id = jo::self<jo::Player>(this)->goto_target_;  // ebp-0x70
            const i32 k = jo::self<jo::Player>(this)->goto_kind;   // ebp-0x74
            switch (jo::view<i32>(jo::self<jo::Player>(this)->goto_time)) {
            case 1:
                reset();
                joan::orig::as_ctrl(this)->set_target_43a7d0(joan::orig::arg::i32(id), joan::orig::arg::i32(k), i32{1});  // 0x0043a7d0
                break;
            case 2:
                jo::view<u16>(jo::self<jo::Player>(this)->goto_active) = 0;
                jo::view<i32>(jo::self<jo::Player>(this)->goto_time) = 1;
                joan::orig::as_ctrl(this)->set_target_43a7d0(joan::orig::arg::i32(id), joan::orig::arg::i32(k), i32{2});
                break;
            case 3:
                joan::orig::as_player(this)->goto_target(joan::orig::arg::i32(jo::self<jo::Player>(this)->goto_target_), joan::orig::arg::i32(jo::self<jo::Player>(this)->goto_kind));  // 0x0043a9e0
                break;
            default:
                break;
            }
            return;
        }
        Vec3 tmp2;  // ebp-0xa4
        Vec3 d2 = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp2, &tgt_pos, &my_pos));  // ebp-0x98
        const Vec3 n = *import_call<Vec3*>(d::iat_v3d_87_vector_normalize_1, &d2);                // normalize in place, ebp-0x5c
        const double ang = joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(n.y), joan::orig::arg::f32(n.x)));         // 0x00421020 atan2
        const float deg = static_cast<float>(Fpu{true}.div(ang, global<float>(d::flt_0_017453292)));
        joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(deg));  // 0x0050abd0
        joan::orig::as_squad(unit)->clear();       // 0x00422f10
        switch (jo::view<i32>(jo::self<jo::Player>(this)->goto_time)) {
        case 1:
            if (unit_flag(unit) != 0) order(1, 0, 0);
            break;
        case 2: {
            const u32 elapsed = global<u32>(d::g_now) - jo::self<jo::Player>(this)->press_time;
            if (elapsed > jo::as_unit(unit)->combo_max_time || elapsed <= jo::as_unit(unit)->combo_min_time) {
                if (static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x1c) break;
                if (jo::self<jo::Player>(this)->goto_active == 0) {
                    if (order(1, 0, 0) != 0) jo::view<u16>(jo::self<jo::Player>(this)->goto_active) = 1;
                } else {
                    jo::view<u16>(jo::self<jo::Player>(this)->goto_active) = 0;
                    jo::view<i32>(jo::self<jo::Player>(this)->goto_time) = 1;
                }
                break;
            }
            const float rnd = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->rand_unit_inclusive());  // 0x00590660, ebp-0x60
            i32 a = 0;  // ebp-0x64
            i32 b = 0;  // ebp-0x68
            if (rnd > global<float>(d::flt_0_6)) {
                a = 1;
            } else if (rnd > global<float>(d::flt_0_3)) {
                b = 1;
            }
            if (order(a, b, 0) != 0) jo::view<u16>(jo::self<jo::Player>(this)->goto_active) = 1;
            break;
        }
        case 3: {
            const i32 v = jo::as_unit(unit)->mana;  // ebp-0x6c
            if (unit_flag(unit) != 0 && v >= global<i32>(d::g_unit_egatk_mp)) {
                joan::orig::as_order(this)->try_437910(joan::orig::arg::u32(i32{1}));  // 0x00437910
                reset();
            } else if (unit_flag(unit) != 0) {
                order(1, 0, 0);
                reset();
            }
            break;
        }
        default:
            break;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_09", name, addr, fn)
B("Cmd::follow_target_43acb0", 0x0043acb0, &H::follow_target);
#undef B

}  // namespace game_b111_09
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
namespace {
namespace game_units5 {

JOAN_COUNTER(c_attach_unit, "Cell::attach_unit");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // `this` through a volatile copy: the cell functions are called with a null cell (detach
    // only), and the compiler may otherwise fold `this == nullptr` to false.
    std::uint8_t* maybe_null_self() {
        std::uint8_t* volatile p = self();
        return p;
    }

    // 0x0043b190 (ret 4)
    void attach_unit(std::uint8_t* unit) {
        std::uint8_t* const cell = maybe_null_self();
        c_attach_unit.hit();
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell);
        if (cell == old) return;
        if (old != nullptr) {
            std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell_prev);
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell_next);
            if (prev != nullptr) jo::view<std::uint8_t*>(jo::as_map_object(prev)->cell_next) = next;
            else jo::view<std::uint8_t*>(jo::as_cell(old)->objects_head) = next;
            if (next != nullptr) jo::view<std::uint8_t*>(jo::as_map_object(next)->cell_prev) = prev;
            else jo::view<std::uint8_t*>(jo::as_cell(old)->objects_tail) = prev;
            jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell_prev) = nullptr;
            jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell_next) = nullptr;
            jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell) = nullptr;
        }
        if (cell == nullptr) return;
        std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_cell(cell)->objects_head);
        jo::view<std::uint8_t*>(jo::as_cell(cell)->objects_head) = unit;
        if (next != nullptr) jo::view<std::uint8_t*>(jo::as_map_object(next)->cell_prev) = unit;
        else jo::view<std::uint8_t*>(jo::as_cell(cell)->objects_tail) = unit;
        jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell_prev) = nullptr;
        jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell_next) = next;
        jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell) = cell;
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("Cell::attach_unit", 0x0043b190, &H::attach_unit);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

void* const kUnitTable = obj(d::g_units);

using u32 = std::uint32_t;

struct RV3 {  // 12-byte vector copied as raw dwords
    u32 x, y, z;
};
static_assert(sizeof(RV3) == 12, "RV3");

JOAN_COUNTER(c_object_reach, "Cmd::check_object_reach");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
RV3 vsub(const void* a, const void* b) {  // 0x0060cc90 (a - b), result copied out of the temp
    RV3 tmp;
    return *joan::orig::arg::as<RV3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}
i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {
    // ------------------------------------------------------------------ 0x0043b2d0
    void check_object_reach() {
        c_object_reach.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) {
            joan::orig::as_player(this)->reset_goto();  // 0x0043ac60
            return;
        }
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        RV3 p;  // ebp-0x14
        joan::orig::as_unit(unit)->position(&p);  // 0x004fedb0
        if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->goto_target_)) != 0) return;  // 0x00425d50
        u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Player>(this)->goto_target_)));  // 0x00425d20
        RV3 q;  // ebp-0x24
        joan::orig::as_map_object(o)->position(&q);  // 0x00499930
        RV3 w = vsub(&p, &q);         // ebp-0x48
        const float len = v3d_float(d::iat_v3d_87_length, static_cast<void*>(&w));  // length
        float sphere[4];  // ebp-0x38
        joan::orig::as_actor(jo::view<void*>(jo::as_map_object(o)->actor))->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), 1u);  // 0x005acc80
        const float r = sphere[3];
        if (!(r + global<float>(d::flt_15) > len)) return;  // + 15.0f
        joan::orig::as_squad(unit)->clear();  // 0x00422f10
        if (static_cast<i32>(jo::as_map_object(o)->class_id) != 1) return;
        const u32 g1 = global<u32>(d::g_now);
        const u32 g2 = global<u32>(d::g_now);
        joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(// 0x004eef00 (ret 0x40)
                       0x20), 2u, joan::orig::arg::i16(1), 1u, joan::orig::arg::i16(sx16(jo::view<u16[2]>(jo::self<jo::Player>(this)->unit_id_)[0])), static_cast<std::int8_t>(joan::orig::arg::i32(1)), f3(p), 0x3f800000u, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::u32(g2), joan::orig::arg::u32(g1), static_cast<std::int8_t>(joan::orig::arg::i32(1)), nullptr, joan::orig::arg::i16(0));
        joan::orig::as_player(this)->reset_goto();  // 0x0043ac60
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Cmd::check_object_reach", 0x0043b2d0, &H::check_object_reach);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using u32 = std::uint32_t;

struct H {
    u32 dtor_43b440() {
        joan::orig::as_list(this)->clear_4734a0();                          // 0x004734a0
        return thiscall<u32>(fn::dtor_DynArrayB, this);              // 0x00585710
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::dtor_43b440", 0x0043b440, &H::dtor_43b440);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01 {

void* const kItemTypes = obj(d::g_item_res);

using u32 = std::uint32_t;

struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "by-value 12-byte argument");
struct FVec {
    float x, y, z;
};
static_assert(sizeof(FVec) == 12, "by-value 12-byte argument");

JOAN_COUNTER(c_consume_43b490, "Order::consume_43b490");
JOAN_COUNTER(c_fire_43bc50, "Order::fire_43bc50");
JOAN_COUNTER(c_alencon_43c080, "Order::alencon_43c080");

constexpr img::Addr kUnits = d::g_units;

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* unit_find(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3f0
u8* item_type(u32 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(w))); }  // 0x00425ce0
u32 g_now() { return global<u32>(d::g_now); }
using joan::orig::helpers::cf;

FVec vsub(const void* a, const void* b) {  // 0x0060cc90 (out, a, b) -> *out
    FVec t;
    return *joan::orig::arg::as<FVec*>(joan::orig::vec_sub(&t, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}

void spiritual(u8* rec) {
    const u32 h = joan::orig::arg::as<u32>(joan::orig::as_name_list(obj(d::g_pemi_res))->find_4c2060(str(d::s_spiritual_txt), static_cast<std::int8_t>(joan::orig::arg::i32(1u))));  // 0x004c2060
    if (h == 0) return;
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(h)));  // 0x0041b440
    void* const fx = jo::self<jo::PemiScript>(p)->effect;
    joan::orig::Scene* const who = jo::as_actor(jo::as_unit(rec)->actor)->scene;
    const u32 r = joan::orig::arg::as<u32>(joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(who, fx, obj(d::g_pos), obj(d::g_pos), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), 0, 0, 0x1388));                              // 0x0041baf0
    void* q = joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(joan::orig::arg::i32(r));  // 0x0041bd60
    joan::orig::as_effect(q)->set_owner(joan::orig::arg::mptr(jo::as_unit(rec)->actor), joan::orig::arg::mptr(0u));             // 0x0041b0d0
}

struct H {
    u32 dtor_43c750() {
        jo::self<jo::Order0c_a>(this)->vtable = obj(d::vt_Obj_9d3ab4);
        joan::orig::as_unit(this)->dtor_427c90();          // 0x00427c90
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->destruct());   // 0x00415790
    }

    // ------------------------------------------------------------ 0x0043b490
    // one pass: nearest inventory entry of `kind`; returns true when an entry was found
    bool consume_pass(u8* rec, i32 vc, i32 kind, std::uintptr_t use) {
        i32 best = 0, bestd = 0;
        for (i32 i = joan::orig::as_dynarray(jo::view<void*>(jo::as_modifiers(jo::as_unit(rec)->modifiers)->grid_16))->size(); i > 0; --i) {  // 0x00417ad0
            void* list = jo::view<void*>(jo::as_modifiers(jo::as_unit(rec)->modifiers)->grid_16);
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(list)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00457f60
            u8* e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::view<void*>(jo::as_modifiers(jo::as_unit(rec)->modifiers)->grid_16))->get(joan::orig::arg::i32(i)));  // 0x00457e90
            if (jo::view<i16>(jo::as_item_def(item_type(jo::view<u16>(jo::as_inv_item(e)->type)))->type_id) != kind) continue;
            const i32 d = joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(jo::as_item_def(item_type(jo::view<u16>(jo::as_inv_item(e)->type)))->damage - vc)));  // abs
            if (best == 0 || !(d > bestd)) {
                bestd = d;
                best = i;
            }
        }
        if (best == 0) return false;
        u8* e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::view<void*>(jo::as_modifiers(jo::as_unit(rec)->modifiers)->grid_16))->get(joan::orig::arg::i32(best)));  // 0x00457e90
        if (thiscall<i32>(use, rec, static_cast<i32>(jo::as_inv_item(e)->type), 0u, 0u) != 0) {
            jo::as_inv_item(e)->count = static_cast<i16>(jo::as_inv_item(e)->count - 1);
            if (jo::as_inv_item(e)->count == 0) {
                joan::orig::as_grid(jo::view<void*>(jo::as_modifiers(jo::as_unit(rec)->modifiers)->grid_16))->fill_from(joan::orig::arg::i32(best));  // 0x00457ec0
            }
            if (jo::as_unit(rec)->actor != nullptr) spiritual(rec);
        }
        return true;
    }
    void consume_43b490(u32 arg) {
        c_consume_43b490.hit();
        if (arg == 0) arg = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
        if (unit_dead(arg)) return;
        u8* rec = unit_rec(arg);
        if (jo::as_unit(rec)->modifiers == nullptr) return;
        const i32 vc = joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->hit_points_max()) - jo::as_unit(rec)->hit_points;  // 0x00523320
        if (consume_pass(rec, vc, 0x15, fn::code_value(fn::Unit_heal))) return;  // 0x0050b4f0
        (void)consume_pass(rec, vc, 0x19, fn::code_value(fn::Unit_heal_and_mana));        // 0x0050bcd0
    }

    // ------------------------------------------------------------ 0x0043bc50
    u32 fire_43bc50() {
        c_fire_43bc50.hit();
        if (unit_dead(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) return 0;
        u8* rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->find_target(0xc8));  // 0x004d6320
        if (n <= 0) return 0;
        const u32 t1 = g_now();
        const u32 t2 = g_now();
        const Vec3 v = jo::view<Vec3>(jo::as_unit(rec)->position_);
        const u32 a3 = (v.z & 0xffff0000u) | (static_cast<u32>(n) & 0xffffu);
        const u32 a5 = (v.y & 0xffff0000u) | static_cast<u32>(jo::view<u16[2]>(jo::self<jo::Player>(this)->unit_id_)[0]);
        joan::orig::as_triggers(obj(d::g_a5ccd0))->add(0x200u, 2u, joan::orig::arg::i16(a3), 1u, joan::orig::arg::i16(a5), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), f3(v), 0x43480000u, static_cast<std::int8_t>(joan::orig::arg::i32(0u)), joan::orig::arg::u32(t2), joan::orig::arg::u32(t1), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), 0u, joan::orig::arg::i16(0u));  // 0x004eef00
        return 1;
    }

    // ------------------------------------------------------------ 0x0043c080
    u32 alencon_43c080() {
        c_alencon_43c080.hit();
        if (unit_dead(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) return 0;
        u8* rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->find_zone_index());  // 0x005156d0
        if (v <= 0) return 0;
        u8 flag = 0;
        void* o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(v)));  // 0x0043c450
        // slot 0x1c is 0x0046cb90 (returns this) only in the Siege vtable 0x009d0980: the kind-0x10 entry is a Siege
        u8* d = virtual_call<u8*>(o, 0x1c);
        if (jo::as_siege(d)->i32_79 != 0) {
            const u32 name = joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(str(d::s_ALENCON_9f9cf8)));  // 0x005270b0 'ALENCON'
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_units(obj(kUnits))->find_by_3b(joan::orig::arg::i32(name)));                   // 0x0051fd50
            u8* q = unit_find(id);
            if (q != nullptr && static_cast<i32>(jo::as_unit(q)->id) == jo::self<jo::Player>(this)->unit_id_) {
                FVec dv = vsub(jo::as_unit(q)->position_, jo::as_siege(d)->pos);
                if (!(v3d_float(d::iat_v3d_87_length, &dv) >= cf(d::flt_1024))) flag = 1;  // length < 1024
            }
        } else {
            flag = 1;
        }
        if (flag == 0) return 0;
        global<u32>(d::g_a4856d) = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
        global<i32>(d::g_a48569) = v;
        joan::orig::as_workshop_screen(obj(d::g_workshop_screen))->set_open(1u);  // 0x006ed3e0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Order::consume_43b490", 0x0043b490, &H::consume_43b490);
B("Order::fire_43bc50", 0x0043bc50, &H::fire_43bc50);
B("Order::alencon_43c080", 0x0043c080, &H::alencon_43c080);
B("Obj::dtor_43c750", 0x0043c750, &H::dtor_43c750);
#undef B

}  // namespace game_b104_01
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

constexpr fn::Code kListGet = fn::List_get_41b3f0;    // 0x0041b3f0
void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

constexpr img::Addr kGameUnits(d::g_units);

JOAN_COUNTER(c_unit_gate, "UnitDraw::draw");

struct H {
    void* list_get_wrap(std::int32_t i) { return list_get(this, i); }                   // 0x0043c450 / 0x00485730 / 0x0063e3c0

    // 0x0043b890 UnitDraw::draw: hidden (0x00522850) unless the unit is alive, visible and
    // not the player's mount view; else lights, body (0x004fb320) and extras (0x005215f0).
    void unit_draw() {
        std::uint8_t* const unit = static_cast<std::uint8_t*>(list_get(img::obj(kGameUnits), jo::self<jo::UnitDraw>(this)->unit_id));
        if (unit == nullptr) return;
        if (jo::as_unit(unit)->actor == nullptr) return;
        c_unit_gate.hit();
        if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_actor(jo::as_unit(unit)->actor)->byte_10a()) & 0xff) == 0 ||           // 0x0044cee0
            static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_shop(img::obj(d::g_game_a5d380))->active())) != 0 ||  // 0x0042ced0
            global<std::uint32_t>(d::g_camera_lock_flag_a48ce7) == 0 ||
            (static_cast<std::int32_t>(jo::as_unit(unit)->id) == global<std::int32_t>(d::g_player_unit_recno) && global<std::int32_t>(d::g_control_mode) != 4) ||
            jo::as_unit(unit)->move_mode >= 3) {
            joan::orig::as_unit_draw(this)->hide();  // 0x00522850
            return;
        }
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_) != nullptr && jo::as_unit(unit)->move_mode < 3) joan::orig::as_unit_draw(this)->show();  // 0x005221c0
        joan::orig::as_camera(global<void*>(d::g_camera))->update_matrices();  // 0x006141a0 Camera::update
        joan::orig::as_unit_draw(this)->plate();                      // 0x004fb320
        joan::orig::as_unit_draw(this)->label();                      // 0x005215f0
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
#define W2(name, addr, fn) JOAN_REPLACE("game/world2", name, addr, fn)
W1("List::get_e", 0x0043c450, &H::list_get_wrap);
W2("UnitDraw::draw", 0x0043b890, &H::unit_draw);
#undef W1
#undef W2

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);
void* const kItems = obj(d::g_item_res);
void* const kEvents = obj(d::g_pemi_res);
void* const kEffects = obj(d::g_particle_effect_array);

JOAN_COUNTER(c_spiritual, "Unit::use_spiritual_item");
JOAN_COUNTER(c_spawn_reset, "Pair::spawn_and_reset");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x0043b990
    void use_spiritual_item(i32 id) {
        c_spiritual.hit();
        if (id == 0) id = jo::self<jo::Player>(this)->unit_id_;
        if (unit_missing(id) != 0) return;
        u8* const unit = unit_get(id);
        if (jo::as_unit(unit)->modifiers == nullptr) return;
        const i32 age = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->value_149()) - jo::as_unit(unit)->mana;  // 0x00523700
        i32 best = 0;
        i32 best_d = 0;
        for (i32 i = list_count(jo::view<void*>(jo::as_modifiers(jo::as_unit(unit)->modifiers)->grid_16)); i > 0; --i) {
            void* const list = jo::view<void*>(jo::as_modifiers(jo::as_unit(unit)->modifiers)->grid_16);
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(list)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00457f60
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::view<void*>(jo::as_modifiers(jo::as_unit(unit)->modifiers)->grid_16))->get(joan::orig::arg::i32(i)));  // 0x00457e90
            u8* const item = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(jo::as_inv_item(e)->type)));  // 0x00425ce0
            if (static_cast<i32>(jo::view<i16>(jo::as_item_def(item)->type_id)) != 0x17) continue;
            u8* const item2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(jo::as_inv_item(e)->type)));
            const i32 d = joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(jo::as_item_def(item2)->damage - age)));  // abs
            if (best == 0 || d <= best_d) {
                best_d = d;
                best = i;
            }
        }
        if (best == 0) return;
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::view<void*>(jo::as_modifiers(jo::as_unit(unit)->modifiers)->grid_16))->get(joan::orig::arg::i32(best)));  // 0x00457e90
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->heal_group(joan::orig::arg::u32(static_cast<i32>(jo::as_inv_item(e)->type)), 0, 0)) == 0) return;  // 0x0050ba00
        jo::as_inv_item(e)->count = static_cast<i16>(jo::as_inv_item(e)->count - 1);
        if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(jo::view<void*>(jo::as_modifiers(jo::as_unit(unit)->modifiers)->grid_16))->fill_from(joan::orig::arg::i32(best));  // 0x00457ec0
        if (jo::as_unit(unit)->actor == nullptr) return;
        const std::int32_t def = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_list(kEvents)->find_4c2060(str(d::s_spiritual_txt), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060 "spiritual.txt"
        if (def == 0) return;
        void* const pos = obj(d::g_pos);
        u8* const ed = joan::orig::arg::as<u8*>(joan::orig::as_name_list(kEvents)->entry(def));
  // 0x0041b440
        const i32 effect = joan::orig::arg::as<i32>(joan::orig::as_model_list(kEffects)->add(jo::as_actor(jo::as_unit(unit)->actor)->scene, jo::self<jo::PemiScript>(ed)->effect, pos, pos, static_cast<std::int8_t>(joan::orig::arg::i32(1)), 0, 0, 0x1388));  // 0x0041baf0
        void* const fx = joan::orig::as_dynarray(kEffects)->ptr_at(joan::orig::arg::i32(effect));  // 0x0041bd60
        joan::orig::as_effect(fx)->set_owner(joan::orig::arg::mptr(static_cast<void*>(jo::view<u8*>(jo::as_unit(unit)->actor))), joan::orig::arg::mptr(static_cast<void*>(nullptr)));  // 0x0041b0d0
    }

    // ------------------------------------------------------------------ 0x0043c210
    i32 spawn_and_reset() {
        c_spawn_reset.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->nearest_enemy(0x64));  // 0x0048dfb0
        if (unit_missing(n) != 0) return 0;
        u8* const v = unit_get(n);
        joan::orig::as_member(v)->release();              // 0x0042fcb0
        joan::orig::as_unit(v)->link_to(joan::orig::arg::i32(jo::self<jo::Player>(this)->unit_id_));   // 0x00438f80
        joan::orig::as_unit(v)->release_slot();              // 0x00528300
        if (jo::as_unit(v)->orders != nullptr && jo::view<u32>(jo::as_order(jo::as_unit(v)->orders)->current.kind) != 0) {
            joan::orig::as_action(jo::as_unit(v)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(v)));  // 0x004bb660
            joan::orig::dynarray(jo::as_unit(v)->orders)->zap(1);              // 0x00584580
        }
        if (jo::as_unit(v)->draw != nullptr) {
            if (jo::as_unit_draw(jo::as_unit(v)->draw)->label_ != nullptr) {
                joan::orig::as_unit_draw(jo::as_unit(v)->draw)->detach_b();  // 0x005228e0
                void* const p = jo::view<void*>(jo::as_unit_draw(jo::as_unit(v)->draw)->label_);
                if (p != nullptr) virtual_call<void*>(p, 0, 1);  // scalar deleting dtor
                jo::as_unit_draw(jo::as_unit(v)->draw)->label_ = nullptr;
            }
            joan::orig::as_unit_draw(jo::as_unit(v)->draw)->create_label();  // 0x00520e80
        }
        return 1;
    }

    // ------------------------------------------------------------------ 0x0043c480
    void store_heading() {
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) {
            jo::view<u32>(jo::self<jo::Player>(this)->mount_heading_deg) = 0;
            return;
        }
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        jo::self<jo::Player>(this)->mount_heading_deg = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading()) / global<float>(d::flt_0_017453292);  // 0x004ff850
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Unit::use_spiritual_item", 0x0043b990, &H::use_spiritual_item);
B95("Pair::spawn_and_reset", 0x0043c210, &H::spawn_and_reset);
B95("Obj::store_heading", 0x0043c480, &H::store_heading);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_spawn_effect, "Order::spawn_effect");

i32 unit_invalid(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------------ 0x0043bd40
    i32 spawn_effect() {
        c_spawn_effect.hit();
        if (unit_invalid(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->nearest_target_a(0x64));  // 0x0046c1f0
        if (n <= 0) return 0;
        V3 pos;
        std::memcpy(&pos, jo::as_unit(unit)->position_, sizeof(V3));
        const i32 g1 = global<i32>(d::g_now);
        const i32 g2 = global<i32>(d::g_now);
        joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(// 0x004eef00 (ret 0x40)
                       static_cast<i32>(0x200)), joan::orig::arg::u32(static_cast<i32>(1)), joan::orig::arg::i16(static_cast<i16>(n)), joan::orig::arg::u32(static_cast<i32>(1)), joan::orig::arg::i16(jo::as_unit(unit)->id), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(1))), f3(pos), joan::orig::arg::bits(100.0f), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(0))), joan::orig::arg::u32(g2), joan::orig::arg::u32(g1), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(1))), nullptr, joan::orig::arg::i16(static_cast<i16>(0)));
        jo::view<u32>(jo::self<jo::Player>(this)->jump_hold_ms) = 0;
        return 1;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order::spawn_effect", 0x0043bd40, &H::spawn_effect);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0043be40
    i32 try_link_43be40() {
        const u32 id = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        const u32 target = joan::orig::arg::as<u32>(joan::orig::as_unit(unit)->nearest_target_b(i32{0xc8}));  // 0x0046fa80
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(target)) != 0) return 0;
        const u32 self_id = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);  // pushed before the lookup in the original
        void* const other = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(target));
        joan::orig::as_unit(other)->hire(joan::orig::arg::u32(self_id));  // 0x004e8500
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Unit::try_link_43be40", 0x0043be40, &H::try_link_43be40);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b107_08.cpp (game/b107_08)
namespace {
namespace game_b107_08 {

using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);

i32 unit_valid_check(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_recharge, "Unit::recharge_43bef0");

struct H {
    // 0x0043bef0
    i32 recharge() {
        c_recharge.hit();
        if (unit_valid_check(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        u8* const u = unit_get(jo::self<jo::Player>(this)->unit_id_);
        const i32 id = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->nearest_target_c(100));  // 0x00471c80
        if (unit_valid_check(id) != 0) return 0;
        joan::orig::as_unit(unit_get(id))->drop_carried();  // 0x005199b0
        joan::orig::as_unit(unit_get(id))->release_slot();  // 0x00528300
        jo::as_unit(unit_get(id))->vision_range = 2000;
        global<u32>(d::g_get_a591c8) = global<u32>(d::g_get_a591c8) + 1;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_08", name, addr, fn)
B("Unit::recharge_43bef0", 0x0043bef0, &H::recharge);
#undef B

}  // namespace game_b107_08
}  // namespace

// ---------------------------------------- from modules/game_b107_19.cpp (game/b107_19)
namespace {
namespace game_b107_19 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0043bfe0
    u32 start_6c0310_43bfe0() {
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return 0;  // 0x00428b70
        void* unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        const i32 count = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->find_zone());                       // 0x00514b80
        if (count > 0) {
            global<u32>(d::g_a48959) = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
            global<i32>(d::g_a48955) = count;
            joan::orig::as_barracks_screen(obj(d::g_barracks_screen))->set_open(1);  // 0x006c0310
            return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_19", name, addr, fn)
B("Unit::start_6c0310_43bfe0", 0x0043bfe0, &H::start_6c0310_43bfe0);
#undef B

}  // namespace game_b107_19
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

struct H {
    // 0x0043c3c0 / 0x0043c3f0 (this: the Input 0x00a516b0, every caller): clicks[i].x / .y, i unchecked as in the
    // original
    std::uint32_t at_672(std::int32_t i) { return static_cast<std::uint32_t>(jo::self<jo::Input>(this)->clicks[i].x); }
    std::uint32_t at_676(std::int32_t i) { return static_cast<std::uint32_t>(jo::self<jo::Input>(this)->clicks[i].y); }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Table::at_672", 0x0043c3c0, &H::at_672);
M("Table::at_676", 0x0043c3f0, &H::at_676);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b111_13.cpp (fix/uninit)
namespace {
namespace game_b111_13 {

using u32 = std::uint32_t;

const char* rstr(img::Addr a) { return &img::global<const char>(a); }
using joan::orig::helpers::wrap_sub;
using joan::orig::helpers::wrap_mul;

constexpr img::Addr kSoldiers(d::g_units);
constexpr img::Addr kSounds(d::g_critical_hud);

JOAN_COUNTER(c_promotion, "Unit::promotion_update_43ca90");

struct H {
    // ------------------------------------------------------------------------ 0x0043ca90
    // Promotion cost: the entry's +0x18, scaled by 0x00497e10(0x15, id) percent when +0x1c1.
    static i32 promotion_cost(u8* unit, const u8* entry) {
        i32 cost = jo::self<jo::AnimGroupRec>(entry)->min_mana;
        if (jo::as_unit(unit)->modifiers != nullptr) {
            const i32 pct = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::as_unit(unit)->modifiers)->sum(0x15, static_cast<i32>(jo::as_unit(unit)->id)));  // 0x00497e10
            cost = wrap_mul(pct, cost) / 100;
        }
        return cost;
    }

    i16 promotion_update(i32 a, i32 b) {
        c_promotion.hit();
        if (joan::orig::as_dynarray(img::obj(kSoldiers))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return 0;  // 0x00428b70
        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kSoldiers))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x0041b3c0
        if (!(jo::as_unit(u)->combo_state > 0)) {
            jo::self<jo::Player>(this)->button_up[0] = 1;
            jo::self<jo::Player>(this)->button_up[1] = 1;
            jo::self<jo::Player>(this)->button_up[2] = 1;
            jo::self<jo::Player>(this)->button_up[3] = 1;
        }
        if (a != 0) {
            if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[0]) == 0) {
                jo::self<jo::Player>(this)->button_down[0] = 1;
            } else if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[0]) == 2) {
                jo::self<jo::Player>(this)->button_down[0] = 3;
            }
        } else {
            if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[0]) == 1) {
                jo::self<jo::Player>(this)->button_down[0] = 2;
            } else if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[0]) == 3) {
                jo::self<jo::Player>(this)->button_down[0] = 2;
            } else if (jo::as_unit(u)->combo_state > 0) {
                jo::self<jo::Player>(this)->button_down[0] = 2;
            }
            jo::self<jo::Player>(this)->button_up[0] = 1;
        }
        if (b != 0) {
            if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[1]) == 0) {
                jo::self<jo::Player>(this)->button_down[1] = 1;
            } else if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[1]) == 2) {
                jo::self<jo::Player>(this)->button_down[1] = 3;
            }
        } else {
            if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[1]) == 1) {
                jo::self<jo::Player>(this)->button_down[1] = 2;
            } else if (static_cast<i8>(jo::self<jo::Player>(this)->button_down[1]) == 3) {
                jo::self<jo::Player>(this)->button_down[1] = 2;
            } else if (jo::as_unit(u)->combo_state > 0) {
                jo::self<jo::Player>(this)->button_down[1] = 2;
            }
            jo::self<jo::Player>(this)->button_up[1] = 1;
        }
        if (jo::as_unit(u)->combo_state == -1) {
            if (global<u32>(d::g_now) - jo::as_unit(u)->combo_time > 0xfau) {
                jo::as_unit(u)->combo_time = 0;
                jo::as_unit(u)->combo_state = 0;
            }
        }
        const u32 now = global<u32>(d::g_now);  // [ebp-0xc]
        if (a != 0 && jo::as_unit(u)->combo_time == 0) {
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) != 0) {  // 0x00500ba0
                if (static_cast<i32>(jo::as_unit(u)->action) != 1) {
                    joan::orig::as_critical_hud(img::obj(kSounds))->input(static_cast<std::int8_t>(joan::orig::arg::i32(0x41)), static_cast<std::int8_t>(joan::orig::arg::i32(1)));  // 0x00462810
                    joan::orig::as_unit(u)->set_action(0x49, 0x41, 1);  // 0x00501190
                    jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
                    jo::self<jo::Player>(this)->order_state = 0;
                    return 1;
                }
            } else if (jo::as_unit(u)->combo_state > 0 && static_cast<i8>(jo::self<jo::Player>(this)->button_down[0]) == 3 &&
                       static_cast<i8>(jo::self<jo::Player>(this)->button_up[0]) == 1) {
                jo::self<jo::Player>(this)->button_up[0] = 0;
                if (!(now - jo::self<jo::Player>(this)->press_time < jo::as_unit(u)->combo_min_time) && now - jo::self<jo::Player>(this)->press_time < jo::as_unit(u)->combo_max_time) {
                    jo::view<u16>(jo::self<jo::Player>(this)->combo_step) = 1;
                    jo::self<jo::Player>(this)->order_state = 1;
                    jo::view<u16>(jo::as_unit(u)->combo_queued) = 1;
                    return 1;
                }
            }
        }
        if (b != 0 && jo::as_unit(u)->combo_time == 0) {
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) != 0) {
                if (static_cast<i32>(jo::as_unit(u)->action) != 1) {
                    const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_anim_table(jo::as_unit(u)->anim_set)->find(0x49, joan::orig::arg::str(rstr(d::s_B))));  // 0x00416e30
                    if (idx != 0) {
                        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(u)->anim_set)->entry(joan::orig::arg::u32(idx)));  // 0x004168d0
                        if (joan::orig::arg::as<i32>(joan::orig::as_anim_set_entry(e)->is_yes()) != 0 ||                             // 0x00416ed0
                            joan::orig::arg::as<i32>(joan::orig::as_unit(u)->find_link(0x10, joan::orig::arg::str(0))) != 0) {                    // 0x0050d3d0
                            const i32 cost = promotion_cost(u, e);
                            if (!(jo::as_unit(u)->mana < cost)) {
                                joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(wrap_sub(jo::as_unit(u)->mana, cost)));  // 0x004fe5d0
                                joan::orig::as_critical_hud(img::obj(kSounds))->input(static_cast<std::int8_t>(joan::orig::arg::i32(0x42)), static_cast<std::int8_t>(joan::orig::arg::i32(1)));
                                joan::orig::as_unit(u)->set_action(0x49, 0x42, 1);
                                jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
                                jo::self<jo::Player>(this)->order_state = 0;
                                return 1;
                            }
                        }
                    }
                }
            } else if (jo::as_unit(u)->combo_state > 0 && static_cast<i8>(jo::self<jo::Player>(this)->button_down[1]) == 3 &&
                       static_cast<i8>(jo::self<jo::Player>(this)->button_up[1]) == 1) {
                jo::self<jo::Player>(this)->button_up[1] = 0;
                if (!(now - jo::self<jo::Player>(this)->press_time < jo::as_unit(u)->combo_min_time) && now - jo::self<jo::Player>(this)->press_time < jo::as_unit(u)->combo_max_time) {
                    jo::view<u16>(jo::self<jo::Player>(this)->combo_step) = 2;
                    jo::self<jo::Player>(this)->order_state = 1;
                    jo::view<u16>(jo::as_unit(u)->combo_queued) = 1;
                    return 1;
                }
            }
        }
        if (!(now - jo::self<jo::Player>(this)->press_time < jo::as_unit(u)->combo_max_time) && static_cast<i8>(jo::self<jo::Player>(this)->order_state) == 1) {
            void* const list = jo::as_unit(u)->anim_set;  // [ebp-0x1c]
            i8 tag = 0x26;                           // [ebp-0x24]
            if (std::strcmp(joan::orig::arg::str(reinterpret_cast<char*>(jo::as_unit(u)->armour_flags)), joan::orig::arg::str(rstr(d::s_AAA))) == 0) {  // "AAA"
                jo::as_unit(u)->combo_state = 1;
                jo::as_unit(u)->armour_flags[1] = 0;
                jo::as_unit(u)->armour_flags[2] = 0;
            }
            if (std::strcmp(joan::orig::arg::str(reinterpret_cast<char*>(jo::as_unit(u)->armour_flags)), joan::orig::arg::str(rstr(d::s_BBB))) == 0) {  // "BBB"
                jo::as_unit(u)->combo_state = 1;
                jo::as_unit(u)->armour_flags[1] = 0;
                jo::as_unit(u)->armour_flags[2] = 0;
            }
            // fix/uninit: [ebp-0x20] is never written when no entry passes the header compares.
            i32 found = 0;
            u8* e = nullptr;  // [ebp-0x2c]
            i32 j = 0;        // [ebp-0x30]
            for (i32 n = jo::as_anim_set(list)->group_count; n > 0; --n) {
                e = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(list)->entry(joan::orig::arg::u32(n)));  // 0x004168d0
                if (jo::view<i32>(jo::as_anim_group_rec(e)->is_jump) != static_cast<i32>(jo::as_unit(u)->action)) continue;
                if (jo::as_anim_group_rec(e)->kind != static_cast<i32>(jo::as_unit(u)->anim_kind)) continue;
                if (jo::as_anim_group_rec(e)->weapon_kind != static_cast<i32>(jo::as_unit(u)->anim_item_using)) continue;
                if (jo::as_anim_group_rec(e)->direction != static_cast<i32>(jo::as_unit(u)->anim_special)) continue;
                found = 1;
                for (j = 0; j < static_cast<i32>(jo::as_unit(u)->combo_state); ++j) {
                    if (static_cast<i8>(e[8 + j]) != static_cast<i8>(u[0x227 + j])) {
                        found = 0;
                        break;
                    }
                }
                if (found != 0) {
                    if (static_cast<i32>(jo::self<jo::Player>(this)->combo_step) == 1) {
                        tag = 0x41;
                    } else if (static_cast<i32>(jo::self<jo::Player>(this)->combo_step) == 2) {
                        tag = 0x42;
                    } else if (static_cast<i32>(jo::self<jo::Player>(this)->combo_step) == 4) {
                        found = 0;
                    }
                    if (static_cast<i8>(e[8 + j]) != tag) found = 0;
                }
                if (found == 0) continue;
                for (++j; j < 6; ++j) {
                    if (static_cast<i8>(e[8 + j]) != 0) {
                        found = 0;
                        break;
                    }
                }
                if (found == 1) break;
            }
            if (found == 1) {
                if (static_cast<i32>(jo::self<jo::Player>(this)->combo_step) == 1) {
                    joan::orig::as_unit(u)->set_action(0x49, 0x41, 1);
                    joan::orig::as_critical_hud(img::obj(kSounds))->input(static_cast<std::int8_t>(joan::orig::arg::i32(0x41)), static_cast<std::int8_t>(joan::orig::arg::i32(0)));
                } else if (static_cast<i32>(jo::self<jo::Player>(this)->combo_step) == 2) {
                    const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_anim_table(jo::as_unit(u)->anim_set)->find(0x49, joan::orig::arg::str(rstr(d::s_B))));
                    if (idx != 0) {
                        u8* const pe = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(u)->anim_set)->entry(joan::orig::arg::u32(idx)));
                        const i32 cost = promotion_cost(u, pe);
                        if (!(jo::as_unit(u)->mana < cost)) {
                            joan::orig::as_unit(u)->set_145(joan::orig::arg::i32(wrap_sub(jo::as_unit(u)->mana, cost)));
                            joan::orig::as_critical_hud(img::obj(kSounds))->input(static_cast<std::int8_t>(joan::orig::arg::i32(0x42)), static_cast<std::int8_t>(joan::orig::arg::i32(0)));
                            joan::orig::as_unit(u)->set_action(0x49, 0x42, 1);
                        }
                    }
                }
                jo::self<jo::Player>(this)->press_time = global<u32>(d::g_now);
            } else {
                jo::self<jo::Player>(this)->button_down[0] = 0;
                jo::self<jo::Player>(this)->button_down[1] = 0;
                jo::self<jo::Player>(this)->button_down[2] = 0;
                jo::self<jo::Player>(this)->button_down[3] = 0;
            }
            jo::self<jo::Player>(this)->order_state = 0;
            jo::view<u16>(jo::as_unit(u)->combo_queued) = 0;
            return 1;
        }
        if (!(now - jo::self<jo::Player>(this)->press_time < jo::as_unit(u)->combo_max_time)) {
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->can_act()) != 0 && jo::as_unit(u)->combo_state == 0) {
                jo::self<jo::Player>(this)->button_down[0] = 0;
                jo::self<jo::Player>(this)->button_down[1] = 0;
                jo::self<jo::Player>(this)->button_down[2] = 0;
                jo::self<jo::Player>(this)->button_down[3] = 0;
            }
        }
        return 0;
    }
};

// fix/uninit: 0x0043ca90 compares the never-written "entry found" flag [ebp-0x20] with 1 when the
// promotion list is empty or no entry passes the header compares; the flag starts at 0 here.
JOAN_REPLACE("fix/uninit", "Unit::promotion_update_43ca90", 0x0043ca90, &H::promotion_update);

}  // namespace game_b111_13
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

void* const kCameras = obj(d::g_camera_man);

using u32 = std::uint32_t;

u32 bits(float v) {
    u32 b;
    std::memcpy(&b, &v, 4);
    return b;
}

void* const kUnits = obj(d::g_units);

struct Box {  // 40-byte camera box (0x00423fe0 copies +0x28..+0x50 of the camera)
    u32 d[10];
};
static_assert(sizeof(Box) == 40, "camera box");
struct Vec3 {
    float x, y, z;
};

JOAN_COUNTER(c_throw, "Unit::throw_to");

// Camera of the active view: 0x00563c80(0x00563dc0()) on 0xa48cc0.
void* active_camera() {
    const u32 id = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kCameras)->target());  // 0x00563dc0
    return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(joan::orig::arg::i32(id)));    // 0x00563c80
}

struct H {
    // ------------------------------------------------------------------ 0x0043d560
    i16 throw_to(i32 enable, i32 range) {
        c_throw.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return 0;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x0041b3c0
        if (jo::as_unit(unit)->actor == nullptr) return 0;
        Box box{};
        joan::orig::as_camera_motion(active_camera())->end(&box);  // 0x00423fe0
        if (enable == 0) return 0;
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->can_act())) == 0) return 0;  // 0x00500ba0
        if (jo::as_unit(unit)->action == 1) return 0;
        u32 angle = 0;
        joan::orig::as_angle(&angle)->construct();  // 0x00576120
        float first = joan::orig::arg::as<float>(joan::orig::as_pad(this)->stick_angle_433af0());  // 0x00433af0
        if (bits(first) != 0xbf800000u) {
            const float r = joan::orig::arg::as<float>(joan::orig::as_pad(this)->stick_angle_433af0());
            const float y = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&box, joan::orig::arg::f32(r)));  // 0x0042fe30
            joan::orig::as_angle(&angle)->set_byte(joan::orig::arg::f32(y));                         // 0x005762c0
        } else {
            joan::orig::as_angle(&angle)->set_byte(0.0f);
        }
        const float y2 = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&box, joan::orig::arg::f32(first)));
        joan::orig::as_angle(&angle)->set_byte(joan::orig::arg::f32(y2));
        const float scale_x = static_cast<float>(static_cast<i32>(static_cast<u32>(range) * 0x578u));
        jo::as_unit(unit)->throw_target[0] = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos()) * scale_x;  // 0x00576ac0
        const float scale_y = static_cast<float>(static_cast<i32>(static_cast<u32>(range) * 0x578u));
        jo::as_unit(unit)->throw_target[1] = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin()) * scale_y;  // 0x005768d0
        jo::view<u32>(jo::as_unit(unit)->throw_target[2]) = 0x461c4000u;
        Vec3 target;
        std::memcpy(&target, jo::as_unit(unit)->throw_target, sizeof target);
        joan::orig::as_unit(unit)->set_view(joan::orig::arg::bits(target.x), joan::orig::arg::bits(target.y), joan::orig::arg::bits(target.z));  // 0x00505cc0
        joan::orig::as_unit(unit)->set_action(i32{0x1a}, i32{0}, i32{1});           // 0x00501190
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->input(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0x44})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));        // 0x00462810
        jo::self<jo::Player>(this)->u32_a8 = 0;
        return 1;
    }
};

JOAN_REPLACE("game/b108_07", "Unit::throw_to", 0x0043d560, &H::throw_to);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01_2 {

using u32 = std::uint32_t;

void* const kTypes = obj(d::g_item_res);

i32 word_arg(u32 raw) { return static_cast<i32>(static_cast<i16>(raw)); }

struct H {
    // ------------------------------------------------------------------ 0x0043d730
    u8* set_type_43d730(u32 type) {
        joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type));                                  // 0x004178e0
        u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(word_arg(type))));  // 0x00425ce0
        jo::self<jo::Order0c_a>(this)->u32_c6 = jo::view<u32>(jo::as_item_def(rec)->i32_5e);
        jo::view<u32>(jo::self<jo::Order0c_a>(this)->amount) = jo::self<jo::Order0c_a>(this)->u32_c6;
        jo::self<jo::Order0c_a>(this)->busy = jo::view<u32>(jo::as_item_def(rec)->model_count);
        return rec;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Unit::set_type_43d730", 0x0043d730, &H::set_type_43d730);
#undef B

}  // namespace game_b103_01_2
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

using joan::orig::helpers::units;

using u32 = std::uint32_t;

float fconst(img::Addr a) { return img::global<const float>(a); }

JOAN_COUNTER(c_order_apply, "Order::apply_43d7a0");

struct H {
    // ------------------------------------------------------------------ 0x0043d7a0
    void order_apply(i32 cmd, i32 force) {
        c_order_apply.hit();
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id))) != 0) return;
        void* const target = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id)));
        if (jo::view<i8>(jo::as_unit(unit)->mounted_flag) == 0) return;
        {
            const i32 s = jo::as_unit(unit)->anim_kind;
            if (s == 0x4f || s == 0x50 || s == 0x51 || s == 0x52) return;
        }
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(target)->can_act())) == 0) return;  // 0x00500ba0
        if (static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_unit(target)->is_kind_special())) != 0 &&     // 0x00508ab0
            jo::as_unit(unit)->anim_kind == 0x49)
            return;
        if (cmd == 0 && jo::as_unit(unit)->anim_kind == 0x49) {
            const float v = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->anim_phase());  // 0x005045f0
            if (!(v >= fconst(d::flt_100))) joan::orig::as_unit(unit)->start_paired_anim();  // 0x004533e0
            return;
        }
        i16 stance = static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_player(this)->order_icon()));  // 0x004350f0
        if (cmd < 0) stance = 0x45;
        if (cmd != 0 && stance != 0x43) {
            const bool quick = cmd == 0x14 || cmd == 0xc || cmd == 0xd;
            if (force != 0 || (jo::self<jo::Player>(this)->mode == 3 && quick)) {
                if (cmd != 0x1c && cmd != 0x22) {
                    joan::orig::as_unit(unit)->react_mount(joan::orig::arg::i32(quick ? 0u : 1u));  // 0x00470430 (reads the byte)
                    jo::self<jo::Player>(this)->mount_heading_deg = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e());  // 0x0050ac00
                    return;
                }
                // falls to the target-state check below
            } else {
                if (stance == 0x44) {
                    joan::orig::as_unit(unit)->react_mount(1);
                } else if (stance == 0x45) {
                    joan::orig::as_unit(unit)->react_mount(0);
                } else if (stance == 0x46) {
                    joan::orig::as_unit(unit)->hit_effect_466d50();  // 0x00466d50
                }
                jo::self<jo::Player>(this)->mount_heading_deg = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e());
                return;
            }
        }
        const i32 ts = jo::as_unit(target)->anim_kind;
        if (ts == 0xb || ts == 9 || ts == 0xa || ts == 0x10 || cmd == 0) joan::orig::as_unit(unit)->start_paired_anim();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Order::apply_43d7a0", 0x0043d7a0, &H::order_apply);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01_2 {

using u32 = std::uint32_t;

void stop_unit(void* unit) { joan::orig::as_unit(unit)->play_idle(0, 1); }      // 0x00508b90

struct H {
    // ------------------------------------------------------------------------ 0x0043da10
    void finish_unit(u8* unit) {
        if (jo::view<i32>(jo::as_unit(unit)->name_index) == 0 || jo::as_unit(unit)->cell == nullptr ||
            jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->tracks) == nullptr)
            joan::orig::as_squad(unit)->clear();  // 0x00422f10
        jo::as_unit(unit)->wait_count = jo::as_unit(unit)->wait_limit;
        stop_unit(unit);
        jo::view<u32>(jo::self<jo::Order>(this)->current.kind) = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order::finish_unit", 0x0043da10, &H::finish_unit);
#undef B93

}  // namespace game_b93_01_2
}  // namespace

// ---------------------------------------- from modules/game_b106_19.cpp (game/b106_19)
namespace {
namespace game_b106_19 {

using joan::call::cdecl_call;

void* const kCamera = obj(d::g_camera_man);

void* const kRandom = obj(d::g_misc);

using u32 = std::uint32_t;

double cd(img::Addr a) { return global<double>(a); }
float fbits(u32 v) {
    float f;
    std::memcpy(&f, &v, 4);
    return f;
}
u32 bitsf(float f) {
    u32 v;
    std::memcpy(&v, &f, 4);
    return v;
}

void* const kSoldiers = obj(d::g_units);
void* const kModels = obj(d::g_physic_sys);

constexpr u32 kMinusOne = 0xbf800000u;  // -1.0f

JOAN_COUNTER(c_steer, "UnitCtl::steer_update");

// ---------------------------------------------------------------- 0x0043dab0 helpers
struct CamState {
    u8 b[0x28];
};
static_assert(sizeof(CamState) == 0x28, "CamState");

float st24(CamState& s) {
    float v;
    std::memcpy(&v, s.b + 0x10, 4);
    return v;
}
void set_st24(CamState& s, float v) { std::memcpy(s.b + 0x10, &v, 4); }

// fild qword (u32 0xa500b6) * c24 * x, each product rounded at PC24.
float time_scale(float x) {
    const Fpu fpu{true};
    const double g = static_cast<double>(global<u32>(d::g_frame_elapsed_ms));
    return static_cast<float>(fpu.mul(fpu.mul(g, static_cast<double>(cf(d::flt_0_000034906585))), static_cast<double>(x)));
}

// Common tail: optional add to the camera yaw, then wrap into range.
void yaw_add_wrap(void* self, CamState& st, float add) {
    if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_player(self)->heading_changed())) != 0) set_st24(st, st24(st) + add);  // 0x0043ef30
    if (!(st24(st) >= cf(d::flt_m3_1415927))) set_st24(st, st24(st) + cf(d::flt_6_2831855));
    if (st24(st) > cf(d::flt_3_1415927)) set_st24(st, st24(st) - cf(d::flt_6_2831855));
}

// min(fabs(x), g_a33740) as computed by the original (fabs called again when not clamped).
float clamp_abs(float x) {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(x)));  // 0x00427d60 fabs
    if (a > static_cast<double>(global<float>(d::g_turn_ang))) return global<float>(d::g_turn_ang);
    return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(x))));
}

// x87 arithmetic at the game's precision control (PC24) with the x87 exponent range: src/fp/x87.hpp.
using joan::fp::pc24_mul_m32;

// 0x0043ea91 / 0x0043eac9: the cos / sin wrappers 0x00425670 / 0x004256a0 return the CRT's st0, which the
// original multiplies by the float `len` under PC24 (`fmul dword`) before `fstp dword`: the wrapper result as
// the double its facade returns, times len rounded to 24 bits with the x87 exponent range (pc24_mul_m32), then
// stored as a float (the cast: fstp dword).
float trig_fmul(double st0, const float* m) { return static_cast<float>(pc24_mul_m32(st0, m)); }

float angle_diff(void* a_obj, void* b_obj) {
    u32 tmp = 0;
    void* const r = joan::orig::arg::as<void*>(joan::orig::as_turn(a_obj)->sub(&tmp, joan::orig::arg::mptr(b_obj)));  // 0x005775b0
    return joan::orig::arg::as<float>(joan::orig::as_angle(r)->degrees());                            // 0x00577450
}

struct H {
    // ------------------------------------------------------------------------ 0x0043dab0
    void steer_update(u32 a1, i32 a2, float a3, i32 a4) {
        c_steer.hit();
        if (a4 != 0) return;
        if (joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x0041b3c0
        if (joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id))) != 0) return;
        u8* const mount = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(unit)->mount_id))));
        if (static_cast<i32>(jo::view<i8>(jo::as_unit(unit)->mounted_flag)) == 0) return;
        {
            const i32 t = jo::as_unit(unit)->anim_kind;
            if (t == 0x4f || t == 0x50 || t == 0x51 || t == 0x52) return;
            const i32 mt = jo::as_unit(mount)->anim_kind;
            if (mt == 0x3e || mt == 0x3f) return;
        }
        CamState st;
        {
            const i32 mode = joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target());           // 0x00563dc0
            void* const cs = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamera)->motion(joan::orig::arg::i32(mode)));    // 0x00563c80
            joan::orig::as_camera_motion(cs)->end(&st);                                  // 0x00423fe0
        }
        {
            const i32 mt = jo::as_unit(mount)->anim_kind;
            if (mt == 1 || mt == 2 || mt == 3) {
                if (jo::self<jo::Player>(this)->mode == 3 && static_cast<i32>(global<i8>(d::g_a5802c)) == 1 && a2 == 0 &&
                    a1 != kMinusOne)
                    mounted_steer(unit, st, a1);
                return;
            }
        }
        if (static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x18) {
            u32 dir = 0;  // 0x00576120 angle object (one byte used)
            joan::orig::as_angle(&dir)->construct();  // 0x00576120
            if (a1 != kMinusOne) {
                const float v = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&st, joan::orig::arg::f32(a1)));  // 0x0042fe30
                joan::orig::as_angle(&dir)->set_byte(joan::orig::arg::f32(v));                               // 0x005762c0
            } else {
                joan::orig::as_angle(&dir)->set_byte(joan::orig::arg::f32(u32{0}));
            }
            (void)joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(unit)->actor)->record)));  // 0x006af310
            if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones))->zone_free())) == 0 &&
                a2 != 0 && a1 != kMinusOne && joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target()) != 3) {
                const float v = joan::orig::arg::as<float>(joan::orig::as_angle(&dir)->byte_to_float());  // 0x00576300
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(v));                      // 0x0050abd0
                if (joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target()) == 0) yaw_from_a1(st, fbits(a1));
            }
        }
        if (static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x1a && a3 > cf(d::flt_180)) {
            u32 dir = 0;
            joan::orig::as_angle(&dir)->construct();  // 0x00576120
            if (a1 != kMinusOne) {
                const float v = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&st, joan::orig::arg::f32(a1)));  // 0x0042fe30
                joan::orig::as_angle(&dir)->set_byte(joan::orig::arg::f32(v));                               // 0x005762c0
            } else {
                joan::orig::as_angle(&dir)->set_byte(joan::orig::arg::f32(u32{0}));
            }
            u8* const m68 = joan::orig::arg::as<u8*>(joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(unit)->actor)->record))));  // 0x006af310
            if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones))->zone_free())) == 0 &&
                a2 != 0 && a1 != kMinusOne && joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target()) != 3) {
                const float v = joan::orig::arg::as<float>(joan::orig::as_angle(&dir)->byte_to_float());  // 0x00576300
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(v));                      // 0x0050abd0
                if (joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target()) == 0) yaw_from_a1(st, fbits(a1));
            }
            const float k1 = static_cast<float>(static_cast<i32>(static_cast<u32>(a2) * 0x578u));
            jo::as_shot(m68)->state.move_target[0] = joan::orig::arg::as<float>(joan::orig::as_angle(&dir)->cos()) * k1;  // 0x00576ac0
            const float k2 = static_cast<float>(static_cast<i32>(static_cast<u32>(a2) * 0x578u));
            jo::as_shot(m68)->state.move_target[1] = joan::orig::arg::as<float>(joan::orig::as_angle(&dir)->sin()) * k2;  // 0x005768d0
        }
        if (a2 != 0 && a1 != kMinusOne && static_cast<i32>(jo::as_unit(unit)->anim_kind) == 0x1a) {
            const float v = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&st, joan::orig::arg::f32(a1)));  // 0x0042fe30
            joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(v));                               // 0x0050abd0
            if (static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(jo::as_unit(unit)->cell)->camera_zones))->zone_free())) == 0 &&
                joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target()) == 0)
                yaw_from_a1(st, fbits(a1));
        }
        if (static_cast<i32>(jo::as_unit(unit)->action) == 1) {
            u8* const m74 = joan::orig::arg::as<u8*>(joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(unit)->actor)->record))));  // 0x006af310
            if (jo::as_shot(m74)->state.vec0_z > cf(d::k_9cf5c4)) {
                jo::self<jo::Player>(this)->turn = 1;
            } else if (!(jo::as_shot(m74)->state.vec0_z > cf(d::k_9cf5c4)) && static_cast<i32>(jo::self<jo::Player>(this)->turn) == 1 &&
                       static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->can_act())) != 0) {  // 0x00500ba0
                jo::as_unit(unit)->armour_flags[0] = 0;
                jo::as_unit(unit)->armour_flags[1] = 0;
                jo::as_unit(unit)->armour_flags[2] = 0;
                jo::as_unit(unit)->armour_flags[3] = 0;
                jo::as_unit(unit)->armour_flags[4] = 0;
                jo::as_unit(unit)->armour_flags[5] = 0;
                if (static_cast<i32>(jo::as_unit(unit)->combo_state) != -1) jo::view<u8>(jo::as_unit(unit)->combo_state) = 0;
                jo::view<u16>(jo::as_unit(unit)->combo_queued) = 0;
                joan::orig::as_unit(unit)->set_action(i32{6}, i32{0}, i32{1});  // 0x00501190
                jo::self<jo::Player>(this)->turn = 2;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target()) == 0) {
            void* const cs = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamera)->motion(i32{0}));  // 0x00563c80
            joan::orig::as_camera_motion(cs)->retarget(&st);                                   // 0x00423f40
        }
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(mount)->can_act())) == 0) return;  // 0x00500ba0
        if (a2 != 0 && a1 != kMinusOne) {
            if (static_cast<i32>(jo::as_unit(unit)->action) == 1) {
                if (static_cast<i32>(jo::self<jo::Player>(this)->turn) == 2) riding_turn(unit, st, a1);
            } else {
                if (a1 == 0x43870000u && jo::self<jo::Player>(this)->mode == 3) return;  // 270.0f
                walking_turn(unit, st, a1);
            }
        }
        jo::self<jo::Player>(this)->mount_heading = jo::as_unit(unit)->heading_goal * cf(d::flt_0_017453292);
    }

    // 0x0043dab0 part: block at 0x0043dc1c (mount types 1..3, camera mode 3).
    void mounted_steer(u8* unit, CamState& st, u32 a1) {
        const float f38 = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&st, joan::orig::arg::f32(a1)));              // 0x0042fe30
        const float f3c = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading()) / cf(d::flt_0_017453292);       // 0x004ff850
        u32 a40 = 0, a44 = 0;
        joan::orig::as_angle(&a40)->construct_577190();       // 0x00577190
        joan::orig::as_angle(&a40)->set_degrees(joan::orig::arg::f32(f38));  // 0x005773e0
        joan::orig::as_angle(&a44)->construct_577190();
        joan::orig::as_angle(&a44)->set_degrees(joan::orig::arg::f32(f3c));
        float f48 = angle_diff(&a44, &a40);
        if (!(f48 >= cf(d::k_9cf5c4))) f48 = f48 + cf(d::flt_360);
        float f4c = f3c;
        if (f48 > cf(d::k_9cf5c4) && !(f48 >= cf(d::flt_360))) {
            if (!(f48 >= cf(d::flt_180))) {
                const float d = clamp_abs(f48);
                f4c = f3c - d;
            } else {
                const float d = clamp_abs(cf(d::flt_360) - f48);
                f4c = f3c + d;
            }
        }
        joan::orig::as_angle(&a40)->set_degrees(joan::orig::arg::f32(f4c));
        joan::orig::as_angle(&a44)->set_degrees(joan::orig::arg::f32(jo::view<u32>(jo::self<jo::Player>(this)->mount_heading_deg)));
        f48 = angle_diff(&a44, &a40);
        if (!(f48 >= cf(d::k_9cf5c4))) f48 = f48 + cf(d::flt_360);
        if (f48 > cf(d::flt_15) && !(f48 >= cf(d::flt_180))) {
            f4c = jo::self<jo::Player>(this)->mount_heading_deg - cf(d::flt_15);
        } else if (f48 > cf(d::flt_180) && !(f48 >= cf(d::flt_345))) {
            f4c = jo::self<jo::Player>(this)->mount_heading_deg + cf(d::flt_15);
        }
        joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(f4c));  // 0x0050abd0
    }

    // 0x0043df27-style block: yaw delta from a1, scaled by the frame time, added to the camera yaw.
    void yaw_from_a1(CamState& st, float a1f) {
        float d;
        if (!(a1f >= cf(d::flt_180)))
            d = a1f - cf(d::flt_90);
        else
            d = cf(d::flt_270) - a1f;
        d = time_scale(d);
        yaw_add_wrap(this, st, d);
    }

    // 0x0043e464 .. 0x0043e7fa.
    void walking_turn(u8* unit, CamState& st, u32 a1) {
        float f78;
        if (jo::self<jo::Player>(this)->mode == 3 && static_cast<i32>(global<i8>(d::g_a5802c)) == 0) {
            const double m = joan::orig::arg::as<double>(joan::orig::libc::fmod(joan::orig::arg::f64(static_cast<double>(fbits(a1))), joan::orig::arg::f64(360.0)));  // CRT fmod
            a1 = bitsf(static_cast<float>(m));
            const float a1f = fbits(a1);
            if (a1f > cf(d::flt_180)) {
                const float r = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading());  // 0x004ff850
                f78 = r / cf(d::flt_0_017453292) + a1f - cf(d::flt_270);
            } else {
                const float r = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading());
                f78 = r / cf(d::flt_0_017453292) + a1f - cf(d::flt_90);
            }
        } else {
            f78 = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&st, joan::orig::arg::f32(a1)));  // 0x0042fe30
        }
        const float f7c = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit)->heading()) / cf(d::flt_0_017453292);  // 0x004ff850
        u32 a80 = 0, a84 = 0;
        joan::orig::as_angle(&a80)->construct_577190();       // 0x00577190
        joan::orig::as_angle(&a80)->set_degrees(joan::orig::arg::f32(f78));  // 0x005773e0
        joan::orig::as_angle(&a84)->construct_577190();
        joan::orig::as_angle(&a84)->set_degrees(joan::orig::arg::f32(f7c));
        float f88 = angle_diff(&a84, &a80);
        if (!(f88 >= cf(d::k_9cf5c4))) f88 = f88 + cf(d::flt_360);
        if (f88 > cf(d::k_9cf5c4) && !(f88 >= cf(d::flt_360))) {
            if (!(f88 >= cf(d::flt_180))) {
                const float d = clamp_abs(f88);
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(f7c - d));  // 0x0050abd0
            } else {
                const float d = clamp_abs(cf(d::flt_360) - f88);
                joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(f7c + d));
            }
        }
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->follow_mode)) == 0 && joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamera)->target()) == 0) {
            const float a1f = fbits(a1);
            float f94 = 0.0f;
            if (jo::self<jo::Player>(this)->mode == 3) {
                if (a1f > cf(d::flt_180))
                    f94 = a1f - cf(d::flt_270);
                else
                    f94 = a1f - cf(d::flt_90);
            } else {
                if (!(a1f >= cf(d::flt_180)))
                    f94 = a1f - cf(d::flt_90);
                else
                    f94 = cf(d::flt_270) - a1f;
                f94 = time_scale(f94);
            }
            const float hi = global<float>(d::g_turn_ang) * cf(d::flt_0_017453292);
            const float lo = -global<float>(d::g_turn_ang) * cf(d::flt_0_017453292);
            f94 = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_misc(kRandom)->limit_double(joan::orig::arg::f64(static_cast<double>(f94)), joan::orig::arg::f64(static_cast<double>(lo)), joan::orig::arg::f64(static_cast<double>(hi)))));  // 0x00590b00
            yaw_add_wrap(this, st, f94);
            void* const cs = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamera)->motion(i32{0}));  // 0x00563c80
            joan::orig::as_camera_motion(cs)->retarget(&st);                                   // 0x00423f40
        }
    }

    // 0x0043e812 .. 0x0043eae6 (riding, state 2).
    void riding_turn(u8* unit, CamState& st, u32 a1) {
        const float f98 = joan::orig::arg::as<float>(joan::orig::as_player(this)->heading_to_degrees(&st, joan::orig::arg::f32(a1)));  // 0x0042fe30
        auto cur = [unit]() { return joan::orig::arg::as<float>(joan::orig::as_unit(unit)->float_1e()); };  // 0x0050ac00
        auto set = [unit](float v) { joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(v)); };      // 0x0050abd0
        auto set_raw = [unit](float v) { joan::orig::as_unit(unit)->set_heading(joan::orig::arg::f32(bitsf(v))); };
        const float c10 = cf(d::flt_180), c04 = cf(d::flt_30), c9c8 = cf(d::flt_360);
        if (!(cur() > f98)) {
            if (!(f98 - cur() > c10)) {
                if (f98 - cur() > c04) {
                    set(cur() + c04);
                } else {
                    set_raw(f98);
                }
            } else {
                const float ff0 = f98 + c9c8;
                if (ff0 - cur() > c04) {
                    set(cur() - c04);
                } else {
                    set_raw(f98);
                }
            }
        } else {
            if (!(cur() - f98 > c10)) {
                if (cur() - f98 > c04) {
                    set(cur() - c04);
                } else {
                    set_raw(f98);
                }
            } else {
                if ((cur() - f98) + c9c8 > c04) {
                    set(cur() + c04);
                } else {
                    set_raw(f98);
                }
            }
        }
        if (!(cur() >= cf(d::k_9cf5c4))) {
            set(cur() + c9c8);
        } else if (cur() > c9c8) {
            set(cur() - c9c8);
        }
        u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_players(kModels)->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(unit)->actor)->record))));  // 0x006af310
        const float sq = jo::view<float>(jo::as_shot(m)->state.vec0_x) * jo::view<float>(jo::as_shot(m)->state.vec0_x) + jo::as_shot(m)->state.vec0_y * jo::as_shot(m)->state.vec0_y;
        const float len = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sq)))));  // sqrt
        const Fpu fpu{true};
        {
            const double r = static_cast<double>(cur());
            const float ang = static_cast<float>(fpu.div(fpu.mul(r, cd(d::dbl_3_141592653589793)), cd(d::dbl_9cfbe8)));
            jo::view<float>(jo::as_shot(m)->state.vec0_x) = trig_fmul(joan::orig::arg::as<double>(joan::orig::libc::cos_f(joan::orig::arg::f32(ang))), &len);  // 0x00425670 cos, fmul len
        }
        {
            const double r = static_cast<double>(cur());
            const float ang = static_cast<float>(fpu.div(fpu.mul(r, cd(d::dbl_3_141592653589793)), cd(d::dbl_9cfbe8)));
            jo::as_shot(m)->state.vec0_y = trig_fmul(joan::orig::arg::as<double>(joan::orig::libc::sin_f(joan::orig::arg::f32(ang))), &len);  // 0x004256a0 sin, fmul len
        }
        jo::self<jo::Player>(this)->turn = 0;
    }
};

#define B106(name, addr, fn) JOAN_REPLACE("game/b106_19", name, addr, fn)
B106("UnitCtl::steer_update", 0x0043dab0, &H::steer_update);
#undef B106

}  // namespace game_b106_19
}  // namespace

// ---------------------------------------- from modules/game_b88_04.cpp (game/b88_04)
namespace {
namespace game_b88_04 {

using joan::orig::helpers::addr_of;

JOAN_COUNTER(c_unlink_place, "Place::unlink_owner");
JOAN_COUNTER(c_unlink_unit, "Unit::unlink_owner");

struct H {
    // ------------------------------------------------------------ 0x0043edb0 (ret 4)
    std::uintptr_t unlink_place(std::uint8_t* p) {
        c_unlink_place.hit();
        if (jo::view<void*>(jo::as_place(p)->model) != nullptr && jo::view<void*>(jo::as_place(p)->model->scene) != nullptr)
            joan::orig::as_scene(jo::view<void*>(jo::as_place(p)->model->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::as_place(p)->model)));  // 0x005efdb0
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_place(p)->cell);  // ebp-0xc
        if (old == nullptr) return 0;                           // new owner (ebp-0x8) is 0
        std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_place(p)->cell_prev);
        std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_place(p)->cell_next);
        if (prev != nullptr)
            jo::view<std::uint8_t*>(jo::as_place(prev)->cell_next) = next;
        else
            jo::view<std::uint8_t*>(jo::as_cell(old)->places_head) = next;
        if (next != nullptr)
            jo::view<std::uint8_t*>(jo::as_place(next)->cell_prev) = prev;
        else
            jo::view<std::uint8_t*>(jo::as_cell(old)->places_tail) = prev;
        jo::view<void*>(jo::as_place(p)->cell_prev) = nullptr;
        jo::view<void*>(jo::as_place(p)->cell_next) = nullptr;
        jo::view<void*>(jo::as_place(p)->cell) = nullptr;
        return addr_of(p);
    }

    // ------------------------------------------------------------ 0x0043eb10 (ret 4)
    std::uintptr_t unlink_unit(std::uint8_t* p) {
        c_unlink_unit.hit();
        if (jo::as_unit(p)->actor != nullptr && jo::view<void*>(jo::as_actor(jo::as_unit(p)->actor)->scene) != nullptr)
            joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::as_unit(p)->actor)->scene))->detach(joan::orig::arg::mptr(jo::as_unit(p)->actor));  // 0x005efdb0
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_unit(p)->cell);  // ebp-0xc
        if (old == nullptr) return 0;                           // new owner (ebp-0x8) is 0
        std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_unit(p)->cell_prev);
        std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_unit(p)->cell_next);
        if (prev != nullptr)
            jo::view<std::uint8_t*>(jo::as_unit(prev)->cell_next) = next;
        else
            jo::view<std::uint8_t*>(jo::as_cell(old)->units_head) = next;
        if (next != nullptr)
            jo::view<std::uint8_t*>(jo::as_unit(next)->cell_prev) = prev;
        else
            jo::view<std::uint8_t*>(jo::as_cell(old)->units_tail) = prev;
        jo::as_unit(p)->cell_prev = nullptr;
        jo::as_unit(p)->cell_next = nullptr;
        jo::as_unit(p)->cell = nullptr;
        return addr_of(p);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_04", name, addr, fn)
B("Place::unlink_owner", 0x0043edb0, &H::unlink_place);
B("Unit::unlink_owner", 0x0043eb10, &H::unlink_unit);
#undef B

}  // namespace game_b88_04
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
namespace {
namespace game_b89_05 {

JOAN_COUNTER(c_enter, "Station::enter");

struct H {
    // ------------------------------------------------------------ 0x0043ec80 (ret 4)
    std::uint8_t* station_enter(std::uint8_t* unit) {
        c_enter.hit();
        std::uint8_t* const self = reinterpret_cast<std::uint8_t*>(this);
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_place(unit)->cell);
        if (self == old) return unit;
        if (old != nullptr) {
            std::uint8_t* const a = jo::view<std::uint8_t*>(jo::as_place(unit)->cell_prev);
            std::uint8_t* const b = jo::view<std::uint8_t*>(jo::as_place(unit)->cell_next);
            if (a != nullptr) {
                jo::view<std::uint8_t*>(jo::as_place(a)->cell_next) = b;
            } else {
                jo::view<std::uint8_t*>(jo::as_cell(old)->places_head) = b;
            }
            if (b != nullptr) {
                jo::view<std::uint8_t*>(jo::as_place(b)->cell_prev) = a;
            } else {
                jo::view<std::uint8_t*>(jo::as_cell(old)->places_tail) = a;
            }
            jo::as_place(unit)->cell_prev = nullptr;
            jo::as_place(unit)->cell_next = nullptr;
            jo::as_place(unit)->cell = nullptr;
        }
        if (self == nullptr) return unit;
        std::uint8_t* const next = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->places_head);  // prev is a constant 0 (dead branch)
        jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->places_head) = unit;
        if (next != nullptr) {
            jo::view<std::uint8_t*>(jo::as_place(next)->cell_prev) = unit;
        } else {
            jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->places_tail) = unit;
        }
        jo::as_place(unit)->cell_prev = nullptr;
        jo::view<std::uint8_t*>(jo::as_place(unit)->cell_next) = next;
        jo::view<std::uint8_t*>(jo::as_place(unit)->cell) = self;
        return self;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("Station::enter", 0x0043ec80, &H::station_enter);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01_2 {

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_heading, "Pair::heading_changed");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x0043ef30
    i16 heading_changed() {
        c_heading.hit();
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) return 0;
        u8* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        if (unit_missing(jo::as_unit(unit)->mount_id) != 0) return 0;
        unit_get(jo::as_unit(unit)->mount_id);
        const float h = jo::as_unit(unit)->heading_goal * global<float>(d::flt_0_017453292);
        if (!(h < jo::self<jo::Player>(this)->mount_heading || h > jo::self<jo::Player>(this)->mount_heading)) return 0;  // equal or NaN
        return 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Pair::heading_changed", 0x0043ef30, &H::heading_changed);
#undef B96

}  // namespace game_b96_01_2
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01_2 {

using joan::call::cdecl_call;

using joan::hook::x87::ftol;

void* const kCams = obj(d::g_camera_man);

using u32 = std::uint32_t;

using joan::orig::helpers::fbits;
u32 bitsf(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}

struct FVec {
    float x, y, z;
};
static_assert(sizeof(FVec) == 12, "by-value 12-byte argument");

JOAN_COUNTER(c_cam_43f000, "Camera::terrain_clamp_43f000");

constexpr img::Addr kUnits = d::g_units;

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8 unit_alive(void* rec) { return joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->can_act()); }                // 0x00500ba0
void* cam_entry(u32 i) { return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCams)->motion(joan::orig::arg::i32(i))); }        // 0x00563c80

FVec vsub(const void* a, const void* b) {  // 0x0060cc90 (out, a, b) -> *out
    FVec t;
    return *joan::orig::arg::as<FVec*>(joan::orig::vec_sub(&t, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}

struct H {
    // ------------------------------------------------------------ 0x0043f000
    void cam_43f000(u32) {
        c_cam_43f000.hit();
        if (unit_dead(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) return;
        u8* rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        if (unit_alive(rec) == 0) return;
        {
            const i16 k = jo::as_unit(rec)->anim_kind;
            if (k == 0x51 || k == 0x52 || k == 0x4f || k == 0x50) return;
        }
        const Fpu fpu{true};
        float e = joan::orig::arg::as<float>(joan::orig::as_player(this)->look_sensitivity());  // 0x004322d0
        e = e + cf(d::flt_1);
        e = e * cf(d::flt_0_7);
        e = e + cf(d::flt_0_2);
        const float e2 = e * cf(d::flt_0_005);
        const float f10 = static_cast<float>(fpu.mul(e2, static_cast<double>(global<i32>(d::g_frame_elapsed_ms))));
        const i32 v14 = ftol(fpu.mul(static_cast<double>(global<i32>(d::g_mouse_delta_x)), f10));
        u32 cam[10];  // [ebp-0x3c]: +4 position, +0x10 heading, +0x14 pitch, +0x18 / +0x1c
        {
            const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kCams)->target());  // 0x00563dc0
            joan::orig::as_camera_motion(cam_entry(idx))->end(joan::orig::arg::mptr(cam));    // 0x00423fe0
        }
        if (static_cast<i8>(global<u8>(d::g_a5802c)) == 1) {
            cam[4] = jo::view<u32>(jo::as_unit(rec)->heading);
        } else if (v14 != 0) {
            u32 a40, a44, a48, e4;
            joan::orig::as_angle(&a40)->construct_577190();                       // 0x00577190
            joan::orig::as_angle(&a40)->set_wrapped(joan::orig::arg::f32(jo::view<u32>(jo::as_unit(rec)->heading)));   // 0x00577340
            joan::orig::as_angle(&a44)->construct_577190();
            const float h = static_cast<float>(
                fpu.sub(fbits(cam[4]), fpu.mul(static_cast<double>(v14), cf(d::flt_0_017453292))));
            joan::orig::as_angle(&a44)->set_wrapped(joan::orig::arg::f32(h));
            a48 = *joan::orig::arg::as<u32*>(joan::orig::as_turn(&a44)->sub(&e4, &a40));  // 0x005775b0
            if (joan::orig::arg::as<float>(joan::orig::as_angle(&a48)->degrees()) > cf(d::g_a5802e)) {  // 0x00577450
                const float r = joan::orig::arg::as<float>(joan::orig::as_angle(&a48)->degrees());
                const float lim = cf(d::flt_360) - cf(d::g_a5802e);
                if (lim > r) {
                    if (v14 < 0) {
                        const float g = cf(d::g_a5802e) * cf(d::flt_0_017453292);
                        joan::orig::as_angle(&a44)->set_wrapped(joan::orig::arg::f32(g + jo::as_unit(rec)->heading));
                    } else {
                        const float g = cf(d::g_a5802e) * cf(d::flt_0_017453292);
                        joan::orig::as_angle(&a44)->set_wrapped(joan::orig::arg::f32(jo::as_unit(rec)->heading - g));
                    }
                }
            }
            cam[4] = bitsf(joan::orig::arg::as<float>(joan::orig::as_angle(&a44)->to_radians()));  // 0x005773b0
        }
        if (global<i32>(d::g_mouse_delta_y) != 0) {
            const u32 v4c = cam[5];
            const float sgn = static_cast<i8>(global<u8>(d::g_cfg_invert_look)) != 0 ? fbits(0xbf800000) : fbits(0x3f800000);
            const i32 t = ftol(fpu.div(fpu.mul(static_cast<double>(global<i32>(d::g_mouse_delta_y)), f10), cf(d::flt_2)));
            double x = fpu.mul(static_cast<double>(t), cf(d::flt_0_017453292));
            x = fpu.mul(x, sgn);
            x = fpu.add(x, fbits(cam[5]));
            cam[5] = bitsf(static_cast<float>(x));
            if (fbits(cam[5]) > cf(d::flt_0_61086524)) cam[5] = 0x3f1c61aa;
            if (!(fbits(cam[5]) >= cf(d::flt_m0_34906584))) cam[5] = 0xbeb2b8c2;
            if (global<i32>(d::g_mouse_delta_y) < 0) {
                u32 m[16];
                joan::orig::orbit_matrix(joan::orig::arg::mptr(m), joan::orig::arg::f32(cam[4]), joan::orig::arg::f32(cam[5]), joan::orig::arg::f32(cam[6]), joan::orig::arg::f32(0u));  // 0x0060bd00
                FVec dir;
                joan::orig::matrix_axis_z(&dir, joan::orig::arg::ptr(m));                          // 0x0060c220
                FVec t108;
                const FVec sc = *joan::orig::arg::as<FVec*>(joan::orig::vec_scale(&t108, &dir, joan::orig::arg::f32(cam[7])));  // 0x0060cd00
                const FVec p = vsub(&cam[1], &sc);
                float box[6] = {p.x, p.y, p.z, p.x, p.y, p.z};
                u32 cnt = 0;
                void* const* const rr = joan::orig::arg::as<void* const*>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(joan::orig::arg::mptr(box), &cnt));  // 0x00429090: a list of cell pointers
                if (cnt != 0) {
                    u8* ts = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(*rr)->grid(joan::orig::arg::i16(0u)));  // 0x005600c0
                    if (ts != nullptr) {
                        const float d0 = static_cast<float>(
                            fpu.div(fpu.sub(box[0], static_cast<double>(jo::as_cell(*rr)->origin_x)),
                                    static_cast<double>(jo::as_path_space(ts)->cell_w)));
                        const float d4 = static_cast<float>(
                            fpu.div(fpu.sub(box[1], static_cast<double>(jo::as_cell(*rr)->origin_y)),
                                    static_cast<double>(jo::as_path_space(ts)->cell_h)));
                        const i32 i0 = ftol(d0);
                        const i32 i1 = ftol(d4);
                        const u32 even = ((static_cast<u32>(i0) + static_cast<u32>(i1)) & 1u) == 0 ? 1u : 0u;
                        const float hgt = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::as_path_space(ts)->grid)->height_4c7b70(joan::orig::arg::f32(d0), joan::orig::arg::f32(d4), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70
                        if (hgt > p.z) cam[5] = v4c;  // [ebp-0xa0]: the copied p.z (box[2] equal, box read-only)
                    }
                }
            }
        }
        joan::orig::as_camera_motion(cam_entry(0))->retarget(joan::orig::arg::mptr(cam));  // 0x00423f40
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Camera::terrain_clamp_43f000", 0x0043f000, &H::cam_43f000);
#undef B

}  // namespace game_b104_01_2
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world2)
namespace {
namespace game_world_2 {

struct H {
    // 0x0043f4e0 Cell::free_textures(boxes, count): objects (+0x6c, count +0x64) whose box
    // misses the first box (the original passes the first box every time).
    void cell_free_textures(const void* boxes, std::int32_t count) {
        for (std::int32_t i = 0; i < jo::self<jo::Cell>(this)->sector_count; ++i) {
            std::uint8_t* const obj = jo::view<std::uint8_t**>(jo::self<jo::Cell>(this)->sectors)[i];
            std::int32_t k = 0;
            for (; k < count; ++k)
                if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_box(jo::as_scene_s(obj)->desc)->overlaps(joan::orig::arg::mptr(boxes))) & 0xff) != 0) break;  // 0x0060d6b0 (x86: obj + 8)
            if (k < count) continue;
            // The three objects are Planes (SceneS::plane / plane2, Actor::plane); 0x004191e0 (facade Texture::release)
            // reads Plane +0x90..+0x98, so the byte at +5 is Plane::buffer_mode.
            auto release = [](void* t) {
                if (t != nullptr && static_cast<std::int8_t>(jo::as_plane(t)->buffer_mode) == 0) joan::orig::as_texture(t)->release();  // 0x004191e0
            };
            release(jo::view<void*>(jo::as_scene_s(obj)->plane));
            release(jo::view<void*>(jo::as_scene_s(jo::view<std::uint8_t**>(jo::self<jo::Cell>(this)->sectors)[i])->plane2));
            void* const model = jo::view<void*>(jo::as_scene_s(jo::view<std::uint8_t**>(jo::self<jo::Cell>(this)->sectors)[i])->object);
            if (model != nullptr) release(jo::view<void*>(jo::as_actor(model)->plane));
        }
    }
};

#define W2(name, addr, fn) JOAN_REPLACE("game/world2", name, addr, fn)
W2("Cell::free_textures", 0x0043f4e0, &H::cell_free_textures);
#undef W2

}  // namespace game_world_2
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01_2 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x0043f610", 0x0043f610, &init_wrapper<fn::init_pi_a33710>);
B("init_0x0043f670", 0x0043f670, &init_wrapper<fn::init_a33714>);
B("init_0x0043f6c0", 0x0043f6c0, &init_wrapper<fn::init_a33718>);
B("init_0x0043f710", 0x0043f710, &init_wrapper<fn::init_a3371c>);
B("init_0x0043f760", 0x0043f760, &init_wrapper<fn::init_43f780>);
#undef B

}  // namespace game_b105_01_2
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01_2 {

using joan::call::cdecl_call;

static void JOAN_CDECL init_pi_a33710() {                                                  // 0x0043f630
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a33710) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0 (exact)
}

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("init_pi_a33710", 0x0043f630, &init_pi_a33710);
#undef B99

}  // namespace game_b99_01_2
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01_2 {

using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a33714() { div_global(d::g_a33714, d::g_a33710, d::flt_2); }  // 0x0043f690 (/2)

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_a33714", 0x0043f690, &init_a33714);
#undef B98

}  // namespace game_b98_01_2
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01_2 {

// g_out = g_in / g_div (fld dword / fdiv dword / fstp dword).
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a33718() { div_global(d::g_a33718, d::g_a33710, d::flt_3); }  // 0x0043f6e0

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("init_a33718", 0x0043f6e0, &init_a33718);
#undef B94

}  // namespace game_b94_01_2
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01_2 {

using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a3371c() { div_global(d::g_a3371c, d::g_a33710, d::flt_4); }  // 0x0043f730 (/4)

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_a3371c", 0x0043f730, &init_a3371c);
#undef B97

}  // namespace game_b97_01_2
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01_2 {

// 0x0043f780: [0xa33724] = packed colour (0xff, 0xff, 0xff, 0xcc).
void JOAN_CDECL init_43f780() {
    std::uint32_t tmp = 0;  // ebp-0x4, written by the ctor
    const std::uint32_t* const c =
        joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&tmp)->construct(0xffu, 0xffu, 0xffu, 0xccu));  // 0x0042a260
    global<std::uint32_t>(d::g_a33724) = *c;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_43f780", 0x0043f780, &init_43f780);
#undef B

}  // namespace game_b100_01_2
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (game/b111_16)
//
//      0x0043a7d0 Ctrl::set_target_43a7d0     (thiscall(id, mode, unused), ret 0xc) -> ax.
//                 Old reason ("mode 2 at 0x0043b165") was a misread: 2 is the unused third
//                 argument. Every caller passes mode 0 / 1 or +0xd7 (only ever written with
//                 0 / 1 / a copy). Any other mode runs the untouched original body.
//  * module fix/uninit (installed only with [Fixes] UninitReads=1):
//
//  * batch 118/03 (stage S5 maps no original body): with module fix/uninit enabled
//    (joan::hook::module_enabled("fix/uninit"), read on the first call) the original-body case
//    above runs the C++ version with the never-written value set to its neutral value instead:
//      0x0043a7d0 mode not 0 / 1: target vector 0 (neither lookup runs), stored mode as passed;
//    With fix/uninit off (mapped mode) the original body still runs.
namespace {
namespace game_b111_16 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Vec {
    float x, y, z;
};
static_assert(sizeof(Vec) == 12, "vec3 by value");

void* const kUnits = obj(d::g_units);
void* const kObjects = obj(d::g_items);

JOAN_COUNTER(c_set_target, "Ctrl::set_target_43a7d0");

struct H {
    // -------------------------------------------------------------- 0x0043a7d0
    i16 set_target(i32 id, i32 mode, i32 unused) {
        if (mode != 0 && mode != 1 && !fix_uninit()) {
            // Never passed by the game (see header); keeps the original's uninitialised Vec3 path.
            return static_cast<i16>(thiscall<u32>(fn::Ctrl_set_target_43a7d0_body, this, id, mode, unused));
        }
        c_set_target.hit();
        if (id == 0) return 0;
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return 0;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x0041b3c0
        if (jo::view<i32>(jo::self<jo::Player>(this)->goto_time) == 2) return 0;
        Vec v{};
        if (mode == 0) {
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)))->dead()) != 0) return 0;  // 0x0042d0e0
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id));
            joan::orig::as_unit(u)->position(&v);  // 0x004fedb0
        } else if (mode == 1) {
            if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
            void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
            joan::orig::as_map_object(o)->position(&v);                            // 0x00499930
        }  // other modes (fix/uninit only): the never-written target vector [ebp-0x14] is 0
        if (joan::orig::arg::as<i16>(joan::orig::as_player(this)->move_to(v)) == 0) return 0;  // 0x0043a580
        jo::self<jo::Player>(this)->goto_target_ = id;
        jo::self<jo::Player>(this)->goto_kind = mode;
        const i32 hp = jo::as_unit(unit)->mana;
        if (jo::view<i32>(jo::self<jo::Player>(this)->goto_time) == 1 && hp >= global<i32>(d::g_unit_egatk_mp)) {
            jo::view<i32>(jo::self<jo::Player>(this)->goto_time) = 2;
            joan::orig::as_unit(unit)->set_145(joan::orig::arg::i32(jo::as_unit(unit)->mana - global<i32>(d::g_unit_egatk_mp)));  // 0x004fe5d0
        } else {
            jo::view<i32>(jo::self<jo::Player>(this)->goto_time) = 1;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_16", name, addr, fn)
B("Ctrl::set_target_43a7d0", 0x0043a7d0, &H::set_target);
#undef B

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Player::enter_mode2() -> void { joan::bind::member<&::game_b94_01::H::reset_db>(this); }  // 0x438ad0
auto joan::orig::Unit::stop_438b00() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::stop_438b00>(this)); }  // 0x438b00
auto joan::orig::Unit::input_438b30() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b108_10::H::input_438b30>(this)); }  // 0x438b30
auto joan::orig::Unit::link_to(std::int32_t a0) -> void { joan::bind::member<&::game_b86_05::H::link_to>(this, a0); }  // 0x438f80
auto joan::orig::Player::tick_mode2() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_17::H::update_439090>(this)); }  // 0x439090
auto joan::orig::Player::draw_mode2() -> void { joan::bind::member<&::game_b82_18::H::empty>(this); }  // 0x4390f0
auto joan::orig::Player::pick_object(std::uint32_t a0, std::uint32_t a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b106_20::H::pick_439110>(this, a0, a1, a2)); }  // 0x439110
auto joan::orig::Map::load_res(const char* a0, void* a1, std::uint32_t a2, std::uint32_t a3) -> void { joan::bind::member<&::game_b84_05::H::map_load_res>(this, a0, a1, a2, a3); }  // 0x439ff0
auto joan::orig::Player::pick_ray(std::int32_t a0, std::int32_t a1, float a2, float a3, float a4, void* a5) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b96_01::H::pick_ray>(this, a0, a1, a2, a3, a4, a5)); }  // 0x43a3b0
auto joan::orig::Trigger::timer_expired() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b98_01::H::timer_expired>(this)); }  // 0x43a530
auto joan::orig::Player::goto_object(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_12::H::try_set_43a650>(this, a0)); }  // 0x43a650
auto joan::orig::Player::stub_zero_43a720(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_01::H::zero16_43a720>(this, a0)); }  // 0x43a720
auto joan::orig::Player::try_kind_1e(std::uint32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b108_11::H::try_1e>(this, a0)); }  // 0x43a740
auto joan::orig::Ctrl::set_target_43a7d0(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b111_16::H::set_target>(this, a0, a1, a2)); }  // 0x43a7d0
auto joan::orig::Order::destruct_45() -> void { joan::bind::member<&::game_b106_15::H::dtor_43a980>(this); }  // 0x43a980
auto joan::orig::Player::goto_target(std::int32_t a0, std::int32_t a1) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b108_19::H::goto_target>(this, a0, a1)); }  // 0x43a9e0
auto joan::orig::TrackNode::release() -> void { joan::bind::member<&::game_b100_01::H::release_43ab30>(this); }  // 0x43ab30
auto joan::orig::Player::goto_object_43abd0(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::goto_object>(this, a0)); }  // 0x43abd0
auto joan::orig::Player::reset_goto() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_05::H::reset_43ac60>(this)); }  // 0x43ac60
auto joan::orig::Cmd::follow_target_43acb0() -> void { joan::bind::member<&::game_b111_09::H::follow_target>(this); }  // 0x43acb0
auto joan::orig::Cell::attach_unit(void* a0) -> void { joan::bind::member<&::game_units5::H::attach_unit>(this, a0); }  // 0x43b190
auto joan::orig::Cmd::check_object_reach() -> void { joan::bind::member<&::game_b97_01::H::check_object_reach>(this); }  // 0x43b2d0
auto joan::orig::Order::consume_43b490(std::uint32_t a0) -> void { joan::bind::member<&::game_b104_01::H::consume_43b490>(this, a0); }  // 0x43b490
auto joan::orig::UnitDraw::draw() -> void { joan::bind::member<&::game_world::H::unit_draw>(this); }  // 0x43b890
auto joan::orig::Unit::use_spiritual_item_43b990(std::int32_t a0) -> void { joan::bind::member<&::game_b95_01::H::use_spiritual_item>(this, a0); }  // 0x43b990
auto joan::orig::Action::not_kind_53(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_01::H::not_kind_53>(this, a0)); }  // 0x43bbc0
auto joan::orig::Order::fire_43bc50() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_01::H::fire_43bc50>(this)); }  // 0x43bc50
auto joan::orig::Order::spawn_effect() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_01::H::spawn_effect>(this)); }  // 0x43bd40
auto joan::orig::Unit::try_link_43be40() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_04::H::try_link_43be40>(this)); }  // 0x43be40
auto joan::orig::Unit::recharge_43bef0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b107_08::H::recharge>(this)); }  // 0x43bef0
auto joan::orig::Unit::start_6c0310_43bfe0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_19::H::start_6c0310_43bfe0>(this)); }  // 0x43bfe0
auto joan::orig::Order::alencon_43c080() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_01::H::alencon_43c080>(this)); }  // 0x43c080
auto joan::orig::Unit::dtor_43c1c0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_01::H::dtor_43c1c0>(this)); }  // 0x43c1c0
auto joan::orig::Player::link_nearest_enemy() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_01::H::spawn_and_reset>(this)); }  // 0x43c210
auto joan::orig::Table::at_672(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_03::H::at_672>(this, a0)); }  // 0x43c3c0
auto joan::orig::Table::at_676(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_03::H::at_676>(this, a0)); }  // 0x43c3f0
auto joan::orig::List::empty() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_01::H::list_empty>(this)); }  // 0x43c420
auto joan::orig::List::get_e(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::list_get_wrap>(this, a0)); }  // 0x43c450
auto joan::orig::Player::store_heading() -> void { joan::bind::member<&::game_b95_01::H::store_heading>(this); }  // 0x43c480
auto joan::orig::Player::reset_mount() -> void { joan::bind::member<&::game_b85_06::H::reset_43c500>(this); }  // 0x43c500
auto joan::orig::Player::dispatch_keys(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b108_11::H::dispatch>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x43c5c0
auto joan::orig::Player::draw_mounted() -> void { joan::bind::member<&::game_b82_18::H::empty>(this); }  // 0x43c7b0
auto joan::orig::Player::try_mount(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b96_01::H::try_attack>(this, a0)); }  // 0x43c7d0
auto joan::orig::Player::try_dismount(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b100_01::H::try_dismount>(this, a0)); }  // 0x43c920
auto joan::orig::Unit::promotion_update_43ca90(std::int32_t a0, std::int32_t a1) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b111_13::H::promotion_update>(this, a0, a1)); }  // 0x43ca90
auto joan::orig::Order::apply_43d7a0(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b108_15::H::order_apply>(this, a0, a1); }  // 0x43d7a0
auto joan::orig::Order::finish_unit(void* a0) -> void { joan::bind::member<&::game_b93_01_2::H::finish_unit>(this, a0); }  // 0x43da10
auto joan::orig::Player::steer_update(std::uint32_t a0, std::int32_t a1, float a2, std::int32_t a3) -> void { joan::bind::member<&::game_b106_19::H::steer_update>(this, a0, a1, a2, a3); }  // 0x43dab0
auto joan::orig::Unit::unlink_owner(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_04::H::unlink_unit>(this, a0)); }  // 0x43eb10
auto joan::orig::Cell::attach_place(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_05::H::station_enter>(this, a0)); }  // 0x43ec80
auto joan::orig::Place::unlink_owner(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b88_04::H::unlink_place>(this, a0)); }  // 0x43edb0
auto joan::orig::Player::heading_changed() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b96_01_2::H::heading_changed>(this)); }  // 0x43ef30
auto joan::orig::Camera::terrain_clamp_43f000(std::uint32_t a0) -> void { joan::bind::member<&::game_b104_01_2::H::cam_43f000>(this, a0); }  // 0x43f000
auto joan::orig::Cell::free_textures(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_world_2::H::cell_free_textures>(this, a0, a1); }  // 0x43f4e0
