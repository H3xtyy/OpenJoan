// Reimplementation of String (d:\ww\ilib\str.cpp, 15 functions), Directory
// (d:\ww\ilib\dir.cpp, 2), GameSet (d:\ww\ilib\gameset.cpp, 8) and
// Misc::extract_file_name - 26 functions. Decompiled 1:1 including quirks.
//
// Calls into other engine classes go through their incremental-link thunks,
// exactly like the original code, so they reach whichever implementation
// (original or reimplemented) is installed.
//
// Until stage S7c a verify mode ran joan.exe's original next to the String functions and the GameSet
// lookups and compared the results; joan.exe's code no longer exists and the comparison code was removed.
// (This note keeps the comment's length: the mem() calls below pass __LINE__ where the original passes its
// own source line.)
#include "../platform/win32_types.hpp"

#include "../platform/files.hpp"

#include <cstring>
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../original/engine.hpp"
#include "../original/gameset.hpp"
#include "../registry.hpp"
namespace {
constexpr const char* kSrc = "joan-re/src/ilib/gameset.cpp";

JOAN_COUNTER(c_str_ctor, "String::String()");
JOAN_COUNTER(c_str_ctor_s, "String::String(char*)");
JOAN_COUNTER(c_str_ctor_copy, "String::String(String&)");
JOAN_COUNTER(c_str_substr, "String::substr");
JOAN_COUNTER(c_str_upper, "String::upper");
JOAN_COUNTER(c_str_lower, "String::lower");
JOAN_COUNTER(c_str_at, "String::at");
JOAN_COUNTER(c_str_assign_str, "String::operator=(String&)");
JOAN_COUNTER(c_str_assign, "String::operator=(char*)");
JOAN_COUNTER(c_str_assign_int, "String::operator=(int)");
JOAN_COUNTER(c_str_append_str, "String::operator+=(String&)");
JOAN_COUNTER(c_str_append, "String::operator+=(char*)");
JOAN_COUNTER(c_str_append_int, "String::operator+=(int)");
JOAN_COUNTER(c_str_repeat, "String::operator*=(int)");
JOAN_COUNTER(c_str_append_char, "String::operator+=(char)");
JOAN_COUNTER(c_dir_ctor, "Directory::Directory");
JOAN_COUNTER(c_dir_read, "Directory::read");
JOAN_COUNTER(c_gs_init, "GameSet::init");
JOAN_COUNTER(c_gs_deinit, "GameSet::deinit");
JOAN_COUNTER(c_gs_header, "GameSet::load_set_header");
JOAN_COUNTER(c_gs_open_set, "GameSet::open_set");
JOAN_COUNTER(c_gs_close_set, "GameSet::close_set");
JOAN_COUNTER(c_gs_open_db, "GameSet::open_db");
JOAN_COUNTER(c_gs_get_db, "GameSet::get_db");
JOAN_COUNTER(c_gs_find_set, "GameSet::find_set");
JOAN_COUNTER(c_misc_extract, "Misc::extract_file_name");

using joan::hook::original;
using joan::orig::Database;
using joan::orig::Directory;
using joan::orig::DynArray;
using joan::orig::ResourceIdx;
using joan::orig::String;

// String's shared result buffer (0x00a667cc).
char* static_buf() { return &joan::hook::global<char>(d::g_a667cc); }

// Locale-dependent case conversion of the game's CRT (_strupr 0x007b1800, _strlwr 0x007c5dc0).
void crt_strupr(char* s) { original<char*(JOAN_CDECL*)(char*)>(fn::crt_strupr)(s); }
void crt_strlwr(char* s) { original<char*(JOAN_CDECL*)(char*)>(fn::crt_strlwr)(s); }

// Misc::format(int value, int type) inline wrapper (0x004d69a0, via its thunk): number as text.
char* misc_format(std::int32_t value, std::int32_t type) {
    using Fn = char* (joan::orig::Misc::*)(std::int32_t, std::int32_t);
    return (joan::orig::misc().*original<Fn>(fn::Misc_format_int))(value, type);
}
// Misc::str_cut(dst, src, max_len) (0x00590210 via thunk): copies the trimmed text of a dBase field.
void misc_str_cut(char* dst, const char* src, std::int32_t max_len) {
    using Fn = void (joan::orig::Misc::*)(char*, const char*, std::int32_t);
    (joan::orig::misc().*original<Fn>(fn::copy_field))(dst, src, max_len);
}
void misc_extract_file_name(char* dst, const char* path) {
    using Fn = void (joan::orig::Misc::*)(char*, const char*);
    (joan::orig::misc().*original<Fn>(fn::Misc_extract_file_name))(dst, path);
}

// Thunks of the other engine classes GameSet and Directory use.
void res_init(ResourceIdx* r, const char* name, char read_all, char use_common_buf) {
    using Fn = void (ResourceIdx::*)(const char*, char, char);
    (r->*original<Fn>(fn::init_59c550))(name, read_all, use_common_buf);
}
void res_deinit(ResourceIdx* r) { (r->*original<void (ResourceIdx::*)()>(fn::deinit_59c780))(); }
char* res_read(ResourceIdx* r, const char* name) {
    return (r->*original<char* (ResourceIdx::*)(const char*)>(fn::read_name_59c950))(name);
}
void db_open_mem(Database* d, char* data, std::int32_t read_fields) {
    (d->*original<void (Database::*)(char*, std::int32_t)>(fn::open_mem_580ae0))(data, read_fields);
}
char* db_read(Database* d, std::int32_t rec) {
    return (d->*original<char* (Database::*)(std::int32_t)>(fn::read_580c70))(rec);
}
void da_construct(DynArray* a, std::int32_t size, std::int32_t block, std::int32_t init) {
    (a->*original<DynArray* (DynArray::*)(std::int32_t, std::int32_t, std::int32_t)>(fn::DynArray))(size, block, init);
}
void da_destruct(DynArray* a) { (a->*original<void (DynArray::*)()>(fn::dtor_DynArray))(); }
void da_linkin(DynArray* a, const void* ent) {
    (a->*original<void (DynArray::*)(const void*)>(fn::linkin_584620))(ent);
}
void da_quick_sort(DynArray* a, int(JOAN_CDECL* cmp)(const void*, const void*)) {
    (a->*original<void (DynArray::*)(int(JOAN_CDECL*)(const void*, const void*))>(fn::quick_sort))(cmp);
}

// strncat(dst, src, kMaxLen - strlen(dst)), then force the terminator, as every += does.
void cat_limited(String* s, const char* src) {
    std::strncat(s->str_buf, src, static_cast<std::size_t>(String::kMaxLen - static_cast<int>(std::strlen(s->str_buf))));
    s->str_buf[String::kMaxLen] = 0;
}
}  // namespace

