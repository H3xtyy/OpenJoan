// Reimplementation of the engine's dBase table reader (d:\ww\ilib\db.cpp,
// 10 of 15 functions - the 4 table *writers* at 0x00581250-0x00581df0 are
// editor-only and never called by the game) and of ResourceDb
// (d:\ww\ilib\resdb.cpp, all 5 functions).
//
// The portable equivalent used by the standalone engine is src/ilib/database.*.
// (The verify-mode wrappers of the side-effect-free functions were removed with joan.exe's
// code in stage S7c; this note keeps the __LINE__ values passed to mem() below unchanged.)
#include <cstring>
#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "../original/database.hpp"
#include "../original/engine.hpp"
#include "../registry.hpp"

namespace {
constexpr const char* kSrc = "joan-re/src/ilib/database.cpp";

JOAN_COUNTER(c_destruct, "Database::~Database");
JOAN_COUNTER(c_open, "Database::open");
JOAN_COUNTER(c_open_mem, "Database::open(mem)");
JOAN_COUNTER(c_read, "Database::read");
JOAN_COUNTER(c_deinit, "Database::deinit");
JOAN_COUNTER(c_rec_size, "Database::rec_data_size");
JOAN_COUNTER(c_ftype, "Database::field_type");
JOAN_COUNTER(c_foffset, "Database::field_offset");
JOAN_COUNTER(c_flen, "Database::field_len");
JOAN_COUNTER(c_fdec, "Database::field_dec");
JOAN_COUNTER(c_rdb_deinit, "ResourceDb::deinit");
JOAN_COUNTER(c_rdb_read, "ResourceDb::read");
JOAN_COUNTER(c_rdb_file, "ResourceDb::get_file");
JOAN_COUNTER(c_rdb_init, "ResourceDb::init_imported");
JOAN_COUNTER(c_rdb_read_imp, "ResourceDb::read_imported");

// Counts field descriptors up to the 0x0D terminator, as open/open_mem do.
std::int32_t count_fields(const joan::orig::DbfFieldDesc* fields, std::int32_t bytes) {
    std::int32_t n = 0;
    while (bytes > 0 && reinterpret_cast<const char*>(&fields[n])[0] != 0x0d) {
        ++n;
        bytes -= 32;
    }
    return n;
}
}  // namespace

