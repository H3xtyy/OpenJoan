// game/gamefila.cpp: functions of the original d:\ww\gamefila.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
//
//    - GameFiles::add 0x00477440 (name) ret 4 (gamefila.cpp).
namespace {
namespace game_b84_05 {

using joan::orig::helpers::strcpy_;

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_files_add, "GameFiles::add");

struct H {
    // ---- 0x00477440 GameFiles::add(name) (gamefila.cpp) ---------------------------------------
    std::int32_t files_add(const char* name) {
        c_files_add.hit();
        void* p = joan::orig::as_mem(kMem)->add(static_cast<std::uint32_t>(sizeof(jo::GameFile)), str(d::s_d_ww_gamefila_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_9ff28c)) + 1));  // 0x0058e7b0
        joan::orig::as_game_file(p)->clear();  // 0x00477cf0
        std::int8_t sign;
        if (name != nullptr) {
            strcpy_(reinterpret_cast<char*>(p) + 3, name);  // x64: needs GameFile+0x3 char[0x105]
            sign = joan::orig::arg::as<std::int32_t>(joan::orig::as_game_file(p)->load_check(joan::orig::arg::mptr(name))) != 0 ? 1 : -1;  // 0x00477d80
        } else {
            sign = joan::orig::arg::as<std::int32_t>(joan::orig::as_game_file(p)->ok()) != 0 ? 1 : -1;  // 0x00477d60
        }
        joan::orig::dynarray(this)->linkin(static_cast<void*>(&p));              // 0x00584620
        const std::int16_t n = joan::orig::arg::as<std::int16_t>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        jo::as_game_file(p)->index = n;
        return static_cast<std::int32_t>(jo::as_game_file(p)->index) * static_cast<std::int32_t>(sign);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("GameFiles::add", 0x00477440, &H::files_add);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x00477520 GameFileList::remove(id)          (thiscall, ret 4)
namespace {
namespace game_b101_02 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

i32 sw(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }  // word argument

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_filelist_remove, "GameFileList::remove");

void heap_free(void* p, const char* file, i32 line) {
    joan::orig::as_mem(kHeap)->del_58ed70(p, file, line);  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------ 0x00477520
    u32 filelist_remove(i32 id) {
        c_filelist_remove.hit();
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_units(this)->get_430410(joan::orig::arg::i16(sw(id))));  // 0x00430410
        joan::orig::as_game_file(p)->close();                                    // 0x00477e60
        joan::orig::as_game_file(p)->destroy(0u);                                 // 0x004775a0
        heap_free(p, str(d::s_d_ww_gamefila_cpp), static_cast<i32>(global<i16>(d::g_9ff290)) + 6);
        joan::orig::as_dynarray(this)->linkout_584a10(sw(id));  // 0x00584a10
        return 0;  // void callee; callers discard the result
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("GameFileList::remove", 0x00477520, &H::filelist_remove);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
//
//      0x00477030 GameFileList::clear              (thiscall, ret 0) - 0x00477520 for n..1, 0x00584580(1).
namespace {
namespace game_b102_02 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_gfl_clear, "GameFileList::clear");

// ---------------------------------------------------------------- shared game helpers
i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {
    // 0x00477030
    u32 gfl_clear() {
        c_gfl_clear.hit();
        for (i16 n = static_cast<i16>(list_count(this)); static_cast<i32>(n) >= 1; n = static_cast<i16>(n - 1))
            joan::orig::as_save_dir(this)->remove(static_cast<i32>(n));  // 0x00477520
        joan::orig::as_dynarray(this)->zap_584580(1);              // 0x00584580
        return 0;  // void callee; callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("GameFileList::clear", 0x00477030, &H::gfl_clear);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
//
//    - SaveDir::browse_4770a0 0x004770a0 (ret 8, path, bool descend): descend: builds the new
//      directory from +0x1d (".\\" = none / "..\\" = strip last component / append) and recurses
//      with descend = 0. Else lists "*.SAV" (finder 0x00582ad0 / 0x00582b00 / 0x00417ad0 /
//      0x00477cc0, file 0x004179c0 / 0x00586da0 / 0x00587260 / 0x005872b0) from the last entry
//      down, reading each save (0x00477440, 0x00477c80 abs, 0x00477520 + 0x004775f0 when <= 0),
//      then +0x1d = path, 0x00477b10(0x004024c3), finder dtor 0x00477410 (its eax returned).
//
//  - Stack objects keep the original frame sizes (strings 0x194, file 0x110, finder 0x20, path
//    buffers 0x108 / 0x11c). 0x004770a0's buf[len - 1] store has one spare byte before buf
//    (the original writes into the next local for an empty string).
namespace {
namespace game_b108_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::str;
JOAN_COUNTER(c_browse, "SaveDir::browse_4770a0");

struct H {
    // ------------------------------------------------------------------ 0x004770a0
    u32 browse_4770a0(char* path, u32 descend) {
        c_browse.hit();
        if ((descend & 0xff) != 0) {
            alignas(4) char area[4 + 0x108];  // [ebp-0x11c] spare byte, buf at [ebp-0x118]
            char* const buf = area + 4;
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(jo::self<jo::SaveDir>(this)->path));  // strcpy
            if (std::strcmp(joan::orig::arg::str(path), str(d::s_dot_bslash)) != 0) {         // strcmp ".\\"
                if (std::strcmp(joan::orig::arg::str(path), str(d::s_dot_dot_bslash)) == 0) {     // "..\\"
                    const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(buf)));  // strlen
                    buf[static_cast<i32>(len) - 1] = 0;
                    char* const p = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(buf), i32{0x5c}));    // strrchr
                    *reinterpret_cast<volatile u8*>(reinterpret_cast<std::uintptr_t>(p) + 1) = 0;
                } else {
                    std::strcat(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(path));  // strcat
                }
            }
            return joan::orig::arg::as<u32>(joan::orig::as_save_dir(this)->browse(joan::orig::arg::str(buf), joan::orig::arg::u32(i32{0})));  // 0x004770a0
        }
        joan::orig::as_save_dir(this)->clear();  // 0x00477030
        alignas(4) char pattern[0x11c];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(pattern)), joan::orig::arg::str(path));
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(pattern)), str(d::s_SAV_9d3584));  // '*.SAV'
        alignas(alignof(void*)) u8 finder[((sizeof(jo::Directory) > 0x20 ? sizeof(jo::Directory) : 0x20) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger Directory
        joan::orig::as_directory(finder)->construct_582ad0();                             // 0x00582ad0
        joan::orig::as_directory(finder)->read_582b00(joan::orig::arg::str(pattern), i32{0});            // 0x00582b00
        for (i32 i = joan::orig::as_dynarray(finder)->size(); i >= 1; --i) {  // 0x00417ad0
            char* const name = joan::orig::arg::as<char*>(joan::orig::as_list(finder)->item_477cc0(joan::orig::arg::i32(i)));  // 0x00477cc0
            alignas(4) char full[0x108];
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(full)), joan::orig::arg::str(path));
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(full)), joan::orig::arg::str(name));
            alignas(alignof(void*)) u8 file[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger File
            joan::orig::as_file(file)->construct();  // 0x004179c0
            if (joan::orig::as_file(file)->file_open(joan::orig::arg::str(full), i32{0}, static_cast<char>(i32{1})) == 0) {  // 0x00586da0
                joan::orig::as_file(file)->destruct();                             // 0x005872b0
                continue;
            }
            const i16 v = joan::orig::arg::as<i16>(joan::orig::as_save_dir(this)->add(joan::orig::arg::str(static_cast<void*>(file))));  // 0x00477440
            const i16 a = joan::orig::arg::as<i16>(joan::orig::iabs(i32{v}));                     // 0x00477c80
            if (!(v > 0)) {
                joan::orig::as_save_dir(this)->remove(i32{a});            // 0x00477520
                joan::orig::as_popup(this)->set_text_4775f0(joan::orig::arg::mptr(img::addr(d::kEmpty)));   // 0x004775f0
            }
            joan::orig::as_file(file)->file_close();   // 0x00587260
            joan::orig::as_file(file)->destruct();  // 0x005872b0
        }
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::SaveDir>(this)->path)), joan::orig::arg::str(path));
        joan::orig::as_list(this)->renumber(fn::code_value(fn::cmp_477bd0));  // 0x00477b10(0x00477bd0 thunk)
        return joan::orig::arg::as<u32>(joan::orig::as_directory(finder)->destruct_477410());           // 0x00477410
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("SaveDir::browse_4770a0", 0x004770a0, &H::browse_4770a0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
//
//      0x004775a0 X::scalar_deleting_dtor    (thiscall(flags), ret 4) - 0x00477d30, delete.
namespace {
namespace game_b100_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004775a0
    void* scalar_dtor(u32 flags) {
        joan::orig::as_game_file(this)->destruct();  // 0x00477d30
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("X::scalar_deleting_dtor", 0x004775a0, &H::scalar_dtor);
#undef B100

}  // namespace game_b100_03
}  // namespace

// ---------------------------------------- from modules/game_b85_01.cpp (game/b85_01)
//
//    - GameFile::clear 0x00477cf0 (memset 0x1a7, returns this), GameFile::ok 0x00477d60.
namespace {
namespace game_b85_01 {

using joan::orig::helpers::memset_;

struct H {
    void* clear() {                                                                              // 0x00477cf0
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::GameFile)));  // 0x1a7 on x86
        return this;
    }
    std::int32_t ok() { return 1; }  // 0x00477d60
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_01", name, addr, fn)
B("GameFile::clear", 0x00477cf0, &H::clear);
B("GameFile::ok", 0x00477d60, &H::ok);
#undef B

}  // namespace game_b85_01
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;

