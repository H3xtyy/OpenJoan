// game/scenario.cpp: functions of the original d:\ww\scenario.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::cdecl_call;

void* const kMem = obj(d::g_mem);

void* mem_alloc(u32 size, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(size), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}

using i8 = std::int8_t;

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
//
//    - Scenario::reset 0x004cc850 (no args) (scenario.cpp).
//
//  * 0x004cc850 frees the scenario array [this+0x10] inside the per-entry loop (original bug):
//    reproduced literally (a second iteration reads through the null pointer like the original).
namespace {
namespace game_b84_03 {

struct Obj {};
// Virtual call through vtable slot `index` (thiscall).
template <class R = void, class... A>
R vcall(void* o, std::int32_t index, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    std::memcpy(&method, &joan::call::vtable_of(o)[index], sizeof(void*));
    return (reinterpret_cast<Obj*>(o)->*method)(args...);
}

JOAN_COUNTER(c_scen_reset, "Scenario::reset");

constexpr img::Addr kMemory = d::g_mem;

void* memset_(void* d, std::int32_t c, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }
using joan::orig::helpers::line_base;
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------ 0x004cc850
    std::uint32_t scen_reset() {
        c_scen_reset.hit();
        const img::Addr cpp = d::s_d_ww_scenario_cpp;  // "d:\ww\scenario.cpp"
        for (std::int32_t i = 0; i < jo::self<jo::Scenario>(this)->entry_count; ++i) {
            auto ent = [&]() { return &jo::self<jo::Scenario>(this)->entries[i]; };
            if (jo::as_scenario_entry(ent())->script != nullptr) {
                void* const o = jo::view<void*>(jo::as_scenario_entry(ent())->script);
                if (o != nullptr) vcall<void*>(o, 0, 1);  // scalar deleting destructor
                jo::as_scenario_entry(ent())->script = nullptr;
            }
            if (jo::as_scenario_entry(ent())->name != nullptr) {
                mem_free(jo::view<void*>(jo::as_scenario_entry(ent())->name), cpp, line_base(d::g_a02e5c) + 0xc);
                jo::as_scenario_entry(ent())->name = nullptr;
            }
            if (jo::as_scenario_entry(ent())->name2 != nullptr) {
                mem_free(jo::view<void*>(jo::as_scenario_entry(ent())->name2), cpp, line_base(d::g_a02e5c) + 0x11);
                jo::as_scenario_entry(ent())->name2 = nullptr;
            }
            mem_free(jo::view<void*>(jo::self<jo::Scenario>(this)->entries), cpp, line_base(d::g_a02e5c) + 0x15);
            jo::self<jo::Scenario>(this)->entries = nullptr;
        }
        joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->clear();  // 0x00639030
        if (jo::self<jo::Scenario>(this)->local.script != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::Scenario>(this)->local.script);
            if (o != nullptr) vcall<void*>(o, 0, 1);
            jo::self<jo::Scenario>(this)->local.script = nullptr;
        }
        if (jo::self<jo::Scenario>(this)->local.name != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Scenario>(this)->local.name), cpp, line_base(d::g_a02e5c) + 0x23);
            jo::self<jo::Scenario>(this)->local.name = nullptr;
        }
        if (jo::self<jo::Scenario>(this)->local.name2 != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Scenario>(this)->local.name2), cpp, line_base(d::g_a02e5c) + 0x29);
            jo::self<jo::Scenario>(this)->local.name2 = nullptr;
        }
        memset_(&jo::self<jo::Scenario>(this)->local, 0, sizeof(jo::ScenarioEntry));
        if (jo::self<jo::Scenario>(this)->type_count != 0) {
            for (std::int32_t j = 0; j < static_cast<std::int32_t>(jo::self<jo::Scenario>(this)->type_count); ++j) {
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::self<jo::Scenario>(this)->types[j].value)));  // 0x0042d0a0
                joan::orig::as_unit_type(m)->release_model_slot(0);  // 0x00527690
            }
            mem_free(jo::view<void*>(jo::self<jo::Scenario>(this)->types), cpp, line_base(d::g_a02e5c) + 0x37);
            jo::self<jo::Scenario>(this)->types = nullptr;
            jo::self<jo::Scenario>(this)->type_count = 0;
        }
        if (jo::self<jo::Scenario>(this)->item_count != 0) {
            for (std::int32_t j = 0; j < static_cast<std::int32_t>(jo::self<jo::Scenario>(this)->item_count); ++j) {
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::self<jo::Scenario>(this)->items[j].value)));  // 0x00425ce0
                joan::orig::as_res(m)->release_slot(0, 0);  // 0x004a3e30
            }
            mem_free(jo::view<void*>(jo::self<jo::Scenario>(this)->items), cpp, line_base(d::g_a02e5c) + 0x44);
            jo::self<jo::Scenario>(this)->items = nullptr;
            jo::self<jo::Scenario>(this)->item_count = 0;
        }
        if (jo::self<jo::Scenario>(this)->squad_count != 0) {
            for (std::int32_t j = 0; j < static_cast<std::int32_t>(jo::self<jo::Scenario>(this)->squad_count); ++j) {
                void* const o = jo::view<void*>(jo::self<jo::Scenario>(this)->squads[j].unit);
                if (o != nullptr) joan::orig::as_unit(o)->destroy(1u);  // 0x00471530
            }
            mem_free(jo::view<void*>(jo::self<jo::Scenario>(this)->squads), cpp, line_base(d::g_a02e5c) + 0x50);
            jo::self<jo::Scenario>(this)->squads = nullptr;
            jo::self<jo::Scenario>(this)->squad_count = 0;
        }
        jo::view<std::uint32_t>(jo::self<jo::Scenario>(this)->entry_count) = 0;
        jo::self<jo::Scenario>(this)->id = 0;
        jo::self<jo::Scenario>(this)->index = 0;
        jo::self<jo::Scenario>(this)->target_stage = 0;
        for (std::int32_t i = 0; i < 13; ++i) jo::self<jo::Scenario>(this)->counters[i] = 0;
        jo::self<jo::Scenario>(this)->u8_20e = 0;
        jo::self<jo::Scenario>(this)->crate_counters[0] = 0;
        jo::self<jo::Scenario>(this)->crate_counters[1] = 0;
        jo::self<jo::Scenario>(this)->crate_counters[2] = 0;
        jo::self<jo::Scenario>(this)->crate_counters[3] = 0;
        jo::self<jo::Scenario>(this)->food_can_be_picked = 0;
        jo::self<jo::Scenario>(this)->citizen_saved = 0;
        jo::self<jo::Scenario>(this)->u8_220 = 0;
        joan::orig::as_scenario(this)->clear();  // 0x004cddc0
        return 0;  // callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("Scenario::reset", 0x004cc850, &H::scen_reset);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
