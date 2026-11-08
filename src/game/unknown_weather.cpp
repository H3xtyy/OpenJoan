// game/unknown_weather.cpp: functions whose original source file is unknown (formerly game/_unplaced/weather.cpp;
// re/attribution.csv: between ww/uti3d/videocap.cpp and ww/enphysic/collimgr.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using i16 = std::int16_t;

using joan::img::str;

using joan::orig::helpers::at;

using joan::call::cdecl_call;

using joan::hook::x87::Fpu;

using i8 = std::int8_t;

using joan::hook::x87::ftol;

void* const kResources = obj(d::g_pemi_res);

}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct alignas(4) Path {  // 0x194 bytes in the frame (ebp-0x1a0 .. ebp-0xc)
    u8 bytes[0x194];
};

JOAN_COUNTER(c_obj689b10, "Obj689b10::Obj");
JOAN_COUNTER(c_thunder, "ThunderList::update");

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x00689b10
    void* ctor_689b10() {
        c_obj689b10.hit();
        joan::orig::dynarray(&jo::self<jo::Weather>(this)->bolts)->construct(static_cast<i32>(sizeof(void*)), 0xa, 0);  // pointer records (4 on x86)  // 0x005843d0
        joan::orig::dynarray(&jo::self<jo::Weather>(this)->thunder)->construct(static_cast<i32>(sizeof(void*)), 0xa, 0);  // pointer records (4 on x86)  // 0x005843d0
        joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->rain)->construct();             // 0x006758f0
        joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->snow)->construct();             // 0x006758f0
        joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->ripple)->construct();            // 0x006758f0
        memset_(this, 0, static_cast<u32>(offsetof(jo::Weather, bolts)));  // the fields before the lists (0x26)
        return this;
    }

    // ------------------------------------------------------------------ 0x0068c2a0
    void thunder_update() {
        c_thunder.hit();
        void* const list = &jo::self<jo::Weather>(this)->thunder;
        for (i32 i = joan::orig::as_dynarray(list)->size(); i > 0; --i) {  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (e == nullptr) continue;
            if (static_cast<i32>(jo::as_lightning_bolt(e)->flash) != 0 && jo::as_lightning_bolt(e)->flash_time < global<u32>(d::g_now)) {
                joan::orig::as_d3d(obj(d::g_sys3d))->set_gamma(joan::orig::arg::f32(jo::as_lightning_bolt(e)->gamma));  // 0x00602b50
                joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                    // 0x00602cd0
                jo::as_lightning_bolt(e)->flash = 0;
                Path path;
                joan::orig::as_string(&path)->construct();                // 0x005a1170
                joan::orig::as_string(&path)->assign(str(d::s_Thunder));                // 0x005a1440 "Thunder"
                const i32 r = joan::orig::as_misc(obj(d::g_misc))->random(3);  // 0x00590540
                joan::orig::as_string(&path)->append(joan::orig::arg::i32(r + 1));   // 0x005a15d0
                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
                joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(s);                 // 0x00492c40
            }
            if (jo::as_lightning_bolt(e)->u32_c < global<u32>(d::g_now)) {
                joan::orig::as_lightning_bolt(e)->close();           // 0x0068c840
                joan::orig::dynarray(list)->linkout(joan::orig::arg::i32(i));  // 0x00584a10
                if (e != nullptr) joan::orig::as_lightning_bolt(e)->destroy(1u);  // 0x00689f60 (delete)
            }
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj689b10::Obj", 0x00689b10, &H::ctor_689b10);
B96("ThunderList::update", 0x0068c2a0, &H::thunder_update);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using u32 = std::uint32_t;

JOAN_COUNTER(c_dtor689bf0, "Obj689bf0::~dtor");

float pi_from_atan() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(Fpu{true}.mul(a, 4.0));       // qword [0x9cf788] = 4.0
}
// 0x0068c980
void JOAN_CDECL init_pi_a6a3d0() { global<float>(d::g_pi_a6a3d0) = pi_from_atan(); }

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x00689bf0
    u32 dtor_689bf0() {
        c_dtor689bf0.hit();
        joan::orig::as_zones(&jo::self<jo::Weather>(this)->ripple)->dtor();            // 0x00675930
        joan::orig::as_zones(&jo::self<jo::Weather>(this)->snow)->dtor();             // 0x00675930
        joan::orig::as_zones(&jo::self<jo::Weather>(this)->rain)->dtor();             // 0x00675930
        joan::orig::dynarray(&jo::self<jo::Weather>(this)->thunder)->destruct();        // 0x005844b0
        return thiscall<u32>(fn::dtor_DynArray, &jo::self<jo::Weather>(this)->bolts);  // 0x005844b0
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj689bf0::~dtor", 0x00689bf0, &H::dtor_689bf0);
B97("init_pi_a6a3d0", 0x0068c980, &init_pi_a6a3d0);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

using u16 = std::uint16_t;

using u32 = std::uint32_t;


struct H {
    // ------------------------------------------------------------------ 0x00689ca0
    void* init_once() {
        if (jo::self<jo::Weather>(this)->initialized != 0) return this;
        joan::orig::as_weather_system(this)->init_rain();  // 0x0068b380
        jo::view<u16>(jo::self<jo::Weather>(this)->active) = 1;
        joan::orig::as_weather_system(this)->load_snow();  // 0x0068b600
        joan::orig::as_weather_system(this)->init_ripple();  // 0x0068b900
        jo::self<jo::Weather>(this)->initialized = 1;
        return this;
    }
    // ------------------------------------------------------------------ 0x0068ac20
    u32 activate(i32 kind) {
        if (kind == 1 && joan::orig::arg::as<u8>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->rain)->initialized()) != 0)  // 0x0068c940
            return joan::orig::arg::as<u32>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->rain)->parts_alive());            // 0x006772f0
        if (kind == 1 && joan::orig::arg::as<u8>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->ripple)->initialized()) != 0)
            return joan::orig::arg::as<u32>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->ripple)->parts_alive());
        if (kind == 2 && joan::orig::arg::as<u8>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->snow)->initialized()) != 0)
            return joan::orig::arg::as<u32>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->snow)->parts_alive());
        return 0;
    }
    // ------------------------------------------------------------------ 0x0068ae90
    void* sound_release() {
        if (jo::self<jo::Weather>(this)->model_res == 0) return this;
        joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(reinterpret_cast<const i16&>(jo::self<jo::Weather>(this)->model_res)));  // 0x004b16a0
        if (jo::self<jo::Weather>(this)->anim_res != 0) joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->release(joan::orig::arg::i16(reinterpret_cast<const i16&>(jo::self<jo::Weather>(this)->anim_res)));  // 0x00417e00
        jo::view<u32>(jo::self<jo::Weather>(this)->model_res) = 0;
        jo::view<u32>(jo::self<jo::Weather>(this)->anim_res) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("Panel::init_once", 0x00689ca0, &H::init_once);
B("Slots::activate", 0x0068ac20, &H::activate);
B("Sound::release", 0x0068ae90, &H::sound_release);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

JOAN_COUNTER(c_sound_release, "Sound3D::release");
JOAN_COUNTER(c_detach_channels, "Sound3D::detach_channels");

