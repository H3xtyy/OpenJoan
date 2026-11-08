// SPDX-License-Identifier: GPL-3.0-or-later
// enphysic/rbody.cpp: functions of the original d:\ww\enphysic\rbody.cpp (re/attribution.csv). The file name is the
// Restaurant Empire 2 engine's (enphysic/rbody.cpp, re/re2_engine_matches.csv): joan.exe has no __FILE__ string for it.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using i16 = std::int16_t;
using u16 = std::uint16_t;

using u32 = std::uint32_t;

constexpr img::Addr kGameUnits(d::g_units);

}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

u32 bits(float v) {
    u32 r;
    std::memcpy(&r, &v, 4);
    return r;
}
float fl(u32 v) {
    float r;
    std::memcpy(&r, &v, 4);
    return r;
}

// Vec3 (12 bytes), passed by pointer or by value.
struct Vec3 {
    u32 v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");


void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_shot_init, "Shot::init_6b08e0");

struct H {
    // ------------------------------------------------------------------------ 0x006b08e0
    // max(+0x27e, +0x282) as the original picks it (fcompp: +0x282 < +0x27e or NaN -> +0x27e).
    float max_27e() {
        const float a = jo::self<jo::Shot>(this)->extent[0], b = jo::self<jo::Shot>(this)->extent[1];
        return !(b >= a) ? jo::self<jo::Shot>(this)->extent[0] : jo::self<jo::Shot>(this)->extent[1];
    }
    void init_6b08e0(void* owner, jo::Actor* a, u32 b, const void* v, i16 type, i16 flag) {
        c_shot_init.hit();
        jo::self<jo::Shot>(this)->actor = a;
        jo::self<jo::Shot>(this)->u32_27a = b;
        std::memcpy(jo::self<jo::Shot>(this)->extent, v, 12);
        jo::self<jo::Shot>(this)->type = static_cast<i32>(type);
        jo::self<jo::Shot>(this)->hit = 0;
        jo::self<jo::Shot>(this)->u16_2ac = 0;
        jo::self<jo::Shot>(this)->active_link = 0;
        jo::view<u16>(jo::self<jo::Shot>(this)->u16_2a2) = 0;
        jo::self<jo::Shot>(this)->idle_frames = 0;
        joan::orig::as_camera(this)->reset_state(joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->saved));    // 0x006b0d10
        joan::orig::as_camera(this)->reset_state(joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->state));  // 0x006b0d10
        joan::orig::as_body(this)->pull_position();                       // 0x006b18a0
        u32 l2c;
        {
            const i32 t = jo::self<jo::Shot>(this)->type;
            if (t == 9 || t == 0xa || t == 0xb || t == 0xe || t == 0xf || t == 0x11)
                l2c = bits(max_27e() / global<float>(d::flt_1));
            else
                l2c = 0;
        }
        u32 l8 = l2c;
        {
            const i32 t = jo::self<jo::Shot>(this)->type;
            if (t == 7 || t == 6) l8 = bits(max_27e() / global<float>(d::flt_3));
        }
        if (static_cast<i32>(flag) == 0) {
            std::memcpy(&jo::self<jo::Shot>(this)->saved, &jo::self<jo::Shot>(this)->state, sizeof(jo::BodyState));
            return;
        }
        u32 lc = jo::view<u32>(jo::self<jo::Shot>(this)->state.pos_z);
        {
            const i32 t = jo::self<jo::Shot>(this)->type;
            if (t == 5 || t == 2 || t == 3 || t == 4 || t == 0x12) {
                void* const p = jo::self<jo::Shot>(this)->actor->owner;  // a Missile: +0x2a shooter unit id, +0x2e object id
                bool done = false;
                if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::as_missile(p)->shooter_unit_id)) == 0) {  // 0x00428b70
                    void* const r = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::as_missile(p)->shooter_unit_id));  // 0x0041b3c0
                    if (jo::as_unit(r)->layer != 0) {
                        void* const q = joan::orig::as_players(owner)->list.ptr_at(joan::orig::arg::i32(jo::as_missile(p)->shooter_record));  // 0x006af310
                        if (jo::as_shot(q)->state.zone != 0) jo::self<jo::Shot>(this)->state.zone = jo::as_shot(q)->state.zone;
                        done = true;
                    }
                }
                if (!done && joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::as_missile(p)->shooter_unit_id)) == 0) {  // 0x00428b70
                    void* const q = joan::orig::as_players(owner)->list.ptr_at(joan::orig::arg::i32(jo::as_missile(p)->shooter_record));  // 0x006af310
                    lc = jo::view<u32>(jo::as_shot(q)->state.pos_z);
                }
            }
        }
        if (jo::as_players(owner)->cell_count > 0) {
            u32 l28 = jo::self<jo::Shot>(this)->state.cell;
            const i32 ty = static_cast<i32>(type);
            const float k = global<float>(d::flt_46_1);
            const float p3 = fl(lc) * k;
            const float p2 = jo::self<jo::Shot>(this)->state.pos_y * k;
            const float p1 = jo::self<jo::Shot>(this)->state.pos_x * k;
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_body(this)->side());  // 0x006b1dd0 (ax; Picker::pick reads only its low word)
            Vec3 out{};
            joan::orig::as_players(owner)->pick(&out, &l28, joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->state.sub), joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->state.zone), joan::orig::arg::u32(r), joan::orig::arg::bits(// 0x006adfd0
                          p1), joan::orig::arg::bits(p2), joan::orig::arg::bits(p3), joan::orig::arg::u32(l8), joan::orig::arg::i32(ty));
            jo::self<jo::Shot>(this)->state.cell = l28;
        }
        {
            const i32 t = jo::self<jo::Shot>(this)->type;
            if (t == 5 || t == 4 || t == 0x12 || t == 3) jo::self<jo::Shot>(this)->state.on_ground = 0;
        }
        std::memcpy(&jo::self<jo::Shot>(this)->saved, &jo::self<jo::Shot>(this)->state, sizeof(jo::BodyState));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("RigidBody::init", 0x006b08e0, &H::init_6b08e0);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b82_12.cpp (game/b82_12)
