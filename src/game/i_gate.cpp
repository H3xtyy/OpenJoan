// game/i_gate.cpp: functions of the original d:\ww\i_gate.cpp (re/attribution.csv).
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
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cmath>
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::Fpu;

using joan::call::cdecl_call;

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
//
//    - Gate::on_remove 0x0048c410 (ret 0, vtable 0x009d388c slot 1, returns 0x004251b0's eax),
//      Gate::update 0x0048c4d0 (slot 3, void: no direct callers; the zero-argument slot-3 call
//      sites are the object update loop 0x004afb90 (eax ignored) and self-calls of unrelated
//      classes (vtables 0x009d5d68 / 0x009d8f08 / 0x009d99b0 / script nodes); the only slot-3
//      sites that read eax are COM / D3DX, script nodes (1 argument) and the model loader
//      0x005e189e (material objects); the sibling overrides 0x0048ec30 / 0x004903b0 are empty),
//      Gate::toggle 0x0048c670 (slot 4, eax reproduced on every path).
//
//  - 0x0048c4d0: 0x0048c890 returns st0 (fisub result), taken as double for the "> 0.0" tests.
namespace {
namespace game_b108_05 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_gate_remove, "Gate::on_remove_48c410");
JOAN_COUNTER(c_gate_update, "Gate::update_48c4d0");
JOAN_COUNTER(c_gate_toggle, "Gate::toggle_48c670");

