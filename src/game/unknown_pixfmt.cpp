// game/unknown_pixfmt.cpp: functions whose original source file is unknown (formerly game/_unplaced/pixfmt.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/bind.hpp"
#include "pixfmt.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b109_11.cpp (game/b109_11)
namespace {
namespace game_b109_11 {

using u8 = std::uint8_t;

using joan::call::cdecl_call;

using i32 = std::int32_t;

using u16 = std::uint16_t;

JOAN_COUNTER(c_pixfmt, "set_pixel_format_5706f0");

// ---------------------------------------------------------------- 0x005706f0
// Op table 0xa15073: slot 0 pack (0x00RRGGBB -> pixel), slot 1 unpack. The original also filled slots 2..10
// with the half-blend ops for the RLE blend blitters (escape codes 0xef..0xf7); those blitters, the escape ops
// and the blend ops were dead and are deleted (cleanup C1), so only the two live slots are stored.
constexpr fn::Code kOpsRgb15[2] = {fn::Pixel_pack_rgb555, fn::Pixel_unpack_rgb555};
constexpr fn::Code kOpsRgb16[2] = {fn::Pixel_pack_rgb565, fn::Pixel_unpack_rgb565};
constexpr fn::Code kOpsBgr15[2] = {fn::Pixel_pack_bgr555, fn::Pixel_unpack_bgr555};
constexpr fn::Code kOpsBgr16[2] = {fn::Pixel_pack_bgr565, fn::Pixel_unpack_bgr565};
constexpr fn::Code kOpsRgb32[2] = {fn::Pixel_pack_rgb32, fn::Pixel_unpack_rgb32};
constexpr fn::Code kOpsBgr32[2] = {fn::Pixel_pack_bgr32, fn::Pixel_unpack_bgr32};

void pf_ops(const fn::Code* ops) {
    for (i32 i = 0; i < 2; ++i) global<u32>(d::g_a15073 + 4 * i) = static_cast<u32>(fn::code_value(ops[i]));
}
// 16-bit formats: word masks at 0xa15024/26/28, shifts at 0xa15036/37/38, shade masks 0xa1503d..
void pf_16(const fn::Code* ops, u16 m24, u16 m26, u16 m28, u8 s36, u8 s37, u8 s38, bool six5) {
    pf_ops(ops);
    global<u32>(d::g_a15039) = 2;
    global<u16>(d::g_a15024) = m24;
    global<u16>(d::g_a15026) = m26;
    global<u16>(d::g_a15028) = m28;
    global<u8>(d::g_a15036) = s36;
    global<u8>(d::g_a15037) = s37;
    global<u8>(d::g_a15038) = s38;
    const u16 shades555[9] = {0x7fff, 0x3def, 0x1ce7, 0x0c63, 0x0421, 0, 0, 0, 0};
    const u16 shades565[9] = {0xffff, 0x7bef, 0x39e7, 0x18e3, 0x0861, 0x0020, 0, 0, 0};
    const u16* shades = six5 ? shades565 : shades555;
    for (i32 i = 0; i < 9; ++i) global<u16>(d::g_a1503d + 2 * i) = shades[i];
}
// 24 / 32-bit formats: dword masks at 0xa1502a/2e/32, shifts, shade masks 0xa1504f..
void pf_32(const fn::Code* ops, u32 bpp, bool bgr) {
    pf_ops(ops);
    global<u32>(d::g_a15039) = bpp;
    global<u32>(d::g_a1502a) = bgr ? 0xff0000u : 0xffu;
    global<u32>(d::g_a1502e) = 0xff00u;
    global<u32>(d::g_a15032) = bgr ? 0xffu : 0xff0000u;
    global<u8>(d::g_a15036) = bgr ? u8{8} : u8{0x18};
    global<u8>(d::g_a15037) = 0x10;
    global<u8>(d::g_a15038) = bgr ? u8{0x18} : u8{8};
    const u32 shades[9] = {0xffffff, 0x7f7f7f, 0x3f3f3f, 0x1f1f1f, 0x0f0f0f, 0x070707, 0x030303, 0x010101, 0};
    for (i32 i = 0; i < 9; ++i) global<u32>(d::g_a1504f + 4 * i) = shades[i];
}

i32 JOAN_STDCALL set_pixel_format(i32 mode) {
    c_pixfmt.hit();
    global<i32>(d::g_asm_get_a15020) = mode;
    // The per-format init stubs preserve every register and take no arguments.
    switch (mode) {
    case 0:
        pf_16(kOpsRgb15, 0x1f, 0x3e0, 0x7c00, 0xb, 6, 1, false);
        joan::orig::pixel::init_rgb555();  // thunk -> 0x00571248
        break;
    case 1:
        pf_16(kOpsRgb16, 0x1f, 0x7e0, 0xf800, 0xb, 5, 0, true);
        joan::orig::pixel::init_rgb565();  // thunk -> 0x005712df
        break;
    case 2:
        pf_16(kOpsBgr15, 0x7c00, 0x3e0, 0x1f, 1, 6, 0xb, false);
        joan::orig::pixel::init_bgr555();  // thunk -> 0x00571369
        break;
    case 3:
        pf_16(kOpsBgr16, 0xf800, 0x7e0, 0x1f, 0, 5, 0xb, true);
        joan::orig::pixel::init_bgr565();  // thunk -> 0x005713f3
        break;
    case 4:
        pf_32(kOpsRgb32, 3, false);
        joan::orig::pixel::init_rgb32();  // thunk -> 0x0057145a
        break;
    case 5:
        pf_32(kOpsBgr32, 3, true);
        joan::orig::pixel::init_bgr32();  // thunk -> 0x005714d2
        break;
    case 6:
    case 10:
        pf_32(kOpsRgb32, 4, false);
        joan::orig::pixel::init_rgb32();  // thunk -> 0x0057145a
        break;
    default:  // 7 and every other value
        pf_32(kOpsBgr32, 4, true);
        joan::orig::pixel::init_bgr32();  // thunk -> 0x005714d2
        break;
    }
    return mode;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_11", name, addr, fn)
B("set_pixel_format_5706f0", 0x005706f0, &set_pixel_format);
#undef B

}  // namespace game_b109_11
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

// 0x005711b7 (hand-written asm): returns dword [0xa15020].
std::uint32_t JOAN_CDECL asm_get_a15020() { return global<std::uint32_t>(d::g_asm_get_a15020); }

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("asm_get_a15020", 0x005711b7, &asm_get_a15020);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b115_02.cpp (game/b115_02)
namespace {
namespace game_b115_02 {

using i32 = std::int32_t;

// Pixel-format ops (hand-written asm in joan.exe, ported to C++ in P1). In the original each op took its operand in
// eax and returned it in eax, and Pixel::pack_color / unpack_color called them through the op table 0xa15073 /
// 0xa15077 that set_pixel_format fills. Those two were the only callers, so caller and callees are converted
// together: the ops are plain functions u32(u32), and pack_color / unpack_color pick them from the format mode
// 0xa15020, which set_pixel_format (the only writer of the mode and of the table) stores together with the table.
// ops_of below is set_pixel_format's switch. (Before the first set_pixel_format the original called a null table
// entry; mode 0 then selects rgb555 here.) The table is still filled for the game state but no longer called
// through. Bit operations are transcribed instruction by instruction (rotates, byte / word partial shifts, partial
// register merges), so every bit of eax - also the bits above the colour - is the original's.
constexpr u32 rotl(u32 v, u32 n) { return (v << n) | (v >> (32u - n)); }
constexpr u32 rotr(u32 v, u32 n) { return (v >> n) | (v << (32u - n)); }
constexpr u32 bswap(u32 v) {
    return (v >> 24) | ((v >> 8) & 0xff00u) | ((v << 8) & 0xff0000u) | (v << 24);
}
constexpr u32 shr_al(u32 v, u32 n) { return (v & ~0xffu) | ((v & 0xffu) >> n); }       // shr al, n
constexpr u32 shr_ax(u32 v, u32 n) { return (v & ~0xffffu) | ((v & 0xffffu) >> n); }  // shr ax, n
// mov edx, eax / shl eax, a / shl edx, b / mov ah, dh / shr edx, c / mov al, dl
constexpr u32 unpack16(u32 v, u32 a, u32 b, u32 c) {
    const u32 hi = v << a, mid = v << b, lo = mid >> c;
    return (hi & 0xffff0000u) | (mid & 0xff00u) | (lo & 0xffu);
}
using Op = u32(JOAN_CDECL*)(u32);

// ------------------------------------------------------------------ 0x00571225 Pixel::pack_rgb555 (thunk 0x0040833c)
// 0x00RRGGBB -> surface pixel (op table slot 0): ror 16, shr al 3, rol 8, shr ax 3, rol 5.
u32 JOAN_CDECL pack_rgb555_571225(u32 c) { return rotl(shr_ax(rotl(shr_al(rotr(c, 16), 3), 8), 3), 5); }
// ------------------------------------------------------------------ 0x00571236 Pixel::unpack_rgb555 (thunk 0x004064f1)
// surface pixel -> 0x00RRGGBB (op table slot 1).
u32 JOAN_CDECL unpack_rgb555_571236(u32 p) { return unpack16(p, 9, 6, 3); }
// ------------------------------------------------------------------ 0x00571248 Pixel::init_rgb555 (thunk 0x00408503)
// pushad, an empty 32-step counting loop, popad: no effect but the flags. Callers: set_pixel_format 0x5706f0.
void JOAN_CDECL init_rgb555_571248() {}
// ------------------------------------------------------------------ 0x005712bc Pixel::pack_rgb565 (thunk 0x004081a2)
u32 JOAN_CDECL pack_rgb565_5712bc(u32 c) { return rotl(shr_ax(rotl(shr_al(rotr(c, 16), 3), 8), 2), 5); }
// ------------------------------------------------------------------ 0x005712cd Pixel::unpack_rgb565 (thunk 0x00406541)
u32 JOAN_CDECL unpack_rgb565_5712cd(u32 p) { return unpack16(p, 8, 5, 2); }
// ------------------------------------------------------------------ 0x005712df Pixel::init_rgb565 (thunk 0x00408076)
// A bare ret (per-format init hook), as are the other init_* below. Callers: set_pixel_format 0x5706f0.
void JOAN_CDECL init_rgb565_5712df() {}
// ------------------------------------------------------------------ 0x00571344 Pixel::pack_bgr555 (thunk 0x0040819d)
// bswap, rol 8, shr al 3, rol 8, shr ax 3, rol 5.
u32 JOAN_CDECL pack_bgr555_571344(u32 c) { return rotl(shr_ax(rotl(shr_al(rotl(bswap(c), 8), 3), 8), 3), 5); }
// ------------------------------------------------------------------ 0x00571357 Pixel::unpack_bgr555 (thunk 0x00401861)
u32 JOAN_CDECL unpack_bgr555_571357(u32 p) { return unpack16(p, 0x13, 6, 0xd); }
// ------------------------------------------------------------------ 0x00571369 Pixel::init_bgr555 (thunk 0x00403580)
void JOAN_CDECL init_bgr555_571369() {}
// ------------------------------------------------------------------ 0x005713ce Pixel::pack_bgr565 (thunk 0x00408198)
u32 JOAN_CDECL pack_bgr565_5713ce(u32 c) { return rotl(shr_ax(rotl(shr_al(rotl(bswap(c), 8), 3), 8), 2), 5); }
// ------------------------------------------------------------------ 0x005713e1 Pixel::unpack_bgr565 (thunk 0x00401852)
u32 JOAN_CDECL unpack_bgr565_5713e1(u32 p) { return unpack16(p, 0x13, 5, 0xd); }
// ------------------------------------------------------------------ 0x005713f3 Pixel::init_bgr565 (thunk 0x0040357b)
void JOAN_CDECL init_bgr565_5713f3() {}
// ------------------------------------------------------------------ 0x00571458 Pixel::pack_rgb32 (thunk 0x00408337)
// A bare ret: the colour is the pixel.
u32 JOAN_CDECL pack_rgb32_571458(u32 c) { return c; }
// ------------------------------------------------------------------ 0x00571459 Pixel::unpack_rgb32 (thunk 0x00406a69)
u32 JOAN_CDECL unpack_rgb32_571459(u32 p) { return p; }
// ------------------------------------------------------------------ 0x0057145a Pixel::init_rgb32 (thunk 0x004084ea)
void JOAN_CDECL init_rgb32_57145a() {}
// ------------------------------------------------------------------ 0x005714c6 Pixel::pack_bgr32 (thunk 0x00408332)
// bswap, ror 8.
u32 JOAN_CDECL pack_bgr32_5714c6(u32 c) { return rotr(bswap(c), 8); }
// ------------------------------------------------------------------ 0x005714cc Pixel::unpack_bgr32 (thunk 0x00401d52)
// rol 8, bswap.
u32 JOAN_CDECL unpack_bgr32_5714cc(u32 p) { return bswap(rotl(p, 8)); }
// ------------------------------------------------------------------ 0x005714d2 Pixel::init_bgr32 (thunk 0x00403701)
void JOAN_CDECL init_bgr32_5714d2() {}

// The ops set_pixel_format 0x5706f0 puts into the op table for `mode` (its switch: 0 rgb555, 1 rgb565, 2 bgr555,
// 3 bgr565, 4 / 6 / 10 rgb32, 5 and every other value bgr32).
struct Ops {
    Op pack, unpack;
};
Ops ops_of(i32 mode) {
    switch (mode) {
        case 0: return {&pack_rgb555_571225, &unpack_rgb555_571236};
        case 1: return {&pack_rgb565_5712bc, &unpack_rgb565_5712cd};
        case 2: return {&pack_bgr555_571344, &unpack_bgr555_571357};
        case 3: return {&pack_bgr565_5713ce, &unpack_bgr565_5713e1};
        case 4:
        case 6:
        case 10: return {&pack_rgb32_571458, &unpack_rgb32_571459};
        default: return {&pack_bgr32_5714c6, &unpack_bgr32_5714cc};
    }
}

// ------------------------------------------------------------------ 0x00571205 Pixel::pack_color (thunk 0x00407ef0)
// Pixel::pack_color(colour 0x00RRGGBB) -> the current format's pack op (op table slot 0). stdcall, 1 stack arg.
// Caller: rgb_to_pixel 0x610160 (game_b108_07; Pal::rgb_610110 was dead, deleted in C1).
u32 JOAN_STDCALL pack_color_571205(u32 colour) { return ops_of(global<i32>(d::g_asm_get_a15020)).pack(colour); }

// ------------------------------------------------------------------ 0x00571215 Pixel::unpack_color (thunk 0x004050e7)
// Pixel::unpack_color(pixel) -> the current format's unpack op (op table slot 1). stdcall, 1 stack arg (called as
// thiscall through the slot: ecx is ignored, the callee pops 4 bytes either way). Callers: 0x6101b0 (game_b108_04).
u32 JOAN_STDCALL unpack_color_571215(u32 pixel) { return ops_of(global<i32>(d::g_asm_get_a15020)).unpack(pixel); }

#define R(name, addr, fn) JOAN_REPLACE("game/b115_02", name, addr, &fn)
R("Pixel::pack_color", 0x00571205, pack_color_571205);
R("Pixel::unpack_color", 0x00571215, unpack_color_571215);
R("Pixel::pack_rgb555", 0x00571225, pack_rgb555_571225);
R("Pixel::unpack_rgb555", 0x00571236, unpack_rgb555_571236);
R("Pixel::init_rgb555", 0x00571248, init_rgb555_571248);
R("Pixel::pack_rgb565", 0x005712bc, pack_rgb565_5712bc);
R("Pixel::unpack_rgb565", 0x005712cd, unpack_rgb565_5712cd);
R("Pixel::init_rgb565", 0x005712df, init_rgb565_5712df);
R("Pixel::pack_bgr555", 0x00571344, pack_bgr555_571344);
R("Pixel::unpack_bgr555", 0x00571357, unpack_bgr555_571357);
R("Pixel::init_bgr555", 0x00571369, init_bgr555_571369);
R("Pixel::pack_bgr565", 0x005713ce, pack_bgr565_5713ce);
R("Pixel::unpack_bgr565", 0x005713e1, unpack_bgr565_5713e1);
R("Pixel::init_bgr565", 0x005713f3, init_bgr565_5713f3);
R("Pixel::pack_rgb32", 0x00571458, pack_rgb32_571458);
R("Pixel::unpack_rgb32", 0x00571459, unpack_rgb32_571459);
R("Pixel::init_rgb32", 0x0057145a, init_rgb32_57145a);
R("Pixel::pack_bgr32", 0x005714c6, pack_bgr32_5714c6);
R("Pixel::unpack_bgr32", 0x005714cc, unpack_bgr32_5714cc);
R("Pixel::init_bgr32", 0x005714d2, init_bgr32_5714d2);
#undef R

}  // namespace game_b115_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::set_pixel_format_5706f0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b109_11::set_pixel_format>(a0)); }  // 0x5706f0
auto joan::orig::asm_get_a15020() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b90_04::asm_get_a15020>()); }  // 0x5711b7
auto joan::orig::pixel::pack_color(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b115_02::pack_color_571205>(a0)); }  // 0x571205
auto joan::orig::pixel::init_rgb555() -> void { joan::bind::free<&::game_b115_02::init_rgb555_571248>(); }  // 0x571248
auto joan::orig::pixel::init_rgb565() -> void { joan::bind::free<&::game_b115_02::init_rgb565_5712df>(); }  // 0x5712df
auto joan::orig::pixel::init_bgr555() -> void { joan::bind::free<&::game_b115_02::init_bgr555_571369>(); }  // 0x571369
auto joan::orig::pixel::init_bgr565() -> void { joan::bind::free<&::game_b115_02::init_bgr565_5713f3>(); }  // 0x5713f3
auto joan::orig::pixel::init_rgb32() -> void { joan::bind::free<&::game_b115_02::init_rgb32_57145a>(); }  // 0x57145a
auto joan::orig::pixel::init_bgr32() -> void { joan::bind::free<&::game_b115_02::init_bgr32_5714d2>(); }  // 0x5714d2

// ---------------------------------------- hand-written entry points (pixfmt.hpp)
std::uint32_t joan::pixfmt::unpack_color(std::uint32_t pixel) { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b115_02::unpack_color_571215>(pixel)); }  // 0x571215
