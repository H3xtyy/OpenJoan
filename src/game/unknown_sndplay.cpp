// game/unknown_sndplay.cpp: functions whose original source file is unknown (formerly game/_unplaced/sndplay.cpp;
// re/attribution.csv: between ww/sitemres.cpp and ww/speech_s.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "sound_slots.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;

using u8 = std::uint8_t;

using joan::call::cdecl_call;
using joan::img::str;

using i8 = std::int8_t;

struct Obj {};

}  // namespace

// ---------------------------------------- the sound slots above the original 12 (game/sound_slots.hpp)
namespace {
jo::StreamSlot g_extra_slots[joan::sound_slots::kFixed - joan::sound_slots::kOriginal];  // zero = empty, as Struct134::zero leaves the game's
}  // namespace

int joan::sound_slots::count() { return joan::hook::settings().fix_sound_slots ? kFixed : kOriginal; }
void* joan::sound_slots::entry(void* list, std::int32_t i) {
    if (i > kOriginal) return &g_extra_slots[i - kOriginal - 1];
    return &jo::self<jo::Panels>(list)->slots[i - 1];  // 1-based
}

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::memset_;

JOAN_COUNTER(c_has_kind, "Slots::has_kind");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004cfee0
    bool has_kind(std::int32_t kind) {
        c_has_kind.hit();
        if (kind == 0) return false;
        for (std::int16_t i = 1; static_cast<std::int32_t>(i) <= joan::sound_slots::count(); i = static_cast<std::int16_t>(i + 1)) {
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(self())->entry(joan::orig::arg::i16(static_cast<std::int32_t>(i))));  // 0x004cf690
            if (jo::as_stream_slot(slot)->source == nullptr) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound_slot_entry(slot)->id()) != kind) continue;  // 0x004d0440
            if (static_cast<std::int32_t>(joan::orig::arg::as<std::int8_t>(joan::orig::as_sound3d(jo::as_stream_slot(slot)->source)->state())) == 4) continue;  // 0x0059fc90
            return true;
        }
        return false;
    }

    void* zero_3b() {  // 0x004cea10
        memset_(self(), 0, static_cast<u32>(sizeof(jo::UnitStats)));  // 0x3b (UnitStats::construct)
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Slots::has_kind", 0x004cfee0, &H::has_kind);
M("Obj::zero_3b", 0x004cea10, &H::zero_3b);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

using joan::call::thiscall;

struct H {
    // 0x004cea40 StreamSlot::release
    void slot_release() {
        if (jo::view<void*>(jo::self<jo::StreamSlot>(this)->source) != nullptr) {
            joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::StreamSlot>(this)->source))->stop();                                      // 0x0059f680
            const std::uint32_t v = joan::orig::arg::as<std::uint32_t>(joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::StreamSlot>(this)->source))->queued_59fd60());  // 0x0059fd60
            joan::orig::as_sound3d(jo::view<void*>(jo::self<jo::StreamSlot>(this)->source))->unqueue(joan::orig::arg::i32(v));                                   // 0x0059f4b0
        }
        jo::view<std::int32_t>(jo::self<jo::StreamSlot>(this)->key) = 0;
        jo::self<jo::StreamSlot>(this)->id = -1;
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("StreamSlot::release", 0x004cea40, &H::slot_release);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
namespace {
namespace game_b95_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_134() {  // 0x004ceab0
        memset_(this, 0, static_cast<u32>(sizeof(jo::Sounds)));  // 0x134 on x86 (Struct134::zero)
        if (this == joan::img::obj(d::g_sfx_player)) memset_(joan::sound_slots::entry(this, joan::sound_slots::kOriginal + 1), 0, static_cast<u32>((joan::sound_slots::kFixed - joan::sound_slots::kOriginal) * sizeof(jo::StreamSlot)));  // ours too
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("Struct134::zero", 0x004ceab0, &H::zero_134);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004ceaf0
    u32 call_4ceaf0() { return joan::orig::arg::as<u32>(joan::orig::as_sounds(this)->shutdown()); }  // 0x004ceb60
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Obj::call_4ceaf0", 0x004ceaf0, &H::call_4ceaf0);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004ceb20 / 0x004de9c0
    u32 reset_4ceb20() {
        joan::orig::as_sounds(this)->alloc_sources();                                                      // 0x004cf900
        joan::orig::as_party(this)->set_scale(static_cast<i32>(global<i16>(d::g_sfx_volume)));  // 0x0045f6b0
        return 0;  // 0x0045f6b0 returns nothing; the caller (game.cpp run) ignores the result
    }
};

JOAN_REPLACE("game/b108_07", "Obj::reset_4ceb20", 0x004ceb20, &H::reset_4ceb20);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    u32 reset_4ceb60() {
        joan::orig::as_sounds(this)->stop_all(0u);         // 0x004ceb90
        return joan::orig::arg::as<u32>(joan::orig::as_sound_list(this)->free_all());     // 0x00491770
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("X::reset_4ceb60", 0x004ceb60, &H::reset_4ceb60);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_4ceb90, "Panels::reset_4ceb90");

struct H {
    // 0x004ceb90
    void panels_reset(u32 flag) {
        c_4ceb90.hit();
        jo::Sound3D* jo::Panels::* const sources[4] = {&jo::Panels::source_a, &jo::Panels::source_b, &jo::Panels::source_c, &jo::Panels::source_d};
        for (i32 n = 0; n < 4; ++n) {
            jo::Sound3D* jo::Panels::* const m = sources[n];
            if (jo::self<jo::Panels>(this)->*m == nullptr) continue;
            joan::orig::as_sound3d(jo::self<jo::Panels>(this)->*m)->stop();                     // 0x0059f680
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_sound3d(jo::self<jo::Panels>(this)->*m)->queued_59fd60());  // 0x0059fd60
            joan::orig::as_sound3d(jo::self<jo::Panels>(this)->*m)->unqueue(joan::orig::arg::i32(v));                  // 0x0059f4b0
        }
        for (i32 i = 1; i <= joan::sound_slots::count(); ++i) {
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(static_cast<i16>(i))));  // 0x004cf690
            if ((flag & 0xff) == 0) joan::orig::as_stream_slot(slot)->release();                         // 0x004cea40
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Panels::reset_4ceb90", 0x004ceb90, &H::panels_reset);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::call::thiscall;

JOAN_COUNTER(c_trigger_slot, "Scenario::trigger_slot");

struct H {
    // ---- scenario --------------------------------------------------------------------------
    void trigger_slot(std::int32_t key) {  // 0x004cecd0
        c_trigger_slot.hit();
        if (key == 0) return;
        for (std::int16_t i = 1; i <= joan::sound_slots::count(); i = static_cast<std::int16_t>(i + 1)) {
            std::uint8_t* const slot = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(i)));  // 0x004cf690
            if (jo::as_stream_slot(slot)->source == nullptr) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound_slot_entry(slot)->id()) != key) continue;               // 0x004d0440
            if (joan::orig::arg::as<std::int8_t>(joan::orig::as_sound3d(jo::as_stream_slot(slot)->source)->state()) == 4) continue;  // 0x0059fc90
            joan::orig::as_stream_slot(slot)->release();                                                  // 0x004cea40
            return;
        }
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Scenario::trigger_slot", 0x004cecd0, &H::trigger_slot);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::call::thiscall;

