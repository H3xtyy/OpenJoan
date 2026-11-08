// [Audio] SoundsInMemory (see sound_archives.hpp).
//
// A long-name archive (ResourceIdxL, ilib/res_idx_impl.hpp): u32 count, then count + 1 index records
// { char name[34]; u32 offset; } - record i's data is [offset(i), offset(i + 1)) of the file. The original looks a
// name up by binary search over the names sorted with qsort; for a name that appears once that is simply the record
// of that name, which is all find() answers (a name found twice or not at all goes the original way, from disk).
#include "sound_archives.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../compiler.hpp"
#include "../platform/files.hpp"
#include "../registry.hpp"

namespace joan::sound_archives {
namespace {

constexpr std::size_t kNameBytes = 34;
constexpr std::size_t kRecordBytes = kNameBytes + 4;
constexpr const char* kWaveRes = "RESOURCE\\WAVE.RES";

struct Entry {
    const char* name;  // in Archive::bytes (NUL-terminated within its 34 bytes: checked)
    std::uint32_t offset, size;
};

struct Archive {
    std::string path;      // the game's relative path
    bool mission = false;  // a mission's voice archive (dropped by the next mission)
    std::atomic<bool> ready{false};
    bool logged = false;
    std::vector<char> bytes;
    std::vector<Entry> entries;  // sorted by name; names found twice removed
    double ms = 0;
};

std::vector<std::shared_ptr<Archive>> g_archives;

bool enabled() { return hook::settings().sounds_in_memory != 0; }

std::uint32_t read_u32(const char* p) {
    std::uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

// Reading thread: the whole file, then its index. Leaves `entries` empty when the file is not a well-formed archive.
void load(const std::shared_ptr<Archive>& a, const std::string& file) {
    const auto t0 = std::chrono::steady_clock::now();
    if (std::FILE* f = std::fopen(platform::files::native_path(file.c_str()).c_str(), "rb")) {
        std::fseek(f, 0, SEEK_END);
        const long n = std::ftell(f);
        std::fseek(f, 0, SEEK_SET);
        if (n > 4) {
            a->bytes.resize(static_cast<std::size_t>(n));
            if (std::fread(a->bytes.data(), 1, a->bytes.size(), f) != a->bytes.size()) a->bytes.clear();
        }
        std::fclose(f);
    }
    const std::size_t total = a->bytes.size();
    const std::uint32_t count = total > 4 ? read_u32(a->bytes.data()) : 0;
    bool ok = count > 0 && 4 + (static_cast<std::uint64_t>(count) + 1) * kRecordBytes <= total;
    for (std::uint32_t i = 0; ok && i < count; ++i) {
        const char* const rec = a->bytes.data() + 4 + i * kRecordBytes;
        const std::uint32_t from = read_u32(rec + kNameBytes), to = read_u32(rec + kRecordBytes + kNameBytes);
        if (std::memchr(rec, 0, kNameBytes) == nullptr || from > to || to > total) {
            ok = false;
            break;
        }
        a->entries.push_back({rec, from, to - from});
    }
    if (ok) {
        auto less = [](const Entry& x, const Entry& y) { return std::strcmp(x.name, y.name) < 0; };
        std::sort(a->entries.begin(), a->entries.end(), less);
        std::vector<Entry> unique;
        for (std::size_t i = 0; i < a->entries.size();) {
            std::size_t j = i + 1;
            while (j < a->entries.size() && std::strcmp(a->entries[j].name, a->entries[i].name) == 0) ++j;
            if (j == i + 1) unique.push_back(a->entries[i]);
            i = j;
        }
        a->entries = std::move(unique);
    } else {
        a->entries.clear();
        a->bytes.clear();
    }
    a->ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    a->ready.store(true, std::memory_order_release);
}

void start_reading(const char* path, bool mission) {
    for (const auto& a : g_archives)
        if (_stricmp(a->path.c_str(), path) == 0) return;
    auto a = std::make_shared<Archive>();
    a->path = path;
    a->mission = mission;
    g_archives.push_back(a);
    // The thread keeps its own reference: an archive dropped while it is still read is freed when the read ends.
    std::thread(load, a, hook::data_dir() + path).detach();
}

// The MUSIC folder: every file in it, by lower-case name.
struct MusicFolder {
    std::atomic<bool> ready{false};
    bool logged = false;
    std::vector<std::pair<std::string, std::shared_ptr<const std::vector<std::uint8_t>>>> files;  // sorted by name
    std::size_t bytes = 0;
    double ms = 0;
};

MusicFolder* g_music = nullptr;  // never freed: its reading thread may still run at exit

std::string lower(std::string s) {
    for (char& c : s)
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    return s;
}

void load_music(MusicFolder* f, const std::string& dir) {
    const auto t0 = std::chrono::steady_clock::now();
    std::error_code ec;
    for (const auto& e : std::filesystem::directory_iterator(platform::files::native_path(dir.c_str()), ec)) {
        if (!e.is_regular_file(ec)) continue;
        std::ifstream file(e.path(), std::ios::binary);  // (a path, not a narrow name: any characters on Windows)
        const auto n = e.file_size(ec);
        if (!file || ec || n == 0) continue;
        auto bytes = std::make_shared<std::vector<std::uint8_t>>(static_cast<std::size_t>(n));
        if (!file.read(reinterpret_cast<char*>(bytes->data()), static_cast<std::streamsize>(n))) bytes->clear();
        if (bytes->empty()) continue;
        f->bytes += bytes->size();
        f->files.emplace_back(lower(e.path().filename().string()), std::move(bytes));
    }
    std::sort(f->files.begin(), f->files.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    f->ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    f->ready.store(true, std::memory_order_release);
}

}  // namespace

void start() {
    if (!enabled()) return;
    start_reading(kWaveRes, false);
    g_music = new MusicFolder;
    std::thread(load_music, g_music, hook::data_dir() + "MUSIC").detach();
}

std::shared_ptr<const std::vector<std::uint8_t>> music_file(const char* path) {
    if (g_music == nullptr || path == nullptr || !g_music->ready.load(std::memory_order_acquire)) return nullptr;
    if (!g_music->logged) {
        g_music->logged = true;
        hook::log_line("[Audio] SoundsInMemory: MUSIC in memory (%.1f MB, %zu files, read in %.0f ms)",
                       static_cast<double>(g_music->bytes) / 1048576.0, g_music->files.size(), g_music->ms);
    }
    // "MUSIC\name": the folder, then a name without further folders.
    if (_strnicmp(path, "MUSIC", 5) != 0 || (path[5] != '\\' && path[5] != '/')) return nullptr;
    const char* const name = path + 6;
    if (std::strpbrk(name, "\\/") != nullptr) return nullptr;
    const std::string key = lower(name);
    const auto it = std::lower_bound(g_music->files.begin(), g_music->files.end(), key,
                                     [](const auto& e, const std::string& k) { return e.first < k; });
    if (it == g_music->files.end() || it->first != key) return nullptr;
    return it->second;
}

void mission_archive(const char* path, bool fresh) {
    if (!enabled() || path == nullptr) return;
    if (fresh)
        g_archives.erase(std::remove_if(g_archives.begin(), g_archives.end(),
                                        [&](const std::shared_ptr<Archive>& a) {
                                            return a->mission && _stricmp(a->path.c_str(), path) != 0;
                                        }),
                         g_archives.end());
    start_reading(path, true);
}

const char* find(const char* path, const char* name, std::uint32_t& size) {
    if (path == nullptr || name == nullptr) return nullptr;
    for (const auto& a : g_archives) {
        if (_stricmp(a->path.c_str(), path) != 0) continue;
        if (!a->ready.load(std::memory_order_acquire)) return nullptr;
        if (!a->logged) {
            a->logged = true;
            if (a->entries.empty())
                hook::log_line("[Audio] SoundsInMemory: %s could not be read as an archive - its sounds load from disk",
                               a->path.c_str());
            else
                hook::log_line("[Audio] SoundsInMemory: %s in memory (%.1f MB, %zu sounds, read in %.0f ms)",
                               a->path.c_str(), static_cast<double>(a->bytes.size()) / 1048576.0, a->entries.size(), a->ms);
        }
        const auto it = std::lower_bound(a->entries.begin(), a->entries.end(), name,
                                         [](const Entry& e, const char* n) { return std::strcmp(e.name, n) < 0; });
        if (it == a->entries.end() || std::strcmp(it->name, name) != 0) return nullptr;
        size = it->size;
        return a->bytes.data() + it->offset;
    }
    return nullptr;
}

}  // namespace joan::sound_archives
