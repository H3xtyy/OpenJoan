// game/itemres.cpp: functions of the original d:\ww\itemres.cpp (re/attribution.csv).
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
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using u16 = std::uint16_t;

using joan::call::cdecl_call;

void* const kMem = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b107_16.cpp (game/b107_16)
//
//    - ItemRes::release_49f620 0x0049f620 (itemres.cpp): when byte +0x24 == 1 frees +0x18,
//      0x0049f7d0, frees +0x14 (and clears +0), +0xc, +0x10 (0x0058ed70 on 0xa48838 with
//      line [0xa00c7c] + 5 / 0xd / 0x13 / 0x18), 0x005a17d0 on +0x1c, byte +0x24 = 0.
//      Returns this when skipped, else 0x005a17d0's eax (callers 0x00470845 / 0x0049f5db
//      ignore eax).
namespace {
namespace game_b107_16 {

using i8 = std::int8_t;

using joan::orig::helpers::at;

JOAN_COUNTER(c_release_49f620, "ItemRes::release_49f620");

struct H {
    // ------------------------------------------------------------------ 0x0049f620
    template <class T>
    void free_field(T*& field, i32 line_add) {
        const i32 line = static_cast<i32>(global<i16>(d::g_a00c7c)) + line_add;
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(field), str(d::s_d_ww_itemres_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
        field = nullptr;
    }
    std::uintptr_t release_49f620() {  // eax: this, or 0x005a17d0's result (a pointer)
        c_release_49f620.hit();
        if (static_cast<i8>(jo::self<jo::ItemRes>(this)->loaded) != 1) return reinterpret_cast<std::uintptr_t>(this);
        if (jo::self<jo::ItemRes>(this)->sorted_index != nullptr) free_field(jo::self<jo::ItemRes>(this)->sorted_index, 5);
        joan::orig::as_table(this)->update_all();  // 0x0049f7d0
        if (jo::self<jo::ItemRes>(this)->items != nullptr) {
            free_field(jo::self<jo::ItemRes>(this)->items, 0xd);
            jo::view<u32>(jo::self<jo::ItemRes>(this)->item_count) = 0;
        }
        if (jo::self<jo::ItemRes>(this)->types != nullptr) free_field(jo::self<jo::ItemRes>(this)->types, 0x13);
        if (jo::self<jo::ItemRes>(this)->classes != nullptr) free_field(jo::self<jo::ItemRes>(this)->classes, 0x18);
        const std::uintptr_t eax = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_list(&jo::self<jo::ItemRes>(this)->strings)->clear_5a17d0());  // 0x005a17d0
        jo::self<jo::ItemRes>(this)->loaded = 0;
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_16", name, addr, fn)
B("ItemRes::release_49f620", 0x0049f620, &H::release_49f620);
#undef B

}  // namespace game_b107_16
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
//
//    - 0x0049f780 ItemRes::load              (thiscall, ret 0): 0x005a17a0(+0x1c), 0x0049f830,
//      0x0049f970, 0x0049fbb0, byte +0x24 = 1; returns this.
//
//    - 0x0049fdf0 ItemRes::scan_dir          (thiscall(dir), ret 4, recursive): every "*.pro" in
//      dir appends a 0x2bf record (realloc 0x0058eb30, itemres.cpp) loaded by 0x004a0160 from a
//      0x19c-byte reader (0x00589550 / 0x00589890); then recurses into sub-directories other
//      than "." / "..". _findfirst / _findnext / strcpy / strcat / strcmp / memset are the
//      game's CRT (called directly as in the original); the find handles are never closed (as
//      in the original). Returns 0xffffffff (eax of the last _findnext / _findfirst test).
namespace {
namespace game_b108_06 {

using joan::orig::helpers::at;
using joan::orig::helpers::addr;

struct alignas(4) Reader {  // 0x00589550 text reader (FileTxt), [ebp-0x4e0] .. [ebp-0x344]
    u8 b[(sizeof(jo::FileTxt) + 3u) & ~3u];  // 0x19c on x86
};
struct FindData {  // MSVC 6 _finddata_t, [ebp-0x338] .. [ebp-0x220]
    u32 attrib;
    i32 time_create, time_access, time_write;
    u32 size;
    char name[260];
};
static_assert(sizeof(FindData) == 0x118, "FindData");

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_scan_dir, "ItemRes::scan_dir");

// ---------------------------------------------------------------------------- CRT (direct)
using joan::orig::helpers::strcpy_;
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
using joan::orig::helpers::strcmp_;
using joan::orig::helpers::memset_;

struct H {
    std::uintptr_t itemres_load() {  // 0x0049f780 (eax: this)
        joan::orig::as_string_pool(&jo::self<jo::ItemRes>(this)->strings)->reset();  // 0x005a17a0
        joan::orig::as_item_res(this)->load_49f830();             // 0x0049f830
        joan::orig::as_item_res(this)->load_49f970();             // 0x0049f970
        joan::orig::as_item_res(this)->load_49fbb0();             // 0x0049fbb0
        jo::self<jo::ItemRes>(this)->loaded = 1;
        return addr(this);
    }

    // ------------------------------------------------------------------ 0x0049fdf0
    u32 scan_dir(const char* dir) {
        c_scan_dir.hit();
        char pattern[0x108];  // [ebp-0x118]
        FindData fd;          // [ebp-0x338]
        char path[0x108];     // [ebp-0x220] .. [ebp-0x118]
        Reader reader;        // [ebp-0x4e0]
        strcpy_(pattern, dir);
        strcat_(pattern, str(d::s_pro));  // "*.pro"
        i32 h = joan::orig::arg::as<i32>(joan::orig::libc::_findfirst(joan::orig::arg::str(pattern), &fd));  // _findfirst
        i32 rc = 0;
        while (rc != -1 && h != -1) {
            jo::self<jo::ItemRes>(this)->item_count = jo::self<jo::ItemRes>(this)->item_count + 1;
            const i32 line = static_cast<i32>(global<i16>(d::g_a00c8c)) + 0x18;
            jo::view<void*>(jo::self<jo::ItemRes>(this)->items) = joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ItemRes>(this)->items)), joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::self<jo::ItemRes>(this)->item_count) * static_cast<u32>(sizeof(jo::ItemDef)))), str(d::s_d_ww_itemres_cpp), joan::orig::arg::i32(line));  // 0x0058eb30 realloc (itemres.cpp)
            jo::ItemDef* const rec = jo::as_item_def(jo::view<u8*>(jo::self<jo::ItemRes>(this)->items) + static_cast<i32>(static_cast<u32>(jo::self<jo::ItemRes>(this)->item_count) * static_cast<u32>(sizeof(jo::ItemDef))) - static_cast<i32>(sizeof(jo::ItemDef)));
            strcpy_(path, dir);
            strcat_(path, fd.name);
            memset_(rec, 0, sizeof(jo::ItemDef));
            rec->id = static_cast<u16>(jo::self<jo::ItemRes>(this)->item_count);
            joan::orig::as_file_txt(&reader)->construct(joan::orig::arg::str(path));  // 0x00589550
            joan::orig::as_item_res(this)->parse_item(&reader, joan::orig::arg::mptr(rec));  // 0x004a0160
            rc = joan::orig::arg::as<i32>(joan::orig::libc::_findnext(joan::orig::arg::i32(h), &fd));  // _findnext
            joan::orig::as_file_txt(&reader)->destruct();  // 0x00589890
        }
        strcpy_(pattern, dir);
        strcat_(pattern, str(d::s_star));  // "*"
        h = joan::orig::arg::as<i32>(joan::orig::libc::_findfirst(joan::orig::arg::str(pattern), &fd));
        rc = 0;
        while (rc != -1 && h != -1) {
            if ((fd.attrib & 0x10) != 0 && strcmp_(str(d::s_dot_9d36e8), fd.name) != 0 &&
                strcmp_(str(d::s_dot_dot), fd.name) != 0) {  // "." / ".."
                strcpy_(path, dir);
                strcat_(path, fd.name);
                strcat_(path, str(d::s_bslash));  // "\\"
                joan::orig::as_item_res(this)->scan_dir(joan::orig::arg::str(static_cast<const char*>(path)));  // 0x0049fdf0
            }
            rc = joan::orig::arg::as<i32>(joan::orig::libc::_findnext(joan::orig::arg::i32(h), &fd));
        }
        return 0xffffffffu;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("ItemRes::load", 0x0049f780, &H::itemres_load);
B108("ItemRes::scan_dir", 0x0049fdf0, &H::scan_dir);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x0049f7d0 Table::update_all     (thiscall, ret 0) - 0x004a37e0 on entries 1..[+0].
namespace {
namespace game_b99_03 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_update_all, "Table::update_all");

