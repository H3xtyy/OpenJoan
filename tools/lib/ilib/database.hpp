// dBase III tables (.dbf), the engine's data-definition format.
//
// Reimplements the Enlight engine's Database class (d:\ww\ilib\db.cpp,
// joan.exe 0x00580880..0x00582350). Every table in STD.SET, INTERFAC.SET,
// RESOURCE etc. is a dBase III file, as is CURSOR.RES.
//
// Layout:
//   +0  u8  version (0x03)          +1  u8 yy, mm, dd (last update)
//   +4  u32 record_count            +8  u16 header_size   +10 u16 record_size
//   +32 field descriptors, 32 bytes each, terminated by 0x0D:
//       char name[11]; char type; u32 addr; u8 length; u8 decimals; u8 pad[14]
//   header_size: records, each = 1 delete-flag byte (' ' or '*') + fields.
//
// Only 'C' (character) and 'N' (numeric, ASCII) fields occur in the game data.
// The engine reads records as raw fixed-width bytes and converts fields on use
// (misc.atoi(rec->field, len)); `Record::raw()` gives the same access.
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace joan::ilib {

struct DbField {
    std::string name;
    char type = 'C';
    std::uint8_t length = 0;
    std::uint8_t decimals = 0;
    std::uint16_t offset = 0;  // offset inside a record, including the delete flag byte
};

class Database;

class DbRecord {
public:
    DbRecord(const Database& db, std::span<const std::byte> bytes) : db_(&db), bytes_(bytes) {}

    [[nodiscard]] bool deleted() const { return !bytes_.empty() && bytes_[0] == std::byte{'*'}; }

    /// Raw field text exactly as stored (space padded).
    [[nodiscard]] std::string_view raw(std::size_t field_index) const;
    [[nodiscard]] std::optional<std::string_view> raw(std::string_view field_name) const;

    /// Field text with trailing spaces removed (engine: misc.rtrim).
    [[nodiscard]] std::string str(std::string_view field_name) const;
    /// Numeric field (engine: misc.atoi). Returns `fallback` if the field is missing or blank.
    [[nodiscard]] long long num(std::string_view field_name, long long fallback = 0) const;
    [[nodiscard]] double real(std::string_view field_name, double fallback = 0.0) const;

    [[nodiscard]] std::span<const std::byte> bytes() const { return bytes_; }

private:
    const Database* db_;
    std::span<const std::byte> bytes_;
};

class Database {
public:
    /// Parses a .dbf image. Keeps a copy of the data. Throws joan::FormatError.
    static Database parse(std::span<const std::byte> data);

    [[nodiscard]] const std::vector<DbField>& fields() const { return fields_; }
    [[nodiscard]] std::size_t record_count() const { return record_count_; }
    [[nodiscard]] std::size_t record_size() const { return record_size_; }
    [[nodiscard]] std::optional<std::size_t> field_index(std::string_view name) const;

    /// 0-based record access (the engine's read(recNo) is 1-based).
    [[nodiscard]] DbRecord record(std::size_t index) const;

    /// Iterates over non-deleted records.
    template <class Fn>
    void for_each(Fn&& fn) const {
        for (std::size_t i = 0; i < record_count_; ++i) {
            DbRecord r = record(i);
            if (!r.deleted()) fn(r);
        }
    }

    /// Exports the table as CSV (header row + records).
    [[nodiscard]] std::string to_csv() const;

private:
    std::vector<std::byte> data_;
    std::vector<DbField> fields_;
    std::size_t record_count_ = 0;
    std::size_t header_size_ = 0;
    std::size_t record_size_ = 0;
};

}  // namespace joan::ilib
