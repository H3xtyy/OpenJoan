// Compile-time check of the typed image hash table (hash_types.inc, re/replay/gen_hash_types.py) against the
// compiler's own layout of the data image (src/data/layout.hpp): every pointer entry names a pointer member (a
// pointer, a reference or uptr) of 4 bytes at its original offset, every untyped entry an array of exactly that many
// unsigned bytes at its offset. A retyped member makes this file fail to compile until the table is regenerated. Offsets
// are those of the original (x86) layout. The x64 build checks the translation the replay hashes through
// (x86_view.hpp: the x86-layout view of joan_image): every pointer member is 8 bytes at the x64 offset of its original
// address and the next field 8 bytes further, every untyped array lies at the x64 offset of its address in one run
// (JOAN_IMAGE_LAYOUT_PROBE, re/image/gen_xlate.py's layout probe: member types only). No code.
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "../../data/layout.hpp"
#include "x86_view.hpp"

namespace joan::replay::typed_check {

using data::layout::Image;

template <class T>
constexpr bool kIsPointer = std::is_pointer_v<T> || std::is_member_pointer_v<T> || std::is_same_v<T, data::uptr>;

// (offsetof on struct Image, which has members of classes with methods: fine for these packed plain-data classes)
#if defined(__clang__)
#pragma clang diagnostic ignored "-Winvalid-offsetof"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

#define JOAN_HASH_MEMBER(path) std::remove_cvref_t<decltype(std::declval<Image&>().path)>
#define JOAN_HASH_BASE_MEMBER(base, member) std::remove_cvref_t<decltype(std::declval<base&>().member)>
#if UINTPTR_MAX == 0xffffffffu
#define JOAN_HASH_PTR(va, path)                                                                                    \
    static_assert(kIsPointer<JOAN_HASH_MEMBER(path)> && sizeof(JOAN_HASH_MEMBER(path)) == 4 &&                     \
                      offsetof(Image, path) == (va) - 0x9cf000u,                                                    \
                  "hash_types.inc: " #path " (re/replay/gen_hash_types.py)");
#define JOAN_HASH_UNTYPED(va, size, path)                                                                          \
    static_assert(std::is_same_v<JOAN_HASH_MEMBER(path), data::u8[size]> && offsetof(Image, path) == (va) - 0x9cf000u, \
                  "hash_types.inc: " #path " (re/replay/gen_hash_types.py)");
#define JOAN_HASH_PTR_BASE(va, object, base, member)                                                       \
    static_assert(kIsPointer<JOAN_HASH_BASE_MEMBER(base, member)> &&                                          \
                      sizeof(JOAN_HASH_BASE_MEMBER(base, member)) == 4 &&                                    \
                      std::is_base_of_v<base, JOAN_HASH_MEMBER(object)> &&                                   \
                      offsetof(Image, object) + offsetof(base, member) == (va) - 0x9cf000u,                  \
                  "hash_types.inc: " #object " " #base "::" #member " (re/replay/gen_hash_types.py)");
#define JOAN_HASH_UNTYPED_BASE(va, size, object, base, member)                                               \
    static_assert(std::is_same_v<JOAN_HASH_BASE_MEMBER(base, member), data::u8[size]> &&                     \
                      std::is_base_of_v<base, JOAN_HASH_MEMBER(object)> &&                                   \
                      offsetof(Image, object) + offsetof(base, member) == (va) - 0x9cf000u,                  \
                  "hash_types.inc: " #object " " #base "::" #member " (re/replay/gen_hash_types.py)");
#elif !defined(JOAN_IMAGE_LAYOUT_PROBE)
// x64: the original address va lies at x86_view::x64_offset_of(va); a pointer is the widened 8-byte slot there
constexpr bool slot8(std::uint32_t va) {
    using x86_view::x64_offset_of;
    return x64_offset_of(va + 3) == x64_offset_of(va) + 3 && x64_offset_of(va + 4) == x64_offset_of(va) + 8;
}
constexpr bool contiguous(std::uint32_t va, std::uint32_t size) {
    return x86_view::x64_offset_of(va + size - 1) == x86_view::x64_offset_of(va) + size - 1;
}
#define JOAN_HASH_X64(va) x86_view::x64_offset_of(va)  // (offset from the image start 0x9cf000, run 0 at offset 0)
#define JOAN_HASH_PTR(va, path)                                                                                    \
    static_assert(kIsPointer<JOAN_HASH_MEMBER(path)> && sizeof(JOAN_HASH_MEMBER(path)) == 8 &&                     \
                      offsetof(Image, path) == JOAN_HASH_X64(va) && slot8(va),                                      \
                  "hash_types.inc: " #path " (re/replay/gen_hash_types.py, x64 view)");
#define JOAN_HASH_UNTYPED(va, size, path)                                                                          \
    static_assert(std::is_same_v<JOAN_HASH_MEMBER(path), data::u8[size]> &&                                        \
                      offsetof(Image, path) == JOAN_HASH_X64(va) && contiguous(va, size),                          \
                  "hash_types.inc: " #path " (re/replay/gen_hash_types.py, x64 view)");
#define JOAN_HASH_PTR_BASE(va, object, base, member)                                                       \
    static_assert(kIsPointer<JOAN_HASH_BASE_MEMBER(base, member)> &&                                          \
                      sizeof(JOAN_HASH_BASE_MEMBER(base, member)) == 8 &&                                    \
                      std::is_base_of_v<base, JOAN_HASH_MEMBER(object)> &&                                   \
                      offsetof(Image, object) + offsetof(base, member) == JOAN_HASH_X64(va) && slot8(va),    \
                  "hash_types.inc: " #object " " #base "::" #member " (re/replay/gen_hash_types.py, x64 view)");
#define JOAN_HASH_UNTYPED_BASE(va, size, object, base, member)                                               \
    static_assert(std::is_same_v<JOAN_HASH_BASE_MEMBER(base, member), data::u8[size]> &&                     \
                      std::is_base_of_v<base, JOAN_HASH_MEMBER(object)> &&                                   \
                      offsetof(Image, object) + offsetof(base, member) == JOAN_HASH_X64(va) &&               \
                      contiguous(va, size),                                                                  \
                  "hash_types.inc: " #object " " #base "::" #member " (re/replay/gen_hash_types.py, x64 view)");
static_assert(sizeof(Image) == x86_view::kX64ImageSize, "src/data/image_xlate.inc is stale (re/gen_data_curated.py)");
#else
#define JOAN_HASH_PTR_BASE(va, object, base, member)                    \
    static_assert(kIsPointer<JOAN_HASH_BASE_MEMBER(base, member)>, \
                  "hash_types.inc: " #object " " #base "::" #member " (re/replay/gen_hash_types.py)");
#define JOAN_HASH_UNTYPED_BASE(va, size, object, base, member)                           \
    static_assert(std::is_same_v<JOAN_HASH_BASE_MEMBER(base, member), data::u8[size]>, \
                  "hash_types.inc: " #object " " #base "::" #member " (re/replay/gen_hash_types.py)");
#define JOAN_HASH_PTR(va, path) \
    static_assert(kIsPointer<JOAN_HASH_MEMBER(path)>, "hash_types.inc: " #path " (re/replay/gen_hash_types.py)");
#define JOAN_HASH_UNTYPED(va, size, path)                                       \
    static_assert(std::is_same_v<JOAN_HASH_MEMBER(path), data::u8[size]>, \
                  "hash_types.inc: " #path " (re/replay/gen_hash_types.py)");
#endif
#include "hash_types.inc"
#undef JOAN_HASH_PTR
#undef JOAN_HASH_UNTYPED
#undef JOAN_HASH_PTR_BASE
#undef JOAN_HASH_UNTYPED_BASE
#undef JOAN_HASH_BASE_MEMBER
#undef JOAN_HASH_MEMBER
#ifdef JOAN_HASH_X64
#undef JOAN_HASH_X64
#endif

}  // namespace joan::replay::typed_check