using joan::orig::helpers::node_remove;  // 0x005efdb0
std::int32_t list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // 0x00689d00 Sound3D::release(char stop_arg)
    void sound_release(char arg) {
        c_sound_release.hit();
        void* const plays = &jo::self<jo::Weather>(this)->thunder;
        for (std::int32_t i = list_count(plays); i > 0; --i) {
            void* const e = joan::orig::as_dynarray(plays)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (e == nullptr) continue;
            joan::orig::as_lightning_bolt(e)->close();                    // 0x0068c840
            if (e != nullptr) joan::orig::as_lightning_bolt(e)->destroy(1u);  // 0x00689f60 delete
            joan::orig::dynarray(plays)->linkout(joan::orig::arg::i32(i));  // 0x00584a10
        }
        joan::orig::dynarray(plays)->zap(1);  // 0x00584580
        if (jo::self<jo::Weather>(this)->initialized == 0) return;
        jo::EffectState* const channels[3] = {&jo::self<jo::Weather>(this)->rain, &jo::self<jo::Weather>(this)->snow, &jo::self<jo::Weather>(this)->ripple};
        for (std::int32_t k = 0; k < 3; ++k) {
            void* const ch = channels[k];
            if (jo::view<void*>(jo::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(ch)->object()))->scene) != nullptr) {  // 0x0041b280
                void* const obj = joan::orig::arg::as<void*>(joan::orig::as_effect_state(ch)->object());
                node_remove(jo::view<void*>(jo::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(ch)->object()))->scene), obj);
            }
        }
        joan::orig::as_sounds(this)->stop(0);  // 0x0068ab40
        joan::orig::as_actor(&jo::self<jo::Weather>(this)->rain)->release_675b30(joan::orig::arg::u32(arg));   // 0x00675b30
        joan::orig::as_actor(&jo::self<jo::Weather>(this)->snow)->release_675b30(joan::orig::arg::u32(arg));
        joan::orig::as_actor(&jo::self<jo::Weather>(this)->ripple)->release_675b30(joan::orig::arg::u32(arg));
        if (jo::view<std::int32_t>(jo::self<jo::Weather>(this)->rain_voice) != 0) {
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(img::obj(d::g_sfx_player))->entry(joan::orig::arg::i16(static_cast<std::int16_t>(reinterpret_cast<const std::uint16_t&>(jo::self<jo::Weather>(this)->rain_voice)))));  // 0x004cf690
            joan::orig::as_stream_slot(slot)->release();  // 0x004cea40
            jo::view<std::int32_t>(jo::self<jo::Weather>(this)->rain_voice) = 0;
        }
        for (std::int32_t i = 0; i < 3; ++i) jo::self<jo::Weather>(this)->slot_flags[i] = 0;
        jo::self<jo::Weather>(this)->initialized = 0;
    }

    // 0x0068aa10 Sound3D::detach_channels
    void detach_channels() {
        c_detach_channels.hit();
        jo::EffectState* const channels[3] = {&jo::self<jo::Weather>(this)->rain, &jo::self<jo::Weather>(this)->snow, &jo::self<jo::Weather>(this)->ripple};
        for (std::int32_t k = 0; k < 3; ++k) {
            void* const ch = channels[k];
            if (joan::orig::arg::as<void*>(joan::orig::as_effect_state(ch)->object()) == nullptr) continue;  // 0x0041b280
            if (jo::view<void*>(jo::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(ch)->object()))->scene) == nullptr) continue;
            void* const obj = joan::orig::arg::as<void*>(joan::orig::as_effect_state(ch)->object());
            node_remove(jo::view<void*>(jo::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(ch)->object()))->scene), obj);
        }
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("Sound3D::release", 0x00689d00, &H::sound_release);
L81("Sound3D::detach_channels", 0x0068aa10, &H::detach_channels);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_leaves82a.cpp (game/leaves82a)
namespace {
namespace game_leaves82a {

struct Obj {};

// Virtual `scalar deleting destructor' (vtable slot 0, thiscall, flags).
void* virtual_delete(void* object, std::uint32_t flags) {
    using Virtual = void* (Obj::*)(std::uint32_t);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(object)[0], sizeof(void*));
    return (static_cast<Obj*>(object)->*method)(flags);
}

JOAN_COUNTER(c_sounds_stop, "Sounds::stop");
JOAN_COUNTER(c_entries_flush, "Entries::flush");
JOAN_COUNTER(c_entry_close, "Entry::close");

struct H {
    // 0x00689f60 scalar deleting destructor: 0x0068c800, operator delete when flags & 1.
    void* entry_delete(std::uint32_t flags) {
        joan::orig::as_lightning_bolt(this)->destruct();  // 0x0068c800
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }

    // 0x0068ab40 Sounds::stop(which): 0 / 1 stop +0x60 and +0x150 (0x00677120) when +0x60 is
    // playing (0x0068c940) and free the voice +4 (0x004cf690 / 0x004cea40 on 0xa5cf98);
    // 0 / 2 stop +0xd8 when playing.
    void sounds_stop(std::int32_t which) {
        c_sounds_stop.hit();
        std::uint8_t* const self = reinterpret_cast<std::uint8_t*>(this);
        if (which == 0 || which == 1) {
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_effect_state(&jo::as_weather(self)->rain)->initialized()) != 0) {  // 0x0068c940
                joan::orig::as_effect_state(&jo::as_weather(self)->rain)->parts_stop();   // 0x00677120
                joan::orig::as_effect_state(&jo::as_weather(self)->ripple)->parts_stop();  // 0x00677120
                if (jo::self<jo::Weather>(this)->rain_voice != 0) {
                    void* const voice = joan::orig::arg::as<void*>(joan::orig::as_sound_list(obj(d::g_sfx_player))->entry(joan::orig::arg::i16(jo::self<jo::Weather>(this)->rain_voice)));  // 0x004cf690 (word)
                    joan::orig::as_stream_slot(voice)->release();                                                          // 0x004cea40
                    jo::self<jo::Weather>(this)->rain_voice = 0;
                }
            }
        }
        if (which == 0 || which == 2) {
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_effect_state(&jo::as_weather(self)->snow)->initialized()) != 0)  // 0x0068c940
                joan::orig::as_effect_state(&jo::as_weather(self)->snow)->parts_stop();                       // 0x00677120
        }
    }

    // 0x0068c6c0 Entries::flush: while 0x0068c570 yields an entry: release its resource +0x14
    // on 0xa52420 (0x00602b50 / 0x00602cd0), close it (0x0068c840), and remove / delete every
    // slot of the list +0x43 that holds it.
    void entries_flush() {
        c_entries_flush.hit();
        void* const list = &jo::self<jo::Weather>(this)->thunder;
        for (;;) {
            void* const entry = joan::orig::arg::as<void*>(joan::orig::as_weather_system(this)->active_lightning());  // 0x0068c570
            if (entry == nullptr) return;
            joan::orig::as_d3d(obj(d::g_sys3d))->set_gamma(joan::orig::arg::f32(jo::as_lightning_bolt(entry)->gamma));  // 0x00602b50
            joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                                 // 0x00602cd0
            joan::orig::as_lightning_bolt(entry)->close();                                           // 0x0068c840
            for (std::int32_t i = joan::orig::as_dynarray(list)->size(); i > 0; --i) {  // 0x00417ad0
                void* const item = joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
                if (item == nullptr) continue;
                if (item != entry) continue;
                joan::orig::dynarray(list)->linkout(joan::orig::arg::i32(i));  // 0x00584a10
                if (entry != nullptr) joan::orig::as_lightning_bolt(entry)->destroy(1u);  // 0x00689f60 delete
            }
        }
    }

    // 0x0068c840 Entry::close: +0 object (0x006356c0(0), virtual delete), +0x18 / +0x14
    // resource on 0xa52420, +4 handle (0x005eb700 on (*0xa45f3e)+0x6e); clears +8 / +0xc / +0x18.
    void entry_close() {
        c_entry_close.hit();
        if (jo::view<void*>(jo::self<jo::LightningBolt>(this)->billboard) != nullptr) {
            joan::orig::as_billboard(jo::view<void*>(jo::self<jo::LightningBolt>(this)->billboard))->release(0u);  // 0x006356c0
            void* const object = jo::view<void*>(jo::self<jo::LightningBolt>(this)->billboard);
            if (object != nullptr) virtual_delete(object, 1);
            jo::view<void*>(jo::self<jo::LightningBolt>(this)->billboard) = nullptr;
        }
        if (static_cast<std::int32_t>(jo::self<jo::LightningBolt>(this)->flash) != 0) {
            joan::orig::as_d3d(obj(d::g_sys3d))->set_gamma(joan::orig::arg::f32(jo::self<jo::LightningBolt>(this)->gamma));  // 0x00602b50
            joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                          // 0x00602cd0
        }
        if (jo::self<jo::LightningBolt>(this)->light != 0) {
            joan::orig::as_scene(
jo::view<void*>(jo::as_world(global<std::uint8_t*>(d::g_world))->scene))->remove_light(joan::orig::arg::mptr(jo::self<jo::LightningBolt>(this)->light));  // 0x005eb700
            jo::self<jo::LightningBolt>(this)->light = 0;
        }
        jo::self<jo::LightningBolt>(this)->u32_8 = 0;
        jo::self<jo::LightningBolt>(this)->u32_c = 0;
        jo::self<jo::LightningBolt>(this)->flash = 0;
    }
};

