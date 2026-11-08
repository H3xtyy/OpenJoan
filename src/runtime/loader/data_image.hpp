// joan.exe's data image as an object of openjoan.exe (STANDALONE.md S6a / S6b, docs/RUNTIME.md): what
// openjoan.exe used to read from joan.exe besides its code, as our own data:
//  - the import table (src/import_table.inc), generated once from joan.exe by re/gen_data_image.py (its other output,
//    image_layout.inc - joan.exe's header fields and section table - went with the synthetic header in stage S7d and
//    with the Linux harness in OpenJoan Phase 1);
//  - the initial contents of .rdata / .data as typed, named C++ (src/data/, re/gen_data_curated.py): one packed
//    struct per run of objects ("chunk"), all of them members of ONE object, joan_image (src/data/image.cpp), at
//    their original offsets from img::kVa = 0x009cf000; .bss and .idata follow in the same object. Objects proven
//    dead (EH tables, data only the replaced CRT / D3DX / libjpeg code read ...) are not emitted; their ranges
//    (src/data/dead.cpp) stay zero, or are filled with 0xCD for [Loader] PoisonDead=1 so that a game run proves
//    nothing reads them.
// Startup (src/image_runtime.cpp, before any other static constructor of ours): check_sections() checks the dead
// ranges against the section table (the values are committed source, ADR-0001), relocate() moves every stored data pointer
// (data_relocs.inc) to where joan_image really is; code pointers keep their original thunk / function addresses (no
// code of joan.exe exists there: stage S7c) until relocate_code() points them at what their code slots hold, after
// the replacements are installed and before the game runs (loader/joan_re.cpp).
//
// Platform independent (no <windows.h>): tests/data_image_reference_test.cpp runs the same code on copies of the image
// at arbitrary bases and compares the committed values with the reference joan.exe when $JOAN_GAME_DIR holds it.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

/// The data image as an object of the executable: import table, committed initial contents, start-up relocation.
namespace joan::data_image {

/// Byte that fills the ranges of dead objects when [Loader] PoisonDead=1 (zero otherwise, see the file comment).
inline constexpr std::uint8_t kPoisonByte = 0xCD;

/// One import address table entry of joan.exe (generated).
struct Import {
    std::uint32_t slot;     ///< IAT slot: ORIGINAL address (inside .idata of the image; the loader rebases it)
    std::uint16_t dll;      ///< index into import_dlls()
    std::uint16_t ordinal;  ///< import by ordinal when nonzero (DSOUND.dll #1)
    const char* name;       ///< import by name otherwise
};

/// DLLs in joan.exe's import descriptor order (the order the loader loads them).
std::span<const char* const> import_dlls();
/// Every IAT slot, grouped by DLL in import_dlls() order.
std::span<const Import> imports();

/// Per initialised section (.rdata, .data) of the unrelocated image.
struct SectionResult {
    const char* name = nullptr;      ///< section name (".rdata", ".data")
    std::uint32_t va = 0;            ///< original address of the section start
    std::uint32_t size = 0;          ///< bytes of the section in the image
    std::size_t emitted_bytes = 0;   ///< bytes of typed chunks (generator)
    std::size_t dead_ranges = 0;     ///< ranges left out (zero, or kPoisonByte)
    std::size_t dead_bytes = 0;      ///< total bytes of those ranges
};
/// Result of check_sections().
struct CheckResult {
    SectionResult rdata;          ///< the .rdata section
    SectionResult data;           ///< the .data section
    const char* error = nullptr;  ///< a dead range outside its section (generated tables inconsistent); null if fine
    /// True when the structure is consistent (no error).
    bool ok() const { return !error; }
};
/// The sections of the image (src/data/dead.cpp kSections) and the dead ranges inside each: structure only, no
/// checksum (the values are committed source, ADR-0001).
CheckResult check_sections();

/// Result of relocate().
struct RelocResult {
    std::uintptr_t base = 0;          ///< where the image was relocated to (address of image[0])
    std::size_t data = 0;             ///< data pointers moved: word = base + (value - kVa)
    std::size_t code = 0;             ///< code pointers checked and left as they are (relocate_code moves them)
    std::size_t mismatches = 0;       ///< words that did not hold their generated value (left unchanged)
    std::uint32_t first_mismatch = 0; ///< original address of the first one
    std::uint32_t first_word = 0 /**< the value found there */, first_expected = 0 /**< the committed value */;
    std::size_t out_of_range = 0;     ///< data reloc values outside [kVa, kEnd] (a generator error; left unchanged)
    /// True when every word matched and every value was in range.
    bool ok() const { return mismatches == 0 && out_of_range == 0; }
};
/// Relocates the data pointers of data_relocs.inc inside `image` (joan_image or a copy of it) for an image whose
/// first byte is at address `base`. Every word must still hold its generated original value (else it is counted
/// and left alone). Run exactly once, before anything reads a pointer out of the image.
RelocResult relocate(std::uint8_t* image, std::uintptr_t base);
/// Result of relocate_code().
struct CodeRelocResult {
    std::size_t code = 0;              ///< code pointers set to their slot's target
    std::size_t mismatches = 0;        ///< words that no longer held their original value (left unchanged)
    std::uint32_t first_mismatch = 0;  ///< original address of the first one
    std::uint32_t first_word = 0 /**< the value found there */, first_expected = 0 /**< the original value */;
    std::size_t unresolved = 0;        ///< values without a code slot (left unchanged; a generator error)
    std::uint32_t first_unresolved = 0;  ///< the first such value
    /// True when every word matched and every value had a slot.
    bool ok() const { return mismatches == 0 && unresolved == 0; }
};
/// Stage S7c: sets every code pointer of data_relocs.inc inside `image` to `resolve(original value)` (what the code
/// slot of that original address holds). Every word must still hold its original value. Run exactly once, after the
/// code table is sealed and before the game reads any code pointer out of the image.
CodeRelocResult relocate_code(std::uint8_t* image, std::uintptr_t (*resolve)(std::uintptr_t original));
/// Number of pointer dwords in data_relocs.inc (data, code).
std::size_t reloc_count(bool code);

/// Fills the dead ranges with kPoisonByte ([Loader] PoisonDead=1); returns the bytes filled.
std::size_t poison_dead(std::uint8_t* image);

/// The ranges left out of the image (dead objects), for the reference test / diagnostics.
struct DeadRange {
    std::uint32_t va;    ///< original address of the range
    std::uint32_t size;  ///< bytes
};
/// Number of dead ranges.
std::size_t dead_range_count();
/// Dead range `i` (`i < dead_range_count()`).
DeadRange dead_range(std::size_t i);

/// The live data object at or before original address `va` (data_symbols.inc), for diagnostics: its name and start,
/// or nullptr.
const char* symbol_at(std::uint32_t va, std::uint32_t* start);

}  // namespace joan::data_image
