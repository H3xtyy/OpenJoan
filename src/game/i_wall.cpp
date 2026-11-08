// game/i_wall.cpp: functions of the original d:\ww\i_wall.cpp (re/attribution.csv).
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
#include "sound_slots.hpp"
#include "../x87.hpp"
#include <cmath>
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
//
//    - Wall::release_487460 0x00487460 (ret 0): 0x004251b0, two owned objects +0xd2 / +0xda (detach
//      0x005efdb0, delete, type record 0x004a3e30 with +0xd6 / +0xde), buffer +0xeb freed by 0x0058ed70
//      on 0xa48838 ("d:\ww\i_wall.cpp", line [0xa0075c] + 0x1d) when +0xe7. eax: this / 0x0058ed70.
namespace {
namespace game_b103_03 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::orig::helpers::addr;

void* const kTypes = obj(d::g_item_res);

u8* type_record(i16 type) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0

JOAN_COUNTER(c_release_487460, "Wall::release_487460");

struct H {
    // ------------------------------------------------------------------ 0x00487460
    void release_owned(jo::Actor*& owned, std::int32_t& slot) {
        u8* const p = jo::view<u8*>(owned);
        if (jo::as_scene_object(p)->scene != nullptr) joan::orig::as_scene(jo::as_scene_object(p)->scene)->detach(joan::orig::arg::mptr(static_cast<void*>(p)));  // 0x005efdb0
        void* const q = jo::view<void*>(owned);
        if (q != nullptr) joan::orig::as_scene_object(q)->destroy(1u);  // 0x00420840 (scalar deleting dtor)
        owned = nullptr;
        const u32 arg = jo::view<u32>(slot);
        u8* const rec = type_record(jo::self<jo::Wall>(this)->type_id);
        joan::orig::as_res(rec)->release_slot(joan::orig::arg::i32(arg), 0);  // 0x004a3e30
    }
    std::uintptr_t release_487460() {  // eax: this or 0
        c_release_487460.hit();
        joan::orig::as_unit(this)->release_4251b0();  // 0x004251b0
        if (jo::self<jo::Wall>(this)->actor_a != nullptr) release_owned(jo::self<jo::Wall>(this)->actor_a, jo::self<jo::Wall>(this)->level_a);
        if (jo::self<jo::Wall>(this)->actor_b != nullptr) release_owned(jo::self<jo::Wall>(this)->actor_b, jo::self<jo::Wall>(this)->level_b);
        if (jo::self<jo::Wall>(this)->block_count == 0) return addr(this);
        const i32 line = static_cast<i32>(global<i16>(d::g_a0075c)) + 0x1d;
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::Wall>(this)->block_ids), &img::global<const char>(d::s_d_ww_i_wall_cpp), line);  // 0x0058ed70 (a heap block)
        jo::self<jo::Wall>(this)->block_ids = nullptr;
        jo::self<jo::Wall>(this)->block_count = 0;
        return 0;  // void callee; the only caller ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("Wall::release_487460", 0x00487460, &H::release_487460);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
namespace {
namespace game_b103_03_2 {

using joan::call::cdecl_call;

void JOAN_CDECL init_4925e0() { joan::orig::init_pi_a39c70(); }
void JOAN_CDECL init_492680() { joan::orig::init_426420(); }
void JOAN_CDECL init_4926c0() { joan::orig::init_a39c7c(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("init_4925e0", 0x004925e0, &init_4925e0);
B("init_492680", 0x00492680, &init_492680);
B("init_4926c0", 0x004926c0, &init_4926c0);
#undef B

}  // namespace game_b103_03_2
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::op_delete;  // operator delete

// ---------------------------------------------------------------------- static initialisers
template <fn::Code Worker>
void JOAN_CDECL static_init() { cdecl_call<void>(Worker); }

struct H {
    // ------------------------------------------------------------------ deleting destructors
    template <fn::Code Dtor>
    H* deleting_dtor(u32 flags) {
        thiscall<u32>(Dtor, this);
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }

    // ------------------------------------------------------------------ trivial
    H* self() { return this; }
    i32 zero_492e00() { return 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("init_492630", 0x00492630, &static_init<fn::init_a39c74>);
B("Obj::deleting_dtor_4928b0", 0x004928b0, &H::deleting_dtor<fn::Obj_dtor_4380a0>);  // 0x004380a0
B("Obj::self_493320", 0x00493320, &H::self);
B("Obj::zero_492e00", 0x00492e00, &H::zero_492e00);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using joan::hook::global;

void JOAN_CDECL init_a39c74() { global<float>(d::g_a39c74) = global<float>(d::g_a39c70) / 2.0f; }  // 0x00492650

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("init_a39c74", 0x00492650, &init_a39c74);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;

JOAN_COUNTER(c_issue, "Orders::issue_492720");

struct H {
    // ------------------------------------------------------------------------ 0x00492720
    u32 issue_492720(i32 a, i32 b, i32 c, i32 d, i32 e, i32 /*unused*/, i32 g, i32 h) {
        c_issue.hit();
        void* const pl = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(b));  // 0x0047b600
        if (joan::orig::arg::as<i32>(joan::orig::as_nation(pl)->is_kind_3()) == 0) return 0;                  // 0x004b4590
        u8* const pa = joan::orig::arg::as<u8*>(joan::orig::as_player(pl)->relation(joan::orig::arg::i32(a)));                   // 0x004b42e0
        if (static_cast<i8>(pa[c + 0xcf]) != 0) return 0;
        i32 kind = 0;
        switch (c) {
            case 3: kind = 5; break;
            case 13: kind = 0xe; break;
            case 14: kind = 0xd; break;
            case 15: kind = 0x10; break;
            case 16: kind = 0xf; break;
            default: break;
        }
        if (kind != 0) {
            void* const q = joan::orig::arg::as<void*>(joan::orig::as_player(pl)->relation(joan::orig::arg::i32(a)));  // 0x004b42e0
            joan::orig::as_table(q)->raise_to(joan::orig::arg::i32(kind), 0x78);           // 0x004b8d60
        }
        alignas(4) u8 order[0x18] = {};
        joan::orig::as_treaty(order)->zero();  // 0x004e29d0 (memset 0x17)
        memset_(order, 0, 0x17);
        order[0xf] = static_cast<u8>(a);
        order[0xe] = static_cast<u8>(b);
        jo::view<u16>(jo::as_msg(order)->type) = static_cast<u16>(c);
        jo::view<u16>(jo::as_msg(order)->arg) = static_cast<u16>(d);
        jo::view<u16>(jo::as_msg(order)->arg2) = static_cast<u16>(e);
        jo::as_msg(order)->u16_6 = static_cast<u16>(g);
        jo::as_msg(order)->u16_8 = static_cast<u16>(h);
        joan::orig::as_order(this)->dispatch_4e3bb0(joan::orig::arg::mptr(order), 0);  // 0x004e3bb0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Orders::issue_492720", 0x00492720, &H::issue_492720);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b107_08.cpp (game/b107_08)
namespace {
namespace game_b107_08 {

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

JOAN_COUNTER(c_is_idle, "MouseCursor::is_idle_4928f0");

struct H {
    // 0x004928f0 helper: kind 0 / kind 2 tail (0x492950.. / 0x492a20..).
    i32 idle_tail() {
        if (jo::self<jo::Order0c_a>(this)->actor == nullptr) return 1;
        u8* const a = jo::view<u8*>(jo::self<jo::Order0c_a>(this)->actor);
        if (!eq_or_nan(static_cast<double>(jo::as_actor(a)->fade))) {
            const double dt = static_cast<double>(static_cast<u32>(now() - jo::as_actor(a)->fade_38));
            const double m = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(jo::self<jo::Order0c_a>(this)->actor->fade))));  // fabs
            const double q = Fpu{true}.div(global<double>(d::dbl_1000), m);  // 1000.0 / fabs (fdivr qword)
            if (q >= dt) return 0;  // test ah,1 je: neither less nor unordered
        }
        return eq_or_nan(joan::orig::arg::as<double>(joan::orig::as_mouse_cursor(this)->time_left())) ? 1 : 0;  // 0x004907c0 (st0)
    }

    // 0x004928f0
    i32 is_idle() {
        c_is_idle.hit();
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::self<jo::Order0c_a>(this)->type_id)));  // 0x00425ce0
        if (jo::self<jo::Order0c_a>(this)->busy != 0 || jo::view<u32>(jo::as_item_def(t)->model_count) == 0 || jo::view<u32>(jo::self<jo::Order0c_a>(this)->amount) != 0) return 0;
        const i32 kind = static_cast<i32>(jo::view<i8>(jo::as_item_def(t)->model_kinds[0]));
        if (kind == 0) return idle_tail();
        if (kind == 1) return 1;
        if (kind != 2) return 0;
        return idle_tail();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_08", name, addr, fn)
B("MouseCursor::is_idle_4928f0", 0x004928f0, &H::is_idle);
#undef B

}  // namespace game_b107_08
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::img::obj;

using i32 = std::int32_t;

using joan::hook::x87::ftol;

JOAN_COUNTER(c_take_half, "Obj::take_half");

struct H {
    // ------------------------------------------------------------------ 0x00492b10
    float take_half(i32 id, i32 /*unused*/) {
        c_take_half.hit();
        void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        const float t = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->defence_value()) * global<float>(d::flt_0_5);  // 0x00510bf0
        constexpr Fpu fpu{true};
        const double x = fpu.sub(static_cast<double>(jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6)), t);
        // fcomp 0.0 / test ah,1: below or unordered -> 0, else the (re-computed, same) difference.
        const float v = !(x >= static_cast<double>(global<float>(d::k_9cf5c4))) ? 0.0f : static_cast<float>(x);
        jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6) = joan::hook::x87::ftol(v);
        return t;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::take_half", 0x00492b10, &H::take_half);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

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

using joan::hook::x87::ftol;

struct Box24 {  // 6 dwords passed by value to 0x006afb10
    u32 d[6];
};
static_assert(sizeof(Box24) == 24, "Box24");

void* const kUnits = obj(d::g_items);
void* const kTypes = obj(d::g_item_res);
void* const kFx = obj(d::g_physic_sys);

JOAN_COUNTER(c_take_c6, "Obj::take_c6");
JOAN_COUNTER(c_spawn_effect, "Obj::spawn_effect");

// Shared body of 0x0048ca30 / 0x00492bb0: moves the unit's +0x72 amount out of field `off`.
float take_amount(i32& stock, i32 id) {
    if (joan::orig::as_units(kUnits)->list.empty_at(joan::orig::arg::i32(id)) != 0) return global<float>(d::k_9cf5c4);  // 0x00425d50
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    const float amount = static_cast<float>(jo::view<i32>(jo::as_map_object(unit)->damage));
    const Fpu fpu{true};
    const float diff = static_cast<float>(fpu.sub(static_cast<double>(stock), static_cast<double>(amount)));
    float v;
    if (!(diff >= global<float>(d::k_9cf5c4))) {  // fcomp, test ah,1: below or unordered -> 0
        v = 0.0f;
    } else {
        v = static_cast<float>(fpu.sub(static_cast<double>(stock), static_cast<double>(amount)));
    }
    stock = joan::hook::x87::ftol(static_cast<double>(v));
    return amount;
}

struct H {
    // ------------------------------------------------------------------------ 0x00492bb0
    float take_c6(i32 id, i32) {
        c_take_c6.hit();
        return take_amount(jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6), id);
    }