#define LEAVES82A(name, addr, fn) JOAN_REPLACE("game/leaves82a", name, addr, fn)
LEAVES82A("Entry::scalar_deleting_destructor", 0x00689f60, &H::entry_delete);
LEAVES82A("Sounds::stop", 0x0068ab40, &H::sounds_stop);
LEAVES82A("Entries::flush", 0x0068c6c0, &H::entries_flush);
LEAVES82A("Entry::close", 0x0068c840, &H::entry_close);
#undef LEAVES82A

}  // namespace game_leaves82a
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using u32 = std::uint32_t;

using joan::orig::helpers::addr;

// ------------------------------------------------------------------------------- cdecl
void JOAN_CDECL div_a6a3d0() {  // 0x0068c9e0
    global<float>(d::g_a6a3d4) = global<float>(d::g_pi_a6a3d0) / global<float>(d::flt_2);
}

struct H {

    std::uintptr_t flag_slot(i32 n) {  // 0x00689fb0
        jo::self<jo::Weather>(this)->slot_flags[n] = 1;
        if (n == 1) return joan::orig::arg::as<u32>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->rain)->parts_start());  // 0x00677250
        if (n == 2) return joan::orig::arg::as<u32>(joan::orig::as_effect_state(&jo::self<jo::Weather>(this)->snow)->parts_start());
        return addr(this) + static_cast<u32>(n);
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::flag_slot", 0x00689fb0, &H::flag_slot);
B98("Globals::div_a6a3d0", 0x0068c9e0, &div_a6a3d0);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

JOAN_COUNTER(c_release_parts, "Obj::release_parts");

// The three part slots of 0x0068a010: 0x0041b280 get, then 0x005efdb0 on its +0x18.
void release_slot(jo::EffectState* slot) {
    if (joan::orig::arg::as<void*>(joan::orig::as_effect_state(slot)->object()) == nullptr) return;                           // 0x0041b280
    if (jo::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(slot)->object()))->scene == nullptr) return;
    void* const p = joan::orig::arg::as<void*>(joan::orig::as_effect_state(slot)->object());
    joan::orig::as_scene(jo::view<void*>(jo::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(slot)->object()))->scene))->detach(joan::orig::arg::mptr(p));     // 0x005efdb0
}

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x0068a010
    void release_parts() {
        c_release_parts.hit();
        if (joan::orig::arg::as<u8>(joan::orig::as_bytes(this)->at(1)) == 0) {  // 0x0041bf10
            release_slot(&jo::self<jo::Weather>(this)->rain);
            release_slot(&jo::self<jo::Weather>(this)->ripple);
        }
        if (joan::orig::arg::as<u8>(joan::orig::as_bytes(this)->at(2)) == 0) release_slot(&jo::self<jo::Weather>(this)->snow);
    }
};