struct H {
    // ------------------------------------------------------------------------ 0x0049f7d0
    void update_all() {
        c_update_all.hit();
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::ItemRes>(this)->item_count; i = static_cast<i16>(i + 1)) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(this)->get(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x00425ce0
            joan::orig::as_item_res(e)->clear();                                             // 0x004a37e0
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("Table::update_all", 0x0049f7d0, &H::update_all);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
//
//      0x0049f830 ItemRes::load                 (thiscall, ret 0) - "ITEMCLAS" table to 0x2c records.
//
//  - 0x00472b00 / 0x0047be60 / 0x0047c250 / 0x0049f830 / 0x004af950: eax as the original leaves it
//    is returned (memcpy result / callee result / loop index).
namespace {
namespace game_b97_02 {

using joan::orig::helpers::at;

void* const kStr = obj(d::g_misc);

JOAN_COUNTER(c_item_load, "ItemRes::load");

void* crt_memset(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // ------------------------------------------------------------------ 0x0049f830
    i32 item_load() {
        c_item_load.hit();
        void* const tbl = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_ITEMCLAS)));  // 0x0058b180 "ITEMCLAS"
        jo::self<jo::ItemRes>(this)->class_count = joan::orig::arg::as<i32>(joan::orig::as_database(tbl)->rec_count());  // 0x00415280
        const i32 line = static_cast<i32>(global<i16>(d::g_a00c80)) + 9;
        const u32 bytes = static_cast<u32>(jo::self<jo::ItemRes>(this)->class_count) * static_cast<u32>(sizeof(jo::ItemClass));
        jo::self<jo::ItemRes>(this)->classes = static_cast<jo::ItemClass*>(joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(static_cast<i32>(bytes)), str(d::s_d_ww_itemres_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        crt_memset(jo::self<jo::ItemRes>(this)->classes, 0, static_cast<u32>(static_cast<u32>(jo::self<jo::ItemRes>(this)->class_count) * static_cast<u32>(sizeof(jo::ItemClass))));
        i32 i = 0;
        for (; i < jo::self<jo::ItemRes>(this)->class_count; ++i) {
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_database(tbl)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::ItemClass* const e = &jo::self<jo::ItemRes>(this)->classes[i];
            e->id = static_cast<u16>(i + 1);
            joan::orig::as_misc(kStr)->copy_field_590210(joan::orig::arg::str(e->name), joan::orig::arg::str(rec), 8);           // 0x00590210
            joan::orig::as_misc(kStr)->copy_field_590210(joan::orig::arg::str(e->desc), joan::orig::arg::str(rec + 8), 0x20);  // 0x00590210
        }
        return i;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("ItemRes::load", 0x0049f830, &H::item_load);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x0049f970 ItemRes::load                (thiscall, ret 0)    - "ITEMTYPE" records.
namespace {
namespace game_b96_02 {

JOAN_COUNTER(c_itemres_load, "ItemRes::load");

using joan::orig::helpers::line_of;
void* mem_alloc(i32 n, img::Addr file, i32 line) {
    return joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(n), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
}

struct H {
    // ------------------------------------------------------------------------ 0x0049f970
    void itemres_load() {
        c_itemres_load.hit();
        void* const kFmt = obj(d::g_misc);
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_ITEMTYPE)));  // 0x0058b180 "ITEMTYPE"
        jo::self<jo::ItemRes>(this)->type_count = joan::orig::arg::as<i32>(joan::orig::as_database(t)->rec_count());  // 0x00415280
        {
            const i32 line = line_of(d::g_a00c84, 9);
            jo::self<jo::ItemRes>(this)->types = static_cast<jo::ItemType*>(mem_alloc(jo::self<jo::ItemRes>(this)->type_count * static_cast<i32>(sizeof(jo::ItemType)), d::s_d_ww_itemres_cpp, line));  // "itemres.cpp"
        }
        std::memset(joan::orig::arg::mptr(jo::self<jo::ItemRes>(this)->types), 0, joan::orig::arg::u32(jo::self<jo::ItemRes>(this)->type_count * static_cast<i32>(sizeof(jo::ItemType))));  // memset
        for (i32 i = 0; i < jo::self<jo::ItemRes>(this)->type_count; ++i) {
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_database(t)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            jo::ItemType* const e = jo::self<jo::ItemRes>(this)->types + i;
            const char* const x = joan::orig::arg::as<const char*>(joan::orig::as_misc(kFmt)->rtrim_field_590170(joan::orig::arg::str(rec), 8));  // 0x00590170
            e->class_id = joan::orig::arg::as<u16>(joan::orig::as_name_table(this)->find_49fab0(joan::orig::arg::str(x)));     // 0x0049fab0
            joan::orig::as_misc(kFmt)->copy_field_590210(joan::orig::arg::str(e->name), joan::orig::arg::str(rec + 8), 0x20);  // 0x00590210
            e->id = static_cast<u16>(i + 1);
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("ItemRes::load", 0x0049f970, &H::itemres_load);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x0049fab0 NameTable::find(const char* name)   ret 4   1-based index of the 0x2c record
//                                                              whose name (+2) matches (0x7c07b0)
//
//      0x004a37e0 ItemRes::clear()                    ret 0   frees +0x249 / +0x251 (itemres.cpp)
namespace {
namespace game_b92_03 {

using joan::orig::helpers::at;

void* const kMemory = obj(d::g_mem);

using joan::orig::helpers::line_base;
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

JOAN_COUNTER(c_itemres_clear, "ItemRes::clear");

struct H {
    // 0x0049fab0
    std::int32_t find_name(const char* name) {
        for (std::int32_t i = 0; i < jo::self<jo::ItemRes>(this)->class_count; ++i) {
            const char* const s = jo::view<char*>(jo::self<jo::ItemRes>(this)->classes) + i * 0x2c + 2;
            if (joan::crt::stricmp(joan::orig::arg::str(s), joan::orig::arg::str(name)) == 0) return i + 1;
        }
        return 0;
    }

    // 0x004a37e0
    void itemres_clear() {
        c_itemres_clear.hit();
        for (std::int32_t i = 0; i < jo::self<jo::ItemDef>(this)->model_count; ++i)
            if (jo::self<jo::ItemDef>(this)->model_names[i] != nullptr) jo::self<jo::ItemDef>(this)->model_names[i] = nullptr;
        for (std::int32_t i = 0; i < jo::self<jo::ItemDef>(this)->model_count; ++i)
            if (jo::self<jo::ItemDef>(this)->anim_names[i] != nullptr) jo::self<jo::ItemDef>(this)->anim_names[i] = nullptr;
        if (jo::self<jo::ItemDef>(this)->description != nullptr) jo::self<jo::ItemDef>(this)->description = nullptr;
        for (std::int32_t i = 0; i < 4; ++i)
            if (jo::self<jo::ItemDef>(this)->emitter_names[i] != nullptr) jo::self<jo::ItemDef>(this)->emitter_names[i] = nullptr;
        if (jo::view<void*>(jo::self<jo::ItemDef>(this)->requirements) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::ItemDef>(this)->requirements), str(d::s_d_ww_itemres_cpp), line_base(d::g_a00c98) + 0x3b);
            jo::view<void*>(jo::self<jo::ItemDef>(this)->requirements) = nullptr;
        }
        jo::self<jo::ItemDef>(this)->requirement_count = 0;
        if (jo::view<void*>(jo::self<jo::ItemDef>(this)->effects) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::ItemDef>(this)->effects), str(d::s_d_ww_itemres_cpp), line_base(d::g_a00c98) + 0x41);
            jo::view<void*>(jo::self<jo::ItemDef>(this)->effects) = nullptr;
        }
        jo::self<jo::ItemDef>(this)->effect_count = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("NameTable::find", 0x0049fab0, &H::find_name);
B("ItemRes::clear", 0x004a37e0, &H::itemres_clear);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x0049fb30 Names::index_of         (thiscall(name), ret 4) - 1-based, _stricmp, stride 0x25.
namespace {
namespace game_b94_02 {

using joan::orig::helpers::at;

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x0049fb30
    i32 names_index_of(const char* name) {
        for (i32 i = 0; i < jo::self<jo::ItemRes>(this)->type_count; ++i) {
            if (stricmp_(jo::self<jo::ItemRes>(this)->types[i].name, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Names::index_of", 0x0049fb30, &H::names_index_of);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
//
//    - ItemRes::load_49fbb0 0x0049fbb0 (ret 0, itemres.cpp): same scheme as UnitRes::load
//      (game_b107_10): "ITEM\\" "ITEM.RES", records of 0x2bf bytes; +0 counts the records.
namespace {
namespace game_b108_01 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_itemres, "ItemRes::load_49fbb0");

struct H {
    // -------------------------------------------------------------- 0x0049fbb0
    u32 itemres_load() {
        c_itemres.hit();
        alignas(4) char res[(sizeof(jo::ResourceIdxL) + 3u) & ~3u];  // 0x138 on x86
        alignas(4) char path[(sizeof(jo::String) + 3u) & ~3u];       // 0x194
        alignas(4) char item[(sizeof(jo::FileTxt) + 3u) & ~3u];      // 0x19c
        joan::orig::as_res_idx(res)->construct();                       // 0x004179f0
        joan::orig::as_string(path)->construct();               // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_ITEM_9cf648));               // 0x005a1440 "ITEM\\"
        joan::orig::as_string(path)->append(str(d::s_ITEM_RES));                   // 0x005a1550 "ITEM.RES"
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(name), static_cast<char>(i32{1}), static_cast<char>(i32{0}));  // 0x0059b920
        const i32 line = static_cast<i32>(global<i16>(d::g_a00c88)) + 0xd;
        jo::self<jo::ItemRes>(this)->items = joan::orig::arg::as<jo::ItemDef*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(jo::as_res_idx_l(res)->rec_count * static_cast<i32>(sizeof(jo::ItemDef))), joan::orig::arg::str(// 0x0058e7b0
                                            str(d::s_d_ww_itemres_cpp)), joan::orig::arg::i32(line)));
        std::memset(joan::orig::arg::mptr(jo::view<u8*>(jo::self<jo::ItemRes>(this)->items)), i32{0}, joan::orig::arg::u32(jo::as_res_idx_l(res)->rec_count * static_cast<i32>(sizeof(jo::ItemDef))));  // memset
        jo::view<u32>(jo::self<jo::ItemRes>(this)->item_count) = 0;
        for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
            jo::ItemDef* const rec = &jo::self<jo::ItemRes>(this)->items[i - 1];
            rec->id = static_cast<u16>(i);
            const u32 a = joan::orig::arg::as<u32>(joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
            char* const b = joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i));  // 0x0059bf70
            joan::orig::as_file_txt(item)->construct_mem(joan::orig::arg::str(b), joan::orig::arg::i32(a));  // 0x00589780
            joan::orig::as_item_res(this)->parse_item(joan::orig::arg::mptr(static_cast<void*>(item)), joan::orig::arg::mptr(rec));  // 0x004a0160
            joan::orig::as_file_txt(item)->destruct();              // 0x00589890
            jo::view<u32>(jo::self<jo::ItemRes>(this)->item_count) = jo::view<u32>(jo::self<jo::ItemRes>(this)->item_count) + 1;
        }
        joan::orig::as_item_res(this)->scan_dir(str(d::s_ITEM_9cf648));  // 0x0049fdf0
        joan::orig::as_item_res(this)->sort_index();                   // 0x004a3590
        joan::orig::as_archive(res)->dtor();             // 0x00417a30
        return 0;  // 0x00417a30 returns nothing; the only caller discards the result
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("ItemRes::load_49fbb0", 0x0049fbb0, &H::itemres_load);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b106_02.cpp (game/b106_02)
//
//      0x004a0160 ItemRes::parse_item(stream, item)   (thiscall, ret 8) - reads one item
//                 definition from the token stream (next_token 0x00589c70, read_line
//                 0x00589f10): fixed header (kind, class, names, model, icon, description,
//                 structure type, kind-specific animation / model tables, stats), then a
//                 keyword loop (Damage .. ShowArmoryInterface) until the stream is empty.
//                 Returns 0 (the original leaves eax = the null token).
//
//  - 0x004a0160 HasLight: the original writes buf[strlen(buf) - 1] = 0; for an empty line
//    that is buf[-1], a byte of a dead float local - the buffer here has a spare leading
//    byte for it.
namespace {
namespace game_b106_02 {

using joan::hook::x87::Fpu;

using joan::orig::helpers::at;

// CRT
using joan::orig::helpers::crt_atoi;
double crt_atof(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
bool same(const char* a, img::Addr b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(str(b))) == 0; }  // _stricmp
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
void crt_strcpy(char* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void crt_strcat(char* d, const char* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void crt_memset(void* d, i32 v, u32 n) { std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_parse_item, "ItemRes::parse_item");

// ---------------------------------------------------------------------------------------
// 0x004a0160 item parser
struct ItemParser {
    void* self;    // the resource manager (ecx)
    void* stream;  // token stream
    u8* it;        // item record
    char line_buf[0x104];  // ebp-0x3c0

    char* next() { return joan::orig::as_file_txt(stream)->get_token(1); }  // 0x00589c70
    void read_line(char* buf, i32 n) { joan::orig::as_file_txt(stream)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); }  // 0x00589f10
    char* intern(const char* s) {  // 0x005a18d0 on self+0x1c
        return joan::orig::arg::as<char*>(joan::orig::as_string_pool(&jo::as_item_res(self)->strings)->add(joan::orig::arg::str(s), joan::orig::arg::i32(-1)));
    }
    char* line_intern() {
        read_line(line_buf, 0x100);
        return intern(line_buf);
    }
    float next_atof() {  // next(); fstp dword [..] of atof(next())
        next();
        return static_cast<float>(crt_atof(next()));
    }
    i32 next_atoi() {  // next(); atoi(next())
        next();
        return crt_atoi(next());
    }
    static bool flag(i32 v) { return v != 0; }  // neg / sbb / neg

    // Animation / model entry i (kinds 1, 2/3/4 and 10/18).
    void model_entry(i32 i, char*& tok, bool extended) {
        if (same(tok, d::s_AnimName)) {  // AnimName
            jo::as_item_def(it)->anim_names[i] = line_intern();
            jo::as_item_def(it)->model_kinds[i] = 0;
            tok = next();
        } else if (same(tok, d::s_ModelName)) {  // ModelName
            jo::as_item_def(it)->model_kinds[i] = 1;
            jo::as_item_def(it)->model_names[i] = line_intern();
            tok = next();
            if (same(tok, d::s_AnimName)) {  // AnimName
                jo::as_item_def(it)->anim_names[i] = line_intern();
                jo::as_item_def(it)->model_kinds[i] = 2;
                tok = next();
            }
        } else if (extended && same(tok, d::s_AddModelName)) {  // AddModelName
            jo::as_item_def(it)->model_kinds[i] = 3;
            jo::as_item_def(it)->model_names[i] = line_intern();
            tok = next();
        } else if (extended && same(tok, d::s_ModelName2)) {  // ModelName2
            jo::as_item_def(it)->model_kinds[i] = 4;
            jo::as_item_def(it)->model_names[i] = line_intern();
            tok = next();
        }
    }
    void clear_tables(bool with_67) {
        if (with_67) crt_memset(jo::as_item_def(it)->model_values, 0, static_cast<u32>(jo::as_item_def(it)->model_count) * static_cast<u32>(sizeof(jo::as_item_def(it)->model_values[0])));
        crt_memset(jo::as_item_def(it)->model_names, 0, static_cast<u32>(jo::as_item_def(it)->model_count) * static_cast<u32>(sizeof(jo::as_item_def(it)->model_names[0])));
        crt_memset(jo::as_item_def(it)->anim_names, 0, static_cast<u32>(jo::as_item_def(it)->model_count) * static_cast<u32>(sizeof(jo::as_item_def(it)->anim_names[0])));
        crt_memset(jo::as_item_def(it)->model_kinds, 0, static_cast<u32>(jo::as_item_def(it)->model_count));
    }

    void header(char* path) {
        jo::as_item_def(it)->can_drop = 1;
        jo::as_item_def(it)->can_sell = 1;
        char* tok = next();
        tok = next();
        crt_atoi(tok);  // result dead
        tok = next();
        jo::as_item_def(it)->class_id = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_name_table(self)->find_49fab0(joan::orig::arg::str(next()))));  // 0x0049fab0
        tok = next();
        char kind_buf[0x24];  // ebp-0x30
        read_line(kind_buf, 0x20);
        jo::as_item_def(it)->type_id = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_names(self)->index_of(joan::orig::arg::str(kind_buf))));  // 0x0049fb30
        tok = next();
        char name_buf[0x84];  // ebp-0xb4
        read_line(name_buf, 0x80);
        jo::as_item_def(it)->name = intern(name_buf);
        crt_strcpy(path, str(d::s_ITEM_9cf648));  // "ITEM\"
        char* const slash = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(jo::as_item_def(it)->name), 0x5c));  // strrchr
        if (slash != nullptr) {
            joan::orig::libc::strncat(joan::orig::arg::str(path), joan::orig::arg::str(jo::as_item_def(it)->name), joan::orig::arg::u32(static_cast<u32>(slash - jo::as_item_def(it)->name)));
            const u32 n = crt_strlen(str(d::s_ITEM_9cf648));
            const u32 idx = static_cast<u32>(slash - jo::as_item_def(it)->name) + n;
            path[idx] = 0;
            crt_strcat(path, str(d::s_bslash));  // "\"
        }
        tok = next();
        char icon_buf[0x104];  // ebp-0x2bc
        read_line(icon_buf, 0x100);
        jo::view<u16>(jo::as_item_def(it)->icon_sprite) = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_model_res(obj(d::g_model_res_mgr))->add(joan::orig::arg::str(path), joan::orig::arg::str(icon_buf), 0)));  // 0x004b18d0
        tok = next();
        tok = next();
        if (!same(tok, d::s_none)) {  // "none"
            jo::view<u16>(jo::as_item_def(it)->model_sprite) = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_model_res(obj(d::g_model_res_mgr))->add(str(d::s_MODEL_9d3750), joan::orig::arg::str(tok), 0)));
        } else {
            jo::view<u16>(jo::as_item_def(it)->model_sprite) = 0;
        }
        tok = next();
        read_line(line_buf, 0x100);
        jo::as_item_def(it)->bone = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::bone_id_of_name(joan::orig::arg::str(line_buf))));  // 0x00423220
        tok = next();
        if (same(tok, d::s_Description)) {  // Description
            char desc[0x104];  // ebp-0x4c4
            read_line(desc, 0x100);
            for (;;) {
                char* const p = joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(desc), str(d::s_n)));  // strstr "\n"
                if (p == nullptr) break;
                *p = 0xa;
                const u32 len = crt_strlen(p + 2);
                std::memcpy(joan::orig::arg::mptr(p + 1), joan::orig::arg::ptr(p + 2), joan::orig::arg::u32(len + 1));
            }
            jo::as_item_def(it)->description = intern(desc);
            tok = next();
        }
        if (same(tok, d::s_StructionType)) {  // StructionType
            tok = next();
            if (same(tok, d::s_TOWER)) jo::as_item_def(it)->struction_type = 1;         // TOWER
            else if (same(tok, d::s_WALL)) jo::as_item_def(it)->struction_type = 2;    // WALL
            else if (same(tok, d::s_GATE)) jo::as_item_def(it)->struction_type = 3;    // GATE
            else if (same(tok, d::s_WOODGATE)) jo::as_item_def(it)->struction_type = 4;    // WOODGATE
            else if (same(tok, d::s_WOODDOOR)) jo::as_item_def(it)->struction_type = 5;    // WOODDOOR
            else if (same(tok, d::s_TRUNK)) jo::as_item_def(it)->struction_type = 6;    // TRUNK
            tok = next();
        }
        read_line(line_buf, 0x100);
        char* const base = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(line_buf), 0x5c));  // strrchr
        if (base == nullptr) jo::as_item_def(it)->file_name = intern(line_buf);
        else jo::as_item_def(it)->file_name = intern(base + 1);
    }

    void kind_tables(const char* path) {
        char* tok;
        if (jo::view<i16>(jo::as_item_def(it)->type_id) == 0x25) {
            tok = next();
            tok = next();
            jo::as_item_def(it)->model_count = crt_atoi(tok);
            tok = next();
            jo::as_item_def(it)->anim_names[0] = line_intern();
            tok = next();
        } else if (jo::view<i16>(jo::as_item_def(it)->class_id) == 6) {
            tok = next();
            tok = next();
            jo::as_item_def(it)->model_count = crt_atoi(tok);
            tok = next();
            for (i32 i = 0; i < jo::as_item_def(it)->model_count; ++i) {
                jo::as_item_def(it)->model_names[i] = line_intern();
                joan::orig::as_model_res(obj(d::g_model_res_mgr))->add(joan::orig::arg::str(path), joan::orig::arg::str(jo::as_item_def(it)->model_names[i]), 0);  // 0x004b18d0
                tok = next();
            }
        } else if (jo::view<i16>(jo::as_item_def(it)->class_id) == 1) {
            tok = next();
            tok = next();
            jo::as_item_def(it)->i32_5e = crt_atoi(tok);
            tok = next();
            if (same(tok, d::s_notBlock)) {  // notBlock
                tok = next();
                jo::as_item_def(it)->not_block = crt_atoi(tok);
                tok = next();
            }
            if (same(tok, d::s_NeedKey)) {  // NeedKey
                tok = next();
                jo::as_item_def(it)->need_key = static_cast<u8>(crt_atoi(tok));
                tok = next();
            }
            tok = next();
            jo::as_item_def(it)->model_count = crt_atoi(tok);
            if (jo::as_item_def(it)->model_count != 0) {
                clear_tables(true);
                tok = next();
                for (i32 i = 0; i < jo::as_item_def(it)->model_count; ++i) {
                    tok = next();
                    jo::as_item_def(it)->model_values[i] = crt_atoi(tok);
                    tok = next();
                    model_entry(i, tok, false);
                }
            }
        } else if (jo::view<i16>(jo::as_item_def(it)->class_id) == 2 || jo::view<i16>(jo::as_item_def(it)->type_id) == 4 || jo::view<i16>(jo::as_item_def(it)->class_id) == 3) {
            tok = next();
            tok = next();
            jo::as_item_def(it)->i32_5e = crt_atoi(tok);
            tok = next();
            tok = next();
            jo::as_item_def(it)->model_count = crt_atoi(tok);
            if (jo::as_item_def(it)->model_count != 0) {
                clear_tables(true);
                tok = next();
                for (i32 i = 0; i < jo::as_item_def(it)->model_count; ++i) {
                    tok = next();
                    jo::as_item_def(it)->model_values[i] = crt_atoi(tok);
                    tok = next();
                    model_entry(i, tok, true);
                }
            }
        } else if (jo::view<i16>(jo::as_item_def(it)->class_id) == 0xa || jo::view<i16>(jo::as_item_def(it)->class_id) == 0x12) {
            tok = next();
            if (same(tok, d::s_NeedKey)) {  // NeedKey
                tok = next();
                jo::as_item_def(it)->need_key = static_cast<u8>(crt_atoi(tok));
                tok = next();
            }
            tok = next();
            jo::as_item_def(it)->model_count = crt_atoi(tok);
            clear_tables(false);
            tok = next();
            for (i32 i = 0; i < jo::as_item_def(it)->model_count; ++i) model_entry(i, tok, false);
        } else if (jo::view<i16>(jo::as_item_def(it)->class_id) == 7) {
            tok = next();
            tok = next();
            jo::as_item_def(it)->i32_23d = crt_atoi(tok);
            tok = next();
        } else {
            tok = next();
        }
    }

    void stats() {
        crt_memset(jo::as_item_def(it)->slot_refs, 0, (static_cast<u32>(jo::as_item_def(it)->model_count) + 1u) * static_cast<u32>(sizeof(jo::as_item_def(it)->slot_refs[0])));
        char* tok = next();
        jo::as_item_def(it)->stats[0] = static_cast<u16>(crt_atoi(tok));
        jo::as_item_def(it)->stats[1] = static_cast<u16>(next_atoi());
        jo::as_item_def(it)->stats[2] = static_cast<u16>(next_atoi());
        jo::as_item_def(it)->stats[3] = static_cast<u16>(next_atoi());
        jo::as_item_def(it)->stats[4] = static_cast<u16>(next_atoi());
        tok = next();
        if (same(tok, d::s_Price)) {  // Price
            tok = next();
            jo::as_item_def(it)->price = crt_atoi(tok);
            tok = next();
        }
        tok = next();
        jo::as_item_def(it)->i32_32 = crt_atoi(tok);
        if (jo::view<i16>(jo::as_item_def(it)->class_id) == 6) {
            jo::as_item_def(it)->i32_42 = next_atoi();
            tok = next();
            tok = next();
            if (same(tok, d::s_BLUE)) jo::as_item_def(it)->colour = 0;       // BLUE
            else if (same(tok, d::s_GREEN)) jo::as_item_def(it)->colour = 1;  // GREEN
            else if (same(tok, d::s_YELLOW)) jo::as_item_def(it)->colour = 2;  // YELLOW
        }
    }

    // One keyword of the main loop (0x004a12d0 .. 0x004a2b0c).
    void keyword(char* tok) {
        if (same(tok, d::s_Damage)) {  // Damage
            jo::as_item_def(it)->damage = crt_atoi(next());
        } else if (same(tok, d::s_IncDamage)) {  // IncDamage
            jo::as_item_def(it)->inc_damage = crt_atoi(next());
        } else if (same(tok, d::s_DamageType)) {  // DamageType
            jo::view<u32>(jo::as_item_def(it)->damage_type) = joan::orig::arg::as<u32>(joan::orig::as_stats(self)->stat_id(joan::orig::arg::str(next())));  // 0x004a45f0
        } else if (same(tok, d::s_MinLevel)) {  // MinLevel
            jo::as_item_def(it)->min_level = crt_atoi(next());
        } else if (same(tok, d::s_MaxLevel)) {  // MaxLevel
            jo::as_item_def(it)->max_level = crt_atoi(next());
        } else if (same(tok, d::s_Defense)) {  // Defense
            jo::as_item_def(it)->defense = crt_atoi(next());
        } else if (same(tok, d::s_Duration)) {  // Duration
            jo::as_item_def(it)->duration = crt_atoi(next());
        } else if (same(tok, d::s_BlurVertexCtr)) {  // BlurVertexCtr
            jo::view<u16>(jo::as_item_def(it)->blur_vertex_count) = static_cast<u16>(crt_atoi(next()));
            if (jo::as_item_def(it)->blur_vertex_count != 0) {
                next();
                char blur[0x24];  // ebp-0x500
                read_line(blur, 0x20);
// strncpy of 0x20 bytes from the 0x24-byte line buffer, as the original does.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
                std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_item_def(it)->blur_name)), joan::orig::arg::str(blur), 0x20u);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
                for (i32 i = 0; i < jo::as_item_def(it)->blur_vertex_count; ++i) {
                    jo::as_item_def(it)->blur_vertices[i * 3] = next_atof();
                    jo::as_item_def(it)->blur_vertices[i * 3 + 1] = next_atof();
                    jo::as_item_def(it)->blur_vertices[i * 3 + 2] = next_atof();
                }
            }
        } else if (same(tok, d::s_HasEmitter)) {  // HasEmitter
            if (crt_atoi(next()) != 0) {
                for (i32 i = 0; i < 4; ++i) {
                    next();
                    read_line(line_buf, 0x100);
                    if (!same(line_buf, d::s_NONE_9d4350)) jo::as_item_def(it)->emitter_names[i] = intern(line_buf);  // NONE
                    jo::as_item_def(it)->emitter_pos[i * 3] = next_atof();
                    jo::as_item_def(it)->emitter_pos[i * 3 + 1] = next_atof();
                    jo::as_item_def(it)->emitter_pos[i * 3 + 2] = next_atof();
                    jo::as_item_def(it)->emitter_scale[i] = next_atof();
                    jo::as_item_def(it)->emitter_flags[i] = static_cast<u8>(next_atoi());
                }
            }
        } else if (same(tok, d::s_NeedCollision)) {  // NeedCollision
            jo::as_item_def(it)->need_collision = static_cast<u16>(crt_atoi(next()));
        } else if (same(tok, d::s_CanTransparent)) {  // CanTransparent
            jo::as_item_def(it)->can_transparent = flag(crt_atoi(next()));
        } else if (same(tok, d::s_ModifyCamera)) {  // ModifyCamera
            jo::as_item_def(it)->modify_camera = flag(crt_atoi(next()));
        } else if (same(tok, d::s_HasAntiPortal)) {  // HasAntiPortal
            jo::as_item_def(it)->has_anti_portal = flag(crt_atoi(next()));
        } else if (same(tok, d::s_SkipIntersectWithItem)) {  // SkipIntersectWithItem
            jo::as_item_def(it)->skip_intersect_with_item = flag(crt_atoi(next()));
        } else if (same(tok, d::s_AntiPortalPercentage)) {  // AntiPortalPercentage
            jo::as_item_def(it)->anti_portal_percentage = static_cast<u16>(crt_atoi(next()));
        } else if (same(tok, d::s_EnlargeSceneSector)) {  // EnlargeSceneSector
            jo::as_item_def(it)->enlarge_scene_sector = flag(crt_atoi(next()));
        } else if (same(tok, d::s_Pickable)) {  // Pickable
            jo::as_item_def(it)->pickable = static_cast<u8>(crt_atoi(next()));
        } else if (same(tok, d::s_ItemUsage)) {  // ItemUsage
            char* const v = next();
            if (same(v, d::s_BOTH)) jo::as_item_def(it)->item_usage = 0;       // BOTH
            else if (same(v, d::s_MAIN)) jo::as_item_def(it)->item_usage = 2;  // MAIN
            else if (same(v, d::s_AI)) jo::as_item_def(it)->item_usage = 1;  // AI
        } else if (same(tok, d::s_TerrainSound)) {  // TerrainSound
            jo::as_item_def(it)->terrain_sound = static_cast<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(next()));  // 0x004d12f0
            jo::as_item_def(it)->terrain_sound_arg = next_atoi();
        } else if (same(tok, d::s_Sound)) {  // Sound
            jo::as_item_def(it)->sounds[0] = static_cast<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(next()));  // 0x004d12f0
        } else if (same(tok, d::s_Sound2)) {  // Sound2
            jo::as_item_def(it)->sounds[1] = static_cast<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(next()));  // 0x004d12f0
        } else if (same(tok, d::s_Sound3)) {  // Sound3
            jo::as_item_def(it)->sounds[2] = static_cast<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(next()));  // 0x004d12f0
        } else if (same(tok, d::s_HasBoundingBox)) {  // HasBoundingBox
            jo::view<u16>(jo::as_item_def(it)->has_bounding_box) = static_cast<u16>(crt_atoi(next()));
            if (jo::as_item_def(it)->has_bounding_box != 0) {
                for (i32 k = 0; k < 6; ++k) jo::as_item_def(it)->bounding_box[k] = next_atof();
            }
        } else if (same(tok, d::s_CollisionType)) {  // CollisionType
            jo::as_item_def(it)->collision_type = joan::orig::arg::as<u32>(joan::orig::as_item_res(self)->shape_from_name(joan::orig::arg::str(next())));  // 0x004a4540
            const u32 type = jo::as_item_def(it)->collision_type;
            i32 first = 0, last = -1;
            if (type == 1) first = 0, last = 3;
            else if (type == 2) first = 4, last = 9;
            else if (type == 3) first = 10, last = 14;
            for (i32 k = first; k <= last; ++k) jo::as_item_def(it)->collision_params[k] = static_cast<float>(next_atoi());
        } else if (same(tok, d::s_HasLight)) {  // HasLight
            has_light();
        } else if (same(tok, d::s_HasLight2d)) {  // HasLight2d
            jo::as_item_def(it)->has_light2d = static_cast<u8>(crt_atoi(next()));
            jo::as_item_def(it)->light_kind = next_atoi();
            jo::as_item_def(it)->light[0] = next_atof();
            jo::as_item_def(it)->light[1] = next_atof();
            jo::as_item_def(it)->light[2] = next_atof();
        } else if (same(tok, d::s_RequiredLevel)) {  // RequiredLevel
            jo::as_item_def(it)->required_level = crt_atoi(next());
        } else if (same(tok, d::s_EffectiveRequirementType)) {  // EffectiveRequirementType
            const i32 line = static_cast<i32>(global<i16>(d::g_a00c90)) + 0x393;
            const u32 size = (jo::view<u32>(jo::as_item_def(it)->requirement_count) + 1u) << 4;
            jo::as_item_def(it)->requirements = joan::orig::arg::as<jo::ItemRequirement*>(joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::as_item_def(it)->requirements), joan::orig::arg::u32(size), str(d::s_d_ww_itemres_cpp), joan::orig::arg::i32(line)));  // 0x0058eb30
            const u32 v0 = joan::orig::arg::as<u32>(joan::orig::as_stats(self)->stat_id(joan::orig::arg::str(next())));  // 0x004a45f0
            jo::view<u32>(jo::as_item_requirement(jo::view<u8*>(jo::as_item_def(it)->requirements) + static_cast<i32>(jo::view<u32>(jo::as_item_def(it)->requirement_count) << 4))->kind) = v0;
            const i32 v1 = next_atoi();
            jo::as_item_requirement(jo::view<u8*>(jo::as_item_def(it)->requirements) + static_cast<i32>(jo::view<u32>(jo::as_item_def(it)->requirement_count) << 4))->base = v1;
            const i32 v2 = next_atoi();
            jo::as_item_requirement(jo::view<u8*>(jo::as_item_def(it)->requirements) + static_cast<i32>(jo::view<u32>(jo::as_item_def(it)->requirement_count) << 4))->per_level = v2;
            next();
            char* const w = next();
            if (same(w, d::s_BELOW)) {  // BELOW
                jo::view<u32>(jo::as_item_requirement(jo::view<u8*>(jo::as_item_def(it)->requirements) + static_cast<i32>(jo::view<u32>(jo::as_item_def(it)->requirement_count) << 4))->is_min) = 0;
            } else if (same(w, d::s_OVER)) {  // OVER
                jo::view<u32>(jo::as_item_requirement(jo::view<u8*>(jo::as_item_def(it)->requirements) + static_cast<i32>(jo::view<u32>(jo::as_item_def(it)->requirement_count) << 4))->is_min) = 1;
            }
            jo::view<u32>(jo::as_item_def(it)->requirement_count) = jo::view<u32>(jo::as_item_def(it)->requirement_count) + 1;
        } else if (same(tok, d::s_AdditionalEffectType)) {  // AdditionalEffectType
            const i32 line = static_cast<i32>(global<i16>(d::g_a00c90)) + 0x3b2;
            const i32 size = (jo::as_item_def(it)->effect_count + 1) * 0xc;
            jo::as_item_def(it)->effects = joan::orig::arg::as<jo::ItemModifier*>(joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::as_item_def(it)->effects), joan::orig::arg::u32(size), str(d::s_d_ww_itemres_cpp), joan::orig::arg::i32(line)));  // 0x0058eb30
            const u32 v0 = joan::orig::arg::as<u32>(joan::orig::as_stats(self)->stat_id(joan::orig::arg::str(next())));  // 0x004a45f0
            jo::view<u32>(jo::as_item_modifier(jo::view<u8*>(jo::as_item_def(it)->effects) + jo::as_item_def(it)->effect_count * 0xc)->kind) = v0;
            const i32 v1 = next_atoi();
            jo::as_item_modifier(jo::view<u8*>(jo::as_item_def(it)->effects) + jo::as_item_def(it)->effect_count * 0xc)->base = v1;
            const i32 v2 = next_atoi();
            jo::as_item_modifier(jo::view<u8*>(jo::as_item_def(it)->effects) + jo::as_item_def(it)->effect_count * 0xc)->per_level = v2;
            jo::as_item_def(it)->effect_count = jo::as_item_def(it)->effect_count + 1;
        } else if (same(tok, d::s_UsableUnit)) {  // UsableUnit
            jo::as_item_def(it)->usable_unit = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(next()))));  // 0x005270b0
        } else if (same(tok, d::s_DetailDescriptionFilename)) {  // DetailDescriptionFilename
            char* const v = next();
            if (!same(v, d::s_none)) crt_strcpy(jo::as_item_def(it)->str_25f, v);  // "none"
        } else if (same(tok, d::s_DetailDescriptionNation)) {  // DetailDescriptionNation
            crt_strcpy(jo::as_item_def(it)->str_27f, next());
        } else if (same(tok, d::s_UniqueItem)) {  // UniqueItem
            jo::as_item_def(it)->unique_item = static_cast<u8>(crt_atoi(next()));
        } else if (same(tok, d::s_CanDrop)) {  // CanDrop
            jo::as_item_def(it)->can_drop = static_cast<u8>(crt_atoi(next()));
        } else if (same(tok, d::s_CanSell)) {  // CanSell
            jo::as_item_def(it)->can_sell = static_cast<u8>(crt_atoi(next()));
        } else if (same(tok, d::s_CombineItem)) {  // CombineItem
            jo::as_item_def(it)->combine_item = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(next()))));  // 0x004a36c0
        } else if (same(tok, d::s_ResultItem)) {  // ResultItem
            jo::as_item_def(it)->result_item = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(next()))));
        } else if (same(tok, d::s_HasPath)) {  // HasPath
            jo::as_item_def(it)->has_path = static_cast<u8>(crt_atoi(next()));
        } else if (same(tok, d::s_OpenWithInteract)) {  // OpenWithInteract
            jo::as_item_def(it)->open_with_interact = static_cast<u8>(crt_atoi(next()));
        } else if (same(tok, d::s_MessageName)) {  // MessageName
            jo::as_item_def(it)->message_name = line_intern();
        } else if (same(tok, d::s_ShowArmoryInterface)) {  // ShowArmoryInterface
            jo::as_item_def(it)->show_armory = static_cast<u8>(crt_atoi(next()));
        }
    }

    void has_light() {
        jo::as_item_def(it)->sound_params[0] = static_cast<u8>(crt_atoi(next()));
        jo::as_item_def(it)->sound_params[1] = static_cast<u8>(next_atoi());
        jo::as_item_def(it)->sound_params[3] = static_cast<u8>(next_atoi());
        jo::as_item_def(it)->sound_params[4] = static_cast<u8>(next_atoi());
        jo::as_item_def(it)->sound_params[5] = static_cast<u8>(next_atoi());
        jo::as_item_def(it)->sound_params[6] = static_cast<u8>(next_atoi());
        const Fpu fpu{true};
        float scale = 1.0f;
        if (same(next(), d::s_HD)) scale = 0.0174532924f;  // "HD": 0x3c8efa35
        jo::as_item_def(it)->sound_floats[3] = static_cast<float>(fpu.mul(crt_atof(next()), scale));
        float scale2 = 1.0f;
        if (same(next(), d::s_VD)) scale2 = 0.0174532924f;  // "VD"
        jo::as_item_def(it)->sound_floats[4] = static_cast<float>(fpu.mul(crt_atof(next()), scale2));
        jo::as_item_def(it)->sound_floats[0] = next_atof();
        jo::as_item_def(it)->sound_floats[1] = next_atof();
        jo::as_item_def(it)->sound_floats[2] = next_atof();
        jo::as_item_def(it)->sound_floats[5] = next_atof();
        jo::as_item_def(it)->sound_floats[6] = next_atof();
        jo::as_item_def(it)->sound_floats[7] = next_atof();
        jo::as_item_def(it)->sound_floats[8] = next_atof();
        next();
        char raw[0x45];  // ebp-0x615 .. : raw[0] stands for the dead byte below the buffer
        char* const buf = raw + 1;  // ebp-0x614
        read_line(buf, 0x40);
        const u32 len = crt_strlen(buf);
        buf[static_cast<i32>(len) - 1] = 0;
        jo::as_item_def(it)->sound_params[2] = static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::sound::attenuation_of_name(joan::orig::arg::str(buf))));  // 0x004b03e0
    }
};