struct H {
    void sound_pause(std::int32_t id) {                                                  // 0x004ced90
        if (id == 0) return;
        for (std::int16_t w = 1; w <= joan::sound_slots::count(); w = static_cast<std::int16_t>(w + 1)) {
            void** const e = joan::orig::arg::as<void**>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(w)));
            if (*e == nullptr) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sound_slot_entry(e)->id()) != id) continue;
            if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_sound3d(*e)->state())) != 3) continue;
            joan::orig::as_sound(*e)->play();  // 0x0059f620
            return;
        }
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Sounds::pause", 0x004ced90, &H::sound_pause);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16 {

using joan::call::thiscall;

using joan::img::obj;

JOAN_COUNTER(c_play_def, "Sound::play_def");

struct H {
    // ------------------------------------------------------------------ 0x004ceef0
    std::int32_t play_def(const char* name) {
        c_play_def.hit();
        std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(obj(d::g_sound_def_table))->find_select(name));  // 0x004d0d80
        if (global<void*>(d::g_sound3d_listener) == nullptr) return 0;
        if (def == nullptr) return 0;
        void* const buf = joan::orig::arg::as<void*>(joan::orig::as_wave_res(def)->buffer());  // 0x004d09c0
        if (buf == nullptr) return 0;
        const std::int16_t n = joan::orig::arg::as<std::int16_t>(joan::orig::as_party(this)->first_free());  // 0x0045f450
        if (n <= 0) return 0;
        void* const s = joan::orig::arg::as<void*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(n)));  // 0x004cf690
        joan::orig::as_sound_slot_entry(s)->set_key(0u);                            // 0x004d06b0
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->queue(joan::orig::arg::mptr(buf));            // 0x0059f460
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->set_position(joan::orig::arg::mptr(global<std::uint8_t*>(d::g_sound3d_listener) + 5));  // 0x0059fb40
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->set_looping(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::uint8_t>(jo::as_wave_res(def)->flags & 3))));  // 0x0059fa80
        const joan::hook::x87::Fpu fpu{true};
        const float gain = static_cast<float>(
            fpu.mul(fpu.mul(static_cast<double>(global<std::int16_t>(d::g_sfx_volume)), jo::as_wave_res(def)->volume),
                    global<float>(d::flt_0_01)));  // * 0.01
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->set_gain(joan::orig::arg::bits(gain));                         // 0x0059f700
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->set_max_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(def)->max_dist)));  // 0x0059f920
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->set_reference_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(def)->min_dist)));  // 0x0059f970
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::uint8_t>(0))));  // 0x0059fae0
        joan::orig::as_sound3d(jo::as_stream_slot(s)->source)->set_rolloff(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(def)->rolloff)));  // 0x0059f9c0
        joan::orig::as_sound_slot_entry(s)->set_id(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_wave_res(def)->index))));  // 0x004d0550
        joan::orig::as_sound(jo::as_stream_slot(s)->source)->play();                                 // 0x0059f620
        return n;
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Sound::play_def", 0x004ceef0, &H::play_def);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {

