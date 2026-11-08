// Start / stop of the reimplementation inside openjoan.exe (loader/joan_re.cpp links all of this code directly).
// (Until stage S7b the same code also ran as a dinput8.dll proxy inside the original joan.exe.)
#include "host.hpp"

#include <cstdint>
#include <string>

#include "code_table.hpp"
#include "../fixes.hpp"
#include "image.hpp"
#include "import_census.hpp"
#include "crash.hpp"
#include "../display.hpp"
#include "../ilib/sound_archives.hpp"
#include "../platform/files.hpp"
#include "../platform/window.hpp"
#include "../registry.hpp"
#include "replay/replay.hpp"

namespace joan::host {

void start(const std::string& dir) {
    const std::filesystem::path ini = joan::platform::settings_path(dir);
    joan::hook::load_settings(ini.string());
    joan::hook::log_line("---- OpenJoan loaded (no joan.exe code or file; data image at 0x%08llx, code slots at 0x%08llx)",
                         static_cast<unsigned long long>(joan::img::base()),
                         static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(joan_code)));
    if (ini.filename() == joan::platform::kSettingsFallbackName)
        joan::hook::log_line("settings: %s read as the fallback (the name before the Phase 1 rename) - rename it %s",
                             joan::platform::kSettingsFallbackName, joan::platform::kSettingsName);
    joan::hook::install_crash_handler();
    // [Audio] SoundsInMemory: WAVE.RES read on its own thread while the game starts.
    joan::sound_archives::start();
    // [Platform] FilesInMemory: the item / particle / item-script folders read on their own thread likewise.
    joan::platform::files::start_memory_files();
    joan::code_table::begin();
    const int n = joan::hook::install_all();
    joan::hook::log_line("%d of %u replacements installed", n, static_cast<unsigned>(joan::hook::replacements().size()));
    joan::fixes::install();
    joan::display::apply_patches();
    joan::display::install_canvas_hooks();
    // [Debug] Record / Replay: wraps the game's imports (after every other IAT change above; src/replay/replay.hpp).
    joan::replay::install();
    // [Debug] ImportCensus: counting thunks in front of every import slot (after the replay wrappers).
    joan::import_census::install();
    // Last: every slot is final now (replacements, fixes, canvas wrappers). Resolves the code table, runs the
    // JOAN_LATE_CODE variables; nothing may retarget a slot afterwards.
    joan::code_table::seal();
}

void shutdown() {
    static bool done = false;
    if (done) return;
    done = true;
    joan::hook::write_counter_report();
    joan::import_census::write_report();
    joan::code_table::write_report();
}

}  // namespace joan::host
