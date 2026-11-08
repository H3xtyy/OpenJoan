// The code slot table (docs/RUNTIME.md) - see code_table.hpp (order of calls), code.hpp (how our C++
// uses it) and code_slots_core.hpp (slot data, install operations, resolution rule).
#include "../callconv.hpp"
#include "code_table.hpp"
#include "../platform/memory.hpp"
#include "../platform/window.hpp"

#include "../platform/win32_types.hpp"
#include "../compiler.hpp"

#include <algorithm>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

#include "code.hpp"
#include "code_slots_core.hpp"
#include "crash.hpp"
#include "host.hpp"
#include "image_runtime.hpp"
#include "patch.hpp"
#include "../registry.hpp"

namespace {

using joan::code::SlotInfo;

#define JOAN_CODE_SLOTS
const SlotInfo kSlotInfo[] = {
#include "code_slots.inc"
};
#undef JOAN_CODE_SLOTS
#define JOAN_CODE_SLOT_NAMES
const char* const kSlotNames[] = {
#include "code_slots.inc"
};
#undef JOAN_CODE_SLOT_NAMES
#define JOAN_CODE_SLOTS_BY_VA
const std::uint32_t kSlotsByVa[] = {
#include "code_slots.inc"
};
#undef JOAN_CODE_SLOTS_BY_VA
#define JOAN_CODE_ALIAS_GROUPS
const std::uint32_t kAliasGroups[] = {
#include "code_slots.inc"
};
#undef JOAN_CODE_ALIAS_GROUPS

constexpr std::size_t kSlots = sizeof kSlotInfo / sizeof kSlotInfo[0];
constexpr std::size_t kNamed = sizeof kSlotNames / sizeof kSlotNames[0];
constexpr std::size_t kByVa = sizeof kSlotsByVa / sizeof kSlotsByVa[0];
static_assert(kSlots == JOAN_CODE_SLOT_COUNT && kNamed == JOAN_CODE_NAMED_SLOTS, "code_slots.inc");

// One page-aligned block of whole pages: made read-only at seal() without touching other data.
constexpr std::size_t kTableEntries = (kSlots + 1023) / 1024 * 1024;

}  // namespace

extern "C" {
alignas(4096) std::uintptr_t joan_code[kTableEntries] = {};
}