struct H {
    u32 parse_item(void* stream, void* item) {  // 0x004a0160
        c_parse_item.hit();
        ItemParser p{this, stream, static_cast<u8*>(item), {}};
        char path[0x104];  // ebp-0x1b8
        p.header(path);
        p.kind_tables(path);
        p.stats();
        for (;;) {
            char* const tok = p.next();
            if (tok == nullptr) return 0;
            p.keyword(tok);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_02", name, addr, fn)
B("ItemRes::parse_item", 0x004a0160, &H::parse_item);
#undef B

}  // namespace game_b106_02
}  // namespace

// ---------------------------------------- from modules/game_objects1.cpp (game/objects1)
//
//  * module game/objects1:
//      GroundObject::update_position 0x004fee60 (after +0xe changed: re-files the object in its
//        map cell / scene, moves the attached effects, then places the child units of its type
//        record on the terrain),
//      GroundObject::set_position 0x004fedf0, GroundObject::id 0x004ff830,
//      GroundObject::heading 0x004ff850, Placements::find 0x004a2b20,
//      Terrain::height 0x0066cae0, (Scene::query_box 0x005ef610 is in game/leaves80b),
//      Sky::ray_clear 0x004d30f0 (true when nothing in the scene blocks the segment from the
//        camera to a point; called by the sun flare in game_overlays.cpp).
namespace {
namespace game_objects1 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_find, "Placements::find");

// 0x004a2b20 Placements::find(base, count, id): the first 0x34-byte entry whose +0 or +4 is id.
std::uint8_t* JOAN_CDECL placements_find(std::uint8_t* base, std::int32_t count, jo::PathSpace* key) {
    c_find.hit();
    if (key == nullptr) return nullptr;
    std::uint8_t* e = base;
    for (std::int32_t i = 0; i < count; ++i, e += sizeof(jo::RWorldCell)) {
        if (jo::as_rworld_cell(e)->key_a == key || jo::as_rworld_cell(e)->key_b == key) return e;
    }
    return nullptr;
}

#define OB1(name, addr, fn) JOAN_REPLACE("game/objects1", name, addr, fn)
OB1("Placements::find", 0x004a2b20, &placements_find);
#undef OB1

}  // namespace game_objects1
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
//
//      0x004a3590 ItemRes::sort_index           (thiscall, ret 0) - qsort with 0x004a3650.
//
//  - 0x004a3590: void (qsort leaves eax undefined).
namespace {
namespace game_b98_02 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_sort_index, "ItemRes::sort_index");

