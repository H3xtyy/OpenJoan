// game/sitemres.cpp: functions of the original d:\ww\sitemres.cpp (re/attribution.csv).
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
    void* reset_4ce390() {
        joan::orig::as_sell_item_res(this)->load();  // 0x004ce420
        jo::self<jo::SItemRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Obj::reset_4ce390", 0x004ce390, &H::reset_4ce390);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - List::update_all_4ca700 0x004ca700 / List::update_all_4ce3c0 0x004ce3c0: short index
//      1..[+0] calling 0x004cad80 / 0x004ce980 on element 0x00469520 / 0x00469e60(i).
namespace {
namespace game_b100_05 {

JOAN_COUNTER(c_update_4ce3c0, "List::update_all_4ce3c0");

struct H {
    // ------------------------------------------------------------------ 0x004ce3c0
    void update_all_4ce3c0() {
        c_update_4ce3c0.hit();
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::SItemRes>(this)->count; i = static_cast<i16>(i + 1)) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_sell_item_res(this)->entry_at(joan::orig::arg::i32(i)));  // 0x00469e60
            joan::orig::as_sell_item_row(e)->free_13();                          // 0x004ce980
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("List::update_all_4ce3c0", 0x004ce3c0, &H::update_all_4ce3c0);
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

JOAN_COUNTER(c_sellitem_load, "SellItemRes::load");

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
    jo::SItemRes* const sr = jo::self<jo::SItemRes>(self);
    sr->count = jo::as_res_idx_l(res)->rec_count;
    sr->entries = static_cast<jo::SellItemEntry*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(sr->count) * stride)), joan::orig::arg::str(src), joan::orig::arg::i32(line)));  // 0x0058e7b0
    crt_memset(sr->entries, 0, static_cast<u32>(sr->count) * stride);
    for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
        jo::SellItemEntry* const e = reinterpret_cast<jo::SellItemEntry*>(reinterpret_cast<u8*>(sr->entries) + (static_cast<u32>(i) * stride - stride));
        e->id = static_cast<i16>(i);
        const i32 r1 = joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i));  // 0x0059bdd0
        void* const r2 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
        joan::orig::as_file_txt(item)->construct_mem(joan::orig::arg::str(r2), joan::orig::arg::i32(r1));  // 0x00589780
        thiscall<u32>(record, self, static_cast<void*>(item), static_cast<void*>(e));
        joan::orig::as_file_txt(item)->destruct();  // 0x00589890
    }
    joan::orig::as_archive(res)->dtor();  // 0x00417a30
    return 0;  // void callee; SellItemRes::load's caller drops the eax
}

struct H {
    // ------------------------------------------------------------------ 0x004ce420
    u32 sellitem_load() {
        c_sellitem_load.hit();
        return res_load(this, str(d::s_ITEM_9cf648), str(d::s_SELLITEM_RES), str(d::s_d_ww_sitemres_cpp), static_cast<i32>(global<i16>(d::g_a02e84)) + 0xb,
                        static_cast<u32>(sizeof(jo::SellItemEntry)), fn::code_value(fn::SItemRes_parse));  // 0x004ce630
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("SellItemRes::load", 0x004ce420, &H::sellitem_load);
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
    u32 sellitem_dtor() { joan::orig::as_sell_item_res(this)->release(); return 0; }  // 0x004ce300
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("SellItemRes::dtor", 0x004ce2d0, &H::sellitem_dtor);
#undef B

}  // namespace game_b102_04_2
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
//
//    - SItemRes::release 0x004ce300 (ret 0): as 0x004ca640 with 0x004ce3c0 / sitemres.cpp.
//
//    - SItemRes::parse 0x004ce630 (reader, rec), ret 8: name, "Type" (POTION / WEAPON), a
//      float, then (item, float, int, int) rows until "End" into a 14-byte element list; an int
//      of -1 is replaced by dword +0x36 of item record 0x00425ce0(id).
//
//  - SEH frames in 0x004c8220 / 0x004c8490 / 0x004c9930 / 0x004ca980 / 0x004ce630 only guard
//    the File object or new + ctor: normal path reproduced.
//
//  - 0x004ce630 pushes the item id with eax's upper half still 0xffff (from the atoi -1); the
//    same dword is passed (0x00425ce0 reads only the word).
namespace {
namespace game_b101_06 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::op_new;
using joan::orig::helpers::crt_atoi;
float crt_atof(const char* s) { return joan::orig::arg::as<float>(joan::orig::libc::atof(joan::orig::arg::str(s))); }  // st0 -> fstp dword
char* crt_strncpy(void* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
i32 line_of(img::Addr g, i32 add_) { return static_cast<i32>(global<i16>(g)) + add_; }

// ---------------------------------------------------------------- record readers
char* next_token(void* reader) { return joan::orig::as_file_txt(reader)->get_token(1); }  // 0x00589c70

using joan::orig::ItemRow;

void* new_list(i32 elem) {  // new(0x1d) + 0x005843d0(elem, 0x1e, 0)
    void* const p = op_new(sizeof(jo::DynArray));  // 0x1d on x86
    if (p == nullptr) return nullptr;
    return joan::orig::dynarray(p)->construct(joan::orig::arg::i32(elem), 0x1e, 0);  // 0x005843d0
}
void list_add(void* list, const void* e) { joan::orig::dynarray(list)->linkin(joan::orig::arg::ptr(e)); }  // 0x00584620

JOAN_COUNTER(c_item_parse, "SItemRes::parse");

struct H {
    // ------------------------------------------------------------------------ 0x004ce300
    void sitem_release() {
        if (jo::view<i8>(jo::self<jo::SItemRes>(this)->loaded) != 1) return;
        joan::orig::as_list(this)->update_all_4ce3c0();  // 0x004ce3c0
        if (jo::self<jo::SItemRes>(this)->entries != nullptr) {
            const i32 line = line_of(d::g_a02e80, 7);
            joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SItemRes>(this)->entries)), str(d::s_d_ww_sitemres_cpp), joan::orig::arg::i32(line));  // 0x0058ed70 "sitemres.cpp"
            jo::self<jo::SItemRes>(this)->entries = nullptr;
            jo::view<u32>(jo::self<jo::SItemRes>(this)->count) = 0;
        }
        jo::self<jo::SItemRes>(this)->loaded = 0;
    }

