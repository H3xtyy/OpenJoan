// Runtime side of the relocatable data image (docs/RUNTIME.md) - see image_runtime.hpp.
#include "image_runtime.hpp"
#include "../platform/memory.hpp"
#include "../platform/window.hpp"

#include "../platform/win32_types.hpp"
#include "../compiler.hpp"

#include <atomic>
#include <shared_mutex>
#include <cstdio>

#include "code.hpp"
#include "code_table.hpp"
#include "crash.hpp"
#include "image.hpp"
#include "../registry.hpp"

// The data relocation must happen before any other static constructor of ours could read a pointer out of the
// image: the compiler segment runs before the library and user segments (C4074: this is on purpose). Every other
// object of this file is constant-initialised.
#if defined(_MSC_VER)
#pragma warning(disable : 4074)
#pragma init_seg(compiler)
#endif

namespace joan::image_runtime {
namespace {

Startup g_startup;

// Runs from the static initialiser below, before every other static constructor of ours (compiler segment): the
// image is still exactly as the compiler laid it out, nothing has read a pointer out of it yet.
void run_startup() {
    if (g_startup.ran) return;
    g_startup.ran = true;
    g_startup.check = data_image::check_sections();
    g_startup.relocs = data_image::relocate(joan_image, img::base());
}

// ---------------------------------------------------------------- stray image addresses (raw overloads)

struct Seen {
    std::uintptr_t address;
    std::uintptr_t caller;
};
constexpr std::size_t kSeenMax = 256;
Seen g_seen[kSeenMax];
std::size_t g_seen_count = 0;
std::shared_mutex g_seen_lock;

// "s_ANIM_9cf034+0x2" for an original image address.
void describe_data(std::uintptr_t va, char* out, std::size_t size) {
    std::uint32_t start = 0;
    const char* name = data_image::symbol_at(static_cast<std::uint32_t>(va), &start);
    if (name)
        std::snprintf(out, size, "%s+0x%x", name, static_cast<unsigned>(va - start));
    else
        std::snprintf(out, size, "?");
}

void describe_code(std::uintptr_t eip, char* out, std::size_t size) {
    const std::string where = hook::describe_address(eip);
    char what[200];
    code_table::describe(eip, what, sizeof what);
    if (what[0])
        std::snprintf(out, size, "%s, %s", where.c_str(), what);
    else
        std::snprintf(out, size, "%s", where.c_str());
}

// ---------------------------------------------------------------- mapped pages of the old range

constexpr std::size_t kOldPages = (kOldHigh - kOldLow) >> 12;
constexpr std::size_t kWords = (kOldPages + 31) / 32;
// Bit per page of the old range: known to be mapped (accessible) / mapped and executable. Only ever set: a heap or
// image page of ours does not go away while the game runs (and a pointer into one that did is no original address).
std::atomic<std::uint32_t> g_mapped[kWords];
std::atomic<std::uint32_t> g_mapped_exec[kWords];


// openjoan.exe itself (set by install_stray_guard): a fault inside our own image is a bug of ours, not a stray.
std::uintptr_t g_self_low = 0, g_self_high = 0;

// ---------------------------------------------------------------- the guard

std::atomic<bool> g_guard_fired{false};
thread_local int t_probing = 0;  // ProbeScope depth on this thread

platform::memory::FaultAction on_stray_access(const platform::memory::Fault& f) {
    namespace mem = platform::memory;
    if (f.type != mem::Fault::kAccess) return mem::FaultAction::kPass;
    if (t_probing != 0) return mem::FaultAction::kPass;  // a ProbeScope: the probe's own __except handles it
    const std::uintptr_t va = f.address;
    if (va < kOldLow || va >= kOldHigh) return mem::FaultAction::kPass;
    if (va - g_self_low < g_self_high - g_self_low) return mem::FaultAction::kPass;  // our own image: crash handler
    const bool executing = f.access == mem::kExec;
    // Stage S7d: nothing is reserved there. A stray original address faults only where nothing is mapped - or, for a
    // call / jump into the old code range, where what is mapped is no code (DEP). Any other fault at these addresses
    // (memory of ours that happens to lie here, used wrongly) is an ordinary crash: crash handler.
    mem::Region region;
    const bool committed = mem::query(reinterpret_cast<const void*>(va), region) && region.committed &&
                           region.access != mem::kNone && !region.guard;
    const bool code_range = va - fn::kOldCodeBegin < fn::kOldCodeEnd - fn::kOldCodeBegin;
    if (committed && !(executing && code_range && !(region.access & mem::kExec))) return mem::FaultAction::kPass;
    if (g_guard_fired.exchange(true)) {
        for (;;) platform::sleep_ms(1000);  // another thread is already reporting
    }
    const std::uintptr_t eip = f.pc;
    const char* const how = f.access == mem::kRead ? "reading" : f.access == mem::kWrite ? "writing" : "executing";
    char what[200], code[320], text[700];
    const char* why;
    if (img::in_old_range(va)) {
        describe_data(va, what, sizeof what);
        describe_code(eip, code, sizeof code);
        std::snprintf(text, sizeof text,
                      "stray absolute data access: %s 0x%08x (original image address, %s; now at 0x%08llx) at EIP 0x%08llx (%s)",
                      how, static_cast<unsigned>(va), what, static_cast<unsigned long long>(img::base() + img::offset_of(static_cast<std::uint32_t>(va))),
                      static_cast<unsigned long long>(eip), code);
        why = "The game read or wrote joan.exe's data at its old fixed address, which no longer exists.";
    } else if (code_range) {
        // Executing there: a call / jump to an original code address that bypassed the code table (EIP = va; the
        // caller's return address is on top of the stack). Reading / writing: code bytes nobody has any more.
        code_table::describe(va, what, sizeof what);
        std::uintptr_t from = eip;
        if (executing && eip == va) {
            const auto* sp = reinterpret_cast<const std::uintptr_t*>(f.sp);
            if (mem::accessible(sp, mem::kRead)) from = *sp;
        }
        describe_code(from, code, sizeof code);
        std::snprintf(text, sizeof text, "stray code address: %s 0x%08x (%s%s) %s 0x%08llx (%s)", how,
                      static_cast<unsigned>(va), what[0] ? what : "joan.exe's old code range",
                      committed ? "; data of ours is mapped there now: DEP" : "",
                      executing && eip == va ? "called from" : "at EIP", static_cast<unsigned long long>(from), code);
        why = "The game used joan.exe's code at its old fixed address, which no longer exists (a code address that did "
              "not go through the code table).";
    } else {
        describe_code(eip, code, sizeof code);
        std::snprintf(text, sizeof text, "stray access to joan.exe's old address range: %s 0x%08x (%s) at EIP 0x%08llx (%s)",
                      how, static_cast<unsigned>(va),
                      va < fn::kOldCodeBegin ? "its PE header: GetModuleHandle(NULL) is openjoan.exe now"
                                             : va >= img::kEnd ? ".rsrc / .reloc" : "padding after .text",
                      static_cast<unsigned long long>(eip), code);
        why = "The game accessed joan.exe's old address range, which no longer exists.";
    }
    hook::log_line("%s", text);
    char box[1100];
    std::snprintf(box, sizeof box, "%s\n\n%s See " JOAN_RE_LOG " in the game folder.", text, why);
    joan::platform::error_box("Joan of Arc (OpenJoan)", box);
    platform::memory::end_process(4);
    return platform::memory::FaultAction::kPass;
}

}  // namespace

const Startup& startup() {
    run_startup();
    return g_startup;
}

ProbeScope::ProbeScope() noexcept { ++t_probing; }
ProbeScope::~ProbeScope() { --t_probing; }

void install_stray_guard() {
    platform::memory::self_range(g_self_low, g_self_high);
    platform::memory::add_fault_handler(&on_stray_access);
}

bool old_range_mapped(std::uintptr_t v, bool execute) noexcept {
    const std::uintptr_t page = (v - kOldLow) >> 12;
    if (page >= kOldPages) return false;
    std::atomic<std::uint32_t>* const bits = execute ? g_mapped_exec : g_mapped;
    if (bits[page >> 5].load(std::memory_order_relaxed) & (1u << (page & 31))) return true;
    namespace mem = platform::memory;
    mem::Region r;
    if (!mem::query(reinterpret_cast<const void*>(v), r) || !r.committed || r.access == mem::kNone || r.guard ||
        (execute && !(r.access & mem::kExec)))
        return false;
    // The whole region the query reported has these attributes: remember all of its pages inside the old range.
    const std::uintptr_t region = r.base;
    const std::uintptr_t first = region <= kOldLow ? 0 : (region - kOldLow) >> 12;
    const std::uintptr_t region_end = region + r.size;  // page aligned; may wrap only at 4 GB (not in 32-bit user space)
    const std::uintptr_t last = region_end >= kOldHigh ? kOldPages : (region_end - kOldLow) >> 12;
    for (std::uintptr_t p = first; p < last; ++p) bits[p >> 5].fetch_or(1u << (p & 31), std::memory_order_relaxed);
    return true;
}

std::size_t stray_count() {
    g_seen_lock.lock_shared();
    const std::size_t n = g_seen_count;
    g_seen_lock.unlock_shared();
    return n;
}

}  // namespace joan::image_runtime

