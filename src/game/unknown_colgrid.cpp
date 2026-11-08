// game/unknown_colgrid.cpp: functions whose original source file is unknown (formerly game/_unplaced/colgrid.cpp;
// re/attribution.csv: between ww/enphysic/collimgr.cpp and ww/enphysic/collis_b.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

using joan::orig::helpers::v3d_call;

// ---------------------------------------------------------------- 0x0068de40 helpers
struct Box {  // min x, y, z, max x, y, z (0x0060d430 point box)
    float v[6];
};

using joan::call::import_call;


struct Vec3 {
    float x, y, z;
};

using joan::hook::x87::ftol;

}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ memset ctors
    H* clear_584() {  // 0x0068db10
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::Collisions)));  // 0x584 bytes on x86
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::clear_584", 0x0068db10, &H::clear_584);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

struct H {
    // ------------------------------------------------------------------ 0x0068db50
    u32 call_68dbc0() {
        joan::orig::as_collision_grid(this)->clear();  // 0x0068dbc0
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("Obj::call_68dbc0", 0x0068db50, &H::call_68dbc0);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

struct H {
    H* init_68db80(jo::DynArray* v) {  // 0x0068db80
        jo::self<jo::Collisions>(this)->objects = v;
        jo::view<u32>(jo::self<jo::Collisions>(this)->zone_count) = 0;
        return this;
    }
};

// ---------------------------------------------------------------------------- static initialisers
static void JOAN_CDECL init_a6a3ec() { global<float>(d::g_a6a3ec) = global<float>(d::g_a6a3e4) / global<float>(d::flt_3); }  // 0x00694b00

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("Obj68d::init", 0x0068db80, &H::init_68db80);
B("init_a6a3ec", 0x00694b00, &init_a6a3ec);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {
    void clear_580() { jo::self<jo::Collisions>(this)->zone_count = 0; }  // 0x0068dbc0
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj68::clear_580", 0x0068dbc0, &H::clear_580);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14 {

JOAN_COUNTER(c_coll_add, "Collisions::add");
JOAN_COUNTER(c_coll_remove, "Collisions::remove");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x0068dbf0 Collisions::add(a, b, x, y, info[6]) (ret 0x14).
    void* coll_add(jo::PathSpace* a, jo::PathSpace* b, std::int32_t x, std::int32_t y, const std::uint32_t* info) {
        c_coll_add.hit();
        std::int32_t i = 0;
        for (; i < jo::self<jo::Collisions>(this)->zone_count; ++i) {
            if (jo::self<jo::Collisions>(this)->zones[i].key_a == a && jo::self<jo::Collisions>(this)->zones[i].key_b == b) break;
        }
        if (i < jo::self<jo::Collisions>(this)->zone_count) return this;
        jo::RWorldCell& z = jo::self<jo::Collisions>(this)->zones[jo::self<jo::Collisions>(this)->zone_count];
        z.key_a = a;
        z.key_b = b;
        z.x = x;
        z.y = y;
        z.fx = static_cast<float>(x);
        z.fy = static_cast<float>(y);
        z.u32_18 = 0;
        for (std::int32_t k = 0; k < 6; ++k) jo::view<std::uint32_t>(z.box[k]) = info[k];
        jo::self<jo::Collisions>(this)->zone_count = jo::self<jo::Collisions>(this)->zone_count + 1;
        return this;
    }

    // 0x0068dd80 Collisions::remove(a, b) (ret 8).
    void coll_remove(jo::PathSpace* a, jo::PathSpace* b) {
        c_coll_remove.hit();
        for (std::int32_t i = jo::self<jo::Collisions>(this)->zone_count; i > 0; --i) {
            if (jo::self<jo::Collisions>(this)->zones[i - 1].key_a != a) continue;
            if (jo::self<jo::Collisions>(this)->zones[i - 1].key_b != b) continue;
            joan::orig::as_misc(img::obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(static_cast<void*>(self() + offsetof(jo::Collisions, zones))), joan::orig::arg::i32(jo::self<jo::Collisions>(this)->zone_count), static_cast<std::int32_t>(sizeof(jo::RWorldCell)), joan::orig::arg::i32(i));  // 0x00591070
            jo::self<jo::Collisions>(this)->zone_count = jo::self<jo::Collisions>(this)->zone_count - 1;
            return;
        }
    }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("Collisions::add", 0x0068dbf0, &H::coll_add);
B82_14("Collisions::remove", 0x0068dd80, &H::coll_remove);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b111_20.cpp (fix/uninit)
namespace {
namespace game_b111_20 {

using u16 = std::uint16_t;

using joan::orig::helpers::gf;

using joan::orig::helpers::gd;

double c_fabs(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }

// x87 compare helpers (fcomp + fnstsw).
bool eq_or_unord(float a, float b) { return !(a < b) && !(a > b); }

JOAN_COUNTER(c_collide, "Players::collide_68de40");

static_assert(sizeof(Box) == 24, "Box");

// After a hit: shrink the actor's shape by 0.005 when *out == 0 and the actor kind is 0xb.
void shrink_after_hit(u8* actor, const float* out, bool with_box) {
    if (!eq_or_unord(*out, gf(d::k_9cf5c4)) || jo::as_actor(actor)->kind != 0xb) return;
    const float k = gf(d::flt_0_005);
    const i32 shape = jo::view<i32>(jo::as_actor(actor)->aim_mode);
    if (shape == 1) {
        jo::as_actor(actor)->aim_sphere[3] = jo::as_actor(actor)->aim_sphere[3] - k;
    } else if (with_box && shape == 2) {
        jo::as_actor(actor)->aim_box[0] = jo::as_actor(actor)->aim_box[0] + k;
        jo::as_actor(actor)->aim_box[1] = jo::as_actor(actor)->aim_box[1] + k;
        jo::as_actor(actor)->aim_box[2] = jo::as_actor(actor)->aim_box[2] + k;
        jo::as_actor(actor)->aim_box[3] = jo::as_actor(actor)->aim_box[3] - k;
        jo::as_actor(actor)->aim_box[4] = jo::as_actor(actor)->aim_box[4] - k;
        jo::as_actor(actor)->aim_box[5] = jo::as_actor(actor)->aim_box[5] - k;
    } else if (shape == 3) {
        jo::as_actor(actor)->collide_radius = jo::as_actor(actor)->collide_radius - k;
        jo::as_actor(actor)->collide_height = jo::as_actor(actor)->collide_height - k;
    }
}

// Common hit bookkeeping (rec+0x298 / +0x29a, owner flag when the actor kind is 0xb).
void mark_hit(u8* rec, u8* other_actor, u8* p) {
    jo::as_shot(rec)->hit = 1;
    jo::as_shot(rec)->hit_obj = jo::as_actor(other_actor)->kind == 0xb ? jo::as_shot(p)->id : 0u;
    u8* const actor = jo::view<u8*>(jo::as_shot(rec)->actor);
    if (jo::as_actor(actor)->kind == 0xb) {
        const u32 id = jo::view<u32>(jo::as_unit(jo::as_actor(actor)->owner)->rider_id);
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00428b70
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            const u32 key = jo::view<u32>(jo::as_actor(jo::as_unit(u)->actor)->record);
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(key)));  // 0x006af310
            jo::as_shot(q)->hit = 1;
        }
    }
}

bool skip_player(u8* p, u8* rec) {
    if (p == nullptr || p == rec) return true;
    const i32 t = jo::as_shot(p)->type;
    return t == 2 || t == 3 || t == 4 || t == 5 || t == 0x12;
}

// Outside 2r in x or y (fabs of the PC24 difference vs r + r): `test ah,1` jne -> skip.
bool outside_2r(float pos, float other, float r) {
    const Fpu fpu{true};
    const double d = c_fabs(fpu.sub(pos, other));
    const double r2 = static_cast<double>(r) + static_cast<double>(r);
    return !(r2 >= d);
}

bool skip_kind(u8* o) {
    const i32 k = jo::as_map_object(o)->item_type;
    return k == 8 || k == 9 || k == 0xa || k == 0xb || k == 0xc;
}

i32 hit_test(float* out, u8* rec, float* dir, u8* other) {
    return static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::collide::test_6a1bb0(joan::orig::arg::mptr(out), joan::orig::arg::mptr(jo::view<u8*>(jo::as_shot(rec)->actor)), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(other), joan::orig::arg::mptr(obj(d::g_pos)))));  // 0x006a1bb0
}

