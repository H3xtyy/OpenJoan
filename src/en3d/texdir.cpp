// en3d/texdir.cpp: functions of the original d:\ww\en3d\texdir.cpp (re/attribution.csv).
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
#include <cmath>
#include <limits>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

void* const kMem = obj(d::g_mem);

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00604150 TexDir::clear()                               thiscall
//
//      0x00604440 TexDir::add(const char*)                      thiscall, ret 4
//
//      0x00604650 TexDir::add_entry(name, flags, list, kind)    thiscall, ret 0x10
namespace {
namespace game_b92_05 {

// Memory manager free / realloc with source file and line (0x0058ed70 / 0x0058eb30).
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}
void* mem_realloc(void* p, std::int32_t size, img::Addr file, std::int32_t line) {
    return joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058eb30
}
using joan::orig::helpers::line_base;

constexpr img::Addr kTexDirCpp = d::s_d_ww_en3d_texdir_cpp;    // 'd:\ww\en3d\texdir.cpp'

void crt_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }
void crt_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(d))), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_texdir_clear, "TexDir::clear");
JOAN_COUNTER(c_texdir_add, "TexDir::add");
JOAN_COUNTER(c_texdir_add_entry, "TexDir::add_entry");

struct H {
    // 0x00604150
    void texdir_clear() {
        c_texdir_clear.hit();
        if (jo::self<jo::TexDir>(this)->ready == 0) return;
        jo::self<jo::TexDir>(this)->tag_count = 0;
        if (jo::self<jo::TexDir>(this)->tags != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::TexDir>(this)->tags), kTexDirCpp, line_base(d::g_a18524) + 6);
            jo::self<jo::TexDir>(this)->tags = nullptr;
        }
        jo::self<jo::TexDir>(this)->tag_cap = 0;
        for (std::int32_t i = jo::self<jo::TexDir>(this)->dir_count; i > 0; --i) {
            if (jo::self<jo::TexDir>(this)->dirs[i - 1].buf_2d != nullptr)
                mem_free(jo::self<jo::TexDir>(this)->dirs[i - 1].buf_2d, kTexDirCpp, line_base(d::g_a18524) + 0x10);
            if (jo::self<jo::TexDir>(this)->dirs[i - 1].files != nullptr)
                mem_free(jo::view<void*>(jo::self<jo::TexDir>(this)->dirs[i - 1].files), kTexDirCpp, line_base(d::g_a18524) + 0x14);
        }
        jo::self<jo::TexDir>(this)->dir_count = 0;
        if (jo::self<jo::TexDir>(this)->dirs != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::TexDir>(this)->dirs), kTexDirCpp, line_base(d::g_a18524) + 0x1a);
            jo::self<jo::TexDir>(this)->dirs = nullptr;
        }
        jo::self<jo::TexDir>(this)->dir_cap = 0;
        jo::self<jo::TexDir>(this)->ready = 0;
    }

    // 0x00604440
    std::int32_t texdir_add(const char* name) {
        c_texdir_add.hit();
        const std::int32_t r = joan::orig::arg::as<std::int32_t>(joan::orig::as_tex_dir(this)->dir_index(joan::orig::arg::str(name)));  // 0x006054c0
        if (r >= 0) return r;
        if (!(jo::self<jo::TexDir>(this)->dir_cap > jo::self<jo::TexDir>(this)->dir_count)) {
            jo::self<jo::TexDir>(this)->dir_cap = jo::self<jo::TexDir>(this)->dir_count + 4;
            jo::view<void*>(jo::self<jo::TexDir>(this)->dirs) = mem_realloc(jo::view<void*>(jo::self<jo::TexDir>(this)->dirs), jo::self<jo::TexDir>(this)->dir_cap * static_cast<std::int32_t>(sizeof(jo::TexDirEntry)), kTexDirCpp, line_base(d::g_a1852c) + 0xc);
        }
        const std::int32_t idx = jo::self<jo::TexDir>(this)->dir_count;
        crt_memset(&jo::self<jo::TexDir>(this)->dirs[idx], 0, static_cast<std::uint32_t>(sizeof(jo::TexDirEntry)));  // 0x3d on x86
        jo::self<jo::TexDir>(this)->dirs[idx].index = jo::self<jo::TexDir>(this)->dir_count;
        crt_strcpy(jo::self<jo::TexDir>(this)->dirs[idx].name, name);  // dirs + idx * 0x3d + 4 (x86)
        const std::int32_t n = jo::self<jo::TexDir>(this)->dir_count + 1;
        jo::self<jo::TexDir>(this)->dir_count = n;
        return n;
    }

    // 0x00604650
    std::int32_t texdir_add_entry(const char* name, std::uint32_t flags, jo::TexDirEntry* list, std::uint32_t kind) {
        c_texdir_add_entry.hit();
        constexpr std::int32_t kFolder = static_cast<std::int32_t>(sizeof(jo::TexFolder));  // 0x10c, no pointers
        for (std::int32_t i = 0; i < list->file_count; ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(name), joan::orig::arg::str(static_cast<const char*>(list->files[i].path))) == 0) {
                list->files[i].recurse = static_cast<std::uint8_t>(list->files[i].recurse | static_cast<std::uint8_t>(flags));
                return i;
            }
        }
        if (!(list->file_cap > list->file_count)) {
            list->file_cap = list->file_count + 4;
            jo::view<void*>(list->files) = mem_realloc(jo::view<void*>(list->files), list->file_cap * kFolder, kTexDirCpp,
                                                line_base(d::g_a18530) + 0x16);
        }
        const std::int32_t idx = list->file_count;
        crt_memset(&list->files[idx], 0, kFolder);
        crt_strcpy(list->files[idx].path, name);
        list->files[idx].recurse = static_cast<std::uint8_t>(flags);
        list->files[idx].tex_state = 0;
        list->files[idx].sub_state = 0;
        list->files[idx].kind = static_cast<std::uint8_t>(kind);
        list->file_count = list->file_count + 1;
        return idx;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("TextureDirectory::deinit", 0x00604150, &H::texdir_clear);