struct H {
    // ------------------------------------------------------------------ 0x004a3590
    void sort_index() {
        c_sort_index.hit();
        const i32 line = static_cast<i32>(global<i16>(d::g_a00c94)) + 2;
        jo::view<void*>(jo::self<jo::ItemRes>(this)->sorted_index) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ItemRes>(this)->sorted_index)), joan::orig::arg::u32(jo::self<jo::ItemRes>(this)->item_count << 2), str(d::s_d_ww_itemres_cpp), joan::orig::arg::i32(line));  // 0x0058eb30
        for (i32 i = 0; i < jo::self<jo::ItemRes>(this)->item_count; ++i) jo::self<jo::ItemRes>(this)->sorted_index[i].value = i + 1;
        global<void*>(d::g_item_res_sort_owner) = this;
        joan::orig::libc::qsort(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ItemRes>(this)->sorted_index)), joan::orig::arg::u32(jo::self<jo::ItemRes>(this)->item_count), 4u, fn::code_ptr(fn::ItemRes_compare_4a3650));  // qsort
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("ItemRes::sort_index", 0x004a3590, &H::sort_index);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b103_03.cpp (game/b103_03)
//
//    - ItemRes::compare_4a3650 0x004a3650 (cdecl qsort comparator, short ids -> record +0x10 names,
//      0x007c07b0); ItemRes::compare_key_4a3790 0x004a3790 (cdecl bsearch comparator).
//
//    - Obj::clear_4a39b0 0x004a39b0 (ret 0, returns this): +0x1e9 = 0, +0x18 = 0.
namespace {
namespace game_b103_03 {

using joan::orig::helpers::at;

// ---- cdecl comparators ----
// 0x004a3650: qsort comparator, elements hold a short item id.
i32 JOAN_CDECL compare_4a3650(const i16* a, const i16* b) {
    jo::ItemDef* const ra = jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(global<void*>(d::g_item_res_sort_owner))->get(joan::orig::arg::i16(*a))));  // 0x00425ce0
    jo::ItemDef* const rb = jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(global<void*>(d::g_item_res_sort_owner))->get(joan::orig::arg::i16(*b))));  // 0x00425ce0
    return joan::crt::stricmp(joan::orig::arg::str(ra->name), joan::orig::arg::str(rb->name));
}
// 0x004a3790: bsearch comparator (key name, element with a short item id).
i32 JOAN_CDECL compare_key_4a3790(const char* key, const i16* elem) {  // key: the name (bsearch passes the pointer)
    jo::ItemDef* const r = jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(global<void*>(d::g_item_res_sort_owner))->get(joan::orig::arg::i16(*elem))));  // 0x00425ce0
    return joan::crt::stricmp(key, joan::orig::arg::str(r->name));
}

