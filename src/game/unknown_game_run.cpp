// game/unknown_game_run.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_004994b0.cpp; re/attribution.csv: between ww/i_gate.cpp and ww/itemres.cpp). They move to their
// original file when evidence names it (Phase 3).
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

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::global;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004994b0
    void set_state(jo::World* state) {
        if (jo::self<jo::MapObject>(this)->world == state) return;
        if (jo::self<jo::MapObject>(this)->cell != nullptr) joan::orig::as_unit(jo::view<void*>(jo::self<jo::MapObject>(this)->cell))->set_sector(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x0054f110
        if (jo::self<jo::MapObject>(this)->actor != nullptr && jo::self<jo::MapObject>(this)->actor->scene != nullptr)
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::MapObject>(this)->actor->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::MapObject>(this)->actor)));  // 0x005efdb0
        jo::self<jo::MapObject>(this)->world = state;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Obj::set_state", 0x004994b0, &H::set_state);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_mode, "Mode::set_499540");

struct H {
    // ------------------------------------------------------------------ 0x00499540
    static u32 mode_call(fn::Code thunk) { return thiscall<u32>(thunk, obj(d::g_game_state)); }
    u32 set_mode_499540(u32 mode_arg) {
        c_mode.hit();
        static constexpr fn::Code kLeave[14] = {fn::Mission_teardown_47b2b0, fn::Obj_stub_47b920, fn::Obj_stub_47b9a0, fn::Obj_stub_47ba20, fn::Obj_stub_47ba60,
                                                      fn::Obj_stub_47bae0, fn::Obj_stub_47bb60, fn::kNoCode,          fn::kNoCode,          fn::Obj_dtor_global_a3cf30,
                                                      fn::Obj_call_a501f0, fn::Obj_reset_a56a4b, fn::Obj_stop_a46030, fn::X_fwd_6b5880};
        static constexpr fn::Code kEnter[14] = {fn::Game_init_sample_world_4797a0, fn::Obj_stub_47b810, fn::Obj_stub_47b940, fn::Obj_stub_47b9c0, fn::Obj_stub_47ba40,
                                                      fn::Obj_stub_47bac0, fn::Obj_stub_47bb40, fn::kNoCode,          fn::kNoCode,          fn::Obj_set_a3cf30,
                                                      fn::init_47bcb0, fn::X_fwd_755840, fn::X_fwd_6d61d0, fn::Obj_call_47bf70};
        {
            const u32 k = static_cast<u32>(static_cast<i32>(jo::self<jo::Wnd>(this)->mode) - 2);
            if (k <= 13 && kLeave[k] != fn::kNoCode) mode_call(kLeave[k]);
        }
        if (static_cast<i32>(jo::self<jo::Wnd>(this)->mode) != static_cast<i32>(static_cast<i8>(mode_arg & 0xff))) {
            jo::self<jo::Wnd>(this)->prev_mode = jo::view<u8>(jo::self<jo::Wnd>(this)->mode);
            jo::view<u8>(jo::self<jo::Wnd>(this)->mode) = static_cast<u8>(mode_arg);
            jo::view<u8>(jo::self<jo::Wnd>(this)->next_mode) = static_cast<u8>(mode_arg);
        }
        const i32 mode = jo::self<jo::Wnd>(this)->mode;
        const u32 k = static_cast<u32>(mode - 2);
        if (k <= 13 && kEnter[k] != fn::kNoCode) return mode_call(kEnter[k]);
        return static_cast<u32>(mode);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Mode::set_499540", 0x00499540, &H::set_mode_499540);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_state, "Obj::set_state_499770");

struct H {
    // -------------------------------------------------------------- 0x00499770
    std::intptr_t set_state(i32 id_arg, i32 state_arg, i32 flag_arg) {
        c_state.hit();
        const i32 id = static_cast<i16>(id_arg);
        const i32 state = static_cast<i8>(state_arg);
        const i32 flag = static_cast<i8>(flag_arg);
        if (id == static_cast<i32>(jo::self<jo::Nation>(this)->id)) return id;
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->relation(joan::orig::arg::i32(id)));  // 0x004b42e0
        if (flag == 0 && jo::as_relation(e)->attitude == 1 && state == 0) {
            const i32 limit = jo::as_relation(e)->since + 5;
            if (global<i32>(d::g_get_a47bb0) < limit) return limit;
        }
        joan::orig::as_nation(this)->is_kind_3();  // 0x004b4590
        jo::as_relation(e)->attitude = static_cast<i8>(state);
        jo::view<u32>(jo::as_relation(e)->since) = global<u32>(d::g_get_a47bb0);
        const i32 m = state * 0x14;
        if (m < static_cast<i32>(jo::as_relation(e)->i16_12)) jo::as_relation(e)->i16_12 = static_cast<i16>(m);
        if (state == 4 || state == 3 || state == 1) {
            joan::orig::as_table(this)->set_byte1(joan::orig::arg::i32(id), i32{0});  // 0x004b44e0
        } else if (state == 0) {
            joan::orig::as_table(this)->set_byte1(joan::orig::arg::i32(id), i32{1});  // 0x004b44e0
        }
        if (flag != 0) return flag;
        const i32 own = jo::self<jo::Nation>(this)->id;
        void* const other = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(joan::orig::arg::i32(id));  // 0x0047b600
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_nation(other)->set_relation_state(joan::orig::arg::i32(own), joan::orig::arg::i32(state), i32{1}));     // 0x00499770
        if (state != 0) return r;
        return joan::orig::arg::as<i32>(joan::orig::as_player(this)->set_relation(joan::orig::arg::i32(id), static_cast<char>(joan::orig::arg::i32(i32{0}))));  // 0x004b4610
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Obj::set_state_499770", 0x00499770, &H::set_state);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

struct Vec3 {
    std::uint32_t a, b, c;
};
static_assert(sizeof(Vec3) == 12, "vec3");

struct H {
    // 0x00499930
    std::uint32_t get_pos(Vec3* out) {
        // dword-by-dword (read, write) like the original, in case out overlaps +0x56
        const std::uint32_t* const src = &jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->pos[0]);
        std::uint32_t* const dst = reinterpret_cast<std::uint32_t*>(out);
        dst[0] = src[0];
        dst[1] = src[1];
        const std::uint32_t c = src[2];
        dst[2] = c;
        return c;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::get_pos", 0x00499930, &H::get_pos);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using u16 = std::uint16_t;

using joan::hook::x87::ftol;

using u64 = std::uint64_t;

template <class R, class... A>
R import_call(img::Addr slot, A... args) {  // stdcall import through its IAT slot
    return (*reinterpret_cast<R(JOAN_STDCALL* const*)(A...)>(img::addr(slot)))(args...);
}

void* const kGame = obj(d::g_game_a50088);
void* const kTimer = obj(d::g_cpu_usage);
void* const kFrame = obj(d::g_frame_a51658);

struct Msg {  // MSG, native layout (PeekMessageA / GetMessageA write all of it: 0x30 bytes on x64)
    void* hwnd;
    u32 message;
    std::uintptr_t wparam;
    std::intptr_t lparam;
    u32 time;
    i32 x, y;
};
static_assert(sizeof(Msg) == (sizeof(void*) == 4 ? 28 : 48), "MSG");

JOAN_COUNTER(c_run, "Game::run");

struct H {
    // ------------------------------------------------------------------ 0x00499960
    std::uintptr_t run(i32 restart) {
        c_run.hit();
        joan::orig::as_profiler(kTimer)->init(i32{0x61});  // 0x0045c040
        if (restart == 0) jo::self<jo::Wnd>(this)->frame_count = 1;
        if (jo::view<i8>(jo::self<jo::Wnd>(this)->u8_14[4]) != 0) {
            const u32 now = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());  // 0x00591050
            joan::orig::as_timer(kFrame)->init(i32{0x1f4}, i32{0x1f4}, joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(now));  // 0x0058a830
        } else {
            const u32 now = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());
            joan::orig::as_timer(kFrame)->init(i32{0x1e}, i32{0x1e}, joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(now));
        }
        jo::self<jo::Wnd>(this)->tick_count = 0;
        jo::self<jo::Wnd>(this)->game_time = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());
        jo::self<jo::Wnd>(this)->real_time = jo::self<jo::Wnd>(this)->game_time;
        jo::self<jo::Wnd>(this)->frame_ms = 0;
        u32 previous = jo::self<jo::Wnd>(this)->real_time;
        for (;;) {
            if (jo::view<i8>(jo::self<jo::Wnd>(this)->u8_10) != 0) break;
            Msg msg{};
            if (import_call<i32>(d::iat_USER32_PeekMessageA, &msg, static_cast<void*>(nullptr), u32{0}, u32{0}, u32{0}) != 0) {  // PeekMessageA
                if (import_call<i32>(d::iat_USER32_GetMessageA, &msg, static_cast<void*>(nullptr), u32{0}, u32{0}) == 0) {      // GetMessageA
                    jo::self<jo::Wnd>(this)->u8_10 = 1;
                    break;
                }
                import_call<i32>(d::iat_USER32_TranslateMessage, &msg);  // TranslateMessage
                import_call<i32>(d::iat_USER32_DispatchMessageA, &msg);  // DispatchMessageA
                continue;
            }
            const u32 clock = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());
            const u32 wait_ms = joan::orig::arg::as<u32>(joan::orig::as_timer(kFrame)->remaining_ms(joan::orig::arg::u32(clock)));  // 0x0058aa90
            const u32 wait = import_call<u32>(d::iat_USER32_MsgWaitForMultipleObjectsEx, u32{0}, static_cast<const void*>(nullptr), wait_ms, u32{0xff}, u32{2});  // MsgWaitForMultipleObjectsEx
            if (wait != 0x102) continue;
            const u32 now = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050());
            u64 tsc = 0;
            joan::orig::as_profiler(kTimer)->read_clock(&tsc);  // 0x0045c990
            u64 elapsed = tsc - global<u64>(d::g_a485c8);
            joan::orig::as_profiler(kTimer)->frame(&elapsed);              // 0x0045cef0
            joan::orig::as_profiler(kTimer)->read_clock(obj(d::g_a485c8));
            joan::orig::as_timer(kFrame)->update_fps(joan::orig::arg::u32(now));                    // 0x0058ab10
            joan::orig::as_game(kGame)->reset_three();                          // 0x00421b60
            joan::orig::as_sound_effects(obj(d::g_sound_effects))->flush();                // 0x00490eb0
            i32 drawn = 0;
            if (static_cast<i32>(jo::self<jo::Wnd>(this)->next_mode) != static_cast<i32>(jo::self<jo::Wnd>(this)->mode)) joan::orig::as_mode(this)->set_499540(joan::orig::arg::u32(jo::view<char>(jo::self<jo::Wnd>(this)->next_mode)));  // 0x00499540
            if (jo::view<i8>(jo::self<jo::Wnd>(this)->active) != 0 && jo::view<i8>(jo::self<jo::Wnd>(this)->marked) == 0 && (joan::orig::arg::as<i32>(joan::orig::as_timer(kFrame)->not_lagging()) != 0 || jo::view<i8>(jo::self<jo::Wnd>(this)->need_paint) != 0)) {  // 0x0058acb0
                joan::orig::as_game(this)->draw();  // 0x004d48f0
                jo::self<jo::Wnd>(this)->tick_count = jo::self<jo::Wnd>(this)->tick_count + 1;
                drawn = 1;
                jo::self<jo::Wnd>(this)->need_paint = 0;
                joan::orig::as_game(kGame)->reset_three();
                joan::orig::as_game(this)->process_input_debug_menu();            // 0x0052f930
                joan::orig::as_cursor(obj(d::g_mouse))->clear_deltas();  // 0x004d8560
                global<u16>(d::g_a51802) = global<u16>(d::g_key_modifiers);
                joan::orig::as_game(kGame)->reset_three();
                global<u32>(d::g_a516e8) = global<u32>(d::g_mouse_delta_x);
                global<u32>(d::g_a516ec) = global<u32>(d::g_mouse_delta_y);
                global<u32>(d::g_a516f0) = global<u32>(d::g_mouse_wheel);
                global<u32>(d::g_mouse_delta_x) = 0;
                global<u32>(d::g_mouse_delta_y) = 0;
                global<u32>(d::g_mouse_wheel) = 0;
            }
            jo::self<jo::Wnd>(this)->frame_ms = now - previous;
            if (jo::self<jo::Wnd>(this)->frame_ms > 0x3e8) jo::self<jo::Wnd>(this)->frame_ms = 0x3e8;
            if (global<u32>(d::g_video_playing) != 0) jo::self<jo::Wnd>(this)->frame_ms = 0x21;
            if (jo::self<jo::Wnd>(this)->frame_ms == 0) {
                jo::view<u32>(jo::self<jo::Wnd>(this)->frame_scale) = 0x3f800000u;
            } else {
                const Fpu fpu{true};
                jo::self<jo::Wnd>(this)->frame_scale = static_cast<float>(fpu.mul(fpu.div(static_cast<double>(jo::self<jo::Wnd>(this)->frame_ms), global<float>(d::flt_1000)),
                                                             global<float>(d::flt_30)));
            }
            if (jo::self<jo::Wnd>(this)->frame_scale > global<float>(d::flt_5)) jo::view<u32>(jo::self<jo::Wnd>(this)->frame_scale) = 0x40a00000u;
            jo::self<jo::Wnd>(this)->game_dt = ftol(Fpu{true}.mul(static_cast<double>(jo::self<jo::Wnd>(this)->frame_ms), jo::self<jo::Wnd>(this)->speed));
            jo::self<jo::Wnd>(this)->real_time = jo::self<jo::Wnd>(this)->real_time + jo::self<jo::Wnd>(this)->frame_ms;
            jo::self<jo::Wnd>(this)->game_time = jo::self<jo::Wnd>(this)->game_time + jo::view<u32>(jo::self<jo::Wnd>(this)->game_dt);
            if (joan::orig::arg::as<i32>(joan::orig::as_timer(kFrame)->flag_14_clear()) != 0 && jo::view<i8>(jo::self<jo::Wnd>(this)->active) != 0 && jo::view<i8>(jo::self<jo::Wnd>(this)->marked) == 0) {  // 0x0058ac80
                joan::orig::as_game(this)->tick_4e5fc0();  // 0x004e5fc0
                jo::self<jo::Wnd>(this)->frame_count = jo::self<jo::Wnd>(this)->frame_count + 1;
                joan::orig::as_game(kGame)->reset_three();
            }
            if (drawn != 0) joan::orig::as_game(kGame)->reset_three();
            if (jo::view<i8>(jo::self<jo::Wnd>(this)->active) != 0 && jo::view<i8>(jo::self<jo::Wnd>(this)->marked) == 0 && drawn != 0) {
                joan::orig::as_game(kGame)->flush_loader();                               // 0x00603af0
                if (global<u32>(d::g_video_playing) != 0) joan::orig::video_copy_frame();  // direct call
            }
            joan::orig::as_stats(kFrame)->tick();  // 0x0058ace0
            previous = now;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_profiler(kTimer)->release());  // 0x0045c5c0
    }
};