    // ------------------------------------------------------------------------ 0x00493060
    i32 spawn_effect(i32 on) {
        c_spawn_effect.hit();
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Order0c_a>(this)->type_id))));  // 0x00425ce0
        i32 result = 0;
        Box24 box;
        std::memcpy(&box, jo::as_item_def(type)->bounding_box, sizeof(box));  // ItemDef +0x1c7 on x86 (behind seven pointers on x64)
        u8 m[0x40];
        void* mp = nullptr;
        const float zero = global<float>(d::k_9cf5c4);
        const float rx = jo::self<jo::Order0c_a>(this)->yaw_, ry = jo::self<jo::Order0c_a>(this)->pitch, rz = jo::self<jo::Order0c_a>(this)->roll_;
        // fcomp + test ah,0x40 je: continue when equal or unordered.
        const bool rx0 = !(rx < zero || rx > zero);
        const bool ry0 = rx0 && !(ry < zero || ry > zero);
        const bool rz0 = ry0 && !(rz < zero || rz > zero);
        if (rz0) {
            joan::orig::vec_add_in_place(static_cast<void*>(&box.d[0]), joan::orig::arg::ptr(&jo::self<jo::Order0c_a>(this)->pos));  // 0x0060cdc0
            joan::orig::vec_add_in_place(static_cast<void*>(&box.d[3]), joan::orig::arg::ptr(&jo::self<jo::Order0c_a>(this)->pos));  // 0x0060cdc0
        } else {
            joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Order0c_a>(this)->yaw_)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Order0c_a>(this)->pitch)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Order0c_a>(this)->roll_)));  // 0x0060bf40
            joan::orig::set_30(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::mptr(&jo::self<jo::Order0c_a>(this)->pos));                             // 0x0060c360
            mp = m;
        }
        const i32 mode = static_cast<i32>(jo::view<i8>(jo::as_item_def(type)->item_usage));
        if (mode == 1) {
            result = joan::orig::arg::as<i32>(joan::orig::as_players(kFx)->assign_matches(box, mp, i32{1}, static_cast<std::int8_t>(joan::orig::arg::i32(on)), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0}))));  // 0x006afb10
        } else if (mode == 2) {
            result = joan::orig::arg::as<i32>(joan::orig::as_players(kFx)->assign_matches(box, mp, i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(on)), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0}))));
        } else if (mode == 0) {
            result = joan::orig::arg::as<i32>(joan::orig::as_players(kFx)->assign_matches(box, mp, i32{-1}, static_cast<std::int8_t>(joan::orig::arg::i32(on)), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0}))));
        }
        if (result == 0) return 0;
        if (static_cast<i32>(static_cast<i8>(on)) != 0) {
            if (jo::view<u8*>(jo::self<jo::Order0c_a>(this)->actor) != nullptr) {
                jo::self<jo::Order0c_a>(this)->actor->aim_mode = jo::as_item_def(type)->collision_type;
                if (jo::self<jo::Order0c_a>(this)->actor->record != 0) {
                    joan::orig::as_players(kFx)->set_active(joan::orig::arg::as<std::int32_t>(jo::self<jo::Order0c_a>(this)->actor->record), joan::orig::arg::i16(i32{0}));  // 0x006ad980
                }
            }
        } else if (jo::view<u8*>(jo::self<jo::Order0c_a>(this)->actor) != nullptr) {
            jo::self<jo::Order0c_a>(this)->actor->aim_mode = 0;
            if (jo::self<jo::Order0c_a>(this)->actor->record != 0) {
                joan::orig::as_players(kFx)->set_active(joan::orig::arg::as<std::int32_t>(jo::self<jo::Order0c_a>(this)->actor->record), joan::orig::arg::i16(i32{0}));  // 0x006ad980
            }
        }
        return result;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::take_c6", 0x00492bb0, &H::take_c6);
