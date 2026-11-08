// ilib/db.cpp: functions of the original d:\ww\ilib\db.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstring>
#include "../original/helpers.hpp"
#include <cstdint>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "../original/database.hpp"
#include "../original/engine.hpp"
#include "../registry.hpp"
#include "../data_names.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../platform/win32_types.hpp"
#include <cstdarg>
#include <cstddef>
#include "../original/gameset.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/gen/records_s6.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from ilib/database.cpp (ilib/db.cpp)
namespace {
namespace ilib_database {

using joan::orig::Database;

#define DB(name, addr, fn) JOAN_REPLACE("ilib/db.cpp", "Database::" name, addr, fn)
DB("~Database", 0x00580810, &Database::destruct);
DB("open", 0x00580880, &Database::open);
DB("open(mem)", 0x00580ae0, &Database::open_mem);
DB("read", 0x00580c70, &Database::read);
DB("deinit", 0x00580da0, &Database::deinit);
DB("rec_data_size", 0x00580ed0, &Database::rec_data_size);
DB("field_type", 0x00580f00, &Database::field_type);
DB("field_offset", 0x00580fb0, &Database::field_offset);
DB("field_len", 0x00581070, &Database::field_len);
DB("field_dec", 0x00581170, &Database::field_dec);
#undef DB

}  // namespace ilib_database
}  // namespace

// ---------------------------------------- from modules/game_b101_09.cpp (game/b101_09)
//
//    - Database::write 0x00581250 (unused, const char* name), ret 8 -> int: header, field
//      descriptors, 0x0d, records (0x20 + data), 0x1a. File object destroyed on every path.
//
//  - 0x0056f640 / 0x0056f8e0 / 0x00571b60 / 0x00571d30 / 0x00572150 / 0x00575480 / 0x005756a0,
//    0x0057e4c0, 0x0057e780 and 0x00581250 have no references at all (no call, no data dword for
//    the function or its thunk): dead code, replaced for completeness. Only the listed entry points
//    are replaced; the sibling asm entry points that follow each of them (e.g. 0x0056f6f1,
//    0x00571bd2) stay original.
namespace {
namespace game_b101_09 {

JOAN_COUNTER(c_db_write, "Database::write");

struct alignas(4) File {
    u8 bytes[0x210];  // 0x110 in the original frame (0x004179c0); room
};
i32 file_write(File& f, const void* p, u32 n) { return joan::orig::as_file(&f)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
void file_close(File& f) { joan::orig::as_file(&f)->destruct(); }  // 0x005872b0 ~File
// The 0x20-byte dBase header (Database +0x10e on x86): [Debug] SaveFieldwise=1 save::put_DbfHeader, 0 the raw bytes.
i32 write_header(File& f, const jo::DbfHeader& h) {
    if (!joan::save::fieldwise()) return file_write(f, &h, 0x20);
    joan::save::Out o(*joan::orig::as_file(&f), joan::save::kDbfHeader, &h);
    joan::save::put_DbfHeader(o, h);
    return o.end();  // 0x005872e0
}

struct H {

    // 0x00581250
    i32 db_write(u32 /*unused*/, const char* name) {
        c_db_write.hit();
        if (jo::self<jo::Database>(this)->fields == nullptr || jo::view<u32>(jo::self<jo::Database>(this)->field_count) == 0 || jo::self<jo::Database>(this)->db_buf == nullptr) {
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_write_DataBase));  // 0x00586cb0 "Cannot write DataBase"
            return 0;
        }
        File file;
        joan::orig::as_file(&file)->construct();                // 0x004179c0
        joan::orig::as_file(&file)->file_create(joan::orig::arg::str(name), 1, static_cast<char>(0u));  // 0x00586f00 open
        u8 cr = 0x0d;
        jo::self<jo::Database>(this)->header.header_size = static_cast<u16>((jo::view<u32>(jo::self<jo::Database>(this)->field_count) << 5) + 0x21u);
        jo::view<u32>(jo::self<jo::Database>(this)->header.rec_count) = joan::orig::arg::as<u32>(joan::orig::as_database(this)->rec_count());  // 0x00415280
        if (write_header(file, jo::self<jo::Database>(this)->header) == 0) {
            file_close(file);
            return 0;
        }
        if (file_write(file, jo::view<void*>(jo::self<jo::Database>(this)->fields), jo::view<u32>(jo::self<jo::Database>(this)->field_count) << 5) == 0) {
            file_close(file);
            return 0;
        }
        if (file_write(file, &cr, 1) == 0) {
            file_close(file);
            return 0;
        }
        u8 blank = 0x20;
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_database(this)->rec_count()); ++i) {  // 0x00415280
            if (file_write(file, &blank, 1) == 0) {
                file_close(file);
                return 0;
            }
            const u32 len = static_cast<u32>(jo::self<jo::Database>(this)->header.rec_size) - 1u;
            void* const rec = joan::orig::arg::as<void*>(joan::orig::as_database(this)->read(joan::orig::arg::i32(i)));  // 0x00580c70
            if (file_write(file, rec, len) == 0) {
                file_close(file);
                return 0;
            }
        }
        u8 eof = 0x1a;
        if (file_write(file, &eof, 1) == 0) {
            file_close(file);
            return 0;
        }
        file_close(file);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_09", name, addr, fn)
B("Database::write", 0x00581250, &H::db_write);
#undef B

}  // namespace game_b101_09
}  // namespace

