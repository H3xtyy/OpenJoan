// game/unknown_music.cpp: functions whose original source file is unknown (formerly game/_unplaced/music.cpp;
// re/attribution.csv: between ww/ilib/mouse.cpp and ww/ilib/resdb.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../platform/win32_types.hpp"
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>
#include <cstdint>
#include <limits>
#include <cstdarg>
#include <vector>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../registry.hpp"
#include "../original/misc.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../ilib/dinput_formats.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::hook::global;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

JOAN_COUNTER(c_close_59ad70, "Obj::close_59ad70");

struct H {
    void* clear_24() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::SoundStream));  // SoundStream ctor, 0x24 on x86
        return this;
    }

    void delete_slot(jo::Video*& slot) {
        if (slot == nullptr) return;
        void* const o = slot;
        if (o != nullptr) joan::orig::as_video(o)->destroy(1u);  // 0x00586150
        slot = nullptr;
    }

    // 0x0059ad70 (eax = this on every path)
    void* close_59ad70() {
        c_close_59ad70.hit();
        if (jo::self<jo::SoundStream>(this)->open == 0) return this;
        joan::orig::as_slot(this)->release();  // 0x0059ae00
        delete_slot(jo::self<jo::SoundStream>(this)->video);
        jo::self<jo::SoundStream>(this)->open = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Obj::clear_24", 0x0059acd0, &H::clear_24);
B("Obj::close_59ad70", 0x0059ad70, &H::close_59ad70);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

struct H {
    void* call_59ad70() { return joan::orig::as_sound_stream(this)->close(); }                                     // 0x0059ad00
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Obj::call_59ad70", 0x0059ad00, &H::call_59ad70);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

struct H {
    // 0x0059ad30 (eax = the stored value)
    std::int32_t init_59ad30() {
        joan::orig::as_sound_stream(this)->close();  // 0x0059ad70
        const std::int32_t v = global<std::int16_t>(d::g_music_volume);
        jo::self<jo::SoundStream>(this)->volume = v;
        jo::self<jo::SoundStream>(this)->open = 1;
        return v;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("Obj::init_59ad30", 0x0059ad30, &H::init_59ad30);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x0059ae00
    std::int32_t slot_release() {
        if (jo::self<jo::SoundStream>(this)->open == 0) return -1;
        if (jo::view<void*>(jo::self<jo::SoundStream>(this)->video) != nullptr) joan::orig::as_video(jo::view<void*>(jo::self<jo::SoundStream>(this)->video))->close();  // 0x00686160
        jo::view<std::uint32_t>(jo::self<jo::SoundStream>(this)->track) = 0;
        return 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Slot::release", 0x0059ae00, &H::slot_release);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_music_play, "Music::play");

// ---------------------------------------------------------------------------- CRT (direct)
using joan::orig::helpers::strcpy_;
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
using joan::orig::helpers::new_;  // operator new

struct H {
    // ------------------------------------------------------------------ 0x0059ae60
    u32 music_play(i32 index, u32 b) {
        if (jo::self<jo::SoundStream>(this)->open == 0) return 0;
        if (index < 1 || index > global<i32>(d::g_music_res)) return 0;
        if (global<jo::Sound3D*>(d::g_a5cf9c) == nullptr) return 0;
        c_music_play.hit();
        joan::orig::as_slot(this)->release();  // 0x0059ae00
        if (jo::self<jo::SoundStream>(this)->video == nullptr) {
            void* const p = new_(static_cast<std::uint32_t>(sizeof(jo::Video)));  // 0x30 on x86
            jo::view<void*>(jo::self<jo::SoundStream>(this)->video) = p != nullptr ? joan::orig::as_video(p)->construct() : nullptr;  // 0x006845d0
        }
        const u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_music_res(obj(d::g_music_res))->track(joan::orig::arg::i32(index)));  // 0x004d8be0
        char path[0x104];  // [ebp-0x118]
        strcpy_(path, str(d::s_MUSIC_9d6248));  // "MUSIC\\"
        strcat_(path, jo::as_music_track(rec)->file);
        joan::orig::as_video(jo::view<void*>(jo::self<jo::SoundStream>(this)->video))->open_684650(joan::orig::arg::str(static_cast<const char*>(path)), global<jo::Sound3D*>(d::g_a5cf9c));  // 0x00684650
        if (jo::self<jo::SoundStream>(this)->video->sound != nullptr) {
            joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::SoundStream>(this)->video->sound))->set_position(img::obj(d::g_pos));  // 0x0059fb40
            joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::SoundStream>(this)->video->sound))->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(1)));            // 0x0059fae0
        }
        jo::self<jo::SoundStream>(this)->track = index;
        jo::view<u32>(jo::self<jo::SoundStream>(this)->mode) = b;
        jo::self<jo::SoundStream>(this)->start_time = global<u32>(d::g_game_time_ms);
        jo::self<jo::SoundStream>(this)->fade_ms = 0x1f40;
        joan::orig::as_sound(this)->set_volume_59b150(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::SoundStream>(this)->volume)));  // 0x0059b150
        return joan::orig::arg::as<u32>(joan::orig::as_video(jo::view<void*>(jo::self<jo::SoundStream>(this)->video))->has_audio_stream());  // 0x00685990
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Music::play", 0x0059ae60, &H::music_play);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_stream2.cpp (game/stream2)
namespace {
namespace game_stream2 {

JOAN_COUNTER(c_running, "SoundStream::running");

struct H {
    std::int32_t running(std::int32_t tag) {  // 0x0059b070 (on 0xa5d0d0)
        if (jo::view<std::int32_t>(jo::self<jo::SoundStream>(this)->open) == 0) return 0;
        c_running.hit();
        if (jo::self<jo::SoundStream>(this)->track == 0) return 0;
        if (jo::self<jo::SoundStream>(this)->video == nullptr) return 0;
        if (tag != 0 && tag != jo::self<jo::SoundStream>(this)->track) return 0;
        if (jo::self<jo::SoundStream>(this)->mode == 1) return 1;
        const std::int32_t r = thiscall<std::int32_t>(
            fn::VideoFile_get_next, jo::view<void*>(jo::self<jo::SoundStream>(this)->video), global<std::uint32_t>(d::g_game_time_ms) - jo::self<jo::SoundStream>(this)->start_time);  // 0x00685a80
        if (r == 4 || r == 3) return 0;
        if (jo::self<jo::SoundStream>(this)->mode == 2 && jo::view<std::int32_t>(jo::self<jo::SoundStream>(this)->fade_ms) == 0) return 0;
        return 1;
    }
};

#define STREAM2(name, addr, fn) JOAN_REPLACE("game/stream2", name, addr, fn)
STREAM2("SoundStream::running", 0x0059b070, &H::running);
#undef STREAM2

}  // namespace game_stream2
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (game/b86_04)
namespace {
namespace en3d_b86_04 {

JOAN_COUNTER(c_set_volume, "Sound::set_volume");

struct H {
    // 0x0059b150
    void set_volume(std::int32_t v) {
        c_set_volume.hit();
        if (jo::view<std::int32_t>(jo::self<jo::SoundStream>(this)->open) == 0) return;
        jo::self<jo::SoundStream>(this)->volume = v;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound_stream(this)->running(0)) != 0 && global<void*>(d::g_a5cf9c) != nullptr) {  // 0x0059b070
            const joan::hook::x87::Fpu fpu{true};
            const float scaled = static_cast<float>(fpu.mul(static_cast<double>(v), static_cast<double>(global<float>(d::flt_0_01))));
            joan::orig::as_sound3d(global<void*>(d::g_a5cf9c))->set_gain(joan::orig::arg::bits(scaled));  // 0x0059f700
        }
    }
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b86_04", name, addr, fn)
GAME("Sound::set_volume", 0x0059b150, &H::set_volume);
#undef GAME

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/game_stream1.cpp (game/stream1)
namespace {
namespace game_stream1 {

JOAN_COUNTER(c_stream, "SoundStream::update");

// (int)this[0x1c] * 0.01f * (int)count / divisor at PC24 (fild, fmul m32, fimul m32, fdiv m32).
float volume(std::int32_t base, std::int32_t count, img::Addr divisor) {
    const joan::hook::x87::Fpu fpu{true};
    double v = fpu.mul(static_cast<double>(base), static_cast<double>(global<float>(d::flt_0_01)));  // 0.01
    v = fpu.mul(v, static_cast<double>(count));
    v = fpu.div(v, static_cast<double>(global<float>(divisor)));
    return static_cast<float>(v);
}

struct H {
    void stream_update() {  // 0x0059b1c0 (on 0xa5d0d0)
        if (jo::view<std::int32_t>(jo::self<jo::SoundStream>(this)->open) == 0) return;
        c_stream.hit();
        if (jo::self<jo::SoundStream>(this)->mode == 1) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound_stream(this)->running(0)) != 0) {  // 0x0059b070
                const std::int32_t r = thiscall<std::int32_t>(
                    fn::VideoFile_get_next, jo::view<void*>(jo::self<jo::SoundStream>(this)->video), global<std::uint32_t>(d::g_game_time_ms) - jo::self<jo::SoundStream>(this)->start_time);  // 0x00685a80
                if (r == 4 || r == 3) {
                    joan::orig::as_video(jo::view<void*>(jo::self<jo::SoundStream>(this)->video))->seek(0u);  // 0x00686250
                    jo::self<jo::SoundStream>(this)->start_time = global<std::uint32_t>(d::g_game_time_ms);
                }
            }
        } else if (jo::self<jo::SoundStream>(this)->mode == 2) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound_stream(this)->running(0)) != 0) {  // 0x0059b070
                if (global<void*>(d::g_a5cf9c) != nullptr)
                    joan::orig::as_sound3d(global<void*>(d::g_a5cf9c))->set_gain(joan::orig::arg::bits(volume(jo::self<jo::SoundStream>(this)->volume, jo::view<std::int32_t>(jo::self<jo::SoundStream>(this)->fade_ms), d::flt_8000)));  // 0x0059f700, /8000
                if (jo::self<jo::SoundStream>(this)->last_tick != global<std::uint32_t>(d::g_game_time_ms)) {
                    std::uint32_t left;
                    if (jo::self<jo::SoundStream>(this)->fade_ms <= global<std::uint32_t>(d::g_frame_elapsed_ms))
                        left = 0;
                    else
                        left = jo::self<jo::SoundStream>(this)->fade_ms - global<std::uint32_t>(d::g_frame_elapsed_ms);
                    jo::self<jo::SoundStream>(this)->fade_ms = left;
                }
            }
        }
        jo::self<jo::SoundStream>(this)->last_tick = global<std::uint32_t>(d::g_game_time_ms);
    }
};