// ---------------------------------------------------------------------------- scale setters
static void JOAN_CDECL scale_68ca30() { global<float>(d::g_a6a3d8) = global<float>(d::g_pi_a6a3d0) / global<float>(d::flt_3); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::release_parts", 0x0068a010, &H::release_parts);
B95("scale_68ca30", 0x0068ca30, &scale_68ca30);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b106_04.cpp (game/b106_04)
namespace {
namespace game_b106_04 {

using joan::call::import_call;

using u32 = std::uint32_t;

float f_of(u32 bits) {
    float f;
    std::memcpy(&f, &bits, 4);
    return f;
}
u32 bits_of(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}

struct Vec3 {  // D3DVECTOR as raw dwords
    u32 v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");
// The by-value vector argument as the callee declares it: float members (System V: XMM registers).
struct F3 {
    float x, y, z;
};
F3 f3(const Vec3& v) {
    F3 f;
    std::memcpy(&f, &v, sizeof f);
    return f;
}

struct Mat {  // D3DMATRIX as raw dwords
    u32 m[16];
};
static_assert(sizeof(Mat) == 64, "Mat");

Vec3 load_vec(const void* p) {
    Vec3 v;
    std::memcpy(&v, p, 12);
    return v;
}

JOAN_COUNTER(c_cam_track, "Camera::track_68a170");

struct H {
    // -----------------------------------------------------------------------------------
    // 0x00525080
    struct Frame525 {         // ebp-0x16c .. ebp-0xc of the original
        char b16c[0x20];      // ebp-0x16c
        u8 pad14c[4];         // ebp-0x14c (NUL of the 0x20 read)
        char b148[0x28];      // ebp-0x148
        u8 pad120[4];         // ebp-0x120 (NUL of the 0x28 read)
        char b11c[8];         // ebp-0x11c
        u8 pad114[4];         // ebp-0x114 (NUL of the 8 read)
        char b110[0x100];     // ebp-0x110
        i32 i;                // ebp-0x10 (NUL of the 0x100 read lands in its low byte)
    };
    static_assert(sizeof(Frame525) == 0x160 && offsetof(Frame525, i) == 0x15c, "frame");

    // -----------------------------------------------------------------------------------
    // 0x0068a170
    static void sync_owner(jo::EffectState* base, u8* p28) {
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_effect_state(base)->object());  // 0x0041b280
        if (jo::as_actor(o)->scene != jo::as_actor(jo::as_unit(p28)->actor)->scene) {
            void* const a = joan::orig::arg::as<void*>(joan::orig::as_effect_state(base)->object());
            void* const b = joan::orig::arg::as<void*>(joan::orig::as_effect_state(base)->object());
            joan::orig::as_scene(jo::view<void*>(jo::as_actor(b)->scene))->detach(joan::orig::arg::mptr(a));  // 0x005efdb0
            void* const c = joan::orig::arg::as<void*>(joan::orig::as_effect_state(base)->object());
            joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::as_unit(p28)->actor)->scene))->attach(joan::orig::arg::mptr(c));  // 0x005efc50
        }
    }
    static void set_active(u8* self, i32 on) {
        joan::orig::as_model_instance(joan::orig::arg::as<void*>(joan::orig::as_effect_state(&jo::as_weather(self)->rain)->object()))->set_10a(joan::orig::arg::u32(on));  // 0x0042cf20
    }

    void cam_track(u32 a1, void* a2, i32 mode) {
        c_cam_track.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        Vec3 v10;
        Vec3 v1c;
        {
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_camera(a2)->matrix());  // 0x00616e90
            joan::orig::matrix_axis_z(&v10, joan::orig::arg::ptr(m));           // 0x0060c220
        }
        {
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_camera(a2)->matrix());  // 0x00616e90
            joan::orig::matrix_position(&v1c, joan::orig::arg::ptr(m));           // 0x0060c310
        }
        if (global<i32>(d::g_control_mode) != 4 &&
            static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) == 0) {  // 0x004301c0
            if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) == 0) {  // 0x00428b70
                u8* const p20 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
                v1c = load_vec(jo::as_unit(p20)->position_);
                if (jo::as_unit(p20)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::as_unit(p20)->actor)->record) != 0) {
                    u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(p20)->actor)->record))));  // 0x006af310
                    v1c.v[2] = bits_of(jo::view<float>(jo::as_shot(r)->state.cell) + global<float>(d::flt_250));  /* Body +0x218 = Shot::state +0xd6 */  // the Players list (0x006af310) holds Body records
                } else {
                    v1c.v[2] = bits_of(f_of(v1c.v[2]) + global<float>(d::flt_250));
                }
            } else {
                v1c.v[2] = bits_of(f_of(v1c.v[2]) + global<float>(d::flt_50));
            }
        }
        u8* p28 = nullptr;
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) == 0)
            p28 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));
        u8 flag = 1;
        if (p28 != nullptr && global<i32>(d::g_control_mode) != 4) {
            for (i32 i = joan::orig::as_dynarray(obj(d::g_place_array))->size(); i > 0; --i) {  // 0x00417ad0
                if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_place_array))->is_missing_ca0(joan::orig::arg::i32(i))) != 0) continue;  // 0x00456ca0
                u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_place_array))->get_c70(joan::orig::arg::i32(i)));      // 0x00456c70
                if (jo::as_place(p)->kind != 4) continue;
                Vec3 v40 = load_vec(jo::as_unit(p28)->position_);
                const float ang = jo::view<float>(jo::as_place(p)->angle);
                const float k = global<float>(d::k_9cf5c4);
                if (ang < k || ang > k) {
                    Mat m;
                    joan::orig::matrix_rotation(&m, joan::orig::arg::f32(jo::as_place(p)->angle));  // 0x0060bc50
                    joan::orig::set_30(&m, joan::orig::arg::mptr(jo::as_place(p)->position_));         // 0x0060c360
                    import_call<void*>(d::iat_v3d_87_matrix_inverse_1, &m);                  // matrix_inverse
                    import_call<void*>(d::iat_v3d_87_point_transform_2, &v40, &m);            // point_transform
                }
                if (static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::as_place(p)->box)->contains(&v40))) != 0) flag = 0;  // 0x0060d5f0
            }
        }
        if (mode == 1 && static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_effect_state(&jo::as_weather(self)->rain)->initialized())) != 0) {  // 0x0068c940
            if (p28 != nullptr && jo::as_unit(p28)->actor != nullptr && flag != 0) {
                jo::as_weather(self)->u32_22 = jo::as_unit(p28)->cell;
                sync_owner(&jo::as_weather(self)->rain, p28);
                if (jo::view<i8>(jo::as_weather(self)->rain.visible) != 0)
                    joan::orig::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(&jo::as_weather(self)->rain)->object()))->set_position_5ac310(&v1c);  // 0x005ac310
                joan::orig::as_effect_state(&jo::as_weather(self)->rain)->set_position(f3(v1c));                                     // 0x0041b2a0
                joan::orig::as_effect_state(&jo::as_weather(self)->rain)->update_parts(joan::orig::arg::f32(a1), joan::orig::arg::mptr(a2));                                  // 0x00675e90
                if (joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->rand_unit_inclusive()) > static_cast<double>(global<float>(d::flt_0_999))) {  // 0x00590660
                    const i32 r = joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->random_range(2, 5));  // 0x00590690
                    joan::orig::as_weather_system(self)->advance_time(joan::orig::arg::i32(r));                                   // 0x0068c600
                }
                Vec3 v8c = load_vec(jo::as_unit(p28)->position_);
                u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_waters(obj(d::g_water_array))->find_at(joan::orig::arg::mptr(jo::as_unit(p28)->position_), 0));  // 0x00469d10
                if (g != nullptr && jo::view<i8>(jo::as_water(g)->bytes_0[2]) != 0) {
                    v8c.v[2] = bits_of(jo::as_water(g)->box[2] + global<float>(d::flt_0_1));
                    joan::orig::as_effect_state(&jo::as_weather(self)->ripple)->parts_start();  // 0x00677250
                    sync_owner(&jo::as_weather(self)->ripple, p28);
                    if (jo::view<i8>(jo::as_weather(self)->ripple.visible) != 0)
                        joan::orig::as_actor(joan::orig::arg::as<void*>(joan::orig::as_effect_state(&jo::as_weather(self)->ripple)->object()))->set_position_5ac310(&v8c);  // 0x005ac310
                    joan::orig::as_effect_state(&jo::as_weather(self)->ripple)->set_position(f3(v8c));                                     // 0x0041b2a0
                } else {
                    joan::orig::as_effect_state(&jo::as_weather(self)->ripple)->parts_hide();  // 0x006771c0
                }
                joan::orig::as_effect_state(&jo::as_weather(self)->ripple)->update_parts(joan::orig::arg::f32(a1), joan::orig::arg::mptr(a2));  // 0x00675e90
                if (jo::as_weather(self)->ripple.object_ != nullptr && jo::as_weather(self)->ripple.object_->plane != nullptr) {
                    u8* const t = jo::view<u8*>(jo::as_weather(self)->ripple.object_->plane);
                    for (i32 i = 0; i < jo::as_plane(t)->material_count; ++i) {
                        jo::Material* const e = &jo::as_plane(t)->materials[i];  // 0x54-byte records on x86
                        jo::as_material(e)->raster = jo::as_material(e)->raster | 0x10;
                    }
                }
            }
            set_active(self, flag != 0 ? 1 : 0);
        }
        if (mode == 2 && static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_effect_state(&jo::as_weather(self)->snow)->initialized())) != 0) {  // 0x0068c940
            if (p28 != nullptr && jo::as_unit(p28)->actor != nullptr && flag != 0)
                jo::as_weather(self)->u32_22 = jo::as_unit(p28)->cell;
            if (p28 != nullptr && jo::as_unit(p28)->actor != nullptr) sync_owner(&jo::as_weather(self)->snow, p28);
            if (flag != 0) {
                set_active(self, 1);
                v1c.v[2] = bits_of(f_of(v1c.v[2]) - global<float>(d::flt_250));
                joan::orig::as_effect_state(&jo::as_weather(self)->snow)->set_position(f3(v1c));      // 0x0041b2a0
                joan::orig::as_effect_state(&jo::as_weather(self)->snow)->update_parts(joan::orig::arg::f32(a1), joan::orig::arg::mptr(a2));  // 0x00675e90
            } else {
                set_active(self, 0);
            }
        }
    }
};

JOAN_REPLACE("game/b106_04", "Camera::track_68a170", 0x0068a170, &H::cam_track);

}  // namespace game_b106_04
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using u32 = std::uint32_t;

using joan::orig::helpers::addr_of;

