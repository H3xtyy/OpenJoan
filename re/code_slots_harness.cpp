// Decision-equivalence harness for stage S7c-main (docs/RUNTIME.md): proves that the code slot table resolves
// every original code address to exactly what stage S5's generated code range reached - the same replacement, or a
// trap - for the real registration list of a game run and for random configurations.
//
// Build (Linux, from the repository root; OLD = a directory holding stage S5's codegen_core.hpp and code_map.inc, e.g.
// `git show 2904a6c:src/codegen_core.hpp > OLD/codegen_core.hpp`, the same for src/code_map.inc):
//   g++ -std=c++20 -O2 -Wall -Wextra -I OLD -I hook re/code_slots_harness.cpp -o code_slots_harness
// Run:
//   code_slots_harness <registrations.txt> <canvas.txt> <relocs.txt> [random runs] [seed]
//   registrations.txt: one line per registration in registry order: `<address hex> <installed 0|1> <iat 0|1>
//                      <function id>` (re/code_slots_harness_input.py writes it from a joan_re.log of a run);
//   canvas.txt: the addresses canvas_hooks.cpp wraps (kScreens, sprite queue, viewport, model), one hex per line;
//   relocs.txt: the code relocation values of src/data/data_relocs.inc, one hex per line.
// Both models get the same install sequence (install_all in registry order, [Fixes] MouseTurn, the canvas wrappers):
//   OLD: S5 - codegen_core.hpp build_image into a buffer standing for 0x00401000.., redirect_function scanning the
//        thunk `jmp`s, patch_entry = write_entry, finalize_image; "reach(va)" follows the jmp chain from va.
//   NEW: src/code_slots_core.hpp Slots on src/code_slots.inc: redirect / patch_entry / resolve; "reach(va)" is
//        the slot's value.
// For every slot address, every thunk, every code relocation value and every canvas wrapper target the two must agree
// (both a trap, or the same pointer). Prints the S5 finalize statistics (compare with joan_re.log of the run).
// Stage S7d (ASLR: our code may lie inside joan.exe's old code range): every configuration is also run through the
// NEW model a second time with every pointer of ours (replacements, the MouseTurn fix, the canvas stubs) placed AT
// original code addresses (slot addresses / thunk targets, the worst case for telling an original address from a
// pointer of ours); both NEW runs must reach the same thing for every address (modulo the placement).
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <random>
#include <string>
#include <vector>

#include "codegen_core.hpp"     // stage S5 (OLD directory)
#include "runtime/code_slots_core.hpp"  // stage S7c (src/)

std::vector<std::uint32_t> g_canvas;  // canvas.txt

