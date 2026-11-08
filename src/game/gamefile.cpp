// game/gamefile.cpp: functions of the original d:\ww\gamefile.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/s5.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

// Error popup used after every failed write: if (!*(char*)0xa54b0a && *text) 0x004775f0.
void popup(const char* text) {
    if (global<std::int8_t>(d::g_gamefile_error_quiet) != 0) return;
    if (*reinterpret_cast<const std::int8_t*>(text) == 0) return;
    joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(text));  // 0x004775f0
}

// The .SAV header after the "RSav" chunk (write_header 0x478cc0 / read_header 0x478520): one chunk per GameFile field,
// field-wise (re/a6/SAVES_PLAN.md S5, re/save/records.json s5 GameFile*). off = the x86 offset (the raw path's range).
namespace sv = joan::save;
struct HeaderChunk {
    std::int32_t off;
    std::int32_t size;  // as written (+0x2: written 4, read 1)
    const sv::Record* rec;
    void (*put)(sv::Out&, const jo::GameFile&);
    void (*get)(sv::In&, jo::GameFile&);
};
const HeaderChunk kHeaderChunks[10] = {
    {0x002, 4, &sv::kGameFileFlags, &sv::put_GameFileFlags<jo::GameFile>, nullptr},
    {0x108, 8, &sv::kGameFileTime, &sv::put_GameFileTime<jo::GameFile>, &sv::get_GameFileTime<jo::GameFile>},
    {0x110, 8, &sv::kGameFileLocalTime, &sv::put_GameFileLocalTime<jo::GameFile>, &sv::get_GameFileLocalTime<jo::GameFile>},
    {0x118, 1, &sv::kGameFileMode, &sv::put_GameFileMode<jo::GameFile>, &sv::get_GameFileMode<jo::GameFile>},
    {0x119, 1, &sv::kGameFile119, &sv::put_GameFile119<jo::GameFile>, &sv::get_GameFile119<jo::GameFile>},
    {0x19f, 4, &sv::kGameFileGameTime, &sv::put_GameFileGameTime<jo::GameFile>, &sv::get_GameFileGameTime<jo::GameFile>},
    {0x1a3, 4, &sv::kGameFile1a3, &sv::put_GameFile1a3<jo::GameFile>, &sv::get_GameFile1a3<jo::GameFile>},
    {0x11a, 0x80, &sv::kGameFileName, &sv::put_GameFileName<jo::GameFile>, &sv::get_GameFileName<jo::GameFile>},
    {0x19a, 4, &sv::kGameFile19a, &sv::put_GameFile19a<jo::GameFile>, &sv::get_GameFile19a<jo::GameFile>},
    {0x19e, 1, &sv::kGameFile19e, &sv::put_GameFile19e<jo::GameFile>, &sv::get_GameFile19e<jo::GameFile>},
};

}  // namespace