// CRT (called directly as in the original)
void crt_strcpy(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void crt_strcat(void* d, const void* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_ripple, "Ripple::load_68ace0");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0068ace0
    std::uintptr_t ripple_load() {
        c_ripple.hit();
        if (jo::view<u32>(jo::self<jo::Weather>(this)->model_res) != 0) return addr_of(this);
        void* const items = obj(d::g_model_res_mgr);
        void* const anims = obj(d::g_anim_res_mgr);
        jo::view<u32>(jo::self<jo::Weather>(this)->model_res) = joan::orig::arg::as<u32>(joan::orig::as_model_res(items)->add(str(d::s_ITEM_9cf648), str(d::s_ripple_am1), i32{0}));  // 0x004b18d0
        char buf[0x104];
        void* rec = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(items)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Weather>(this)->model_res))));  // 0x0041a180
        crt_strcpy(buf, jo::as_model_entry(rec)->dir);
        crt_strcat(buf, str(d::s_ANIM));  // "ANIM\\"
        jo::view<u32>(jo::self<jo::Weather>(this)->anim_res) = joan::orig::arg::as<u32>(joan::orig::as_anim_res_mgr(anims)->add_slot(nullptr));  // 0x00417470
        rec = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(items)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Weather>(this)->model_res))));
        const char* const file = jo::as_model_entry(rec)->file;
        void* a = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(anims)->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Weather>(this)->anim_res))));  // 0x004174f0
        joan::orig::as_path(a)->init(joan::orig::arg::str(buf), joan::orig::arg::str(file));                            // 0x00418550
        if (jo::view<u32>(jo::self<jo::Weather>(this)->anim_res) != 0) joan::orig::as_anim_set(anims)->acquire_417540(joan::orig::arg::u32(reinterpret_cast<const i16&>(jo::self<jo::Weather>(this)->anim_res)));  // 0x00417540
        joan::orig::as_model_res_mgr(items)->set_params(joan::orig::arg::i16(reinterpret_cast<const i16&>(jo::self<jo::Weather>(this)->model_res)), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), joan::orig::arg::i16(reinterpret_cast<const i16&>(jo::self<jo::Weather>(this)->anim_res)));  // 0x004b0ec0
        joan::orig::as_model_res(items)->acquire(joan::orig::arg::i16(reinterpret_cast<const i16&>(jo::self<jo::Weather>(this)->model_res)));  // 0x004b0f40
        rec = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(items)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Weather>(this)->model_res))));
        jo::as_model_entry(rec)->actor_def->model->mesh->materials->raster |= 0x110;
        jo::self<jo::Weather>(this)->u32_c = 0;
        jo::self<jo::Weather>(this)->u32_10 = 0x28;
        return addr_of(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("Script::parse_func_643c00", 0x00643c00, &H::parse_func);
// Not registered: a direct callee (script lexer 0x6423e0 / token reader) throws C++ exceptions through this frame.
// B("Script::fwd_6437d0", 0x006437d0, &H::fwd_6437d0);
B("Ripple::load_68ace0", 0x0068ace0, &H::ripple_load);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b104_12.cpp (game/b104_12)
namespace {
namespace game_b104_12 {

using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);
void* const kArrays = obj(d::g_misc);

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12);

JOAN_COUNTER(c_spawn, "Action::spawn_undefined");
JOAN_COUNTER(c_advance, "X::advance_time");

struct H {
    // ------------------------------------------------------------------------ 0x0068c600
    void advance_time(i32 n) {
        c_advance.hit();
        i32 t = global<i32>(d::g_now);
        for (i32 k = n; k > 0; --k) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_weather_system(this)->spawn_thunder(joan::orig::arg::u32(t)));  // 0x0068bbb0
            const float scale = static_cast<float>(global<i32>(d::g_a3cdec));
            const float half = global<float>(d::g_weather_lg_v) / global<float>(d::flt_2);
            const float hi = half + global<float>(d::flt_1);
            const float lo = global<float>(d::flt_1) - global<float>(d::g_weather_lg_v) / global<float>(d::flt_2);
            const double r = joan::orig::arg::as<double>(joan::orig::as_misc(kArrays)->random_float(joan::orig::arg::f32(lo), joan::orig::arg::f32(hi)));  // 0x005906f0
            const i32 add = ftol(Fpu{true}.mul(r, static_cast<double>(scale)));
            t = jo::view<i32>(jo::as_lightning_bolt(e)->u32_c) + add;
        }
    }

    // ------------------------------------------------------------------------ 0x0068af10
    i32 rnd(i32 n) { return joan::orig::as_misc(kArrays)->random(joan::orig::arg::i32(n)); }  // 0x00590540
    float offset(float base, i32 range) {
        if (rnd(2) != 0) return static_cast<float>(rnd(range)) + base;
        return base - static_cast<float>(rnd(range));
    }
    void spawn_undefined() {
        c_spawn.hit();
        if (global<i32>(d::g_player_unit_recno) == 0) return;
        Vec3 here;
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0
        joan::orig::as_unit(u)->position(&here);                                               // 0x004fedb0
        Vec3 p{0.0f, 0.0f, 0.0f};  // x / y stay unset in the original only in the unreachable > 9 band
        const i32 band = rnd(10);
        if (band <= 4) {
            p.x = offset(here.x, 100);
            p.y = offset(here.y, 100);
        } else if (band <= 7) {
            p.x = offset(here.x, 200);
            p.y = offset(here.y, 200);
        } else if (band <= 9) {
            p.x = offset(here.x, 300);
            p.y = offset(here.y, 300);
        }
        u8* const zone = joan::orig::arg::as<u8*>(joan::orig::as_waters(obj(d::g_water_array))->find_at(&p, 0));  // 0x00469d10 (reads x / y)
        if (zone == nullptr || jo::view<i8>(jo::as_water(zone)->bytes_0[2]) == 0) return;
        p.z = jo::as_water(zone)->box[2] + global<float>(d::flt_1);
        void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::SceneObject)));  // operator new (0x10c on x86)
        void* act = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Weather>(this)->model_res))));  // 0x0041a180
        jo::as_scene_object(act)->def = jo::as_model_entry(t)->actor_def;
        joan::orig::as_scene_object(act)->init();  // 0x005aab70
        jo::as_scene_object(act)->kind = -1;
        const u32 now = global<u32>(d::g_now);
        void* const anim_slot = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Weather>(this)->anim_res))));  // 0x004174f0
        joan::orig::as_actor(act)->start_anim(joan::orig::arg::mptr(jo::as_anim_slot(anim_slot)->anim), &img::global<const char>(d::s_undefine_action), joan::orig::arg::u32(now), 0u, 0u, 0u);  // 0x005abe30
        jo::view<u32>(jo::as_scene_object(act)->fade) = 0xbf800000u;  // -1.0f
        jo::as_scene_object(act)->fade_38 = global<u32>(d::g_now);
        joan::orig::as_actor(act)->set_position_5ac310(&p);                                   // 0x005ac310
        joan::orig::dynarray(&jo::self<jo::Weather>(this)->bolts)->linkin(&act);                       // 0x00584620
        joan::orig::as_scene(jo::as_water(zone)->u32_30)->attach(joan::orig::arg::mptr(act));  // 0x005efc50
        jo::view<i32>(jo::self<jo::Weather>(this)->u32_c) = jo::view<i32>(jo::self<jo::Weather>(this)->u32_c) + 1;
        jo::self<jo::Weather>(this)->last_spawn = global<u32>(d::g_now);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_12", name, addr, fn)
