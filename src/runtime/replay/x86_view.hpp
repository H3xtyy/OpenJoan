// The x86-layout view of joan_image for the replay's image hash (replay.cpp image_bytes; p4_x64_link.md 2.2 item 5).
// A recording and its replay compare hashes of .data + .bss in the ORIGINAL (x86) layout - byte offsets from
// 0x009f4000, 4-byte pointer fields, the typed table hash_types.inc in x86 offsets. On x64 joan_image has the x64
// layout (every stored pointer widened to 8 bytes, src/data/image_xlate.inc), so the hash first copies the hashed
// range into an x86-layout buffer: every original byte is read from the x64 location of the same field (the run of
// image_xlate.inc that holds its address), a widened pointer contributes its low 4 bytes (little endian: the low 32
// bits, all a 4-byte slot of the original layout can show). The hash then runs on that buffer as on x86, except that
// every widened pointer is normalised from its full value (for_each_wide_pointer; replay.cpp normalise_wide): its
// low half alone cannot be told apart from an address of the heap's or the image's 32-bit range. x86 builds hash joan_image itself (no view, no copy).
//
// Also the inverse mapping of an x64 image offset, for an x64 recording's address layout replayed by any build
// (replay.cpp normalise_in). Platform independent (no <windows.h>): re/replay/test_x86_view.cpp checks it on a host
// over every entry of hash_types.inc, src/replay/hash_types_check.cpp against the compiler's x64 layout.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

/// The x86-layout view of joan_image used by the replay image hash on x64 builds.
namespace joan::replay::x86_view {

inline constexpr std::uint32_t kImageVa = 0x009cf000u;  ///< original address of joan_image[0]
inline constexpr std::uint32_t kX86ImageSize = 0x000a1000u;  ///< bytes of joan_image in the original (x86) layout

/// One run of image_xlate.inc: original addresses va.. (up to the next run's va) lie at off + (address - va).
struct Run {
    std::uint32_t va /**< first original address of the run */, off /**< its offset in the x64 joan_image */;
};

// The x64 layout of joan_image (generated from both compiler layouts; the same file image.hpp uses on x64).
// (layout.hpp leaves its own JOAN_IMAGE_X64_SIZE defined on x64: saved and restored around the two includes)
#pragma push_macro("JOAN_IMAGE_X64_SIZE")
#pragma push_macro("JOAN_XLATE")
#undef JOAN_IMAGE_X64_SIZE
#undef JOAN_XLATE
#define JOAN_IMAGE_X64_SIZE(size) inline constexpr std::uint32_t kX64ImageSize = size;
#define JOAN_XLATE(va, off)
#include "../../data/image_xlate.inc"
#undef JOAN_XLATE
#undef JOAN_IMAGE_X64_SIZE
#define JOAN_XLATE(va, off) {va, off},
/// The runs of image_xlate.inc (generated) of the x64 layout of joan_image.
inline constexpr Run kX64Runs[] = {
#include "../../data/image_xlate.inc"
};
#undef JOAN_XLATE
#pragma pop_macro("JOAN_XLATE")
#pragma pop_macro("JOAN_IMAGE_X64_SIZE")
/// Number of entries of kX64Runs.
inline constexpr std::size_t kX64RunCount = sizeof kX64Runs / sizeof kX64Runs[0];

/// Index of the run that holds original address va (runs sorted by va, the first at the image start).
template <class R>
constexpr std::size_t run_of(const R* runs, std::size_t n, std::uint32_t va) noexcept {
    std::size_t lo = 0, hi = n;
    while (hi - lo > 1) {
        const std::size_t mid = (lo + hi) / 2;
        (runs[mid].va <= va ? lo : hi) = mid;
    }
    return lo;
}

/// x64 offset of original address va / original address of x64 offset off (as img::offset_of / original_of on x64).
template <class R>
constexpr std::uint32_t offset_of(const R* runs, std::size_t n, std::uint32_t va) noexcept {
    const std::size_t i = run_of(runs, n, va);
    return runs[i].off + (va - runs[i].va);
}
template <class R>
/// Original address at offset `off` of an image laid out by `runs` (inverse of offset_of(runs, n, va)).
constexpr std::uint32_t original_of(const R* runs, std::size_t n, std::uint32_t off) noexcept {
    std::size_t lo = 0, hi = n;
    while (hi - lo > 1) {
        const std::size_t mid = (lo + hi) / 2;
        (runs[mid].off <= off ? lo : hi) = mid;
    }
    return runs[lo].va + (off - runs[lo].off);
}
/// x64 offset of original address `va` (offset_of() over kX64Runs).
constexpr std::uint32_t x64_offset_of(std::uint32_t va) noexcept { return offset_of(kX64Runs, kX64RunCount, va); }
/// Original address at x64 offset `off` (original_of() over kX64Runs).
constexpr std::uint32_t x64_original_of(std::uint32_t off) noexcept { return original_of(kX64Runs, kX64RunCount, off); }

/// dst[0, size) = the bytes of original addresses [va, va + size) in the x86 layout, read from `image` (joan_image in
/// the layout of `runs`, its first byte at original address kImageVa). [va, va + size) must lie below the last run's
/// va (image_xlate.inc ends with a run at the image end). Each run is copied as one block of its x86 length: a run
/// ends after a pointer whose x64 slot is 4 bytes longer, and those upper 4 bytes are not part of the x86 view.
template <class R>
void copy(const R* runs, std::size_t n, const std::uint8_t* image, std::uint32_t va, std::uint32_t size,
          std::uint8_t* dst) noexcept {
    const std::uint32_t end = va + size;
    for (std::size_t i = run_of(runs, n, va); va < end && i < n; ++i) {
        const std::uint32_t run_end = i + 1 < n ? runs[i + 1].va : end;
        const std::uint32_t stop = run_end < end ? run_end : end;
        if (stop <= va) continue;
        std::memcpy(dst, image + runs[i].off + (va - runs[i].va), stop - va);
        dst += stop - va;
        va = stop;
    }
}

/// Calls f(va, value) for every widened pointer slot that lies wholly in [va, va + size): the slot's original address
/// and its full 8-byte value (a run that ends 4 bytes longer in the x64 layout than in the x86 one ends with a pointer).
template <class R, class F>
void for_each_wide_pointer(const R* runs, std::size_t n, const std::uint8_t* image, std::uint32_t va,
                           std::uint32_t size, F&& f) {
    const std::uint32_t end = va + size;
    for (std::size_t i = run_of(runs, n, va); i + 1 < n && runs[i].va < end; ++i) {
        const std::uint32_t len86 = runs[i + 1].va - runs[i].va, len64 = runs[i + 1].off - runs[i].off;
        if (len64 != len86 + 4 || len86 < 4) continue;
        const std::uint32_t slot = runs[i + 1].va - 4;
        if (slot < va || slot + 4 > end) continue;
        std::uint64_t value = 0;
        std::memcpy(&value, image + runs[i].off + len86 - 4, 8);
        f(slot, value);
    }
}

}  // namespace joan::replay::x86_view