JOAN_REPLACE("game/b108_07", "Game::run", 0x00499960, &H::run);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_paused, "Group::set_paused");

void* const kSounds = obj(d::g_sfx_player);

using joan::orig::helpers::count_of;  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x0049b5e0
    void set_paused(std::int8_t flag) {
        c_paused.hit();
        if (jo::self<jo::MessageList>(this)->paused == flag) return;
        if (flag != 0) {
            jo::self<jo::MessageList>(this)->pause_time = global<std::int32_t>(d::g_now);
            for (std::int32_t i = count_of(this); i > 0; --i) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;  // 0x004af300
                std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));  // 0x004af2d0
                if (jo::view<std::int32_t>(jo::as_message(m)->sound_id) != 0)
                    joan::orig::as_sounds(kSounds)->pause(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_message(m)->sound_id)));  // 0x004ced90
            }
        } else {
            const std::int32_t delta = static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::MessageList>(this)->pause_time) -
                                                                 static_cast<std::uint32_t>(global<std::int32_t>(d::g_now)));
            for (std::int32_t i = count_of(this); i > 0; --i) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;  // 0x004af300
                std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));  // 0x004af2d0
                const std::uint32_t t = joan::orig::arg::as<std::uint32_t>(joan::orig::as_message(m)->time());       // 0x004af040
                joan::orig::as_message(m)->set_time(joan::orig::arg::u32(static_cast<std::int32_t>(t + static_cast<std::uint32_t>(delta))));  // 0x004af070
                if (jo::view<std::int32_t>(jo::as_message(m)->sound_id) != 0)
                    joan::orig::as_sounds(kSounds)->resume(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_message(m)->sound_id)));  // 0x00470c90
            }
        }
        jo::self<jo::MessageList>(this)->paused = flag;
    }

    // ------------------------------------------------------------------ 0x00499fd0
    void set_62(std::uint32_t v) {
        jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->yaw_) = v;
        if (jo::self<jo::MapObject>(this)->actor != nullptr) joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_yaw(joan::orig::arg::u32(v));  // 0x005ac6a0
    }

    // ------------------------------------------------------------------ 0x0049adc0
    void set_6a(std::uint32_t v) {
        jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->roll_) = v;
        if (jo::self<jo::MapObject>(this)->actor != nullptr) joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_roll(joan::orig::arg::u32(v));  // 0x005ac980
    }

    // ------------------------------------------------------------------ 0x00499e20
    float get_62() { return jo::self<jo::MapObject>(this)->yaw_; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("Group::set_paused", 0x0049b5e0, &H::set_paused);
B("Obj::set_6a", 0x0049adc0, &H::set_6a);
B("Obj::set_62", 0x00499fd0, &H::set_62);
B("Obj::get_62", 0x00499e20, &H::get_62);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

using joan::call::thiscall;

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }

JOAN_COUNTER(c_bump_oldest, "Timers::bump_oldest");

struct H {
    // 0x00499e40
    void bump_oldest() {
        c_bump_oldest.hit();
        if (jo::self<jo::MessageList>(this)->dirty != 0) return;
        jo::self<jo::MessageList>(this)->dirty = 1;
        std::uint32_t min1 = now();
        std::uint8_t flag = 0;
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;           // 0x004af300
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));                      // 0x004af2d0
            if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_message(e)->priority()) < min1) min1 = joan::orig::arg::as<std::uint32_t>(joan::orig::as_message(e)->priority());  // 0x004aef10
        }
        std::uint32_t min2 = now();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;           // 0x004af300
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));                      // 0x004af2d0
            if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_message(e)->priority()) == min1) continue;              // 0x004aef10
            if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_message(e)->priority()) < min2) {                       // 0x004aef10
                min2 = joan::orig::arg::as<std::uint32_t>(joan::orig::as_message(e)->priority());                         // 0x004aef10
                flag = jo::view<std::uint8_t>(jo::as_message(e)->has_pic) == 0 ? 1 : 0;
            }
        }
        if (flag == 0) return;
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;           // 0x004af300
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));                      // 0x004af2d0
            if (jo::view<std::uint8_t>(jo::as_message(e)->has_pic) != 0) continue;
            jo::view<std::uint16_t>(jo::as_message(e)->shown_len) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(jo::as_message(e)->shown_len) + 0x28);
        }
    }

    // 0x0049a8c0
    float get_66() { return jo::self<jo::MapObject>(this)->pitch; }

    // 0x0049ab00
    void set_66(float v) {  // a float argument (XMM on x64; the facade declares it float) - the bits go to +0x66 and to the actor
        const std::uint32_t bits = joan::orig::arg::bits(v);
        jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->pitch) = bits;
        if (jo::self<jo::MapObject>(this)->actor != nullptr) joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_pitch(joan::orig::arg::u32(bits));  // 0x005ac810
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Timers::bump_oldest", 0x00499e40, &H::bump_oldest);
B("Unit::get_66", 0x0049a8c0, &H::get_66);
B("Unit::set_66", 0x0049ab00, &H::set_66);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b109_04.cpp (game/b109_04)
namespace {
namespace game_b109_04 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::orig::helpers::wadd;

using joan::orig::helpers::gf;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kSoldiers = obj(d::g_units);
void* const kUnitTypes = obj(d::g_item_res);
void* const kRanges = obj(d::g_model_refs);

JOAN_COUNTER(c_reach, "Unit::can_reach_49a010");

u8* soldier(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
// the ModelSlot of the model-ref id (its times[2] / [6] / [7] are read below)
jo::ModelSlot* range_rec(u32 c) { return joan::orig::arg::as<jo::ModelSlot*>(joan::orig::as_model_refs(kRanges)->get(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(c))))); }  // 0x00416f20