namespace joan::orig {

// ---------------------------------------------------------------------------
// String
// ---------------------------------------------------------------------------

// 0x005a1170
String* String::construct() {
    c_str_ctor.hit();
    str_buf[0] = 0;
    return this;
}

// 0x005a11a0
String* String::construct(const char* s) {
    c_str_ctor_s.hit();
    std::strncpy(str_buf, s, kMaxLen);
    str_buf[kMaxLen] = 0;
    return this;
}

// 0x005a11f0 - copies all 401 bytes, garbage after the terminator included.
String* String::construct_copy(const String* s) {
    c_str_ctor_copy.hit();
    std::memcpy(str_buf, s->str_buf, sizeof(str_buf));
    return this;
}

// 0x005a1230 - `len` characters from `pos` (len <= 0: to the end) in the static buffer.
// Quirk kept: if `len` reaches past the end, the result points into this string itself.
char* String::substr(std::int32_t pos, std::int32_t len) {
    c_str_substr.hit();
    char* const out = static_buf();
    out[0] = 0;
    const auto n = static_cast<std::int32_t>(std::strlen(str_buf));
    if (pos >= n) return out;
    if (len <= 0) len = n - pos;
    else if (len > n - pos) return str_buf + pos;
    std::strncpy(out, str_buf + pos, static_cast<std::size_t>(len));
    out[len] = 0;
    return out;
}

// 0x005a12e0
char* String::upper() {
    c_str_upper.hit();
    char* const out = static_buf();
    std::memcpy(out, str_buf, std::strlen(str_buf) + 1);
    crt_strupr(out);
    return out;
}

// 0x005a1340
char* String::lower() {
    c_str_lower.hit();
    char* const out = static_buf();
    std::memcpy(out, str_buf, std::strlen(str_buf) + 1);
    crt_strlwr(out);
    return out;
}

// 0x005a13a0
std::int32_t String::at(const char* sub) {
    c_str_at.hit();
    const char* p = std::strstr(str_buf, sub);
    return p ? static_cast<std::int32_t>(p - str_buf) : -1;
}

// 0x005a1400
String* String::assign(const String* s) {
    c_str_assign_str.hit();
    std::memcpy(str_buf, s->str_buf, sizeof(str_buf));
    return this;
}

// 0x005a1440
String* String::assign(const char* s) {
    c_str_assign.hit();
    std::strncpy(str_buf, s, kMaxLen);
    str_buf[kMaxLen] = 0;
    return this;
}

// 0x005a1490
String* String::assign(std::int32_t value) {
    c_str_assign_int.hit();
    std::strncpy(str_buf, misc_format(value, 1), kMaxLen);
    str_buf[kMaxLen] = 0;
    return this;
}

// 0x005a14f0
String* String::append_str(const String* s) {
    c_str_append_str.hit();
    cat_limited(this, s->str_buf);
    return this;
}

// 0x005a1550
String* String::append(const char* s) {
    c_str_append.hit();
    cat_limited(this, s);
    return this;
}

// 0x005a15d0
String* String::append(std::int32_t value) {
    c_str_append_int.hit();
    // The original measures the length before formatting; misc_format does not touch this string.
    const auto room = static_cast<std::size_t>(kMaxLen - static_cast<int>(std::strlen(str_buf)));
    std::strncat(str_buf, misc_format(value, 1), room);
    str_buf[kMaxLen] = 0;
    return this;
}

// 0x005a1640 - appends the string to itself `times - 1` times.
// Quirk kept: each strncat may add up to 400 characters regardless of the
// remaining room, so long strings overflow the buffer exactly as in the original.
String* String::repeat(std::int32_t times) {
    c_str_repeat.hit();
    char* const copy = static_buf();
    std::memcpy(copy, str_buf, std::strlen(str_buf) + 1);
    for (std::int32_t i = 1; i < times; ++i) std::strncat(str_buf, copy, kMaxLen);
    str_buf[kMaxLen] = 0;
    return this;
}

// 0x005a16d0
String* String::append(char c) {
    c_str_append_char.hit();
    const char tmp[2] = {c, 0};
    std::strncat(str_buf, tmp, kMaxLen);
    str_buf[kMaxLen] = 0;
    return this;
}

// ---------------------------------------------------------------------------
// Misc::extract_file_name (d:\ww\ilib\misc.cpp, 0x00590d90)
// ---------------------------------------------------------------------------

void Misc::extract_file_name(char* dst, const char* path) {
    c_misc_extract.hit();
    auto i = static_cast<std::int32_t>(std::strlen(path));
    while (i >= 0 && path[i] != '\\') --i;  // starts at the terminator, as the original
    std::strncpy(dst, path + i + 1, 0x104);
    dst[0x104] = 0;
}

// ---------------------------------------------------------------------------
// Directory
// ---------------------------------------------------------------------------

// 0x00582ad0
Directory* Directory::construct() {
    c_dir_ctor.hit();
    da_construct(this, sizeof(FileInfo), 0x14, 0);
    return this;
}

// 0x00582b00 - appends every match of `wildcard` (names only); sort != 0 sorts
// them case-insensitively with the game's own comparator. Returns the count.
// Quirk kept: FindClose is called even when nothing matched (handle -1).
std::int32_t Directory::read(const char* wildcard, std::int32_t sort) {
    c_dir_read.hit();
    WIN32_FIND_DATAA fd;
    const HANDLE h = joan::platform::files::FindFirstFileA(wildcard, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            FileInfo info;
            misc_extract_file_name(info.name, fd.cFileName);
            info.size = fd.nFileSizeLow;
            info.time_lo = fd.ftLastWriteTime.dwLowDateTime;
            info.time_hi = fd.ftLastWriteTime.dwHighDateTime;
            da_linkin(this, &info);
        } while (joan::platform::files::FindNextFileA(h, &fd));
    }
    joan::platform::files::FindClose(h);
    if (sort) {
        using Cmp = int(JOAN_CDECL*)(const void*, const void*);
        da_quick_sort(this, reinterpret_cast<Cmp>(fn::code_value(fn::stricmp_wrap_582bf0)));  // _stricmp on the names
    }
    return size();
}