B("Action::spawn_undefined", 0x0068af10, &H::spawn_undefined);
B("X::advance_time", 0x0068c600, &H::advance_time);
#undef B

}  // namespace game_b104_12
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
namespace {
namespace game_b100_15 {

using u32 = std::uint32_t;


using joan::orig::helpers::op_new;
using joan::orig::helpers::crt_memset;

struct Vec4 {
    u32 x, y, z, w;
};
static_assert(sizeof(Vec4) == 16, "Vec4");
struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_rain_init, "Rain::init");

struct H {
    // ------------------------------------------------------------------ 0x0068b380
    void* rain_init() {
        c_rain_init.hit();
        const u32 id = joan::orig::arg::as<u32>(joan::orig::as_name_list(kResources)->find_4c2060(str(d::s_rain_txt), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060 "rain.txt"
        u8* const res = joan::orig::arg::as<u8*>(joan::orig::as_name_list(kResources)->entry(joan::orig::arg::i32(id)));                 // 0x0041b440
        void* const def = jo::as_pemi_script(res)->effect;
        void* const sub = &jo::self<jo::Weather>(this)->rain;
        joan::orig::as_effect_state(sub)->init();                                              // 0x006759a0
        joan::orig::as_handle(sub)->assign(joan::orig::arg::mptr(def));                                         // 0x00679300
        joan::orig::as_effect_state(sub)->set_name(joan::orig::arg::str(joan::orig::arg::as<u32>(joan::orig::as_effect_state(def)->name())));              // 0x0067a1c0(0x0041b3a0)
        joan::orig::as_effect_state(sub)->set_source_blend(joan::orig::arg::i16(joan::orig::as_effect_state(def)->source_blend()));              // 0x0041b320(0x0041b300)
        joan::orig::as_effect_state(sub)->set_dest_blend(joan::orig::arg::i16(joan::orig::as_effect_state(def)->dest_blend()));              // 0x0041b370(0x0041b350)
        joan::orig::as_effect_state(sub)->start();                                              // 0x0041b170

        void* const p1 = op_new(static_cast<u32>(sizeof(jo::ActorDef)));
        void* const effect = p1 != nullptr ? joan::orig::as_actor_def(p1)->construct() : nullptr;  // 0x005af390
        Vec4 sphere{global<u32>(d::g_pos), global<u32>(d::g_a69cdc), global<u32>(d::g_a69ce0), 0x43480000u};
        jo::Plane* const v24 = joan::orig::as_effect_state(sub)->model();                              // 0x0041b230
        joan::orig::as_actor_def(effect)->create(v24, &sphere);                             // 0x005b02f0

        void* const p2 = op_new(static_cast<u32>(sizeof(jo::SceneObject)));
        u8* const actor = p2 != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_scene_object(p2)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::view<void*>(jo::as_scene_object(actor)->def) = effect;
        joan::orig::as_scene_object(actor)->init();                                            // 0x005aab70
        Vec3 origin{0, 0, 0};
        joan::orig::as_actor(actor)->set_position_5ac310(&origin);                                   // 0x005ac310
        jo::self<jo::Rain>(this)->rain.visible = 0;
        joan::orig::as_effect_state(sub)->set_model(jo::as_scene_object(actor)->plane);                        // 0x0041b200
        joan::orig::as_effect_state(sub)->set_actor(jo::as_actor(actor));                                       // 0x0041b250
        jo::view<u32>(jo::as_scene_object(actor)->holder_kind) = 2;
        jo::as_scene_object(actor)->holder = static_cast<jo::Attachment*>(sub);
        jo::view<u32>(jo::as_scene_object(actor)->kind) = 2;
        return actor;
    }

    // ------------------------------------------------------------------ memset ctors
    H* clear_1a() {  // 0x0068c7d0
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::LightningBolt)));  // 0x1a on x86
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("Rain::init", 0x0068b380, &H::rain_init);
B("Obj::clear_1a", 0x0068c7d0, &H::clear_1a);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
namespace {
namespace game_b101_14 {

using u32 = std::uint32_t;


using joan::orig::helpers::op_new;      // operator new

JOAN_COUNTER(c_snow, "Weather::load_snow");

struct H {
    // ------------------------------------------------------------------ 0x0068b600
    void load_snow() {
        c_snow.hit();
        char name[0x40];  // original: 32-byte buffer; "snow" + "%d.txt" of a short fits
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name)), str(d::s_snow_txt));  // strcpy "snow.txt"
        char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(name), 0x2e));             // strrchr '.'
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dot)), str(d::kEmpty));  // strcpy ""
        joan::orig::libc::sprintf(joan::orig::arg::str(dot), str(d::s_d_txt), static_cast<i32>(jo::self<jo::Weather>(this)->active));  // sprintf "%d.txt"
        void* const table = obj(d::g_pemi_res);
        const i32 handle = joan::orig::arg::as<i32>(joan::orig::as_name_list(table)->find_4c2060(joan::orig::arg::str(name), static_cast<std::int8_t>(joan::orig::arg::i32(1))));                // 0x004c2060
        u8* const res = joan::orig::arg::as<u8*>(joan::orig::as_name_list(table)->entry(joan::orig::arg::i32(handle)));                    // 0x0041b440
        void* const def = jo::as_pemi_script(res)->effect;
        u8* const fx = reinterpret_cast<u8*>(&jo::self<jo::Weather>(this)->snow);
        joan::orig::as_effect_state(fx)->init();                                                // 0x006759a0
        joan::orig::as_handle(fx)->assign(joan::orig::arg::mptr(def));                                           // 0x00679300
        joan::orig::as_effect_state(fx)->set_name(joan::orig::arg::str(joan::orig::arg::as<u32>(joan::orig::as_effect_state(def)->name())));               // 0x0067a1c0(0x0041b3a0)
        joan::orig::as_effect_state(fx)->set_source_blend(joan::orig::arg::i16(joan::orig::as_effect_state(def)->source_blend()));               // 0x0041b320(0x0041b300)
        joan::orig::as_effect_state(fx)->set_dest_blend(joan::orig::arg::i16(joan::orig::as_effect_state(def)->dest_blend()));               // 0x0041b370(0x0041b350)
        joan::orig::as_effect_state(fx)->start();                                                // 0x0041b170
        void* const mem = op_new(static_cast<u32>(sizeof(jo::ActorDef)));
        void* const actor = mem != nullptr ? joan::orig::as_actor_def(mem)->construct() : nullptr;  // 0x005af390
        u32 pos[4] = {global<u32>(d::g_pos), global<u32>(d::g_a69cdc), global<u32>(d::g_a69ce0), 0x43480000u};
        jo::Plane* const model = joan::orig::as_effect_state(fx)->model();                              // 0x0041b230
        joan::orig::as_actor_def(actor)->create(model, pos);                                 // 0x005b02f0
        void* const mem2 = op_new(static_cast<u32>(sizeof(jo::SceneObject)));
        u8* const scene = mem2 != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_scene_object(mem2)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::view<void*>(jo::as_scene_object(scene)->def) = actor;
        joan::orig::as_scene_object(scene)->init();                                             // 0x005aab70
        u32 zero[3] = {0, 0, 0};
        joan::orig::as_actor(scene)->set_position_5ac310(joan::orig::arg::mptr(zero));                                       // 0x005ac310
        jo::self<jo::Weather>(this)->snow.visible = 0;
        joan::orig::as_effect_state(fx)->set_model(jo::as_scene_object(scene)->plane);                          // 0x0041b200
        joan::orig::as_effect_state(fx)->set_actor(jo::as_actor(scene));                                         // 0x0041b250
        jo::view<u32>(jo::as_scene_object(scene)->holder_kind) = 2;
        jo::view<u8*>(jo::as_scene_object(scene)->holder) = fx;
        jo::view<u32>(jo::as_scene_object(scene)->kind) = 2;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("Weather::load_snow", 0x0068b600, &H::load_snow);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using u32 = std::uint32_t;


using joan::orig::helpers::op_new;

struct Vec4 {
    u32 x, y, z, w;
};
static_assert(sizeof(Vec4) == 16, "Vec4");
struct Vec3 {
    u32 x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_ripple_init, "Ripple::init");