namespace joan::code_table {
namespace {

std::uintptr_t g_current[kSlots];  // install-time state (code_slots_core.hpp Slots)
std::uint8_t g_ours[kSlots];
constinit code::Slots g_table(kSlotInfo, kSlots, g_current, g_ours, kSlotsByVa, kByVa);
std::uint8_t* g_stubs = nullptr;   // kSlots trap stubs (push slot; call trap_hit)
bool g_begun = false;
std::atomic<bool> g_sealed{false};
Stats g_stats;

// Resolved pointer -> original address (saves: fn::original_code_address), built at seal: sorted by pointer, thunk
// slots first, then by slot index.
struct Back {
    std::uintptr_t pointer;
    std::uint32_t va;
};
std::vector<Back> g_back;

// JOAN_LATE_CODE registrations (constructed during static initialisation: a plain pointer, constant initialised).
fn::LateCode* g_late = nullptr;

std::uint32_t u32(std::uintptr_t v) { return static_cast<std::uint32_t>(v); }
std::uintptr_t stub_of(std::size_t slot) { return reinterpret_cast<std::uintptr_t>(g_stubs + slot * code::kTrapStubSize); }
std::uintptr_t trap_or_zero(std::size_t slot) { return g_stubs ? stub_of(slot) : 0; }

/// Best name of a slot: its fn:: name, else the registered name of its address.
const char* name_of_va(std::uint32_t va) {
    if (const hook::Replacement* r = hook::registration(va)) return r->name;
    return "";
}
const char* name_of_slot(std::size_t slot) {
    if (slot < kNamed && kSlotNames[slot][0]) return kSlotNames[slot];
    return name_of_va(kSlotInfo[slot].va);
}

// ---------------------------------------------------------------- traps

struct Fired {
    std::uint32_t slot;  // kNoSlot: a computed address without a slot (stray_code_address)
    std::uintptr_t caller;
    std::uint32_t value;
};
constexpr std::uint32_t kNoSlot = 0xFFFFFFFFu;
constexpr unsigned kFiredMax = 16;
Fired g_fired[kFiredMax];
std::atomic<unsigned> g_fired_count{0};
std::atomic<bool> g_ending{false};

void describe_trap(const Fired& f, char* out, std::size_t size) {
    if (f.slot == kNoSlot) {
        std::snprintf(out, size, "original code needed: computed address 0x%08x (inside joan.exe's old code range, "
                      "no slot: not a function entry; called from 0x%08llx)", static_cast<unsigned>(f.value),
                      static_cast<unsigned long long>(f.caller));
        return;
    }
    const SlotInfo& s = kSlotInfo[f.slot];
    if (s.flags & code::kSlotThunk) {
        const hook::Replacement* r = hook::registration(s.target);
        std::snprintf(out, size, "original code needed: 0x%08x -> 0x%08x %s (thunk; %s; called from 0x%08llx)",
                      static_cast<unsigned>(s.va), static_cast<unsigned>(s.target),
                      r ? r->name : name_of_slot(f.slot),
                      r ? "registered but not installed: switched off ([Hooks] Disable / [Fixes] UninitReads=0)" : "not replaced",
                      static_cast<unsigned long long>(f.caller));
    } else {
        const bool replaced = hook::installed_replacement(s.va) != nullptr;
        std::snprintf(out, size, "original code needed: 0x%08x %s (entry; %s; called from 0x%08llx)",
                      static_cast<unsigned>(s.va), name_of_slot(f.slot),
                      replaced ? "body of a replaced function reached by its real address (fallback / verify call)"
                      : hook::registration(s.va) ? "registered but not installed: switched off ([Hooks] Disable / [Fixes] UninitReads=0)"
                                                 : "not replaced",
                      static_cast<unsigned long long>(f.caller));
    }
}

[[noreturn]] void fire(const Fired& f) {
    const unsigned n = g_fired_count.fetch_add(1);
    if (n < kFiredMax) g_fired[n] = f;
    char text[512];
    describe_trap(f, text, sizeof text);
    hook::log_line("%s", text);
    if (g_ending.exchange(true)) {
        for (;;) std::this_thread::sleep_for(std::chrono::hours(1));  // another thread is already ending the process
    }
    host::shutdown();  // call statistics + write_report (lists this trap)
    char box[640];
    std::snprintf(box, sizeof box, "%s\n\nThe game reached joan.exe code that has no C++ replacement. See " JOAN_RE_LOG
                  " in the game folder.", text);
    joan::platform::error_box("Joan of Arc (OpenJoan)", box);
    std::_Exit(3);
}

// Called by every trap stub (push slot; call trap_hit): `caller` is the dword that was on top of the stack when the
// slot was called - the return address of the call.
[[noreturn]] void JOAN_CDECL trap_hit(std::uint32_t slot, std::uintptr_t caller) { fire({slot, caller, 0}); }

// Target of a computed address without a slot (stray_code_address): the value is the last one reported.
std::atomic<std::uint32_t> g_unknown_value{0};
JOAN_NOINLINE void JOAN_CDECL unknown_code_trap() {
    fire({kNoSlot, reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS()), g_unknown_value.load()});
}

// ---------------------------------------------------------------- reads before seal, stray code addresses

struct Seen {
    std::uintptr_t value;
    std::uintptr_t caller;
};
constexpr std::size_t kSeenMax = 256;
Seen g_early[kSeenMax];
std::size_t g_early_count = 0;
Seen g_stray[kSeenMax];
std::size_t g_stray_count = 0;
std::shared_mutex g_seen_lock;

bool remember(Seen* list, std::size_t& count, std::uintptr_t value, std::uintptr_t caller) {
    g_seen_lock.lock();
    bool first = true;
    for (std::size_t i = 0; i < count && first; ++i) first = !(list[i].value == value && list[i].caller == caller);
    if (first && count < kSeenMax) list[count++] = {value, caller};
    g_seen_lock.unlock();
    return first;
}

}  // namespace