namespace joan::img {

JOAN_NOINLINE std::uintptr_t stray_image_address(std::uintptr_t raw) noexcept {
    namespace ir = joan::image_runtime;
    // Stage S7d: nothing is reserved at joan.exe's old data addresses. With ASLR joan_image itself, the rest of our
    // image, the heap or a DLL can lie there: a value that points at mapped memory is such a pointer and is used as
    // it is. Only a value where nothing is mapped (an access would fault) is an original image address.
    if (in_image(raw) || ir::old_range_mapped(raw, false)) return raw;
    const auto caller = reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS());
    bool first = true;
    ir::g_seen_lock.lock();
    for (std::size_t i = 0; i < ir::g_seen_count && first; ++i)
        first = !(ir::g_seen[i].address == raw && ir::g_seen[i].caller == caller);
    if (first && ir::g_seen_count < ir::kSeenMax) ir::g_seen[ir::g_seen_count++] = {raw, caller};
    ir::g_seen_lock.unlock();
    if (first) {
        char data[160], code[320];
        ir::describe_data(raw, data, sizeof data);
        ir::describe_code(caller, code, sizeof code);
        joan::hook::log_line("stray image address 0x%08x (%s) used through a raw image.hpp helper at 0x%08llx (%s) - "
                             "rebased, game continues",
                             static_cast<unsigned>(raw), data, static_cast<unsigned long long>(caller), code);
    }
    return base() + offset_of(static_cast<std::uint32_t>(raw));
}

}  // namespace joan::img

// Runs in the compiler segment (pragma at the top of this file); GCC / Clang (MinGW, Linux): the first priority
// available to programs, before every constructor without one.
namespace {
struct ImageStartup {
    ImageStartup() { joan::image_runtime::startup(); }
};
#if defined(_MSC_VER)
ImageStartup g_image_startup;
#else
__attribute__((init_priority(101))) ImageStartup g_image_startup;
#endif
}  // namespace
