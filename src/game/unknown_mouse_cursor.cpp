// game/unknown_mouse_cursor.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_00493450.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
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

using joan::call::cdecl_call;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

void JOAN_CDECL init_493450() { joan::orig::init_pi_a39c84(); }
void JOAN_CDECL init_493590() { joan::orig::init_a39c88(); }

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("init_493450", 0x00493450, &init_493450);
B("init_493590", 0x00493590, &init_493590);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

void JOAN_CDECL init_pi_a39c84() {  // 0x004934f0
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(d::g_a39c84) = static_cast<float>(t * 4.0);  // qword 0x9cf788 = 4.0
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("init_pi_a39c84", 0x004934f0, &init_pi_a39c84);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

void JOAN_CDECL init_a39c88() { global<float>(d::g_a39c88) = global<float>(d::g_a39c84) / 2.0f; }  // 0x00493650

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("init_a39c88", 0x00493650, &init_a39c88);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b109_08.cpp (game/b109_08)
namespace {
namespace game_b109_08 {

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;


JOAN_COUNTER(c_on_event, "MouseCr::on_event_493680");

struct H {
    // ---------------------------------------------------------------- 0x00493680 helpers
    static bool nonzero(float v) {  // fcomp [0x9cf5c4]; test ah,0x40; jne skip
        const float z = global<float>(d::k_9cf5c4);
        return v < z || v > z;
    }
    void vcall_18() {
        const std::uintptr_t fn = static_cast<const std::uintptr_t*>(jo::self<jo::Order0c_a>(this)->vtable)[0x18 / 4];  // vtable +0x18
        thiscall<i32>(fn, this, static_cast<i32>(static_cast<u32>(jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy)) - 1u));
    }
    void start_anim(u8* type) {
        if (jo::view<i8>(jo::self<jo::Order0c_a>(this)->active) == 0 || jo::self<jo::Order0c_a>(this)->actor == nullptr) return;
        const u32 g = global<u32>(d::g_now);
        const char* const v = jo::as_item_def(type)->anim_names[jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy)];
        jo::Actor* const a = jo::self<jo::Order0c_a>(this)->actor;
        joan::orig::as_actor(a)->start_anim(joan::orig::arg::mptr(a->anim), v, joan::orig::arg::u32(g), 0u, 0u, 0u);  // 0x005abe30
        jo::self<jo::Order0c_a>(this)->actor->fade_38 = global<u32>(d::g_now);
        jo::view<u32>(jo::self<jo::Order0c_a>(this)->actor->fade) = 0xbe4ccccdu;  // -0.2f
    }
    // false: the original jumps straight to the return.
    bool react(u8* type) {
        if (jo::self<jo::Order0c_a>(this)->amount > jo::as_item_def(type)->model_values[jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy) - 1]) return true;
        const i8 k = jo::view<i8>(jo::as_item_def(type)->model_kinds[jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy) - 1]);
        if (k == 0) {
            vcall_18();
            start_anim(type);
        } else if (k == 1) {
            vcall_18();
        } else if (k == 2) {
            vcall_18();
            start_anim(type);
        } else {
            return false;
        }
        return true;
    }
    void idle_check(u8* type) {
        if (jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy) == 0 && jo::as_item_def(type)->model_count != 0 && jo::self<jo::Order0c_a>(this)->amount == 0) {
            joan::orig::as_map_object(this)->set_collision(0);  // 0x00493060
        }
    }
    void camera_check(u8* ev) {
        if (static_cast<i32>(jo::view<i16>(jo::as_save_entry(ev)->ref_id)) == static_cast<i32>(global<i16>(d::g_a47c36))) {
            joan::orig::as_hud(obj(d::g_player_hud))->set_unit_portrait(static_cast<i32>(jo::self<jo::Order0c_a>(this)->portrait_unit));  // 0x006fe340
        }
    }
    u8* unit_type() {
        return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Order0c_a>(this)->type_id))));  // 0x00425ce0
    }
    // Emitter k (2 / 3) of the item type: script id, position, scale (as the argument).
    void link(u8* type, i32 k) {
        const u32 val = jo::view<u32>(jo::as_item_def(type)->emitter_scale[k]);
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(type)->emitter_scripts[k]))));  // 0x0041b440
        void* const rv = jo::as_pemi_script(r)->effect;
        joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(jo::self<jo::Order0c_a>(this)->actor->scene, rv, &jo::as_item_def(type)->emitter_pos[k * 3], jo::self<jo::Order0c_a>(this)->pos,
 static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::i32(val), 0, 0x1388);  // 0x0041baf0
    }

    // ------------------------------------------------------------------------ 0x00493680
    u8 on_event_493680(i16 id) {
        c_on_event.hit();
        u8* const ev = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(static_cast<i32>(id))));  // 0x00485f20
        auto sub = [ev]() { return static_cast<i32>(jo::view<i16>(jo::as_save_entry(ev)->ref_id)); };  // re-read at each use
        switch (jo::as_save_entry(ev)->type) {
            case 0x40:
            case 0x2000: {
                if (jo::view<i32>(jo::as_save_entry(ev)->ref_kind) != 1) break;
                const float v = joan::orig::arg::as<float>(joan::orig::as_unit(this)->drain_452280(joan::orig::arg::i32(sub()), 0));  // 0x00452280
                u8* const type = unit_type();
                if (jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy) == 0 || !nonzero(v)) break;
                if (!react(type)) break;
                idle_check(type);
                camera_check(ev);
                break;
            }
            case 0x4000: {
                if (jo::view<i32>(jo::as_save_entry(ev)->ref_kind) != 2) break;
                const float v = joan::orig::arg::as<float>(joan::orig::as_map_object(this)->drain_item(joan::orig::arg::i32(sub()), 0));  // 0x00452b20
                u8* const type = unit_type();
                if (jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy) == 0 || !nonzero(v)) break;
                if (!react(type)) break;
                idle_check(type);
                break;
            }
            case 1: {
                if (jo::view<i32>(jo::as_save_entry(ev)->ref_kind) != 1) break;
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(this)->can_reach_49a010(joan::orig::arg::u32(sub()), 0u)) == 0) break;  // 0x0049a010
                u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(sub())));  // 0x0041b3c0
                const char* const name =
                    joan::orig::arg::as<const char*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
                if (joan::crt::stricmp(joan::orig::arg::str(jo::as_unit_type(name)->name), str(d::s_LAHIRE_9cfb3c)) != 0) break;  // _stricmp(.., "LAHIRE")
                const float v = joan::orig::arg::as<float>(joan::orig::as_map_object(this)->drain_hp(joan::orig::arg::i32(sub()), 0));  // 0x0044a5f0
                u8* const type = unit_type();
                if (jo::view<i32>(jo::self<jo::Order0c_a>(this)->busy) == 0 || !nonzero(v)) break;
                if (jo::view<u32>(jo::as_item_def(type)->emitter_scripts[2]) != 0) link(type, 2);
                if (jo::view<u32>(jo::as_item_def(type)->emitter_scripts[3]) != 0) link(type, 3);
                if (!react(type)) break;
                idle_check(type);
                camera_check(ev);
                break;
            }
            default: break;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_08", name, addr, fn)
B("MouseCr::on_event_493680", 0x00493680, &H::on_event_493680);
#undef B

}  // namespace game_b109_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_pi_a39c84() -> void { joan::bind::free<&::game_b96_02::init_pi_a39c84>(); }  // 0x4934f0
auto joan::orig::init_a39c88() -> void { joan::bind::free<&::game_b99_03::init_a39c88>(); }  // 0x493650
