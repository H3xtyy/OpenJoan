// Batch S3 (world, re/a6/SAVES_PLAN.md 4): the generated records of section s3 (gen/records_s3.hpp) plus the one
// chunk the generator cannot express alone: the Player chunk (.SAV step 15, 0x126e bytes), whose dword +0x1261 is a
// 32-bit scalar holding a data image address (D1, SAVES_PLAN 2.4). It is put together from the generated PlayerHead
// [0, 0x1261) and PlayerTail [0x1265, 0x126e) and that word.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "gen/records_s3.hpp"
#include "io.hpp"

namespace joan::save {

namespace s3_detail {
template <std::size_t A, std::size_t B>
struct Joined {
    Field f[A + 1 + B];
};
// a, then mid, then b with its offsets moved by b_off (the fields of one chunk made of two generated records).
template <std::size_t A, std::size_t B>
constexpr Joined<A, B> join(const Field (&a)[A], const Field& mid, const Field (&b)[B], std::uint32_t b_off) {
    Joined<A, B> j{};
    for (std::size_t i = 0; i < A; ++i) j.f[i] = a[i];
    j.f[A] = mid;
    for (std::size_t i = 0; i < B; ++i) {
        j.f[A + 1 + i] = b[i];
        j.f[A + 1 + i].off += b_off;
    }
    return j;
}
inline constexpr std::uint32_t kPlayerWordOff = 0x1261;
inline constexpr auto kPlayerFields = join(kFields_PlayerHead, Field{kPlayerWordOff, 4, 4, "u32_1261", Type::Word, Ptr::Image},
                                           kFields_PlayerTail, kPlayerWordOff + 4);
}  // namespace s3_detail

static_assert(kPlayerHead.size == s3_detail::kPlayerWordOff && kPlayerTail.size == 0x126e - 0x1265);
/// Player (g_a5b9e8, GameFile::write_camera_4857c0 / Obj::load_485930): PlayerHead + u32_1261 + PlayerTail.
inline constexpr Record kPlayer{"Player", "Player", 0x126e, Legacy::Raw, s3_detail::kPlayerFields.f,
                                sizeof(s3_detail::kPlayerFields.f) / sizeof(Field)};

/// u32_1261 is written as joan.exe's original address (Ptr::Image: the current build wrote the relocated one - D1).
template <class T>
void put_Player(Out& o, const T& r) {
#if UINTPTR_MAX == 0xffffffffu
    static_assert(offsetof(T, rtsicon_res) == s3_detail::kPlayerWordOff);
#endif
    put_PlayerHead(o, r);
    o.ptr(r.rtsicon_res, Ptr::Image);
    put_PlayerTail(o, r);
}
/// u32_1261 is read back as stored (= the raw read; X::reset_state 0x00485990 zeroes it right after; on x64 the
/// stored original address is never dereferenced before that).
template <class T>
void get_Player(In& i, T& r) {
    get_PlayerHead(i, r);
    r.rtsicon_res = reinterpret_cast<decltype(r.rtsicon_res)>(static_cast<std::uintptr_t>(i.u32()));
    get_PlayerTail(i, r);
}

/// SaveCompare's old bytes of the Player chunk (x86): the raw object with D1 applied, so that only unintended
/// differences are reported. buf: 0x126e bytes.
inline const void* player_legacy(std::uint8_t* buf, const void* obj) {
    std::memcpy(buf, obj, kPlayer.size);
    std::uint32_t v;
    std::memcpy(&v, buf + s3_detail::kPlayerWordOff, 4);
    v = hooks().image_out(v);
    std::memcpy(buf + s3_detail::kPlayerWordOff, &v, 4);
    return buf;
}

}  // namespace joan::save
