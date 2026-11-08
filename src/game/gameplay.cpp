// game/gameplay.cpp: functions of the original d:\ww\gameplay.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cstddef>
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
using u16 = std::uint16_t;
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

using joan::call::cdecl_call;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
//
//      0x0047c070 Strings::clear                (thiscall, ret 0) - returns this.
namespace {
namespace game_b98_02 {

JOAN_COUNTER(c_strings_clear, "Strings::clear");

struct H {
    // ------------------------------------------------------------------ 0x0047c070
    void* strings_clear() {
        c_strings_clear.hit();
        if (jo::view<void*>(jo::self<jo::Ini>(this)->entries) == nullptr) return this;
        for (u16 i = 1; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::Ini>(this)->entry_count); i = static_cast<u16>(i + 1)) {
            void* const s = joan::orig::arg::as<void*>(joan::orig::as_table(this)->get_47c150(joan::orig::arg::u32(static_cast<u32>(i))));  // 0x0047c150
            const i32 line = static_cast<i32>(global<i16>(d::g_9ff79c)) + 7;
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(s), str(d::s_d_ww_gameplay_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
        }
        const i32 line = static_cast<i32>(global<i16>(d::g_9ff79c)) + 0xa;
        joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Ini>(this)->entries)), str(d::s_d_ww_gameplay_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
        jo::view<void*>(jo::self<jo::Ini>(this)->entries) = nullptr;
        jo::self<jo::Ini>(this)->entry_count = 0;
        return this;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Strings::clear", 0x0047c070, &H::strings_clear);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
//
//      0x0047c150 Table::get(ushort)             (thiscall, ret 4)
namespace {
namespace game_b95_02 {

struct H {
    // ------------------------------------------------------------------ 0x0047c150
    void* table_get(u32 i) { return jo::view<void*>(jo::self<jo::Ini>(this)->entries[static_cast<i32>(i & 0xffff) - 1].entry); }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Table::get", 0x0047c150, &H::table_get);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
//
//    - Ini::value 0x0047c180 (key), ret 4 / Ini::section 0x0047cbf0 (name), ret 4: strcpy to a
//      stack buffer, bsearch (0x7b365b) over [+9]/[+0xd] (resp. [0]/[+4]) with comparators
//      0x0047c210 / 0x0047cc60, returns *result.
//
//    - Obj::dtor_47cac0 0x0047cac0: 0x0059c780 then 0x005872b0 (SEH frame, normal path).
//
//  - 0x0047cbf0's original buffer is 0x10 bytes before the saved this at [ebp-4] (0x0047c180:
//    0x10c bytes); longer keys corrupt the original's frame. All callers pass short literal
//    names ("CHARMOVE", ...). The replacement uses a larger buffer.
namespace {
namespace game_b90_03 {

void* strcpy_(void* d, const void* s) { return joan::orig::arg::as<void*>(std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s))); }
void* bsearch_(const void* key, const void* base, std::uint32_t num, std::uint32_t width, std::uintptr_t cmp) {
    return joan::orig::arg::as<void*>(joan::orig::libc::bsearch(joan::orig::arg::mptr(key), joan::orig::arg::mptr(base), joan::orig::arg::u32(num), joan::orig::arg::u32(width), joan::orig::arg::cmp(cmp)));
}

JOAN_COUNTER(c_ini_value, "Ini::value");
JOAN_COUNTER(c_ini_section, "Ini::section");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x0047c180
    jo::IniEntry* ini_value(const char* key) {
        c_ini_value.hit();
        char buf[0x110];
        strcpy_(buf, key);
        const char* p = buf;
        void* const r = bsearch_(&p, jo::self<jo::Ini>(this)->entries, static_cast<std::uint32_t>(jo::self<jo::Ini>(this)->entry_count), static_cast<std::uint32_t>(sizeof(jo::IniEntryPtr)),
                                 fn::code_value(fn::cmp_47c210));
        return *static_cast<jo::IniEntry**>(r);
    }

    // 0x0047cac0
    std::uint32_t dtor_47cac0() {
        joan::orig::as_res_idx(self())->deinit();                        // 0x0059c780
        joan::orig::as_file(self())->destruct_5872b0();       // 0x005872b0
        return 0;  // callers (Obj::load_res, an atexit dtor) discard the result; ~File returns nothing
    }

    // 0x0047cbf0
    jo::Ini* ini_section(const char* name) {
        c_ini_section.hit();
        char buf[0x110];
        strcpy_(buf, name);
        const char* p = buf;
        void* const r = bsearch_(&p, jo::self<jo::IniFile>(this)->sections, static_cast<std::uint32_t>(jo::self<jo::IniFile>(this)->section_count), static_cast<std::uint32_t>(sizeof(jo::IniPtr)),
                                 fn::code_value(fn::cmp_47cc60));
        return *static_cast<jo::Ini**>(r);
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Ini::value", 0x0047c180, &H::ini_value);
M("Obj::dtor_47cac0", 0x0047cac0, &H::dtor_47cac0);
M("Ini::section", 0x0047cbf0, &H::ini_section);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
namespace {
namespace game_b103_02 {

i32 JOAN_CDECL cmp_47c210(const void* a, const void* b) {
    const char* const pa = *static_cast<char* const*>(a);
    const char* const pb = *static_cast<char* const*>(b);
    return std::strcmp(joan::orig::arg::str(pa), joan::orig::arg::str(pb));  // strcmp
}
i32 JOAN_CDECL cmp_47cc60(const void* a, const void* b) {
    const char* const pa = *static_cast<char* const*>(a);
    const char* const pb = *static_cast<char* const*>(b);
    return std::strcmp(joan::orig::arg::str(pa), joan::orig::arg::str(pb));  // strcmp
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("cmp_47c210", 0x0047c210, &cmp_47c210);
B("cmp_47cc60", 0x0047cc60, &cmp_47cc60);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
//
//      0x0047c250 Strings::add(src)             (thiscall, ret 4) - grow +9 by one, new 0x109 copy.
//
//  - 0x00472b00 / 0x0047be60 / 0x0047c250 / 0x0049f830 / 0x004af950: eax as the original leaves it
//    is returned (memcpy result / callee result / loop index).
namespace {
namespace game_b97_02 {

JOAN_COUNTER(c_strings_add, "Strings::add");

using joan::orig::helpers::crt_memcpy;

struct H {
    // ------------------------------------------------------------------ 0x0047c250
    void* strings_add(const void* src) {
        c_strings_add.hit();
        jo::self<jo::Ini>(this)->entry_count = static_cast<u16>(jo::self<jo::Ini>(this)->entry_count + 1);
        const i32 line = static_cast<i32>(global<i16>(d::g_9ff7a0));
        jo::view<void*>(jo::self<jo::Ini>(this)->entries) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Ini>(this)->entries)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Ini>(this)->entry_count) * static_cast<i32>(sizeof(jo::IniEntryPtr))), str(d::s_d_ww_gameplay_cpp), joan::orig::arg::i32(line + 0x21));  // 0x0058eb30 (pointer table: << 2 on x86)
        void* const s = joan::orig::as_mem(kMem)->add(0x109u, str(d::s_d_ww_gameplay_cpp), joan::orig::arg::i32(line + 0x23));  // 0x0058e7b0
        jo::self<jo::Ini>(this)->entries[static_cast<i32>(jo::self<jo::Ini>(this)->entry_count) - 1].entry = static_cast<jo::IniEntry*>(s);
        return crt_memcpy(jo::self<jo::Ini>(this)->entries[static_cast<i32>(jo::self<jo::Ini>(this)->entry_count) - 1].entry, src, 0x109);
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Strings::add", 0x0047c250, &H::strings_add);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x0047c330 Obj::sort_47c330              (thiscall, ret 0) - qsort(+0x9, word +0xd, 4, 0x0047c210).
//
//      0x0047c9e0 StrTable::free_all            (thiscall, ret 0) - gameplay.cpp; returns this.
namespace {
namespace game_b99_02 {

using joan::orig::helpers::u;

JOAN_COUNTER(c_free_all, "StrTable::free_all");

// 0x0058ed70 Mem::free(ptr, file, line) ret 0xc.
i32 mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del_58ed70(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); return 0; }  // 0x0058ed70 (callers discard the result)

struct H {
    // ------------------------------------------------------------------ 0x0047c330
    void sort_47c330() {
        joan::orig::libc::qsort(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Ini>(this)->entries)), joan::orig::arg::u32(static_cast<u32>(jo::self<jo::Ini>(this)->entry_count)), static_cast<u32>(sizeof(jo::IniEntryPtr)), fn::code_value(fn::cmp_47c210));  // qsort (4 on x86)
    }

    // ------------------------------------------------------------------ 0x0047c9e0
    std::uintptr_t free_all() {
        c_free_all.hit();
        if (jo::view<void*>(jo::self<jo::IniFile>(this)->sections) == nullptr) return u(this);
        for (u16 i = 1; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::IniFile>(this)->section_count); i = static_cast<u16>(i + 1)) {
            void* const s = joan::orig::arg::as<void*>(joan::orig::as_table(this)->at_47cb30(joan::orig::arg::u32(static_cast<i32>(i))));  // 0x0047cb30 (low word)
            joan::orig::as_ini_section(s)->clear();  // 0x0047c070
            mem_free(s, str(d::s_d_ww_gameplay_cpp), static_cast<i32>(global<i16>(d::g_9ff7a8)) + 9);
        }
        mem_free(jo::view<void*>(jo::self<jo::IniFile>(this)->sections), str(d::s_d_ww_gameplay_cpp), static_cast<i32>(global<i16>(d::g_9ff7a8)) + 0xc);
        jo::self<jo::IniFile>(this)->sections = nullptr;
        jo::self<jo::IniFile>(this)->section_count = 0;
        return u(this);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::sort_47c330", 0x0047c330, &H::sort_47c330);
B99("StrTable::free_all", 0x0047c9e0, &H::free_all);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
//
//      0x0047c370 Small6::zero               (thiscall, ret 0) - memset 6, returns this.
//
//      0x0047c3a0 IniFile::dtor              (thiscall, ret 0) - 0x0047c9e0(this).
//
//      0x0047c6c0 IniFile::parse             (thiscall(text, len), ret 8) - line loop, switch on the
//                 first char ('\0' / '\t' / ' ' / '/' skip, '{' section, '[' key, '~' end,
//                 anything else (incl. negative chars) returns at once), then sort / finish.
//
//  - 0x0047c6c0: the key/value record (0x109 bytes) at ebp-0x22c is uninitialised between the
//    key's NUL and the value at +9 when the key is shorter than 8 chars; 0x0047c250 copies the
//    whole record to the heap but lookups compare only the NUL-terminated strings, so those bytes
//    are dead: the record is zeroed here. Reviewed: the heap records are only reached through
//    0x0047c180 (bsearch, comparator 0x0047c210 = strcmp on the key) whose ~180 callers all read
//    the value string at +9 (atof / atoi / first char), 0x0047c330 (qsort, same comparator) and
//    0x0047c150 (only used by the section dtor 0x0047c070 to free them); nothing writes them to a
//    file. The value part (+9..+0x108) is fully written by strncpy's zero padding + rec[0x108]. Line buffer is 0x10c bytes (read_line writes its NUL
//    at buf[0x109]). Its dead `if (0)` blocks have no calls.
namespace {
namespace game_b100_03 {

using i8 = std::int8_t;

JOAN_COUNTER(c_ini_parse, "IniFile::parse");

using joan::orig::helpers::memset_;        // memset
using joan::orig::helpers::memcpy_;  // memcpy

struct H {
    // ------------------------------------------------------------------------ 0x0047c370
    void* zero_6() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::IniFile)));  // 6 on x86 (the IniFile 0xa3ce5a)
        return this;
    }

    // ------------------------------------------------------------------------ 0x0047c3a0
    void ini_dtor() { joan::orig::as_ini(this)->free_all(); }  // 0x0047c9e0

    // ------------------------------------------------------------------------ 0x0047c6c0
    void ini_parse(const char* text, i32 len) {
        c_ini_parse.hit();
        alignas(4) char line[0x10c];
        alignas(4) char rec[0x109];
        void* section = nullptr;
        while (len > 0) {
            const i32 n = joan::orig::arg::as<i32>(joan::orig::min_int(joan::orig::arg::i32(len), 0x109));  // 0x0046f020 min
            const i32 used = joan::orig::arg::as<i32>(joan::orig::as_ini(this)->copy_line(joan::orig::arg::str(static_cast<char*>(line)), joan::orig::arg::str(text), joan::orig::arg::i32(n)));  // 0x0047ce80
            const i32 c = static_cast<i32>(static_cast<i8>(line[0]));
            switch (c) {
                case 0x00:
                case 0x09:
                case 0x20:
                case 0x2f: break;
                case 0x7e: len = -1; break;  // '~'
                case 0x7b: {                 // '{'
                    char* const p = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(static_cast<char*>(line + 1)), 0x7d));  // strrchr
                    *p = 0;
                    section = joan::orig::arg::as<void*>(joan::orig::as_ini(this)->intern_section(joan::orig::arg::str(static_cast<char*>(line + 1))));  // 0x0047cca0
                    break;
                }
                case 0x5b: {  // '['
                    char* const p = joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(static_cast<char*>(line + 1)), 0x5d));  // ']'
                    const i32 klen = static_cast<i32>(reinterpret_cast<std::uintptr_t>(p) -
                                                      reinterpret_cast<std::uintptr_t>(&line[0])) - 1;
                    std::memset(rec, 0, sizeof(rec));  // see notes: dead uninitialised bytes
                    memcpy_(rec, line + 1, static_cast<u32>(klen));
                    rec[klen] = 0;
                    std::strncpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(rec + 9))), joan::orig::arg::str(static_cast<char*>(line + klen + 2)), 0xffu);  // strncpy
                    rec[0x108] = 0;
                    joan::orig::as_ini_section(section)->add(joan::orig::arg::mptr(static_cast<char*>(rec)));  // 0x0047c250
                    break;
                }
                default: return;  // unsigned > 0x7e or any other char
            }
            text += used;
            len -= used;
        }
        joan::orig::as_table(this)->sort();  // 0x0047cf20
        for (u16 i = 1; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::IniFile>(this)->section_count); i = static_cast<u16>(i + 1)) {
            void* const s = joan::orig::arg::as<void*>(joan::orig::as_table(this)->at_47cb30(joan::orig::arg::u32(static_cast<u32>(i))));  // 0x0047cb30
            joan::orig::as_ini_section(s)->sort();                                             // 0x0047c330
        }
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("Small6::zero", 0x0047c370, &H::zero_6);
B100("IniFile::dtor", 0x0047c3a0, &H::ini_dtor);
B100("IniFile::parse", 0x0047c6c0, &H::ini_parse);
#undef B100

}  // namespace game_b100_03
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
//
//      0x0047c3d0 Gameplay::load_file(name)        (thiscall, ret 4) - stack File (0x004179c0).
//
//  - SEH frames in 0x0046ec30 / 0x0047c3d0 / 0x004804f0 / 0x00481cd0 only guard destructors or
//    new + ctor: normal path reproduced.
namespace {
namespace game_b102_02 {

struct alignas(4) File {
    u8 bytes[0x210];  // 0x110 in the original frame (0x004179c0); room
};

JOAN_COUNTER(c_gp_load, "Gameplay::load_file");

void* const kHeap = obj(d::g_mem);

void* heap_alloc(u32 n, const char* file, i32 line) {
    return joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(n), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}
void heap_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

i32 file_read(void* f, void* p, u32 n) { return joan::orig::as_file(f)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n)); }  // 0x005873a0

struct H {
    // 0x0047c3d0
    u32 gp_load_file(const char* name) {
        c_gp_load.hit();
        File file;
        joan::orig::as_file(&file)->construct();           // 0x004179c0
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(name), 1, static_cast<char>(0));  // 0x00586da0
        const u32 size = joan::orig::arg::as<u32>(joan::orig::as_file(&file)->file_size());  // 0x00587f30
        const char* const src = str(d::s_d_ww_gameplay_cpp);       // "d:\ww\gameplay.cpp"
        void* const buf = heap_alloc(size, src, static_cast<i32>(global<i16>(d::g_9ff7a4)) + 9);
        file_read(&file, buf, size);
        joan::orig::as_ini(this)->parse(joan::orig::arg::str(buf), joan::orig::arg::i32(size));  // 0x0047c6c0
        heap_free(buf, src, static_cast<i32>(global<i16>(d::g_9ff7a4)) + 0xf);
        joan::orig::as_file(&file)->file_close();          // 0x00587260
        joan::orig::as_file(&file)->destruct_5872b0();  // 0x005872b0 ~File
        return 0;  // no callers read the result; ~File returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Gameplay::load_file", 0x0047c3d0, &H::gp_load_file);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x0047c520 Obj::load_res(name, packed)       (thiscall, ret 8) - two stack resource
//                 readers (0x0047cdf0 / 0x0047cb60).
//
//  - 0x0047c520: the stack readers get the original frame gaps (0x138 / 0x13c bytes).
namespace {
namespace game_b101_02 {

JOAN_COUNTER(c_load_res, "Obj::load_res");

struct H {
    // ------------------------------------------------------------------ 0x0047c520
    u32 load_res(void* name, i32 packed) {
        c_load_res.hit();
        if ((static_cast<u32>(packed) & 0xffu) != 0) {
            alignas(8) u8 res[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x148] (x64: the larger ResourceIdxL)
            joan::orig::as_archive(res)->construct(joan::orig::arg::str(name), 0u, 0u);  // 0x0047cdf0
            for (i32 i = jo::as_res_idx_l(res)->rec_count; i >= 1; --i) {
                void* const r1 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
                void* const r2 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
                joan::orig::as_ini(this)->parse(joan::orig::arg::str(r2), static_cast<std::int32_t>(joan::orig::arg::as<std::uintptr_t>(r1)));                    // 0x0047c6c0
            }
            joan::orig::as_archive(res)->dtor();  // 0x00417a30
            return 0;  // sole caller (game.cpp) discards the result; ~Archive returns nothing
        }
        alignas(8) u8 res[((sizeof(jo::ResourceIdx) > 0x13c ? sizeof(jo::ResourceIdx) : 0x13c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x284] (x64: the larger ResourceIdx)
        joan::orig::as_anim_set(res)->AnimSet3(joan::orig::arg::str(name), 0, 0);  // 0x0047cb60
        for (i32 i = jo::as_res_idx(res)->rec_count; i >= 1; --i) {
            void* const r1 = joan::orig::arg::as<void*>(joan::orig::as_res_idx(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059ca10
            void* const r2 = joan::orig::arg::as<void*>(joan::orig::as_res_idx(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059cbb0
            joan::orig::as_ini(this)->parse(joan::orig::arg::str(r2), static_cast<std::int32_t>(joan::orig::arg::as<std::uintptr_t>(r1)));                    // 0x0047c6c0
        }
        return joan::orig::arg::as<u32>(joan::orig::as_res_idx_file(res)->dtor_47cac0());  // 0x0047cac0
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("Obj::load_res", 0x0047c520, &H::load_res);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x0047cb30 Table::at               (thiscall(word i), ret 4) - [this][i - 1].
namespace {
namespace game_b94_02 {

struct H {
    // ------------------------------------------------------------------------ 0x0047cb30
    void* table_at(u32 i) {
        const i32 idx = static_cast<i32>(i & 0xffffu);
        return jo::view<void*>(jo::self<jo::IniFile>(this)->sections[idx - 1].section);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Table::at", 0x0047cb30, &H::table_at);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
//
//      0x0047cb60 AnimSet::AnimSet(a,b,c) (thiscall, ret 0xc; SEH frame only guards the base
//                 constructor on unwind - normal path kept)
namespace {
namespace game_b93_01 {

struct H {
    // ------------------------------------------------------------------------ 0x0047cb60
    void* animset_ctor3(const char* a, i32 b, i32 c) {
        joan::orig::as_file(this)->construct();  // 0x004179c0
        jo::as_res_idx(this)->init_flag = 0;
        joan::orig::as_res_idx(this)->init(a, static_cast<char>(b), static_cast<char>(c));  // 0x0059c550
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("AnimSet::AnimSet3", 0x0047cb60, &H::animset_ctor3);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x0047cca0 StrPool::intern(s)           (thiscall, ret 4)    - _lfind, else append strdup.
//
//  - 0x0047cca0: the original strcpy's s into a 16-byte stack buffer used as the _lfind key;
//    a longer s would smash the original's saved `this` (crash), so a larger buffer is used here.
namespace {
namespace game_b96_02 {

JOAN_COUNTER(c_intern, "StrPool::intern");

using joan::orig::helpers::line_of;

struct H {
    // ------------------------------------------------------------------------ 0x0047cca0
    char* intern(const char* s) {
        c_intern.hit();
        char buf[0x100];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(s));  // strcpy
        char* key = buf;
        u32 num = jo::self<jo::IniFile>(this)->section_count;
        char** const hit = joan::orig::arg::as<char**>(joan::orig::libc::_lfind(&key, joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::IniFile>(this)->sections)), &num, static_cast<u32>(sizeof(char*)), // _lfind (4 on x86)
                                              reinterpret_cast<void*>(fn::code_value(fn::cmp_47cc60))));
        if (hit != nullptr) return *hit;
        jo::self<jo::IniFile>(this)->section_count = static_cast<u16>(jo::self<jo::IniFile>(this)->section_count + 1);
        {
            const i32 line = line_of(d::g_9ff7ac, 0x19);
            jo::view<void*>(jo::self<jo::IniFile>(this)->sections) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::IniFile>(this)->sections)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::IniFile>(this)->section_count) * static_cast<i32>(sizeof(char*))), str(d::s_d_ww_gameplay_cpp), joan::orig::arg::i32(line));  // 0x0058eb30 "gameplay.cpp" (pointer table: << 2 on x86)
        }
        char* const e = joan::orig::arg::as<char*>(joan::orig::as_mem(kMem)->add_clear(static_cast<u32>(sizeof(jo::Ini)), str(d::s_d_ww_gameplay_cpp), joan::orig::arg::i32(line_of(d::g_9ff7ac, 0x1b))));  // 0x0058e8d0
        jo::view<char**>(jo::self<jo::IniFile>(this)->sections)[jo::self<jo::IniFile>(this)->section_count - 1] = e;
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::view<char**>(jo::self<jo::IniFile>(this)->sections)[jo::self<jo::IniFile>(this)->section_count - 1])), joan::orig::arg::str(s));  // strcpy
        return jo::view<char**>(jo::self<jo::IniFile>(this)->sections)[jo::self<jo::IniFile>(this)->section_count - 1];
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("StrPool::intern", 0x0047cca0, &H::intern);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

using joan::img::str;

struct H {
    // 0x0047cdf0 Archive::Archive(a, b, c)
    void* ctor_47cdf0(const char* a, std::uint32_t b, std::uint32_t c) {
        joan::orig::as_file(this)->construct();  // 0x004179c0
        jo::as_res_idx_l(this)->init_flag = 0;
        joan::orig::as_res_idx_l(this)->init(a, static_cast<char>(b), static_cast<char>(c));  // 0x0059b920
        return this;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Archive::Archive", 0x0047cdf0, &H::ctor_47cdf0);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b99_02_2.cpp (game/b99_02)
namespace {
namespace game_b99_02_2 {

using i8 = std::int8_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_copy_line, "Obj::copy_line");

struct H {
    // ------------------------------------------------------------------ 0x0047ce80
    i32 copy_line(char* dst, const char* src, i32 n) {
        c_copy_line.hit();
        i32 i = 0;
        for (; i < n; ++i) {
            const i8 c = static_cast<i8>(src[i]);
            if (c == 0 || c == 0xd || c == 0xa) break;
            dst[i] = src[i];
        }
        dst[i] = 0;
        return i + 1;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::copy_line", 0x0047ce80, &H::copy_line);
#undef B99

}  // namespace game_b99_02_2
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02_2 {

using joan::call::cdecl_call;

using u16 = std::uint16_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x0047cf20
    void table_sort() {
        joan::orig::libc::qsort(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::IniFile>(this)->sections)), joan::orig::arg::u32(static_cast<u32>(jo::self<jo::IniFile>(this)->section_count)), joan::orig::arg::u32(static_cast<u32>(sizeof(jo::IniPtr))), std::uintptr_t{fn::code_value(fn::cmp_47cc60)});  // qsort (4 on x86)
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Table::sort", 0x0047cf20, &H::table_sort);
#undef B94

}  // namespace game_b94_02_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::IniSection::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_02::H::strings_clear>(this)); }  // 0x47c070
auto joan::orig::Table::get_47c150(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_02::H::table_get>(this, a0)); }  // 0x47c150
auto joan::orig::Ini::value(const char* a0) -> IniEntry* { return joan::bind::conv<IniEntry*>(joan::bind::member<&::game_b90_03::H::ini_value>(this, a0)); }  // 0x47c180
auto joan::orig::IniSection::add(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_02::H::strings_add>(this, a0)); }  // 0x47c250
auto joan::orig::IniSection::sort() -> void { joan::bind::member<&::game_b99_02::H::sort_47c330>(this); }  // 0x47c330
auto joan::orig::Ini::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_03::H::zero_6>(this)); }  // 0x47c370
auto joan::orig::Ini::load_res(void* a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::load_res>(this, a0, a1)); }  // 0x47c520
auto joan::orig::Ini::parse(const char* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b100_03::H::ini_parse>(this, a0, a1); }  // 0x47c6c0
auto joan::orig::Ini::free_all() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::free_all>(this)); }  // 0x47c9e0
auto joan::orig::ResIdxFile::dtor_47cac0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_03::H::dtor_47cac0>(this)); }  // 0x47cac0
auto joan::orig::Table::at_47cb30(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_02::H::table_at>(this, a0)); }  // 0x47cb30
auto joan::orig::AnimSet::AnimSet3(const char* a0, std::int32_t a1, std::int32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_01::H::animset_ctor3>(this, a0, a1, a2)); }  // 0x47cb60
auto joan::orig::Ini::section(const char* a0) -> Ini* { return joan::bind::conv<Ini*>(joan::bind::member<&::game_b90_03::H::ini_section>(this, a0)); }  // 0x47cbf0
auto joan::orig::Ini::intern_section(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_02::H::intern>(this, a0)); }  // 0x47cca0
auto joan::orig::Archive::construct(const char* a0, std::uint32_t a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::ctor_47cdf0>(this, a0, a1, a2)); }  // 0x47cdf0
auto joan::orig::Ini::copy_line(const char* a0, const char* a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_02_2::H::copy_line>(this, a0, a1, a2)); }  // 0x47ce80
auto joan::orig::Table::sort() -> void { joan::bind::member<&::game_b94_02_2::H::table_sort>(this); }  // 0x47cf20
