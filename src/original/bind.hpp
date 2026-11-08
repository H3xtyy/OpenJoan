// Direct calls of the replacements from the typed facades (docs/X64.md, re/bind/gen_bridges.py).
//
// A facade member such as `Unit::detach_scene()` used to call the original address through its code slot
// (joan::call::thiscall on joan_code[]), i.e. through a member-function pointer forged from an integer, with the
// caller's guess of the signature. Now it is declared in the class and defined next to the replacement that the
// slot holds (in the replacement's .cpp), as a direct C++ call:
//
//     auto joan::orig::Unit::detach_scene() -> void { joan::bind::member<&::game_b84_01::H::detach_scene>(this); }
//
// The compiler sees the real callee, so its parameter types are checked; where the facade's view differs (another
// pointer class, the other signedness, a 32-bit integer carrying a pointer) the value is converted explicitly by
// conv<>. Those integer<->pointer conversions compile only where a pointer has 32 bits (x86): on x64 they are
// errors, which lists exactly the call sites that still pass pointers as integers.
#pragma once
#include "../callconv.hpp"

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

/// Conversions and direct calls from the typed facades to the replacement functions.
namespace joan::bind {

/// A value as the callee's parameter type (or the facade's result type).
template <class To, class From>
inline To conv(From&& v) noexcept {
    using F = std::remove_cv_t<std::remove_reference_t<From>>;
    if constexpr (std::is_reference_v<To>) {
        using T = std::remove_reference_t<To>;
        return *static_cast<T*>(const_cast<void*>(static_cast<const volatile void*>(&v)));  // the same object
    } else if constexpr (std::is_same_v<std::remove_cv_t<To>, F>) {
        return v;
    } else if constexpr (std::is_pointer_v<To> && std::is_pointer_v<F>) {
        return static_cast<To>(const_cast<void*>(static_cast<const volatile void*>(v)));
    } else if constexpr (std::is_pointer_v<To> || std::is_pointer_v<F>) {
        static_assert(sizeof(void*) == 4, "a pointer passed as a 32-bit integer: type this parameter");
        return reinterpret_cast<To>(v);
    } else {
        return static_cast<To>(v);
    }
}

/// Decomposes a function or member-function pointer type into `ret`, `args` (a std::tuple) and, for members, `cls`.
template <class F>
struct fn_traits;
/// Specialisation for `R (C::*)(P...)`.
template <class R, class C, class... P>
struct fn_traits<R (C::*)(P...)> {
    using ret = R;  ///< result type
    using cls = C;  ///< class type (const-qualified for a const member)
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
/// Specialisation for `R (C::*)(P...) const`.
template <class R, class C, class... P>
struct fn_traits<R (C::*)(P...) const> {
    using ret = R;  ///< result type
    using cls = const C;  ///< class type (const-qualified for a const member)
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
/// Specialisation for `R (C::*)(P...) noexcept`.
template <class R, class C, class... P>
struct fn_traits<R (C::*)(P...) noexcept> {
    using ret = R;  ///< result type
    using cls = C;  ///< class type (const-qualified for a const member)
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
/// Specialisation for `R (C::*)(P...) const noexcept`.
template <class R, class C, class... P>
struct fn_traits<R (C::*)(P...) const noexcept> {
    using ret = R;  ///< result type
    using cls = const C;  ///< class type (const-qualified for a const member)
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
/// Specialisation for `R (*)(P...)`.
template <class R, class... P>
struct fn_traits<R (*)(P...)> {
    using ret = R;  ///< result type
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
/// Specialisation for `R (*)(P...) noexcept`.
template <class R, class... P>
struct fn_traits<R (*)(P...) noexcept> {
    using ret = R;  ///< result type
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
#if defined(JOAN_X86_CONVENTIONS)  // __stdcall: a function type of its own
/// Specialisation for `R(JOAN_STDCALL*)(P...)`.
template <class R, class... P>
struct fn_traits<R(JOAN_STDCALL*)(P...)> {
    using ret = R;  ///< result type
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
/// Specialisation for `R(JOAN_STDCALL*)(P...) noexcept`.
template <class R, class... P>
struct fn_traits<R(JOAN_STDCALL*)(P...) noexcept> {
    using ret = R;  ///< result type
    using args = std::tuple<P...>;  ///< parameter types as a std::tuple
};
#endif

template <auto Fn, class Self, std::size_t... I, class... A>
/// Index-sequence form of member(): calls the member-function pointer `Fn` on `self` with the arguments converted by conv() to the callee's parameter types.
inline decltype(auto) member_i(Self* self, std::index_sequence<I...>, A&&... a) {
    using T = fn_traits<decltype(Fn)>;
    auto* const h = static_cast<typename T::cls*>(const_cast<void*>(static_cast<const volatile void*>(self)));
    return (h->*Fn)(conv<std::tuple_element_t<I, typename T::args>>(static_cast<A&&>(a))...);
}

/// Calls the replacement member function Fn (of its `struct H`) on `self` (the facade's `this`).
template <auto Fn, class Self, class... A>
inline decltype(auto) member(Self* self, A&&... a) {
    return member_i<Fn>(self, std::index_sequence_for<A...>{}, static_cast<A&&>(a)...);
}

template <auto Fn, std::size_t... I, class... A>
/// Index-sequence form of free(): calls the function pointer `Fn` with the arguments converted by conv() to the callee's parameter types.
inline decltype(auto) free_i(std::index_sequence<I...>, A&&... a) {
    using T = fn_traits<decltype(Fn)>;
    return Fn(conv<std::tuple_element_t<I, typename T::args>>(static_cast<A&&>(a))...);
}

/// Calls the replacement free function Fn.
template <auto Fn, class... A>
inline decltype(auto) free(A&&... a) {
    return free_i<Fn>(std::index_sequence_for<A...>{}, static_cast<A&&>(a)...);
}

}  // namespace joan::bind