//
//    - Scenario::set_4ccf40 0x004ccf40 (scenario.cpp) (int v), ret 4: when v == 0 or v != [this]:
//      0x004cc850, [this] = v, 0x004ccfb0, +0x1ae = 0, 0x004b3160 on 0x00a53c28 (v) -> its eax;
//      otherwise eax = this (both reproduced; the callers' functions return eax).
namespace {
namespace game_b105_06 {

using joan::orig::helpers::addr_of;

JOAN_COUNTER(c_set_4ccf40, "Scenario::set_4ccf40");

struct H {
    // ------------------------------------------------------------------------ 0x004ccf40
    std::uintptr_t set_4ccf40(u32 v) {  // eax: this or 0x004b3160's result
        c_set_4ccf40.hit();
        if (v != 0 && v == jo::self<jo::Scenario>(this)->id) return addr_of(this);
        joan::orig::as_scenario(this)->reset();  // 0x004cc850
        jo::self<jo::Scenario>(this)->id = v;
        joan::orig::as_scenario(this)->load_4ccfb0();  // 0x004ccfb0
        jo::self<jo::Scenario>(this)->local.u32_19a = 0;
        return joan::orig::arg::as<u32>(joan::orig::as_msg_log(obj(d::g_portraits))->clear_slot(joan::orig::arg::i32(v)));  // 0x004b3160
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Scenario::set_4ccf40", 0x004ccf40, &H::set_4ccf40);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
namespace {
namespace game_b104_04 {

void* const kText = obj(d::g_misc);

void* game_memset(void* p, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
char* strcpy_(void* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_4ccfb0, "Scenario::load_4ccfb0");

struct H {
    // ---------------------------------------------------------------- 0x004ccfb0
    u32 scenario_load() {
        c_4ccfb0.hit();
        alignas(4) u8 path[(sizeof(jo::String) + 3u) & ~3u];     // 0x194
        alignas(4) u8 reader[(sizeof(jo::FileTxt) + 3u) & ~3u];  // 0x19c on x86
        joan::orig::as_string(path)->construct();                // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_SCENARIO));                // 0x005a1440 'SCENARIO\'
        joan::orig::as_string(path)->append(str(d::s_scenario));                    // 0x005a1550 'scenario_'
        if (jo::view<i32>(jo::self<jo::Scenario>(this)->id) < 10) joan::orig::as_string(path)->append(str(d::s_0));                // '0'
        joan::orig::as_string(path)->append(joan::orig::arg::i32(jo::view<i32>(jo::self<jo::Scenario>(this)->id)));  // 0x005a15d0 append number
        joan::orig::as_string(path)->append(str(d::s_SCR));                    // '.SCR'
        const i32 found = joan::orig::as_misc(kText)->is_file_exist(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str())));  // 0x00590b80
        if (found == 0) return 0;
        joan::orig::as_file_txt(reader)->construct(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str())));  // 0x00589550
        auto next = [&]() { return joan::orig::as_file_txt(reader)->get_token(1); };      // 0x00589c70
        next();
        next();
        next();
        jo::self<jo::Scenario>(this)->entry_count = atoi_(next());
        jo::view<void*>(jo::self<jo::Scenario>(this)->entries) = mem_alloc(static_cast<u32>(jo::self<jo::Scenario>(this)->entry_count * static_cast<i32>(sizeof(jo::ScenarioEntry))), d::s_d_ww_scenario_cpp,
                                   static_cast<i32>(global<i16>(d::g_a02e60)) + 0x1a);
        game_memset(jo::view<void*>(jo::self<jo::Scenario>(this)->entries), 0, static_cast<u32>(jo::self<jo::Scenario>(this)->entry_count * static_cast<i32>(sizeof(jo::ScenarioEntry))));
        static const i32 kOff[6] = {0, 0x41, 0x82, 0xc3, 0x104, 0x145};
        for (i32 i = 0; i < jo::self<jo::Scenario>(this)->entry_count; ++i) {
            for (i32 s = 0; s < 6; ++s) {
                next();
                const char* const v = next();
                strcpy_(reinterpret_cast<u8*>(jo::self<jo::Scenario>(this)->entries[i].texts) + kOff[s], v);
            }
            u8* const e = reinterpret_cast<u8*>(&jo::self<jo::Scenario>(this)->entries[i]);
            jo::as_scenario_entry(e)->u32_19a = 0;
            jo::as_scenario_entry(e)->u32_18a = 0;
            jo::as_scenario_entry(e)->u32_18e = 0;
            jo::as_scenario_entry(e)->u32_192 = 0;
            jo::as_scenario_entry(e)->u32_196 = 0;
            jo::as_scenario_entry(e)->script = nullptr;
            jo::view<u32>(jo::as_scenario_entry(e)->time_limit_ms) = 0xffffffffu;
            jo::view<u32>(jo::as_scenario_entry(e)->time_left_ms) = 0xffffffffu;
            jo::as_scenario_entry(e)->name = nullptr;
            jo::as_scenario_entry(e)->name2 = nullptr;
        }
        joan::orig::as_file_txt(reader)->destruct_589890();  // 0x00589890 ~reader
        return 0;  // callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("Scenario::load_4ccfb0", 0x004ccfb0, &H::scenario_load);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
//
//    - List::index_of 0x00416dc0 (id), ret 4; Unit::set_level 0x0045b720 (v), ret 4;
//      Unit::clear_flags 0x0050d0d0; Rec::set 0x004b9c80 (6 args), ret 0x18;
//      Unit::poke 0x00528440; Counter::step 0x005315a0; Obj::kind 0x004cd510;
//      ctors 0x00469e90 / 0x00469560 / 0x00468300 / 0x00467420 (base 0x0046d520, vtable,
//      memset +0x79); scalar deleting dtor 0x00548430; zero-ctors 0x00531140 / 0x004cea10 /
//      0x00495950; 0x0066e4b0 (0x005843d0(0x24, 0x10, 0)); List::is_empty_at 0x00485900;
//      List::at 0x00485790; Rec::at 0x004c2470; Rec::at 0x00484540; 0x0063ed40;
//      Rec::float_at 0x004c24b0; setters 0x00503fe0 / 0x00500d10 / 0x004fecd0 / 0x004feca0 /
//      0x004fec70 / 0x004fec40 / 0x004fec10 / 0x004febe0 / 0x004febb0 / 0x004feb80 /
//      0x004fe800 / 0x004fe7d0 / 0x004fe7a0 / 0x004fe770 / 0x004fe740 / 0x004fe710 /
//      0x004fe6e0 / 0x004fe670 / 0x004fe640 / 0x00503fb0 / 0x004fd630; no-op 0x00518d10.
namespace {
namespace game_b89_06 {

struct H {
    // 0x004cd510
    std::uint32_t kind() {
        if (jo::self<jo::Scenario>(this)->id != 0) {
            const std::uint32_t o = (jo::self<jo::Scenario>(this)->index - 1u) * static_cast<std::uint32_t>(sizeof(jo::ScenarioEntry));
            return reinterpret_cast<jo::ScenarioEntry*>(reinterpret_cast<std::uint8_t*>(jo::self<jo::Scenario>(this)->entries) + o)->kind;
        }
        return jo::self<jo::Scenario>(this)->local.kind;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::kind", 0x004cd510, &H::kind);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
//
//      0x004cd570 Scenario::set_1ae        (thiscall(v), ret 4)
//
//  - eax kept where cheap: 0x004c15a0 / 0x004d1b20 return this, 0x004cd570 returns v (owner
//    path) or this.
namespace {
namespace game_b95_03 {

struct H {
    // ------------------------------------------------------------------------ small ones
    std::uintptr_t set_1ae(u32 v) {  // 0x004cd570; eax: v or this
        if (jo::self<jo::Scenario>(this)->id != 0) {
            jo::self<jo::Scenario>(this)->entries[static_cast<i32>(jo::self<jo::Scenario>(this)->index - 1u)].kind = v;
            return v;
        }
        jo::self<jo::Scenario>(this)->local.kind = v;
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("Scenario::set_1ae", 0x004cd570, &H::set_1ae);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - Timer::tick 0x004cd5d0 (ret 0): counts down by [0xa500be], sets [0xa59298] = 2.
//
//    - Scenario::push_word 0x004cd960 (short), ret 4: realloc 0x0058eb30 + append.
namespace {
namespace game_b100_05 {

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_tick, "Timer::tick");
JOAN_COUNTER(c_push_word, "Scenario::push_word");

struct H {
    // ------------------------------------------------------------------ 0x004cd5d0
    void tick() {
        c_tick.hit();
        if (static_cast<i32>(global<i8>(d::g_flag_a59298)) != 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_counter(obj(d::g_scenarios))->at_least_10()) == 0) return;  // 0x0041c7e0
        if (jo::self<jo::Scenario>(this)->id != 0) {
            u8* const rec =
                reinterpret_cast<u8*>(&jo::self<jo::Scenario>(this)->entries[jo::view<i32>(jo::self<jo::Scenario>(this)->index) - 1]);
            if (jo::as_scenario_entry(rec)->time_limit_ms > 0) {
                jo::as_scenario_entry(rec)->time_left_ms = static_cast<i32>(static_cast<u32>(jo::as_scenario_entry(rec)->time_left_ms) -
                                                       global<u32>(d::g_frame_step_ms));
                if (jo::as_scenario_entry(rec)->time_left_ms <= 0) global<u8>(d::g_flag_a59298) = 2;
            }
        } else if (jo::self<jo::Scenario>(this)->local.time_limit_ms > 0) {
            jo::self<jo::Scenario>(this)->local.time_left_ms = static_cast<i32>(static_cast<u32>(jo::self<jo::Scenario>(this)->local.time_left_ms) - global<u32>(d::g_frame_step_ms));
            if (jo::self<jo::Scenario>(this)->local.time_left_ms <= 0) global<u8>(d::g_flag_a59298) = 2;
        }
        if (global<u8>(d::g_game_running) == 0) return;
        if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) != 0) return;  // 0x004301c0
        jo::self<jo::Scenario>(this)->timer_a = jo::self<jo::Scenario>(this)->timer_a + global<u32>(d::g_frame_step_ms);
        jo::self<jo::Scenario>(this)->timer_b = jo::self<jo::Scenario>(this)->timer_b + global<u32>(d::g_frame_step_ms);
        jo::self<jo::Scenario>(this)->timer_c = jo::self<jo::Scenario>(this)->timer_c + global<u32>(d::g_frame_step_ms);
    }

    // ------------------------------------------------------------------ 0x004cd960
    void push_word(i16 v) {
        c_push_word.hit();
        const i32 line = static_cast<i32>(global<i16>(d::g_a02e68)) + 1;
        const i32 bytes = static_cast<i32>(jo::self<jo::Scenario>(this)->item_count) * 2 + 2;
        jo::view<u8*>(jo::self<jo::Scenario>(this)->items) = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<u8*>(jo::self<jo::Scenario>(this)->items)), joan::orig::arg::u32(bytes), str(d::s_d_ww_scenario_cpp), joan::orig::arg::i32(line)));  // 0x0058eb30
        jo::self<jo::Scenario>(this)->items[static_cast<i32>(jo::self<jo::Scenario>(this)->item_count)].value = v;
        jo::self<jo::Scenario>(this)->item_count = static_cast<i16>(jo::self<jo::Scenario>(this)->item_count + 1);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Timer::tick", 0x004cd5d0, &H::tick);
B("Scenario::push_word", 0x004cd960, &H::push_word);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
//
//      0x004cd790 scenario_mode(name)          (cdecl) - SCENARIO_*_MODE -> 0..3 (default 3).
namespace {
namespace game_b98_03 {

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

JOAN_COUNTER(c_scenario_mode, "scenario_mode");

// 0x004cd790
i32 JOAN_CDECL scenario_mode(const char* name) {
    c_scenario_mode.hit();
    if (stricmp_(name, str(d::s_SCENARIO_NORMAL_MODE)) == 0) return 0;  // "SCENARIO_NORMAL_MODE"
    if (stricmp_(name, str(d::s_SCENARIO_INTERACT_MODE)) == 0) return 1;  // "SCENARIO_INTERACT_MODE"
    if (stricmp_(name, str(d::s_SCENARIO_TRAINING_MODE)) == 0) return 2;  // "SCENARIO_TRAINING_MODE"
    if (stricmp_(name, str(d::s_SCENARIO_BATTLE_MODE)) == 0) return 3;  // "SCENARIO_BATTLE_MODE"
    return 3;
}

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("scenario_mode", 0x004cd790, &scenario_mode);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x004cd840 Scenario::current()                 ret 0
//
//      0x004cda10 Scenario::add_copy(src)             ret 4   new 0x4b4 + 0x004155e0, realloc
//
//  - 0x004b0250 / 0x004cda10: SEH frames only guard operator new + ctor (normal path kept).
//
//  - 0x004cda10: 0x004c96f0 is called on an uninitialised local whose `this` is never used;
//    a zeroed 0x78-byte buffer is passed.
namespace {
namespace game_b92_03 {

void* const kMemory = obj(d::g_mem);

using joan::orig::helpers::game_new;  // operator new

using joan::orig::helpers::line_base;

JOAN_COUNTER(c_add_copy, "Scenario::add_copy");

struct H {
    // 0x004cd840
    void* current() {
        if (jo::view<std::int32_t>(jo::self<jo::Scenario>(this)->id) == 0) return &jo::self<jo::Scenario>(this)->local;
        if (jo::view<std::int32_t>(jo::self<jo::Scenario>(this)->index) == 0 || jo::view<std::int32_t>(jo::self<jo::Scenario>(this)->index) > jo::self<jo::Scenario>(this)->entry_count) return nullptr;
        return &jo::self<jo::Scenario>(this)->entries[jo::view<std::int32_t>(jo::self<jo::Scenario>(this)->index) - 1];
    }

    // 0x004cda10
    void add_copy(std::uint8_t* src) {
        c_add_copy.hit();
        void* const mem = game_new(sizeof(jo::Squad));  // 0x4b4 on x86
        std::uint8_t* copy = nullptr;
        if (mem != nullptr) copy = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_squad(mem)->construct());  // 0x004155e0
        const std::int32_t count = jo::self<jo::Scenario>(this)->squad_count;
        jo::view<void**>(jo::self<jo::Scenario>(this)->squads) = joan::orig::arg::as<void**>(joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Scenario>(this)->squads)), joan::orig::arg::u32((count + 1) * static_cast<std::int32_t>(sizeof(jo::UnitPtr))), str(d::s_d_ww_scenario_cpp), joan::orig::arg::i32(line_base(d::g_a02e6c) + 4)));  // 0x0058eb30
        jo::view<void**>(jo::self<jo::Scenario>(this)->squads)[jo::self<jo::Scenario>(this)->squad_count] = copy;
        jo::self<jo::Scenario>(this)->squad_count = static_cast<std::int16_t>(jo::self<jo::Scenario>(this)->squad_count + 1);
        alignas(4) std::uint8_t local[0x78];
        std::memset(local, 0, sizeof(local));
        joan::orig::as_unit(local)->copy_to(joan::orig::arg::mptr(src), joan::orig::arg::mptr(copy));  // 0x004c96f0
        jo::view<std::uint16_t>(jo::as_unit(copy)->kills) = jo::view<std::uint16_t>(jo::as_unit(src)->kills);
        jo::view<std::uint16_t>(jo::as_unit(copy)->squad_kills) = jo::view<std::uint16_t>(jo::as_unit(src)->squad_kills);
        jo::view<std::uint16_t>(jo::as_unit(copy)->squad_losses) = jo::view<std::uint16_t>(jo::as_unit(src)->squad_losses);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Scenario::current", 0x004cd840, &H::current);
B("Scenario::add_copy", 0x004cda10, &H::add_copy);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x004cd8b0 Scenario::push_word   (thiscall(word), ret 4) - realloc'd word array.
//
//      0x004cde30 Scenario::set_name    (thiscall(name), ret 4)
namespace {
namespace game_b99_03 {

JOAN_COUNTER(c_push_word, "Scenario::push_word");
JOAN_COUNTER(c_set_name, "Scenario::set_name");

char* strcpy_(void* d, const void* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcpy
u32 strlen_(const void* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }  // strlen

void mem_free(void* p, img::Addr file, i32 line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------------ 0x004cd8b0
    void push_word(u32 word_v) {
        c_push_word.hit();
        const i32 line = static_cast<i32>(global<i16>(d::g_a02e64)) + 1;
        const i32 size = static_cast<i32>(jo::self<jo::Scenario>(this)->type_count) * 2 + 2;
        jo::view<void*>(jo::self<jo::Scenario>(this)->types) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Scenario>(this)->types)), joan::orig::arg::u32(size), str(d::s_d_ww_scenario_cpp), joan::orig::arg::i32(line));  // 0x0058eb30 "scenario.cpp"
        jo::view<u16>(jo::self<jo::Scenario>(this)->types[static_cast<i32>(jo::self<jo::Scenario>(this)->type_count)].value) = static_cast<u16>(word_v);
        jo::view<u16>(jo::self<jo::Scenario>(this)->type_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Scenario>(this)->type_count) + 1);
    }

    // ------------------------------------------------------------------------ 0x004cde30
    char* set_name(const char* name) {
        c_set_name.hit();
        if (jo::self<jo::ScenarioEntry>(this)->name != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name), d::s_d_ww_scenario_cpp, static_cast<i32>(global<i16>(d::g_a02e70)) + 3);
            jo::self<jo::ScenarioEntry>(this)->name = nullptr;
        }
        const i32 line = static_cast<i32>(global<i16>(d::g_a02e70)) + 7;
        jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name) = mem_alloc(strlen_(name) + 1, d::s_d_ww_scenario_cpp, line);
        return strcpy_(jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name), name);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Scenario::push_word", 0x004cd8b0, &H::push_word);
B99("Scenario::set_name", 0x004cde30, &H::set_name);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
//
//      0x004cdbc0 Scenario::apply_list       (thiscall, ret 0) - 0xa47ca8->0x004ca290(), then for
//                 each of the i16 [+0x287] dwords at [+0x283]: obj->0x004c90a0(v).
//
//  - eax at return is replicated where the original leaves a defined value (callee results,
//    this, loop counter in 0x004cdbc0).
namespace {
namespace game_b97_03 {

JOAN_COUNTER(c_apply_list, "Scenario::apply_list");

struct H {
    // ------------------------------------------------------------------------ 0x004cdbc0
    std::uintptr_t apply_list() {  // eax: the profile or the last count
        c_apply_list.hit();
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
        if (o == nullptr) return 0;
        std::uintptr_t r = reinterpret_cast<std::uintptr_t>(o);
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Scenario>(this)->squad_count); ++i) {
            joan::orig::as_profile(o)->attach_unit(joan::orig::arg::mptr(jo::self<jo::Scenario>(this)->squads[i].unit));  // 0x004c90a0 (pointer entries)
            r = static_cast<u32>(i + 1);
        }
        return r;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("Scenario::apply_list", 0x004cdbc0, &H::apply_list);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
//
//      0x004cdc40 Scenario::remove_id      (thiscall(short id), ret 4) - list of shorts at +0x235,
//                 count short +0x275.
//
//  - 0x004cdc40: when id > 0, the last slot is cleared and the count decremented even if the
//    id was not found (original behaviour kept).
namespace {
namespace game_b96_03 {

JOAN_COUNTER(c_remove_id, "Scenario::remove_id");

struct H {
    // ------------------------------------------------------------------------ 0x004cdc40
    void remove_id(i16 id) {
        c_remove_id.hit();
        if (!(static_cast<i32>(id) > 0)) return;
        i8 found = 0;
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Scenario>(this)->id_count); ++i) {
            if (static_cast<i32>(jo::self<jo::Scenario>(this)->ids[i]) == static_cast<i32>(id)) found = 1;
            if (static_cast<i32>(found) == 1 && i < static_cast<i32>(jo::self<jo::Scenario>(this)->id_count) - 1)
                jo::view<u16>(jo::self<jo::Scenario>(this)->ids[i]) = jo::view<u16>(jo::self<jo::Scenario>(this)->ids[i + 1]);
        }
        jo::view<u16>(jo::self<jo::Scenario>(this)->ids[static_cast<i32>(jo::self<jo::Scenario>(this)->id_count) - 1]) = 0;  // id_list_pad when id_count == 0
        jo::view<u16>(jo::self<jo::Scenario>(this)->id_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Scenario>(this)->id_count) - 1);
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("Scenario::remove_id", 0x004cdc40, &H::remove_id);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
//
//      0x004cdd40 Scenario::push_word      (thiscall(short), ret 4) - up to 0x1f words.
namespace {
namespace game_b93_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004cdd40
    void push_word(i16 v) {
        if (static_cast<i32>(v) > 0 && static_cast<i32>(jo::self<jo::Scenario>(this)->id_count) < 0x1f) {
            jo::self<jo::Scenario>(this)->ids[static_cast<i32>(jo::self<jo::Scenario>(this)->id_count)] = v;
            jo::self<jo::Scenario>(this)->id_count = static_cast<i16>(jo::self<jo::Scenario>(this)->id_count + 1);
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Scenario::push_word", 0x004cdd40, &H::push_word);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
//
//    - unit command resets 0x0043c500, 0x0042d1d0, 0x0042a770; Scenario::trigger_slot
//      0x004cecd0; Scenario::clear 0x004cddc0; message box init 0x0046c360; water init
//      0x00546b50; ref add 0x00437af0; release +0x7e 0x0054a040.
namespace {
namespace game_b85_06 {

struct H {
    void scenario_clear() {  // 0x004cddc0
        for (std::int32_t i = 0; i < jo::self<jo::Scenario>(this)->id_count; ++i) jo::view<std::uint16_t>(jo::self<jo::Scenario>(this)->ids[i]) = 0;
        jo::view<std::uint16_t>(jo::self<jo::Scenario>(this)->id_count) = 0;
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Scenario::clear", 0x004cddc0, &H::scenario_clear);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
//
//      0x004cdf00 Scenario::set_name       (thiscall(str), ret 4) - frees +0x1a2, strdup's str.
namespace {
namespace game_b94_03 {

JOAN_COUNTER(c_set_name, "Scenario::set_name");

using joan::orig::helpers::strcpy_;  // strcpy
using joan::orig::helpers::strlen_;  // strlen
using joan::orig::helpers::line_of;

struct H {
    // ------------------------------------------------------------------------ 0x004cdf00
    void set_name(const char* s) {
        c_set_name.hit();
        if (jo::self<jo::ScenarioEntry>(this)->name2 != nullptr) {
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name2)), str(d::s_d_ww_scenario_cpp), joan::orig::arg::i32(// 0x0058ed70 free
                           line_of(d::g_a02e74, 3)));                             // "d:\ww\scenario.cpp"
            jo::self<jo::ScenarioEntry>(this)->name2 = nullptr;
        }
        const i32 line = line_of(d::g_a02e74, 7);
        const u32 n = strlen_(s) + 1;
        jo::view<void*>(jo::self<jo::ScenarioEntry>(this)->name2) = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(n), str(d::s_d_ww_scenario_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
        strcpy_(jo::self<jo::ScenarioEntry>(this)->name2, s);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("Scenario::set_name", 0x004cdf00, &H::set_name);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x004cc800 Obj::forward_4cc850()               ret 0
//
//  - 0x004baa30 / 0x004baa70 / 0x004baee0 / 0x004b0250 / 0x004cc800 end with a call; its eax
//    is returned.
namespace {
namespace game_b92_03_2 {

using joan::call::thiscall;

struct H {
    // 0x004cc800
    std::uint32_t forward_4cc850() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_scenario(this)->reset()); }  // 0x004cc850
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Obj::forward_4cc850", 0x004cc800, &H::forward_4cc850);
#undef B

}  // namespace game_b92_03_2
}  // namespace

// ---------------------------------------- from modules/game_b105_06_2.cpp (game/b105_06)
namespace {
namespace game_b105_06_2 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_4cc830", 0x004cc830, &H::nop);
#undef B

}  // namespace game_b105_06_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Scenario::reset_wrap() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_03_2::H::forward_4cc850>(this)); }  // 0x4cc800
auto joan::orig::Scenario::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_03::H::scen_reset>(this)); }  // 0x4cc850
auto joan::orig::Scenario::set_4ccf40(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_06::H::set_4ccf40>(this, a0)); }  // 0x4ccf40
auto joan::orig::Scenario::load_4ccfb0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_04::H::scenario_load>(this)); }  // 0x4ccfb0
auto joan::orig::Scenario::mode() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b89_06::H::kind>(this)); }  // 0x4cd510
auto joan::orig::Scenario::set_1ae(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_03::H::set_1ae>(this, a0)); }  // 0x4cd570
auto joan::orig::Timer::tick() -> void { joan::bind::member<&::game_b100_05::H::tick>(this); }  // 0x4cd5d0
auto joan::orig::scenario_mode(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_03::scenario_mode>(a0)); }  // 0x4cd790
auto joan::orig::Scenario::current() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_03::H::current>(this)); }  // 0x4cd840
auto joan::orig::Scenario::push_word_4cd8b0(std::uint32_t a0) -> void { joan::bind::member<&::game_b99_03::H::push_word>(this, a0); }  // 0x4cd8b0
auto joan::orig::Scenario::push_word_4cd960(std::int16_t a0) -> void { joan::bind::member<&::game_b100_05::H::push_word>(this, a0); }  // 0x4cd960
auto joan::orig::Scenario::add_copy(void* a0) -> void { joan::bind::member<&::game_b92_03::H::add_copy>(this, a0); }  // 0x4cda10
auto joan::orig::Scenario::apply_list() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_03::H::apply_list>(this)); }  // 0x4cdbc0
auto joan::orig::Scenario::remove_id(std::int16_t a0) -> void { joan::bind::member<&::game_b96_03::H::remove_id>(this, a0); }  // 0x4cdc40
auto joan::orig::Scenario::push_word_4cdd40(std::int16_t a0) -> void { joan::bind::member<&::game_b93_02::H::push_word>(this, a0); }  // 0x4cdd40
auto joan::orig::Scenario::clear() -> void { joan::bind::member<&::game_b85_06::H::scenario_clear>(this); }  // 0x4cddc0
auto joan::orig::Scenario::set_name_4cde30(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_03::H::set_name>(this, a0)); }  // 0x4cde30
auto joan::orig::Scenario::set_name_4cdf00(const char* a0) -> void { joan::bind::member<&::game_b94_03::H::set_name>(this, a0); }  // 0x4cdf00