// --------------------------------------------------------------------- 0x0049a010 helpers
// [this+0x56 / +0x5a] relative point rotated by the angle object a (cos / sin raw st0).
struct Rot {
    float x, y;
};
Rot rotate(const void* self, float* a, float px, float py) {
    const Fpu fpu{true};
    const double c1 = joan::orig::arg::as<double>(joan::orig::as_turns(a)->cos_577b50());  // 0x00577b50 cos
    const float t1 = static_cast<float>(fpu.mul(static_cast<double>(px - jo::as_map_object(self)->pos[0]), c1));
    const double s1 = joan::orig::arg::as<double>(joan::orig::as_turns(a)->sin_577b10());  // 0x00577b10 sin
    const double p1 = fpu.mul(static_cast<double>(py - jo::as_map_object(self)->pos[1]), s1);
    const float x = static_cast<float>(fpu.sub(static_cast<double>(t1), p1));
    const double s2 = joan::orig::arg::as<double>(joan::orig::as_turns(a)->sin_577b10());
    const float t2 = static_cast<float>(fpu.mul(static_cast<double>(px - jo::as_map_object(self)->pos[0]), s2));
    const double c2 = joan::orig::arg::as<double>(joan::orig::as_turns(a)->cos_577b50());
    const double p2 = fpu.mul(static_cast<double>(py - jo::as_map_object(self)->pos[1]), c2);
    const float y = static_cast<float>(fpu.add(p2, static_cast<double>(t2)));
    return Rot{x, y};
}