// ---------------------------------------- from modules/game_b85_01.cpp (game/b85_01)
//
//    - GameFile::write_header 0x00478cc0 (file) ret 4 (gamefile.cpp): "RSav" magic, then the
//      header fields +0x2 (4), +0x108 (8), +0x110 (8), +0x118 (1), +0x119 (1), +0x19f (4),
//      +0x1a3 (4), +0x11a (0x80), +0x19a (4), +0x19e (1).
//
//    - GameFile::load_check 0x00477d80 (arg) ret 4: 0x00478520, "Error loading file." popup,
//      flag bit 0x80 of +0x2.
//
//    - GameFile::stamp 0x00478480 (no args): GetSystemTimeAsFileTime / FileTimeToLocalFileTime
//      (through the IAT slots 0xa6da14 / 0xa6da18), level bytes 0xa500c2/3, 0xa47bb0/4.
//
//  * 0x00474ec0 never touches `this` (only stored in the frame); 0x00478cc0 writes this's fields.
namespace {
namespace game_b85_01 {

using joan::call::stdcall_import;

// 0x005872e0 File::write(buf, size) ret 8.
std::int32_t file_write(void* file, const void* buf, std::int32_t size) {
    return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(buf), joan::orig::arg::u32(size));
}

JOAN_COUNTER(c_header, "GameFile::write_header");
JOAN_COUNTER(c_load_check, "GameFile::load_check");
JOAN_COUNTER(c_stamp, "GameFile::stamp");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // ---- 0x00478cc0 GameFile::write_header(file) (gamefile.cpp) ----------------------------
    std::int32_t write_header(void* file) {
        c_header.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S5): each chunk field by field; 0 = the raw writes.
        jo::File& f = *jo::as_file(file);
        if (sv::s5::write_u32(f, sv::s5::kMagic, "RSav") == 0) {  // File::file_write 0x005872e0
            popup(str(d::kEmpty));
            return 0;
        }
        const bool fieldwise = sv::fieldwise();
        for (const HeaderChunk& c : kHeaderChunks) {
            std::int32_t ok;
            if (fieldwise) {
                sv::Out o(f, *c.rec, self() + c.off);
                c.put(o, *jo::self<jo::GameFile>(this));
                ok = o.end();
            } else {
                ok = file_write(file, self() + c.off, c.size);
            }
            if (ok == 0) {
                popup(str(d::kEmpty));
                return 0;
            }
        }
        return 1;
    }

    // ---- 0x00477d80 GameFile::load_check(arg) ret 4 ----------------------------------------
    std::int32_t load_check(void* arg) {
        c_load_check.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_game_file(this)->read_header(joan::orig::arg::mptr(arg))) == 0) {  // 0x00478520
            popup(str(d::s_Error_loading_file));                                   // "Error loading file."
            return 0;
        }
        if ((reinterpret_cast<const std::uint8_t&>(jo::self<jo::GameFile>(this)->flags) & 0x80) != 0) return 0;
        return 1;
    }

    // ---- 0x00478480 GameFile::stamp ---------------------------------------------------------
    void stamp() {
        c_stamp.hit();
        stdcall_import<void>(d::iat_KERNEL32_GetSystemTimeAsFileTime, static_cast<void*>(jo::self<jo::GameFile>(this)->file_time));  // GetSystemTimeAsFileTime
        stdcall_import<std::int32_t>(d::iat_KERNEL32_FileTimeToLocalFileTime, static_cast<const void*>(jo::self<jo::GameFile>(this)->file_time),
                                     static_cast<void*>(jo::self<jo::GameFile>(this)->local_time));  // FileTimeToLocalFileTime
        jo::self<jo::GameFile>(this)->mode = global<std::uint8_t>(d::g_game_mode);
        jo::self<jo::GameFile>(this)->u8_119 = global<std::uint8_t>(d::g_a500c3);
        jo::self<jo::GameFile>(this)->game_time = global<std::uint32_t>(d::g_get_a47bb0);
        jo::self<jo::GameFile>(this)->u32_1a3 = global<std::uint32_t>(d::g_calendar_minute_of_day);
        reinterpret_cast<std::uint8_t&>(jo::self<jo::GameFile>(this)->flags) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_01", name, addr, fn)
B("GameFile::write_header", 0x00478cc0, &H::write_header);
B("GameFile::load_check", 0x00477d80, &H::load_check);
B("GameFile::stamp", 0x00478480, &H::stamp);
#undef B

}  // namespace game_b85_01
}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
//
//      0x00477e60 Obj::nop_477e60         (thiscall, ret 0) - empty.
namespace {
namespace game_b94_02 {

struct H {
    // ------------------------------------------------------------------------ 0x00477e60 / 0x004953b0 / 0x004b8b20
    void nop_477e60() {}
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Obj::nop_477e60", 0x00477e60, &H::nop_477e60);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
//
//    - GameFile::load_477e80 0x00477e80 (ret 0, gamefile.cpp): stack file object (0x004179c0,
//      0x110 bytes), open this+3 (0x00586da0), 0x00478520, 0x004735c0, close / dtor; error
//      messages through 0x004775f0 on 0xa549e8 (dead __FILE__/__LINE__ blocks have no calls).
namespace {
namespace game_b108_12 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

u8* add(const void* p, std::int32_t off) { return static_cast<u8*>(const_cast<void*>(p)) + off; }

JOAN_COUNTER(c_477e80, "GameFile::load_477e80");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x00477e80
    static void load_error(img::Addr msg) {
        if (static_cast<i8>(global<u8>(d::g_gamefile_error_quiet)) == 0 && static_cast<i8>(*str(msg)) != 0) {
            joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(str(msg)));  // 0x004775f0
        }
    }
    u32 load_477e80() {
        c_477e80.hit();
        alignas(4) jo::File file_obj;  // the 0x110-byte frame slot of the original; a real File (x64: 0x112 bytes)
        void* const file = &file_obj;
        joan::orig::as_file(file)->construct();  // 0x004179c0
        if (joan::orig::as_file(file)->file_open(joan::orig::arg::str(add(this, 3)), i32{0}, static_cast<char>(i32{1})) == 0) {  // 0x00586da0  // x64: needs GameFile+0x3 char[0x105]
            load_error(d::s_Error_opening_file);  // "Error opening file."
            joan::orig::as_file(file)->destruct();  // 0x005872b0
            return 0;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_game_file(this)->read_header(joan::orig::arg::mptr(static_cast<void*>(file)))) == 0) {  // 0x00478520
            load_error(d::s_Error_loading_file);  // "Error loading file."
            joan::orig::as_file(file)->destruct();
            return 0;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_game_file(this)->save_subsystems(joan::orig::arg::mptr(static_cast<void*>(file)))) == 0) {  // 0x004735c0
            load_error(d::s_Error_loading_file);
            joan::orig::as_file(file)->destruct();
            return 0;
        }
        joan::orig::as_file(file)->file_close();   // 0x00587260
        joan::orig::as_file(file)->destruct();  // 0x005872b0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("GameFile::load_477e80", 0x00477e80, &H::load_477e80);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
//
//    - GameFile::save 0x00478180 (no args, gamefile.cpp): open (0x00586f00), write header
//      (0x00478cc0) and body (0x00474ec0), close; error popups through 0xa549e8.
//
//  * SEH frames (0x00474df0, 0x0041a650, 0x00417b60, 0x00478180, 0x004731e0, 0x00439ff0) only
//    guard operator new + ctor and the stack objects' destructors: normal path reproduced.
//
//  * 0x00478180: the "File name / Line number" sprintf branches are dead (xor reg / test / je).
namespace {
namespace game_b84_05 {

using File = jo::File;  // 0x004179c0 (a real File: x64 0x112 bytes)

JOAN_COUNTER(c_save, "GameFile::save");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // ---- 0x00478180 GameFile::save (gamefile.cpp) ------------------------------------------------
    void save_error(const char* text) {
        if (global<std::int8_t>(d::g_gamefile_error_quiet) != 0) return;
        if (*reinterpret_cast<const std::int8_t*>(text) == 0) return;
        joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(text));  // 0x004775f0
    }
    std::int32_t save() {
        c_save.hit();
        joan::orig::as_game_file(this)->stamp();  // 0x00478480
        alignas(4) File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        std::int32_t result;
        if (joan::orig::as_file(&file)->file_create(joan::orig::arg::str(self() + 3), 0, static_cast<char>(1)) == 0) {  // 0x00586f00 open
            save_error(str(d::s_Error_creating_file));  // "Error creating file."
            result = 0;
        } else if (joan::orig::arg::as<std::int32_t>(joan::orig::as_game_file(this)->write_header_478cc0(joan::orig::arg::mptr(static_cast<void*>(&file)))) == 0) {  // 0x00478cc0
            save_error(str(d::s_Error_saving_file));  // "Error saving file."
            result = 0;
        } else if (joan::orig::arg::as<std::int32_t>(joan::orig::as_game_file(this)->write_body(joan::orig::arg::mptr(static_cast<void*>(&file)))) == 0) {  // 0x00474ec0
            save_error(str(d::s_Error_saving_file));
            result = 0;
        } else {
            joan::orig::as_file(&file)->file_close();  // 0x00587260 close
            result = 1;
        }
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 dtor
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("GameFile::save", 0x00478180, &H::save);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
//
//    - GameFile::read_header 0x00478520 (file) ret 4 (gamefile.cpp, used by GameFile::load_check
//      0x00477d80): "RSav" magic (else "Invalid file format." popup), then the fields +0x2 (1),
//      +0x108 (8), +0x110 (8), +0x118 (1), +0x119 (1), +0x19f (4), +0x1a3 (4), +0x11a (0x80),
//      +0x19a (4), +0x19e (1) through File::read 0x005873a0; any failed read -> "" popup, 0.
//
//  * 0x00478520: the "File name / Line number" sprintf + 0x00586bc0 branches after every failed
//    read are dead (xor reg / test reg / je) and dropped (their 0xc00 bytes of stack buffers too).
//    The magic local is uninitialised in the original; File::read returns the ReadFile result, so
//    a short read at EOF (file < 4 bytes) succeeds and compares partly stale stack bytes with
//    "RSav". With 0 here such a file is always rejected; the original rejects it too unless the
//    garbage happens to complete "RSav" (practically never).
namespace {
namespace game_b86_06 {

// 0x005873a0 File::read(buf, size) ret 8.
std::int32_t file_read(void* file, void* buf, std::int32_t size) {
    return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(size));
}

JOAN_COUNTER(c_read_header, "GameFile::read_header");

// 0x00478520 header reads after the magic, in order: {offset, size}.
struct ReadStep {
    std::int32_t offset;
    std::int32_t size;
};
const ReadStep kHeader[10] = {
    {0x2, 1}, {0x108, 8}, {0x110, 8}, {0x118, 1}, {0x119, 1},
    {0x19f, 4}, {0x1a3, 4}, {0x11a, 0x80}, {0x19a, 4}, {0x19e, 1},
};

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // ---- 0x00478520 GameFile::read_header(file) ret 4 (gamefile.cpp) -----------------------
    std::int32_t read_header(void* file) {
        c_read_header.hit();
        jo::File& f = *jo::as_file(file);
        std::uint32_t magic = 0;
        if (sv::s5::read_u32(f, magic, "RSav") == 0) {  // File::file_read 0x005873a0
            popup(str(d::kEmpty));
            return 0;
        }
        if (magic != sv::s5::kMagic) {  // "RSav"
            popup(str(d::s_Invalid_file_format));  // "Invalid file format."
            return 0;
        }
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S5): field-wise chunks; 0 = the raw reads.
        const bool fieldwise = sv::fieldwise();
        jo::GameFile& g = *jo::self<jo::GameFile>(this);
        for (int i = 0; i < 10; ++i) {
            std::int32_t ok;
            if (!fieldwise) {
                ok = file_read(file, self() + kHeader[i].offset, kHeader[i].size);
            } else if (kHeaderChunks[i].get == nullptr) {  // +0x2: written as 4 bytes, read as 1 (the low byte of flags)
                sv::In in(sv::port(f), 1, "GameFileFlags", &g.flags);
                g.flags = (g.flags & ~0xffu) | in.u8();
                ok = in.end();
            } else {
                sv::In in(f, *kHeaderChunks[i].rec, self() + kHeaderChunks[i].off);
                kHeaderChunks[i].get(in, g);
                ok = in.end();
            }
            if (ok == 0) {
                popup(str(d::kEmpty));
                return 0;
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("GameFile::read_header", 0x00478520, &H::read_header);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x00478b40 Unit::start_effect(char b) ret 4
namespace {
namespace game_b92_02 {

using joan::call::cdecl_call;
using joan::call::import_call;
struct Six {
    std::uint32_t v[6];
};
static_assert(sizeof(Six) == 24, "six");

JOAN_COUNTER(c_start_effect, "Unit::start_effect");

struct H {
    // 0x00478b40
    std::int32_t start_effect(char b) {
        c_start_effect.hit();
        std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::MapObject>(this)->type_id))));  // 0x00425ce0
        std::int32_t r = 0;
        if (jo::as_item_def(def)->has_bounding_box == 0) return r;
        alignas(4) std::uint8_t m[0x40];
        if (jo::self<jo::MapObject>(this)->actor != nullptr) {
            const void* const src = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->bone)->matrix());  // 0x0060ef80
            std::memcpy(m, src, sizeof m);
        } else {
            joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(static_cast<void*>(m)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->yaw_)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->pitch)), joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::MapObject>(this)->roll_)));                                            // 0x0060bf40
            import_call<void*>(d::iat_v3d_87_matrix_multiply_translation, static_cast<void*>(m), static_cast<void*>(jo::self<jo::MapObject>(this)->pos));  // multiply_translation
        }
        const std::int32_t kind = static_cast<std::int8_t>(jo::as_item_def(def)->item_usage);
        std::int32_t mode;
        if (kind == 1)
            mode = 1;
        else if (kind == 2)
            mode = 0;
        else if (kind == 0)
            mode = -1;
        else
            return r;
        Six s;
        std::memcpy(&s, jo::as_item_def(def)->bounding_box, sizeof s);  // passed by value (x64: by reference, as the callee's Key24)
        r = joan::orig::arg::as<std::int32_t>(joan::orig::as_players(obj(d::g_physic_sys))->assign_matches(s, static_cast<void*>(m), joan::orig::arg::i32(mode), static_cast<std::int8_t>(joan::orig::arg::i32(b)), static_cast<std::uint8_t>(joan::orig::arg::u32(1))));  // 0x006afb10
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::start_effect", 0x00478b40, &H::start_effect);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b85_01.cpp (game/b85_01)
//
//    - Popup::set_text 0x004775f0 (text) ret 4: memcpy 0x104 to +0x122, NUL at +0x226.
namespace {
namespace game_b85_01_2 {

using joan::img::str;

void* memcpy_(void* d, const void* s, std::uint32_t n) { return joan::orig::arg::as<void*>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n))); }

