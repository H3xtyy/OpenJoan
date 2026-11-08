// game/unknown_i_obj12.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_obj12.cpp;
// re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::orig::helpers::addr;

using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;

void* const kTypes = obj(d::g_item_res);

u8* type_record(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0

}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
namespace {
namespace game_b105_04 {

// ---------------------------------------------------------------- CRT initialisers
template <fn::Code Callee>
u32 JOAN_CDECL init_call() {
    return cdecl_call<u32>(Callee);
}

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}

    H* self0() { return this; }

    // ---------------------------------------------------------------- scalar deleting dtors
    template <fn::Code DtorThunk>
    H* scalar_dtor(u32 flags) {
        thiscall<u32>(DtorThunk, this);
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(this));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
B("Obj::stub_4903b0", 0x004903b0, &H::stub0);
B("Obj::self_4903d0", 0x004903d0, &H::self0);
B("Obj::scalar_dtor_48f790", 0x0048f790, &H::scalar_dtor<fn::Obj48f_dtor>);  // 0x0048f7e0
B("init_48f5f0", 0x0048f5f0, &init_call<fn::init_a39c20>);
B("init_48f650", 0x0048f650, &init_call<fn::init_a39c24>);
B("init_48f6a0", 0x0048f6a0, &init_call<fn::init_a39c28>);
B("init_48f6f0", 0x0048f6f0, &init_call<fn::init_48f710>);
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::hook::x87::Fpu;

float pi_value() {
    constexpr Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));
}

void JOAN_CDECL init_a39c20() { global<float>(d::g_pi_a39c20) = pi_value(); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_a39c20", 0x0048f610, &init_a39c20);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

// ---------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39c24() { div_global(d::g_a39c24, d::g_pi_a39c20, d::flt_2); }  // 0x0048f670

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_a39c24", 0x0048f670, &init_a39c24);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

void JOAN_CDECL init_a39c28() { global<float>(d::g_a39c28) = global<float>(d::g_pi_a39c20) / global<float>(d::flt_3); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a39c28", 0x0048f6c0, &init_a39c28);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

// ---------------------------------------------------------------------------- 0x0048f710
static void JOAN_CDECL init_48f710() { global<float>(d::g_a39c2c) = global<float>(d::g_pi_a39c20) / global<float>(d::flt_4); }

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("init_48f710", 0x0048f710, &init_48f710);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {


using joan::orig::helpers::memset_;  // memset

// Order constructors: base 0x00416a50, vtable, memset(this + 0xc2, 0, n).
using joan::orig::helpers::order_ctor;

struct H {
    void* order_48f740() { return order_ctor(this, d::vt_Obj_9d3a18, 0); }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order00::ctor", 0x0048f740, &H::order_48f740);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

struct Vec3 {  // float members: passed by value to List43::add (System V: XMM registers)
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
Vec3 vec_at(const float* p) {
    Vec3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

void* const kEvents = obj(d::g_a5ccd0);

u32 now() { return global<u32>(d::g_now); }
u8* types(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (reads the low word)

// Anim player (0x005abe30, 6 args, ret 0x18).
void anim_play(u8* a, const char* name) { joan::orig::as_actor(a)->start_anim(joan::orig::arg::mptr(jo::as_actor(a)->anim), name, joan::orig::arg::u32(now()), 0u, 0u, 0u); }

// Sound at the object's position: 0x0041baf0(handle, sample, src, pos, 1, arg, 0, 5000); emitter k (2 / 3) of the
// item type: script id, position, scale (as the argument).
void play_sound(u8* self, u8* t, i32 k) {
    const u32 arg = jo::view<u32>(jo::as_item_def(t)->emitter_scale[k]);
    void* const pos = jo::as_map_object(self)->pos;
    void* const src = &jo::as_item_def(t)->emitter_pos[k * 3];
    u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(t)->emitter_scripts[k]))));  // 0x0041b440
    void* const r24 = jo::as_pemi_script(r)->effect;
    jo::Scene* const h = jo::as_map_object(self)->actor->scene;
    joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(h, r24, src, pos, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(arg), 0, 0x1388);  // 0x0041baf0
}

// TriggerList::add 0x004eef00 (16 dword args, ret 0x40) with the 0x4000 / 200.0 pattern.
void trigger_4000(u8* self, u32 kind) {
    const u32 t1 = now();
    const u32 t0 = now();
    joan::orig::as_triggers(kEvents)->add(0x4000u, joan::orig::arg::u32(kind), joan::orig::arg::i16(0u), 2u, joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_map_object(self)->index))), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), vec_at(jo::as_map_object(self)->pos), 0x43480000u, static_cast<std::int8_t>(joan::orig::arg::i32(0u)), joan::orig::arg::u32(t0), joan::orig::arg::u32(t1), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), 0u, joan::orig::arg::i16(0u));
}