struct H {
    // ------------------------------------------------------------------ 0x0048c410
    u32 gate_on_remove() {
        c_gate_remove.hit();
        if (jo::self<jo::Gate>(this)->side == 0) {
            joan::orig::as_gate(this)->update_48d4b0(joan::orig::arg::u32(i32{1}));  // 0x0048d4b0
        } else if (jo::self<jo::Gate>(this)->side == 1) {
            joan::orig::as_gate(this)->update_48d4b0(joan::orig::arg::u32(i32{0}));  // 0x0048d4b0
        }
        if (jo::view<u32>(jo::self<jo::Gate>(this)->blocked_count) != 0) {
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::self<jo::Gate>(this)->blocked_cells), str(d::s_d_ww_i_gate_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a006dc)) + 9));  // 0x0058ed70
            jo::self<jo::Gate>(this)->blocked_cells = nullptr;
            jo::view<u32>(jo::self<jo::Gate>(this)->blocked_count) = 0;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_4251b0());  // 0x004251b0
    }

    // 0x005abe30 (thunk 0x0040407a, ret 0x18) on the +0x86 sound / animation object.
    void gate_event(img::Addr name) {
        void* const a = jo::self<jo::Gate>(this)->actor;
        joan::orig::as_actor(a)->start_anim(jo::as_actor(a)->anim, str(name), joan::orig::arg::u32(global<u32>(d::g_now)), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));
    }

    // ------------------------------------------------------------------ 0x0048c4d0
    void gate_update() {
        c_gate_update.hit();
        const i32 state = jo::self<jo::Gate>(this)->state;
        if (state == 2) {
            const double r = joan::orig::arg::as<double>(joan::orig::as_gate(this)->time_left());  // 0x0048c890
            if (!(r > 0.0)) jo::view<u8>(jo::self<jo::Gate>(this)->state) = 0;
        } else if (jo::self<jo::Gate>(this)->state == 3) {
            const double r = joan::orig::arg::as<double>(joan::orig::as_gate(this)->time_left());  // 0x0048c890
            if (!(r > 0.0)) {
                joan::orig::as_gate(this)->update_48d4b0(joan::orig::arg::u32(i32{0}));  // 0x0048d4b0
                jo::view<u8>(jo::self<jo::Gate>(this)->state) = 1;
            }
        } else if (jo::self<jo::Gate>(this)->state == 4) {
            if (joan::orig::arg::as<i32>(joan::orig::as_gate(this)->update_48d4b0(joan::orig::arg::u32(i32{1}))) != 0) {  // 0x0048d4b0
                if (jo::self<jo::Gate>(this)->actor != nullptr) gate_event(d::s_f_close);  // "f_close"
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
                if (jo::view<u32>(jo::as_item_def(e)->sounds[0]) != 0) {
                    Vec3 pos;
                    std::memcpy(&pos, jo::self<jo::Gate>(this)->pos, sizeof pos);
                    void* const e2 =
                        joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
                    joan::orig::as_sound_list(obj(d::g_sfx_player))->play_at(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(e2)->sounds[0])), pos);  // 0x004950a0
                }
                jo::view<u8>(jo::self<jo::Gate>(this)->state) = 2;
            }
        }
    }

    // ------------------------------------------------------------------ 0x0048c670
    i32 gate_toggle() {
        c_gate_toggle.hit();
        joan::orig::as_unit(this)->create_sprite();  // 0x0042f210
        if (jo::self<jo::Gate>(this)->state == 1) {
            if (jo::self<jo::Gate>(this)->actor != nullptr) gate_event(d::s_f_opened);  // "f_opened"
            return joan::orig::arg::as<i32>(joan::orig::as_gate(this)->update_48d4b0(joan::orig::arg::u32(i32{0})));        // 0x0048d4b0
        }
        const i32 state = jo::self<jo::Gate>(this)->state;
        if (state != 0) return state;
        if (jo::self<jo::Gate>(this)->actor != nullptr) gate_event(d::s_f_closed);  // "f_closed"
        return joan::orig::arg::as<i32>(joan::orig::as_gate(this)->update_48d4b0(joan::orig::arg::u32(i32{1})));        // 0x0048d4b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Gate::on_remove_48c410", 0x0048c410, &H::gate_on_remove);
B("Gate::update_48c4d0", 0x0048c4d0, &H::gate_update);
B("Gate::toggle_48c670", 0x0048c670, &H::gate_toggle);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
//
//    - Obj::stop_48c760 0x0048c760 (ret 0), vtable 0x9d38b8: eax of every path reproduced.
namespace {
namespace game_b102_03 {

using joan::orig::helpers::addr;

void* const kMarkers = obj(d::g_physic_sys);

JOAN_COUNTER(c_stop_48c760, "Obj::stop_48c760");

struct H {
    // ------------------------------------------------------------------ 0x0048c760
    std::uintptr_t stop_48c760() {
        c_stop_48c760.hit();
        std::uintptr_t eax;
        if (static_cast<i32>(jo::self<jo::Gate>(this)->state) != 1) {
            eax = static_cast<u32>(static_cast<i32>(jo::self<jo::Gate>(this)->state));
            if (static_cast<i32>(eax) != 4) return eax;
        } else {
            eax = addr(this);
        }
        if (jo::self<jo::Gate>(this)->actor == nullptr) return eax;
        eax = joan::orig::arg::as<std::uintptr_t>(jo::self<jo::Gate>(this)->actor);
        jo::self<jo::Gate>(this)->actor->aim_mode = 0;
        if (jo::view<u32>(jo::self<jo::Gate>(this)->actor->record) == 0) return eax;
        return joan::orig::arg::as<u32>(joan::orig::as_players(kMarkers)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Gate>(this)->actor->record)), joan::orig::arg::i16(0)));  // 0x006ad980
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::stop_48c760", 0x0048c760, &H::stop_48c760);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
//
//    - empty thiscall stubs (never write eax; the C++ versions leave eax untouched as well):
//        ret 0: 0x00481360, 0x00481450, 0x00481690, 0x00481780, 0x00481870, 0x00481a50,
//               0x0048bf20, 0x0048bf40, 0x0048bf60, 0x0048bf80, 0x0048c800, 0x0048ec30,
//               0x0048ec80, 0x0048eca0, 0x004903b0 (vtable slots in 0x9d37f8..0x9d3da8),
//               0x00486400 (only caller 0x0048639f, eax overwritten with 1 afterwards),
//               0x004876b0 (only caller 0x00470f28 on 0xa3ce90, eax unused),
//               0x00481960, 0x00488370 (no references: dead code; the .text "hits" of 0x407883
//               found by findvt are misaligned instruction bytes, not references).
//        ret 4: 0x0048bfa0, 0x0048bfc0, 0x0048c850, 0x0048c870, 0x0048ecf0, 0x0048ed10
//               (slots 0x3c / 0x40 of the vtables 0x9d37cc / 0x9d388c / 0x9d397c / 0x9d3a18 /
//               0x9d3c88 / 0x9d3d78; every override in the hierarchy - 0x00490700, 0x00490750,
//               0x00491da0, 0x00491dc0, 0x00492480, 0x004924a0 - is empty too, so the virtual is
//               void and no caller reads eax. An -O0 build may load the unused argument into eax.)
namespace {
namespace game_b105_04 {

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}
    void stub4(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
B("Obj::stub_48c800", 0x0048c800, &H::stub0);
B("Obj::stub_48c850", 0x0048c850, &H::stub4);
B("Obj::stub_48c870", 0x0048c870, &H::stub4);
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
//
//    - Obj::base_48c820 / base_48ecc0 0x0048c820 / 0x0048ecc0 (ret 0): 0x00419950, eax passed through.
namespace {
namespace game_b103_03 {

struct H {
    // ------------------------------------------------------------------ 0x0048c820 / 0x0048ecc0
    u32 base_419950() { return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_419950()); }  // 0x00419950
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Obj::base_48c820", 0x0048c820, &H::base_419950);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x0048c890 Obj::time_left          (thiscall, ret 0, st0) - forwards 0x0041d280's st0.
//
//      0x0048ca30 Obj::take_c2            (thiscall(id, unused), ret 8, float)
//
//  - 0x0048c890 returns 0x0041d280's st0 unchanged (taken and returned as double).
//
//  - 0x0048ca30 / 0x00492bb0: int - float subtraction at PC24 via Fpu, _ftol truncation.
namespace {
namespace game_b94_02 {

void* const kUnits = obj(d::g_items);

JOAN_COUNTER(c_take_c2, "Obj::take_c2");

// Shared body of 0x0048ca30 / 0x00492bb0: moves the unit's +0x72 amount out of `field`.
float take_amount(i32& field, i32 id) {
    if (joan::orig::as_units(kUnits)->list.empty_at(joan::orig::arg::i32(id)) != 0) return global<float>(d::k_9cf5c4);  // 0x00425d50
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    const float amount = static_cast<float>(jo::view<i32>(jo::as_map_object(unit)->damage));
    const Fpu fpu{true};
    const float diff = static_cast<float>(fpu.sub(static_cast<double>(field), static_cast<double>(amount)));
    float v;
    if (!(diff >= global<float>(d::k_9cf5c4))) {  // fcomp, test ah,1: below or unordered -> 0
        v = 0.0f;
    } else {
        v = static_cast<float>(fpu.sub(static_cast<double>(field), static_cast<double>(amount)));
    }
    field = joan::hook::x87::ftol(static_cast<double>(v));
    return amount;
}

struct H {
    // ------------------------------------------------------------------------ 0x0048c890
    double time_left() { return joan::orig::arg::as<double>(joan::orig::as_timer(this)->remaining_41d280()); }  // 0x0041d280 (st0 passed through)

    // ------------------------------------------------------------------------ 0x0048ca30
    float take_c2(i32 id, i32) {
        c_take_c2.hit();
        return take_amount(jo::self<jo::Gate>(this)->hp, id);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::time_left", 0x0048c890, &H::time_left);
B94("Obj::take_c2", 0x0048ca30, &H::take_c2);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x0048c8c0 Unit::take_damage(id, -)     (thiscall, ret 8)    - float return (st0).
//
//  - 0x0048c8c0: 0x005104f0 returns a float loaded from memory (exact), * 0.5f / * 3.0f are float
//    ops; fild int - float uses Fpu{true}. Second stack arg unused.
namespace {
namespace game_b96_02 {

JOAN_COUNTER(c_take_damage, "Unit::take_damage");

struct H {
    // ------------------------------------------------------------------------ 0x0048c8c0
    float take_damage(u32 id, u32 /*unused*/) {
        c_take_damage.hit();
        void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        float r = joan::orig::arg::as<float>(joan::orig::as_unit(u)->attack_value_5104f0()) * global<float>(d::flt_0_5);  // 0x005104f0, 0.5f
        const char* const name =
            joan::orig::arg::as<const char*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
        if (joan::crt::stricmp(joan::orig::arg::str(name + 4), str(d::s_LAHIRE_9cfb3c)) == 0)  // _stricmp "LAHIRE"
            r = r * global<float>(d::flt_3);  // 3.0f
        const Fpu fpu{true};
        jo::self<jo::Gate>(this)->hp = joan::hook::x87::ftol(fpu.sub(static_cast<double>(jo::self<jo::Gate>(this)->hp), static_cast<double>(r)));
        if (jo::self<jo::Gate>(this)->hp < 0) jo::self<jo::Gate>(this)->hp = 0;
        return r;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Unit::take_damage", 0x0048c8c0, &H::take_damage);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x0048c990 Res::drain_unit       (thiscall(id, unused), ret 8, float) - c = 0x00510bf0(unit
//                 id) * 0.5; int +0xc2 = ftol(max(+0xc2 - c, 0)); returns c.
namespace {
namespace game_b99_03 {

using joan::hook::x87::ftol;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_drain_unit, "Res::drain_unit");

struct H {
    // ------------------------------------------------------------------------ 0x0048c990
    float apply_drain(float c) {
        const Fpu fpu{true};
        float v;
        const double d = fpu.sub(static_cast<double>(jo::self<jo::Gate>(this)->hp), c);
        if (!(d >= 0.0)) {
            v = 0.0f;
        } else {
            v = static_cast<float>(fpu.sub(static_cast<double>(jo::self<jo::Gate>(this)->hp), c));
        }
        jo::self<jo::Gate>(this)->hp = ftol(v);
        return c;
    }
    float drain_unit(i32 id, u32) {
        c_drain_unit.hit();
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id));   // 0x0041b3c0
        const float c = joan::orig::arg::as<float>(joan::orig::as_unit(u)->defence_value()) * 0.5f;      // 0x00510bf0, fmul 0x9cf77c
        return apply_drain(c);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Res::drain_unit", 0x0048c990, &H::drain_unit);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
//
//    - Hero::on_event_48caf0 0x0048caf0 (ret 4, vtable 0x9d388c slot 18): event dispatch on the
//      event type (0x40/0x2000, 0x4000, 1, 0x20, 0x200); returns al = 1.
//
//  - 0x0048caf0: 0x0041b440 (ret 4) takes only the last push; the rest belong to 0x0041baf0
//    (ret 0x20). The float argument is computed before 0x0041b440 is called, as in the original.
namespace {
namespace game_b108_01 {

// fcomp + fnstsw + test ah,0x40 jnz: equal or unordered.
bool eq_or_unord(float a, float b) { return !(a < b) && !(a > b); }

struct V3 {  // a 12-byte float vector (by value: XMM registers under System V, as the callee declares it)
    float x, y, z;
};

V3 load_v3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

JOAN_COUNTER(c_event, "Hero::on_event_48caf0");

struct H {
    // -------------------------------------------------------------- 0x0048caf0
    // Common tail of the 0x40 / 0x4000 / 1 handlers (from the 0x0048e430 call on).
    // Returns false when 0x0048e430 failed (the handler ends there).
    bool event_trigger(u8* ev, u8* data) {
        if (joan::orig::arg::as<i32>(joan::orig::as_gate(this)->advance_stage(jo::as_trigger(ev)->center)) == 0) return false;  // 0x0048e430
        if (jo::self<jo::Gate>(this)->busy == 0 && jo::view<u32>(jo::as_item_def(data)->model_count) != 0 && jo::view<u32>(jo::self<jo::Gate>(this)->hp) == 0) {
            if (jo::view<u32>(jo::as_item_def(data)->sounds[2]) != 0) {
                const V3 v = load_v3(jo::self<jo::Gate>(this)->pos);
                joan::orig::as_sound_list(obj(d::g_sfx_player))->play_at(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(data)->sounds[2])), v);  // 0x004950a0
            }
            joan::orig::as_gate(this)->update_48d4b0(joan::orig::arg::u32(i32{0}));  // 0x0048d4b0
        }
        return true;
    }
    void event_toggle() {
        if (jo::self<jo::Gate>(this)->state == 0) {
            joan::orig::as_gate(this)->set_door_state(static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})));  // 0x0048d2b0
        } else if (jo::self<jo::Gate>(this)->state == 1) {
            joan::orig::as_gate(this)->set_door_state(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x0048d2b0
        }
    }
    void event_notify(u8* ev) {
        if (static_cast<i32>(jo::as_trigger(ev)->ref_id) == static_cast<i32>(global<i16>(d::g_a47c36))) {
            joan::orig::as_hud(obj(d::g_player_hud))->set_unit_portrait(static_cast<i32>(jo::self<jo::Gate>(this)->portrait_unit));  // 0x006fe340
        }
    }

    u8 on_event(i32 id_arg) {
        c_event.hit();
        u8* const ev = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(id_arg)))));  // 0x00485f20
        const i32 kind = jo::view<i32>(jo::as_trigger(ev)->flags);
        const float zero = global<float>(d::k_9cf5c4);
        if (kind == 0x40 || kind == 0x2000) {
            jo::self<jo::Gate>(this)->attacked = 1;
            if (jo::view<i8>(jo::self<jo::Gate>(this)->invincible) != 0) return 1;
            if (jo::as_trigger(ev)->ref_kind == 1 && jo::self<jo::Gate>(this)->state == 0) {
                const float f = joan::orig::arg::as<float>(joan::orig::as_res(this)->drain_unit(static_cast<i32>(jo::as_trigger(ev)->ref_id), joan::orig::arg::u32(i32{0})));  // 0x0048c990
                u8* const data = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
                if (jo::self<jo::Gate>(this)->busy != 0 && !eq_or_unord(f, zero)) {
                    if (!event_trigger(ev, data)) return 1;
                    event_notify(ev);
                }
            }
        } else if (kind == 0x4000) {
            jo::self<jo::Gate>(this)->attacked = 1;
            if (jo::view<i8>(jo::self<jo::Gate>(this)->invincible) != 0) return 1;
            if (jo::as_trigger(ev)->ref_kind == 2 && jo::self<jo::Gate>(this)->state == 0) {
                const float f = joan::orig::arg::as<float>(joan::orig::as_gate(this)->take_amount(static_cast<i32>(jo::as_trigger(ev)->ref_id), i32{0}));  // 0x0048ca30
                u8* const data = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
                if (jo::self<jo::Gate>(this)->busy != 0 && !eq_or_unord(f, zero)) {
                    event_trigger(ev, data);
                }
            }
        } else if (kind == 1) {
            jo::self<jo::Gate>(this)->attacked = 1;
            if (jo::view<i8>(jo::self<jo::Gate>(this)->invincible) != 0) return 1;
            if (jo::as_trigger(ev)->ref_kind != 1 || jo::self<jo::Gate>(this)->state != 0) return 1;
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(this)->can_reach_49a010(joan::orig::arg::u32(static_cast<i32>(jo::as_trigger(ev)->ref_id)), joan::orig::arg::u32(i32{1}))) == 0) return 1;  // 0x0049a010
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(jo::as_trigger(ev)->ref_id)));  // 0x0041b3c0
            u8* const t1 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
            if (joan::crt::stricmp(joan::orig::arg::str(jo::as_unit_type(t1)->name), str(d::s_LAHIRE_9cfb3c)) != 0) {
                u8* const t2 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
                if (static_cast<i32>(jo::as_unit_type(t2)->kind) != 0xb) return 1;
            }
            const float f = joan::orig::arg::as<float>(joan::orig::as_unit(this)->take_damage(joan::orig::arg::u32(static_cast<i32>(jo::as_trigger(ev)->ref_id)), joan::orig::arg::u32(i32{0})));  // 0x0048c8c0
            u8* const data = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
            if (jo::self<jo::Gate>(this)->busy == 0 || eq_or_unord(f, zero)) return 1;
            if (!event_trigger(ev, data)) return 1;
            event_notify(ev);
            u8* const t3 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
            if (joan::crt::stricmp(joan::orig::arg::str(jo::as_unit_type(t3)->name), str(d::s_LAHIRE_9cfb3c)) != 0) return 1;
            u8* const d2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
            if (jo::view<u32>(jo::as_item_def(d2)->sounds[1]) != 0) {
                const V3 v = load_v3(jo::self<jo::Gate>(this)->pos);
                u8* const d3 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
                joan::orig::as_sound_list(obj(d::g_sfx_player))->play_at(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(d3)->sounds[1])), v);  // 0x004950a0
            }
            if (jo::view<u32>(jo::as_item_def(data)->emitter_scripts[3]) != 0) {
                const float fl = jo::as_item_def(data)->emitter_scale[3] + jo::as_unit(unit)->heading;
                u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(data)->emitter_scripts[3]))));  // 0x0041b440
                void* const o24 = jo::as_pemi_script(o)->effect;
                joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(jo::as_actor(jo::self<jo::Gate>(this)->actor)->scene, o24, // 0x0041baf0
                              &jo::as_item_def(data)->emitter_pos[9], jo::as_trigger(ev)->center, static_cast<std::int8_t>(joan::orig::arg::i32(u8{1})), joan::orig::arg::bits(fl), i32{0}, i32{0x1388});
            }
        } else if (kind == 0x20) {
            event_toggle();
        } else if (kind == 0x200) {
            if (jo::self<jo::Gate>(this)->interact_trigger == 0) return 1;
            if (jo::as_trigger(ev)->ref_kind != 1) return 1;
            if (joan::orig::arg::as<i8>(joan::orig::as_map_object(this)->can_target(static_cast<i32>(jo::as_trigger(ev)->ref_id))) == 0) return 1;  // 0x00428600
            if (jo::view<i8>(jo::self<jo::Gate>(this)->need_key) == 0) event_toggle();
            if (jo::self<jo::Gate>(this)->state == 0) {
                u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(jo::as_trigger(ev)->ref_id)));  // 0x0041b3c0
                jo::view<u16>(jo::as_unit(unit)->interact_item) = jo::view<u16>(jo::self<jo::Gate>(this)->index);
            }
        }
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Hero::on_event_48caf0", 0x0048caf0, &H::on_event);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
//
//      0x0048d2b0 Door::set_state(char)             (thiscall, ret 4) - "f_open" / "f_close".
//
//  - 0x0048d2b0 / 0x0060ffd0 / 0x007380b0 / 0x00456900: the value left in eax is returned
//    (0x0048d2b0's and 0x007380b0's callers return `mov al, 1` on top of it).
namespace {
namespace game_b108_18 {

using joan::orig::helpers::u;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "12-byte by-value vector");

JOAN_COUNTER(c_door_state, "Door::set_state");

struct H {
    // ------------------------------------------------------------------ 0x0048d2b0
    std::uintptr_t notify_sound(const char* event) {  // shared body of the two state branches
        void* const sound = jo::view<void*>(jo::self<jo::Gate>(this)->actor);
        joan::orig::as_actor(sound)->start_anim(jo::as_actor(sound)->anim, event, joan::orig::arg::u32(global<i32>(d::g_now)), 0u, 0u, 0u);  // 0x005abe30
        u8* const owner = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));  // 0x00425ce0
        if (jo::as_item_def(owner)->sounds[0] == 0) return u(owner);
        const Vec3 pos = *reinterpret_cast<const Vec3*>(jo::self<jo::Gate>(this)->pos);  // copied to the argument slot before the getter runs
        u8* const owner2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id))));
        return joan::orig::arg::as<u32>(joan::orig::as_sound_list(obj(d::g_sfx_player))->play_at(joan::orig::arg::i32(jo::as_item_def(owner2)->sounds[0]), pos));  // 0x004950a0
    }
    std::uintptr_t door_set_state(i8 mode) {
        c_door_state.hit();
        if (mode == 1) {
            std::uintptr_t eax = 1;
            if (jo::view<void*>(jo::self<jo::Gate>(this)->actor) != nullptr) eax = notify_sound(str(d::s_f_open));  // "f_open"
            jo::view<u8>(jo::self<jo::Gate>(this)->state) = 3;
            return eax;
        }
        if (mode != 0) return static_cast<u32>(static_cast<i32>(mode));
        if (joan::orig::arg::as<i32>(joan::orig::as_gate(this)->update_48d4b0(1u)) == 0) {  // 0x0048d4b0
            jo::view<u8>(jo::self<jo::Gate>(this)->state) = 4;
            return 0;
        }
        if (jo::view<void*>(jo::self<jo::Gate>(this)->actor) != nullptr) notify_sound(str(d::s_f_close));  // "f_close"
        jo::view<u8>(jo::self<jo::Gate>(this)->state) = 2;
        return u(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Door::set_state", 0x0048d2b0, &H::door_set_state);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b109_16.cpp (game/b109_16)
//
//    - Gate::update_48d4b0 0x0048d4b0 (ret 4, (char flag), i_gate.cpp): returns the 0x006afb10 /
//
//  - 0x0048d4b0: word args pushed as `mov cx, word; push ecx` to 0x00425ce0 / 0x0042d0a0 (word
//    read only); the flag byte pushed with garbage upper bits to 0x006afb10 / 0x006af710 (both
//    read [arg] as movsx byte only). The 0x005ef610 count and the 0x006af710 out pointer are
//    written by the callees before any read.
namespace {
namespace game_b109_16 {

using joan::call::import_call;
using joan::orig::helpers::gf;  // float constant in .rdata

struct Box24 {
    u32 v[6];
};
static_assert(sizeof(Box24) == 24, "box");

const Fpu fpu{true};

JOAN_COUNTER(c_gate, "Gate::update_48d4b0");

struct H {
    // ---- 0x0048d4b0 ---------------------------------------------------------------------------
    // 0x00425ce0 on 0xa58f50 (thunk 0x00409278): gate type record of this+0xa.
    u8* gate_type() { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Gate>(this)->type_id)))); }
    void gate_corner(u8* seg, i32 slot, const jo::PfdPoint* pt, bool top) {
        float v[3];
        v[0] = static_cast<float>(pt->xyz[0] + jo::self<jo::Gate>(this)->cell->origin_x);
        v[1] = static_cast<float>(pt->xyz[1] + jo::self<jo::Gate>(this)->cell->origin_y);
        if (top)
            v[2] = static_cast<float>(pt->xyz[2] + static_cast<i32>(jo::view<i16>(jo::as_item_def(gate_type())->stats[1])) + 0x32);
        else
            v[2] = static_cast<float>(pt->xyz[2] - 0x32);
        joan::orig::as_occluder(seg)->set_corner(joan::orig::arg::i32(slot), joan::orig::arg::mptr(static_cast<void*>(v)));  // 0x00530ee0
    }
    static void gate_edge(u8* seg, i32 dst, i32 a, i32 b) {
        u8 tmp[12];
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::vec_sub(static_cast<void*>(tmp), static_cast<void*>(seg + a), static_cast<void*>(seg + b)));  // 0x0060cc90
        u32 v[3];
        std::memcpy(v, r, 12);
        std::memcpy(seg + dst, v, 12);
    }
    void gate_build(u8* path, i32 handle) {
        jo::TerrainBlock* const g = joan::orig::arg::as<jo::TerrainBlock*>(joan::orig::as_billboard(jo::as_path_space(path)->zones)->item(joan::orig::arg::i32(handle)));  // 0x00637da0
        jo::self<jo::Gate>(this)->blocked_count = g->count;
        jo::view<u8*>(jo::self<jo::Gate>(this)->blocked_cells) = joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::Gate>(this)->blocked_count) << 2), str(d::s_d_ww_i_gate_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a006e0)) + 0x67)));  // 0x0058e7b0
        std::memset(static_cast<void*>(jo::self<jo::Gate>(this)->blocked_cells), i32{0}, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::Gate>(this)->blocked_count) << 2));
        for (i32 j = 0; j < g->count; ++j) {
            const jo::PfdPoint* const pt = g->points[j].item;
            const jo::PfdPoint* const nx = j == g->count - 1 ? g->points[0].item : g->points[j + 1].item;
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_world_cell(jo::self<jo::Gate>(this)->cell)->add_occluder());  // 0x00560c70
            jo::self<jo::Gate>(this)->blocked_cells[j].value = id;
            u8* const seg = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(jo::self<jo::Gate>(this)->cell)->occluder(joan::orig::arg::i32(jo::self<jo::Gate>(this)->blocked_cells[j].value)));  // 0x00560d30
            joan::orig::as_occluder(seg)->set_enabled();  // 0x00530eb0
            gate_corner(seg, 0, pt, false);
            gate_corner(seg, 1, nx, false);
            gate_corner(seg, 2, nx, true);
            gate_corner(seg, 3, pt, true);
            joan::orig::plane_from_points(static_cast<void*>(seg + 0x30), static_cast<void*>(seg), static_cast<void*>(seg + 0xc), static_cast<void*>(seg + 0x18));  // 0x0060d920
            gate_edge(seg, 0x40, 0xc, 0);
            gate_edge(seg, 0x4c, 0x18, 0xc);
            gate_edge(seg, 0x58, 0x24, 0x18);
            gate_edge(seg, 0x64, 0, 0x24);
        }
    }
    void gate_mark(i32 value) {
        for (i32 k = 0; k < jo::self<jo::Gate>(this)->blocked_count; ++k) {
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(jo::self<jo::Gate>(this)->cell)->occluder(joan::orig::arg::i32(jo::self<jo::Gate>(this)->blocked_cells[k].value)));  // 0x00560d30
            jo::as_occluder(o)->enabled = value;
        }
    }
    i32 gate_update(u32 flag_arg) {  // 0x0048d4b0
        c_gate.hit();
        const u8 flag = static_cast<u8>(flag_arg);
        u8* const type = gate_type();
        i32 handle = 0;
        if (jo::view<u32>(jo::as_item_def(type)->not_block) != 0) return 0;
        if (static_cast<i8>(flag) != 0 && jo::self<jo::Gate>(this)->world != nullptr && jo::self<jo::Gate>(this)->world->scene != nullptr) {
            float box[6];
            box[0] = jo::self<jo::Gate>(this)->pos[0] - gf(d::flt_200) - gf(d::flt_100);
            box[1] = jo::self<jo::Gate>(this)->pos[1] - gf(d::flt_200) - gf(d::flt_100);
            box[2] = jo::self<jo::Gate>(this)->pos[2] - gf(d::flt_200) - gf(d::flt_100);
            box[3] = jo::self<jo::Gate>(this)->pos[0] + gf(d::flt_200) + gf(d::flt_100);
            box[4] = jo::self<jo::Gate>(this)->pos[1] + gf(d::flt_200) + gf(d::flt_100);
            box[5] = jo::self<jo::Gate>(this)->pos[2] + gf(d::flt_200) + gf(d::flt_100);
            i32 count = 0;  // written (0) by 0x005ef610 first
            u8** const list = joan::orig::arg::as<u8**>(joan::orig::as_scene(jo::self<jo::Gate>(this)->world->scene)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
            for (i32 i = 0; i < count; ++i) {
                u8* const head = joan::orig::arg::as<u8*>(joan::orig::as_scene_s(list[i])->first_actor());  // 0x005effa0
                u8* node = head;
                if (node == nullptr) continue;
                for (;;) {
                    if (node != jo::view<u8*>(jo::self<jo::Gate>(this)->actor) && jo::as_actor(node)->kind == 0xb) {
                        u8* const unit = static_cast<u8*>(jo::as_actor(node)->owner);
                        bool check = static_cast<i32>(jo::as_unit_type(joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id)))))->unit_class) == 8;  // 0x0042d0a0
                        if (!check) check = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->mounted()) != 0;  // 0x00430190
                        if (!check)
                            check = static_cast<i32>(jo::as_unit_type(joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id)))))->kind) == 2;
                        if (check) {
                            const double ddx = fpu.sub(reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[0], jo::self<jo::Gate>(this)->pos[0]);
                            const double ddy = fpu.sub(reinterpret_cast<const float*>(jo::as_unit(unit)->position_)[1], jo::self<jo::Gate>(this)->pos[1]);
                            const double sum = fpu.add(fpu.mul(ddx, ddx), fpu.mul(ddy, ddy));
                            if (!(sum > static_cast<double>(gf(d::flt_9d3968)))) return 0;
                        }
                    }
                    node = jo::view<u8*>(jo::as_actor(node)->scene_prev);
                    if (node == nullptr || node == head) break;
                }
            }
        }
        const i8 mode = jo::view<i8>(jo::as_item_def(type)->item_usage);
        if (jo::as_item_def(type)->has_bounding_box != 0) {
            u32 m[16];
            if (jo::self<jo::Gate>(this)->actor != nullptr) {
                std::memcpy(m, joan::orig::arg::as<u8*>(joan::orig::as_bone(jo::self<jo::Gate>(this)->actor->bone)->matrix()), 64);  // 0x0060ef80
            } else {
                joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->yaw_)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pitch)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->roll_)));  // 0x0060bf40
                import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(m), static_cast<void*>(jo::self<jo::Gate>(this)->pos));  // multiply_translation
            }
            Box24 b;
            std::memcpy(&b, jo::as_item_def(type)->bounding_box, 24);
            if (mode == 1)
                handle = joan::orig::arg::as<i32>(joan::orig::as_players(obj(d::g_physic_sys))->assign_matches(b, static_cast<void*>(m), i32{1}, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(flag))), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0}))));  // 0x006afb10
            else if (mode == 2)
                handle = joan::orig::arg::as<i32>(joan::orig::as_players(obj(d::g_physic_sys))->assign_matches(b, static_cast<void*>(m), i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(flag))), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0}))));
            else if (mode == 0)
                handle = joan::orig::arg::as<i32>(joan::orig::as_players(obj(d::g_physic_sys))->assign_matches(b, static_cast<void*>(m), i32{-1}, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(flag))), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0}))));
            return handle;
        }
        u8* path = nullptr;  // written (0) by 0x006af710 first
        if (mode == 1)
            handle = joan::orig::arg::as<i32>(joan::orig::as_path(obj(d::g_physic_sys))->link_6af710(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[0])), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[1])), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[2])), i32{1}, joan::orig::arg::i32(static_cast<u32>(flag)), &path));  // 0x006af710
        else if (mode == 2)
            handle = joan::orig::arg::as<i32>(joan::orig::as_path(obj(d::g_physic_sys))->link_6af710(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[0])), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[1])), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[2])), i32{0}, joan::orig::arg::i32(static_cast<u32>(flag)), &path));
        else if (mode == 0)
            handle = joan::orig::arg::as<i32>(joan::orig::as_path(obj(d::g_physic_sys))->link_6af710(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[0])), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[1])), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Gate>(this)->pos[2])), i32{-1}, joan::orig::arg::i32(static_cast<u32>(flag)), &path));
        if (handle == 0) return 0;
        if (static_cast<i8>(flag) != 0) {
            if (path != nullptr && jo::self<jo::Gate>(this)->cell != nullptr) {
                if (jo::self<jo::Gate>(this)->blocked_count != 0)
                    gate_mark(1);
                else
                    gate_build(path, handle);
            }
            if (jo::self<jo::Gate>(this)->actor != nullptr) {
                jo::self<jo::Gate>(this)->actor->aim_mode = jo::as_item_def(type)->collision_type;
                if (jo::view<u32>(jo::self<jo::Gate>(this)->actor->record) != 0)
                    joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Gate>(this)->actor->record)), joan::orig::arg::i16(i32{0}));  // 0x006ad980
            }
        } else {
            if (jo::self<jo::Gate>(this)->cell != nullptr) gate_mark(0);
            if (jo::self<jo::Gate>(this)->actor != nullptr) {
                jo::self<jo::Gate>(this)->actor->aim_mode = 0;
                if (jo::view<u32>(jo::self<jo::Gate>(this)->actor->record) != 0)
                    joan::orig::as_players(obj(d::g_physic_sys))->set_active(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Gate>(this)->actor->record)), joan::orig::arg::i16(i32{0}));  // 0x006ad980
            }
        }
        return handle;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_16", name, addr, fn)