B94("Obj::spawn_effect", 0x00493060, &H::spawn_effect);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_play_kind, "Sound::play_kind");

// Common sound-slot set-up tail of 0x00492c40 / 0x004950a0 (slot calls on *slot).
float sound_volume(const void* self, const void* e) {
    const joan::hook::x87::Fpu fpu{true};
    const double v = fpu.mul(static_cast<double>(jo::view<std::int32_t>(jo::as_sound(self)->u32_0)), static_cast<double>(jo::as_wave_res(e)->volume));
    return static_cast<float>(fpu.mul(v, static_cast<double>(global<float>(d::flt_0_01))));
}

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00492c40
    std::int32_t play_kind(const char* id) {
        c_play_kind.hit();
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(obj(d::g_sound_def_table))->find_select(id));  // 0x004d0d80
        if (global<void*>(d::g_sound3d_listener) == nullptr) return 0;
        if (e == nullptr) return 0;
        void* const s = joan::orig::arg::as<void*>(joan::orig::as_wave_res(e)->buffer());  // 0x004d09c0
        if (s == nullptr) return 0;
        const std::int16_t idx = joan::orig::arg::as<std::int16_t>(joan::orig::as_party(self())->first_free());  // 0x0045f450
        if (static_cast<std::int32_t>(idx) <= 0) return 0;
        void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(self())->entry(joan::orig::arg::i16(static_cast<std::int32_t>(idx))));  // 0x004cf690
        joan::orig::as_sound_slot_entry(slot)->set_key(0u);                                                         // 0x004d06b0
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->queue(joan::orig::arg::mptr(s));                                           // 0x0059f460
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_position(joan::orig::arg::mptr(static_cast<std::uint8_t*>(global<void*>(d::g_sound3d_listener)) + 5));     // 0x0059fb40
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_looping(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::as_wave_res(e)->flags & 3))));    // 0x0059fa80
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_gain(joan::orig::arg::bits(sound_volume(self(), e)));                    // 0x0059f700
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_max_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(e)->max_dist)));                 // 0x0059f920
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_reference_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(e)->min_dist)));                 // 0x0059f970
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));                                           // 0x0059fae0
        joan::orig::as_sound3d(jo::as_sound_slot_entry(slot)->sound)->set_rolloff(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(e)->rolloff)));                 // 0x0059f9c0
        joan::orig::as_sound_slot_entry(slot)->set_id(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_wave_res(e)->index))));        // 0x004d0550
        joan::orig::as_sound(jo::as_sound_slot_entry(slot)->sound)->play();                                              // 0x0059f620
        return static_cast<std::int32_t>(idx);
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Sound::play_kind", 0x00492c40, &H::play_kind);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