JOAN_COUNTER(c_event_48fa50, "Obj48f::on_event");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    u32 dtor_48f7e0() {
        jo::self<jo::Order00>(this)->vtable = img::obj(d::vt_Obj_9d3a18);
        joan::orig::as_map_object(this)->release_48f8a0();         // 0x0048f8a0
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->destruct());  // 0x00415790
    }

    // ------------------------------------------------------------------ 0x0048fa50
    // Returns true when the state kind was 0 / 1 / 2 (the 0x4000 path then plays the sounds).
    bool event_state(u8* t) {
        if (jo::self<jo::Order00>(this)->busy == 0) return false;
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[jo::view<i32>(jo::self<jo::Order00>(this)->busy) - 1])) == 0) {
            virtual_call<void>(this, 0x18, jo::view<i32>(jo::self<jo::Order00>(this)->busy) - 1);
            if (static_cast<i32>(jo::view<i8>(jo::self<jo::Order00>(this)->active)) != 0 && jo::self<jo::Order00>(this)->actor != nullptr) {
                anim_play(jo::view<u8*>(jo::self<jo::Order00>(this)->actor), jo::as_item_def(t)->anim_names[jo::view<i32>(jo::self<jo::Order00>(this)->busy)]);
                trigger_4000(self(), 2);
                trigger_4000(self(), 1);
            }
            return true;
        }
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[jo::view<i32>(jo::self<jo::Order00>(this)->busy) - 1])) == 1) {
            virtual_call<void>(this, 0x18, jo::view<i32>(jo::self<jo::Order00>(this)->busy) - 1);
            trigger_4000(self(), 2);
            trigger_4000(self(), 1);
            return true;
        }
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[jo::view<i32>(jo::self<jo::Order00>(this)->busy) - 1])) == 2) {
            virtual_call<void>(this, 0x18, jo::view<i32>(jo::self<jo::Order00>(this)->busy) - 1);
            if (static_cast<i32>(jo::view<i8>(jo::self<jo::Order00>(this)->active)) != 0 && jo::self<jo::Order00>(this)->actor != nullptr)
                anim_play(jo::view<u8*>(jo::self<jo::Order00>(this)->actor), jo::as_item_def(t)->anim_names[jo::view<i32>(jo::self<jo::Order00>(this)->busy)]);
            return true;
        }
        return false;
    }
    u8 on_event_48fa50(i16 ev) {
        c_event_48fa50.hit();
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(kEvents)->at_485f20(joan::orig::arg::u32(static_cast<i32>(ev))));  // 0x00485f20
        const u32 code = jo::view<u32>(jo::as_save_entry(e)->type);
        if (code == 0x2000) {
            if (jo::as_save_entry(e)->ref_kind != 1) return 1;
            u8* const t = types(jo::self<jo::Order00>(this)->type_id);
            event_state(t);
        } else if (code == 0x4000) {
            if (jo::as_save_entry(e)->ref_kind != 2) return 1;
            u8* const t = types(jo::self<jo::Order00>(this)->type_id);
            if (!event_state(t)) return 1;
            if (static_cast<i32>(jo::view<i8>(jo::self<jo::Order00>(this)->active)) == 0 || jo::self<jo::Order00>(this)->actor == nullptr) return 1;
            if (jo::view<u32>(jo::as_item_def(t)->emitter_scripts[2]) != 0) play_sound(self(), t, 2);
            if (jo::view<u32>(jo::as_item_def(t)->emitter_scripts[3]) != 0) play_sound(self(), t, 3);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Obj48f::~Obj48f", 0x0048f7e0, &H::dtor_48f7e0);
B("Obj48f::on_event", 0x0048fa50, &H::on_event_48fa50);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {


struct H {
    // ------------------------------------------------------------------ 0x0048a040 / 0x0048f8a0
    u32 base_4251b0() { return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_4251b0()); }  // 0x004251b0

    // ------------------------------------------------------------------ 0x0048f850
    u8* init_48f850(u32 type) {
        joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type));  // 0x004178e0
        u8* const t = type_record(static_cast<i16>(type));
        jo::self<jo::Order00>(this)->busy = jo::view<u32>(jo::as_item_def(t)->model_count);
        return t;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::init_48f850", 0x0048f850, &H::init_48f850);
B("Obj::base_48f8a0", 0x0048f8a0, &H::base_4251b0);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::img::str;

JOAN_COUNTER(c_tower_fire, "Tower::update_fire");

struct H {
    // ------------------------------------------------------------------ 0x00490110
    std::uintptr_t tower_fire() {
        const u32 base = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->create_sprite());  // 0x0042f210
        if (global<i32>(d::g_mission) == 4) return base;
        if (jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6) == 0) return addr(this);
        if (jo::view<i32>(jo::self<jo::Wall>(this)->max_hp) == 0) return addr(this);
        const i32 pct = static_cast<i32>(static_cast<u32>(jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6)) * 100u) / jo::view<i32>(jo::self<jo::Wall>(this)->max_hp);
        if (pct >= 0x32) return static_cast<u32>(pct);
        u8* const kind = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Wall>(this)->type_id))));  // 0x00425ce0
        if (jo::view<i32>(jo::as_item_def(kind)->struction_type) != 1) return addr(kind);
        if (jo::self<jo::Wall>(this)->actor == nullptr) return addr(kind);
        c_tower_fire.hit();
        std::uintptr_t last = 0;
        for (i32 k = 0; k < 2; ++k) {
            i16& slot = jo::self<jo::Wall>(this)->sound_slots[k];  // +0x96 / +0x98
            const char* file = str(k == 0 ? img::Addr{d::s_towerfire_txt} : img::Addr{d::s_towerfiresmoke_txt});  // "towerfire.txt" / "towerfiresmoke.txt"
            const u32 v62 = jo::view<u32>(jo::self<jo::Wall>(this)->yaw_);
            const u32 res = joan::orig::arg::as<u32>(joan::orig::as_name_list(obj(d::g_pemi_res))->find_4c2060(joan::orig::arg::str(file), static_cast<std::int8_t>(joan::orig::arg::i32(1u))));    // 0x004c2060
            u8* const fx = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(res)));           // 0x0041b440
            void* const fx24 = jo::as_pemi_script(fx)->effect;
            jo::Scene* const model = jo::self<jo::Wall>(this)->actor->scene;
            jo::view<u16>(slot) = joan::orig::arg::as<u16>(joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(model, fx24, img::obj(d::g_pos), jo::self<jo::Wall>(this)->pos, static_cast<std::int8_t>(joan::orig::arg::i32(1u)), joan::orig::arg::i32(v62), 0, 0));  // 0x0041baf0
            jo::Actor* const owner = jo::self<jo::Wall>(this)->actor;
            void* const e = joan::orig::as_dynarray(obj(d::g_particle_effect_array))->ptr_at(static_cast<i32>(slot));  // 0x0041bd60
            last = joan::orig::arg::as<u32>(joan::orig::as_effect(e)->set_owner(joan::orig::arg::mptr(owner), joan::orig::arg::mptr(0u)));  // 0x0041b0d0
        }
        return last;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Tower::update_fire", 0x00490110, &H::tower_fire);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {


