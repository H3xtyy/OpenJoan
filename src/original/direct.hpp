// Direct calls of reimplemented members from game code that does not have typed objects yet: game code still holds
// most objects and arguments as void* / u32. These turn such values into the typed object (or argument) so a call
// reads `joan::orig::as_file(f)->file_read(buf, n)` instead of going through the code slot of the original
// address (refactor batches R1.., docs/RUNTIME.md).
#pragma once
#include "../callconv.hpp"

#include <bit>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "file.hpp"
#include "file_mem.hpp"
#include "file_txt.hpp"
#include "blob.hpp"
#include "colour.hpp"
#include "date.hpp"
#include "dynarray.hpp"
#include "engine.hpp"
#include "en3d.hpp"
#include "database.hpp"
#include "mem.hpp"
#include "gameset.hpp"
#include "misc.hpp"
#include "point.hpp"
#include "records.hpp"
#include "res_idx.hpp"
#include "tables.hpp"
#include "ui.hpp"
#include "unit.hpp"
#include "vecmath.hpp"
#include "gen/classes.hpp"

namespace joan::orig {

/// Argument conversions for direct calls from game code that still holds objects as void* / u32.
namespace arg {
/// A pointer argument given as a pointer, nullptr or a 32-bit integer.
template <class T>
const void* ptr(T v) noexcept {
    if constexpr (std::is_pointer_v<T>) return v;
    else if constexpr (std::is_null_pointer_v<T>) return nullptr;
    else return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(v));
}
/// A mutable pointer argument (same inputs as ptr).
template <class T>
void* mptr(T v) noexcept {
    return const_cast<void*>(ptr(v));
}
/// A string argument (same inputs as ptr).
template <class T>
const char* str(T v) noexcept {
    return static_cast<const char*>(ptr(v));
}
/// Integer arguments given as any integer / enum type (the slot call passed them as 32-bit values).
template <class T>
std::int32_t i32(T v) noexcept {
    static_assert(!std::is_pointer_v<T>, "pointer passed where the original takes an int");
    static_assert(!std::is_floating_point_v<T>, "a float passed where the original takes an int: pass its bits (arg::bits)");
    return static_cast<std::int32_t>(v);
}
/// As i32, converted to `std::uint32_t`.
template <class T>
std::uint32_t u32(T v) noexcept {
    static_assert(!std::is_pointer_v<T>, "pointer passed where the original takes an int");
    static_assert(!std::is_floating_point_v<T>, "a float passed where the original takes an int: pass its bits (arg::bits)");
    return static_cast<std::uint32_t>(v);
}
/// As i32, converted to `std::int16_t`.
template <class T>
std::int16_t i16(T v) noexcept {
    static_assert(!std::is_pointer_v<T>, "pointer passed where the original takes an int");
    static_assert(!std::is_floating_point_v<T>, "a float passed where the original takes an int: pass its bits (arg::bits)");
    return static_cast<std::int16_t>(v);
}
/// As i32, converted to `std::uint16_t`.
template <class T>
std::uint16_t u16(T v) noexcept {
    static_assert(!std::is_pointer_v<T>, "pointer passed where the original takes an int");
    static_assert(!std::is_floating_point_v<T>, "a float passed where the original takes an int: pass its bits (arg::bits)");
    return static_cast<std::uint16_t>(v);
}
/// A bool argument given as bool or as an integer: the slot call pushed the integer and the callee looked at its low
/// byte only (R14).
template <class T>
bool b(T v) noexcept {
    static_assert(!std::is_pointer_v<T>, "pointer passed where the original takes a bool");
    static_assert(!std::is_floating_point_v<T>, "a float passed where the original takes a bool");
    if constexpr (std::is_same_v<T, bool>) return v;
    else return static_cast<std::uint8_t>(v) != 0;
}
/// The bit pattern of a float passed where the callee takes a 32-bit integer (the slot call pushed the float as is).
inline std::uint32_t bits(float v) noexcept {
    std::uint32_t b;
    std::memcpy(&b, &v, sizeof b);
    return b;
}
/// A comparison function given as a code address (fn::code_value) or a function pointer.
template <class T>
auto cmp(T v) noexcept {
    using Fn = int(JOAN_CDECL*)(const void*, const void*);
    if constexpr (std::is_pointer_v<T>) return reinterpret_cast<Fn>(v);
    else return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(v));
}
/// A File* argument given as a pointer or a 32-bit integer.
template <class T>
File* file(T v) noexcept {
    return static_cast<File*>(const_cast<void*>(ptr(v)));
}
/// A float argument given as a float or as its bit pattern in a 32-bit integer / pointer (the slot call pushed
/// the dword as is; R7a).
template <class T>
float f32(T v) noexcept {
    if constexpr (std::is_same_v<T, float>) {
        return v;
    } else {
        static_assert(!std::is_same_v<T, double> && !std::is_same_v<T, long double>, "double passed where the original takes a float");
        std::uint32_t bits;
        // A pointer-typed argument carries the float's bit pattern in its low 32 bits (the dword the slot call pushed).
        if constexpr (std::is_pointer_v<T>) bits = static_cast<std::uint32_t>(std::bit_cast<std::uintptr_t>(v));
        else bits = static_cast<std::uint32_t>(v);
        float f;
        std::memcpy(&f, &bits, sizeof f);
        return f;
    }
}
/// A double argument (the slot call pushed 8 bytes only for a double).
template <class T>
double f64(T v) noexcept {
    static_assert(std::is_same_v<T, double>, "the original takes a double");
    return v;
}
/// A result converted to the type the caller used (the slot call returned it as R).
/// A pointer asked for as a 32-bit integer (`as<u32>(p)`, `as<i32>(p)`: the x86 code kept addresses in dwords) is
/// returned as std::uintptr_t (std::intptr_t for a signed R), the whole pointer: the same type as R on x86 (object-identical), the full 8 bytes on
/// x64. Where the value then flows into a 32-bit parameter or variable the compiler's -Wshorten-64-to-32 / C4244
/// names the site, which is the list of facades and locals still to be retyped (Phase 2, skill oj-x64-port).
template <class R, class V>
auto as(V v) noexcept {
    if constexpr (std::is_pointer_v<V> && std::is_integral_v<R> && sizeof(R) == 4)
        return static_cast<std::conditional_t<std::is_signed_v<R>, std::intptr_t, std::uintptr_t>>(reinterpret_cast<std::uintptr_t>(v));
    else if constexpr (std::is_pointer_v<V> && std::is_integral_v<R>) return static_cast<R>(reinterpret_cast<std::uintptr_t>(v));
    else if constexpr (std::is_pointer_v<V> && std::is_pointer_v<R>) return static_cast<R>(const_cast<void*>(static_cast<const void*>(v)));
    else if constexpr (std::is_integral_v<V> && std::is_pointer_v<R>) return reinterpret_cast<R>(static_cast<std::uintptr_t>(v));
    else return static_cast<R>(v);
}
}  // namespace arg