B("Gate::update_48d4b0", 0x0048d4b0, &H::gate_update);
#undef B

}  // namespace game_b109_16
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
//
//      0x0048c2f0 Obj48c2f0::~Obj               (thiscall, ret 0)   - SEH frame, normal path.
//
//  - 0x0048c2f0 / 0x00736120: the SEH frames only guard a callee / new+ctor (delete on throw);
//    normal-path behaviour reproduced.
namespace {
namespace game_b108_13 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x0048c2f0
    void dtor_48c2f0() {
        jo::self<jo::Gate>(this)->vtable = img::obj(d::vt_Obj_9d388c);
        joan::orig::as_gate(this)->on_remove();  // 0x0048c410
        joan::orig::as_map_object(this)->destruct();  // 0x00415790
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Obj48c2f0::dtor", 0x0048c2f0, &H::dtor_48c2f0);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
//
//    - Sound::play_kind 0x00492c40 (id), ret 4 / Sound::play_at 0x004950a0 (id, vec), ret 0x10:
//      free sound slot (0x0045f450 / 0x004cf690) set up from the sound record.
namespace {
namespace game_b90_03 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_play_at, "Sound::play_at");

// Common sound-slot set-up tail of 0x00492c40 / 0x004950a0 (slot calls on *slot).
float sound_volume(const void* self, const void* e) {
    const joan::hook::x87::Fpu fpu{true};
    const double v = fpu.mul(static_cast<double>(jo::as_sound_list(self)->volume), static_cast<double>(jo::as_wave_res(e)->volume));
    return static_cast<float>(fpu.mul(v, static_cast<double>(global<float>(d::flt_0_01))));
}

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004950a0
    std::int32_t play_at(std::int32_t id, Vec3 pos) {
        c_play_at.hit();
        if (static_cast<std::int32_t>(global<std::int8_t>(d::g_game_mode)) == 0xc) return 0;
        if (global<void*>(d::g_sound3d_listener) == nullptr) return 0;
        if (id == 0) return 0;
        std::uint8_t* e;
        if (id > 0x4000)
            e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->at(joan::orig::arg::i16(id)));  // 0x004d0860 (reads the word)
        else
            e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->get(joan::orig::arg::i16(id)));  // 0x004d0820 (reads the word)
        void* const s = joan::orig::arg::as<void*>(joan::orig::as_wave_res(e)->buffer());  // 0x004d09c0
        if (s == nullptr) return 0;
        const std::int16_t idx = joan::orig::arg::as<std::int16_t>(joan::orig::as_party(self())->first_free());  // 0x0045f450
        if (static_cast<std::int32_t>(idx) <= 0) return 0;
        void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(self())->entry(joan::orig::arg::i16(static_cast<std::int32_t>(idx))));  // 0x004cf690
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->queue(joan::orig::arg::mptr(s));                                           // 0x0059f460
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_position(&pos);                                        // 0x0059fb40 (copies 12 bytes)
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_looping(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::as_wave_res(e)->flags & 3))));    // 0x0059fa80
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_gain(joan::orig::arg::bits(sound_volume(self(), e)));                    // 0x0059f700
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_max_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(e)->max_dist)));                 // 0x0059f920
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_reference_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(e)->min_dist)));                 // 0x0059f970
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));                                           // 0x0059fae0
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_rolloff(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(e)->rolloff)));                 // 0x0059f9c0
        joan::orig::as_sound_slot_entry(slot)->set_id(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_wave_res(e)->index))));        // 0x004d0550
        joan::orig::as_sound_slot_entry(slot)->set_key(0u);                                                         // 0x004d06b0
        joan::orig::as_sound(jo::as_sound_slot_entry(slot)->sound)->play();                                              // 0x0059f620
        return static_cast<std::int32_t>(idx);
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Sound::play_at", 0x004950a0, &H::play_at);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b94_02_2.cpp (game/b94_02)
namespace {
namespace game_b94_02_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::import_call;
struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_nearest, "Unit::nearest_enemy");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------------ 0x0048dfb0
    i32 nearest_enemy(i32 range) {
        c_nearest.hit();
        V3 pos;
        joan::orig::as_unit(this)->position(&pos);  // 0x004fedb0
        float best_dist = static_cast<float>(range);
        i32 best = 0;
        if (jo::self<jo::Unit>(this)->cell == nullptr) return 0;
        if (static_cast<i32>(jo::self<jo::Unit>(this)->group_id) == 0) return 0;
        if (jo::self<jo::Unit>(this)->group == nullptr) return 0;
        const i32 level = static_cast<i32>(jo::as_group(jo::self<jo::Unit>(this)->group)->count);
        if (level >= joan::orig::arg::as<i32>(joan::orig::as_unit(this)->morale_level())) return 0;  // 0x00448080
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(this)->dead())) != 0) return 0;  // 0x0042d0e0
        for (u8* p = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(jo::self<jo::Unit>(this)->cell)->get_8c()); p != nullptr; p = jo::view<u8*>(jo::as_unit(p)->cell_prev)) {  // 0x0054ee10
            if (static_cast<i32>(jo::as_unit(p)->side) != static_cast<i32>(jo::self<jo::Unit>(this)->side)) continue;
            if (static_cast<i32>(jo::as_unit(p)->group_id) == static_cast<i32>(jo::self<jo::Unit>(this)->group_id)) continue;
            if (jo::as_unit(p)->rank >= 3) continue;
            if (p == self()) continue;
            if (static_cast<i32>(jo::as_unit(p)->id) == global<i32>(d::g_player_unit_recno)) continue;
            if (static_cast<i32>(jo::as_unit(p)->speech_off) != 0) continue;
            if (jo::view<u32>(jo::as_unit(p)->weapon) != 0) continue;
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(p)->type_id))));  // 0x0042d0a0
            const i32 kind = static_cast<i32>(jo::as_unit_type(t)->unit_class);
            if (kind == 0 || kind == 8 || kind == 9) continue;
            const i32 w = static_cast<i32>(jo::as_unit_type(t)->kind);
            if (w != 0 && w != 1) continue;
            if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_game(p)->byte_2e1())) != 0) continue;  // 0x00432b90
            if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(p)->dead())) != 0) continue;  // 0x0042d0e0
            V3 other;
            joan::orig::as_unit(p)->position(&other);  // 0x004fedb0
            V3 tmp;
            const V3* const d = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &other, &pos));  // 0x0060cc90
            V3 delta = *d;
            const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, &delta));  // length
            const float angle = joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to_unit(static_cast<i32>(jo::as_unit(p)->id)));  // 0x00505960
            const float hi = 90.0f, lo = -90.0f;
            if (len > best_dist) continue;      // test ah,0x41 je
            if (!(angle >= lo)) continue;       // test ah,1 jne (below or unordered)
            if (angle > hi) continue;           // test ah,0x41 je
            best_dist = len;
            best = static_cast<i32>(jo::as_unit(p)->id);
        }
        return best;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Unit::nearest_enemy", 0x0048dfb0, &H::nearest_enemy);