using joan::orig::helpers::u;
using joan::orig::helpers::operator_new;
// Model record table 0xa58f50 (word index).
u8* model_rec(i16 idx) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(idx)))); }  // 0x00425ce0

JOAN_COUNTER(c_attach_model, "Unit::attach_model");

struct H {
    // ------------------------------------------------------------------ 0x00492e20
    std::uintptr_t attach_model(i32 i) {  // eax: this, 0x0049b4d0's result or 0
        c_attach_model.hit();
        u8* const rec = model_rec(jo::self<jo::Wall>(this)->type_id);
        const i32 key = joan::orig::arg::as<i32>(joan::orig::as_item_def(rec)->sprite_id(joan::orig::arg::i32(i)));  // 0x004a3f20
        void* const p = operator_new(sizeof(jo::SceneObject));  // 0x10c on x86
        jo::view<void*>(jo::self<jo::Wall>(this)->actor_b) = p != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(p)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        joan::orig::as_item_def(rec)->acquire_anim(joan::orig::arg::i32(i), 0);                                          // 0x004a39f0
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(key)));            // 0x0041a180
        jo::self<jo::Wall>(this)->actor_b->def = jo::as_model_entry(e)->actor_def;
        if (jo::view<u32>(jo::as_item_def(rec)->slot_anims[i]) != 0) {
            u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_item_def(rec)->slot_anims[i]))));  // 0x004174f0
            jo::self<jo::Wall>(this)->actor_b->anim = jo::as_anim_slot(s)->anim;
        }
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b))->init();                                  // 0x005aab70
        u8* const e2 = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(key)));       // 0x0041a180
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b))->set_skin(joan::orig::arg::u32(jo::view<u32>(jo::as_model_entry(e2)->anim_db_id)));              // 0x005add40
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b))->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Wall>(this)->pos)));  // 0x005ac310
        jo::view<u32>(jo::self<jo::Wall>(this)->actor_b->scale) = jo::self<jo::Wall>(this)->scale;
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b))->set_yaw(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Wall>(this)->yaw_)));  // 0x005ac6a0
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b))->set_roll(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Wall>(this)->roll_)));  // 0x005ac980
        jo::view<u32>(jo::self<jo::Wall>(this)->actor_b->kind) = jo::view<i16>(jo::as_item_def(rec)->type_id) == 6 ? 0xau : 1u;
        jo::self<jo::Wall>(this)->actor_b->owner = this;
        std::uintptr_t r = u(this);
        if (jo::view<u32>(jo::as_item_def(rec)->slot_anims[i]) != 0) {
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b))->start_anim(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b->anim)), str(d::s_default_9d3d60), joan::orig::arg::u32(global<u32>(d::g_now)), 1u, 0u, 0u);                    // 0x005abe30 'default'
            r = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->play_named_sound(str(d::s_default_9d3d60)));                // 0x0049b4d0
        }
        if (jo::self<jo::Wall>(this)->scene != nullptr) {
            r = u(this);
            if (jo::self<jo::Wall>(this)->actor_b != nullptr) {
                joan::orig::as_scene(jo::view<void*>(jo::self<jo::Wall>(this)->scene))->attach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b)));  // 0x005efc50
                r = 0;  // void callee; the only caller ignores the result
            }
        }
        return r;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Unit::attach_model", 0x00492e20, &H::attach_model);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::import_call;

