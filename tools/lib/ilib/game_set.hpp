// Game sets: resource archives whose entries are dBase tables.
//
// Reimplements GameSet (d:\ww\ilib\gameset.cpp, joan.exe ~0x0058ae30).
// Resource/STD.SET holds the main data tables (units, items, nations,
// animations, models, ...), Resource/INTERFAC.SET the UI layouts and
// Resource/RESOURCE an older/editor copy of STD. Every set has a HEADER
// table with a CODE and DES (description) field.
#pragma once

#include <map>
#include <string>
#include <string_view>

#include "ilib/database.hpp"
#include "ilib/res_archive.hpp"

namespace joan {
class Vfs;
}

namespace joan::ilib {

class GameSet {
public:
    static GameSet open(const Vfs& vfs, std::string_view game_path);
    static GameSet from_archive(ResArchive archive);

    [[nodiscard]] const ResArchive& archive() const { return archive_; }
    [[nodiscard]] const std::string& code() const { return code_; }
    [[nodiscard]] const std::string& description() const { return description_; }

    /// Returns the named table (case-insensitive), parsed once and cached.
    /// Throws std::runtime_error if the set has no such table.
    const Database& db(std::string_view name) const;
    [[nodiscard]] bool has(std::string_view name) const { return archive_.find(name) != nullptr; }

private:
    ResArchive archive_;
    std::string code_;
    std::string description_;
    mutable std::map<std::string, Database, std::less<>> cache_;
};

}  // namespace joan::ilib