#undef B94

}  // namespace game_b94_02_2
}  // namespace

// ---------------------------------------- from modules/game_b101_03.cpp (game/b101_03)
namespace {
namespace game_b101_03 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

bool eq_or_nan(double v) { return !(v < 0.0 || v > 0.0); }  // fcomp 0 / test ah,0x40 jnz

void* const kTypes = obj(d::g_item_res);

u32 now() { return global<u32>(d::g_now); }

JOAN_COUNTER(c_can_act, "Obj::can_act");

struct H {
    // ------------------------------------------------------------------ 0x0048e270
    i32 can_act() {
        c_can_act.hit();
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Gate>(this)->type_id)));  // 0x00425ce0
        if (jo::self<jo::Gate>(this)->busy != 0 || jo::view<u32>(jo::as_item_def(t)->model_count) == 0 || jo::view<u32>(jo::self<jo::Gate>(this)->hp) != 0) return 0;
        const i32 kind = static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[0]));
        if (kind == 0) {
            if (jo::self<jo::Gate>(this)->actor == nullptr) return 1;
            return eq_or_nan(joan::orig::arg::as<double>(joan::orig::as_gate(this)->time_left())) ? 1 : 0;  // 0x0048c890 (st0)
        }
        if (kind == 1) return 1;
        if (kind != 2) return 0;
        if (jo::self<jo::Gate>(this)->actor == nullptr) return 1;
        u8* const a = jo::view<u8*>(jo::self<jo::Gate>(this)->actor);
        if (!eq_or_nan(static_cast<double>(jo::as_actor(a)->fade))) {
            const double dt = static_cast<double>(static_cast<u32>(now() - jo::as_actor(a)->fade_38));
            const double m = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::self<jo::Gate>(this)->actor->fade))));  // fabs
            const double q = Fpu{true}.div(global<double>(d::dbl_1000), m);  // 1000.0 / fabs (fdivr qword)
            if (q >= dt) return 0;  // test ah,1 je: neither less nor unordered
        }
        return eq_or_nan(joan::orig::arg::as<double>(joan::orig::as_gate(this)->time_left())) ? 1 : 0;  // 0x0048c890 (st0)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_03", name, addr, fn)
