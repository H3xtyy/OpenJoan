// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] MissionCount (see mission_count.hpp).
#include "mission_count.hpp"

#include <cstdio>
#include <cstring>

#include "../original/direct.hpp"
#include "../platform/files.hpp"
#include "../platform/win32_types.hpp"
#include "../registry.hpp"

namespace jo = joan::orig;

namespace joan::mission_count {
namespace {

constexpr std::int32_t kOriginalLast = 8;  // the original's last mission (0x007226a0, the continue branch of 0x00722950)
constexpr std::int32_t kMaxMission = 99;

std::int32_t g_last = 0;  // 0 = not scanned yet

bool fix_on() { return joan::hook::settings().fix_mission_count; }

// SCENARIO\scenario_NN.SCR as Scenario::load 0x004ccfb0 builds it ("0" in front of a number below 10), looked up as
// Misc::is_file_exist 0x00590b80 does (FindFirstFileA, recorded by the replay).
bool scenario_file_exists(std::int32_t n) {
    char path[64];
    std::snprintf(path, sizeof path, "SCENARIO\\scenario_%02d.SCR", static_cast<int>(n));
    WIN32_FIND_DATAA fd;
    const HANDLE h = joan::platform::files::FindFirstFileA(path, &fd);
    if (h == INVALID_HANDLE_VALUE) return false;
    joan::platform::files::FindClose(h);
    return true;
}

}  // namespace

std::int32_t last_mission() {
    if (!fix_on()) return kOriginalLast;
    if (g_last == 0) {
        std::int32_t n = 0;
        while (n < kMaxMission && scenario_file_exists(n + 1)) ++n;
        g_last = n > 0 ? n : kOriginalLast;
    }
    return g_last;
}

std::uint32_t table_mission(std::uint32_t mission) {
    if (!fix_on()) return mission;
    return mission > static_cast<std::uint32_t>(kOriginalLast) ? static_cast<std::uint32_t>(kOriginalLast) : mission;
}

bool has_victory_text(void* strings, std::int32_t mission) {
    if (!fix_on()) return true;
    // The lookup of Strings::get 0x005a3090 (ilib/txtres.cpp): section name and key; the original reads the entry past
    // the end of the table when there is none.
    jo::TextRes* const table = jo::as_text_res(strings);
    for (std::int32_t i = 0; i < table->count; ++i) {
        jo::TxtEntry* const entry = &table->entries[i];
        if (std::strcmp(entry->name, "VICTS") == 0 && static_cast<std::int32_t>(entry->key) == mission) return true;
    }
    return false;
}

}  // namespace joan::mission_count
