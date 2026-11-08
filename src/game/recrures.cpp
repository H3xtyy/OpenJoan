// game/recrures.cpp: functions of the original d:\ww\recrures.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
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

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using i16 = std::int16_t;
using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
//
//    - Obj::reset_4ca6d0 0x004ca6d0 / Obj::reset_4ce390 0x004ce390, ret 0: call + byte +8 = 1,
//      return this.
namespace {
namespace game_b103_04 {

struct H {
    void* reset_4ca6d0() {
        joan::orig::as_recruit_res(this)->load();  // 0x004ca760
        jo::self<jo::RecruitRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Obj::reset_4ca6d0", 0x004ca6d0, &H::reset_4ca6d0);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - List::update_all_4ca700 0x004ca700 / List::update_all_4ce3c0 0x004ce3c0: short index
//      1..[+0] calling 0x004cad80 / 0x004ce980 on element 0x00469520 / 0x00469e60(i).
namespace {
namespace game_b100_05 {

JOAN_COUNTER(c_update_4ca700, "List::update_all_4ca700");

struct H {
    // ------------------------------------------------------------------ 0x004ca700
    void update_all_4ca700() {
        c_update_4ca700.hit();
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::RecruitRes>(this)->count; i = static_cast<i16>(i + 1)) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_recruit_res(this)->group_at(joan::orig::arg::u32(i)));  // 0x00469520
            joan::orig::as_recruit_row(e)->free_23();                          // 0x004cad80
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("List::update_all_4ca700", 0x004ca700, &H::update_all_4ca700);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
//
//    - RecruitRes::load 0x004ca760 ("UNIT\RECRUIT.RES", 0xb4-byte records, 0x4ca980) /
//      SellItemRes::load 0x004ce420 ("ITEM\SELLITEM.RES", 0x1c-byte records, 0x4ce630).
namespace {
namespace game_b102_04 {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_recruit_load, "RecruitRes::load");

// RecruitRes::load / SellItemRes::load (identical code, different names / sizes).
u32 res_load(void* self, const char* dir, const char* file, const char* src, i32 line, u32 stride, std::uintptr_t record) {
    alignas(8) u8 res[(sizeof(jo::ResourceIdxL) + 3u) & ~3u];  // [ebp-0x148] (0x138 on x86)
    alignas(8) u8 path[(sizeof(jo::String) + 3u) & ~3u];       // [ebp-0x2dc] (0x194)
    alignas(8) u8 item[(sizeof(jo::FileTxt) + 3u) & ~3u];      // [ebp-0x480] (0x19c on x86)
    joan::orig::as_res_idx(res)->construct();   // 0x004179f0
    joan::orig::as_string(path)->construct();  // 0x005a1170
    joan::orig::as_string(path)->assign(joan::orig::arg::str(dir));  // 0x005a1440
    joan::orig::as_string(path)->append(joan::orig::arg::str(file));  // 0x005a1550
    void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
    joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(name), static_cast<char>(1), static_cast<char>(0));  // 0x0059b920
    jo::as_recruit_res(self)->count = jo::as_res_idx_l(res)->rec_count;
    jo::as_recruit_res(self)->entries = static_cast<jo::RecruitEntry*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::as_recruit_res(self)->count) * stride)), joan::orig::arg::str(src), joan::orig::arg::i32(line)));  // 0x0058e7b0
    crt_memset(jo::as_recruit_res(self)->entries, 0, static_cast<u32>(jo::as_recruit_res(self)->count) * stride);
    for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
        jo::RecruitEntry* const e = reinterpret_cast<jo::RecruitEntry*>(reinterpret_cast<u8*>(jo::as_recruit_res(self)->entries) + (static_cast<u32>(i) * stride - stride));
        e->id = static_cast<i16>(i);
        const i32 r1 = joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i));  // 0x0059bdd0
        void* const r2 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
        joan::orig::as_file_txt(item)->construct_mem(joan::orig::arg::str(r2), joan::orig::arg::i32(r1));  // 0x00589780
        thiscall<u32>(record, self, static_cast<void*>(item), static_cast<void*>(e));
        joan::orig::as_file_txt(item)->destruct();  // 0x00589890
    }
    joan::orig::as_archive(res)->dtor();  // 0x00417a30
    return 0;  // 0x00417a30 returns nothing; RecruitRes::load's caller ignores the result
}

