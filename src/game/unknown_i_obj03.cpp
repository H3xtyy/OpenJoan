// game/unknown_i_obj03.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_obj03.cpp;
// re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
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
using joan::hook::x87::Fpu;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using i16 = std::int16_t;

using joan::hook::x87::ftol;

using i8 = std::int8_t;

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
    void stub4(u32) {}

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
B("Obj::stub_48ec30", 0x0048ec30, &H::stub0);
B("Obj::stub_48ec80", 0x0048ec80, &H::stub0);
B("Obj::stub_48eca0", 0x0048eca0, &H::stub0);
B("Obj::stub_48ecf0", 0x0048ecf0, &H::stub4);
B("Obj::stub_48ed10", 0x0048ed10, &H::stub4);
B("Obj::self_48f5d0", 0x0048f5d0, &H::self0);
B("Obj::scalar_dtor_48eac0", 0x0048eac0, &H::scalar_dtor<fn::Obj48e_dtor>);  // 0x0048eb10
B("init_48e920", 0x0048e920, &init_call<fn::init_a39c0c>);
B("init_48e980", 0x0048e980, &init_call<fn::init_a39c10>);
B("init_48e9d0", 0x0048e9d0, &init_call<fn::init_a39c14>);
B("init_48ea20", 0x0048ea20, &init_call<fn::init_a39c18>);
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

float pi_value() {
    constexpr Fpu fpu{true};
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(fpu.mul(a, global<double>(d::dbl_4)));
}

