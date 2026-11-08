// See anim_cache.hpp.
//
// A cache file (ANIM_TMP\<directory + name, lower case, separators as '_'>.jac):
//   "JOANAC1\0", u64 hash, u32 track count, u32 key count per track, then every track's 32-byte keys.
// The hash (FNV-1a, 64 bits) covers everything make_relative's result depends on: the x87 precision, each bone's
// parent (the .AM3's tree), the track and key counts and the keys as read.
#include "anim_cache.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

#include "../compiler.hpp"
#include "../fp/x87.hpp"
#include "../original/direct.hpp"
#include "../original/layout_access.hpp"
#include "../platform/files.hpp"
#include "../registry.hpp"

namespace jo = joan::orig;
namespace fs = std::filesystem;

namespace joan::anim_cache {
namespace {

constexpr char kMagic[8] = {'J', 'O', 'A', 'N', 'A', 'C', '1', '\0'};
constexpr std::size_t kKeyBytes = 32;

bool enabled() { return hook::settings().anim_cache != 0; }

struct Stats {
    unsigned restored = 0, stored = 0;
} g_stats;

fs::path cache_dir() { return fs::path(platform::files::native_path((hook::data_dir() + "ANIM_TMP").c_str())); }

fs::path cache_file(const char* directory, const char* name) {
    std::string key = std::string(directory ? directory : "") + (name ? name : "");
    for (char& c : key) {
        if (c == '\\' || c == '/' || c == ':') c = '_';
        else if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    return cache_dir() / (key + ".jac");
}

void mix(std::uint64_t& h, const void* p, std::size_t n) {
    const auto* b = static_cast<const std::uint8_t*>(p);
    for (std::size_t i = 0; i < n; ++i) h = (h ^ b[i]) * 0x100000001b3ull;
}

std::uint64_t keys_hash(const jo::AnimDef& def) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    const auto precision = static_cast<std::uint8_t>(fp::current_precision());
    mix(h, &precision, 1);
    mix(h, &def.track_count, sizeof def.track_count);
    for (std::int32_t i = 0; i < def.track_count; ++i) {
        const std::int32_t parent = jo::as_bone_tree(def.bones)->parent(i);  // 0x005b9fe0
        mix(h, &parent, sizeof parent);
        const jo::Track& t = def.tracks[i];
        mix(h, &t.key_count, sizeof t.key_count);
        if (t.key_count > 0 && t.keys != nullptr) mix(h, t.keys, static_cast<std::size_t>(t.key_count) * kKeyBytes);
    }
    return h;
}

bool usable(const jo::AnimDef& def) { return def.track_count > 0 && def.tracks != nullptr && def.bones != nullptr; }

}  // namespace

bool restore(void* anim_def, const char* directory, const char* name, std::uint64_t& hash) {
    hash = 0;
    if (!enabled()) return false;
    jo::AnimDef& def = *jo::as_anim_def(anim_def);
    // Already resolved: resolve_bones changes nothing then, so there is nothing to cache.
    if (!usable(def) || def.bones_resolved != 0) return false;
    hash = keys_hash(def);
    std::ifstream in(cache_file(directory, name), std::ios::binary);
    if (!in) return false;
    char magic[8];
    std::uint64_t stored_hash = 0;
    std::uint32_t tracks = 0;
    if (!in.read(magic, 8) || std::memcmp(magic, kMagic, 8) != 0) return false;
    if (!in.read(reinterpret_cast<char*>(&stored_hash), 8) || stored_hash != hash) return false;
    if (!in.read(reinterpret_cast<char*>(&tracks), 4) || tracks != static_cast<std::uint32_t>(def.track_count)) return false;
    std::vector<std::uint32_t> counts(tracks);
    if (!in.read(reinterpret_cast<char*>(counts.data()), static_cast<std::streamsize>(tracks * 4u))) return false;
    for (std::uint32_t i = 0; i < tracks; ++i)
        if (counts[i] != static_cast<std::uint32_t>(std::max(def.tracks[i].key_count, 0))) return false;
    // Read whole before any key is written: a short file leaves the animation as read (then converted as usual).
    std::size_t total = 0;
    for (std::uint32_t c : counts) total += c * kKeyBytes;
    std::vector<char> keys(total);
    if (total != 0 && !in.read(keys.data(), static_cast<std::streamsize>(total))) return false;
    std::size_t at = 0;
    for (std::uint32_t i = 0; i < tracks; ++i) {
        const std::size_t n = counts[i] * kKeyBytes;
        if (n != 0) std::memcpy(def.tracks[i].keys, keys.data() + at, n);
        at += n;
    }
    // The rest of what resolve_bones (0x005b6a90) does: ActorDef links its model's bones to the animation only
    // when this is set (0x005c56d0).
    def.bones_resolved = 1;
    ++g_stats.restored;
    return true;
}

void store(void* anim_def, const char* directory, const char* name, std::uint64_t hash) {
    if (!enabled() || hash == 0) return;
    const jo::AnimDef& def = *jo::as_anim_def(anim_def);
    if (!usable(def)) return;
    std::error_code ec;
    fs::create_directories(cache_dir(), ec);
    const fs::path file = cache_file(directory, name);
    const fs::path temp = fs::path(file).concat(".tmp");
    {
        std::ofstream out(temp, std::ios::binary | std::ios::trunc);
        if (!out) return;
        const std::uint32_t tracks = static_cast<std::uint32_t>(def.track_count);
        out.write(kMagic, 8);
        out.write(reinterpret_cast<const char*>(&hash), 8);
        out.write(reinterpret_cast<const char*>(&tracks), 4);
        for (std::uint32_t i = 0; i < tracks; ++i) {
            const std::uint32_t n = static_cast<std::uint32_t>(std::max(def.tracks[i].key_count, 0));
            out.write(reinterpret_cast<const char*>(&n), 4);
        }
        for (std::uint32_t i = 0; i < tracks; ++i) {
            const jo::Track& t = def.tracks[i];
            if (t.key_count > 0 && t.keys != nullptr)
                out.write(reinterpret_cast<const char*>(t.keys), static_cast<std::streamsize>(t.key_count) * static_cast<std::streamsize>(kKeyBytes));
        }
        if (!out) {
            out.close();
            fs::remove(temp, ec);
            return;
        }
    }
    fs::rename(temp, file, ec);  // whole files only: a crash mid-write leaves no half cache file
    if (ec) fs::remove(temp, ec);
    else ++g_stats.stored;
}

void build_all() {
    if (!enabled()) return;
    const auto t0 = std::chrono::steady_clock::now();
    const std::string root = hook::data_dir();
    const fs::path base(platform::files::native_path(root.c_str()));
    unsigned found = 0, built = 0;
    const Stats before = g_stats;
    std::error_code ec;
    for (const char* top : {"ANIM", "ITEM", "UNIT"}) {
        const fs::path dir(platform::files::native_path((root + top).c_str()));
        for (fs::recursive_directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
            if (!it->is_regular_file(ec)) continue;
            const fs::path& p = it->path();
            std::string ext = p.extension().string();
            for (char& c : ext) c = static_cast<char>(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
            if (ext != ".AM2") continue;
            // Only an animation with a bone tree (.AM3 beside it, any case) is converted.
            bool has_am3 = false;
            for (const auto& s : fs::directory_iterator(p.parent_path(), ec)) {
                std::string n = s.path().filename().string(), want = p.stem().string() + ".am3";
                if (n.size() == want.size() && _stricmp(n.c_str(), want.c_str()) == 0) {
                    has_am3 = true;
                    break;
                }
            }
            ec.clear();
            if (!has_am3) continue;
            ++found;
            // The game's own name for it: the folder below the game folder with backslashes and a trailing one.
            std::string rel = fs::relative(p.parent_path(), base, ec).string();
            if (ec) {
                ec.clear();
                continue;
            }
            std::replace(rel.begin(), rel.end(), '/', '\\');
            rel += '\\';
            const std::string stem = p.stem().string();
            if (fs::exists(cache_file(rel.c_str(), stem.c_str()), ec)) continue;
            // As the original's own scan_dir (0x0047cf60, the dev build of its .AMT cache) loads each animation.
            alignas(alignof(void*)) static std::uint8_t loader[sizeof(jo::AnimDef)];
            jo::as_anim_def(loader)->construct();                                   // 0x005b2390
            jo::as_anim_def(loader)->load(rel.c_str(), stem.c_str(), 0u);          // 0x005b23d0 (stores the cache)
            jo::as_anim_def(loader)->release(0u);                                   // 0x005b4170
            jo::as_anim_def(loader)->destruct();                                    // 0x00418c30
            ++built;
        }
        ec.clear();
    }
    hook::log_line("[Game] AnimCache: %u animations with bone trees, %u converted now (%u stored) in %.0f ms; cache in "
                   "ANIM_TMP\\",
                   found, built, g_stats.stored - before.stored,
                   std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count());
}

}  // namespace joan::anim_cache
