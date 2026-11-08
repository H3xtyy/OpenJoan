// SPDX-License-Identifier: GPL-3.0-or-later
// Small helpers the ported game code used to define again in every batch module (Phase 3 helper merge): the CRT and
// operator new / delete wrappers with the argument conventions of joan::orig::arg, and the untyped field accessor.
// A file that had its own identical copy now has `using joan::orig::helpers::<name>;` in the same place, so every call
// resolves to the same name as before. Behaviour is the copies': nothing here is new.
#pragma once

#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "direct.hpp"
#include "layout_access.hpp"

/// Small helpers shared by the ported game code: CRT and operator new wrappers, untyped field access.
namespace joan::orig::helpers {

/// The `T` at byte `offset` of an untyped object (the original's field offsets; typed layouts replace these uses).
template <class T>
T& at(const void* p, std::int32_t offset) {
    return *reinterpret_cast<T*>(static_cast<std::uint8_t*>(const_cast<void*>(p)) + offset);
}

/// memset / strcpy / strlen / _stricmp of the game's CRT (MSVC 6, src/lib/crt) with the original's argument widths.
inline void* memset_(void* p, std::int32_t v, std::uint32_t n) {
    return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n));
}
/// As memset_ (the same call).
inline void* crt_memset(void* p, std::int32_t v, std::uint32_t n) {
    return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n));
}
/// strcpy of the game's CRT.
inline char* strcpy_(char* d, const char* s) {
    return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s));
}
/// strlen of the game's CRT; the length as a 32-bit value.
inline std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
/// _stricmp of the game's CRT (src/lib/crt).
inline std::int32_t stricmp_(const char* a, const char* b) {
    return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b));
}

/// The game's operator new / delete (the CRT heap of src/lib/crt, 0x007b1d35 / 0x007b1d2a).
inline void* game_new(std::uint32_t n) { return joan::crt::operator_new(joan::orig::arg::u32(n)); }
/// As game_new.
inline void* op_new(std::uint32_t n) { return joan::crt::operator_new(joan::orig::arg::u32(n)); }
/// operator delete of the game's CRT heap.
inline void game_delete(void* p) { joan::crt::operator_delete(joan::orig::arg::mptr(p)); }
/// As game_delete.
inline void op_delete(void* p) { joan::crt::operator_delete(joan::orig::arg::mptr(p)); }

