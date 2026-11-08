// joan.exe code addresses as seen by our C++ (docs/RUNTIME.md). Every code address is a typed constant of
// src/fn_names.hpp (`fn::Name`, fn::Code); the helpers below say how it is used. No implicit conversion to or from
// integers: a code address used as a number is a compile error and has to say so (`code_value(fn::X)`).
//
// Stage S7c-main (now): joan.exe's code does not exist at run time, not even as a generated jump table. A Code is the
// index of a SLOT of the code table joan_code[] (src/code_table.cpp, src/code_slots.inc), which holds what a call of
// the original address reaches: the installed C++ replacement, or a trap stub ("original code needed: <name>").
// install_all / fixes / canvas fill the table where they used to retarget thunks; code_table::seal() (end of
// host::start, before any game code runs) resolves every slot, assigns the JOAN_LATE_CODE variables and
// the loader then points the data image's code pointers at the resolved targets.
//   - call helpers (src/call.hpp) call through the slot: target(Code) = joan_code[slot] (one memory-indirect call,
//     like the old `call thunk` + `jmp`);
//   - tables of code addresses hold fn::Code (constant-initialised) and convert at the point of use; a namespace-scope
//     initialiser must never call code_value / code_ptr / target (it would run before seal());
//   - code_value / code_ptr are the resolved pointer, valid from seal() on (read earlier, the value is reported once:
//     `code value ... read before the code table was sealed`);
//   - va() is the original address: the name's ID (registry keys, logs);
//   - target(raw): a computed address (vtable entry, function pointer read from a game object) is called as it is;
//     a value in joan.exe's old code range 0x00401000..0x009ce700 is checked out of line: if executable memory is
//     mapped there (stage S7d: with ASLR our own code, a trap stub or a DLL can lie in that range) it is called as it
//     is, otherwise it can only be a missed name: reported once per value and caller (`stray code address`) and
//     resolved through its slot.
//
// No Windows headers: the Linux harnesses can use this file too.
#pragma once

#include <cstdint>

/// The code table (src/code_table.cpp): one pointer per slot, read-only after seal(). Global namespace, C linkage:
/// inline asm may use it too.
extern "C" std::uintptr_t joan_code[];