/// The object at `p` (a pointer or a 32-bit integer holding one, as untyped game code keeps them).
template <class T, class P>
T* object(P p) noexcept {
    return static_cast<T*>(const_cast<void*>(arg::ptr(p)));
}

/// The object at `p` (see object()) as a `File`.
template <class P>
inline File* as_file(P p) noexcept { return object<File>(p); }
/// The object at `p` (see object()) as a `DynArray`.
template <class P>
inline DynArray* as_dynarray(P p) noexcept { return object<DynArray>(p); }
/// The object at `p` (see object()) as a `FileMem`.
template <class P>
inline FileMem* as_file_mem(P p) noexcept { return object<FileMem>(p); }
/// The object at `p` (see object()) as a `FileTxt`.
template <class P>
inline FileTxt* as_file_txt(P p) noexcept { return object<FileTxt>(p); }
/// The object at `p` (see object()) as a `Mem`.
template <class P>
inline Mem* as_mem(P p) noexcept { return object<Mem>(p); }
/// The object at `p` (see object()) as a `Blob`.
template <class P>
inline Blob* as_blob(P p) noexcept { return object<Blob>(p); }
/// The object at `p` (see object()) as a `Database`.
template <class P>
inline Database* as_database(P p) noexcept { return object<Database>(p); }
/// The object at `p` (see object()) as a `ResourceDb`.
template <class P>
inline ResourceDb* as_resource_db(P p) noexcept { return object<ResourceDb>(p); }
/// The object at `p` (see object()) as a `ResourceIdx`.
template <class P>
inline ResourceIdx* as_res_idx(P p) noexcept { return object<ResourceIdx>(p); }
/// The object at `p` (see object()) as a `ResourceIdxL`.
template <class P>
inline ResourceIdxL* as_res_idx_l(P p) noexcept { return object<ResourceIdxL>(p); }
/// The object at `p` (see object()) as a `String`.
template <class P>
inline String* as_string(P p) noexcept { return object<String>(p); }
/// The object at `p` (see object()) as a `Misc`.
template <class P>
inline Misc* as_misc(P p) noexcept { return object<Misc>(p); }
/// The object at `p` (see object()) as a `Units`.
template <class P>
inline Units* as_units(P p) noexcept { return object<Units>(p); }
/// The object at `p` (see object()) as a `Players`.
template <class P>
inline Players* as_players(P p) noexcept { return object<Players>(p); }
/// The object at `p` (see object()) as a `Records203`.
template <class P>
inline Records203* as_records_203(P p) noexcept { return object<Records203>(p); }
/// The object at `p` (see object()) as a `Records2bf`.
template <class P>
inline Records2bf* as_records_2bf(P p) noexcept { return object<Records2bf>(p); }
/// The object at `p` (see object()) as a `Colour`.
template <class P>
inline Colour* as_colour(P p) noexcept { return object<Colour>(p); }
/// The object at `p` (see object()) as a `Point`.
template <class P>
inline Point* as_point(P p) noexcept { return object<Point>(p); }
/// The object at `p` (see object()) as a `Unit`.
template <class P>
inline Unit* as_unit(P p) noexcept { return object<Unit>(p); }
// R7a: vector helpers (vecmath.hpp)
/// The object at `p` (see object()) as a `Aabb`.
template <class P>
inline Aabb* as_aabb(P p) noexcept { return object<Aabb>(p); }
// R7b: 2D UI (ui.hpp)
/// The object at `p` (see object()) as a `SpriteQueue`.
template <class P>
inline SpriteQueue* as_sprite_queue(P p) noexcept { return object<SpriteQueue>(p); }
/// The object at `p` (see object()) as a `Widget`.
template <class P>
inline Widget* as_widget(P p) noexcept { return object<Widget>(p); }
/// The object at `p` (see object()) as a `Button`.
template <class P>
inline Button* as_button(P p) noexcept { return object<Button>(p); }
/// The object at `p` (see object()) as a `Font`.
template <class P>
inline Font* as_font(P p) noexcept { return object<Font>(p); }
// R7c: en3d objects (en3d.hpp)
/// The object at `p` (see object()) as a `TextureManager`.
template <class P>
inline TextureManager* as_texture_manager(P p) noexcept { return object<TextureManager>(p); }
/// The object at `p` (see object()) as a `Material`.
template <class P>
inline Material* as_material(P p) noexcept { return object<Material>(p); }
/// The object at `p` (see object()) as a `Bone`.
template <class P>
inline Bone* as_bone(P p) noexcept { return object<Bone>(p); }
/// The object at `p` (see object()) as a `Billboard`.
template <class P>
inline Billboard* as_billboard(P p) noexcept { return object<Billboard>(p); }
/// The object at `p` (see object()) as a `Box`.
template <class P>
inline Box* as_box(P p) noexcept { return object<Box>(p); }
/// The object at `p` (see object()) as a `DateInfo`.
template <class P>
inline DateInfo* as_date_info(P p) noexcept { return object<DateInfo>(p); }
/// The object at `p` (see object()) as a `RecruitRow`.
template <class P>
inline RecruitRow* as_recruit_row(P p) noexcept { return object<RecruitRow>(p); }
/// The object at `p` (see object()) as a `Vec3`.
template <class P>
inline Vec3* as_vec3(P p) noexcept { return object<Vec3>(p); }
/// The object at `p` (see object()) as a `GameSet`.
template <class P>
inline GameSet* as_game_set(P p) noexcept { return object<GameSet>(p); }
/// The object at `p` (see object()) as a `DynArray`.
template <class P>
inline DynArray* as_dyn_array(P p) noexcept { return object<DynArray>(p); }
/// The object at `p` (see object()) as a `DynArrayB`.
template <class P>
inline DynArrayB* as_dyn_array_b(P p) noexcept { return object<DynArrayB>(p); }
/// The object at `p` (see object()) as a `Directory`.
template <class P>
inline Directory* as_directory(P p) noexcept { return object<Directory>(p); }

}  // namespace joan::orig

#include "gen/accessors.hpp"
#include "gen/free_functions.hpp"
