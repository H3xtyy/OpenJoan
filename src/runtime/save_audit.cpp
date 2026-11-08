// [Debug] SaveAudit=1 - see save_audit.hpp. Stage S7b question: do saves hold absolute addresses of joan.exe's image
// (pointers into .rdata / .data / .bss, vtables, thunks)? Those were valid in the original because its base never
// moved; since the data image is relocated (S7b-main) they are not. Since S7b-main a value inside joan_image (where
// the data image is now) is reported too, with its original address: that is what a save written now may hold.
// SaveAudit=2 (re/a6/SAVES_PLAN.md 3.3) adds heap values, and maps the values in field-wise chunks to their record's
// fields. This file also installs the game's save::hooks() (src/save/io.hpp: [Debug] SaveFieldwise / SaveCompare /
// SaveAudit=2, image address relocation) and reports SaveCompare.
#include "save_audit.hpp"

#include "../platform/win32_types.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "../data/data_common.hpp"
#include "code.hpp"
#include "code_slots_core.hpp"
#include "code_table.hpp"
#include "det_heap.hpp"
#include "image.hpp"
#include "../registry.hpp"
#include "../save/io.hpp"
#include "../platform/memory.hpp"

namespace joan::hook::save_audit {
namespace {

using namespace joan::data;  // u8, i16, Guid ... of the data_symbols.inc types

// Live objects of the data image (src/data/data_symbols.inc, re/gen_data_curated.py).
struct Symbol {
    std::uint32_t va;
    std::uint32_t size;
    const char* name;
    const char* section;
};
#define JOAN_DATA_SYMBOL(section, type, name, address) \
    {address, static_cast<std::uint32_t>(sizeof(type)), #name, #section},
const Symbol kSymbols[] = {
#include "../data/data_symbols.inc"
};
#undef JOAN_DATA_SYMBOL

// Original VAs (the values in a save are original addresses): header, .text, data image (image.hpp), .idata, .rsrc.
constexpr std::uint32_t kLow = 0x00400000, kHigh = 0x00a96000;  // header .. end of .reloc
constexpr std::uint32_t kText = 0x00401000, kTextEnd = 0x009ce700;
constexpr std::uint32_t kImage = img::kVa, kIatEnd = img::kEnd, kIat = img::kEnd - 0x3000;  // .idata: last 3 pages
constexpr int kMaxLines = 5000;

enum Kind { kHeader, kThunk, kCode, kObjectStart, kInsideObject, kVtable, kString, kUnnamedData, kIatSlot, kRsrc,
            kJoanImage, kJoanCode, kHeap, kKinds };
// kJoanImage / kJoanCode count, in addition to their kind, the values that are addresses inside joan_image
// (relocated data) / resolved code pointers of the code table (stage S7c: what a code value of the game now is).
const char* const kKindNames[kKinds] = {"header", "thunk", "code", "object", "inside-object", "vtable", "string",
                                        "unnamed-data", "iat", "rsrc/reloc", "joan_image", "joan_code", "heap"};

struct State {
    std::mutex mutex;
    std::vector<Symbol> symbols;  // sorted by address
    // (file, offset, value, caller) already logged
    std::set<std::tuple<std::string, std::uint32_t, std::uint32_t, std::uintptr_t>> seen;
    std::map<std::string, std::array<int, kKinds>> per_file;
    int lines = 0;
    bool capped = false;
    // SaveCompare per file: record name -> {chunks compared, mismatching fields}
    std::map<std::string, std::map<std::string, std::array<int, 2>>> compared;
    int compare_lines = 0;
};
State& state() {
    static State s;
    return s;
}

const Symbol* find_symbol(State& s, std::uint32_t v) {
    if (s.symbols.empty()) {
        s.symbols.assign(std::begin(kSymbols), std::end(kSymbols));
        std::sort(s.symbols.begin(), s.symbols.end(), [](const Symbol& a, const Symbol& b) { return a.va < b.va; });
    }
    auto it = std::upper_bound(s.symbols.begin(), s.symbols.end(), v,
                               [](std::uint32_t x, const Symbol& sym) { return x < sym.va; });
    if (it == s.symbols.begin()) return nullptr;
    --it;
    return v < it->va + std::max<std::uint32_t>(it->size, 1) ? &*it : nullptr;
}

// What lies at v (an address of the original image); text into out.
Kind classify(State& s, std::uint32_t v, char* out, std::size_t n) {
    if (v < kText) {
        std::snprintf(out, n, "PE header");
        return kHeader;
    }
    if (v < kTextEnd) {
        char slot[160];
        code_table::describe(v, slot, sizeof slot);
        std::snprintf(out, n, "%s", slot[0] ? slot : ".text");
        return code::is_thunk(v) ? kThunk : kCode;
    }
    if (v >= kIat && v < kIatEnd) {
        std::snprintf(out, n, "import address table slot");
        return kIatSlot;
    }
    if (v >= kIatEnd) {
        std::snprintf(out, n, ".rsrc / .reloc");
        return kRsrc;
    }
    if (v < kImage) {
        std::snprintf(out, n, ".text end padding");
        return kCode;
    }
    const Symbol* sym = find_symbol(s, v);
    if (sym == nullptr) {
        std::snprintf(out, n, "data image, no live object");
        return kUnnamedData;
    }
    const bool vtable = std::strncmp(sym->name, "vt_", 3) == 0;
    const bool string = std::strncmp(sym->name, "s_", 2) == 0;
    if (v == sym->va) {
        std::snprintf(out, n, "%s %s (%u bytes)", sym->section, sym->name, static_cast<unsigned>(sym->size));
        return vtable ? kVtable : string ? kString : kObjectStart;
    }
    std::snprintf(out, n, "%s %s+0x%x", sym->section, sym->name, static_cast<unsigned>(v - sym->va));
    return vtable ? kVtable : string ? kString : kInsideObject;
}

}  // namespace

bool enabled() { return settings().save_audit != 0 || settings().save_compare; }

namespace {
// SaveAudit=2: a value that points into the game's heap (the deterministic heap while recording / replaying, else
// committed private memory: Win32 heaps, VirtualAlloc blocks, stacks).
bool heap_value(std::uint32_t v) {
    if (v < 0x10000) return false;
    if (det_heap::active()) return det_heap::contains(v);
    platform::memory::Region r;
    if (!platform::memory::query(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(v)), r)) return false;
    return r.committed && r.private_memory;
}
}  // namespace

void scan(const char* file_name, std::int32_t pos, const void* buf, std::uint32_t size, const char* what,
          const void* caller) {
    if (buf == nullptr || size < 4 || settings().save_audit == 0) return;
    const bool heap = settings().save_audit >= 2;
    State& s = state();
    std::lock_guard lock(s.mutex);
    const std::string file = file_name != nullptr ? file_name : "?";
    auto& counts = s.per_file.try_emplace(file).first->second;
    const auto* p = static_cast<const std::uint8_t*>(buf);
    static const std::uintptr_t base = [] {  // the base of our own module (openjoan.exe)
        std::uintptr_t low = 0, high = 0;
        platform::memory::self_range(low, high);
        return low;
    }();
    const auto ret = reinterpret_cast<std::uintptr_t>(caller);
    for (std::uint32_t off = 0; off + 4 <= size; ++off) {
        std::uint32_t v = 0;
        std::memcpy(&v, p + off, 4);
        // A pointer into joan_image (where the data image is now) counts as the original address it stands for.
        const bool moved = img::in_image(v);
        // A resolved code pointer (a replacement / trap stub some slot holds) counts as the original code address.
        const std::uint32_t code = moved ? 0 : fn::original_code_address(v);
        const std::uint32_t original =
            moved ? img::original_of(static_cast<std::uint32_t>(v - img::base())) : code != 0 ? code : v;
        const bool in_heap = heap && !moved && code == 0 && heap_value(v);
        if (!moved && code == 0 && !in_heap && (v < kLow || v >= kHigh)) continue;
        char text[160];
        const Kind kind = in_heap ? kHeap : classify(s, original, text, sizeof text);
        if (in_heap) std::snprintf(text, sizeof text, "%s", det_heap::active() ? "deterministic heap" : "private memory");
        ++counts[kind];
        if (moved) ++counts[kJoanImage];
        if (code != 0) ++counts[kJoanCode];
        if (!s.seen.insert({file, off, v, ret}).second) continue;
        if (s.lines >= kMaxLines) {
            if (!s.capped) log_line("SaveAudit: %d lines logged - further hits only counted", kMaxLines);
            s.capped = true;
            continue;
        }
        ++s.lines;
        char where[64] = "";
        if (moved)
            std::snprintf(where, sizeof where, " = joan_image address of 0x%08x", static_cast<unsigned>(original));
        else if (code != 0)
            std::snprintf(where, sizeof where, " = code pointer of 0x%08x", static_cast<unsigned>(original));
        log_line("SaveAudit %s pos %ld +0x%x%s: 0x%08x%s %s [%s] (%s, %u bytes, caller 0x%08llx = image+0x%llx)",
                 file.c_str(), static_cast<long>(pos), static_cast<unsigned>(off), off % 4 == 0 ? "" : " (unaligned)",
                 static_cast<unsigned>(v), where, kKindNames[kind], text, what, static_cast<unsigned>(size),
                 static_cast<unsigned long long>(ret), static_cast<unsigned long long>(ret - base));
    }
}

void close(const char* file_name) {
    State& s = state();
    std::lock_guard lock(s.mutex);
    const auto cit = s.compared.find(file_name != nullptr ? file_name : "");
    if (cit != s.compared.end()) {
        int chunks = 0, bad = 0;
        std::string per;
        for (const auto& [rec, n] : cit->second) {
            chunks += n[0];
            bad += n[1];
            per += ' ' + rec + '=' + std::to_string(n[0]) + (n[1] != 0 ? "/" + std::to_string(n[1]) + " bad" : "");
        }
        log_line("SaveCompare %s closed: %d records compared, %d mismatching fields:%s", cit->first.c_str(), chunks,
                 bad, per.c_str());
        s.compared.erase(cit);
    }
    const auto it = s.per_file.find(file_name != nullptr ? file_name : "?");
    if (it == s.per_file.end()) return;
    std::string sum;
    int total = 0;
    for (int k = 0; k < kKinds; ++k) {
        if (it->second[k] == 0) continue;
        total += it->second[k];
        sum += ' ';
        sum += kKindNames[k];
        sum += '=';
        sum += std::to_string(it->second[k]);
    }
    log_line("SaveAudit %s closed: %d dwords in the image ranges%s", it->first.c_str(),
             total - it->second[kJoanImage] - it->second[kJoanCode] - it->second[kHeap],
             sum.c_str());
    s.per_file.erase(it);
}

// ------------------------------------------------------------------------------------------- save::hooks() (save/io.hpp)
namespace {

const char* const kTypeNames[] = {"u8", "i8", "char", "u16", "i16", "u32", "i32", "f32", "bytes", "gap", "opaque",
                                  "ptr", "word"};
const char* const kPtrNames[] = {"Flag", "Cleared", "Kept", "Image", "Stale"};

std::string field_text(const save::Field* f, std::uint32_t off) {
    if (f == nullptr) return "(no field)";
    std::string t = f->name;
    if (f->elem > 1 && f->size > f->elem) t += '[' + std::to_string((off - f->off) / f->elem) + ']';
    t += " (";
    t += kTypeNames[static_cast<int>(f->type)];
    if (f->type == save::Type::Pointer || f->type == save::Type::Word) {
        t += ' ';
        t += kPtrNames[static_cast<int>(f->kind)];
    }
    t += ')';
    return t;
}

std::uint32_t le32(const std::uint8_t* p) {
    std::uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

// SaveAudit=2 over one field-wise chunk: every image / heap value, named with the field it lies in.
void audit_chunk(State& s, const save::Event& e) {
    if (e.rec == nullptr || e.size < 4) return;
    for (std::uint32_t off = 0; off + 4 <= e.size; ++off) {
        const save::Field* f = save::field_at(*e.rec, off);
        const bool whole = f != nullptr && off + 4 <= f->off + f->size;
        if (off % 4 != 0 && !whole) continue;  // an unaligned dword across two fields is no value of the record
        const std::uint32_t v = le32(e.now + off);
        const bool image = img::in_image(v) || fn::original_code_address(v) != 0 || (v >= kLow && v < kHigh);
        const bool heap = !image && heap_value(v);
        if (!image && !heap) continue;
        const char* where = "typed";
        if (f == nullptr) where = "outside the table";
        else if (!whole) where = "straddles fields";
        else if (f->type == save::Type::Gap) where = "gap";
        else if (f->type == save::Type::Opaque) where = "opaque";
        else if (f->type == save::Type::Pointer || f->type == save::Type::Word) where = "pointer field";
        else if (f->type == save::Type::Char)
            where = std::memchr(e.now + f->off, 0, off - f->off) != nullptr ? "char tail" : "text";
        if (!s.seen.insert({e.file, off, v, reinterpret_cast<std::uintptr_t>(e.rec)}).second) continue;
        if (s.lines >= kMaxLines) continue;
        ++s.lines;
        log_line("SaveAudit2 %s %s+0x%x %s [%s]: 0x%08x %s", e.file, e.name, static_cast<unsigned>(off),
                 field_text(f, off).c_str(), where, static_cast<unsigned>(v), image ? "image address" : "heap");
    }
}

void on_event(const save::Event& e) {
    State& s = state();
    std::lock_guard lock(s.mutex);
    const char* dir = e.dir == save::Dir::Write ? "write" : "read";
    switch (e.kind) {
    case save::Event::Compared: {
        auto& n = s.compared[e.file][e.name];
        ++n[0];
        n[1] += static_cast<int>(e.off);
        return;
    }
    case save::Event::Mismatch: {
        if (s.compare_lines >= kMaxLines) return;
        ++s.compare_lines;
        const std::uint32_t at = e.field != nullptr ? e.field->off : e.off;
        const std::uint32_t len = e.field != nullptr ? std::min<std::uint32_t>(e.field->size, 16) : 1;
        char was[40] = "", now[40] = "";
        for (std::uint32_t i = 0; i < len && at + i < e.size; ++i) {
            std::snprintf(was + 2 * i, 3, "%02x", e.legacy[at + i]);
            std::snprintf(now + 2 * i, 3, "%02x", e.now[at + i]);
        }
        log_line("SaveCompare MISMATCH %s %s %s +0x%x %s: old %s new %s", e.file, e.name, dir,
                 static_cast<unsigned>(e.off), field_text(e.field, e.off).c_str(), was, now);
        return;
    }
    case save::Event::Audit:
        audit_chunk(s, e);
        return;
    case save::Event::Misuse:
        log_line("SaveCompare MISUSE %s %s %s: %u of %u bytes %s", e.file, e.name, dir, static_cast<unsigned>(e.off),
                 static_cast<unsigned>(e.size), e.dir == save::Dir::Write ? "written" : "read");
        return;
    }
}

// Ptr::Image: the original address of a joan_image / code pointer (as img::save_words) and back (img::load_words).
std::uint32_t image_out(std::uintptr_t v) {
    if (img::in_image(v)) return img::original_of(static_cast<std::uint32_t>(v - img::base()));
    if (const std::uint32_t code = fn::original_code_address(v)) return code;
    return static_cast<std::uint32_t>(v);
}
std::uintptr_t image_in(std::uint32_t v) {
    if (img::in_old_range(v)) return img::base() + img::offset_of(v);
    if (v - fn::kOldCodeBegin < fn::kOldCodeEnd - fn::kOldCodeBegin)
        if (const std::uintptr_t code = fn::resolved_code_address(v)) return code;
    return v;
}

struct InstallHooks {
    InstallHooks() {
        save::Hooks& h = save::hooks();
        h.fieldwise = [] { return settings().save_fieldwise; };
        h.compare = [] { return settings().save_compare; };
        h.audit = [] { return settings().save_audit >= 2; };
        h.image_out = &image_out;
        h.image_in = &image_in;
        h.legacy_out = [](void* dst, const void* src, std::uint32_t n) { img::save_words(dst, src, n); };
        h.legacy_in = [](void* p, std::uint32_t n) { img::load_words(p, n); };
        h.event = &on_event;
    }
} g_install_hooks;

}  // namespace

}  // namespace joan::hook::save_audit
