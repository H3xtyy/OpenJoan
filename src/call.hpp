// Calls of joan.exe code from our C++ (docs/RUNTIME.md): the shared call helpers that replace the
// module-local copies (re/rewrite_call_helpers.py). A module says `using joan::call::thiscall;` (etc.) in its anonymous
// namespace; src/patch.hpp includes this header, so every module that includes registry.hpp / patch.hpp has it.
//
// The target is an fn:: name (fn::Code) or a computed address (an integer: a vtable entry, a function pointer read
// from a game object); fn::target() turns either into the address to call. S7c-prep: the original address (thunk or
// real entry), exactly as the integer helpers did. The target type is a template parameter (deduced), so the compiled
// code is the same as with the old `std::uintptr_t address` parameter; img::Addr and pointers do not convert.
//
// Helpers that differ in behaviour from these (other calling convention or default return type under the same name,
// vtable-offset helpers com_call / virtual_call / vcall, IAT helpers under other names) stay module-local; see
// docs/RUNTIME.md.
#pragma once
#include "callconv.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "runtime/code.hpp"
#include "runtime/image.hpp"

namespace joan::call {

/// Turns a code address (fn:: name or computed) into a callable function or member-function pointer (single
/// inheritance: MSVC's is the address; the Itanium C++ ABI's of GCC / Clang is {address, `this` adjustment 0}), used
/// to call not-yet-reimplemented originals.
template <class Fn, class T>
Fn original(T address) {
    if constexpr (sizeof(Fn) == sizeof(void*)) {
        union {
            std::uintptr_t addr;
            Fn fn;
        } u{};
        u.addr = fn::target(address);
        return u.fn;
    } else {
        static_assert(std::is_member_function_pointer_v<Fn> && sizeof(Fn) == 2 * sizeof(void*),
                      "only plain functions / single-inheritance member functions");
        union {
            struct {
                std::uintptr_t ptr;
                std::ptrdiff_t adj;
            } p;
            Fn fn;
        } u{};
        u.p.ptr = fn::target(address);
        return u.fn;
    }
}

// The call helpers have internal linkage, like the module-local copies they replace: the optimiser inlines them exactly
// as before (an inline function with external linkage is weighed differently and may be emitted out of line).
namespace {

namespace detail {
struct Obj {};  // complete, no bases: R (Obj::*)(A...) is a plain 4-byte code pointer (MSVC single inheritance)
}  // namespace detail

/// __thiscall (this in ecx), through a pointer to member: a class-type R is returned through the hidden pointer, as
/// MSVC 6 member functions do.
// GCC compiles a call through a member pointer with a branch for virtual functions (lowest bit of the pointer set)
// that reads the object's vtable; with `self` pointing at a small local, -Warray-bounds / -Wmaybe-uninitialized
// report that branch at every inlined call. It never runs: code slot targets are our functions (16-byte aligned) or
// trap stubs (10 / 24 bytes apart), always even.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
template <class R = void, class T, class... A>
R thiscall(T address, const void* self, A... args) {
    using Fn = R (detail::Obj::*)(A...);
    return (static_cast<detail::Obj*>(const_cast<void*>(self))->*original<Fn>(address))(args...);
}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

template <class R = void, class T, class... A>
R cdecl_call(T address, A... args) {
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(fn::target(address))(args...);
}

template <class R = void, class T, class... A>
R stdcall_call(T address, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(fn::target(address))(args...);
}

/// __cdecl call through an import address table slot of the data image (d::iat_*).
template <class R = void, class... A>
R import_call(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_CDECL*)(A...)>(img::global<std::uintptr_t>(slot))(args...);
}

/// __stdcall call through an import address table slot of the data image (d::iat_*).
template <class R = void, class... A>
R stdcall_import(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(img::global<std::uintptr_t>(slot))(args...);
}

/// The low 32 bits of `p`, for one case only: an original function whose result is whatever a register happened to
/// hold (eax left at `this` or at a pointer by a function with no meaningful result), where every caller ignores the
/// value or tests only its low byte. Never for a value that is stored, compared as an address or turned back into a
/// pointer: those need a pointer-sized type.
inline std::uint32_t register_low_bits(const void* p) noexcept {
    return static_cast<std::uint32_t>(std::bit_cast<std::uintptr_t>(p));
}

/// The low 32 bits of `p` as an emulated x86 stack frame keeps a pointer in one dword slot, where the code keeps the
/// upper half separately (a high-half slot rejoined on read, or the pointer itself beside the frame word): the x86
/// frame contents stay as the original's, and x64 never rebuilds the pointer from this word alone. Also for a 4-byte
/// data-image slot in which the original kept a pointer that only its own (not ported) assembly read back, written so
/// that the x86 image and its replay hash stay as the original's (blit16.cpp / blitfx.cpp: g_surface_base).
inline std::uint32_t pointer_low_dword(const void* p) noexcept {
    return static_cast<std::uint32_t>(std::bit_cast<std::uintptr_t>(p));
}

/// The virtual function table of `object` (a game class, a COM interface): its first member, a pointer to an array of
/// pointer-sized entries (4 bytes on x86, 8 on x64; MSVC single inheritance and COM, as GCC / Clang lay them out for
/// our own classes). Index it with a slot number, never with a byte offset (x86_vslot).
inline void* const* vtable_of(const void* object) { return *static_cast<void* const* const*>(object); }

/// The same table, writable: for patching entries of a table that lives in writable memory (replay/com.cpp hooks).
inline void** mutable_vtable_of(void* object) { return *static_cast<void** const*>(object); }

/// Replaces the virtual function table of `object` (hooking a COM interface): `table` must outlive the object's use.
inline void set_vtable(void* object, void* const* table) { *static_cast<void* const**>(object) = table; }

/// The slot number of a vtable byte offset as the original code addresses it (`call [eax+off]`, 4-byte entries).
/// The result has the type of the argument (the index arithmetic stays as the original's).
template <class T>
constexpr T x86_vslot(T byte_offset) { return static_cast<T>(byte_offset / 4); }

}  // namespace
}  // namespace joan::call