JOAN_COUNTER(c_is_idle_4902c0, "Obj::is_idle_4902c0");

struct H {
    // ------------------------------------------------------------------ 0x004902c0
    i32 is_idle_4902c0() {
        c_is_idle_4902c0.hit();
        if (jo::self<jo::Order00>(this)->busy != 0) return 0;
        if (jo::view<u32>(jo::as_item_def(type_record(jo::self<jo::Order00>(this)->type_id))->model_count) == 0) return 0;
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(type_record(jo::self<jo::Order00>(this)->type_id))->model_kinds[0])) == 0 ||
            static_cast<i32>(jo::view<i8>(jo::as_item_def(type_record(jo::self<jo::Order00>(this)->type_id))->model_kinds[0])) == 2) {
            const float v = joan::orig::arg::as<float>(joan::orig::as_unit(this)->time_left());  // 0x0048f8d0
            if (!(v < 0.0f || v > 0.0f)) return 1;
            return 0;
        }
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(type_record(jo::self<jo::Order00>(this)->type_id))->model_kinds[0])) == 1) return 1;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::is_idle_4902c0", 0x004902c0, &H::is_idle_4902c0);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::hook::global;
using joan::hook::x87::Fpu;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::orig::helpers::addr;

void* const kTypes = obj(d::g_item_res);

