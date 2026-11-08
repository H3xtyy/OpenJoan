#include "ilib/game_set.hpp"

#include <stdexcept>

#include "core/vfs.hpp"

namespace joan::ilib {

GameSet GameSet::open(const Vfs& vfs, std::string_view game_path) {
    return from_archive(ResArchive::parse(vfs.read(game_path), ResFormat::ShortIndex));
}

GameSet GameSet::from_archive(ResArchive archive) {
    GameSet set;
    set.archive_ = std::move(archive);
    if (set.has("HEADER")) {
        const Database& header = set.db("HEADER");
        if (header.record_count() > 0) {
            set.code_ = header.record(0).str("CODE");
            set.description_ = header.record(0).str("DES");
        }
    }
    return set;
}

const Database& GameSet::db(std::string_view name) const {
    const ResEntry* entry = archive_.find(name);
    if (!entry) throw std::runtime_error("game set has no table named " + std::string(name));
    if (auto it = cache_.find(entry->name); it != cache_.end()) return it->second;
    return cache_.emplace(entry->name, Database::parse(archive_.data(*entry))).first->second;
}

}  // namespace joan::ilib