struct H {
    // ------------------------------------------------------------ 0x0068de40
    u32 collide(void* /*a*/, u8* rec, void* /*b*/, float* dir, float* out) {
        c_collide.hit();
        float pos[3] = {0.0f, 0.0f, 0.0f};
        joan::orig::as_actor(jo::view<u8*>(jo::as_shot(rec)->actor))->position(joan::orig::arg::mptr(pos));  // 0x005ac270
        if (eq_or_unord(dir[0], gf(d::k_9cf5c4)) && eq_or_unord(dir[1], gf(d::k_9cf5c4)) &&
            !(joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(dir[2]))) > gd(d::dbl_0_01))) {  // 0x00427d60 fabs
            jo::as_shot(rec)->hit = 0;
            jo::as_shot(rec)->hit_obj = 0;
            return 0;
        }
        u8* const actor = jo::view<u8*>(jo::as_shot(rec)->actor);
        if (jo::as_actor(actor)->scene == nullptr) return 0;
        i32 count = 0;
        const Fpu fpu{true};
        float r = 0.0f;
        {
            const double len = v3d_call<double>(d::iat_v3d_87_length, dir);  // length
            r = static_cast<float>(fpu.add(fpu.add(len, gf(d::flt_250)), gf(d::flt_20)));
        }
        float h = 0.0f;  // fix/uninit: [ebp-0x20] unwritten for shape types other than 1..3
        switch (jo::view<i32>(jo::as_actor(actor)->aim_mode)) {
            case 1:
                r = r + jo::as_actor(actor)->aim_sphere[3];
                h = jo::as_actor(actor)->aim_sphere[3];
                break;
            case 2: {
                const float dx = jo::as_actor(actor)->aim_box[3] - jo::as_actor(actor)->aim_box[0];
                const float dy = jo::as_actor(actor)->aim_box[4] - jo::as_actor(actor)->aim_box[1];
                const float sq = dx * dx + dy * dy;
                const double root = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(sq)));  // 0x004256d0
                r = static_cast<float>(fpu.add(root, r));
                h = jo::as_actor(actor)->aim_box[5] - jo::as_actor(actor)->aim_box[2];
                break;
            }
            case 3:
                r = r + jo::as_actor(actor)->collide_radius;
                h = jo::as_actor(actor)->collide_radius;
                break;
            default: break;
        }
        Box box;
        joan::orig::as_aabb(&box)->set_point(joan::orig::arg::ptr(pos));  // 0x0060d430
        box.v[0] = box.v[0] - r;
        box.v[1] = box.v[1] - r;
        box.v[2] = box.v[2] - (r - h);
        box.v[3] = box.v[3] + r;
        box.v[4] = box.v[4] + r;
        box.v[5] = (r + h) + box.v[5];
        void** const list = joan::orig::arg::as<void**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(&box, &count));  // 0x00429090
        // The original then widens box z by 1000 (dead local stores, dropped).
        for (i32 i = 0; i < count; ++i) {
            // soldiers
            for (u8* s = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(list[i])->get_90()); s != nullptr;) {  // 0x0054eec0
                u8* const next = jo::view<u8*>(jo::as_unit(s)->cell_next);
                if (outside_2r(pos[0], jo::view<float[3]>(jo::as_unit(s)->position_)[0], r) || outside_2r(pos[1], jo::view<float[3]>(jo::as_unit(s)->position_)[1], r)) {
                    s = next;
                    continue;
                }
                u8* const sa = jo::view<u8*>(jo::as_unit(s)->actor);
                if (sa == nullptr || jo::view<u32>(jo::as_actor(sa)->record) == 0) {
                    s = next;
                    continue;
                }
                u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(sa)->record))));  // 0x006af310
                if (skip_player(p, rec)) {
                    s = next;
                    continue;
                }
                jo::view<u32>(out[0]) = 0;
                bool flag = false;
                const i32 kind = jo::as_shot(rec)->actor->kind;
                if (kind == 6 || kind == 7 || kind == 8 || kind == 9) {
                    if (jo::as_unit(s)->mount_stick != 0 ||
                        joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(s)->weapon))) == 0) {  // 0x00428b70
                        jo::view<i32>(jo::as_actor(jo::as_unit(s)->actor)->aim_mode) = 3;
                        flag = true;
                    }
                }
                if (hit_test(out, rec, dir, jo::view<u8*>(jo::as_unit(s)->actor)) != 0) {
                    joan::orig::as_actor(jo::view<u8*>(jo::as_shot(rec)->actor))->set_position_5ac310(joan::orig::arg::mptr(pos));  // 0x005ac310
                    mark_hit(rec, jo::view<u8*>(jo::as_unit(s)->actor), p);
                    shrink_after_hit(jo::view<u8*>(jo::as_shot(rec)->actor), out, true);
                    if (flag) jo::view<i32>(jo::as_actor(jo::as_unit(s)->actor)->aim_mode) = 0;
                    return jo::as_shot(p)->id;
                }
                if (flag) jo::view<i32>(jo::as_actor(jo::as_unit(s)->actor)->aim_mode) = 0;
                s = next;
            }
            // objects
            for (u8* o = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(list[i])->get_98()); o != nullptr;) {  // 0x0054f510
                u8* const next = jo::view<u8*>(jo::as_map_object(o)->cell_next);
                u8* const oa = jo::view<u8*>(jo::as_map_object(o)->actor);
                if (oa == nullptr || jo::view<u32>(jo::as_actor(oa)->record) == 0 || skip_kind(o)) {
                    o = next;
                    continue;
                }
                u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(oa)->record))));  // 0x006af310
                if (skip_player(p, rec)) {
                    o = next;
                    continue;
                }
                if (jo::as_shot(p)->type == 0x14 && (jo::as_shot(rec)->type == 1 || jo::as_shot(rec)->type == 0)) {
                    o = next;
                    continue;
                }
                const i32 type_id = jo::as_map_object(o)->type_id;
                u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(type_id)));  // 0x00425ce0
                if (joan::orig::arg::as<i32>(joan::orig::as_item_def(t)->is_soldier()) == 0) {                            // 0x004a4050
                    u8* const t2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(type_id)));
                    if (jo::view<i16>(jo::as_item_def(t2)->need_collision) == 0) {
                        o = next;
                        continue;
                    }
                }
                const Fpu f{true};
                if (c_fabs(f.sub(pos[0], jo::as_map_object(o)->pos[0])) > gd(d::dbl_1000) ||
                    c_fabs(f.sub(pos[1], jo::as_map_object(o)->pos[1])) > gd(d::dbl_1000)) {
                    o = next;
                    continue;
                }
                jo::view<u32>(out[0]) = 0;
                if (hit_test(out, rec, dir, jo::view<u8*>(jo::as_map_object(o)->actor)) != 0) {
                    mark_hit(rec, jo::view<u8*>(jo::as_map_object(o)->actor), p);
                    shrink_after_hit(jo::view<u8*>(jo::as_shot(rec)->actor), out, false);
                    return jo::as_shot(p)->id;
                }
                o = next;
            }
            // object references
            for (u8* ref = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(list[i])->get_a0()); ref != nullptr;) {  // 0x0054f960
                u8* const next = jo::view<u8*>(jo::as_world_item(ref)->cell_next);
                const i32 oid = jo::as_world_item(ref)->object_id;
                if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(oid)) != 0) {  // 0x00425d50
                    ref = next;
                    continue;
                }
                u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(oid)));  // 0x00425d20
                u8* const oa = jo::view<u8*>(jo::as_map_object(o)->actor);
                if (oa == nullptr || jo::view<u32>(jo::as_actor(oa)->record) == 0 || skip_kind(o)) {
                    ref = next;
                    continue;
                }
                u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(oa)->record))));  // 0x006af310
                if (skip_player(p, rec)) {
                    ref = next;
                    continue;
                }
                if (outside_2r(pos[0], jo::as_map_object(o)->pos[0], r) || outside_2r(pos[1], jo::as_map_object(o)->pos[1], r)) {
                    ref = next;
                    continue;
                }
                jo::view<u32>(out[0]) = 0;
                if (hit_test(out, rec, dir, jo::view<u8*>(jo::as_map_object(o)->actor)) != 0) {
                    mark_hit(rec, jo::view<u8*>(jo::as_map_object(o)->actor), p);
                    shrink_after_hit(jo::view<u8*>(jo::as_shot(rec)->actor), out, false);
                    return jo::as_shot(p)->id;
                }
                ref = next;
            }
        }
        jo::as_shot(rec)->hit = 0;
        jo::as_shot(rec)->hit_obj = 0;
        return 0;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: shape height [ebp-0x20] is 0 for shape types other than 1..3 (garbage before).
F("Players::collide_68de40", 0x0068de40, &H::collide);
#undef F

}  // namespace game_b111_20
}  // namespace