using joan::call::thiscall;

using joan::img::obj;

JOAN_COUNTER(c_play_sound, "Unit::play_sound");

struct H {
    // 0x004cf0f0
    std::int32_t play_sound(std::int32_t id) {
        c_play_sound.hit();
        if (static_cast<std::int32_t>(static_cast<std::int8_t>(global<std::uint8_t>(d::g_game_mode))) == 0xc) return 0;
        if (global<void*>(d::g_sound3d_listener) == nullptr) return 0;
        if (id == 0) return 0;
        std::uint8_t* def;
        if (id > 0x4000)
            def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->at(joan::orig::arg::i16(static_cast<std::int16_t>(id))));  // 0x004d0860
        else
            def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->get(joan::orig::arg::i16(static_cast<std::int16_t>(id))));  // 0x004d0820
        void* const sample = joan::orig::arg::as<void*>(joan::orig::as_wave_res(def)->buffer());  // 0x004d09c0 (x64: a pointer)
        if (sample == nullptr) return 0;
        const std::int16_t ch = joan::orig::arg::as<std::int16_t>(joan::orig::as_party(this)->first_free());  // 0x0045f450
        if (static_cast<std::int32_t>(ch) <= 0) return 0;
        void** const slot = joan::orig::arg::as<void**>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(ch)));  // 0x004cf690
        joan::orig::as_sound3d(*slot)->queue(joan::orig::arg::mptr(sample));                              // 0x0059f460
        joan::orig::as_sound3d(*slot)->set_position(joan::orig::arg::mptr(global<std::uint8_t*>(d::g_sound3d_listener) + 5));  // 0x0059fb40
        joan::orig::as_sound3d(*slot)->set_looping(static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::uint8_t>(jo::as_wave_res(def)->flags & 3))));  // 0x0059fa80
        const joan::hook::x87::Fpu fpu{true};
        const float vol = static_cast<float>(fpu.mul(
            fpu.mul(static_cast<double>(jo::self<jo::Panels>(this)->volume), jo::as_wave_res(def)->volume), global<float>(d::flt_0_01)));
        joan::orig::as_sound3d(*slot)->set_gain(joan::orig::arg::bits(vol));                          // 0x0059f700
        joan::orig::as_sound3d(*slot)->set_max_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(def)->max_dist)));  // 0x0059f920
        joan::orig::as_sound3d(*slot)->set_reference_distance(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(def)->min_dist)));  // 0x0059f970
        joan::orig::as_sound3d(*slot)->set_relative(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));                             // 0x0059fae0
        joan::orig::as_sound3d(*slot)->set_rolloff(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_wave_res(def)->rolloff)));  // 0x0059f9c0
        joan::orig::as_sound_slot_entry(slot)->set_id(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_wave_res(def)->index))));  // 0x004d0550
        joan::orig::as_sound_slot_entry(slot)->set_key(0u);                              // 0x004d06b0
        joan::orig::as_sound(*slot)->play();                                // 0x0059f620
        return ch;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Unit::play_sound", 0x004cf0f0, &H::play_sound);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
