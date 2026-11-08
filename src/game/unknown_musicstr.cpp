// game/unknown_musicstr.cpp: functions whose original source file is unknown (formerly game/_unplaced/musicstr.cpp;
// re/attribution.csv: between ww/ilib/dynarrb.cpp and ww/ilib/error.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdarg>
#include <chrono>
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::hook::global;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

JOAN_COUNTER(c_close_586010, "Obj::close_586010");

struct H {
    // 0x00585f70 / 0x0059acd0
    void* clear_50() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::SoundStreams3));  // 0x50 on x86
        return this;
    }

    void delete_slot(jo::Video*& slot) {
        if (slot == nullptr) return;
        void* const o = slot;
        if (o != nullptr) joan::orig::as_video(o)->destroy(1u);  // 0x00586150
        slot = nullptr;
    }

    // 0x00586010
    void close_586010() {
        c_close_586010.hit();
        if (jo::self<jo::SoundStreams3>(this)->open == 0) return;
        joan::orig::as_slots(this)->release(0);  // 0x005861a0
        joan::orig::as_slots(this)->release(1);
        joan::orig::as_slots(this)->release(2);
        delete_slot(jo::self<jo::SoundStreams3>(this)->videos[0]);
        delete_slot(jo::self<jo::SoundStreams3>(this)->videos[1]);
        delete_slot(jo::self<jo::SoundStreams3>(this)->videos[2]);
        jo::self<jo::SoundStreams3>(this)->open = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Obj::clear_50", 0x00585f70, &H::clear_50);
B("Obj::close_586010", 0x00586010, &H::close_586010);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
namespace {
namespace game_b97_05 {

struct H {
    // 0x00585fa0
    std::uint32_t call_586010() { joan::orig::as_ambient_streams(this)->close(); return 0; }  // 0x00586010 (void; atexit handler, result ignored)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Obj::call_586010", 0x00585fa0, &H::call_586010);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
namespace {
namespace game_b99_06 {

using i16 = std::int16_t;

struct H {
    // 0x00585fd0
    void init_585fd0() {
        joan::orig::as_ambient_streams(this)->close();  // 0x00586010
        jo::self<jo::SoundStreams3>(this)->volume = static_cast<i32>(global<i16>(d::g_a57ff4));
        jo::self<jo::SoundStreams3>(this)->open = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("Obj::init_585fd0", 0x00585fd0, &H::init_585fd0);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x00586150
    void* scalar_dtor(std::uint32_t flags) {
        joan::orig::as_video(this)->shutdown();  // 0x00684620
        if (flags & 1u) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Obj::scalar_dtor_586150", 0x00586150, &H::scalar_dtor);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x005861a0
    std::int32_t slots_release(std::int32_t i) {
        if (jo::self<jo::Slots>(this)->u32_0 == 0) return -1;
        void* const s = jo::self<jo::SoundStreams3>(this)->videos[i];
        if (s != nullptr) joan::orig::as_video(s)->close();  // 0x00686160
        jo::view<std::uint32_t>(jo::self<jo::SoundStreams3>(this)->tag[i]) = 0;
        return 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Slots::release", 0x005861a0, &H::slots_release);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using joan::img::obj;

using joan::img::str;

using u8 = std::uint8_t;

JOAN_COUNTER(c_music, "Music::play");

struct H {
    // ------------------------------------------------------------------ 0x00586210
    u32 play_music(i32 track, i32 slot, u32 loop) {
        c_music.hit();
        if (jo::self<jo::SoundStreams3>(this)->open == 0) return 0;
        if (track < 1 || track > global<i32>(d::g_music_res)) return 0;
        if (slot == 0 && global<void*>(d::g_a5cfa0) == nullptr) return 0;
        if (slot == 1 && global<void*>(d::g_a5cfa4) == nullptr) return 0;
        if (slot == 2 && global<void*>(d::g_a5cfa8) == nullptr) return 0;
        joan::orig::as_slots(this)->release(joan::orig::arg::i32(slot));  // 0x005861a0
        if (jo::self<jo::SoundStreams3>(this)->videos[slot] == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Video)));  // 0x30 on x86; operator new
            void* channel = nullptr;
            if (mem != nullptr) channel = joan::orig::as_video(mem)->construct();  // 0x006845d0
            jo::self<jo::SoundStreams3>(this)->videos[slot] = static_cast<jo::Video*>(channel);
        }
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_music_res(obj(d::g_music_res))->track(joan::orig::arg::i32(track)));  // 0x004d8be0
        char path[0x104];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_MUSIC_9d6248));  // strcpy "MUSIC\\"
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), joan::orig::arg::str(jo::as_music_track(rec)->file));  // strcat
        jo::Sound3D* sound;  // the Panels' Sound3D source of the slot
        if (slot == 0)
            sound = global<jo::Sound3D*>(d::g_a5cfa0);
        else if (slot == 1)
            sound = global<jo::Sound3D*>(d::g_a5cfa4);
        else
            sound = global<jo::Sound3D*>(d::g_a5cfa8);
        const auto t0 = std::chrono::steady_clock::now();
        joan::orig::as_video(jo::self<jo::SoundStreams3>(this)->videos[slot])->open_684650(joan::orig::arg::str(static_cast<char*>(path)), sound);  // 0x00684650
        joan::hook::log_line("[Audio] music %s started in slot %d: open took %.1f ms", path, slot,
                             std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count());
        if (jo::self<jo::SoundStreams3>(this)->videos[slot]->sound != nullptr) {
            joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::SoundStreams3>(this)->videos[slot]->sound))->set_position(joan::orig::arg::mptr(obj(d::g_pos)));  // 0x0059fb40
            joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::SoundStreams3>(this)->videos[slot]->sound))->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})));  // 0x0059fae0
        }
        jo::self<jo::SoundStreams3>(this)->tag[slot] = track;
        jo::view<u32>(jo::self<jo::SoundStreams3>(this)->mode[slot]) = loop;
        jo::self<jo::SoundStreams3>(this)->start_time[slot] = global<u32>(d::g_game_time_ms);
        jo::self<jo::SoundStreams3>(this)->fade_time[slot] = 0x1388;
        joan::orig::as_volume(this)->set(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::SoundStreams3>(this)->volume)));                // 0x00586620
        return joan::orig::arg::as<u32>(joan::orig::as_video(jo::self<jo::SoundStreams3>(this)->videos[slot])->has_audio_stream());  // 0x00685990
    }
};