// ---------------------------------------- from modules/game_b82_01.cpp (game/b82_01)
namespace {
namespace game_b82_01 {

struct V3 {
    float x, y, z;
};
struct Plane {  // 16 bytes (a, b, c, d), passed by value to the shape tests
    float v[4];
};
struct Cyl {  // 20 bytes: centre x, y, z, radius, height
    float v[5];
};
struct M44 {
    float m[16];
};
static_assert(sizeof(V3) == 12 && sizeof(Plane) == 16 && sizeof(Cyl) == 20 && sizeof(Box) == 24 && sizeof(M44) == 64,
              "sizes");

JOAN_COUNTER(c_check_move, "Placement::check_move");

// --- helpers (each one call of the original, through its thunk) ---------------------------
V3 vadd(const V3& a, const V3& b) {
    V3 t;
    return *joan::orig::arg::as<V3*>(joan::orig::vec_add(&t, &a, &b));  // 0x0060cc20
}
void transform(V3* out, const void* in, const void* m) {
    import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(out), in, m);  // point_transform(out, in, m)
}
void transform_in_place(void* v, const void* m) {
    import_call<void*>(d::iat_v3d_87_point_transform_2, v, m);  // point_transform(v, m)
}
void plane(const V3& a, const V3& b, const V3& c, Plane* out) {
    joan::orig::plane_from_points(static_cast<void*>(out), &a, &b, &c);  // 0x0060d920
}
void plane_ctor(Plane* p) { joan::orig::as_plane_equation(p)->init_425700(); }  // 0x00425700 (empty constructor)
std::uint8_t sphere_hit(Cyl c, Plane a, Plane b, Plane s) {
    return joan::orig::arg::as<std::uint8_t>(joan::orig::collide::cylinder_sphere(c, a, b, s));  // 0x0069bd60
}
std::uint8_t box_hit(Cyl c, Plane a, Plane b, Box s) {
    return joan::orig::arg::as<std::uint8_t>(joan::orig::collide::cylinder_box(c, a, b, s));  // 0x0069c0f0
}
std::uint8_t cyl_hit(Cyl c, Plane a, Plane b, Cyl s) {
    return joan::orig::arg::as<std::uint8_t>(joan::orig::collide::cylinder_cylinder(c, a, b, s));  // 0x0069b390
}
void* players_get(std::uint32_t id) { return joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(id)); }  // 0x006af310

// The mover's bounding box under matrix m: O = m * (+0xcc), axes scaled by the extents.
void pose(void* body, const void* m, V3* o, V3* x, V3* y, V3* z) {
    transform(o, jo::as_actor(body)->aim_box, m);
    V3 axis, t;
    {
        void* const a = joan::orig::arg::as<void*>(joan::orig::matrix_axis_x(&axis, joan::orig::arg::mptr(m)));  // 0x0060c180 axis x
        *x = *joan::orig::arg::as<V3*>(joan::orig::vec_scaled(&t, joan::orig::arg::f32(jo::as_actor(body)->aim_box[3] - jo::as_actor(body)->aim_box[0]), joan::orig::arg::ptr(a)));  // 0x0060cd60
    }
    {
        void* const a = joan::orig::arg::as<void*>(joan::orig::matrix_row1(&axis, joan::orig::arg::mptr(m)));  // 0x0060c1d0 axis y
        *y = *joan::orig::arg::as<V3*>(joan::orig::vec_scaled(&t, joan::orig::arg::f32(jo::as_actor(body)->aim_box[4] - jo::as_actor(body)->aim_box[1]), joan::orig::arg::ptr(a)));
    }
    {
        void* const a = joan::orig::arg::as<void*>(joan::orig::matrix_axis_z(&axis, joan::orig::arg::ptr(m)));  // 0x0060c220 axis z
        *z = *joan::orig::arg::as<V3*>(joan::orig::vec_scaled(&t, joan::orig::arg::f32(jo::as_actor(body)->aim_box[5] - jo::as_actor(body)->aim_box[2]), joan::orig::arg::ptr(a)));
    }
}

// Eight corners c[0..7] of a pose, in the add order of the original. along_x: the x extent is
// at least the y extent (first branch).
void corners(V3* c, const V3& o, const V3& x, const V3& y, const V3& z, bool along_x) {
    c[0] = o;
    if (along_x) {
        c[1] = vadd(o, z);
        c[2] = vadd(vadd(o, x), z);
        c[3] = vadd(o, x);
        c[4] = vadd(vadd(o, y), z);
        c[5] = vadd(o, y);
        c[6] = vadd(vadd(o, x), y);
        c[7] = vadd(vadd(vadd(o, x), y), z);
    } else {
        c[1] = vadd(o, y);
        c[2] = vadd(vadd(o, y), z);
        c[3] = vadd(o, z);
        c[4] = vadd(vadd(o, y), x);
        c[5] = vadd(o, x);
        c[6] = vadd(vadd(o, x), z);
        c[7] = vadd(vadd(vadd(o, x), y), z);
    }
}

struct Volume {
    Cyl cyl;      // ebp-0x94
    Plane p[4];   // ebp-0xa4, -0xb4, -0xc4, -0xd4
    V3 c[16];     // c[0..7] new pose (ebp-0x1c4..-0x170), c[8..15] old pose (ebp-0x164..-0x110)
};

// Box shape of another object (lists 1 and 2): the volume moved into the box's frame.
std::uint8_t box_in_frame(const Volume& v, void* other_body) {
    M44 inv;
    import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(&inv), jo::view<void*>(jo::as_actor(other_body)->bone));  // matrix_inverse
    Plane q[4];
    plane_ctor(&q[0]);
    plane_ctor(&q[1]);
    plane_ctor(&q[2]);
    plane_ctor(&q[3]);
    Cyl cyl = v.cyl;
    transform(reinterpret_cast<V3*>(&cyl), &v.cyl, &inv);
    V3 t[16];
    for (int i = 8; i < 16; ++i) transform(&t[i], &v.c[i], &inv);
    for (int i = 0; i < 8; ++i) transform(&t[i], &v.c[i], &inv);
    Box box;
    std::memcpy(&box, jo::as_actor(other_body)->aim_box, sizeof box);
    plane(t[8], t[9], t[10], &q[0]);
    plane(t[5], t[6], t[7], &q[1]);
    std::uint8_t hit = box_hit(cyl, q[0], q[1], box);
    if (hit != 0) return hit;
    plane(t[0], t[1], t[2], &q[2]);
    plane(t[13], t[14], t[15], &q[3]);
    return box_hit(cyl, q[2], q[3], box);
}

// Record kinds that never block (lists 1 to 3).
bool skipped_kind(const void* other, const void* rec) {
    if (other == nullptr || other == rec) return true;
    const std::int32_t k = jo::as_shot(other)->type;
    return k == 2 || k == 3 || k == 4 || k == 5 || k == 0x12;
}
bool skipped_unit_type(const void* unit) {
    const std::int32_t t = jo::as_map_object(unit)->item_type;
    return t == 8 || t == 9 || t == 0xa || t == 0xb || t == 0xc;
}