// ---------------------------------------- from modules/game_b104_08.cpp (game/b104_08)
//
//    - DataBase::append_record 0x005815d0 (record, path), ret 8: rewrites the .db file (header,
//      field table, all records, the new record, EOF 0x1a). Dead code (no references).
//
//    - DataBase::delete_record 0x00581a00 (index, path), ret 8: rewrites the file without record
//      `index`. Dead code (no references).
//
//    - DataBase::add_field 0x00581df0 (FieldDesc by value (32 bytes), path), ret 0x24: rewrites
//      the file with a new field (each record padded with `len` spaces).
namespace {
namespace game_b104_08 {

using joan::hook::global;
using i16 = std::int16_t;

void* const kMem = obj(d::g_mem);
void* const kErr = obj(d::g_err);
const char* const kDbCpp = str(d::s_d_ww_ilib_db_cpp);         // "d:\ww\ilib\db.cpp"
const char* const kCannotWrite = str(d::s_Cannot_write_DataBase);   // "Cannot write DataBase"

JOAN_COUNTER(c_append_record, "DataBase::append_record");
JOAN_COUNTER(c_delete_record, "DataBase::delete_record");
JOAN_COUNTER(c_add_field, "DataBase::add_field");

// ---- DataBase file helpers ----
struct DbFile {
    // File: 0x10e bytes on x86 (name 0x105, handle +0x105, +0x109, +0x10d), 0x112 on x64
    alignas(alignof(void*)) u8 raw[sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110];
    DbFile() { joan::orig::as_file(raw)->construct(); }                   // 0x004179c0
    void open(const char* path) { joan::orig::as_file(raw)->file_create(joan::orig::arg::str(path), 1, static_cast<char>(0)); }  // 0x00586f00
    i32 write(const void* p, u32 n) { return joan::orig::as_file(raw)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
    // The 0x20-byte dBase header (Database +0x10e on x86): [Debug] SaveFieldwise=1 save::put_DbfHeader, 0 the raw bytes.
    i32 write_header(const jo::DbfHeader& h) {
        if (!joan::save::fieldwise()) return write(&h, 0x20);
        joan::save::Out o(*joan::orig::as_file(raw), joan::save::kDbfHeader, &h);
        joan::save::put_DbfHeader(o, h);
        return o.end();  // 0x005872e0
    }
    void close() { joan::orig::as_file(raw)->destruct(); }              // 0x005872b0
};

struct FieldDesc {  // 32-byte field descriptor passed by value
    u8 b[32];
};
static_assert(sizeof(FieldDesc) == 32, "FieldDesc size");

struct H {

