// The code slot table joan_code[] (docs/RUNTIME.md; src/code.hpp, src/code_slots_core.hpp): what a
// call of each original joan.exe code address reaches, now that no joan.exe code exists at run time.
//
// Order in openjoan.exe (loader/joan_re.cpp, host.cpp):
//   begin()  (start of host::start: trap stubs, thunk slots at their original targets)
//   -> install_all (redirect_function / patch_entry), fixes, display, canvas hooks, d3d8 hook
//   -> seal()  (end of host::start: every slot resolved to its replacement or trap stub, JOAN_LATE_CODE variables
//      assigned, alias check, the table made read-only)
//   -> the loader points the data image's code pointers at their slots' targets (resolve()), protects .rdata
//   -> the game (CRT entry through its slot).
// A trap stub reached ends the game with "original code needed: <address> <name> ..." in openjoan.log and a message
// box, as stage S5's generated code range did.
#pragma once

#include <cstddef>
#include <cstdint>

/// The code slot table joan_code[]: begin / seal of the table and the resolution of every original code address.
namespace joan::code_table {

/// Start of host::start: allocates the trap stubs, puts every thunk slot at its original target.
void begin();
/// End of host::start (after every replacement / fix / wrapper is in place): resolves every slot, assigns the
/// JOAN_LATE_CODE variables, checks the alias groups, logs the counts, makes joan_code[] read-only.
void seal();
/// Whether seal() has run (every slot final and joan_code[] read-only).
bool sealed();

/// The trap stubs [begin, begin + count * code::kTrapStubSize) (replay: the address layout of a recording), 0 before
/// begin().
std::uintptr_t stubs_begin();
/// End (exclusive) of the trap stubs; 0 before begin().
std::uintptr_t stubs_end();

/// Slot of an original code address, or -1.
std::ptrdiff_t slot_of(std::uintptr_t va);
/// After seal(): what a call of original address `va` reaches (the slot's value), 0 when the address has no slot.
std::uintptr_t resolve(std::uintptr_t va);

/// Thunk slots that currently lead to `target` (canvas: is a replacement installed through thunks?).
int count_thunks_to(std::uintptr_t target);
/// The entry patch of the real slot of `original` (patch_entry; 0: none).
std::uintptr_t entry_patch(std::uintptr_t original);
/// Where the real address `original` will lead after seal (the S5 entry rule: installed replacement or the slot's
/// trap stub; 0 when it has no slot). Valid once install_all ran (canvas wrappers that wrap an original entry).
std::uintptr_t entry_target(std::uintptr_t original);

/// "slot 123 0x00401005 Video_seek -> 0x00686250" style description of a value our code holds: a trap stub, a slot's
/// resolved pointer, an original code address. Empty string when it is none of these.
void describe(std::uintptr_t value, char* out, std::size_t size);

/// Counters of the table, filled by seal() and reported by write_report().
struct Stats {
    unsigned slots = 0;  ///< slots in the table
    unsigned thunks_to_code = 0 /**< thunk slots leading to our C++ (code::Resolution::thunk_code) */,
        thunk_traps = 0 /**< thunk slots that resolved to a trap (thunk_trap) */,
        thunk_traps_registered = 0 /**< of those traps, ones whose original target is a registered, uninstalled name */;
    unsigned entries_to_replacement = 0 /**< real slots leading to the installed replacement */,
        entry_traps = 0 /**< real slots, not replaced: trap */,
        entry_traps_replaced = 0 /**< replaced functions reached only through thunks: trap */,
        unused = 0 /**< free slot indices */;
    unsigned late_code = 0 /**< JOAN_LATE_CODE variables assigned at seal() */,
        early_reads = 0 /**< code values read before seal() (fn::early_code_value) */,
        alias_errors = 0 /**< slot pairs of an alias group (values a comparison must tell apart) resolving to one pointer */;
};
/// The counters of the last seal().
const Stats& stats();
/// openjoan.log at exit: the traps that fired, the stray code addresses seen.
void write_report();

}  // namespace joan::code_table