struct H {
    // 0x0068ee50 (ret 0x18).
    std::uint32_t check_move(void* players, void* rec, void* pos_arg, std::uint32_t a, std::uint32_t b, std::uint32_t c) {
        c_check_move.hit();
        (void)players;
        (void)pos_arg;
        std::uint32_t result = 0;
        void* const body = jo::view<void*>(jo::as_shot(rec)->actor);
        V3 pos;
        joan::orig::as_actor(body)->position(&pos);  // 0x005ac270 position
        if (jo::as_actor(body)->scene == nullptr) return 0;
        std::int32_t cell_count = 0;
        float radius = 0.0f, ext_x = 0.0f, ext_y = 0.0f;  // see the header (shape > 3)
        const std::uint32_t shape = jo::as_actor(body)->aim_mode;
        if (shape <= 3) {
            if (shape != 2) return 0;
            V3 hi = jo::view<V3[2]>(jo::as_actor(body)->aim_box)[1];
            hi.z = 0.0f;
            V3 lo = jo::view<V3[2]>(jo::as_actor(body)->aim_box)[0];
            lo.z = 0.0f;
            const float len_hi = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&hi)));
            const float len_lo = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&lo)));
            if (!(len_lo >= len_hi))  // fcomp, test ah,1
                radius = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&hi)));
            else
                radius = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&lo)));
            ext_x = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(jo::as_actor(body)->aim_box[3] - jo::as_actor(body)->aim_box[0])));  // 0x00427d60 fabs
            ext_y = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(jo::as_actor(body)->aim_box[4] - jo::as_actor(body)->aim_box[1])));
        }

        M44 m;
        joan::orig::matrix_rotation_ypr(&m, joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c));  // 0x0060bf40 rotation
        {
            const float s = jo::as_actor(body)->scale;
            if (s < global<float>(d::flt_1) || s > global<float>(d::flt_1)) {  // != 1.0 (test ah,0x40)
                m.m[0] = m.m[0] * jo::as_actor(body)->scale;
                m.m[1] = m.m[1] * jo::as_actor(body)->scale;
                m.m[2] = m.m[2] * jo::as_actor(body)->scale;
                m.m[4] = m.m[4] * jo::as_actor(body)->scale;
                m.m[5] = m.m[5] * jo::as_actor(body)->scale;
                m.m[6] = m.m[6] * jo::as_actor(body)->scale;
                m.m[8] = m.m[8] * jo::as_actor(body)->scale;
                m.m[9] = m.m[9] * jo::as_actor(body)->scale;
                m.m[10] = m.m[10] * jo::as_actor(body)->scale;
            }
        }
        void* const old_m = jo::view<void*>(jo::as_actor(body)->bone);
        m.m[12] = jo::as_bone(old_m)->local[12];
        m.m[13] = jo::as_bone(old_m)->local[13];
        m.m[14] = jo::as_bone(old_m)->local[14];

        Volume v;
        v.cyl.v[0] = pos.x;
        v.cyl.v[1] = pos.y;
        v.cyl.v[2] = pos.z;
        {
            const float h = jo::as_actor(body)->aim_box[5] - jo::as_actor(body)->aim_box[2];
            v.cyl.v[4] = h;
            v.cyl.v[2] = h * global<float>(d::flt_0_5) + v.cyl.v[2];  // 0.5
        }
        v.cyl.v[3] = radius;
        plane_ctor(&v.p[0]);
        plane_ctor(&v.p[1]);
        plane_ctor(&v.p[2]);
        plane_ctor(&v.p[3]);

        V3 o, x, y, z;
        pose(body, jo::view<void*>(jo::as_actor(body)->bone), &o, &x, &y, &z);
        const bool along_x = ext_x >= ext_y;  // fcomp, test ah,1 jne: x < y or unordered -> second form
        corners(&v.c[8], o, x, y, z, along_x);
        pose(body, &m, &o, &x, &y, &z);
        corners(&v.c[0], o, x, y, z, along_x);

        plane(v.c[8], v.c[9], v.c[10], &v.p[0]);
        plane(v.c[5], v.c[6], v.c[7], &v.p[1]);
        plane(v.c[0], v.c[1], v.c[2], &v.p[2]);
        plane(v.c[13], v.c[14], v.c[15], &v.p[3]);

        const float grow = radius * global<float>(d::flt_1_5) + global<float>(d::flt_200);  // 1.5, 200
        float half;
        {
            const double fz1 = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::as_actor(body)->aim_box[5]))));  // fabs
            const double fz0 = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::as_actor(body)->aim_box[2]))));
            double pick;
            if (!(fz0 >= fz1))
                pick = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::as_actor(body)->aim_box[5]))));
            else
                pick = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::as_actor(body)->aim_box[2]))));
            half = static_cast<float>(pick);
        }
        Box area;
        joan::orig::as_aabb(&area)->set_point(&pos);  // 0x0060d430 Box::set_point
        area.v[0] = area.v[0] - grow;
        area.v[1] = area.v[1] - grow;
        area.v[2] = area.v[2] - half * global<float>(d::flt_1_1);  // 1.1
        area.v[3] = area.v[3] + grow;
        area.v[4] = area.v[4] + grow;
        area.v[5] = half * global<float>(d::flt_1_1) + area.v[5];
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(&area, &cell_count));  // 0x00429090
        area.v[2] = area.v[2] - global<float>(d::flt_1000);  // dead stores of the original
        area.v[5] = area.v[5] + global<float>(d::flt_1000);

        for (std::int32_t i = 0; i < cell_count; ++i) {
            // list 1: objects with a placement record at +0x2f5
            void* next = nullptr;
            for (void* u = joan::orig::arg::as<void*>(joan::orig::as_world_cell(cells[i])->get_90()); u != nullptr; u = next) {  // 0x0054eec0
                next = joan::orig::arg::as<void*>(joan::orig::get_341(joan::orig::arg::mptr(u)));                                       // 0x0054ef40
                if (jo::as_unit(u)->actor == nullptr || jo::view<std::int32_t>(jo::as_actor(jo::as_unit(u)->actor)->record) == 0) continue;
                void* const other = players_get(jo::view<std::uint32_t>(jo::as_actor(jo::as_unit(u)->actor)->record));
                if (skipped_kind(other, rec)) continue;
                std::uint8_t hit = 0;
                void* const ob = jo::view<void*>(jo::as_shot(other)->actor);
                const std::uint32_t oshape = jo::as_actor(ob)->aim_mode;
                if (oshape <= 3) {
                    if (oshape == 0) continue;
                    if (oshape == 1) {
                        Plane s;
                        std::memcpy(&s, jo::as_actor(ob)->aim_sphere, sizeof s);
                        transform_in_place(&s, jo::view<void*>(jo::as_actor(ob)->bone));
                        hit = sphere_hit(v.cyl, v.p[0], v.p[1], s);
                        if (hit == 0) hit = sphere_hit(v.cyl, v.p[2], v.p[3], s);
                    } else if (oshape == 2) {
                        hit = box_in_frame(v, ob);
                    } else {
                        Cyl s;
                        std::memcpy(&s, &jo::as_actor(ob)->collide_kind, sizeof s);  // collide_kind .. collide_offset
                        transform_in_place(&s, jo::view<void*>(jo::as_actor(ob)->bone));
                        hit = cyl_hit(v.cyl, v.p[0], v.p[1], s);
                        if (hit == 0) hit = cyl_hit(v.cyl, v.p[2], v.p[3], s);
                    }
                }
                if (hit == 0) continue;
                if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(jo::as_actor(ob)->owner)->byte_2e1()) == 0 &&  // 0x00432b90
                    !(jo::as_shot(other)->state.goal_len > global<float>(d::k_9cf5c4)) &&       // test ah,0x41 je: > 0
                    jo::view<std::int16_t>(jo::as_shot(other)->state.goal_flag) == 0) {
                    const std::int32_t k = jo::as_shot(other)->type;
                    if (k != 0x11 && k != 0x13 && k != 0x10 && k != 0xf && k != 0xb && k != 7 && k != 6) {
                        V3 opos, t;
                        joan::orig::as_actor(ob)->position(&opos);  // 0x005ac270
                        V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&t, &opos, &pos));  // 0x0060cc90 vec_sub
                        d.z = 0.0f;
                        import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(&d));  // normalize
                        V3 push = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t, &d, 25.0f));  // 0x0060cd00 (0x41c80000)
                        joan::orig::as_player(other)->start_move(&push, joan::orig::arg::i32(global<std::uint32_t>(d::g_now)), joan::orig::arg::u32(global<std::uint32_t>(d::g_frame_step_ms) << 1));  // 0x006b12a0
                    }
                }
                jo::view<std::int16_t>(jo::as_shot(rec)->hit) = 1;
                jo::as_shot(rec)->hit_obj = jo::as_shot(other)->id;
                result = 1;
            }

            // list 2: objects with a record at +0x86
            for (void* u = joan::orig::arg::as<void*>(joan::orig::as_world_cell(cells[i])->get_98()); u != nullptr; u = next) {  // 0x0054f510
                next = joan::orig::arg::as<void*>(joan::orig::get_b2(joan::orig::arg::mptr(u)));                                       // 0x0054f530
                if (jo::view<void*>(jo::as_map_object(u)->actor) == nullptr || jo::view<std::int32_t>(jo::as_map_object(u)->actor->record) == 0) continue;
                if (skipped_unit_type(u)) continue;
                void* const other = players_get(jo::view<std::uint32_t>(jo::as_map_object(u)->actor->record));
                if (skipped_kind(other, rec)) continue;
                if (jo::as_shot(other)->type == 0x14 && jo::as_shot(rec)->type == 1) continue;
                if (jo::as_shot(other)->type == 0x14 && jo::as_shot(rec)->type == 0) continue;
                std::uint8_t hit = 0;
                void* const ob = jo::view<void*>(jo::as_shot(other)->actor);
                const std::uint32_t oshape = jo::as_actor(ob)->aim_mode;
                if (oshape <= 3) {
                    if (oshape == 0) continue;
                    if (oshape == 1) {
                        Plane s;
                        std::memcpy(&s, jo::as_actor(ob)->aim_sphere, sizeof s);
                        transform_in_place(&s, jo::view<void*>(jo::as_actor(ob)->bone));
                        hit = sphere_hit(v.cyl, v.p[0], v.p[1], s);
                        if (hit == 0) hit = sphere_hit(v.cyl, v.p[2], v.p[3], s);
                    } else if (oshape == 2) {
                        hit = box_in_frame(v, ob);
                    } else {
                        Cyl s;
                        std::memcpy(&s, &jo::as_actor(ob)->collide_kind, sizeof s);  // collide_kind .. collide_offset
                        transform_in_place(&s, jo::view<void*>(jo::as_actor(ob)->bone));
                        hit = cyl_hit(v.cyl, v.p[0], v.p[1], s);
                        if (hit == 0) hit = cyl_hit(v.cyl, v.p[0], v.p[1], s);  // planes 0 / 1 again (original)
                    }
                }
                if (hit != 0) return 1;
            }

            // list 3: units of the cell
            for (void* e = joan::orig::arg::as<void*>(joan::orig::as_world_cell(cells[i])->get_a0()); e != nullptr; e = next) {  // 0x0054f960
                next = joan::orig::arg::as<void*>(joan::orig::get_42(joan::orig::arg::mptr(e)));                                       // 0x0054f980
                const std::int32_t id = jo::as_world_item(e)->object_id;
                if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id))) != 0) continue;  // 0x00425d50
                void* const u = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
                if (skipped_unit_type(u)) continue;
                if (jo::view<void*>(jo::as_map_object(u)->actor) == nullptr || jo::view<std::int32_t>(jo::as_map_object(u)->actor->record) == 0) continue;
                void* const other = players_get(jo::view<std::uint32_t>(jo::as_map_object(u)->actor->record));
                if (skipped_kind(other, rec)) continue;
                std::uint8_t hit = 0;
                void* const ob = jo::view<void*>(jo::as_shot(other)->actor);
                const std::uint32_t oshape = jo::as_actor(ob)->aim_mode;
                if (oshape <= 3) {
                    if (oshape == 0) continue;
                    if (oshape == 1) {
                        Plane s;
                        std::memcpy(&s, jo::as_actor(ob)->aim_sphere, sizeof s);
                        hit = sphere_hit(v.cyl, v.p[0], v.p[1], s);
                        if (hit == 0) hit = sphere_hit(v.cyl, v.p[2], v.p[3], s);
                    } else if (oshape == 2) {
                        Box s;
                        std::memcpy(&s, jo::as_actor(ob)->aim_box, sizeof s);
                        hit = box_hit(v.cyl, v.p[0], v.p[1], s);
                        if (hit == 0) hit = box_hit(v.cyl, v.p[2], v.p[3], s);
                    } else {
                        Cyl s;
                        std::memcpy(&s, &jo::as_actor(ob)->collide_kind, sizeof s);  // collide_kind .. collide_offset
                        hit = cyl_hit(v.cyl, v.p[0], v.p[1], s);
                        if (hit == 0) hit = cyl_hit(v.cyl, v.p[0], v.p[1], s);  // planes 0 / 1 again (original)
                    }
                }
                if (hit != 0) return 1;
            }
        }
        return result;
    }
};

