// SPDX-License-Identifier: GPL-3.0-or-later
// The committed data image against the reference build (ADR-0001), and the relocation machinery of the loader.
//
// Links the committed object (src/data/image.cpp: joan_image_typed, alias joan_image) with src/data/dead.cpp and the
// platform-independent start-up code src/runtime/loader/data_image.cpp, as joan_re does, and checks:
//  1. structure: every symbol of data_symbols.inc fits its section, every pointer word of data_relocs.inc holds its
//     original value, relocations do not overlap;
//  2. the reference: when $JOAN_GAME_DIR/joan.exe exists and is the reference build (SHA-256
//     ad1dd30fcd9e7dfa7a0d738eecbba0a33b32509c54f9a25f646d497c30d31c7c), every byte of the committed .rdata / .data
//     equals the executable's byte at the same original address - pointer slots through their original value
//     (8 bytes on x64, zero-extended), dead ranges zero, every other byte through img::offset_of (image_xlate.inc on
//     x64). Without the executable the step is SKIPPED (exit code 77: CTest marks the test skipped);
//  3. relocation of copies at many bases (identity, low, high, near 4 GB, above 4 GB on x64, random): every data
//     pointer becomes base + offset, every code pointer keeps its value, nothing else changes, a second relocation
//     changes nothing; relocate_code with a stand-in resolver likewise;
//  4. the real object relocated in place where it is linked, read through image.hpp; the save helpers (x86 layout).
//
// Successor of joan-re's re/data_image_harness.cpp (Phase 1 step 4, 2026-10-29): the section CRC check and the
// image.bin for the python checks went with the extraction path.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <vector>

#include "data/data_common.hpp"
#include "runtime/image.hpp"
#include "runtime/loader/data_image.hpp"

// image.hpp's raw overloads call this for a stray address; the test never passes one (joan_re: image_runtime).
std::uintptr_t joan::img::stray_image_address(std::uintptr_t raw) noexcept {
    std::fprintf(stderr, "stray image address 0x%08lx\n", static_cast<unsigned long>(raw));
    std::abort();
}

// image.hpp's save helpers ask the code table which original code address a pointer stands for (joan_re:
// src/runtime/code_table.cpp after seal()). Stand-in: original code address v <-> "resolved pointer" v + kCodeDelta.
constexpr std::uint32_t kCodeDelta = 0x40000000u;
std::uint32_t joan::fn::original_code_address(std::uintptr_t pointer) noexcept {
    const std::uintptr_t v = pointer - kCodeDelta;
    return v - fn::kOldCodeBegin < fn::kOldCodeEnd - fn::kOldCodeBegin ? static_cast<std::uint32_t>(v) : 0;
}
std::uintptr_t joan::fn::resolved_code_address(std::uint32_t va) noexcept { return va + kCodeDelta; }