struct H {
    // ------------------------------------------------------------------ 0x004ca760
    u32 recruit_load() {
        c_recruit_load.hit();
        return res_load(this, str(d::s_UNIT_9d5748), str(d::s_RECRUIT_RES), str(d::s_d_ww_recrures_cpp), static_cast<i32>(global<i16>(d::g_a0295c)) + 0xb,
                        static_cast<u32>(sizeof(jo::RecruitEntry)), fn::code_value(fn::RecruitRes_parse));  // 0x004ca980
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("RecruitRes::load", 0x004ca760, &H::recruit_load);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
//
//    - RecruitRes::dtor 0x004ca610 -> 0x4ca640 / SellItemRes::dtor 0x004ce2d0 -> 0x4ce300.
namespace {
namespace game_b102_04_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x004ca610 / 0x004ce2d0
    u32 recruit_dtor() {
        joan::orig::as_recruit_res(this)->release();  // 0x004ca640
        return 0;  // 0x004ca640 returns nothing; the atexit caller (0x0056a2d0) ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("RecruitRes::dtor", 0x004ca610, &H::recruit_dtor);
#undef B

}  // namespace game_b102_04_2
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
//
//    - RecruitRes::release 0x004ca640 (ret 0): when byte +8 == 1: 0x004ca700, frees +4
//      (recrures.cpp), +4 = +0 = 0, byte +8 = 0.
//
//    - RecruitRes::parse 0x004ca980 (reader, rec), ret 8: name, "Location", "Level",
//      "Population", a float, then (unit type, float, int) rows until "End" into a 10-byte
//      element list 0x005843d0(10, 0x1e, 0) at rec+0x23.
//
//    - SItemRes::release 0x004ce300 (ret 0): as 0x004ca640 with 0x004ce3c0 / sitemres.cpp.
//
//  - SEH frames in 0x004c8220 / 0x004c8490 / 0x004c9930 / 0x004ca980 / 0x004ce630 only guard
//    the File object or new + ctor: normal path reproduced.
//
//  - 0x004ca980 strcpy's the (strncpy'd, maybe unterminated) 0x20-byte name at rec+2 into a
//    0x84-byte stack buffer; our buffer is larger, so only a malformed record (no NUL within
//    0x84 bytes of rec+2) could behave differently (the original would smash its SEH frame).
namespace {
namespace game_b101_06 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::op_new;
using joan::orig::helpers::crt_atoi;
float crt_atof(const char* s) { return joan::orig::arg::as<float>(joan::orig::libc::atof(joan::orig::arg::str(s))); }  // st0 -> fstp dword
// strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
char* crt_strncpy(void* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
char* crt_strcpy(void* d, const void* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
i32 line_of(img::Addr g, i32 add_) { return static_cast<i32>(global<i16>(g)) + add_; }

// ---------------------------------------------------------------- record readers
char* next_token(void* reader) { return joan::orig::as_file_txt(reader)->get_token(1); }  // 0x00589c70

using joan::orig::RecruitRow;  // packed, original/records.hpp

void* new_list(i32 elem) {  // new(0x1d) + 0x005843d0(elem, 0x1e, 0)
    void* const p = op_new(sizeof(jo::DynArray));  // 0x1d on x86
    if (p == nullptr) return nullptr;
    return joan::orig::dynarray(p)->construct(joan::orig::arg::i32(elem), 0x1e, 0);  // 0x005843d0
}
void list_add(void* list, const void* e) { joan::orig::dynarray(list)->linkin(joan::orig::arg::ptr(e)); }  // 0x00584620

JOAN_COUNTER(c_recruit_parse, "RecruitRes::parse");

struct H {
    // ------------------------------------------------------------------------ 0x004ca640
    void recruit_release() {
        if (jo::view<i8>(jo::self<jo::RecruitRes>(this)->loaded) != 1) return;
        joan::orig::as_list(this)->update_all_4ca700();  // 0x004ca700
        if (jo::self<jo::RecruitRes>(this)->entries != nullptr) {
            const i32 line = line_of(d::g_a02958, 7);
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::RecruitRes>(this)->entries)), str(d::s_d_ww_recrures_cpp), joan::orig::arg::i32(line));  // 0x0058ed70 "recrures.cpp"
            jo::self<jo::RecruitRes>(this)->entries = nullptr;
            jo::view<u32>(jo::self<jo::RecruitRes>(this)->count) = 0;
        }
        jo::self<jo::RecruitRes>(this)->loaded = 0;
    }

    // ------------------------------------------------------------------------ 0x004ca980
    void recruit_parse(void* reader, u8* rec) {
        c_recruit_parse.hit();
        char buf[0x200];  // 0x84 bytes in the original frame (read_line writes buf[0x81])
        char* t = next_token(reader);
        t = next_token(reader);
        crt_atoi(t);  // result unused
        t = next_token(reader);
        crt_strncpy(rec + 2, next_token(reader), 0x20);
        crt_strcpy(buf, rec + 2);
        t = next_token(reader);
        if (crt_stricmp(t, str(d::s_Location)) == 0) {  // "Location"
            joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(static_cast<void*>(buf)), 0x81);                         // 0x00589f10
            t = next_token(reader);
        }
        crt_strncpy(jo::as_recruit_entry(rec)->location, buf, 0x80);  // RecruitEntry +0x2b on x86 (behind a pointer on x64)
        jo::as_recruit_entry(rec)->level = -1;
        if (crt_stricmp(t, str(d::s_Level)) == 0) {  // "Level"
            t = next_token(reader);
            jo::as_recruit_entry(rec)->level = crt_atoi(t);
            t = next_token(reader);
        }
        jo::as_recruit_entry(rec)->population = -1;
        if (crt_stricmp(t, str(d::s_Population)) == 0) {  // "Population"
            t = next_token(reader);
            jo::as_recruit_entry(rec)->population = crt_atoi(t);
            t = next_token(reader);
        }
        jo::as_recruit_entry(rec)->value = crt_atof(next_token(reader));
        jo::view<void*>(jo::as_recruit_entry(rec)->rows) = new_list(10);
        RecruitRow row;
        for (;;) {
            t = next_token(reader);
            if (t == nullptr) break;
            if (crt_stricmp(t, str(d::s_End)) == 0) break;  // "End"
            row.type = joan::orig::arg::as<i16>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(t)));  // 0x005270b0
            t = next_token(reader);
            row.value = crt_atof(t);
            t = next_token(reader);
            row.count = crt_atoi(t);
            list_add(jo::view<void*>(jo::as_recruit_entry(rec)->rows), &row);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("RecruitRes::release", 0x004ca640, &H::recruit_release);
B("RecruitRes::parse", 0x004ca980, &H::recruit_parse);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_9() {  // 0x004ca5e0
        memset_(this, 0, sizeof(jo::RecruitRes));  // 9 on x86
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Struct9::zero_4ca5e0", 0x004ca5e0, &H::zero_9);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using i32 = std::int32_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

JOAN_COUNTER(c_find_b4, "Table::find_b4");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x004cad00
    i32 find_b4(const char* name) {
        c_find_b4.hit();
        for (i32 i = 0; i < jo::self<jo::RecruitRes>(this)->count; ++i) {
            if (stricmp_(jo::self<jo::RecruitRes>(this)->entries[i].name, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("Table::find_b4", 0x004cad00, &H::find_b4);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x004cad80
    // eax is kept exact: the caller loop 0x004ca700 reuses it (mov ax / push eax) and returns its
    // upper half up the chain 0x004ca640 -> 0x004ca5xx.
    void* free_23() {
        void* r = this;
        if (jo::self<jo::RecruitEntry>(this)->rows != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::RecruitEntry>(this)->rows);
            r = p;
            if (p != nullptr) r = joan::orig::arg::as<void*>(joan::orig::as_dynarray(p)->destroy(1u));  // 0x00463fc0
            jo::self<jo::RecruitEntry>(this)->rows = nullptr;
        }
        return r;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("X::free_23", 0x004cad80, &H::free_23);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_4cadf0", 0x004cadf0, &H::nop);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::RecruitRes::release() -> void { joan::bind::member<&::game_b101_06::H::recruit_release>(this); }  // 0x4ca640
auto joan::orig::List::update_all_4ca700() -> void { joan::bind::member<&::game_b100_05::H::update_all_4ca700>(this); }  // 0x4ca700
auto joan::orig::RecruitRes::load() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_04::H::recruit_load>(this)); }  // 0x4ca760
auto joan::orig::Table::find_b4(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_02::H::find_b4>(this, a0)); }  // 0x4cad00
auto joan::orig::RecruitRow::free_23() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_03::H::free_23>(this)); }  // 0x4cad80
