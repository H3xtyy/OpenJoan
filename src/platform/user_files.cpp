// SPDX-License-Identifier: GPL-3.0-or-later
// user_files.hpp.
#include "user_files.hpp"

#include <cstdlib>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

namespace joan::platform::user_files {

namespace {

fs::path g_folder;

bool can_write_here() {
    const fs::path probe = ".openjoan-write-test";
    bool ok = false;
    {
        std::ofstream file(probe, std::ios::binary | std::ios::trunc);
        ok = file.good();
        if (ok) {
            file.put('x');
            ok = file.good();
        }
    }
    std::error_code ec;
    fs::remove(probe, ec);
    return ok;
}

}  // namespace

fs::path users_folder() {
#ifdef _WIN32
    for (const char* name : {"LOCALAPPDATA", "APPDATA"})
        if (const char* value = std::getenv(name); value != nullptr && *value != '\0') return fs::path(value) / "OpenJoan";
#else
    if (const char* value = std::getenv("XDG_DATA_HOME"); value != nullptr && *value != '\0') return fs::path(value) / "openjoan";
    if (const char* value = std::getenv("HOME"); value != nullptr && *value != '\0') return fs::path(value) / ".local" / "share" / "openjoan";
#endif
    return {};
}

void detect() {
    if (can_write_here()) return;
    use_folder(users_folder());
}

void use_folder(const fs::path& folder) { g_folder = folder; }

bool active() { return !g_folder.empty(); }

const fs::path& folder() { return g_folder; }

fs::path for_read(const fs::path& game, const fs::path& user) {
    std::error_code ec;
    return fs::exists(user, ec) ? user : game;
}

fs::path for_write(const fs::path& game, const fs::path& user) {
    std::error_code ec;
    if (user.has_parent_path()) fs::create_directories(user.parent_path(), ec);
    if (!fs::exists(user, ec) && fs::is_regular_file(game, ec)) fs::copy_file(game, user, ec);
    return user;
}

}  // namespace joan::platform::user_files