void begin() {
    if (g_begun) return;
    g_begun = true;
    const std::size_t bytes = kSlots * code::kTrapStubSize;
    g_stubs = static_cast<std::uint8_t*>(platform::memory::allocate(bytes, platform::memory::kReadWrite));
    if (!g_stubs) {
        hook::log_line("code: could not allocate the trap stubs");
        return;
    }
    const auto handler = reinterpret_cast<std::uintptr_t>(&trap_hit);
    for (std::size_t i = 0; i < kSlots; ++i)
        code::put_trap_stub(g_stubs + i * code::kTrapStubSize, stub_of(i), static_cast<std::uint32_t>(i), handler);
    platform::memory::protect(g_stubs, bytes, platform::memory::kReadExec);
    platform::memory::flush_code(g_stubs, bytes);
    g_table.reset();
}

bool sealed() { return g_sealed.load(std::memory_order_acquire); }

void seal() {
    if (sealed()) return;
    begin();
    Stats& st = g_stats;
    st = {};
    st.slots = static_cast<unsigned>(kSlots);
    // Every registered code address with its installed replacement, sorted (one pass over the registry).
    struct Known {
        std::uint32_t va;
        const hook::Replacement* reg;
        const void* rep;
        bool operator<(const Known& o) const { return va < o.va; }
    };
    std::vector<Known> known;
    for (const hook::Replacement& r : hook::replacements())
        if (!r.iat_saved) known.push_back({u32(r.original), &r, nullptr});
    std::stable_sort(known.begin(), known.end());
    known.erase(std::unique(known.begin(), known.end(), [](const Known& a, const Known& b) { return a.va == b.va; }),
                known.end());
    for (Known& k : known) k.rep = hook::installed_replacement(k.va);
    auto lookup = [&](std::uint32_t va) -> const Known* {
        const auto it = std::lower_bound(known.begin(), known.end(), Known{va, nullptr, nullptr});
        return it != known.end() && it->va == va ? &*it : nullptr;
    };
    for (std::size_t i = 0; i < kSlots; ++i) {
        code::Resolution how{};
        joan_code[i] = g_table.resolve(
            i,
            [&](std::uint32_t va) -> std::uintptr_t {
                const Known* k = lookup(va);
                return k && k->rep ? reinterpret_cast<std::uintptr_t>(k->rep) : 0;
            },
            [](std::size_t s) { return trap_or_zero(s); }, &how);
        const SlotInfo& s = kSlotInfo[i];
        switch (how) {
            case code::Resolution::thunk_code: ++st.thunks_to_code; break;
            case code::Resolution::thunk_trap:
                ++st.thunk_traps;
                if (const Known* k = lookup(s.target)) {
                    ++st.thunk_traps_registered;
                    hook::log_line("code: thunk 0x%08x -> 0x%08x %s [%s] leads to a trap (registered, not installed)",
                                   static_cast<unsigned>(s.va), static_cast<unsigned>(s.target), k->reg->name,
                                   k->reg->module);
                }
                break;
            case code::Resolution::entry_replacement: ++st.entries_to_replacement; break;
            case code::Resolution::entry_trap:
                ++st.entry_traps;
                if ((s.flags & (code::kData | code::kHook)) && lookup(s.va))
                    hook::log_line("code: entry 0x%08x %s is referenced but its replacement is not installed - trap",
                                   static_cast<unsigned>(s.va), name_of_slot(i));
                break;
            case code::Resolution::entry_trap_replaced:
                ++st.entry_traps;
                ++st.entry_traps_replaced;
                break;
            case code::Resolution::unused: ++st.unused; break;
        }
    }
    g_back.reserve(kSlots);
    for (std::size_t i = 0; i < kSlots; ++i)
        if (kSlotInfo[i].va) g_back.push_back({joan_code[i], kSlotInfo[i].va});
    std::stable_sort(g_back.begin(), g_back.end(), [](const Back& a, const Back& b) {
        return a.pointer != b.pointer ? a.pointer < b.pointer : code::is_thunk(a.va) > code::is_thunk(b.va);
    });
    g_sealed.store(true, std::memory_order_release);

    // JOAN_LATE_CODE variables (inline asm call targets).
    for (fn::LateCode* l = g_late; l; l = l->next) {
        *l->variable = l->code != fn::kNoCode ? joan_code[static_cast<std::uint32_t>(l->code)]
                                              : fn::resolved_code_address(l->raw_va);
        if (*l->variable == 0) hook::log_line("code: ERROR JOAN_LATE_CODE_VA 0x%08x has no slot", static_cast<unsigned>(l->raw_va));
        ++st.late_code;
    }

    // Alias groups: code values one comparison of our C++ tells apart must stay distinct.
    for (std::size_t i = 0; i < sizeof kAliasGroups / sizeof kAliasGroups[0];) {
        std::size_t end = i;
        while (kAliasGroups[end] != 0xFFFFFFFFu) ++end;
        for (std::size_t a = i; a < end; ++a)
            for (std::size_t b = a + 1; b < end; ++b) {
                const std::uint32_t sa = kAliasGroups[a], sb = kAliasGroups[b];
                if (kSlotInfo[sa].va != kSlotInfo[sb].va && joan_code[sa] == joan_code[sb]) {
                    ++st.alias_errors;
                    hook::log_line("code: ERROR alias: 0x%08x %s and 0x%08x %s both resolve to 0x%08x - a comparison of "
                                   "these code values can no longer tell them apart",
                                   static_cast<unsigned>(kSlotInfo[sa].va), name_of_slot(sa),
                                   static_cast<unsigned>(kSlotInfo[sb].va), name_of_slot(sb),
                                   static_cast<unsigned>(joan_code[sa]));
                }
            }
        i = end + 1;
    }

    {
        g_seen_lock.lock_shared();
        st.early_reads = static_cast<unsigned>(g_early_count);
        for (std::size_t i = 0; i < g_early_count; ++i) {
            hook::log_line("code: ERROR code value of slot %u (0x%08x %s) was read before the code table was sealed, "
                           "at 0x%08llx %s", static_cast<unsigned>(g_early[i].value),
                           static_cast<unsigned>(kSlotInfo[g_early[i].value].va), name_of_slot(g_early[i].value),
                           static_cast<unsigned long long>(g_early[i].caller), hook::describe_address(g_early[i].caller).c_str());
        }
        g_seen_lock.unlock_shared();
    }

    const bool locked = platform::memory::protect(joan_code, sizeof joan_code, platform::memory::kRead);
    hook::log_line("code slots: %u filled / %u traps (thunks: %u to C++, %u to traps of which %u registered but not "
                   "installed; entries: %u to replacements, %u traps of which %u replaced bodies; %u unused); %u "
                   "JOAN_LATE_CODE; %u early reads, %u alias errors; table at 0x%08llx %s, trap stubs at "
                   "0x%08llx",
                   st.thunks_to_code + st.entries_to_replacement, st.thunk_traps + st.entry_traps + st.unused,
                   st.thunks_to_code, st.thunk_traps, st.thunk_traps_registered, st.entries_to_replacement,
                   st.entry_traps, st.entry_traps_replaced, st.unused, st.late_code, st.early_reads,
                   st.alias_errors, static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(joan_code)),
                   locked ? "read-only" : "(NOT read-only)", static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(g_stubs)));
}

