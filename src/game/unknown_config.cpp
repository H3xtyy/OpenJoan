// game/unknown_config.cpp: functions whose original source file is unknown (formerly game/_unplaced/config.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/gen/records_s5.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::img::obj;
using joan::img::str;

using u16 = std::uint16_t;

using u32 = std::uint32_t;

using joan::hook::global;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using joan::call::thiscall;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using joan::orig::helpers::addr;

void* const kUtil = obj(d::g_misc);

JOAN_COUNTER(c_lookup_45a7e0, "Res::lookup_45a7e0");

struct H {
    // ---------------------------------------------------------------- 0x00459e30
    u32 reset_459e30() {
        joan::orig::as_config(this)->defaults();              // 0x00459ec0
        joan::orig::as_config(this)->clear_14_16();              // 0x00459f30
        joan::orig::as_config(this)->reset_player();              // 0x00459fb0
        joan::orig::as_config(this)->defaults_options();  // 0x0045a010
        return 0;  // callers discard the result; Options::defaults returns nothing
    }

    // ---------------------------------------------------------------- 0x0045a420
    std::uintptr_t assign_45a420(const u8* src) {
        jo::view<u32>(jo::self<jo::Config>(this)->kind_value) = jo::view<u32>(jo::as_config(src)->kind_value);
        jo::self<jo::Config>(this)->kind_byte = src[0x4];
        jo::self<jo::Config>(this)->u8_5 = src[0x5];
        jo::view<u16>(jo::self<jo::Config>(this)->u16_6) = jo::view<u16>(jo::as_config(src)->u16_6);
        jo::self<jo::Config>(this)->u8_8 = src[0x8];
        jo::self<jo::Config>(this)->u8_9 = src[0x9];
        jo::self<jo::Config>(this)->kind = src[0xa];
        jo::self<jo::Config>(this)->u32_b = jo::as_config(src)->u32_b;
        jo::self<jo::Config>(this)->u8_f = src[0xf];
        jo::view<u32>(jo::self<jo::Config>(this)->u32_10) = jo::view<u32>(jo::as_config(src)->u32_10);
        return addr(this);
    }

