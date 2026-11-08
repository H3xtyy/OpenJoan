// Runtime side of the relocatable data image (docs/RUNTIME.md; image.hpp, loader/data_image.hpp):
//  - before any other static constructor of ours: structure check of the unrelocated joan_image and relocation of its
//    stored data pointers (results for the loader's log),
//  - `stray image address` logging for the raw overloads of image.hpp,
//  - old_range_mapped(): whether a value inside joan.exe's old address range is a pointer to something mapped there
//    now (stage S7d: nothing is reserved at the old addresses; with ASLR our own image, the heap, DLLs can lie there),
//    used by the raw-value stray checks of code and data,
//  - the stray guard: a vectored handler for access violations at joan.exe's old addresses 0x00400000..0x00a96000
//    where nothing is mapped (or, for a call / jump into the old code range, nothing executable: DEP). It logs the
//    faulting address (data symbol / code slot), the instruction or caller and the nearest replacement, then ends the
//    game. Since S7d nothing is reserved for it: it costs nothing until such a fault happens, and a stray access to
//    an old address where something of ours happens to be mapped is NOT caught (see S7_DESIGN.md 5).
#pragma once

#include <cstddef>
#include <cstdint>

#include "loader/data_image.hpp"

/// Run-time side of the relocatable data image: structure check, relocation and the stray-address guard.
namespace joan::image_runtime {

/// Result of the start-up step, kept for the loader's log.
struct Startup {
    bool ran = false;               ///< the step has run
    data_image::CheckResult check;  ///< structure check of the unrelocated image
    data_image::RelocResult relocs; ///< relocation of the stored data pointers
};
/// What the startup step did (it runs from a compiler-segment static initialiser, before main).
const Startup& startup();

/// Installs the vectored handler for faults in joan.exe's old address range.
void install_stray_guard();

/// joan.exe's old address range (header .. end of .reloc): values there are recognised as original addresses.
inline constexpr std::uintptr_t kOldLow = 0x00400000 /**< first address of the range */,
                                kOldHigh = 0x00a96000 /**< end (exclusive) of the range */;

/// v (inside the old range) points at memory that is mapped now: committed and accessible, and for `execute`
/// also executable. False outside the old range. Pages found mapped are remembered (later calls: one bit test); a
/// page found unmapped is asked again next time.
bool old_range_mapped(std::uintptr_t v, bool execute) noexcept;

/// While one of these lives on a thread, the stray guard ignores that thread's faults: a probe that reads an
/// arbitrary value under SEH (replay's safe_read_u32) handles its own access violations. A vectored handler runs
/// before the probe's __except, so without this a probe of a value in the old range whose page went away since the
/// probe's readable-range snapshot ended the game as a "stray code address" (R13 test32 replay: 0x005c004c).
struct ProbeScope {
    ProbeScope() noexcept;
    ~ProbeScope();
    ProbeScope(const ProbeScope&) = delete;
    ProbeScope& operator=(const ProbeScope&) = delete;
};

/// Number of distinct (address, caller) pairs stray_image_address() has seen.
std::size_t stray_count();

}  // namespace joan::image_runtime