namespace {

namespace s5 = joan::hook::codegen;
namespace s7 = joan::code;

#define JOAN_CODE_THUNKS
const std::uint32_t kThunkTargets[] = {
#include "code_map.inc"
};
#undef JOAN_CODE_THUNKS
#define JOAN_CODE_ENTRIES
const s5::Entry kEntries[] = {
#include "code_map.inc"
};
#undef JOAN_CODE_ENTRIES
constexpr std::size_t kThunks = sizeof kThunkTargets / sizeof kThunkTargets[0];
constexpr std::size_t kCount = sizeof kEntries / sizeof kEntries[0];
constexpr std::uint32_t kThunkFirst = JOAN_CODE_THUNK_FIRST;

#define JOAN_CODE_SLOTS
const s7::SlotInfo kSlotInfo[] = {
#include "runtime/code_slots.inc"
};
#undef JOAN_CODE_SLOTS
#define JOAN_CODE_SLOTS_BY_VA
const std::uint32_t kSlotsByVa[] = {
#include "runtime/code_slots.inc"
};
#undef JOAN_CODE_SLOTS_BY_VA
constexpr std::size_t kSlots = sizeof kSlotInfo / sizeof kSlotInfo[0];

constexpr std::uint32_t kTrap = 0xFFFFFFFFu;  // "reaches a trap" (both models)
constexpr std::uint32_t kStubsAt = 0x70000000u, kHandler = 0x7f000000u;
constexpr std::uint32_t kFix = 0x7e000000u;               // [Fixes] MouseTurn replacement
constexpr std::uint32_t kCanvasStub = 0x7d000000u;        // + 16 * i: canvas wrapper stubs
constexpr std::uint32_t kRep = 0x40000000u;               // + 16 * function id: replacement functions

struct Reg {
    std::uint32_t address;
    bool installed;
    bool iat;
    std::uint32_t fn;  // function id (same id = same C++ function = same pointer)
};

struct Config {
    std::vector<Reg> regs;
    bool mouse_turn = true;
    bool canvas = true;
};

std::uintptr_t rep_ptr(const Reg& r) { return kRep + 16u * r.fn; }

// Where the pointers of ours are (stage S7d placement check). The default: far from joan.exe's old range (as S5 needed).
struct Place {
    std::map<std::uint32_t, std::uint32_t> rep;  // empty: rep_ptr
    std::uint32_t fix = kFix;
    std::vector<std::uint32_t> canvas;           // empty: kCanvasStub + 16 * i
    std::uintptr_t rep_of(const Reg& r) const { return rep.empty() ? rep_ptr(r) : rep.at(r.fn); }
    std::uint32_t canvas_of(std::size_t i) const {
        return canvas.empty() ? kCanvasStub + 16u * static_cast<std::uint32_t>(i) : canvas[i];
    }
};
const Place kHigh{};

// installed_replacement: the first installed registration of a code address.
std::map<std::uint32_t, std::uintptr_t> installed_map(const Config& c, const Place& p = kHigh) {
    std::map<std::uint32_t, std::uintptr_t> m;
    for (const Reg& r : c.regs)
        if (r.installed && !r.iat) m.emplace(r.address, p.rep_of(r));
    return m;
}

// ------------------------------------------------------------------------------------------------ OLD (stage S5)
struct Old {
    std::vector<std::uint8_t> text = std::vector<std::uint8_t>(s5::kTextEnd - s5::kTextBegin);
    std::vector<std::uint8_t> stubs = std::vector<std::uint8_t>((kCount + kThunks) * s5::kTrapStubSize);
    unsigned st[7] = {};
    std::vector<std::uint32_t> canvas_target;

    std::uint8_t* at(std::uint32_t va) { return text.data() + (va - s5::kTextBegin); }

    int redirect(std::uintptr_t original, std::uintptr_t replacement) {  // HEAD patch.cpp redirect_function
        int n = 0;
        for (std::uint32_t a = 0x00401000; a + 5 <= 0x0040ac90; a += 5) {
            std::uint32_t to = 0;
            if (!s5::jmp_target(at(a), a, to) || to != original) continue;
            s5::put_jmp(at(a), a, static_cast<std::uint32_t>(replacement));
            ++n;
        }
        return n;
    }
    bool patch_entry(std::uintptr_t original, std::uintptr_t replacement) {  // HEAD codegen.cpp write_entry
        const std::ptrdiff_t i = s5::find_entry(kEntries, kCount, static_cast<std::uint32_t>(original));
        if (i < 0 || kEntries[i].room < s5::kJmpSize) return false;
        s5::put_jmp(at(kEntries[i].address), kEntries[i].address, static_cast<std::uint32_t>(replacement));
        return true;
    }
    int count_thunks(std::uintptr_t target) {
        int n = 0;
        for (std::uint32_t a = 0x00401000; a + 5 <= 0x0040ac90; a += 5) {
            std::uint32_t to = 0;
            if (s5::jmp_target(at(a), a, to) && to == target) ++n;
        }
        return n;
    }

