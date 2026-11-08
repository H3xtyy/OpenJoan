// game/lightsys.cpp: functions of the original d:\ww\lightsys.cpp (re/attribution.csv).
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
#include "../original/records.hpp"
#include <cmath>
#include <cstddef>
#include <cstdarg>
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

struct Obj {};
using joan::call::thiscall;

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kLightSysCpp(d::s_d_ww_lightsys_cpp);  // "d:\ww\lightsys.cpp"

constexpr std::int32_t kDayRec = static_cast<std::int32_t>(sizeof(jo::LightDay));  // x86: 10 bytes
// Day table entry k (byte offset k * sizeof, as the original's address arithmetic).
inline jo::LightDay* day(std::uint8_t* days, std::int32_t k) { return jo::as_light_day(days + k * kDayRec); }
// Light definition i of the current preset (LightPreset::lights[9]).
inline jo::LightDef* light_def(void* sys, std::int32_t i) {
    return jo::as_light_def(reinterpret_cast<std::uint8_t*>(jo::as_light_sys(sys)->current.lights) + i * static_cast<std::int32_t>(sizeof(jo::LightDef)));
}

}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
//
//      0x004a5970 LightSys::clear()
namespace {
namespace game_units5 {

JOAN_COUNTER(c_light_clear, "LightSys::clear");

struct H {

    // 0x004a5970 (ret 0)
    void light_clear() {
        c_light_clear.hit();
        if (jo::view<void*>(jo::self<jo::LightSys>(this)->sets) != nullptr) {
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->sets)), joan::orig::arg::str(img::str(kLightSysCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a01184)) + 3));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::LightSys>(this)->sets) = nullptr;
        }
        jo::self<jo::LightSys>(this)->set_count = 0;
        if (jo::view<void*>(jo::self<jo::LightSys>(this)->specials) != nullptr) {
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->specials)), joan::orig::arg::str(img::str(kLightSysCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a01184)) + 0xa));
            jo::view<void*>(jo::self<jo::LightSys>(this)->specials) = nullptr;
        }
        jo::self<jo::LightSys>(this)->special_count = 0;
        if (jo::view<void*>(jo::self<jo::LightSys>(this)->days) != nullptr) {
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->days)), joan::orig::arg::str(img::str(kLightSysCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a01184)) + 0x11));
            jo::view<void*>(jo::self<jo::LightSys>(this)->days) = nullptr;
        }
        jo::self<jo::LightSys>(this)->day_count = 0;
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->light_count); ++i) jo::self<jo::LightSys>(this)->lights[i] = nullptr;
        jo::self<jo::LightSys>(this)->light_count = 0;
        joan::orig::as_lens_flares(&jo::self<jo::LightSys>(this)->flares)->clear();                       // 0x006626a0
        std::memset(joan::orig::arg::mptr(&jo::self<jo::LightSys>(this)->state_a), 0, sizeof(jo::self<jo::LightSys>(this)->state_a));  // memset
        std::memset(joan::orig::arg::mptr(&jo::self<jo::LightSys>(this)->state_b), 0, sizeof(jo::self<jo::LightSys>(this)->state_b));  // memset
        joan::orig::as_sound3d(img::obj(d::g_weather_system))->release(static_cast<char>(joan::orig::arg::i32(0)));  // 0x00689d00
        if (jo::view<void*>(jo::self<jo::LightSys>(this)->moon) != nullptr) {
            joan::orig::as_scene(jo::as_world(global<std::uint8_t*>(d::g_world))->scene)->remove_sorted(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->moon)));  // 0x005eded0
            void* const o = jo::view<void*>(jo::self<jo::LightSys>(this)->moon);
            if (o != nullptr) {
                using Virtual = void* (Obj::*)(std::int32_t);
                JOAN_MEMBER_POINTER_CHECK(Virtual);
                Virtual method{};
                std::memcpy(&method, &joan::call::vtable_of(o)[0], sizeof(void*));
                (reinterpret_cast<Obj*>(o)->*method)(1);
            }
            jo::view<void*>(jo::self<jo::LightSys>(this)->moon) = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::LightSys>(this)->event_a_time) = 0;
        jo::self<jo::LightSys>(this)->event_a_state = 0;
        jo::view<std::uint32_t>(jo::self<jo::LightSys>(this)->event_b_time) = 0;
        jo::self<jo::LightSys>(this)->event_b_state = 0;
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("LightSys::clear", 0x004a5970, &H::light_clear);
#undef U5

}  // namespace game_units5
}  // namespace

// ---------------------------------------- from modules/game_light1.cpp (game/light1)
//
//  * module game/light1:
//      LightSys::load_lights 0x004a5bf0 (ret 4): MAP\<name>.lgt and MAP\<name>.sca. The .lgt
//        gives the light count per set (+0x5b0) and the set count (+0x0); the sets (0x18c
//        bytes each, +0x2) get per light (0x1a bytes from +0x42) an enabled byte (+0x43), and
//        when enabled the set name (strncpy 0x40 into the set), a byte (+0x42), four ints as
//        floats (+0x44..+0x50) and two angles in degrees * pi/180 (+0x58, +0x54); then the
//        set's +0x12c block is initialised (0x00617ee0) and the .sca supplies 14 floats
//        (+0x178..+0x188, +0x168..+0x170, +0x174 = 100, +0x15c..+0x164 normalized).
//      LightSys::load_special 0x004a6840 (ret 0): same for MAP\special.lgt / .sca into the
//        +0x6 / +0x8 sets; the light count is a local and the enabled byte is not tested.
//      LightSys::load_day 0x004a7450 (ret 4): MAP\<name>.day: +0x4c4 count, the two flicker
//        chances / durations (+0x58c, +0x590 / 100, +0x59c, +0x5a0 / 100), three names
//        (+0x54c, +0x4cc, +0x50c) and the 10-byte day table (+0x4c8: id from 0x004a9120,
//        two floats).
//      LensFlares::init 0x00662640 (ret 0x14), LensFlares::clear 0x006626a0,
//      LensFlares::add 0x00662710 (ret 0x10; 0x19b-byte entries, realloc'd array).
//      TerrainBlock::normal 0x006645b0 (ret 0xc; height-map rotation of the up vector, or the
//        normal of the plane of the block's three corner heights; returns 1).
//      P2::sub 0x006639e0 (cdecl, explicit out pointer), PfdData::PfdData 0x00664fd0
//      (memset 0x38), PfdData::~ 0x00665000 (helper calls + member deletes),
//      P2::length_sq 0x0066a440, P2::copy_to 0x0066a4f0 (ret 4).
namespace {
namespace game_light1 {

using joan::hook::x87::Fpu;

using joan::call::cdecl_call;
using joan::call::import_call;

constexpr img::Addr kMap(d::s_MAP_9cf818);          // "MAP\"
constexpr img::Addr kLgt(d::s_lgt);          // ".lgt"
constexpr img::Addr kSca(d::s_sca);          // ".sca"
constexpr img::Addr kDay(d::s_day);          // ".day"
constexpr img::Addr kSpecial(d::s_special);      // "special"
constexpr double kDegToRad = 0.017453292519943295;  // qword 0x009d37a8
constexpr double kHundred = 100.0;                  // qword 0x009d03c0

JOAN_COUNTER(c_load_lights, "LightSys::load_lights");
JOAN_COUNTER(c_load_special, "LightSys::load_special");
JOAN_COUNTER(c_load_day, "LightSys::load_day");

// Path string object (no destructor in the original) and the text file reader.
void str_init(void* s) { joan::orig::as_string(s)->construct(); }                     // 0x005a1170
void str_assign(void* s, const void* text) { joan::orig::as_string(s)->assign(joan::orig::arg::str(text)); }  // 0x005a1440
void str_append(void* s, const void* text) { joan::orig::as_string(s)->append(joan::orig::arg::str(text)); }  // 0x005a1550
const char* str_c(void* s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str()); }  // 0x00417d90
// The frame slot of a text reader (FileTxt): 0x1a0 on x86; FileTxt holds pointers (0x1a6 bytes on x64).
constexpr std::size_t kReader = sizeof(jo::FileTxt) > 0x1a0 ? sizeof(jo::FileTxt) : 0x1a0;
void reader_open(void* r, const char* path) { joan::orig::as_file_txt(r)->construct(joan::orig::arg::str(path)); }  // 0x00589550
void reader_close(void* r) { joan::orig::as_file_txt(r)->destruct(); }                               // 0x00589890
const char* token(void* r) { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(r)->get_token(1)); }        // 0x00589c70
void read_line(void* r, char* buf, std::int32_t n) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); }  // 0x00589f10
std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
// "key value" pairs: the key token is skipped, the second token converted.
std::int32_t next_int(void* r) {
    token(r);
    return atoi_(token(r));
}
double next_float(void* r) {
    token(r);
    return atof_(token(r));
}

