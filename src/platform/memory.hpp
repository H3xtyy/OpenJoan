// Virtual memory and processor faults for our own code (memory.cpp): what the code table, the data image, the stray
// guard, the patchers and the heaps ask of the system, behind one interface - VirtualAlloc / VirtualProtect /
// VirtualQuery / FlushInstructionCache / vectored exception handlers on Windows, mmap / mprotect / /proc/self/maps /
// signal handlers elsewhere. (The game's own imports of these go through platform/process.cpp.)
#pragma once

#include <cstddef>
#include <cstdint>

/// Virtual memory and processor-fault handling behind one interface for every platform.
namespace joan::platform::memory {

/// Page access: a combination of these bits.
enum Access : unsigned { kNone = 0, kRead = 1, kWrite = 2, kExec = 4, kReadWrite = 3, kReadExec = 5, kAll = 7 };

/// Size in bytes of a page of this system.
std::size_t page_size();

/// Address space without access (nothing committed); at `at` when given (nullptr if that place is taken).
void* reserve(std::size_t n, void* at = nullptr);
/// Makes [p, p + n) of a reservation usable with `access`.
bool commit(void* p, std::size_t n, unsigned access);
/// reserve + commit; nullptr on failure.
void* allocate(std::size_t n, unsigned access, void* at = nullptr);
/// Gives the pages of [p, p + n) back (they stay reserved, without access).
bool decommit(void* p, std::size_t n);
/// Frees a whole reservation (p and n as reserve / allocate got them).
bool release(void* p, std::size_t n);

/// A page protection as the system keeps it (to put back exactly what was there).
struct Protection {
    std::uint32_t native = 0;  ///< The system's value (Windows PAGE_* constant; POSIX PROT_* bits).
};
/// Sets the access of [p, p + n); `previous` receives the protection of its first page.
bool protect(void* p, std::size_t n, unsigned access, Protection* previous = nullptr);
/// Puts a protection back (from protect's `previous`).
bool restore(void* p, std::size_t n, Protection previous);

/// [p, p + n) writable (and keeping its other access) while this lives: patches of code / read-only data.
class Writable {
   public:
    /// Makes the range writable with `access`; check ok().
    Writable(void* p, std::size_t n, unsigned access = kReadWrite);
    ~Writable();
    Writable(const Writable&) = delete;
    Writable& operator=(const Writable&) = delete;
    /// True when the protection change succeeded.
    bool ok() const { return ok_; }

   private:
    void* p_;
    std::size_t n_;
    Protection previous_;
    bool ok_;
};

/// After writing code: the processor sees the new bytes (p = nullptr: all of them).
void flush_code(const void* p, std::size_t n);

/// The mapping that holds an address.
struct Region {
    std::uintptr_t base = 0;  ///< Start of the run of pages with the same state around the address.
    std::uintptr_t size = 0;  ///< Length of that run in bytes.
    bool committed = false;   ///< Mapped (usable or not).
    unsigned access = kNone;  ///< Access bits (Access) of the run.
    bool guard = false;       ///< A guard page (Windows: the stack's growth page).
    bool private_memory = false;  ///< The process's own memory (heaps, stacks, allocations), not a file / image.
};
/// False if the address could not be asked about.
bool query(const void* p, Region& out);
/// Mapped, not a guard page, and with every bit of `need`.
bool accessible(const void* p, unsigned need = kRead);

/// The address range of our own executable's image.
void self_range(std::uintptr_t& low, std::uintptr_t& high);

// ---------------------------------------------------------------- faults
/// A processor fault on some thread, as a handler sees it.
struct Fault {
    /// Kind of fault.
    enum Type { kAccess, kBreakpoint, kIllegalInstruction, kOther } type = kOther;  ///< What happened.
    std::uint32_t code = 0;      ///< The system's number (Windows exception code / signal number).
    std::uintptr_t address = 0;  ///< kAccess: the address accessed.
    unsigned access = kRead;     ///< kAccess: kRead / kWrite / kExec.
    std::uintptr_t pc = 0;       ///< The instruction pointer at the fault.
    std::uintptr_t sp = 0;       ///< The stack pointer at the fault.
    void* native = nullptr;      ///< EXCEPTION_POINTERS* (Windows) / ucontext_t* (POSIX): register dumps.
};
/// What a fault handler decides: kPass (not this handler's, the next one decides; in the end the system: the process ends) or kResume (fixed, run on at the possibly changed context).
enum class FaultAction {
    kPass,  ///< not this handler's: the next one decides (in the end the system: the process ends)
    kResume,  ///< the handler fixed it: run on at the (possibly changed) context
};
/// A fault handler: gets the fault and returns its decision; installed with add_fault_handler().
using FaultHandler = FaultAction (*)(const Fault&);
/// Installs `h` in front of every handler installed before it (the newest runs first). Up to 8 handlers.
bool add_fault_handler(FaultHandler h);
/// Ends the process at once with `code` (no exit handlers: what a fault handler that reported a fatal error does).
[[noreturn]] void end_process(unsigned code);

}  // namespace joan::platform::memory
