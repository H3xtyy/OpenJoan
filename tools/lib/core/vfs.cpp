#include "core/vfs.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <system_error>

#include "core/file_util.hpp"

namespace joan {

Vfs::Vfs(std::filesystem::path root) : root_(std::move(root)) {
    std::error_code ec;
    if (!std::filesystem::is_directory(root_, ec))
        throw std::runtime_error("game directory does not exist: " + root_.string());
}

std::string Vfs::normalise(std::string_view game_path) {
    std::string out;
    out.reserve(game_path.size());
    for (char c : game_path) {
        if (c == '\\') c = '/';
        if (c == '/' && (out.empty() || out.back() == '/')) continue;  // collapse "//" and leading '/'
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    while (out.starts_with("./")) out.erase(0, 2);
    if (!out.empty() && out.back() == '/') out.pop_back();
    return out;
}

void Vfs::scan_dir(const std::string& norm_dir, const std::filesystem::path& real_dir) const {
    if (scanned_dirs_[norm_dir]) return;
    scanned_dirs_[norm_dir] = true;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(real_dir, ec)) {
        std::string name = normalise(entry.path().filename().string());
        std::string key = norm_dir.empty() ? name : norm_dir + "/" + name;
        // Keep the first match if two entries differ only in case.
        cache_.emplace(std::move(key), entry.path());
    }
}

std::optional<std::filesystem::path> Vfs::resolve(std::string_view game_path) const {
    const std::string norm = normalise(game_path);
    if (norm.empty()) return root_;
    if (auto it = cache_.find(norm); it != cache_.end()) return it->second;

    // Walk the path component by component, scanning each directory once.
    std::string prefix;
    std::filesystem::path real = root_;
    std::size_t start = 0;
    while (start <= norm.size()) {
        const std::size_t slash = norm.find('/', start);
        const std::string part = norm.substr(start, slash == std::string::npos ? std::string::npos : slash - start);
        scan_dir(prefix, real);
        const std::string key = prefix.empty() ? part : prefix + "/" + part;
        auto it = cache_.find(key);
        if (it == cache_.end()) return std::nullopt;
        real = it->second;
        prefix = key;
        if (slash == std::string::npos) break;
        start = slash + 1;
    }
    return real;
}

std::vector<std::byte> Vfs::read(std::string_view game_path) const {
    auto path = resolve(game_path);
    if (!path) throw std::runtime_error("game file not found: " + std::string(game_path));
    return read_file(*path);
}

bool looks_like_game_dir(const std::filesystem::path& dir) {
    try {
        Vfs vfs(dir);
        return vfs.exists("Resource/STD.SET") && vfs.exists("Resource/RESOURCE");
    } catch (const std::exception&) {
        return false;
    }
}

}  // namespace joan
