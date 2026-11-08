// game/unknown_soundres.cpp: functions whose original source file is unknown (formerly game/_unplaced/soundres.cpp;
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

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/records.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;
// One element of the SOUND table (SortedNames 0xa59a72): a WaveRes without the dialogue table's trailing priority
// (0x8f bytes on x86; the vtable and the buffer pointer make it 0x97 on x64).
constexpr u32 kSoundEntry = static_cast<u32>(offsetof(jo::WaveRes, priority));

using joan::call::stdcall_call;

using joan::img::str;

using joan::img::obj;

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_6() {  // 0x004d0b90
        memset_(this, 0, static_cast<u32>(sizeof(jo::SortedNames)));  // 6 on x86 (the SortedNames 0xa59a72)
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Struct6::zero", 0x004d0b90, &H::zero_6);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {

struct H {
    // ------------------------------------------------------------------ 0x004d0bc0 / 0x004d15f0
    u32 call_4d0bc0() { joan::orig::as_sorted_names(this)->clear(); return 0; }  // 0x004d0c20 (returns nothing; the only caller is an atexit stub)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::call_4d0bc0", 0x004d0bc0, &H::call_4d0bc0);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    u32 fwd_4d0bf0() { return joan::orig::arg::as<u32>(joan::orig::as_sorted_names(this)->load()); }  // 0x004d0de0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::fwd_4d0bf0", 0x004d0bf0, &H::fwd_4d0bf0);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {

JOAN_COUNTER(c_array_clear, "Array::clear");

struct H {
    // ------------------------------------------------------------------------ 0x004d0c20
    void array_clear() {
        c_array_clear.hit();
        if (jo::self<jo::SortedNames>(this)->items == nullptr) return;
        for (i16 i = 1; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::SortedNames>(this)->count); i = static_cast<i16>(i + 1)) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_sorted_names(this)->get(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x004d0820
            joan::orig::as_wave_res(e)->release_if();                                                 // 0x004d0990
        }
        void* const p = jo::view<void*>(jo::self<jo::SortedNames>(this)->items);
        if (p != nullptr) joan::orig::as_wave_res(p)->vector_destroy(3u);  // 0x004d0ce0 (deleting dtor)
        jo::self<jo::SortedNames>(this)->items = nullptr;
        jo::self<jo::SortedNames>(this)->count = 0;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Array::clear", 0x004d0c20, &H::array_clear);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

JOAN_COUNTER(c_vec_delete, "Obj8f::vector_delete");

struct H {
    // ------------------------------------------------------------------------ 0x004d0ce0
    void* vector_delete(u32 flags) {
        c_vec_delete.hit();
        if ((flags & 2) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), kSoundEntry, joan::orig::arg::i32(reinterpret_cast<const i32*>(this)[-1]), joan::orig::arg::as<std::uintptr_t>(fn::code_ptr(fn::Obj4d0_dtor)));  // eh vector destructor iterator, dtor 0x004d0940
            void* const block = reinterpret_cast<u8*>(this) - 4;
            if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(block));  // operator delete
            return block;
        }
        joan::orig::as_wave_res(this)->destruct();  // 0x004d0940
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("Obj8f::vector_delete", 0x004d0ce0, &H::vector_delete);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct H {
    // ------------------------------------------------------------------ 0x004d0d80
    void* find_select(const char* key) {
        const std::int16_t i = joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(this)->bsearch(key, joan::orig::arg::i16(static_cast<std::int16_t>(1)), joan::orig::arg::i16(jo::self<jo::SortedNames>(this)->count)));  // 0x004d1220
        if (!(static_cast<std::int32_t>(i) > 0)) return nullptr;
        return joan::orig::arg::as<void*>(joan::orig::as_sorted_names(this)->get(joan::orig::arg::i16(i)));  // 0x004d0820
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("List::find_select", 0x004d0d80, &H::find_select);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
namespace {
namespace game_b104_04 {

using i8 = std::int8_t;

void* const kResSets = obj(d::g_game_set);
void* const kText = obj(d::g_misc);

void* res_table(img::Addr name) { return joan::orig::arg::as<void*>(joan::orig::as_game_set(kResSets)->open_db_58b180(joan::orig::arg::str(str(name)))); }  // 0x0058b180
i32 res_count(void* t) { return joan::orig::arg::as<i32>(joan::orig::as_database(t)->rec_count()); }                                    // 0x00415280
u8* res_row(void* t, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_database(t)->read(joan::orig::arg::i32(i))); }  // 0x00580c70
void text_copy(void* dst, const void* src, i32 n) { joan::orig::as_misc(kText)->copy_field_590210(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::i32(n)); }  // 0x00590210

JOAN_COUNTER(c_4d0de0, "SoundRes::load_4d0de0");

struct H {
    // ---------------------------------------------------------------- 0x004d0de0
    double text_atof(const void* p, i32 n) { return joan::orig::arg::as<double>(joan::orig::as_misc(kText)->atof_field_5902d0(joan::orig::arg::str(p), joan::orig::arg::u32(n))); }  // 0x005902d0
    i32 text_atoi(const void* p, i32 n) { return joan::orig::arg::as<i32>(joan::orig::as_misc(kText)->atoi_field_590270(joan::orig::arg::str(p), joan::orig::arg::u32(n))); }        // 0x00590270
    u8* sound_entry(i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_sorted_names(this)->get(joan::orig::arg::i16(i))); }                          // 0x004d0820
    i32 sound_load() {
        c_4d0de0.hit();
        void* const t = res_table(d::s_SOUND);  // 'SOUND'
        jo::view<u16>(jo::self<jo::SortedNames>(this)->count) = static_cast<u16>(res_count(t));
        const i32 cnt = jo::self<jo::SortedNames>(this)->count;
        u8* const block = joan::orig::arg::as<u8*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(cnt) * kSoundEntry + 4u)));  // operator new
        u8* arr = nullptr;
        if (block != nullptr) {
            *reinterpret_cast<i32*>(block) = cnt;  // the new[] cookie: the element count
            joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 4)), kSoundEntry, joan::orig::arg::i32(cnt), joan::orig::arg::as<std::uintptr_t>(fn::code_ptr(fn::Obj8f_ctor)), joan::orig::arg::as<std::uintptr_t>(fn::code_ptr(fn::Obj4d0_dtor)));  // eh vector ctor: 0x004d08f0 / 0x004d0940
            arr = block + 4;
        }
        jo::view<u8*>(jo::self<jo::SortedNames>(this)->items) = arr;
        for (i16 i = 1; i <= jo::self<jo::SortedNames>(this)->count; i = static_cast<i16>(i + 1)) {
            u8* const row = res_row(t, i);
            u8* const e = sound_entry(i);
            text_copy(jo::as_wave_res(e)->name, row, 0x30);
            text_copy(jo::as_wave_res(e)->file, row + 0x30, 0x30);
            jo::as_wave_res(e)->max_play = static_cast<u8>(joan::orig::arg::as<i32>(joan::orig::int_max(joan::orig::arg::i32(text_atoi(row + 0x60, 2)), 1)));  // 0x004893c0 max
            jo::as_wave_res(e)->volume = static_cast<float>(text_atof(row + 0x62, 6));
            jo::as_wave_res(e)->min_pitch = static_cast<float>(text_atof(row + 0x68, 5));
            jo::as_wave_res(e)->max_pitch = static_cast<float>(text_atof(row + 0x6d, 5));
            jo::as_wave_res(e)->min_dist = static_cast<float>(text_atof(row + 0x72, 8));
            jo::as_wave_res(e)->max_dist = static_cast<float>(text_atof(row + 0x7a, 8));
            jo::as_wave_res(e)->rolloff = static_cast<float>(text_atof(row + 0x82, 7));
            jo::as_wave_res(e)->sound_time_ms = joan::hook::x87::ftol(text_atof(row + 0x89, 5));
            const u8 y1 = static_cast<i8>(row[0x8e]) == 0x59 ? 1 : 0;
            jo::as_wave_res(e)->flags = static_cast<u8>((jo::as_wave_res(e)->flags & 0xfc) | (y1 & 3));
            const u8 y2 = static_cast<i8>(row[0x8f]) == 0x59 ? 1 : 0;
            jo::as_wave_res(e)->flags = static_cast<u8>((jo::as_wave_res(e)->flags & 0xf3) | ((y2 & 3) << 2));
            jo::as_wave_res(e)->delay_begin = static_cast<u16>(text_atoi(row + 0x90, 5));
            jo::as_wave_res(e)->delay_end = static_cast<u16>(text_atoi(row + 0x95, 5));
            const i8 c = static_cast<i8>(row[0x9a]);  // text columns of the SOUND table row
            jo::as_wave_res(e)->stream = (c == 'Y' || c == 'y') ? 1 : 0;
            joan::orig::as_wave_res(e)->nop_4d0970();  // 0x004d0970
        }
        joan::orig::libc::qsort(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SortedNames>(this)->items)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::SortedNames>(this)->count)), kSoundEntry, fn::code_ptr(fn::compare_name6));  // qsort
        for (i16 i = 1; i <= jo::self<jo::SortedNames>(this)->count; i = static_cast<i16>(i + 1)) jo::as_wave_res(sound_entry(i))->index = static_cast<u16>(i);
        return jo::self<jo::SortedNames>(this)->count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("SoundRes::load_4d0de0", 0x004d0de0, &H::sound_load);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
