// ilib/se.cpp: functions of the original d:\ww\ilib\se.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstdarg>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x00490d30 Se::build_tables             (thiscall, ret 0)
namespace {
namespace game_b96_02 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;

JOAN_COUNTER(c_se_build, "Se::build_tables");

using joan::orig::helpers::line_of;
void* mem_alloc(i32 n, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}

struct H {

    // ------------------------------------------------------------------------ 0x00490d30
    void se_build() {
        c_se_build.hit();
        jo::view<u32>(jo::self<jo::Se>(this)->count_main) = jo::view<u32>(jo::self<jo::Se>(this)->archive.rec_count);
        const i32 n1 = jo::self<jo::Se>(this)->count_main;
        jo::view<u32>(jo::self<jo::Se>(this)->count_ext) = jo::view<u32>(jo::self<jo::Se>(this)->ext_archive->rec_count);
        const i32 n2 = jo::self<jo::Se>(this)->count_ext;
        jo::self<jo::Se>(this)->count = static_cast<i32>(jo::view<u32>(jo::self<jo::Se>(this)->count_main) + jo::view<u32>(jo::self<jo::Se>(this)->count_ext));
        {
            const i32 line = line_of(d::g_a16394, 5);
            jo::view<void*>(jo::self<jo::Se>(this)->entries) = mem_alloc(jo::self<jo::Se>(this)->count * static_cast<i32>(sizeof(jo::Top4)), d::s_d_ww_ilib_se_cpp, line);  // "se.cpp" (0x28 on x86)
        }
        {
            const i32 line = line_of(d::g_a16394, 6);
            jo::view<void*>(jo::self<jo::Se>(this)->playing) = mem_alloc(jo::self<jo::Se>(this)->count, d::s_d_ww_ilib_se_cpp, line);
        }
        i16 i = 0;
        for (; static_cast<i32>(i) < n1; i = static_cast<i16>(i + 1)) {
            jo::self<jo::Se>(this)->entries[static_cast<i32>(i)].rec_no = static_cast<u16>(i + 1);
            void* const v = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(&jo::self<jo::Se>(this)->archive)->read_rec(joan::orig::arg::i32(static_cast<i32>(i) + 1)));  // 0x0059bf70
            jo::self<jo::Se>(this)->entries[static_cast<i32>(i)].sample = v;
            jo::self<jo::Se>(this)->playing[static_cast<i32>(i)] = 0;
        }
        for (i16 j = 0; static_cast<i32>(j) < n2; j = static_cast<i16>(j + 1), i = static_cast<i16>(i + 1)) {
            jo::self<jo::Se>(this)->entries[static_cast<i32>(i)].rec_no = static_cast<u16>(j + 1);
            jo::self<jo::Se>(this)->entries[static_cast<i32>(i)].sample = nullptr;
            jo::self<jo::Se>(this)->playing[static_cast<i32>(i)] = 0;
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Se::build_tables", 0x00490d30, &H::se_build);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x0059e0c0 Se::free             (thiscall, ret 0)
namespace {
namespace game_b92_04 {

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {

    // ------------------------------------------------------------------------ 0x0059e0c0
    void se_free() {
        if (jo::self<jo::Se>(this)->loaded != 0) {
            jo::self<jo::Se>(this)->loaded = 0;
            if (jo::self<jo::Se>(this)->enabled != 0) {
                mem_free(jo::view<void*>(jo::self<jo::Se>(this)->entries), str(d::s_d_ww_ilib_se_cpp), static_cast<i32>(global<i16>(d::g_a16390)) + 6);  // se.cpp
                mem_free(jo::view<void*>(jo::self<jo::Se>(this)->playing), str(d::s_d_ww_ilib_se_cpp), static_cast<i32>(global<i16>(d::g_a16390)) + 7);
            }
        }
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Se::free", 0x0059e0c0, &H::se_free);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
//
//    - Archive::~holder 0x0059dfa0 (SEH frame, normal path): 0x0059e0c0, 0x00417a30 on +0x68.
namespace {
namespace game_b97_05 {

using joan::call::thiscall;

struct H {

    // 0x0059dfa0 (SEH frame: normal path)
    void holder_dtor() {
        joan::orig::as_sound_effects(this)->free();          // 0x0059e0c0
        joan::orig::as_archive(&jo::self<jo::Se>(this)->archive)->dtor();  // 0x00417a30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("Archive::holder_dtor", 0x0059dfa0, &H::holder_dtor);
#undef B

}  // namespace game_b97_05
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
namespace {
namespace game_b100_10 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_each, "Array28::each_59ded0");

struct H {

    // ------------------------------------------------------------------------ 0x0059e220
    void each_59ded0() {
        c_each.hit();
        for (i32 i = 0; i < jo::self<jo::Se>(this)->count; ++i) {
            joan::orig::as_top4_list(&jo::self<jo::Se>(this)->entries[i])->clear();  // 0x0059ded0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Array28::each_59ded0", 0x0059e220, &H::each_59ded0);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::hook::global;
using i8 = std::int8_t;

using joan::orig::helpers::addr;

JOAN_COUNTER(c_fx_trigger, "Effects::trigger");

struct H {

    // ------------------------------------------------------------------ 0x0059e400 (dead code)
    std::uintptr_t fx_trigger(i32 i, i32 b, u32 c) {
        c_fx_trigger.hit();
        if (jo::view<i32>(jo::self<jo::Se>(this)->enabled) == 0 || global<i8>(d::g_a57ff8) == 0) return addr(this);
        if (b < 10) return addr(this);
        if (i == 0) return addr(this);
        jo::Top4* const e = &jo::self<jo::Se>(this)->entries[i - 1];
        joan::orig::as_top4_list(e)->insert(joan::orig::arg::i32(b), joan::orig::arg::u32(c));  // 0x0059dd30
        return 0;  // no callers; Top4::insert returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("Effects::trigger", 0x0059e400, &H::fx_trigger);
#undef B

}  // namespace game_b103_10
}  // namespace

// ---------------------------------------- from modules/game_b102_09.cpp (game/b102_09)
namespace {
namespace game_b102_09 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::hook::global;
using i8 = std::int8_t;

JOAN_COUNTER(c_fx_trigger, "Effects::trigger");

struct H {

    // ------------------------------------------------------------------------ 0x0059e460
    std::uintptr_t fx_trigger(const char* a, i32 b, u32 c) {
        c_fx_trigger.hit();
        if (jo::self<jo::Se>(this)->enabled == 0 || static_cast<i8>(global<u8>(d::g_a57ff8)) == 0)
            return reinterpret_cast<std::uintptr_t>(this);
        const i32 k = joan::orig::arg::as<i32>(joan::orig::as_sound_effects(this)->find(joan::orig::arg::str(a)));  // 0x00457e00
        if (b < 10 || k == 0) return static_cast<u32>(k);
        jo::Top4* const e = &jo::self<jo::Se>(this)->entries[k - 1];
        joan::orig::as_top4_list(e)->insert(joan::orig::arg::i32(b), joan::orig::arg::u32(c));  // 0x0059dd30
        return 0;  // no callers; Top4::insert returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_09", name, addr, fn)
B("Effects::trigger", 0x0059e460, &H::fx_trigger);
#undef B

}  // namespace game_b102_09
}  // namespace

// ---------------------------------------- from modules/game_b104_09.cpp (game/b104_09)
namespace {
namespace game_b104_09 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

struct H {

    // 0x0059e550
    const void* dispatch_59e550(i32 n) {  // the index record (a pointer)
        if (n > jo::self<jo::Se>(this)->count_main) return joan::orig::arg::as<const void*>(joan::orig::as_res_idx_l(jo::view<void*>(jo::self<jo::Se>(this)->ext_archive))->get_index_rec(joan::orig::arg::i32(n - jo::self<jo::Se>(this)->count_main)));  // 0x0059c3b0
        return joan::orig::arg::as<const void*>(joan::orig::as_res_idx_l(&jo::self<jo::Se>(this)->archive)->get_index_rec(joan::orig::arg::i32(n)));  // 0x0059c3b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_09", name, addr, fn)
B("Obj::dispatch_59e550", 0x0059e550, &H::dispatch_59e550);
#undef B

}  // namespace game_b104_09
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::hook::global;
using i8 = std::int8_t;

using i16 = std::int16_t;

struct S8 {  // 8-byte by-value object (copy ctor 0x005a81a0)
    u32 a, b;
};

JOAN_COUNTER(c_sound, "Sound::play_59ebd0");

struct H {
    // -------------------------------------------------------------- 0x0059ebd0
    u32 sound_play(const char* key, S8 value) {
        c_sound.hit();
        if (global<i8>(d::g_a57ff8) == 0) return 0;
        const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_sound_effects(this)->find(joan::orig::arg::str(key)));  // 0x00457e00
        if (idx == 0) return 0;
        const jo::Top4* const e = &jo::self<jo::Se>(this)->entries[idx - 1];
        if (e->sample != nullptr) {
            S8 copy{0, 0};
            joan::orig::as_volume(&copy)->ctor(&value);  // 0x005a81a0
            return joan::orig::as_audio(jo::self<jo::Se>(this)->audio)->play_memory(joan::orig::arg::mptr(e->sample), joan::orig::arg::u32(copy.a), joan::orig::arg::u32(copy.b));  // 0x0057b580 (S8 by value = two dwords)
        }
        S8 copy{0, 0};
        joan::orig::as_volume(&copy)->ctor(&value);  // 0x005a81a0
        return joan::orig::as_audio(jo::self<jo::Se>(this)->audio)->play_resource(joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(e->rec_no))), joan::orig::arg::i32(copy.a), joan::orig::arg::i32(copy.b));  // 0x0057b0d0 (S8 by value = two dwords)
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Sound::play_59ebd0", 0x0059ebd0, &H::sound_play);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

struct H {
    // 0x0059ec90
    void* self_59ec90() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Obj::self_59ec90", 0x0059ec90, &H::self_59ec90);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SoundEffects::build_tables() -> void { joan::bind::member<&::game_b96_02::H::se_build>(this); }  // 0x490d30
auto joan::orig::SoundEffects::free() -> void { joan::bind::member<&::game_b92_04::H::se_free>(this); }  // 0x59e0c0
auto joan::orig::SoundEffects::clear_recent() -> void { joan::bind::member<&::game_b100_10::H::each_59ded0>(this); }  // 0x59e220
auto joan::orig::Top4Entry::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_05::H::self_59ec90>(this)); }  // 0x59ec90