// ------------------------------------------------------------------------------------ H
struct H {
    // ---------------------------------------------------------------- 0x0049a010
    u8 can_reach_49a010(u32 id, u32 mode) {
        c_reach.hit();
        u8* const e = soldier(id);
        if (jo::view<i8>(jo::as_unit(e)->in_scene) == 0) return 0;
        const u32 c = jo::view<u32>(jo::as_anim_group_rec(joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(e)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(e)->anim_entry)))))->model_ref);  // 0x004168d0
        i32 r10 = 0;
        const i32 r14 = 0;
        if (jo::as_unit(e)->carried_a != 0) {
            u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(static_cast<i32>(jo::as_unit(e)->carried_a)));  // 0x00425d20
            r10 = jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(a)->type_id)))))->i32_32;  // 0x00425ce0
        }
        if (jo::as_unit(e)->carried_b != 0) {
            u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(static_cast<i32>(jo::as_unit(e)->carried_b)));
            r10 = jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(a)->type_id)))))->i32_32;
        }
        if (r10 < r14) r10 = r14;
        if (joan::orig::arg::as<i32>(joan::orig::as_players(obj(d::g_nations))->hostile(static_cast<i32>(jo::self<jo::MapObject>(this)->side), joan::orig::arg::i32(// 0x004b6860
                          static_cast<i32>(jo::as_unit(e)->side)))) == 0)
            return 0;
        V3 v20{0.0f, 0.0f, 0.0f};
        joan::orig::as_unit(e)->position(&v20);  // 0x004fedb0
        V3 v2c{0.0f, 0.0f, 0.0f};
        joan::orig::as_map_object(this)->position(&v2c);  // 0x00499930
        {
            const i32 reach = wadd(range_rec(c)->times[2], r10);
            if (joan::orig::arg::as<u8>(joan::orig::as_unit(this)->in_footprint_41c5e0(v20.x, v20.y, joan::orig::arg::bits(v20.z), joan::orig::arg::i32(reach))) == 0) return 0;  // 0x0041c5e0
        }
        const double dz = joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(v20.z - v2c.z))));  // fabs
        {
            const i32 reach = wadd(range_rec(c)->times[2], r10);
            if (!(static_cast<double>(reach) >= dz)) return 0;
        }
        float f30 = static_cast<float>(range_rec(c)->times[6]);
        float f34 = static_cast<float>(range_rec(c)->times[7]);
        const float f38 = static_cast<float>(wadd(range_rec(c)->times[2], r10));
        if (mode == 0) {
            const float f8c = joan::orig::arg::as<float>(joan::orig::as_unit(e)->face_object(static_cast<i32>(jo::self<jo::MapObject>(this)->index)));  // 0x005059b0
            if (!(f30 >= f34)) {
                const float t = f30;
                f30 = f34;
                f34 = t;
            }
            if (!(f8c >= f34)) return 0;
            if (f8c > f30) return 0;
            return 1;
        }
        (void)joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::MapObject>(this)->type_id)));  // 0x00425ce0 (dead result)
        float a40 = 0.0f;
        joan::orig::as_angle(&a40)->construct_577190();              // 0x00577190
        joan::orig::as_angle(&a40)->set_wrapped(joan::orig::arg::f32(-jo::self<jo::MapObject>(this)->yaw_));    // 0x00577340
        const Rot ra = rotate(this, &a40, v20.x, v20.y);
        const V3 A{ra.x, ra.y, 0.0f};  // z: uninitialised in the original, never read
        float a88 = 0.0f;
        joan::orig::as_angle(&a88)->construct_577190();  // 0x00577190
        const Fpu fpu{true};
        auto test = [&](float heading) -> bool {
            joan::orig::as_angle(&a88)->set_wrapped(joan::orig::arg::f32(heading));  // 0x00577340
            const double cosv = joan::orig::arg::as<double>(joan::orig::as_turns(&a88)->cos_577b50());
            const float f74 = static_cast<float>(fpu.add(fpu.mul(cosv, static_cast<double>(f38)), static_cast<double>(v20.x)));
            const double s = joan::orig::arg::as<double>(joan::orig::as_turns(&a88)->sin_577b10());
            const float f78 = static_cast<float>(fpu.add(fpu.mul(s, static_cast<double>(f38)), static_cast<double>(v20.y)));
            const Rot rb = rotate(this, &a40, f74, f78);
            const V3 B{rb.x, rb.y, 0.0f};  // z: uninitialised in the original, never read
            return joan::orig::arg::as<u8>(joan::orig::as_map_object(this)->rect_hit(A, B)) != 0;  // 0x00444a20
        };
        if (test(jo::as_unit(e)->heading)) return 1;
        if (test(f30 * gf(d::flt_0_017453292) + jo::as_unit(e)->heading)) return 1;
        if (test(f34 * gf(d::flt_0_017453292) + jo::as_unit(e)->heading)) return 1;
        if (f30 > gf(d::flt_90) || !(f30 >= gf(d::flt_m90))) {
            const float q = f30 > gf(d::k_9cf5c4) ? 1.57079637f : -1.57079637f;  // 0x3fc90fdb / 0xbfc90fdb
            if (test(q + jo::as_unit(e)->heading)) return 1;
        }
        if (f34 > gf(d::flt_90) || !(f34 >= gf(d::flt_m90))) {
            const float q = f34 > gf(d::k_9cf5c4) ? 1.57079637f : -1.57079637f;
            if (test(q + jo::as_unit(e)->heading)) return 1;
        }
        return 0;
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_04", name, addr, fn)
B("Unit::can_reach_49a010", 0x0049a010, &H::can_reach_49a010);
#undef B

}  // namespace game_b109_04
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

using joan::call::thiscall;
using joan::img::obj;


void* const kArmies = obj(d::g_nations);

i32 army_missing(i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_list(kArmies)->is_null(joan::orig::arg::i32(id))); }  // 0x00484870
u8* army_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kArmies)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0047b600

JOAN_COUNTER(c_find_unit, "Region::find_unit");

struct H {
    // ------------------------------------------------------------------ 0x0049a8e0
    i32 find_unit(i32* count) {
        c_find_unit.hit();
        if (jo::view<void*>(jo::self<jo::Place>(this)->world) == nullptr) return 0;
        if (army_missing(jo::self<jo::Place>(this)->nation) == 0) (void)army_get(jo::self<jo::Place>(this)->nation);  // result unused
        float box[6];
        std::memcpy(box, jo::self<jo::Place>(this)->box, sizeof box);
        const float g = global<float>(d::flt_2000);
        box[0] = box[0] - g;
        box[1] = box[1] - g;
        box[2] = box[2] - g;
        box[3] = box[3] + g;
        box[4] = box[4] + g;
        box[5] = box[5] + g;
        i32 n = 0;
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Place>(this)->world))->cells_in_box(joan::orig::arg::mptr(box), &n));  // 0x00429090
        i32 result = 0;
        i32 found = 0;
        for (i32 i = 0; i < n; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cells[i])->get_8c());  // 0x0054ee10
            if (first == nullptr) continue;
            u8* e = first;
            do {
                if (jo::as_unit(e)->hit_points > 0 || jo::view<u32>(jo::as_unit(e)->state_120) != 0) {
                    if (static_cast<i32>(jo::as_unit(e)->group_id) == static_cast<i32>(jo::self<jo::Place>(this)->index) + 1000) {
                        if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(box)->contains(joan::orig::arg::ptr(jo::as_unit(e)->position_))) & 0xffu) != 0) {  // 0x0060d5f0
                            result = jo::as_unit(e)->id;
                            ++found;
                        }
                    }
                }
                e = jo::view<u8*>(jo::as_unit(e)->cell_prev);
            } while (e != nullptr && e != first);
        }
        *count = found;
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Region::find_unit", 0x0049a8e0, &H::find_unit);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