// Path = "MAP\" + name + ext.
void make_path(void* path, const char* name, const char* ext) {
    str_assign(path, img::str(kMap));
    str_append(path, name);
    str_append(path, ext);
}

// Per-light fields of a set (the enabled byte already stored).
void read_light(void* lgt, jo::LightPreset* set, jo::LightDef* light, char* buf) {
    const Fpu fpu{true};
    token(lgt);
    read_line(lgt, buf, 0x40);
#if defined(__GNUC__) && !defined(__clang__)
// strncpy(name, buf, 0x40) into the 64-byte name field without terminator, as the original does
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
    std::strncpy(static_cast<char*>(joan::orig::arg::mptr(set->name)), joan::orig::arg::str(static_cast<const char*>(buf)), 0x40u);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    light->mode = static_cast<std::uint8_t>(next_int(lgt));
    light->colour[0] = static_cast<float>(next_int(lgt));
    light->colour[1] = static_cast<float>(next_int(lgt));
    light->colour[2] = static_cast<float>(next_int(lgt));
    light->colour[3] = static_cast<float>(next_int(lgt));
    light->angle_a = static_cast<float>(fpu.mul(next_float(lgt), kDegToRad));
    light->angle_b = static_cast<float>(fpu.mul(next_float(lgt), kDegToRad));
}

// The .sca part of a set.
void read_scale(void* sca, jo::LightPreset* set, char* buf) {
    joan::orig::as_light_params(&set->params)->construct();  // 0x00617ee0
    token(sca);
    read_line(sca, buf, 0x40);
    set->params.scale_a[0] = static_cast<float>(next_float(sca));
    set->params.scale_a[1] = static_cast<float>(next_float(sca));
    set->params.scale_a[2] = static_cast<float>(next_float(sca));
    set->params.scale_a[3] = static_cast<float>(next_float(sca));
    set->params.scale_a[4] = static_cast<float>(next_float(sca));
    set->params.scale_b[0] = static_cast<float>(next_float(sca));
    set->params.scale_b[1] = static_cast<float>(next_float(sca));
    set->params.scale_b[2] = static_cast<float>(next_float(sca));
    jo::view<std::uint32_t>(set->params.f32_48) = 0x42c80000u;  // 100.0f
    set->params.sun_dir[0] = static_cast<float>(next_float(sca));
    set->params.sun_dir[1] = static_cast<float>(next_float(sca));
    set->params.sun_dir[2] = static_cast<float>(next_float(sca));
    import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(set->params.sun_dir));  // vector_normalize (in place)
}

struct H {

    // 0x004a5bf0 LightSys::load_lights(name) (ret 4).
    void load_lights(const char* name) {
        c_load_lights.hit();
        alignas(8) std::uint8_t path[0x1a0];  // ebp-0x1a4
        alignas(8) std::uint8_t lgt[kReader];   // ebp-0x384
        alignas(8) std::uint8_t sca[kReader];   // ebp-0x520
        alignas(8) char buf[0x48];            // ebp-0x1e8 (0x40 + the terminator spill)
        str_init(path);
        make_path(path, name, img::str(kLgt));
        reader_open(lgt, str_c(path));
        make_path(path, name, img::str(kSca));
        reader_open(sca, str_c(path));
        jo::self<jo::LightSys>(this)->light_count = static_cast<std::int16_t>(next_int(lgt));
        jo::self<jo::LightSys>(this)->set_count = static_cast<std::int16_t>(next_int(lgt));
        jo::view<void*>(jo::self<jo::LightSys>(this)->sets) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->set_count) * static_cast<std::int32_t>(sizeof(jo::LightPreset))), joan::orig::arg::str(img::str(kLightSysCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a01188)) + 0x1a));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->sets)), 0, joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->set_count) * static_cast<std::int32_t>(sizeof(jo::LightPreset))));  // memset
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->set_count); ++i) {
            jo::self<jo::LightSys>(this)->sets[i].light_count = jo::self<jo::LightSys>(this)->light_count;
            for (std::int32_t j = 0; j < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->light_count); ++j) {
                const std::int32_t on = next_int(lgt);
                jo::LightPreset* const set = jo::self<jo::LightSys>(this)->sets + i;
                set->lights[j].enabled = static_cast<std::uint8_t>(on);
                if (jo::view<std::int8_t>(set->lights[j].enabled) == 0) continue;
                read_light(lgt, set, &set->lights[j], buf);
            }
            read_scale(sca, jo::self<jo::LightSys>(this)->sets + i, buf);
        }
        reader_close(sca);
        reader_close(lgt);
    }

    // 0x004a6840 LightSys::load_special() (ret 0).
    void load_special() {
        c_load_special.hit();
        alignas(8) std::uint8_t path[0x1a0];  // ebp-0x1a4
        alignas(8) std::uint8_t lgt[kReader];   // ebp-0x384
        alignas(8) std::uint8_t sca[kReader];   // ebp-0x520
        alignas(8) char buf[0x48];            // ebp-0x1e8
        str_init(path);
        make_path(path, img::str(kSpecial), img::str(kLgt));
        reader_open(lgt, str_c(path));
        make_path(path, img::str(kSpecial), img::str(kSca));
        reader_open(sca, str_c(path));
        const std::int32_t count = next_int(lgt);  // ebp-0x524
        jo::self<jo::LightSys>(this)->special_count = static_cast<std::int16_t>(next_int(lgt));
        jo::view<void*>(jo::self<jo::LightSys>(this)->specials) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->special_count) * static_cast<std::int32_t>(sizeof(jo::LightPreset))), joan::orig::arg::str(img::str(kLightSysCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a0118c)) + 0x1b));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->specials)), 0, joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->special_count) * static_cast<std::int32_t>(sizeof(jo::LightPreset))));  // memset
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->special_count); ++i) {
            jo::self<jo::LightSys>(this)->specials[i].light_count = static_cast<std::int16_t>(count);
            for (std::int32_t j = 0; j < count; ++j) {
                const std::int32_t on = next_int(lgt);
                jo::LightPreset* const set = jo::self<jo::LightSys>(this)->specials + i;
                set->lights[j].enabled = static_cast<std::uint8_t>(on);
                read_light(lgt, set, &set->lights[j], buf);
            }
            read_scale(sca, jo::self<jo::LightSys>(this)->specials + i, buf);
        }
        reader_close(sca);
        reader_close(lgt);
    }

    // 0x004a7450 LightSys::load_day(name) (ret 4).
    void load_day(const char* name) {
        c_load_day.hit();
        const Fpu fpu{true};
        alignas(8) std::uint8_t path[0x1a0];  // ebp-0x1a4
        alignas(8) std::uint8_t day[kReader];   // ebp-0x384
        alignas(8) char text[0x100];          // ebp-0x484
        alignas(8) char buf[0x48];            // ebp-0x1e8
        str_init(path);
        make_path(path, name, img::str(kDay));
        reader_open(day, str_c(path));
        jo::self<jo::LightSys>(this)->day_count = static_cast<std::int16_t>(next_int(day));
        jo::self<jo::LightSys>(this)->event_a_chance = static_cast<float>(next_float(day));
        jo::self<jo::LightSys>(this)->event_a_rate = static_cast<float>(fpu.div(next_float(day), kHundred));
        jo::self<jo::LightSys>(this)->event_b_chance = static_cast<float>(next_float(day));
        jo::self<jo::LightSys>(this)->event_b_rate = static_cast<float>(fpu.div(next_float(day), kHundred));
        const std::int32_t names[3] = {static_cast<std::int32_t>(offsetof(jo::LightSys, sky_name_a)), static_cast<std::int32_t>(offsetof(jo::LightSys, sky_name_b)),
                                       static_cast<std::int32_t>(offsetof(jo::LightSys, sky_name_c))};
        for (std::int32_t k = 0; k < 3; ++k) {
            token(day);
            read_line(day, text, 0x40);
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(this) + names[k])), joan::orig::arg::str(static_cast<const char*>(text)));  // strcpy
        }
        jo::view<void*>(jo::self<jo::LightSys>(this)->days) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->day_count) * kDayRec), joan::orig::arg::str(img::str(kLightSysCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a01190)) + 0x2d));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->days)), 0, joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->day_count) * kDayRec));  // memset
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->day_count); ++i) {
            token(day);
            read_line(day, buf, 0x40);
            const std::uint32_t id = joan::orig::arg::as<std::uint32_t>(joan::orig::as_light_sys(this)->find_set(joan::orig::arg::str(static_cast<const char*>(buf))));  // 0x004a9120
            (jo::as_light_day(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->days) + i * kDayRec))->preset = static_cast<std::int16_t>(id);
            (jo::as_light_day(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->days) + i * kDayRec))->time_from = static_cast<float>(next_float(day));
            (jo::as_light_day(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->days) + i * kDayRec))->time_to = static_cast<float>(next_float(day));
        }
        reader_close(day);
    }
};