    // ---------------------------------------------------------------- 0x0045a7e0
    u32 lookup_45a7e0(const void* name, i32 len) {
        // len > 15 overruns the 16-byte stack buffer (buf[len] = 0): keep the original body.
        if (static_cast<u32>(len) > 15u) return thiscall<u32>(fn::Res_lookup_45a7e0_body, this, name, len);
        c_lookup_45a7e0.hit();
        if (jo::self<jo::Se>(this)->enabled == 0) return 0;  // `this` is the Se object g_a58b00
        char buf[16];
        std::memcpy(static_cast<void*>(buf), joan::orig::arg::ptr(name), joan::orig::arg::u32(len));  // memcpy
        buf[len] = 0;
        joan::orig::as_misc(kUtil)->rtrim_58f610(joan::orig::arg::str(static_cast<char*>(buf)));  // 0x0058f610
        u32 r = joan::orig::arg::as<u32>(joan::orig::as_res_idx_l(&jo::self<jo::Se>(this)->archive)->get_index(joan::orig::arg::str(static_cast<char*>(buf))));  // 0x0059be20
        if (r != 0) return r;
        r = joan::orig::arg::as<u32>(joan::orig::as_res_idx_l(jo::self<jo::Se>(this)->ext_archive)->get_index(joan::orig::arg::str(static_cast<char*>(buf))));
        if (r != 0) return r + static_cast<u32>(jo::self<jo::Se>(this)->count_main);
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Obj::reset_459e30", 0x00459e30, &H::reset_459e30);
B("Rec::assign_45a420", 0x0045a420, &H::assign_45a420);
B("Res::lookup_45a7e0", 0x0045a7e0, &H::lookup_45a7e0);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

using joan::call::thiscall;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using joan::orig::helpers::at;

void* const kFiles = obj(d::g_misc);

struct alignas(4) File {
    u8 bytes[0x210];  // 0x110 used (0x004179c0)
};

JOAN_COUNTER(c_load, "Obj::load_45aaf0");

struct H {
    // ------------------------------------------------------------------ tiny methods
    void nop() {}
    u32 const_64_45ac40() { return 0x64; }
    void* clear_f_45ad60() {
        jo::self<jo::Config>(this)->u8_f = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x0045aaf0
    i32 load_45aaf0(const char* name) {
        c_load.hit();
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0 ctor
        if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(name)) == 0 ||  // 0x00590b80
            joan::orig::as_file(&file)->file_open(joan::orig::arg::str(name), 1, static_cast<char>(0)) == 0) {  // 0x00586da0 open
            joan::orig::as_file(&file)->destruct();                      // 0x005872b0 dtor
            return 0;
        }
        i32 result = 0;
        if (joan::orig::as_file(&file)->file_size() == 0xb05)                        // 0x00587f30
            result = joan::orig::arg::as<i32>(joan::orig::as_config(this)->read(joan::orig::arg::mptr(static_cast<void*>(&file)), 0));  // 0x0045adc0
        joan::orig::as_file(&file)->file_close();  // 0x00587260 close
        jo::self<jo::Config>(this)->u8_14 = 0;
        jo::self<jo::Config>(this)->u8_15 = 0;
        jo::self<jo::Config>(this)->u8_16 = 0;
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 dtor
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::load_45aaf0", 0x0045aaf0, &H::load_45aaf0);
B("Obj::const_64_45ac40", 0x0045ac40, &H::const_64_45ac40);
B("Obj::clear_f_45ad60", 0x0045ad60, &H::clear_f_45ad60);
B("Obj::nop_459e70", 0x00459e70, &H::nop);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

using joan::call::thiscall;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using i16 = std::int16_t;

struct H {
    // ------------------------------------------------------------------ 0x00459e90
    u32 load_config() {
        return joan::orig::arg::as<u32>(joan::orig::as_config(obj(d::g_a57fb8))->save_file(str(d::s_CONFIG_DAT)));  // 0x0045a9f0 "CONFIG.DAT"
    }

    // ------------------------------------------------------------------ 0x00459ec0
    void* config_defaults() {
        joan::orig::as_config(this)->set_kind(1);  // 0x0045acb0
        jo::self<jo::Config>(this)->u8_5 = 1;
        jo::self<jo::Config>(this)->u16_6 = 0x2d;
        jo::self<jo::Config>(this)->u8_8 = 1;
        jo::self<jo::Config>(this)->u8_9 = 2;
        jo::self<jo::Config>(this)->u8_f = 0;
        jo::self<jo::Config>(this)->u32_10 = 0x14;
        jo::self<jo::Config>(this)->u8_b4 = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x0045a950
    void* set_4a_4e() {
        jo::self<jo::Config>(this)->u8_4a = 1;
        jo::self<jo::Config>(this)->u8_4b = 1;
        jo::self<jo::Config>(this)->u8_4c = 1;
        jo::self<jo::Config>(this)->u8_4d = 1;
        jo::self<jo::Config>(this)->u8_4e = 1;
        return this;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Obj::load_config", 0x00459e90, &H::load_config);
B100("Config::defaults", 0x00459ec0, &H::config_defaults);
B100("Obj::set_4a_4e", 0x0045a950, &H::set_4a_4e);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------ 0x00459f30
    void clear_14_16() {
        jo::self<jo::Config>(this)->u8_14 = 0;
        jo::self<jo::Config>(this)->u8_15 = 0;
        jo::self<jo::Config>(this)->u8_16 = 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::clear_14_16", 0x00459f30, &H::clear_14_16);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using i32 = std::int32_t;

using u8 = std::uint8_t;
using i16 = std::int16_t;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ clears
    void* clear_459f70() {
        jo::self<jo::Config>(this)->u8_14 = 0;
        jo::self<jo::Config>(this)->u8_15 = 0;
        jo::self<jo::Config>(this)->u8_16 = 0;
        return this;
    }
    void* clear_45a900() {
        jo::self<jo::Config>(this)->u8_45 = 0;
        jo::self<jo::Config>(this)->u8_46 = 0;
        jo::self<jo::Config>(this)->u8_47 = 0;
        jo::self<jo::Config>(this)->u8_48 = 0;
        jo::self<jo::Config>(this)->u8_49 = 0;
        jo::self<jo::Config>(this)->u32_53 = 0;
        return this;
    }
    void* clear_45a9a0() {
        jo::self<jo::Config>(this)->u8_4a = 0;
        jo::self<jo::Config>(this)->u8_4b = 0;
        jo::self<jo::Config>(this)->u8_4c = 0;
        jo::self<jo::Config>(this)->u8_4d = 0;
        jo::self<jo::Config>(this)->u8_4e = 0;
        return this;
    }
    // ------------------------------------------------------------------ 0x0045ad00
    u32 random_45ad00() {
        void* rng = obj(d::g_misc);
        const u32 r1 = joan::orig::arg::as<u32>(joan::orig::as_misc(rng)->random(8));  // 0x00590540
        jo::self<jo::Config>(this)->kind_value = *reinterpret_cast<const i16*>(static_cast<std::uintptr_t>(r1 * 2u + img::addr(d::g_9fb1cc)));
        const u32 r2 = joan::orig::arg::as<u32>(joan::orig::as_misc(rng)->random(8));  // 0x00590540
        const u8 b = *reinterpret_cast<const u8*>(static_cast<std::uintptr_t>(r2 + img::addr(d::g_9fb1dc)));
        jo::self<jo::Config>(this)->kind_byte = b;
        return (r2 & 0xffffff00u) | b;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::clear_459f70", 0x00459f70, &H::clear_459f70);
B("Obj::clear_45a900", 0x0045a900, &H::clear_45a900);
B("Obj::clear_45a9a0", 0x0045a9a0, &H::clear_45a9a0);
B("Obj::random_45ad00", 0x0045ad00, &H::random_45ad00);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

using i32 = std::int32_t;

using u8 = std::uint8_t;

void* const kRand = obj(d::g_misc);

JOAN_COUNTER(c_timers, "Obj::roll_timers");

using joan::orig::helpers::strcpy_;
i32 rand_n(i32 n) { return joan::orig::as_misc(kRand)->random(joan::orig::arg::i32(n)); }  // 0x00590540

struct H {
    // ------------------------------------------------------------------ 0x00459fb0
    void reset_player() {
        strcpy_(jo::self<jo::Config>(this)->player_name, str(d::s_Player));  // "Player"
        jo::self<jo::Config>(this)->u8_2c = 0;
        jo::self<jo::Config>(this)->u8_2d = 1;
        jo::self<jo::Config>(this)->u32_2e = 1;
        jo::self<jo::Config>(this)->u8_32 = 0;
    }

    // ------------------------------------------------------------------ 0x0045b160
    void roll_timer(float& chance, float& value, std::uint32_t& flag) {  // LightSys members
        const i32 r = rand_n(0x64);
        if (static_cast<double>(r) >= static_cast<double>(chance)) return;  // C0 clear
        const float span = global<float>(d::g_a47bc8) - global<float>(d::g_calendar_day_start_minute);
        const i32 r2 = rand_n(joan::hook::x87::ftol(static_cast<double>(span)));
        const Fpu fpu{true};
        value = static_cast<float>(fpu.add(static_cast<double>(r2), static_cast<double>(global<float>(d::g_calendar_day_start_minute))));
        flag = 0;
    }
    void roll_timers() {
        c_timers.hit();
        // the members of the typed layout (0x58c / 0x594 / 0x598 and 0x59c / 0x5a4 / 0x5a8 on x86)
        jo::LightSys* const ls = jo::self<jo::LightSys>(this);
        roll_timer(ls->event_a_chance, ls->event_a_time, ls->event_a_state);
        roll_timer(ls->event_b_chance, ls->event_b_time, ls->event_b_state);
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::reset_player", 0x00459fb0, &H::reset_player);
B96("Obj::roll_timers", 0x0045b160, &H::roll_timers);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
namespace {
namespace game_b101_01 {

using joan::call::thiscall;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using u64 = std::uint64_t;

JOAN_COUNTER(c_defaults, "Options::defaults_45a010");
JOAN_COUNTER(c_wave_load, "Wave::load_45a350");
JOAN_COUNTER(c_assign, "Options::assign_45a4d0");

struct H {
    // ------------------------------------------------------------------ 0x0045a010
    void defaults_45a010() {
        c_defaults.hit();
        using Div = u64(JOAN_STDCALL*)(u64, u64);  // _aulldiv
        const u64 speed64 = reinterpret_cast<Div>(static_cast<std::uintptr_t>(fn::code_value(fn::crt_aulldiv)))(
            static_cast<u64>(global<u32>(d::g_a485ec)) | (static_cast<u64>(global<u32>(d::g_a485f0)) << 32), 1000000u);
        const i32 speed = static_cast<i32>(static_cast<u32>(speed64));
        jo::self<jo::Config>(this)->u8_33 = 1;
        jo::self<jo::Config>(this)->u8_34 = 5;
        jo::self<jo::Config>(this)->u16_35 = 3;
        jo::self<jo::Config>(this)->tooltip_level = 2;
        jo::self<jo::Config>(this)->u8_38 = 0;
        jo::self<jo::Config>(this)->u8_39 = 1;
        jo::self<jo::Config>(this)->music_volume = 0x4d;
        jo::self<jo::Config>(this)->ambient_volume = 0x27;
        jo::self<jo::Config>(this)->u16_3e = 0x64;
        jo::self<jo::Config>(this)->sound_enabled = 1;
        jo::self<jo::Config>(this)->sfx_volume = 0x64;
        jo::self<jo::Config>(this)->sound_3d_enabled = 1;
        jo::self<jo::Config>(this)->u8_44 = 1;
        jo::self<jo::Config>(this)->u8_45 = 1;
        jo::self<jo::Config>(this)->u8_46 = 1;
        jo::self<jo::Config>(this)->u8_47 = 1;
        jo::self<jo::Config>(this)->u8_48 = 1;
        jo::self<jo::Config>(this)->u8_49 = 0;
        jo::self<jo::Config>(this)->u8_4a = 1;
        jo::self<jo::Config>(this)->u8_4b = 1;
        jo::self<jo::Config>(this)->u8_4c = 1;
        jo::self<jo::Config>(this)->u8_4d = 0;
        jo::self<jo::Config>(this)->u8_4e = 1;
        jo::self<jo::Config>(this)->u32_4f = 0x14;
        jo::self<jo::Config>(this)->u32_53 = 5;
        jo::self<jo::Config>(this)->u32_57 = 0x64;
        jo::self<jo::Config>(this)->u32_5b = 0x64;
        jo::self<jo::Config>(this)->u32_5f = 0x5a;
        jo::self<jo::Config>(this)->u32_63 = 0x64;
        jo::self<jo::Config>(this)->u32_67 = 0x46;
        jo::self<jo::Config>(this)->u32_7a = 0;
        joan::orig::as_config(this)->set_defaults_45();  // 0x0045a8b0
        joan::orig::as_config(this)->set_defaults_4a();  // 0x0045a950
        jo::self<jo::Config>(this)->u32_53 = 0;
        jo::self<jo::Config>(this)->control_scheme = 3;
        jo::self<jo::Config>(this)->u32_6f = 0;
        jo::self<jo::Config>(this)->invert_look = 0;
        jo::view<u32>(jo::self<jo::Config>(this)->fov) = 0x42b40000u;  // 90.0f
        jo::self<jo::Config>(this)->adapter_index = 0;
        jo::self<jo::Config>(this)->screen_width = 0x400;
        jo::self<jo::Config>(this)->screen_height = 0x300;
        jo::self<jo::Config>(this)->colour_depth = 0x20;
        jo::self<jo::Config>(this)->refresh_rate = 0x3c;
        jo::self<jo::Config>(this)->texture_quality = 1;
        jo::self<jo::Config>(this)->u8_93 = 2;
        jo::self<jo::Config>(this)->u8_94 = 3;
        jo::self<jo::Config>(this)->draw_distance = 0x2001;
        jo::self<jo::Config>(this)->u8_95 = 5;
        jo::self<jo::Config>(this)->trilinear_filtering = 1;
        jo::self<jo::Config>(this)->u8_97 = 1;
        if (speed <= 0x3e8) {
            jo::view<u8>(jo::self<jo::Config>(this)->detail) = 1;
        } else if (speed <= 0x5dc) {
            jo::view<u8>(jo::self<jo::Config>(this)->detail) = 2;
        } else {
            jo::view<u8>(jo::self<jo::Config>(this)->detail) = 3;
        }
        jo::view<u8>(jo::self<jo::Config>(this)->shadow_quality) = speed <= 0x3e8 ? 0 : 1;
        jo::view<u8>(jo::self<jo::Config>(this)->water_reflection) = 1;
        jo::self<jo::Config>(this)->grass_enabled = speed <= 0x2bc ? 0 : 1;
        jo::self<jo::Config>(this)->view_distance = 0x320;
        jo::self<jo::Config>(this)->grass_density_percent = 0x32;
        if (speed <= 0x2bc) {
            jo::self<jo::Config>(this)->view_distance = 0x1f4;
            jo::self<jo::Config>(this)->grass_density_percent = 0x1e;
        } else if (speed >= 0x7d0) {
            jo::self<jo::Config>(this)->view_distance = 0x640;
            jo::self<jo::Config>(this)->grass_density_percent = 0x50;
        } else {
            jo::view<i32>(jo::self<jo::Config>(this)->view_distance) = ((speed - 0x2bc) * 0xb / 0x514 + 5) * 0x64;
            jo::view<i32>(jo::self<jo::Config>(this)->grass_density_percent) = ((speed - 0x2bc) * 5 / 0x514 + 3) * 0xa;
        }
        jo::view<u32>(jo::self<jo::Config>(this)->brightness) = 0xbe4ccccdu;  // -0.2f
        jo::view<u32>(jo::self<jo::Config>(this)->contrast) = 0x3f8ccccdu;  // 1.1f
        jo::view<u32>(jo::self<jo::Config>(this)->gamma) = 0x3f800000u;  // 1.0f
    }

    // ------------------------------------------------------------------ 0x0045a350
    // `this` is the Se object (the "Wave" view of 0x0045a350 is Se: +0x50 loaded, +0x54 enabled, +0x64 audio,
    // +0x68 archive); typed as Se so the members move with Se's x64 layout.
    void wave_load_45a350() {
        c_wave_load.hit();
        joan::orig::as_sound_effects(this)->free();  // 0x0059e0c0
        jo::self<jo::Se>(this)->enabled = static_cast<i32>(jo::view<i8>(jo::self<jo::Se>(this)->audio->wav_init_flag));
        if (jo::self<jo::Se>(this)->enabled == 0) {
            jo::self<jo::Se>(this)->loaded = 1;
            return;
        }
        alignas(4) u8 path[0x194];  // [ebp-0x198] up to the frame's `this` slot
        joan::orig::as_string(static_cast<void*>(path))->construct();               // 0x005a1170
        joan::orig::as_string(static_cast<void*>(path))->assign(str(d::s_RESOURCE));               // 0x005a1440 "RESOURCE\\"
        joan::orig::as_string(static_cast<void*>(path))->append(str(d::s_A_WAVE1_RES));                   // 0x005a1550 "A_WAVE1.RES"
        const char* const name = joan::orig::as_string(static_cast<void*>(path))->c_str();  // 0x00417d90 (x64: a pointer, not a u32)
        joan::orig::as_res_idx_l(&jo::self<jo::Se>(this)->archive)->init(  // Se::archive (+0x68 on x86)
            name, static_cast<char>(1), static_cast<char>(0));  // 0x0059b920
        joan::orig::as_sound_effects(this)->build_tables();  // 0x00490d30
        jo::self<jo::Se>(this)->loaded = 1;
        joan::orig::as_sound_effects(this)->clear_recent();  // 0x0059e220
    }

    // ------------------------------------------------------------------ 0x0045a4d0
    void* assign_45a4d0(const u8* s) {
        c_assign.hit();
        jo::self<jo::Config>(this)->u8_33 = jo::as_config(s)->u8_33;
        jo::self<jo::Config>(this)->u8_34 = jo::as_config(s)->u8_34;
        jo::self<jo::Config>(this)->u16_35 = jo::as_config(s)->u16_35;
        jo::self<jo::Config>(this)->tooltip_level = jo::as_config(s)->tooltip_level;
        jo::self<jo::Config>(this)->u8_38 = jo::as_config(s)->u8_38;
        jo::self<jo::Config>(this)->u8_39 = jo::as_config(s)->u8_39;
        jo::self<jo::Config>(this)->music_volume = jo::as_config(s)->music_volume;
        jo::self<jo::Config>(this)->ambient_volume = jo::as_config(s)->ambient_volume;
        jo::self<jo::Config>(this)->u16_3e = jo::as_config(s)->u16_3e;
        jo::self<jo::Config>(this)->sound_enabled = jo::as_config(s)->sound_enabled;
        jo::self<jo::Config>(this)->sfx_volume = jo::as_config(s)->sfx_volume;
        jo::self<jo::Config>(this)->sound_3d_enabled = jo::as_config(s)->sound_3d_enabled;
        jo::self<jo::Config>(this)->u8_44 = jo::as_config(s)->u8_44;
        jo::self<jo::Config>(this)->u8_45 = jo::as_config(s)->u8_45;
        jo::self<jo::Config>(this)->u8_46 = jo::as_config(s)->u8_46;
        jo::self<jo::Config>(this)->u8_47 = jo::as_config(s)->u8_47;
        jo::self<jo::Config>(this)->u8_48 = jo::as_config(s)->u8_48;
        jo::self<jo::Config>(this)->u8_49 = jo::as_config(s)->u8_49;
        jo::self<jo::Config>(this)->u8_4a = jo::as_config(s)->u8_4a;
        jo::self<jo::Config>(this)->u8_4b = jo::as_config(s)->u8_4b;
        jo::self<jo::Config>(this)->u8_4c = jo::as_config(s)->u8_4c;
        jo::self<jo::Config>(this)->u8_4d = jo::as_config(s)->u8_4d;
        jo::self<jo::Config>(this)->u8_4e = jo::as_config(s)->u8_4e;
        jo::self<jo::Config>(this)->u32_4f = jo::as_config(s)->u32_4f;
        jo::self<jo::Config>(this)->u32_53 = jo::as_config(s)->u32_53;
        jo::self<jo::Config>(this)->u32_57 = jo::as_config(s)->u32_57;
        jo::self<jo::Config>(this)->u32_5b = jo::as_config(s)->u32_5b;
        jo::self<jo::Config>(this)->u32_5f = jo::as_config(s)->u32_5f;
        jo::self<jo::Config>(this)->u32_63 = jo::as_config(s)->u32_63;
        jo::self<jo::Config>(this)->u32_67 = jo::as_config(s)->u32_67;
        jo::self<jo::Config>(this)->control_scheme = jo::as_config(s)->control_scheme;
        jo::self<jo::Config>(this)->u32_6f = jo::as_config(s)->u32_6f;
        jo::self<jo::Config>(this)->invert_look = jo::as_config(s)->invert_look;
        jo::view<u32>(jo::self<jo::Config>(this)->fov) = jo::view<u32>(jo::as_config(s)->fov);
        jo::self<jo::Config>(this)->u32_7a = jo::as_config(s)->u32_7a;
        jo::self<jo::Config>(this)->adapter_index = jo::as_config(s)->adapter_index;
        jo::self<jo::Config>(this)->screen_width = jo::as_config(s)->screen_width;
        jo::self<jo::Config>(this)->screen_height = jo::as_config(s)->screen_height;
        jo::self<jo::Config>(this)->colour_depth = jo::as_config(s)->colour_depth;
        jo::self<jo::Config>(this)->refresh_rate = jo::as_config(s)->refresh_rate;
        jo::self<jo::Config>(this)->texture_quality = jo::as_config(s)->texture_quality;
        jo::self<jo::Config>(this)->u8_93 = jo::as_config(s)->u8_93;
        jo::self<jo::Config>(this)->u8_94 = jo::as_config(s)->u8_94;
        jo::self<jo::Config>(this)->draw_distance = jo::as_config(s)->draw_distance;
        jo::self<jo::Config>(this)->u8_95 = jo::as_config(s)->u8_95;
        jo::self<jo::Config>(this)->trilinear_filtering = jo::as_config(s)->trilinear_filtering;
        jo::self<jo::Config>(this)->u8_97 = jo::as_config(s)->u8_97;
        jo::view<u8>(jo::self<jo::Config>(this)->detail) = jo::view<u8>(jo::as_config(s)->detail);
        jo::view<u32>(jo::self<jo::Config>(this)->brightness) = jo::view<u32>(jo::as_config(s)->brightness);
        jo::view<u32>(jo::self<jo::Config>(this)->contrast) = jo::view<u32>(jo::as_config(s)->contrast);
        jo::view<u32>(jo::self<jo::Config>(this)->gamma) = jo::view<u32>(jo::as_config(s)->gamma);
        jo::self<jo::Config>(this)->u8_b4 = jo::as_config(s)->u8_b4;
        return std::memcpy(static_cast<void*>(jo::self<jo::Config>(this)->saved), static_cast<const void*>(jo::as_config(s)->saved), joan::orig::arg::u32(static_cast<u32>(0xa50)));  // memcpy
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Options::defaults_45a010", 0x0045a010, &H::defaults_45a010);
B("Wave::load_45a350", 0x0045a350, &H::wave_load_45a350);
B("Options::assign_45a4d0", 0x0045a4d0, &H::assign_45a4d0);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using joan::call::thiscall;

using u8 = std::uint8_t;

using Reader = jo::File;  // 0x004179c0, frame slot ebp-0x120 (0x110 bytes); a real File (x64: 0x112 bytes)

JOAN_COUNTER(c_load_file, "Obj::load_file");

struct H {
    // ------------------------------------------------------------------ 0x0045a8b0
    void set_defaults_45() {
        jo::self<jo::Config>(this)->u8_45 = 1;
        jo::self<jo::Config>(this)->u8_46 = 1;
        jo::self<jo::Config>(this)->u8_47 = 1;
        jo::self<jo::Config>(this)->u8_48 = 1;
        jo::self<jo::Config>(this)->u8_49 = 0;
        jo::self<jo::Config>(this)->u32_53 = 1;
    }

    // ------------------------------------------------------------------ 0x0045a9f0
    u32 load_file(const char* path) {
        c_load_file.hit();
        alignas(4) Reader rd;                        // ebp-0x120
        joan::orig::as_file(&rd)->construct();            // 0x004179c0
        if (joan::orig::as_file(&rd)->file_create(joan::orig::arg::str(path), 1, static_cast<char>(0)) == 0) {  // 0x00586f00
            joan::orig::as_file(&rd)->destruct();       // 0x005872b0
            return 0;
        }
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_config(this)->write(&rd));  // 0x0045ad80
        joan::orig::as_file(&rd)->file_close();                       // 0x00587260
        joan::orig::as_file(&rd)->destruct();                   // 0x005872b0
        return r;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::set_defaults_45", 0x0045a8b0, &H::set_defaults_45);
B99("Obj::load_file", 0x0045a9f0, &H::load_file);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b107_15.cpp (game/b107_15)
namespace {
namespace game_b107_15 {

using joan::call::thiscall;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using i16 = std::int16_t;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------------ 0x0045ac60
    i32 scaled_45ac60(i32 d) {
        const i16 w = static_cast<i16>(static_cast<u32>(jo::self<jo::Config>(this)->kind_value) + static_cast<u32>(d));
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_config(this)->const_64());  // 0x0045ac40
        const u32 k = (static_cast<u32>(static_cast<i32>(w)) - static_cast<u32>(jo::self<jo::Config>(this)->kind_value)) * 10u;
        return static_cast<i32>(static_cast<u32>(r) + k);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_15", name, addr, fn)
B("Range::scaled_45ac60", 0x0045ac60, &H::scaled_45ac60);
#undef B

}  // namespace game_b107_15
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using i32 = std::int32_t;

using u8 = std::uint8_t;
using i16 = std::int16_t;

struct H {
    // ------------------------------------------------------------------ 0x0045acb0
    i32 set_kind(i32 k) {
        jo::self<jo::Config>(this)->kind = static_cast<u8>(k);
        jo::self<jo::Config>(this)->kind_value = static_cast<i32>(global<i16>(d::g_9fb1ca + static_cast<u32>(k) * 2u));
        jo::self<jo::Config>(this)->kind_byte = global<u8>(d::g_9fb1db + static_cast<u32>(k));
        return k;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::set_kind", 0x0045acb0, &H::set_kind);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using i32 = std::int32_t;

// CONFIG.DAT = one 0xb05-byte record (File without vary-size framing): Config +0..+0xb5, then the key snapshot
// KeyBinding[55] at +0xb5 (X::save_snapshot 0x4cc380 / RemapKeys::load 0x4cc220; re/save/records.json s5
// ConfigHead / KeyBinding). The C++ Config ends at +0x16a for now, the snapshot is addressed past it.
constexpr u32 kConfigSize = 0xb05, kKeysOff = 0xb5, kKeys = 55;
static_assert(kKeysOff + kKeys * 0x30 == kConfigSize, "CONFIG.DAT layout");

struct H {
    // ------------------------------------------------------------------ 0x0045ad80
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S5): field by field in one chunk; 0 = the raw write.
    i32 write_b05(void* file) {
        if (!joan::save::fieldwise()) {
            joan::orig::as_file(file)->file_write(static_cast<void*>(this), kConfigSize);  // 0x005872e0
            return 1;
        }
        const auto* const keys = reinterpret_cast<const jo::KeyBinding*>(reinterpret_cast<const std::uint8_t*>(this) + kKeysOff);
        joan::save::Out o(joan::save::port(*jo::as_file(file)), kConfigSize, "Config", this);
        joan::save::put_ConfigHead(o, *jo::self<jo::Config>(this));
        for (u32 i = 0; i < kKeys; ++i) joan::save::put_KeyBinding(o, keys[i]);
        o.end();  // File::file_write 0x005872e0 (result ignored, as in the original)
        return 1;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::write_b05", 0x0045ad80, &H::write_b05);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
namespace {
namespace game_b104_02 {

using i32 = std::int32_t;

using u8 = std::uint8_t;

using joan::orig::helpers::at;
u8* bp(const void* p) { return static_cast<u8*>(const_cast<void*>(p)); }

struct alignas(4) Snapshot {
    u8 b[0xa50];
};

using joan::orig::helpers::memcpy_;

JOAN_COUNTER(c_save_restore, "Obj::save_restore_45adc0");

struct H {
    // ------------------------------------------------------------------ 0x0045adc0
    u32 save_restore_45adc0(void* file, i32 restore) {
        c_save_restore.hit();
        const u8 b39 = jo::self<jo::Config>(this)->u8_39;
        const u16 w3a = jo::self<jo::Config>(this)->music_volume, w3c = jo::self<jo::Config>(this)->ambient_volume, w3e = jo::self<jo::Config>(this)->u16_3e;
        const u8 b40 = jo::self<jo::Config>(this)->sound_enabled;
        const u16 w41 = jo::self<jo::Config>(this)->sfx_volume;
        const u8 b44 = jo::self<jo::Config>(this)->u8_44, b37 = jo::self<jo::Config>(this)->tooltip_level;
        const u32 d7a = jo::self<jo::Config>(this)->u32_7a, d6b = jo::self<jo::Config>(this)->control_scheme, d6f = jo::self<jo::Config>(this)->u32_6f;
        const u8 b73 = jo::self<jo::Config>(this)->invert_look;
        const u32 d76 = jo::view<u32>(jo::self<jo::Config>(this)->fov), d7e = jo::self<jo::Config>(this)->adapter_index, d82 = jo::self<jo::Config>(this)->screen_width, d86 = jo::self<jo::Config>(this)->screen_height,
                  d8a = jo::self<jo::Config>(this)->colour_depth, d8e = jo::self<jo::Config>(this)->refresh_rate;
        const u8 b92 = jo::self<jo::Config>(this)->texture_quality, b93 = jo::self<jo::Config>(this)->u8_93, b94 = jo::self<jo::Config>(this)->u8_94;
        const u32 d98 = jo::self<jo::Config>(this)->draw_distance;
        const u8 b95 = jo::self<jo::Config>(this)->u8_95, b96 = jo::self<jo::Config>(this)->trilinear_filtering, b97 = jo::self<jo::Config>(this)->u8_97, b9c = jo::view<u8>(jo::self<jo::Config>(this)->detail), b9d = jo::view<u8>(jo::self<jo::Config>(this)->shadow_quality),
                 b9e = jo::view<u8>(jo::self<jo::Config>(this)->water_reflection), b9f = jo::self<jo::Config>(this)->grass_enabled;
        const u32 da0 = jo::self<jo::Config>(this)->view_distance, da4 = jo::self<jo::Config>(this)->grass_density_percent, da8 = jo::view<u32>(jo::self<jo::Config>(this)->brightness), dac = jo::view<u32>(jo::self<jo::Config>(this)->contrast), db0 = jo::view<u32>(jo::self<jo::Config>(this)->gamma);
        const u8 bb4 = jo::self<jo::Config>(this)->u8_b4;
        Snapshot snap;  // [ebp-0xae0]
        memcpy_(&snap, bp(this) + offsetof(jo::Config, saved), 0xa50);
        u32 r;  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S5): field by field from one chunk; 0 = raw read
        if (joan::save::fieldwise()) {
            auto* const keys = reinterpret_cast<jo::KeyBinding*>(bp(this) + offsetof(jo::Config, saved));
            joan::save::In in(joan::save::port(*jo::as_file(file)), 0xb05u, "Config", this);
            joan::save::get_ConfigHead(in, *jo::self<jo::Config>(this));
            for (u32 i = 0; i < 55; ++i) joan::save::get_KeyBinding(in, keys[i]);
            r = static_cast<u32>(in.end());  // File::file_read 0x005873a0
        } else {
            r = joan::orig::arg::as<u32>(joan::orig::as_file(file)->file_read(static_cast<void*>(this), 0xb05u));  // 0x005873a0
        }
        if (restore != 0) {
            jo::self<jo::Config>(this)->u8_39 = b39;
            jo::self<jo::Config>(this)->music_volume = w3a;
            jo::self<jo::Config>(this)->ambient_volume = w3c;
            jo::self<jo::Config>(this)->u16_3e = w3e;
            jo::self<jo::Config>(this)->sound_enabled = b40;
            jo::self<jo::Config>(this)->sfx_volume = w41;
            jo::self<jo::Config>(this)->u8_44 = b44;
            jo::self<jo::Config>(this)->tooltip_level = b37;
            jo::self<jo::Config>(this)->u32_7a = d7a;
            jo::self<jo::Config>(this)->control_scheme = d6b;
            jo::self<jo::Config>(this)->u32_6f = d6f;
            jo::self<jo::Config>(this)->invert_look = b73;
            jo::view<u32>(jo::self<jo::Config>(this)->fov) = d76;
            jo::self<jo::Config>(this)->adapter_index = d7e;
            jo::self<jo::Config>(this)->screen_width = d82;
            jo::self<jo::Config>(this)->screen_height = d86;
            jo::self<jo::Config>(this)->colour_depth = d8a;
            jo::self<jo::Config>(this)->refresh_rate = d8e;
            jo::self<jo::Config>(this)->texture_quality = b92;
            jo::self<jo::Config>(this)->u8_93 = b93;
            jo::self<jo::Config>(this)->u8_94 = b94;
            jo::self<jo::Config>(this)->draw_distance = d98;
            jo::self<jo::Config>(this)->u8_95 = b95;
            jo::self<jo::Config>(this)->trilinear_filtering = b96;
            jo::self<jo::Config>(this)->u8_97 = b97;
            jo::view<u8>(jo::self<jo::Config>(this)->detail) = b9c;
            jo::view<u8>(jo::self<jo::Config>(this)->shadow_quality) = b9d;
            jo::view<u8>(jo::self<jo::Config>(this)->water_reflection) = b9e;
            jo::self<jo::Config>(this)->grass_enabled = b9f;
            jo::self<jo::Config>(this)->view_distance = da0;
            jo::self<jo::Config>(this)->grass_density_percent = da4;
            jo::view<u32>(jo::self<jo::Config>(this)->brightness) = da8;
            jo::view<u32>(jo::self<jo::Config>(this)->contrast) = dac;
            jo::view<u32>(jo::self<jo::Config>(this)->gamma) = db0;
            jo::self<jo::Config>(this)->u8_b4 = bb4;
            memcpy_(bp(this) + offsetof(jo::Config, saved), &snap, 0xa50);
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Obj::save_restore_45adc0", 0x0045adc0, &H::save_restore_45adc0);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b111_19.cpp (game/b111_19)
namespace {
namespace game_b111_19 {

using joan::call::cdecl_call;

using i32 = std::int32_t;

using u8 = std::uint8_t;

using joan::call::stdcall_call;

template <class R = long, class... A>
R com_call(void* self, u32 offset, A... args) {  // COM method (stdcall, this first) at x86 vtable offset (slot offset / 4)
    const std::uintptr_t fn = (*static_cast<const std::uintptr_t* const*>(self))[joan::call::x86_vslot(offset)];
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(fn)(self, args...);
}
struct alignas(4) Str {  // 0x005a11a0 string object: 0x190 chars + NUL (no destructor)
    char b[0x194];
};

JOAN_COUNTER(c_caps, "D3DSetup::check_caps_45b240");

// ---- fix/uninit: 0x00433af0 ------------------------------------------------------------------
struct H {
    // ---- 0x0045b240 --------------------------------------------------------------------------
    void check_caps() {
        c_caps.hit();
        if ((jo::self<jo::Config>(this)->detail == 3 || jo::self<jo::Config>(this)->detail == 2) && global<i32>(d::g_stencil_bits) < 4) jo::self<jo::Config>(this)->detail = 1;
        struct {
            u32 adapter, device_type;
            void* focus_window;  // HWND: 8 bytes on x64
            u32 behavior;
        } params = {};  // [ebp-0x14] D3DDEVICE_CREATION_PARAMETERS
        com_call(global<void*>(d::g_game_device), 0x24, static_cast<void*>(&params));  // GetCreationParameters
        u8 ident[0x42c];  // [ebp-0x440] D3DADAPTER_IDENTIFIER8 (Description at +0x200)
        std::memset(ident, 0, sizeof ident);
        com_call(global<void*>(d::g_a52424), 0x14, params.adapter, u32{0}, static_cast<void*>(ident));  // GetAdapterIdentifier
        Str s;
        joan::orig::as_string(&s)->construct(joan::orig::arg::str(static_cast<const char*>(reinterpret_cast<char*>(ident + 0x200))));  // 0x005a11a0
        i32 gf3 = 0;  // [ebp-0x5d8]
        const char* const lower = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->lower());  // 0x005a1340 (copy + _strlwr)
        if (joan::orig::arg::as<const char*>(joan::orig::libc::strstr(joan::orig::arg::str(lower), str(d::s_geforce3))) != nullptr) gf3 = 1;  // strstr "geforce3"
        if (jo::self<jo::Config>(this)->shadow_quality == 2) {
            if (global<i32>(d::g_a52733) == 0 && global<i32>(d::g_a5253c) == 0) {
                jo::self<jo::Config>(this)->shadow_quality = 1;
            } else {
                void* tex = nullptr;   // [ebp-0x5e4] (read only on success)
                void* surf = nullptr;  // [ebp-0x5e8]
                const long r1 = joan::orig::arg::as<long>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(u32{0x40}), joan::orig::arg::u32(u32{0x40}), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(global<u32>(d::g_a52444)), joan::orig::arg::u32(u32{0}), &tex));  // D3DXCreateTexture
                void* const dev = global<void*>(d::g_game_device);
                const long r2 = com_call(dev, 0x68, u32{0x40}, u32{0x40}, global<u32>(d::g_a52727), u32{0},
                                         &surf);  // CreateDepthStencilSurface
                if (r1 == 0) com_call<u32>(tex, 8);   // Release
                if (r2 == 0) com_call<u32>(surf, 8);  // Release
                if (r1 != 0 || r2 != 0) jo::self<jo::Config>(this)->shadow_quality = 1;
            }
        }
        if (jo::self<jo::Config>(this)->shadow_quality == 1 && global<i32>(d::g_a52633) == 0) jo::self<jo::Config>(this)->shadow_quality = 0;
        if (jo::self<jo::Config>(this)->water_reflection != 0) {
            if ((global<u32>(d::g_a52500) & 0x200000u) != 0) {
                void* tex = nullptr;  // [ebp-0x5f0]
                const long r = joan::orig::arg::as<long>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(u32{0x100}), joan::orig::arg::u32(u32{0x100}), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{0x3c}), joan::orig::arg::u32(u32{1}), &tex));
                if (r == 0) {
                    com_call<u32>(tex, 8);  // Release
                    if (global<u32>(d::g_a52504) < 2) jo::self<jo::Config>(this)->water_reflection = 0;
                } else {
                    jo::self<jo::Config>(this)->water_reflection = 0;
                }
            } else {
                jo::self<jo::Config>(this)->water_reflection = 0;
            }
            if (jo::self<jo::Config>(this)->shadow_quality == 2 && gf3 != 0) jo::self<jo::Config>(this)->water_reflection = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_19", name, addr, fn)
B("D3DSetup::check_caps_45b240", 0x0045b240, &H::check_caps);
#undef B

}  // namespace game_b111_19
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}
using joan::orig::helpers::device;

JOAN_COUNTER(c_cursor, "Mouse::update_cursor");

struct H {
    // ---- 0x0045b4c0 Mouse::update_cursor(unused) ---------------------------------------------
    // eax: the ShowCursor (vtable +0x30) result.
    std::uint32_t update_cursor(void*) {
        c_cursor.hit();
        const std::int32_t x = jo::self<jo::MouseCursor>(this)->x1 + jo::self<jo::MouseCursor>(this)->hot_spot_x;
        const std::int32_t y = jo::self<jo::MouseCursor>(this)->y1 + jo::self<jo::MouseCursor>(this)->hot_spot_y;
        com_call<void>(device(), 0x2c, x, y, static_cast<std::uint32_t>(1));  // SetCursorPosition
        return static_cast<std::uint32_t>(
            com_call<std::int32_t>(device(), 0x30, static_cast<std::int32_t>(1)));  // ShowCursor
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Mouse::update_cursor", 0x0045b4c0, &H::update_cursor);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Config::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_01::H::reset_459e30>(this)); }  // 0x459e30
auto joan::orig::Config::init_nop() -> void { joan::bind::member<&::game_b105_02::H::nop>(this); }  // 0x459e70
auto joan::orig::Config::defaults() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_02::H::config_defaults>(this)); }  // 0x459ec0
auto joan::orig::Config::clear_14_16() -> void { joan::bind::member<&::game_b95_01::H::clear_14_16>(this); }  // 0x459f30
auto joan::orig::Config::reset_player() -> void { joan::bind::member<&::game_b96_01::H::reset_player>(this); }  // 0x459fb0
auto joan::orig::Config::defaults_options() -> void { joan::bind::member<&::game_b101_01::H::defaults_45a010>(this); }  // 0x45a010
auto joan::orig::SoundEffects::load() -> void { joan::bind::member<&::game_b101_01::H::wave_load_45a350>(this); }  // 0x45a350
auto joan::orig::Config::set_defaults_45() -> void { joan::bind::member<&::game_b99_01::H::set_defaults_45>(this); }  // 0x45a8b0
auto joan::orig::Config::set_defaults_4a() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_02::H::set_4a_4e>(this)); }  // 0x45a950
auto joan::orig::Config::save_file(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_01::H::load_file>(this, a0)); }  // 0x45a9f0
auto joan::orig::Config::load(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b105_02::H::load_45aaf0>(this, a0)); }  // 0x45aaf0
auto joan::orig::Config::const_64() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_02::H::const_64_45ac40>(this)); }  // 0x45ac40
auto joan::orig::Config::set_kind(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_01::H::set_kind>(this, a0)); }  // 0x45acb0
auto joan::orig::Config::write(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_01::H::write_b05>(this, a0)); }  // 0x45ad80
auto joan::orig::Config::read(void* a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_02::H::save_restore_45adc0>(this, a0, a1)); }  // 0x45adc0
auto joan::orig::LightSys::roll_timers() -> void { joan::bind::member<&::game_b96_01::H::roll_timers>(this); }  // 0x45b160
auto joan::orig::Config::check_caps() -> void { joan::bind::member<&::game_b111_19::H::check_caps>(this); }  // 0x45b240
auto joan::orig::MouseCursor::update_cursor(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_05::H::update_cursor>(this, a0)); }  // 0x45b4c0