#define STREAM1(name, addr, fn) JOAN_REPLACE("game/stream1", name, addr, fn)
STREAM1("SoundStream::update", 0x0059b1c0, &H::stream_update);
#undef STREAM1

}  // namespace game_stream1
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
namespace {
namespace game_b94_05 {

struct H {
    // 0x0059b330
    void set_mode(std::int32_t mode) {
        if (mode != 1 && mode == 2 && jo::self<jo::SoundStream>(this)->mode != mode) jo::self<jo::SoundStream>(this)->fade_ms = 0x1f40;
        jo::self<jo::SoundStream>(this)->mode = mode;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("Obj::set_mode", 0x0059b330, &H::set_mode);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SoundStream::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_05::H::clear_24>(this)); }  // 0x59acd0
auto joan::orig::SoundStream::init() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_05::H::init_59ad30>(this)); }  // 0x59ad30
auto joan::orig::SoundStream::close() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_05::H::close_59ad70>(this)); }  // 0x59ad70
auto joan::orig::Slot::release() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::slot_release>(this)); }  // 0x59ae00
auto joan::orig::SoundStream::play(std::int32_t a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::music_play>(this, a0, a1)); }  // 0x59ae60
auto joan::orig::SoundStream::running(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_stream2::H::running>(this, a0)); }  // 0x59b070
auto joan::orig::Sound::set_volume_59b150(std::int32_t a0) -> void { joan::bind::member<&::en3d_b86_04::H::set_volume>(this, a0); }  // 0x59b150
auto joan::orig::SoundStream::update() -> void { joan::bind::member<&::game_stream1::H::stream_update>(this); }  // 0x59b1c0
auto joan::orig::SoundStream::set_mode(std::int32_t a0) -> void { joan::bind::member<&::game_b94_05::H::set_mode>(this, a0); }  // 0x59b330