    // ---- DataBase ----
    i32 record_count() { return joan::orig::arg::as<i32>(joan::orig::as_database(this)->rec_count()); }                       // 0x00415280
    void* record(i32 i) { return joan::orig::arg::as<void*>(joan::orig::as_database(this)->read(joan::orig::arg::i32(i))); }  // 0x00580c70
    bool db_writable() {
        if (jo::self<jo::Database>(this)->fields != nullptr && jo::view<u32>(jo::self<jo::Database>(this)->field_count) != 0 && jo::self<jo::Database>(this)->db_buf != nullptr) return true;
        joan::orig::error::fatal(joan::orig::arg::mptr(kErr), joan::orig::arg::str(kCannotWrite));  // 0x00586cb0
        return false;
    }
    static i32 fail(DbFile& file) {
        file.close();
        return 0;
    }

    // 0x005815d0
    i32 append_record(const void* rec, const char* path) {
        c_append_record.hit();
        if (!db_writable()) return 0;
        DbFile file;
        file.open(path);
        u8 cr = 0xd;
        jo::self<jo::Database>(this)->header.header_size = static_cast<u16>(jo::view<u32>(jo::self<jo::Database>(this)->field_count) * 32u + 0x21u);
        jo::view<u32>(jo::self<jo::Database>(this)->header.rec_count) = static_cast<u32>(record_count()) + 1u;
        if (file.write_header(jo::self<jo::Database>(this)->header) == 0) return fail(file);
        if (file.write(jo::view<void*>(jo::self<jo::Database>(this)->fields), jo::view<u32>(jo::self<jo::Database>(this)->field_count) << 5) == 0) return fail(file);
        if (file.write(&cr, 1) == 0) return fail(file);
        u8 space = 0x20;
        for (i32 i = 1; i < record_count(); ++i) {
            if (file.write(&space, 1) == 0) return fail(file);
            const u32 len = static_cast<u32>(jo::self<jo::Database>(this)->header.rec_size) - 1u;
            void* const r = record(i);
            if (file.write(r, len) == 0) return fail(file);
        }
        if (file.write(&space, 1) == 0) return fail(file);
        if (file.write(rec, static_cast<u32>(jo::self<jo::Database>(this)->header.rec_size) - 1u) == 0) return fail(file);
        u8 eof = 0x1a;
        if (file.write(&eof, 1) == 0) return fail(file);
        file.close();
        return 1;
    }

    // 0x00581a00
    i32 delete_record(i32 index, const char* path) {
        c_delete_record.hit();
        if (!db_writable()) return 0;
        if (index <= 0 || index > record_count()) return 0;
        DbFile file;
        file.open(path);
        u8 cr = 0xd;
        jo::self<jo::Database>(this)->header.header_size = static_cast<u16>(jo::view<u32>(jo::self<jo::Database>(this)->field_count) * 32u + 0x21u);
        jo::view<u32>(jo::self<jo::Database>(this)->header.rec_count) = static_cast<u32>(record_count()) - 1u;
        if (file.write_header(jo::self<jo::Database>(this)->header) == 0) return fail(file);
        if (file.write(jo::view<void*>(jo::self<jo::Database>(this)->fields), jo::view<u32>(jo::self<jo::Database>(this)->field_count) << 5) == 0) return fail(file);
        if (file.write(&cr, 1) == 0) return fail(file);
        u8 space = 0x20;
        jo::view<u32>(jo::self<jo::Database>(this)->header.rec_count) = static_cast<u32>(record_count()) + 1u;
        for (i32 i = 1; !(i > static_cast<i32>(static_cast<u32>(record_count()) + 1u)); ++i) {
            if (i == index) continue;
            if (file.write(&space, 1) == 0) return fail(file);
            const u32 len = static_cast<u32>(jo::self<jo::Database>(this)->header.rec_size) - 1u;
            void* const r = record(i);
            if (file.write(r, len) == 0) return fail(file);
        }
        jo::view<u32>(jo::self<jo::Database>(this)->header.rec_count) = static_cast<u32>(record_count()) - 1u;
        u8 eof = 0x1a;
        if (file.write(&eof, 1) == 0) return fail(file);
        file.close();
        return 1;
    }