struct V3u {  // 12-byte float vector, by value (XMM registers under System V, as the callers declare it)
    float x, y, z;
};
static_assert(sizeof(V3u) == 12, "vector size");

JOAN_COUNTER(c_farthest, "Obj::farthest");

struct H {
    // ------------------------------------------------------------------ 0x00493340
    u32 farthest(V3u pos) {
        c_farthest.hit();
        float best = 0.0f;
        i16 best_i = 0;
        for (i16 i = 1; static_cast<i32>(i) <= joan::sound_slots::count(); i = static_cast<i16>(i + 1)) {
            jo::StreamSlot* const slot = jo::as_stream_slot(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x004cf690
            if (slot->source == nullptr) continue;
            V3u tmp;
            const V3u* const d = joan::orig::arg::as<const V3u*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(&slot->source->position), &pos));  // 0x0060cc90 sub (Sound3D+0x2f)
            V3u dv = *d;
            // length returns st0 (a double in the replacement); the original stores it as a float (fstp dword)
            const float len = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&dv)));  // length
            // fcomp best / test ah,1 jne skip: below or unordered -> skip.
            if (len >= best) {
                best = len;
                best_i = i;
            }
        }
        if (static_cast<i32>(best_i) != 0) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(static_cast<i32>(best_i))));  // 0x004cf690
            joan::orig::as_stream_slot(e)->release();  // 0x004cea40
            if constexpr (!joan::kReleaseBuild) {  // a sound cut short to make room (game/sound_slots.hpp)
                static std::uint32_t taken = 0;
                const std::uint32_t now = ++taken;
                if ((now & (now - 1)) == 0)
                    joan::hook::log_line("[Sound] all %d sound slots busy: the farthest sound was cut short (%u times)", joan::sound_slots::count(), taken);
            }
        }
        return static_cast<u16>(best_i);  // upper half was the void callee's eax; the only caller reads the low 16 bits
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::farthest", 0x00493340, &H::farthest);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::hook::global;

static void JOAN_CDECL init_a39c7c() { global<float>(d::g_a39c7c) = global<float>(d::g_a39c70) / global<float>(d::flt_4); }  // 0x004926f0

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("init_a39c7c", 0x004926f0, &init_a39c7c);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Wall::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_03::H::release_487460>(this)); }  // 0x487460
auto joan::orig::init_a39c7c() -> void { joan::bind::free<&::game_b100_04::init_a39c7c>(); }  // 0x4926f0
auto joan::orig::Building::take_half(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b97_02::H::take_half>(this, a0, a1)); }  // 0x492b10
auto joan::orig::Building::take_amount(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b94_02::H::take_c6>(this, a0, a1)); }  // 0x492bb0
auto joan::orig::Sound::play_kind(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::play_kind>(this, a0)); }  // 0x492c40
auto joan::orig::Unit::attach_model(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_02::H::attach_model>(this, a0)); }  // 0x492e20
auto joan::orig::MapObject::set_collision(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_02::H::spawn_effect>(this, a0)); }  // 0x493060
