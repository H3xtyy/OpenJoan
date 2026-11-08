// game/grassres.cpp: functions of the original d:\ww\grassres.cpp (re/attribution.csv).
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
#include "../original/records.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using i16 = std::int16_t;
using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
//
//    - GrassRes::load_and_mark 0x00487cd0 (ret 0): 0x00487d60 (thunk 0x004011bd), byte +8 = 1.
namespace {
namespace game_b102_03 {

struct H {
    // ------------------------------------------------------------------ 0x00487cd0
    H* load_and_mark() {
        joan::orig::as_grass_res(this)->load();  // 0x00487d60
        jo::self<jo::GrassRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("GrassRes::load_and_mark", 0x00487cd0, &H::load_and_mark);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x00487d00 Obj::delete_all_487d00        (thiscall, ret 0)
//
//  - 0x00487d00: when the loop does not run, eax is the caller's; the only caller 0x00487c40
//    enters with eax = this (mov eax,[ebp-4]) and returns that eax on one path - this here.
namespace {
namespace game_b99_02 {

using u16 = std::uint16_t;

using joan::orig::helpers::u;

JOAN_COUNTER(c_delete_all, "Obj::delete_all_487d00");

struct H {
    // ------------------------------------------------------------------ 0x00487d00
    std::uintptr_t delete_all_487d00() {
        c_delete_all.hit();
        std::uintptr_t r = u(this);  // caller's eax: the only caller (0x00487c40) enters with eax = this
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::GrassRes>(this)->count; ) {
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_table(this)->at_4671c0(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x004671c0 (low word)
            r = joan::orig::arg::as<u32>(joan::orig::as_grass_type(p)->delete_entries());  // 0x00488300
            i = static_cast<i16>(i + 1);
            r = (r & 0xffff0000u) | static_cast<u16>(i);
        }
        return r;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::delete_all_487d00", 0x00487d00, &H::delete_all_487d00);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b101_03.cpp (game/b101_03)
//
//    - GrassRes::load 0x00487d60 (ret 0): reads ITEM\DYNGRASS.RES (same shape as 0x00463aa0).
//
//  - SEH frames (0x480880, 0x480ce0, 0x482f70, 0x4855b0, 0x485f50, 0x487d60) only guard new + ctor
//    or the stack objects' destructors: normal path reproduced.
//
//  - Stack objects of 0x487d60 get the whole gap of the original frame (as in game_b100_02.cpp).
namespace {
namespace game_b101_03 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_grass_load, "GrassRes::load");

struct H {
    // ------------------------------------------------------------------ 0x00487d60
    void grass_load() {
        c_grass_load.hit();
        alignas(8) u8 res[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];   // [ebp-0x148] (x64: the larger ResourceIdxL)
        alignas(8) u8 path[0x194];  // [ebp-0x2dc]
        alignas(8) u8 item[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x480] (x64: the larger FileTxt)
        joan::orig::as_res_idx(res)->construct();             // 0x004179f0
        joan::orig::as_string(path)->construct();     // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_ITEM_9cf648));     // 0x005a1440 "ITEM\\"
        joan::orig::as_string(path)->append(str(d::s_DYNGRASS_RES));         // 0x005a1550 "DYNGRASS.RES"
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(name), static_cast<char>(1), static_cast<char>(0));  // 0x0059b920
        jo::self<jo::GrassRes>(this)->count = jo::as_res_idx_l(res)->rec_count;
        jo::view<void*>(jo::self<jo::GrassRes>(this)->rows) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::self<jo::GrassRes>(this)->count) * sizeof(jo::GrassType))), str(d::s_d_ww_grassres_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffce4)) + 0xb));  // 0x0058e7b0
        crt_memset(jo::view<void*>(jo::self<jo::GrassRes>(this)->rows), 0, static_cast<u32>(jo::self<jo::GrassRes>(this)->count) * sizeof(jo::GrassType));
        for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
            jo::GrassType* const e = jo::self<jo::GrassRes>(this)->rows + (i - 1);
            e->id = static_cast<i16>(i);
            void* const r1 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
            void* const r2 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
            joan::orig::as_file_txt(item)->construct_mem(joan::orig::arg::str(r2), joan::orig::arg::i32(reinterpret_cast<std::uintptr_t>(r1)));             // 0x00589780
            joan::orig::as_grass_res(this)->read_entries(joan::orig::arg::mptr(static_cast<void*>(item)), joan::orig::arg::mptr(static_cast<void*>(e)));  // 0x00487f70
            joan::orig::as_file_txt(item)->destruct();                            // 0x00589890
        }
        joan::orig::as_archive(res)->dtor();  // 0x00417a30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_03", name, addr, fn)
B("GrassRes::load", 0x00487d60, &H::grass_load);
#undef B

}  // namespace game_b101_03
}  // namespace

