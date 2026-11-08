// Installation of the reimplementation (docs/RUNTIME.md).
//
// joan.exe was linked incrementally: every call, vtable slot and function pointer went through a table of `jmp rel32`
// thunks at 0x00401005.. (8015 thunks). joan.exe's code no longer exists at run time: every original code address
// (thunk or real) is a slot of the code table (src/code_table.cpp, code_slots_core.hpp), and installing a
// replacement fills slots exactly where stage S5 retargeted a thunk or wrote an entry jump.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

#include "../call.hpp"
#include "image.hpp"

/// Installation of the replacements into the code table, and the diagnostics hooks that share this namespace.
namespace joan::hook {

// (Until stage S7d a synthetic PE header of joan.exe sat at 0x00400000 and is_supported_executable() checked its
// timestamp. joan.exe's data and code are generated from one build, 0x400BA40A, and are part of openjoan.exe: there
// is nothing left to check at run time. GetModuleHandle(NULL) is openjoan.exe itself.)

/// Every thunk slot that still leads to the ORIGINAL address `original` (no redirect yet) now leads to
/// `replacement`. Returns the number of slots (0 = no thunk leads there). Only before code_table::seal()
/// (afterwards an error, logged, nothing changed).
int redirect_function(std::uintptr_t original, const void* replacement);

/// Every thunk slot that an earlier redirect pointed at `current` (a pointer of ours: an installed replacement)
/// now leads to `replacement` (fixes, canvas wrappers). Kept apart from redirect_function: since stage S7d (ASLR) a
/// pointer of ours may be numerically equal to some original address. Same rules as redirect_function.
int redirect_replacement(const void* current, const void* replacement);

/// For functions without an incremental-link thunk (static functions called directly, library entries): records
/// `replacement` as the entry patch of the real slot of `original`; false when the address has no slot. At seal the
/// real slot follows stage S5's entry rule (code_slots_core.hpp entry_jumps_to_replacement).
bool patch_entry(std::uintptr_t original, const void* replacement);

/// Points the import address table slot at original address `slot` (a joan.exe IAT entry, e.g. 0x00a6de44
/// v3d_87.dll!dot_product; the slot itself is in joan_image) to `replacement`, after saving the bound pointer in *saved.
/// Every import_call of ours then runs the replacement; *saved still reaches the DLL. Returns false if the slot is
/// empty or not writable.
bool patch_import(std::uintptr_t slot, const void* replacement, std::uintptr_t* saved);

/// Converts a pointer-to-member-function (single inheritance, MSVC) or a free
/// function pointer into a plain code address.
template <class Fn>
const void* code_address(Fn fn) {
    if constexpr (sizeof(Fn) == sizeof(void*)) {
        union {
            Fn fn;
            const void* ptr;
        } u{};
        u.fn = fn;
        return u.ptr;
    } else {
        // Itanium C++ ABI (GCC / Clang elsewhere): {address, `this` adjustment}; a virtual member function's pointer
        // holds its vtable offset + 1 instead of an address - none of ours is registered that way (nullptr if one is).
        static_assert(std::is_member_function_pointer_v<Fn> && sizeof(Fn) == 2 * sizeof(void*),
                      "only plain functions / single-inheritance member functions");
        union {
            Fn fn;
            struct {
                std::uintptr_t ptr;
                std::ptrdiff_t adj;
            } p;
        } u{};
        u.fn = fn;
        if ((u.p.ptr & 1) != 0 || u.p.adj != 0) return nullptr;
        return reinterpret_cast<const void*>(u.p.ptr);
    }
}

/// The inverse: turns a code address (an fn:: name through its slot, or a computed address) into a callable function
/// or member-function pointer. Defined in call.hpp.
using joan::call::original;

/// A global of the original executable's data image: global<T>(d::Name) (img::Addr / img::Sym), or a computed
/// address (raw overload). See image.hpp.
using joan::img::global;

}  // namespace joan::hook
