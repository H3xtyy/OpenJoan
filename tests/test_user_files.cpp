// SPDX-License-Identifier: GPL-3.0-or-later
// The user's folder for a game folder that cannot be written (src/platform/user_files.hpp).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "../src/platform/user_files.hpp"

namespace fs = std::filesystem;
using namespace joan::platform::user_files;

namespace {

int g_failures = 0;
void check(bool ok, const char* what) {
    if (!ok) {
        std::printf("FAILED: %s\n", what);
        ++g_failures;
    }
}

void write(const fs::path& p, const std::string& text) {
    fs::create_directories(p.parent_path());
    std::ofstream(p, std::ios::binary) << text;
}

std::string read(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(f), {});
}

}  // namespace

int main() {
    const fs::path base = fs::temp_directory_path() / "openjoan_user_files_test";
    fs::remove_all(base);
    const fs::path game = base / "game", user = base / "user";

    check(!active(), "a writable game folder uses no other folder");
    use_folder(user);
    check(active() && folder() == user, "the user's folder is in use");

    write(game / "SAVE" / "old.sav", "game");
    const fs::path game_file = game / "SAVE" / "old.sav", user_file = user / "SAVE" / "old.sav";
    check(for_read(game_file, user_file) == game_file, "a file only in the game folder is read from there");

    const fs::path target = for_write(game_file, user_file);
    check(target == user_file && read(user_file) == "game", "writing starts from a copy of the game's file");
    write(user_file, "user");
    check(for_read(game_file, user_file) == user_file, "the user's copy is read back");
    check(read(game_file) == "game", "the game folder is left alone");

    const fs::path fresh_game = game / "SAVE" / "Player.dat", fresh_user = user / "deep" / "SAVE" / "Player.dat";
    check(for_write(fresh_game, fresh_user) == fresh_user && fs::is_directory(fresh_user.parent_path()), "a new file gets its folders");

    use_folder({});
    check(!active(), "the folder can be switched off");
    fs::remove_all(base);
    if (g_failures == 0) std::printf("user_files: all checks passed\n");
    return g_failures == 0 ? 0 : 1;
}