#define L1(name, addr, fn) JOAN_REPLACE("game/light1", name, addr, fn)
L1("LightSys::load_lights", 0x004a5bf0, &H::load_lights);
L1("LightSys::load_special", 0x004a6840, &H::load_special);
L1("LightSys::load_day", 0x004a7450, &H::load_day);
#undef L1

}  // namespace game_light1
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
//
//      0x004a71f0 Light::trigger(src)           (thiscall, ret 4)
namespace {
namespace game_b98_02 {

using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::u;

struct V3u {  // 12-byte float vector, by value (XMM registers under System V, as the callee declares it)
    float x, y, z;
};
static_assert(sizeof(V3u) == 12, "vector size");

JOAN_COUNTER(c_light_trigger, "Light::trigger");

struct H {
    // ------------------------------------------------------------------ 0x004a71f0
    std::uintptr_t light_trigger(void* src) {  // eax: the Gate pointer, a flag or 0x004eef00's result
        c_light_trigger.hit();
        void* const objects = obj(d::g_items);
        std::uintptr_t r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_units(objects)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Order>(this)->current.target)));  // 0x00425d50
        if (r == 0) {
            void* const o = joan::orig::as_units(objects)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Order>(this)->current.target));  // 0x00425d20
            const bool flag = jo::self<jo::Order>(this)->current.radius != 0;  // read before the virtual call, as the original
            using Virtual = const jo::Gate* (Obj::*)();  // slot 0x4c returns the Gate
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(o)[0x4c / 4], sizeof(void*));
            const jo::Gate* const p = (reinterpret_cast<Obj*>(o)->*method)();
            if (flag) {
                if (static_cast<i32>(p->state) != 0) {  // Gate+0xd1
                    r = u(p);
                } else {
                    r = fire(src);
                }
            } else {
                const i32 k = static_cast<i32>(p->state);
                r = static_cast<u32>(k);
                if (k == 1) r = fire(src);
            }
        }
        jo::self<jo::Order>(this)->current.kind = 0;
        return r;
    }
    static u32 fire(void* src) {
        V3u v;
        joan::orig::as_unit(src)->position(&v);  // 0x004fedb0 (position at +0xe)
        const u32 now = global<u32>(d::g_now);
        // 0x004eef00 reads the 5th arg as a word.
        return joan::orig::arg::as<u32>(joan::orig::as_triggers(obj(d::g_a5ccd0))->add(0x20u, 2u, joan::orig::arg::i16(1), 1u, joan::orig::arg::i16(static_cast<i32>(jo::as_unit(src)->id)), static_cast<std::int8_t>(joan::orig::arg::i32(1)), v, 0x3f800000u, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::u32(now), joan::orig::arg::u32(now), static_cast<std::int8_t>(joan::orig::arg::i32(1)), 0u, joan::orig::arg::i16(0)));  // 0x004eef00
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Light::trigger", 0x004a71f0, &H::light_trigger);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x004a4fb0 ObjK::dtor                   (thiscall, ret 0)    - SEH, normal path only.
//
//  - SEH frames in 0x004a4fb0 / 0x004b6450 only guard member dtors. Normal path only.
namespace {
namespace game_b96_02 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {

    // ------------------------------------------------------------------------ SEH dtors
    u32 dtor_k() {  // 0x004a4fb0
        joan::orig::as_light_sys(this)->clear();                       // 0x004a5970
        joan::orig::as_lens_flares(&jo::self<jo::LightSys>(this)->flares)->destruct();       // 0x00662620
        return 0;  // atexit stub: result ignored; LensFlares dtor returns nothing
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("ObjK::dtor", 0x004a4fb0, &H::dtor_k);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_units5.cpp (game/units5)
//
//      0x004a5020 LightSys::init(arg, scene) - lights, random flicker times, lens flares
namespace {
namespace game_units5_2 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

using joan::call::thiscall;

constexpr img::Addr kRandom(d::g_misc);

JOAN_COUNTER(c_light_init, "LightSys::init");

std::uint32_t colour(std::int32_t r, std::int32_t g, std::int32_t b, std::int32_t a) {
    std::uint32_t tmp = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}

struct H {
    // 0x004a5020 (ret 8)
    void light_init(const char* arg, void* scene) {
        c_light_init.hit();
        joan::orig::as_light_sys(this)->clear();                                                    // 0x004a5970
        joan::orig::as_light_sys(this)->load_lights(joan::orig::arg::str(&img::global<const char>(d::s_common)));        // 0x004a5bf0 "common"
        joan::orig::as_light_sys(this)->load_special();                                                    // 0x004a6840
        joan::orig::as_light_sys(this)->load_day(arg);                                               // 0x004a7450
        std::int32_t count = 0;
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->day_count); ++i) {
            if (global<float>(d::g_calendar_minute_of_day) >= (jo::as_light_day(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->days) + i * kDayRec))->time_from) count = i + 1;
        }
        jo::self<jo::LightSys>(this)->day_index = count != 0 ? static_cast<std::int16_t>(count) : static_cast<std::int16_t>(1);
        joan::orig::as_units(this)->load_record(static_cast<std::int32_t>(jo::as_light_day(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->days) + (static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->day_index) - 1) * kDayRec)->preset));  // 0x0042e160
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->current.light_count); ++i) {
            std::uint8_t* const light = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(scene)->add_light());  // 0x005eb5d0
            jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->lights[i]) = light;
            jo::LightDef* const def = light_def(this, i);
            const std::uint8_t mode = def->mode;
            if (mode == 0) {
                joan::orig::as_camera(jo::self<jo::LightSys>(this)->lights[i])->init(0u);  // 0x005c58a0
            } else if (mode == 1) {
                joan::orig::as_camera(jo::self<jo::LightSys>(this)->lights[i])->init(1u);  // 0x005c58a0
                // The original's "first" flag is set right before it is tested: always taken.
                jo::view<std::int32_t>(jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->casts_shadows) = 1;
                joan::orig::as_scene(scene)->set_camera(joan::orig::arg::mptr(jo::self<jo::LightSys>(this)->lights[i]));  // 0x005ed9d0
            }
            jo::view<std::uint32_t>(jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->orbit_a) = jo::view<std::uint32_t>(def->angle_a);
            jo::view<std::uint32_t>(jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->orbit_b) = jo::view<std::uint32_t>(def->angle_b);
            const std::int32_t a = ftol(def->colour[3]);
            const std::int32_t b = ftol(def->colour[2]);
            const std::int32_t g = ftol(def->colour[1]);
            const std::int32_t r = ftol(def->colour[0]);
            jo::view<std::uint32_t>(jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->colour) = colour(r, g, b, a);
            jo::as_light(jo::self<jo::LightSys>(this)->lights[i])->active_ =
                static_cast<std::int32_t>(jo::view<std::int8_t>(def->enabled)) != 0 ? 1 : 0;
            joan::orig::as_light(jo::self<jo::LightSys>(this)->lights[i])->invalidate();  // 0x005c5c00
        }
        const Fpu fpu{true};
        for (std::int32_t k = 0; k < 2; ++k) {
            const float& chance = k == 0 ? jo::self<jo::LightSys>(this)->event_a_chance : jo::self<jo::LightSys>(this)->event_b_chance;
            float& time = k == 0 ? jo::self<jo::LightSys>(this)->event_a_time : jo::self<jo::LightSys>(this)->event_b_time;
            std::uint32_t& state = k == 0 ? jo::self<jo::LightSys>(this)->event_a_state : jo::self<jo::LightSys>(this)->event_b_state;
            const std::int32_t r = joan::orig::as_misc(img::obj(kRandom))->random(100);  // 0x00590540
            if (!(static_cast<double>(r) >= static_cast<double>(chance))) {
                const std::int32_t range = ftol(fpu.sub(global<float>(d::g_a47bc8), global<float>(d::g_calendar_day_start_minute)));
                const std::int32_t r2 = joan::orig::as_misc(img::obj(kRandom))->random(joan::orig::arg::i32(range));  // 0x00590540
                time = static_cast<float>(fpu.add(static_cast<double>(r2), global<float>(d::g_calendar_day_start_minute)));
                jo::view<std::int32_t>(state) = 0;
            }
        }
        const std::int32_t hh = global<std::int32_t>(d::g_screen_h) / 2;
        const std::int32_t hw = global<std::int32_t>(d::g_screen_w) / 2;
        void* const flares = &jo::self<jo::LightSys>(this)->flares;
        joan::orig::as_lens_flares(flares)->init(joan::orig::arg::i16(100), 0u, 0x3f800000u, joan::orig::arg::i16(hw), joan::orig::arg::i16(hh));  // 0x00662640
        joan::orig::as_lens_flares(flares)->add(joan::orig::arg::u32(colour(0xaa, 0xaa, 0xaa, 0x55)), joan::orig::arg::i16(0xdc), 0x3f800000u, joan::orig::arg::str(&img::global<const char>(d::s_sun01a)));  // 0x00662710 "sun01a"
        joan::orig::as_lens_flares(flares)->add(joan::orig::arg::u32(colour(0xaa, 0xaa, 0xaa, 0x55)), joan::orig::arg::i16(0xc6), 0x3f1eb852u, joan::orig::arg::str(&img::global<const char>(d::s_len03a)));  // "len03a"
        joan::orig::as_lens_flares(flares)->add(joan::orig::arg::u32(colour(0xaa, 0xaa, 0xaa, 0x55)), joan::orig::arg::i16(0x160), 0x3ebd70a4u, joan::orig::arg::str(&img::global<const char>(d::s_len02a)));  // "len02a"
        joan::orig::as_lens_flares(flares)->add(joan::orig::arg::u32(colour(0xaa, 0xaa, 0xaa, 0x55)), joan::orig::arg::i16(0x1ce), 0x3de147aeu, joan::orig::arg::str(&img::global<const char>(d::s_len03a)));  // "len03a"
        joan::orig::as_lens_flares(flares)->add(joan::orig::arg::u32(colour(0xaa, 0xaa, 0xaa, 0x55)), joan::orig::arg::i16(0x14a), 0xbe4ccccdu, joan::orig::arg::str(&img::global<const char>(d::s_len01a)));  // "len01a"
        jo::self<jo::LightSys>(this)->time = global<std::uint32_t>(d::g_calendar_minute_of_day);
    }
};