struct H {
    // ------------------------------------------------------------------ 0x004a39b0
    H* clear_4a39b0() {
        jo::self<jo::ItemDef>(this)->collision_type = 0;
        jo::self<jo::ItemDef>(this)->message_name = nullptr;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_03", name, addr, fn)
B("ItemRes::compare_4a3650", 0x004a3650, &compare_4a3650);
B("ItemRes::compare_key_4a3790", 0x004a3790, &compare_key_4a3790);
B("Obj::clear_4a39b0", 0x004a39b0, &H::clear_4a39b0);
#undef B

}  // namespace game_b103_03
}  // namespace

// ---------------------------------------- from modules/game_b82_13.cpp (game/b82_13)
//
//      0x004a36c0 NameTable::find(name) (ret 4): with a sorted index (+0x18) a CRT bsearch
//                 (comparator 0x004a3790, which reads the table from 0xa39e18 = this),
//                 returning the found entry's value or 0; otherwise a linear _stricmp scan over
//                 the 0x2bf-byte records (+0x14, name pointer at +0x10), 1-based or 0.
namespace {
namespace game_b82_13 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_name_find, "NameTable::find");

struct H {
    // 0x004a36c0 (ret 4)
    std::int32_t name_find(const char* name) {
        c_name_find.hit();
        if (jo::view<void*>(jo::self<jo::ItemRes>(this)->sorted_index) != nullptr) {
            global<void*>(d::g_item_res_sort_owner) = this;
            const std::int32_t* const hit = cdecl_call<std::int32_t*>(
                fn::crt_bsearch, name, jo::view<void*>(jo::self<jo::ItemRes>(this)->sorted_index), jo::self<jo::ItemRes>(this)->item_count, 4, reinterpret_cast<void*>(fn::code_value(fn::ItemRes_compare_key_4a3790)));  // bsearch
            if (hit == nullptr) return 0;
            return *hit;
        }
        for (std::int32_t i = 0; i < jo::self<jo::ItemRes>(this)->item_count; ++i) {
            const char* const entry = jo::self<jo::ItemRes>(this)->items[i].name;
            if (joan::crt::stricmp(joan::orig::arg::str(entry), joan::orig::arg::str(name)) == 0) return i + 1;  // _stricmp
        }
        return 0;
    }
};

