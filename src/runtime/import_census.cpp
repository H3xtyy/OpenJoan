// [Debug] ImportCensus=1: how often the game calls each of joan.exe's imports during a session (the work list of
// phase C5, the remaining Windows calls - src/platform/README.md). Every IAT slot gets a 16-byte thunk
//     lock inc dword ptr [counter] ; jmp dword ptr [target]
// in front of whatever the slot held (the Windows function, one of our overrides, or the replay layer's wrapper), so
// calling conventions and results are untouched. At exit openjoan_imports.txt (next to openjoan.log) lists every
// import with its call count, by DLL, the never-called ones last.
#include "import_census.hpp"

#include "../platform/win32_types.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "image.hpp"
#include "loader/data_image.hpp"
#include "../registry.hpp"
#include "../platform/ini.hpp"
#include "../platform/memory.hpp"

namespace joan::import_census {
namespace {

struct Entry {
    const char* dll;
    const char* name;
    std::uint16_t ordinal;
    std::uintptr_t* slot;
};

std::vector<Entry> g_entries;
volatile LONG* g_counts = nullptr;  // one counter per entry
std::uintptr_t* g_targets = nullptr;
bool g_on = false;

}  // namespace

void install() {
    if constexpr (joan::kReleaseBuild) return;  // a debug-build tool (build_flavor.hpp)
    const std::string ini = joan::hook::ini_path();
    if (joan::platform::ini::get_int("Debug", "ImportCensus", 0, ini.c_str()) == 0) return;
    namespace di = joan::data_image;
    const auto dlls = di::import_dlls();
    for (const di::Import& im : di::imports()) {
        auto* slot = &img::global<std::uintptr_t>(img::Addr(im.slot));
        if (*slot == 0) continue;  // filled later (DirectInput8Create on first use): not counted
        g_entries.push_back({dlls[im.dll], im.name, im.ordinal, slot});
    }
    const std::size_t n = g_entries.size();
    constexpr std::size_t kThunk = 16;
    // Block layout: n thunks, n LONG counters, n target pointers (aligned for std::uintptr_t). One block, so on x64
    // every counter / target is within rel32 reach of its thunk.
    const std::size_t counts_off = n * kThunk;
    const std::size_t targets_off =
        counts_off + ((n * sizeof(LONG) + alignof(std::uintptr_t) - 1) & ~(alignof(std::uintptr_t) - 1));
    auto* mem = static_cast<std::uint8_t*>(
        joan::platform::memory::allocate(targets_off + n * sizeof(std::uintptr_t), joan::platform::memory::kAll));
    if (!mem) {
        joan::hook::log_line("ImportCensus: could not allocate the counting thunks");
        return;
    }
    g_counts = reinterpret_cast<volatile LONG*>(mem + counts_off);
    g_targets = reinterpret_cast<std::uintptr_t*>(mem + targets_off);
    for (std::size_t i = 0; i < n; ++i) {
        std::uint8_t* t = mem + i * kThunk;
        g_counts[i] = 0;
        g_targets[i] = *g_entries[i].slot;
#if defined(_M_X64) || defined(__x86_64__)
        // x64: ModRM 05 is [rip + disp32], relative to the end of each instruction (t + 7 / t + 13).
        const auto rel = [](const volatile void* to, const std::uint8_t* next) {
            return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(to) - reinterpret_cast<std::intptr_t>(next));
        };
        const std::int32_t counter = rel(&g_counts[i], t + 7);
        const std::int32_t target = rel(&g_targets[i], t + 13);
#else
        // i686: ModRM 05 is an absolute [disp32], the address itself (4 bytes on this target).
        static_assert(sizeof(void*) == 4);
        const auto counter = std::bit_cast<std::uint32_t>(&g_counts[i]);
        const auto target = std::bit_cast<std::uint32_t>(&g_targets[i]);
#endif
        t[0] = 0xF0;  // lock
        t[1] = 0xFF;  // inc dword ptr [imm32]
        t[2] = 0x05;
        std::memcpy(t + 3, &counter, 4);
        t[7] = 0xFF;  // jmp [imm32] (x64: jmp qword ptr [rip + disp32])
        t[8] = 0x25;
        std::memcpy(t + 9, &target, 4);
        t[13] = t[14] = t[15] = 0xCC;
        const joan::platform::memory::Writable writable(g_entries[i].slot, sizeof(std::uintptr_t));
        *g_entries[i].slot = reinterpret_cast<std::uintptr_t>(t);
    }
    joan::platform::memory::flush_code(mem, n * kThunk);
    g_on = true;
    joan::hook::log_line("ImportCensus: %u imports counted (report: openjoan_imports.txt at exit)", static_cast<unsigned>(n));
}

void write_report() {
    if (!g_on) return;
    g_on = false;
    const std::string path = joan::hook::data_dir() + "openjoan_imports.txt";
    FILE* f = std::fopen(path.c_str(), "w");
    if (!f) return;
    std::fprintf(f, "# joan.exe imports called during this session ([Debug] ImportCensus=1)\n");
    std::fprintf(f, "# dll  calls  import\n");
    std::vector<std::size_t> order(g_entries.size());
    for (std::size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::stable_sort(order.begin(), order.end(), [](std::size_t a, std::size_t b) {
        const int c = _stricmp(g_entries[a].dll, g_entries[b].dll);
        if (c != 0) return c < 0;
        return g_counts[a] > g_counts[b];
    });
    unsigned called = 0;
    for (std::size_t i : order) {
        const Entry& e = g_entries[i];
        const long c = g_counts[i];
        if (c) ++called;
        if (e.name)
            std::fprintf(f, "%-14s %10ld  %s\n", e.dll, c, e.name);
        else
            std::fprintf(f, "%-14s %10ld  #%u\n", e.dll, c, static_cast<unsigned>(e.ordinal));
    }
    std::fprintf(f, "# %u of %u imports called\n", called, static_cast<unsigned>(g_entries.size()));
    std::fclose(f);
    joan::hook::log_line("ImportCensus: %u of %u imports called - %s", called, static_cast<unsigned>(g_entries.size()),
                         path.c_str());
}

}  // namespace joan::import_census