/// Typed original code addresses (fn::Code, one per address) and the helpers that turn them into slots and targets.
namespace joan::fn {

/// An original code address, as the index of its slot in joan_code[]. A scoped enumeration, not a class: no
/// conversion to or from integers, usable as a template argument, and in the compiled code a plain 32-bit index. Two
/// names are equal when they are the same slot (the same original address: src/fn_names.hpp has one name per
/// address).
enum class Code : std::uint32_t {};

/// "No code" in tables of call targets (a Code is a slot index: 0 is a valid slot).
inline constexpr Code kNoCode{0xFFFFFFFFu};

/// joan.exe's old code range (.text). Since stage S7d (ASLR, nothing reserved there) a pointer of ours can lie inside
/// it: a test against this range alone is only valid for values that are original addresses by contract.
inline constexpr std::uint32_t kOldCodeBegin = 0x00401000;
/// End (exclusive) of the original code range, 0x009ce700 (see kOldCodeBegin).
inline constexpr std::uint32_t kOldCodeEnd = 0x009ce700;

/// The original VA of the name: its ID (registry keys, logs). A table lookup (src/code_table.cpp).
std::uint32_t va(Code c) noexcept;

/// Reports a code value read before seal() (once per slot, with the caller) and returns what the slot will hold if
/// that is known yet (its trap stub), else 0.
std::uintptr_t early_code_value(Code c) noexcept;
/// target(raw) for a value in the old code range: returned as it is when executable memory is mapped there (a pointer
/// of ours / a DLL's), else reported once per value and caller (`stray code address`) and resolved through the slot of
/// that address (a trap when there is none).
std::uintptr_t stray_code_address(std::uintptr_t raw) noexcept;

/// Save files (img::save_words / load_words): a code pointer the game may have stored in a saved record is written as
/// the original address it stands for and turned back into the resolved pointer when read, so saves stay those of
/// joan.exe in both directions. original_code_address: the original address whose slot holds `pointer` (a thunk's
/// when several do, the lowest slot), 0 when no slot holds it (or before seal()). resolved_code_address: what the
/// slot of original address `va` holds, 0 when `va` has no slot (or before seal()).
std::uint32_t original_code_address(std::uintptr_t pointer) noexcept;
/// The pointer in the slot of original address `va`, 0 when `va` has no slot (see the comment above).
std::uintptr_t resolved_code_address(std::uint32_t va) noexcept;

/// The code address as a number, where the game sees it as data: stored into game objects / tables / vtables, passed
/// as a callback (atexit, qsort, WNDCLASS, ...), compared with a pointer read from an object. The resolved pointer
/// (the replacement, or the slot's trap stub); valid from seal() on.
inline std::uintptr_t code_value(Code c) noexcept {
    const std::uintptr_t v = joan_code[static_cast<std::uint32_t>(c)];
    return v != 0 ? v : early_code_value(c);
}
/// The code address as a pointer (a callback passed as a pointer argument).
inline void* code_ptr(Code c) noexcept { return reinterpret_cast<void*>(code_value(c)); }

/// The address a call helper (src/call.hpp) or joan::hook::original<> jumps to: an fn:: name through its slot, or a
/// computed address (a vtable entry, a function pointer read from a game object: the "raw" form).
inline std::uintptr_t target(Code c) noexcept { return joan_code[static_cast<std::uint32_t>(c)]; }
/// Call target for an already computed address (a vtable entry, a function pointer read from a game object): an original code address is resolved through its slot, any other value is returned unchanged.
inline std::uintptr_t target(std::uintptr_t raw) noexcept {
    return raw - kOldCodeBegin < kOldCodeEnd - kOldCodeBegin ? stray_code_address(raw) : raw;
}

/// JOAN_LATE_CODE registration (constructed during static initialisation, assigned by seal()).
struct LateCode {
    std::uintptr_t* variable;  ///< the variable seal() assigns: a code pointer (8 bytes on x64)
    Code code;                 ///< the slot to resolve, or kNoCode when raw_va is used
    std::uint32_t raw_va;      ///< JOAN_LATE_CODE_VA: an original address without an fn:: name (code == kNoCode)
    LateCode* next;            ///< next registration of the intrusive list (set by the constructor)
    /// Registers `v` to receive the resolved pointer of `c` at seal().
    LateCode(std::uintptr_t* v, Code c) noexcept;
    /// Registers `v` to receive the resolved pointer of the slot of original address `original_va` at seal().
    LateCode(std::uintptr_t* v, std::uint32_t original_va) noexcept;
};

}  // namespace joan::fn

#define JOAN_CODE_CAT2(a, b) a##b
#define JOAN_CODE_CAT(a, b) JOAN_CODE_CAT2(a, b)

/// A namespace-scope dword that holds a code address for inline asm (`call dword ptr [kc_x]`) or for C++ that reads it
/// as a number: `JOAN_LATE_CODE(kc_x, fn::X);` declares `std::uintptr_t kc_x`, zero until seal() assigns the resolved
/// pointer (after install_all: it is not known before; nothing of the game runs before seal()).
#define JOAN_LATE_CODE(name, code)                                                                      \
    std::uintptr_t name = 0;                                                                            \
    [[maybe_unused]] static ::joan::fn::LateCode JOAN_CODE_CAT(joan_late_code_, name) { &name, code }

/// As JOAN_LATE_CODE, for an original code address that has a slot but no fn:: name (a library entry inline asm
/// called by its number): seal() assigns the resolved pointer of the slot of `original_va` (its replacement or trap).
#define JOAN_LATE_CODE_VA(name, original_va)                                                            \
    std::uintptr_t name = 0;                                                                            \
    [[maybe_unused]] static ::joan::fn::LateCode JOAN_CODE_CAT(joan_late_code_, name) {                 \
        &name, static_cast<std::uint32_t>(original_va)                                                  \
    }