#define B13(name, addr, fn) JOAN_REPLACE("game/b82_13", name, addr, fn)
B13("NameTable::find", 0x004a36c0, &H::name_find);
#undef B13

}  // namespace game_b82_13
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
//
//    - Item::acquire_anim 0x004a39f0 (index, unused) ret 8: refcount +0x9b[index]; on first use
//      resolves the item sprite (0x004a3f20), shares / loads the ANIM\ animation (+0xaf[index],
//      0x004184b0 / 0x00417470 / 0x00418550 / 0x00417540) and acquires the sprite (0x004b0ec0,
//      0x004b0f40); refcount +0xc: on first use acquires sprite +0x8 and loads the up to four
//      +0x122[i] ".TXT" event scripts into +0x172[i] (0x004c2060). Returns the new +0x9b count.
//
//    - Item::sprite_id 0x004a3f20 (index) ret 4: +6 when index == 0 or +0x73[index] == 0, else
//      registers "ITEM\<dir of +0x10>\" + +0x73[index] with the sprite table (0x004b18d0).
//
//    - Item::is_kind_1_or_4 0x004a4180, int_max 0x004893c0 (cdecl), Obj::set_62 0x00499fd0,
//      Obj::set_6a 0x0049adc0, Obj::set_6e 0x0049cac0, Obj::get_62 0x00499e20 (float),
//      gfile3.cpp list getters 0x004853e0 (+4) / 0x00485f20, wrappers 0x004ae6c0 / 0x004b9290.
namespace {
namespace game_b84_06 {

using joan::orig::helpers::at;

// Path string (0x194 bytes, 0x005a1170 ctor, no destructor).
struct alignas(4) Path {
    char b[0x194];
};

JOAN_COUNTER(c_acquire, "Item::acquire_anim");
JOAN_COUNTER(c_sprite, "Item::sprite_id");
// The low word of slot_anims[i], read as the original's own 16-bit load (movsx word) at the element's byte offset.
std::int16_t anim_low16(const jo::ItemDef* d, std::int32_t i) {
    return *reinterpret_cast<const std::int16_t*>(reinterpret_cast<const std::uint8_t*>(d->slot_anims) + i * static_cast<std::int32_t>(sizeof(std::int32_t)));
}

void* const kSprites = obj(d::g_model_res_mgr);  // sprite / model table
void* const kAnims = obj(d::g_anim_res_mgr);    // animation table

jo::ModelEntry* def_of(std::int32_t id) { return jo::as_model_entry(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(kSprites)->entry(joan::orig::arg::u32(id)))); }  // 0x0041a180

struct H {
    // ------------------------------------------------------------------ 0x004a39f0
    std::int32_t acquire_anim(std::int32_t index, std::int32_t /*unused*/) {
        c_acquire.hit();
        std::int32_t* const ref = &jo::self<jo::ItemDef>(this)->slot_refs[index];
        *ref = *ref + 1;
        if (*ref == 1) {
            const std::int32_t sprite = joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(this)->sprite_id(joan::orig::arg::i32(index)));  // 0x004a3f20
            if (jo::view<std::int8_t>(def_of(sprite)->type) == 0x41) {
                std::int32_t* const anim = &jo::self<jo::ItemDef>(this)->slot_anims[index];
                if (*anim == 0) {
                    for (std::int32_t j = 0; j <= jo::self<jo::ItemDef>(this)->model_count; ++j) {
                        if (j == index) continue;
                        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(this)->sprite_id(joan::orig::arg::i32(j))) != sprite) continue;  // 0x004a3f20
                        if (jo::self<jo::ItemDef>(this)->slot_anims[j] == 0) continue;
                        *anim = jo::self<jo::ItemDef>(this)->slot_anims[j];
                        break;
                    }
                }
                if (*anim == 0) {
                    char buf[0x104];  // [ebp-0x114]
                    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), joan::orig::arg::str(def_of(sprite)->dir));  // strcpy
                    std::strcat(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), str(d::s_ANIM));  // strcat "ANIM\"
                    *anim = joan::orig::arg::as<std::int32_t>(joan::orig::as_path(kAnims)->find(joan::orig::arg::str(static_cast<char*>(buf)), joan::orig::arg::str(def_of(sprite)->file)));  // 0x004184b0
                    if (*anim == 0) {
                        *anim = joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(kAnims)->add_slot(0u));  // 0x00417470
                        const char* const a = def_of(sprite)->file;
                        void* const slot = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kAnims)->slot_4174f0(joan::orig::arg::u32(*anim)));  // 0x004174f0
                        joan::orig::as_path(slot)->init(joan::orig::arg::str(static_cast<char*>(buf)), joan::orig::arg::str(a));  // 0x00418550
                    }
                }
            }
            if (jo::self<jo::ItemDef>(this)->slot_anims[index] != 0)
                joan::orig::as_anim_set(kAnims)->acquire_417540(joan::orig::arg::u32(anim_low16(jo::self<jo::ItemDef>(this), index)));  // 0x00417540
            if (jo::view<std::int8_t>(def_of(sprite)->type) != 0x41) {
                joan::orig::as_model_res_mgr(kSprites)->set_params(joan::orig::arg::i16(static_cast<std::int16_t>(sprite)), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(0))), joan::orig::arg::i16(static_cast<std::int16_t>(0)));  // 0x004b0ec0
            } else {
                const std::int16_t w = anim_low16(jo::self<jo::ItemDef>(this), index);
                const std::uint8_t has = static_cast<std::uint8_t>(jo::self<jo::ItemDef>(this)->slot_anims[index] != 0);
                joan::orig::as_model_res_mgr(kSprites)->set_params(joan::orig::arg::i16(static_cast<std::int16_t>(sprite)), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(has)), joan::orig::arg::i16(w));  // 0x004b0ec0
            }
            joan::orig::as_model_res(kSprites)->acquire(joan::orig::arg::i16(static_cast<std::int16_t>(sprite)));  // 0x004b0f40
        }
        jo::self<jo::ItemDef>(this)->ref_count = jo::self<jo::ItemDef>(this)->ref_count + 1;
        if (jo::self<jo::ItemDef>(this)->ref_count == 1) {
            if (jo::self<jo::ItemDef>(this)->model_sprite != 0) {
                joan::orig::as_model_res_mgr(kSprites)->set_params(joan::orig::arg::i16(jo::self<jo::ItemDef>(this)->model_sprite), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(0))), joan::orig::arg::i16(static_cast<std::int16_t>(0)));  // 0x004b0ec0
                joan::orig::as_model_res(kSprites)->acquire(joan::orig::arg::i16(jo::self<jo::ItemDef>(this)->model_sprite));               // 0x004b0f40
            }
            for (std::int32_t i = 0; i < 4; ++i) {
                if (jo::self<jo::ItemDef>(this)->emitter_scripts[i] != 0) continue;
                if (jo::self<jo::ItemDef>(this)->emitter_names[i] == nullptr) continue;
                Path path;
                joan::orig::as_string(&path)->construct();                               // 0x005a1170 ctor
                joan::orig::as_string(&path)->assign(joan::orig::arg::str(jo::self<jo::ItemDef>(this)->emitter_names[i]));  // 0x005a1440 assign
                joan::orig::as_string(&path)->append(str(d::s_TXT));                                   // 0x005a1550 ".TXT"
                const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90 c_str
                jo::self<jo::ItemDef>(this)->emitter_scripts[i] =
                    joan::orig::arg::as<std::int32_t>(joan::orig::as_name_list(obj(d::g_pemi_res))->find_4c2060(joan::orig::arg::str(name), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::int32_t>(1)))));  // 0x004c2060
            }
        }
        return *ref;
    }

    // ------------------------------------------------------------------ 0x004a3f20
    std::int32_t sprite_id(std::int32_t index) {
        c_sprite.hit();
        if (index == 0) return jo::self<jo::ItemDef>(this)->icon_sprite;
        // +0x73 + 4 * index on x86 = model_names[index - 1] (the original indexed the value array past its end into the name pointers)
        if (jo::self<jo::ItemDef>(this)->model_names[index - 1] == nullptr) return jo::self<jo::ItemDef>(this)->icon_sprite;
        char buf[0x108];  // [ebp-0x108]
        const char* const item_dir = str(d::s_ITEM_9cf648);  // "ITEM\\"
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), item_dir);  // strcpy "ITEM\"
        char* const sep = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(jo::self<jo::ItemDef>(this)->name), 0x5c));       // strrchr '\\'
        if (sep != nullptr) {
            joan::orig::libc::strncat(joan::orig::arg::str(static_cast<char*>(buf)), joan::orig::arg::str(jo::self<jo::ItemDef>(this)->name), joan::orig::arg::u32(static_cast<std::uint32_t>(sep - jo::self<jo::ItemDef>(this)->name)));  // strncat
            const std::uint32_t n = joan::orig::arg::as<std::uint32_t>(std::strlen(item_dir));  // strlen
            buf[static_cast<std::int32_t>(static_cast<std::uint32_t>(sep - jo::self<jo::ItemDef>(this)->name) + n)] = 0;  // name length < 2^32
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), str(d::s_bslash));  // strcat "\"
        }
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_model_res(kSprites)->add(joan::orig::arg::str(static_cast<char*>(buf)), joan::orig::arg::str(jo::self<jo::ItemDef>(this)->model_names[index - 1]), static_cast<std::int32_t>(0)));  // 0x004b18d0
    }

    // ------------------------------------------------------------------ 0x004a4180
    std::int32_t is_kind_1_or_4() {
        const std::int32_t k = jo::view<std::int16_t>(jo::self<jo::ItemDef>(this)->class_id);
        return (k == 1 || k == 4) ? 1 : 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("Item::acquire_anim", 0x004a39f0, &H::acquire_anim);
B("Item::sprite_id", 0x004a3f20, &H::sprite_id);
B("Item::is_kind_1_or_4", 0x004a4180, &H::is_kind_1_or_4);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - Res::release_slot 0x004a3e30 (slot, unused) ret 8.
namespace {
namespace game_b83_07 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_slot_release, "Res::release_slot");
// The low word of slot_anims[i], read as the original's own 16-bit load (movsx word) at the element's byte offset.
std::int16_t anim_low16(const jo::ItemDef* d, std::int32_t i) {
    return *reinterpret_cast<const std::int16_t*>(reinterpret_cast<const std::uint8_t*>(d->slot_anims) + i * static_cast<std::int32_t>(sizeof(std::int32_t)));
}

constexpr img::Addr kAnimRes = d::g_anim_res_mgr, kModelRes = d::g_model_res_mgr;

struct H {
    // ------------------------------------------------------------------ 0x004a3e30
    std::int32_t slot_release(std::int32_t slot, std::int32_t /*unused*/) {
        c_slot_release.hit();
        --jo::self<jo::ItemDef>(this)->ref_count;
        if (jo::self<jo::ItemDef>(this)->ref_count == 0 && jo::self<jo::ItemDef>(this)->model_sprite != 0)
            joan::orig::as_model_res_mgr(obj(kModelRes))->release(joan::orig::arg::i16(jo::self<jo::ItemDef>(this)->model_sprite));  // 0x004b16a0
        std::int32_t* const refs = &jo::self<jo::ItemDef>(this)->slot_refs[slot];
        --*refs;
        if (*refs == 0) {
            const std::int32_t model = joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(this)->sprite_id(joan::orig::arg::i32(slot)));  // 0x004a3f20
            joan::orig::as_model_res_mgr(obj(kModelRes))->release(joan::orig::arg::i16(static_cast<std::int16_t>(model)));
            if (jo::view<std::uint32_t>(jo::self<jo::ItemDef>(this)->slot_anims[slot]) != 0)
                joan::orig::as_anim_res_mgr(obj(kAnimRes))->release(joan::orig::arg::i16(anim_low16(jo::self<jo::ItemDef>(this), slot)));  // 0x00417e00
        }
        return *refs;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Res::release_slot", 0x004a3e30, &H::slot_release);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
//
//    - UnitDef::is_soldier 0x004a4050, Messages::update 0x004ae390, Messages::tick 0x004af530.
namespace {
namespace game_leaves82b {

using joan::orig::helpers::at;

struct H {
    // 0x004a4050 UnitDef::is_soldier: kind (short +0) in the list.
    std::int32_t is_soldier() {
        const std::int32_t k = static_cast<std::int16_t>(jo::self<jo::ItemDef>(this)->class_id);
        switch (k) {
        case 7: case 6: case 1: case 10: case 0x12: case 4: case 0x11: case 0xb: case 0xd:
        case 0xe: case 2: case 0xf: case 9: case 8: case 3: case 0x13: case 0x14:
            return 1;
        default:
            return 0;
        }
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("UnitDef::is_soldier", 0x004a4050, &H::is_soldier);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
//
//      - small direct callees: Modifiers::slot 0x004974f0, PtrList::is_empty_at 0x00457f60,
//        PtrList::get 0x00457e90, ModTable::value_own 0x004a41c0, ModTable::value 0x004a4200,
//        EffectRegistry::remove 0x0041bbf0, SoundSet::remove 0x006adf70, scalar deleting
//        destructors 0x004b0c20 / 0x004b0c70, Holder::release 0x006356c0.
namespace {
namespace game_units4 {

using joan::orig::helpers::at;

using joan::orig::helpers::wrap_add;
using joan::orig::helpers::wrap_mul;

JOAN_COUNTER(c_mod_value, "ModTable::value");

struct H {
    // 0x004a41c0 ModTable::value_own(level) = +0x4e + (level - +0x36) * +0x52
    std::int32_t value_own(std::int32_t level) {
        return wrap_add(jo::self<jo::ItemDef>(this)->damage, wrap_mul(wrap_add(level, -jo::self<jo::ItemDef>(this)->min_level), jo::self<jo::ItemDef>(this)->inc_damage));
    }
    // 0x004a4200 ModTable::value(kind, level, out) (ret 0xc)
    std::int32_t value(std::int32_t kind, std::int32_t level, std::int32_t* out) {
        c_mod_value.hit();
        if (jo::self<jo::ItemDef>(this)->damage_type == kind) {
            *out = wrap_add(jo::self<jo::ItemDef>(this)->damage, wrap_mul(wrap_add(level, -jo::self<jo::ItemDef>(this)->min_level), jo::self<jo::ItemDef>(this)->inc_damage));
            return 1;
        }
        for (std::int32_t i = 0; i < jo::self<jo::ItemDef>(this)->effect_count; ++i) {
            jo::ItemModifier* const entry = jo::as_item_modifier(jo::view<std::uint8_t*>(jo::self<jo::ItemDef>(this)->effects) + i * 0xc);
            if (entry->kind != kind) continue;
            *out = wrap_add(entry->base, wrap_mul(wrap_add(level, -jo::self<jo::ItemDef>(this)->min_level), entry->per_level));
            return 1;
        }
        return 0;
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("ModTable::value_own", 0x004a41c0, &H::value_own);
U4("ModTable::value", 0x004a4200, &H::value);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_b111_20.cpp (fix/uninit)
//
//    - ModTable::requirements_met 0x004a42f0 (thiscall(level, unit_id), ret 8 -> 1 / 0):
//      for each 16-byte entry (type, base, mult, flag) v = base + (level - this+0x36) * mult,
//      types 1 / 2 / 3 / 4 / 5 / 6 / 18 (table 0x004a448d / 0x004a44ad) fill have / need, then
//      flag == 0 ? have > need : have < need fails. UNINIT READ: an entry of any other type
//      compares the previous iteration's have / need - uninitialised stack ([ebp-0x14] /
//      [ebp-0x18]) for the first entry. We start them at 0 (an unknown requirement on the first
//      entry passes: 0 > 0 and 0 < 0 are false); later entries keep the stale values exactly.
namespace {
namespace game_b111_20 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_req, "ModTable::requirements_met_4a42f0");

struct H {
    // ------------------------------------------------------------ 0x004a42f0
    i32 requirements_met(i32 level, i32 unit_id) {
        c_req.hit();
        jo::Unit* const u = jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(unit_id))));  // 0x0041b3c0
        i32 have = 0, need = 0;  // fix/uninit: [ebp-0x14] / [ebp-0x18]
        for (i32 i = 0; i < jo::self<jo::ItemDef>(this)->requirement_count; ++i) {
            jo::ItemRequirement* const e = jo::as_item_requirement(jo::view<u8*>(jo::self<jo::ItemDef>(this)->requirements) + i * 16);
            const u32 diff = static_cast<u32>(level) - jo::view<u32>(jo::self<jo::ItemDef>(this)->min_level);
            const i32 v = static_cast<i32>(jo::view<u32>(e->base) + diff * jo::view<u32>(e->per_level));
            switch (e->kind) {
                case 1:
                    have = u->hit_points;
                    need = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_unit(u)->hit_points_max()) * static_cast<u32>(v)) / 100;  // 0x00523320
                    break;
                case 2:
                    have = u->mana;
                    need = static_cast<i32>(joan::orig::arg::as<u32>(joan::orig::as_unit(u)->value_149()) * static_cast<u32>(v)) / 100;  // 0x00523700
                    break;
                case 3: have = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->stat_3()); need = v; break;  // 0x005237e0
                case 4: have = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->stat_4()); need = v; break;  // 0x00523860
                case 5: have = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->level_sum()); need = v; break;  // 0x005238c0
                case 6: have = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->stat_6()); need = v; break;  // 0x00523920
                case 18: have = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->group_avg_5103c0()); need = v; break;  // 0x005103c0
                default: break;
            }
            if (e->is_min == 0) {
                if (have > need) return 0;
            } else {
                if (have < need) return 0;
            }
        }
        return 1;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: have / need start at 0 (first entry of an unhandled type compared stack garbage).
F("ModTable::requirements_met_4a42f0", 0x004a42f0, &H::requirements_met);
#undef F

}  // namespace game_b111_20
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
//
//    - Stats::stat_id 0x004a45f0 (name), ret 4 -> int: _stricmp table, 1..0x18, 0 when unknown.
namespace {
namespace game_b104_03 {

i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_stat_id, "Stats::stat_id");

struct H {
    // ------------------------------------------------------------------ 0x004a45f0
    i32 stat_id(const char* name) {
        c_stat_id.hit();
        static const img::Addr kNames[24] = {
            d::s_MAX_HIT_POINT, d::s_MAX_ENERGY_POINT, d::s_STRENGTH, d::s_DEXTERITY, d::s_DEFENSE, d::s_LEADERSHIP,
            d::s_WEAPON_DAMAGE, d::s_WEAPON_DAMAGE_MULTIPLIER, d::s_REDUCE_MELEE_DAMAGE_SUSTAINE, d::s_ARROW_DEFLECT_OFF, d::s_A_ATTACK_DAMAGE, d::s_B_ATTACK_DAMAGE,
            d::s_RUSH_ATTACK_DAMAGE, d::s_DOWN_ATTACK_DAMAGE, d::s_JUMP_ATTACK_B_DAMAGE, d::s_RANGE_DISTANCE, d::s_RATE_OF_FIRE, d::s_MORALE_BASE,
            d::s_MORALE_DROP, d::s_FATAL_KILL, d::s_EP_CONSUME, d::s_DAMAGE_SUSTAINED, d::s_TROOP_SIZE, d::s_NONE_9d4350};
        static const i32 kIds[24] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc,
                                     0xe, 0xf, 0xd, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18};
        for (i32 i = 0; i < 24; ++i)
            if (crt_stricmp(name, str(kNames[i])) == 0) return kIds[i];
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Stats::stat_id", 0x004a45f0, &H::stat_id);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b107_14.cpp (game/b107_14)
//
//    - 0x004a49a0 ItemTable::load_items (thiscall, ret 0): opens "ITEM\\ITEM.RES" in a stack
//      reslx object (0x004179f0 ctor, 0x0059b920 open), for i = 1..count (+0x121, re-read each
//      pass) builds an item (0x00589780) from entry i, clears a 0x2bf-byte record, parses it
//      (0x004a0160), looks up the slot (0x004a36c0 -> 0x00425ce0) and copies three words
//      (+0x255/+0x257/+0x259) into it; record dtor 0x004a37e0, item dtor 0x00589890, finally the
//      reslx dtor 0x00417a30 whose eax is returned (the only caller 0x0046f3a0 ignores eax).
//      The stack objects are laid out in one frame array with the original ebp offsets, so any
//      over-long write by a callee lands where it did in the original. The path object at
//      ebp-0x2dc has no destructor in the original either. The SEH frame only unwinds those
//      objects on an exception; the normal path is identical.
namespace {
namespace game_b107_14 {

using joan::orig::helpers::at;
const char* rstr(std::uintptr_t a) { return &img::global<const char>(a); }

JOAN_COUNTER(c_load_items, "ItemTable::load_items");

struct H {
    // 0x004a49a0
    u32 load_items() {
        c_load_items.hit();
        // The original frame: rec [ebp-0x73c], item [ebp-0x47c], path [ebp-0x2dc], res [ebp-0x148]; one object per
        // local (the types' sizes: ItemDef / FileTxt / ResourceIdxL hold pointers, larger on x64).
        struct Frame {
            jo::ItemDef rec;
            jo::FileTxt item;
            jo::String path;
            jo::ResourceIdxL res;
        };
        alignas(8) Frame fr;
        std::memset(static_cast<void*>(&fr), 0, sizeof fr);  // deterministic; the original never reads unset bytes
        u8* const res = reinterpret_cast<u8*>(&fr.res);
        u8* const path = reinterpret_cast<u8*>(&fr.path);
        u8* const item = reinterpret_cast<u8*>(&fr.item);
        u8* const rec = reinterpret_cast<u8*>(&fr.rec);

        joan::orig::as_res_idx(res)->construct();                             // 0x004179f0 reslx ctor
        joan::orig::as_string(path)->construct();                     // 0x005a1170
        joan::orig::as_string(path)->assign(joan::orig::arg::str(rstr(img::addr(d::s_ITEM_9cf648))));  // 0x005a1440 "ITEM\\"
        joan::orig::as_string(path)->append(joan::orig::arg::str(rstr(img::addr(d::s_ITEM_RES))));  // 0x005a1550 "ITEM.RES"
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(name), static_cast<char>(1), static_cast<char>(0));  // 0x0059b920
        for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
            const u32 size = joan::orig::arg::as<u32>(joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
            void* const data = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
            joan::orig::as_file_txt(item)->construct_mem(joan::orig::arg::str(data), joan::orig::arg::i32(size));  // 0x00589780
            std::memset(rec, 0, sizeof(jo::ItemDef));
            joan::orig::as_item_res(this)->parse_item(joan::orig::arg::mptr(item), joan::orig::arg::mptr(rec));               // 0x004a0160
            const u32 slot = joan::orig::arg::as<u32>(joan::orig::as_name_table(this)->find_4a36c0(joan::orig::arg::str(jo::as_item_def(rec)->name)));  // 0x004a36c0
            jo::ItemDef* const p = jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(this)->get(joan::orig::arg::i16(static_cast<u16>(slot)))));  // 0x00425ce0
            p->usable_unit = jo::as_item_def(rec)->usable_unit;
            p->combine_item = jo::as_item_def(rec)->combine_item;
            p->result_item = jo::as_item_def(rec)->result_item;
            joan::orig::as_item_res(rec)->clear();                           // 0x004a37e0
            joan::orig::as_file_txt(item)->destruct();                      // 0x00589890
        }
        joan::orig::as_archive(res)->dtor();                        // 0x00417a30
        return 0;  // 0x00417a30 returns nothing; the only caller (game.cpp run) discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_14", name, addr, fn)
B("ItemTable::load_items", 0x004a49a0, &H::load_items);
#undef B

}  // namespace game_b107_14
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
//
//      0x0049f5b0 Obj49f::dtor_49f5b0           (thiscall, ret 0)
namespace {
namespace game_b108_14 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------------ 0x0049f5b0
    u32 dtor_49f5b0() {
        joan::orig::as_item_res(this)->release_49f620();                 // 0x0049f620
        return joan::orig::arg::as<u32>(joan::orig::as_string_pool(&jo::self<jo::ItemRes>(this)->strings)->destruct());  // 0x005a1770
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Obj49f::dtor_49f5b0", 0x0049f5b0, &H::dtor_49f5b0);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - shape_from_name 0x004a4540 (s), ret 4: None 0 / Box3d 2 / Cylinder 3 / Sphere 1.
namespace {
namespace game_b100_04 {

using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::img::str;

i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

struct H {
    // ------------------------------------------------------------------ 0x004a4540
    i32 shape_from_name(const char* s) {
        if (crt_stricmp(s, str(d::s_None)) == 0) return 0;  // "None"
        if (crt_stricmp(s, str(d::s_Box3d)) == 0) return 2;  // "Box3d"
        if (crt_stricmp(s, str(d::s_Cylinder)) == 0) return 3;  // "Cylinder"
        if (crt_stricmp(s, str(d::s_Sphere)) == 0) return 1;  // "Sphere"
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("shape_from_name", 0x004a4540, &H::shape_from_name);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b100_04_2.cpp (game/b100_04)
namespace {
namespace game_b100_04_2 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x0049f570
    void* ctor_49f570() {
        joan::orig::as_string_pool(&jo::self<jo::ItemRes>(this)->strings)->construct();  // 0x005a1730
        crt_memset(this, 0, sizeof(jo::ItemRes));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("Obj::ctor_49f570", 0x0049f570, &H::ctor_49f570);
#undef B

}  // namespace game_b100_04_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ItemRes::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_14::H::dtor_49f5b0>(this)); }  // 0x49f5b0
auto joan::orig::ItemRes::release_49f620() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_16::H::release_49f620>(this)); }  // 0x49f620
auto joan::orig::Table::update_all() -> void { joan::bind::member<&::game_b99_03::H::update_all>(this); }  // 0x49f7d0
auto joan::orig::ItemRes::load_49f830() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_02::H::item_load>(this)); }  // 0x49f830
auto joan::orig::ItemRes::load_49f970() -> void { joan::bind::member<&::game_b96_02::H::itemres_load>(this); }  // 0x49f970
auto joan::orig::NameTable::find_49fab0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_03::H::find_name>(this, a0)); }  // 0x49fab0
auto joan::orig::Names::index_of(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_02::H::names_index_of>(this, a0)); }  // 0x49fb30
auto joan::orig::ItemRes::load_49fbb0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_01::H::itemres_load>(this)); }  // 0x49fbb0
auto joan::orig::ItemRes::scan_dir(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::scan_dir>(this, a0)); }  // 0x49fdf0
auto joan::orig::ItemRes::parse_item(void* a0, void* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_02::H::parse_item>(this, a0, a1)); }  // 0x4a0160
auto joan::orig::placements::find(void* a0, std::int32_t a1, PathSpace* a2) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_objects1::placements_find>(a0, a1, a2)); }  // 0x4a2b20
auto joan::orig::ItemRes::sort_index() -> void { joan::bind::member<&::game_b98_02::H::sort_index>(this); }  // 0x4a3590
auto joan::orig::NameTable::find_4a36c0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_13::H::name_find>(this, a0)); }  // 0x4a36c0
auto joan::orig::ItemRes::clear() -> void { joan::bind::member<&::game_b92_03::H::itemres_clear>(this); }  // 0x4a37e0
auto joan::orig::ItemDef::acquire_anim(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_06::H::acquire_anim>(this, a0, a1)); }  // 0x4a39f0
auto joan::orig::Res::release_slot(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::slot_release>(this, a0, a1)); }  // 0x4a3e30
auto joan::orig::ItemDef::sprite_id(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_06::H::sprite_id>(this, a0)); }  // 0x4a3f20
auto joan::orig::ItemDef::is_soldier() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82b::H::is_soldier>(this)); }  // 0x4a4050
auto joan::orig::ItemDef::is_kind_1_or_4() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_06::H::is_kind_1_or_4>(this)); }  // 0x4a4180
auto joan::orig::ModTable::value_own(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units4::H::value_own>(this, a0)); }  // 0x4a41c0
auto joan::orig::ModTable::value(std::int32_t a0, std::int32_t a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units4::H::value>(this, a0, a1, a2)); }  // 0x4a4200
auto joan::orig::ModTable::requirements_met_4a42f0(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_20::H::requirements_met>(this, a0, a1)); }  // 0x4a42f0
auto joan::orig::ItemRes::shape_from_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_04::H::shape_from_name>(this, a0)); }  // 0x4a4540
auto joan::orig::Stats::stat_id(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_03::H::stat_id>(this, a0)); }  // 0x4a45f0