namespace {

using namespace joan::data;
namespace di = joan::data_image;
constexpr std::uint32_t kVa = img::kVa, kSize = img::kSize;
constexpr int kSkip = 77;  // CTest SKIP_RETURN_CODE
const char* const kReferenceSha256 = "ad1dd30fcd9e7dfa7a0d738eecbba0a33b32509c54f9a25f646d497c30d31c7c";

std::uintptr_t resolve_code(std::uintptr_t original) { return original + kCodeDelta; }

// Every live symbol: its type's size must fit the image range of its section.
struct Sym {
    const char* section;
    const char* name;
    unsigned long long address;
    unsigned long long size;
};
#define JOAN_DATA_SYMBOL(section, type, name, address) {#section, #name, address, sizeof(type)},
const Sym kSymbols[] = {
#include "data/data_symbols.inc"
};
#undef JOAN_DATA_SYMBOL

struct Reloc {
    std::uint32_t address;
    bool code;
    std::uint32_t value;
};
#define JOAN_DATA_RELOC(address, kind, value) {address, JOAN_RELOC_KIND_##kind, value},
constexpr bool JOAN_RELOC_KIND_code = true;
constexpr bool JOAN_RELOC_KIND_data = false;
const Reloc kRelocs[] = {
#include "data/data_relocs.inc"
};
#undef JOAN_DATA_RELOC
constexpr std::size_t kRelocCount = sizeof kRelocs / sizeof kRelocs[0];

// A stored pointer of the image at original address va (pointer-sized: 8 bytes in the x64 layout).
std::uintptr_t word(const std::uint8_t* image, std::uint32_t va) {
    std::uintptr_t v;
    std::memcpy(&v, image + img::offset_of(va), sizeof v);
    return v;
}

int g_bad = 0;
void problem(const char* fmt, unsigned long long a = 0, unsigned long long b = 0, unsigned long long c = 0) {
    if (g_bad++ < 40) {
        std::fprintf(stderr, fmt, a, b, c);
        std::fputc('\n', stderr);
    }
}

// ---------------------------------------------------------------- SHA-256 (FIPS 180-4), for the reference check
struct Sha256 {
    std::uint32_t h[8] = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au, 0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};
    std::uint8_t buf[64] = {};
    std::size_t n = 0;
    unsigned long long total = 0;
    static std::uint32_t rotr(std::uint32_t x, int k) { return (x >> k) | (x << (32 - k)); }
    void block(const std::uint8_t* p) {
        static const std::uint32_t K[64] = {
            0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
            0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
            0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
            0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
            0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
            0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
            0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
            0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};
        std::uint32_t w[64];
        for (int i = 0; i < 16; ++i)
            w[i] = (std::uint32_t{p[4 * i]} << 24) | (std::uint32_t{p[4 * i + 1]} << 16) | (std::uint32_t{p[4 * i + 2]} << 8) | p[4 * i + 3];
        for (int i = 16; i < 64; ++i) {
            const std::uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            const std::uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        std::uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
        for (int i = 0; i < 64; ++i) {
            const std::uint32_t t1 = hh + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ (~e & g)) + K[i] + w[i];
            const std::uint32_t t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
            hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }
    void update(const std::uint8_t* p, std::size_t len) {
        total += len;
        while (len) {
            const std::size_t k = 64 - n < len ? 64 - n : len;
            std::memcpy(buf + n, p, k);
            n += k; p += k; len -= k;
            if (n == 64) { block(buf); n = 0; }
        }
    }
    std::string hex() {
        const unsigned long long bits = total * 8;
        std::uint8_t pad[72] = {0x80};
        const std::size_t padlen = (n < 56 ? 56 - n : 120 - n);
        std::uint8_t len[8];
        for (int i = 0; i < 8; ++i) len[i] = static_cast<std::uint8_t>(bits >> (56 - 8 * i));
        update(pad, padlen);
        update(len, 8);
        char out[65];
        for (int i = 0; i < 8; ++i) std::snprintf(out + 8 * i, 9, "%08x", h[i]);
        return out;
    }
};

// ---------------------------------------------------------------- the reference executable (PE32)
struct Exe {
    std::vector<std::uint8_t> bytes;
    struct Section { std::uint32_t va, vsize, raw, rawsize; };
    std::vector<Section> sections;
    std::uint32_t base = 0;
    bool load(const std::string& path) {
        std::FILE* f = std::fopen(path.c_str(), "rb");
        if (!f) return false;
        std::uint8_t chunk[65536];
        for (std::size_t got; (got = std::fread(chunk, 1, sizeof chunk, f)) > 0;) bytes.insert(bytes.end(), chunk, chunk + got);
        std::fclose(f);
        if (bytes.size() < 0x200 || bytes[0] != 'M' || bytes[1] != 'Z') return false;
        const std::uint32_t pe = u32(0x3c);
        if (pe + 0x100 > bytes.size() || u32(pe) != 0x00004550u) return false;
        const std::uint32_t nsec = u16(pe + 6), optsz = u16(pe + 20);
        base = u32(pe + 24 + 28);
        std::uint32_t so = pe + 24 + optsz;
        for (std::uint32_t i = 0; i < nsec; ++i, so += 40)
            sections.push_back({u32(so + 12), u32(so + 8), u32(so + 20), u32(so + 16)});
        return true;
    }
    std::uint32_t u16(std::size_t o) const { return bytes[o] | (bytes[o + 1] << 8); }
    std::uint32_t u32(std::size_t o) const {
        return bytes[o] | (bytes[o + 1] << 8) | (bytes[o + 2] << 16) | (static_cast<std::uint32_t>(bytes[o + 3]) << 24);
    }
    /// The byte at original (virtual) address va: a file byte, or 0 in the virtual part of a section.
    bool byte_at(std::uint32_t va, std::uint8_t& out) const {
        const std::uint32_t rva = va - base;
        for (const Section& s : sections) {
            if (rva - s.va >= s.vsize) continue;
            const std::uint32_t off = rva - s.va;
            out = off < s.rawsize && s.raw + off < bytes.size() ? bytes[s.raw + off] : 0;
            return true;
        }
        return false;
    }
};

// 2. Every byte of the committed .rdata / .data against the executable's.
int check_reference(const std::uint8_t* image) {
    const char* dir = std::getenv("JOAN_GAME_DIR");
    if (!dir || !*dir) {
        std::printf("reference: JOAN_GAME_DIR is not set - SKIPPED\n");
        return kSkip;
    }
    const std::string path = std::string(dir) + "/joan.exe";
    Exe exe;
    if (!exe.load(path)) {
        std::printf("reference: %s not found or not a PE file - SKIPPED\n", path.c_str());
        return kSkip;
    }
    Sha256 sha;
    sha.update(exe.bytes.data(), exe.bytes.size());
    const std::string digest = sha.hex();
    if (digest != kReferenceSha256) {
        std::printf("reference: %s is not the reference build (SHA-256 %s) - SKIPPED\n", path.c_str(), digest.c_str());
        return kSkip;
    }
    // dead ranges: zero in the image, not compared
    std::vector<std::uint8_t> dead(kSize, 0);
    for (std::size_t i = 0; i < di::dead_range_count(); ++i) {
        const di::DeadRange d = di::dead_range(i);
        for (std::uint32_t a = d.va; a < d.va + d.size; ++a) dead[img::offset_of(a)] = 1;
    }
    // pointer slots: compared as words (their original value), their bytes not again
    std::vector<std::uint8_t> ptr(kSize, 0);
    std::size_t words = 0, bytes = 0, deadzero = 0;
    for (const Reloc& x : kRelocs) {
        std::uint8_t b[4];
        std::uint32_t want = 0;
        for (int k = 0; k < 4; ++k) {
            if (!exe.byte_at(x.address + k, b[k])) { problem("reference: reloc 0x%llx outside the executable", x.address); break; }
            want |= std::uint32_t{b[k]} << (8 * k);
        }
        const std::uintptr_t got = word(image, x.address);
        if (got != want) problem("reference: pointer at 0x%llx holds 0x%llx, the executable 0x%llx", x.address, got, want);
        for (std::size_t k = 0; k < sizeof(std::uintptr_t); ++k) ptr[img::offset_of(x.address) + k] = 1;
        ++words;
    }
    for (std::size_t i = 0; i < kSectionCount; ++i) {
        const Section& s = kSections[i];
        for (std::uint32_t va = s.va; va < s.va + s.size; ++va) {
            const std::uint32_t off = img::offset_of(va);
            if (ptr[off]) continue;
            if (dead[off]) {
                if (image[off] != 0) problem("reference: dead byte at 0x%llx is 0x%llx, not zero", va, image[off]);
                ++deadzero;
                continue;
            }
            std::uint8_t want = 0;
            if (!exe.byte_at(va, want)) { problem("reference: 0x%llx outside the executable", va); continue; }
            if (image[off] != want) {
                std::uint32_t start = 0;
                const char* name = di::symbol_at(va, &start);
                problem("reference: byte at 0x%llx is 0x%llx, the executable has 0x%llx", va, image[off], want);
                if (name && g_bad <= 40) std::fprintf(stderr, "    in %s+0x%x\n", name, static_cast<unsigned>(va - start));
            }
            ++bytes;
        }
    }
    std::printf("reference: %s is the reference build; %zu bytes, %zu pointer words compared, %zu dead bytes zero\n", path.c_str(),
                bytes, words, deadzero);
    return 0;
}

// 3. Relocates a copy of `pristine` for `base` and checks every byte.
void check_base(const std::vector<std::uint8_t>& pristine, const std::vector<bool>& in_reloc, std::uintptr_t base) {
    std::vector<std::uint8_t> img(pristine);
    const di::RelocResult r = di::relocate(img.data(), base);
    if (!r.ok() || r.data != di::reloc_count(false) || r.code != di::reloc_count(true))
        problem("base 0x%llx: relocate reported %llu data / %llu code / problems", base, r.data, r.code);
    for (const Reloc& x : kRelocs) {
        const std::uintptr_t got = word(img.data(), x.address);
        const std::uintptr_t want = x.code ? x.value : base + img::offset_of(x.value);
        if (got != want) problem("base 0x%llx: reloc at 0x%llx holds 0x%llx", base, x.address, got);
        if (!x.code && got - base > kSize)
            problem("base 0x%llx: data reloc at 0x%llx points outside the moved image (0x%llx)", base, x.address, got);
    }
    std::size_t changed = 0;
    for (std::uint32_t i = 0; i < kSize; ++i)
        if (!in_reloc[i] && img[i] != pristine[i]) ++changed;
    if (changed) problem("base 0x%llx: %llu bytes outside the relocated words changed", base, changed);
    std::vector<std::uint8_t> again(img);
    const di::RelocResult r2 = di::relocate(again.data(), base + 0x10000u);
    if (base != kVa && (r2.data != 0 || again != img)) problem("base 0x%llx: a second relocation changed %llu words", base, r2.data);
    std::vector<std::uint8_t> code(img);
    const di::CodeRelocResult rc = di::relocate_code(code.data(), &resolve_code);
    if (!rc.ok() || rc.code != di::reloc_count(true))
        problem("base 0x%llx: relocate_code reported %llu code / %llu mismatches", base, rc.code, rc.mismatches);
    for (const Reloc& x : kRelocs) {
        const std::uintptr_t got = word(code.data(), x.address);
        const std::uintptr_t want = x.code ? x.value + kCodeDelta : base + img::offset_of(x.value);
        if (got != want) problem("base 0x%llx: after relocate_code, reloc at 0x%llx holds 0x%llx", base, x.address, got);
    }
    changed = 0;
    for (std::uint32_t i = 0; i < kSize; ++i)
        if (!in_reloc[i] && code[i] != pristine[i]) ++changed;
    if (changed) problem("base 0x%llx: relocate_code changed %llu bytes outside the relocated words", base, changed);
    std::vector<std::uint8_t> code2(code);
    const di::CodeRelocResult rc2 = di::relocate_code(code2.data(), &resolve_code);
    if (rc2.code != 0 || rc2.mismatches != di::reloc_count(true) || code2 != code)
        problem("base 0x%llx: a second relocate_code changed %llu words", base, rc2.code);
}

}  // namespace