u8* type_record(u16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

JOAN_COUNTER(c_time_left, "Unit::time_left");

struct H {
    // ------------------------------------------------------------------ 0x0048f8d0
    float time_left() {
        c_time_left.hit();
        if (jo::self<jo::Order00>(this)->actor == nullptr) return global<float>(d::k_9cf5c4);
        const i32 v = global<i32>(d::g_now) - jo::view<i32>(jo::self<jo::Order00>(this)->actor->start_time);
        u8* const rec = type_record(jo::view<u16>(jo::self<jo::Order00>(this)->type_id));
        if (addr(rec) + offsetof(jo::ItemDef, anim_names) == 0) return global<float>(d::k_9cf5c4);  // &rec->anim_names == 0 (integer test, as the original)
        if (jo::as_item_def(rec)->anim_names[jo::view<i32>(jo::self<jo::Order00>(this)->busy)] == nullptr) return global<float>(d::k_9cf5c4);
        const char* const entry = jo::as_item_def(rec)->anim_names[jo::view<i32>(jo::self<jo::Order00>(this)->busy)];
        const i32 k = static_cast<i16>(
            joan::orig::arg::as<u16>(joan::orig::as_anim_def(jo::view<void*>(jo::self<jo::Order00>(this)->actor->anim))->lookup_word(entry, static_cast<std::uint8_t>(joan::orig::arg::u32(1)), jo::view<void*>(jo::self<jo::Order00>(this)->actor))));  // 0x005b4a00
        void* const a = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::self<jo::Order00>(this)->actor->anim))->frame(joan::orig::arg::i32(k), 0u));  // 0x005b5dd0
        const float x = joan::orig::arg::as<float>(joan::orig::as_range(a)->span());                                    // 0x0042d130
        if (!(static_cast<double>(static_cast<u32>(v)) >= static_cast<double>(x))) {
            void* const b = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::self<jo::Order00>(this)->actor->anim))->frame(joan::orig::arg::i32(k), 0u));
            const float x2 = joan::orig::arg::as<float>(joan::orig::as_range(b)->span());
            return static_cast<float>(Fpu{true}.sub(x2, static_cast<double>(v)));
        }
        return global<float>(d::k_9cf5c4);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Unit::time_left", 0x0048f8d0, &H::time_left);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MapObject::release_48f8a0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_03::H::base_4251b0>(this)); }  // 0x48f8a0
auto joan::orig::Unit::time_left() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_04::H::time_left>(this)); }  // 0x48f8d0