JOAN_REPLACE("game/b82_01", "Placement::check_move", 0x0068ee50, &H::check_move);

}  // namespace game_b82_01
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
namespace {
namespace game_b101_14 {

static_assert(sizeof(Vec3) == 12, "Vec3");
struct Seg {
    float v[6];
};
static_assert(sizeof(Seg) == 24, "Seg");

JOAN_COUNTER(c_blocked, "Scene::segment_blocked");

struct H {
    // ------------------------------------------------------------------ 0x00692c10
    i32 segment_blocked(u8* o, const Vec3* pos, const float* top) {
        c_blocked.hit();
        if (jo::view<u32>(jo::as_shot(o)->type) != 0) return 0;
        if (jo::as_shot(o)->actor == nullptr) return 0;
        u8* const owner = jo::view<u8*>(jo::as_shot(o)->actor->owner);
        // 0x005ef610 reads a box (min, max): the degenerate box {pos, pos} (the original keeps the point and its copy adjacent, [ebp-0x20] / [ebp-0x14]).
        Vec3 key_box[2] = {*pos, *pos};
        const Vec3& key = key_box[0];  // [ebp-0x20]
        i32 count = 0;
        void* const grid = jo::view<void*>(jo::as_world(global<u8*>(d::g_world))->scene);
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_scene(grid)->nodes_in_box(key_box, &count));  // 0x005ef610
        Seg seg;  // [ebp-0x40]: start, end
        seg.v[0] = key.x;
        seg.v[1] = key.y;
        seg.v[2] = key.z;
        seg.v[3] = key.x;
        seg.v[4] = key.y;
        seg.v[5] = key.z;
        {
            u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(owner)->type_id)));  // 0x0042d0a0
            const i32 half = static_cast<i32>(jo::as_unit_type(g)->height) / 2;
            seg.v[2] = static_cast<float>(Fpu{true}.add(static_cast<double>(half), static_cast<double>(seg.v[2])));
        }
        {
            u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(owner)->type_id)));  // 0x0042d0a0
            const i32 h = static_cast<i32>(jo::as_unit_type(g)->height);
            const Fpu fpu{true};
            const double s = fpu.add(static_cast<double>(h), static_cast<double>(top[2]));
            seg.v[5] = static_cast<float>(fpu.add(s, static_cast<double>(seg.v[5])));
        }
        for (i32 i = count - 1; i >= 0; --i) {
            u8* const cell = static_cast<u8*>(cells[i]);
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(&seg, joan::orig::arg::ptr(jo::as_scene_s(cell)->desc))) == 0) continue;  // 0x0060dfd0 (the node's box, +0x8 on x86)
            for (u8* a = joan::orig::arg::as<u8*>(joan::orig::as_scene_node(cell)->attachments()); a != nullptr; a = reinterpret_cast<u8*>(jo::as_actor(a)->scene_next)) {  // 0x005efff0
                u8* const plane = jo::view<u8*>(jo::as_actor(a)->plane);
                if (jo::as_actor(a)->kind != 1) continue;
                u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(jo::as_actor(a)->owner)->type_id)));  // 0x00425ce0
                if (static_cast<i32>(jo::view<i16>(jo::as_item_def(unit)->need_collision)) != 0) continue;
                const i32 kind = static_cast<i32>(jo::view<i16>(jo::as_item_def(unit)->class_id));
                if (kind == 6 || kind == 7 || kind == 8 || kind == 9 || kind == 0x10 || kind == 0xf ||
                    kind == 0x13)
                    continue;
                if (plane != nullptr && static_cast<i32>(static_cast<std::int8_t>(plane[4])) == 7) continue;
                if (plane == nullptr) continue;
                float sphere[4];  // [ebp-0x74]
                joan::orig::as_actor(a)->bounding_sphere(joan::orig::arg::mptr(sphere), 1u);  // 0x005acc80
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&seg, joan::orig::arg::mptr(sphere))) == 0) continue;  // 0x0060e3f0
                float dist = 0.0f;                                              // [ebp-0x78]
                if (joan::orig::arg::as<i32>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(a)->plane))->segment_hit(&dist, &seg)) == 0) continue;  // 0x005d4a80
                return 1;  // "have best" flag [ebp-0x50] is always 0: the distance compare is dead
            }
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("Scene::segment_blocked", 0x00692c10, &H::segment_blocked);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

JOAN_COUNTER(c_pick, "Map::pick_692f40");