struct H {
    // ------------------------------------------------------------------ 0x00476ee0
    void* ctor_476ee0() {
        joan::orig::dynarray(this)->construct(static_cast<i32>(sizeof(void*)), 0xa, 0);  // 0x005843d0 (GameFile* records: 4 on x86)
        memset_(jo::self<jo::SaveDir>(this)->path, 0, 0x205);
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::ctor_476ee0", 0x00476ee0, &H::ctor_476ee0);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
namespace {
namespace game_b103_02 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00476f30
    u32 gfl_dtor() {
        joan::orig::as_save_dir(this)->clear();               // 0x00477030
        joan::orig::as_dynarray(this)->destruct_5844b0();   // 0x005844b0
        return 0;  // void callee; the atexit stub's result is ignored
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("GameFileList::~GameFileList", 0x00476f30, &H::gfl_dtor);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;
using joan::img::obj;

using joan::img::str;

struct H {
    // 0x00476fa0
    u32 save_dir_init() {
        char* const path = &jo::self<jo::SaveDir>(this)->path[0];
#if defined(__GNUC__) && !defined(__clang__)
// strncpy with the buffer size as bound and no terminator, as 0x00476fa0 does (path[0x104])
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_SAVE), 0x104u);  // strncpy(path, "SAVE\\", 0x104)
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        jo::self<jo::SaveDir>(this)->u8_121 = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->is_dir_exist_590c00(joan::orig::arg::str(path))) == 0)  // 0x00590c00
            joan::orig::libc::_mkdir(joan::orig::arg::str(path));                       // _mkdir
        return joan::orig::arg::as<u32>(joan::orig::as_save_dir(this)->browse(joan::orig::arg::str(path), 0u));             // 0x004770a0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("SaveDir::init", 0x00476fa0, &H::save_dir_init);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b84_05_2.cpp (game/b84_05)
namespace {
namespace game_b84_05_2 {

using joan::call::thiscall;

using joan::call::cdecl_call;
using joan::img::obj;

struct alignas(4) Six {  // 24-byte by-value argument
    std::uint32_t v[6];
};

void* const kRes = obj(d::g_item_res);

JOAN_COUNTER(c_play_effect, "Unit::play_effect");

struct H {
    // ---- 0x00477360 Unit::play_effect(b) ------------------------------------------------------
    std::int32_t play_effect(char b) {
        c_play_effect.hit();
        std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kRes)->get(joan::orig::arg::i16(jo::self<jo::Order0c_b>(this)->type_id)));  // 0x00425ce0
        alignas(4) std::uint8_t m[0x40];
        joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Order0c_b>(this)->yaw_)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Order0c_b>(this)->pitch)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::Order0c_b>(this)->roll_)));                              // 0x0060bf40
        joan::orig::set_30(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::mptr(jo::self<jo::Order0c_b>(this)->pos));  // 0x0060c360 (+0x56)
        Six s;
        std::memcpy(&s, jo::as_item_def(def)->bounding_box, sizeof s);  // +0x1c7
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_players(obj(d::g_physic_sys))->assign_matches(s, static_cast<void*>(m), joan::orig::arg::i32(-1), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)))) == 0) return 0;  // 0x006afb10
        jo::view<std::int32_t>(jo::self<jo::Order0c_b>(this)->u32_c2) = static_cast<std::int32_t>(static_cast<std::int8_t>(b));
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Unit::play_effect", 0x00477360, &H::play_effect);
#undef B

}  // namespace game_b84_05_2
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using joan::call::thiscall;

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00477410
    std::uint32_t dtor_477410() { return thiscall<std::uint32_t>(fn::dtor_DynArray, self()); }  // 0x005844b0
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::dtor_477410", 0x00477410, &H::dtor_477410);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16 {