struct H {
    // ------------------------------------------------------------------ 0x0068b900
    void* ripple_init() {
        c_ripple_init.hit();
        const u32 id = joan::orig::arg::as<u32>(joan::orig::as_name_list(kResources)->find_4c2060(str(d::s_ripple_txt), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060 "ripple.txt"
        u8* const res = joan::orig::arg::as<u8*>(joan::orig::as_name_list(kResources)->entry(joan::orig::arg::i32(id)));                 // 0x0041b440
        void* const def = jo::as_pemi_script(res)->effect;
        void* const sub = &jo::self<jo::Weather>(this)->ripple;
        joan::orig::as_effect_state(sub)->init();                                              // 0x006759a0
        joan::orig::as_handle(sub)->assign(joan::orig::arg::mptr(def));                                         // 0x00679300
        joan::orig::as_effect_state(sub)->set_name(joan::orig::arg::str(joan::orig::arg::as<u32>(joan::orig::as_effect_state(def)->name())));              // 0x0067a1c0(0x0041b3a0)
        joan::orig::as_effect_state(sub)->set_source_blend(joan::orig::arg::i16(joan::orig::as_effect_state(def)->source_blend()));              // 0x0041b320(0x0041b300)
        joan::orig::as_effect_state(sub)->set_dest_blend(joan::orig::arg::i16(joan::orig::as_effect_state(def)->dest_blend()));              // 0x0041b370(0x0041b350)
        joan::orig::as_effect_state(sub)->start();                                              // 0x0041b170

        void* const p1 = op_new(static_cast<u32>(sizeof(jo::ActorDef)));
        void* const effect = p1 != nullptr ? joan::orig::as_actor_def(p1)->construct() : nullptr;  // 0x005af390
        Vec4 sphere{global<u32>(d::g_pos), global<u32>(d::g_a69cdc), global<u32>(d::g_a69ce0), 0x43480000u};
        jo::Plane* const v24 = joan::orig::as_effect_state(sub)->model();                              // 0x0041b230
        joan::orig::as_actor_def(effect)->create(v24, &sphere);                             // 0x005b02f0

        void* const p2 = op_new(static_cast<u32>(sizeof(jo::SceneObject)));
        u8* const actor = p2 != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_scene_object(p2)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::view<void*>(jo::as_scene_object(actor)->def) = effect;
        joan::orig::as_scene_object(actor)->init();                                            // 0x005aab70
        Vec3 origin{0, 0, 0};
        joan::orig::as_actor(actor)->set_position_5ac310(&origin);                                   // 0x005ac310
        jo::self<jo::Ripple>(this)->ripple.visible = 0;
        joan::orig::as_effect_state(sub)->set_model(jo::as_scene_object(actor)->plane);                        // 0x0041b200
        joan::orig::as_effect_state(sub)->set_actor(jo::as_actor(actor));                                       // 0x0041b250
        jo::view<u32>(jo::as_scene_object(actor)->holder_kind) = 2;
        jo::as_scene_object(actor)->holder = static_cast<jo::Attachment*>(sub);
        jo::view<u32>(jo::as_scene_object(actor)->kind) = 2;
        return actor;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Ripple::init", 0x0068b900, &H::ripple_init);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

constexpr Fpu kFpu{true};

using u32 = std::uint32_t;

u32 bits(float v) {
    u32 r;
    std::memcpy(&r, &v, 4);
    return r;
}
float fl(u32 v) {
    float r;
    std::memcpy(&r, &v, 4);
    return r;
}

using joan::orig::helpers::op_new;  // operator new

// Vec3 (12 bytes), passed by pointer or by value.
struct Vec3 {
    u32 v[3];
};
static_assert(sizeof(Vec3) == 12, "Vec3");
// Camera block copied by 0x6140a0 (10 dwords).
struct CamBlock {
    u32 v[10];
};
static_assert(sizeof(CamBlock) == 0x28, "CamBlock");

void* const kUtil = obj(d::g_misc);

JOAN_COUNTER(c_thunder, "Weather::spawn_thunder");

struct H {
    // ------------------------------------------------------------------------ 0x0068bbb0
    void* spawn_thunder(u32 base) {
        c_thunder.hit();
        alignas(4) char name[0x194];
        joan::orig::as_string(name)->construct(str(d::s_thunder));      // 0x005a11a0 ('thunder')
        i32 r;
        for (;;) {
            r = joan::orig::arg::as<i32>(joan::orig::as_misc(kUtil)->random_range(1, 4));  // 0x00590690
            if (r != jo::self<jo::Weather>(this)->i32_1c) {
                jo::self<jo::Weather>(this)->i32_1c = r;
                break;
            }
        }
        if (r < 10) joan::orig::as_string(name)->append(str(d::s_0));                // 0x005a1550 ('0')
        joan::orig::as_string(name)->append(joan::orig::arg::i32(r));                // 0x005a15d0
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(// 0x006099c0
                                          joan::orig::arg::as<void*>(joan::orig::as_string(name)->c_str())), 1, 0u));  // 0x00417d90
        const float hi = global<float>(d::g_weather_lg_v) / global<float>(d::flt_2) + global<float>(d::flt_1);
        const float lo = global<float>(d::flt_1) - global<float>(d::g_weather_lg_v) / global<float>(d::flt_2);
        const float s = joan::orig::arg::as<float>(joan::orig::as_misc(kUtil)->random_float(joan::orig::arg::f32(lo), joan::orig::arg::f32(hi)));  // 0x005906f0
        const double ds = static_cast<double>(s);
        const double half = static_cast<double>(global<float>(d::flt_0_5));
        const float w = static_cast<float>(kFpu.mul(kFpu.mul(static_cast<double>(jo::view<i32>(jo::as_texture(res)->width)), ds), half));
        const float h = static_cast<float>(kFpu.mul(kFpu.mul(static_cast<double>(jo::view<i32>(jo::as_texture(res)->height)), ds), half));
        if (res != nullptr) joan::orig::texture_ref::release(joan::orig::arg::mptr(res));  // 0x0042cef0

        CamBlock cam{};
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(&cam);  // 0x006140a0
        Vec3 fwd{}, side{};
        joan::orig::matrix_axis_z(&fwd, joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_camera(global<void*>(d::g_camera))->bone())));   // 0x0060c220 / 0x00613de0
        joan::orig::matrix_axis_x(&side, joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::as_camera(global<void*>(d::g_camera))->bone())));  // 0x0060c180 / 0x00613de0
        Vec3 tmp{};
        Vec3 a = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&tmp, &fwd, joan::orig::arg::f32(cam.v[7])));  // 0x0060cd00 (scale)
        Vec3 tmp2{};
        Vec3 pos = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp2, &cam.v[1], &a));  // 0x0060cc90 (sub)
        {
            const float d = static_cast<float>(kFpu.add(static_cast<double>(global<i32>(d::g_a3cddc)),
                                                         kFpu.mul(static_cast<double>(global<i32>(d::g_weather_lg_r)), ds)));
            Vec3 t3{};
            Vec3 b = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&t3, &fwd, joan::orig::arg::f32(d)));  // 0x0060cd00
            joan::orig::vec_add_in_place(&pos, &b);                // 0x0060cdc0 (add)
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_misc(kUtil)->random_range(0, 1)) != 0) {  // 0x00590690
            const i32 v = joan::orig::arg::as<i32>(joan::orig::as_misc(kUtil)->random_range(1, joan::orig::arg::i32(global<i32>(d::g_weather_lg_r))));
            Vec3 t4{};
            Vec3 b = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&t4, &side, joan::orig::arg::f32(static_cast<float>(v))));
            joan::orig::vec_add_in_place(&pos, &b);  // 0x0060cdc0 (add)
        } else {
            const i32 v = joan::orig::arg::as<i32>(joan::orig::as_misc(kUtil)->random_range(1, joan::orig::arg::i32(global<i32>(d::g_weather_lg_r))));
            Vec3 t4{};
            Vec3 b = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&t4, &side, joan::orig::arg::f32(static_cast<float>(v))));
            joan::orig::vec3_sub_assign(&pos, &b);  // 0x0060ce20 (sub)
        }
        {
            const double t1 = kFpu.add(static_cast<double>(global<i32>(d::g_a3cdd4)), static_cast<double>(fl(cam.v[3])));
            const i32 span = static_cast<i32>(global<u32>(d::g_a3cde0) - global<u32>(d::g_a3cdd4));
            const double t2 = kFpu.mul(static_cast<double>(span), ds);
            pos.v[2] = bits(static_cast<float>(kFpu.add(t1, t2)));
        }
        void* fx;
        {
            void* const m = op_new(static_cast<u32>(sizeof(jo::LightningBolt)));  // 0x1a on x86
            fx = m != nullptr ? joan::orig::as_lightning_bolt(m)->construct() : nullptr;  // 0x0068c7d0
        }
        {
            void* const m = op_new(static_cast<u32>(sizeof(jo::Billboard)));  // 0x37 on x86
            jo::view<void*>(jo::as_lightning_bolt(fx)->billboard) = m != nullptr ? joan::orig::as_billboard(m)->construct() : nullptr;  // 0x006351a0
        }
        u32 rgba = 0;
        const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&rgba)->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
        void* const nm = joan::orig::arg::as<void*>(joan::orig::as_string(name)->c_str());     // 0x00417d90
        joan::orig::as_billboard(jo::view<void*>(jo::as_lightning_bolt(fx)->billboard))->create(pos.v[0], pos.v[1], pos.v[2], bits(w), bits(h), joan::orig::arg::str(nm), joan::orig::arg::i16(2), joan::orig::arg::u32(colour), 0u, 0u, 0u, 0u,  // 0x00635260
                      0x11u, 1u, 2u);
        void* e = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::as_lightning_bolt(fx)->billboard))->plane());  // 0x00635e80
        jo::as_plane(e)->materials->raster = 0x100;
        e = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::as_lightning_bolt(fx)->billboard))->plane());
        joan::orig::as_plane(e)->set_format(4);  // 0x005cdc70
        e = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::as_lightning_bolt(fx)->billboard))->plane());
        joan::orig::as_plane(e)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        jo::as_lightning_bolt(fx)->u32_8 = base;
        {
            const i32 dx = joan::hook::x87::ftol(kFpu.mul(static_cast<double>(global<i32>(d::g_a3cdd8)), ds));
            jo::as_lightning_bolt(fx)->u32_c = base + static_cast<u32>(dx);
        }
        {
            const i32 dy = joan::hook::x87::ftol(kFpu.mul(static_cast<double>(global<i32>(d::g_a3cdcc)), ds));
            jo::as_lightning_bolt(fx)->flash_time = base + static_cast<u32>(dy);
        }
        void* slot = fx;
        joan::orig::dynarray(&jo::self<jo::Weather>(this)->thunder)->linkin(&slot);      // 0x00584620 (may update slot)
        return slot;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Weather::spawn_thunder", 0x0068bbb0, &H::spawn_thunder);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx2)