#define U5(name, addr, fn) JOAN_REPLACE("game/units5", name, addr, fn)
U5("LightSys::init", 0x004a5020, &H::light_init);
#undef U5

}  // namespace game_units5_2
}  // namespace

// ---------------------------------------- from modules/game_b82_13.cpp (game/b82_13)
//
//      0x004a9120 LightSys::find_set(name) (ret 4): 1-based index of the 0x18c-byte set
//                 (+0x2, count (short)+0x0) whose name _stricmp-matches, else 0.
namespace {
namespace game_b82_13 {

using joan::call::cdecl_call;

JOAN_COUNTER(c_find_set, "LightSys::find_set");

struct H {
    // 0x004a9120 (ret 4)
    std::int32_t find_set(const char* name) {
        c_find_set.hit();
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::LightSys>(this)->set_count); ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->sets) + i * 0x18c), joan::orig::arg::str(name)) == 0) return i + 1;  // _stricmp
        }
        return 0;
    }
};

#define B13(name, addr, fn) JOAN_REPLACE("game/b82_13", name, addr, fn)
B13("LightSys::find_set", 0x004a9120, &H::find_set);
#undef B13

}  // namespace game_b82_13
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - LightSys::lit_by 0x004a65d0 (id), ret 4.
//
//  - 0x004a65d0: when 0x484870 reports the +0x26 unit missing, the record pointer stays null
//    and is dereferenced later, exactly as in the original (volatile reads, so no UB-based
//    optimisation can remove the 0x484870 branch).
namespace {
namespace game_b100_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::img::obj;

void* const kUnits = obj(d::g_nations);
void* const kUnitDefs = obj(d::g_units);

JOAN_COUNTER(c_lit_by, "LightSys::lit_by");

struct H {
    // ------------------------------------------------------------------ 0x004a65d0
    i32 lit_by(u32 id) {
        c_lit_by.hit();
        if (joan::orig::as_dynarray(kUnitDefs)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitDefs)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        if (jo::self<jo::Region>(this)->world == nullptr) return 0;
        i32 hits = 0;
        u8* unit = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kUnits)->is_null(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Region>(this)->nation)))) == 0)  // 0x00484870
            unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Region>(this)->nation))));  // 0x0047b600
        i32 count = 0;
        u8** const arr = joan::orig::arg::as<u8**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Region>(this)->world))->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Region>(this)->box), &count));  // 0x00429090
        for (i32 i = 0; i < count; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(arr[i])->get_8c());  // 0x0054ee10
            u8* cur = first;
            if (cur == nullptr) continue;
            do {
                if (jo::as_unit(cur)->hit_points > 0 || jo::view<u32>(jo::as_unit(cur)->state_120) != 0) {
                    const u8 inside = joan::orig::arg::as<u8>(joan::orig::as_aabb(jo::self<jo::Region>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(cur)->position_)));  // 0x0060d5f0
                    if (inside != 0 && jo::as_unit(cur)->side != 0 &&
                        joan::orig::arg::as<i32>(joan::orig::as_players(kUnits)->hostile(static_cast<i32>(jo::as_unit(def)->side), static_cast<i32>(jo::as_unit(cur)->side))) != 0) {  // 0x004b6860
                        ++hits;
                        return 1;
                    }
                }
                cur = jo::view<u8*>(jo::as_unit(cur)->cell_prev);
            } while (cur != nullptr && cur != first);
        }
        u8* lst = joan::orig::arg::as<u8*>(joan::orig::as_place(this)->facility_head());  // 0x004c48b0
        // unit may be null here (original faults too); volatile so the compiler cannot treat
        // the null path as unreachable and drop the 0x484870 test.
        const i16 unit_w0 = *reinterpret_cast<volatile i16*>(unit);
        if (unit_w0 == 0) return hits;
        if (joan::orig::arg::as<i32>(joan::orig::as_players(kUnits)->hostile(static_cast<i32>(*reinterpret_cast<volatile i16*>(unit)), static_cast<i32>(jo::as_unit(def)->side))) == 0)
            return hits;
        while (lst != nullptr) {
            if (static_cast<i32>(jo::as_facility(lst)->kind) == 1) {
                if (jo::as_building(virtual_call<u8*>(lst, 0x18))->unit_kind != 0) ++hits;  // slot 0x18: the facility's building
                for (i32 k = 0; k < static_cast<i32>(jo::as_building(virtual_call<u8*>(lst, 0x18))->queue_n); ++k) {
                    hits += jo::as_building(virtual_call<u8*>(lst, 0x18))->queue_count[k];
                    if (hits != 0) return 1;
                }
            }
            lst = jo::view<u8*>(jo::as_facility(lst)->prev);
        }
        return hits;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("LightSys::lit_by", 0x004a65d0, &H::lit_by);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_overlays.cpp (game/sky1)