struct H {
    // -------------------------------------------------------------- 0x00692f40
    u8 map_pick(u8* who, i32 key_arg) {
        c_pick.hit();
        float v[3] = {0.0f, 0.0f, 0.0f};
        joan::orig::as_actor(who)->position(joan::orig::arg::mptr(static_cast<void*>(v)));  // 0x005ac270
        u32 blk[4] = {0, 0, 0, 0};  // [ebp-0x20..-0x18] and [ebp-0x14]
        const i32 ix = joan::hook::x87::ftol(static_cast<double>(v[0]));
        const i32 iy = joan::hook::x87::ftol(static_cast<double>(v[1]));
        const i32 iz = joan::hook::x87::ftol(static_cast<double>(v[2]));
        i32 found = -1;
        for (i32 i = 0; i < jo::self<jo::Collisions>(this)->zone_count; ++i) {
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::self<jo::Collisions>(this)->zones[i].box)->contains(static_cast<void*>(v))) == 0) continue;  // 0x0060d5f0
            u32 o3c = 0, o44 = 0, o40 = 0, o48 = 0;
            const float fz = static_cast<float>(iz);
            const float fy = static_cast<float>(iy - jo::self<jo::Collisions>(this)->zones[i].y);
            const float fx = static_cast<float>(ix - jo::self<jo::Collisions>(this)->zones[i].x);
            void* const cell = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(&jo::self<jo::Collisions>(this)->zones[i])->layer(joan::orig::arg::i16(key_arg)));  // 0x006aac10
            joan::orig::as_path(cell)->find_6731d0(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), joan::orig::arg::f32(fz), joan::orig::arg::u32(i32{0}), &o3c, &o44, &o40, &o48, joan::orig::arg::u32(i32{0}));  // 0x006731d0
            if (o40 != 0) {
                found = i;
                break;
            }
        }
        if (found == -1) return 0;
        for (i32 n = joan::orig::as_dynarray(jo::self<jo::Collisions>(this)->objects)->size(); n > 0; --n) {  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::self<jo::Collisions>(this)->objects)->ptr_at(joan::orig::arg::i32(n)));  // 0x0041b3f0
            if (e != nullptr && jo::view<u8*>(jo::as_shot(e)->actor) == who) continue;
            if (e == nullptr) continue;
            if (joan::orig::arg::as<u8>(joan::orig::collide::test_6a1bb0(&blk[3], joan::orig::arg::mptr(who), &blk[0], joan::orig::arg::mptr(jo::as_shot(e)->actor), &blk[0])) != 0) return 0;  // 0x006a1bb0
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Map::pick_692f40", 0x00692f40, &H::map_pick);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

struct Mat {  // D3DMATRIX
    u32 m[16];
};
static_assert(sizeof(Mat) == 0x40, "Mat");
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_can_place, "Formation::can_place");

struct H {
    // ------------------------------------------------------------------------ 0x006932d0
    static void unit_matrix(void* u, Mat& m, Vec3& pos) {
        if (jo::as_unit(u)->actor != nullptr) {
            void* const node = jo::view<void*>(jo::as_actor(jo::as_unit(u)->actor)->bone);
            m = *joan::orig::arg::as<const Mat*>(joan::orig::as_bone(node)->matrix());  // 0x0060ef80
        } else {
            const float a = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());  // 0x004ff850
            joan::orig::matrix_rotation_ypr(&m, joan::orig::arg::bits(a), 0u, 0u);       // 0x0060bf40
            import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, &m, &pos);        // matrix_multiply_translation
        }
    }
    u8 can_place(i32 id, i32 b, i32 c) {
        c_can_place.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        void* const me = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        Vec3 pos;
        joan::orig::as_unit(me)->position(&pos);  // 0x004fedb0
        const i32 ix = ftol(pos.x), iy = ftol(pos.y), iz = ftol(pos.z);
        void* slot = nullptr;  // written by the callee: the GridCell pointer (pointer-wide)
        i32 ok = 0;
        joan::orig::find_zone_pair_6aae70(joan::orig::arg::mptr(jo::self<jo::Collisions>(this)->zones), joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Collisions>(this)->zone_count)), joan::orig::arg::bits(static_cast<float>(ix)), joan::orig::arg::bits(static_cast<float>(iy)), joan::orig::arg::bits(static_cast<float>(iz)), joan::orig::arg::u32(c), joan::orig::arg::u32(static_cast<i16>(b)), &slot, &ok, 1u);  // 0x006aae70
        if (ok == 0 || slot == nullptr) return 0;
        Mat mine;
        unit_matrix(me, mine, pos);
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (u == nullptr || i == id) continue;
            joan::orig::as_unit(u)->position(&pos);  // 0x004fedb0
            Mat other;
            unit_matrix(u, other, pos);
            const u8 hit = joan::orig::arg::as<u8>(joan::orig::shapes_overlap(static_cast<i32>(jo::as_unit(me)->type_id), &mine, static_cast<i32>(jo::as_unit(u)->type_id), &other));  // 0x006a3d10
            if (hit != 0) return 0;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Formation::can_place", 0x006932d0, &H::can_place);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using joan::orig::helpers::at;
using joan::orig::helpers::wsub;

struct Vf {
    float x, y, z;
};

JOAN_COUNTER(c_693580, "Region::hit_693580");