struct H {
    // 0x0049ab40
    void* slot52(std::int16_t i) {
        return &jo::self<jo::TrackRes>(this)->paths[i - 1];  // 0x52-byte PathGraph records, 1-based
    }
    // 0x0049ad10
    float float_6a() { return jo::self<jo::MapObject>(this)->roll_; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Table52::slot", 0x0049ab40, &H::slot52);
B("Obj::float_6a", 0x0049ad10, &H::float_6a);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_advance, "List::advance_state");

i32 count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------------ 0x0049ab70
    void advance_state() {
        c_advance.hit();
        const u32 who = global<i32>(d::g_control_mode) == 4 ? global<u32>(d::g_control_base_mode) : global<u32>(d::g_control_mode);
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(obj(d::g_keys_a48820))->test(joan::orig::arg::i32(who), 0x2b)) == 0) return;  // 0x004cb820
        for (i32 i = count(this); i > 0; --i) {
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;  // 0x004af300
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));   // 0x004af2d0
            if (jo::view<i32>(jo::as_message(e)->state) == 3) {
                const float v = static_cast<float>(static_cast<i32>(jo::as_message(e)->len));
                joan::orig::as_message(e)->set_3b(joan::orig::arg::bits(v));  // 0x004af140
                jo::view<i32>(jo::as_message(e)->state) = 4;
                return;
            }
            if (jo::view<i32>(jo::as_message(e)->state) == 4 || jo::view<i32>(jo::as_message(e)->state) == 5) {
                jo::view<i32>(jo::as_message(e)->state) = 6;
                joan::orig::as_timers(obj(d::g_names))->bump_oldest();  // 0x00499e40
                return;
            }
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("List::advance_state", 0x0049ab70, &H::advance_state);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_ui1.cpp (game/sky2)
namespace {
namespace game_ui1 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::img::str;


constexpr img::Addr kTextures(d::g_texture_array);

JOAN_COUNTER(c_sky, "World::setup_sky");

struct H {
    // ---- game/sky2 ---------------------------------------------------------------------
    // 0x0049ae00 World::setup_sky (the original's SEH frame only frees the two `new`s if their
    // constructors throw; normal path kept). ret 0.
    void setup_sky() {
        c_sky.hit();
        if (static_cast<std::int8_t>(jo::self<jo::World>(this)->loaded) != 0) return;
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Scene)));  // 0xd5 on i686
        jo::view<void*>(jo::self<jo::World>(this)->scene) = mem != nullptr ? joan::orig::as_scene(mem)->construct() : nullptr;  // 0x005e9a00 Scene ctor
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->reset();                                          // 0x005e9ac0
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->setup(joan::orig::arg::mptr(jo::self<jo::World>(this)->bounds));                       // 0x005e9e50
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->set_fog_flag(joan::orig::arg::i32(static_cast<std::int8_t>(global<std::uint8_t>(d::g_a58054)) != 0 ? 1 : 0));  // 0x005ee140
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->set_mode(1u);                                       // 0x005ef730
        joan::orig::as_world(this)->load_pfd();                                                    // 0x00442d30
        joan::orig::as_sound_stream(img::obj(d::g_snd))->update();                     // 0x0059b1c0
        joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();                     // 0x005866d0
        char* name = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(jo::self<jo::World>(this)->name), 0x5c));  // strrchr
        name = name + 1;
        name = joan::orig::arg::as<char*>(joan::orig::libc::_strupr(joan::orig::arg::str(name)));  // _strupr
        void* const weather = img::obj(d::g_mini_map);
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_bmp_res(&jo::self<jo::World>(this)->bitmaps)->load(joan::orig::arg::str(name)))) != 0)  // 0x00620a50
            joan::orig::as_panel(weather)->set_unit(joan::orig::arg::i16(jo::self<jo::World>(this)->world_id));                                          // 0x00715d50
        else
            joan::orig::as_panel(weather)->set_unit(joan::orig::arg::i16(static_cast<std::int16_t>(0)));
        global<std::uint8_t*>(d::g_main_light) = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->add_light());  // 0x005eb5d0
        joan::orig::as_camera(global<std::uint8_t*>(d::g_main_light))->init(3u);                            // 0x005c58a0
        jo::as_light(global<std::uint8_t*>(d::g_main_light))->colour.r = 0xff;
        jo::as_light(global<std::uint8_t*>(d::g_main_light))->colour.g = 0xff;
        jo::as_light(global<std::uint8_t*>(d::g_main_light))->colour.b = 0xff;
        jo::as_light(global<std::uint8_t*>(d::g_main_light))->colour.a = 0xff;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->orbit_b) = 0x3fc90fdbu;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->orbit_a) = 0;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->range) = 0x437a0000u;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->position[0]) = 0x42c80000u;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->position[1]) = 0x42c80000u;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->position[2]) = 0x43480000u;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->theta) = 0x3eb2b8c2u;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->phi) = 0x3eb2b8c2u;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->attenuation0) = 0x3f800000u;
        jo::view<std::uint32_t>(jo::as_light(global<std::uint8_t*>(d::g_main_light))->attenuation1) = 0x3a83126fu;
        jo::as_light(global<std::uint8_t*>(d::g_main_light))->active_ = 0;
        joan::orig::as_light(global<std::uint8_t*>(d::g_main_light))->invalidate();  // 0x005c5c00
        joan::orig::as_light_sys(img::obj(d::g_light_sys))->init(jo::self<jo::World>(this)->name, joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::World>(this)->scene)));                        // 0x004a5020
        joan::orig::as_cloud_layers(img::obj(d::g_cloud_layers))->build(joan::orig::arg::str(img::obj(d::g_a482d4)), joan::orig::arg::str(img::obj(d::g_a48314)));  // 0x00458720
        joan::orig::as_sound_stream(img::obj(d::g_snd))->update();  // 0x0059b1c0
        joan::orig::as_ambient_streams(img::obj(d::g_mus))->update();  // 0x005866d0
        void* const mem2 = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SkyBox)));  // 0x124 on i686
        std::uint8_t* const sky = mem2 != nullptr ? joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sky_box(mem2)->construct()) : nullptr;  // 0x005f5100 Plane ctor
        if (global<std::int32_t>(d::g_a52623) != 0) {
            alignas(4) std::uint8_t file[sizeof(jo::FileMem)];  // 0x11c bytes on i686
            joan::orig::as_file_mem(file)->construct();                                                 // 0x00587f80 ctor
            joan::orig::as_file_mem(file)->open_588000(joan::orig::arg::str(img::obj(d::g_a48354)), 1, static_cast<char>(joan::orig::arg::i32(0)));             // 0x00588000
            joan::orig::as_plane(sky)->load_5ddd70(joan::orig::arg::mptr(file), static_cast<std::uint8_t>(joan::orig::arg::u32(0)));                                                // 0x005ddd70
            for (std::int32_t i = 0; i < jo::as_plane(sky)->material_count; ++i) {
                jo::as_plane(sky)->materials[i].texture_mode = 0x10;
                jo::as_plane(sky)->materials[i].blend = 2;
            }
            joan::orig::as_plane(sky)->set_100(1u);  // 0x005d6e20
            joan::orig::as_file_mem(file)->destruct();    // 0x00587fd0 dtor
        } else if (global<std::int32_t>(d::g_a52637) != 0) {
            joan::orig::as_sky_box(sky)->init(1);                                          // 0x005f51b0
            const std::int32_t first = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(sky)->add_materials(1));  // 0x005cd470
            jo::Material* const material = jo::as_plane(sky)->materials + first;
            joan::orig::as_material(material)->construct();  // 0x005d52a0
            material->blend = 2;
            void* const texture = joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(&img::global<const char>(d::s_SKY1)), 2, 0u);  // 0x006099c0 'SKY1'
            joan::orig::as_material(material)->set_texture(0, texture);  // 0x005d5650
            joan::orig::texture_ref::release(texture);             // 0x0042cef0
            joan::orig::as_sky_box(sky)->build(500.0f, joan::orig::arg::i32(first), 0x3f);  // 0x005f5240
        } else {
            joan::orig::as_sky_box(sky)->init(0);
            const std::int32_t first = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(sky)->add_materials(6));
            for (std::int32_t i = 0; i < 6; ++i) {
                jo::Material* const material = jo::as_plane(sky)->materials + (first + i);
                joan::orig::as_material(material)->construct();
                material->blend = 2;
                char face[8] = {};
                std::memcpy(face, img::obj(d::s_SKY11), 6);  // "SKY11"
                face[4] = static_cast<char>(i + 0x31);
                void* const texture = joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(static_cast<const char*>(face)), 1, 0u);
                joan::orig::as_material(material)->set_texture(0, texture);
                joan::orig::texture_ref::release(texture);
            }
            joan::orig::as_sky_box(sky)->build(500.0f, joan::orig::arg::i32(first), 0x3f);
        }
        joan::orig::as_plane(sky)->invalidate(joan::orig::arg::u32(-1));             // 0x005cf920
        joan::orig::as_plane(sky)->sort_by_material(1);              // 0x005cf1d0
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::World>(this)->scene))->set_sky(joan::orig::arg::mptr(sky));  // 0x005ea060
        jo::self<jo::World>(this)->loaded = 1;
    }
};