std::ptrdiff_t slot_of(std::uintptr_t va) { return g_table.find(va); }

std::uintptr_t resolve(std::uintptr_t va) {
    const std::ptrdiff_t s = g_table.find(va);
    return s < 0 ? 0 : joan_code[s];
}

int count_thunks_to(std::uintptr_t target) { return g_table.count_thunks_to(target); }
std::uintptr_t entry_patch(std::uintptr_t original) { return g_table.entry_patch(original); }

std::uintptr_t entry_target(std::uintptr_t original) {
    const std::ptrdiff_t s = g_table.find(original);
    if (s < 0 || (kSlotInfo[s].flags & code::kSlotThunk)) return 0;
    return g_table.resolve(
        static_cast<std::size_t>(s),
        [](std::uint32_t va) { return reinterpret_cast<std::uintptr_t>(hook::installed_replacement(va)); },
        [](std::size_t i) { return trap_or_zero(i); });
}

void describe(std::uintptr_t value, char* out, std::size_t size) {
    out[0] = 0;
    if (g_stubs && value >= stub_of(0) && value < stub_of(kSlots)) {
        const std::size_t s = (value - stub_of(0)) / code::kTrapStubSize;
        std::snprintf(out, size, "trap stub of slot %u (0x%08x %s)", static_cast<unsigned>(s),
                      static_cast<unsigned>(kSlotInfo[s].va), name_of_slot(s));
        return;
    }
    // An original code address only if no code is mapped there (stage S7d: with ASLR our own code may lie inside
    // joan.exe's old code range; a value there that is executable memory is a pointer of ours or of a DLL).
    if (code::in_old_code(value) && !image_runtime::old_range_mapped(value, true)) {
        const std::ptrdiff_t s = g_table.find(value);
        if (s >= 0)
            std::snprintf(out, size, "original code address, slot %d (%s%s)", static_cast<int>(s), name_of_slot(s),
                          (kSlotInfo[s].flags & code::kSlotThunk) ? ", thunk" : "");
        else
            std::snprintf(out, size, "original code address without a slot");
        return;
    }
    // The nearest replacement below the address, within 1 MB: farther away the address is not in our code (a system
    // library's, on x64 the first frame-pointer walks named libc frames after a game function billions of bytes away).
    if (const hook::Replacement* r = hook::nearest_replacement(value)) {
        const std::uintptr_t distance = value - reinterpret_cast<std::uintptr_t>(r->replacement);
        if (distance < (static_cast<std::uintptr_t>(1) << 20))
            std::snprintf(out, size, "%s +0x%x", r->name, static_cast<unsigned>(distance));
    }
}

