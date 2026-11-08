// joan.exe's data image (.rdata | .data | .bss | .idata, original VA 0x009cf000..0x00a70000) as seen by our C++
// (docs/RUNTIME.md). Every data address is a typed constant of src/data_names.hpp (`d::Name`,
// img::Sym<T>) or an offset from one (`d::Name + 0x10`, img::Addr); the helpers below turn it into a pointer at the
// moment it is used. No implicit conversion to or from integers: a data address used as a number is a compile error
// and has to say so (`addr(d::X)`).
//
// Stage S7b-main (now): the image is one object of openjoan.exe, `joan_image` (src/data/image.cpp, generated), and
// base() is its address: `ref(d::X)` compiles to `[joan_image + (X - kVa)]` with a base relocation, as cheap as the
// old absolute address. Inline asm names the same object (`fld dword ptr [joan_image + 0x794]`, re/asm_rebase.py).
// Pointers stored inside the image (data_relocs.inc) are relocated at startup before anything reads them
// (src/loader/data_image.cpp, image_runtime.cpp). Stage S7d: openjoan.exe is ASLR-relocated and nothing is reserved
// at the original range 0x009cf000..0x00a70000 any more - joan_image itself, the heap or a DLL may lie there. A
// missed absolute access faults (and is logged by image_runtime's guard) only where nothing is mapped. Every test of
// a value against the original range below is therefore applied to values that are ORIGINAL addresses by contract
// (names, save files), or, for the raw overloads, decided at run time by what is mapped there.
//
// No Windows headers: the Linux harness (re/data_image_harness.cpp) uses this file too.
#pragma once

#include <cstdint>
#include <cstring>

#include "code.hpp"

// Original (x86) layout: offset = address - kVa. 64-bit build (docs/X64.md image): the stored pointers of the
// image are 8 bytes, so its layout is the compiler's x64 layout of src/data/layout.hpp and an original address maps
// to its offset through the runs of src/data/image_xlate.inc (generated from both layouts).
#if UINTPTR_MAX == 0xffffffffu
#define JOAN_IMAGE_X64 0
#define JOAN_IMAGE_SIZE (0x00a70000 - 0x009cf000)
#else
#define JOAN_IMAGE_X64 1
#ifdef JOAN_IMAGE_LAYOUT_PROBE  // re/image/gen_xlate.py reading the layout that image_xlate.inc is made from
#define JOAN_XLATE_FILE "data/image_xlate_probe.inc"
#else
#define JOAN_XLATE_FILE "data/image_xlate.inc"
#endif
#define JOAN_IMAGE_X64_SIZE(size) inline constexpr std::uint32_t kImageX64Size = size;
/// The data image joan_image as seen by C++: typed data addresses and the helpers that turn them into pointers.
namespace joan::img {
#include JOAN_XLATE_FILE
}  // namespace joan::img
#undef JOAN_IMAGE_X64_SIZE
#define JOAN_IMAGE_SIZE (::joan::img::kImageX64Size)
#endif

// Global namespace and C linkage: MSVC inline asm cannot use qualified names. The size is part of the declaration:
// inline asm needs a complete type (clang rejects an operand of incomplete array type). Defined in
// src/data/image.cpp as joan_image_typed (the typed layout); /alternatename maps this name to it.
extern "C" unsigned char joan_image[JOAN_IMAGE_SIZE];
// The original address of joan_image[0] for inline asm, which cannot use img::kVa: `[joan_image + slot - JOAN_IMAGE_VA]`
// for an operand given as an original address (lib/imports/import_stubs.cpp's import stubs).
#define JOAN_IMAGE_VA 0x009cf000

