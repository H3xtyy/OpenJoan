// game/uscrres.cpp: functions of the original d:\ww\uscrres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
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

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
//
//      0x00546140 UScrRes::free           (ret 0).
//
//  - 0x00531620 / 0x00560fe0 / 0x00546140 return void (callers ignore eax; 0x00531690 is
//    itself replaced as void in game/b93_02).
namespace {
namespace game_b97_04 {

using i8 = std::int8_t;

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_uscr_free, "UScrRes::free");

using joan::orig::helpers::line_of;
void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {

    // ------------------------------------------------------------------------ 0x00546140
    void uscr_free() {
        c_uscr_free.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::UScrRes>(this)->loaded)) != 1) return;
        joan::orig::as_list(this)->delete_22();  // 0x00546200
        if (jo::self<jo::UScrRes>(this)->scripts != nullptr) {
            mem_free(jo::self<jo::UScrRes>(this)->scripts, str(d::s_d_ww_uscrres_cpp), line_of(d::g_a0a88c, 7));  // "d:\ww\uscrres.cpp"
            jo::self<jo::UScrRes>(this)->scripts = nullptr;
            jo::view<u32>(jo::self<jo::UScrRes>(this)->count) = 0;
        }
        jo::self<jo::UScrRes>(this)->loaded = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("UScrRes::free", 0x00546140, &H::uscr_free);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b101_08.cpp (game/b101_08)
//
//    - Obj::init_5461d0 0x005461d0 (ret 0): 0x005462b0 then byte +8 = 1; returns this.
namespace {
namespace game_b101_08 {

struct H {

    // ------------------------------------------------------------------ 0x005461d0
    void* init_5461d0() {
        joan::orig::as_unit_script_res(this)->load();  // 0x005462b0
        jo::self<jo::UScrRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_08", name, addr, fn)
B("Obj::init_5461d0", 0x005461d0, &H::init_5461d0);
#undef B

}  // namespace game_b101_08
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
//
//    - List::delete_22 0x00546200 (ret 0): for i = 1 .. [this] deletes entry->+0x22 (virtual
//      slot 0 with 1) and clears it.
namespace {
namespace game_b96_04 {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_delete_22, "List::delete_22");

struct H {

    // ------------------------------------------------------------------------ 0x00546200
    void delete_22() {
        c_delete_22.hit();
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::UScrRes>(this)->count; i = static_cast<i16>(i + 1)) {
            jo::UnitScript* const e = joan::orig::arg::as<jo::UnitScript*>(joan::orig::as_uscr_res(this)->entry(static_cast<i32>(i)));  // 0x00484540
            if (e->object == nullptr) continue;
            void* const p = e->object;
            if (p != nullptr) virtual_call<void*>(p, 0, 1u);  // scalar deleting destructor
            e->object = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("List::delete_22", 0x00546200, &H::delete_22);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
//
//    - UnitAIRes::load 0x005462b0 (ret 0, SEH frame: normal path only): reads
//      "SCRIPT\UNIT\unitAI.res" into a 0x28-byte record array (+0 count, +4 records).
//
//  - 0x005462b0: local resource object 0x135 bytes (0x4179f0 / 0x417a30, count dword at
//    +0x121), path string 0x194 bytes (same frame space as the original).
namespace {
namespace game_b100_08 {

void* const kMemory = obj(d::g_mem);

JOAN_COUNTER(c_ai_load, "UnitAIRes::load");

// ------------------------------------------------------------------------------- thiscall
struct H {

    // 0x005462b0
    void ai_load() {
        c_ai_load.hit();
        alignas(alignof(void*)) std::uint8_t res[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger ResourceIdxL
        alignas(4) std::uint8_t path[0x194];
        joan::orig::as_res_idx(res)->construct();                     // 0x004179f0
        joan::orig::as_string(path)->construct();             // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_SCRIPT_UNIT));             // 0x005a1440 "SCRIPT\UNIT\"
        joan::orig::as_string(path)->append(str(d::s_unitAI_res));                 // 0x005a1550 "unitAI.res"
        const char* const full = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(full), static_cast<char>(i32{1}), static_cast<char>(i32{0}));  // 0x0059b920
        jo::self<jo::UScrRes>(this)->count = jo::as_res_idx_l(res)->rec_count;
        jo::self<jo::UScrRes>(this)->scripts = static_cast<jo::UnitScript*>(joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::self<jo::UScrRes>(this)->count) * static_cast<u32>(sizeof(jo::UnitScript)))), joan::orig::arg::str(// 0x0058e7b0
                                      str(d::s_d_ww_uscrres_cpp)), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a0a890)) + 8)));
        std::memset(joan::orig::arg::mptr(jo::self<jo::UScrRes>(this)->scripts), i32{0}, joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::self<jo::UScrRes>(this)->count) * static_cast<u32>(sizeof(jo::UnitScript)))));  // memset
        for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
            jo::UnitScript* const rec = jo::self<jo::UScrRes>(this)->scripts + i - 1;
            rec->id = static_cast<i16>(i);
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_res_idx_l(res)->get_index_rec(joan::orig::arg::i32(i)));  // 0x0059c3b0
// strncpy without terminator into the fixed 32-byte name field, as the original does.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(rec->name)), joan::orig::arg::str(name), u32{0x20});  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
            rec->kind = joan::orig::arg::as<i16>(joan::orig::as_unit_script_res(this)->kind_of_name(joan::orig::arg::str(rec->name)));  // 0x00546520
        }
        joan::orig::as_archive(res)->dtor();  // 0x00417a30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("UnitAIRes::load", 0x005462b0, &H::ai_load);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