namespace {
namespace game_b82_12 {

// fcomp + test ah,0x40 + je "differs": equal OR unordered falls through.
bool same_or_unordered(float a, float b) { return !(a < b || a > b); }

constexpr img::Addr kPlayers(d::g_physic_sys);

using joan::orig::helpers::as_float;
std::uint32_t as_bits(float f) {
    std::uint32_t u;
    std::memcpy(&u, &f, 4);
    return u;
}

// 12-byte vector moved as raw dwords (by-value argument / hidden-return buffer).
struct V3u {
    std::uint32_t x, y, z;
};
static_assert(sizeof(V3u) == 12, "V3u");
// A 12-byte vector argument by value with float members (System V passes them in XMM registers, as the callee expects).
struct V3F {
    float x, y, z;
};
V3F as_v3f(const V3u& v) {
    V3F f;
    std::memcpy(&f, &v, sizeof f);
    return f;
}

JOAN_COUNTER(c_sync, "Record::sync_position");
JOAN_COUNTER(c_pull, "Record::pull_position");

struct H {
    // Moves linked unit `unit` to the body's position (+0x30/+0x34/+0x38) offset in x / y by
    // the body's +0/+4/+8 vector scaled by `scale_bits` (added when `add`, else subtracted),
    // then copies the record links to the unit's record. Shared tail of 0x006b1350.
    void move_linked(void* unit, std::uint32_t scale_bits, bool add) {
        void* const body = jo::view<void*>(jo::self<jo::Shot>(this)->actor);
        joan::orig::as_bone(jo::view<void*>(jo::as_actor(body)->bone))->update();  // 0x0060ee60
        float px = jo::as_bone(jo::view<void*>(jo::self<jo::Shot>(this)->actor->bone))->local[12];
        float py = jo::as_bone(jo::view<void*>(jo::self<jo::Shot>(this)->actor->bone))->local[13];
        const std::uint32_t pz = jo::view<std::uint32_t>(jo::as_bone(jo::view<void*>(jo::self<jo::Shot>(this)->actor->bone))->local[14]);
        V3u d;
        d.x = jo::view<std::uint32_t>(jo::as_bone(jo::view<void*>(jo::self<jo::Shot>(this)->actor->bone))->local[0]);
        d.y = jo::view<std::uint32_t>(jo::as_bone(jo::view<void*>(jo::self<jo::Shot>(this)->actor->bone))->local[1]);
        d.z = jo::view<std::uint32_t>(jo::as_bone(jo::view<void*>(jo::self<jo::Shot>(this)->actor->bone))->local[2]);
        joan::orig::vec_scale_in_place(&d, joan::orig::arg::f32(scale_bits));  // 0x0060ce80 scale in place
        if (add) {
            px = px + as_float(d.x);
            py = py + as_float(d.y);
        } else {
            px = px - as_float(d.x);
            py = py - as_float(d.y);
        }
        V3u pos;
        pos.x = as_bits(px);
        pos.y = as_bits(py);
        pos.z = pz;
        joan::orig::as_ground_object(unit)->set_position(as_v3f(pos));  // 0x004fedf0

        joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_unit(unit)->actor)->bone))->update();  // 0x0060ee60
        void* const rec = joan::orig::as_players(img::obj(kPlayers))->list.ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_actor(jo::as_unit(unit)->actor)->record)));  // 0x006af310
        jo::as_shot(rec)->state.cell = jo::self<jo::Shot>(this)->state.cell;
        jo::as_shot(rec)->state.zone = jo::self<jo::Shot>(this)->state.zone;
        jo::as_shot(rec)->state.sub = jo::self<jo::Shot>(this)->state.sub;
    }

    // 0x006b1350 (ret)
    void sync_position() {
        c_sync.hit();
        const std::uint32_t id = jo::self<jo::Shot>(this)->id;
        const std::uint8_t changed = joan::orig::arg::as<std::uint8_t>(joan::orig::vector_differs(joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->state.pos_x), joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->saved.pos_x)));  // 0x0060cf40
        if (changed != 0) {
            V3u tmp;
            const V3u* const r = joan::orig::arg::as<const V3u*>(joan::orig::vec_scale(&tmp, joan::orig::arg::ptr(&jo::self<jo::Shot>(this)->state.pos_x), joan::orig::arg::f32(0x42386666u)));  // 0x0060cd00
            V3u v = *r;
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::Shot>(this)->actor))->set_position_5ac310(&v);  // 0x005ac310
        }
        if (joan::orig::as_players(img::obj(kPlayers))->list.ptr_at(joan::orig::arg::i32(id)) == nullptr) return;  // 0x006af310
        if (!same_or_unordered(jo::self<jo::Shot>(this)->state.angle_x, jo::self<jo::Shot>(this)->saved.angle_x) || !same_or_unordered(jo::self<jo::Shot>(this)->state.angle_y, jo::self<jo::Shot>(this)->saved.angle_y) ||
            !same_or_unordered(jo::self<jo::Shot>(this)->state.angle_z, jo::self<jo::Shot>(this)->saved.angle_z)) {
            joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Shot>(this)->actor))->set_angles(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.angle_x)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.angle_y)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.angle_z)));  // 0x005acaf0
        }
        if (jo::self<jo::Shot>(this)->actor->kind != 0xb) return;
        void* const c = jo::self<jo::Shot>(this)->actor->owner;
        if (joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_unit(c)->rider_id))) == 0) {  // 0x00428b70
            void* const u = joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_unit(c)->rider_id)));  // 0x0041b3c0
            if (jo::as_unit(u)->actor != nullptr) {
                const std::int32_t kind = jo::as_unit(u)->anim_kind;
                if (kind != 0x51 && kind != 0x52 && kind != 0x4f && kind != 0x50) move_linked(u, 0x41f00000u, true);
            }
        }
        if (joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_unit(c)->rider_ref))) != 0) return;  // 0x00428b70
        void* const u2 = joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_unit(c)->rider_ref)));  // 0x0041b3c0
        if (jo::as_unit(u2)->actor == nullptr) return;
        move_linked(u2, 0x42a00000u, false);
    }

    // 0x006b18a0 (ret); returns 0x005acc30's eax.
    std::uintptr_t pull_position() {
        c_pull.hit();
        V3u v;
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Shot>(this)->actor))->position(&v);  // 0x005ac270
        V3u tmp;
        const V3u* const r = joan::orig::arg::as<const V3u*>(joan::orig::vec_scale(&tmp, &v, joan::orig::arg::f32(0x3cb1b35eu)));  // 0x0060cd00
        const V3u s = *r;
        jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.pos_x) = s.x;
        jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.pos_y) = s.y;
        jo::view<std::uint32_t>(jo::self<jo::Shot>(this)->state.pos_z) = s.z;
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_actor(jo::view<void*>(jo::self<jo::Shot>(this)->actor))->get_xyz(joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->state.angle_x), joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->state.angle_y), joan::orig::arg::mptr(&jo::self<jo::Shot>(this)->state.angle_z)));  // 0x005acc30
    }
};

