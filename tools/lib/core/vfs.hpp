// Virtual file system over the original game directory.
//
// The game data refers to files with DOS-style paths in arbitrary case
// ("scenario1\\S1L1w", "Resource\\STD.SET", "model\\!alen_H.SMO"). On Windows
// that just works; on case-sensitive file systems (Linux, Steam Deck, macOS
// with case-sensitive APFS) it does not. The Vfs resolves such paths
// case-insensitively and with either slash direction.
#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace joan {

class Vfs {
public:
    /// Mounts the game installation directory (the folder containing joan.exe).
    explicit Vfs(std::filesystem::path root);

    [[nodiscard]] const std::filesystem::path& root() const { return root_; }

    /// Resolves a game-relative path ("Resource\\STD.SET") to a real path.
    [[nodiscard]] std::optional<std::filesystem::path> resolve(std::string_view game_path) const;

    [[nodiscard]] bool exists(std::string_view game_path) const { return resolve(game_path).has_value(); }

    /// Reads a game file. Throws std::runtime_error if missing.
    [[nodiscard]] std::vector<std::byte> read(std::string_view game_path) const;

    /// Normalises a game path: forward slashes, lower case, no leading "./".
    static std::string normalise(std::string_view game_path);

private:
    std::filesystem::path root_;
    // normalised relative path -> real path. Built lazily per directory.
    mutable std::unordered_map<std::string, std::filesystem::path> cache_;
    mutable std::unordered_map<std::string, bool> scanned_dirs_;

    void scan_dir(const std::string& norm_dir, const std::filesystem::path& real_dir) const;
};

/// Heuristic check that a folder looks like a Joan of Arc installation.
bool looks_like_game_dir(const std::filesystem::path& dir);

}  // namespace joan