// ---------------------------------------- from modules/game_b101_03.cpp (game/b101_03)
//
//    - Obj::call_487c10 0x00487c10: forwards to 0x00487c40 (eax kept).
namespace {
namespace game_b101_03_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00487c10
    u32 call_487c10() { return joan::orig::arg::as<u32>(joan::orig::as_grass_res(this)->free()); }  // 0x00487c40
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_03", name, addr, fn)
B("Obj::call_487c10", 0x00487c10, &H::call_487c10);
#undef B

}  // namespace game_b101_03_2
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - GrassRes::free 0x00487c40 (ret 0): when +8 == 1: 0x487d00, heap free of +4
//      ("grassres.cpp", line [0x9ffce0] + 7), +4 = +0 = 0; +8 = 0.
//
//    - GrassRes::read_entries 0x00487f70 (reader, target), ret 8 (SEH frame: normal path only):
//      header tokens (atoi), strncpy(target + 2, token, 0x20), new 0x1d dynarr (0x5843d0
//      (0xe, 0x1e, 0)) at target+0x23, then 14-byte entries until "End" / no token: type id
//      (0x4a36c0), two atoi words, two atof * (pi / 180) floats.
//
//  - 0x00487f70: atof (0x7b2110) leaves an fld qword (double) in st0; the fmul qword at PC24
//    goes through Fpu{true}. The first atoi result is unused (call kept).
namespace {
namespace game_b100_04 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;
using joan::orig::helpers::addr;

void* const kHeap = obj(d::g_mem);
void* const kTypes = obj(d::g_item_res);

using joan::orig::helpers::op_new;
using joan::orig::helpers::crt_atoi;
double crt_atof(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
char* crt_strncpy(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

using joan::orig::GrassEntry;  // packed, original/records.hpp

JOAN_COUNTER(c_grass_read, "GrassRes::read_entries");
JOAN_COUNTER(c_grass_free, "GrassRes::free");

struct H {
    // ------------------------------------------------------------------ 0x00487c40
    std::uintptr_t grass_free() {
        c_grass_free.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::GrassRes>(this)->loaded)) != 1) return addr(this);
        std::uintptr_t eax = joan::orig::arg::as<u32>(joan::orig::as_grass_res(this)->delete_types());  // 0x00487d00
        if (jo::view<void*>(jo::self<jo::GrassRes>(this)->rows) != nullptr) {
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::GrassRes>(this)->rows)), str(d::s_d_ww_grassres_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9ffce0)) + 7));  // 0x0058ed70
            jo::self<jo::GrassRes>(this)->rows = nullptr;
            jo::view<u32>(jo::self<jo::GrassRes>(this)->count) = 0;
            eax = addr(this);
        }
        jo::self<jo::GrassRes>(this)->loaded = 0;
        return eax;
    }

    // ------------------------------------------------------------------ 0x00487f70
    u32 grass_read(void* reader, u8* target) {
        c_grass_read.hit();
        const Fpu fpu{true};
        char* t = joan::orig::as_file_txt(reader)->get_token(1);  // 0x00589c70
        t = joan::orig::as_file_txt(reader)->get_token(1);
        crt_atoi(t);
        t = joan::orig::as_file_txt(reader)->get_token(1);
        crt_strncpy(reinterpret_cast<char*>(target + 2), joan::orig::as_file_txt(reader)->get_token(1), 0x20);
        void* const p = op_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        void* const list = p != nullptr ? joan::orig::dynarray(p)->construct(0xe, 0x1e, 0) : nullptr;  // 0x005843d0
        jo::view<void*>(jo::as_grass_type(target)->entries) = list;
        for (;;) {
            t = joan::orig::as_file_txt(reader)->get_token(1);
            if (t == nullptr) break;
            if (crt_stricmp(t, str(d::s_End)) == 0) break;  // "End"
            GrassEntry e;
            e.type = joan::orig::arg::as<u16>(joan::orig::as_name_table(kTypes)->find_4a36c0(joan::orig::arg::str(t)));  // 0x004a36c0
            t = joan::orig::as_file_txt(reader)->get_token(1);
            e.a = static_cast<u16>(crt_atoi(t));
            t = joan::orig::as_file_txt(reader)->get_token(1);
            e.b = static_cast<u16>(crt_atoi(t));
            t = joan::orig::as_file_txt(reader)->get_token(1);
            e.r0 = static_cast<float>(fpu.mul(crt_atof(t), global<double>(d::dbl_0_017453292519943295)));
            t = joan::orig::as_file_txt(reader)->get_token(1);
            e.r1 = static_cast<float>(fpu.mul(crt_atof(t), global<double>(d::dbl_0_017453292519943295)));
            joan::orig::dynarray(jo::view<void*>(jo::as_grass_type(target)->entries))->linkin(&e);      // 0x00584620
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("GrassRes::free", 0x00487c40, &H::grass_free);
B("GrassRes::read_entries", 0x00487f70, &H::grass_read);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

struct H {
    // ------------------------------------------------------------------------ 0x00487be0
    void* zero_9() {
        std::memset(joan::orig::arg::mptr(this), 0, static_cast<u32>(sizeof(jo::GrassRes)));  // memset (9 on x86: the GrassRes 0xa50680)
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("Struct9::zero", 0x00487be0, &H::zero_9);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using joan::call::cdecl_call;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_name_find, "NameTable::find");

struct H {
    // ------------------------------------------------------------------ 0x00488190
    i32 name_find(const char* name) {
        c_name_find.hit();
        for (i32 i = 0; i < jo::self<jo::GrassRes>(this)->count; ++i) {
            if (stricmp_(jo::self<jo::GrassRes>(this)->rows[i].name, name) == 0) return i + 1;  // GrassType +2 name
        }
        return 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("NameTable::find", 0x00488190, &H::name_find);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
namespace {
namespace game_b97_02 {

using joan::call::cdecl_call;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::orig::helpers::stricmp_;

using joan::call::thiscall;

using joan::img::str;

JOAN_COUNTER(c_find_m, "Table::find_m");

struct Path {
    u8 buf[0x194];  // 0x005a15d0 writes the NUL at +0x190
};

struct H {
    // ------------------------------------------------------------------ 0x00488210
    i32 find_m(const char* name, i32 n) {
        c_find_m.hit();
        Path p;
        joan::orig::as_string(&p)->construct();          // 0x005a1170
        joan::orig::as_string(&p)->assign(joan::orig::arg::str(name));  // 0x005a1440
        joan::orig::as_string(&p)->append(str(d::s_M_9d37b4));                // 0x005a1550 "_M"
        joan::orig::as_string(&p)->append(joan::orig::arg::i32(n));  // 0x005a15d0
        for (i32 i = 0; i < jo::self<jo::GrassRes>(this)->count; ++i) {
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str());  // 0x00417d90
            if (stricmp_(jo::self<jo::GrassRes>(this)->rows[i].name, s) == 0) return i + 1;  // GrassType +2 name
        }
        return joan::orig::arg::as<i32>(joan::orig::as_name_table(this)->find_488190(joan::orig::arg::str(name)));  // 0x00488190
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Table::find_m", 0x00488210, &H::find_m);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
namespace {
namespace game_b98_02 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::orig::helpers::u;

struct H {
    // ------------------------------------------------------------------ 0x00488300
    std::uintptr_t delete_23() {
        if (jo::view<void*>(jo::self<jo::GrassType>(this)->entries) == nullptr) return u(this);
        void* const p = jo::view<void*>(jo::self<jo::GrassType>(this)->entries);
        std::uintptr_t r = 0;
        if (p != nullptr) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dynarray(p)->destroy(1u));  // 0x00463fc0 (scalar deleting dtor)
        jo::view<void*>(jo::self<jo::GrassType>(this)->entries) = nullptr;
        return r;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Obj::delete_23", 0x00488300, &H::delete_23);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b105_04.cpp (game/b105_04)
namespace {
namespace game_b105_04 {

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_04", name, addr, fn)
B("Obj::stub_488370", 0x00488370, &H::stub0);
#undef B

}  // namespace game_b105_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::GrassRes::free() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::grass_free>(this)); }  // 0x487c40
auto joan::orig::GrassRes::delete_types() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::delete_all_487d00>(this)); }  // 0x487d00
auto joan::orig::GrassRes::load() -> void { joan::bind::member<&::game_b101_03::H::grass_load>(this); }  // 0x487d60
auto joan::orig::GrassRes::read_entries(void* a0, void* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_04::H::grass_read>(this, a0, a1)); }  // 0x487f70
auto joan::orig::NameTable::find_488190(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_02::H::name_find>(this, a0)); }  // 0x488190
auto joan::orig::Table::find_m(const char* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_02::H::find_m>(this, a0, a1)); }  // 0x488210
auto joan::orig::GrassType::delete_entries() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_02::H::delete_23>(this)); }  // 0x488300