/// A pointer as an integer of its own width (diagnostics, lookups by address).
inline std::uintptr_t addr(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }
/// As addr.
inline std::uintptr_t u(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

/// Further helpers merged from the batch modules (each `using joan::orig::helpers::<name>;` where a copy stood).
namespace jo = joan::orig;
struct Obj {};

/// a + b with 32-bit wrap-around.
inline std::int32_t add32(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
/// As addr.
inline std::uintptr_t addr_of(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }
/// Calls the cdecl function whose pointer is stored at the image address `slot` (an OpenAL import slot of the original) with `args`, returning R.
template <class R = void, class... A>
inline R al_call(joan::img::Addr slot, A... args) { return reinterpret_cast<R(JOAN_CDECL*)(A...)>(joan::img::global<std::uintptr_t>(slot))(args...); }
/// Widget::scale_to on `o` with the five raw argument words (the first as 16 bits, the next two as floats, the fourth as a char).
inline void anim_set(void* o, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e) { joan::orig::as_widget(o)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e)); }
/// anim_set(o, 0, 0x40c00000, 0, 0, 0): the original's call with the float 6.0 as second argument.
inline void anim_set6(void* o) { anim_set(o, 0, 0x40c00000u, 0, 0, 0); }
/// The float with the bit pattern `bits`.
inline float as_float(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
/// The float at the image address `a`.
inline float cf(joan::img::Addr a) { return joan::img::global<float>(a); }
/// Number of records of the DynArray `list`.
inline std::int32_t count_of(const void* list) { return joan::orig::as_dynarray(list)->size(); }
/// abs of the game's CRT.
inline std::int32_t crt_abs(std::int32_t v) { return joan::orig::arg::as<std::int32_t>(joan::orig::libc::abs(joan::orig::arg::i32(v))); }
/// atoi of the game's CRT.
inline std::int32_t crt_atoi(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
/// free of the game's CRT (joan::orig::free_).
inline void crt_free(void* p) { joan::orig::free_(joan::orig::arg::mptr(p)); }
/// memcpy with the original's argument widths.
inline void* crt_memcpy(void* d, const void* s, std::uint32_t n) { return std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n)); }
/// strcmp of two game strings.
inline std::int32_t crt_strcmp(const char* a, const char* b) { return std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
/// As game_delete.
inline void delete_(void* p) { joan::crt::operator_delete(joan::orig::arg::mptr(p)); }
/// The game's Direct3D device pointer (d::g_game_device).
inline void* device() { return joan::img::global<void*>(d::g_game_device); }
/// True when a < b or a > b (false for equal values and for NaNs).
inline bool differs(float a, float b) { return a < b || a > b; }
/// Stores the float at `in` divided by the float at `div` into the float at `out` (all image addresses).
inline void div_global(joan::img::Addr out, joan::img::Addr in, joan::img::Addr div) { joan::img::global<float>(out) = joan::img::global<float>(in) / joan::img::global<float>(div); }
/// Stores the float at `src` divided by the float at `divisor` into the float at `dst` (all image addresses).
inline void div_init(joan::img::Addr dst, joan::img::Addr src, joan::img::Addr divisor) { joan::img::global<float>(dst) = joan::img::global<float>(src) / joan::img::global<float>(divisor); }
/// The float with the bit pattern `b`.
inline float fbits(std::uint32_t b) { float f; std::memcpy(&f, &b, sizeof f); return f; }
/// The float at the image address `a`, widened to double.
inline double fd(joan::img::Addr a) { return static_cast<double>(joan::img::global<float>(a)); }
/// The integer `v` as a double.
inline double fild(std::int32_t v) { return static_cast<double>(v); }
/// The double at the image address `a`.
inline double gd(joan::img::Addr a) { return joan::img::global<double>(a); }
/// The float at the image address `a`.
inline float gf(joan::img::Addr a) { return joan::img::global<float>(a); }
/// a * b with 32-bit wrap-around.
inline std::int32_t imul(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
/// The 16-bit value at the image address `g`, sign-extended (the base of a __LINE__ number passed to the allocator, see original/blob.hpp).
inline std::int32_t line_base(joan::img::Addr g) { return static_cast<std::int32_t>(joan::img::global<std::int16_t>(g)); }
/// line_base(g) + add.
inline std::int32_t line_of(joan::img::Addr g, std::int32_t add) { return static_cast<std::int32_t>(joan::img::global<std::int16_t>(g)) + add; }
/// memcpy with the original's argument widths.
inline void* memcpy_(void* d, const void* s, std::uint32_t n) { return std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n)); }
/// a * b with 32-bit wrap-around.
inline std::int32_t mul(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
/// As game_new.
inline void* new_(std::uint32_t n) { return joan::crt::operator_new(joan::orig::arg::u32(n)); }
/// Scene::detach of `child` on the scene node `node`.
inline void node_remove(void* node, void* child) { joan::orig::as_scene(node)->detach(joan::orig::arg::mptr(child)); }
/// strings::get on `self` with the name, the number `n` and the remaining arguments; returns the text.
template <class... A>
inline const char* open_text(void* self, const char* name, std::int32_t n, A... rest) { return static_cast<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...)); }
/// As game_new.
inline void* operator_new(std::uint32_t size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }
/// Constructs a MapObject in `self`, sets its vtable to the one at the image address `vtbl` and zeroes the `n` bytes that follow the MapObject part.
inline void* order_ctor(void* self, joan::img::Addr vtbl, std::uint32_t n) { joan::orig::as_map_object(self)->construct(); jo::view<std::uintptr_t>(jo::as_map_object(self)->vtable) = joan::img::addr(vtbl); memset_(static_cast<std::uint8_t*>(self) + sizeof(jo::MapObject), 0, n); return self; }
/// atan(1.0) of the game's CRT times the double d::dbl_4, as a float.
inline float pi_from_atan() { const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0))); return static_cast<float>(a * joan::img::global<double>(d::dbl_4)); }
/// The distance of `point` from `plane` (joan::orig::plane_distance).
inline float plane_distance(const void* plane, const void* point) { return joan::orig::arg::as<float>(joan::orig::plane_distance(joan::orig::arg::mptr(plane), joan::orig::arg::mptr(point))); }
/// Updates the game's sound stream (d::g_snd) and ambient streams (d::g_mus).
inline void pump() { joan::orig::as_sound_stream(obj(d::g_snd))->update(); joan::orig::as_ambient_streams(obj(d::g_mus))->update(); }
/// The game's random number generator (Misc::random on d::g_misc) for `n`.
inline std::int32_t random(std::int32_t n) { return joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(n)); }
/// Clears the Rect9 at the image address `g`.
inline void rect9_clear(joan::img::Addr g) { joan::orig::as_rect9(obj(g))->clear(); }
/// Registers the code address `Fn` with the CRT's atexit.
template <joan::fn::Code Fn>
inline std::int32_t JOAN_CDECL reg_atexit() { return joan::crt::atexit(joan::fn::code_ptr(Fn)); }
/// `v` as a signed 32-bit value.
inline std::int32_t s(std::uint32_t v) { return static_cast<std::int32_t>(v); }
/// The low 16 bits of `v` sign-extended to 32 bits.
inline std::int32_t s16(std::uint32_t v) { return static_cast<std::int16_t>(static_cast<std::uint16_t>(v)); }
/// The low 8 bits of `v` sign-extended to 32 bits.
inline std::int32_t s8(std::uint32_t v) { return static_cast<std::int8_t>(static_cast<std::uint8_t>(v)); }
/// The body of a scalar deleting destructor: frees `p` when bit 0 of `flags` is set.
inline void scalar_delete(void* p, std::uint32_t flags) { if ((flags & 1) != 0) game_delete(p); }
/// The game's screen height in pixels (d::g_screen_h).
inline std::int32_t screen_h() { return joan::img::global<std::int32_t>(d::g_screen_h); }
/// The game's screen width in pixels (d::g_screen_w).
inline std::int32_t screen_w() { return joan::img::global<std::int32_t>(d::g_screen_w); }
/// strcmp of two game strings.
inline std::int32_t strcmp_(const char* a, const char* b) { return std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
/// Clears the text descriptor at the image address `g`.
inline void text_desc_clear(joan::img::Addr g) { joan::orig::as_text_desc(obj(g))->clear(); }
/// The kind of a script token (the kind field of the token record).
inline std::int32_t type(const std::uint8_t* tok) { return jo::as_token(tok)->kind; }
/// The unit table object (d::g_units).
inline void* units() { return obj(d::g_units); }
/// As al_call, for a slot of the v3d_87.dll import table.
template <class R = void, class... A>
inline R v3d_call(joan::img::Addr slot, A... args) { return reinterpret_cast<R(JOAN_CDECL*)(A...)>(joan::img::global<std::uintptr_t>(slot))(args...); }
/// Calls the virtual member at byte `offset` of the vtable of `object` with `args`; the offset is the x86 vtable offset (4 bytes per slot).
template <class R = void, class... A>
inline R virtual_call(void* object, std::int32_t offset, A... args) { using Fn = R (Obj::*)(A...); JOAN_MEMBER_POINTER_CHECK(Fn); Fn method{}; std::memcpy(&method, &joan::call::vtable_of(object)[joan::call::x86_vslot(offset)], sizeof(void*)); return (reinterpret_cast<Obj*>(object)->*method)(args...); }
/// a + b with 32-bit wrap-around.
inline std::int32_t wadd(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
/// The original constructor 0x00620ee0 of an en2d widget on `p`.
inline void widget_ctor(void* p) { joan::orig::as_widget(p)->ctor_620ee0(); }
/// As al_call, for a stdcall slot (an import of the original executable).
template <class R = std::int32_t, class... A>
inline R win_call(joan::img::Addr slot, A... args) { return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(joan::img::global<std::uintptr_t>(slot))(args...); }
/// a + b with 32-bit wrap-around.
inline std::int32_t wrap_add(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
/// a * b with 32-bit wrap-around.
inline std::int32_t wrap_mul(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
/// a - b with 32-bit wrap-around.
inline std::int32_t wrap_sub(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
/// a - b with 32-bit wrap-around.
inline std::int32_t wsub(std::int32_t a, std::int32_t b) { return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }

}  // namespace joan::orig::helpers