const Stats& stats() { return g_stats; }

void write_report() {
    const unsigned n = g_fired_count.load();
    hook::log_line("---- code: %u trap%s fired", n, n == 1 ? "" : "s");
    for (unsigned i = 0; i < n && i < kFiredMax; ++i) {
        char text[512];
        describe_trap(g_fired[i], text, sizeof text);
        hook::log_line("code: %s", text);
    }
    g_seen_lock.lock_shared();
    const std::size_t strays = g_stray_count;
    g_seen_lock.unlock_shared();
    hook::log_line("---- code: %u stray code address%s (original code addresses called through a raw value)",
                   static_cast<unsigned>(strays), strays == 1 ? "" : "es");
}

std::uintptr_t stubs_begin() { return reinterpret_cast<std::uintptr_t>(g_stubs); }
std::uintptr_t stubs_end() { return g_stubs ? stub_of(kSlots) : 0; }

}  // namespace joan::code_table

namespace joan::fn {

namespace ct = joan::code_table;

LateCode::LateCode(std::uintptr_t* v, Code c) noexcept : variable(v), code(c), raw_va(0), next(ct::g_late) {
    ct::g_late = this;
}
LateCode::LateCode(std::uintptr_t* v, std::uint32_t original_va) noexcept
    : variable(v), code(kNoCode), raw_va(original_va), next(ct::g_late) {
    ct::g_late = this;
}

std::uint32_t va(Code c) noexcept {
    const auto i = static_cast<std::uint32_t>(c);
    return i < kSlots ? kSlotInfo[i].va : 0;
}

std::uint32_t original_code_address(std::uintptr_t pointer) noexcept {
    if (!ct::sealed() || pointer == 0) return 0;
    const auto it = std::lower_bound(ct::g_back.begin(), ct::g_back.end(), pointer,
                                     [](const ct::Back& b, std::uintptr_t p) { return b.pointer < p; });
    return it != ct::g_back.end() && it->pointer == pointer ? it->va : 0;
}

std::uintptr_t resolved_code_address(std::uint32_t original) noexcept {
    if (!ct::sealed()) return 0;
    const std::ptrdiff_t s = ct::g_table.find(original);
    return s < 0 ? 0 : joan_code[s];
}

JOAN_NOINLINE std::uintptr_t early_code_value(Code c) noexcept {
    const auto slot = static_cast<std::uint32_t>(c);
    const auto caller = reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS());
    if (slot >= kSlots) return 0;
    if (ct::remember(ct::g_early, ct::g_early_count, slot, caller) && ct::sealed())
        hook::log_line("code: ERROR code value of slot %u (0x%08x %s) is 0 at 0x%08llx", static_cast<unsigned>(slot),
                       static_cast<unsigned>(kSlotInfo[slot].va), ct::name_of_slot(slot),
                       static_cast<unsigned long long>(caller));
    return ct::trap_or_zero(slot);
}