// ---------------------------------------------------------------------------
// GameSet
// ---------------------------------------------------------------------------

// 0x0058adf0
void GameSet::init() {
    c_gs_init.hit();
    set_opened_flag = 0;
    (this->*original<void (GameSet::*)()>(fn::load_set_header))();  // load_set_header
    init_flag = 1;
}

// 0x0058ae30
void GameSet::deinit() {
    c_gs_deinit.hit();
    if (!init_flag) return;
    (this->*original<void (GameSet::*)()>(fn::close_set))();  // close_set
    mem().del(set_info_array, kSrc, __LINE__);
    init_flag = 0;
}

// 0x0058ae90 - reads the HEADER table of every RESOURCE\*.SET into set_info_array.
void GameSet::load_set_header() {
    c_gs_header.hit();
    Directory dir;
    (dir.*original<Directory* (Directory::*)()>(fn::Directory_Directory))();  // Directory()
    String str;
    (str.*original<String* (String::*)()>(fn::String))();       // String()
    (dir.*original<std::int32_t (Directory::*)(const char*, std::int32_t)>(fn::Directory_read))("RESOURCE\\*.SET", 1);
    set_count = static_cast<std::int16_t>(dir.size());
    set_info_array = static_cast<SetInfo*>(mem().add(static_cast<std::uint32_t>(set_count * sizeof(SetInfo)), kSrc, __LINE__));
    using AssignFn = String* (String::*)(const char*);
    for (std::int32_t i = 1; i <= dir.size(); ++i) {
        (str.*original<AssignFn>(fn::op_assign_char_ptr))("RESOURCE\\");
        (str.*original<AssignFn>(fn::op_add_assign_char_ptr))(dir.get(i));  // += file name
        res_init(&set_res, str.str_buf, 0, 0);
        char* header = res_read(&set_res, "HEADER");
        db_open_mem(&set_db, header, 0);
        char* rec = db_read(&set_db, 1);
        SetInfo* info = &set_info_array[i - 1];
        misc_str_cut(info->code, rec, 8);
        misc_str_cut(info->des, rec + 8, 0x3c);
    }
    da_destruct(&dir);
}