#define K2(name, addr, fn) JOAN_REPLACE("game/sky2", name, addr, fn)
K2("World::setup_sky", 0x0049ae00, &H::setup_sky);
#undef K2

}  // namespace game_ui1
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

struct H {
    // ------------------------------------------------------------------ 0x0049b410
    float one_49b410(u32 /*unused*/) { return global<float>(d::flt_1); }  // 1.0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::one_49b410", 0x0049b410, &H::one_49b410);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

struct V3 {  // 12-byte vector passed by value to SoundList::play_effect (Unit::emit_particles takes float members: XMM registers on System V)
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "v3");

JOAN_COUNTER(c_named_sound, "Unit::play_named_sound");

struct H {
    // 0x0049b4d0
    std::int32_t play_named_sound(const char* suffix) {
        c_named_sound.hit();
        std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::self<jo::MapObject>(this)->type_id)));  // 0x00425ce0
        char* const slash = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(jo::as_item_def(rec)->name), 0x5c));  // strrchr
        alignas(4) std::uint8_t name[0x194];
        joan::orig::as_string(name)->construct();  // 0x005a1170
        if (slash != nullptr)
            joan::orig::as_string(name)->assign(joan::orig::arg::str(static_cast<const char*>(slash + 1)));  // 0x005a1440
        else
            joan::orig::as_string(name)->assign(joan::orig::arg::str(static_cast<const char*>(jo::as_item_def(rec)->name)));
        joan::orig::as_string(name)->append(str(d::s_us_9cf774));  // 0x005a1550 "_"
        joan::orig::as_string(name)->append(joan::orig::arg::str(suffix));
        const std::int32_t sound =
            joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(joan::orig::arg::str(joan::orig::as_string(name)->c_str())));  // 0x004d12f0, 0x00417d90
        if (sound == 0) return 0;
        const V3 pos = jo::view<V3>(jo::self<jo::MapObject>(this)->pos);
        char* const text = joan::orig::as_string(name)->c_str();  // 0x00417d90
        joan::orig::as_sound_list(obj(d::g_sfx_player))->play_effect(text, pos);  // 0x00494810
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Unit::play_named_sound", 0x0049b4d0, &H::play_named_sound);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
namespace {
namespace game_b100_04 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

void* const kTypes = obj(d::g_item_res);
void* const kLights = obj(d::g_lights);

u8* type_record(u16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

JOAN_COUNTER(c_setup_light, "Unit::setup_light");

struct H {
    // ------------------------------------------------------------------ 0x0049b7a0
    std::uintptr_t setup_light() {
        c_setup_light.hit();
        if (jo::self<jo::MapObject>(this)->light_slot != 0) return reinterpret_cast<std::uintptr_t>(this);
        if (jo::self<jo::MapObject>(this)->actor == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        u8* const rec = type_record(jo::view<u16>(jo::self<jo::MapObject>(this)->type_id));
        if (static_cast<i32>(jo::view<i8>(jo::as_item_def(rec)->sound_params[0])) == 0) return 0;
        u8* const light = joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::view<void*>(jo::as_world(global<void*>(d::g_world))->scene))->add_light());  // 0x005eb5d0
        const i32 kind = jo::view<i8>(jo::as_item_def(rec)->sound_params[1]);
        if (static_cast<u32>(kind) <= 3) joan::orig::as_camera(light)->init(joan::orig::arg::u32(kind));  // 0x005c58a0
        std::memcpy(&jo::as_light(light)->colour, &jo::as_item_def(rec)->sound_params[3], 4);  // the dword at sound_params[3..6]
        jo::view<u32>(jo::as_light(light)->orbit_a) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[3]);
        jo::view<u32>(jo::as_light(light)->orbit_b) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[4]);
        jo::view<u32>(jo::as_light(light)->position[0]) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[0]);
        jo::view<u32>(jo::as_light(light)->position[1]) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[1]);
        jo::view<u32>(jo::as_light(light)->position[2]) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[2]);
        jo::view<u32>(jo::as_light(light)->attenuation0) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[5]);
        jo::view<u32>(jo::as_light(light)->attenuation1) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[6]);
        jo::view<u32>(jo::as_light(light)->attenuation2) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[7]);
        jo::view<u32>(jo::as_light(light)->range) = jo::view<u32>(jo::as_item_def(rec)->sound_floats[8]);
        jo::view<u16>(jo::self<jo::MapObject>(this)->light_slot) = joan::orig::arg::as<u16>(joan::orig::as_light_faders(kLights)->add_group());  // 0x004b0640
        void* const slot = joan::orig::arg::as<void*>(joan::orig::as_list(kLights)->get_at_4(static_cast<i32>(jo::self<jo::MapObject>(this)->light_slot)));  // 0x00484710
        joan::orig::as_children(slot)->set_type(joan::orig::arg::u32(static_cast<i32>(jo::view<i8>(jo::as_item_def(rec)->sound_params[2]))));                   // 0x004afcf0
        joan::orig::as_children(slot)->add_light(light)