using joan::call::thiscall;

using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------------ 0x00546110
    i32 forward_546140() {
        joan::orig::as_unit_script_res(this)->free();  // 0x00546140
        return 0;  // 0x00546140 returns nothing; the atexit caller (0x0056a090) ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::forward_546140", 0x00546110, &H::forward_546140);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
//
//      0x00546520 UScrRes::kind_of_name   (name), ret 4 - _stricmp table, default 0.
namespace {
namespace game_b97_04_2 {

using joan::call::cdecl_call;
using joan::img::str;

using i32 = std::int32_t;

JOAN_COUNTER(c_kind_of_name, "UScrRes::kind_of_name");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x00546520
    i32 kind_of_name(const char* s) {
        c_kind_of_name.hit();
        if (stricmp_(s, str(d::s_human_light_infantry)) == 0) return 1;  // "human_light_infantry"
        if (stricmp_(s, str(d::s_human_light_general)) == 0) return 2;  // "human_light_general"
        if (stricmp_(s, str(d::s_special_general)) == 0) return 3;  // "special_general"
        if (stricmp_(s, str(d::s_human_heavy_infantry)) == 0) return 4;  // "human_heavy_infantry"
        if (stricmp_(s, str(d::s_human_heavy_general)) == 0) return 5;  // "human_heavy_general"
        if (stricmp_(s, str(d::s_human_archer)) == 0) return 6;  // "human_archer"
        if (stricmp_(s, str(d::s_human_archer_ex)) == 0) return 6;  // "human_archer_ex"
        if (stricmp_(s, str(d::s_archer_gen)) == 0) return 7;  // "archer_gen"
        if (stricmp_(s, str(d::s_archer_ex_gen)) == 0) return 7;  // "archer_ex_gen"
        if (stricmp_(s, str(d::s_human_cannon)) == 0) return 8;  // "human_cannon"
        if (stricmp_(s, str(d::s_autobow)) == 0) return 9;  // "autobow"
        if (stricmp_(s, str(d::s_batterram)) == 0) return 0xa;  // "batterram"
        if (stricmp_(s, str(d::s_camp_horse)) == 0) return 0xb;  // "camp_horse"
        if (stricmp_(s, str(d::s_cart)) == 0) return 0xc;  // "cart"
        if (stricmp_(s, str(d::s_citizen)) == 0) return 0xd;  // "citizen"
        if (stricmp_(s, str(d::s_human_trebuchet)) == 0) return 0xe;  // "human_trebuchet"
        if (stricmp_(s, str(d::s_pig)) == 0) return 0xf;  // "pig"
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("UScrRes::kind_of_name", 0x00546520, &H::kind_of_name);
#undef B

}  // namespace game_b97_04_2
}  // namespace

// ---------------------------------------- from modules/game_b99_05_2.cpp (game/b99_05)
namespace {
namespace game_b99_05_2 {

using joan::call::cdecl_call;

using i32 = std::int32_t;


JOAN_COUNTER(c_name_find, "NameTable::find");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }   // _stricmp

struct H {

    // ------------------------------------------------------------------------ 0x005464a0
    i32 name_find(const char* name) {
        c_name_find.hit();
        for (i32 i = 0; i < jo::self<jo::UScrRes>(this)->count; ++i) {
            if (stricmp_(jo::self<jo::UScrRes>(this)->scripts[i].name, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("NameTable::find", 0x005464a0, &H::name_find);
#undef B

}  // namespace game_b99_05_2
}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
namespace {
namespace game_b89_04 {


using joan::call::thiscall;

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

struct H {
    // ------------------------------------------------------------------------ 0x005467b0
    std::int32_t order_dispatch(void* arg) {
        switch (sx16(jo::self<jo::UnitScript>(this)->kind)) {  // `this` is a script record (UnitScript), not an Order
        case 1: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_1(arg));   // 0x00531f90
        case 2: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_2(arg));   // 0x005322c0
        case 3: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_3(arg));   // 0x00532520
        case 4: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_4(arg));   // 0x00532780
        case 5: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_5(arg));   // 0x00532a00
        case 6: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_6(arg));   // 0x00532c60
        case 7: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_7(arg));   // 0x00532ef0
        case 8: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_8(arg));   // 0x00533170
        case 9: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_9(arg));   // 0x00533370
        case 10: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_10(arg));  // 0x00533440
        case 11: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_11(arg));  // 0x005335e0
        case 12: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_12(arg));  // 0x005336e0
        case 13: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_13(arg));  // 0x00533810
        case 14: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_14(arg));  // 0x00533920
        case 15: return joan::orig::arg::as<std::int32_t>(joan::orig::as_order(this)->run_15(arg));  // 0x00533b10
        default: return 0;
        }
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("Order::dispatch", 0x005467b0, &H::order_dispatch);
#undef B89

}  // namespace game_b89_04
}  // namespace