    // ------------------------------------------------------------------------ 0x004ce630
    void sitem_parse(void* reader, u8* rec) {
        c_item_parse.hit();
        char* t = next_token(reader);
        t = next_token(reader);
        crt_atoi(t);  // result unused
        t = next_token(reader);
        crt_strncpy(rec + 2, next_token(reader), 0x10);
        t = next_token(reader);
        jo::as_sell_item_entry(rec)->type = 0;
        if (crt_stricmp(t, str(d::s_Type)) == 0) {  // "Type"
            t = next_token(reader);
            if (crt_stricmp(t, str(d::s_POTION)) == 0) {         // "POTION"
                jo::as_sell_item_entry(rec)->type = 0;
            } else if (crt_stricmp(t, str(d::s_WEAPON)) == 0) {  // "WEAPON"
                jo::as_sell_item_entry(rec)->type = 1;
            }
            t = next_token(reader);
        }
        jo::as_sell_item_entry(rec)->value = crt_atof(next_token(reader));
        jo::view<void*>(jo::as_sell_item_entry(rec)->rows) = new_list(0xe);
        ItemRow row;
        for (;;) {
            t = next_token(reader);
            if (t == nullptr) break;
            if (crt_stricmp(t, str(d::s_End)) == 0) break;  // "End"
            row.item = joan::orig::arg::as<i16>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(t)));  // 0x004a36c0
            t = next_token(reader);
            row.value = crt_atof(t);
            t = next_token(reader);
            row.a = crt_atoi(t);
            t = next_token(reader);
            row.b = crt_atoi(t);
            if (row.b == -1) {
                // push eax: upper half = 0xffff left by the atoi result -1
                const u32 arg = 0xffff0000u | static_cast<u16>(row.item);
                u8* const item = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(arg)));  // 0x00425ce0
                row.b = jo::as_item_def(item)->min_level;
            }
            list_add(jo::view<void*>(jo::as_sell_item_entry(rec)->rows), &row);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("SItemRes::release", 0x004ce300, &H::sitem_release);
B("SItemRes::parse", 0x004ce630, &H::sitem_parse);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    void* zero_9() {  // 0x004ce2a0
        memset_(this, 0, sizeof(jo::SItemRes));  // 9 on x86
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("Struct9::zero", 0x004ce2a0, &H::zero_9);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::orig::helpers::at;

JOAN_COUNTER(c_find_name, "Table::find_name");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x004ce900
    i32 find_name(const char* name) {
        c_find_name.hit();
        for (i32 i = 0; i < jo::self<jo::SItemRes>(this)->count; ++i) {
            if (stricmp_(jo::self<jo::SItemRes>(this)->entries[i].name, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("Table::find_name", 0x004ce900, &H::find_name);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------------ 0x004ce980
    void delete_13() {
        if (jo::self<jo::SellItemEntry>(this)->rows == nullptr) return;
        void* const p = jo::view<void*>(jo::self<jo::SellItemEntry>(this)->rows);
        if (p != nullptr) joan::orig::as_dynarray(p)->destroy(1u);  // 0x00463fc0
        jo::self<jo::SellItemEntry>(this)->rows = nullptr;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::delete_13", 0x004ce980, &H::delete_13);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_4ce9f0", 0x004ce9f0, &H::nop);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SellItemRes::release() -> void { joan::bind::member<&::game_b101_06::H::sitem_release>(this); }  // 0x4ce300
auto joan::orig::List::update_all_4ce3c0() -> void { joan::bind::member<&::game_b100_05::H::update_all_4ce3c0>(this); }  // 0x4ce3c0
auto joan::orig::SellItemRes::load() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_04::H::sellitem_load>(this)); }  // 0x4ce420
auto joan::orig::Table::find_name_4ce900(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_03::H::find_name>(this, a0)); }  // 0x4ce900
auto joan::orig::SellItemRow::free_13() -> void { joan::bind::member<&::game_b99_03::H::delete_13>(this); }  // 0x4ce980
