// Standalone executable entry point.
//
// Status: bootstrap. It locates the original game data, loads the core data
// sets through the reimplemented ilib layer and opens an SDL3/OpenGL window.
// Game systems are moved over from joan.exe module by module (see
// docs/ROADMAP.md); until then the original game is played through the hook
// DLL (src/), which swaps reimplemented functions into joan.exe.
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>  // provides WinMain on Windows GUI builds

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <optional>
#include <string>

#include "core/log.hpp"
#include "core/vfs.hpp"
#include "ilib/game_set.hpp"
#include "platform/window.hpp"
#include "render/gl.hpp"

using namespace joan;

namespace {

std::optional<std::filesystem::path> find_game_dir(int argc, char** argv) {
    for (int i = 1; i + 1 < argc; ++i)
        if (std::string(argv[i]) == "--game-dir") return std::filesystem::path(argv[i + 1]);
    if (const char* env = std::getenv("JOAN_GAME_DIR")) return std::filesystem::path(env);
    // Default: the executable sits next to the original joan.exe.
    const std::filesystem::path exe_dir = platform::executable_dir();
    if (looks_like_game_dir(exe_dir)) return exe_dir;
    if (looks_like_game_dir(std::filesystem::current_path())) return std::filesystem::current_path();
    return std::nullopt;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const auto game_dir = find_game_dir(argc, argv);
        if (!game_dir || !looks_like_game_dir(*game_dir)) {
            log::error("Joan of Arc game data not found. Pass --game-dir <folder containing joan.exe>, "
                       "set JOAN_GAME_DIR, or place this executable in the game folder.");
            return 1;
        }
        Vfs vfs(*game_dir);
        log::info("game data: {}", game_dir->string());

        const auto std_set = ilib::GameSet::open(vfs, "Resource/STD.SET");
        const auto ui_set = ilib::GameSet::open(vfs, "Resource/INTERFAC.SET");
        log::info("loaded {} ({}, {} tables) and {} ({}, {} tables)", std_set.code(), std_set.description(),
                  std_set.archive().size(), ui_set.code(), ui_set.description(), ui_set.archive().size());

        platform::Window window(platform::WindowConfig{});
        const char* missing = nullptr;
        if (!gl::load([](const char* n) { return reinterpret_cast<void*>(SDL_GL_GetProcAddress(n)); }, &missing)) {
            log::error("OpenGL function missing: {}", missing ? missing : "?");
            return 1;
        }
        log::info("OpenGL {} on {}", reinterpret_cast<const char*>(gl::GetString(gl::VERSION)),
                  reinterpret_cast<const char*>(gl::GetString(gl::RENDERER)));

        while (window.poll_events()) {
            int w = 0, h = 0;
            window.drawable_size(w, h);
            gl::Viewport(0, 0, w, h);
            gl::ClearColor(0.08f, 0.07f, 0.06f, 1.0f);
            gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
            window.swap();
        }
        return 0;
    } catch (const std::exception& e) {
        log::error("fatal: {}", e.what());
        return 1;
    }
}