namespace joan::orig {

// ===========================================================================
// Database
// ===========================================================================

// 0x00580810 - ~Database: deinit, then ~File.
void Database::destruct() {
    c_destruct.hit();
    deinit();
    file.destruct();
}

// 0x00580880 - opens a .dbf file. buffer_all: load every record and close the
// file; otherwise records are read one at a time into rec_buf on demand.
void Database::open(const char* name, std::int32_t buffer_all, std::int32_t read_fields) {
    c_open.hit();
    deinit();
    file.file_open(name, 1, 0);
    file.file_read(&header, sizeof(header));

    if (read_fields) {
        const std::int32_t bytes = header.header_size - 32;
        fields = static_cast<DbfFieldDesc*>(mem().add(static_cast<std::uint32_t>(bytes), kSrc, __LINE__));
        file.file_read(fields, static_cast<std::uint32_t>(bytes));
        field_count = count_fields(fields, bytes);
    } else {
        fields = nullptr;
        field_count = 0;
    }

    if (buffer_all) {
        const std::uint32_t total = static_cast<std::uint32_t>(header.rec_size) * static_cast<std::uint32_t>(header.rec_count);
        db_buf = static_cast<char*>(mem().add(total, kSrc, __LINE__));
        file.file_seek(header.header_size + 1, -1);  // +1: skip record 1's delete flag
        file.file_read(db_buf, total);
        file.file_close();
        db_buf_allocated = 1;
    } else {
        rec_buf = static_cast<char*>(mem().add(header.rec_size, kSrc, __LINE__));
    }
    cur_rec_no = 1;
}

// 0x00580ae0 - uses a .dbf image already in memory (e.g. an entry of a .SET
// archive). The records are not copied; db_buf points into `data`.
void Database::open_mem(char* data, std::int32_t read_fields) {
    c_open_mem.hit();
    deinit();
    std::memcpy(&header, data, sizeof(header));

    if (read_fields) {
        const std::int32_t bytes = header.header_size - 32;
        fields = static_cast<DbfFieldDesc*>(mem().add(static_cast<std::uint32_t>(bytes), kSrc, __LINE__));
        std::memcpy(fields, data + 32, static_cast<std::size_t>(bytes));
        field_count = count_fields(fields, bytes);
    } else {
        fields = nullptr;
        field_count = 0;
    }
    db_buf = data + header.header_size + 1;
    db_buf_allocated = 0;
    cur_rec_no = 1;
}

// 0x00580c70 - returns record `rec_no` (1-based; <= 0 means cur_rec_no),
// pointing just past its delete flag, or nullptr when out of range.
// Quirk kept: last_read_no is never reset by deinit/open, so reopening a
// different file on the same object can return a stale rec_buf for that number.
char* Database::read(std::int32_t rec_no) {
    c_read.hit();
    if (rec_no <= 0) rec_no = cur_rec_no;
    if (rec_no < 1 || rec_no > header.rec_count) return nullptr;
    if (db_buf) return db_buf + static_cast<std::int32_t>(header.rec_size) * (rec_no - 1);
    if (rec_no == last_read_no) return rec_buf;

    file.file_seek(header.header_size + static_cast<std::int32_t>(header.rec_size) * (rec_no - 1) + 1, -1);
    file.file_read(rec_buf, header.rec_size);
    last_read_no = rec_no;
    return rec_buf;
}

// 0x00580da0 - quirk kept: a borrowed db_buf (open_mem) is not cleared.
void Database::deinit() {
    c_deinit.hit();
    if (rec_buf) {
        mem().del(rec_buf, kSrc, __LINE__);
        rec_buf = nullptr;
    }
    if (fields) mem().del(fields, kSrc, __LINE__);
    fields = nullptr;
    field_count = 0;
    if (db_buf && db_buf_allocated) {
        mem().del(db_buf, kSrc, __LINE__);
        db_buf = nullptr;
        db_buf_allocated = 0;
    }
}

// 0x00580ed0 - record size without the delete flag.
std::int32_t Database::rec_data_size() {
    c_rec_size.hit();
    return static_cast<std::int32_t>(header.rec_size) - 1;
}

namespace {
// Field lookup used by the four getters: case-insensitive (CRT _stricmp).
DbfFieldDesc* find_field(Database* db, const char* name) {
    for (std::int32_t i = 0; i < db->field_count; ++i)
        if (_stricmp(name, db->fields[i].name) == 0) return &db->fields[i];
    return nullptr;
}
}  // namespace

// 0x00580f00 - field type letter; -1 without field info, 0 if not found.
char Database::field_type(const char* name) {
    c_ftype.hit();
    if (!fields || !field_count) return static_cast<char>(-1);
    const DbfFieldDesc* f = find_field(this, name);
    return f ? f->type : 0;
}

// 0x00580fb0 - offset of the field inside the record data (after the delete flag), -1 if unknown.
std::int32_t Database::field_offset(const char* name) {
    c_foffset.hit();
    if (!fields || !field_count) return -1;
    const DbfFieldDesc* f = find_field(this, name);
    return f ? f->offset - 1 : -1;
}

// 0x00581070 - field length: one byte for 'N' fields, a u16 (length + decimals bytes) otherwise.
std::int32_t Database::field_len(const char* name) {
    c_flen.hit();
    if (!fields || !field_count) return -1;
    const DbfFieldDesc* f = find_field(this, name);
    if (!f) return -1;
    if (f->type == 'N') return f->length;
    std::uint16_t len16;
    std::memcpy(&len16, &f->length, sizeof(len16));
    return len16;
}

// 0x00581170 - decimals of an 'N' field, -1 otherwise.
std::int32_t Database::field_dec(const char* name) {
    c_fdec.hit();
    if (!fields || !field_count) return -1;
    const DbfFieldDesc* f = find_field(this, name);
    if (!f || f->type != 'N') return -1;
    return f->decimals;
}

// ===========================================================================
// ResourceDb
// ===========================================================================

// 0x0059b440
void ResourceDb::deinit() {
    c_rdb_deinit.hit();
    if (!init_flag) return;
    if (!use_common_buf && data_buf) {
        mem().del(data_buf, kSrc, __LINE__);
        data_buf = nullptr;
    }
    if (!read_all) file.file_close();
    init_flag = 0;
}

// 0x0059b4f0 - reads the resource whose file offset is stored in the
// index field of record `rec_no` of `db`. Each resource is prefixed with
// its size as a u32. Returns nullptr if the field is blank.
char* ResourceDb::read(std::int32_t rec_no) {
    c_rdb_read.hit();
    char* rec = db->read(rec_no);
    if (!rec) return nullptr;
    const char* ptr_field = rec + index_offset;
    if (std::memcmp(ptr_field, "    ", 4) == 0) return nullptr;

    std::int32_t offset;
    std::memcpy(&offset, ptr_field, sizeof(offset));
    file.file_seek(offset, -1);
    data_buf_size = file.file_get_long();
    if (!use_common_buf)
        data_buf = static_cast<char*>(mem().resize(data_buf, static_cast<std::uint32_t>(data_buf_size), kSrc, __LINE__));
    file.file_read(data_buf, static_cast<std::uint32_t>(data_buf_size));
    return data_buf;
}

// 0x0059b620 - positions the file at the data of the current record's
// resource (after its size prefix). Quirk kept: the blank test compares
// 8 bytes against spaces although the offset field is 4 bytes wide.
File* ResourceDb::get_file() {
    c_rdb_file.hit();
    char* rec = db->read(0);
    if (!rec) return nullptr;
    const char* ptr_field = rec + index_offset;
    if (std::memcmp(ptr_field, "        ", 8) == 0) return nullptr;

    std::int32_t offset;
    std::memcpy(&offset, ptr_field, sizeof(offset));
    file.file_seek(offset + 4, -1);
    return &file;
}

// 0x0059b6d0 - opens a resource file addressed by raw offsets (no table).
// read_all: load the whole file and close it.
void ResourceDb::init_imported(const char* name, char read_all_, std::int32_t use_common_buf_) {
    c_rdb_init.hit();
    deinit();
    db = nullptr;
    index_offset = 0;
    read_all = read_all_;
    file.file_open(name, 1, 0);

    if (read_all) {
        data_buf_size = file.file_size();
        data_buf = static_cast<char*>(mem().add(static_cast<std::uint32_t>(data_buf_size), kSrc, __LINE__));
        file.file_read(data_buf, static_cast<std::uint32_t>(data_buf_size));
        file.file_close();
        use_common_buf = 0;
    } else {
        use_common_buf = use_common_buf_;
        data_buf = use_common_buf ? common_res_buf() : nullptr;
    }
    init_flag = 1;
}

// 0x0059b830 - resource at a raw file offset (u32 size prefix, then data).
char* ResourceDb::read_imported(std::int32_t offset) {
    c_rdb_read_imp.hit();
    if (read_all) return data_buf + offset + 4;

    file.file_seek(offset, -1);
    data_buf_size = file.file_get_long();
    if (!use_common_buf)
        data_buf = static_cast<char*>(mem().resize(data_buf, static_cast<std::uint32_t>(data_buf_size), kSrc, __LINE__));
    file.file_read(data_buf, static_cast<std::uint32_t>(data_buf_size));
    return data_buf;
}

}  // namespace joan::orig

using joan::orig::Database;
using joan::orig::ResourceDb;
#define DB(name, addr, fn) JOAN_REPLACE("ilib/db.cpp", "Database::" name, addr, fn)
#undef DB
#define RDB(name, addr, fn) JOAN_REPLACE("ilib/resdb.cpp", "ResourceDb::" name, addr, fn)
#undef RDB