using joan::call::thiscall;
using joan::call::cdecl_call;

JOAN_COUNTER(c_find, "Paths::find");

using joan::orig::helpers::stricmp_;

struct H {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"  // an output buffer behind a facade's const char* parameter
#endif
    // ------------------------------------------------------------------ 0x00477640
    void* find_path(const char* name) {
        c_find.hit();
        for (std::int16_t k = static_cast<std::int16_t>(joan::orig::as_dynarray(this)->size()); k >= 1; --k) {  // 0x00417ad0
            char* const e = joan::orig::arg::as<char*>(joan::orig::as_units(this)->get_430410(joan::orig::arg::i16(k)));  // 0x00430410
            if (stricmp_(e + 3, name) == 0) return e;
        }
        if (joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(name), 0x5c)) != nullptr) return nullptr;  // strchr
        for (std::int16_t k = static_cast<std::int16_t>(joan::orig::as_dynarray(this)->size()); k >= 1; --k) {
            char* const e = joan::orig::arg::as<char*>(joan::orig::as_units(this)->get_430410(joan::orig::arg::i16(k)));
            char* const base = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(e + 3), 0x5c));  // strrchr
            if (base != nullptr && stricmp_(base + 1, name) == 0) return e;
        }
        return nullptr;
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Paths::find", 0x00477640, &H::find_path);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_save_name, "Saves::file_name");