void JOAN_CDECL init_a39c0c() { global<float>(d::g_a39c0c) = pi_value(); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a39c0c", 0x0048e940, &init_a39c0c);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

// ---------------------------------------------------------------- static initialisers
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a39c10() { div_global(d::g_a39c10, d::g_a39c0c, d::flt_2); }  // 0x0048e9a0

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_a39c10", 0x0048e9a0, &init_a39c10);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

void JOAN_CDECL init_a39c14() { global<float>(d::g_a39c14) = global<float>(d::g_a39c0c) / global<float>(d::flt_3); }

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("init_a39c14", 0x0048e9f0, &init_a39c14);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::orig::helpers::memset_;  // memset

// Order constructors: base 0x00416a50, vtable, memset(this + 0xc2, 0, n).
using joan::orig::helpers::order_ctor;

struct H {
    void* order_48ea70() { return order_ctor(this, d::vt_Obj_9d397c, static_cast<u32>(sizeof(jo::Order0c_c) - sizeof(jo::MapObject))); }  // 0xc
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order0c_c::ctor", 0x0048ea70, &H::order_48ea70);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

struct H {
    u32 dtor_48eb10() {
        jo::self<jo::Order0c_c>(this)->vtable = img::obj(d::vt_Obj_9d397c);
        joan::orig::as_map_object(this)->on_remove();         // 0x0048ec00
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->destruct());  // 0x00415790
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Obj48e::~Obj48e", 0x0048eb10, &H::dtor_48eb10);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

void* const kTypes = obj(d::g_item_res);
void* const kUnits = obj(d::g_units);

u8* type_record(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0

JOAN_COUNTER(c_consume_48edf0, "Obj::consume_48edf0");

struct H {
    // ------------------------------------------------------------------ 0x0048eb80
    u8* init_48eb80(u32 type) {
        joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type));  // 0x004178e0
        u8* const t = type_record(static_cast<i16>(type));
        jo::self<jo::Order0c_c>(this)->u32_c6[0] = jo::view<u32>(jo::as_item_def(t)->i32_5e);
        jo::view<u32>(jo::self<jo::Order0c_c>(this)->durability) = jo::self<jo::Order0c_c>(this)->u32_c6[0];
        jo::self<jo::Order0c_c>(this)->busy = jo::view<u32>(jo::as_item_def(t)->model_count);
        return t;
    }

    // ------------------------------------------------------------------ 0x0048ed60
    float consume_48ed60(u32 id, u32 /*unused*/) {
        const Fpu fpu{true};
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        const float amount = joan::orig::arg::as<float>(joan::orig::as_unit(u)->attack_value_5104f0());      // 0x005104f0
        const float d = static_cast<float>(fpu.mul(amount, global<float>(d::flt_0_5)));  // 0.5
        jo::self<jo::Order0c_c>(this)->durability = ftol(fpu.sub(static_cast<double>(jo::self<jo::Order0c_c>(this)->durability), d));
        if (jo::self<jo::Order0c_c>(this)->durability < 0) jo::self<jo::Order0c_c>(this)->durability = 0;
        return d;
    }

    // ------------------------------------------------------------------ 0x0048edf0
    float consume_48edf0(u32 id, u32 /*unused*/) {
        c_consume_48edf0.hit();
        const Fpu fpu{true};
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        const float amount = joan::orig::arg::as<float>(joan::orig::as_unit(u)->defence_value());      // 0x00510bf0
        const float d = static_cast<float>(fpu.mul(amount, global<float>(d::flt_0_5)));  // 0.5
        const double v = fpu.sub(static_cast<double>(jo::self<jo::Order0c_c>(this)->durability), d);
        float left;
        if (!(v >= static_cast<double>(global<float>(d::k_9cf5c4)))) {  // test ah,1: below or unordered
            left = 0.0f;
        } else {
            left = static_cast<float>(fpu.sub(static_cast<double>(jo::self<jo::Order0c_c>(this)->durability), d));
        }
        jo::self<jo::Order0c_c>(this)->durability = ftol(left);
        return d;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::init_48eb80", 0x0048eb80, &H::init_48eb80);
B("Obj::consume_48ed60", 0x0048ed60, &H::consume_48ed60);
B("Obj::consume_48edf0", 0x0048edf0, &H::consume_48edf0);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03 {

struct H {
    // ------------------------------------------------------------------ 0x0048c820 / 0x0048ecc0
    u32 base_419950() { return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_419950()); }  // 0x00419950

    // ------------------------------------------------------------------ 0x0048ec00
    u32 reset_48ec00() {
        joan::orig::as_map_object(this)->refresh_48f360(0);         // 0x0048f360
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_4251b0());  // 0x004251b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::reset_48ec00", 0x0048ec00, &H::reset_48ec00);
B("Obj::base_48ecc0", 0x0048ecc0, &H::base_419950);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

struct H {
    // ------------------------------------------------------------------ 0x0048ec50
    u32 refresh_48ec50() {
        joan::orig::as_unit(this)->create_sprite();            // 0x0042f210
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->refresh_48f360(1));  // 0x0048f360
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Obj48ec50::refresh", 0x0048ec50, &H::refresh_48ec50);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct H {
    u32 call_41d280() { return joan::orig::arg::as<u32>(joan::orig::as_timer(this)->remaining_41d280()); }                   // 0x0041d280
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("X::call_41d280", 0x0048ed30, &H::call_41d280);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

void* const kTypes = obj(d::g_items);

JOAN_COUNTER(c_drain_type, "Res::drain_type");

struct H {
    // ------------------------------------------------------------------------ 0x0048c990
    float apply_drain(float c) {
        const Fpu fpu{true};
        float v;
        const double d = fpu.sub(static_cast<double>(jo::self<jo::Order0c_c>(this)->durability), c);
        if (!(d >= 0.0)) {
            v = 0.0f;
        } else {
            v = static_cast<float>(fpu.sub(static_cast<double>(jo::self<jo::Order0c_c>(this)->durability), c));
        }
        jo::self<jo::Order0c_c>(this)->durability = ftol(v);
        return c;
    }

    // ------------------------------------------------------------------------ 0x0048ee90
    float drain_type(i32 id, u32) {
        c_drain_type.hit();
        if (joan::orig::as_units(kTypes)->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0.0f;  // 0x00425d50
        void* const e = joan::orig::as_units(kTypes)->list.ptr_at(joan::orig::arg::i32(id));      // 0x00425d20
        const float c = static_cast<float>(jo::view<i32>(jo::as_map_object(e)->damage));  // e: a MapObject of the g_a460a0 list
        return apply_drain(c);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Res::drain_type", 0x0048ee90, &H::drain_type);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b101_03.cpp (game/b101_03)
namespace {
namespace game_b101_03 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

bool eq_or_nan(double v) { return !(v < 0.0 || v > 0.0); }  // fcomp 0 / test ah,0x40 jnz

void* const kTypes = obj(d::g_item_res);

u32 now() { return global<u32>(d::g_now); }

JOAN_COUNTER(c_on_event, "Obj::on_event");

struct H {
    // 0x0048ef50 helper: 0x005abe30 on +0x86 with the type's +0x87 entry, then time / -0.2.
    void start_anim(u8* t) {
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Order0c_c>(this)->active)) == 0 || jo::self<jo::Order0c_c>(this)->actor == nullptr) return;
        const u32 time = now();
        const char* const id = jo::as_item_def(t)->anim_names[jo::view<i32>(jo::self<jo::Order0c_c>(this)->busy)];
        jo::AnimDef* const a4 = jo::self<jo::Order0c_c>(this)->actor->anim;
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Order0c_c>(this)->actor))->start_anim(joan::orig::arg::mptr(a4), id, joan::orig::arg::u32(time), 0u, 0u, 0u);  // 0x005abe30
        jo::self<jo::Order0c_c>(this)->actor->fade_38 = now();
        jo::view<u32>(jo::self<jo::Order0c_c>(this)->actor->fade) = 0xbe4ccccdu;  // -0.2f
    }
    // Emitter k (2 / 3) of the item type: script id, position, scale (as the sound argument).
    void play_sound(u8* t, i32 k) {
        if (jo::view<u32>(jo::as_item_def(t)->emitter_scripts[k]) == 0) return;
        const u32 arg = jo::view<u32>(jo::as_item_def(t)->emitter_scale[k]);
        void* const pos = jo::as_map_object(this)->pos;
        void* const src = &jo::as_item_def(t)->emitter_pos[k * 3];
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(t)->emitter_scripts[k]))));  // 0x0041b440
        void* const r24 = jo::as_pemi_script(r)->effect;
        jo::Scene* const h = jo::self<jo::Order0c_c>(this)->actor->scene;
        joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(h, r24, src, pos, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(arg), 0, 0x1388);  // 0x0041baf0
    }

    // ------------------------------------------------------------------ 0x0048ef50
    u8 on_event(i16 ev) {
        c_on_event.hit();
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(static_cast<i32>(ev))));  // 0x00485f20
        if (jo::view<u32>(jo::as_save_entry(e)->type) != 0x4000) return 1;
        if (jo::as_save_entry(e)->ref_kind != 2) return 1;
        const float v = joan::orig::arg::as<float>(joan::orig::as_res(this)->drain_type(static_cast<i32>(jo::view<i16>(jo::as_save_entry(e)->ref_id)), 0u));  // 0x0048ee90
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Order0c_c>(this)->type_id)));  // 0x00425ce0
        if (jo::self<jo::Order0c_c>(this)->busy == 0) return 1;
        if (eq_or_nan(static_cast<double>(v))) return 1;
        if (jo::self<jo::Order0c_c>(this)->durability > jo::as_item_def(t)->model_values[jo::view<i32>(jo::self<jo::Order0c_c>(this)->busy) - 1]) return 1;
        const i32 mode = static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[jo::view<i32>(jo::self<jo::Order0c_c>(this)->busy) - 1]));
        if (mode == 0) {
            virtual_call<u32>(this, 0x18, jo::view<i32>(jo::self<jo::Order0c_c>(this)->busy) - 1);
            start_anim(t);
        } else if (mode == 1) {
            virtual_call<u32>(this, 0x18, jo::view<i32>(jo::self<jo::Order0c_c>(this)->busy) - 1);
        } else if (mode == 2) {
            virtual_call<u32>(this, 0x18, jo::view<i32>(jo::self<jo::Order0c_c>(this)->busy) - 1);
            start_anim(t);
        } else {
            return 1;
        }
        if (jo::self<jo::Order0c_c>(this)->busy == 0 && jo::view<u32>(jo::as_item_def(t)->model_count) != 0 && jo::view<u32>(jo::self<jo::Order0c_c>(this)->durability) == 0) {
            joan::orig::as_map_object(this)->refresh_48f360(0);  // 0x0048f360
        }
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Order0c_c>(this)->active)) == 0 || jo::self<jo::Order0c_c>(this)->actor == nullptr) return 1;
        play_sound(t, 2);
        play_sound(t, 3);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_03", name, addr, fn)