namespace {
namespace game_overlays {

using joan::hook::global;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::obj;


bool below_or_nan(float a, float b) { return !(a >= b); }

using joan::orig::helpers::operator_new;
std::uint32_t colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
}

JOAN_COUNTER(c_sky, "Sky::moon");

struct H {
    // Sky (0xa47e08): +0x168 light direction, +0x170 its height (< 0: sun up), +0x5ac moon
    // billboard, +0x5d6 lens flare. The moon sits 1500 units along the direction from the
    // camera.
    void moon_create_at(const float p[3]) {
        void* const mem = operator_new(sizeof(jo::Billboard));  // 0x37 on x86
        jo::view<void*>(jo::self<jo::LightSys>(this)->moon) = mem != nullptr ? joan::orig::as_billboard(mem)->construct() : nullptr;  // 0x006351a0
        const std::uint32_t white = colour(0xff, 0xff, 0xff, 0xff);
        std::uint32_t q[3];
        std::memcpy(q, p, sizeof q);
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::LightSys>(this)->moon))->create(joan::orig::arg::u32(q[0]), joan::orig::arg::u32(q[1]), joan::orig::arg::u32(q[2]), 0x42c80000u, 0x42c80000u, joan::orig::arg::str(&img::global<const char>(d::s_moon01)), joan::orig::arg::i16(// "moon01"
                 static_cast<std::int32_t>(0)), joan::orig::arg::u32(white), 0u, 0u, 0u, 0u, 4u, 0x20002u, 1u);  // 0x00635260
    }
    void moon_move(const float p[3]) {
        std::uint32_t q[3];
        std::memcpy(q, p, sizeof q);
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::LightSys>(this)->moon))->set_position(joan::orig::arg::u32(q[0]), joan::orig::arg::u32(q[1]), joan::orig::arg::u32(q[2]));  // 0x00635d40
    }
    static void camera_position(float out[3]) {
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_camera(global<void*>(d::g_camera))->bone()))->matrix());  // 0x0060ef80(0x00613de0)
        joan::orig::matrix_position(static_cast<void*>(out), joan::orig::arg::ptr(m));                                           // 0x0060c310
    }
    void sky_moon() {  // 0x004a7980
        if (global<void*>(d::g_camera) == nullptr || global<std::int32_t>(d::g_control_mode) == 4) return;
        c_sky.hit();
        float cam[3], d[3];
        camera_position(cam);
        reinterpret_cast<void*(JOAN_CDECL*)(void*, const void*)>(global<std::uintptr_t>(d::iat_v3d_87_vector_normalize_2))(d, jo::self<jo::LightSys>(this)->current.params.sun_dir);  // vector_normalize(out, in)
        const float p[3] = {d[0] * 1500.0f + cam[0], d[1] * 1500.0f + cam[1], d[2] * 1500.0f + cam[2]};
        if (below_or_nan(jo::self<jo::LightSys>(this)->current.params.sun_dir[2], 0.0f)) return;
        if (jo::view<void*>(jo::self<jo::LightSys>(this)->moon) == nullptr) moon_create_at(p);
        moon_move(p);
    }
    void sky_after() {  // 0x004a7ba0
        if (global<void*>(d::g_camera) == nullptr || global<std::int32_t>(d::g_control_mode) == 4) return;
        void* const camera = global<void*>(d::g_camera);
        float cam[3], d[3];
        camera_position(cam);
        reinterpret_cast<void*(JOAN_CDECL*)(void*, const void*)>(global<std::uintptr_t>(d::iat_v3d_87_vector_normalize_2))(d, jo::self<jo::LightSys>(this)->current.params.sun_dir);
        float sun[3];
        for (std::int32_t k = 0; k < 3; ++k) sun[k] = cam[k] - (jo::as_camera(global<void*>(d::g_camera))->far_plane / 2.0f) * d[k];
        const float p[3] = {d[0] * 1500.0f + cam[0], d[1] * 1500.0f + cam[1], d[2] * 1500.0f + cam[2]};
        std::int32_t sx = 0, sy = 0;
        float sz = 0.0f;
        joan::orig::as_camera(camera)->project(joan::orig::arg::mptr(static_cast<void*>(sun)), &sx, &sy, &sz);  // 0x006153c0 project
        if (below_or_nan(jo::self<jo::LightSys>(this)->current.params.sun_dir[2], 0.0f)) {
            if (below_or_nan(sz, 1.0f) && (joan::orig::arg::as<std::uint8_t>(joan::orig::as_bytes(img::obj(d::g_weather_system))->at(1)) & 0xff) == 0) {  // 0x0041bf10
                struct V3 {
                    float x, y, z;
                } s{sun[0], sun[1], sun[2]};
                if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_light_sys(this)->ray_clear(joan::orig::arg::mptr(global<void*>(d::g_camera)), s))) != 0) {  // 0x004d30f0 visible
                    std::int32_t fx = 0, fy = 0;
                    joan::orig::as_grid(global<void*>(d::g_camera))->to_local(joan::orig::arg::i32(sx), joan::orig::arg::i32(sy), &fx, &fy);  // 0x00615340
                    joan::orig::as_lens_flares(&jo::self<jo::LightSys>(this)->flares)->draw(joan::orig::arg::i16(static_cast<std::int16_t>(fx)), joan::orig::arg::i16(static_cast<std::int16_t>(fy)), 1.0f);  // 0x006628a0 flare
                }
            }
            if (jo::view<void*>(jo::self<jo::LightSys>(this)->moon) != nullptr) {
                joan::orig::as_scene(jo::as_world(global<void*>(d::g_world))->scene)->remove_sorted(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LightSys>(this)->moon)));  // 0x005eded0
                void* const moon = jo::view<void*>(jo::self<jo::LightSys>(this)->moon);
                if (moon != nullptr) {
                    using Del = void* (Obj::*)(std::uint32_t);
                    Del method{};
                    std::memcpy(&method, &joan::call::vtable_of(moon)[0], sizeof(void*));
                    (static_cast<Obj*>(moon)->*method)(1u);
                }
                jo::self<jo::LightSys>(this)->moon = nullptr;
            }
            return;
        }
        if (jo::view<void*>(jo::self<jo::LightSys>(this)->moon) == nullptr) moon_create_at(p);
        moon_move(p);
    }
};

#define SKY1(name, addr, fn) JOAN_REPLACE("game/sky1", name, addr, fn)
SKY1("Sky::moon", 0x004a7980, &H::sky_moon);
SKY1("Sky::after_frame", 0x004a7ba0, &H::sky_after);
#undef SKY1

}  // namespace game_overlays
}  // namespace

