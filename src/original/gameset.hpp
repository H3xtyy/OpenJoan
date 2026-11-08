// Original layouts of String (d:\ww\ilib\str.cpp, joan.exe 0x005a1170-0x005a1714),
// Directory (d:\ww\ilib\dir.cpp, 0x00582ad0-0x00582c0f) and GameSet
// (d:\ww\ilib\gameset.cpp, 0x0058adf0-0x0058b2ab): the code that finds the
// RESOURCE\*.SET data sets at start-up and opens their dBase tables.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

#include "database.hpp"
#include "dynarray.hpp"
#include "res_idx.hpp"

namespace joan::orig {

#pragma pack(push, 1)

/// Fixed-capacity string (400 characters + NUL), used by value on the stack.
/// substr(), upper(), lower() and operator*= work in one shared static buffer.
struct String {
    static constexpr int kMaxLen = 0x190;  ///< characters, excluding the NUL
    char str_buf[kMaxLen + 1];  ///< the text, NUL terminated

    [[nodiscard]] char* c_str() { return str_buf; }  ///< 0x00417d90 (returns this)
    String* construct();  ///< 0x005a1170 String()
    String* construct(const char* s);  ///< 0x005a11a0 String(char*)
    String* construct_copy(const String* s);  ///< 0x005a11f0 String(String&)
    char* substr(std::int32_t pos, std::int32_t len);  ///< 0x005a1230
    char* upper();  ///< 0x005a12e0
    char* lower();  ///< 0x005a1340
    std::int32_t at(const char* sub);  ///< 0x005a13a0 index of `sub`, -1 if absent
    String* assign(const String* s);  ///< 0x005a1400 operator=(String&)
    String* assign(const char* s);  ///< 0x005a1440 operator=(char*)
    String* assign(std::int32_t value);  ///< 0x005a1490 operator=(int)
    String* append_str(const String* s);  ///< 0x005a14f0 operator+=(String&)
    String* append(const char* s);  ///< 0x005a1550 operator+=(char*)
    String* append(std::int32_t value);  ///< 0x005a15d0 operator+=(int)
    String* repeat(std::int32_t times);  ///< 0x005a1640 operator*=(int)
    String* append(char c);  ///< 0x005a16d0 operator+=(char)

    // The other String functions the game code calls (typed, through their slots).
#include "gen/string_methods.inc"
};

/// One Directory record (a WIN32_FIND_DATA digest), 0x111 bytes.
struct FileInfo {
    char name[0x105];  ///< file name (cFileName, truncated to 0x104 characters)
    std::uint32_t size;  ///< nFileSizeLow
    std::uint32_t time_lo;  ///< ftLastWriteTime
    std::uint32_t time_hi;  ///< high dword of the FILETIME
};

/// Directory listing: a DynArray of FileInfo.
struct Directory : DynArray {
    Directory* construct();  ///< 0x00582ad0
    std::int32_t read(const char* wildcard, std::int32_t sort);  ///< 0x00582b00

    // The other Directory functions the game code calls (typed, through their slots).
#include "gen/directory_methods.inc"
};

/// Header record of each .SET file.
struct SetInfo {
    char code[9];  ///< e.g. "STD"
    char des[61];  ///< human-readable description of the set
};

/// The data-set manager (global game set): finds the RESOURCE\*.SET files, remembers the current set and opens its
/// tables. Owns set_info_array; set_res / set_db are the open set's index and table.
struct GameSet {
    char init_flag;  ///< 0x000
    std::int16_t cur_set_id;  ///< 0x001  1-based
    std::int16_t set_count;  ///< 0x003
    SetInfo* set_info_array;  ///< 0x005
    ResourceIdx set_res;  ///< 0x009
    Database set_db;  ///< 0x13e
    char set_opened_flag;  ///< 0x285

    void init();  ///< 0x0058adf0
    void deinit();  ///< 0x0058ae30
    void load_set_header();  ///< 0x0058ae90
    void open_set(std::int32_t set_id);  ///< 0x0058b080 (only the low 16 bits are kept)
    void close_set();  ///< 0x0058b140
    Database* open_db(const char* name);  ///< 0x0058b180
    Database* get_db();  ///< 0x0058b220
    std::int32_t find_set(const char* code);  ///< 0x0058b250 1-based, 0 if absent

    // The other GameSet functions the game code calls (typed, through their slots).
#include "gen/game_set_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(String) == 0x191);
static_assert(sizeof(FileInfo) == 0x111);
static_assert(sizeof(Directory) == sizeof(DynArray));
static_assert(sizeof(SetInfo) == 0x46);
static_assert(offsetof(GameSet, set_res) == 0x9);
static_assert(offsetof(GameSet, set_db) == 0x13e);
static_assert(offsetof(GameSet, set_opened_flag) == 0x285);
#endif

}  // namespace joan::orig