#define B12(name, addr, fn) JOAN_REPLACE("game/b82_12", name, addr, fn)
B12("RigidBody::set_object_state", 0x006b1350, &H::sync_position);
B12("RigidBody::get_object_state", 0x006b18a0, &H::pull_position);
#undef B12

}  // namespace game_b82_12
}  // namespace

// ---------------------------------------- from modules/game_players1.cpp (game/players1)
namespace {
namespace game_players1 {

JOAN_COUNTER(c_side, "Record::side");

struct H {
    // 0x006b1dd0 Record::side() (ret), returns a word.
    std::int16_t record_side() {
        c_side.hit();
        const std::int32_t kind = jo::self<jo::Shot>(this)->type;
        if (kind == 0) return 0;
        if (kind != 7) return 1;
        void* const owner = jo::self<jo::Shot>(this)->actor->owner;
        if (joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_unit(owner)->rider_id))) != 0) return 1;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_unit(owner)->rider_id)));  // 0x0041b3c0
        return jo::as_unit(unit)->layer;
    }
};

#define P1(name, addr, fn) JOAN_REPLACE("game/players1", name, addr, fn)
P1("RigidBody::is_unit_ai", 0x006b1dd0, &H::record_side);
#undef P1

}  // namespace game_players1
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {
    void dtor_6b1ea0() {  // 0x006b1ea0
        joan::orig::as_record_list(this)->clear();  // 0x006b1f30
        joan::orig::dynarray_b(this)->destruct();  // 0x00585710
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("RigidBodyArray::~RigidBodyArray", 0x006b1ea0, &H::dtor_6b1ea0);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

struct H {
    // 0x006b1f30
    std::int32_t owner_list_clear() {
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x006b0790
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_record_list(this)->get(joan::orig::arg::u32(i)));                          // 0x006b0760
            if (p != nullptr) joan::orig::as_body(p)->destroy(1u);                           // 0x006b1ff0
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));                        // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        return 0;  // void callee; both callers (shot.cpp, rworld.cpp) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("RigidBodyArray::deinit", 0x006b1f30, &H::owner_list_clear);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

struct V3 {
    float x, y, z;
};

struct H {
    // 0x006b12a0 (ret 0xc)
    void start_move(const V3* pos, std::int32_t start, std::uint32_t duration) {
        std::memcpy(jo::self<jo::Shot>(this)->state.goal, pos, sizeof(jo::self<jo::Shot>(this)->state.goal));  // +0x236
        jo::self<jo::Shot>(this)->state.goal_start = start;
        jo::self<jo::Shot>(this)->state.goal_end = static_cast<std::uint32_t>(start) + duration;
        jo::self<jo::Shot>(this)->state.goal_len = static_cast<float>(static_cast<double>(duration));  // fild qword (zero-extended)
        jo::self<jo::Shot>(this)->state.goal_flag = 0;
        joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::self<jo::Shot>(this)->id), joan::orig::arg::i16(1));  // 0x006ad980
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("RigidBody::push", 0x006b12a0, &H::start_move);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

struct H {
    // ------------------------------------------------------------------ 0x006b1e70
    void* intlist_ctor() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("RigidBodyArray::RigidBodyArray", 0x006b1e70, &H::intlist_ctor);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15_2 {
struct H {
    // ------------------------------------------------------------------------ 0x006b1f10 etc.
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("RigidBodyArray::init", 0x006b1f10, &H::nop);
#undef B

}  // namespace game_b103_15_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Body::init(void* a0, Actor* a1, std::uint32_t a2, void* a3, std::int16_t a4, std::int16_t a5) -> void { joan::bind::member<&::game_b103_15::H::init_6b08e0>(this, a0, a1, a2, a3, a4, a5); }  // 0x6b08e0
auto joan::orig::Body::sync_position() -> void { joan::bind::member<&::game_b82_12::H::sync_position>(this); }  // 0x6b1350
auto joan::orig::Body::pull_position() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_12::H::pull_position>(this)); }  // 0x6b18a0
auto joan::orig::Body::side() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_players1::H::record_side>(this)); }  // 0x6b1dd0
auto joan::orig::RecordList::destruct() -> void { joan::bind::member<&::game_b93_06::H::dtor_6b1ea0>(this); }  // 0x6b1ea0
auto joan::orig::RecordList::clear() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_06::H::owner_list_clear>(this)); }  // 0x6b1f30
auto joan::orig::Player::start_move(void* a0, std::int32_t a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b83_04::H::start_move>(this, a0, a1, a2); }  // 0x6b12a0
auto joan::orig::RecordList::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_08::H::intlist_ctor>(this)); }  // 0x6b1e70
auto joan::orig::RecordList::reset() -> void { joan::bind::member<&::game_b103_15_2::H::nop>(this); }  // 0x6b1f10
