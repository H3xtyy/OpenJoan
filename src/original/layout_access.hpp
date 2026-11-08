// Access to the game's objects through their data members (docs/X64.md, re/layout/codemod_members.py).
//
// The game code reached fields as raw bytes: `f<T>(0x25)` = the T at this+0x25. The codemod writes them as members
// of the object's class (src/original/gen/*_fields.inc, from re/layout/layouts.json):
//   jo::self<jo::DynArrayB>(this)->last_ele            the member itself
//   jo::view<T>(jo::self<jo::X>(this)->member)          the member's bytes read / written as another type T of the
//                                                       same width (what the original code did at that site)
// Both are exactly the address arithmetic of the raw access, so the compiled code does not change.
#pragma once

/// Hand-written typed views of the original game's objects (the layouts of src/original and the generated src/original/gen).
namespace joan::orig {

/// `p` (the `this` of a replacement function, which is the object) as the object's class.
template <class C, class P>
inline C* self(const P* p) noexcept {
    return reinterpret_cast<C*>(const_cast<P*>(p));
}

/// A member's storage read / written as another type of the same size (the original's own reinterpretation).
template <class T, class M>
inline T& view(M& member) noexcept {
    static_assert(sizeof(T) == sizeof(M), "view: the access width must be the member's width");
    return *static_cast<T*>(static_cast<void*>(&member));
}
template <class T, class M>
/// Read-only form of view().
inline const T& view(const M& member) noexcept {
    static_assert(sizeof(T) == sizeof(M), "view: the access width must be the member's width");
    return *static_cast<const T*>(static_cast<const void*>(&member));
}

}  // namespace joan::orig