int main(int argc, char** argv) {
    unsigned seed = 1;
    for (int i = 1; i < argc; ++i)
        if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc) seed = static_cast<unsigned>(std::strtoul(argv[++i], nullptr, 0));

    // 1. the image as the compiler built it
    const std::vector<std::uint8_t> pristine(joan_image, joan_image + kSize);
    const di::CheckResult c = di::check_sections();
    if (c.error) {
        std::fprintf(stderr, "check: %s\n", c.error);
        return 1;
    }
    for (const di::SectionResult* s : {&c.rdata, &c.data})
        std::printf("%-6s 0x%08x size 0x%05x: %zu bytes typed, %zu dead ranges (%zu bytes)\n", s->name,
                    static_cast<unsigned>(s->va), static_cast<unsigned>(s->size), s->emitted_bytes, s->dead_ranges, s->dead_bytes);
    for (const Sym& s : kSymbols) {
        const bool bss = std::strcmp(s.section, "bss") == 0;
        const unsigned long long lo = std::strcmp(s.section, "rdata") == 0 ? c.rdata.va : bss ? 0xa34000u : c.data.va;
        const unsigned long long hi = std::strcmp(s.section, "rdata") == 0 ? c.rdata.va + c.rdata.size
                                      : bss ? 0xa6cd44u : c.data.va + c.data.size;
        if (s.address < lo || s.address + s.size > hi) problem("symbol 0x%llx size %llu outside its section", s.address, s.size);
    }
    std::vector<bool> in_reloc(kSize, false);
    std::size_t ndata = 0, ncode = 0;
    std::printf("layout: %s, joan_image 0x%x bytes\n", JOAN_IMAGE_X64 ? "x64 (image_xlate.inc)" : "x86 (original)",
                static_cast<unsigned>(kSize));
    for (const Reloc& x : kRelocs) {
        if (x.address < kVa || x.address + 4 > img::kEnd) problem("reloc 0x%llx outside the image", x.address);
        if (word(pristine.data(), x.address) != x.value)
            problem("reloc 0x%llx: image holds 0x%llx, expected 0x%llx", x.address, word(pristine.data(), x.address), x.value);
        if (!x.code && (x.value < kVa || x.value > img::kEnd)) problem("data reloc 0x%llx value 0x%llx outside the image", x.address, x.value);
        for (std::size_t k = 0; k < sizeof(std::uintptr_t); ++k) {
            if (in_reloc[img::offset_of(x.address) + k]) problem("reloc 0x%llx overlaps another", x.address);
            in_reloc[img::offset_of(x.address) + k] = true;
        }
        (x.code ? ncode : ndata)++;
    }
    std::printf("%zu symbols; %zu relocations (%zu data, %zu code) hold their original values\n",
                sizeof kSymbols / sizeof kSymbols[0], kRelocCount, ndata, ncode);

    // 2. the reference build
    const int reference = check_reference(pristine.data());

    // 3. relocation at many bases
    std::vector<std::uintptr_t> bases = {kVa, 0x00010000u, 0x00400000u, 0x20000000u, 0x7fff0000u - kSize,
                                         0x80000000u, 0xfff00000u - kSize, 0xffffffffu - kSize + 1u - 0x1000u};
    std::mt19937 rng(seed);
    for (int i = 0; i < 24; ++i) bases.push_back((rng() & ~0xfffu) % (0u - kSize - 0x1000u));