B("Obj::on_event", 0x0048ef50, &H::on_event);
#undef B

}  // namespace game_b101_03
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

double crt_fabs(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }

void* const kTypes = obj(d::g_item_res);

JOAN_COUNTER(c_morale, "Army::morale_term");
JOAN_COUNTER(c_can_act, "Unit::can_act");

struct H {
    // ------------------------------------------------------------------ 0x0048f290
    float morale_term(i32 k) {
        c_morale.hit();
        const Fpu fpu{true};
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(k)));  // 0x004b42e0
        float a = static_cast<float>(fpu.div(static_cast<double>(jo::as_relation(rec)->opinion), static_cast<double>(global<float>(d::flt_500))));
        if (!(a >= global<float>(d::k_9cf5c4))) a = a / global<float>(d::flt_2);
        const float s = joan::orig::arg::as<float>(joan::orig::as_nation(this)->year_value());  // 0x004b52f0
        const float b = s / (jo::self<jo::Nation>(this)->goods * jo::self<jo::Nation>(this)->production + global<float>(d::flt_1));
        u8* const rec2 = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(k)));  // 0x004b42e0
        const float v1 = joan::orig::arg::as<float>(joan::orig::as_relation(rec2)->day_value());    // 0x004b8ed0
        const float v2 = joan::orig::arg::as<float>(joan::orig::as_relation(rec2)->days_value());    // 0x004b8e80
        const float c = (v1 - v2) / (jo::self<jo::Nation>(this)->goods * jo::self<jo::Nation>(this)->price_trend + global<float>(d::flt_1));
        float r = jo::self<jo::Nation>(this)->traits[2] * global<float>(d::flt_0_5);
        r = r + global<float>(d::flt_0_25);
        r = r + a;
        r = r + b;
        r = r - c;
        return r;
    }

    // ------------------------------------------------------------------ 0x0048f380
    // Timer block (+0x86 present): 1 / 0 result, or -1 for "go on to 0x48ed30".
    i32 timer_due() {
        u8* const q = jo::view<u8*>(jo::self<jo::Order0c_c>(this)->actor);
        const float period = jo::as_actor(q)->fade;
        const float zero = global<float>(d::k_9cf5c4);
        if (!(period == zero || period != period)) {  // fcomp; test ah,0x40 jne: equal / unordered
            const double elapsed = static_cast<double>(global<u32>(d::g_now) - jo::as_actor(q)->fade_38);
            const double r = Fpu{true}.div(global<double>(d::dbl_1000), crt_fabs(static_cast<double>(period)));
            if (r >= elapsed) return 0;  // test ah,1 je: neither less nor unordered
        }
        const float v = joan::orig::arg::as<float>(joan::orig::as_map_object(this)->time_left());  // 0x0048ed30
        if (v == zero || v != v) return 1;           // test ah,0x40 je -> 0
        return 0;
    }
    u32 can_act() {
        c_can_act.hit();
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Order0c_c>(this)->type_id)));  // 0x00425ce0
        if (jo::self<jo::Order0c_c>(this)->busy != 0) return 0;
        if (jo::view<u32>(jo::as_item_def(type)->model_count) == 0) return 0;
        if (jo::view<u32>(jo::self<jo::Order0c_c>(this)->durability) != 0) return 0;
        const i32 mode = jo::view<i8>(jo::as_item_def(type)->model_kinds[0]);
        if (mode == 0) {
            if (jo::self<jo::Order0c_c>(this)->actor == nullptr) return 1;
            return static_cast<u32>(timer_due());
        }
        if (mode == 1) return 1;
        if (mode == 2) {
            if (jo::self<jo::Order0c_c>(this)->actor == nullptr) return 1;
            return static_cast<u32>(timer_due());
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Army::morale_term", 0x0048f290, &H::morale_term);
B("Unit::can_act", 0x0048f380, &H::can_act);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

struct H {
    // 0x0048f360
    std::int32_t always1(std::int32_t) { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("always_1_48f360", 0x0048f360, &H::always1);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::hook::global;

static void JOAN_CDECL init_a39c18() { global<float>(d::g_a39c18) = global<float>(d::g_a39c0c) / global<float>(d::flt_4); }  // 0x0048ea40

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("init_a39c18", 0x0048ea40, &init_a39c18);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MapObject::on_remove() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_03::H::reset_48ec00>(this)); }  // 0x48ec00
auto joan::orig::MapObject::time_left() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_02::H::call_41d280>(this)); }  // 0x48ed30
auto joan::orig::Res::drain_type(std::int32_t a0, std::uint32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b99_03::H::drain_type>(this, a0, a1)); }  // 0x48ee90
auto joan::orig::Army::morale_term(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b101_04::H::morale_term>(this, a0)); }  // 0x48f290
auto joan::orig::MapObject::refresh_48f360(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::always1>(this, a0)); }  // 0x48f360