// ---------------------------------------- from modules/game_b107_07.cpp (game/b107_07)
namespace {
namespace game_b107_07 {

using joan::hook::global;

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_weather_tick, "Weather::tick_4a7f70");

void* const kWeatherMgr = obj(d::g_weather_system);
float zero() { return global<float>(d::k_9cf5c4); }  // 0.0f
void crt_memset(void* p, i32 v, u32 n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

// ---- 0x004a7f70 helpers ----
struct Weather {
    // fade step of a running fade (amount > 0): min(amount, dt), dt when amount > dt.
    static float step(const float& fade, float dt) {
        const float a = fade;
        return (a > dt) ? dt : fade;
    }
    static void madd(float& dst, float k, const float& src) {
        dst = k * src + dst;
    }

    // time-trigger table loop (three identical copies in the original)
    static void triggers(void* w) {
        for (i32 k = 0; k < static_cast<i32>(jo::as_light_sys(w)->day_count); ++k) {
            const float t = global<float>(d::g_calendar_minute_of_day);
            u8* arr = jo::view<u8*>(jo::as_light_sys(w)->days);
            if (k != static_cast<i32>(jo::as_light_sys(w)->day_count) - 1) {
                // `test ah,1; jne skip` -> fire only when t >= arr[k] (ordered)
                if (t >= day(arr, k)->time_from) {
                    arr = jo::view<u8*>(jo::as_light_sys(w)->days);
                    // `test ah,0x41; je skip` -> skip when t > arr[k+1]
                    if (!(global<float>(d::g_calendar_minute_of_day) > day(arr, k + 1)->time_from)) {
                        if (static_cast<i32>(jo::as_light_sys(w)->day_index) != k + 1) {
                            const u32 d = jo::view<u32>(day(jo::view<u8*>(jo::as_light_sys(w)->days), k)->time_to);
                            const i32 id = day(jo::view<u8*>(jo::as_light_sys(w)->days), k)->preset;
                            joan::orig::as_anim(w)->set_key_4b21a0(joan::orig::arg::i32(id), joan::orig::arg::u32(d));  // 0x004b21a0
                            jo::as_light_sys(w)->day_index = static_cast<i16>(k + 1);
                        }
                    }
                }
            } else {
                if (t >= day(arr, k)->time_from) {
                    if (static_cast<i32>(jo::as_light_sys(w)->day_index) != k + 1) {
                        const u32 d = jo::view<u32>(day(jo::view<u8*>(jo::as_light_sys(w)->days), k)->time_to);
                        const i32 id = day(jo::view<u8*>(jo::as_light_sys(w)->days), k)->preset;
                        joan::orig::as_anim(w)->set_key_4b21a0(joan::orig::arg::i32(id), joan::orig::arg::u32(d));  // 0x004b21a0
                        jo::as_light_sys(w)->day_index = static_cast<i16>(k + 1);
                    }
                }
            }
        }
    }

    // RAIN (flag 1: base 0x590, name 0x9d4710) / SNOW (flag 2: base 0x5a0, name 0x9d4708)
    // +0x4 = trigger time, +0x8 = remaining duration, +0x0 = intensity argument.
    static void precip(void* w, const float& rate, float& time, std::uint32_t& left_bits, i32 flag, img::Addr name, float dt) {
        float& left = jo::view<float>(left_bits);
        const float tm = time;
        if (tm < zero() || tm > zero()) {  // `test ah,0x40; jne skip`: equal or unordered skips
            if (global<float>(d::g_calendar_minute_of_day) >= time) {  // `test ah,1; jne skip`
                if (joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) == 0) {  // 0x004301c0 (movsx al)
                    if (joan::orig::arg::as<u8>(joan::orig::as_bytes(kWeatherMgr)->at(joan::orig::arg::i32(flag))) == 0) {  // 0x0041bf10
                        if (!(left > zero())) {  // `test ah,0x41; je skip`
                            const u32 fx = joan::orig::arg::as<u32>(joan::orig::as_table(w)->find_4a9530(joan::orig::arg::str(str(name))));  // 0x004a9530
                            if (fx != 0) {
                                joan::orig::as_weather_system(w)->start_transition(joan::orig::arg::i32(fx), 0x42700000u, joan::orig::arg::u32(jo::view<u32>(rate)));  // 0x004c02b0 (60.0f)
                                const i32 n = joan::orig::as_misc(obj(d::g_misc))->random(0xb4) + 0x190;  // 0x00590540
                                left = static_cast<float>(n);
                                joan::orig::as_panel(kWeatherMgr)->init_once();        // 0x00689ca0
                                joan::orig::as_weather_system(kWeatherMgr)->start_effect(joan::orig::arg::i32(flag));  // 0x00689fb0
                            }
                        }
                    }
                }
            }
        }
        if (left > zero()) {  // `test ah,0x41; jne skip`
            left = left - dt;
        }
        if (joan::orig::arg::as<u8>(joan::orig::as_bytes(kWeatherMgr)->at(joan::orig::arg::i32(flag))) != 0) {  // 0x0041bf10
            if (!(left > zero())) {  // `test ah,0x41; je skip`
                joan::orig::as_weather_system(kWeatherMgr)->stop_precip(joan::orig::arg::i32(flag));  // 0x004a9940
                jo::view<u32>(time) = 0;
                crt_memset(&jo::as_light_sys(w)->state_b, 0, sizeof(jo::as_light_sys(w)->state_b));
                joan::orig::as_light_params(&jo::as_light_sys(w)->state_b.preset.params)->construct();  // 0x00617ee0
                triggers(w);
            }
        }
    }
};

struct H {
    // 0x004a7f70
    void tick_4a7f70(jo::Scene* /*unused*/) {
        c_weather_tick.hit();
        void* const w = this;
        jo::LightSys* const ls = jo::self<jo::LightSys>(this);
        const float dt = joan::orig::arg::as<float>(joan::orig::as_track(obj(d::g_calendar))->wrap_delta(joan::orig::arg::f32(ls->time)));  // 0x004956a0
        jo::LightPreset& cur = ls->current;

        // ---- fade block 1 (state_a, fade at +0x324) ----
        if (ls->state_a.fade > zero()) {  // `test ah,0x41; jne else`
            jo::LightPreset& a = ls->state_a.preset;
            const float x = Weather::step(ls->state_a.fade, dt);
            const float s = x * ls->state_a.scale;
            for (i32 i = 0; i < static_cast<i32>(a.light_count); ++i) {
                Weather::madd(cur.lights[i].colour[0], s, a.lights[i].colour[0]);
                Weather::madd(cur.lights[i].colour[1], s, a.lights[i].colour[1]);
                Weather::madd(cur.lights[i].colour[2], s, a.lights[i].colour[2]);
                Weather::madd(cur.lights[i].colour[3], s, a.lights[i].colour[3]);
                Weather::madd(cur.lights[i].angle_a, x, a.lights[i].angle_a);
                Weather::madd(cur.lights[i].angle_b, x, a.lights[i].angle_b);
                cur.lights[i].enabled = a.lights[i].enabled;
            }
            for (i32 k = 0; k < 5; ++k) Weather::madd(cur.params.scale_a[k], s, a.params.scale_a[k]);
            for (i32 k = 0; k < 3; ++k) Weather::madd(cur.params.scale_b[k], s, a.params.scale_b[k]);
            for (i32 k = 0; k < 3; ++k) Weather::madd(cur.params.sun_dir[k], x, a.params.sun_dir[k]);
            ls->state_a.fade = ls->state_a.fade - dt;
        } else {
            crt_memset(&ls->state_a, 0, sizeof(ls->state_a));
            joan::orig::as_light_params(&ls->state_a.preset.params)->construct();  // 0x00617ee0
        }

        // ---- fade block 2 (state_b, fade at +0x4b8) ----
        if (ls->state_b.fade > zero()) {
            jo::LightPreset& b = ls->state_b.preset;
            const float x = Weather::step(ls->state_b.fade, dt);
            const float s = x * ls->state_b.scale;
            for (i32 j = 0; j < static_cast<i32>(b.light_count); ++j) {
                Weather::madd(cur.lights[j].colour[0], s, b.lights[j].colour[0]);
                Weather::madd(cur.lights[j].colour[1], s, b.lights[j].colour[1]);
                Weather::madd(cur.lights[j].colour[2], s, b.lights[j].colour[2]);
                Weather::madd(cur.lights[j].colour[3], s, b.lights[j].colour[3]);
                cur.lights[j].enabled = b.lights[j].enabled;
            }
            for (i32 k = 0; k < 5; ++k) Weather::madd(cur.params.scale_a[k], s, b.params.scale_a[k]);
            for (i32 k = 0; k < 3; ++k) Weather::madd(cur.params.scale_b[k], s, b.params.scale_b[k]);
            ls->state_b.fade = ls->state_b.fade - dt;
        } else {
            const u32 keep = jo::view<u32>(ls->state_b.scale);
            crt_memset(&ls->state_b, 0, sizeof(ls->state_b));
            joan::orig::as_light_params(&ls->state_b.preset.params)->construct();  // 0x00617ee0
            jo::view<u32>(ls->state_b.scale) = keep;
        }

        joan::orig::as_light_sys(w)->sync_lights();  // 0x0046a560
        jo::self<jo::LightSys>(this)->time = global<u32>(d::g_calendar_minute_of_day);
        Weather::triggers(w);

        Weather::precip(w, jo::self<jo::LightSys>(this)->event_a_rate, jo::self<jo::LightSys>(this)->event_a_time, jo::self<jo::LightSys>(this)->event_a_state, 1, d::s_RAIN, dt);  // 'RAIN'
        Weather::precip(w, jo::self<jo::LightSys>(this)->event_b_rate, jo::self<jo::LightSys>(this)->event_b_time, jo::self<jo::LightSys>(this)->event_b_state, 2, d::s_SNOW, dt);  // 'SNOW'
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_07", name, addr, fn)
B("Weather::tick_4a7f70", 0x004a7f70, &H::tick_4a7f70);
#undef B

}  // namespace game_b107_07
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

struct Obj {};
using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;

using u16 = std::uint16_t;

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;


void* const kArmies = obj(d::g_nations);
void* const kIds = obj(d::g_units);
void* const kKinds = obj(d::g_types);

i32 army_missing(i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_list(kArmies)->is_null(joan::orig::arg::i32(id))); }  // 0x00484870
u8* army_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kArmies)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0047b600
// Volatile word read: the original dereferences a possibly-null pointer here.
i32 word_at(const void* p) { return *reinterpret_cast<const volatile i16*>(p); }

JOAN_COUNTER(c_enemy_of, "Region::has_enemy_of");

// The +0x1ee list tail shared by 0x4a8c20 / 0x4ab020. Returns true with *result when the
// original returns from inside the loop.
bool list_tail(u8* p, i32& cnt) {
    while (p != nullptr) {
        if (static_cast<i32>(jo::as_facility(p)->kind) == 1) {
            if (static_cast<i32>(jo::as_building(virtual_call<u8*>(p, 0x18))->unit_kind) != 0) cnt = static_cast<i32>(static_cast<u32>(cnt) + 1u);  // slot 0x18: the building
            for (i32 j = 0; j < static_cast<i32>(jo::as_building(virtual_call<u8*>(p, 0x18))->queue_n); ++j) {
                u8* const d = virtual_call<u8*>(p, 0x18);
                cnt = static_cast<i32>(static_cast<u32>(cnt) + static_cast<u32>(static_cast<i32>(jo::as_building(d)->queue_count[j])));
                if (cnt != 0) return true;
            }
        }
        p = jo::view<u8*>(jo::as_facility(p)->prev);
    }
    return false;
}

struct H {
    // Box-scan filter shared by 0x4a8c20 / 0x4ab020: kind record of a unit that passes, or null.
    u8* scan_kind(u8* e) {
        if (!(jo::as_unit(e)->hit_points > 0 || jo::view<u32>(jo::as_unit(e)->state_120) != 0)) return nullptr;
        if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(e)->position_))) & 0xffu) == 0) return nullptr;  // 0x0060d5f0
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(e)->type_id)))));  // 0x0042d0a0
        const i32 s = jo::as_unit_type(t)->unit_class;
        if (s == 0 || s == 8 || s == 9) return nullptr;
        const i32 k = jo::as_unit_type(t)->kind;
        if (k == 7 || k == 2 || k == 3) return nullptr;
        return t;
    }

    // ------------------------------------------------------------------ 0x004a8c20
    i32 has_enemy_of(i32 id) {
        c_enemy_of.hit();
        if (joan::orig::as_dynarray(kIds)->empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00428b70
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kIds)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        if (jo::view<void*>(jo::self<jo::Place>(this)->world) == nullptr) return 0;
        i32 cnt = 0;
        u8* owner = nullptr;
        if (army_missing(jo::self<jo::Place>(this)->nation) == 0) owner = army_get(jo::self<jo::Place>(this)->nation);
        i32 n = 0;
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Place>(this)->world))->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Place>(this)->box), &n));  // 0x00429090
        for (i32 i = 0; i < n; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cells[i])->get_8c());  // 0x0054ee10
            if (first == nullptr) continue;
            u8* e = first;
            do {
                if (scan_kind(e) != nullptr && static_cast<i32>(jo::as_unit(e)->side) != 0 &&
                    joan::orig::arg::as<i32>(joan::orig::as_players(kArmies)->hostile(static_cast<i32>(jo::as_unit(rec)->side), static_cast<i32>(jo::as_unit(e)->side))) != 0)  // 0x004b6860
                    return 1;
                e = jo::view<u8*>(jo::as_unit(e)->cell_prev);
            } while (e != nullptr && e != first);
        }
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_place(this)->facility_head());  // 0x004c48b0
        const i32 w = word_at(owner);
        if (w == 0) return cnt;
        if (joan::orig::arg::as<i32>(joan::orig::as_players(kArmies)->hostile(joan::orig::arg::i32(w), static_cast<i32>(jo::as_unit(rec)->side))) == 0) return cnt;  // 0x004b6860
        if (list_tail(p, cnt)) return 1;
        return cnt;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Region::has_enemy_of", 0x004a8c20, &H::has_enemy_of);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::img::obj;