JOAN_REPLACE("game/b108_07", "Music::play", 0x00586210, &H::play_music);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_stream2.cpp (game/stream2)
namespace {
namespace game_stream2 {

JOAN_COUNTER(c_running3, "SoundStreams3::running");

struct H {
    std::int32_t running3(std::int32_t tag, std::int32_t slot) {  // 0x005864f0 (on 0xa5cd70)
        if (jo::view<std::int32_t>(jo::self<jo::SoundStreams3>(this)->open) == 0) return 0;
        c_running3.hit();
        if (jo::self<jo::SoundStreams3>(this)->tag[slot] == 0) return 0;
        if (jo::self<jo::SoundStreams3>(this)->videos[slot] == nullptr) return 0;
        if (tag != 0 && tag != jo::self<jo::SoundStreams3>(this)->tag[slot]) return 0;
        if (jo::self<jo::SoundStreams3>(this)->mode[slot] == 1) return 1;
        const std::int32_t r = thiscall<std::int32_t>(
            fn::VideoFile_get_next, static_cast<void*>(jo::self<jo::SoundStreams3>(this)->videos[slot]), global<std::uint32_t>(d::g_game_time_ms) - jo::self<jo::SoundStreams3>(this)->start_time[slot]);  // 0x00685a80
        if (r == 4 || r == 3) return 0;
        if (jo::self<jo::SoundStreams3>(this)->mode[slot] == 2 && jo::view<std::int32_t>(jo::self<jo::SoundStreams3>(this)->fade_time[slot]) == 0) return 0;
        if (jo::self<jo::SoundStreams3>(this)->mode[slot] == 0 && r == 0) return 0;
        return 1;
    }
};

#define STREAM2(name, addr, fn) JOAN_REPLACE("game/stream2", name, addr, fn)
STREAM2("SoundStreams3::running", 0x005864f0, &H::running3);
#undef STREAM2

}  // namespace game_stream2
}  // namespace