#if JOAN_IMAGE_X64
    for (const std::uint64_t b : {0x100000000ull, 0x7ff6a0000000ull, 0x00007ffffff00000ull - kSize})
        bases.push_back(static_cast<std::uintptr_t>(b));  // x64: above 4 GB
#endif
    for (const std::uintptr_t b : bases) check_base(pristine, in_reloc, b);
    std::printf("relocation checked at %zu bases (identity, 0x00010000, 0x20000000, 0x80000000, near 4 GB, 24 random)\n", bases.size());

    // 4. the real object, relocated in place where it is linked, read through image.hpp
    const auto real = reinterpret_cast<std::uintptr_t>(joan_image);
    const bool overlap = real < img::kEnd && real + kSize > img::kVa;
    const di::RelocResult r = di::relocate(joan_image, real);
    std::printf("joan_image at 0x%llx%s: %zu data relocated, %zu code kept, %zu mismatches\n", static_cast<unsigned long long>(real),
                overlap ? " (overlapping its original range 0x009cf000..0x00a70000)" : "", r.data, r.code, r.mismatches);
    if (!r.ok()) ++g_bad;
    for (const Reloc& x : kRelocs) {
        const std::uintptr_t got = img::global<std::uintptr_t>(img::Addr(x.address));
        const std::uintptr_t want = x.code ? x.value : img::addr(img::Addr(x.value));
        if (got != want) problem("in place: reloc 0x%llx holds 0x%llx, want 0x%llx", x.address, got, want);
        if (!x.code && x.value < img::kEnd && !img::in_image(static_cast<std::uintptr_t>(got)))
            problem("in place: reloc 0x%llx -> 0x%llx not in_image", x.address, got);
    }