using joan::hook::x87::Fpu;


void* const kPlayers = obj(d::g_nations);

std::int32_t players_count() { return joan::orig::as_dynarray(kPlayers)->size(); }  // 0x00417ad0
std::uint8_t* player(std::int32_t i) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(i))); }  // 0x0047b600
std::uint8_t* relation(void* p, std::int32_t i) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_player(p)->relation(joan::orig::arg::i32(i))); }  // 0x004b42e0

JOAN_COUNTER(c_score, "Player::score");

struct H {
    // 0x004a8f20
    float score(std::int32_t other) {
        c_score.hit();
        const joan::hook::x87::Fpu fpu{true};
        const float k01 = global<float>(d::flt_0_1);
        const float k0001 = global<float>(d::flt_9d5360);
        std::int32_t n = 0, above_me = 0, above_other = 0, hostile = 0;
        for (std::int32_t i = 1; i <= players_count(); ++i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(kPlayers)->is_unset(joan::orig::arg::i32(i))) != 0) continue;  // 0x004b6780
            ++n;
            std::uint8_t* const pi = player(i);
            std::uint8_t* const po = player(other);
            if (jo::as_nation(pi)->strength > jo::as_nation(po)->strength) ++above_other;
            std::uint8_t* const pj = player(i);
            if (jo::as_nation(pj)->strength > jo::self<jo::Nation>(this)->strength) ++above_me;
            if (i != static_cast<std::int32_t>(jo::self<jo::Nation>(this)->id)) {
                if (static_cast<std::int32_t>(static_cast<std::int8_t>(relation(this, i)[3])) <= 0) ++hostile;
            }
        }
        if (n >= 3) {
            const std::int32_t d = static_cast<std::int32_t>(static_cast<std::uint32_t>(above_me) -
                                                             static_cast<std::uint32_t>(above_other));
            const float t1c = static_cast<float>(
                fpu.mul(fpu.div(static_cast<double>(d), static_cast<double>(n)), k01));
            const float t20 = jo::self<jo::Nation>(this)->traits[3] * k01;
            const float t24 = global<float>(d::g_a01774) * jo::self<jo::Nation>(this)->score_goods * k0001;
            const double a = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(global<float>(d::g_a0177c) * jo::self<jo::Nation>(this)->wealth)));  // 0x00427d60 fabs
            const float t28 = static_cast<float>(fpu.mul(a, k0001));
            const float t2c = static_cast<float>(fpu.mul(static_cast<double>(hostile), k01));
            return t1c + t20 + t24 + t28 - t2c;
        }
        const float t30 = jo::self<jo::Nation>(this)->traits[3] * k01;
        const float t34 = global<float>(d::g_a01774) * jo::self<jo::Nation>(this)->score_goods * k0001;
        const double a = joan::orig::arg::as<double>(joan::orig::libc::fabs_f(joan::orig::arg::f32(global<float>(d::g_a0177c) * jo::self<jo::Nation>(this)->wealth)));  // 0x00427d60 fabs
        const float t38 = static_cast<float>(fpu.mul(a, k0001));
        const float t3c = static_cast<float>(fpu.mul(static_cast<double>(hostile), k01));
        return t30 + t34 - t3c + t38;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Player::score", 0x004a8f20, &H::score);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;
using joan::img::str;

using joan::hook::x87::Fpu;
using u16 = std::uint16_t;

using i8 = std::int8_t;


void* const kTypes = obj(d::g_item_res);