struct H {
    // 0x00477770
    char* save_name(char* out, std::uint32_t autosave) {
        c_save_name.hit();
        if ((autosave & 0xffu) != 0) {
            char fmt[16];
            std::memcpy(fmt, str(d::s_AUTO_03d_SAV), 14);  // "_AUTO%03d.SAV"
            char name[0x104];
            std::uint8_t* best = nullptr;
            std::int32_t i;
            for (i = 0; i < 15; ++i) {
                joan::orig::libc::sprintf(joan::orig::arg::str(static_cast<char*>(name)), joan::orig::arg::str(static_cast<const char*>(fmt)), i);  // sprintf
                std::uint8_t* const cur = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_paths(this)->find(joan::orig::arg::str(static_cast<char*>(name))));     // 0x00477640
                if (cur == nullptr) break;
                if (best != nullptr) {
                    using Cmp = std::int32_t(JOAN_STDCALL*)(const void*, const void*);
                    const std::int32_t r = reinterpret_cast<Cmp>(global<std::uintptr_t>(d::iat_KERNEL32_CompareFileTime))(
                        static_cast<const void*>(cur + 0x108), static_cast<const void*>(best + 0x108));  // CompareFileTime
                    if (r >= 0) continue;
                }
                best = cur;
            }
            if (i >= 15) {
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(out)), joan::orig::arg::str(reinterpret_cast<const char*>(best + 3)));  // strcpy
            } else {
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(out)), joan::orig::arg::str(jo::self<jo::SaveDir>(this)->path));  // strcpy (+0x1d)
                std::strcat(static_cast<char*>(joan::orig::arg::mptr(out)), joan::orig::arg::str(static_cast<const char*>(name)));  // strcat
            }
        } else {
            char fmt[0x104];
            char name[0x104];
            joan::orig::libc::sprintf(joan::orig::arg::str(static_cast<char*>(fmt)), str(d::s_SAVE_s_04d_s), static_cast<const char*>(out), str(d::s_SAV_9d35c0));  // sprintf "SAVE\\%s%%04d.%s", out, "SAV"
            for (std::int32_t j = 0; j < 0x2710; ++j) {
                joan::orig::libc::sprintf(joan::orig::arg::str(static_cast<char*>(name)), joan::orig::arg::str(static_cast<const char*>(fmt)), j);  // sprintf
                if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(static_cast<char*>(name))) == 0) break;  // 0x00590b80
                if (j == 0x270f) break;
            }
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(out)), joan::orig::arg::str(static_cast<const char*>(name)));  // strcpy
        }
        return out;
    }

    // 0x00477cc0
    void* item(std::int32_t i) { return joan::orig::as_dynarray(this)->get(joan::orig::arg::i32(i)); }  // 0x00417200
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Saves::file_name", 0x00477770, &H::save_name);
B("List::item_477cc0", 0x00477cc0, &H::item);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b84_05_3.cpp (game/b84_05)
namespace {
namespace game_b84_05_3 {

using joan::call::thiscall;

std::int32_t list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

JOAN_COUNTER(c_renumber, "List::renumber");

struct H {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    // ---- 0x00477b10 List::renumber(n) ---------------------------------------------------------
    void renumber(std::uintptr_t n) {  // the comparator's code address (pointer-sized through the facade)
        c_renumber.hit();
        joan::orig::dynarray(this)->quick_sort(joan::orig::arg::cmp(n));  // 0x005850b0
        std::int16_t i = static_cast<std::int16_t>(list_count(this));
        for (; static_cast<std::int32_t>(i) >= 1; i = static_cast<std::int16_t>(i - 1)) {
            std::int16_t* const item = joan::orig::arg::as<std::int16_t*>(joan::orig::as_units(this)->get_430410(joan::orig::arg::i16(i)));  // 0x00430410
            *item = i;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("List::renumber", 0x00477b10, &H::renumber);
#undef B

}  // namespace game_b84_05_3
}  // namespace

// ---------------------------------------- from modules/game_b103_02_2.cpp (game/b103_02)
namespace {
namespace game_b103_02_2 {

using joan::call::cdecl_call;

using i32 = std::int32_t;

// ---------------------------------------------------------------------- cdecl comparators
i32 JOAN_CDECL cmp_477b80(const void* a, const void* b) {
    const char* const pa = *static_cast<char* const*>(a);
    const char* const pb = *static_cast<char* const*>(b);
    return joan::crt::stricmp(joan::orig::arg::str(pa + 3), joan::orig::arg::str(pb + 3));  // _stricmp
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("cmp_477b80", 0x00477b80, &cmp_477b80);
#undef B

}  // namespace game_b103_02_2
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
namespace {
namespace game_b102_02_2 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using u8 = std::uint8_t;

// 0x00477bd0 qsort comparator of GameFile*: descending by file_time (high, then low dword, unsigned); equal -> -1.
i32 JOAN_CDECL cmp_477bd0(const void* a, const void* b) {
    const u8* const pa = *static_cast<u8* const*>(a);
    const u8* const pb = *static_cast<u8* const*>(b);
    if (jo::as_game_file(pa)->file_time[1] > jo::as_game_file(pb)->file_time[1]) return -1;
    if (jo::as_game_file(pa)->file_time[1] < jo::as_game_file(pb)->file_time[1]) return 1;
    if (jo::as_game_file(pa)->file_time[0] > jo::as_game_file(pb)->file_time[0]) return -1;
    if (jo::as_game_file(pa)->file_time[0] < jo::as_game_file(pb)->file_time[0]) return 1;
    return -1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("cmp_477bd0", 0x00477bd0, &cmp_477bd0);
#undef B

}  // namespace game_b102_02_2
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
namespace {
namespace game_b96_02 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

i32 JOAN_CDECL iabs(i32 v) {  // 0x00477c80
    return v > 0 ? v : static_cast<i32>(0u - static_cast<u32>(v));
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("iabs", 0x00477c80, &iabs);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00477d30
    u32 call_477e60() { joan::orig::as_game_file(this)->close(); return 0; }  // 0x00477e60 (void; result unused)
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::call_477e60", 0x00477d30, &H::call_477e60);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SaveDir::init() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_09::H::save_dir_init>(this)); }  // 0x476fa0
auto joan::orig::SaveDir::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b102_02::H::gfl_clear>(this)); }  // 0x477030
auto joan::orig::SaveDir::browse(const char* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_04::H::browse_4770a0>(this, a0, a1)); }  // 0x4770a0
auto joan::orig::Unit::play_effect(char a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05_2::H::play_effect>(this, a0)); }  // 0x477360
auto joan::orig::Directory::destruct_477410() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_03::H::dtor_477410>(this)); }  // 0x477410
auto joan::orig::SaveDir::add(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::files_add>(this, a0)); }  // 0x477440
auto joan::orig::SaveDir::remove(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::filelist_remove>(this, a0)); }  // 0x477520
auto joan::orig::GameFile::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_03::H::scalar_dtor>(this, a0)); }  // 0x4775a0
auto joan::orig::Paths::find(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_16::H::find_path>(this, a0)); }  // 0x477640
auto joan::orig::SaveDir::file_name(const char* a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b92_02::H::save_name>(this, a0, a1)); }  // 0x477770
auto joan::orig::List::renumber(std::uintptr_t a0) -> void { joan::bind::member<&::game_b84_05_3::H::renumber>(this, a0); }  // 0x477b10
auto joan::orig::iabs(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_02::iabs>(a0)); }  // 0x477c80
auto joan::orig::List::item_477cc0(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_02::H::item>(this, a0)); }  // 0x477cc0
auto joan::orig::GameFile::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_01::H::clear>(this)); }  // 0x477cf0
auto joan::orig::GameFile::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::call_477e60>(this)); }  // 0x477d30
auto joan::orig::GameFile::ok() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_01::H::ok>(this)); }  // 0x477d60