B("TextureDirectory::add_prefix", 0x00604440, &H::texdir_add);
B("TextureDirectory::add_path_internal", 0x00604650, &H::texdir_add_entry);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
//
//      0x00604310 TexDir::add(name)          (thiscall, ret 4) - adds an 8-byte entry (name,
//                 word bit mask at +6) unless _stricmp finds it.
namespace {
namespace game_b95_06 {

JOAN_COUNTER(c_texdir_add, "TexDir::add");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }        // _stricmp
using joan::orig::helpers::strcpy_;  // strcpy
void* mem_realloc(void* p, u32 size, const char* file, i32 line) {
    return joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}

struct H {
    // ------------------------------------------------------------------------ 0x00604310
    void texdir_add(const char* name) {
        c_texdir_add.hit();
        for (i32 i = 0; i < jo::self<jo::TexDir>(this)->tag_count; ++i)
            if (stricmp_(name, jo::view<u8*>(jo::self<jo::TexDir>(this)->tags) + i * 8) == 0) return;
        if (!(jo::self<jo::TexDir>(this)->tag_cap > jo::self<jo::TexDir>(this)->tag_count)) {
            jo::self<jo::TexDir>(this)->tag_cap = jo::self<jo::TexDir>(this)->tag_count + 4;
            const i32 line = static_cast<i32>(global<i16>(d::g_a18528)) + 0xe;
            jo::view<void*>(jo::self<jo::TexDir>(this)->tags) = mem_realloc(jo::view<void*>(jo::self<jo::TexDir>(this)->tags), static_cast<u32>(jo::self<jo::TexDir>(this)->tag_cap) << 3, str(d::s_d_ww_en3d_texdir_cpp), line);
        }
        strcpy_(reinterpret_cast<char*>(jo::view<u8*>(jo::self<jo::TexDir>(this)->tags) + jo::self<jo::TexDir>(this)->tag_count * 8), name);
        const u32 bit = 1u << (static_cast<u32>(jo::self<jo::TexDir>(this)->tag_count) & 31);
        jo::self<jo::TexDir>(this)->tags[jo::self<jo::TexDir>(this)->tag_count].bit = static_cast<u16>(bit);
        jo::self<jo::TexDir>(this)->tag_count = jo::self<jo::TexDir>(this)->tag_count + 1;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("TextureDirectory::add_extension", 0x00604310, &H::texdir_add);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x00604560 TexDir::add_path(path, a, key, b) (thiscall, ret 0x10) - strips a trailing '\'.
namespace {
namespace game_b96_06 {

using joan::orig::helpers::strlen_;

JOAN_COUNTER(c_add_path, "TexDir::add_path");

struct H {
    // ------------------------------------------------------------------ 0x00604560
    u32 add_path(const char* path, u32 a, std::uintptr_t key, u32 b) {  // key: a directory-name string (pointer, 0 = none; the facade passes it whole)
        c_add_path.hit();
        i32 index = 0;
        if (key != 0) index = joan::orig::arg::as<i32>(joan::orig::as_tex_dir(this)->dir_index(reinterpret_cast<const char*>(key)));  // 0x006054c0
        u8* const entry = reinterpret_cast<u8*>(reinterpret_cast<std::uintptr_t>(jo::self<jo::TexDir>(this)->dirs) + static_cast<u32>(index) * static_cast<u32>(sizeof(jo::TexDirEntry)));  // address-sized arithmetic, as the original
        char buf[0x104];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(path));  // strcpy
        const u32 len = strlen_(buf);
        if (len >= 2 && static_cast<i32>(static_cast<i8>(buf[len - 1])) == 0x5c) buf[len - 1] = 0;
        return joan::orig::arg::as<u32>(joan::orig::as_tex_dir(this)->add_entry(joan::orig::arg::str(static_cast<const char*>(buf)), joan::orig::arg::u32(static_cast<u8>(a)), joan::orig::arg::mptr(static_cast<void*>(entry)), joan::orig::arg::u32(static_cast<u8>(b))));  // 0x00604650
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("TextureDirectory::poll_files", 0x00604560, &H::add_path);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
//
//      0x00604850 TexDir::reset_flags      (thiscall, ret 0)
namespace {
namespace game_b108_17 {

JOAN_COUNTER(c_texdir_reset, "TexDir::reset_flags");

struct H {
    // ------------------------------------------------------------------------ 0x00604850
    void texdir_reset() {
        c_texdir_reset.hit();
        for (i32 i = 0; i < jo::self<jo::TexDir>(this)->dir_count; ++i) {
            jo::TexDirEntry* const e = &jo::self<jo::TexDir>(this)->dirs[i];  // dirs + i * 0x3d (x86)
            for (i32 j = 0; j < e->file_count; ++j) {
                e->files[j].tex_state = 0;
                e->files[j].sub_state = 0;
            }
            e->name_count = 0;
            for (i32 j = 0; j < e->file_count; ++j) joan::orig::as_tex_dir(this)->scan_604960(joan::orig::arg::mptr(e), joan::orig::arg::i32(j));  // 0x00604960
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("TexDir::reset_flags", 0x00604850, &H::texdir_reset);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b109_11.cpp (game/b109_11)
//
//    - TexDir::scan_604960 0x00604960 (ret 8, en3d/texdir.cpp; callers drop eax): scans directory
//      record idx of the table (+0x39, records of 0x10c bytes): counts texture files (extension in
//      this->+0x18 table, name <= 32 chars) and sub-directories; inserts the textures sorted into
//      the table's name array (+0x2d, 0x28-byte entries, 0x005911a0 search / 0x005910f0 insert,
//      grown by 0x0058eb30), then recurses into the sub-directories (0x00604650 adds the record,
//      recursion through thunk 0x004020a9). CRT _makepath / _findfirst / _findnext / _findclose /
//      _splitpath / strcmp / _stricmp / strlen / strcpy called at their addresses.
//
//  - 0x00604960: the stack buffers are larger than the original's; contents are identical. The
//    sub-directory path ([ebp-0x760], 0x108 bytes) can only spill into locals that are dead by
//    then; the search spec ([ebp-0x114], 0x108 bytes) would reach the live bytes b8 / bc only
//    for a record path of >= 259 characters (beyond MAX_PATH, never produced in practice).
//    The insert position written by 0x005911a0 is read only when it returns 0 (it is always
//    written on that path); it is zero-initialised here.
namespace {
namespace game_b109_11 {

void* const kRes = obj(d::g_misc);

JOAN_COUNTER(c_texdir, "TexDir::scan_604960");

struct H {
    // ------------------------------------------------------------ 0x00604960
    static u8* dir_rec(u8* tab, i32 idx) { return reinterpret_cast<u8*>(&jo::as_tex_dir_entry(tab)->files[idx]); }

    // Extension lookup in this->+0x18 (8-byte entries, count +0x10); returns the word at +6 or 0.
    i16 tex_code(const char* ext) const {
        for (i32 i = 0; i < jo::self<jo::TexDir>(this)->tag_count; ++i) {
            jo::TexTag* const e = jo::as_tex_tag(jo::view<u8*>(jo::self<jo::TexDir>(this)->tags) + i * 8);
            if (joan::crt::stricmp(joan::orig::arg::str(ext), joan::orig::arg::str(static_cast<void*>(e))) == 0) return jo::view<i16>(e->bit);  // _stricmp
        }
        return 0;
    }

    void texdir_scan(u8* tab, i32 idx) {
        c_texdir.hit();
        const u8 b8 = jo::as_tex_folder(dir_rec(tab, idx))->recurse;
        const u8 bc = jo::as_tex_folder(dir_rec(tab, idx))->kind;
        if (jo::view<i8>(jo::as_tex_folder(dir_rec(tab, idx))->tex_state) != 0 &&
            (static_cast<i8>(b8) == 0 || jo::view<i8>(jo::as_tex_folder(dir_rec(tab, idx))->sub_state) != 0))
            return;
        if (static_cast<i8>(bc) != 0) return;
        char spec[0x220];
        joan::orig::libc::_makepath(joan::orig::arg::mptr(spec), joan::orig::arg::mptr(static_cast<const char*>(nullptr)), joan::orig::arg::mptr(// _makepath
                         reinterpret_cast<const char*>(dir_rec(tab, idx))), joan::orig::arg::mptr(str(d::s_star)), joan::orig::arg::mptr(static_cast<const char*>(nullptr)));
        i32 ndirs = 0;
        i32 nfiles = 0;
        struct FindData {  // struct _finddata_t (MSVC 6), 0x118 bytes
            u32 attrib;
            i32 time_create;
            i32 time_access;
            i32 time_write;
            u32 size;
            char name[260];
        };
        static_assert(sizeof(FindData) == 0x118, "finddata size");
        FindData fd;
        char* const name = fd.name;
        auto find_first = [&]() { return joan::orig::arg::as<i32>(joan::orig::libc::_findfirst(joan::orig::arg::str(static_cast<const char*>(spec)), joan::orig::arg::mptr(static_cast<void*>(&fd)))); };
        auto find_next = [&](i32 h) { return joan::orig::arg::as<i32>(joan::orig::libc::_findnext(joan::orig::arg::i32(h), joan::orig::arg::mptr(static_cast<void*>(&fd)))); };
        auto find_close = [&](i32 h) { joan::orig::libc::_findclose(joan::orig::arg::i32(h)); };
        auto is_sub_dir = [&]() {
            return std::strcmp(joan::orig::arg::str(static_cast<const char*>(name)), str(d::s_dot_9d36e8)) != 0 &&  // strcmp
                   std::strcmp(joan::orig::arg::str(static_cast<const char*>(name)), str(d::s_dot_dot)) != 0;
        };
        char fname[0x100];
        char ext[0x100];
        i32 h = find_first();
        if (h != -1) {
            do {
                if ((fd.attrib & 0x10) != 0) {
                    if (static_cast<i8>(b8) != 0 && is_sub_dir()) ++ndirs;
                } else {
                    joan::orig::libc::_splitpath(joan::orig::arg::mptr(static_cast<const char*>(name)), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(// _splitpath
                                     static_cast<char*>(nullptr)), joan::orig::arg::mptr(fname), joan::orig::arg::mptr(ext));
                    const i16 code = tex_code(ext);
                    if (code != 0 && joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(static_cast<const char*>(fname)))) <= 0x20u) ++nfiles;  // strlen
                }
            } while (find_next(h) == 0);
            find_close(h);
        }
        if (jo::view<i8>(jo::as_tex_folder(dir_rec(tab, idx))->tex_state) == 0) {
            if (nfiles > 0) {
                jo::as_tex_folder(dir_rec(tab, idx))->tex_state = 1;
                if (jo::as_tex_dir_entry(tab)->name_count + nfiles > jo::as_tex_dir_entry(tab)->name_cap) {
                    jo::as_tex_dir_entry(tab)->name_cap = jo::as_tex_dir_entry(tab)->name_count + nfiles + 4;
                    const i32 line = static_cast<i32>(global<i16>(d::g_a18534)) + 0x5c;
                    jo::as_tex_dir_entry(tab)->buf_2d = joan::orig::arg::as<jo::TexName*>(joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::view<u8*>(jo::as_tex_dir_entry(tab)->buf_2d)), joan::orig::arg::u32(// 0x0058eb30
                                                       jo::as_tex_dir_entry(tab)->name_cap * static_cast<i32>(sizeof(jo::TexName))), str(d::s_d_ww_en3d_texdir_cpp), joan::orig::arg::i32(line)));
                }
                h = find_first();
                if (h != -1) {
                    do {
                        if ((fd.attrib & 0x10) == 0) {
                            char fname2[0x100];
                            char ext2[0x100];
                            joan::orig::libc::_splitpath(joan::orig::arg::mptr(static_cast<const char*>(name)), joan::orig::arg::mptr(// _splitpath
                                             static_cast<char*>(nullptr)), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(fname2), joan::orig::arg::mptr(ext2));
                            const i16 code = tex_code(ext2);
                            if (code != 0 && joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(static_cast<const char*>(fname2)))) <= 0x20u) {
                                alignas(4) i32 pos = 0;
                                const i32 found = joan::orig::arg::as<i32>(joan::orig::as_misc(kRes)->bin_search_5911a0(joan::orig::arg::mptr(static_cast<char*>(fname2)), joan::orig::arg::str(// 0x005911a0
                                                                jo::view<u8*>(jo::as_tex_dir_entry(tab)->buf_2d)), joan::orig::arg::i32(jo::as_tex_dir_entry(tab)->name_count), static_cast<i32>(sizeof(jo::TexName)), fn::code_value(fn::cmp_stricmp_605190), &pos));
                                if (found == 0) {
                                    u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_misc(kRes)->insert_array_rec_5910f0(joan::orig::arg::str(jo::view<u8*>(jo::as_tex_dir_entry(tab)->buf_2d)), joan::orig::arg::i32(// 0x005910f0
                                                                jo::as_tex_dir_entry(tab)->name_count), static_cast<i32>(sizeof(jo::TexName)), joan::orig::arg::i32(pos), joan::orig::arg::mptr(i32{0})));
                                    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(e))), joan::orig::arg::str(// strcpy
                                                      static_cast<const char*>(fname2)));
                                    jo::as_tex_name(e)->dir = idx;
                                    jo::view<i16>(jo::as_tex_name(e)->ext_mask) = code;
                                    jo::as_tex_dir_entry(tab)->name_count += 1;
                                } else {
                                    u8* const e = jo::view<u8*>(jo::as_tex_dir_entry(tab)->buf_2d) + (found - 1) * static_cast<i32>(sizeof(jo::TexName));
                                    if (idx == jo::as_tex_name(e)->dir) jo::as_tex_name(e)->ext_mask |= static_cast<u16>(code);
                                }
                            }
                        }
                    } while (find_next(h) == 0);
                    find_close(h);
                }
            }
            jo::as_tex_folder(dir_rec(tab, idx))->tex_state = 2;
        }
        if (static_cast<i8>(b8) == 0 || jo::view<i8>(jo::as_tex_folder(dir_rec(tab, idx))->sub_state) != 0) return;
        if (ndirs > 0) {
            jo::as_tex_folder(dir_rec(tab, idx))->sub_state = 1;
            h = find_first();
            if (h != -1) {
                do {
                    if ((fd.attrib & 0x10) != 0 && static_cast<i8>(b8) != 0 && is_sub_dir()) {
                        char sub[0x220];
                        joan::orig::libc::_makepath(joan::orig::arg::mptr(sub), joan::orig::arg::mptr(static_cast<const char*>(nullptr)), joan::orig::arg::mptr(// _makepath
                                         reinterpret_cast<const char*>(dir_rec(tab, idx))), joan::orig::arg::mptr(static_cast<const char*>(name)), joan::orig::arg::mptr(static_cast<const char*>(nullptr)));
                        const i32 ni = joan::orig::arg::as<i32>(joan::orig::as_tex_dir(this)->add_entry(joan::orig::arg::str(static_cast<char*>(sub)), joan::orig::arg::u32(b8), joan::orig::arg::mptr(tab), joan::orig::arg::u32(bc)));  // 0x00604650
                        joan::orig::as_tex_dir(this)->scan_604960(joan::orig::arg::mptr(tab), joan::orig::arg::i32(ni));  // 0x00604960
                    }
                } while (find_next(h) == 0);
                find_close(h);
            }
        }
        jo::as_tex_folder(dir_rec(tab, idx))->sub_state = 2;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_11", name, addr, fn)
B("TextureDirectory::scan_files_in_path", 0x00604960, &H::texdir_scan);
#undef B

}  // namespace game_b109_11
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Obj::call_604150 0x006040e0 (ret 0), Obj::call_609260 0x00608de0 (ret 0).
namespace {
namespace game_b99_07 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x006040e0
    u32 call_604150() {
        joan::orig::as_tex_dir(this)->clear();  // 0x00604150
        return 0;  // 0x00604150 returns nothing; the atexit caller ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::call_604150", 0x006040e0, &H::call_604150);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - Obj::init_604110 0x00604110 (ret 0): 0x604150, +0 = 1, 0x604440(0x9cfb9c).
namespace {
namespace game_b98_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00604110
    u32 init_604110() {
        joan::orig::as_tex_dir(this)->clear();  // 0x00604150
        jo::view<u32>(jo::self<jo::TexDir>(this)->ready) = 1;
        return joan::orig::arg::as<u32>(joan::orig::as_tex_dir(this)->add_604440(joan::orig::arg::str(img::addr(d::kEmpty))));  // 0x00604440
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("TextureDirectory::init", 0x00604110, &H::init_604110);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (en3d/helpers33)
//
//    - TexturePaths::dir_index 0x006054c0, TextureManager::set_name 0x0060a670,
//      Game::flush_loader 0x00603af0.
namespace {
namespace game_leaves81 {

using joan::call::cdecl_call;

struct H {
    // 0x006054c0 TexturePaths::dir_index(name): entries (+0xc, 0x3d bytes, name at +4,
    // count +4) compared with _stricmp; -1 when absent.
    std::int32_t tex_dir_index(const char* name) {
        for (std::int32_t i = 0; i < jo::self<jo::TexDir>(this)->dir_count; ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(name), joan::orig::arg::str(static_cast<const char*>(jo::self<jo::TexDir>(this)->dirs[i].name))) == 0)
                return i;
        }
        return -1;
    }
};

#define T33(name, addr, fn) JOAN_REPLACE("en3d/helpers33", name, addr, fn)
T33("TextureDirectory::find_prefix", 0x006054c0, &H::tex_dir_index);
#undef T33

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
//
//    - 0x00608e10 TexPaths::build        (thiscall, ret 0) -> eax of 0x00604850(0xa59718).
//
//  - 0x00608e10: the 3x3 directory table is indexed by the byte 0xa5804a (texture quality) =
//    CONFIG.DAT object 0xa57fb8 + 0x92. Its writers: the defaults 0x0045a010 (= 1, via 0x00459e30
//    at startup and from the options menu 0x0074a700), the raw 0xb05-byte CONFIG.DAT read
namespace {
namespace game_b108_16 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}
using joan::hook::global;
using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_texpaths, "TexPaths::build");

struct H {
    // --------------------------------------------------------------------- 0x00608e10
    u32 texpaths_build() {
        c_texpaths.hit();
        // Quality byte outside 0..2 (only possible through a hand-edited CONFIG.DAT): the original
        // indexes its 3x3 table out of range into other stack slots. Keep that case on the original
        // body (none of the callees below writes the byte, so the entry check is equivalent).
        {
            // Batch 118/03: with fix/uninit enabled (module_enabled, read on the first call) the C++
            // version runs instead and uses the quality-0 row of the table (neutral selector 0;
            // the original's out-of-range slots are caller / frame data, not reproducible).
            const i8 q = global<i8>(d::g_a5804a);
            if ((q < 0 || q > 2) && !fix_uninit()) {
                using Fn = u32 (H::*)();
                return (this->*joan::hook::original<Fn>(fn::TexPaths_build_body))();
            }
        }
        void* const reg = obj(d::g_texture_dir);
        void* const fs = obj(d::g_misc);
        joan::orig::as_texture_manager(this)->clear();  // 0x00609260
        jo::self<jo::TextureManager>(this)->mode_ = 0;
        jo::self<jo::TextureManager>(this)->requeue = 1;
        jo::self<jo::TextureManager>(this)->stamp = 1;
        joan::orig::as_tex_dir(reg)->add_604310(str(d::s_DDS_9d9c00));  // 0x00604310 ".DDS"
        joan::orig::as_tex_dir(reg)->add_604310(str(d::s_TGA_9d9bf8));  // 0x00604310 ".TGA"
        joan::orig::as_tex_dir(reg)->add_604310(str(d::s_BMP));  // 0x00604310 ".BMP"
        joan::orig::as_tex_dir(reg)->add_604440(str(d::s_ITEM_9d9bf0));  // 0x00604440 "ITEM"

        alignas(4) u8 path1[0x1a0];
        for (i8 n = global<i8>(d::g_a48858); n >= 0; n = static_cast<i8>(n - 1)) {
            joan::orig::as_string(path1)->construct();  // 0x005a1170
            const char* const dir = str(d::s_INT8X6);  // "INT8X6\\" (both branches of the original)
            if (joan::orig::arg::as<i32>(joan::orig::as_misc(fs)->is_dir_exist_590c00(joan::orig::arg::str(dir))) == 0) continue;  // 0x00590c00
            joan::orig::as_tex_dir(reg)->add_path(joan::orig::arg::str(dir), joan::orig::arg::u32(i32{0}), i32{0}, joan::orig::arg::u32(i32{0}));  // 0x00604560
            joan::orig::as_string(path1)->assign(joan::orig::arg::str(dir));         // 0x005a1440
            joan::orig::as_string(path1)->append(str(d::s_ITEM_9cf648));                       // 0x005a1550 "ITEM\\"
            const char* const p = joan::orig::arg::as<const char*>(joan::orig::as_string(path1)->c_str());  // 0x00417d90
            if (joan::orig::arg::as<i32>(joan::orig::as_misc(fs)->is_dir_exist_590c00(joan::orig::arg::str(p))) == 0) continue;            // 0x00590c00
            const char* const q = joan::orig::arg::as<const char*>(joan::orig::as_string(path1)->c_str());  // 0x00417d90
            joan::bind::member<&::game_b96_06::H::add_path>(reg, joan::orig::arg::str(q), joan::orig::arg::u32(i32{0}), reinterpret_cast<std::uintptr_t>(str(d::s_ITEM_9d9bf0)), joan::orig::arg::u32(i32{0}));  // 0x00604560 "ITEM"
        }
        joan::orig::as_tex_dir(reg)->add_path(str(d::s_SAVE), joan::orig::arg::u32(i32{0}), i32{0}, joan::orig::arg::u32(i32{1}));  // 0x00604560 "SAVE\\"

        const char* const lo = str(d::s_TEXTUREL);   // "TEXTUREL\\"
        const char* const mid = str(d::s_TEXTURE);  // "TEXTURE\\"
        const char* const hi = str(d::s_TEXTUREH);   // "TEXTUREH\\"
        const char* const table[9] = {lo, mid, hi, mid, lo, hi, hi, mid, lo};
        const img::Addr subdirs[5] = {d::s_MISC, d::s_PARTICLE, d::s_TERRAIN, d::s_UNIT_9d5748, d::s_ITEM_9cf648};
        // MISC\, PARTICLE\, TERRAIN\, UNIT\, ITEM\ (jump table 0x0060916e)

        alignas(4) u8 path2[0x1a0];
        for (i8 i = 0; i < 3; i = static_cast<i8>(i + 1)) {
            joan::orig::as_string(path2)->construct();  // 0x005a1170
            i32 quality = static_cast<i32>(global<i8>(d::g_a5804a));
            if (quality < 0 || quality > 2) quality = 0;  // fix/uninit only (entry check)
            const char* const dir = table[quality * 3 + i];
            if (joan::orig::arg::as<i32>(joan::orig::as_misc(fs)->is_dir_exist_590c00(joan::orig::arg::str(dir))) == 0) continue;  // 0x00590c00
            for (i8 j = 4; j >= 0; j = static_cast<i8>(j - 1)) {
                joan::orig::as_string(path2)->assign(joan::orig::arg::str(dir));     // 0x005a1440
                joan::orig::as_string(path2)->append(joan::orig::arg::str(str(subdirs[j])));  // 0x005a1550
                const char* const p = joan::orig::arg::as<const char*>(joan::orig::as_string(path2)->c_str());  // 0x00417d90
                if (joan::orig::arg::as<i32>(joan::orig::as_misc(fs)->is_dir_exist_590c00(joan::orig::arg::str(p))) != 0) {                    // 0x00590c00
                    const char* const q = joan::orig::arg::as<const char*>(joan::orig::as_string(path2)->c_str());  // 0x00417d90
                    joan::orig::as_tex_dir(reg)->add_path(joan::orig::arg::str(q), joan::orig::arg::u32(i32{0}), i32{0}, joan::orig::arg::u32(i32{0}));       // 0x00604560
                }
            }
            joan::orig::as_tex_dir(reg)->add_path(joan::orig::arg::str(dir), joan::orig::arg::u32(i32{1}), i32{0}, joan::orig::arg::u32(i32{0}));  // 0x00604560
        }
        joan::orig::as_tex_dir(reg)->reset_flags();  // 0x00604850
        return 0;  // 0x00604850 returns nothing; the caller (sysen) ignores the result
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("TextureArray::init", 0x00608e10, &H::texpaths_build);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

void* crt_memset(void* d, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x006040b0
    void* clear_1c() {
        crt_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::TexDir)));  // 0x1c (x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::clear_1c", 0x006040b0, &H::clear_1c);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using i32 = std::int32_t;

using joan::call::cdecl_call;

i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

// ---------------------------------------------------------------------------- cdecl helpers
// 0x00605190
static i32 JOAN_CDECL cmp_stricmp(const char* a, const char* b) { return crt_stricmp(a, b); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("cmp_stricmp_605190", 0x00605190, &cmp_stricmp);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_leaves81_2.cpp (en3d/helpers33)
namespace {
namespace game_leaves81_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::img::str;


JOAN_COUNTER(c_tex_find, "TexturePaths::find");

struct finddata {  // MSVC 6 _finddata_t
    std::uint32_t attrib;
    std::int32_t time_create, time_access, time_write;
    std::uint32_t size;
    char name[260];
};
static_assert(sizeof(finddata) == 0x118, "_finddata_t");

struct H {
    // 0x00605540 TexturePaths::find(fname, dir, out, recurse): out = dir\fname.ext for the
    // first known extension (+0x18 table, +0x10 count) whose mask matches the files found;
    // else (recurse) the same in each sub directory. Returns out or 0.
    char* tex_find(const char* fname, const char* dir, char* out, std::int32_t recurse) {
        c_tex_find.hit();
        std::int16_t mask = 0;
        char spec[260];
        joan::orig::libc::_makepath(joan::orig::arg::mptr(spec), joan::orig::arg::mptr(static_cast<const char*>(nullptr)), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(fname), joan::orig::arg::mptr(&img::global<const char>(d::s_dot_star)));  // _makepath ".*"
        finddata fd;
        std::int32_t handle = joan::orig::arg::as<std::int32_t>(joan::orig::libc::_findfirst(joan::orig::arg::str(static_cast<const char*>(spec)), &fd));  // _findfirst
        if (handle != -1) {
            do {
                if ((fd.attrib & 0x10) != 0) continue;
                char name[256];
                char ext[256];
                joan::orig::libc::_splitpath(joan::orig::arg::mptr(static_cast<const char*>(fd.name)), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(name), joan::orig::arg::mptr(ext));  // _splitpath
                std::int16_t found = 0;
                for (std::int32_t i = 0; i < jo::self<jo::TexDir>(this)->tag_count; ++i) {
                    if (joan::crt::stricmp(joan::orig::arg::str(static_cast<const char*>(ext)), joan::orig::arg::str(reinterpret_cast<const char*>(jo::view<std::uint8_t*>(jo::self<jo::TexDir>(this)->tags) + i * 8))) == 0) {  // _stricmp
                        found = jo::view<std::int16_t>(jo::self<jo::TexDir>(this)->tags[i].bit);
                        break;
                    }
                }
                if (found == 0) continue;
                if (joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(static_cast<const char*>(name)))) > 0x20u) continue;  // strlen
                mask = static_cast<std::int16_t>(mask | found);
            } while (joan::orig::arg::as<std::int32_t>(joan::orig::libc::_findnext(joan::orig::arg::i32(handle), &fd)) == 0);  // _findnext
        }
        if (mask != 0) {
            std::int32_t i = 0;
            for (; i < jo::self<jo::TexDir>(this)->tag_count; ++i) {
                if ((static_cast<std::int32_t>(mask) &
                     static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::TexDir>(this)->tags[i].bit))) != 0)
                    break;
            }
            joan::orig::libc::_makepath(joan::orig::arg::mptr(out), joan::orig::arg::mptr(static_cast<const char*>(nullptr)), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(fname), joan::orig::arg::mptr(reinterpret_cast<const char*>(jo::view<std::uint8_t*>(jo::self<jo::TexDir>(this)->tags) + i * 8)));  // _makepath
            return out;
        }
        if (recurse == 0) return nullptr;
        joan::orig::libc::_makepath(joan::orig::arg::mptr(spec), joan::orig::arg::mptr(static_cast<const char*>(nullptr)), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(&img::global<const char>(d::s_star)), joan::orig::arg::mptr(static_cast<const char*>(nullptr)));  // "*"
        handle = joan::orig::arg::as<std::int32_t>(joan::orig::libc::_findfirst(joan::orig::arg::str(static_cast<const char*>(spec)), &fd));
        if (handle == -1) return nullptr;
        do {
            if ((fd.attrib & 0x10) != 0) continue;  // original skips directories here too (it
            // "recurses" into plain files, whose _findfirst then fails) - kept as is.
            if (recurse == 0) continue;
            if (std::strcmp(joan::orig::arg::str(static_cast<const char*>(fd.name)), joan::orig::arg::str(&img::global<const char>(d::s_dot_9d36e8))) == 0)
                continue;  // "."
            if (std::strcmp(joan::orig::arg::str(static_cast<const char*>(fd.name)), joan::orig::arg::str(&img::global<const char>(d::s_dot_dot))) == 0)
                continue;  // ".."
            char sub[0x114];  // original buffer size (ebp-0x540)
            joan::orig::libc::_makepath(joan::orig::arg::mptr(static_cast<char*>(sub)), joan::orig::arg::mptr(static_cast<const char*>(nullptr)), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(static_cast<const char*>(fd.name)), joan::orig::arg::mptr(static_cast<const char*>(nullptr)));
            char* const r = joan::orig::arg::as<char*>(joan::orig::as_tex_dir(this)->find(joan::orig::arg::str(fname), joan::orig::arg::str(static_cast<const char*>(sub)), joan::orig::arg::str(out), joan::orig::arg::i32(recurse)));  // 0x00605540
            if (r != nullptr) return r;
        } while (joan::orig::arg::as<std::int32_t>(joan::orig::libc::_findnext(joan::orig::arg::i32(handle), &fd)) == 0);
        return nullptr;
    }
};

#define T33(name, addr, fn) JOAN_REPLACE("en3d/helpers33", name, addr, fn)
T33("TexturePaths::find", 0x00605540, &H::tex_find);
#undef T33

}  // namespace game_leaves81_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::TexDir::init() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_06::H::init_604110>(this)); }  // 0x604110
auto joan::orig::TexDir::clear() -> void { joan::bind::member<&::game_b92_05::H::texdir_clear>(this); }  // 0x604150
auto joan::orig::TexDir::add_604310(const char* a0) -> void { joan::bind::member<&::game_b95_06::H::texdir_add>(this, a0); }  // 0x604310
auto joan::orig::TexDir::add_604440(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::texdir_add>(this, a0)); }  // 0x604440
auto joan::orig::TexDir::add_path(const char* a0, std::uint32_t a1, std::uintptr_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_06::H::add_path>(this, a0, a1, a2, a3)); }  // 0x604560
auto joan::orig::TexDir::add_entry(const char* a0, std::uint32_t a1, void* a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::texdir_add_entry>(this, a0, a1, a2, a3)); }  // 0x604650
auto joan::orig::TexDir::reset_flags() -> void { joan::bind::member<&::game_b108_17::H::texdir_reset>(this); }  // 0x604850
auto joan::orig::TexDir::scan_604960(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b109_11::H::texdir_scan>(this, a0, a1); }  // 0x604960
auto joan::orig::TexDir::dir_index(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves81::H::tex_dir_index>(this, a0)); }  // 0x6054c0
auto joan::orig::TexDir::find(const char* a0, const char* a1, const char* a2, std::int32_t a3) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_leaves81_2::H::tex_find>(this, a0, a1, a2, a3)); }  // 0x605540
auto joan::orig::TextureManager::build_paths() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::texpaths_build>(this)); }  // 0x608e10