struct H {
    // ---- small -----------------------------------------------------------------------------
    void set_text(const void* text) {  // 0x004775f0
        // The object is the SaveDir (g_saves): the text buffer is its char_122 (x86 +0x122, +4 on x64, after body_buf).
        memcpy_(jo::self<jo::SaveDir>(this)->char_122, text, 0x104u);  // 0x104
        jo::self<jo::SaveDir>(this)->char_122[0x104] = 0;  // x86 +0x226
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_01", name, addr, fn)
B("Popup::set_text", 0x004775f0, &H::set_text);
#undef B

}  // namespace game_b85_01_2
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
//
//      0x004779e0 SaveGame::store_slot          (thiscall, ret 0)   - dead code (no refs).
namespace {
namespace game_b108_13 {

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
template <class R = void, class... A>
R api_call(img::Addr iat_slot, A... args) {  // Win32 import through its IAT slot
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(global<std::uintptr_t>(iat_slot))(args...);
}
using joan::img::obj;
using joan::img::str;

void* const kUtil = obj(d::g_misc);

// CRT
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_store_slot, "SaveGame::store_slot_4779e0");

struct H {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"  // an output buffer behind a facade's const char* parameter
#endif
    // ------------------------------------------------------------------ 0x004779e0
    std::uintptr_t store_slot() {
        c_store_slot.hit();
        char name[0x108];                           // [ebp-0x10c]
        joan::orig::as_save_dir(this)->file_name(joan::orig::arg::str(static_cast<char*>(name)), 1u);  // 0x00477770
        const i16 slot = static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_save_dir(this)->add(joan::orig::arg::str(0))));  // 0x00477440 (ax used)
        u8* const entry = joan::orig::arg::as<u8*>(joan::orig::as_units(this)->get_430410(joan::orig::arg::i16(static_cast<i32>(slot))));  // 0x00430410
        crt_strcpy(reinterpret_cast<char*>(entry + 3), name);
        u8 ok = 1;
        if (joan::orig::arg::as<i32>(joan::orig::as_game_file(entry)->save()) == 0) {  // 0x00478180
            ok = 0;
            if (joan::orig::as_misc(kUtil)->is_file_exist(joan::orig::arg::str(entry + 3)) != 0)  // 0x00590b80 exists
                api_call<i32>(d::iat_KERNEL32_DeleteFileA, reinterpret_cast<const char*>(entry + 3));  // DeleteFileA
        }
        joan::orig::as_save_dir(this)->browse(str(d::s_dot_bslash), 1u);  // 0x004770a0
        if (ok != 0) return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_paths(this)->find(joan::orig::arg::str(static_cast<char*>(name))));  // 0x00477640
        return 0;
    }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("SaveGame::store_slot_4779e0", 0x004779e0, &H::store_slot);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Popup::set_text_4775f0(void* a0) -> void { joan::bind::member<&::game_b85_01_2::H::set_text>(this, a0); }  // 0x4775f0
auto joan::orig::GameFile::load_check(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_01::H::load_check>(this, a0)); }  // 0x477d80
auto joan::orig::GameFile::close() -> void { joan::bind::member<&::game_b94_02::H::nop_477e60>(this); }  // 0x477e60
auto joan::orig::GameFile::load_477e80() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_12::H::load_477e80>(this)); }  // 0x477e80
auto joan::orig::GameFile::save() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::save>(this)); }  // 0x478180
auto joan::orig::GameFile::stamp() -> void { joan::bind::member<&::game_b85_01::H::stamp>(this); }  // 0x478480
auto joan::orig::GameFile::read_header(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b86_06::H::read_header>(this, a0)); }  // 0x478520
auto joan::orig::Unit::start_effect(char a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_02::H::start_effect>(this, a0)); }  // 0x478b40
auto joan::orig::GameFile::write_header_478cc0(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_01::H::write_header>(this, a0)); }  // 0x478cc0