#if !JOAN_IMAGE_X64
    // save_words / load_words: a record with a vtable and a data pointer (as the PointList records of SaveAudit);
    // x86 layout only until Phase 2 makes the save records field-wise.
    if (real <= 0xffffffffu - kSize) {
        std::uint32_t rec[9] = {};
        rec[0] = static_cast<std::uint32_t>(img::addr(img::Addr(0x009d9a2cu)));
        rec[5] = static_cast<std::uint32_t>(img::addr(img::Addr(0x00a403a0u)));
        rec[6] = rec[0];
        rec[1] = 0x3f800000u;  // 1.0f stays
        rec[2] = 0x00012345u;  // a small number stays
        rec[8] = 0x0040876fu + kCodeDelta;  // a code pointer (stage S7c): written as the original code address
        std::uint32_t disk[9];
        img::save_words(disk, rec, sizeof rec);
        if (disk[0] != 0x009d9a2cu || disk[5] != 0x00a403a0u || disk[6] != 0x009d9a2cu || disk[1] != rec[1] ||
            disk[2] != rec[2] || disk[8] != 0x0040876fu)
            problem("save_words: 0x%llx 0x%llx 0x%llx", disk[0], disk[5], disk[8]);
        img::load_words(disk, sizeof disk);
        if (std::memcmp(disk, rec, sizeof rec) != 0) problem("load_words: 0x%llx 0x%llx 0x%llx", disk[0], disk[5], disk[8]);
        std::printf("save_words / load_words round trip OK\n");
    }
#endif
    if (g_bad) {
        std::fprintf(stderr, "FAILED: %d problem(s)\n", g_bad);
        return 1;
    }
    std::printf("OK%s\n", reference == kSkip ? " (reference comparison skipped)" : "");
    return reference == kSkip ? kSkip : 0;
}