    void run(const Config& c) {
        s5::build_image(text.data(), kThunkTargets, kThunks, kThunkFirst, kEntries, kCount, stubs.data(), kStubsAt,
                        kHandler);
        const auto inst = installed_map(c);
        auto installed = [&](std::uint32_t a) -> std::uintptr_t {
            const auto it = inst.find(a);
            return it == inst.end() ? 0 : it->second;
        };
        for (const Reg& r : c.regs) {
            if (!r.installed || r.iat) continue;
            if (redirect(r.address, rep_ptr(r)) == 0) patch_entry(r.address, rep_ptr(r));
        }
        if (c.mouse_turn) {  // fixes.cpp
            const std::uintptr_t exact = installed(0x00431790);
            redirect(0x00431790, kFix);
            if (exact) redirect(exact, kFix);
        }
        canvas_target.clear();
        return_after_canvas(c, installed);
        s5::finalize_image(text.data(), kThunks, kThunkFirst, kEntries, kCount, kStubsAt,
                           [&](std::uint32_t a) { return static_cast<std::uint32_t>(installed(a)); },
                           [&](s5::Decision d, std::size_t) { ++st[static_cast<int>(d)]; });
    }
    template <class Inst>
    void return_after_canvas(const Config& c, Inst&& installed) {
        if (!c.canvas) return;
        for (std::size_t i = 0; i < g_canvas.size(); ++i) {  // canvas_hooks.cpp wrap_entry (no JOAN_CANVAS_SELF)
            const std::uint32_t a = g_canvas[i];
            const std::uint32_t stub = kCanvasStub + 16u * static_cast<std::uint32_t>(i);
            std::uintptr_t target = 0;
            if (const std::uintptr_t rep = installed(a)) {
                target = rep;
                if (redirect(rep, stub) > 0) {
                    canvas_target.push_back(static_cast<std::uint32_t>(target));
                    continue;
                }
            }
            target = a;  // S5: the stub jumps to the original address (the generated entry)
            redirect(a, stub);
            canvas_target.push_back(static_cast<std::uint32_t>(target));
        }
    }

    // Where a jump to `va` ends: a pointer of ours, or kTrap (a trap stub or int3).
    std::uint32_t reach(std::uint32_t va) {
        for (int hop = 0; hop < 8; ++hop) {
            if (va < s5::kTextBegin || va >= s5::kTextEnd) {
                if (va >= kStubsAt && va < kStubsAt + stubs.size()) return kTrap;
                return va;
            }
            std::uint32_t to = 0;
            if (!s5::jmp_target(at(va), va, to)) return kTrap;  // int3
            va = to;
        }
        return kTrap;
    }
};

// ------------------------------------------------------------------------------------------------ NEW (stage S7c)
struct New {
    std::vector<std::uintptr_t> current = std::vector<std::uintptr_t>(kSlots);
    std::vector<std::uint8_t> ours = std::vector<std::uint8_t>(kSlots);
    std::vector<std::uintptr_t> value = std::vector<std::uintptr_t>(kSlots);
    s7::Slots table{kSlotInfo, kSlots, current.data(), ours.data(), kSlotsByVa, sizeof kSlotsByVa / sizeof kSlotsByVa[0]};
    unsigned st[6] = {};
    std::vector<std::uint32_t> canvas_target;
    const Place* place = &kHigh;