namespace {
namespace game_b102_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

using joan::hook::x87::Fpu;

// ---------------------------------------------------------------------------- 0x004d0b10
static const char* JOAN_CDECL format_37(void* self, i32 /*unused*/ a, ...) {
    va_list args;
    va_start(args, a);
    void* const text = joan::orig::arg::as<void*>(joan::orig::strings::format(joan::orig::arg::str(jo::as_wave_res(self)->file), joan::orig::arg::mptr(static_cast<void*>(args))));  // 0x005a31e0
    va_end(args);
    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(obj(d::g_a3a14c))), joan::orig::arg::str(text));  // strcpy
    return str(d::g_a3a14c);
}

struct H {
    // ------------------------------------------------------------------ 0x004cf290
    void set_volume_pct(i32 v) {
        if (global<void*>(d::g_sound3d_listener) == nullptr) return;
        const float s = static_cast<float>(Fpu{true}.mul(static_cast<double>(v), static_cast<double>(global<float>(d::flt_0_01))));
        joan::orig::as_sound3dlistener(global<void*>(d::g_sound3d_listener))->set_gain(joan::orig::arg::f32(s));  // 0x0059feb0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("Sound::set_volume_pct", 0x004cf290, &H::set_volume_pct);
B("Text::format_37", 0x004d0b10, &format_37);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
namespace {
namespace game_b101_06 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

struct H {
    // ------------------------------------------------------------------------ 0x004cf440
    i32 list_get(i32 n) {
        return joan::orig::arg::as<i32>(joan::orig::as_direct_music(obj(d::g_a3ce50))->set_volume(joan::orig::arg::u32(static_cast<i32>((static_cast<u32>(n) - 0x64u) * 0x60u)), 0u));  // 0x00583670
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("Obj::list_get", 0x004cf440, &H::list_get);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {


struct H {
    std::int32_t entry_id() { return jo::self<jo::StreamSlot>(this)->id; }   // 0x004d0440
    // 0x004cf690: movsx of the word argument.
    void* entry(std::int16_t i) { return joan::sound_slots::entry(this, i); }  // 1-based; above 12: ours (game/sound_slots.hpp)
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("SoundEntry::id", 0x004d0440, &H::entry_id);
L("SoundList::entry", 0x004cf690, &H::entry);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    u32 can_save() {  // 0x004cf8c0
        if (global<u8>(d::g_game_running) == 0) return 0;
        if (global<void*>(d::g_sound3d_listener) == nullptr) return 0;
        if (global<i8>(d::g_a50098) != 0) return 0;
        return 1;
    }
    u32 virtual_4_1_1() {  // 0x004d0ae0
        const std::uintptr_t fn = (*reinterpret_cast<std::uintptr_t**>(this))[1];
        return cdecl_call<u32>(fn, this, 1, 1);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("can_save", 0x004cf8c0, &H::can_save);
B("Obj::virtual_4_1_1", 0x004d0ae0, &H::virtual_4_1_1);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

JOAN_COUNTER(c_refresh, "Cache::refresh_4cf900");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x004cf900
    void refresh_4cf900() {
        c_refresh.hit();
        void* const src = obj(d::g_sound3d_system);
        jo::self<jo::Panels>(this)->source_a = joan::orig::arg::as<jo::Sound3D*>(joan::orig::as_sound3d(src)->alloc_source());  // 0x004907e0
        jo::self<jo::Panels>(this)->source_b = joan::orig::arg::as<jo::Sound3D*>(joan::orig::as_sound3d(src)->alloc_source());
        jo::self<jo::Panels>(this)->source_c = joan::orig::arg::as<jo::Sound3D*>(joan::orig::as_sound3d(src)->alloc_source());
        jo::self<jo::Panels>(this)->source_d = joan::orig::arg::as<jo::Sound3D*>(joan::orig::as_sound3d(src)->alloc_source());
        for (i16 i = 1; i <= joan::sound_slots::count(); i = static_cast<i16>(i + 1)) {
            void** const slot = joan::orig::arg::as<void**>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(i)));  // 0x004cf690
            *slot = joan::orig::arg::as<void*>(joan::orig::as_sound3d(src)->alloc_source());  // x64: the whole source pointer
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Cache::refresh_4cf900", 0x004cf900, &H::refresh_4cf900);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_has_4cffa0, "Slots::has_4cffa0");

struct H {
    void* me() { return this; }

    // ------------------------------------------------------------------ 0x004cffa0
    u8 has_4cffa0(u32 key) {
        c_has_4cffa0.hit();
        if (key == 0) return 0;
        for (i16 i = 1; i <= joan::sound_slots::count(); i = static_cast<i16>(i + 1)) {
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(me())->entry(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x004cf690
            if (jo::as_stream_slot(slot)->source == nullptr) continue;
            if (joan::orig::arg::as<u32>(joan::orig::as_sound_slot_entry(slot)->key()) != key) continue;          // 0x004d0610
            if (joan::orig::arg::as<i8>(joan::orig::as_sound3d(jo::as_stream_slot(slot)->source)->state()) != 4) return 1;  // 0x0059fc90
        }
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Slots::has_4cffa0", 0x004cffa0, &H::has_4cffa0);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_any_slot, "Obj::any_slot_at_4d00e0");

struct H {
    // ------------------------------------------------------------------ 0x004d00e0
    u8 any_slot_at(Vec3 pos) {
        c_any_slot.hit();
        for (i16 i = 1; i <= joan::sound_slots::count(); i = static_cast<i16>(i + 1)) {
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_sound_list(this)->entry(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x004cf690
            if (jo::as_stream_slot(slot)->source == nullptr) continue;
            Vec3 tmp;
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_sound_slot(slot)->position(joan::orig::arg::mptr(static_cast<void*>(&tmp))));  // 0x004384a0
            if ((joan::orig::arg::as<u32>(joan::orig::vec_equal(joan::orig::arg::ptr(p), static_cast<void*>(&pos))) & 0xffu) == 0) continue;  // 0x0060ced0
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_sound3d(jo::as_stream_slot(slot)->source)->state())) != 4) return 1;  // 0x0059fc90
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Obj::any_slot_at_4d00e0", 0x004d00e0, &H::any_slot_at);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

// Virtual call through vtable slot `index` (thiscall).
template <class R = void, class... A>
R vcall(void* o, std::int32_t index, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(o)[index], sizeof(void*));
    return (reinterpret_cast<Obj*>(o)->*method)(args...);
}

struct H {
    // ------------------------------------------------------------------ 0x004d09c0
    joan::orig::Sound3DBuffer* get_8b() {
        if (jo::self<jo::WaveRes>(this)->buffer_ == nullptr) vcall(this, 0);
        return jo::self<jo::WaveRes>(this)->buffer_;
    }

    H* set_8(std::uint32_t v) {  // 0x004d06b0
        jo::self<jo::StreamSlot>(this)->key = v;
        return this;
    }
    H* set_4(std::uint32_t v) {  // 0x004d0550
        jo::view<std::uint32_t>(jo::self<jo::StreamSlot>(this)->id) = v;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Obj::get_8b", 0x004d09c0, &H::get_8b);
B("Obj::set_8", 0x004d06b0, &H::set_8);
B("Obj::set_4", 0x004d0550, &H::set_4);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004d0610
    u32 get_8() { return jo::self<jo::StreamSlot>(this)->key; }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Obj::get_8", 0x004d0610, &H::get_8);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x004d0770
    u8* set_c(u32 a, u32 b, u32 c) {
        u32* const p = jo::self<jo::StreamSlot>(this)->pos;
        p[0] = a;
        p[1] = b;
        p[2] = c;
        return reinterpret_cast<u8*>(p);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::set_c", 0x004d0770, &H::set_c);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

// One element of the SOUND table: a WaveRes without the dialogue table's priority (0x8f bytes on x86).
constexpr std::int32_t kSoundEntry = static_cast<std::int32_t>(offsetof(jo::WaveRes, priority));

struct H {
    // ------------------------------------------------------------------ 0x004d0820
    void* name_get(std::int16_t i) { return jo::view<std::uint8_t*>(jo::self<jo::SortedNames>(this)->items) + static_cast<std::int32_t>(i) * kSoundEntry - kSoundEntry; }  // 0x8f-byte entries on x86 (WaveRes without priority)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("SortedNames::get", 0x004d0820, &H::name_get);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

using joan::call::thiscall;

struct H {
    // ---- 0x004d0860 Defs::at(id) ret 4 -----------------------------------------------------
    std::uint8_t* defs_at(std::int16_t id) {
        const std::uint32_t index = static_cast<std::uint32_t>(static_cast<std::int32_t>(id) - 0x4001);
        return static_cast<std::uint8_t*>(static_cast<void*>(jo::self<jo::NamedTable>(this)->table)) + index * static_cast<std::uint32_t>(sizeof(jo::WaveRes));  // 0x93 on x86; x64: the vtable and buffer pointer widen it
    }

    // ---- 0x004d0990 -------------------------------------------------------------------------
    void release_if() {
        if (jo::self<jo::WaveRes>(this)->buffer_ != nullptr) joan::orig::as_wave_res(this)->release();  // 0x004d0a00
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Defs::at", 0x004d0860, &H::defs_at);
B("Obj::release_if", 0x004d0990, &H::release_if);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x004d08f0
    void* ctor_8f() {
        jo::self<jo::WaveRes>(this)->vtable = obj(d::vt_WaveRes);
        memset_(&jo::self<jo::WaveRes>(this)->index, 0, static_cast<u32>(offsetof(jo::WaveRes, priority) - offsetof(jo::WaveRes, index)));  // +0x4 .. +0x8f (index .. buffer_; 0x8b on x86)
        return this;
    }

    // ------------------------------------------------------------------------ small ones
    void dtor_empty() {}                     // 0x004d0970
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("Obj8f::ctor", 0x004d08f0, &H::ctor_8f);
B94("Obj::dtor_empty", 0x004d0970, &H::dtor_empty);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

using joan::call::thiscall;

struct H {
    // 0x004d0940
    void dtor_4d0940() {
        jo::self<jo::WaveRes>(this)->vtable = obj(d::vt_WaveRes);
        joan::orig::as_wave_res(this)->release_if();  // 0x004d0990
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj4d0::~Obj4d0", 0x004d0940, &H::dtor_4d0940);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b87_01.cpp (game/b87_01)
namespace {
namespace game_b87_01 {

using joan::call::thiscall;

using joan::img::obj;

struct H {
    // ---- 0x004d0a00 Obj::release() ------------------------------------------------------------
    void* release() {  // eax = this on both paths
        if (jo::self<jo::WaveRes>(this)->buffer_ == nullptr) return this;
        joan::orig::as_sound3d(obj(d::g_sound3d_system))->release_buffer(jo::self<jo::WaveRes>(this)->buffer_);  // 0x00490bc0
        jo::self<jo::WaveRes>(this)->buffer_ = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_01", name, addr, fn)
B("Obj::release", 0x004d0a00, &H::release);
#undef B

}  // namespace game_b87_01
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}

struct H {
    // ------------------------------------------------------------------ 0x004d0a60
    std::uintptr_t wave_open() {
        void* const r = joan::orig::as_sound3d(obj(d::g_sound3d_system))->create_buffer_48d120();  // 0x0048d120
        jo::self<jo::WaveRes>(this)->buffer_ = static_cast<jo::Sound3DBuffer*>(r);
        if (jo::self<jo::WaveRes>(this)->buffer_ == nullptr) return joan::orig::arg::as<std::uintptr_t>(r);
        char* const name = vcall<char*>(this, 8);
        char* const up = joan::orig::arg::as<char*>(joan::orig::libc::_strupr(joan::orig::arg::str(name)));  // _strupr
        return joan::orig::arg::as<u32>(joan::orig::as_sound3dbuffer(jo::view<void*>(jo::self<jo::WaveRes>(this)->buffer_))->load(str(d::s_RESOURCE_WAVE_RES), up, 1u));  // 0x0059ef00
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("WaveRes::open", 0x004d0a60, &H::wave_open);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::UnitStats::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::zero_3b>(this)); }  // 0x4cea10
auto joan::orig::StreamSlot::release() -> void { joan::bind::member<&::game_leaves81::H::slot_release>(this); }  // 0x4cea40
auto joan::orig::Sounds::zero() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_03::H::zero_134>(this)); }  // 0x4ceab0
auto joan::orig::Sounds::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_11::H::call_4ceaf0>(this)); }  // 0x4ceaf0
auto joan::orig::Sounds::shutdown() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::reset_4ceb60>(this)); }  // 0x4ceb60
auto joan::orig::Sounds::stop_all(std::uint32_t a0) -> void { joan::bind::member<&::game_b108_09::H::panels_reset>(this, a0); }  // 0x4ceb90
auto joan::orig::Scenario::trigger_slot(std::int32_t a0) -> void { joan::bind::member<&::game_b85_06::H::trigger_slot>(this, a0); }  // 0x4cecd0
auto joan::orig::Sounds::pause(std::int32_t a0) -> void { joan::bind::member<&::game_world::H::sound_pause>(this, a0); }  // 0x4ced90
auto joan::orig::Sound::play_def(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_16::H::play_def>(this, a0)); }  // 0x4ceef0
auto joan::orig::Unit::play_sound(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_03::H::play_sound>(this, a0)); }  // 0x4cf0f0
auto joan::orig::SoundList::entry(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves80a::H::entry>(this, a0)); }  // 0x4cf690
auto joan::orig::Sounds::alloc_sources() -> void { joan::bind::member<&::game_b108_08::H::refresh_4cf900>(this); }  // 0x4cf900
auto joan::orig::Sounds::has_kind(std::int32_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b89_06::H::has_kind>(this, a0)); }  // 0x4cfee0
auto joan::orig::SoundSlotEntry::id() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves80a::H::entry_id>(this)); }  // 0x4d0440
auto joan::orig::SoundSlotEntry::set_id(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::set_4>(this, a0)); }  // 0x4d0550
auto joan::orig::SoundSlotEntry::key() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_03::H::get_8>(this)); }  // 0x4d0610
auto joan::orig::SoundSlotEntry::set_key(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::set_8>(this, a0)); }  // 0x4d06b0
auto joan::orig::SoundSlotEntry::set_position(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_05::H::set_c>(this, a0, a1, a2)); }  // 0x4d0770
auto joan::orig::SortedNames::get(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_03::H::name_get>(this, a0)); }  // 0x4d0820
auto joan::orig::SoundResTable::at(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_06::H::defs_at>(this, a0)); }  // 0x4d0860
auto joan::orig::WaveRes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_03::H::ctor_8f>(this)); }  // 0x4d08f0
auto joan::orig::WaveRes::destruct() -> void { joan::bind::member<&::game_b88_06::H::dtor_4d0940>(this); }  // 0x4d0940
auto joan::orig::WaveRes::nop_4d0970() -> void { joan::bind::member<&::game_b94_03::H::dtor_empty>(this); }  // 0x4d0970
auto joan::orig::WaveRes::release_if() -> void { joan::bind::member<&::game_b86_06::H::release_if>(this); }  // 0x4d0990
auto joan::orig::WaveRes::buffer() -> Sound3DBuffer* { return joan::bind::conv<Sound3DBuffer*>(joan::bind::member<&::game_b83_07::H::get_8b>(this)); }  // 0x4d09c0
auto joan::orig::WaveRes::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b87_01::H::release>(this)); }  // 0x4d0a00