namespace {
namespace game_b84_03 {

JOAN_COUNTER(c_bsearch, "SortedNames::bsearch");

using joan::orig::helpers::stricmp_;

struct H {
    // ------------------------------------------------------------------ 0x004d1220
    std::int16_t name_bsearch(const char* key, std::int16_t lo, std::int16_t hi) {
        c_bsearch.hit();
        if (static_cast<std::int32_t>(lo) > static_cast<std::int32_t>(hi)) return 0;
        const std::int16_t mid = static_cast<std::int16_t>((static_cast<std::int32_t>(lo) + static_cast<std::int32_t>(hi)) >> 1);
        std::uint8_t* const item = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_sorted_names(this)->get(joan::orig::arg::i16(mid)));  // 0x004d0820
        const std::int32_t c = stricmp_(key, reinterpret_cast<const char*>(item + offsetof(jo::WaveRes, name)));  // +6 on x86
        if (c == 0) return mid;
        if (c > 0)
            return joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(this)->bsearch(joan::orig::arg::str(key), joan::orig::arg::i16(static_cast<std::int16_t>(mid + 1)), joan::orig::arg::i16(hi)));  // 0x004d1220
        return joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(this)->bsearch(joan::orig::arg::str(key), joan::orig::arg::i16(lo), joan::orig::arg::i16(static_cast<std::int16_t>(mid - 1))));
    }

    // ------------------------------------------------------------------ 0x004d12f0
    std::int16_t name_find(const char* key) {
        const std::int16_t i = joan::orig::arg::as<std::int16_t>(joan::orig::as_sorted_names(this)->bsearch(joan::orig::arg::str(key), joan::orig::arg::i16(static_cast<std::int16_t>(1)), joan::orig::arg::i16(jo::self<jo::SortedNames>(this)->count)));  // 0x004d1220
        if (!(static_cast<std::int32_t>(i) > 0)) return 0;
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("SortedNames::bsearch", 0x004d1220, &H::name_bsearch);
B("SortedNames::find", 0x004d12f0, &H::name_find);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
namespace {
namespace game_b102_04 {

using joan::hook::global;

using joan::orig::helpers::addr;

JOAN_COUNTER(c_speakers_tick, "Speakers::tick");

struct H {
    // ------------------------------------------------------------------ 0x004d1340
    std::uintptr_t speakers_tick() {
        c_speakers_tick.hit();
        global<i32>(d::g_a3a254) = global<i32>(d::g_a3a254) + 1;
        std::uintptr_t eax = static_cast<u32>(global<i32>(d::g_a3a254));
        if (!(global<i32>(d::g_a3a254) > 0x190)) return eax;
        global<i32>(d::g_a3a254) = 0;
        i16 i = jo::self<jo::SortedNames>(this)->count;
        while (static_cast<i32>(i) >= 1) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->get(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(i)))));  // 0x004d0820
            eax = addr(e);
            if (jo::view<void*>(jo::as_wave_res(e)->buffer_) != nullptr) {
                const i32 w = jo::as_wave_res(e)->buffer_->refs;
                eax = static_cast<u32>(w);
                if (!(w > 0)) {
                    joan::orig::as_wave_res(e)->release();  // 0x004d0a00
                    eax = 0;  // 0x004d0a00 returns nothing; Speakers::tick has no callers
                }
            }
            i = static_cast<i16>(static_cast<u16>(i) - 1u);
            eax = (eax & 0xffff0000u) | static_cast<u16>(i);
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("Speakers::tick", 0x004d1340, &H::speakers_tick);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

// ------------------------------------------------------------------ 0x004d1400 (cdecl)
i32 JOAN_CDECL compare_name6(const u8* a, const u8* b) {
    const i32 r = joan::crt::stricmp(joan::orig::arg::str(a + offsetof(jo::WaveRes, name)), joan::orig::arg::str(b + offsetof(jo::WaveRes, name)));  // _stricmp (+6 on x86)
    if (r > 0) return 1;
    if (r < 0) return -1;
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("compare_name6", 0x004d1400, &compare_name6);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using joan::orig::helpers::strlen_;  // strlen

struct H {
    // ------------------------------------------------------------------------ 0x004d1470
    bool is_ts_prefix(const char* s) {
        const u32 n = strlen_(str(d::s_TS_9d5b38));  // "TS_"
        return joan::orig::arg::as<i32>(joan::orig::libc::_strnicmp(joan::orig::arg::str(s), str(d::s_TS_9d5b38), n)) == 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("is_ts_prefix", 0x004d1470, &H::is_ts_prefix);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x004d14c0
    std::uint8_t is_minus_99(std::int32_t v) { return v == -99 ? 1 : 0; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("is_minus_99", 0x004d14c0, &H::is_minus_99);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SortedNames::clear() -> void { joan::bind::member<&::game_b98_03::H::array_clear>(this); }  // 0x4d0c20
auto joan::orig::WaveRes::vector_destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_03::H::vector_delete>(this, a0)); }  // 0x4d0ce0
auto joan::orig::List::find_select(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::find_select>(this, a0)); }  // 0x4d0d80
auto joan::orig::SortedNames::load() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_04::H::sound_load>(this)); }  // 0x4d0de0
auto joan::orig::SortedNames::bsearch(const char* a0, std::int16_t a1, std::int16_t a2) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b84_03::H::name_bsearch>(this, a0, a1, a2)); }  // 0x4d1220
auto joan::orig::SortedNames::find(const char* a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b84_03::H::name_find>(this, a0)); }  // 0x4d12f0
auto joan::orig::SortedNames::is_ts_prefix(const char* a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b96_03::H::is_ts_prefix>(this, a0)); }  // 0x4d1470
auto joan::orig::SortedNames::is_none(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b90_02::H::is_minus_99>(this, a0)); }  // 0x4d14c0