// ---------------------------------------- from modules/game_stream1.cpp (game/stream1)
namespace {
namespace game_stream1 {

JOAN_COUNTER(c_streams3, "SoundStreams3::update");

// (int)this[0x1c] * 0.01f * (int)count / divisor at PC24 (fild, fmul m32, fimul m32, fdiv m32).
float volume(std::int32_t base, std::int32_t count, img::Addr divisor) {
    const joan::hook::x87::Fpu fpu{true};
    double v = fpu.mul(static_cast<double>(base), static_cast<double>(global<float>(d::flt_0_01)));  // 0.01
    v = fpu.mul(v, static_cast<double>(count));
    v = fpu.div(v, static_cast<double>(global<float>(divisor)));
    return static_cast<float>(v);
}

struct H {
    void streams3_update() {  // 0x005866d0 (on 0xa5cd70)
        if (jo::view<std::int32_t>(jo::self<jo::SoundStreams3>(this)->open) == 0) return;
        c_streams3.hit();
        for (std::int32_t i = 0; i < 3; ++i) {
            void* target;
            if (i == 0)
                target = global<void*>(d::g_a5cfa0);
            else if (i == 1)
                target = global<void*>(d::g_a5cfa4);
            else
                target = global<void*>(d::g_a5cfa8);
            const std::int32_t state = jo::self<jo::SoundStreams3>(this)->mode[i];
            if (state == 1) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_ambient_streams(this)->running(0, joan::orig::arg::i32(i))) != 0) {  // 0x005864f0
                    const std::int32_t r = thiscall<std::int32_t>(
                        fn::VideoFile_get_next, static_cast<void*>(jo::self<jo::SoundStreams3>(this)->videos[i]),
                        global<std::uint32_t>(d::g_game_time_ms) - jo::self<jo::SoundStreams3>(this)->start_time[i]);  // 0x00685a80
                    if (r == 4 || r == 3) {
                        joan::orig::as_video(jo::self<jo::SoundStreams3>(this)->videos[i])->seek(0u);  // 0x00686250
                        jo::self<jo::SoundStreams3>(this)->start_time[i] = global<std::uint32_t>(d::g_game_time_ms);
                    }
                }
            } else if (state == 2) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_ambient_streams(this)->running(0, joan::orig::arg::i32(i))) != 0) {  // 0x005864f0
                    if (target != nullptr)
                        joan::orig::as_sound3d(target)->set_gain(joan::orig::arg::bits(volume(jo::self<jo::SoundStreams3>(this)->volume, jo::view<std::int32_t>(jo::self<jo::SoundStreams3>(this)->fade_time[i]), d::flt_5000)));  // 0x0059f700, /5000
                    if (jo::self<jo::SoundStreams3>(this)->u32_40 != global<std::uint32_t>(d::g_game_time_ms)) {
                        std::uint32_t left;
                        if (jo::self<jo::SoundStreams3>(this)->fade_time[i] <= global<std::uint32_t>(d::g_frame_elapsed_ms))
                            left = 0;
                        else
                            left = jo::self<jo::SoundStreams3>(this)->fade_time[i] - global<std::uint32_t>(d::g_frame_elapsed_ms);
                        jo::self<jo::SoundStreams3>(this)->fade_time[i] = left;
                    }
                }
            } else if (state == 0) {
                joan::orig::as_ambient_streams(this)->running(0, joan::orig::arg::i32(i));  // 0x005864f0
            }
        }
        jo::self<jo::SoundStreams3>(this)->u32_40 = global<std::uint32_t>(d::g_game_time_ms);
    }
};

#define STREAM1(name, addr, fn) JOAN_REPLACE("game/stream1", name, addr, fn)
STREAM1("SoundStreams3::update", 0x005866d0, &H::streams3_update);
#undef STREAM1

}  // namespace game_stream1
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

struct H {
    // 0x005868f0
    void set_channel_mode(std::int32_t mode, std::int32_t idx) {
        if (mode != 1 && mode == 2 && jo::self<jo::SoundStreams3>(this)->mode[idx] != mode) {
            jo::self<jo::SoundStreams3>(this)->fade_time[idx] = 0x1388;
        }
        jo::self<jo::SoundStreams3>(this)->mode[idx] = mode;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Obj::set_channel_mode", 0x005868f0, &H::set_channel_mode);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;

using joan::hook::global;

using joan::hook::global;

using joan::call::thiscall;

struct H {
    // 0x00586620
    void set_volume(std::int32_t v) {
        if (jo::view<std::int32_t>(jo::self<jo::SoundStreams3>(this)->open) == 0) return;
        jo::self<jo::SoundStreams3>(this)->volume = v;
        const joan::hook::x87::Fpu fpu{true};
        const img::Addr slots[3] = {d::g_a5cfa0, d::g_a5cfa4, d::g_a5cfa8};
        for (int k = 0; k < 3; ++k) {
            if (global<void*>(slots[k]) == nullptr) continue;
            const float g = static_cast<float>(fpu.mul(static_cast<double>(v), global<float>(d::flt_0_01)));  // 0.01f
            joan::orig::as_sound3d(global<void*>(slots[k]))->set_gain(joan::orig::arg::bits(g));  // 0x0059f700
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Volume::set", 0x00586620, &H::set_volume);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::AmbientStreams::init() -> void { joan::bind::member<&::game_b99_06::H::init_585fd0>(this); }  // 0x585fd0
auto joan::orig::AmbientStreams::close() -> void { joan::bind::member<&::game_b95_05::H::close_586010>(this); }  // 0x586010
auto joan::orig::Video::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_05::H::scalar_dtor>(this, a0)); }  // 0x586150
auto joan::orig::Slots::release(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::slots_release>(this, a0)); }  // 0x5861a0
auto joan::orig::AmbientStreams::play(std::int32_t a0, std::int32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::play_music>(this, a0, a1, a2)); }  // 0x586210
auto joan::orig::AmbientStreams::running(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_stream2::H::running3>(this, a0, a1)); }  // 0x5864f0
auto joan::orig::Volume::set(std::int32_t a0) -> void { joan::bind::member<&::game_b91_02::H::set_volume>(this, a0); }  // 0x586620
auto joan::orig::AmbientStreams::update() -> void { joan::bind::member<&::game_stream1::H::streams3_update>(this); }  // 0x5866d0
auto joan::orig::AmbientStreams::set_channel_mode(std::int32_t a0, std::int32_t a1) -> void { joan::bind::member<&::game_b98_05::H::set_channel_mode>(this, a0, a1); }  // 0x5868f0