namespace joan::img {

inline constexpr std::uint32_t kVa = JOAN_IMAGE_VA;  ///< original VA of joan_image[0] (.rdata start)
inline constexpr std::uint32_t kEnd = 0x00a70000;  ///< end of .idata (rdata | data | bss | idata)
inline constexpr std::uint32_t kSize = JOAN_IMAGE_SIZE;  ///< bytes of joan_image (= kEnd - kVa in the x86 layout)
static_assert(sizeof(joan_image) == kSize);

#if JOAN_IMAGE_X64
/// One run of image_xlate.inc (generated): original addresses from `va` up to the next run lie at `off` + distance.
struct XlateRun {
    std::uint32_t va;   ///< first original address of the run
    std::uint32_t off;  ///< its offset in joan_image
};
#define JOAN_XLATE(va, off) {va, off},
/// The runs of image_xlate.inc (generated), sorted by original address, as read by offset_of() and original_of().
inline constexpr XlateRun kXlate[] = {
#include JOAN_XLATE_FILE
};
#undef JOAN_XLATE
#endif
/// Offset in joan_image of an original data address (any address of [kVa, kEnd]; others map linearly from the image
/// start). x86: address - kVa. x64: through the run that holds it (binary search; constant-evaluated for names).
constexpr std::uint32_t offset_of(std::uint32_t va) noexcept {
#if JOAN_IMAGE_X64
    if (va < kVa) return va - kVa;
    std::size_t lo = 0, hi = sizeof kXlate / sizeof kXlate[0];
    while (hi - lo > 1) {
        const std::size_t mid = (lo + hi) / 2;
        (kXlate[mid].va <= va ? lo : hi) = mid;
    }
    return kXlate[lo].off + (va - kXlate[lo].va);
#else
    return va - kVa;
#endif
}
/// The original address at offset off of joan_image (inverse of offset_of; an offset inside the upper half of a
/// widened pointer maps to the pointer's last original byte + the rest).
constexpr std::uint32_t original_of(std::uint32_t off) noexcept {
#if JOAN_IMAGE_X64
    std::size_t lo = 0, hi = sizeof kXlate / sizeof kXlate[0];
    while (hi - lo > 1) {
        const std::size_t mid = (lo + hi) / 2;
        (kXlate[mid].off <= off ? lo : hi) = mid;
    }
    return kXlate[lo].va + (off - kXlate[lo].off);
#else
    return off + kVa;
#endif
}
/// The .rdata part of joan_image (read-only after relocation): .rdata incl. its page padding up to .data (0x009f4000).
inline constexpr std::uint32_t kRdataSize = offset_of(0x009f4000u);

/// An original data address (structural: usable as a template argument, C++20). The constructor is explicit, so no
/// integer becomes an Addr by accident (not even through brace elision in a table): `img::Addr{0x...}` says it.
struct Addr {
    std::uint32_t va;  ///< the original address
#if JOAN_IMAGE_X64
    std::uint32_t off;  ///< offset in joan_image (offset_of(va)): computed once, by the compiler for d:: names
    /// The address `v` of the original image; on x64 also computes its offset in the widened layout.
    constexpr explicit Addr(std::uint32_t v) noexcept : va(v), off(offset_of(v)) {}
#else
    /// The address `v` of the original image.
    constexpr explicit Addr(std::uint32_t v) noexcept : va(v) {}
#endif
    // std::int64_t: any integer offset (signed, unsigned, 64-bit) converts without a narrowing warning
    /// The address `d` original bytes further (wraps in 32 bits like the original's address arithmetic).
    constexpr Addr operator+(std::int64_t d) const noexcept { return Addr(va + static_cast<std::uint32_t>(d)); }
    /// The address `d` original bytes earlier.
    constexpr Addr operator-(std::int64_t d) const noexcept { return Addr(va - static_cast<std::uint32_t>(d)); }
    /// Moves this address `d` original bytes further.
    constexpr Addr& operator+=(std::int64_t d) noexcept { return *this = *this + d; }
    /// Moves this address `d` original bytes earlier.
    constexpr Addr& operator-=(std::int64_t d) noexcept { return *this = *this - d; }
    // order and equality of two image addresses do not depend on where the image is
    /// Compares the original addresses (independent of where the image is).
    friend constexpr bool operator==(Addr a, Addr b) noexcept { return a.va == b.va; }
    /// @copydoc operator==
    friend constexpr bool operator!=(Addr a, Addr b) noexcept { return a.va != b.va; }
    /// @copydoc operator==
    friend constexpr bool operator<(Addr a, Addr b) noexcept { return a.va < b.va; }
    /// @copydoc operator==
    friend constexpr bool operator<=(Addr a, Addr b) noexcept { return a.va <= b.va; }
    /// @copydoc operator==
    friend constexpr bool operator>(Addr a, Addr b) noexcept { return a.va > b.va; }
    /// @copydoc operator==
    friend constexpr bool operator>=(Addr a, Addr b) noexcept { return a.va >= b.va; }
};
/// Its offset in joan_image.
constexpr std::uint32_t offset(Addr a) noexcept {
#if JOAN_IMAGE_X64
    return a.off;
#else
    return a.va - kVa;
#endif
}
/// "No address" in a table of addresses (the original's null pointer): addr / obj / str of it are null.
inline constexpr Addr kNoAddr{0};

/// Distance between two addresses of the image (relocation-invariant).
constexpr std::int32_t operator-(Addr a, Addr b) noexcept { return static_cast<std::int32_t>(a.va - b.va); }

/// A typed symbol of src/data_names.hpp: ref(d::flt_1080) -> float&. Offsets (d::X + n) are plain Addr.
template <class T>
struct Sym : Addr {
    /// The symbol at original address `v`.
    constexpr explicit Sym(std::uint32_t v) noexcept : Addr(v) {}
};

/// Where the image is now: the address of joan_image (a link-time constant plus base relocation).
inline std::uintptr_t base() noexcept { return reinterpret_cast<std::uintptr_t>(joan_image); }
/// The current address of an image address (as a number); kNoAddr stays 0 (a null entry of a table of names).
inline std::uintptr_t addr(Addr a) noexcept { return a.va == 0 ? 0 : base() + offset(a); }
/// The same without the kNoAddr test, for the accessors below: a reference to "no address" is never formed (it was a
/// null dereference before), and for an address computed at run time (d::X + i * 4) the test would cost a compare.
inline std::uintptr_t location(Addr a) noexcept { return base() + offset(a); }
template <class T>
/// The object of type T at the typed symbol `s` (a reference into joan_image).
T& ref(Sym<T> s) noexcept {
    return *reinterpret_cast<T*>(location(s));
}
template <class T>
/// The object of type T at original data address `a` (a reference into joan_image).
T& global(Addr a) noexcept {
    return *reinterpret_cast<T*>(location(a));
}
/// Pointer to the object at original data address `a`; nullptr for kNoAddr.
inline void* obj(Addr a) noexcept { return reinterpret_cast<void*>(addr(a)); }
/// The string at original data address `a`.
inline const char* str(Addr a) noexcept { return reinterpret_cast<const char*>(addr(a)); }

/// p points into the image (replaces literal range checks). Unsigned arithmetic: correct at any base.
inline bool in_image(std::uintptr_t v) noexcept { return v - base() < kSize; }
/// As in_image(std::uintptr_t) for a pointer.
inline bool in_image(const void* p) noexcept { return in_image(reinterpret_cast<std::uintptr_t>(p)); }
/// v lies in the image's ORIGINAL range [kVa, kEnd). Only for values that are original addresses by contract (since
/// stage S7d a live pointer of ours can lie in this range too).
constexpr bool in_old_range(std::uintptr_t v) noexcept { return v - kVa < kSize; }

// Raw overloads: computed / dynamic addresses (pointers read from game objects, `base + index * size` of runtime
// values). A value in the OLD range [kVa, kEnd) is either a missed literal / unrelocated pointer, or (stage S7d, ASLR)
// a real pointer to something that now lies there: stray_image_address decides by what is mapped (inside joan_image or
// any mapped page: a real pointer, returned as it is; nothing mapped: an original address, logged once per (address,
// caller) as `stray image address` (src/image_runtime.cpp) and rebased, and the game goes on).
// JOAN_IMG_STRAY_CHECK=0 compiles the plain identity (only for the offline object comparison, re/s7b_objcmp.py).
#ifndef JOAN_IMG_STRAY_CHECK
#define JOAN_IMG_STRAY_CHECK 1
#endif
/// raw (in the old range) as a pointer: unchanged if it points at mapped memory, else logged (once per address and
/// caller) and rebased into joan_image. Never inlined: its return address is the site that used the stray address.
std::uintptr_t stray_image_address(std::uintptr_t raw) noexcept;
/// `raw` as an address usable now: a value in the old range [kVa, kEnd) goes through stray_image_address() (JOAN_IMG_STRAY_CHECK=1), any other value is returned unchanged.
inline std::uintptr_t checked(std::uintptr_t raw) noexcept {
#if JOAN_IMG_STRAY_CHECK
    if (in_old_range(raw)) return stray_image_address(raw);
#endif
    return raw;
}
template <class T>
/// The object of type T at `raw` (a computed address; see checked()).
T& global(std::uintptr_t raw) noexcept {
    return *reinterpret_cast<T*>(checked(raw));
}
/// Pointer for the computed address `raw` (see checked()).
inline void* obj(std::uintptr_t raw) noexcept { return reinterpret_cast<void*>(checked(raw)); }
/// The string at the computed address `raw` (see checked()).
inline const char* str(std::uintptr_t raw) noexcept { return reinterpret_cast<const char*>(checked(raw)); }

// Saved game data that may hold pointers into the image (docs/RUNTIME.md: the File::file_write SaveAudit of
// B0 found raw 36-byte PointList records with image addresses, game_b95_02 / game_b97_02). On disk such a dword is
// written as the ORIGINAL address (as joan.exe wrote it, so old saves and new saves agree); on load it is relocated
// back. Only for records known to be read and written raw; a dword that is no image address passes unchanged.
/// Copies n bytes (whole dwords at offsets 0, 4, ... from src; a tail < 4 bytes is copied as is) and turns every
/// dword that points into joan_image into its original address, every resolved code pointer (stage S7c: what a code
/// value of the game is now) into its original code address.
inline void save_words(void* dst, const void* src, std::uint32_t n) noexcept {
    std::memcpy(dst, src, n);
    auto* d = static_cast<unsigned char*>(dst);
    for (std::uint32_t i = 0; i + 4 <= n; i += 4) {
        std::uint32_t v;
        std::memcpy(&v, d + i, 4);
        if (in_image(v)) {
            v = original_of(static_cast<std::uint32_t>(v - base()));
            std::memcpy(d + i, &v, 4);
        } else if (const std::uint32_t code = fn::original_code_address(v)) {
            std::memcpy(d + i, &code, 4);  // a code pointer: the original code address (stage S7c)
        }
    }
}
/// In place: every dword in the original image range becomes the matching address in joan_image, every original
/// code address that has a slot the pointer its slot holds.
inline void load_words(void* p, std::uint32_t n) noexcept {
    auto* d = static_cast<unsigned char*>(p);
    for (std::uint32_t i = 0; i + 4 <= n; i += 4) {
        std::uint32_t v;
        std::memcpy(&v, d + i, 4);
        if (in_old_range(v)) {
            v = static_cast<std::uint32_t>(base() + offset_of(v));  // x64: saves become field-wise (A6)
            std::memcpy(d + i, &v, 4);
        } else if (v - fn::kOldCodeBegin < fn::kOldCodeEnd - fn::kOldCodeBegin) {
            if (const std::uintptr_t code = fn::resolved_code_address(v)) {
                v = static_cast<std::uint32_t>(code);
                std::memcpy(d + i, &v, 4);
            }
        }
    }
}

}  // namespace joan::img

namespace img = joan::img;