B("Obj::can_act", 0x0048e270, &H::can_act);
#undef B

}  // namespace game_b101_03
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::img::str;

double crt_fabs(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }

void* const kTypes = obj(d::g_item_res);

u32 now() { return global<u32>(d::g_now); }
u8* types(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (reads the low word)

// Anim player (0x005abe30, 6 args, ret 0x18).
void anim_play(u8* a, const char* name) { joan::orig::as_actor(a)->start_anim(jo::as_actor(a)->anim, name, joan::orig::arg::u32(now()), 0u, 0u, 0u); }

// Sound at the object's position: 0x0041baf0(handle, sample, src, pos, 1, arg, 0, 5000).
void play_sound(u8* self, const std::int32_t& id, const float* src_pos, const float& arg_f) {
    const u32 arg = jo::view<u32>(arg_f);
    void* const pos = jo::as_map_object(self)->pos;
    void* const src = const_cast<float*>(src_pos);
    u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(id))));  // 0x0041b440
    void* const r24 = jo::as_pemi_script(r)->effect;
    jo::Scene* const h = jo::as_map_object(self)->actor->scene;
    joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(h, r24, src, pos, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(arg), 0, 0x1388);  // 0x0041baf0
}

JOAN_COUNTER(c_state_48e430, "Obj::set_state_48e430");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x0048e430
    // Path object (0x194 bytes in the frame; 0x005a1170 writes byte 0, 0x005a1440 bounds to 0x190).
    struct alignas(4) Path {
        u8 bytes[0x194];
    };
    void anim_with_variant(u8* t, float m) {
        Path path;
        joan::orig::as_string(&path)->construct();                                // 0x005a1170
        joan::orig::as_string(&path)->assign(joan::orig::arg::str(jo::as_item_def(t)->anim_names[jo::view<i32>(jo::self<jo::Gate>(this)->busy)]));  // 0x005a1440
        joan::orig::as_string(&path)->append(str(d::s_F_9d0068));                                    // 0x005a1550 "F"
        const double a = crt_fabs(static_cast<double>(m));
        if (a > global<double>(d::dbl_90) && jo::self<jo::Gate>(this)->actor->anim != nullptr) {  // 90.0
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
            if (joan::orig::arg::as<i32>(joan::orig::as_anim_def(jo::view<void*>(jo::self<jo::Gate>(this)->actor->anim))->find_action(joan::orig::arg::str(name))) != 0) {  // 0x005b5be0
                const u32 time = now();
                const char* const n2 = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());
                joan::orig::as_actor(jo::view<u8*>(jo::self<jo::Gate>(this)->actor))->start_anim(jo::self<jo::Gate>(this)->actor->anim, n2, joan::orig::arg::u32(time), 0u, 0u, 0u);
                return;
            }
        }
        anim_play(jo::view<u8*>(jo::self<jo::Gate>(this)->actor), joan::orig::arg::str(jo::as_item_def(t)->anim_names[jo::view<i32>(jo::self<jo::Gate>(this)->busy)]));
    }
    i32 set_state_48e430(void* arg) {
        c_state_48e430.hit();
        u8* const t = types(jo::self<jo::Gate>(this)->type_id);
        const float m = joan::orig::arg::as<float>(joan::orig::as_map_object(this)->heading_to(arg));  // 0x0041af80 (fstp dword)
        if (jo::self<jo::Gate>(this)->hp > jo::as_item_def(t)->model_values[jo::view<i32>(jo::self<jo::Gate>(this)->busy) - 1]) return 1;
        const i32 kind0 = static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[jo::view<i32>(jo::self<jo::Gate>(this)->busy) - 1]));
        if (kind0 == 0) {
            virtual_call<void>(this, 0x18, jo::view<i32>(jo::self<jo::Gate>(this)->busy) - 1);
            if (static_cast<i32>(jo::view<i8>(jo::self<jo::Gate>(this)->active)) != 0 && jo::self<jo::Gate>(this)->actor != nullptr) anim_with_variant(t, m);
        } else if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[jo::view<i32>(jo::self<jo::Gate>(this)->busy) - 1])) == 1) {
            virtual_call<void>(this, 0x18, jo::view<i32>(jo::self<jo::Gate>(this)->busy) - 1);
        } else if (static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[jo::view<i32>(jo::self<jo::Gate>(this)->busy) - 1])) == 2) {
            virtual_call<void>(this, 0x18, jo::view<i32>(jo::self<jo::Gate>(this)->busy) - 1);
            if (static_cast<i32>(jo::view<i8>(jo::self<jo::Gate>(this)->active)) != 0 && jo::self<jo::Gate>(this)->actor != nullptr) {
                anim_with_variant(t, m);
                jo::self<jo::Gate>(this)->actor->fade_38 = now();
                jo::view<u32>(jo::self<jo::Gate>(this)->actor->fade) = 0xbe99999au;  // -0.3f
            }
        } else {
            return 0;
        }
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Gate>(this)->active)) != 0 && jo::self<jo::Gate>(this)->actor != nullptr && jo::view<u32>(jo::as_item_def(t)->emitter_scripts[2]) != 0)
            play_sound(self(), jo::as_item_def(t)->emitter_scripts[2], &jo::as_item_def(t)->emitter_pos[6], jo::as_item_def(t)->emitter_scale[2]);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Obj::set_state_48e430", 0x0048e430, &H::set_state_48e430);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

