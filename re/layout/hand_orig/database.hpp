// Original memory layouts of the engine's dBase table reader (d:\ww\ilib\db.cpp,
// joan.exe 0x00580810-0x00582350) and of ResourceDb (d:\ww\ilib\resdb.cpp,
// 0x0059b440-0x0059b920), a resource file whose entries are located through a
// 4-byte offset stored in a dBase field (e.g. HBITMAPPTR in CURSOR.RES).
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>

#include "file.hpp"

namespace joan::orig {

#pragma pack(push, 1)

struct DbfHeader {            // the first 32 bytes of a .dbf file, copied verbatim
    std::uint8_t version;     // 0x03
    std::uint8_t date[3];     // yy mm dd
    std::int32_t rec_count;
    std::uint16_t header_size;
    std::uint16_t rec_size;   // including the 1-byte delete flag
    std::uint8_t reserved[20];
};
static_assert(sizeof(DbfHeader) == 32);

struct DbfFieldDesc {         // 32-byte field descriptor
    char name[11];
    char type;                // 'C', 'N', ...
    std::int32_t offset;      // offset inside the record incl. delete flag (written by the tools)
    std::uint8_t length;      // for 'C' fields length is the u16 at this position
    std::uint8_t decimals;
    std::uint8_t reserved[14];
};
static_assert(sizeof(DbfFieldDesc) == 32);

struct Database {
    File file;                    // 0x000  (base class in the original source)
    DbfHeader header;             // 0x10e
    char* rec_buf;                // 0x12e  one record, used when not buffered
    std::int32_t cur_rec_no;      // 0x132  set to 1 by open; read(0) means "current"
    std::int32_t last_read_no;    // 0x136  record held in rec_buf (never reset - see read)
    char* db_buf;                 // 0x13a  all records (points at record 1, after its delete flag)
    char db_buf_allocated;        // 0x13e  1 = db_buf owned (open from file), 0 = borrowed (open from memory)
    DbfFieldDesc* fields;         // 0x13f
    std::int32_t field_count;     // 0x143

    // --- reimplemented members (src/ilib/database.cpp) ---
    void destruct();                                                           // 0x00580810  ~Database
    void open(const char* name, std::int32_t buffer_all, std::int32_t read_fields);  // 0x00580880
    void open_mem(char* data, std::int32_t read_fields);                       // 0x00580ae0
    char* read(std::int32_t rec_no);                                           // 0x00580c70  1-based
    void deinit();                                                             // 0x00580da0
    std::int32_t rec_data_size();                                              // 0x00580ed0  rec_size - 1
    char field_type(const char* name);                                         // 0x00580f00
    std::int32_t field_offset(const char* name);                               // 0x00580fb0
    std::int32_t field_len(const char* name);                                  // 0x00581070
    std::int32_t field_dec(const char* name);                                  // 0x00581170

    // The other Database functions the game code calls (typed, through their slots).
#include "gen/database_methods.inc"
};

struct ResourceDb {
    File file;                     // 0x000  (base class)
    char init_flag;                // 0x10e
    Database* db;                  // 0x10f  table whose records hold the data offsets
    std::int32_t index_offset;     // 0x113  offset of the 4-byte pointer field inside a record
    std::int32_t use_common_buf;   // 0x117
    char* data_buf;                // 0x11b
    std::int32_t data_buf_size;    // 0x11f
    char read_all;                 // 0x123

    // --- reimplemented members (src/ilib/database.cpp) ---
    void deinit();                                                             // 0x0059b440
    char* read(std::int32_t rec_no);                                           // 0x0059b4f0
    File* get_file();                                                          // 0x0059b620
    void init_imported(const char* name, char read_all, std::int32_t use_common_buf);  // 0x0059b6d0
    char* read_imported(std::int32_t offset);                                  // 0x0059b830

    // The other ResourceDb functions the game code calls (typed, through their slots).
#include "gen/resource_db_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Database, header) == 0x10e);
static_assert(offsetof(Database, rec_buf) == 0x12e);
static_assert(offsetof(Database, last_read_no) == 0x136);
static_assert(offsetof(Database, db_buf_allocated) == 0x13e);
static_assert(offsetof(Database, fields) == 0x13f);
static_assert(offsetof(Database, field_count) == 0x143);
static_assert(offsetof(ResourceDb, db) == 0x10f);
static_assert(offsetof(ResourceDb, use_common_buf) == 0x117);
static_assert(offsetof(ResourceDb, data_buf_size) == 0x11f);
static_assert(offsetof(ResourceDb, read_all) == 0x123);
#endif

}  // namespace joan::orig
