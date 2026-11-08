#include "en3d/asset_paths.hpp"

#include <cctype>
#include <system_error>

namespace joan::en3d {

namespace fs = std::filesystem;

namespace {

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

std::optional<fs::path> find_in_dir(const fs::path& dir, const std::string& stem, const std::string& ext_lower) {
    std::error_code ec;
    for (const auto& e : fs::directory_iterator(dir, ec)) {
        if (!e.is_regular_file(ec)) continue;
        const fs::path& p = e.path();
        if (lower(p.stem().string()) == stem && lower(p.extension().string()) == ext_lower) return p;
    }
    return std::nullopt;
}

}  // namespace

std::string stem_lower(std::string_view path) {
    std::string s(path);
    if (const auto slash = s.find_last_of("/\\"); slash != std::string::npos) s.erase(0, slash + 1);
    if (const auto dot = s.rfind('.'); dot != std::string::npos) s.erase(dot);
    return lower(s);
}

TextureIndex::TextureIndex(const fs::path& game_dir) {
    std::error_code ec;
    fs::path root;
    for (const auto& e : fs::directory_iterator(game_dir, ec))
        if (e.is_directory(ec) && lower(e.path().filename().string()) == "texture") root = e.path();
    if (root.empty()) return;
    for (auto it = fs::recursive_directory_iterator(root, ec); !ec && it != fs::recursive_directory_iterator();
         it.increment(ec)) {
        if (!it->is_regular_file(ec) || lower(it->path().extension().string()) != ".dds") continue;
        // Keep the shallowest match if a name occurs twice.
        by_stem_.try_emplace(lower(it->path().stem().string()), it->path());
    }
}

std::optional<fs::path> TextureIndex::find(std::string_view texture_name) const {
    if (auto it = by_stem_.find(stem_lower(texture_name)); it != by_stem_.end()) return it->second;
    return std::nullopt;
}

std::optional<fs::path> find_sibling(const fs::path& model_file, std::string_view extension) {
    return find_in_dir(model_file.parent_path(), lower(model_file.stem().string()), lower(std::string(extension)));
}

std::optional<fs::path> find_animation(const fs::path& game_dir, const fs::path& model_file) {
    const std::string stem = lower(model_file.stem().string());
    if (auto p = find_in_dir(model_file.parent_path(), stem, ".am2")) return p;
    std::error_code ec;
    for (const auto& e : fs::directory_iterator(game_dir, ec))
        if (e.is_directory(ec) && lower(e.path().filename().string()) == "anim")
            if (auto p = find_in_dir(e.path(), stem, ".am2")) return p;
    return std::nullopt;
}

}  // namespace joan::en3d