struct H {
    // 0x00693580
    bool region_hit(float x, float y, float z, u32 w_raw) {
        c_693580.hit();
        const i32 ix = joan::hook::x87::ftol(x);
        const i32 iy = joan::hook::x87::ftol(y);
        const i32 iz = joan::hook::x87::ftol(z);
        const Vf pos{x, y, z};
        i32 hit = -1;
        u32 r18 = 0;
        for (i32 i = 0; i < jo::self<jo::Collisions>(this)->zone_count; ++i) {
            jo::RWorldCell* const zone = &jo::self<jo::Collisions>(this)->zones[i];
            if (joan::orig::arg::as<u8>(joan::orig::as_aabb(zone->box)->contains(&pos)) == 0) continue;  // 0x0060d5f0
            // 0x006731d0 zeroes f20 / f24 first thing and only ever writes f20 together with f28 and
            // f24 together with f2c (its second loop at 0x006733ed is unreachable). Inside it, f28 / f2c
            // decide a branch only when f20 / f24 != 0 (both branch arms meet otherwise). Here f28 is
            // read when f24 == 0: if f20 == 0 the result is 0 on both compare outcomes, and f20 != 0
            // implies f28 was written. So the start values of f28 / f2c are dead.
            u32 f20 = 0, f24 = 0;
            float f28 = 0.0f, f2c = 0.0f;
            const float fz = static_cast<float>(iz);
            const float fy = static_cast<float>(wsub(iy, jo::self<jo::Collisions>(this)->zones[i].y));
            const float fx = static_cast<float>(wsub(ix, jo::self<jo::Collisions>(this)->zones[i].x));
            void* const o = joan::orig::arg::as<void*>(joan::orig::as_grid_cell(zone)->layer(joan::orig::arg::i16(static_cast<i16>(w_raw))));  // 0x006aac10
            joan::orig::as_path(o)->find_6731d0(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), joan::orig::arg::f32(fz), 0u, &f20, &f28, &f24, &f2c, 0u);              // 0x006731d0
            hit = i;
            r18 = f24;
            if (f24 == 0) {
                const float d = f28 - z;
                if (!(d >= global<float>(d::flt_4))) r18 = f20;
            }
            break;
        }
        if (hit == -1) return false;
        return r18 != 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Region::hit_693580", 0x00693580, &H::region_hit);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

struct H {
    // ------------------------------------------------------------------------ 0x00693720
    i32 true_693720(u32, u32) { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::true_693720", 0x00693720, &H::true_693720);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (fix/uninit)
namespace {
namespace game_b115_11 {

using i8 = std::int8_t;

using joan::orig::helpers::at;

const Fpu fpu{true};

JOAN_COUNTER(c_proj, "Projectile::hit_test_693750 (fix/uninit)");

static_assert(sizeof(Vec3) == 12, "Vec3");
struct Blob24 {
    u8 b[24];
};
struct Blob20 {
    u8 b[20];
};
struct alignas(4) Seg {  // [ebp-0x40] segment (0x0060d430 start / 0x0060d490 end)
    u8 b[0x18];
};
struct alignas(4) Sphere {  // [ebp-0x64] bound from 0x005acc80
    u8 b[0x10];
};
struct alignas(4) Mat {
    float m[16];
};
// x87 compare with 0.0 + `test ah,0x40`: equal or unordered.
bool eqz(float v) { return !(v < 0.0f) && !(v > 0.0f); }
// Excluded unit kinds (+0x50 in 8..12, +0x4e in {7, 8, 9, 15, 19}).
bool excluded_kind(const u8* o) {
    const i32 a = jo::as_map_object(o)->item_type;
    if (a == 8 || a == 9 || a == 0xa || a == 0xb || a == 0xc) return true;
    const i32 b = jo::as_map_object(o)->class_id;
    return b == 7 || b == 8 || b == 9 || b == 0xf || b == 0x13;
}
// 0x00425ce0 on 0xa58f50 (word id) -> record; byte +0x1e3 set = hidden.
bool hidden_kind(const u8* o) {
    const u8* const rec = joan::orig::arg::as<const u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(o)->type_id))));
    return jo::as_item_def(rec)->skip_intersect_with_item != 0;
}
// Material flag of triangle `tri` (1-based) of the node's mesh.
i32 tri_material_flag(const u8* node, i32 tri) {
    const u8* const mesh = jo::view<u8*>(jo::as_actor(node)->plane);
    const u8* const table = jo::as_plane(mesh)->data_pending != 0 ? reinterpret_cast<const u8*>(jo::as_plane(mesh)->source->triangle_materials) : jo::view<u8*>(jo::as_plane(mesh)->triangle_materials);
    const u32 mat = table[static_cast<u32>(tri) * 2u - 2u] | (static_cast<u32>(table[static_cast<u32>(tri) * 2u - 1u]) << 8);
    const jo::Plane* const lib = jo::as_actor(node)->def->model->mesh;
    return jo::view<i32>(lib->materials[mat].blend);
}
struct H {
    // ======================================================== 0x00693750
    // Projectile hit test (this unused): first the swept segment pos -> pos + dir + 2 * speed * n
    // against the objects of the grid cells it crosses (exact mesh test for kind-7 meshes,
    // triangle test otherwise), then the delayed ground hit.
    i32 projectile_hit(u8* p, const Vec3* pos, const Vec3* dir, float* out) {
        c_proj.hit();
        Vec3 n{};
        v3d_call<Vec3*>(d::iat_v3d_87_vector_normalize_2, &n, dir);  // normalize(out, in)
        // The swept segment: the segment tests below take one pointer to both points (the original keeps them
        // adjacent at [ebp-0x28] / [ebp-0x1c]); two separate locals left the end point to the frame layout.
        struct Line {
            Vec3 start, end;
        };
        static_assert(sizeof(Line) == 24, "Line: two adjacent points");
        Line line{*pos, {}};
        Vec3& start = line.start;              // [ebp-0x28]
        Vec3& end = line.end;                  // [ebp-0x1c]
        const float speed = jo::as_shot(p)->actor->def->model->sphere[3];
        Vec3 t1{}, t2{}, t3{};
        joan::orig::vec_scale(&t1, &n, joan::orig::arg::f32(speed));   // 0x0060cd00 scale
        joan::orig::vec_scale(&t2, &t1, 2.0f);
        joan::orig::vec_add(&t3, joan::orig::arg::ptr(pos), joan::orig::arg::ptr(dir));    // 0x0060cc20 add
        joan::orig::vec_add(&end, &t3, &t2);   // [ebp-0x1c]
        Seg seg{};
        joan::orig::as_aabb(&seg)->set_point(&start);  // 0x0060d430
        joan::orig::as_aabb(&seg)->extend(&end);    // 0x0060d490
        i32 count = 0;                               // [ebp-0x44]
        void* const grid = jo::view<void*>(jo::as_world(global<void*>(d::g_world))->scene);
        void** cells = joan::orig::arg::as<void**>(joan::orig::as_scene(grid)->nodes_in_box(&seg, &count));  // 0x005ef610
        // The shot's owner is its Missile: +0xc kind, +0x26, +0x56 / +0x5a / +0x5e times, +0x62 hit object.
        u8* const desc = static_cast<u8*>(jo::as_shot(p)->actor->owner);         // [ebp-0x80]
        i32 dur = 0;     // [ebp-0x84] fix/uninit: 0 for descriptor types 3 / > 5
        u32 zoff = 0;    // [ebp-0x88] (float bits) fix/uninit: 0.0
        switch (jo::as_missile(desc)->state) {  // Missile::state (+0xc)
            case 0:
                dur = ftol(global<float>(d::g_a39d30));
                zoff = global<u32>(d::g_a39d48);
                break;
            case 1:
                dur = global<i32>(d::g_a39d90);
                zoff = global<u32>(d::g_a39cac);
                break;
            case 2:
                dur = ftol(global<float>(d::g_a39ccc));
                zoff = global<u32>(d::g_a39cd0);
                break;
            case 4:
                dur = ftol(global<float>(d::g_a39db0));
                zoff = global<u32>(d::g_a39dd0);
                break;
            case 5:
                dur = ftol(global<float>(d::g_a39d40));
                zoff = global<u32>(d::g_a39d4c);
                break;
            default:
                break;
        }
        float t = 0.0f;  // [ebp-0x50]: written by every callee that reports a hit
        for (i32 i = 0; i < count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(&line, joan::orig::arg::ptr(jo::as_scene_s(cells[i])->desc))) == 0) continue;  // 0x0060dfd0
            for (u8* node = joan::orig::arg::as<u8*>(joan::orig::as_scene_s(cells[i])->first_actor()); node != nullptr;  // 0x005effa0
                 node = reinterpret_cast<u8*>(jo::as_actor(node)->scene_prev)) {
                if (jo::view<u8*>(jo::as_shot(p)->actor) == node) continue;
                if (jo::as_actor(node)->kind != 1) continue;
                u8* const o = jo::view<u8*>(jo::as_actor(node)->owner);  // [ebp-0x94]
                if (excluded_kind(o) || hidden_kind(o)) continue;
                u8* const mesh = jo::view<u8*>(jo::as_actor(node)->plane);  // [ebp-0x54]
                if (mesh != nullptr && jo::as_plane(mesh)->vertex_format == 7) {
                    Sphere bound{};
                    joan::orig::as_actor(node)->bounding_sphere(&bound, 1u);                    // 0x005acc80
                    if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&line, &bound)) == 0) continue;  // 0x0060e3f0
                    u8 hit = 0;
                    joan::orig::as_bone(jo::view<u8*>(jo::as_actor(node)->bone))->update();  // 0x0060ee60
                    Mat inv{};
                    v3d_call<void*>(d::iat_v3d_87_matrix_inverse_2, &inv, jo::as_actor(node)->bone->matrix_);  // matrix_inverse
                    Vec3 lp{}, le{};
                    v3d_call<void*>(d::iat_v3d_87_point_transform_3, &lp, &start, &inv);  // point_transform(out, in, m)
                    v3d_call<void*>(d::iat_v3d_87_point_transform_3, &le, &end, &inv);
                    const i32 shape = jo::view<i32>(jo::as_actor(node)->aim_mode);
                    if (shape == 1) {
                        Vec3 d{};
                        joan::orig::vec_sub(&d, &le, &lp);  // 0x0060cc90
                        hit = joan::orig::arg::as<u8>(joan::orig::shape::sphere_hit(&t, lp, d, jo::as_actor(node)->aim_sphere));  // 0x006a0f90
                    } else if (shape == 2) {
                        Vec3 d{};
                        joan::orig::vec_sub(&d, &le, &lp);
                        Blob24 b{};
                        std::memcpy(b.b, jo::as_actor(node)->aim_box, sizeof b.b);
                        hit = joan::orig::arg::as<u8>(joan::orig::shape::box_hit(&t, lp, d, b));  // 0x00698970
                    } else if (shape == 3) {
                        Vec3 d{};
                        joan::orig::vec_sub(&d, &le, &lp);
                        Blob20 b{};
                        std::memcpy(b.b, &jo::as_actor(node)->collide_kind, sizeof b.b);
                        hit = joan::orig::arg::as<u8>(joan::orig::shape::cylinder_hit(&t, lp, d, b));  // 0x00699ec0
                    }
                    if (hit == 0) continue;
                    std::memcpy(out, &t, 4);
                    if (jo::as_missile(desc)->expired == 0) continue;
                    const double now = static_cast<double>(global<u32>(d::g_now));
                    const double dt = static_cast<double>(global<u32>(d::g_frame_step_ms));
                    const double sum = fpu.add(now, fpu.mul(dt, fpu.sub(static_cast<double>(t), 1.0)));
                    const double when = static_cast<double>(jo::as_missile(desc)->hit_time);
                    if (when > sum) return static_cast<i32>(jo::as_map_object(o)->index);
                    continue;
                }
                if (mesh == nullptr) continue;
                if (!eqz(jo::view<float>(jo::as_shot(p)->saved.vec0_x)) || !eqz(jo::as_shot(p)->saved.vec0_y) || !eqz(jo::as_shot(p)->saved.vec0_z)) continue;
                u8* const o2 = jo::view<u8*>(jo::as_actor(node)->owner);  // [ebp-0xe0]
                if (excluded_kind(o2) || hidden_kind(o2)) continue;
                Sphere bound{};
                joan::orig::as_actor(node)->bounding_sphere(&bound, 1u);
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&line, &bound)) == 0) continue;
                const i32 tri = joan::orig::arg::as<i32>(joan::orig::as_plane(jo::view<u8*>(jo::as_actor(node)->plane))->segment_hit(&t, &line));  // 0x005d4a80
                if (tri == 0) continue;
                if (tri_material_flag(node, tri) == 1) continue;
                std::memcpy(out, &t, 4);
                return static_cast<i32>(jo::as_map_object(o2)->index);
            }
        }
        auto ratio_out = [desc, out]() {  // 1 - (now - +0x5e) / dt
            const double num = static_cast<double>(global<u32>(d::g_now) - jo::as_missile(desc)->hit_time);
            const float r = static_cast<float>(
                fpu.sub(1.0, fpu.div(num, static_cast<double>(global<i32>(d::g_frame_step_ms)))));
            *out = r;
        };
        float frac = 1.0f;  // [ebp-0xf0]
        i32 who = 0;        // [ebp-0xf4]
        if (jo::as_missile(desc)->hit_object != 0 && global<u32>(d::g_now) - global<u32>(d::g_frame_step_ms) < jo::as_missile(desc)->hit_time &&
            jo::as_missile(desc)->hit_time <= global<u32>(d::g_now)) {
            ratio_out();
            return jo::as_missile(desc)->hit_object;
        }
        if (jo::as_missile(desc)->shot_end > global<u32>(d::g_now)) return 0;
        if (eqz(jo::view<float>(jo::as_shot(p)->saved.vec0_x)) && eqz(jo::as_shot(p)->saved.vec0_y) && eqz(jo::as_shot(p)->saved.vec0_z)) return 0;
        jo::as_missile(desc)->shot_start = global<u32>(d::g_now) - global<u32>(d::g_frame_step_ms);
        jo::as_missile(desc)->shot_end = global<u32>(d::g_now) - global<u32>(d::g_frame_step_ms) + static_cast<u32>(dur);
        jo::as_missile(desc)->hit_object = 0;
        const double k = static_cast<double>(global<float>(d::flt_46_1));  // 46.1
        float zf = 0.0f;
        std::memcpy(&zf, &zoff, 4);
        const double s0 = fpu.mul(static_cast<double>(jo::view<float>(jo::as_shot(p)->saved.vec0_x)), k);
        const double s1 = fpu.mul(static_cast<double>(jo::as_shot(p)->saved.vec0_y), k);
        const double s2 = fpu.mul(fpu.sub(static_cast<double>(jo::as_shot(p)->saved.vec0_z), static_cast<double>(zf)), k);
        const double ms = global<double>(d::dbl_0_001);  // 0.001
        const double ddur = static_cast<double>(static_cast<u32>(dur));
        end.x = static_cast<float>(fpu.add(static_cast<double>(pos->x), fpu.mul(fpu.mul(ddur, static_cast<double>(static_cast<float>(s0))), ms)));
        end.y = static_cast<float>(fpu.add(static_cast<double>(pos->y), fpu.mul(fpu.mul(ddur, static_cast<double>(static_cast<float>(s1))), ms)));
        end.z = static_cast<float>(fpu.add(static_cast<double>(pos->z), fpu.mul(fpu.mul(ddur, static_cast<double>(static_cast<float>(s2))), ms)));
        const Vec3 target = end;  // [ebp-0x10c]
        joan::orig::as_aabb(&seg)->set_point(&start);
        joan::orig::as_aabb(&seg)->extend(&end);
        cells = joan::orig::arg::as<void**>(joan::orig::as_scene(grid)->nodes_in_box(&seg, &count));
        for (i32 i = 0; i < count; ++i) {
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(&line, joan::orig::arg::ptr(jo::as_scene_s(cells[i])->desc))) == 0) continue;
            for (u8* node = joan::orig::arg::as<u8*>(joan::orig::as_scene_s(cells[i])->first_actor()); node != nullptr;
                 node = reinterpret_cast<u8*>(jo::as_actor(node)->scene_prev)) {
                if (jo::view<u8*>(jo::as_shot(p)->actor) == node) continue;
                if (jo::as_actor(node)->kind != 1) continue;
                u8* const mesh = jo::view<u8*>(jo::as_actor(node)->plane);
                if (mesh == nullptr || jo::as_plane(mesh)->vertex_format == 7) continue;
                u8* const o = jo::view<u8*>(jo::as_actor(node)->owner);  // [ebp-0x118]
                if (excluded_kind(o) || hidden_kind(o)) continue;
                Sphere bound{};
                joan::orig::as_actor(node)->bounding_sphere(&bound, 1u);
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&line, &bound)) == 0) continue;
                const i32 tri = joan::orig::arg::as<i32>(joan::orig::as_plane(jo::view<u8*>(jo::as_actor(node)->plane))->segment_hit(&t, &line));
                if (tri == 0) continue;
                if (tri_material_flag(node, tri) == 1) continue;
                frac = static_cast<float>(fpu.mul(frac, t));
                end.x = static_cast<float>(fpu.add(fpu.mul(fpu.sub(target.x, start.x), frac), start.x));
                end.y = static_cast<float>(fpu.add(fpu.mul(fpu.sub(target.y, start.y), frac), start.y));
                end.z = static_cast<float>(fpu.add(fpu.mul(fpu.sub(target.z, start.z), frac), start.z));
                who = static_cast<i32>(jo::as_map_object(o)->index);
                if (!(frac > global<float>(d::flt_0_001))) {  // 0.001
                    jo::view<u32>(out[0]) = 0;
                    return who;
                }
            }
        }
        const u32 base = global<u32>(d::g_now) - global<u32>(d::g_frame_step_ms);
        jo::as_missile(desc)->hit_time = base + static_cast<u32>(ftol(fpu.mul(ddur, static_cast<double>(frac))));
        jo::as_missile(desc)->hit_object = who;
        if (jo::as_missile(desc)->hit_object != 0 && jo::as_missile(desc)->hit_time < global<u32>(d::g_now)) {
            ratio_out();
            return jo::as_missile(desc)->hit_object;
        }
        return 0;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: descriptor types 3 / > 5 leave [ebp-0x84] / [ebp-0x88] unset -> 0 / 0.0.