;                                                   // 0x004b0250
        jo::Bone* const model = jo::self<jo::MapObject>(this)->actor->bone;
        if (jo::as_light(light)->bone_ == nullptr) return joan::orig::arg::as<u32>(model);
        joan::orig::as_bone(jo::view<void*>(jo::as_light(light)->bone_))->set_parent(joan::orig::arg::mptr(model));  // 0x0060ec70
        return 0;  // 0x0060ec70 returns nothing; the caller (mission.cpp) ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Unit::setup_light", 0x0049b7a0, &H::setup_light);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MapObject::set_world(World* a0) -> void { joan::bind::member<&::game_b83_07::H::set_state>(this, a0); }  // 0x4994b0
auto joan::orig::Mode::set_499540(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_04::H::set_mode_499540>(this, a0)); }  // 0x499540
auto joan::orig::Nation::set_relation_state(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_01::H::set_state>(this, a0, a1, a2)); }  // 0x499770
auto joan::orig::MapObject::position(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_03::H::get_pos>(this, a0)); }  // 0x499930
auto joan::orig::MapObject::yaw() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b84_06::H::get_62>(this)); }  // 0x499e20
auto joan::orig::Timers::bump_oldest() -> void { joan::bind::member<&::game_b92_02::H::bump_oldest>(this); }  // 0x499e40
auto joan::orig::MapObject::set_yaw(std::uint32_t a0) -> void { joan::bind::member<&::game_b84_06::H::set_62>(this, a0); }  // 0x499fd0
auto joan::orig::Unit::can_reach_49a010(std::uint32_t a0, std::uint32_t a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_04::H::can_reach_49a010>(this, a0, a1)); }  // 0x49a010
auto joan::orig::Unit::get_66() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_02::H::get_66>(this)); }  // 0x49a8c0
auto joan::orig::Table52::slot(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_03::H::slot52>(this, a0)); }  // 0x49ab40
auto joan::orig::List::advance_state() -> void { joan::bind::member<&::game_b99_03::H::advance_state>(this); }  // 0x49ab70
auto joan::orig::MapObject::roll() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_03::H::float_6a>(this)); }  // 0x49ad10
auto joan::orig::MapObject::set_roll(std::uint32_t a0) -> void { joan::bind::member<&::game_b84_06::H::set_6a>(this, a0); }  // 0x49adc0
auto joan::orig::World::setup_sky() -> void { joan::bind::member<&::game_ui1::H::setup_sky>(this); }  // 0x49ae00
auto joan::orig::Unit::play_named_sound(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::play_named_sound>(this, a0)); }  // 0x49b4d0
auto joan::orig::Group::set_paused(std::int8_t a0) -> void { joan::bind::member<&::game_b84_06::H::set_paused>(this, a0); }  // 0x49b5e0
auto joan::orig::Unit::setup_light() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b100_04::H::setup_light>(this)); }  // 0x49b7a0