u32 now() { return global<u32>(d::g_now); }
bool eq_or_nan(double v) { return !(v < 0.0 || v > 0.0); }  // fcomp 0 / test ah,0x40 jnz
u8* types(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (reads the low word)
double crt_fabs(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }

// fabs + 1000.0 timer test on an anim block (+0x30 speed, +0x38 start): true = the code path the
// original takes when the speed is 0 / NaN or 1000 / |speed| < elapsed (or unordered).
bool timer_done(u8* a_now, jo::Actor* const& actor) {
    if (eq_or_nan(static_cast<double>(jo::as_actor(a_now)->fade))) return true;
    const double dt = static_cast<double>(static_cast<u32>(now() - actor->fade_38));
    const double m = crt_fabs(static_cast<double>(actor->fade));
    const double q = Fpu{true}.div(global<double>(d::dbl_1000), m);  // 1000.0 / fabs (fdivr qword)
    return !(q >= dt);
}

JOAN_COUNTER(c_update_4a9180, "Obj4a9::update");

struct H {
    // ------------------------------------------------------------------ 0x004a9180
    void release_effect(jo::Actor*& actor, std::int32_t& slot) {
        if (actor->scene != nullptr)
            joan::orig::as_scene(actor->scene)->detach(joan::orig::arg::mptr(actor));  // 0x005efdb0
        void* const p = actor;
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840 (scalar dtor)
        actor = nullptr;
        const u32 arg = jo::view<u32>(slot);
        u8* const t = types(jo::self<jo::Wall>(this)->type_id);
        joan::orig::as_res(t)->release_slot(joan::orig::arg::i32(arg), 0);  // 0x004a3e30
    }
    void update_4a9180() {
        c_update_4a9180.hit();
        if (jo::self<jo::Wall>(this)->actor_a != nullptr && timer_done(jo::view<u8*>(jo::self<jo::Wall>(this)->actor_a), jo::self<jo::Wall>(this)->actor_a)) release_effect(jo::self<jo::Wall>(this)->actor_a, jo::self<jo::Wall>(this)->level_a);
        if (jo::self<jo::Wall>(this)->actor_b != nullptr) {
            const double v = joan::orig::arg::as<double>(joan::orig::as_unit(this)->anim_time_left_46ca50());  // 0x0046ca50 (st0)
            if (!(v > 0.0) && static_cast<i32>(jo::view<i16>(jo::self<jo::Wall>(this)->u16_e2)) == 0) {
                const u32 time = now();
                joan::orig::as_actor(jo::view<u8*>(jo::self<jo::Wall>(this)->actor_b))->start_anim(joan::orig::arg::mptr(jo::self<jo::Wall>(this)->actor_b->anim), str(d::s_f_idle), joan::orig::arg::u32(time), 0u, 0u, 0u);  // 0x005abe30 "f_idle"
                jo::self<jo::Wall>(this)->actor_b->fade_38 = now();
                jo::view<u32>(jo::self<jo::Wall>(this)->actor_b->fade) = 0xbe99999au;  // -0.3f
                jo::self<jo::Wall>(this)->u16_e2 = 1;
            } else if (static_cast<i32>(jo::view<i16>(jo::self<jo::Wall>(this)->u16_e2)) != 0 && timer_done(jo::view<u8*>(jo::self<jo::Wall>(this)->actor_b), jo::self<jo::Wall>(this)->actor_b)) {
                release_effect(jo::self<jo::Wall>(this)->actor_b, jo::self<jo::Wall>(this)->level_b);
                joan::orig::as_unit(this)->start_effect(static_cast<char>(joan::orig::arg::i32(0)));  // 0x00478b40
                jo::self<jo::Wall>(this)->u16_e4 = 0;
            }
        } else if (static_cast<i32>(jo::view<i16>(jo::self<jo::Wall>(this)->u16_e2)) != 0) {
            jo::self<jo::Wall>(this)->u16_e2 = 0;
        }
        if (jo::view<i32>(jo::self<jo::Wall>(this)->busy) > 0 && static_cast<i32>(jo::view<i16>(jo::self<jo::Wall>(this)->u16_e4)) == 0 && static_cast<i32>(jo::view<i8>(jo::self<jo::Wall>(this)->active)) != 0) {
            joan::orig::as_unit(this)->start_effect(static_cast<char>(joan::orig::arg::i32(1)));  // 0x00478b40
            jo::self<jo::Wall>(this)->u16_e4 = 1;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Obj4a9::update", 0x004a9180, &H::update_4a9180);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx2)
namespace {
namespace game_fx {

using joan::call::cdecl_call;


JOAN_COUNTER(c_find, "Table::find");

struct H {
    // 0x004a9530 Table::find(name): 1-based index of the entry (0x18c bytes, +8 base) whose
    // name matches case-insensitively, -1 if none.
    std::int32_t table_find(const char* name) {
        c_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::LightSys>(this)->special_count; ++i)
            if (joan::crt::stricmp(joan::orig::arg::str(jo::view<std::uint8_t*>(jo::self<jo::LightSys>(this)->specials) + i * 0x18c), joan::orig::arg::str(name)) == 0) return i + 1;  // _stricmp
        return -1;
    }
};

#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX2("Table::find", 0x004a9530, &H::table_find);
#undef FX2

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b106_12.cpp (game/b106_12)
namespace {
namespace game_b106_12 {

using joan::call::thiscall;

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004a9940
    u32 reset_then_68ab40(i32 a) {
        joan::orig::as_weather_system(this)->clear_flag(joan::orig::arg::i32(a));         // 0x004a9aa0
        joan::orig::as_sounds(this)->stop(joan::orig::arg::i32(a));  // 0x0068ab40
        return 0;  // sole caller discards the result; Sounds::stop returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_12", name, addr, fn)
B("Obj::reset_then_68ab40", 0x004a9940, &H::reset_then_68ab40);
#undef B

}  // namespace game_b106_12
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------ 0x004a9aa0
    std::uintptr_t cut_4a9aa0(i32 n) {  // eax: the pointer
        u8* const p = reinterpret_cast<u8*>(this) + n;
        p[1] = 0;
        return reinterpret_cast<std::uintptr_t>(p);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Str::cut_4a9aa0", 0x004a9aa0, &H::cut_4a9aa0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b100_04_2.cpp (game/b100_04)
namespace {
namespace game_b100_04_2 {

using joan::call::thiscall;

using u8 = std::uint8_t;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x004a4ed0
    void* ctor_4a4ed0() {
        joan::orig::as_light_state(&jo::self<jo::LightSys>(this)->current)->construct_base();    // 0x004a4f50
        joan::orig::as_light_state(&jo::self<jo::LightSys>(this)->state_a)->construct();  // 0x004a4f80
        joan::orig::as_light_state(&jo::self<jo::LightSys>(this)->state_b)->construct();
        joan::orig::as_lens_flares(&jo::self<jo::LightSys>(this)->flares)->construct();  // 0x006625f0
        crt_memset(this, 0, sizeof(jo::LightSys));  // 0x5ea on x86
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::ctor_4a4ed0", 0x004a4ed0, &H::ctor_4a4ed0);
#undef B

}  // namespace game_b100_04_2
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x004a4f50
    H* ctor_4a4f50() {
        joan::orig::as_shader_constants(&jo::self<jo::LightState>(this)->preset.params)->construct();  // 0x00617d80 (+0x12c)
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::ctor_4a4f50", 0x004a4f50, &H::ctor_4a4f50);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004a4f80
    void* ctor_4a4f80() {
        joan::orig::as_light_state(this)->construct_base();  // 0x004a4f50
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Obj::Obj_4a4f80", 0x004a4f80, &H::ctor_4a4f80);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04_2 {


using joan::call::thiscall;

using u8 = std::uint8_t;

using joan::hook::global;
using joan::hook::x87::Fpu;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::img::obj;

void* const kRandom = obj(d::g_misc);

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

i32 random(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_formation, "Army::pick_formation");

struct H {
    // ------------------------------------------------------------------ 0x004a5630
    void pick_formation() {
        c_formation.hit();
        const Fpu fpu{true};
        i32 last = 0;
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::LightSys>(this)->day_count); ++i) {
            // fcomp; test ah,1 jne skip: taken unless [0xa47bb4] >= entry (ordered)
            if (global<float>(d::g_calendar_minute_of_day) >= jo::as_light_day(jo::view<u8*>(jo::self<jo::LightSys>(this)->days) + i * kDayRec)->time_from) last = i + 1;
        }
        jo::self<jo::LightSys>(this)->day_index = last != 0 ? static_cast<i16>(last) : static_cast<i16>(1);
        joan::orig::as_units(this)->load_record(static_cast<i32>(jo::as_light_day(jo::view<u8*>(jo::self<jo::LightSys>(this)->days) + (static_cast<i32>(jo::self<jo::LightSys>(this)->day_index) - 1) * kDayRec)->preset));  // 0x0042e160

        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::LightSys>(this)->current.light_count); ++i) {
            jo::LightDef* const rec = light_def(this, i);
            jo::view<u32>(jo::as_light(jo::view<u8*>(jo::self<jo::LightSys>(this)->lights[i]))->orbit_a) = jo::view<u32>(rec->angle_a);
            jo::view<u32>(jo::as_light(jo::view<u8*>(jo::self<jo::LightSys>(this)->lights[i]))->orbit_b) = jo::view<u32>(rec->angle_b);
            const i32 c3 = ftol(rec->colour[3]);
            const i32 c2 = ftol(rec->colour[2]);
            const i32 c1 = ftol(rec->colour[1]);
            const i32 c0 = ftol(rec->colour[0]);
            u32 tmp = 0;
            const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(c0, c1, c2, c3));  // 0x0042a260
            jo::view<u32>(jo::as_light(jo::view<u8*>(jo::self<jo::LightSys>(this)->lights[i]))->colour) = colour;
            jo::as_light(jo::view<u8*>(jo::self<jo::LightSys>(this)->lights[i]))->active_ = jo::view<i8>(rec->enabled) != 0 ? 1 : 0;
            joan::orig::as_light(jo::view<u8*>(jo::self<jo::LightSys>(this)->lights[i]))->invalidate();  // 0x005c5c00
        }

        const i32 r1 = random(100);
        if (!(static_cast<double>(r1) >= static_cast<double>(jo::self<jo::LightSys>(this)->event_a_chance))) {  // test ah,1: less or unordered
            const i32 n = ftol(global<float>(d::g_a47bc8) - global<float>(d::g_calendar_day_start_minute));
            const i32 r = random(n);
            jo::self<jo::LightSys>(this)->event_a_time = static_cast<float>(fpu.add(static_cast<double>(r), static_cast<double>(global<float>(d::g_calendar_day_start_minute))));
            jo::self<jo::LightSys>(this)->event_a_state = 0;
        }
        const i32 r2 = random(100);
        if (!(static_cast<double>(r2) >= static_cast<double>(jo::self<jo::LightSys>(this)->event_b_chance))) {
            const i32 n = ftol(global<float>(d::g_a47bc8) - global<float>(d::g_calendar_day_start_minute));
            const i32 r = random(n);
            jo::self<jo::LightSys>(this)->event_b_time = static_cast<float>(fpu.add(static_cast<double>(r), static_cast<double>(global<float>(d::g_calendar_day_start_minute))));
            jo::self<jo::LightSys>(this)->event_b_state = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Army::pick_formation", 0x004a5630, &H::pick_formation);
#undef B

}  // namespace game_b101_04_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::LightState::construct_base() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_02::H::ctor_4a4f50>(this)); }  // 0x4a4f50
auto joan::orig::LightState::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_02::H::ctor_4a4f80>(this)); }  // 0x4a4f80
auto joan::orig::LightSys::init(const char* a0, void* a1) -> void { joan::bind::member<&::game_units5_2::H::light_init>(this, a0, a1); }  // 0x4a5020
auto joan::orig::Army::pick_formation() -> void { joan::bind::member<&::game_b101_04_2::H::pick_formation>(this); }  // 0x4a5630
auto joan::orig::LightSys::clear() -> void { joan::bind::member<&::game_units5::H::light_clear>(this); }  // 0x4a5970
auto joan::orig::LightSys::load_lights(const char* a0) -> void { joan::bind::member<&::game_light1::H::load_lights>(this, a0); }  // 0x4a5bf0
auto joan::orig::LightSys::lit_by(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_04::H::lit_by>(this, a0)); }  // 0x4a65d0
auto joan::orig::LightSys::load_special() -> void { joan::bind::member<&::game_light1::H::load_special>(this); }  // 0x4a6840
auto joan::orig::LightSys::load_day(const char* a0) -> void { joan::bind::member<&::game_light1::H::load_day>(this, a0); }  // 0x4a7450
auto joan::orig::LightSys::update_moon() -> void { joan::bind::member<&::game_overlays::H::sky_moon>(this); }  // 0x4a7980
auto joan::orig::LightSys::after_frame() -> void { joan::bind::member<&::game_overlays::H::sky_after>(this); }  // 0x4a7ba0
auto joan::orig::LightSys::tick_weather(Scene* a0) -> void { joan::bind::member<&::game_b107_07::H::tick_4a7f70>(this, a0); }  // 0x4a7f70
auto joan::orig::Place::has_enemy_of(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_04::H::has_enemy_of>(this, a0)); }  // 0x4a8c20
auto joan::orig::Player::score(std::int32_t a0) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b92_03::H::score>(this, a0)); }  // 0x4a8f20
auto joan::orig::LightSys::find_set(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_13::H::find_set>(this, a0)); }  // 0x4a9120
auto joan::orig::Table::find_4a9530(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::table_find>(this, a0)); }  // 0x4a9530
auto joan::orig::WeatherSystem::stop_precip(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_12::H::reset_then_68ab40>(this, a0)); }  // 0x4a9940
auto joan::orig::WeatherSystem::clear_flag(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_05::H::cut_4a9aa0>(this, a0)); }  // 0x4a9aa0