F("Projectile::hit_test_693750", 0x00693750, &H::projectile_hit);
#undef F

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_694a30", 0x00694a30, &static_init<fn::init_pi_a6a3e4>);
B("init_694a90", 0x00694a90, &static_init<fn::init_a6a3e8>);
B("init_694ae0", 0x00694ae0, &static_init<fn::init_a6a3ec>);
B("init_694b30", 0x00694b30, &static_init<fn::scale_694b50>);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

// 0x00694a50
void JOAN_CDECL init_pi_a6a3e4() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a6a3e4) = static_cast<float>(Fpu{true}.mul(a, 4.0));  // qword [0x9cf788]
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("init_pi_a6a3e4", 0x00694a50, &init_pi_a6a3e4);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

// 0x00694ab0
void JOAN_CDECL init_a6a3e8() { global<float>(d::g_a6a3e8) = global<float>(d::g_a6a3e4) / global<float>(d::flt_2); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("init_a6a3e8", 0x00694ab0, &init_a6a3e8);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

static void JOAN_CDECL scale_694b50() { global<float>(d::g_a6a3f0) = global<float>(d::g_a6a3e4) / global<float>(d::flt_4); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("scale_694b50", 0x00694b50, &scale_694b50);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CollisionGrid::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_10::H::clear_584>(this)); }  // 0x68db10
auto joan::orig::CollisionGrid::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_07::H::call_68dbc0>(this)); }  // 0x68db50
auto joan::orig::CollisionGrid::init(DynArray* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_15::H::init_68db80>(this, a0)); }  // 0x68db80
auto joan::orig::CollisionGrid::clear() -> void { joan::bind::member<&::game_b93_06::H::clear_580>(this); }  // 0x68dbc0
auto joan::orig::CollisionGrid::add(PathSpace* a0, PathSpace* a1, std::int32_t a2, std::int32_t a3, void* a4) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_14::H::coll_add>(this, a0, a1, a2, a3, a4)); }  // 0x68dbf0
auto joan::orig::CollisionGrid::remove(PathSpace* a0, PathSpace* a1) -> void { joan::bind::member<&::game_b82_14::H::coll_remove>(this, a0, a1); }  // 0x68dd80
auto joan::orig::Players::collide_68de40(void* a0, void* a1, void* a2, void* a3, void* a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_20::H::collide>(this, a0, a1, a2, a3, a4)); }  // 0x68de40
auto joan::orig::CollisionGrid::check_move(void* a0, void* a1, void* a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_01::H::check_move>(this, a0, a1, a2, a3, a4, a5)); }  // 0x68ee50
auto joan::orig::Scene::segment_blocked(void* a0, void* a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_14::H::segment_blocked>(this, a0, a1, a2)); }  // 0x692c10
auto joan::orig::Formation::can_place(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_17::H::can_place>(this, a0, a1, a2)); }  // 0x6932d0
auto joan::orig::CollisionGrid::region_hit(float a0, float a1, float a2, std::uint32_t a3) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b108_09::H::region_hit>(this, a0, a1, a2, a3)); }  // 0x693580
auto joan::orig::CollisionGrid::projectile_hit(void* a0, void* a1, void* a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b115_11::H::projectile_hit>(this, a0, a1, a2, a3)); }  // 0x693750