    void run(const Config& c) {
        table.reset();
        const Place& pl = *place;
        const auto inst = installed_map(c, pl);
        auto installed = [&](std::uint32_t a) -> std::uintptr_t {
            const auto it = inst.find(a);
            return it == inst.end() ? 0 : it->second;
        };
        auto trap = [](std::size_t) -> std::uintptr_t { return kTrap; };
        for (const Reg& r : c.regs) {
            if (!r.installed || r.iat) continue;
            if (table.redirect(r.address, pl.rep_of(r)) == 0) table.patch_entry(r.address, pl.rep_of(r));
        }
        if (c.mouse_turn) {  // fixes.cpp: redirect_function, then redirect_replacement
            const std::uintptr_t exact = installed(0x00431790);
            table.redirect(0x00431790, pl.fix);
            if (exact) table.redirect_ours(exact, pl.fix);
        }
        canvas_target.clear();
        if (c.canvas) {
            for (std::size_t i = 0; i < g_canvas.size(); ++i) {  // canvas_hooks.cpp wrap_entry (S7c / S7d)
                const std::uint32_t a = g_canvas[i];
                const std::uint32_t stub = pl.canvas_of(i);
                if (const std::uintptr_t rep = installed(a)) {
                    if (table.redirect_ours(rep, stub) > 0) {
                        canvas_target.push_back(static_cast<std::uint32_t>(rep));
                        continue;
                    }
                }
                // code_table::entry_target(a): the real slot's rule result
                const std::ptrdiff_t s = table.find(a);
                std::uintptr_t target = kTrap;
                if (s >= 0 && !(kSlotInfo[s].flags & s7::kSlotThunk))
                    target = table.resolve(static_cast<std::size_t>(s), installed, trap);
                table.redirect(a, stub);
                canvas_target.push_back(static_cast<std::uint32_t>(target));
            }
        }
        for (std::size_t i = 0; i < kSlots; ++i) {
            s7::Resolution how{};
            value[i] = table.resolve(i, installed, trap, &how);
            ++st[static_cast<int>(how)];
        }
    }
    std::uint32_t reach(std::uint32_t va) {
        const std::ptrdiff_t s = table.find(va);
        return s < 0 ? kTrap : static_cast<std::uint32_t>(value[s]);
    }
};


std::vector<std::uint32_t> read_hex(const char* path) {
    std::vector<std::uint32_t> v;
    FILE* f = std::fopen(path, "r");
    if (!f) {
        std::fprintf(stderr, "cannot read %s\n", path);
        std::exit(2);
    }
    char line[256];
    while (std::fgets(line, sizeof line, f))
        if (line[0] != '#' && line[0] != '\n') v.push_back(static_cast<std::uint32_t>(std::strtoul(line, nullptr, 16)));
    std::fclose(f);
    return v;
}

std::vector<Reg> read_regs(const char* path) {
    std::vector<Reg> v;
    FILE* f = std::fopen(path, "r");
    if (!f) {
        std::fprintf(stderr, "cannot read %s\n", path);
        std::exit(2);
    }
    unsigned a, inst, iat, fn;
    while (std::fscanf(f, "%x %u %u %u", &a, &inst, &iat, &fn) == 4) v.push_back({a, inst != 0, iat != 0, fn});
    std::fclose(f);
    return v;
}

struct Result {
    unsigned compared = 0, same_trap = 0, same_ptr = 0, differ = 0;
    unsigned tight = 0;  // S5 int3 (room < 5) where the rule wants the replacement: S7c reaches it (by design)
};

// Compares old and new for every address either model knows. `verbose`: list differences.
Result compare(Old& o, New& n, const std::vector<std::uint32_t>& relocs, bool verbose) {
    Result r;
    auto one = [&](std::uint32_t va, const char* what) {
        const std::uint32_t a = o.reach(va), b = n.reach(va);
        ++r.compared;
        if (a == b) {
            ++(a == kTrap ? r.same_trap : r.same_ptr);
            return;
        }
        const std::ptrdiff_t e = s5::find_entry(kEntries, kCount, va);
        if (a == kTrap && e >= 0 && kEntries[e].room < s5::kJmpSize && b != kTrap) {
            ++r.tight;
            return;
        }
        ++r.differ;
        if (verbose && r.differ <= 40) {
            const std::ptrdiff_t s = n.table.find(va);
            std::printf("  DIFF %s 0x%08x: S5 %s0x%08x, S7c %s0x%08x (slot %d flags 0x%04x)\n", what,
                        static_cast<unsigned>(va), a == kTrap ? "trap " : "", static_cast<unsigned>(a),
                        b == kTrap ? "trap " : "", static_cast<unsigned>(b), static_cast<int>(s),
                        s >= 0 ? static_cast<unsigned>(kSlotInfo[s].flags) : 0u);
        }
    };
    for (std::size_t i = 0; i < kSlots; ++i)
        if (kSlotInfo[i].va) one(kSlotInfo[i].va, (kSlotInfo[i].flags & s7::kSlotThunk) ? "thunk" : "entry");
    for (std::size_t i = 0; i < kThunks; ++i) one(kThunkFirst + 5u * static_cast<std::uint32_t>(i), "thunk(S5)");
    for (std::size_t i = 0; i < kCount; ++i) one(kEntries[i].address, "entry(S5)");
    for (std::uint32_t v : relocs) one(v, "reloc");
    // canvas wrapper targets: where the stub goes on to
    for (std::size_t i = 0; i < o.canvas_target.size() && i < n.canvas_target.size(); ++i) {
        const std::uint32_t a = o.reach(o.canvas_target[i]), b = n.canvas_target[i] == kTrap ? kTrap : n.canvas_target[i];
        ++r.compared;
        if (a == b) {
            ++(a == kTrap ? r.same_trap : r.same_ptr);
        } else {
            ++r.differ;
            if (verbose) std::printf("  DIFF canvas stub %u (0x%08x)\n", static_cast<unsigned>(i), g_canvas[i]);
        }
    }
    return r;
}

// Stage S7d: the same configuration with every pointer of ours placed at original code addresses. Builds the low
// placement once per configuration (fn ids / canvas stubs -> distinct original addresses, shuffled).
Place low_place(const Config& c, std::mt19937& rng) {
    std::vector<std::uint32_t> pool;
    for (std::size_t i = 0; i < kSlots; ++i)
        if (kSlotInfo[i].va) pool.push_back(kSlotInfo[i].va);
    for (std::size_t i = 0; i < kThunks; ++i) pool.push_back(kThunkTargets[i]);
    std::sort(pool.begin(), pool.end());
    pool.erase(std::unique(pool.begin(), pool.end()), pool.end());
    std::shuffle(pool.begin(), pool.end(), rng);
    Place p;
    std::size_t k = 0;
    // the MouseTurn fix exactly at the address it fixes, the canvas stubs at the addresses they wrap
    p.fix = 0x00431790;
    for (std::uint32_t a : g_canvas) p.canvas.push_back(a);
    std::vector<std::uint32_t> used = p.canvas;
    used.push_back(p.fix);
    std::sort(used.begin(), used.end());
    for (const Reg& r : c.regs) {
        if (p.rep.count(r.fn)) continue;
        while (std::binary_search(used.begin(), used.end(), pool[k])) ++k;
        p.rep[r.fn] = pool[k++];
    }
    return p;
}

// Both NEW runs must reach the same thing for every address: a trap, or the same pointer of ours (low -> high).
unsigned compare_placement(const Config& c, New& high, New& low, const std::vector<std::uint32_t>& relocs,
                           unsigned* compared, bool verbose) {
    std::map<std::uint32_t, std::uint32_t> back;  // low pointer -> high pointer
    for (const Reg& r : c.regs) back[static_cast<std::uint32_t>(low.place->rep_of(r))] = static_cast<std::uint32_t>(rep_ptr(r));
    back[low.place->fix] = kFix;
    for (std::size_t i = 0; i < g_canvas.size(); ++i) back[low.place->canvas_of(i)] = kHigh.canvas_of(i);
    unsigned differ = 0;
    auto one = [&](std::uint32_t va) {
        const std::uint32_t h = high.reach(va), l0 = low.reach(va);
        const auto it = back.find(l0);
        const std::uint32_t l = l0 == kTrap ? kTrap : it == back.end() ? 0xDEADBEEFu : it->second;
        ++*compared;
        if (h != l && differ++ < 20 && verbose)
            std::printf("  PLACEMENT DIFF 0x%08x: high 0x%08x, low 0x%08x (-> 0x%08x)\n", static_cast<unsigned>(va),
                        static_cast<unsigned>(h), static_cast<unsigned>(l0), static_cast<unsigned>(l));
    };
    for (std::size_t i = 0; i < kSlots; ++i)
        if (kSlotInfo[i].va) one(kSlotInfo[i].va);
    for (std::size_t i = 0; i < kThunks; ++i) one(kThunkFirst + 5u * static_cast<std::uint32_t>(i));
    for (std::size_t i = 0; i < kCount; ++i) one(kEntries[i].address);
    for (std::uint32_t v : relocs) one(v);
    for (std::size_t i = 0; i < high.canvas_target.size() && i < low.canvas_target.size(); ++i) {
        const std::uint32_t l0 = low.canvas_target[i];
        const auto it = back.find(l0);
        const std::uint32_t l = l0 == kTrap ? kTrap : it == back.end() ? 0xDEADBEEFu : it->second;
        ++*compared;
        if (high.canvas_target[i] != l) ++differ;
    }
    if (high.canvas_target.size() != low.canvas_target.size()) ++differ;
    return differ;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "usage: %s registrations.txt canvas.txt relocs.txt [random runs] [seed]\n", argv[0]);
        return 2;
    }
    Config live;
    live.regs = read_regs(argv[1]);
    g_canvas = read_hex(argv[2]);
    const std::vector<std::uint32_t> relocs = read_hex(argv[3]);
    const int runs = argc > 4 ? std::atoi(argv[4]) : 0;
    const unsigned seed = argc > 5 ? static_cast<unsigned>(std::atoi(argv[5])) : 1u;

    // Slot table sanity: every S5 thunk and entry, every relocation value has a slot; sorted index consistent.
    {
        New n;
        unsigned missing = 0, flag_changes = 0;
        for (std::size_t i = 0; i < kThunks; ++i)
            if (n.table.find(kThunkFirst + 5u * static_cast<std::uint32_t>(i)) < 0) ++missing;
        for (std::size_t i = 0; i < kCount; ++i) {
            const std::ptrdiff_t s = n.table.find(kEntries[i].address);
            if (s < 0) ++missing;
            else if ((kSlotInfo[s].flags & 0x7f) != kEntries[i].flags) {
                // The rule reads DATA / HOOK / ENTRY / THUNK; SELF / FUNC / REG differences are informational (the
                // comparison below shows whether any decision changed).
                const unsigned rule = s7::kData | s7::kHook | s7::kEntry | s7::kThunk;
                const bool relevant = ((kSlotInfo[s].flags ^ kEntries[i].flags) & rule) != 0;
                std::printf("  flags %s at 0x%08x: S5 0x%02x, slots 0x%02x\n", relevant ? "DIFFER (rule bits)" : "differ",
                            static_cast<unsigned>(kEntries[i].address), kEntries[i].flags, kSlotInfo[s].flags & 0x7fu);
                if (relevant) ++flag_changes;
            }
        }
        for (std::uint32_t v : relocs)
            if (n.table.find(v) < 0) ++missing;
        for (std::size_t i = 0; i < kThunks; ++i) {
            const std::ptrdiff_t s = n.table.find(kThunkFirst + 5u * static_cast<std::uint32_t>(i));
            if (s >= 0 && kSlotInfo[s].target != kThunkTargets[i]) ++missing;
        }
        std::printf("slot table: %u slots, %u S5 thunks, %u S5 entries, %u relocation values: %u missing / inconsistent; "
                    "%u entries whose rule flags changed since S5 (decided by the comparison below)\n",
                    static_cast<unsigned>(kSlots), static_cast<unsigned>(kThunks), static_cast<unsigned>(kCount),
                    static_cast<unsigned>(relocs.size()), missing, flag_changes);
        if (missing) return 1;
    }

    int failures = 0;
    {
        Old o;
        New n;
        o.run(live);
        n.run(live);
        std::printf("live configuration (%u registrations, %u installed): S5 finalize: thunks %u to C++ / %u to traps; "
                    "entries %u to replacements / %u traps (%u replaced bodies) / %u int3 (%u needed a jump)\n",
                    static_cast<unsigned>(live.regs.size()),
                    static_cast<unsigned>(std::count_if(live.regs.begin(), live.regs.end(), [](const Reg& r) { return r.installed; })),
                    o.st[5], o.st[6], o.st[0], o.st[1] + o.st[2], o.st[2], o.st[3] + o.st[4], o.st[4]);
        std::printf("  S7c slots: thunks %u to C++ / %u traps; entries %u to replacements / %u traps (%u replaced bodies); "
                    "%u unused\n", n.st[0], n.st[1], n.st[2], n.st[3] + n.st[4], n.st[4], n.st[5]);
        const Result r = compare(o, n, relocs, true);
        std::printf("  compared %u addresses: %u same trap, %u same replacement, %u DIFFERENT; %u S5-int3 entries now "
                    "reaching their replacement (by design)\n", r.compared, r.same_trap, r.same_ptr, r.differ, r.tight);
        failures += r.differ != 0;
        std::mt19937 prng(seed);
        const Place lp = low_place(live, prng);
        New nl;
        nl.place = &lp;
        nl.run(live);
        unsigned pc = 0;
        const unsigned pd = compare_placement(live, n, nl, relocs, &pc, true);
        std::printf("  S7d placement (every pointer of ours at an original code address): %u addresses compared, %u "
                    "DIFFERENT\n", pc, pd);
        failures += pd != 0;
    }
    std::mt19937 rng(seed);
    unsigned pl_total = 0, pl_diff = 0;
    unsigned total = 0, diff = 0, tight = 0;
    for (int k = 0; k < runs; ++k) {
        Config c = live;
        const unsigned drop = 1 + rng() % 60;  // percent of the registrations left out (disabled / not enabled)
        for (Reg& r : c.regs)
            if (rng() % 100 < drop) r.installed = false;
            else if (rng() % 100 < 2) r.installed = true;  // and a few that were off switched on
        // shared C++ functions: some registrations get the function of another one
        for (int s = 0; s < 40; ++s) c.regs[rng() % c.regs.size()].fn = c.regs[rng() % c.regs.size()].fn;
        c.mouse_turn = rng() % 2;
        c.canvas = rng() % 4 != 0;
        Old o;
        New n;
        o.run(c);
        n.run(c);
        const Result r = compare(o, n, relocs, k < 3);
        {
            const Place lp = low_place(c, rng);
            New nl;
            nl.place = &lp;
            nl.run(c);
            const unsigned pd = compare_placement(c, n, nl, relocs, &pl_total, k < 3);
            pl_diff += pd;
            if (pd) {
                ++failures;
                std::printf("  random run %d: S7d placement %u DIFFERENT\n", k, pd);
            }
        }
        total += r.compared;
        diff += r.differ;
        tight += r.tight;
        if (r.differ) {
            ++failures;
            std::printf("  random run %d (drop %u%%): %u DIFFERENT; S5 int3 entries that needed a jump: %u\n", k, drop,
                        r.differ, o.st[4]);
        }
    }
    if (runs) {
        std::printf("%d random configurations: %u comparisons, %u different, %u S5-int3 entries reaching their "
                    "replacement (by design)\n", runs, total, diff, tight);
        std::printf("%d random configurations, S7d placement: %u comparisons, %u different\n", runs, pl_total, pl_diff);
    }
    std::printf("%s\n", failures ? "FAILED" : "OK: every address resolves to the same replacement or a trap in both");
    return failures ? 1 : 0;
}