// ---------------------------------------- from modules/game_b106_20.cpp (game/b106_20)
namespace {
namespace game_b106_20 {


using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

using i16 = std::int16_t;

JOAN_COUNTER(c_dispatch_546970, "Ai::dispatch_546970");

struct H {
    // ------------------------------------------------------------------ 0x00546970
    u32 dispatch_546970(jo::Unit* arg) {
        c_dispatch_546970.hit();
        switch (static_cast<i32>(jo::self<jo::UnitScript>(this)->kind) - 1) {
            case 0: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_a(arg));  // 0x00535c20
            case 1: return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_538340(arg));  // 0x00538340
            case 2: return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53abd0(arg));  // 0x0053abd0
            case 3: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flee_b(arg));  // 0x0053cb10
            case 4: return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->voice_react_53f670(arg));  // 0x0053f670
            case 5: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->run_a(arg, i32{1}, i32{0}));  // 0x00541270
            case 6: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->run_b(arg, i32{1}, i32{0}));  // 0x005432a0
            case 7: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->run_c(arg, i32{1}, i32{0}));  // 0x00544c40
            case 8: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->run_f(arg, joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{0})));  // 0x00544e00
            case 9: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->flag_f_retreat(arg));  // 0x00545380
            case 13: return joan::orig::arg::as<u32>(joan::orig::as_order(this)->run_d(arg, i32{1}, i32{0}));  // 0x00545d00
            default: return 1;
        }
    }
};

JOAN_REPLACE("game/b106_20", "Ai::dispatch_546970", 0x00546970, &H::dispatch_546970);

}  // namespace game_b106_20
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::UnitScriptRes::free() -> void { joan::bind::member<&::game_b97_04::H::uscr_free>(this); }  // 0x546140
auto joan::orig::List::delete_22() -> void { joan::bind::member<&::game_b96_04::H::delete_22>(this); }  // 0x546200
auto joan::orig::UnitScriptRes::load() -> void { joan::bind::member<&::game_b100_08::H::ai_load>(this); }  // 0x5462b0
auto joan::orig::NameTable::find_5464a0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_05_2::H::name_find>(this, a0)); }  // 0x5464a0
auto joan::orig::UnitScriptRes::kind_of_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_04_2::H::kind_of_name>(this, a0)); }  // 0x546520
auto joan::orig::Order::dispatch(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_04::H::order_dispatch>(this, a0)); }  // 0x5467b0
auto joan::orig::Ai::dispatch_546970(Unit* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_20::H::dispatch_546970>(this, a0)); }  // 0x546970