namespace {
namespace game_fx {

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }  // game clock (ms)

// Thunks.
constexpr fn::Code kListCount = fn::List_count_417ad0;   // 0x00417ad0
constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0
constexpr fn::Code kPlaneDraw = fn::Plane_render;   // 0x005d0570
constexpr fn::Code kPlaneOf = fn::Billboard_plane;     // 0x00635e80
constexpr fn::Code kBillboardPlace = fn::Billboard_place;  // 0x00635760

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }
void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

JOAN_COUNTER(c_lightning, "Lightning::draw");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x0068c570 Lightning::active: the newest bolt that is flashing (+0x18) or alive (+4).
    void* lightning_active() {
        for (std::int32_t i = list_count(&jo::self<jo::Weather>(this)->thunder); i > 0; --i) {
            void* const bolt = list_get(&jo::self<jo::Weather>(this)->thunder, i);
            if (bolt == nullptr) continue;
            if (jo::as_lightning_bolt(bolt)->flash != 0) return bolt;
            if (jo::as_lightning_bolt(bolt)->light != 0) return bolt;
        }
        return nullptr;
    }
    // 0x0068c440 Lightning::draw(ctx): bolts whose start time (+8) has come; the first frame
    // of a bolt (until +0x10) flashes the screen through the gamma ramp.
    void lightning_draw(void* ctx) {
        c_lightning.hit();
        for (std::int32_t i = list_count(&jo::self<jo::Weather>(this)->thunder); i > 0; --i) {
            std::uint8_t* const bolt = static_cast<std::uint8_t*>(list_get(&jo::self<jo::Weather>(this)->thunder, i));
            if (bolt == nullptr) continue;
            if (jo::view<void*>(jo::as_lightning_bolt(bolt)->billboard) == nullptr) continue;
            if (jo::as_lightning_bolt(bolt)->u32_8 > now()) continue;
            thiscall(kBillboardPlace, jo::view<void*>(jo::as_lightning_bolt(bolt)->billboard), ctx);
            thiscall(kPlaneDraw, thiscall<void*>(kPlaneOf, jo::view<void*>(jo::as_lightning_bolt(bolt)->billboard)), ctx, 2, 0);
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_weather_system(this)->active_lightning()) != 0) continue;  // 0x0068c570
            if (jo::as_lightning_bolt(bolt)->flash != 0) continue;
            if (jo::as_lightning_bolt(bolt)->flash_time < now()) continue;
            jo::as_lightning_bolt(bolt)->gamma = global<std::uint32_t>(d::g_a52717);
            void* const d3d = img::obj(d::g_sys3d);
            joan::orig::as_d3d(d3d)->set_gamma(joan::orig::arg::f32(global<float>(d::g_a3cdc8)));  // 0x00602b50
            joan::orig::as_d3d(d3d)->apply_gamma();                            // 0x00602cd0
            jo::as_lightning_bolt(bolt)->flash = 1;
        }
    }
};

#define FX2(name, addr, fn) JOAN_REPLACE("game/fx2", name, addr, fn)
FX2("Lightning::active", 0x0068c570, &H::lightning_active);
FX2("Lightning::draw", 0x0068c440, &H::lightning_draw);
#undef FX2

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14 {

using joan::orig::helpers::addr_of;

struct H {
    // 0x0068c800 Entry::~Entry() (ret 0): empty.
    void entry_dtor() {}

    // 0x0068c940 Entry::flag() (ret 0): al = byte +0 (eax upper bytes = this).
    std::uintptr_t entry_flag() { return (addr_of(this) & 0xffffff00u) | jo::self<jo::EffectState>(this)->initialised; }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("Entry::~Entry", 0x0068c800, &H::entry_dtor);
B82_14("Entry::flag", 0x0068c940, &H::entry_flag);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {

using u32 = std::uint32_t;

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

struct H {
    void nop0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
// Static initialisers.
B("init_68c960", 0x0068c960, &static_init<fn::init_pi_a6a3d0>);
B("init_68c9c0", 0x0068c9c0, &static_init<fn::Globals_div_a6a3d0>);
B("init_68ca10", 0x0068ca10, &static_init<fn::scale_68ca30>);
B("init_68ca60", 0x0068ca60, &static_init<fn::scale_quarter>);
B("Obj::nop_68c820", 0x0068c820, &H::nop0);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

// ---------------------------------------------------------------------------- 0x0068ca80
static void JOAN_CDECL scale_quarter() {
    global<float>(d::g_a6a3dc) = global<float>(d::g_pi_a6a3d0) / global<float>(d::flt_4);  // / 4.0f
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("scale_quarter", 0x0068ca80, &scale_quarter);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::init_once() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b102_14::H::init_once>(this)); }  // 0x689ca0
auto joan::orig::Sound3D::release(char a0) -> void { joan::bind::member<&::game_leaves81::H::sound_release>(this, a0); }  // 0x689d00
auto joan::orig::LightningBolt::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82a::H::entry_delete>(this, a0)); }  // 0x689f60
auto joan::orig::WeatherSystem::start_effect(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::flag_slot>(this, a0)); }  // 0x689fb0
auto joan::orig::Camera::track_68a170(std::uint32_t a0, void* a1, std::int32_t a2) -> void { joan::bind::member<&::game_b106_04::H::cam_track>(this, a0, a1, a2); }  // 0x68a170
auto joan::orig::Sound3D::detach_channels() -> void { joan::bind::member<&::game_leaves81::H::detach_channels>(this); }  // 0x68aa10
auto joan::orig::Sounds::stop(std::int32_t a0) -> void { joan::bind::member<&::game_leaves82a::H::sounds_stop>(this, a0); }  // 0x68ab40
auto joan::orig::WeatherSystem::init_rain() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_15::H::rain_init>(this)); }  // 0x68b380
auto joan::orig::WeatherSystem::load_snow() -> void { joan::bind::member<&::game_b101_14::H::load_snow>(this); }  // 0x68b600
auto joan::orig::WeatherSystem::init_ripple() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_10::H::ripple_init>(this)); }  // 0x68b900
auto joan::orig::WeatherSystem::spawn_thunder(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b103_15::H::spawn_thunder>(this, a0)); }  // 0x68bbb0
auto joan::orig::WeatherSystem::draw_lightning(void* a0) -> void { joan::bind::member<&::game_fx::H::lightning_draw>(this, a0); }  // 0x68c440
auto joan::orig::WeatherSystem::active_lightning() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::lightning_active>(this)); }  // 0x68c570
auto joan::orig::WeatherSystem::advance_time(std::int32_t a0) -> void { joan::bind::member<&::game_b104_12::H::advance_time>(this, a0); }  // 0x68c600
auto joan::orig::WeatherSystem::flush_lightning() -> void { joan::bind::member<&::game_leaves82a::H::entries_flush>(this); }  // 0x68c6c0
auto joan::orig::LightningBolt::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_15::H::clear_1a>(this)); }  // 0x68c7d0
auto joan::orig::LightningBolt::destruct() -> void { joan::bind::member<&::game_b82_14::H::entry_dtor>(this); }  // 0x68c800
auto joan::orig::LightningBolt::close() -> void { joan::bind::member<&::game_leaves82a::H::entry_close>(this); }  // 0x68c840
auto joan::orig::EffectState::initialized() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_14::H::entry_flag>(this)); }  // 0x68c940