JOAN_NOINLINE std::uintptr_t stray_code_address(std::uintptr_t raw) noexcept {
    // Stage S7d: nothing is reserved at joan.exe's old addresses any more, and with ASLR our own code (or a DLL's, or
    // a trap stub) can lie in the old code range. A value there that points at executable memory is such a pointer
    // and is called as it is; only a value where no code is mapped (nothing at all, or data: DEP would fault) is an
    // original code address. Known code pages are remembered (one bit test after the first query).
    if (image_runtime::old_range_mapped(raw, true)) return raw;
    const auto caller = reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS());
    const std::ptrdiff_t s = ct::g_table.find(raw);
    if (ct::remember(ct::g_stray, ct::g_stray_count, raw, caller)) {
        char where[200];
        ct::describe(caller, where, sizeof where);
        hook::log_line("stray code address 0x%08x (%s) called through a raw value at 0x%08llx (%s) - %s",
                       static_cast<unsigned>(raw), s >= 0 ? ct::name_of_slot(static_cast<std::size_t>(s)) : "no slot",
                       static_cast<unsigned long long>(caller), where,
                       s >= 0 ? "resolved through its slot, game continues" : "no slot: trap");
    }
    if (s < 0) {
        ct::g_unknown_value.store(static_cast<std::uint32_t>(raw));
        return reinterpret_cast<std::uintptr_t>(&ct::unknown_code_trap);
    }
    const std::uintptr_t v = joan_code[s];
    return v != 0 ? v : ct::trap_or_zero(static_cast<std::size_t>(s));
}

}  // namespace joan::fn

namespace joan::hook {

// patch.hpp: the install operations of stage S5, now on slots (code_slots_core.hpp).
int redirect_function(std::uintptr_t original, const void* replacement) {
    if (code_table::sealed()) {
        log_line("code: ERROR redirect_function(0x%08x) after seal - ignored (the data image and the late "
                 "initialisers already hold the resolved pointers)", static_cast<unsigned>(original));
        return 0;
    }
    code_table::begin();
    return code_table::g_table.redirect(original, reinterpret_cast<std::uintptr_t>(replacement));
}

int redirect_replacement(const void* current, const void* replacement) {
    const auto pointer = reinterpret_cast<std::uintptr_t>(current);
    if (code_table::sealed()) {
        log_line("code: ERROR redirect_replacement(0x%08x) after seal - ignored (the data image and the late "
                 "initialisers already hold the resolved pointers)", static_cast<unsigned>(pointer));
        return 0;
    }
    code_table::begin();
    return code_table::g_table.redirect_ours(pointer, reinterpret_cast<std::uintptr_t>(replacement));
}

bool patch_entry(std::uintptr_t original, const void* replacement) {
    if (code_table::sealed()) return false;
    code_table::begin();
    return code_table::g_table.patch_entry(original, reinterpret_cast<std::uintptr_t>(replacement));
}

}  // namespace joan::hook