    // 0x00581df0
    i32 add_field(FieldDesc desc, const char* path) {
        c_add_field.hit();
        if (!db_writable()) return 0;
        DbFile file;
        file.open(path);
        u8 cr = 0xd;
        jo::self<jo::Database>(this)->header.header_size = static_cast<u16>((jo::view<u32>(jo::self<jo::Database>(this)->field_count) + 1u) * 32u + 0x21u);
        jo::view<u32>(jo::self<jo::Database>(this)->header.rec_count) = static_cast<u32>(record_count());
        u32 raw_len;
        std::memcpy(&raw_len, desc.b + 0x10, 4);
        const u32 len = (desc.b[0xb] == 0x4e) ? (raw_len & 0xffu) : (raw_len & 0xffffu);
        jo::self<jo::Database>(this)->header.rec_size = static_cast<u16>(jo::self<jo::Database>(this)->header.rec_size + static_cast<u16>(len));
        if (file.write_header(jo::self<jo::Database>(this)->header) == 0) return fail(file);
        if (file.write(jo::view<void*>(jo::self<jo::Database>(this)->fields), jo::view<u32>(jo::self<jo::Database>(this)->field_count) << 5) == 0) return fail(file);
        if (file.write(&desc, 0x20) == 0) return fail(file);
        if (file.write(&cr, 1) == 0) return fail(file);
        u8 space = 0x20;
        const i32 line = static_cast<i32>(global<i16>(d::g_a15208)) + 0x29;
        u8* const pad = joan::orig::arg::as<u8*>(joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(len), joan::orig::arg::str(kDbCpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        for (i32 i = 0; i < static_cast<i32>(len); ++i) pad[i] = 0x20;
        jo::self<jo::Database>(this)->header.rec_size = static_cast<u16>(jo::self<jo::Database>(this)->header.rec_size - static_cast<u16>(len));
        for (i32 i = 1; !(i > record_count()); ++i) {
            if (file.write(&space, 1) == 0) return fail(file);
            const u32 rlen = static_cast<u32>(jo::self<jo::Database>(this)->header.rec_size) - 1u;
            void* const r = record(i);
            if (file.write(r, rlen) == 0) return fail(file);
            if (file.write(pad, len) == 0) return fail(file);
        }
        const i32 line2 = static_cast<i32>(global<i16>(d::g_a15208)) + 0x3a;
        joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(pad), joan::orig::arg::str(kDbCpp), joan::orig::arg::i32(line2));  // 0x0058ed70
        jo::self<jo::Database>(this)->header.rec_size = static_cast<u16>(jo::self<jo::Database>(this)->header.rec_size + static_cast<u16>(len));
        u8 eof = 0x1a;
        if (file.write(&eof, 1) == 0) return fail(file);
        jo::view<u32>(jo::self<jo::Database>(this)->field_count) = jo::view<u32>(jo::self<jo::Database>(this)->field_count) + 1u;
        file.close();
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_08", name, addr, fn)
B("DataBase::append_record", 0x005815d0, &H::append_record);
B("DataBase::delete_record", 0x00581a00, &H::delete_record);
B("DataBase::add_field", 0x00581df0, &H::add_field);
#undef B

}  // namespace game_b104_08
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x00580730 Stream::Stream       (thiscall(a, b, c), ret 0xc; SEH frame only guards the
//                 base constructor on unwind - normal path kept)
namespace {
namespace game_b92_04 {

using joan::img::str;

using i32 = std::int32_t;

struct H {

    // ------------------------------------------------------------------------ 0x00580730
    H* stream_ctor(const char* a, i32 b, i32 c) {
        joan::orig::as_file(this)->construct();  // 0x004179c0
        jo::self<jo::Database>(this)->db_buf = nullptr;
        jo::self<jo::Database>(this)->rec_buf = nullptr;
        jo::self<jo::Database>(this)->db_buf_allocated = 0;
        jo::self<jo::Database>(this)->fields = nullptr;
        jo::self<jo::Database>(this)->field_count = 0;
        jo::self<jo::Database>(this)->last_read_no = -1;
        if (a != nullptr) joan::orig::as_database(this)->open(a, joan::orig::arg::i32(b), joan::orig::arg::i32(c));  // 0x00580880
        return this;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Stream::Stream", 0x00580730, &H::stream_ctor);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b111_20.cpp (fix/uninit)
//
//    - DbFile::add_field 0x00582350 (thiscall(name, type, len, arg4), ret 0x10; dead code, no
//      reference to it or its thunk 0x004024d7): builds a 32-byte dBase field descriptor and
//      passes it by value to 0x00581df0 (ret 0x24), which writes it to the file. UNINIT READ:
//      descriptor bytes 18..31 (dBase "reserved") are never written; we zero them.
namespace {
namespace game_b111_20 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::str;

// ---------------------------------------------------------------- 0x00582350 helpers
struct FieldDesc {  // dBase field descriptor (32 bytes, passed by value)
    u8 b[32];
};
static_assert(sizeof(FieldDesc) == 32, "FieldDesc");

struct H {
    // ------------------------------------------------------------ 0x00582350
    u32 add_field(const char* name, u32 type, u32 len, const char* arg4) {  // arg4: the path (a pointer)
        FieldDesc d;
        std::memset(&d, 0, sizeof d);  // fix/uninit: bytes 18..31 never written by the original
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(d.b))), joan::orig::arg::str(name), 0xbu);  // strncpy
        d.b[11] = static_cast<u8>(type & 0xff);
        d.b[16] = static_cast<u8>(len & 0xff);
        d.b[17] = 0;
        const u32 off = jo::self<jo::Database>(this)->header.rec_size;
        std::memcpy(d.b + 12, &off, 4);
        return joan::orig::arg::as<u32>(joan::orig::as_database(this)->add_field(d, joan::orig::arg::str(arg4)));  // 0x00581df0
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: dBase descriptor reserved bytes 18..31 zeroed (stack garbage written to the file before).
F("DbFile::add_field_582350", 0x00582350, &H::add_field);
#undef F

}  // namespace game_b111_20
}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {

    i32 count_5823e0() {
        if (jo::self<jo::Database>(this)->fields == nullptr) return -1;
        return jo::self<jo::Database>(this)->field_count;
    }
    jo::DbfFieldDesc* at_582420(i32 i) {
        if (jo::self<jo::Database>(this)->fields == nullptr) return nullptr;
        if (i >= jo::self<jo::Database>(this)->field_count || i < 0) return nullptr;
        return jo::self<jo::Database>(this)->fields + i;
    }
    jo::DbfFieldDesc* at_582490(i32 i) { return at_582420(i); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Array::count_5823e0", 0x005823e0, &H::count_5823e0);
B("Array::at_582420", 0x00582420, &H::at_582420);
B("Array::at_582490", 0x00582490, &H::at_582490);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_b104_08.cpp (game/b104_08)
namespace {
namespace game_b104_08_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using u8 = std::uint8_t;
using u16 = std::uint16_t;

using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::at;

void* const kErr = obj(d::g_err);
const char* const kCannotWrite = str(d::s_Cannot_write_DataBase);   // "Cannot write DataBase"

JOAN_COUNTER(c_delete_field, "DataBase::delete_field");

// ---- DataBase file helpers ----
struct DbFile {
    // File: 0x10e bytes on x86 (name 0x105, handle +0x105, +0x109, +0x10d), 0x112 on x64
    alignas(alignof(void*)) u8 raw[sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110];
    DbFile() { joan::orig::as_file(raw)->construct(); }                   // 0x004179c0
    void open(const char* path) { joan::orig::as_file(raw)->file_create(joan::orig::arg::str(path), 1, static_cast<char>(0)); }  // 0x00586f00
    i32 write(const void* p, u32 n) { return joan::orig::as_file(raw)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0
    // The 0x20-byte dBase header (Database +0x10e on x86): [Debug] SaveFieldwise=1 save::put_DbfHeader, 0 the raw bytes.
    i32 write_header(const jo::DbfHeader& h) {
        if (!joan::save::fieldwise()) return write(&h, 0x20);
        joan::save::Out o(*joan::orig::as_file(raw), joan::save::kDbfHeader, &h);
        joan::save::put_DbfHeader(o, h);
        return o.end();  // 0x005872e0
    }
    void close() { joan::orig::as_file(raw)->destruct(); }              // 0x005872b0
};

struct H {

    // ---- DataBase ----
    i32 record_count() { return joan::orig::arg::as<i32>(joan::orig::as_database(this)->rec_count()); }                       // 0x00415280
    void* record(i32 i) { return joan::orig::arg::as<void*>(joan::orig::as_database(this)->read(joan::orig::arg::i32(i))); }  // 0x00580c70
    bool db_writable() {
        if (jo::self<jo::Database>(this)->fields != nullptr && jo::view<u32>(jo::self<jo::Database>(this)->field_count) != 0 && jo::self<jo::Database>(this)->db_buf != nullptr) return true;
        joan::orig::error::fatal(joan::orig::arg::mptr(kErr), joan::orig::arg::str(kCannotWrite));  // 0x00586cb0
        return false;
    }
    static i32 fail(DbFile& file) {
        file.close();
        return 0;
    }

    // 0x00582500
    i32 delete_field(const char* name, const char* path) {
        c_delete_field.hit();
        if (!db_writable()) return 0;
        i32 i;
        u32 off = 0;  // [ebp-0x18]
        u32 len = 0;  // [ebp-0x1c]
        for (i = 0; i < jo::self<jo::Database>(this)->field_count; ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(name), joan::orig::arg::str(jo::view<u8*>(jo::self<jo::Database>(this)->fields) + (static_cast<u32>(i) << 5))) == 0) {
                const u8* const fd = jo::view<u8*>(jo::self<jo::Database>(this)->fields) + (static_cast<u32>(i) << 5);
                off = jo::view<u32>(jo::self<jo::DbfFieldDesc>(fd)->offset);
                if (jo::view<u8>(jo::self<jo::DbfFieldDesc>(fd)->type) == 0x4e) {
                    len = jo::self<jo::DbfFieldDesc>(fd)->length;
                } else {
                    len = *reinterpret_cast<const u16*>(&jo::self<jo::DbfFieldDesc>(fd)->length);  // the u16 length of a 'C' field (length + decimals)
                }
                break;
            }
        }
        if (i == jo::self<jo::Database>(this)->field_count) return 0;
        DbFile file;
        file.open(path);
        u8 cr = 0xd;
        jo::self<jo::Database>(this)->header.header_size = static_cast<u16>((jo::view<u32>(jo::self<jo::Database>(this)->field_count) - 1u) * 32u + 0x21u);
        jo::view<u32>(jo::self<jo::Database>(this)->header.rec_count) = static_cast<u32>(record_count());
        jo::self<jo::Database>(this)->header.rec_size = static_cast<u16>(jo::self<jo::Database>(this)->header.rec_size - static_cast<u16>(len));
        if (file.write_header(jo::self<jo::Database>(this)->header) == 0) return fail(file);
        if (file.write(jo::view<void*>(jo::self<jo::Database>(this)->fields), static_cast<u32>(i) << 5) == 0) return fail(file);
        jo::view<u32>(jo::self<jo::Database>(this)->field_count) = jo::view<u32>(jo::self<jo::Database>(this)->field_count) - 1u;
        if (i < jo::self<jo::Database>(this)->field_count) {
            const u32 n = (jo::view<u32>(jo::self<jo::Database>(this)->field_count) - static_cast<u32>(i)) << 5;
            if (file.write(jo::self<jo::Database>(this)->fields + i + 1, n) == 0) return fail(file);
        }
        if (file.write(&cr, 1) == 0) return fail(file);
        u8 space = 0x20;
        jo::self<jo::Database>(this)->header.rec_size = static_cast<u16>(jo::self<jo::Database>(this)->header.rec_size + static_cast<u16>(len));
        for (i32 j = 1; !(j > record_count()); ++j) {
            if (file.write(&space, 1) == 0) return fail(file);
            u8* const r = static_cast<u8*>(record(j));
            if (file.write(r, off - 1u) == 0) return fail(file);
            const u32 rest = static_cast<u32>(jo::self<jo::Database>(this)->header.rec_size) - off - len;
            if (file.write(r + off + len - 1u, rest) == 0) return fail(file);
        }
        jo::self<jo::Database>(this)->header.rec_size = static_cast<u16>(jo::self<jo::Database>(this)->header.rec_size - static_cast<u16>(len));
        u8 eof = 0x1a;
        if (file.write(&eof, 1) == 0) return fail(file);
        file.close();
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_08", name, addr, fn)
B("DataBase::delete_field", 0x00582500, &H::delete_field);
#undef B

}  // namespace game_b104_08_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Database::construct(const char* a0, std::int32_t a1, std::int32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_04::H::stream_ctor>(this, a0, a1, a2)); }  // 0x580730