using joan::call::thiscall;

JOAN_COUNTER(c_orders_find, "Orders::find");

struct H {
    // 0x0048e7f0
    std::int32_t orders_find(const std::uint8_t* key, std::int32_t exact) {
        c_orders_find.hit();
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_talk(this)->count()); i > 0; --i) {  // 0x004b8c40
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_talk(this)->is_deleted(joan::orig::arg::i32(i))) != 0) continue;           // 0x004e4110
            const std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_talk(this)->get_msg(joan::orig::arg::i32(i)));  // 0x004e3fe0
            const std::int32_t k = jo::as_treaty(e)->reply;
            if (k != 1 && k != 0) continue;
            if (jo::as_treaty(e)->kind != jo::as_treaty(key)->kind) continue;
            if (jo::as_treaty(e)->from != jo::as_treaty(key)->from) continue;
            if (jo::as_treaty(e)->to != jo::as_treaty(key)->to) continue;
            if (exact == 0) return i;
            if (jo::as_treaty(e)->arg == jo::as_treaty(key)->arg && jo::as_treaty(e)->arg2 == jo::as_treaty(key)->arg2)
                return i;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Orders::find", 0x0048e7f0, &H::orders_find);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
namespace {
namespace game_b105_04_2 {

using i32 = std::int32_t;

struct H {
    // ---------------------------------------------------------------- getters
    i32 zero0() { return 0; }
    H* self0() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
B("Obj::zero_48e8e0", 0x0048e8e0, &H::zero0);
B("Obj::self_48e900", 0x0048e900, &H::self0);
#undef B

}  // namespace game_b105_04_2
}  // namespace

// ---------------------------------------- from modules/game_b105_04_3.cpp (game/b105_04)
namespace {
namespace game_b105_04_3 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---------------------------------------------------------------- CRT initialisers
template <fn::Code Callee>
u32 JOAN_CDECL init_call() {
    return cdecl_call<u32>(Callee);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
// CRT initialisers
B("init_48c100", 0x0048c100, &init_call<fn::init_a39bf8>);
B("init_48c160", 0x0048c160, &init_call<fn::init_a39bfc>);
B("init_48c1b0", 0x0048c1b0, &init_call<fn::init_a39c00>);
B("init_48c200", 0x0048c200, &init_call<fn::init_a39c04>);
#undef B

}  // namespace game_b105_04_3
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::hook::global;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

void pi_global(img::Addr out) {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(out) = static_cast<float>(Fpu{true}.mul(a, global<double>(d::dbl_4)));
}
static void JOAN_CDECL init_a39bf8() { pi_global(d::g_a39bf8); }                             // 0x0048c120

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("init_a39bf8", 0x0048c120, &init_a39bf8);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using joan::hook::global;

void JOAN_CDECL init_a39bfc() { global<float>(d::g_a39bfc) = global<float>(d::g_a39bf8) / global<float>(d::flt_2); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("init_a39bfc", 0x0048c180, &init_a39bfc);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using joan::hook::global;

void JOAN_CDECL init_a39c00() { global<float>(d::g_a39c00) = global<float>(d::g_a39bf8) / global<float>(d::flt_3); }

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("init_a39c00", 0x0048c1d0, &init_a39c00);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::hook::global;

// ---------------------------------------------------------------------- free functions
static void JOAN_CDECL init_a39c04() { global<float>(d::g_a39c04) = global<float>(d::g_a39bf8) / global<float>(d::flt_4); }  // 0x0048c220

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("init_a39c04", 0x0048c220, &init_a39c04);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::memset_;  // memset

// Order constructors: base 0x00416a50, vtable, memset(this + 0xc2, 0, n).
using joan::orig::helpers::order_ctor;

struct H {
    void* order_48c250() { return order_ctor(this, d::vt_Obj_9d388c, static_cast<u32>(sizeof(jo::Gate) - sizeof(jo::MapObject))); }  // 0x19 on x86
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Order19::ctor", 0x0048c250, &H::order_48c250);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

struct H {
    // ------------------------------------------------------------------ 0x0048c2a0
    void* scalar_dtor(u32 flags) {
        joan::orig::as_gate(this)->destruct();  // 0x0048c2f0
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Obj::scalar_dtor_48c2a0", 0x0048c2a0, &H::scalar_dtor);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03_2 {

using i16 = std::int16_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

void* const kTypes = obj(d::g_item_res);

u8* type_record(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0

struct H {
    // ------------------------------------------------------------------ 0x0048c360
    H* init_48c360(u32 type) {
        joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type));  // 0x004178e0
        u8* const t = type_record(static_cast<i16>(type));
        jo::view<u32>(jo::self<jo::Gate>(this)->max_hp) = jo::view<u32>(jo::as_item_def(t)->i32_5e);
        jo::view<u32>(jo::self<jo::Gate>(this)->hp) = jo::view<u32>(jo::self<jo::Gate>(this)->max_hp);
        jo::self<jo::Gate>(this)->busy = jo::view<u32>(jo::as_item_def(t)->model_count);
        jo::self<jo::Gate>(this)->need_key = jo::as_item_def(t)->need_key;
        jo::self<jo::Gate>(this)->invincible = 0;
        jo::view<u8>(jo::self<jo::Gate>(this)->state) = 0;
        jo::self<jo::Gate>(this)->interact_trigger = 0x200;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::init_48c360", 0x0048c360, &H::init_48c360);
#undef B

}  // namespace game_b102_03_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Gate::destruct() -> void { joan::bind::member<&::game_b108_13::H::dtor_48c2f0>(this); }  // 0x48c2f0
auto joan::orig::Gate::on_remove() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::gate_on_remove>(this)); }  // 0x48c410
auto joan::orig::Gate::time_left() -> double { return joan::bind::conv<double>(joan::bind::member<&::game_b94_02::H::time_left>(this)); }  // 0x48c890
auto joan::orig::Unit::take_damage(std::uint32_t a0, std::uint32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b96_02::H::take_damage>(this, a0, a1)); }  // 0x48c8c0
auto joan::orig::Res::drain_unit(std::int32_t a0, std::uint32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b99_03::H::drain_unit>(this, a0, a1)); }  // 0x48c990
auto joan::orig::Gate::take_amount(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b94_02::H::take_c2>(this, a0, a1)); }  // 0x48ca30
auto joan::orig::Gate::set_door_state(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::door_set_state>(this, a0)); }  // 0x48d2b0
auto joan::orig::Gate::update_48d4b0(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b109_16::H::gate_update>(this, a0)); }  // 0x48d4b0
auto joan::orig::Unit::nearest_enemy(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_02_2::H::nearest_enemy>(this, a0)); }  // 0x48dfb0
auto joan::orig::Gate::advance_stage(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_03::H::set_state_48e430>(this, a0)); }  // 0x48e430
auto joan::orig::Talk::find_order(void* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::orders_find>(this, a0, a1)); }  // 0x48e7f0
