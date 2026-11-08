// joan.exe's data image (STANDALONE.md S6a / S6b, docs/RUNTIME.md) - see data_image.hpp.
#include "data_image.hpp"

#include <cstring>
#include <iterator>

#include "data/data_common.hpp"
#include "runtime/image.hpp"

namespace joan::data_image {
namespace {

#include "runtime/import_table.inc"

const data::Section* section(std::size_t i) { return i < data::kSectionCount ? &data::kSections[i] : nullptr; }

constexpr std::uint32_t kVa = img::kVa, kEnd = img::kEnd;

const char* check_section(const data::Section& sc, SectionResult& r) {
    r.name = sc.name;
    r.va = sc.va;
    r.size = sc.size;
    r.emitted_bytes = sc.emitted_bytes;
    if (sc.va < kVa || sc.va + sc.size > kEnd) return "a section lies outside the image";
    for (std::size_t i = 0; i < data::kDeadRangeCount; ++i) {
        const data::DeadRange& d = data::kDeadRanges[i];
        if (d.va >= sc.va && d.va < sc.va + sc.size) {
            if (d.va + d.size > sc.va + sc.size) return "a dead range crosses its section's end";
            ++r.dead_ranges;
            r.dead_bytes += d.size;
        }
    }
    return nullptr;
}

// data_relocs.inc: every pointer dword of the emitted image, as {original address, code?, original value}.
struct Reloc {
    std::uint32_t va;
    bool code;
    std::uint32_t value;
};
constexpr bool kKind_code = true;
constexpr bool kKind_data = false;
#define JOAN_DATA_RELOC(address, kind, value) {address, kKind_##kind, value},
constexpr Reloc kRelocs[] = {
#include "data/data_relocs.inc"
};
#undef JOAN_DATA_RELOC

// data_symbols.inc: name and original address of every live object (diagnostics only).
struct Symbol {
    std::uint32_t va;
    const char* name;
};
#define JOAN_DATA_SYMBOL(section, type, name, address) {address, #name},
constexpr Symbol kSymbols[] = {
#include "data/data_symbols.inc"
};
#undef JOAN_DATA_SYMBOL

// A stored pointer of the image: pointer-sized (data_common.hpp uptr). Its original value (unrelocated) is the
// original address, zero-extended on x64.
std::uintptr_t load_ptr(const std::uint8_t* p) {
    std::uintptr_t v;
    std::memcpy(&v, p, sizeof v);
    return v;
}
void store_ptr(std::uint8_t* p, std::uintptr_t v) { std::memcpy(p, &v, sizeof v); }

}  // namespace

std::span<const char* const> import_dlls() { return kImportDlls; }
std::span<const Import> imports() { return kImports; }

std::size_t dead_range_count() { return data::kDeadRangeCount; }
DeadRange dead_range(std::size_t i) { return {data::kDeadRanges[i].va, data::kDeadRanges[i].size}; }

CheckResult check_sections() {
    CheckResult r;
    const data::Section* rdata = section(0);
    const data::Section* data = section(1);
    if (!rdata || !data || std::strcmp(rdata->name, ".rdata") != 0 || std::strcmp(data->name, ".data") != 0) {
        r.error = "src/data/dead.cpp has no .rdata / .data sections";
        return r;
    }
    r.error = check_section(*rdata, r.rdata);
    if (!r.error) r.error = check_section(*data, r.data);
    return r;
}

RelocResult relocate(std::uint8_t* image, std::uintptr_t base) {
    RelocResult r;
    r.base = base;
    for (const Reloc& x : kRelocs) {
        std::uint8_t* const p = image + img::offset_of(x.va);
        const std::uintptr_t word = load_ptr(p);
        if (word != x.value) {
            if (r.mismatches++ == 0) {
                r.first_mismatch = x.va;
                r.first_word = static_cast<std::uint32_t>(word);
                r.first_expected = x.value;
            }
            continue;
        }
        if (x.code) {
            ++r.code;  // checked only: relocate_code() moves it once the code table is sealed (stage S7c)
            continue;
        }
        if (x.value < kVa || x.value > kEnd) {
            ++r.out_of_range;
            continue;
        }
        // unsigned arithmetic: the result is right for any base (no sign involved)
        store_ptr(p, base + img::offset_of(x.value));
        ++r.data;
    }
    return r;
}

CodeRelocResult relocate_code(std::uint8_t* image, std::uintptr_t (*resolve)(std::uintptr_t original)) {
    CodeRelocResult r;
    for (const Reloc& x : kRelocs) {
        if (!x.code) continue;
        std::uint8_t* const p = image + img::offset_of(x.va);
        const std::uintptr_t word = load_ptr(p);
        if (word != x.value) {
            if (r.mismatches++ == 0) {
                r.first_mismatch = x.va;
                r.first_word = static_cast<std::uint32_t>(word);
                r.first_expected = x.value;
            }
            continue;
        }
        const std::uintptr_t target = resolve(x.value);
        if (target == 0) {
            if (r.unresolved++ == 0) r.first_unresolved = x.va;
            continue;
        }
        store_ptr(p, target);
        ++r.code;
    }
    return r;
}

std::size_t reloc_count(bool code) {
    std::size_t n = 0;
    for (const Reloc& x : kRelocs)
        if (x.code == code) ++n;
    return n;
}

std::size_t poison_dead(std::uint8_t* image) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < data::kDeadRangeCount; ++i) {
        const data::DeadRange& d = data::kDeadRanges[i];
        // dead ranges hold no pointers (nothing reads them): one run, the same size in both layouts
        std::memset(image + img::offset_of(d.va), kPoisonByte, d.size);
        n += d.size;
    }
    return n;
}

const char* symbol_at(std::uint32_t va, std::uint32_t* start) {
    const Symbol* best = nullptr;
    for (const Symbol& s : kSymbols)
        if (s.va <= va && (!best || s.va > best->va)) best = &s;
    if (!best) return nullptr;
    if (start) *start = best->va;
    return best->name;
}

}  // namespace joan::data_image