// 0x0058b080 - opens RESOURCE\<code>.SET. The previous set is not closed first.
void GameSet::open_set(std::int32_t set_id) {
    c_gs_open_set.hit();
    cur_set_id = static_cast<std::int16_t>(set_id);
    String str;
    (str.*original<String* (String::*)()>(fn::String))();
    using AssignFn = String* (String::*)(const char*);
    (str.*original<AssignFn>(fn::op_assign_char_ptr))("RESOURCE\\");
    (str.*original<AssignFn>(fn::op_add_assign_char_ptr))(set_info_array[set_id - 1].code);
    (str.*original<AssignFn>(fn::op_add_assign_char_ptr))(".SET");
    res_init(&set_res, str.str_buf, 0, 0);
    set_opened_flag = 1;
}

// 0x0058b140
void GameSet::close_set() {
    c_gs_close_set.hit();
    res_deinit(&set_res);
    set_opened_flag = 0;
}

// 0x0058b180 - opens table `name` of the current set. A missing table is a
// fatal error, except "RELEASE" (then a null table is opened, as in the original).
Database* GameSet::open_db(const char* name) {
    c_gs_open_db.hit();
    char* data = res_read(&set_res, name);
    if (!data && std::strcmp(name, "RELEASE") != 0) err_run("Error Loading %s", name);
    db_open_mem(&set_db, data, 0);
    return &set_db;
}

// 0x0058b220
Database* GameSet::get_db() {
    c_gs_get_db.hit();
    return &set_db;
}

// 0x0058b250 - case-sensitive lookup of a set code; 1-based, 0 if absent.
std::int32_t GameSet::find_set(const char* code) {
    c_gs_find_set.hit();
    for (std::int32_t i = 0; i < set_count; ++i)
        if (std::strcmp(set_info_array[i].code, code) == 0) return i + 1;
    return 0;
}

}  // namespace joan::orig

namespace {
using joan::orig::GameSet;
}  // namespace

#define STR(name, addr, fn) JOAN_REPLACE("ilib/str.cpp", "String::" name, addr, fn)
#undef STR

#define GS(name, addr, fn) JOAN_REPLACE("ilib/gameset.cpp", "GameSet::" name, addr, fn)
GS("init", 0x0058adf0, &GameSet::init);
GS("deinit", 0x0058ae30, &GameSet::deinit);
GS("load_set_header", 0x0058ae90, &GameSet::load_set_header);
GS("open_set", 0x0058b080, &GameSet::open_set);
GS("close_set", 0x0058b140, &GameSet::close_set);
GS("open_db", 0x0058b180, &GameSet::open_db);
GS("get_db", 0x0058b220, &GameSet::get_db);
GS("find_set", 0x0058b250, &GameSet::find_set);
#undef GS
