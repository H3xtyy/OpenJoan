// interfac/i_buy.cpp: functions of the original d:\ww\interfac\i_buy.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../fp/fma.hpp"
#include "../original/helpers.hpp"
#include "../callconv.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdlib>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../display.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using joan::hook::x87::Fpu;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;
constexpr u32 kOne = 0x3f800000u;  // 1.0f pushed as an immediate

constexpr img::Addr kSaint = d::s_Saint_Catherine_de_Fierbois;  // "Saint-Catherine-de-Fierbois"
constexpr img::Addr kSword = d::s_JOAN_SWORD_LV10;  // "JOAN_SWORD_LV10"

i32 mouse_x() { return global<i32>(d::g_mouse_x); }
i32 mouse_y() { return global<i32>(d::g_mouse_y); }

void* const kUnits = obj(d::g_units);

void* const kSpeed = obj(d::g_game_a50088);

void* const kTypes = obj(d::g_types);

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

using joan::orig::helpers::addr;  // a pointer left in eax (stale results below)

// A 4-byte member's bits (the original moved floats / ints between camera key records as dwords).
template <class M>
u32& u32_of(M& m) {
    return jo::view<u32>(m);
}
template <class M>
const u32& u32_of(const M& m) {
    return jo::view<u32>(m);
}
// Key i of a camera track (CamTrack::keys, 0x2c-byte records from +0x76).
jo::CamKey& cam_key(void* track, i32 i) {
    return *reinterpret_cast<jo::CamKey*>(static_cast<u8*>(track) + (offsetof(jo::CamTrack, keys) + static_cast<std::size_t>(i) * sizeof(jo::CamKey)));
}

}  // namespace

// ---------------------------------------- from modules/game_b116_05.cpp (game/b116_05)
//
//    - Shop::release_6c27c0 0x006c27c0 (thiscall, ret 0; callers 0x00423d4b / 0x0047066f ignore
//      eax): 0x006c90e0, deletes the two panels +0x8 / +0xc (0x006b33a0(1)), frees the price
//      table +0x5b (0x0058ed70, i_buy.cpp line [0xa237ac] + 0x13), clears +0x56, for the two
//      model slots (+0x138 / +0x134 in the scene +0x5f, +0x221 / +0x21d in the same scene): scene
//      remove (0x005ea2f0), delete (0x00420840(1)), clear the handles, stop the sound
//      (0x004b16a0 on 0x00a50568 when the id > 0), release the camera (0x00613a10 + 0x00562910(1)),
//      then 0x005e9b10 on +0x5f and +0x148. canvas_api.hpp: only listed as the end of the
//
//    - Shop::draw 0x006c4070 (thiscall, ret 0; only caller 0x004d612c ignores eax): background
//      "main" and the coin pile sprite (coins1..4 by the gold +0x57), gold amount (0x00626080),
//      the two titles (0x00626e00), the panel +0x8, both item grids (player +0x30 at x+0x100,
//      shop +0x4e at x+0x210: 0x006c2ba0 item sprite, "#M" suffix when hovered, the
//      Saint-Catherine-de-Fierbois sword shown as JOAN_SWORD_LV10[_n], count / level text with
//      Text::draw_line 0x006255d0), then (no drag in progress, +0x11 == 0) the hovered item's
//      tooltip in either grid (0x005a4c30 stat text, price, sell value / unavailable text,
//
//  - 0x006c4070's locals live in a byte array laid out exactly like the original frame
//    ([ebp-0x840] .. [ebp-0xc], plus padding), and every local is re-read from it where the
//    original reads memory: an over-long item name (strcpy into the 0x84-byte buffers
//    [ebp-0x254] / [ebp-0x2f0]), more than 15 stat values (arrays of 15 dwords), a long text
//    sprintf'd into the 0x40-byte buffers [ebp-0x52c] / [ebp-0x7b0], or an over-long 0x005a1170
//    path object overwrite the same neighbours (this, the item records, the coordinates, the
//    loop counters) as in the original. Writes past [ebp-0xc] (the SEH record / saved ebp /
//    return address in the original - a crash there) go to the padding; stat values past the
//    padding are dropped. Argument temporaries below [ebp-0x7b0] are never overwritten (all
//    overruns go upwards) and are plain C++ values.
//
//  - SEH frame of 0x006c4070 (state 0 around the 0x00456eb0 object [ebp-0x590]): normal path only,
//    no callee throws.
namespace {
namespace shop_cameras { int g_count = 0; }  // the shop's portrait cameras on record (game_b116_13); 0 once released
namespace game_b116_05 {

using joan::hook::x87::ftol;

// thiscall through the object's vtable slot `off`.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
i32 sx(u16 v) { return static_cast<i32>(static_cast<i16>(v)); }
i32 sxb(u8 v) { return static_cast<i32>(static_cast<i8>(v)); }

// CRT (called at the original addresses)
void crt_memset(void* d, i32 c, u32 n) { std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }
void crt_strcpy(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void crt_strcat(void* d, const void* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
u32 crt_strlen(const void* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
i32 crt_stricmp(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
void crt_sprintf(void* d, const void* fmt) { joan::orig::libc::sprintf(joan::orig::arg::str(d), joan::orig::arg::str(fmt)); }

constexpr img::Addr kItems = d::g_item_res;  // item records, 0x00425ce0 (word id)
constexpr img::Addr kTexts = d::g_strings;  // localised texts
constexpr img::Addr kMisc = d::g_misc;   // 0x0058fca0 / 0x004e1360 number formatting
constexpr img::Addr kMouse = d::g_mouse;  // 0x00593980 in-rect test
constexpr img::Addr kSprites = d::g_sprites_a46128;
constexpr img::Addr kHover = d::s_M_9ddd9c;  // "#M"
constexpr img::Addr kNl = d::kNl;     // "\n"

using joan::orig::helpers::screen_w;
using joan::orig::helpers::screen_h;
i32 ox() { return (screen_w() - 0x320) / 2; }  // sub 0x320; cdq; sub eax,edx; sar 1
i32 oy() { return (screen_h() - 0x258) / 2; }

u8* item_rec(const u8* e) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(kItems))->get(joan::orig::arg::i16(sx(jo::view<u16>(jo::as_inv_item(e)->type))))); }  // 0x00425ce0
// 0x005e9640 on a 4-byte temporary (ret 4): returns a pointer to the packed colour.
u32 rgba(u32 v) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}
i32 in_rect(i32 x1, i32 y1, i32 x2, i32 y2) { return joan::orig::arg::as<i32>(joan::orig::as_point(obj(kMouse))->inside(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2))); }  // 0x00593980
// Text::draw_line 0x006255d0 on 0x00a47d18 (12 args, ret 0x30).
void draw_line(i32 x, i32 y, const char* text, u32 col) {
    joan::orig::as_text(obj(d::g_font_a47d18))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(i32{6}), text, joan::orig::arg::u32(col), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
}

JOAN_COUNTER(c_release, "Shop::release_6c27c0");
JOAN_COUNTER(c_draw, "Shop::draw");

// ---------------------------------------------------------------------------------- 0x006c4070
// Original frame from [ebp-0x840] (esp after `sub esp,0x834`) to [ebp-0xc] (exclusive), plus
// padding for writes that would hit the SEH record / saved ebp / return address.
constexpr i32 kFrameLow = 0x840;
constexpr i32 kFramePad = 0x1000;
struct ShopFrame {
    u8 bytes[kFrameLow - 0xc + kFramePad];
    // The pointer locals of the original frame (this, unit, shop record, the two grid entries, the selected inventory
    // entries, the entry row and the two item definitions) are typed members: a frame dword cannot hold an x64 pointer.
    u8* self = nullptr;      // [ebp-0x10]
    u8* unit = nullptr;      // [ebp-0x14]
    u8* shop_rec = nullptr;  // [ebp-0x18]
    u8* e[2] = {};           // [ebp-0x1c0] / [ebp-0x25c]
    u8* sel = nullptr;       // [ebp-0x304]
    u8* ent = nullptr;       // [ebp-0x30c]
    u8* def1 = nullptr;      // [ebp-0x4a4]
    // The two argument blocks of format_5a4c30 ([ebp-0x4e0] / [ebp-0x764], 15 dwords each in the original): pointer-sized
    // slots now (the callee reads them so); a 16th value, which overran into the neighbouring locals, is dropped.
    std::uintptr_t arr1[15] = {}, arr2[15] = {};
    static void arg_put(std::uintptr_t (&a)[15], i32 n, u32 v) {
        if (n >= 0 && n < 15) a[n] = v;
    }
    u8* sel2 = nullptr;      // [ebp-0x53c]
    u8* def2 = nullptr;      // [ebp-0x728]
    u8* p(i32 off) { return bytes + (kFrameLow + off); }  // off = ebp offset (negative)
    u32& dw(i32 off) { return *reinterpret_cast<u32*>(p(off)); }
    i32& in(i32 off) { return *reinterpret_cast<i32*>(p(off)); }
    // mov [ebp + n*4 + base], v (n = the stored counter): dropped past the padding.
    void put(i32 base, i32 n, u32 v) {
        const long long off = static_cast<long long>(base) + static_cast<long long>(n) * 4;
        if (off >= -kFrameLow && off + 4 <= -0xc + kFramePad) dw(static_cast<i32>(off)) = v;
    }
};
// ebp offsets
constexpr i32 kMx = -0x1c, kMy = -0x20, kCoins = -0x1b4,
              kCol = -0x1b8, kI = -0x1bc;
// item grid entry: player grid (base) / shop grid (base2)
struct GridLocals {
    i32 slot, x, y, x2, y2, name, diff;  // slot: index into ShopFrame::e (the entry pointer, [ebp-0x1c0] / [ebp-0x25c])
};
constexpr GridLocals kG1 = {0, -0x1c4, -0x1c8, -0x1cc, -0x1d0, -0x254, -0x258};
constexpr GridLocals kG2 = {1, -0x260, -0x264, -0x268, -0x26c, -0x2f0, -0x2f4};
// tooltip (player grid)
constexpr i32 kCx = -0x2f8, kCy = -0x2fc, kK = -0x300, kJ = -0x308, kPath1 = -0x4a0,
              kN1 = -0x4e4, kL1 = -0x4e8, kPrice1 = -0x4ec, kBuf1 = -0x52c;
// tooltip (shop grid)
constexpr i32 kCx2 = -0x530, kCy2 = -0x534, kK2 = -0x538, kTmp = -0x590, kPath2 = -0x724,
              kN2 = -0x768, kL2 = -0x76c, kPrice2 = -0x770, kBuf2 = -0x7b0;


// Value of one stat row: rows[j].base + rows[j].per_level * (level - item[0x36]).
template <class Row>
u32 stat_row(const Row* rows, i32 j, u32 level_minus) {
    const u32 k = jo::view<u32>(rows[j].per_level);
    return jo::view<u32>(rows[j].base) + k * level_minus;
}

// One grid's item loop (0x006c4478..0x006c4803 / 0x006c4803..0x006c4b7c).
// grid: inventory field (+0x30 / +0x4e); xoff: 0x100 / 0x210; drag_from_shop: the dragged entry
// (+0x231 set, index +0x23d) is hidden in this grid when (+0x241 != 0) equals it; upper: the
// player grid also requires count < 1000 for the count text.
jo::Grid*& grid_of(u8* s, bool shop) { return shop ? jo::as_buy_screen(s)->items.grid_16 : jo::as_buy_screen(s)->items_b.grid_16; }
void draw_grid(ShopFrame& fr, i32 xoff, bool drag_from_shop, bool upper, const GridLocals& g) {
    fr.dw(kI) = joan::orig::arg::as<u32>(joan::orig::as_dynarray(grid_of(fr.self, drag_from_shop))->size());  // 0x00417ad0
    for (;; fr.in(kI) = fr.in(kI) - 1) {
        if (fr.in(kI) <= 0) break;
        if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(grid_of(fr.self, drag_from_shop))->is_empty_at(joan::orig::arg::i32(fr.in(kI)))) != 0) continue;  // 0x00457f60
        if (jo::view<u32>(jo::as_buy_screen(fr.self)->drag_state) != 0 && (jo::view<u32>(jo::as_buy_screen(fr.self)->drag_from_shop) != 0) == drag_from_shop &&
            fr.dw(kI) == jo::view<u32>(jo::as_buy_screen(fr.self)->drag_index))
            continue;
        fr.e[g.slot] = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(grid_of(fr.self, drag_from_shop))->get(joan::orig::arg::i32(fr.in(kI))));  // 0x00417200
        {
            const i32 base = ox();
            fr.in(g.x) = base + sxb(jo::view<u8>(jo::as_inv_item(fr.e[g.slot])->x)) * 0x28 + xoff;
        }
        {
            const i32 base = oy();
            fr.in(g.y) = base + sxb(jo::view<u8>(jo::as_inv_item(fr.e[g.slot])->y)) * 0x28 + 0x38;
        }
        {
            const i32 w = sxb(jo::view<u8>(jo::as_inv_item(fr.e[g.slot])->w)) * 0x28;
            fr.in(g.x2) = fr.in(g.x) + w - 1;
        }
        {
            const i32 h = sxb(jo::view<u8>(jo::as_inv_item(fr.e[g.slot])->h)) * 0x28;
            fr.in(g.y2) = fr.in(g.y) + h - 1;
        }
        char* const name = reinterpret_cast<char*>(fr.p(g.name));
        if (crt_stricmp(jo::as_item_def(item_rec(fr.e[g.slot]))->file_name, str(kSaint)) == 0) {
            crt_memset(name, 0, 0x82);
            crt_strcpy(name, str(kSword));
            const u32 len = crt_strlen(jo::as_item_def(item_rec(fr.e[g.slot]))->name);
            fr.dw(g.diff) = len - crt_strlen(str(kSword));
            if (fr.dw(g.diff) != 0) {
                const u32 n = crt_strlen(name);
                *fr.p(static_cast<i32>(n) + g.name) = 0x5f;
                const u8 digit = static_cast<u8>(fr.dw(g.diff) + 0x30);
                const u32 n2 = crt_strlen(name);
                *fr.p(static_cast<i32>(n2) + g.name) = digit;
            }
        } else {
            crt_strcpy(name, jo::as_item_def(item_rec(fr.e[g.slot]))->name);
        }
        if (in_rect(fr.in(g.x), fr.in(g.y), fr.in(g.x2), fr.in(g.y2)) != 0) crt_strcat(name, str(kHover));
        joan::orig::as_shop(fr.self)->draw_item_icon(joan::orig::arg::i32(fr.in(g.x)), joan::orig::arg::i32(fr.in(g.y)), joan::orig::arg::str(name));  // 0x006c2ba0
        const i32 count = sx(jo::view<u16>(jo::as_inv_item(fr.e[g.slot])->count));
        if (count > 1 && (!upper || sx(jo::view<u16>(jo::as_inv_item(fr.e[g.slot])->count)) < 0x3e8)) {
            const u32 col = rgba(0xffffffffu);
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(kMisc))->format_int_short(joan::orig::arg::i16(sx(jo::view<u16>(jo::as_inv_item(fr.e[g.slot])->count))), joan::orig::arg::u32(i32{1})));  // 0x004e1360
            draw_line(sx(static_cast<u16>(fr.dw(g.x))), sx(static_cast<u16>(fr.dw(g.y))), text, col);
        } else if (sx(jo::as_item_def(item_rec(fr.e[g.slot]))->class_id) == 9) {
            const u32 col = rgba(0xffffffffu);
            const i32 level = sx(jo::view<u16>(jo::as_inv_item(fr.e[g.slot])->level));
            const u32 lv = static_cast<u32>(level) - jo::view<u32>(jo::as_item_def(item_rec(fr.e[g.slot]))->min_level) + 1u;
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(kTexts))->invent_text_1(joan::orig::arg::u32(static_cast<i32>(lv))));  // 0x004dce00
            draw_line(sx(static_cast<u16>(fr.dw(g.x))), sx(static_cast<u16>(fr.dw(g.y))), text, col);
        }
    }
}

// Tooltip of the player grid (0x006c4b9b..0x006c4feb).
void tooltip_player(ShopFrame& fr) {
    {
        const i32 y2 = oy() + 0x240, x2 = ox() + 0x1f0, y1 = oy() + 0x38, x1 = ox() + 0x100;
        if (in_rect(x1, y1, x2, y2) == 0) return;
    }
    {
        const i32 base = ox() + 0x100;
        fr.in(kCx) = (mouse_x() - base) / 0x28;
    }
    {
        const i32 base = oy() + 0x38;
        fr.in(kCy) = (mouse_y() - base) / 0x28;
    }
    fr.dw(kK) = joan::orig::arg::as<u32>(joan::orig::as_grid(jo::as_buy_screen(fr.self)->items_b.grid_16)->find_at(joan::orig::arg::i32(fr.in(kCx)), joan::orig::arg::i32(fr.in(kCy))));  // 0x00458170
    if (fr.dw(kK) == 0) return;
    fr.sel = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::as_buy_screen(fr.self)->items_b.grid_16)->get(joan::orig::arg::i32(fr.dw(kK))));  // 0x00457e90
    for (fr.in(kJ) = 1;; fr.in(kJ) = fr.in(kJ) + 1) {
        if (fr.in(kJ) > joan::orig::arg::as<i32>(joan::orig::as_shop_facility(fr.shop_rec)->stock_count())) return;  // 0x00469dd0
        fr.ent = joan::orig::arg::as<u8*>(joan::orig::as_shop_facility(fr.shop_rec)->stock_at(joan::orig::arg::i32(fr.in(kJ))));  // 0x00469e10
        if (fr.dw(kJ) == fr.dw(kK)) break;
    }
    void* const path = fr.p(kPath1);
    joan::orig::as_string(path)->construct();  // 0x005a1170
    fr.def1 = item_rec(fr.sel);
    if (jo::as_item_def(fr.def1)->description != nullptr) {
        fr.dw(kN1) = 0;
        {
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_mod_table(fr.def1)->value_own(joan::orig::arg::i32(jo::view<u32>(jo::object<jo::ItemRow>(fr.ent)->b))));  // 0x004a41c0
            fr.arg_put(fr.arr1, fr.in(kN1), v);
            fr.in(kN1) = fr.in(kN1) + 1;
        }
        for (fr.in(kL1) = 0;; fr.in(kL1) = fr.in(kL1) + 1) {
            if (!(fr.in(kL1) < jo::as_item_def(fr.def1)->effect_count)) break;
            const u8* const def = fr.def1;
            const u32 lvl = jo::view<u32>(jo::object<jo::ItemRow>(fr.ent)->b) - jo::view<u32>(jo::as_item_def(def)->min_level);
            const u32 v = stat_row(jo::as_item_def(def)->effects, fr.in(kL1), lvl);
            fr.arg_put(fr.arr1, fr.in(kN1), v);
            fr.in(kN1) = fr.in(kN1) + 1;
        }
        for (fr.in(kL1) = 0;; fr.in(kL1) = fr.in(kL1) + 1) {
            if (!(fr.in(kL1) < jo::as_item_def(fr.def1)->requirement_count)) break;
            const u8* const def = fr.def1;
            const u32 lvl = jo::view<u32>(jo::object<jo::ItemRow>(fr.ent)->b) - jo::view<u32>(jo::as_item_def(def)->min_level);
            const u32 v = stat_row(jo::as_item_def(def)->requirements, fr.in(kL1), lvl);
            fr.arg_put(fr.arr1, fr.in(kN1), v);
            fr.in(kN1) = fr.in(kN1) + 1;
        }
        const char* const text =
            joan::orig::arg::as<const char*>(joan::orig::text::format_5a4c30(jo::as_item_def(fr.def1)->description, static_cast<void*>(fr.arr1)));  // 0x005a4c30
        joan::orig::as_string(path)->assign(joan::orig::arg::str(text));                                          // 0x005a1440
        joan::orig::as_string(path)->append(joan::orig::arg::str(str(kNl)));                                          // 0x005a1550
    }
    {
        // fild [def+0x2e]; fmul [ent+2]; fimul (ent[0xa] - def[0x36] + 1); _ftol
        const Fpu fp{true};
        const u8* const def = fr.def1;
        const u8* const ent = fr.ent;
        double a = static_cast<double>(jo::as_item_def(def)->price);
        a = fp.mul(a, static_cast<double>(jo::object<jo::ItemRow>(ent)->value));
        const i32 lv = static_cast<i32>(jo::view<u32>(jo::object<jo::ItemRow>(ent)->b) - jo::view<u32>(jo::as_item_def(def)->min_level) + 1u);
        a = fp.mul(a, static_cast<double>(lv));
        fr.in(kPrice1) = ftol(a);
    }
    char* const buf = reinterpret_cast<char*>(fr.p(kBuf1));
    crt_sprintf(buf, joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(kTexts))->buy_3(joan::orig::arg::u32(fr.in(kPrice1)))));  // 0x004da6c0
    crt_strcat(buf, str(kNl));
    joan::orig::as_string(path)->append(joan::orig::arg::str(static_cast<const char*>(buf)));  // 0x005a1550
    {
        const i32 j = fr.in(kJ);
        if (jo::as_buy_screen(fr.self)->prices[j - 1].value < 0) {
            joan::orig::as_string(path)->append(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(kTexts))->buy_4())));  // 0x004da700, 0x005a1550
        } else {
            const i32 sell = jo::as_buy_screen(fr.self)->prices[j - 1].value;
            crt_sprintf(buf, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(kTexts))->buy_text_5(joan::orig::arg::u32(sell))));  // 0x004da730
            joan::orig::as_string(path)->append(joan::orig::arg::str(static_cast<const char*>(buf)));  // 0x005a1550
        }
    }
    const char* const tip = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
    joan::orig::as_help_box(obj(d::g_tooltip))->draw_box(joan::orig::arg::u32(sx(global<u16>(d::g_mouse_x))), joan::orig::arg::u32(sx(global<u16>(d::g_mouse_y))), jo::as_item_def(fr.def1)->file_name, tip, joan::orig::arg::u32(i32{6}));  // 0x006b8db0
}

// Tooltip of the shop grid (0x006c4feb..0x006c54ce).
void tooltip_shop(ShopFrame& fr) {
    {
        const i32 y2 = oy() + 0x178, x2 = ox() + 0x300, y1 = oy() + 0x38, x1 = ox() + 0x210;
        if (in_rect(x1, y1, x2, y2) == 0) return;
    }
    {
        const i32 base = ox() + 0x210;
        fr.in(kCx2) = (mouse_x() - base) / 0x28;
    }
    {
        const i32 base = oy() + 0x38;
        fr.in(kCy2) = (mouse_y() - base) / 0x28;
    }
    fr.dw(kK2) = joan::orig::arg::as<u32>(joan::orig::as_grid(jo::as_buy_screen(fr.self)->items.grid_16)->find_at(joan::orig::arg::i32(fr.in(kCx2)), joan::orig::arg::i32(fr.in(kCy2))));  // 0x00458170
    if (fr.dw(kK2) == 0) return;
    fr.sel2 = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(jo::as_buy_screen(fr.self)->items.grid_16)->get(joan::orig::arg::i32(fr.dw(kK2))));  // 0x00457e90
    void* const tmp = fr.p(kTmp);
    joan::orig::as_world_item(tmp)->construct();  // 0x00456eb0
    {
        const u8* const e = fr.sel2;
        joan::orig::as_item_stack(tmp)->init(joan::orig::arg::i16(sx(jo::view<u16>(jo::as_inv_item(e)->type))), joan::orig::arg::i16(sx(jo::view<u16>(jo::as_inv_item(e)->level))), joan::orig::arg::i16(sx(jo::view<u16>(jo::as_inv_item(e)->count))), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00456cd0
    }
    void* const path = fr.p(kPath2);
    joan::orig::as_string(path)->construct();  // 0x005a1170
    fr.def2 = item_rec(fr.sel2);
    if (jo::as_item_def(fr.def2)->description != nullptr) {
        fr.dw(kN2) = 0;
        {
            const u32 v = joan::orig::arg::as<u32>(joan::orig::as_mod_table(fr.def2)->value_own(joan::orig::arg::i32(sx(jo::view<u16>(jo::as_inv_item(fr.sel2)->level)))));  // 0x004a41c0
            fr.arg_put(fr.arr2, fr.in(kN2), v);
            fr.in(kN2) = fr.in(kN2) + 1;
        }
        for (fr.in(kL2) = 0;; fr.in(kL2) = fr.in(kL2) + 1) {
            if (!(fr.in(kL2) < jo::as_item_def(fr.def2)->effect_count)) break;
            const u8* const def = fr.def2;
            const u32 lvl = static_cast<u32>(sx(jo::view<u16>(jo::as_inv_item(fr.sel2)->level))) - jo::view<u32>(jo::as_item_def(def)->min_level);
            const u32 v = stat_row(jo::as_item_def(def)->effects, fr.in(kL2), lvl);
            fr.arg_put(fr.arr2, fr.in(kN2), v);
            fr.in(kN2) = fr.in(kN2) + 1;
        }
        for (fr.in(kL2) = 0;; fr.in(kL2) = fr.in(kL2) + 1) {
            if (!(fr.in(kL2) < jo::as_item_def(fr.def2)->requirement_count)) break;
            const u8* const def = fr.def2;
            const u32 lvl = static_cast<u32>(sx(jo::view<u16>(jo::as_inv_item(fr.sel2)->level))) - jo::view<u32>(jo::as_item_def(def)->min_level);
            const u32 v = stat_row(jo::as_item_def(def)->requirements, fr.in(kL2), lvl);
            fr.arg_put(fr.arr2, fr.in(kN2), v);
            fr.in(kN2) = fr.in(kN2) + 1;
        }
        const char* const text =
            joan::orig::arg::as<const char*>(joan::orig::text::format_5a4c30(jo::as_item_def(fr.def2)->description, static_cast<void*>(fr.arr2)));  // 0x005a4c30
        joan::orig::as_string(path)->assign(joan::orig::arg::str(text));                                          // 0x005a1440
        joan::orig::as_string(path)->append(joan::orig::arg::str(str(kNl)));                                          // 0x005a1550
    }
    {
        // fild [def+0x2e]; fmul [shop+0x7d]; fild (short)e[2]; fidiv (short)def[0x2a]; fmulp;
        // fimul ((short)e[4] - def[0x36] + 1); _ftol
        const Fpu fp{true};
        const u8* const def = fr.def2;
        const u8* const e = fr.sel2;
        double a = static_cast<double>(jo::as_item_def(def)->price);
        a = fp.mul(a, static_cast<double>(jo::view<float>(jo::as_shop(fr.shop_rec)->stock_kind)));
        double b = static_cast<double>(sx(jo::view<u16>(jo::as_inv_item(e)->count)));
        b = fp.div(b, static_cast<double>(sx(jo::as_item_def(def)->stats[3])));
        a = fp.mul(a, b);
        const i32 lv = static_cast<i32>(static_cast<u32>(sx(jo::view<u16>(jo::as_inv_item(e)->level))) - jo::view<u32>(jo::as_item_def(def)->min_level) + 1u);
        a = fp.mul(a, static_cast<double>(lv));
        fr.in(kPrice2) = ftol(a);
    }
    char* const buf = reinterpret_cast<char*>(fr.p(kBuf2));
    const u8* const def = fr.def2;
    bool priced = false;  // 0x004da770(price) instead of 0x004da840()
    if (jo::view<i8>(jo::as_item_def(def)->can_sell) != 0) {
        const i32 kind = sx(jo::as_item_def(def)->type_id);
        const bool special = kind == 0x15 || kind == 0x17 || kind == 0x19;
        if (special && jo::as_shop(fr.shop_rec)->fixed_stock == 0) {
            priced = true;
        } else {
            const i32 kind2 = sx(jo::as_item_def(fr.def2)->type_id);
            if (kind2 != 0x15 && kind2 != 0x17 && kind2 != 0x19 &&
                sxb(jo::view<u8>(jo::as_shop(fr.shop_rec)->fixed_stock)) == 1)
                priced = true;
        }
    }
    if (priced)
        crt_sprintf(buf, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(kTexts))->buy_text_6(joan::orig::arg::u32(fr.in(kPrice2)))));  // 0x004da770
    else
        crt_sprintf(buf, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(kTexts))->buy_10()));  // 0x004da840
    joan::orig::as_string(path)->append(joan::orig::arg::str(static_cast<const char*>(buf)));  // 0x005a1550
    const char* const tip = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
    joan::orig::as_help_box(obj(d::g_tooltip))->draw_box(joan::orig::arg::u32(sx(global<u16>(d::g_mouse_x))), joan::orig::arg::u32(sx(global<u16>(d::g_mouse_y))), jo::as_item_def(fr.def2)->file_name, tip, joan::orig::arg::u32(i32{6}));  // 0x006b8db0
    joan::orig::as_world_item(tmp)->destruct();                                // 0x00456ef0
}

struct H {
    // ------------------------------------------------------------------------------ 0x006c27c0
    void release() {
        c_release.hit();
        joan::orig::as_shop(this)->free_sprites();  // 0x006c90e0
        if (jo::self<jo::BuyScreen>(this)->panel_a != nullptr) {
            void* const p = jo::self<jo::BuyScreen>(this)->panel_a;
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(joan::orig::arg::u32(i32{1}));  // 0x006b33a0
            jo::self<jo::BuyScreen>(this)->panel_a = nullptr;
        }
        if (jo::self<jo::BuyScreen>(this)->panel_b != nullptr) {
            void* const p = jo::self<jo::BuyScreen>(this)->panel_b;
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(joan::orig::arg::u32(i32{1}));  // 0x006b33a0
            jo::self<jo::BuyScreen>(this)->panel_b = nullptr;
        }
        if (jo::self<jo::BuyScreen>(this)->prices != nullptr) {
            const i32 line = sx(global<u16>(d::g_a237ac)) + 0x13;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::BuyScreen>(this)->prices), str(d::s_d_ww_interfac_i_buy_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::BuyScreen>(this)->prices = nullptr;
        }
        jo::self<jo::BuyScreen>(this)->items_loaded = 0;
        model_slot(jo::self<jo::BuyScreen>(this)->stage_player);
        camera_slot(jo::self<jo::BuyScreen>(this)->stage_player);
        joan::orig::as_scene(&jo::self<jo::BuyScreen>(this)->stage_player)->clear();  // 0x005e9b10
        model_slot(jo::self<jo::BuyScreen>(this)->stage_shop);
        camera_slot(jo::self<jo::BuyScreen>(this)->stage_shop);
        joan::orig::as_scene(&jo::self<jo::BuyScreen>(this)->stage_shop)->clear();  // 0x005e9b10
    }
    // st.model / u32_dd (handle) / model_res (sound) / u32_e1 (sound2); the model is always removed from the
    // player stage's list (+0x5f), as the original does.
    void model_slot(jo::ShopStage& st) {
        if (st.model == nullptr) return;
        joan::orig::as_list(&jo::self<jo::BuyScreen>(this)->stage_player)->remove_5ea2f0(joan::orig::arg::mptr(st.model));  // 0x005ea2f0
        void* const p = st.model;
        if (p != nullptr) joan::orig::as_scene_object(p)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00420840
        st.model = nullptr;
        st.u32_dd = 0;
        if (st.model_res > 0) joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(sx(static_cast<u16>(st.model_res))));  // 0x004b16a0
        st.model_res = 0;
        st.u32_e1 = 0;
    }
    void camera_slot(jo::ShopStage& st) {
        if (st.camera == nullptr) return;
        shop_cameras::g_count = 0;
        joan::orig::as_model(st.camera)->release_parts();  // 0x00613a10
        void* const p = st.camera;
        if (p != nullptr) joan::orig::as_camera(p)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00562910
        st.camera = nullptr;
    }

    // ------------------------------------------------------------------------------ 0x006c4070
    void draw() {
        joan::canvas::ScreenScope canvas_scope(0x006c4070);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_draw.hit();
        ShopFrame fr;
        std::memset(fr.bytes, 0, sizeof fr.bytes);  // fix/uninit: see the header
        fr.self = reinterpret_cast<u8*>(this);
        if (jo::as_buy_screen(fr.self)->open == 0) return;
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return;  // 0x00428b70
        fr.unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::view<u32>(jo::as_buy_screen(fr.self)->place)))) != 0) return;  // 0x0044cf80
        {
            void* const zone = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::view<u32>(jo::as_buy_screen(fr.self)->place))));  // 0x0043c450
            fr.shop_rec = vcall<u8*>(zone, 0x20);
        }
        joan::orig::as_shop(fr.self)->init();  // 0x006c3230
        {
            const i32 base = ox();
            fr.in(kMx) = mouse_x() - base;
        }
        {
            const i32 base = oy();
            fr.in(kMy) = mouse_y() - base;
        }
        {
            const u32 col = rgba(0xffffffffu);
            joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(ox()), joan::orig::arg::i16(oy()), 6u, str(d::s_main), joan::orig::arg::mptr(jo::as_buy_screen(fr.self)->sheet), joan::orig::arg::u32(col), 3, 0, 0, 1, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500 "main"
        }
        void* const coins = fr.p(kCoins);
        joan::orig::as_string(coins)->construct();  // 0x005a1170
        {
            const i32 gold = jo::as_buy_screen(fr.self)->gold;
            img::Addr name = d::s_coins1;  // coins1
            if (gold >= 0x1770)
                name = d::s_coins4;  // coins4
            else if (jo::as_buy_screen(fr.self)->gold >= 0xfa0)
                name = d::s_coins3;  // coins3
            else if (jo::as_buy_screen(fr.self)->gold >= 0x7d0)
                name = d::s_coins2;  // coins2
            joan::orig::as_string(coins)->assign(joan::orig::arg::str(str(name)));  // 0x005a1440
        }
        {
            const u32 col = rgba(0xffffffffu);
            void* const set = jo::as_buy_screen(fr.self)->sheet;
            const char* const cname = joan::orig::arg::as<const char*>(joan::orig::as_string(coins)->c_str());  // 0x00417d90
            const i32 y = oy() + 0x1d0;
            const i32 x = ox() + 0x10;
            joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 6u, joan::orig::arg::str(cname), joan::orig::arg::mptr(set), joan::orig::arg::u32(col), 3, 0, 0, 1, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
        }
        joan::orig::as_colour(fr.p(kCol))->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff});  // 0x0042a260
        {
            const u32 col = fr.dw(kCol);
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(kMisc))->format_int_58fca0(joan::orig::arg::i32(jo::as_buy_screen(fr.self)->gold), i32{4}));  // 0x0058fca0
            const i32 y = oy() + 0x1d4;
            const i32 x = ox() + 0xc9;
            joan::orig::as_text(obj(d::g_font_24pt_large_numbers_gold))->draw_2(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(i32{6}), text, joan::orig::arg::u32(col), i32{0}, i32{0}, i32{0}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626080
        }
        {
            const u32 col = fr.dw(kCol);
            const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(kTexts))->buy_1());  // 0x004da650
            const i32 y2 = oy() + 0x38, x2 = ox() + 0x1c4, y1 = oy() + 0x10, x1 = ox() + 0x12c;
            joan::orig::as_text_renderer(obj(d::g_dialogs))->draw_wrapped(joan::orig::arg::u32(x1), joan::orig::arg::u32(y1), joan::orig::arg::u32(x2), joan::orig::arg::u32(y2), joan::orig::arg::u32(i32{6}), joan::orig::arg::str(text), joan::orig::arg::u32(col), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), joan::orig::arg::u32(i32{2}));  // 0x00626e00
        }
        {
            const u32 col = fr.dw(kCol);
            const char* const text =
                joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(kTexts))->buy_text_2(jo::as_unit(fr.unit)->name_));  // 0x004da680
            const i32 y2 = oy() + 0x38, x2 = ox() + 0x2d4, y1 = oy() + 0x10, x1 = ox() + 0x23c;
            joan::orig::as_text_renderer(obj(d::g_dialogs))->draw_wrapped(joan::orig::arg::u32(x1), joan::orig::arg::u32(y1), joan::orig::arg::u32(x2), joan::orig::arg::u32(y2), joan::orig::arg::u32(i32{6}), joan::orig::arg::str(text), joan::orig::arg::u32(col), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), joan::orig::arg::u32(i32{2}));  // 0x00626e00
        }
        joan::orig::as_panel(jo::as_buy_screen(fr.self)->panel_a)->reset_6b7cf0();                    // 0x006b7cf0
        joan::orig::as_button(jo::as_buy_screen(fr.self)->panel_a)->draw(6u, 7);  // 0x00622cc0
        draw_grid(fr, 0x100, false, true, kG1);
        draw_grid(fr, 0x210, true, false, kG2);
        if (jo::view<u32>(jo::as_buy_screen(fr.self)->drag_state) != 0) return;
        if (jo::view<i8>(jo::as_buy_screen(fr.self)->dragging) != 0) return;
        tooltip_player(fr);
        tooltip_shop(fr);
    }
};

JOAN_REPLACE("game/b116_05", "Shop::release_6c27c0", 0x006c27c0, &H::release);
// fix/uninit: 0x006c4070 lets 0x005a4c30 read format arguments past the values written -> 0.
JOAN_REPLACE("fix/uninit", "Shop::draw", 0x006c4070, &H::draw);
JOAN_CANVAS_SELF(0x006c4070);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_05
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
//
//    - Obj::clear_6c2b70 0x006c2b70 (interfac/i_buy.cpp): memset(this, 0, 0x259) -> this.
//
//    - Shop::refresh_6c92e0 0x006c92e0 (int which), ret 4 (caller 0x6c2e3f/49 ignores eax:
//      void): rebuilds the model object at +0x138 (which == 0) or +0x221 (which != 0).
//
//    - Obj::flag_6ce410 0x006ce410 -> byte +0x234 != 0 (int).
//
//    - Obj::active_6d3410 0x006d3410 -> al (upper eax = this): +8 && [0xa5b9f8] != 4.
//
//    - Obj::byte8_6d34b0 0x006d34b0 -> al = byte +8 (upper eax = this).
//
//    - Obj::self_6d65b0 0x006d65b0 / Obj::self_6e0e20 0x006e0e20 -> this.
//
//  - SEH frames (0x68bbb0, 0x6b2340, 0x6c92e0) only guard new + ctor: normal path reproduced.
namespace {
namespace game_b103_15 {

using joan::orig::helpers::addr_of;

using joan::orig::helpers::op_new;  // operator new
void* crt_memset(void* d, i32 c, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }

void* const kSounds = obj(d::g_model_res_mgr);

JOAN_COUNTER(c_shop_refresh, "Shop::refresh_6c92e0");

// ---- 0x6c92e0 helper: rebuild one model slot (the player stage +0x5f or the shop stage +0x148) ----
void rebuild_slot(void* self, void* rec, jo::ShopStage& st) {
    const u32 hi = addr_of(self) & 0xffff0000u;
    if (st.model != nullptr) {
        joan::orig::as_list(&st)->remove_5ea2f0(joan::orig::arg::mptr(st.model));  // 0x005ea2f0
        void* const q = st.model;
        if (q != nullptr) joan::orig::as_scene_object(q)->destroy(1u);  // 0x00420840
        st.model = nullptr;
        if (st.model_res > 0)
            joan::orig::as_model_res_mgr(kSounds)->release(joan::orig::arg::i16(hi | static_cast<u16>(st.model_res)));  // 0x004b16a0
        st.model_res = 0;
        st.u32_e1 = 0;
    }
    void* t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(rec)->type_id)))));  // 0x0042d0a0
    st.model_res = jo::as_unit_type(t)->model_ids[jo::as_unit(rec)->collision_id];
    joan::orig::as_model_res(kSounds)->acquire(joan::orig::arg::i16(hi | static_cast<u16>(st.model_res)));  // 0x004b0f40
    t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(rec)->type_id)))));  // 0x0042d0a0
    st.u32_e1 = jo::view<u32>(jo::as_unit_type(t)->model_flags[jo::as_unit(rec)->collision_id]);
    {
        void* const m = op_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
        jo::view<void*>(st.model) = m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(m)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
    }
    void* const g = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(kSounds)->entry(joan::orig::arg::u32(st.model_res)));  // 0x0041a180
    jo::ActorDef* const def = jo::as_model_entry(g)->actor_def;
    st.u32_dd = def;  // ShopStage::u32_dd: the ActorDef
    jo::as_scene_object(st.model)->def = def;
    joan::orig::as_scene_object(st.model)->init();  // 0x005aab70
    jo::as_scene_object(st.model)->kind = -1;
    jo::as_scene_object(st.model)->owner = nullptr;
    void* const p = jo::view<void*>(jo::as_scene_object(st.model)->plane);
    for (i32 i = 0; i < jo::as_plane(p)->material_count; ++i) {
        u32 c0 = 0;
        void* pc = joan::orig::as_colour(&c0)->construct(0, 0, 0, 0);  // 0x0042a260
        joan::orig::as_colour(&jo::as_plane(p)->materials[i].specular)->from_rgba(joan::orig::arg::mptr(pc));  // 0x005e95b0
        jo::as_plane(p)->materials[i].power = 0;
        u32 c1 = 0;
        pc = joan::orig::as_colour(&c1)->construct(0xff, 0xff, 0xff, 0xff);  // 0x0042a260
        joan::orig::as_colour(&jo::as_plane(p)->materials[i].ambient)->from_rgba(joan::orig::arg::mptr(pc));  // 0x005e95b0
    }
    joan::orig::as_scene(&st)->add_actor(joan::orig::arg::mptr(st.model));  // 0x005ea1f0
    joan::orig::as_model_instance(st.model)->set_10a(1u);                  // 0x0042cf20
}

struct H {
    u32 self_hi() { return addr_of(this) & 0xffffff00u; }

    // ------------------------------------------------------------------------ 0x006c2b70
    void* clear_6c2b70() { return crt_memset(this, 0, static_cast<u32>(sizeof(jo::BuyScreen))); }  // 0x259 on x86

    // ------------------------------------------------------------------------ 0x006c92e0
    void refresh_6c92e0(i32 which) {
        c_shop_refresh.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return;  // 0x00428b70
        void* const r14 = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->unit))) != 0) return;
        void* const r18 = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->unit)));
        if (which == 0) {
            rebuild_slot(this, r18, jo::self<jo::BuyScreen>(this)->stage_player);
        } else {
            rebuild_slot(this, r14, jo::self<jo::BuyScreen>(this)->stage_shop);
        }
    }

    // ------------------------------------------------------------------------ 0x006ce410
    i32 flag_6ce410() { return static_cast<i32>(jo::view<i8>(jo::self<jo::Ending>(this)->is_open_)) != 0 ? 1 : 0; }

    // ------------------------------------------------------------------------ 0x006d3410
    u32 active_6d3410() {
        if (static_cast<i32>(jo::self<jo::TalkPrompt>(this)->visible) == 0) return self_hi();
        if (global<u32>(d::g_control_mode) == 4) return self_hi();
        return self_hi() | 1u;
    }

    // ------------------------------------------------------------------------ 0x006d34b0
    u32 byte8_6d34b0() { return self_hi() | jo::view<u8>(jo::self<jo::TalkPrompt>(this)->visible); }

    // ------------------------------------------------------------------------ 0x006d65b0 / 0x006e0e20
    void* self_ret() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::clear_6c2b70", 0x006c2b70, &H::clear_6c2b70);
B("Shop::refresh_6c92e0", 0x006c92e0, &H::refresh_6c92e0);
B("Obj::flag_6ce410", 0x006ce410, &H::flag_6ce410);
B("Obj::active_6d3410", 0x006d3410, &H::active_6d3410);
B("Obj::byte8_6d34b0", 0x006d34b0, &H::byte8_6d34b0);
B("Obj::self_6d65b0", 0x006d65b0, &H::self_ret);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
//
//      0x006c2ba0 draw_item_icon(x, y, name)                    thiscall, ret 0xc
//
//      0x006c99e0 Backpack::place_at_cursor(w, h, *x, *y)       thiscall, ret 0x10
//
//      0x006c9c40 Backpack::item_under(w, h, -, x, y)           thiscall, ret 0x14
//
//      0x006c9d20 Backpack::find_free(w, h, *x, *y)             thiscall, ret 0x10
//
//      0x006c9f10 Backpack::has_enough(id, n, a, b)             thiscall, ret 0x10, al
//
//      0x006de060 Unit::item_under(id, w, h, -, x, y, char grid) thiscall, ret 0x1c
//
//  - 0x006c2ba0: the path buffer is 0x84 bytes in the original; a larger buffer is used here.
//
//  - 0x006c9f10: the first argument is compared as a full dword against the sign-extended word.
namespace {
namespace game_b92_06 {

void* const kImages = obj(d::g_texture_array);
void* const kItems = obj(d::g_item_res);

std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t* item_rec(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0
std::int32_t grid_at(void* grid, std::int32_t x, std::int32_t y) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_grid(grid)->find_at(joan::orig::arg::i32(x), joan::orig::arg::i32(y)));  // 0x00458170
}
// 0x0042a260 Colour(r, g, b, a) (ret 0x10), returns the 4-byte object.
std::uint32_t colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    alignas(4) std::uint8_t c[4];
    std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<std::uint32_t>(*jo::as_colour(p));
}

std::uint32_t u(std::int32_t v) { return static_cast<std::uint32_t>(v); }
using joan::orig::helpers::s;
std::int32_t half(std::int32_t v) { return v / 2; }  // cdq / sub / sar 1

// Backpack geometry (0x006c99e0 / 0x006c9d20).
std::int32_t pack_x0() { return s(u(half(s(u(global<std::int32_t>(d::g_screen_w)) - 0x320u))) + 0x210u); }
std::int32_t pack_x1() { return s(u(half(s(u(global<std::int32_t>(d::g_screen_w)) - 0x320u))) + 0x300u); }
std::int32_t pack_y0() { return s(u(half(s(u(global<std::int32_t>(d::g_screen_h)) - 0x258u))) + 0x38u); }
std::int32_t pack_y1() { return s(u(half(s(u(global<std::int32_t>(d::g_screen_h)) - 0x258u))) + 0x178u); }
std::int32_t pack_cols() { return s(u(pack_x1()) - u(pack_x0())) / 0x28; }
std::int32_t pack_rows() { return s(u(pack_y1()) - u(pack_y0())) / 0x28; }

JOAN_COUNTER(c_icon, "draw_item_icon");
JOAN_COUNTER(c_place, "Backpack::place_at_cursor");
JOAN_COUNTER(c_item_under, "Backpack::item_under");
JOAN_COUNTER(c_find_free, "Backpack::find_free");
JOAN_COUNTER(c_has_enough, "Backpack::has_enough");
JOAN_COUNTER(c_unit_item_under, "Unit::item_under");

struct H {
    // 0x006c2ba0
    std::int32_t draw_item_icon(std::int32_t x, std::int32_t y, const char* name) {
        c_icon.hit();
        char path[0x100];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(path))), str(d::s_ITEM_9cf648));  // strcpy "ITEM\\"
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(path))), joan::orig::arg::str(name));  // strcat
        const std::int32_t saved = joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(kImages)->mode());    // 0x0060a720
        joan::orig::as_texture_manager(kImages)->set_mode(1u);                             // 0x0060a740
        void* const img = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kImages)->load(joan::orig::arg::str(static_cast<const char*>(path)), 1, 0u));  // 0x006099c0
        joan::orig::as_texture_manager(kImages)->set_mode(joan::orig::arg::u32(saved));
        const std::uint32_t c = colour(0xff, 0xff, 0xff, 0xff);
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->blit_image(joan::orig::arg::i16(s(u(x) - 8u)), joan::orig::arg::i16(s(u(y) - 8u)), 6u, joan::orig::arg::mptr(img), joan::orig::arg::u32(c), joan::orig::arg::i16(0), joan::orig::arg::i16(0), joan::orig::arg::i16(0), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(0), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1));  // 0x006332b0, ret 0x3c
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(img)));  // 0x0060a550
    }

    // 0x006c99e0
    void place_at_cursor(std::int32_t w, std::int32_t h, std::int32_t* ox, std::int32_t* oy) {
        c_place.hit();
        const std::int32_t mx = global<std::int32_t>(d::g_mouse_x);
        const std::int32_t my = global<std::int32_t>(d::g_mouse_y);
        if (pack_x0() > mx || mx > pack_x1() || pack_y0() > my || my > pack_y1()) {
            *ox = -1;
            *oy = -1;
            return;
        }
        const std::int32_t cols = pack_cols();
        const std::int32_t rows = pack_rows();
        *ox = s(u(s(u(mx) - u(half(s((u(w) - 1u) * 0x28u))))) - u(pack_x0())) / 0x28;
        *oy = s(u(s(u(my) - u(half(s((u(h) - 1u) * 0x28u))))) - u(pack_y0())) / 0x28;
        if (*ox < 0)
            *ox = 0;
        else if (!(s(u(*ox) + u(w)) < cols))
            *ox = s(u(cols) - u(w));
        if (*oy < 0)
            *oy = 0;
        else if (!(s(u(*oy) + u(h)) < rows))
            *oy = s(u(rows) - u(h));
    }

    // 0x006c9c40
    std::int32_t item_under(std::int32_t w, std::int32_t h, std::int32_t, std::int32_t x0, std::int32_t y0) {
        c_item_under.hit();
        unit_get(global<std::int32_t>(d::g_player_unit_recno));  // result unused
        std::int32_t found = 0;
        for (std::int32_t i = x0; i < s(u(x0) + u(w)); ++i) {
            for (std::int32_t j = y0; j < s(u(y0) + u(h)); ++j) {
                const std::int32_t v = grid_at(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16), i, j);
                if (found == 0 && v != 0)
                    found = v;
                else if (found != 0 && v != 0 && v != found)
                    return 0;
            }
        }
        return found;
    }

    // 0x006c9d20
    std::int32_t find_free(std::int32_t w, std::int32_t h, std::int32_t* ox, std::int32_t* oy) {
        c_find_free.hit();
        const std::int32_t cols = pack_cols();
        const std::int32_t rows = pack_rows();
        std::int32_t ok = 0;
        std::int32_t i = 0;
        std::int32_t j = 0;
        for (i = 0; i < cols; ++i) {
            for (j = 0; j < rows; ++j) {
                ok = 1;
                if (cols < s(u(i) + u(w)) || rows < s(u(j) + u(h))) {
                    ok = 0;
                } else {
                    for (std::int32_t a = i; a < s(u(i) + u(w)); ++a) {
                        for (std::int32_t b = j; b < s(u(j) + u(h)); ++b) {
                            if (grid_at(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16), a, b) != 0) {
                                ok = 0;
                                break;  // only the inner loop, as in the original
                            }
                        }
                    }
                }
                if (ok == 1) break;
            }
            if (ok == 1) break;
        }
        if (ok == 1) {
            *ox = i;
            *oy = j;
            return 1;
        }
        return 0;
    }

    // 0x006c9f10
    std::uint8_t has_enough(std::int32_t id, std::int16_t need, std::int16_t a, std::int16_t b) {
        c_has_enough.hit();
        std::uint8_t* const rec = item_rec(static_cast<std::int16_t>(id));
        std::int32_t sum = 0;
        for (std::int32_t i = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16))->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16))->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;             // 0x00457f60
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16))->get(joan::orig::arg::i32(i)));       // 0x00457e90
            if (static_cast<std::int32_t>(jo::as_inv_item(e)->type) != id) continue;
            if (jo::view<std::int32_t>(jo::as_inv_item(e)->u32_6) != static_cast<std::int32_t>(a)) continue;
            if (jo::view<std::int32_t>(jo::as_inv_item(e)->u32_a) != static_cast<std::int32_t>(b)) continue;
            sum = s(u(sum) + u(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_item_def(rec)->stats[4])) -
                               static_cast<std::int32_t>(jo::as_inv_item(e)->count)));
        }
        return sum >= static_cast<std::int32_t>(need) ? 1 : 0;
    }

    // 0x006de060
    std::int32_t unit_item_under(std::int32_t id, std::int32_t w, std::int32_t h, std::int32_t, std::int32_t x0,
                                 std::int32_t y0, std::int8_t mode) {
        c_unit_item_under.hit();
        std::uint8_t* const unit = unit_get(id);
        std::int32_t found = 0;
        for (std::int32_t i = x0; i < s(u(x0) + u(w)); ++i) {
            for (std::int32_t j = y0; j < s(u(y0) + u(h)); ++j) {
                std::int32_t v;
                if (mode == 0)
                    v = grid_at(jo::as_unit(unit)->modifiers->grid_16, i, j);
                else if (mode == 1)
                    v = grid_at(jo::as_unit(unit)->modifiers->grid_1a, i, j);
                else
                    continue;
                if (found == 0 && v != 0)
                    found = v;
                else if (found != 0 && v != 0 && v != found)
                    return 0;
            }
        }
        return found;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("draw_item_icon", 0x006c2ba0, &H::draw_item_icon);
B("Backpack::place_at_cursor", 0x006c99e0, &H::place_at_cursor);
B("Backpack::item_under", 0x006c9c40, &H::item_under);
B("Backpack::find_free", 0x006c9d20, &H::find_free);
B("Backpack::has_enough", 0x006c9f10, &H::has_enough);
B("Unit::item_under", 0x006de060, &H::unit_item_under);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
//
//    - BuyScreen::free_slot_for 0x006c2ce0 (k), ret 4: unit (k == 0: +0x16, else [0xa5b9e8])
//      -> type record -> slot list (0x4b2730 on 0xa58cb8); last 1-based index whose entry is
//      {0, 1}, else 0.
//
//    - BuyScreen::set_open 0x006c8d00 (char open), ret 4: closing frees buffers / models /
//      sounds (0x58ed70 "i_buy.cpp", 0x495ef0, 0x5ea2f0 + 0x420840, 0x4b16a0), resumes the
//      game speed (0x42f930 on 0xa50088); opening pauses it; 0x595a20 on 0xa516b0 at the end.
//
//    - Obj::init_6ca040 0x006ca040 (ret 0): memset 0x48, +0x41 / +0x43 / +0x42 = 1.
//
//    - Obj::byte_234 0x006d1ef0 / Obj::byte_ac 0x006d37c0 (ret 0): byte getters.
//
//  - 0x006c8d00: fild of a short (exact) pushed as a float argument; _ftol of [0xa500cc] stored
//    as a word. Word arguments pushed with garbage upper bits (0x42d0a0 in 0x6c2ce0, 0x4b16a0)
//    are only read at word width by the callees (checked); passed sign-extended here.
//
//  - Return values: callers of 0x6b5320, 0x6c8d00, 0x6ea020, 0x6fe240, 0x7076c0, 0x70db10,
//    0x717f40, 0x724670, 0x7349b0, 0x741a70, 0x75fb00, 0x6e8010, 0x68b900 ignore eax (checked);
//    the replacements still return the original eax where it is a simple value, void otherwise.
namespace {
namespace game_b99_10 {

void* const kHeap = obj(d::g_mem);
void* const kMouse = obj(d::g_mouse);

using joan::orig::helpers::crt_memset;
i32 ftol(double v) { return joan::hook::x87::ftol(v); }

JOAN_COUNTER(c_free_slot, "BuyScreen::free_slot_for");
JOAN_COUNTER(c_set_open, "BuyScreen::set_open");

struct H {
    // ------------------------------------------------------------------ 0x006c2ce0
    i32 free_slot_for(i32 k) {
        c_free_slot.hit();
        void* const units = obj(d::g_units);
        u8* const unit = k == 0 ? joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->unit))))  // 0x0041b3c0
                                : joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
        const u32 key = jo::view<u32>(jo::as_unit_type(type)->model_flags[jo::as_unit(unit)->collision_id]);
        void* const slots = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(key)));  // 0x004b2730
        for (i32 i = jo::as_anim_set(slots)->group_count; i > 0; --i) {
            i32* const e = joan::orig::arg::as<i32*>(joan::orig::as_anim_set(slots)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
            if (e[0] == 0 && e[1] == 1) return i;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x006c8d00
    std::uintptr_t set_open(i32 open_arg) {
        c_set_open.hit();
        if (static_cast<i32>(static_cast<i8>(open_arg)) > 0) {
            jo::view<u8>(jo::self<jo::BuyScreen>(this)->open) = 1;
            joan::orig::as_shop(this)->load_sprites();  // 0x006c9060
            if (global<u8>(d::g_game_running) != 0) {
                global<u8>(d::g_game_running) = 0;
                global<i16>(d::g_saved_game_speed) = static_cast<i16>(ftol(global<float>(d::g_game_speed)));
                joan::orig::as_game(kSpeed)->set_speed(joan::orig::arg::bits(0.0f));  // 0x0042f930
            }
            jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_state) = 0;
        } else {
            if (jo::self<jo::BuyScreen>(this)->open == 1) {
                jo::self<jo::BuyScreen>(this)->items_loaded = 0;
                if (jo::view<void*>(jo::self<jo::BuyScreen>(this)->prices) != nullptr) {
                    const i32 line = static_cast<i32>(global<i16>(d::g_a237b4)) + 7;
                    joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BuyScreen>(this)->prices)), str(d::s_d_ww_interfac_i_buy_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
                }
                jo::self<jo::BuyScreen>(this)->prices = nullptr;
                if (void* const p = jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16)) joan::orig::as_grid(p)->destroy(1u);  // 0x00495ef0
                jo::self<jo::BuyScreen>(this)->items_b.grid_16 = nullptr;
                if (jo::self<jo::BuyScreen>(this)->stage_player.model != nullptr) {
                    joan::orig::as_list(&jo::self<jo::BuyScreen>(this)->stage_player)->remove_5ea2f0(joan::orig::arg::mptr(jo::self<jo::BuyScreen>(this)->stage_player.model));  // 0x005ea2f0
                    if (void* const p = jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_player.model)) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
                    jo::self<jo::BuyScreen>(this)->stage_player.model = nullptr;
                    jo::self<jo::BuyScreen>(this)->stage_player.u32_dd = 0;
                    if (jo::self<jo::BuyScreen>(this)->stage_player.model_res > 0)
                        joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::self<jo::BuyScreen>(this)->stage_player.model_res))));  // 0x004b16a0
                    jo::view<u32>(jo::self<jo::BuyScreen>(this)->stage_player.model_res) = 0;
                    jo::self<jo::BuyScreen>(this)->stage_player.u32_e1 = 0;
                }
                if (jo::self<jo::BuyScreen>(this)->stage_shop.model != nullptr) {
                    joan::orig::as_list(&jo::self<jo::BuyScreen>(this)->stage_shop)->remove_5ea2f0(joan::orig::arg::mptr(jo::self<jo::BuyScreen>(this)->stage_shop.model));  // 0x005ea2f0
                    if (void* const p = jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_shop.model)) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
                    jo::self<jo::BuyScreen>(this)->stage_shop.model = nullptr;
                    jo::self<jo::BuyScreen>(this)->stage_shop.u32_dd = 0;
                    if (jo::self<jo::BuyScreen>(this)->stage_shop.model_res > 0)
                        joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::self<jo::BuyScreen>(this)->stage_shop.model_res))));  // 0x004b16a0
                    jo::view<u32>(jo::self<jo::BuyScreen>(this)->stage_shop.model_res) = 0;
                    jo::self<jo::BuyScreen>(this)->stage_shop.u32_e1 = 0;
                }
            }
            jo::view<u8>(jo::self<jo::BuyScreen>(this)->open) = 0;
            joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();  // 0x0042fef0
            if (global<u8>(d::g_game_running) == 0) {
                global<u8>(d::g_game_running) = 1;
                const float speed = static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)));  // fild: exact
                joan::orig::as_game(kSpeed)->set_speed(joan::orig::arg::bits(speed));  // 0x0042f930
            }
            joan::orig::as_shop(this)->free_sprites();  // 0x006c90e0
        }
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_parser(kMouse)->reset());  // 0x00595a20
    }

    // ------------------------------------------------------------------ 0x006ca040
    H* init_6ca040() {
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::CutScene)));  // 0x48 on x86
        jo::self<jo::CutScene>(this)->hud_a = 1;
        jo::self<jo::CutScene>(this)->hud_c = 1;
        jo::self<jo::CutScene>(this)->hud_b = 1;
        return this;
    }

    // ------------------------------------------------------------------ byte getters
    u8 byte_234() { return jo::self<jo::Ending>(this)->is_open_; }  // 0x006d1ef0
    u8 byte_ac() { return jo::self<jo::Player>(this)->riding; }    // 0x006d37c0 (Player::flag_ac)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("BuyScreen::free_slot_for", 0x006c2ce0, &H::free_slot_for);
B("BuyScreen::set_open", 0x006c8d00, &H::set_open);
B("Obj::init_6ca040", 0x006ca040, &H::init_6ca040);
B("Obj::byte_234", 0x006d1ef0, &H::byte_234);
B("Obj::byte_ac", 0x006d37c0, &H::byte_ac);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
//
//      0x006c2dd0 Cutscene::setup_cameras        (thiscall, ret 0) - F_IDLE anims, two cameras.
//
//      0x006ca660 Fader::update                  (thiscall, ret 0) - state machine on +0xf.
//
//      0x006d3590 Quest::check_d3590             (thiscall, ret 0)
//
//  - Float code: every x87 op in 0x0068d760 / 0x00694b80 / 0x006994d0 / 0x006c2dd0 has float
//    operands and is written as separate float statements in the same order. 0x006ca660's
//    `fild qword {g, 0}` path runs only for g < 1000, so (float)g is exact there.
//
//  - 0x006ca660 / 0x006d3590 leave a stale eax (last callee's); their only call sites
//    (0x004e6109 / 0x004e611d) ignore eax, so they are void here.
//
//  - 0x006c2dd0: camera struct 0x28 bytes (ctor 0x00616d50 memsets 0x28) as in the original
//    frame; 0x005ad060's two out dwords are written by it on every path and never read here.
//    Its eax (0x00614040's) is ignored by the only caller; returned anyway.
namespace {
namespace game_b104_13 {

using joan::orig::helpers::at;
// x87 `fcomp` + `test ah,0x40` jnz: equal or unordered.
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }
i32 neg_(u32 v) { return static_cast<i32>(0u - v); }

void* const kAnims = obj(d::g_anim_res_mgr);
void* const kNames = obj(d::g_names);
void* const kFlags8f90 = obj(d::g_game_state);
void* const kCameras = obj(d::g_camera_man);
void* const kRes403a0 = obj(d::g_texture_array);

JOAN_COUNTER(c_setup_cameras, "Cutscene::setup_cameras");
JOAN_COUNTER(c_fader, "Fader::update");
JOAN_COUNTER(c_quest, "Quest::check_d3590");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_get(i16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(w)))); }  // 0x0042d0a0
using joan::orig::helpers::stricmp_;

struct H {
    // ------------------------------------------------------------------ 0x006c2dd0
    // F_IDLE animation for one participant (6c2ce0(which) / unit type / anim list).
    void idle_anim(i32 which, u8* u, jo::ShopStage& st) {
        const u32 sel = joan::orig::arg::as<u32>(joan::orig::as_shop(this)->free_slot_for(joan::orig::arg::i32(which)));  // 0x006c2ce0
        u8* const t = type_get(jo::as_unit(u)->type_id);
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kAnims)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit_type(t)->model_flags[jo::as_unit(u)->collision_id]))));  // 0x004b2730
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(o)->entry(joan::orig::arg::u32(sel)));                                               // 0x004168d0
        void* const anim = *joan::orig::arg::as<void**>(joan::orig::as_anim_res_mgr(kAnims)->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_anim_set_entry(r)->anim_id))));                         // 0x004174f0
        joan::orig::as_actor(st.model)->start_anim(joan::orig::arg::mptr(anim), str(d::s_F_IDLE), joan::orig::arg::u32(global<u32>(d::g_game_time_ms)), 1u, 0u, 0x43fa0000u);  // 0x005abe30 ("F_IDLE", 500.0f)
    }
    // Camera target: bone 0x005ba160 of the actor's model, or the default point.
    void cam_point(jo::ShopStage& st, bool call_ee60, float* pt, jo::CameraParams& cam) {
        void* const model = st.model->def->model->bones;
        const u32 bone = joan::orig::arg::as<u32>(joan::orig::as_bone_table(model)->find(1));  // 0x005ba160
        cam.construct();        // 0x00616d50
        if (bone != 0) {
            void* const node = joan::orig::arg::as<void*>(joan::orig::as_plane(st.model->plane)->bone_5cf8f0(joan::orig::arg::i32(bone)));  // 0x005cf8f0
            if (call_ee60) joan::orig::as_bone(node)->update();                                      // 0x0060ee60
            void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(node)->matrix());                              // 0x0060ef80
            joan::orig::matrix_position(static_cast<void*>(pt), joan::orig::arg::ptr(m));                       // 0x0060c310
            pt[2] = pt[2] - global<float>(d::flt_5);                                      // 5.0
            jo::view<u32>(cam.distance) = 0x42480000u;                                               // 50.0f
        } else {
            jo::view<u32>(pt[0]) = 0x42820000u;   // 65.0f
            jo::view<u32>(pt[1]) = 0xc3610000u;   // -225.0f
            jo::view<u32>(pt[2]) = 0x43640000u;   // 228.0f
            jo::view<u32>(cam.distance) = 0x43bb8000u;  // 375.0f
        }
    }
    u32 setup_cameras() {
        const i32 pid = global<i32>(d::g_player_unit_recno);
        const i32 m0 = unit_missing(pid);
        if (m0 != 0) return static_cast<u32>(m0);
        c_setup_cameras.hit();
        u8* const player = unit_get(global<i32>(d::g_player_unit_recno));
        const i32 m1 = unit_missing(jo::self<jo::BuyScreen>(this)->unit);
        if (m1 != 0) return static_cast<u32>(m1);
        u8* const other = unit_get(jo::self<jo::BuyScreen>(this)->unit);
        joan::orig::as_shop(this)->refresh_6c92e0(0);  // 0x006c92e0
        joan::orig::as_shop(this)->refresh_6c92e0(1);  // 0x006c92e0
        idle_anim(0, other, jo::self<jo::BuyScreen>(this)->stage_player);
        idle_anim(1, player, jo::self<jo::BuyScreen>(this)->stage_shop);
        void* o44 = nullptr;  // [ebp-0x44], [ebp-0x48] (outputs, unused; select_mesh writes pointers into both)
        void* o48 = nullptr;
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_player.model))->select_mesh(joan::orig::arg::mptr(jo::self<jo::BuyScreen>(this)->stage_player.camera), 1, joan::orig::arg::mptr(static_cast<void*>(&o44)), joan::orig::arg::mptr(static_cast<void*>(&o48)));  // 0x005ad060
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_shop.model))->select_mesh(joan::orig::arg::mptr(jo::self<jo::BuyScreen>(this)->stage_shop.camera), 1, joan::orig::arg::mptr(static_cast<void*>(&o44)), joan::orig::arg::mptr(static_cast<void*>(&o48)));  // 0x005ad060
        alignas(4) jo::CameraParams cam;  // [ebp-0x34]
        float pt[3];              // [ebp-0x58]
        cam_point(jo::self<jo::BuyScreen>(this)->stage_player, true, pt, cam);
        if (stricmp_(jo::as_unit_type(type_get(jo::as_unit(player)->type_id))->name, str(d::s_LAHIRE_9cfb3c)) == 0) {
            cam.distance = cam.distance + global<float>(d::flt_25);  // "LAHIRE": += 25
        }
        jo::view<u32>(cam.fov) = 0x4016cbe4u;
        cam.target[0] = pt[0];
        cam.target[1] = pt[1];
        cam.target[2] = pt[2];
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_player.camera))->set_frame(joan::orig::arg::mptr(static_cast<void*>(&cam)));  // 0x00614040
        cam_point(jo::self<jo::BuyScreen>(this)->stage_shop, false, pt, cam);
        jo::view<u32>(cam.fov) = 0x4016cbe4u;
        cam.target[0] = pt[0];
        cam.target[1] = pt[1];
        cam.target[2] = pt[2];
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_shop.camera))->set_frame(joan::orig::arg::mptr(static_cast<void*>(&cam)));  // 0x00614040
        return 0;  // void callee; Shop::setup_cameras's caller drops the eax
    }

    // ------------------------------------------------------------------ 0x006ca660
    u16 fade_step() {
        const u32 v = (jo::self<jo::CutScene>(this)->bar_tex->width + global<u32>(d::g_screen_w)) * global<u32>(d::g_frame_step_ms);
        return static_cast<u16>(v / 0x5dcu);
    }
    void fade_finish() {
        jo::self<jo::CutScene>(this)->active = 0;
        joan::orig::as_group(kNames)->set_paused(static_cast<std::int8_t>(joan::orig::arg::i32(0)));      // 0x0049b5e0
        joan::orig::as_game(kFlags8f90)->set_744(static_cast<char>(joan::orig::arg::i32(0)));  // 0x004725f0
    }
    void fader_update() {
        if (jo::self<jo::CutScene>(this)->active == 0) return;
        c_fader.hit();
        const i32 st = jo::self<jo::CutScene>(this)->state;
        if (st == 6) {
            if (jo::view<i8>(jo::self<jo::CutScene>(this)->started) != 0) {
                if (static_cast<i32>(jo::view<i16>(jo::self<jo::CutScene>(this)->bar_size)) <= neg_(jo::self<jo::CutScene>(this)->bar_tex->width)) {
                    jo::self<jo::CutScene>(this)->state = 7;
                    jo::view<u32>(jo::self<jo::CutScene>(this)->i32_36) = global<u32>(d::g_now);
                    jo::self<jo::CutScene>(this)->elapsed = 0x437f0000u;  // 255.0f
                } else {
                    jo::self<jo::CutScene>(this)->bar_size = static_cast<u16>(jo::self<jo::CutScene>(this)->bar_size - fade_step());
                }
            } else if (static_cast<i32>(jo::view<i16>(jo::self<jo::CutScene>(this)->bar_size)) >= global<i32>(d::g_screen_w)) {
                if (jo::view<i8>(jo::self<jo::CutScene>(this)->u8_13) != 0) {
                    jo::self<jo::CutScene>(this)->state = 1;
                } else {
                    fade_finish();
                    jo::self<jo::CutScene>(this)->state = 0;
                }
                jo::view<i8>(jo::self<jo::CutScene>(this)->u8_40) = 0;
                joan::orig::as_texture_manager(kRes403a0)->release(joan::orig::arg::mptr(jo::self<jo::CutScene>(this)->bar_tex));  // 0x0060a550
                jo::self<jo::CutScene>(this)->bar_tex = nullptr;
            } else {
                jo::self<jo::CutScene>(this)->bar_size = static_cast<u16>(jo::self<jo::CutScene>(this)->bar_size + fade_step());
            }
        } else if (st == 7) {
            if (global<u32>(d::g_now) - jo::view<u32>(jo::self<jo::CutScene>(this)->i32_36) >= 0x2eeu) {
                if (jo::view<i8>(jo::self<jo::CutScene>(this)->started) != 0) {
                    jo::self<jo::CutScene>(this)->state = 8;
                    jo::self<jo::CutScene>(this)->bar_size = global<u16>(d::g_screen_w);
                } else {
                    jo::self<jo::CutScene>(this)->state = 6;
                    jo::self<jo::CutScene>(this)->bar_size = 0xffff;
                    joan::orig::as_camera_man(kCameras)->set_mode(joan::orig::arg::i32(jo::self<jo::CutScene>(this)->follow_target));  // 0x00429860
                }
                jo::view<i8>(jo::self<jo::CutScene>(this)->bar_on) = jo::view<i8>(jo::self<jo::CutScene>(this)->bar_on) == 0 ? 1 : 0;
            }
        } else if (st == 8) {
            if (jo::view<i8>(jo::self<jo::CutScene>(this)->started) != 0) {
                if (static_cast<i32>(jo::view<i16>(jo::self<jo::CutScene>(this)->bar_size)) <= neg_(jo::self<jo::CutScene>(this)->bar_tex->width)) {
                    jo::self<jo::CutScene>(this)->state = jo::view<i8>(jo::self<jo::CutScene>(this)->u8_13) != 0 ? 1 : 2;
                    jo::view<i8>(jo::self<jo::CutScene>(this)->started) = 0;
                    joan::orig::as_texture_manager(kRes403a0)->release(joan::orig::arg::mptr(jo::self<jo::CutScene>(this)->bar_tex));  // 0x0060a550
                    jo::self<jo::CutScene>(this)->bar_tex = nullptr;
                } else {
                    jo::self<jo::CutScene>(this)->bar_size = static_cast<u16>(jo::self<jo::CutScene>(this)->bar_size - fade_step());
                }
            } else if (static_cast<i32>(jo::view<i16>(jo::self<jo::CutScene>(this)->bar_size)) >= global<i32>(d::g_screen_w)) {
                jo::self<jo::CutScene>(this)->state = 7;
                jo::view<u32>(jo::self<jo::CutScene>(this)->i32_36) = global<u32>(d::g_now);
                jo::self<jo::CutScene>(this)->elapsed = 0;
            } else {
                jo::self<jo::CutScene>(this)->bar_size = static_cast<u16>(jo::self<jo::CutScene>(this)->bar_size + fade_step());
            }
        } else {
            if (jo::self<jo::CutScene>(this)->zones != nullptr) joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->update();  // 0x00425fc0
            const u32 g = global<u32>(d::g_frame_step_ms);
            float x;
            if (g >= 1000u) {
                x = global<float>(d::flt_30) * jo::self<jo::CutScene>(this)->duration;  // 30.0
            } else {
                x = static_cast<float>(static_cast<i32>(g)) * jo::self<jo::CutScene>(this)->duration;  // fild qword {g, 0}, g < 1000: exact
            }
            x = x / global<float>(d::flt_1000);  // 1000.0
            jo::view<float>(jo::self<jo::CutScene>(this)->elapsed) = x + jo::view<float>(jo::self<jo::CutScene>(this)->elapsed);
            if (!(jo::view<float>(jo::self<jo::CutScene>(this)->elapsed) >= global<float>(d::k_9cf5c4))) jo::self<jo::CutScene>(this)->elapsed = 0;
            if (jo::view<float>(jo::self<jo::CutScene>(this)->elapsed) > global<float>(d::flt_255)) jo::self<jo::CutScene>(this)->elapsed = 0x437f0000u;  // 255.0f
            if (eq_or_nan(jo::view<float>(jo::self<jo::CutScene>(this)->elapsed), global<float>(d::flt_255))) {
                if (joan::orig::as_track(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->is_type1() == 0) {  // 0x00428aa0
                    jo::self<jo::CutScene>(this)->state = 2;
                    jo::view<i8>(jo::self<jo::CutScene>(this)->started) = 0;
                }
            }
            if (eq_or_nan(jo::view<float>(jo::self<jo::CutScene>(this)->elapsed), global<float>(d::k_9cf5c4))) {
                if (joan::orig::as_track(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->is_type1() == 0) {  // 0x00428aa0
                    fade_finish();
                    jo::self<jo::CutScene>(this)->state = 0;
                    joan::orig::as_camera_man(kCameras)->set_mode(joan::orig::arg::i32(jo::self<jo::CutScene>(this)->follow_target));  // 0x00429860
                    jo::view<i8>(jo::self<jo::CutScene>(this)->u8_40) = 0;
                }
            }
        }
    }

    // ------------------------------------------------------------------ 0x006d3590
    void check_d3590() {
        if (unit_missing(global<i32>(d::g_player_unit_recno)) != 0) return;
        c_quest.hit();
        u8* const u = unit_get(global<i32>(d::g_player_unit_recno));
        if (global<i32>(d::g_control_mode) == 4) {
            if (jo::self<jo::TalkPrompt>(this)->leader != 0) {
                joan::orig::as_talk_prompt(this)->show();  // 0x006d3450
            } else {
                joan::orig::as_talk_prompt(this)->hide();  // 0x006d3480
            }
            return;
        }
        jo::self<jo::TalkPrompt>(this)->near_a = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->nearest_target_a(0x64));  // 0x0046c1f0
        jo::self<jo::TalkPrompt>(this)->near_b = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->nearest_target_b(0xc8));  // 0x0046fa80
        jo::self<jo::TalkPrompt>(this)->near_c = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->nearest_target_c(0x64));  // 0x00471c80
        jo::self<jo::TalkPrompt>(this)->enemy = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->nearest_enemy(0x64));  // 0x0048dfb0
        if (joan::orig::arg::as<u32>(joan::orig::as_unit(u)->mounted()) == 0) {             // 0x00430190
            jo::self<jo::TalkPrompt>(this)->enemy_of = static_cast<i32>(joan::orig::as_unit(u)->nearest_enemy_of(2));  // 0x0044f0b0
            if (unit_missing(jo::self<jo::TalkPrompt>(this)->enemy_of) == 0) {
                u8* const v = unit_get(jo::self<jo::TalkPrompt>(this)->enemy_of);
                if (joan::orig::arg::as<u32>(joan::orig::as_unit(v)->get_1e6()) != 0) jo::view<u32>(jo::self<jo::TalkPrompt>(this)->enemy_of) = 0;  // 0x004f2170
            }
        } else {
            jo::view<u32>(jo::self<jo::TalkPrompt>(this)->enemy_of) = 0;
        }
        if (jo::self<jo::TalkPrompt>(this)->near_a != 0 || jo::self<jo::TalkPrompt>(this)->near_b != 0 || jo::self<jo::TalkPrompt>(this)->near_c != 0 || jo::view<u32>(jo::self<jo::TalkPrompt>(this)->enemy_of) != 0 || jo::self<jo::TalkPrompt>(this)->enemy != 0) {
            joan::orig::as_talk_prompt(this)->show();  // 0x006d3450
            return;
        }
        jo::self<jo::TalkPrompt>(this)->object = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->find_target(0xc8));  // 0x004d6320
        if (jo::self<jo::TalkPrompt>(this)->object != 0) {
            joan::orig::as_talk_prompt(this)->show();  // 0x006d3450
            return;
        }
        jo::self<jo::TalkPrompt>(this)->zone = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->find_zone());  // 0x00514b80
        if (jo::self<jo::TalkPrompt>(this)->zone != 0) {
            joan::orig::as_talk_prompt(this)->show();  // 0x006d3450
            return;
        }
        jo::self<jo::TalkPrompt>(this)->u32_2d = joan::orig::arg::as<u32>(joan::orig::as_unit(u)->find_zone_index());  // 0x005156d0
        if (jo::self<jo::TalkPrompt>(this)->u32_2d != 0) {
            joan::orig::as_talk_prompt(this)->show();  // 0x006d3450
            return;
        }
        joan::orig::as_talk_prompt(this)->hide();  // 0x006d3480
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("Cutscene::setup_cameras", 0x006c2dd0, &H::setup_cameras);
B("Fader::update", 0x006ca660, &H::fader_update);
B("Quest::check_d3590", 0x006d3590, &H::check_d3590);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
//
//    - BuyScreen::init_6c3230 0x006c3230 (ret 0, interfac/i_buy.cpp): new(0x3e) + 0x004571e0,
//      list fill via a stack item (0x00456eb0 / 0x00456cd0 / 0x00456ef0). void (caller
//      overwrites eax). SEH frame: normal path only (operator new 0x007b1d35 returns NULL).
namespace {
namespace game_b108_12 {

JOAN_COUNTER(c_6c3230, "BuyScreen::init_6c3230");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x006c3230
    void init_6c3230() {
        c_6c3230.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->place)))) != 0) return;  // 0x0044cf80
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->place))));  // 0x0043c450
        void* const lst = virtual_call<void*>(o, 0x20);
        if (static_cast<i8>(jo::self<jo::BuyScreen>(this)->items_loaded) != 0) return;
        joan::orig::as_shop(this)->setup_cameras();  // 0x006c2dd0
        jo::view<u32>(jo::self<jo::BuyScreen>(this)->gold) = global<u32>(d::g_get_a596c4);
        joan::orig::as_unit_data(&jo::self<jo::BuyScreen>(this)->items)->copy(joan::orig::arg::mptr(jo::as_unit(unit)->modifiers));  // 0x00497a10
        joan::orig::as_sounds(&jo::self<jo::BuyScreen>(this)->items_b)->create(joan::orig::arg::u32(i32{0}));                // 0x004959b0
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Grid)));        // 0x3e on x86; operator new
        void* const p = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_grid(mem)->Grid_4571e0()) : nullptr;  // 0x004571e0
        jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16) = p;
        joan::orig::as_grid(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16))->alloc(i32{1}, i32{6}, i32{0xd});  // 0x00457290
        if (jo::self<jo::BuyScreen>(this)->prices != nullptr) {
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::BuyScreen>(this)->prices), str(d::s_d_ww_interfac_i_buy_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a237b0)) + 0x1a));  // 0x0058ed70
        }
        const i32 line = static_cast<i32>(global<i16>(d::g_a237b0)) + 0x1b;
        const u32 count = joan::orig::arg::as<u32>(joan::orig::as_shop_facility(lst)->stock_count());  // 0x00469dd0
        jo::self<jo::BuyScreen>(this)->prices = joan::orig::arg::as<jo::I32Array*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(count * static_cast<u32>(sizeof(jo::I32Array))), str(d::s_d_ww_interfac_i_buy_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_shop_facility(lst)->stock_count()); ++i) {
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_shop_facility(lst)->stock_at(joan::orig::arg::i32(i)));  // 0x00469e10
            jo::view<u32>(jo::self<jo::BuyScreen>(this)->prices[i - 1].value) = jo::view<u32>(jo::object<jo::ItemRow>(e)->a);
            alignas(4) u8 item[sizeof(jo::WorldItem) + 2];  // frame slot [ebp-0x74, ebp-0x20); the item uses 0x52 bytes (x86)
            joan::orig::as_world_item(item)->construct();  // 0x00456eb0
            joan::orig::as_item_stack(item)->init(joan::orig::arg::i16(static_cast<i32>(jo::object<jo::ItemRow>(e)->item)), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::object<jo::ItemRow>(e)->b))), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::object<jo::ItemRow>(e)->a))), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00456cd0
            u32 left = 0x0000ffffu;  // word slot [ebp-0x78] = -1 (read / written as a word only)
            if (jo::view<u32>(jo::object<jo::ItemRow>(e)->a) != 0) {
                joan::orig::as_inventory(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16))->add_stack(static_cast<i32>(jo::object<jo::ItemRow>(e)->item), static_cast<i32>(static_cast<i16>(jo::object<jo::ItemRow>(e)->b)), &left, joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(item)->stack.uses)), joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(item)->stack.max_uses)), joan::orig::arg::i32(jo::as_world_item(item)->stack.u32_e), i32{0});  // 0x004577a0
            }
            joan::orig::as_world_item(item)->destruct();  // 0x00456ef0
        }
        jo::self<jo::BuyScreen>(this)->items_loaded = 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("BuyScreen::init_6c3230", 0x006c3230, &H::init_6c3230);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
//
//      0x006c3540 BuyScreen::apply           (thiscall, ret 0)
//
//      0x006d2300 Rec31::clear               (thiscall, ret 0) - memset(this, 0, 0x31).
//
//      0x006d6130 Obj::start                 (thiscall, ret 0) - bool al.
namespace {
namespace game_b95_08 {

void* const kUnitTable = obj(d::g_units);
void* const kPlaces = obj(d::g_places);

JOAN_COUNTER(c_buy_apply, "BuyScreen::apply");
JOAN_COUNTER(c_start, "Obj::start");

using joan::orig::helpers::u;
i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x006c3540
    void buy_apply() {
        c_buy_apply.hit();
        const i32 id = global<i32>(d::g_player_unit_recno);
        if (unit_missing(id) != 0) return;
        u8* const unit = unit_get(global<i32>(d::g_player_unit_recno));
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlaces)->empty_at_e(joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->place))) != 0) return;  // 0x0044cf80
        void* const place = joan::orig::arg::as<void*>(joan::orig::as_list(kPlaces)->get_e(joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->place)));  // 0x0043c450
        using Virtual = u8* (Obj::*)();
        JOAN_MEMBER_POINTER_CHECK(Virtual);
        Virtual method{};
        std::memcpy(&method, &joan::call::vtable_of(place)[0x20 / 4], sizeof(void*));
        u8* const list = (reinterpret_cast<Obj*>(place)->*method)();
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_shop_facility(list)->stock_count()); i >= 1; --i) {  // 0x00469dd0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_shop_facility(list)->stock_at(joan::orig::arg::i32(i)));         // 0x00469e10
            if (jo::object<jo::ItemRow>(e)->a >= 0) jo::object<jo::ItemRow>(e)->a = jo::self<jo::BuyScreen>(this)->prices[i - 1].value;
            if (jo::self<jo::BuyScreen>(this)->prices[i - 1].value == 0) joan::orig::dynarray(jo::as_shop(list)->stock)->linkout(joan::orig::arg::i32(i));  // 0x00584a10
        }
        global<i32>(d::g_get_a596c4) = jo::self<jo::BuyScreen>(this)->gold;
        joan::orig::as_unit_data(jo::as_unit(unit)->modifiers)->copy(joan::orig::arg::mptr(&jo::self<jo::BuyScreen>(this)->items));  // 0x00497a10
    }

    // ------------------------------------------------------------------ 0x006d2300
    void* clear31() {
        std::memset(this, 0, sizeof(jo::TalkPrompt));  // 0x31 on x86 (the TalkPrompt 0xa57030)
        return this;
    }

    // ------------------------------------------------------------------ 0x006d6130
    u32 start() {
        c_start.hit();
        if (jo::view<i8>(jo::self<jo::VideoPlayer>(this)->playing) == 0) return u(this) & 0xffffff00u;
        if (global<i32>(d::g_key) == 0) {
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->value(2));  // 0x00594700
            if (r == 0 && global<i8>(d::g_a51d1d) == 0) return 1u;  // eax = r (0), al = 1
        }
        joan::orig::as_video(obj(d::g_video_file))->close();  // 0x00686160
        global<i32>(d::g_movie_start_ms) = 0;
        jo::view<u8>(jo::self<jo::VideoPlayer>(this)->stage) = static_cast<u8>(jo::view<u8>(jo::self<jo::VideoPlayer>(this)->stage) + 1);
        if (jo::self<jo::VideoPlayer>(this)->stage == 2) jo::view<u8>(jo::self<jo::VideoPlayer>(this)->stage) = static_cast<u8>(jo::view<u8>(jo::self<jo::VideoPlayer>(this)->stage) + 1);
        const u32 g = global<u32>(d::g_game_time_ms);
        jo::self<jo::VideoPlayer>(this)->start_time = g;
        return (g & 0xffffff00u) | 1u;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("BuyScreen::apply", 0x006c3540, &H::buy_apply);
B95("Rec31::clear", 0x006d2300, &H::clear31);
B95("Obj::start", 0x006d6130, &H::start);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b116_08.cpp (game/b116_08)
//
//    - MessageBox::draw 0x006d3cb0 (thiscall, depth, x, y, ret 0xc) -> eax: the framed message box
//      (global object 0x00a52df8). Box size: fixed ([this+0x44c] / [this+0x44e], both != -1) or
//      measured: width from the title [this+4] (Font [this+0x450], 0x00624f00, max 360) or
//      [this+0x44c] (raised to the button-row width when smaller), else 456; height 48 + title
//      line (0x006253f0) + body lines [this+0x44] (0x00627d80 on Font [this+0x454]) + button room,
//      both at least 96. x / y == -1 centre it in the box area: the two screen-size reads at
//      0x006d3f6d (W) / 0x006d3f86 (H) are redirected to display::box_size() by the canvas code,
//      so they go through joan::canvas::size_read(); the other screen-size read (0x006d3eb5, the
//      body's max height) reads the real global like the unpatched instruction. Then the 9 frame
//      sprites FRAME_* of set 0x00a57f10 (0x00633500), the title / GOLDBAR / body texts
//      (0x00626e00) and the up-to-3 buttons ([this] bits 0..2: objects +0x458 / +0x79e / +0xae4,
//
//    - Shop::draw_portrait 0x006c3690 (thiscall, ret 0): "wrapped (ui_center)" in the table - this
//      replacement is JOAN_CANVAS_SELF and opens the ScreenScope itself. Two
//      'portrait' sprites, the two names (Text::draw_3 through thunk 0x00402e0f so its scope applies to
//      the call), the held item under the mouse (cell fit 0x006c99e0 / 0x004574a0 / 0x006c9c40 ->
//      highlight sprite 0x00632a70, then the item label 0x006c2ba0 with the name, "JOAN_SWORD_LV10"
//      + "_<n>" for Saint-Catherine-de-Fierbois's sword) and the message box 0x006d5080 at the end.
//
//  - 0x006d3cb0: the title-line height [ebp-0x18] is written only on the measured-size path; with
//    a fixed size and a non-empty title the original passes the uninitialised slot to 0x00626e00.
//    Not reachable in the game: the only fixed-size caller (0x00736269, set_size(0x263, 0x1d4))
//    clears the title right after (0x00736285), the only non-empty titles (0x006ec971,
//    0x0073acdd) are followed at once by set_size(w, -1), 0x006d5080 clears the title and nothing
//    writes 0x00a52dfc / 0x00a53244 / 0x00a53246 directly. The replacement uses 0 there.
//
//  - 0x006d3cb0's eax (0x006d5080 returns it): the button counter when bit 2 or bit 1 of [this] is
//    set, else 0 (the `and eax, 2` of the bit test), as the original's last eax write.
//
//  - 0x006c3690: frame emulation. The item label is strcpy'd (unbounded) from item +0x10 into the
//    0x82-byte buffer [ebp-0xa0] (+0x241 != 0) / [ebp-0x138] (+0x241 == 0). The original frame
//    [ebp-0x19c, ebp) is one byte array; the locals (this [ebp-4], the item [ebp-0x14] /
//    [ebp-0xac], the cell [ebp-0x18]/[ebp-0x1c] / [ebp-0xb0]/[ebp-0xb4], ...) live there and are
//    re-read at the original's points (item and this after the strcpy, this again at 0x006c3e49),
//    so an overlong name (0x8c+ characters reach the item pointer, 0x9c+ / 0x134+ [this]) changes
//    the same later reads. Overruns past ebp (saved ebp / return address in the original) land in a
//    spill area and are dropped.
namespace {
namespace game_b116_08 {

using joan::orig::helpers::at;
i32 sx16(u16 v) { return static_cast<i32>(static_cast<i16>(v)); }

constexpr fn::Code kStrlen = fn::crt_strlen;   // CRT strlen
constexpr fn::Code kMemset = fn::crt_memset;   // CRT memset
constexpr fn::Code kStrcpy = fn::crt_strcpy;   // CRT strcpy
constexpr fn::Code kStricmp = fn::crt_stricmp;  // CRT _stricmp
constexpr img::Addr kSet = d::g_set;                // frame sprite set
constexpr img::Addr kQueue = d::g_sprites_a46128;    // sprite queue
constexpr img::Addr kScreenW = d::g_screen_w;
constexpr img::Addr kScreenH = d::g_screen_h;

u32 strlen_(const void* s) { return cdecl_call<u32>(kStrlen, s); }

// Colour::Colour (0x0042a260 through 0x004098b8) on a fresh 4-byte local, read back through the
// returned pointer like the original (`mov reg, [eax]`).
u32 colour_ff() {
    u32 c = 0;
    const u32* const r = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&c)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
    return *r;
}
// 0x005e9640 (through 0x00407577) on a fresh 4-byte local with -1, read back through eax.
u32 colour_swizzle_ff() {
    u32 c = 0;
    const u32* const r = joan::orig::arg::as<const u32*>(joan::orig::as_color(&c)->Color_dword(joan::orig::arg::u32(i32{-1})));  // 0x005e9640
    return *r;
}

// SpriteQueue::draw_named (0x00633500 through 0x00401bbd) on 0x00a46128: x, y, depth, name, set,
// colour, then the 11 rectangle / flag arguments.
struct Tail {
    i32 a[11];
};
void named(i32 x, i32 y, i32 depth, const char* name, img::Addr set, u32 colour, const Tail& t) {
    joan::orig::as_sprite_queue(obj(kQueue))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(depth), joan::orig::arg::str(name), img::obj(set), joan::orig::arg::u32(colour), joan::orig::arg::i16(t.a[0]), joan::orig::arg::i16(t.a[1]), joan::orig::arg::i16(t.a[2]), joan::orig::arg::i16(t.a[3]), joan::orig::arg::i16(t.a[4]), joan::orig::arg::i16(t.a[5]), joan::orig::arg::i16(t.a[6]), joan::orig::arg::i16(t.a[7]), joan::orig::arg::i16(t.a[8]), joan::orig::arg::i16(t.a[9]), joan::orig::arg::i16(t.a[10]));  // 0x00633500
}
// BmpSet::find (0x00620b40 through 0x00408e31) on the frame set.
u8* find(img::Addr name) { return joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(obj(kSet))->find(joan::orig::arg::str(str(name)))); }  // 0x00620b40


// 0x006c3690's frame [ebp-0x19c, ebp) plus a spill area for overruns past ebp (saved ebp / return
// address in the original), which are dropped.
struct PortraitFrame {
    alignas(4) u8 m[0x19c];
    alignas(4) u8 spill[0x400];
    // Pointer locals of the original frame as typed members (a frame dword cannot hold an x64 pointer).
    u8* self = nullptr;     // [ebp-0x4]
    u8* unit8 = nullptr;    // [ebp-0x8]
    u8* unitc = nullptr;    // [ebp-0xc]
    u8* item[2] = {};       // [ebp-0x14] (shop item) / [ebp-0xac] (inventory item)
    const u8* ent_b = nullptr;  // [ebp-0xa8]
    template <class T>
    T& L(i32 ebp_off) {  // ebp_off < 0
        return at<T>(m, 0x19c + ebp_off);
    }
    u8* P(i32 ebp_off) { return m + 0x19c + ebp_off; }
};
static_assert(offsetof(PortraitFrame, spill) == 0x19c && sizeof(PortraitFrame::spill) == 0x400, "frame");
// Frame slots of the two held-item paths (+0x241 != 0 / == 0).
struct HeldSlots {
    i32 item, cx, cy, buf, diff;  // item: index into PortraitFrame::item
};
constexpr HeldSlots kSlotsA{0, -0x18, -0x1c, -0xa0, -0xa4};
constexpr HeldSlots kSlotsB{1, -0xb0, -0xb4, -0x138, -0x13c};

JOAN_COUNTER(c_box, "MessageBox::draw");
JOAN_COUNTER(c_portrait, "Shop::draw_portrait");

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------------ 0x006d3cb0
    u32 box_draw(i32 depth, i32 x, i32 y) {
        c_box.hit();
        u8* const s = self();
        i32 n = 0;  // [ebp-0x8] button count
        if ((jo::as_message_box(s)->state & 1) != 0) n++;
        if ((jo::as_message_box(s)->state & 2) != 0) n++;
        if ((jo::as_message_box(s)->state & 4) != 0) n++;
        const i32 row = n != 0 ? n * 0xac + (n - 1) * 0x11 : 0;  // [ebp-0xe4]
        const i16 min_w = static_cast<i16>(row + 0x60);         // [ebp-0xc]
        i16 w = 0;   // [ebp-0x10]
        i16 h = 0;   // [ebp-0x14]
        i16 th = 0;  // [ebp-0x18] title line height (unset on the fixed-size path, see header)
        if (static_cast<i32>(jo::as_message_box(s)->x) == -1 || static_cast<i32>(jo::as_message_box(s)->y) == -1) {
            w = jo::as_message_box(s)->x;
            if (static_cast<i32>(jo::as_message_box(s)->x) != -1 && static_cast<i32>(jo::as_message_box(s)->x) < static_cast<i32>(min_w)) jo::as_message_box(s)->x = min_w;
            h = 0x30;
            if (strlen_(jo::as_message_box(s)->title) != 0) {
                if (static_cast<i32>(jo::as_message_box(s)->x) == -1) {
                    const u32 r = joan::orig::arg::as<u32>(joan::orig::as_font(jo::as_message_box(s)->font)->measure(joan::orig::arg::str(jo::as_message_box(s)->title), -1, 0x168, 0, joan::orig::arg::f32(kOne)));  // 0x00624f00
                    w = static_cast<i16>(static_cast<i32>(static_cast<i16>(r)) + 0x60);
                } else {
                    joan::orig::as_font(jo::as_message_box(s)->font)->measure(joan::orig::arg::str(jo::as_message_box(s)->title), -1, joan::orig::arg::i16(static_cast<i32>(w) - 0x60), 0, joan::orig::arg::f32(kOne));  // 0x00624f00
                }
                th = joan::orig::as_font(jo::as_message_box(s)->font)->text_height(2, joan::orig::arg::f32(kOne));  // 0x006253f0
                h = static_cast<i16>(static_cast<i32>(h) + (static_cast<i32>(th) + 0x14));
            } else if (w == -1) {
                w = 0x1c8;
            }
            if (strlen_(jo::as_message_box(s)->text) != 0) {
                i16 lines = 0;   // [ebp-0x20]
                i16 widest = 0;  // [ebp-0x24]
                joan::orig::as_text_box(jo::as_message_box(s)->text_box)->measure_lines(i32{0x30}, i32{0}, joan::orig::arg::i32(static_cast<i32>(w) - 0x30), joan::orig::arg::i32(global<i32>(kScreenH) - 1), joan::orig::arg::str(jo::as_message_box(s)->text), i32{2}, i32{0}, &lines, &widest, joan::orig::arg::u32(kOne), joan::orig::arg::u32(kOne));  // 0x00627d80
                const i32 l = lines;
                h = static_cast<i16>(static_cast<i32>(h) + (static_cast<i32>(jo::as_message_box(s)->text_box->line_height) * l + l * 2 - 2));
                if (jo::as_message_box(s)->state != 0) h = static_cast<i16>(h + 0x14);
            }
            if (jo::as_message_box(s)->state != 0) h = static_cast<i16>(h + 0x24);
            if (w < 0x60) w = 0x60;
            if (h < 0x60) h = 0x60;
        } else {
            w = jo::as_message_box(s)->x;
            h = jo::as_message_box(s)->y;
        }
        // The button row is centred on the box area (off only for a recording made before this fix). The original's 0x11c is (1024 - 0x1c8) / 2, right
        // only on a 1024-wide screen; the box itself follows the real width.
        const i32 row_left = x == -1 && joan::hook::settings().fix_popup_buttons
                                 ? (joan::canvas::size_read(0x006d3f6d) - 0x1c8) / 2
                                 : 0x11c;
        if (x == -1) x = (joan::canvas::size_read(0x006d3f6d) - static_cast<i32>(w)) / 2;
        if (y == -1) y = (joan::canvas::size_read(0x006d3f86) - static_cast<i32>(h)) / 2;
        const i32 W = w, Hh = h;
        const i16 iw = static_cast<i16>(W - 0x60);  // [ebp-0x28]
        const i16 ih = static_cast<i16>(Hh - 0x60);  // [ebp-0x2c]
        const i32 right = x + W - 0x31, bottom = y + Hh - 0x31;

        // top row
        u32 c = colour_ff();
        named(x, y, depth, str(d::s_FRAME_TL), kSet, c, Tail{{3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1}});  // FRAME_TL
        u8* r = find(d::s_FRAME_T);  // FRAME_T
        if (sx16(jo::as_bmp_entry(r)->u16_32) != 0) {
            const i32 m = static_cast<i32>(iw) < sx16(jo::as_bmp_entry(r)->width) ? static_cast<i32>(iw) : sx16(jo::as_bmp_entry(r)->width);
            c = colour_ff();
            named(x + 0x30, y, depth, str(d::s_FRAME_T), kSet, c, Tail{{3, 0, 0, 0, -1, 0, -1, m - 1, 0, right, -1}});
        } else {
            const i32 m = static_cast<i32>(iw) < sx16(jo::as_bmp_entry(r)->width) ? static_cast<i32>(iw) : sx16(jo::as_bmp_entry(r)->width);
            c = colour_ff();
            named(x + 0x30, y, depth, str(d::s_FRAME_T), kSet, c, Tail{{3, 0, 0, 0, 0, -1, m - 1, -1, 0, right, -1}});
        }
        c = colour_ff();
        named(x + W - 0x30, y, depth, str(d::s_FRAME_TR), kSet, c, Tail{{3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1}});  // FRAME_TR

        // middle row
        r = find(d::s_FRAME_L);  // FRAME_L
        if (sx16(jo::as_bmp_entry(r)->u16_32) != 0) {
            const i32 m = static_cast<i32>(ih) < sx16(jo::as_bmp_entry(r)->height) ? static_cast<i32>(ih) : sx16(jo::as_bmp_entry(r)->height);
            c = colour_ff();
            named(x, y + 0x30, depth, str(d::s_FRAME_L), kSet, c, Tail{{3, 0, 0, 0, 0, -1, m - 1, -1, 0, -1, bottom}});
        } else {
            const i32 m = static_cast<i32>(ih) < sx16(jo::as_bmp_entry(r)->height) ? static_cast<i32>(ih) : sx16(jo::as_bmp_entry(r)->height);
            c = colour_ff();
            named(x, y + 0x30, depth, str(d::s_FRAME_L), kSet, c, Tail{{3, 0, 0, 0, -1, 0, -1, m - 1, 0, -1, bottom}});
        }
        c = colour_ff();
        named(x + 0x30, y + 0x30, depth, str(d::s_FRAME_C), kSet, c,
              Tail{{3, 0, 0, 0, -1, -1, -1, -1, 0, right, bottom}});  // FRAME_C
        r = find(d::s_FRAME_R);  // FRAME_R
        if (sx16(jo::as_bmp_entry(r)->u16_32) != 0) {
            const i32 m = static_cast<i32>(ih) < sx16(jo::as_bmp_entry(r)->height) ? static_cast<i32>(ih) : sx16(jo::as_bmp_entry(r)->height);
            c = colour_ff();
            named(x + W - 0x30, y + 0x30, depth, str(d::s_FRAME_R), kSet, c,
                  Tail{{3, 0, 0, 0, 0, -1, m - 1, -1, 0, -1, bottom}});
        } else {
            const i32 m = static_cast<i32>(ih) < sx16(jo::as_bmp_entry(r)->height) ? static_cast<i32>(ih) : sx16(jo::as_bmp_entry(r)->height);
            c = colour_ff();
            named(x + W - 0x30, y + 0x30, depth, str(d::s_FRAME_R), kSet, c,
                  Tail{{3, 0, 0, 0, -1, 0, -1, m - 1, 0, -1, bottom}});
        }

        // bottom row
        c = colour_ff();
        named(x, y + Hh - 0x30, depth, str(d::s_FRAME_BL), kSet, c, Tail{{3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1}});  // FRAME_BL
        r = find(d::s_FRAME_B);  // FRAME_B
        if (sx16(jo::as_bmp_entry(r)->u16_32) != 0) {
            const i32 m = static_cast<i32>(iw) < sx16(jo::as_bmp_entry(r)->width) ? static_cast<i32>(iw) : sx16(jo::as_bmp_entry(r)->width);
            c = colour_ff();
            named(x + 0x30, y + Hh - 0x30, depth, str(d::s_FRAME_B), kSet, c,
                  Tail{{3, 0, 0, 0, -1, 0, -1, m - 1, 0, right, -1}});
        } else {
            const i32 m = static_cast<i32>(iw) < sx16(jo::as_bmp_entry(r)->width) ? static_cast<i32>(iw) : sx16(jo::as_bmp_entry(r)->width);
            c = colour_ff();
            named(x + 0x30, y + Hh - 0x30, depth, str(d::s_FRAME_B), kSet, c,
                  Tail{{3, 0, 0, 0, 0, -1, m - 1, -1, 0, right, -1}});
        }
        c = colour_ff();
        named(x + W - 0x30, y + Hh - 0x30, depth, str(d::s_FRAME_BR), kSet, c,
              Tail{{3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1}});  // FRAME_BR

        // title, gold bar, body
        i16 ty = static_cast<i16>(y + 0x18);  // [ebp-0x34]
        if (strlen_(jo::as_message_box(s)->title) != 0) {
            const u32 tc = colour_swizzle_ff();
            joan::orig::as_text_renderer(jo::as_message_box(s)->font)->draw_wrapped(joan::orig::arg::u32(x + 0x30), joan::orig::arg::u32(static_cast<i32>(ty)), joan::orig::arg::u32(x + W - 0x30), joan::orig::arg::u32(static_cast<i32>(ty) + static_cast<i32>(th)), joan::orig::arg::u32(depth), joan::orig::arg::str(jo::as_message_box(s)->title), joan::orig::arg::u32(tc), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), joan::orig::arg::u32(i32{2}));  // 0x00626e00
            ty = static_cast<i16>(ty + th);
            if (n != 0) gold_bar(depth, x, W, static_cast<i32>(ty) + 8);
            ty = static_cast<i16>(ty + 0x14);
        }
        if (strlen_(jo::as_message_box(s)->text) != 0) {
            const u32 bc = colour_swizzle_ff();
            const i32 y2 = y + Hh - (jo::as_message_box(s)->state != 0 ? 0x14 : 0) - 0x3c;
            joan::orig::as_text_renderer(jo::as_message_box(s)->text_box)->draw_wrapped(joan::orig::arg::u32(x + 0x30), joan::orig::arg::u32(static_cast<i32>(ty)), joan::orig::arg::u32(x + W - 0x30), joan::orig::arg::u32(y2), joan::orig::arg::u32(depth), joan::orig::arg::str(jo::as_message_box(s)->text), joan::orig::arg::u32(bc), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), joan::orig::arg::u32(i32{2}));  // 0x00626e00
        }
        const i16 by = static_cast<i16>(y + Hh - 0x3c);  // [ebp-0x38]
        if (n != 0 && strlen_(jo::as_message_box(s)->title) != 0) gold_bar(depth, x, W, static_cast<i32>(by) - 0xc);

        // buttons
        i32 i = 0;  // [ebp-0x3c]
        u32 eax = 0;
        const i32 bx0 = (0x1c8 - n * 0xac - (n - 1) * 0x11) / 2;
        if ((jo::as_message_box(s)->state & 1) != 0) {
            button(&jo::as_message_box(s)->button0, bx0 + i * 0xbd + row_left, by, depth);
            i++;
        }
        if ((jo::as_message_box(s)->state & 2) != 0) {
            button(&jo::as_message_box(s)->button1, bx0 + i * 0xbd + row_left, by, depth);
            i++;
            eax = static_cast<u32>(i);
        }
        if ((jo::as_message_box(s)->state & 4) != 0) {
            button(&jo::as_message_box(s)->button2, bx0 + i * 0xbd + row_left, by, depth);
            i++;
            eax = static_cast<u32>(i);
        }
        return eax;
    }

    static void gold_bar(i32 depth, i32 x, i32 w, i32 gy) {
        u8* const r = find(d::s_GOLDBAR);  // GOLDBAR
        const i32 right = x + w - 0x31;
        if (sx16(jo::as_bmp_entry(r)->u16_32) != 0) {
            const u32 c = colour_ff();
            named(x + 0x30, gy, depth, str(d::s_GOLDBAR), kSet, c, Tail{{3, 0, 0, 0, -1, -1, -1, w - 0x61, 0, right, -1}});
        } else {
            const u32 c = colour_ff();
            named(x + 0x30, gy, depth, str(d::s_GOLDBAR), kSet, c, Tail{{3, 0, 0, 0, -1, -1, w - 0x61, -1, 0, right, -1}});
        }
    }

    static void button(jo::Button* b, i32 bx, i16 by, i32 depth) {
        joan::orig::as_button(b)->set_rect(joan::orig::arg::u32(bx), joan::orig::arg::u32(static_cast<i32>(by)), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{0}));  // 0x00623580
        joan::orig::as_button(b)->draw(joan::orig::arg::u32(depth), 7);                                       // 0x00622cc0
    }

    // ------------------------------------------------------------------ 0x006c3690
    void portrait_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006c3690);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_portrait.hit();
        PortraitFrame fr{};
        fr.self = reinterpret_cast<u8*>(this);
        u8* const s = self();
        if (static_cast<i32>(jo::as_buy_screen(s)->open) != 0) {
            void* const items = obj(d::g_units);
            if (joan::orig::as_dynarray(items)->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return;  // 0x00428b70
            fr.unit8 = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(items)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
            if (joan::orig::as_dynarray(items)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::as_buy_screen(s)->unit))) != 0) return;               // 0x00428b70
            fr.unitc = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(items)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_buy_screen(s)->unit))));         // 0x0041b3c0
            u32 c = colour_swizzle_ff();
            portrait_sprite(s, 8, c);
            c = colour_swizzle_ff();
            portrait_sprite(s, 0xe0, c);
            joan::orig::as_colour(fr.P(-0x10))->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff});  // 0x0042a260
            const u32 white = fr.L<u32>(-0x10);
            const i32 W2 = (global<i32>(kScreenW) - 0x320) / 2;
            const i32 H2 = (global<i32>(kScreenH) - 0x258) / 2;
            joan::orig::as_text(obj(d::g_dialogs))->draw_3(joan::orig::arg::i32(W2 + 0x3c), joan::orig::arg::i32(H2 + 0xb0), joan::orig::arg::i32(W2 + 0xa4), joan::orig::arg::i32(H2 + 0xd0), joan::orig::arg::u32(i32{6}), joan::orig::arg::str(jo::as_unit(fr.unitc)->name_b), joan::orig::arg::u32(white), i32{0}, i32{0}, i32{0}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626130 Text::draw_3
            const i32 W3 = (global<i32>(kScreenW) - 0x320) / 2;
            const i32 H3 = (global<i32>(kScreenH) - 0x258) / 2;
            joan::orig::as_text(obj(d::g_dialogs))->draw_3(joan::orig::arg::i32(W3 + 0x3c), joan::orig::arg::i32(H3 + 0x188), joan::orig::arg::i32(W3 + 0xa4), joan::orig::arg::i32(H3 + 0x1a8), joan::orig::arg::u32(i32{6}), joan::orig::arg::str(jo::as_unit(fr.unit8)->name_), joan::orig::arg::u32(white), i32{0}, i32{0}, i32{0}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626130 Text::draw_3
            if (jo::view<u32>(jo::as_buy_screen(s)->drag_state) != 0) {
                if (jo::view<u32>(jo::as_buy_screen(s)->drag_from_shop) != 0) {
                    fr.item[0] =
                        joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(jo::as_buy_screen(s)->drag_type)))));  // 0x00425ce0
                    held_item(fr, kSlotsA);
                } else {
                    fr.ent_b = joan::orig::arg::as<const u8*>(joan::orig::as_dynarray(jo::as_buy_screen(s)->items_b.grid_16)->get(joan::orig::arg::i32(jo::view<u32>(jo::as_buy_screen(s)->drag_index))));  // 0x00417200
                    fr.item[1] = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u32>(*reinterpret_cast<const u16*>(
                                                              fr.ent_b)))));  // 0x00425ce0
                    held_item(fr, kSlotsB);
                }
            }
        }
        // 0x006c3e49: [ebp-0x4] re-read (an overlong item label may have overwritten it)
        u8* const t = fr.self;
        if (jo::as_buy_screen(t)->open != 0 && static_cast<i8>(jo::as_buy_screen(t)->dragging) != 0) joan::orig::as_dialog(obj(d::g_popup_a52df8))->init();  // 0x006d5080
    }

    static void portrait_sprite(u8* s, i32 dy, u32 colour) {
        const i32 y = (global<i32>(kScreenH) - 0x258) / 2 + dy;
        const i32 x = (global<i32>(kScreenW) - 0x320) / 2 + 8;
        joan::orig::as_sprite_queue(obj(kQueue))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 6u, str(d::s_portrait), joan::orig::arg::mptr(jo::as_buy_screen(s)->sheet), joan::orig::arg::u32(colour), 3, 0, 0, 1, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500 'portrait'
    }

    // The item held by the mouse: grid highlight when it fits, then its label at the cursor. Every
    // local is read from / written to the emulated frame at the original's points; [this] and the
    // item are re-read after the strcpy into the 0x82-byte label buffer.
    static void held_item(PortraitFrame& fr, const HeldSlots& o) {
        u8* const item = fr.item[o.item];
        joan::orig::as_shop(fr.self)->cell_at_cursor(joan::orig::arg::i32(sx16(jo::as_item_def(item)->stats[0])), joan::orig::arg::i32(sx16(jo::as_item_def(item)->stats[1])), joan::orig::arg::mptr(fr.P(o.cx)), joan::orig::arg::mptr(fr.P(o.cy)));  // 0x006c99e0
        if (fr.L<i32>(o.cx) >= 0 && fr.L<i32>(o.cy) >= 0) {
            bool fits = joan::orig::arg::as<i32>(joan::orig::as_grid(jo::as_buy_screen(fr.self)->items.grid_16)->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(item)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(item)->stats[1])))), i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(fr.L<u8>(o.cx)))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(fr.L<u8>(o.cy)))), i32{0})) != 0;  // 0x004574a0
            if (!fits)
                fits = joan::orig::arg::as<i32>(joan::orig::as_shop(fr.self)->item_under(joan::orig::arg::i32(sx16(jo::as_item_def(item)->stats[0])), joan::orig::arg::i32(sx16(jo::as_item_def(item)->stats[1])), i32{0}, joan::orig::arg::i32(fr.L<i32>(o.cx)), joan::orig::arg::i32(fr.L<i32>(o.cy)))) != 0;  // 0x006c9c40
            if (fits) {
                const i32 tex = global<i32>(d::g_a6a54c);
                const i32 cx = fr.L<i32>(o.cx), cy = fr.L<i32>(o.cy);
                const i32 W2 = (global<i32>(kScreenW) - 0x320) / 2;
                const i32 H2 = (global<i32>(kScreenH) - 0x258) / 2;
                const i32 y2 = H2 + (cy + sx16(jo::as_item_def(item)->stats[1])) * 0x28 + 0x38;
                const i32 x2 = W2 + (cx + sx16(jo::as_item_def(item)->stats[0])) * 0x28 + 0x210;
                const i32 y1 = H2 + cy * 0x28 + 0x38;
                const i32 x1 = W2 + cx * 0x28 + 0x210;
                joan::orig::as_sprite2d(obj(kQueue))->queue(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{6}), joan::orig::arg::u32(tex), joan::orig::arg::mptr(i32{0}), i32{3}, i32{0}, i32{0}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0});  // 0x00632a70
            }
        }
        char* const buf = reinterpret_cast<char*>(fr.P(o.buf));  // 0x82 bytes in the original frame
        if (cdecl_call<i32>(kStricmp, jo::as_item_def(item)->file_name, str(d::s_Saint_Catherine_de_Fierbois)) != 0) {  // 'Saint-Catherine-de-Fierbois'
            cdecl_call<char*>(kStrcpy, buf, jo::as_item_def(item)->name);  // unbounded: may overrun into later locals
        } else {
            cdecl_call<void*>(kMemset, buf, i32{0}, u32{0x82});
            cdecl_call<char*>(kStrcpy, buf, str(d::s_JOAN_SWORD_LV10));  // 'JOAN_SWORD_LV10'
            const i32 len_name = static_cast<i32>(strlen_(jo::as_item_def(fr.item[o.item])->name));
            fr.L<i32>(o.diff) = len_name - static_cast<i32>(strlen_(str(d::s_JOAN_SWORD_LV10)));
            if (fr.L<i32>(o.diff) != 0) {
                fr.P(o.buf)[strlen_(buf)] = '_';
                const u8 ch = static_cast<u8>(fr.L<i32>(o.diff) + 0x30);
                fr.P(o.buf)[strlen_(buf)] = ch;
            }
        }
        const i32 my = global<i32>(d::g_mouse_y) - sx16(jo::as_item_def(fr.item[o.item])->stats[1]) * 0x28 / 2;
        const i32 mx = global<i32>(d::g_mouse_x) - sx16(jo::as_item_def(fr.item[o.item])->stats[0]) * 0x28 / 2;
        joan::orig::as_shop(fr.self)->draw_item_icon(joan::orig::arg::i32(mx), joan::orig::arg::i32(my), joan::orig::arg::str(buf));  // 0x006c2ba0
    }
};

JOAN_REPLACE("game/b116_08", "MessageBox::draw", 0x006d3cb0, &H::box_draw);
JOAN_REPLACE("game/b116_08", "Shop::draw_portrait", 0x006c3690, &H::portrait_draw);
JOAN_CANVAS_SELF(0x006c3690);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_08
}  // namespace

// ---------------------------------------- from modules/game_b111_17.cpp (fix/uninit)
//
//    - Shop::buy_6c5a00 0x006c5a00 (thiscall, ret 0, returns 0/1): when the free-slot search
namespace {
namespace game_b111_17 {


JOAN_COUNTER(c_buy, "Shop::buy_6c5a00");

struct H {
    // -------------------------------------------------------------- 0x006c5a00
    void buy_fail_ui(i8 state, std::uintptr_t text_fn) {
        jo::view<i8>(jo::self<jo::BuyScreen>(this)->dragging) = state;
        joan::orig::as_popup(obj(d::g_popup_a52df8))->set_title(joan::orig::arg::str(0), 0u);                      // 0x006d52c0
        const char* const text = thiscall<const char*>(text_fn, obj(d::g_strings));  // 0x004da7e0 / 0x004da810
        joan::orig::as_popup(obj(d::g_popup_a52df8))->set_text_6d5230(text, 0u);                   // 0x006d5230
        joan::orig::as_popup(obj(d::g_popup_a52df8))->set_buttons(1u);                         // 0x006d5340
        joan::orig::as_popup(obj(d::g_popup_a52df8))->load_gpopup();                            // 0x006d3be0
        joan::orig::as_widget_child(&jo::self<jo::BuyScreen>(this)->panel_a->child)->disable();                          // 0x006ca020
        joan::orig::as_widget_child(&jo::self<jo::BuyScreen>(this)->panel_b->child)->disable();                        // 0x006ca020
    }
    i32& stock() { return jo::self<jo::BuyScreen>(this)->prices[jo::view<i32>(jo::self<jo::BuyScreen>(this)->drag_item) - 1].value; }
    u32 buy() {
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->place)))) != 0) return 0;  // 0x0044cf80
        c_buy.hit();
        void* const shop = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->place))));  // 0x0043c450
        void* const list = virtual_call<void*>(shop, 0x20);
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return 0;  // 0x00428b70
        (void)joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0
        u8* const entry = joan::orig::arg::as<u8*>(joan::orig::as_shop_facility(list)->stock_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_item))));                         // 0x00469e10
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::object<jo::ItemRow>(entry)->item)));  // 0x00425ce0
        const Fpu fpu{true};
        const i32 k = jo::object<jo::ItemRow>(entry)->b - jo::as_item_def(type)->min_level + 1;
        const i32 cost = joan::hook::x87::ftol(
            fpu.mul(fpu.mul(static_cast<double>(jo::as_item_def(type)->price), static_cast<double>(jo::object<jo::ItemRow>(entry)->value)),
                    static_cast<double>(k)));
        i8 ok = 0;
        if (jo::self<jo::BuyScreen>(this)->gold < cost) {
            buy_fail_ui(2, fn::code_value(fn::Buy_open_8));
            jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_state) = 0;
            joan::orig::as_parser(obj(d::g_mouse))->reset();  // 0x00595a20
            return 1;
        }
        if (!(stock() > -1)) {
            ok = 1;
        } else if (stock() > 0) {
            stock() = stock() - 1;
            ok = 1;
        }
        if (ok != 0) {
            alignas(alignof(void*)) u8 item[sizeof(jo::WorldItem) > 0x52 ? sizeof(jo::WorldItem) : 0x52];  // x64: the larger WorldItem
            joan::orig::as_world_item(item)->construct();  // 0x00456eb0
            joan::orig::as_item_stack(item)->init(joan::orig::arg::i16(jo::object<jo::ItemRow>(entry)->item), joan::orig::arg::i16(static_cast<i16>(jo::object<jo::ItemRow>(entry)->b)), joan::orig::arg::i16(static_cast<i16>(jo::object<jo::ItemRow>(entry)->a)), 0u, 0u, 0u);  // 0x00456cd0
            // fix/uninit: 0x006c9d20 writes the slot only when it finds one.
            i32 slot_x = 0;
            i32 slot_y = 0;
            bool place = joan::orig::arg::as<i32>(joan::orig::as_shop(this)->find_free(static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->stats[0])), static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->stats[1])), &slot_x, &slot_y)) != 0;  // 0x006c9d20
            if (!place)
                place = static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_shop(this)->has_enough(static_cast<i32>(jo::object<jo::ItemRow>(entry)->item), joan::orig::arg::i16(jo::view<i16>(jo::as_item_def(type)->stats[3])), joan::orig::arg::i16(static_cast<i16>(jo::as_world_item(item)->stack.uses)), joan::orig::arg::i16(static_cast<i16>(jo::as_world_item(item)->stack.max_uses))))) != 0;  // 0x006c9f10
            if (place) {
                joan::orig::as_inventory(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16))->add_457ba0(joan::orig::arg::i32(jo::object<jo::ItemRow>(entry)->item), joan::orig::arg::i32(static_cast<i16>(jo::object<jo::ItemRow>(entry)->b)), joan::orig::arg::i32(jo::view<i16>(jo::as_item_def(type)->stats[3])), joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(item)->stack.uses)), joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(item)->stack.max_uses)), joan::orig::arg::i32(jo::as_world_item(item)->stack.u32_e), 0, joan::orig::arg::i32(static_cast<u8>(slot_x)), joan::orig::arg::i32(static_cast<u8>(slot_y)));  // 0x00457ba0
                jo::self<jo::BuyScreen>(this)->gold = jo::self<jo::BuyScreen>(this)->gold - cost;
                if (jo::view<i8>(jo::as_item_def(type)->show_armory) != 0 &&
                    joan::orig::as_game(obj(d::g_game_state))->has_word_728(jo::view<i16>(jo::as_item_def(type)->id)) == 0)  // 0x00472a90
                    joan::orig::as_game(obj(d::g_game_state))->push_word_728(joan::orig::arg::i32(jo::view<i16>(jo::as_item_def(type)->id)));               // 0x004729c0
                if (stock() == 0) joan::orig::as_grid(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16))->fill_from(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_index)));  // 0x00457ec0
            } else {
                if (!(stock() < 0)) stock() = stock() + 1;
                buy_fail_ui(1, fn::code_value(fn::Buy_open_9));
            }
            joan::orig::as_world_item(item)->destruct();  // 0x00456ef0
        }
        jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_state) = 0;
        joan::orig::as_parser(obj(d::g_mouse))->reset();  // 0x00595a20
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: slot x / y 0 when 0x006c9d20 found no slot (garbage [ebp-0x80] / [ebp-0x84]).
FIX("Shop::buy_6c5a00", 0x006c5a00, &H::buy);
#undef FIX

}  // namespace game_b111_17
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (fix/uninit)
//
//      0x006c5f10 Shop::drop_item_6c5f10      (thiscall, ret 0) -> 0 / 1. When 0x006c9d20 fails
//                 and 0x006c9f10 accepts, the grid x / y bytes passed to 0x00457ba0 are
//                 uninitialised (used there only for a negative stack size or overfull stacks);
//                 0 is passed.
namespace {
namespace game_b111_16 {

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

void* const kItemDefs = obj(d::g_item_res);
void* const kMsgBox = obj(d::g_popup_a52df8);
void* const kTexts = obj(d::g_strings);

JOAN_COUNTER(c_drop, "Shop::drop_item_6c5f10");

struct H {
    // -------------------------------------------------------------- 0x006c5f10 (fix/uninit)
    void message(std::uintptr_t text_fn) {
        joan::orig::as_popup(kMsgBox)->set_title(joan::orig::arg::str(0), 0u);                    // 0x006d52c0
        void* const text = thiscall<void*>(text_fn, kTexts);         // 0x004da7e0 / 0x004da810
        joan::orig::as_popup(kMsgBox)->set_text_6d5230(joan::orig::arg::str(text), 0u);                 // 0x006d5230 (push 0 first)
        joan::orig::as_popup(kMsgBox)->set_buttons(1u);                       // 0x006d5340
        joan::orig::as_popup(kMsgBox)->load_gpopup();                          // 0x006d3be0
        joan::orig::as_widget_child(&jo::self<jo::BuyScreen>(this)->panel_a->child)->disable();            // 0x006ca020
        joan::orig::as_widget_child(&jo::self<jo::BuyScreen>(this)->panel_b->child)->disable();
    }
    u32 drop_item() {
        c_drop.hit();
        void* const inv = obj(d::g_places);
        if (joan::orig::arg::as<i32>(joan::orig::as_list(inv)->empty_at_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->place)))) != 0) return 0;  // 0x0044cf80
        void* const rec = joan::orig::arg::as<void*>(joan::orig::as_list(inv)->get_e(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::BuyScreen>(this)->place))));  // 0x0043c450
        void* const list = thiscall<void*>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(rec)[0x20 / 4]), rec);  // vtable slot 8
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return 0;
        (void)joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // result unused
        const i32 gx = (global<i32>(d::g_mouse_x) - ((global<i32>(d::g_screen_w) - 0x320) / 2 + 0x100)) / 0x28;
        const i32 gy = (global<i32>(d::g_mouse_y) - ((global<i32>(d::g_screen_h) - 0x258) / 2 + 0x38)) / 0x28;
        const i32 slot = joan::orig::arg::as<i32>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16))->find_at(joan::orig::arg::i32(gx), joan::orig::arg::i32(gy)));  // 0x00458170
        if (slot != 0) {
            jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_state) = 0;
            if (jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_state) != 0 && jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_from_shop) == 0 && jo::self<jo::BuyScreen>(this)->drag_index != slot) {  // dead (just cleared)
                joan::orig::as_parser(obj(d::g_mouse))->reset();
                return 1;
            }
            joan::orig::as_ptr_list(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16))->get(joan::orig::arg::i32(slot));  // 0x00457e90, result unused
            const Fpu fpu{true};
            for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_shop_facility(list)->stock_count()); ++i) {  // 0x00469dd0
                u8* const it = joan::orig::arg::as<u8*>(joan::orig::as_shop_facility(list)->stock_at(joan::orig::arg::i32(i)));         // 0x00469e10
                if (i != slot) continue;
                u8* const def = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemDefs)->get(joan::orig::arg::i16(jo::object<jo::ItemRow>(it)->item)));  // 0x00425ce0
                const i32 k = static_cast<i32>(jo::view<u32>(jo::object<jo::ItemRow>(it)->b) - jo::view<u32>(jo::as_item_def(def)->min_level) + 1u);
                const i32 price = ftol(fpu.mul(fpu.mul(jo::as_item_def(def)->price, jo::object<jo::ItemRow>(it)->value), k));
                bool ok = false;
                i32* const counts = jo::view<i32*>(jo::self<jo::BuyScreen>(this)->prices);
                if (jo::self<jo::BuyScreen>(this)->gold >= price) {
                    if (counts[i - 1] <= -1) {
                        ok = true;
                    } else if (counts[i - 1] > 0) {
                        counts[i - 1] = counts[i - 1] - 1;
                        ok = true;
                    }
                } else {
                    jo::self<jo::BuyScreen>(this)->dragging = 2;
                    message(fn::code_value(fn::Buy_open_8));  // 0x004da7e0
                    jo::view<u32>(jo::self<jo::BuyScreen>(this)->drag_state) = 0;
                    joan::orig::as_parser(obj(d::g_mouse))->reset();  // 0x00595a20
                    return 1;
                }
                if (!ok) continue;
                alignas(4) u8 r[sizeof(jo::WorldItem) + 2] = {};  // 0x54 on x86
                joan::orig::as_world_item(r)->construct();  // 0x00456eb0 ctor
                joan::orig::as_item_stack(r)->init(joan::orig::arg::i16(jo::object<jo::ItemRow>(it)->item), joan::orig::arg::i16(static_cast<i16>(jo::object<jo::ItemRow>(it)->b)), joan::orig::arg::i16(static_cast<i16>(jo::object<jo::ItemRow>(it)->a)), 0u, 0u, 0u);  // 0x00456cd0
                u8* const def2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemDefs)->get(joan::orig::arg::i16(jo::object<jo::ItemRow>(it)->item)));
                i32 gx2 = 0, gy2 = 0;  // fix: uninitialised in the original when 0x006c9d20 fails
                bool placed = joan::orig::arg::as<i32>(joan::orig::as_shop(this)->find_free(static_cast<i32>(jo::view<i16>(jo::as_item_def(def2)->stats[0])), static_cast<i32>(jo::view<i16>(jo::as_item_def(def2)->stats[1])), &gx2, &gy2)) != 0;  // 0x006c9d20
                if (!placed) {
                    placed = joan::orig::arg::as<i8>(joan::orig::as_shop(this)->has_enough(static_cast<i32>(jo::object<jo::ItemRow>(it)->item), joan::orig::arg::i16(jo::view<i16>(jo::as_item_def(def2)->stats[3])), joan::orig::arg::i16(static_cast<i16>(jo::as_world_item(r)->stack.uses)), joan::orig::arg::i16(static_cast<i16>(jo::as_world_item(r)->stack.max_uses)))) != 0;  // 0x006c9f10
                }
                if (placed) {
                    joan::orig::as_inventory(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16))->add_457ba0(joan::orig::arg::i32(jo::object<jo::ItemRow>(it)->item), joan::orig::arg::i32(static_cast<i16>(jo::object<jo::ItemRow>(it)->b)), joan::orig::arg::i32(jo::view<i16>(jo::as_item_def(def2)->stats[3])), joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(r)->stack.uses)), joan::orig::arg::i32(jo::view<u32>(jo::as_world_item(r)->stack.max_uses)), joan::orig::arg::i32(jo::as_world_item(r)->stack.u32_e), 0, joan::orig::arg::i32(static_cast<u8>(gx2)), joan::orig::arg::i32(static_cast<u8>(gy2)));  // 0x00457ba0
                    jo::self<jo::BuyScreen>(this)->gold = jo::self<jo::BuyScreen>(this)->gold - price;
                    if (jo::view<i8>(jo::as_item_def(def2)->show_armory) != 0) {
                        if (joan::orig::as_game(obj(d::g_game_state))->has_word_728(jo::view<i16>(jo::as_item_def(def2)->id)) == 0) {  // 0x00472a90
                            joan::orig::as_game(obj(d::g_game_state))->push_word_728(joan::orig::arg::i32(jo::view<i16>(jo::as_item_def(def2)->id)));                  // 0x004729c0
                        }
                    }
                    if (jo::view<i32*>(jo::self<jo::BuyScreen>(this)->prices)[i - 1] == 0) joan::orig::as_grid(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16))->fill_from(joan::orig::arg::i32(slot));  // 0x00457ec0
                } else {
                    i32* const c2 = jo::view<i32*>(jo::self<jo::BuyScreen>(this)->prices);
                    if (c2[i - 1] >= 0) c2[i - 1] = c2[i - 1] + 1;
                    jo::self<jo::BuyScreen>(this)->dragging = 1;
                    message(fn::code_value(fn::Buy_open_9));  // 0x004da810
                }
                joan::orig::as_world_item(r)->destruct();  // 0x00456ef0 dtor
            }
        }
        joan::orig::as_parser(obj(d::g_mouse))->reset();  // 0x00595a20
        return 1;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: grid x / y bytes for 0x00457ba0 are 0 when 0x006c9d20 failed (original: garbage).
F("Shop::drop_item_6c5f10", 0x006c5f10, &H::drop_item);
#undef F

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- from modules/game_b115_09.cpp (fix/uninit)
//
//    - Shop::mouse_6c6550 0x006c6550 (thiscall, ret 0, returns bool in al, i_buy.cpp): the
//      shop / inventory mouse handler. With +0x231 == 1 and +0x245 == 0 (0x006c7b60: picking from
//      the shop list sets +0x231 = 1 without a held item), the release path 0x006c68b1 ->
//      0x006c6910 -> 0x006c696f passes the never-written cell locals [ebp-0x2c] / [ebp-0x30] to
//      0x00458170 (item at cell). Fix: -1 / -1 (the "mouse outside the grid" value 0x006c99e0
//      writes; 0x00458170 finds no item there). Everything else exact.
namespace {
namespace game_b115_09 {

using joan::hook::x87::ftol;

i32 sb(u8 b) { return static_cast<i32>(static_cast<i8>(b)); }   // byte argument

void* const kItemTypes = obj(d::g_item_res);
void* const kUi = obj(d::g_mouse);

JOAN_COUNTER(c_shop, "Shop::mouse_6c6550");

u8* item_type(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (word)
u8* inv_slot(void* inv, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(inv)->get(joan::orig::arg::i32(i))); }  // 0x00457e90

// ======================================================================= 0x006c6550
i32 ux(i32 off) { return (global<i32>(d::g_screen_w) - 0x320) / 2 + off; }
i32 uy(i32 off) { return (global<i32>(d::g_screen_h) - 0x258) / 2 + off; }
// UI hit tests of 0x00a516b0 on the inventory (R1) and shop (R2) panels.
i32 ui_test(std::uintptr_t thunk, i32 x0, i32 y0, i32 x1, i32 y1) {
    return thiscall<i32>(thunk, kUi, x0, y0, x1, y1, i32{0});
}
i32 r1(std::uintptr_t thunk) { return ui_test(thunk, ux(0x210), uy(0x38), ux(0x300), uy(0x178)); }
i32 r2(std::uintptr_t thunk) { return ui_test(thunk, ux(0x100), uy(0x38), ux(0x1f0), uy(0x240)); }
void ui_reset() { joan::orig::as_parser(kUi)->reset(); }  // 0x00595a20

struct HShop {
    void* inv() { return jo::view<void*>(jo::self<jo::BuyScreen>(this)->items.grid_16); }
    // the dragged item's type: +0x245 read as a word (movsx)
    i32 drag_type() { return static_cast<i32>(static_cast<i16>(jo::self<jo::BuyScreen>(this)->drag_type)); }

    // Held item -> inventory slot index state copy (0x00417200 record into +0x245..+0x255).
    void take(i32 idx) {
        jo::self<jo::BuyScreen>(this)->drag_index = idx;
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(inv())->get(joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->drag_index)));  // 0x00417200
        jo::self<jo::BuyScreen>(this)->drag_type = jo::as_inv_item(rec)->type;
        jo::self<jo::BuyScreen>(this)->drag_level = jo::as_inv_item(rec)->level;
        jo::self<jo::BuyScreen>(this)->drag_count = jo::as_inv_item(rec)->count;
        jo::self<jo::BuyScreen>(this)->u32_24d = jo::as_inv_item(rec)->u32_6;
        jo::self<jo::BuyScreen>(this)->u32_251 = jo::as_inv_item(rec)->u32_a;
        jo::self<jo::BuyScreen>(this)->u32_255 = jo::as_inv_item(rec)->u32_e;
        joan::orig::as_grid(inv())->fill_from(joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->drag_index));  // 0x00457ec0
        jo::self<jo::BuyScreen>(this)->drag_from_shop = 1;
    }
    i32 fits(u8* t, i32 cx, i32 cy) {  // 0x004574a0
        return joan::orig::arg::as<i32>(joan::orig::as_grid(inv())->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(jo::as_item_def(t)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(jo::as_item_def(t)->stats[1])))), i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(cx)))), static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(cy)))), i32{0}));
    }
    i32 swap_at(u8* t, i32 cx, i32 cy) {  // 0x006c9c40
        return joan::orig::arg::as<i32>(joan::orig::as_shop(this)->item_under(joan::orig::arg::i32(static_cast<i32>(static_cast<i16>(jo::as_item_def(t)->stats[0]))), joan::orig::arg::i32(static_cast<i32>(static_cast<i16>(jo::as_item_def(t)->stats[1]))), i32{0}, joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));
    }
    void place(i32 cx, i32 cy) {  // 0x004976e0 on this+0x38 (items)
        joan::orig::as_unit_data(&jo::self<jo::BuyScreen>(this)->items)->add_to_grid16_1a(joan::orig::arg::u16(static_cast<i32>(drag_type())), joan::orig::arg::u16(static_cast<i32>(jo::self<jo::BuyScreen>(this)->drag_level)), joan::orig::arg::u16(static_cast<i32>(jo::self<jo::BuyScreen>(this)->drag_count)), joan::orig::arg::u32(jo::self<jo::BuyScreen>(this)->u32_24d), joan::orig::arg::u32(jo::self<jo::BuyScreen>(this)->u32_251), joan::orig::arg::u32(jo::self<jo::BuyScreen>(this)->u32_255), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(static_cast<u8>(cx)))), static_cast<std::uint8_t>(joan::orig::arg::u32(sb(static_cast<u8>(cy)))), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})));
    }
    void cell_of(u8* t, i32* cx, i32* cy) {  // 0x006c99e0
        joan::orig::as_shop(this)->cell_at_cursor(joan::orig::arg::i32(static_cast<i32>(static_cast<i16>(jo::as_item_def(t)->stats[0]))), joan::orig::arg::i32(static_cast<i32>(static_cast<i16>(jo::as_item_def(t)->stats[1]))), joan::orig::arg::mptr(cx), joan::orig::arg::mptr(cy));
    }

    // Release over the inventory grid (0x006c696f / 0x006c7cd2): t is only read in state 3.
    void release(u8* t, i32& cx, i32& cy) {
        const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_grid(inv())->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170
        if (idx != 0) {
            inv_slot(inv(), idx);  // 0x00457e90 (result unused)
            if (jo::self<jo::BuyScreen>(this)->drag_state == 3 && jo::self<jo::BuyScreen>(this)->drag_index == idx) {
                cell_of(t, &cx, &cy);
                if (cx >= 0 && cy >= 0) {
                    if (fits(t, cx, cy) != 0 || swap_at(t, cx, cy) != 0) {
                        place(cx, cy);
                        jo::self<jo::BuyScreen>(this)->drag_state = 0;
                        jo::self<jo::BuyScreen>(this)->drag_type = 0;
                        ui_reset();
                    }
                }
                return;
            }
            if (jo::self<jo::BuyScreen>(this)->drag_state == 3) {
                cell_of(t, &cx, &cy);
                if (cx < 0 || cy < 0) {
                    jo::self<jo::BuyScreen>(this)->drag_state = 2;
                    return;
                }
                if (fits(t, cx, cy) == 0 && swap_at(t, cx, cy) == 0) {
                    jo::self<jo::BuyScreen>(this)->drag_state = 2;
                    return;
                }
                place(cx, cy);
                jo::self<jo::BuyScreen>(this)->drag_state = 2;
                jo::self<jo::BuyScreen>(this)->drag_start_ms = global<i32>(d::g_now);
                take(idx);
                return;
            }
            jo::self<jo::BuyScreen>(this)->drag_state = 1;
            jo::self<jo::BuyScreen>(this)->drag_start_ms = global<i32>(d::g_now);
            take(idx);
            return;
        }
        if (jo::self<jo::BuyScreen>(this)->drag_state != 3) return;
        cell_of(t, &cx, &cy);
        const i32 other = swap_at(t, cx, cy);
        if (cx < 0 || cy < 0) {
            jo::self<jo::BuyScreen>(this)->drag_state = 2;
            return;
        }
        if (fits(t, cx, cy) == 0 && other == 0) {
            jo::self<jo::BuyScreen>(this)->drag_state = 2;
            return;
        }
        place(cx, cy);
        if (other > 0) {
            jo::self<jo::BuyScreen>(this)->drag_state = 2;
            jo::self<jo::BuyScreen>(this)->drag_start_ms = global<i32>(d::g_now);
            take(other);
        } else {
            jo::self<jo::BuyScreen>(this)->drag_state = 0;
            jo::self<jo::BuyScreen>(this)->drag_type = 0;
            ui_reset();
        }
    }

    // Sale price: item +0x2e * unit rate +0x7d * count / stack(rec +2) * (level - +0x36 + 1).
    static i32 price(u8* t, u8* unit, i32 count, i32 stack, i32 level) {
        const Fpu fpu{true};
        double v = fpu.mul(static_cast<double>(jo::as_item_def(t)->price), static_cast<double>(jo::view<float>(jo::as_shop(unit)->stock_kind)));
        const double q = fpu.div(static_cast<double>(count), static_cast<double>(stack));
        v = fpu.mul(v, q);
        v = fpu.mul(v, static_cast<double>(static_cast<i32>(static_cast<u32>(level) - jo::view<u32>(jo::as_item_def(t)->min_level) + 1u)));
        return ftol(v);
    }
    static bool sellable(u8* t, u8* unit, bool* eligible) {
        // true when the price applies (types 0x15 / 0x17 / 0x19 at a shop of kind 0, others at kind 1)
        const i32 kind = static_cast<i32>(static_cast<i16>(jo::as_item_def(t)->type_id));
        if (kind == 0x15 || kind == 0x17 || kind == 0x19) {
            *eligible = static_cast<i32>(jo::as_shop(unit)->fixed_stock) == 0;
        } else {
            *eligible = static_cast<i32>(jo::as_shop(unit)->fixed_stock) == 1;
        }
        return *eligible;
    }

    bool mouse() {
        c_shop.hit();
        if (jo::self<jo::BuyScreen>(this)->open == 0) return false;
        if (jo::view<i8>(jo::self<jo::BuyScreen>(this)->dragging) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_popup(obj(d::g_popup_a52df8))->poll_buttons()) != 0) {  // 0x006d5120
                joan::orig::as_popup(obj(d::g_popup_a52df8))->free_gpopup();              // 0x006d3c60
                jo::self<jo::BuyScreen>(this)->dragging = 0;
                joan::orig::as_widget_child(&jo::self<jo::BuyScreen>(this)->panel_a->child)->enable();    // 0x006ca000
                joan::orig::as_widget_child(&jo::self<jo::BuyScreen>(this)->panel_b->child)->enable();  // 0x006ca000
            }
            return true;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->place))) != 0) return false;  // 0x0044cf80
        void* const shop = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->place)));     // 0x0043c450
        u8* const unit = thiscall<u8*>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(shop)[0x20 / 4]), shop);  // vtable slot 8;  // [ebp-0x14]
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))) != 0) return false;  // 0x00428b70
        (void)joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno)));  // 0x0041b3c0 ([ebp-0x18], unused)
        if (jo::self<jo::BuyScreen>(this)->drag_state != 0 && joan::orig::arg::as<i32>(joan::orig::as_parser(kUi)->value(i32{1})) != 0) {  // 0x00594700
            if (jo::self<jo::BuyScreen>(this)->drag_from_shop != 0) {
                for (i32 i = 0; i < jo::as_grid(inv())->cell_size; ++i) {
                    for (i32 j = 0; j < jo::as_grid(inv())->cols; ++j) {
                        u8* const t = item_type(drag_type());
                        if (joan::orig::arg::as<i32>(joan::orig::as_grid(inv())->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(jo::as_item_def(t)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(jo::as_item_def(t)->stats[1])))), i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(i)))), static_cast<std::int8_t>(joan::orig::arg::i32(sb(static_cast<u8>(j)))), i32{0})) != 0) {  // 0x004574a0
                            jo::self<jo::BuyScreen>(this)->drag_state = 0;
                            i16 count = jo::self<jo::BuyScreen>(this)->drag_count;  // [ebp-0x28]
                            joan::orig::as_inventory(inv())->add_stack(drag_type(), static_cast<i32>(jo::self<jo::BuyScreen>(this)->drag_level), &count, joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->u32_24d), joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->u32_251), joan::orig::arg::i32(jo::self<jo::BuyScreen>(this)->u32_255), i32{0});  // 0x004577a0
                            jo::self<jo::BuyScreen>(this)->drag_type = 0;
                            return true;
                        }
                    }
                }
            } else {
                jo::self<jo::BuyScreen>(this)->drag_state = 0;
            }
            return true;
        }
        if (jo::self<jo::BuyScreen>(this)->drag_state == 1) {
            bool hit = false;
            if (jo::self<jo::BuyScreen>(this)->drag_from_shop != 0 && r1(fn::code_value(fn::Parser_rect_mode2)) != 0) hit = true;      // 0x00594900
            if (!hit && jo::self<jo::BuyScreen>(this)->drag_from_shop == 0 && r2(fn::code_value(fn::Parser_rect_mode2)) != 0) hit = true;
            if (hit) {
                if (joan::orig::arg::as<i32>(joan::orig::as_input(kUi)->double_click(i32{0})) != 0) {  // 0x00594d30
                    jo::self<jo::BuyScreen>(this)->drag_state = 2;
                } else {
                    jo::self<jo::BuyScreen>(this)->drag_state = 3;
                    // fix/uninit: [ebp-0x2c] / [ebp-0x30] are never written when +0x245 == 0.
                    i32 cx = -1, cy = -1;
                    u8* t = nullptr;  // [ebp-0x34], read only in state 3 (then set)
                    if (jo::self<jo::BuyScreen>(this)->drag_state != 0) {
                        if (jo::self<jo::BuyScreen>(this)->drag_type != 0) {
                            t = item_type(drag_type());
                            cell_of(t, &cx, &cy);
                        } else {
                            jo::self<jo::BuyScreen>(this)->drag_state = 0;
                        }
                    } else {
                        cx = (mouse_x() - ux(0x210)) / 0x28;
                        cy = (mouse_y() - uy(0x38)) / 0x28;
                    }
                    release(t, cx, cy);
                    return true;
                }
            } else if (jo::self<jo::BuyScreen>(this)->drag_from_shop == 0 && r1(fn::code_value(fn::Parser_rect_mode2)) != 0) {
                joan::orig::as_shop(this)->buy_6c5a00();  // 0x006c5a00
            }
        }
        // 0x006c6fc8
        bool drag = false;
        if (jo::self<jo::BuyScreen>(this)->drag_state == 1 && joan::orig::arg::as<i32>(joan::orig::as_input(kUi)->button_down_state(i32{0})) == 0) {  // 0x00595aa0
            drag = true;
        } else if (jo::self<jo::BuyScreen>(this)->drag_state == 2) {
            drag = true;
        }
        if (drag) {
            bool done = false;  // -> 0x006c74e3
            if (jo::self<jo::BuyScreen>(this)->drag_from_shop == 0) {
                if ((jo::self<jo::BuyScreen>(this)->drag_state == 1 && r1(fn::code_value(fn::Parser_rect_mode2)) != 0) || (jo::self<jo::BuyScreen>(this)->drag_state == 2 && r1(fn::code_value(fn::Parser_rect_mode)) != 0)) {
                    joan::orig::as_shop(this)->buy_6c5a00();  // 0x006c5a00
                    done = true;
                }
            }
            if (!done) {
                if (jo::self<jo::BuyScreen>(this)->drag_from_shop != 0 &&
                    ((jo::self<jo::BuyScreen>(this)->drag_state == 1 && r2(fn::code_value(fn::Parser_rect_mode2)) != 0) || (jo::self<jo::BuyScreen>(this)->drag_state == 2 && r2(fn::code_value(fn::Parser_rect_mode2)) != 0) ||
                     (jo::self<jo::BuyScreen>(this)->drag_state == 2 && r2(fn::code_value(fn::Parser_rect_mode)) != 0))) {  // 0x00594900 / 0x005940f0
                    return sell_held(unit);
                }
                if (jo::self<jo::BuyScreen>(this)->drag_from_shop != 0) jo::self<jo::BuyScreen>(this)->drag_state = 2;
            }
            // 0x006c74e3
            if (jo::self<jo::BuyScreen>(this)->drag_state == 1) {
                ui_reset();
                jo::self<jo::BuyScreen>(this)->drag_state = 0;
                return true;
            }
            if (r1(fn::code_value(fn::Parser_rect_mode)) != 0 || r2(fn::code_value(fn::Parser_rect_mode)) != 0) jo::self<jo::BuyScreen>(this)->drag_state = 3;  // 0x005940f0
        }
        // 0x006c75de
        if (jo::self<jo::BuyScreen>(this)->drag_from_shop == 0 && r2(fn::code_value(fn::Joystick_in_rect)) != 0) joan::orig::as_shop(this)->drop_item_6c5f10();  // 0x00594360 / 0x006c5f10
        if (jo::self<jo::BuyScreen>(this)->drag_type == 0 && r1(fn::code_value(fn::Joystick_in_rect)) != 0) {
            const i32 cx = (mouse_x() - ux(0x210)) / 0x28;
            const i32 cy = (mouse_y() - uy(0x38)) / 0x28;
            const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_grid(inv())->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170
            if (idx != 0) {
                jo::self<jo::BuyScreen>(this)->drag_state = 0;
                if (jo::self<jo::BuyScreen>(this)->drag_state != 0 && jo::self<jo::BuyScreen>(this)->drag_from_shop != 0 && jo::self<jo::BuyScreen>(this)->drag_index != idx) {
                    ui_reset();
                    return true;
                }
                sell_slot(unit, idx);
            }
            ui_reset();
            return true;
        }
        // 0x006c79b1
        if (jo::self<jo::BuyScreen>(this)->drag_state == 0 || jo::self<jo::BuyScreen>(this)->drag_state == 3) {
            if (r2(fn::code_value(fn::Parser_rect_mode)) != 0) {
                const i32 cx = (mouse_x() - ux(0x100)) / 0x28;
                const i32 cy = (mouse_y() - uy(0x38)) / 0x28;
                const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_grid(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16))->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170
                if (idx != 0) {
                    u8* const e = inv_slot(jo::view<void*>(jo::self<jo::BuyScreen>(this)->items_b.grid_16), idx);
                    for (i32 k = 1; k <= joan::orig::arg::as<i32>(joan::orig::as_shop_facility(unit)->stock_count()); ++k) {   // 0x00469dd0
                        u8* const goods = joan::orig::arg::as<u8*>(joan::orig::as_shop_facility(unit)->stock_at(joan::orig::arg::i32(k)));        // 0x00469e10
                        if (jo::as_inv_item(e)->type != jo::object<jo::ItemRow>(goods)->item) continue;
                        if (jo::self<jo::BuyScreen>(this)->drag_state == 3 && jo::self<jo::BuyScreen>(this)->drag_index == idx) {
                            jo::self<jo::BuyScreen>(this)->drag_state = 0;
                            ui_reset();
                            continue;
                        }
                        jo::self<jo::BuyScreen>(this)->drag_state = 1;
                        jo::self<jo::BuyScreen>(this)->drag_start_ms = global<i32>(d::g_now);
                        jo::view<i32>(jo::self<jo::BuyScreen>(this)->drag_item) = idx;
                        jo::self<jo::BuyScreen>(this)->drag_index = idx;
                        jo::self<jo::BuyScreen>(this)->drag_from_shop = 0;
                        break;
                    }
                } else {
                    jo::self<jo::BuyScreen>(this)->drag_state = 0;
                }
                return true;
            }
            if (r1(fn::code_value(fn::Parser_rect_mode)) != 0) {
                i32 cx = 0, cy = 0;  // [ebp-0x138] / [ebp-0x13c] (written on both paths)
                u8* t = nullptr;     // [ebp-0x140], read only in state 3 (then set)
                if (jo::self<jo::BuyScreen>(this)->drag_state != 0) {
                    t = item_type(drag_type());
                    cell_of(t, &cx, &cy);
                } else {
                    cx = (mouse_x() - ux(0x210)) / 0x28;
                    cy = (mouse_y() - uy(0x38)) / 0x28;
                }
                release(t, cx, cy);
                return true;
            }
            jo::self<jo::BuyScreen>(this)->drag_state = 0;
        }
        // 0x006c83ba
        if (jo::self<jo::BuyScreen>(this)->drag_state == 0) {
            if (global<i32>(d::g_key) == 0x1b ||
                joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::self<jo::BuyScreen>(this)->panel_a)->mouse(i32{0xd}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
                joan::orig::as_shop(this)->apply();           // 0x006c3540
                joan::orig::as_shop(this)->set_open(i32{0});   // 0x006c8d00
                return true;
            }
        }
        if (jo::self<jo::BuyScreen>(this)->drag_state == 0) ui_reset();
        return true;
    }

    // 0x006c7253: drop the held item on the shop panel (sell it).
    bool sell_held(u8* unit) {
        alignas(4) u8 rec[sizeof(jo::WorldItem) + 2];  // [ebp-0xa0] (0x54 bytes on x86)
        joan::orig::as_world_item(rec)->construct();  // 0x00456eb0
        const i32 w2a = static_cast<i32>(jo::view<i16>(jo::as_item_def(item_type(drag_type()))->stats[3]));
        joan::orig::as_item_stack(rec)->init(joan::orig::arg::i16(drag_type()), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::BuyScreen>(this)->drag_level)), joan::orig::arg::i16(w2a), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00456cd0
        u8* const t = item_type(drag_type());  // [ebp-0xa4]
        bool eligible = false;
        if (jo::view<i8>(jo::as_item_def(t)->can_sell) != 0 && sellable(t, unit, &eligible)) {
            const i32 gold = price(t, unit, static_cast<i32>(jo::self<jo::BuyScreen>(this)->drag_count), static_cast<i32>(static_cast<i16>(jo::as_world_item(rec)->stack.count)), static_cast<i32>(jo::self<jo::BuyScreen>(this)->drag_level));
            jo::self<jo::BuyScreen>(this)->gold = static_cast<i32>(static_cast<u32>(jo::self<jo::BuyScreen>(this)->gold) + static_cast<u32>(gold));
            jo::self<jo::BuyScreen>(this)->drag_state = 0;
            ui_reset();
            joan::orig::as_world_item(rec)->destruct();  // 0x00456ef0
            return true;
        }
        jo::self<jo::BuyScreen>(this)->drag_state = 2;
        joan::orig::as_world_item(rec)->destruct();  // 0x00456ef0
        return true;
    }

    // 0x006c7793: sell the inventory item under the mouse.
    void sell_slot(u8* unit, i32 idx) {
        u8* const e = inv_slot(inv(), idx);  // [ebp-0xbc]
        alignas(4) u8 rec[sizeof(jo::WorldItem) + 2];  // [ebp-0x110] (0x54 bytes on x86)
        joan::orig::as_world_item(rec)->construct();    // 0x00456eb0
        joan::orig::as_item_stack(rec)->init(joan::orig::arg::i16(static_cast<i32>(jo::as_inv_item(e)->type)), joan::orig::arg::i16(static_cast<i32>(jo::as_inv_item(e)->level)), joan::orig::arg::i16(static_cast<i32>(jo::as_inv_item(e)->count)), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x00456cd0
        u8* const t = item_type(static_cast<i32>(jo::as_inv_item(e)->type));                                               // [ebp-0x114]
        bool eligible = false;
        if (jo::view<i8>(jo::as_item_def(t)->can_sell) != 0 && sellable(t, unit, &eligible)) {
            const i32 gold = price(t, unit, static_cast<i32>(jo::as_inv_item(e)->count), static_cast<i32>(static_cast<i16>(jo::as_world_item(rec)->stack.count)), static_cast<i32>(jo::as_inv_item(e)->level));
            jo::self<jo::BuyScreen>(this)->gold = static_cast<i32>(static_cast<u32>(jo::self<jo::BuyScreen>(this)->gold) + static_cast<u32>(gold));
            joan::orig::as_grid(inv())->fill_from(joan::orig::arg::i32(idx));  // 0x00457ec0
        }
        joan::orig::as_world_item(rec)->destruct();  // 0x00456ef0
    }
};

static_assert(sizeof(i16) == 2 && sizeof(float) == 4, "widths");

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: never-written cell locals of the 0x006c6910 release path -> -1 / -1 (no item).
FIX("Shop::mouse_6c6550", 0x006c6550, &HShop::mouse);
#undef FIX

}  // namespace game_b115_09
}  // namespace

// ---------------------------------------- from modules/game_b100_15.cpp (game/b100_15)
//
//    - Obj::clear_1a 0x0068c7d0 / Obj::clear_f 0x006d5a40 (ret 0): memset, returns this.
//
//    - BuyScreen::restart_idle 0x006c8c00 (ret 0, counter): when byte +0x10, for the models
//      +0x138 / +0x221 whose 0x6c9130(k) == 0.0 (or unordered): 0x5abe30 "F_IDLE".
//
//    - Obj6ce::ctor 0x006ce050 (ret 0, SEH frame): 0x629400 on +8 / +0xbf / +0x176, clears.
//
//    - Obj6d3::set_flag_8 0x006d3450 (ret 0): byte +8 = 1, then 0x6d34d0.
//
//    - init_a6a550 0x006d6570 (cdecl): 0x42a260 on 0xa6a550 (0xff, 0xff, 0xff, 0x40).
//
//    - Inventory::item_sound 0x006de4d0 (w), ret 4 (counter): item type (0x425ce0 on 0xa58f50)
//      8..10 "IE_SWORD", 11..12 "IE_BOW", 13..18 "IE_ARROW" through 0x4ceef0 on 0xa5cf98.
//
//  - Return values: eax is reproduced where it is a simple value (loop counters at exit, the
//    callee's eax, this); 0x6c8c00's callers ignore eax (void here).
//
//  - 0x006c8c00: 0x6c9130 returns fild qword (u32) or fld float 0.0 -> taken as double (exact).
//    `test ah,0x40` je skip -> call when equal or unordered.
namespace {
namespace game_b100_15 {


using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_restart_idle, "BuyScreen::restart_idle");
JOAN_COUNTER(c_item_sound, "Inventory::item_sound");

struct H {
    H* clear_f() {  // 0x006d5a40
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::VideoPlayer)));  // 0xf on x86 (the VideoPlayer 0xa46030)
        return this;
    }

    // ------------------------------------------------------------------ 0x006c8c00
    void restart_idle_one(jo::ShopStage& st, i32 k) {
        if (st.model == nullptr) return;
        const double t = joan::orig::arg::as<double>(joan::orig::as_shop(this)->anim_time_left(joan::orig::arg::i32(k)));  // 0x006c9130
        const double z = global<float>(d::k_9cf5c4);              // 0.0
        if (t < z || t > z) return;                              // test ah,0x40: equal or unordered
        jo::Actor* const m = st.model;
        joan::orig::as_actor(m)->start_anim(joan::orig::arg::mptr(m->anim), str(d::s_F_IDLE), joan::orig::arg::u32(global<u32>(d::g_game_time_ms)), 1u, 1u, joan::orig::arg::u32(// 0x005abe30
                      0x43fa0000u));                                                                // "F_IDLE", 500.0f
    }
    void restart_idle() {
        c_restart_idle.hit();
        if (jo::self<jo::BuyScreen>(this)->open == 0) return;
        restart_idle_one(jo::self<jo::BuyScreen>(this)->stage_player, 0);
        restart_idle_one(jo::self<jo::BuyScreen>(this)->stage_shop, 1);
    }

    // ------------------------------------------------------------------ 0x006ce050
    H* ctor_6ce050() {
        joan::orig::as_image2d(&jo::self<jo::Ending>(this)->title)->construct();      // 0x00629400
        joan::orig::as_image2d(&jo::self<jo::Ending>(this)->text)->construct();
        joan::orig::as_image2d(&jo::self<jo::Ending>(this)->frame)->construct();
        jo::self<jo::Ending>(this)->picture = 0;
        jo::view<u16>(jo::self<jo::Ending>(this)->score) = 0;
        jo::view<u8>(jo::self<jo::Ending>(this)->kind) = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x006d3450
    u32 set_flag_8() {
        jo::view<u8>(jo::self<jo::TalkPrompt>(this)->visible) = 1;
        joan::orig::as_talk_prompt(this)->load_sprite();  // 0x006d34d0
        return 0;  // void callee; TalkPrompt::show callers drop the eax
    }

    // ------------------------------------------------------------------ 0x006de4d0
    std::uintptr_t item_sound(u32 item) {  // eax: the record or 0x004ceef0's result
        c_item_sound.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(// 0x00425ce0
                                      static_cast<i32>(static_cast<i16>(item)))));
        const i32 k = static_cast<i32>(jo::view<i16>(jo::as_item_def(rec)->type_id)) - 8;
        if (static_cast<u32>(k) > 10u) return addr(rec);
        std::uintptr_t name;
        if (k <= 2)
            name = img::addr(d::s_IE_SWORD);  // "IE_SWORD"
        else if (k <= 4)
            name = img::addr(d::s_IE_BOW);  // "IE_BOW"
        else
            name = img::addr(d::s_IE_ARROW);  // "IE_ARROW"
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(name)));  // 0x004ceef0
    }
};

static void JOAN_CDECL init_a6a550() {  // 0x006d6570
    joan::orig::as_colour(obj(d::g_a6a550))->construct(0xffu, 0xffu, 0xffu, 0x40u);  // 0x0042a260
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_15", name, addr, fn)
B("BuyScreen::restart_idle", 0x006c8c00, &H::restart_idle);
B("Obj6ce::ctor", 0x006ce050, &H::ctor_6ce050);
B("Obj6d3::set_flag_8", 0x006d3450, &H::set_flag_8);
B("Obj::clear_f", 0x006d5a40, &H::clear_f);
B("init_a6a550", 0x006d6570, &init_a6a550);
B("Inventory::item_sound", 0x006de4d0, &H::item_sound);
#undef B

}  // namespace game_b100_15
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
//
//      0x006cdfa0 Shop::leave               (thiscall)
//
//      0x006d62a0 Talk::load_image          (thiscall) - "TREVOR".
//
//      0x006d6650 X::reset                  (thiscall) - 0x006d66f0 wrapper.
//
//      0x006de200 Grid::cell_at_mouse       (thiscall(w, h, int* x, int* y, char lower), ret 0x14)
namespace {
namespace game_b94_08 {


void* const kImages = obj(d::g_texture_array);

JOAN_COUNTER(c_shop_leave, "Shop::leave");
JOAN_COUNTER(c_grid, "Grid::cell_at_mouse");

u32 image_mode() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kImages)->mode()); }                // 0x0060a720
void set_image_mode(u32 m) { joan::orig::as_texture_manager(kImages)->set_mode(joan::orig::arg::u32(m)); }               // 0x0060a740
void* load_image(const char* name) { return joan::orig::as_texture_manager(kImages)->load(joan::orig::arg::str(name), 1, 0u); }  // 0x006099c0
void free_image(const void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                   // 0x0060a550

// One grid lookup of 0x006de200 (cell size 0x28, grid origin ox / oy, cols / rows).
void grid_cell(i32 w, i32 h, i32* x, i32* y, i32 ox, i32 oy, i32 cols, i32 rows) {
    const i32 hw = static_cast<i32>(static_cast<u32>(w - 1) * 0x28u) / 2;
    *x = static_cast<i32>(static_cast<u32>(global<i32>(d::g_mouse_x)) - static_cast<u32>(hw) - static_cast<u32>(ox)) / 0x28;
    const i32 hh = static_cast<i32>(static_cast<u32>(h - 1) * 0x28u) / 2;
    *y = static_cast<i32>(static_cast<u32>(global<i32>(d::g_mouse_y)) - static_cast<u32>(hh) - static_cast<u32>(oy)) / 0x28;
    if (*x < 0) {
        *x = 0;
    } else if (static_cast<i32>(static_cast<u32>(*x) + static_cast<u32>(w)) >= cols) {
        *x = static_cast<i32>(static_cast<u32>(cols) - static_cast<u32>(w));
    }
    if (*y < 0) {
        *y = 0;
    } else if (static_cast<i32>(static_cast<u32>(*y) + static_cast<u32>(h)) >= rows) {
        *y = static_cast<i32>(static_cast<u32>(rows) - static_cast<u32>(h));
    }
}

struct H {
    u8 byte_10() { return jo::view<u8>(jo::self<jo::BuyScreen>(this)->open); }  // 0x006c9040

    void shop_leave() {  // 0x006cdfa0
        c_shop_leave.hit();
        if (static_cast<i8>(jo::view<u8>(jo::self<jo::CutScene>(this)->active)) == 0) return;
        jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 6;
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(joan::orig::arg::i32(jo::self<jo::CutScene>(this)->follow_target));  // 0x00429860
        jo::view<u8>(jo::self<jo::CutScene>(this)->active) = 0;
        joan::orig::as_group(obj(d::g_names))->set_paused(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x0049b5e0
        joan::orig::as_game(obj(d::g_game_state))->set_744(static_cast<char>(joan::orig::arg::i32(0)));  // 0x004725f0
        jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 0;
        if (jo::self<jo::CutScene>(this)->bar_tex != nullptr) {
            free_image(jo::self<jo::CutScene>(this)->bar_tex);
            jo::self<jo::CutScene>(this)->bar_tex = nullptr;
        }
    }

    void load_trevor() {  // 0x006d62a0
        const u32 mode = image_mode();
        set_image_mode(1);
        // this is the VideoPlayer (0xa46030, reset_6d61d0 calls this), not the BuyScreen
        if (jo::self<jo::VideoPlayer>(this)->tex == nullptr) jo::self<jo::VideoPlayer>(this)->tex = load_image(str(d::s_TREVOR));  // "TREVOR"
        set_image_mode(mode);
    }

    void reset_6d6650() { joan::orig::as_inventory(this)->free_sprite(); }  // 0x006d66f0

    void cell_at_mouse(i32 w, i32 h, i32* x, i32* y, u32 lower) {  // 0x006de200
        c_grid.hit();
        const i32 mx = global<i32>(d::g_mouse_x), my = global<i32>(d::g_mouse_y);
        if (static_cast<i8>(lower) == 0) {
            if (mx >= 0x21a && mx <= 0x30a && my >= 0x3e && my <= 0x17e) {
                grid_cell(w, h, x, y, 0x21a, 0x3e, 6, 8);
            } else {
                *x = -1;
                *y = -1;
            }
        } else {
            if (mx >= 0x21a && mx <= 0x30a && my >= 0x1a6 && my <= 0x246) {
                grid_cell(w, h, x, y, 0x21a, 0x1a6, 6, 4);
            } else {
                *x = -1;
                *y = -1;
            }
        }
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::byte_10", 0x006c9040, &H::byte_10);
B94("Shop::leave", 0x006cdfa0, &H::shop_leave);
B94("Talk::load_image", 0x006d62a0, &H::load_trevor);
B94("X::reset_6d6650", 0x006d6650, &H::reset_6d6650);
B94("Grid::cell_at_mouse", 0x006de200, &H::cell_at_mouse);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
//
//      0x006c9060 Buy::load               (thiscall, ret 0) - "BUY", then 0x006b9420(0xa59780).
//
//      0x006de1d0 X::not_byte8            (thiscall, ret 0)
namespace {
namespace game_b98_08 {


void* const kMenus = obj(d::g_texture_array);

JOAN_COUNTER(c_buy, "Buy::load");

std::uintptr_t menu_load(const char* name) {
    return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_texture_manager(kMenus)->load(joan::orig::arg::str(name), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0
}
u32 menus_get() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kMenus)->mode()); }                // 0x0060a720
void menus_set(u32 v) { joan::orig::as_texture_manager(kMenus)->set_mode(v); } // 0x0060a740

struct H {
    u32 buy_load() {  // 0x006c9060
        c_buy.hit();
        const u32 saved = menus_get();
        menus_set(1);
        if (jo::self<jo::BuyScreen>(this)->tex == nullptr) jo::self<jo::BuyScreen>(this)->tex = reinterpret_cast<void*>(menu_load(str(d::s_BUY)));  // "BUY" (Buy::sprite: the pointer TextureManager::load returns)
        menus_set(saved);
        joan::orig::as_help_box(obj(d::g_tooltip))->load_sprite();  // 0x006b9420
        return 0;  // void callee; Shop::load_sprites's caller drops the eax
    }

    std::uintptr_t ctor_6d2330() {  // 0x006d2330; eax = this
        jo::self<jo::TalkPrompt>(this)->sheet = img::obj(d::g_bmp_res_gmsg);
        jo::self<jo::TalkPrompt>(this)->tex = nullptr;
        jo::self<jo::TalkPrompt>(this)->u32_9 = 0;
        jo::view<u8>(jo::self<jo::TalkPrompt>(this)->visible) = 1;
        return addr(this);
    }
    std::uintptr_t ctor_6d5a70() {  // 0x006d5a70; eax = this
        jo::self<jo::VideoPlayer>(this)->sheet = img::obj(d::g_a5cd60);
        jo::self<jo::VideoPlayer>(this)->tex = nullptr;
        jo::self<jo::VideoPlayer>(this)->playing = 0;
        return addr(this);
    }
    i32 not_byte8() { return jo::self<jo::TalkPrompt>(this)->visible == 0 ? 1 : 0; }  // 0x006de1d0
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Buy::load", 0x006c9060, &H::buy_load);
B98("X::ctor_6d2330", 0x006d2330, &H::ctor_6d2330);
B98("X::ctor_6d5a70", 0x006d5a70, &H::ctor_6d5a70);
B98("X::not_byte8", 0x006de1d0, &H::not_byte8);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
//
//      0x006c90e0 Res1::free_b          (thiscall, ret 0)
//
//      0x006ce390 Obj6c::reset          (thiscall, ret 0)
//
//      0x006d2380 Obj6d::reset          (thiscall, ret 0)
//
//      0x006d3b80 Obj6d::reset_b        (thiscall, ret 0)
//
//      0x006d5ab0 Obj6d::wrap           (thiscall, ret 0)
//
//      0x006d66f0 Res1::free            (thiscall, ret 0)
namespace {
namespace game_b93_06 {


void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(handle); }  // 0x0060a550

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    void free1_b() {  // 0x006c90e0
        if (jo::self<jo::BuyScreen>(this)->tex != nullptr) {
            res_free(jo::self<jo::BuyScreen>(this)->tex);
            jo::self<jo::BuyScreen>(this)->tex = nullptr;
        }
        joan::orig::as_help_box(obj(d::g_tooltip))->free_sprite();  // 0x006b9490
    }

    void reset_6ce390() {  // 0x006ce390
        jo::self<jo::Ending>(this)->is_open_ = 0;
        joan::orig::as_ending(this)->free_pictures();            // 0x006d2280
        joan::orig::as_image2d(&jo::self<jo::Ending>(this)->title)->release();    // 0x00629830
        joan::orig::as_image2d(&jo::self<jo::Ending>(this)->text)->release();
        joan::orig::as_image2d(&jo::self<jo::Ending>(this)->frame)->release();
        jo::view<u16>(jo::self<jo::Ending>(this)->score) = 0;
        jo::view<u8>(jo::self<jo::Ending>(this)->kind) = 0;
    }

    void reset_6d2380() {  // 0x006d2380
        joan::orig::as_talk_prompt(this)->free_sprite();  // 0x006d3540
        jo::view<i32>(jo::self<jo::TalkPrompt>(this)->u32_9) = 0;
    }

    void reset_6d3b80() {  // 0x006d3b80
        joan::orig::as_popup(this)->free_gpopup();           // 0x006d3c60
        joan::orig::as_menu_button(&jo::self<jo::MessageBox>(this)->button0)->clear();  // 0x006217e0 (the popup 0x00a52df8 is the message box)
        joan::orig::as_menu_button(&jo::self<jo::MessageBox>(this)->button1)->clear();
        joan::orig::as_menu_button(&jo::self<jo::MessageBox>(this)->button2)->clear();
    }

    void wrap_6d5ab0() { joan::orig::as_video_player(this)->release_image(); }  // 0x006d5ab0 -> 0x006d6310

    void free1() {  // 0x006d66f0 / 0x006e45b0 / 0x006e4dd0 / 0x006e5fc0; this one on the Inventory (Grid) screen,
        // whose +0 is InventoryScreen::tex (b_a488e0 obj:InventoryScreen).
        if (jo::self<jo::InventoryScreen>(this)->tex != nullptr) {
            res_free(jo::self<jo::InventoryScreen>(this)->tex);
            jo::self<jo::InventoryScreen>(this)->tex = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Res1::free_b", 0x006c90e0, &H::free1_b);
B93("Obj6c::reset", 0x006ce390, &H::reset_6ce390);
B93("Obj6d::reset", 0x006d2380, &H::reset_6d2380);
B93("Obj6d::reset_b", 0x006d3b80, &H::reset_6d3b80);
B93("Obj6d::wrap", 0x006d5ab0, &H::wrap_6d5ab0);
B93("Res1::free_6d66f0", 0x006d66f0, &H::free1);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
//
//      0x006c9130 Obj::time_left(which)             (thiscall, ret 4) - float st0 (64-bit exact int).
//
//      0x006d34d0 Obj::load_gmsg                    (thiscall, ret 0) - GMSG.
//
//      0x006d61d0 Obj6d61d0::reset                  (thiscall, ret 0)
//
//  - 0x006c9130: the result is either the float constant [0x9cf5c4] or `fild qword` of a
//    zero-extended unsigned dword (exact in st0, may exceed 24 bits) - returned as double
//    (st0) so no rounding happens. The divisor comes from _ftol of a float-returning
//    callee (0x0042d130); `div` by 0 traps exactly like the original.
namespace {
namespace game_b97_08 {


void* const kRes = obj(d::g_texture_array);

// 0x006099c0 resource load (3 args) through thunk 0x00404d13.
void* res_load(const char* name) { return joan::orig::as_texture_manager(kRes)->load(joan::orig::arg::str(name), 1, 0u); }

JOAN_COUNTER(c_time_left, "Obj::time_left");

struct H {
    // Shared body of 0x006c9130 for the timer at `t`.
    static double timer_left(u8* t) {
        u32 elapsed = global<u32>(d::g_game_time_ms) - jo::as_actor(t)->start_time;
        void* const anim = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::as_actor(t)->anim)->frame(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(t)->action)), 0u));  // 0x005b5dd0
        const float len = joan::orig::arg::as<float>(joan::orig::as_range(anim)->span());                                    // 0x0042d130
        const u32 period = static_cast<u32>(joan::hook::x87::ftol(len));
        if (jo::as_actor(t)->anim_flags != 0) elapsed = elapsed % period;
        if (period > elapsed) return static_cast<double>(period - elapsed);  // fild qword (exact)
        return static_cast<double>(global<float>(d::k_9cf5c4));
    }

    // ------------------------------------------------------------------ 0x006c9130
    double time_left(i32 which) {
        c_time_left.hit();
        if (which == 0) {
            if (jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_player.model) == nullptr) return static_cast<double>(global<float>(d::k_9cf5c4));
            return timer_left(jo::view<u8*>(jo::self<jo::BuyScreen>(this)->stage_player.model));
        }
        if (jo::view<void*>(jo::self<jo::BuyScreen>(this)->stage_shop.model) == nullptr) return static_cast<double>(global<float>(d::k_9cf5c4));
        return timer_left(jo::view<u8*>(jo::self<jo::BuyScreen>(this)->stage_shop.model));
    }

    // ------------------------------------------------------------------ 0x006d34d0
    void load_gmsg() {
        const i32 saved = joan::orig::arg::as<i32>(joan::orig::as_texture_manager(kRes)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kRes)->set_mode(1u);                      // 0x0060a740
        if (jo::self<jo::TalkPrompt>(this)->tex == nullptr) jo::self<jo::TalkPrompt>(this)->tex = res_load(str(d::s_GMSG));  // "GMSG"
        joan::orig::as_texture_manager(kRes)->set_mode(joan::orig::arg::u32(saved));                  // 0x0060a740
    }

    // ------------------------------------------------------------------ 0x006d61d0
    u32 reset_6d61d0() {
        joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
        jo::view<u8>(jo::self<jo::VideoPlayer>(this)->stage) = 0;
        jo::self<jo::VideoPlayer>(this)->u8_9 = 0;
        jo::self<jo::VideoPlayer>(this)->start_time = 0;
        jo::self<jo::VideoPlayer>(this)->playing = 1;
        joan::orig::as_talk(this)->load_image();  // 0x006d62a0
        return 0;  // void callee; VideoPlayer::start callers drop the eax
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::time_left", 0x006c9130, &H::time_left);
B97("Obj::load_gmsg", 0x006d34d0, &H::load_gmsg);
B97("Obj6d61d0::reset", 0x006d61d0, &H::reset_6d61d0);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
//
//    - sphere_sweep 0x006a0d80 (moving sphere vs sphere, fraction of dir), set_timer
//      0x006ad900, Flag3 set / clear 0x006ca000 / 0x006ca020, lerp 0x006cdf70;
//
//    - popup: load_gpopup 0x006d3be0, free_gpopup 0x006d3c60, set_text 0x006d5230,
//      set_title 0x006d52c0, set_first 0x006d5340, get_444 0x006d5370;
namespace {
namespace game_b90_06 {

constexpr img::Addr kTextures(d::g_texture_array);

struct H {
    // 0x006ca000 / 0x006ca020
    void* flag3_set() {
        jo::self<jo::Flag3>(this)->flag = 1;
        return this;
    }
    void* flag3_clear() {
        jo::self<jo::Flag3>(this)->flag = 0;
        return this;
    }
    // 0x006cdf70
    float lerp(float a, float b, float t) { return (b - a) * t + a; }

    // 0x006d3be0 load the "GPOPUP" texture once (texture manager mip state saved / 1 /
    // restored).
    std::uint32_t load_gpopup() {
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(img::obj(kTextures))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(1u);                          // 0x0060a740
        if (jo::self<jo::MessageBox>(this)->image == 0)
            jo::self<jo::MessageBox>(this)->image = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(&img::global<const char>(d::s_GPOPUP)), 1, 0u));  // 0x006099c0 "GPOPUP"
        joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(saved);  // 0x0060a740
        return 0;  // void callee; Popup::load_gpopup callers drop the eax
    }
    // 0x006d3c60
    void* free_gpopup() {
        if (jo::self<jo::MessageBox>(this)->image != 0) {
            joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(jo::self<jo::MessageBox>(this)->image));  // 0x0060a550
            jo::self<jo::MessageBox>(this)->image = 0;
        }
        return this;
    }
    // 0x006d5230 text (+0x44, 0x400) and its font (+0x454, default 0xa51608).
    std::uintptr_t set_text(const char* s, jo::Font2d* font) {
        std::uintptr_t r;
        if (s != nullptr)
            r = joan::orig::arg::as<std::uintptr_t>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::MessageBox>(this)->text)), joan::orig::arg::str(s), 0x3ffu));  // strncpy (Popup is the MessageBox object)
        else
            r = joan::orig::arg::as<std::uintptr_t>(std::memset(static_cast<void*>(jo::self<jo::MessageBox>(this)->text), 0, 0x400u));  // memset
        if (font != nullptr) {
            jo::self<jo::MessageBox>(this)->text_box = font;
            r = reinterpret_cast<std::uintptr_t>(this);
        } else {
            jo::self<jo::MessageBox>(this)->text_box = jo::object<jo::Font2d>(img::obj(d::g_font_goudy14_22_white));
        }
        return r;
    }
    // 0x006d52c0 title (+4, 0x40) and its font (+0x450).
    std::uintptr_t set_title(const char* s, jo::Font2d* font) {
        std::uintptr_t r;
        if (s != nullptr)
            r = joan::orig::arg::as<std::uintptr_t>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::MessageBox>(this)->title)), joan::orig::arg::str(s), 0x3fu));  // strncpy (Popup is the MessageBox object)
        else
            r = joan::orig::arg::as<std::uintptr_t>(std::memset(static_cast<void*>(jo::self<jo::MessageBox>(this)->title), 0, 0x40u));  // memset
        if (font != nullptr) {
            jo::self<jo::MessageBox>(this)->font = font;
            r = reinterpret_cast<std::uintptr_t>(this);
        } else {
            jo::self<jo::MessageBox>(this)->font = jo::object<jo::Font2d>(img::obj(d::g_font_goudy14_22_white));
        }
        return r;
    }
    // 0x006d5340
    void* set_first(std::uint32_t v) {
        jo::self<jo::MessageBox>(this)->state = v;  // the button flags (Popup is the MessageBox object)
        return this;
    }
    // 0x006d5370
    std::uint32_t get_444() { return jo::self<jo::MessageBox>(this)->u32_444; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Flag3::set", 0x006ca000, &H::flag3_set);
B("Flag3::clear", 0x006ca020, &H::flag3_clear);
B("lerp", 0x006cdf70, &H::lerp);
B("Popup::load_gpopup", 0x006d3be0, &H::load_gpopup);
B("Popup::free_gpopup", 0x006d3c60, &H::free_gpopup);
B("Popup::set_text", 0x006d5230, &H::set_text);
B("Popup::set_title", 0x006d52c0, &H::set_title);
B("Obj::set_first", 0x006d5340, &H::set_first);
B("Obj::get_444", 0x006d5370, &H::get_444);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
//
//      0x006ca0d0 release (+0x14 object, +0x3a texture)
namespace {
namespace game_b84_04 {


void* const kTextures = obj(d::g_texture_array);

struct H {
    void reset_6d3480() {
        jo::view<std::uint8_t>(jo::self<jo::TalkPrompt>(this)->visible) = 0;
        joan::orig::as_talk_prompt(this)->free_sprite();  // 0x006d3540
    }
    void reset_6ca090() {
        joan::orig::as_cutscene(this)->load_camera();  // 0x006cabd0
        jo::self<jo::CutScene>(this)->hud_a = 1;
        jo::self<jo::CutScene>(this)->hud_c = 1;
        jo::self<jo::CutScene>(this)->hud_b = 1;
    }
    void reset_6d6230() {
        if (global<std::uint32_t>(d::g_video_file) != 0) {
            joan::orig::as_video(obj(d::g_video_file))->close();  // 0x00686160
            global<std::uint32_t>(d::g_movie_start_ms) = 0;
        }
        jo::self<jo::VideoPlayer>(this)->playing = 0;
        joan::orig::as_video_player(this)->release_image();  // 0x006d6310
    }
    void release_6ca0d0() {
        if (jo::view<void*>(jo::self<jo::CutScene>(this)->zones) != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::CutScene>(this)->zones);
            if (o != nullptr) joan::orig::as_camera_zones(o)->destroy(1u);  // 0x00420150 scalar deleting dtor
            jo::view<void*>(jo::self<jo::CutScene>(this)->zones) = nullptr;
        }
        if (jo::self<jo::CutScene>(this)->bar_tex != nullptr) {
            joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(jo::self<jo::CutScene>(this)->bar_tex));  // 0x0060a550
            jo::self<jo::CutScene>(this)->bar_tex = nullptr;
        }
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Obj6ca0d0::release", 0x006ca0d0, &H::release_6ca0d0);
M("Obj6d6230::reset", 0x006d6230, &H::reset_6d6230);
M("Obj6ca090::reset", 0x006ca090, &H::reset_6ca090);
M("Obj6d3480::reset", 0x006d3480, &H::reset_6d3480);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
//
//      0x006ca170 Obj6ca::reset()
//
//      0x006cab90 Obj6ca::set(a, b) (ret 8)
namespace {
namespace game_b91_04 {


struct H {
    void* self() { return this; }

    // 0x006ca170
    void* reset_6ca170() {
        jo::self<jo::CutScene>(this)->target = 0;
        jo::view<std::uint8_t>(jo::self<jo::CutScene>(this)->active) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->duration) = 0;
        jo::self<jo::CutScene>(this)->elapsed = 0;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->state) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->cam_pos[0]) = global<std::uint32_t>(d::g_pos);
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->cam_pos[1]) = global<std::uint32_t>(d::g_a69cdc);
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->cam_pos[2]) = global<std::uint32_t>(d::g_a69ce0);
        jo::self<jo::CutScene>(this)->cam_dist = 0;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->cam_pitch) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->cam_yaw) = 0;
        jo::self<jo::CutScene>(this)->hud_a = 1;
        jo::self<jo::CutScene>(this)->hud_c = 1;
        jo::self<jo::CutScene>(this)->hud_b = 1;
        return self();
    }
    // 0x006cab90
    void* set_6cab90(std::uint32_t a, std::uint32_t b) {
        jo::view<std::uint8_t>(jo::self<jo::CutScene>(this)->active) = 1;
        jo::self<jo::CutScene>(this)->target = a;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->duration) = b;
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj6ca::reset", 0x006ca170, &H::reset_6ca170);
B91("Obj6ca::set", 0x006cab90, &H::set_6cab90);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_ui7.cpp (game/ui7)
//
//      - Cinematic::draw_bars 0x006ca230: video frame / fades (states +0xf 6 / 7 / 8) through
//        0x006332b0 / 0x00632a70 on the sprite queue 0x00a46128, then the letterbox bars of
//        height ftol(W * 55 / 640 * +0xb / 255). ret 0.
namespace {
namespace game_ui7 {


constexpr img::Addr kSprites = d::g_sprites_a46128;   // 2D sprite queue
constexpr img::Addr kScreenW = d::g_screen_w, kScreenH = d::g_screen_h;

JOAN_COUNTER(c_bars, "Cinematic::draw_bars");

// 0x00632a70 sprite queue (16 args, ret 0x40): x1, y1, x2, y2 (words), layer, colour,
// texture, mode (word), 3 words, sx1, sy1, sx2, sy2, rot (words).
std::int32_t queue(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t layer, std::uint32_t colour,
                   void* texture, std::int32_t mode, std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t sx1,
                   std::int32_t sy1, std::int32_t sx2, std::int32_t sy2, std::int32_t rot) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_sprite2d(obj(kSprites))->queue(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(layer), joan::orig::arg::u32(colour), joan::orig::arg::mptr(texture), joan::orig::arg::i32(mode), joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(sx1), joan::orig::arg::i32(sy1), joan::orig::arg::i32(sx2), joan::orig::arg::i32(sy2), joan::orig::arg::i32(rot)));  // 0x00632a70
}
// Untextured fill (texture 0, mode 3, no source rectangle).
void fill(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour) {
    queue(x1, y1, x2, y2, 0xa, colour, nullptr, 3, 0, 0, 0, -1, -1, -1, -1, 0);
}

struct H {
    // 0x006ca230 Cinematic::draw_bars. ret 0.
    void draw_bars() {
        std::uint8_t alpha = static_cast<std::uint8_t>(joan::hook::x87::ftol(static_cast<double>(jo::view<float>(jo::self<jo::CutScene>(this)->elapsed))));
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::CutScene>(this)->active)) == 0) return;
        c_bars.hit();
        auto colour = [&] { return static_cast<std::uint32_t>(alpha) << 24; };
        const std::int32_t state = jo::self<jo::CutScene>(this)->state;
        // The wipe (states 6 / 8): the gradient texture is a 1024-wide screen's - drawn W / 1024 times as wide on
        // a larger screen, its edge b (+0x25 bar_size, the cutscene's state: -width .. W) mapped onto
        // -scaled width .. W, so it still starts and ends off the screen (drawing only, not part of the
        // original game; the timing stays the game's).
        const std::int32_t screen_w = global<std::int32_t>(kScreenW);
        std::int32_t tw = 0, tw2 = 0, b2 = 0;
        if (state == 6 || state == 8) {
            tw = jo::view<std::int32_t>(jo::self<jo::CutScene>(this)->bar_tex->width);
            const float k = std::max(1.0f, static_cast<float>(screen_w) / 1024.0f);
            tw2 = static_cast<std::int32_t>(std::lround(static_cast<float>(tw) * k));
            const std::int32_t b = jo::view<std::int16_t>(jo::self<jo::CutScene>(this)->bar_size);
            b2 = -tw2 + static_cast<std::int32_t>(std::lround(static_cast<double>(b + tw) * (screen_w + tw2) / std::max(1, screen_w + tw)));
        }
        const joan::display::SelfScaled self_scaled;  // sized for the screen here: not "unscaled 2D"
        if (state == 6) {
            void* const tex = jo::view<void*>(jo::self<jo::CutScene>(this)->bar_tex);
            const std::int32_t rot = static_cast<std::int8_t>(jo::self<jo::CutScene>(this)->bar_on) != 0 ? 2 : 0;
            joan::orig::as_sprite_queue(obj(kSprites))->blit_image(joan::orig::arg::i16(static_cast<std::int16_t>(b2)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), 0xau, joan::orig::arg::mptr(tex), joan::orig::arg::u32(colour()), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(jo::view<std::int32_t>(jo::as_texture(tex)->width) - 1)), joan::orig::arg::i16(static_cast<std::int16_t>(jo::view<std::int32_t>(jo::as_texture(tex)->height) - 1)), joan::orig::arg::i16(static_cast<std::int16_t>(rot)), joan::orig::arg::i16(static_cast<std::int16_t>(b2 + tw2 - 1)), joan::orig::arg::i16(static_cast<std::int16_t>(global<std::int32_t>(kScreenH) - 1)));  // 0x006332b0
            const std::int32_t right = b2 + tw2;
            if (right < screen_w) {
                alpha = 0xff;
                queue(right, 0, screen_w - 1, global<std::int32_t>(kScreenH) - 1, 0xa, colour(), nullptr, 3, 0, 0, 0, -1, -1, -1, -1, 0);
            }
            return;
        }
        if (state == 7) {
            alpha = 0xff;
            fill(0, 0, global<std::int32_t>(kScreenW) - 1, global<std::int32_t>(kScreenH) - 1, colour());
        } else if (state == 8) {
            void* const tex = jo::view<void*>(jo::self<jo::CutScene>(this)->bar_tex);
            const std::int32_t rot = static_cast<std::int8_t>(jo::self<jo::CutScene>(this)->bar_on) != 0 ? 2 : 0;
            joan::orig::as_sprite_queue(obj(kSprites))->blit_image(joan::orig::arg::i16(static_cast<std::int16_t>(b2)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), 0xau, joan::orig::arg::mptr(tex), joan::orig::arg::u32(colour()), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(jo::view<std::int32_t>(jo::as_texture(tex)->width) - 1)), joan::orig::arg::i16(static_cast<std::int16_t>(jo::view<std::int32_t>(jo::as_texture(tex)->height) - 1)), joan::orig::arg::i16(static_cast<std::int16_t>(rot)), joan::orig::arg::i16(static_cast<std::int16_t>(b2 + tw2 - 1)), joan::orig::arg::i16(static_cast<std::int16_t>(global<std::int32_t>(kScreenH) - 1)));  // 0x006332b0
            if (b2 > 0) {
                alpha = 0xff;
                fill(0, 0, b2 - 1, global<std::int32_t>(kScreenH) - 1, colour());
            }
        }
        // Letterbox bars: ftol(fild(W * 0x37 / 0x280) * +0xb / 255.0f) at PC24.
        const std::int32_t base = global<std::int32_t>(kScreenW) * 0x37 / 0x280;
        const joan::hook::x87::Fpu fpu{true};
        const double v = fpu.div(fpu.mul(static_cast<double>(base), static_cast<double>(jo::view<float>(jo::self<jo::CutScene>(this)->elapsed))),
                                 static_cast<double>(global<float>(d::flt_255)));  // 255.0f
        const std::int32_t bar = joan::hook::x87::ftol(v);
        if (bar <= 1) return;
        fill(0, 0, global<std::int32_t>(kScreenW) - 1, bar - 1, colour());
        fill(0, global<std::int32_t>(kScreenH) - bar, global<std::int32_t>(kScreenW) - 1, global<std::int32_t>(kScreenH) - 1, colour());
    }
};

#define U7(name, addr, fn) JOAN_REPLACE("game/ui7", name, addr, fn)
U7("Cinematic::draw_bars", 0x006ca230, &H::draw_bars);
#undef U7

}  // namespace game_ui7
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
//
//      0x006ca5c0 Menu::pass_through                (thiscall, ret 0) - al.
namespace {
namespace game_b108_18 {


struct H {
    // ------------------------------------------------------------------ 0x006ca5c0
    bool menu_pass_through() {
        if (global<i32>(d::g_key) != 0x1b) return false;
        if (static_cast<i8>(jo::view<u8>(jo::self<jo::CutScene>(this)->active)) == 0) return false;
        if ((static_cast<i8>(jo::view<u8>(jo::self<jo::CutScene>(this)->mode)) == 1 || static_cast<i8>(jo::view<u8>(jo::self<jo::CutScene>(this)->mode_prev)) == 1) && jo::self<jo::CutScene>(this)->state != 2) return true;
        joan::orig::as_cutscene(this)->start_6cc6a0(joan::orig::arg::u32(global<u32>(d::g_now)), joan::orig::arg::bits(-128.0f), 0u, joan::orig::arg::u32(static_cast<i8>(jo::view<u8>(jo::self<jo::CutScene>(this)->mode_prev))));  // 0x006cc6a0
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Menu::pass_through", 0x006ca5c0, &H::menu_pass_through);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
//
//    - Cutscene::load_camera 0x006cabd0 (no args): once-only new 0x4c object (0x00425dc0) at
//      +0x14, 0x00425e20, then RESOURCE\cutscene.cm through 0x00426330 if the file exists.
//
//    - Panel::release_textures4 0x006e8e40, Panel::release_textures2 0x006e7c60 (+0xd18,
//      +0xd1c), TexHandle::release 0x006d3540 / 0x006e0440 / 0x006e0b20 (i_letter.cpp),
//      TexHandle::release_and_reset 0x006d6310 (+ 0x0059b150 on 0x00a5d0d0).
//
//  * 0x006759a0 / 0x006cabd0: SEH frames only guard operator new + ctor; the cutscene path
//    string (0x005a1170) has no destructor in the original (none called).
namespace {
namespace game_b85_05 {

struct alignas(4) Path {  // 0x005a1170 string, no destructor
    std::uint8_t b[0x194];
};

JOAN_COUNTER(c_cutscene, "Cutscene::load_camera");

void* const kTextures = obj(d::g_texture_array);

using joan::orig::helpers::game_new;  // operator new
void release_texture(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }         // 0x0060a550

struct H {
    // ------------------------------------------------------------------ 0x006cabd0
    void cutscene_load() {
        c_cutscene.hit();
        if (jo::view<void*>(jo::self<jo::CutScene>(this)->zones) != nullptr) return;
        void* p = game_new(static_cast<std::uint32_t>(sizeof(jo::CamZones)));  // 0x4c on x86
        void* cam = p != nullptr ? joan::orig::as_camera_zones(p)->construct() : nullptr;  // 0x00425dc0
        jo::view<void*>(jo::self<jo::CutScene>(this)->zones) = cam;
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->init();  // 0x00425e20
        Path path;
        joan::orig::as_string(&path)->construct();           // 0x005a1170
        joan::orig::as_string(&path)->assign(str(d::s_RESOURCE));            // 0x005a1440 "RESOURCE\\"
        joan::orig::as_string(&path)->append(str(d::s_cutscene_cm));                // 0x005a1550 "cutscene.cm"
        const char* name = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(name)) == 0) return;  // 0x00590b80
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->zones->origin[0]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->zones->origin[1]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::CutScene>(this)->zones->origin[2]) = 0;
        const char* name2 = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->load(name2);    // 0x00426330
    }

    // ------------------------------------------------------------------ 0x006d3540 / 0x006e0440 / 0x006e0b20
    void release_handle() {
        if (jo::arg::as<void*>(jo::self<jo::TalkPrompt>(this)->tex) != nullptr) {
            release_texture(jo::arg::as<void*>(jo::self<jo::TalkPrompt>(this)->tex));
            jo::self<jo::TalkPrompt>(this)->tex = nullptr;
        }
    }

    // ------------------------------------------------------------------ 0x006d6310
    void release_and_reset() {
        if (jo::arg::as<void*>(jo::self<jo::VideoPlayer>(this)->tex) != nullptr) {
            release_texture(jo::arg::as<void*>(jo::self<jo::VideoPlayer>(this)->tex));
            jo::self<jo::VideoPlayer>(this)->tex = nullptr;
        }
        joan::orig::as_sound(obj(d::g_snd))->set_volume_59b150(static_cast<std::int32_t>(global<std::int16_t>(d::g_music_volume)));  // 0x0059b150
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Cutscene::load_camera", 0x006cabd0, &H::cutscene_load);
B("TexHandle::release_and_reset", 0x006d6310, &H::release_and_reset);
B("TexHandle::release_6d3540", 0x006d3540, &H::release_handle);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b106_06.cpp (game/b106_06)
//
//      0x006cad50 CutScene::start         (thiscall, ret 0x2c) - saves / clears HUD flags, sets
//                 up the "cut_scene_move" / "cut_scene_play" camera paths, starts the scene.
//                 Returns al only (its single caller 0x0065aeba does movsx eax, al).
//
//  - 0x006cad50: the two trailing writes to the dead stack copy of the camera (after 0x00429d00)
//    have no observable effect and are omitted. `fld/fstp` copies of a float that may be NaN
//    quiet a signalling NaN; reproduced with fld_fstp(). The fdiv / fmul by the deg->rad
//    constant are emulated with the x87 register's extended exponent (round24_wide), so a
//    quotient / product below FLT_MIN is rounded twice (st0, then fstp) and compared unrounded
//    exactly like the original. Not reproduced: fadd of two NaNs with different payloads (the
//    x87 keeps the larger significand, SSE the first operand) - camera coordinates only.
namespace {
namespace game_b106_06 {

JOAN_COUNTER(c_cut_start, "CutScene::start");

// fld dword / fstp dword: a signalling NaN comes out quiet.
u32 fld_fstp(u32 bits) {
    if ((bits & 0x7f800000u) == 0x7f800000u && (bits & 0x007fffffu) != 0) bits |= 0x00400000u;
    return bits;
}
using joan::orig::helpers::as_float;
u32 as_bits(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}

// x87 op at precision control 24: 24-bit significand but the extended exponent range, so a
// result below FLT_MIN is not denormalised in the register (the later fstp m32 rounds it a
// second time). hi + lo is the exact result; the 2^64 scaling is exact (as game_b99_10.cpp).
double round24_wide(double hi, double lo) {
    if (hi == 0.0 || !std::isfinite(hi)) return joan::hook::x87::round_to_float(hi, lo);
    double scale = 1.0;
    while (std::fabs(hi) > 1e30) {
        hi *= 0x1p-64;
        lo *= 0x1p-64;
        scale *= 0x1p64;
    }
    while (std::fabs(hi) < 1e-30) {
        hi *= 0x1p64;
        lo *= 0x1p64;
        scale *= 0x1p-64;
    }
    return joan::hook::x87::round_to_float(hi, lo) * scale;
}
// fld m32 / fdiv m32: st0 value.
double fdiv_reg(float a, float b) {
    const double da = a, db = b;
    const double q = da / db;
    const double r = joan::fp::fma_exact(-q, db, da);
    return round24_wide(q, db != 0.0 ? r / db : 0.0);
}
// fld m32 / fmul m32: st0 value (the double product of two floats is exact).
double fmul_reg(float a, float b) { return round24_wide(static_cast<double>(a) * static_cast<double>(b), 0.0); }
// fstp m32 of an st0 value (round to nearest, denormals kept).
u32 fstp_bits(double v) { return as_bits(static_cast<float>(v)); }

struct H {
    // 0x006cad50
    u8 cut_start(u32 a8, u32 ac, i8 skip_setup, i8 play, u32 px, u32 py, u32 pz, u32 v24, u32 v28, u32 v2c,
                 i8 mode) {
        c_cut_start.hit();
        if (jo::self<jo::CutScene>(this)->active != 0) return 0;
        jo::self<jo::CutScene>(this)->started = 1;
        jo::self<jo::CutScene>(this)->u8_40 = 0;
        jo::self<jo::CutScene>(this)->hud_a = global<u8>(d::g_camera_lock_flag_a48ce3);
        jo::self<jo::CutScene>(this)->hud_c = global<u8>(d::g_camera_lock_flag_a48ce7);
        jo::self<jo::CutScene>(this)->hud_b = global<u8>(d::g_camera_lock_flag_a48ceb);
        global<u32>(d::g_camera_lock_flag_a48ce3) = 0;
        global<u32>(d::g_camera_lock_flag_a48ce7) = 0;
        global<u32>(d::g_camera_lock_flag_a48ceb) = 0;
        jo::self<jo::CutScene>(this)->scene_flag = joan::orig::arg::as<u32>(joan::orig::as_scene(jo::as_world(global<void*>(d::g_world))->scene)->flag_6c());  // 0x004d2a00
        joan::orig::as_scene(jo::as_world(global<void*>(d::g_world))->scene)->set_6c(0u);                     // 0x005ef400
        void* const units = obj(d::g_units);
        for (i32 i = joan::orig::as_dynarray(units)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<u32>(joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00428b70
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3c0
            if (jo::as_unit(u)->draw != nullptr) joan::orig::as_unit_draw(jo::as_unit(u)->draw)->draw();  // 0x0043b890
        }
        {
            const i32 s = global<i32>(d::g_control_mode);
            if (s == 0 || s == 3)
                joan::orig::as_player(obj(d::g_player_unit_recno))->switch_unit(1);  // 0x0042b6e0
            else if (s == 1)
                joan::orig::as_player(obj(d::g_player_unit_recno))->switch_unit(0);
        }
        jo::self<jo::CutScene>(this)->follow_target = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(obj(d::g_camera_man))->target());  // 0x00563dc0
        if (skip_setup != 0) {
            jo::self<jo::CutScene>(this)->u8_13 = 0;
            jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 2;
        } else {
            u32 cam[10];  // [ebp-0x34]
            joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x006140a0
            jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]) = px;
            jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]) = py;
            jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[2]) = pz;
            jo::self<jo::CutScene>(this)->cam_dist = v24;
            jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pitch) = v28;
            jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw) = v2c;
            u32 cam2[10];  // [ebp-0x5c]
            for (int k = 0; k < 10; ++k) cam2[k] = cam[k];
            cam2[1] = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]);
            cam2[2] = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]);
            cam2[3] = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[2]);
            cam2[7] = jo::self<jo::CutScene>(this)->cam_dist;
            const float k_deg = global<float>(d::flt_0_017453292);
            cam2[4] = fstp_bits(fmul_reg(jo::self<jo::CutScene>(this)->cam_pitch, k_deg));
            cam2[5] = fstp_bits(fmul_reg(jo::self<jo::CutScene>(this)->cam_yaw, k_deg));
            void* const cams = obj(d::g_camera_man);
            if (play == 0) {
                jo::self<jo::CutScene>(this)->u8_13 = 0;
                jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 2;
                void* c = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(cams)->motion(5));  // 0x00563c80
                joan::orig::as_camera_motion(c)->start(joan::orig::arg::mptr(static_cast<void*>(cam2)), joan::orig::arg::mptr(static_cast<void*>(cam2)));  // 0x00423ec0
                joan::orig::as_camera_man(cams)->set_mode(5);  // 0x00429860
            } else {
                setup_paths(cam, cam2);
                void* c = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(cams)->motion(5));  // 0x00563c80
                joan::orig::as_camera_motion(c)->start(joan::orig::arg::mptr(static_cast<void*>(cam2)), joan::orig::arg::mptr(static_cast<void*>(cam2)));  // 0x00423ec0
                joan::orig::as_camera_man(cams)->set_mode(5);  // 0x00429860
                joan::orig::as_camera_man(cams)->set_mode(2);  // 0x00429860
                joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->start_track(str(d::s_cut_scene_play), joan::orig::arg::u32(global<u32>(d::g_now)));  // 0x00428850
                jo::self<jo::CutScene>(this)->u8_13 = 1;
                jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 1;
            }
        }
        joan::orig::as_cutscene(this)->begin(joan::orig::arg::u32(a8), joan::orig::arg::u32(ac));           // 0x006cab90
        joan::orig::as_game(obj(d::g_game_state))->set_744(static_cast<char>(joan::orig::arg::i32(u8{1})));  // 0x004725f0
        joan::orig::as_group(obj(d::g_names))->set_paused(static_cast<std::int8_t>(joan::orig::arg::i32(u8{1})));  // 0x0049b5e0
        jo::self<jo::CutScene>(this)->mode = mode;
        jo::self<jo::CutScene>(this)->mode_prev = mode;
        if (mode == 1) {
            jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 6;
            jo::self<jo::CutScene>(this)->bar_size = global<u16>(d::g_screen_w);
            jo::self<jo::CutScene>(this)->elapsed = 0;
            jo::self<jo::CutScene>(this)->bar_on = 0;
            if (jo::self<jo::CutScene>(this)->bar_tex == nullptr) {
                void* const tm = obj(d::g_texture_array);
                const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(tm)->mode());  // 0x0060a720
                joan::orig::as_texture_manager(tm)->set_mode(1u);                 // 0x0060a740
                jo::self<jo::CutScene>(this)->bar_tex = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(tm)->load(str(d::s_cut_scene_texture), 1, 0u));  // 0x006099c0
                joan::orig::as_texture_manager(tm)->set_mode(joan::orig::arg::u32(saved));
            }
        }
        if (global<i32>(d::g_control_mode) == 4) {
            const u8 flag = 1;
            for (i32 i = 1; i <= joan::orig::as_dynarray(units)->size(); ++i) {  // 0x00417ad0
                if (joan::orig::arg::as<u32>(joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(i))) != 0) continue;  // 0x00428b70
                void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3c0
                if (jo::as_unit(u)->actor != nullptr) joan::orig::as_model_instance(jo::as_unit(u)->actor)->set_10a(joan::orig::arg::u32(flag));  // 0x0042cf20
                for (i16* carried = &jo::as_unit(u)->carried_a; carried <= &jo::as_unit(u)->carried_b; ++carried) {  // carried_a, carried_b
                    if (joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_items))->list.empty_at(static_cast<i32>(*carried))) == 0) {  // 0x00425d50
                        void* q = joan::orig::as_units(obj(d::g_items))->list.ptr_at(static_cast<i32>(*carried));  // 0x00425d20
                        if (jo::as_map_object(q)->actor != nullptr) joan::orig::as_model_instance(jo::as_map_object(q)->actor)->set_10a(joan::orig::arg::u32(flag));
                    }
                }
                if (jo::as_unit(u)->trail_a != nullptr && jo::as_unit(u)->trail_a->actor != nullptr)
                    joan::orig::as_model_instance(jo::as_unit(u)->trail_a->actor)->set_10a(joan::orig::arg::u32(flag));
                if (jo::as_unit(u)->trail_b != nullptr && jo::as_unit(u)->trail_b->actor != nullptr)
                    joan::orig::as_model_instance(jo::as_unit(u)->trail_b->actor)->set_10a(joan::orig::arg::u32(flag));
                joan::orig::as_army(obj(d::g_player_unit_recno))->apply(joan::orig::arg::u32(u8{0}));  // 0x00452be0
            }
        }
        void* u = joan::orig::as_dynarray(units)->ptr_at(static_cast<i32>(global<i16>(d::g_a47c36)));  // 0x0041b3c0
        joan::orig::as_table(obj(d::g_orders))->drop_side(joan::orig::arg::i32(jo::view<u16>(jo::as_unit(u)->side)));  // 0x004ead00
        return 1;
    }

    // Part of 0x006cad50 (play branch): fill the "cut_scene_play" path from "cut_scene_move".
    // `this` is the cut scene; cam = [ebp-0x34], cam2 = [ebp-0x5c].
    void setup_paths(const u32* cam, const u32* cam2) {
        auto paths = [this]() -> void* { return jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks); };
        const i32 move = joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(paths())->find(str(d::s_cut_scene_move)));  // 0x00429c70 "cut_scene_move"
        const i32 play = joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(paths())->find(str(d::s_cut_scene_play)));  // 0x00429c70 "cut_scene_play"
        u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_tracks(paths())->get(joan::orig::arg::i16(static_cast<i16>(play))));  // 0x00428b30
        u8* const b = joan::orig::arg::as<u8*>(joan::orig::as_tracks(paths())->get(joan::orig::arg::i16(static_cast<i16>(move))));  // 0x00428b30
        const float k = global<float>(d::flt_0_017453292);
        const float k4 = global<float>(d::flt_4);  // 4.0
        auto A = [a](i32 i) -> jo::CamKey& { return jo::as_cam_track(a)->keys[i]; };
        auto B = [b](i32 i) -> jo::CamKey& { return jo::as_cam_track(b)->keys[i]; };
        auto f = [](u32 bits) { return as_float(bits); };
        auto div_k = [k](u32 bits) { return fdiv_reg(as_float(bits), k); };  // st0 of fld / fdiv k
        i32 i;
        for (i = 0; i < 5; ++i) {
            u32_of(A(i).time) = u32_of(B(i).time);
            u32_of(A(i).point[0]) = cam[1];
            u32_of(A(i).point[1]) = cam[2];
            u32_of(A(i).point[2]) = as_bits(f(cam[3]) + f(u32_of(B(i).point[2])));
            u32_of(A(i).yaw) = fstp_bits(div_k(cam[4]));
            {
                const double q = div_k(cam[5]);  // compared unrounded (fcompp on st0)
                const double bv = f(u32_of(B(i).pitch));
                u32_of(A(i).pitch) = !(q >= bv) ? fld_fstp(u32_of(B(i).pitch)) : fstp_bits(div_k(cam[5]));
            }
            u32_of(A(i).roll) = fstp_bits(div_k(cam[6]));
            u32_of(A(i).zoom) = f(u32_of(B(i).zoom)) > f(cam[7]) ? fld_fstp(u32_of(B(i).zoom)) : cam[7];
        }
        for (i = 5; i < 10; ++i) {
            u32_of(A(i).time) = u32_of(B(i).time);
            {
                const float t = static_cast<float>(i - 5) / k4;
                u32_of(A(i).point[0]) = as_bits(joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(cam[1]), joan::orig::arg::f32(jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0])), joan::orig::arg::f32(t))));  // 0x006cdf70
            }
            {
                const float t = static_cast<float>(i - 5) / k4;
                u32_of(A(i).point[1]) = as_bits(joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(cam[2]), joan::orig::arg::f32(jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1])), joan::orig::arg::f32(t))));  // 0x006cdf70
            }
            u32_of(A(i).point[2]) = as_bits(f(cam[3]) + f(u32_of(B(i).point[2])));
            u32_of(A(i).yaw) = fstp_bits(div_k(cam[4]));
            u32_of(A(i).pitch) = u32_of(B(i).pitch);
            u32_of(A(i).roll) = fstp_bits(div_k(cam[6]));
            u32_of(A(i).zoom) = u32_of(B(i).zoom);
        }
        for (i = 10; i < jo::as_cam_track(a)->key_count; ++i) {
            u32_of(A(i).time) = u32_of(B(i).time);
            u32_of(A(i).point[0]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]);
            u32_of(A(i).point[1]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]);
            u32_of(A(i).point[2]) = as_bits(f(u32_of(B(i).point[2])) + jo::self<jo::CutScene>(this)->cam_pos[2]);
            {
                const float t = static_cast<float>(i - 10) / k4;
                const float from = as_float(fstp_bits(div_k(cam[4])));  // fstp [esp]
                u32_of(A(i).yaw) = as_bits(joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(from), joan::orig::arg::f32(jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pitch)), joan::orig::arg::f32(t))));  // 0x006cdf70
            }
            u32_of(A(i).pitch) = u32_of(B(i).pitch);
            u32_of(A(i).roll) = fstp_bits(div_k(cam[6]));
            u32_of(A(i).zoom) = f(u32_of(B(i).zoom)) > f(cam2[7]) ? fld_fstp(u32_of(B(i).zoom)) : cam2[7];
        }
        u32_of(jo::as_cam_track(a)->keys[0].zoom) = cam[7];
        u32_of(jo::as_cam_track(a)->keys[0].pitch) = fstp_bits(div_k(cam[5]));
        u32_of(jo::as_cam_track(a)->keys[jo::as_cam_track(a)->key_count - 1].zoom) = jo::self<jo::CutScene>(this)->cam_dist;
        u32_of(jo::as_cam_track(a)->keys[jo::as_cam_track(a)->key_count - 1].pitch) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw);
        joan::orig::as_cam_tracks(paths())->prepare(joan::orig::arg::i32(play));  // 0x00429d00
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_06", name, addr, fn)
B("CutScene::start", 0x006cad50, &H::cut_start);
#undef B

}  // namespace game_b106_06
}  // namespace

// ---------------------------------------- from modules/game_b106_08.cpp (game/b106_08)
//
//    0x006cb9e0 CutScene::start(...)          (thiscall, ret 0x24, returns al) - camera path
//               set-up for "cut_scene_move" / "cut_scene_play" (10 + n key records of 0x2c
//               bytes from 0x76), screen texture "cut_scene_texture", unit fade flags.
//
//  - 0x006cb9e0: the two stores into the camera copy at the end of the "play" path
//    ([ebp-0x20] / [ebp-0x18]) are dead (that copy is not used afterwards) and are dropped.
//    Memory-to-memory float copies done with fld / fstp quiet a signalling NaN; fl() keeps
//    that.
namespace {
namespace game_b106_08 {

// fld dword + fstp dword: bit copy, except that a signalling NaN comes out quiet.
float fl(float v) {
    u32 b;
    std::memcpy(&b, &v, 4);
    if ((b & 0x7f800000u) == 0x7f800000u && (b & 0x007fffffu) != 0) b |= 0x00400000u;
    float r;
    std::memcpy(&r, &b, 4);
    return r;
}

JOAN_COUNTER(c_cut_start, "CutScene::start_6cb9e0");

// ---------------------------------------------------------------------------
// 0x006cb9e0 helpers
// ---------------------------------------------------------------------------
constexpr float kDeg = 0.01745329238474369f;  // 0x009cf884
void* anim_owner(void* self) { return jo::as_cut_scene(self)->zones->tracks; }
u32 bits(const float& v) {
    u32 b;
    std::memcpy(&b, &v, 4);
    return b;
}
// 0x006cdf70 (a, b, t) = (b - a) * t + a; the first two pushed as dwords in the 5..9 loop.
float lerp_d(void* self, u32 a, u32 b, float t) { return joan::orig::arg::as<float>(joan::orig::as_cutscene(self)->lerp(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }
float lerp(void* self, float a, float b, float t) { return joan::orig::arg::as<float>(joan::orig::as_cutscene(self)->lerp(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }
void set_fade(void* o) { joan::orig::as_model_instance(o)->set_10a(1u); }  // 0x0042cf20(1)

struct H {
    // 0x006cb9e0
    bool cut_start(u32 a8, u32 ac, std::int8_t play, std::int8_t animate, i32 unit_no, u32 target_24, u32 v28,
                   u32 v2c, std::int8_t mode) {
        c_cut_start.hit();
        if (static_cast<i32>(jo::self<jo::CutScene>(this)->active) != 0) return false;
        jo::self<jo::CutScene>(this)->hud_a = global<u8>(d::g_camera_lock_flag_a48ce3);
        jo::self<jo::CutScene>(this)->hud_c = global<u8>(d::g_camera_lock_flag_a48ce7);
        jo::self<jo::CutScene>(this)->hud_b = global<u8>(d::g_camera_lock_flag_a48ceb);
        global<u32>(d::g_camera_lock_flag_a48ce3) = 0;
        global<u32>(d::g_camera_lock_flag_a48ce7) = 0;
        global<u32>(d::g_camera_lock_flag_a48ceb) = 0;
        jo::self<jo::CutScene>(this)->scene_flag = joan::orig::arg::as<u32>(joan::orig::as_scene(jo::as_world(global<u8*>(d::g_world))->scene)->flag_6c());  // 0x004d2a00
        joan::orig::as_scene(jo::as_world(global<u8*>(d::g_world))->scene)->set_6c(0u);             // 0x005ef400
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3c0
            if (jo::as_unit(u)->draw != nullptr) joan::orig::as_unit_draw(jo::as_unit(u)->draw)->draw();  // 0x0043b890
        }
        {
            const i32 s = global<i32>(d::g_control_mode);
            if (s == 0 || s == 3) {
                joan::orig::as_player(obj(d::g_player_unit_recno))->switch_unit(1);  // 0x0042b6e0
            } else if (s == 1) {
                joan::orig::as_player(obj(d::g_player_unit_recno))->switch_unit(0);
            }
        }
        jo::self<jo::CutScene>(this)->follow_target = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(obj(d::g_camera_man))->target());  // 0x00563dc0
        if (static_cast<i32>(play) == 0) {
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(unit_no)) != 0) return false;  // 0x00428b70
            float cam[10];
            joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(cam));  // 0x006140a0 (copies 10 dwords)
            {
                void* const target = jo::self<jo::CutScene>(this)->cam_pos;
                void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(unit_no));  // 0x0041b3c0
                joan::orig::as_unit(u)->position(joan::orig::arg::mptr(target));                        // 0x004fedb0
            }
            jo::self<jo::CutScene>(this)->cam_pos[2] = jo::self<jo::CutScene>(this)->cam_pos[2] + 50.0f;  // 0x009cfc18
            jo::self<jo::CutScene>(this)->cam_dist = target_24;
            jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pitch) = v28;
            jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw) = v2c;
            float cam2[10];
            std::memcpy(cam2, cam, sizeof(cam2));
            std::memcpy(&cam2[1], jo::self<jo::CutScene>(this)->cam_pos, 12);
            std::memcpy(&cam2[7], &jo::self<jo::CutScene>(this)->cam_dist, 4);
            cam2[4] = jo::self<jo::CutScene>(this)->cam_pitch * kDeg;
            cam2[5] = jo::self<jo::CutScene>(this)->cam_yaw * kDeg;
            if (static_cast<i32>(animate) != 0) {
                play_path(cam, cam2);
            } else {
                jo::self<jo::CutScene>(this)->u8_13 = 0;
                jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 2;
                void* const c = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(5));  // 0x00563c80
                joan::orig::as_camera_motion(c)->start(joan::orig::arg::mptr(static_cast<void*>(cam2)), joan::orig::arg::mptr(static_cast<void*>(cam2)));  // 0x00423ec0
                joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(5);  // 0x00429860
            }
        } else {
            jo::self<jo::CutScene>(this)->u8_13 = 0;
            jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 2;
        }
        jo::self<jo::CutScene>(this)->mode = mode;
        jo::self<jo::CutScene>(this)->mode_prev = mode;
        if (static_cast<i32>(mode) == 1) {
            jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 6;
            jo::self<jo::CutScene>(this)->bar_size = global<u16>(d::g_screen_w);
            jo::self<jo::CutScene>(this)->bar_on = 0;
            if (jo::self<jo::CutScene>(this)->bar_tex == nullptr) {
                void* const tex = obj(d::g_texture_array);
                const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(tex)->mode());  // 0x0060a720
                joan::orig::as_texture_manager(tex)->set_mode(1u);                 // 0x0060a740
                jo::self<jo::CutScene>(this)->bar_tex = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(tex)->load(str(d::s_cut_scene_texture), 1, 0u));  // 0x006099c0 "cut_scene_texture"
                joan::orig::as_texture_manager(tex)->set_mode(joan::orig::arg::u32(saved));  // 0x0060a740
            }
        }
        jo::self<jo::CutScene>(this)->started = 1;
        jo::self<jo::CutScene>(this)->u8_40 = 0;
        joan::orig::as_cutscene(this)->begin(joan::orig::arg::u32(a8), joan::orig::arg::u32(ac));           // 0x006cab90
        joan::orig::as_game(obj(d::g_game_state))->set_744(static_cast<char>(joan::orig::arg::i32(1)));     // 0x004725f0
        joan::orig::as_group(obj(d::g_names))->set_paused(static_cast<std::int8_t>(joan::orig::arg::i32(1)));     // 0x0049b5e0
        if (global<i32>(d::g_control_mode) == 4) {
            for (i32 i = 1; i <= joan::orig::as_dynarray(kUnits)->size(); ++i) {  // 0x00417ad0
                if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
                u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3c0
                if (jo::as_unit(u)->actor != nullptr) set_fade(jo::as_unit(u)->actor);
                fade_unit(static_cast<i32>(jo::as_unit(u)->carried_a));
                fade_unit(static_cast<i32>(jo::as_unit(u)->carried_b));
                if (jo::as_unit(u)->trail_a != nullptr && jo::as_unit(u)->trail_a->actor != nullptr) set_fade(jo::as_unit(u)->trail_a->actor);
                if (jo::as_unit(u)->trail_b != nullptr && jo::as_unit(u)->trail_b->actor != nullptr) set_fade(jo::as_unit(u)->trail_b->actor);
            }
            joan::orig::as_army(obj(d::g_player_unit_recno))->apply(0u);  // 0x00452be0
        }
        {
            u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(global<i16>(d::g_a47c36))));  // 0x0041b3c0
            joan::orig::as_table(obj(d::g_orders))->drop_side(joan::orig::arg::i32(jo::as_unit(u)->side));  // 0x004ead00
        }
        return true;
    }

    // Sub-unit fade of the 0x00a460a0 list (inline in 0x006cb9e0).
    static void fade_unit(i32 id) {
        void* const list = obj(d::g_items);
        if (joan::orig::as_units(list)->list.empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x00425d50
        u8* const s = joan::orig::arg::as<u8*>(joan::orig::as_units(list)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
        if (jo::as_map_object(s)->actor != nullptr) set_fade(jo::as_map_object(s)->actor);
    }

    // The "animate" path of 0x006cb9e0: copies "cut_scene_move" into "cut_scene_play" with the
    // camera key frames, then starts it.
    void play_path(const float* cam, float* cam2) {
        const u32 move = joan::orig::arg::as<u32>(joan::orig::as_cam_tracks(anim_owner(this))->find(str(d::s_cut_scene_move)));  // 0x00429c70 "cut_scene_move"
        const u32 play = joan::orig::arg::as<u32>(joan::orig::as_cam_tracks(anim_owner(this))->find(str(d::s_cut_scene_play)));  // 0x00429c70 "cut_scene_play"
        u8* const dst = joan::orig::arg::as<u8*>(joan::orig::as_tracks(anim_owner(this))->get(joan::orig::arg::i16(static_cast<i16>(play))));  // 0x00428b30
        u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_tracks(anim_owner(this))->get(joan::orig::arg::i16(static_cast<i16>(move))));  // 0x00428b30
        for (i32 i = 0; i < 5; ++i) {
            u32_of(cam_key(dst, i).time) = u32_of(cam_key(src, i).time);
            u32_of(cam_key(dst, i).point[0]) = bits(cam[1]);
            u32_of(cam_key(dst, i).point[1]) = bits(cam[2]);
            cam_key(dst, i).point[2] = cam[3] + cam_key(src, i).point[2];
            cam_key(dst, i).yaw = cam[4] / kDeg;
            {
                const float q = cam[5] / kDeg;
                const float s = cam_key(src, i).pitch;
                cam_key(dst, i).pitch = !(q >= s) ? fl(s) : q;
            }
            cam_key(dst, i).roll = cam[6] / kDeg;
            {
                const float s = cam_key(src, i).zoom;
                if (s > cam[7]) {
                    cam_key(dst, i).zoom = fl(s);
                } else {
                    u32_of(cam_key(dst, i).zoom) = bits(cam[7]);
                }
            }
        }
        for (i32 i = 5; i < 10; ++i) {
            u32_of(cam_key(dst, i).time) = u32_of(cam_key(src, i).time);
            cam_key(dst, i).point[0] = lerp_d(this, bits(cam[1]), jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]), static_cast<float>(i - 5) / 4.0f);  // 0x009cf79c
            cam_key(dst, i).point[1] = lerp_d(this, bits(cam[2]), jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]), static_cast<float>(i - 5) / 4.0f);
            cam_key(dst, i).point[2] = cam[3] + cam_key(src, i).point[2];
            cam_key(dst, i).yaw = cam[4] / kDeg;
            u32_of(cam_key(dst, i).pitch) = u32_of(cam_key(src, i).pitch);
            cam_key(dst, i).roll = cam[6] / kDeg;
            u32_of(cam_key(dst, i).zoom) = u32_of(cam_key(src, i).zoom);
        }
        for (i32 i = 10; i < jo::as_cam_track(dst)->key_count; ++i) {
            u32_of(cam_key(dst, i).time) = u32_of(cam_key(src, i).time);
            u32_of(cam_key(dst, i).point[0]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]);
            u32_of(cam_key(dst, i).point[1]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]);
            cam_key(dst, i).point[2] = cam_key(src, i).point[2] + jo::self<jo::CutScene>(this)->cam_pos[2];
            {
                const float t = static_cast<float>(i - 10) / 4.0f;
                const float b = cam2[4] / kDeg;
                const float a = cam[4] / kDeg;
                cam_key(dst, i).yaw = lerp(this, a, b, t);
            }
            u32_of(cam_key(dst, i).pitch) = u32_of(cam_key(src, i).pitch);
            cam_key(dst, i).roll = cam[6] / kDeg;
            {
                const float s = cam_key(src, i).zoom;
                if (s > cam2[7]) {
                    cam_key(dst, i).zoom = fl(s);
                } else {
                    u32_of(cam_key(dst, i).zoom) = bits(cam2[7]);
                }
            }
        }
        u32_of(cam_key(dst, 0).zoom) = bits(cam[7]);
        cam_key(dst, 0).pitch = cam[5] / kDeg;
        {
            const i32 last = jo::as_cam_track(dst)->key_count - 1;
            u32_of(cam_key(dst, last).zoom) = jo::self<jo::CutScene>(this)->cam_dist;
            u32_of(cam_key(dst, last).pitch) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw);
        }
        joan::orig::as_cam_tracks(anim_owner(this))->prepare(joan::orig::arg::i32(play));  // 0x00429d00
        // (the original then stores dst[n-1] +0x92 * k / +0x9a into its camera copy - dead)
        void* const c = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(5));  // 0x00563c80
        joan::orig::as_camera_motion(c)->start(joan::orig::arg::mptr(static_cast<void*>(cam2)), joan::orig::arg::mptr(static_cast<void*>(cam2)));  // 0x00423ec0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(5);  // 0x00429860
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(2);  // 0x00429860
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->start_track(str(d::s_cut_scene_play), joan::orig::arg::u32(global<u32>(d::g_now)));  // 0x00428850
        jo::self<jo::CutScene>(this)->u8_13 = 1;
        jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_08", name, addr, fn)
B("CutScene::start_6cb9e0", 0x006cb9e0, &H::cut_start);
#undef B

}  // namespace game_b106_08
}  // namespace

// ---------------------------------------- from modules/game_b109_15.cpp (game/b109_15)
//
//    - CutScene::start_6cc6a0 0x006cc6a0 (ret 0x10, (a, b, byte move, byte mode)): returns al
//      (callers 0x0065ae2c / 0x0065b012 movsx al, 0x006ca624 ignores it). Camera key copy
//      cut_scene_move -> cut_scene_play (0x00563c80 / 0x00423fe0 snapshots, lerp 0x006cdf70).
//
//  - 0x006cc6a0: both 0x28-byte stack snapshots are completely written by 0x00423fe0 (rep movs
//    10 dwords). The stores to [ebp-0x18] / [ebp-0x10] after the loops are dead (never read);
//    the reads they come from are of the entry just written. x87 compares: `test ah,1` je ->
//    !(q < s || unordered); `test ah,0x41` jne -> s > limit, NaN to the limit.
namespace {
namespace game_b109_15 {

float fl(u32 bits) {
    float f;
    std::memcpy(&f, &bits, 4);
    return f;
}
u32 bits_of(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}

JOAN_COUNTER(c_cutstart, "CutScene::start_6cc6a0");

struct H {
    // ------------------------------------------------------------------ 0x006cc6a0
    u8 cut_start(u32 a, u32 b, u32 move_flag, u32 mode) {
        c_cutstart.hit();
        if (jo::self<jo::CutScene>(this)->active == 0) return 0;
        if (jo::view<u32>(jo::self<jo::CutScene>(this)->state) != 2 && jo::view<i8>(jo::self<jo::CutScene>(this)->u8_40) != 0) return 1;
        global<i32>(d::g_camera_lock_flag_a48ce3) = jo::view<i8>(jo::self<jo::CutScene>(this)->hud_a);
        global<i32>(d::g_camera_lock_flag_a48ce7) = jo::view<i8>(jo::self<jo::CutScene>(this)->hud_c);
        global<i32>(d::g_camera_lock_flag_a48ceb) = jo::view<i8>(jo::self<jo::CutScene>(this)->hud_b);
        joan::orig::as_scene(jo::as_world(global<void*>(d::g_world))->scene)->set_6c(joan::orig::arg::u32(jo::self<jo::CutScene>(this)->scene_flag));  // 0x005ef400
        if (static_cast<i8>(move_flag) != 0) {
            u32 l2c[10];  // [ebp-0x2c]
            u32 l54[10];  // [ebp-0x54]
            {
                void* s = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(i32{5}));  // 0x00563c80
                joan::orig::as_camera_motion(s)->end(joan::orig::arg::mptr(static_cast<void*>(l2c)));          // 0x00423fe0
                s = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(joan::orig::arg::i32(jo::self<jo::CutScene>(this)->follow_target)));
                joan::orig::as_camera_motion(s)->end(joan::orig::arg::mptr(static_cast<void*>(l54)));
            }
            if (static_cast<i8>(move_flag) != 0) {
                void* const owner = jo::self<jo::CutScene>(this)->zones->tracks;
                const u32 id_move = joan::orig::arg::as<u32>(joan::orig::as_cam_tracks(owner)->find(joan::img::str(d::s_cut_scene_move)));  // 0x00429c70 "cut_scene_move"
                const u32 id_play = joan::orig::arg::as<u32>(joan::orig::as_cam_tracks(jo::self<jo::CutScene>(this)->zones->tracks)->find(joan::img::str(d::s_cut_scene_play)));  // "cut_scene_play"
                u8* const dst = joan::orig::arg::as<u8*>(joan::orig::as_tracks(jo::self<jo::CutScene>(this)->zones->tracks)->get(joan::orig::arg::i16(id_play)));  // 0x00428b30
                u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_tracks(jo::self<jo::CutScene>(this)->zones->tracks)->get(joan::orig::arg::i16(id_move)));
                const float c884 = global<float>(d::flt_0_017453292);
                const float c79c = global<float>(d::flt_4);
                auto lerp = [&](u32 lo, u32 hi, float t) -> u32 {
                    return bits_of(joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(lo), joan::orig::arg::f32(hi), joan::orig::arg::f32(t))));  // 0x006cdf70
                };
                i32 i = 0;
                for (; i < 5; ++i) {
                    u32_of(cam_key(dst, i).time) = u32_of(cam_key(src, i).time);
                    u32_of(cam_key(dst, i).point[0]) = l2c[1];
                    u32_of(cam_key(dst, i).point[1]) = l2c[2];
                    u32_of(cam_key(dst, i).point[2]) = bits_of(fl(l2c[3]) + fl(u32_of(cam_key(src, i).point[2])));
                    u32_of(cam_key(dst, i).yaw) = bits_of(fl(l2c[4]) / c884);
                    {
                        const float s92 = fl(u32_of(cam_key(src, i).pitch));
                        const float q = fl(l2c[5]) / c884;
                        u32_of(cam_key(dst, i).pitch) = (q >= s92) ? bits_of(q) : bits_of(s92);
                    }
                    u32_of(cam_key(dst, i).roll) = bits_of(fl(l2c[6]) / c884);
                    {
                        const float s9a = fl(u32_of(cam_key(src, i).zoom));
                        u32_of(cam_key(dst, i).zoom) = (s9a > fl(l2c[7])) ? bits_of(s9a) : l2c[7];
                    }
                }
                for (i = 5; i < 10; ++i) {
                    u32_of(cam_key(dst, i).time) = u32_of(cam_key(src, i).time);
                    {
                        const float t = static_cast<float>(Fpu{true}.div(static_cast<double>(i - 5), static_cast<double>(c79c)));
                        u32_of(cam_key(dst, i).point[0]) = lerp(l2c[1], l54[1], t);
                    }
                    {
                        const float t = static_cast<float>(Fpu{true}.div(static_cast<double>(i - 5), static_cast<double>(c79c)));
                        u32_of(cam_key(dst, i).point[1]) = lerp(l2c[2], l54[2], t);
                    }
                    u32_of(cam_key(dst, i).point[2]) = bits_of(fl(l2c[3]) + fl(u32_of(cam_key(src, i).point[2])));
                    u32_of(cam_key(dst, i).yaw) = bits_of(fl(l2c[4]) / c884);
                    u32_of(cam_key(dst, i).pitch) = u32_of(cam_key(src, i).pitch);
                    u32_of(cam_key(dst, i).roll) = bits_of(fl(l2c[6]) / c884);
                    u32_of(cam_key(dst, i).zoom) = u32_of(cam_key(src, i).zoom);
                }
                for (i = 10; i < jo::as_cam_track(dst)->key_count; ++i) {
                    u32_of(cam_key(dst, i).time) = u32_of(cam_key(src, i).time);
                    u32_of(cam_key(dst, i).point[0]) = l54[1];
                    u32_of(cam_key(dst, i).point[1]) = l54[2];
                    u32_of(cam_key(dst, i).point[2]) = bits_of(fl(l54[3]) + fl(u32_of(cam_key(src, i).point[2])));
                    {
                        const float t = static_cast<float>(Fpu{true}.div(static_cast<double>(i - 10), static_cast<double>(c79c)));
                        const float hi = fl(l54[4]) / c884;
                        const float lo = fl(l2c[4]) / c884;
                        u32_of(cam_key(dst, i).yaw) = lerp(bits_of(lo), bits_of(hi), t);
                    }
                    u32_of(cam_key(dst, i).pitch) = u32_of(cam_key(src, i).pitch);
                    u32_of(cam_key(dst, i).roll) = bits_of(fl(l2c[6]) / c884);
                    {
                        const float s9a = fl(u32_of(cam_key(src, i).zoom));
                        u32_of(cam_key(dst, i).zoom) = (s9a > fl(l54[7])) ? bits_of(s9a) : l54[7];
                    }
                }
                u32_of(cam_key(dst, 0).zoom) = l2c[7];
                u32_of(cam_key(dst, 0).pitch) = bits_of(fl(l2c[5]) / c884);
                {
                    const i32 last = jo::as_cam_track(dst)->key_count - 1;
                    u32_of(cam_key(dst, last).zoom) = l54[7];
                    u32_of(cam_key(dst, last).pitch) = bits_of(fl(l54[5]) / c884);
                }
                joan::orig::as_cam_tracks(jo::self<jo::CutScene>(this)->zones->tracks)->prepare(joan::orig::arg::i32(id_play));  // 0x00429d00
                // [ebp-0x18] / [ebp-0x10] = last entry's +0x92 * c884 / +0x9a: dead stores.
                {
                    void* s = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(i32{5}));  // 0x00563c80
                    joan::orig::as_camera_motion(s)->start(joan::orig::arg::mptr(static_cast<void*>(l54)), joan::orig::arg::mptr(static_cast<void*>(l54)));  // 0x00423ec0
                }
                joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(i32{5});  // 0x00429860
                joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(i32{2});
                joan::orig::as_camera_zones(jo::self<jo::CutScene>(this)->zones)->start_track(joan::img::str(d::s_cut_scene_play), joan::orig::arg::u32(global<u32>(d::g_now)));  // 0x00428850
                jo::self<jo::CutScene>(this)->u8_13 = 1;
                jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 4;
            }
        }
        jo::view<u8>(jo::self<jo::CutScene>(this)->mode_prev) = static_cast<u8>(mode);
        if (static_cast<i8>(mode) == 1) {
            jo::view<u32>(jo::self<jo::CutScene>(this)->state) = 8;
            if (jo::self<jo::CutScene>(this)->bar_tex == nullptr) {
                const u32 old = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(obj(d::g_texture_array))->mode());  // 0x0060a720
                joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(i32{1}));        // 0x0060a740
                jo::self<jo::CutScene>(this)->bar_tex = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::img::str(d::s_cut_scene_texture), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0 "cut_scene_texture"
                joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(old));
            }
            jo::self<jo::CutScene>(this)->bar_size = static_cast<u16>(0u - jo::self<jo::CutScene>(this)->bar_tex->width);
            jo::self<jo::CutScene>(this)->bar_on = 1;
        }
        jo::self<jo::CutScene>(this)->started = 0;
        jo::self<jo::CutScene>(this)->u8_40 = 1;
        joan::orig::as_army(obj(d::g_player_unit_recno))->apply(joan::orig::arg::u32(i32{1}));  // 0x00452be0
        joan::orig::as_cutscene(this)->begin(joan::orig::arg::u32(a), joan::orig::arg::u32(b));               // 0x006cab90
        joan::orig::as_script(obj(d::g_dialogue))->purge_63d150();          // 0x0063d150
        return 1;
    }

    // ------------------------------------------------------------------ 0x00751750
    struct SlideFrame {
        char text[0x20];  // [ebp-0x34]
        i32 i;            // [ebp-0x14]
        i32 count;        // [ebp-0x10]
        i32 j;            // [ebp-0xc]
        i32 dy;           // [ebp-0x8]
        void* self;       // [ebp-0x4]
    };
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
    static_assert(sizeof(SlideFrame) == 0x34, "slide frame");
#endif
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_15", name, addr, fn)
B("CutScene::start_6cc6a0", 0x006cc6a0, &H::cut_start);
#undef B

}  // namespace game_b109_15
}  // namespace

// ---------------------------------------- from modules/game_b106_09.cpp (game/b106_09)
//
//    - 0x006ccf00 CutScene::start(flag, x, y, z, a18, a1c, a20)  (thiscall, ret 0x1c; caller
//        0x65b0b6 reads al only). Fills the "cut_scene_play" path keyframes from the
//        "cut_scene_move" path and the current camera (0x006140a0 on [0x00a48ccc]), with
//        lerps through 0x006cdf70 (float st0, ret 0xc).
//
//  - 0x006ccf00: the stores to [ebp-0x18] / [ebp-0x10] after 0x00429d00 are dead (never read)
//    and are omitted; every keyframe read / write keeps the original order.
namespace {
namespace game_b106_09 {
using joan::orig::helpers::as_float;
u32 as_bits(float f) {
    u32 b;
    std::memcpy(&b, &f, sizeof b);
    return b;
}

float deg() { return global<float>(d::flt_0_017453292); }  // 0.0174533 (0x3c8efa35)

JOAN_COUNTER(c_cut_scene, "CutScene::start_6ccf00");

struct H {
    // ------------------------------------------------------------------------ 0x006ccf00
    u8 cut_scene_start(u32 flag, u32 x, u32 y, u32 z, u32 a18, u32 a1c, u32 a20) {
        c_cut_scene.hit();
        if (jo::self<jo::CutScene>(this)->state != 2 && jo::self<jo::CutScene>(this)->state != 7) return 0;
        if (!(jo::self<jo::CutScene>(this)->duration >= 0.0f)) return 0;
        if (jo::self<jo::CutScene>(this)->active == 0) return 0;
        u32 o[10];  // [ebp-0x2c]
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(o));  // 0x006140a0
        jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]) = x;
        jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]) = y;
        jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[2]) = z;
        jo::self<jo::CutScene>(this)->cam_dist = a18;
        jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pitch) = a1c;
        jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw) = a20;
        u32 c[10];  // [ebp-0x54]
        std::memcpy(c, o, sizeof c);
        c[1] = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]);
        c[2] = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]);
        c[3] = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[2]);
        c[7] = jo::self<jo::CutScene>(this)->cam_dist;
        c[4] = as_bits(jo::self<jo::CutScene>(this)->cam_pitch * deg());
        c[5] = as_bits(jo::self<jo::CutScene>(this)->cam_yaw * deg());
        if (static_cast<i8>(flag) == 0) {
            jo::self<jo::CutScene>(this)->u8_13 = 0;
            if (jo::self<jo::CutScene>(this)->state != 7) jo::self<jo::CutScene>(this)->state = 2;
            void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(5));  // 0x00563c80
            joan::orig::as_camera_motion(cam)->start(joan::orig::arg::mptr(c), joan::orig::arg::mptr(c));                               // 0x00423ec0
            joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(5);                      // 0x00429860
            return 1;
        }
        const u32 mv = joan::orig::arg::as<u32>(joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->find(str(d::s_cut_scene_move)));  // 0x00429c70 'cut_scene_move'
        const u32 pl = joan::orig::arg::as<u32>(joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->find(str(d::s_cut_scene_play)));  // 0x00429c70 'cut_scene_play'
        u8* const P = joan::orig::arg::as<u8*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->get(joan::orig::arg::i16(pl)));  // 0x00428b30
        u8* const M = joan::orig::arg::as<u8*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->get(joan::orig::arg::i16(mv)));  // 0x00428b30
        const Fpu fpu{true};
        for (i32 i = 0; i < 5; ++i) {
            u32_of(cam_key(P, i).time) = u32_of(cam_key(M, i).time);
            u32_of(cam_key(P, i).point[0]) = o[1];
            u32_of(cam_key(P, i).point[1]) = o[2];
            cam_key(P, i).point[2] = as_float(o[3]) + cam_key(M, i).point[2];
            cam_key(P, i).yaw = as_float(o[4]) / deg();
            {
                const float m92 = cam_key(M, i).pitch;
                const float t = as_float(o[5]) / deg();
                float v;
                if (!(t >= m92)) {  // fcompp: st0 = t < st1 = m92, or unordered
                    v = cam_key(M, i).pitch;
                } else {
                    v = as_float(o[5]) / deg();
                }
                cam_key(P, i).pitch = v;
            }
            cam_key(P, i).roll = as_float(o[6]) / deg();
            if (cam_key(M, i).zoom > as_float(o[7])) {
                cam_key(P, i).zoom = cam_key(M, i).zoom;
            } else {
                u32_of(cam_key(P, i).zoom) = o[7];
            }
        }
        for (i32 i = 5; i < 10; ++i) {
            u32_of(cam_key(P, i).time) = u32_of(cam_key(M, i).time);
            {
                const float frac = static_cast<float>(fpu.div(static_cast<double>(i - 5), 4.0));  // 0x009cf79c
                cam_key(P, i).point[0] =
                    joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(o[1]), joan::orig::arg::f32(jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0])), joan::orig::arg::f32(frac)));  // 0x006cdf70
            }
            {
                const float frac = static_cast<float>(fpu.div(static_cast<double>(i - 5), 4.0));
                cam_key(P, i).point[1] =
                    joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(o[2]), joan::orig::arg::f32(jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1])), joan::orig::arg::f32(frac)));  // 0x006cdf70
            }
            cam_key(P, i).point[2] = as_float(o[3]) + cam_key(M, i).point[2];
            cam_key(P, i).yaw = as_float(o[4]) / deg();
            u32_of(cam_key(P, i).pitch) = u32_of(cam_key(M, i).pitch);
            cam_key(P, i).roll = as_float(o[6]) / deg();
            u32_of(cam_key(P, i).zoom) = u32_of(cam_key(M, i).zoom);
        }
        for (i32 i = 10; i < jo::as_cam_track(P)->key_count; ++i) {
            u32_of(cam_key(P, i).time) = u32_of(cam_key(M, i).time);
            u32_of(cam_key(P, i).point[0]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]);
            u32_of(cam_key(P, i).point[1]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]);
            cam_key(P, i).point[2] = cam_key(M, i).point[2] + jo::self<jo::CutScene>(this)->cam_pos[2];
            {
                const float frac = static_cast<float>(fpu.div(static_cast<double>(i - 10), 4.0));
                const float to = as_float(c[4]) / deg();
                const float from = as_float(o[4]) / deg();
                cam_key(P, i).yaw = joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(from), joan::orig::arg::f32(to), joan::orig::arg::f32(frac)));  // 0x006cdf70
            }
            u32_of(cam_key(P, i).pitch) = u32_of(cam_key(M, i).pitch);
            cam_key(P, i).roll = as_float(o[6]) / deg();
            if (cam_key(M, i).zoom > as_float(c[7])) {
                cam_key(P, i).zoom = cam_key(M, i).zoom;
            } else {
                u32_of(cam_key(P, i).zoom) = c[7];
            }
        }
        u32_of(cam_key(P, 0).zoom) = o[7];
        cam_key(P, 0).pitch = as_float(o[5]) / deg();
        u32_of(cam_key(P, jo::as_cam_track(P)->key_count - 1).zoom) = jo::self<jo::CutScene>(this)->cam_dist;
        u32_of(cam_key(P, jo::as_cam_track(P)->key_count - 1).pitch) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw);
        joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->prepare(joan::orig::arg::i32(pl));  // 0x00429d00
        // (dead stores to [ebp-0x18] / [ebp-0x10] omitted)
        void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(5));  // 0x00563c80
        joan::orig::as_camera_motion(cam)->start(joan::orig::arg::mptr(c), joan::orig::arg::mptr(c));                               // 0x00423ec0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(5);                      // 0x00429860
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(2);                      // 0x00429860
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->start_track(str(d::s_cut_scene_play), joan::orig::arg::u32(global<u32>(d::g_now)));  // 0x00428850 'cut_scene_play'
        jo::self<jo::CutScene>(this)->u8_13 = 1;
        if (jo::self<jo::CutScene>(this)->state != 7) jo::self<jo::CutScene>(this)->state = 3;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_09", name, addr, fn)
B("CutScene::start_6ccf00", 0x006ccf00, &H::cut_scene_start);
#undef B

}  // namespace game_b106_09
}  // namespace

// ---------------------------------------- from modules/game_b106_11.cpp (game/b106_11)
//
//    - CutScene::start_camera 0x006cd720 (thiscall, (char play, int unit, a, b, c), ret 0x14)
//      -> al: state +0xf must be 2 or 7, +7 >= 0.0f, (char)+4 != 0, unit alive; camera
//      snapshot (0x006140a0 on [0xa48ccc]), unit position into +0x18 (+0x20 += 50.0f),
//      +0x24..+0x2c = a, b, c; play: builds the "cut_scene_play" track from
//      "cut_scene_move" (0x00429c70 / 0x00428b30, keys 0..4 blended, 5..9 lerped with
//      0x006cdf70, 10.. +0x72 lerped), 0x00429d00, camera 0x00563c80(5) -> 0x00423ec0,
//      0x00429860(5), (2), 0x00428850 "cut_scene_play", +0x13 = 1, state 3 unless 7;
//      else +0x13 = 0, state 2 unless 7, camera set and 0x00429860(5).
//
//  - 0x004e4bb0 / 0x00514c40 / 0x006cd720 / 0x0053b1b0: word arguments pushed with garbage
//    upper halves (`mov ax, w; push eax`) go to callees that read the low word only
//    (0x004eef00 -> 0x0047e200 slots +0x10 / +0x18, 0x0042d0a0, 0x00425ce0, 0x00428b30,
//    the 0x0065xxxx script helpers) - passed sign- or zero-extended.
//
//  - 0x006cd720: after 0x00429d00 the original recomputes two camera snapshot fields from
//    the new track (fld / fmul / fstp into its stack copy) that are never read again; the
//    dead loads are not repeated. The lerp helper 0x006cdf70 returns st0 at PC24 and is
//    stored with fstp dword - taken as float. (int) / 4.0f fild divisions use Fpu{true}.
namespace {
namespace game_b106_11 {


void* const kCamera = obj(d::g_camera_man);

u32 now() { return global<u32>(d::g_now); }

JOAN_COUNTER(c_cut_cam, "CutScene::start_camera");

struct H {
    // ------------------------------------------------------------------ 0x006cd720
    bool cut_start_camera(char play, i32 unit, u32 a, u32 b, u32 c) {
        c_cut_cam.hit();
        if (jo::self<jo::CutScene>(this)->state != 2 && jo::self<jo::CutScene>(this)->state != 7) return false;
        if (!(jo::self<jo::CutScene>(this)->duration >= global<float>(d::k_9cf5c4))) return false;
        if (jo::self<jo::CutScene>(this)->active == 0) return false;
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(unit)) != 0) return false;  // 0x00428b70
        float cam[10];
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(cam));  // 0x006140a0
        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(unit)));  // 0x0041b3c0
        joan::orig::as_unit(u)->position(joan::orig::arg::mptr(jo::self<jo::CutScene>(this)->cam_pos));                     // 0x004fedb0
        jo::self<jo::CutScene>(this)->cam_pos[2] = jo::self<jo::CutScene>(this)->cam_pos[2] + global<float>(d::flt_50);  // + 50.0f
        jo::self<jo::CutScene>(this)->cam_dist = a;
        jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pitch) = b;
        jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw) = c;
        const float k = global<float>(d::flt_0_017453292);  // deg -> rad
        float cp[10];
        std::memcpy(cp, cam, sizeof cp);
        std::memcpy(&cp[1], jo::self<jo::CutScene>(this)->cam_pos, 12);
        std::memcpy(&cp[7], &jo::self<jo::CutScene>(this)->cam_dist, 4);
        cp[4] = jo::self<jo::CutScene>(this)->cam_pitch * k;
        cp[5] = jo::self<jo::CutScene>(this)->cam_yaw * k;
        if (play != 0) {
            void* const tracks = jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks);
            const i32 move = joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(tracks)->find(str(d::s_cut_scene_move)));  // 0x00429c70 "cut_scene_move"
            const i32 playk = joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(tracks)->find(str(d::s_cut_scene_play)));  // "cut_scene_play"
            u8* const dst = joan::orig::arg::as<u8*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->get(joan::orig::arg::i16(static_cast<i16>(playk))));  // 0x00428b30
            u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->get(joan::orig::arg::i16(static_cast<i16>(move))));
            build_track(dst, src, cam, cp, k);
            u32_of(cam_key(dst, 0).zoom) = to_bits(cam[7]);
            cam_key(dst, 0).pitch = cam[5] / k;
            const i32 n1 = jo::as_cam_track(dst)->key_count - 1;
            u32_of(cam_key(dst, n1).zoom) = jo::self<jo::CutScene>(this)->cam_dist;
            const i32 n2 = jo::as_cam_track(dst)->key_count - 1;
            u32_of(cam_key(dst, n2).pitch) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_yaw);
            joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::CutScene>(this)->zones->tracks))->prepare(joan::orig::arg::i32(playk));  // 0x00429d00
            // (dead: the original reloads two snapshot fields from the track here)
            void* const view = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamera)->motion(i32{5}));  // 0x00563c80
            joan::orig::as_camera_motion(view)->start(joan::orig::arg::mptr(cp), joan::orig::arg::mptr(cp));                              // 0x00423ec0
            joan::orig::as_camera_man(kCamera)->set_mode(i32{5});                      // 0x00429860
            joan::orig::as_camera_man(kCamera)->set_mode(i32{2});
            joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::CutScene>(this)->zones))->start_track(str(d::s_cut_scene_play), joan::orig::arg::u32(now()));  // 0x00428850
            jo::self<jo::CutScene>(this)->u8_13 = 1;
            if (jo::self<jo::CutScene>(this)->state != 7) jo::self<jo::CutScene>(this)->state = 3;
        } else {
            jo::self<jo::CutScene>(this)->u8_13 = 0;
            if (jo::self<jo::CutScene>(this)->state != 7) jo::self<jo::CutScene>(this)->state = 2;
            void* const view = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamera)->motion(i32{5}));  // 0x00563c80
            joan::orig::as_camera_motion(view)->start(joan::orig::arg::mptr(cp), joan::orig::arg::mptr(cp));                              // 0x00423ec0
            joan::orig::as_camera_man(kCamera)->set_mode(i32{5});                      // 0x00429860
        }
        return true;
    }

    static u32 to_bits(float v) {
        u32 r;
        std::memcpy(&r, &v, 4);
        return r;
    }

    float lerp(u32 a, u32 b, float t) { return joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }  // 0x006cdf70
    float lerp(float a, float b, float t) { return joan::orig::arg::as<float>(joan::orig::as_cutscene(this)->lerp(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t))); }

    void build_track(u8* dst, u8* src, const float* cam, const float* cp, float k) {
        const Fpu fpu{true};
        const float k4 = global<float>(d::flt_4);  // 4.0f
        i32 i = 0;
        for (; i < 5; ++i) {
            jo::CamKey& d = cam_key(dst, i);
            const jo::CamKey& s = cam_key(src, i);
            u32_of(d.time) = u32_of(s.time);
            u32_of(d.point[0]) = to_bits(cam[1]);
            u32_of(d.point[1]) = to_bits(cam[2]);
            d.point[2] = cam[3] + s.point[2];
            d.yaw = cam[4] / k;
            {
                const float q = cam[5] / k;
                const float sv = s.pitch;
                d.pitch = (q >= sv) ? q : sv;
            }
            d.roll = cam[6] / k;
            {
                const float sv = s.zoom;
                d.zoom = (sv > cam[7]) ? sv : cam[7];
            }
        }
        for (i = 5; i < 10; ++i) {
            jo::CamKey& d = cam_key(dst, i);
            const jo::CamKey& s = cam_key(src, i);
            u32_of(d.time) = u32_of(s.time);
            const float t1 = static_cast<float>(fpu.div(static_cast<double>(i - 5), k4));
            d.point[0] = lerp(to_bits(cam[1]), jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]), t1);
            const float t2 = static_cast<float>(fpu.div(static_cast<double>(i - 5), k4));
            d.point[1] = lerp(to_bits(cam[2]), jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]), t2);
            d.point[2] = cam[3] + s.point[2];
            d.yaw = cam[4] / k;
            u32_of(d.pitch) = u32_of(s.pitch);
            d.roll = cam[6] / k;
            u32_of(d.zoom) = u32_of(s.zoom);
        }
        for (i = 10; i < jo::as_cam_track(dst)->key_count; ++i) {
            jo::CamKey& d = cam_key(dst, i);
            const jo::CamKey& s = cam_key(src, i);
            u32_of(d.time) = u32_of(s.time);
            u32_of(d.point[0]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[0]);
            u32_of(d.point[1]) = jo::view<u32>(jo::self<jo::CutScene>(this)->cam_pos[1]);
            d.point[2] = s.point[2] + jo::self<jo::CutScene>(this)->cam_pos[2];
            const float t = static_cast<float>(fpu.div(static_cast<double>(i - 10), k4));
            const float hi = cp[4] / k;
            const float lo = cam[4] / k;
            d.yaw = lerp(lo, hi, t);
            u32_of(d.pitch) = u32_of(s.pitch);
            d.roll = cam[6] / k;
            {
                const float sv = s.zoom;
                d.zoom = (sv > cp[7]) ? sv : cp[7];
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_11", name, addr, fn)
B("CutScene::start_camera", 0x006cd720, &H::cut_start_camera);
#undef B

}  // namespace game_b106_11
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
//
//      0x006ce100 Panel::init_good_fin          (thiscall, ret 0)
//
//      0x006d38a0 Obj6d38a0::~dtor              (thiscall, ret 0)
//
//      0x006d65d0 Obj6d65d0::init               (thiscall, ret 0)
//
//  - 0x006ce100: the two words +0x34 / +0x36 pushed for 0x0061e370 (`mov ax/cx, word`): the
//    upper halves differ from the original (ax: result pointer, cx: whatever ecx held after the
//    call) but 0x0061e370 only reads its first two args as words (movsx / mov word), so dead.
namespace {
namespace game_b96_08 {


using joan::orig::helpers::u;

JOAN_COUNTER(c_panel_init, "Panel::init_good_fin");
JOAN_COUNTER(c_dtor6d38a0, "Obj6d38a0::~dtor");

// Colour(dword) on a 4-byte local, the dword copied out.
u32 colour_of(u32 v) {
    u32 c;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&c)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}

struct H {
    // ------------------------------------------------------------------ 0x006ce100
    void* panel_init() {
        c_panel_init.hit();
        const u32 one = 0x3f800000u;  // 1.0f
        void* const fonts = obj(d::g_bmp_ending);
        jo::self<jo::Ending>(this)->picture = nullptr;
        jo::Label* const b1 = &jo::self<jo::Ending>(this)->title;
        {
            const u32 colour = colour_of(0xffffffffu);
            joan::orig::as_label(b1)->create(joan::orig::arg::i32(-0x2e9), joan::orig::arg::i32(-0x35), joan::orig::arg::mptr(fonts), str(d::s_good1), joan::orig::arg::i32(colour), 3, 0, 2, 1, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);  // 0x00629660 "good1"
        }
        joan::orig::as_widget(b1)->scale_to(0, joan::orig::arg::f32(one), 0.0f, 1, 0u);  // 0x0061f1b0
        {
            const u8* const g36 = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(fonts)->find(str(d::s_good1)));  // 0x00620b40
            const u32 w36 = (u(g36) & 0xffff0000u) | jo::as_bmp_entry(g36)->height;
            const u8* const g34 = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(fonts)->find(str(d::s_good1)));  // 0x00620b40
            const u32 w34 = (u(g34) & 0xffff0000u) | jo::as_bmp_entry(g34)->width;
            joan::orig::as_widget(b1)->resize_to(joan::orig::arg::u32(w34), joan::orig::arg::u32(w36), joan::orig::arg::f32(one), joan::orig::arg::f32(0), 1u, 1u, 0u);  // 0x0061e370
        }
        joan::orig::as_widget(b1)->move_to(-745, -53, joan::orig::arg::f32(one), 0.0f, 1, 0u);  // 0x0061e020
        jo::Image2D* const b2 = &jo::self<jo::Ending>(this)->text;
        {
            const u32 colour = colour_of(0xffffffffu);
            joan::orig::as_image2d(b2)->create(0x28, 0x14, joan::orig::as_font2d(obj(d::g_font_dauphin_14_bold)), joan::orig::arg::str(obj(d::kEmpty)), 4, joan::orig::arg::i32(colour), 0, 0, 2, 0, joan::orig::arg::i32(one), joan::orig::arg::i32(one), 0, 1, 1);  // 0x00629530
        }
        joan::orig::as_widget(b2)->scale_to(0, joan::orig::arg::f32(one), 0.0f, 1, 0u);           // 0x0061f1b0
        joan::orig::as_widget(b2)->move_to(0x28, 0x14, joan::orig::arg::f32(one), 0.0f, 1, 0u);  // 0x0061e020
        joan::orig::as_label(b2)->set_layout(0x24eu, 0xdcu, 2u, 1u, 1u, 0u, joan::orig::arg::u32(-1));  // 0x006299b0
        jo::Image2D* const b3 = &jo::self<jo::Ending>(this)->frame;
        {
            const u32 colour = colour_of(0xffffffffu);
            joan::orig::as_label(b3)->create(0x16d, 0x123, joan::orig::arg::mptr(fonts), str(d::s_fin), joan::orig::arg::i32(colour), 3, 0, 2, 1, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);  // 0x00629660 "fin"
        }
        joan::orig::as_widget(b3)->scale_to(0, joan::orig::arg::f32(one), 0.0f, 1, 0u);  // 0x0061f1b0
        jo::self<jo::Ending>(this)->score = 0;
        jo::view<u8>(jo::self<jo::Ending>(this)->kind) = 0;
        return this;
    }

    // ------------------------------------------------------------------ 0x006d38a0
    std::uintptr_t dtor_6d38a0() {
        c_dtor6d38a0.hit();
        joan::orig::as_popup(this)->release_6d3b80();                        // 0x006d3b80
        joan::orig::as_widget(&jo::self<jo::MessageBox>(this)->button2)->dtor_6211a0();              // 0x006211a0
        joan::orig::as_widget(&jo::self<jo::MessageBox>(this)->button1)->dtor_6211a0();              // 0x006211a0
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_widget(&jo::self<jo::MessageBox>(this)->button0)->dtor_6211a0());  // 0x006211a0
    }

    // ------------------------------------------------------------------ 0x006d65d0
    void* init_6d65d0() {
        jo::self<jo::InventoryScreen>(this)->sheet = img::obj(d::g_bmp_res_invent);
        jo::self<jo::InventoryScreen>(this)->tex = nullptr;
        jo::self<jo::InventoryScreen>(this)->carried_item = 0;
        jo::self<jo::InventoryScreen>(this)->carried_source = 0xff;
        jo::self<jo::InventoryScreen>(this)->i32_17 = 0;
        jo::self<jo::InventoryScreen>(this)->carried_count = 0;
        jo::self<jo::InventoryScreen>(this)->i32_23 = 0;
        jo::self<jo::InventoryScreen>(this)->carried_grid_x = 0xff;
        jo::self<jo::InventoryScreen>(this)->carried_grid_y = 0xff;
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Panel::init_good_fin", 0x006ce100, &H::panel_init);
B96("Obj6d38a0::~dtor", 0x006d38a0, &H::dtor_6d38a0);
B96("Obj6d65d0::init", 0x006d65d0, &H::init_6d65d0);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_ui3.cpp (game/ui3)
//
//      - Ending::draw 0x006ce450 (thiscall, ret 0). Does nothing unless +0x234 is set. A
//        17-step state machine on the word +0x231, with two variants picked by the byte +0x233
//        (0 = good ending "goodN" / "END_GOOD_N", 1 = bad ending "badN" / "END_BAD_N"). The
//        step timer +0x22d (dword) advances by the frame time 0x00a500b6; picture steps wait
//        until the picture widget (+0x8, scale +0x19) and the text widget (+0xbf, scale +0xd0)
//        have shrunk to 0, then reload the picture (0x006d2280, 0x006d1f20, name copied to
//        +0x7e, size from the image bank 0x00a45d00 0x00620b40), queue widget animations
//        (0x0061f1b0 scale, 0x0061ea80, 0x0061e020 move, 0x0061e370 resize, 0x006298c0 set
//        text from the string table 0x00a57c08, 0x006299b0) and reset the timer; timed steps
//        fade the widgets out at fixed times and start the speech (0x004ceef0 on 0x00a5cf98,
//        +0x235 = already started). The last step (+0x176 widget, scale +0x187) ends the game
//        (0x0041c780 on 0x00a51294, 0x006d1d30, Game::set_state 0x004d7e70 (15)).
//        Every frame then draws the full-screen black fade quad (alpha from the first 3 s
//        fade-in, else 255; 0x00632a70) and the "mask_t" / "mask_b" letterbox bars
//        (0x00633500) on the sprite queue 0x00a46128, and the three widgets (0x00629bd0).
namespace {
namespace game_ui3 {

constexpr img::Addr kImages = d::g_bmp_ending;   // image bank
constexpr img::Addr kStrings = d::g_strings;  // ending text table
constexpr img::Addr kSpeech = d::g_sfx_player;   // speech / sound player
constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue

// Widget calls (this = widget), every stack argument one raw dword.
// 0x0061f1b0 scale_to(target, speed, accel, instant, arg), ret 0x14.
void scale(void* w, u32 a, u32 b, u32 c, u32 d, u32 e) { joan::orig::as_widget(w)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e)); }
// 0x0061ea80 (6 dwords), ret 0x18.
void anim(void* w, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) { joan::orig::as_widget(w)->scale_size(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(f)); }
// 0x0061e020 move_to(x, y, speed, accel, instant, arg), ret 0x18.
void move(void* w, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) { joan::orig::as_widget(w)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f)); }
// 0x006299b0 (7 dwords), ret 0x1c.
void layout(void* w, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g) { joan::orig::as_label(w)->set_layout(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(f), joan::orig::arg::u32(g)); }
// 0x006298c0 set_name / text, ret 4.
void set_text(void* w, const char* s) { joan::orig::as_image2d(w)->set_name(joan::orig::arg::str(s)); }
// 0x00629bd0 (2 dwords), ret 8.
void draw(void* w, u32 a, u32 b) { joan::orig::as_label(w)->draw(joan::orig::arg::u32(a), joan::orig::arg::u32(b)); }
// 0x004ceef0 on the speech player (name), ret 4.
void speech(img::Addr name) { joan::orig::as_sound(obj(kSpeech))->play_def(str(name)); }  // 0x004ceef0
// Ending text getters on 0x00a57c08 (thiscall, ret 0).
const char* text_of(std::uintptr_t thunk) { return thiscall<const char*>(thunk, obj(kStrings)); }
std::uint8_t* image(img::Addr name) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bmp_res(obj(kImages))->find(joan::orig::arg::str(str(name)))); }  // 0x00620b40

// fild qword (zero-extended dword) ; fcomp dword constant: true when not "below or unordered".
bool reached(u32 t, img::Addr limit) { return static_cast<double>(t) >= static_cast<double>(global<float>(limit)); }
// fld dword ; fcomp 0.0 (0x009cf5c4) ; test ah, 0x40 ; je skip: equal or unordered.
bool zero(float v) {
    const float z = global<float>(d::k_9cf5c4);
    return !(v < z) && !(v > z);
}

JOAN_COUNTER(c_draw, "Ending::draw");

struct H {
    jo::Label* title() { return &jo::self<jo::Ending>(this)->title; }
    jo::Image2D* body() { return &jo::self<jo::Ending>(this)->text; }
    jo::Image2D* frame() { return &jo::self<jo::Ending>(this)->frame; }
    std::int32_t state() { return jo::self<jo::Ending>(this)->score; }
    std::int32_t variant() { return jo::self<jo::Ending>(this)->kind; }
    bool started() { return jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) != 0; }
    void tick() { jo::self<jo::Ending>(this)->u32_22d = jo::self<jo::Ending>(this)->u32_22d + global<u32>(d::g_frame_elapsed_ms); }
    bool reached(img::Addr limit) { return game_ui3::reached(jo::self<jo::Ending>(this)->u32_22d, limit); }
    bool elapsed(u32 ms) { return !(jo::self<jo::Ending>(this)->u32_22d < ms); }
    bool zero(float v) { return game_ui3::zero(v); }
    void set_state(std::int16_t s) { jo::self<jo::Ending>(this)->score = s; }
    void reset_timer() { jo::self<jo::Ending>(this)->u32_22d = 0; }
    void reload() {
        joan::orig::as_ending(this)->free_pictures();  // 0x006d2280
        joan::orig::as_ending(this)->load_pictures();  // 0x006d1f20
    }
    void name(img::Addr s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(title()->sprite_name)), joan::orig::arg::str(str(s))); }  // strcpy
    // 0x0061e370 on +0x8 with the image's word size: the image is looked up twice, height
    // (+0x36) first, then width (+0x34); the callee reads only the low words. ret 0x1c.
    void picture(img::Addr s) {
        const u32 h = jo::as_bmp_entry(image(s))->height;
        const u32 wd = jo::as_bmp_entry(image(s))->width;
        joan::orig::as_widget(title())->resize_to(joan::orig::arg::u32(wd), joan::orig::arg::u32(h), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), 0u, 1u, 0u);
    }
    // strcpy / strcat "\n" / strcat into a 0x400-byte stack buffer.
    void two_lines(char* buf, std::uintptr_t first, std::uintptr_t second) {
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(text_of(first)));
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(buf)), str(d::kNl));  // "\n"
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(text_of(second)));
    }
    void game_over() {
        joan::orig::as_handle(obj(d::g_scenarios))->release(0u);  // 0x0041c780
        joan::orig::as_ending(this)->reset();                 // 0x006d1d30
        joan::orig::as_game(obj(d::g_game_a50088))->set_state(0xfu);  // 0x004d7e70
    }

    // 0x006ce450. ret 0.
    void draw_ending() {
        // canvas: a whole 1024x768 screen, also when the mission's frame draws it (off only for a recording made before this)
        joan::canvas::ScreenScope canvas_scope(joan::hook::settings().fix_ending_layout ? 0x006ce450u : 0u);
        c_draw.hit();
        if (jo::view<std::int8_t>(jo::self<jo::Ending>(this)->is_open_) == 0) return;
        std::uint16_t alpha = 0xff;
        step_into(alpha);
        u32 c = 0;
        u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(0u, 0u, 0u, static_cast<u32>(static_cast<std::uint8_t>(alpha))));  // 0x0042a260
        joan::orig::as_sprite2d(obj(kSprites))->queue(0, 0, 0x400, 0x300, 1u, joan::orig::arg::u32(colour), joan::orig::arg::mptr(0u), 3, 0, 2, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0);  // 0x00632a70
        draw(title(), 1, 7);
        u32 c2 = 0;
        colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c2)->construct(0u, 0u, 0u, static_cast<u32>(static_cast<std::uint8_t>(alpha))));
        joan::orig::as_sprite_queue(obj(kSprites))->draw_named(0, 0, 1u, str(d::s_mask_t), joan::orig::arg::mptr(obj(kImages)), joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500 'mask_t'
        u32 c3 = 0;
        colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c3)->construct(0u, 0u, 0u, static_cast<u32>(static_cast<std::uint8_t>(alpha))));
        joan::orig::as_sprite_queue(obj(kSprites))->draw_named(0, 0x280, 1u, str(d::s_mask_b), joan::orig::arg::mptr(obj(kImages)), joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 'mask_b'
        draw(body(), 1, 7);
        draw(frame(), 1, 7);
    }

    // 0x006ce472..0x006d0da4: the state machine (every branch ends at the draw tail).
    void step_into(std::uint16_t& alpha) {
        if (state() == 0) {
            tick();
            if (!reached(d::flt_3000)) {  // 3000
                const joan::hook::x87::Fpu fpu{true};
                const double v = fpu.mul(fpu.div(static_cast<double>(jo::self<jo::Ending>(this)->u32_22d), static_cast<double>(global<float>(d::flt_3000))),
                                         static_cast<double>(global<float>(d::flt_255)));  // 255
                alpha = static_cast<std::uint16_t>(joan::hook::x87::ftol(v));
            } else {
                set_state(1);
                reload();
                if (variant() == 0) {
                    name(d::s_good1);  // 'good1'
                    alpha = 0xff;
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffd17, 0xffffffcb, 0x3f800000, 0, 1, 0);
                    anim(title(), 0x3f2147ae, 0x42040000, 0, 3, 0, 0);
                    scale(body(), 1, 0x3f000000, 0, 0, 0x7d0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                } else if (variant() == 1) {
                    name(d::s_bad1);  // 'bad1'
                    alpha = 0xff;
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    anim(title(), 0x3f2e978d, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xffffff93, 0xffffffbd, 0x3f800000, 0, 1, 0);
                    anim(title(), 0x3faa9fbe, 0x41980000, 0, 4, 0, 0);
                    scale(body(), 1, 0x3f800000, 0, 1, 0);
                    move(body(), 0x190, 0xfffffed4, 0x42040000, 0, 0, 0x7d0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                }
                reset_timer();
            }
        } else if (state() == 1) {
            tick();
            if (variant() == 0) {
                if (reached(d::flt_21000)) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(2);
                } else if (reached(d::flt_19000)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x7d0)) {
                    speech(d::s_END_GOOD_1);  // 'END_GOOD_1'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            } else if (variant() == 1) {
                if (reached(d::flt_25700)) {
                    scale(body(), 0, 0x3e4ccccd, 0, 0, 0);
                    reset_timer();
                    set_state(2);
                }
                if (reached(d::flt_22200)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x9c4)) {
                    speech(d::s_END_BAD_1);  // 'END_BAD_1'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            }
        } else if (state() == 2) {
            if (variant() == 0) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(3);
                    reload();
                    name(d::s_good2);  // 'good2'
                    picture(d::s_good2);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffd17, 0xfffffe3b, 0x3f800000, 0, 1, 0);
                    move(title(), 0xffffffd3, 0xffffffcb, 0x42040000, 0, 0, 0);
                    set_text(body(), text_of(fn::code_value(fn::Ending_open_02)));  // 0x004de660
                    move(body(), 0x1c2, 0x300, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x3e8, 0x3c8, 0xa, 1, 1, 0, 0xffffffff);
                    scale(body(), 1, 0x3f800000, 0, 1, 0);
                    move(body(), 0x1c2, 0x1cc, 0x42340000, 0xc0400000, 0, 0x7d0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            } else if (variant() == 1) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(3);
                    reload();
                    name(d::s_bad2);  // 'bad2'
                    picture(d::s_bad2);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffd19, 0xfffffdf9, 0x3f800000, 0, 1, 0);
                    anim(title(), 0x3f1d70a4, 0x42180000, 0, 0, 0, 0);
                    set_text(body(), text_of(fn::code_value(fn::Ending_open_10_4de7e0)));  // 0x004de7e0
                    move(body(), 0x3c, 0x300, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x262, 0x42c, 0xa, 1, 1, 0, 0xffffffff);
                    scale(body(), 1, 0x3f800000, 0, 1, 0);
                    move(body(), 0x3c, 0xfffffed4, 0x421c0000, 0, 0, 0x7d0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            }
        } else if (state() == 3) {
            tick();
            if (variant() == 0) {
                if (reached(d::flt_21800)) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(4);
                }
                if (reached(d::flt_19800)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x7d0)) {
                    speech(d::s_END_GOOD_2);  // 'END_GOOD_2'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            } else if (variant() == 1) {
                if (reached(d::flt_23900)) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(4);
                }
                if (reached(d::flt_18900)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x9c4)) {
                    speech(d::s_END_BAD_2);  // 'END_BAD_2'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            }
        } else if (state() == 4) {
            if (variant() == 0) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(5);
                    reload();
                    name(d::s_good3);  // 'good3'
                    picture(d::s_good3);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffe71, 0xffffffcb, 0x3f800000, 0, 1, 0);
                    move(title(), 0xfffffe71, 0xfffffe3b, 0x42000000, 0, 0, 0);
                    set_text(body(), text_of(fn::code_value(fn::Ending_open_03)));  // 0x004de690
                    move(body(), 0x1c2, 0x300, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x3e8, 0x3c8, 0xa, 1, 1, 0, 0xffffffff);
                    scale(body(), 1, 0x3f800000, 0, 1, 0);
                    move(body(), 0x1c2, 0x1cc, 0x42340000, 0xc0400000, 0, 0x7d0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            } else if (variant() == 1) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(5);
                    set_text(body(), text_of(fn::code_value(fn::Menu_ending_b)));  // 0x004de810
                    move(body(), 0xed, 0, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x313, 0x300, 0xa, 1, 1, 0, 0xffffffff);
                    scale(body(), 1, 0x3e99999a, 0, 0, 0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            }
        } else if (state() == 5) {
            tick();
            if (variant() == 0) {
                if (reached(d::flt_8750)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                    reset_timer();
                    set_state(6);
                }
                if (!started() && elapsed(0x7d0)) {
                    speech(d::s_END_GOOD_3);  // 'END_GOOD_3'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            } else if (variant() == 1) {
                if (reached(d::flt_14200)) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(6);
                }
                if (!started() && elapsed(0x1f4)) {
                    speech(d::s_END_BAD_3);  // 'END_BAD_3'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            }
        } else if (state() == 6) {
            if (variant() == 0) {
                if (zero(title()->scale)) {
                    set_state(7);
                    reload();
                    name(d::s_good4);  // 'good4'
                    picture(d::s_good4);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xffffffc6, 0xffffffcb, 0x3f800000, 0, 1, 0);
                    anim(title(), 0x3f2147ae, 0x42480000, 0, 2, 0, 0);
                    reset_timer();
                }
            } else if (variant() == 1) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(7);
                    reload();
                    name(d::s_bad3);  // 'bad4'
                    picture(d::s_bad3);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xffffffcb, 0xffffffcb, 0x3f800000, 0, 1, 0);
                    move(title(), 0xfffffd17, 0xfffffe0a, 0x42480000, 0, 0, 0);
                    char text[0x400];  // ebp-0x408
                    two_lines(text, fn::code_value(fn::Ending_open_0c), fn::code_value(fn::Ending_open_0d));  // 0x004de840, 0x004de870
                    set_text(body(), text);
                    move(body(), 0xed, 0, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x313, 0x300, 0xa, 1, 1, 0, 0xffffffff);
                    scale(body(), 1, 0x3ecccccd, 0, 0, 0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            }
        } else if (state() == 7) {
            if (variant() == 0) {
                tick();
                if (reached(d::flt_8750)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                    reset_timer();
                    set_state(8);
                } else if (reached(d::flt_6750) && jo::view<std::int8_t>(jo::self<jo::Ending>(this)->text.scaling) == 0) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                }
            } else if (variant() == 1) {
                tick();
                if (reached(d::flt_38600)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(8);
                }
                if (jo::view<std::int8_t>(jo::self<jo::Ending>(this)->title.moving) == 0 && jo::view<std::int8_t>(jo::self<jo::Ending>(this)->title.resizing) == 0) {
                    anim(title(), 0x3f1d70a4, 0x42040000, 0, 0, 0, 0);
                }
                if (!started() && elapsed(0x1f4)) {
                    speech(d::s_END_BAD_4);  // 'END_BAD_4'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            }
        } else if (state() == 8) {
            if (variant() == 0) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(9);
                    reload();
                    name(d::s_good5);  // 'good5'
                    picture(d::s_good5);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffd17, 0xffffffcb, 0x3f800000, 0, 1, 0);
                    anim(title(), 0x3f0fdf3b, 0x42300000, 0, 3, 0, 0);
                    set_text(body(), text_of(fn::code_value(fn::Ending_open_4)));  // 0x004de6c0
                    move(body(), 0x1c2, 0x300, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x3e8, 0x3c8, 0xa, 1, 1, 0, 0xffffffff);
                    scale(body(), 1, 0x3f800000, 0, 1, 0);
                    move(body(), 0x1c2, 0x1cc, 0x42480000, 0xc0800000, 0, 0x5dc);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            } else if (variant() == 1) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(9);
                    reload();
                    name(d::s_bad4);  // 'bad5'
                    picture(d::s_bad4);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xffffffc9, 0xfffffeea, 0x3f800000, 0, 1, 0);
                    move(title(), 0xfffffd18, 0xfffffeea, 0x41e00000, 0, 0, 0);
                    char text[0x400];  // ebp-0x808
                    two_lines(text, fn::code_value(fn::Menu_ending_e), fn::code_value(fn::Ending_open_15));  // 0x004de8a0, 0x004de8d0
                    set_text(body(), text);
                    move(body(), 0x41, 0x300, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x267, 0x42c, 0xa, 1, 1, 0, 0xffffffff);
                    scale(body(), 1, 0x3f800000, 0, 1, 0);
                    move(body(), 0x41, 0xfffffed4, 0x420c0000, 0, 0, 0x5dc);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            }
        } else if (state() == 9) {
            if (variant() == 0) {
                tick();
                if (reached(d::flt_17700)) {
                    reset_timer();
                    set_state(0xa);
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                }
                if (reached(d::flt_14700)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x5dc)) {
                    speech(d::s_END_GOOD_4);  // 'END_GOOD_4'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            } else if (variant() == 1) {
                tick();
                if (reached(d::flt_25000)) {
                    reset_timer();
                    set_state(0xa);
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                }
                if (reached(d::flt_20000)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x7d0)) {
                    speech(d::s_END_BAD_5);  // 'END_BAD_5'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            }
        } else if (state() == 0xa) {
            if (variant() == 0) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(0xb);
                    reload();
                    name(d::s_good6);  // 'good6'
                    picture(d::s_good6);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffd1c, 0xfffffeeb, 0x3f800000, 0, 1, 0);
                    move(title(), 0xffffffc6, 0xfffffeeb, 0x41f00000, 0, 0, 0);
                    scale(body(), 0, 0x3f800000, 0, 1, 0);
                    move(body(), 0x32, 0x1cc, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x258, 0x294, 0xa, 1, 1, 0, 0xffffffff);
                    set_text(body(), text_of(fn::code_value(fn::Ending_open_05)));  // 0x004de6f0
                    scale(body(), 1, 0x3f000000, 0, 0, 0x7d0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            } else if (variant() == 1) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(0xb);
                    reload();
                    name(d::s_bad5);  // 'bad6'
                    picture(d::s_bad5);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0x7d0);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffe71, 0xfffffdfe, 0x3f800000, 0, 1, 0);
                    anim(title(), 0x3f1df3b6, 0x41f00000, 0, 6, 0, 0);
                    scale(body(), 0, 0x3f800000, 0, 1, 0);
                    move(body(), 0xed, 0, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x313, 0x300, 0xa, 1, 1, 0, 0xffffffff);
                    set_text(body(), text_of(fn::code_value(fn::Ending_open_10_4de900)));  // 0x004de900
                    scale(body(), 1, 0x3f000000, 0, 0, 0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            }
        } else if (state() == 0xb) {
            if (variant() == 0) {
                tick();
                if (reached(d::flt_20800)) {
                    reset_timer();
                    set_state(0xc);
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                }
                if (reached(d::flt_17800)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x7d0)) {
                    speech(d::s_END_GOOD_5);  // 'END_GOOD_5'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            } else if (variant() == 1) {
                tick();
                if (reached(d::flt_16100)) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(0xc);
                }
                if (!started() && elapsed(0x1f4)) {
                    speech(d::s_END_BAD_6);  // 'END_BAD_6'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            }
        } else if (state() == 0xc) {
            if (variant() == 0) {
                if (zero(title()->scale) && zero(body()->scale)) {
                    set_state(0xd);
                    reload();
                    name(d::s_good7);  // 'good7'
                    picture(d::s_good7);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    anim(title(), 0x3f1d70a4, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xffffffcb, 0xffffffba, 0x3f800000, 0, 1, 0);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0xbb8);
                    anim(title(), 0x3fd04189, 0x42140000, 0, 5, 0, 0);
                    scale(body(), 0, 0x3f800000, 0, 1, 0);
                    move(body(), 0xed, 0, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x313, 0x300, 0xa, 1, 1, 0, 0xffffffff);
                    set_text(body(), text_of(fn::code_value(fn::Menu_ending_6)));  // 0x004de720
                    scale(body(), 1, 0x3f000000, 0, 0, 0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            } else if (variant() == 1) {
                if (zero(body()->scale)) {
                    set_state(0xd);
                    scale(body(), 0, 0x3f800000, 0, 1, 0);
                    move(body(), 0xed, 0, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x313, 0x300, 0xa, 1, 1, 0, 0xffffffff);
                    set_text(body(), text_of(fn::code_value(fn::Ending_open_17)));  // 0x004de930
                    scale(body(), 1, 0x3f000000, 0, 0, 0);
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 0;
                    reset_timer();
                }
            }
        } else if (state() == 0xd) {
            if (variant() == 0) {
                tick();
                if (reached(d::flt_22200) && jo::view<std::int8_t>(jo::self<jo::Ending>(this)->text.scaling) == 0) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(0xe);
                } else if (reached(d::flt_17200)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started()) {
                    speech(d::s_END_GOOD_6);  // 'END_GOOD_6'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            } else if (variant() == 1) {
                tick();
                if (reached(d::flt_10700)) {
                    scale(body(), 0, 0x3e99999a, 0, 0, 0);
                    reset_timer();
                    set_state(0xe);
                }
                if (reached(d::flt_7700) && jo::view<std::int8_t>(jo::self<jo::Ending>(this)->title.scaling) == 0) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (!started() && elapsed(0x1f4)) {
                    speech(d::s_END_BAD_7);  // 'END_BAD_7'
                    jo::view<std::int8_t>(jo::self<jo::Ending>(this)->u8_235) = 1;
                }
            }
        } else if (state() == 0xe) {
            if (variant() == 0) {
                if (zero(body()->scale)) {
                    move(body(), 0xed, 0, 0x3f800000, 0, 1, 0);
                    layout(body(), 0x313, 0x300, 0xa, 1, 1, 0, 0xffffffff);
                    set_text(body(), text_of(fn::code_value(fn::Menu_ending_7)));  // 0x004de750
                    scale(body(), 1, 0x3f000000, 0, 0, 0);
                    speech(d::s_END_GOOD_7);  // 'END_GOOD_7'
                    reset_timer();
                    set_state(0xf);
                }
                if (zero(title()->scale)) {
                    reload();
                    name(d::s_good8);  // 'good8'
                    picture(d::s_good8);
                    anim(title(), 0x3fe3b646, 0x3f800000, 0, 0, 1, 0);
                    move(title(), 0xfffffe71, 0xfffffdee, 0x3f800000, 0, 1, 0);
                    anim(title(), 0x3f1df3b6, 0x41f00000, 0, 6, 0, 0);
                    scale(title(), 1, 0x3e4ccccd, 0, 0, 0xbb8);
                }
            } else if (variant() == 1) {
                if (zero(body()->scale)) {
                    set_state(0xf);
                    scale(frame(), 1, 0x3f000000, 0, 0, 0);
                    reset_timer();
                }
            }
        } else if (state() == 0xf) {
            if (variant() == 0) {
                tick();
                if (reached(d::flt_19500)) {
                    scale(title(), 0, 0x3e4ccccd, 0, 0, 0);
                    scale(body(), 0, 0x3f000000, 0, 0, 0);
                }
                if (zero(body()->scale)) {
                    scale(frame(), 1, 0x3f000000, 0, 0, 0);
                    reset_timer();
                    set_state(0x10);
                }
            } else if (variant() == 1) {
                tick();
                if (reached(d::flt_4000)) {
                    scale(frame(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (zero(frame()->scale)) game_over();
            }
        } else if (state() == 0x10) {
            if (variant() == 0) {
                if (zero(title()->scale)) {
                    scale(frame(), 0, 0x3e4ccccd, 0, 0, 0);
                }
                if (zero(frame()->scale)) game_over();
            }
        }
    }
};

JOAN_REPLACE("game/ui3", "Ending::draw", 0x006ce450, &H::draw_ending);
JOAN_CANVAS_SELF(0x006ce450);  // ScreenScope in the body

}  // namespace game_ui3
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
//
//      0x006d1980 Obj::refresh_6d1980      (thiscall, ret 0) - three 0x0061f3b0 when +0x234.
namespace {
namespace game_b108_17 {

struct H {
    // ------------------------------------------------------------------------ 0x006d1980
    std::uintptr_t refresh_6d1980() {  // eax: this or 0x0061f3b0's result (the only caller drops it)
        if (jo::view<i8>(jo::self<jo::Ending>(this)->is_open_) == 0) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->title)->update();                 // 0x0061f3b0
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->text)->update();
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_widget(&jo::self<jo::Ending>(this)->frame)->update());
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Obj::refresh_6d1980", 0x006d1980, &H::refresh_6d1980);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
//
//      0x006d19e0 EndScreen::open         (thiscall(char kind), ret 4) - good1 / bad1 layout,
//                 ENDING menu text, mouse, resume game speed. Returns the original's eax
//                 (0x0042f930's result, or 0 when the speed flag is clear).
//
//      0x006d3940 PopupScreen::init_buttons (thiscall, ret 0) - three b_ot/b_dw/b_ov buttons with
//                 G_POPUP 1/2/3 texts; returns this (original's eax).
namespace {
namespace game_b101_15 {

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

void* const kMouse = obj(d::g_mouse);
void* const kMenus = obj(d::g_strings);
void* const kSprites = obj(d::g_bmp_ending);
void* const kFont = obj(d::g_font_goudy14_22_white);

u32 button_anim(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    joan::orig::as_widget(b)->scale_to(joan::orig::arg::i16(a0), joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), static_cast<char>(a3), joan::orig::arg::u32(a4));  // 0x0061f1b0
    return 0;  // void callee; results dropped
}
u32 button_move(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    joan::orig::as_widget(b)->move_to(joan::orig::arg::i16(a0), joan::orig::arg::i16(a1), joan::orig::arg::f32(a2), joan::orig::arg::f32(a3), static_cast<char>(a4), joan::orig::arg::u32(a5));  // 0x0061e020
    return 0;  // void callee; results dropped
}

JOAN_COUNTER(c_end_open, "EndScreen::open");
JOAN_COUNTER(c_popup_buttons, "PopupScreen::init_buttons");

struct H {
    // 0x006d19e0 (ret 4)
    u32 end_open(i8 kind) {
        c_end_open.hit();
        jo::self<jo::Ending>(this)->is_open_ = 1;
        jo::self<jo::Ending>(this)->kind = kind;
        const i32 k = kind;
        if (k == 0 || k == 1) {
            const char* const name = k == 0 ? str(d::s_good1) : str(d::s_bad1);  // good1 / bad1
            button_anim(&jo::self<jo::Ending>(this)->title, 0u, 0x3f800000u, 0u, 1u, 0u);
            // Argument evaluation order of the original: +0x36 first, then +0x34.
            const i16 h = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(kSprites)->find(joan::orig::arg::str(name))))->height);  // 0x00620b40
            const i16 w = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(kSprites)->find(joan::orig::arg::str(name))))->width);  // 0x00620b40
            joan::orig::as_widget(&jo::self<jo::Ending>(this)->title)->resize_to(joan::orig::arg::u32(static_cast<i32>(w)), joan::orig::arg::u32(static_cast<i32>(h)), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), 0u, 1u, 0u);  // 0x0061e370 (reads both as words)
            if (k == 0) {
                button_move(&jo::self<jo::Ending>(this)->title, 0xfffffd17u, 0xffffffcbu, 0x3f800000u, 0u, 1u, 0u);
                button_anim(&jo::self<jo::Ending>(this)->text, 0u, 0x3f800000u, 0u, 1u, 0u);
                button_move(&jo::self<jo::Ending>(this)->text, 0x28u, 0x14u, 0x3f800000u, 0u, 1u, 0u);
                joan::orig::as_label(&jo::self<jo::Ending>(this)->text)->set_layout(0x24eu, 0xdcu, 0xau, 1u, 1u, 0u, 0xffffffffu);  // 0x006299b0
                const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_ending(kMenus)->open_1());  // 0x004de630 ENDING 1
                joan::orig::as_image2d(&jo::self<jo::Ending>(this)->text)->set_name(joan::orig::arg::str(text));         // 0x006298c0
            } else {
                button_move(&jo::self<jo::Ending>(this)->title, 0xffffff93u, 0xffffffbdu, 0x3f800000u, 0u, 1u, 0u);
                button_anim(&jo::self<jo::Ending>(this)->text, 0u, 0x3f800000u, 0u, 1u, 0u);
                button_move(&jo::self<jo::Ending>(this)->text, 0x190u, 0x300u, 0x3f800000u, 0u, 1u, 0u);
                joan::orig::as_label(&jo::self<jo::Ending>(this)->text)->set_layout(0x3b6u, 0x42cu, 0xau, 1u, 1u, 0u, 0xffffffffu);  // 0x006299b0
                const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_ending(kMenus)->open_09());  // 0x004de7b0 ENDING 9
                joan::orig::as_image2d(&jo::self<jo::Ending>(this)->text)->set_name(joan::orig::arg::str(text));         // 0x006298c0
            }
        }
        button_anim(&jo::self<jo::Ending>(this)->frame, 0u, 0x3f800000u, 0u, 1u, 0u);
        jo::self<jo::Ending>(this)->text.font = static_cast<jo::Font2d*>(img::obj(d::g_font_dauphin_14_bold));
        jo::view<u16>(jo::self<jo::Ending>(this)->score) = 0;
        jo::self<jo::Ending>(this)->u32_22d = 0;
        joan::orig::as_ending(this)->load_pictures();   // 0x006d1f20
        joan::orig::as_cursor(kMouse)->set_pos();  // 0x00592bd0
        if (global<u8>(d::g_game_running) == 0) return 0;  // eax = zero-extended flag
        global<u8>(d::g_game_running) = 0;
        global<i16>(d::g_saved_game_speed) = static_cast<i16>(ftol(global<float>(d::g_game_speed)));
        joan::orig::as_game(kSpeed)->set_speed(joan::orig::arg::bits(0.0f));  // 0x0042f930
        return 0;  // void callee; EndScreen::open callers drop the eax
    }

    // 0x006d3940 (ret)
    void* popup_buttons() {
        c_popup_buttons.hit();
        static const std::uintptr_t kText[3] = {fn::code_value(fn::GPopup_open_1), fn::code_value(fn::GPopup_open_02), fn::code_value(fn::GPopup_open_03)};  // 0x004de4a0/4d0/500 G_POPUP 1/2/3
        // the popup 0x00a52df8 is the message box (MessageBox::button0..2)
        static jo::Button jo::MessageBox::* const kButton[3] = {&jo::MessageBox::button0, &jo::MessageBox::button1, &jo::MessageBox::button2};
        for (int i = 0; i < 3; ++i) {
            void* const b = &(jo::self<jo::MessageBox>(this)->*kButton[i]);
            joan::orig::as_menu_button(b)->setup(joan::orig::arg::i16(0u), joan::orig::arg::i16(0u), joan::orig::arg::mptr(img::addr(d::g_set)), str(d::s_b_ot), str(d::s_b_dw), str(d::s_b_ov), joan::orig::arg::str(0u), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x006211d0 b_ot / b_dw / b_ov
            u8 colour_obj[8];
            void* const c = joan::orig::as_colour(colour_obj)->construct(0xffu, 0xffu, 0xffu, 0xffu);  // 0x0042a260
            const u32 colour = jo::view<u32>(*jo::as_colour(c));
            const char* const text = thiscall<const char*>(kText[i], kMenus);
            joan::orig::as_button(b)->set_text(text, kFont, joan::orig::arg::i32(colour), 1, 2, 0, 2, 2);  // 0x00623ce0
        }
        jo::view<u16>(jo::self<jo::MessageBox>(this)->x) = 0xffff;
        jo::view<u16>(jo::self<jo::MessageBox>(this)->y) = 0xffff;
        jo::self<jo::MessageBox>(this)->font = jo::object<jo::Font2d>(img::obj(d::g_font_goudy14_22_white));
        jo::self<jo::MessageBox>(this)->text_box = jo::object<jo::Font2d>(img::obj(d::g_font_goudy14_22_white));
        return this;
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("EndScreen::open", 0x006d19e0, &H::end_open);
B15("PopupScreen::init_buttons", 0x006d3940, &H::popup_buttons);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b82_12.cpp (game/b82_12)
//
//      0x006d1f20 Ending::load_pictures() - loads G_END1..8 / B_END1..5 (by +0x233, +0x231)
//                 into +0 and ENDING into +4 with loader flag 1
//
//      0x006d1d30 Ending::reset() - frees the pictures and resets the screen widgets
//
//      0x006d2280 Ending::free_pictures() - frees +0 and (unless +0x234) +4
//
//      0x006d5080 Dialog_a52df8::init() - 0x006d50e0(0x1c8, -1), +0x454, byte +4, 0x006d3cb0
namespace {
namespace game_b82_12 {

constexpr img::Addr kPictures(d::g_texture_array);

JOAN_COUNTER(c_end_load, "Ending::load_pictures");
JOAN_COUNTER(c_end_reset, "Ending::reset");

struct H {
    // 0x006d1f20 (ret); returns 0x0060a740's eax.
    std::uint32_t end_load() {
        c_end_load.hit();
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(img::obj(kPictures))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(img::obj(kPictures))->set_mode(1u);                           // 0x0060a740
        if (jo::self<jo::Ending>(this)->picture == 0) {
            const std::int32_t flag = jo::self<jo::Ending>(this)->kind;
            const char* name = nullptr;
            if (flag == 0) {
                const std::int32_t n = jo::self<jo::Ending>(this)->score;
                if (n < 3) name = &img::global<const char>(d::s_G_END1);        // G_END1
                else if (n < 5) name = &img::global<const char>(d::s_G_END2);   // G_END2
                else if (n < 7) name = &img::global<const char>(d::s_G_END3);   // G_END3
                else if (n < 9) name = &img::global<const char>(d::s_G_END4);   // G_END4
                else if (n < 0xb) name = &img::global<const char>(d::s_G_END5); // G_END5
                else if (n < 0xd) name = &img::global<const char>(d::s_G_END6); // G_END6
                else if (n < 0xf) name = &img::global<const char>(d::s_G_END7); // G_END7
                else if (n < 0x11) name = &img::global<const char>(d::s_G_END8); // G_END8
            } else if (flag == 1) {
                const std::int32_t n = jo::self<jo::Ending>(this)->score;
                if (n < 3) name = &img::global<const char>(d::s_B_END1);        // B_END1
                else if (n < 5) name = &img::global<const char>(d::s_B_END2);   // B_END2
                else if (n < 9) name = &img::global<const char>(d::s_B_END3);   // B_END3
                else if (n < 0xb) name = &img::global<const char>(d::s_B_END4); // B_END4
                else if (n < 0xd) name = &img::global<const char>(d::s_B_END5); // B_END5
            }
            if (name != nullptr) jo::self<jo::Ending>(this)->picture = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kPictures))->load(joan::orig::arg::str(name), 1, 0u));  // 0x006099c0
        }
        if (jo::self<jo::Ending>(this)->picture2 == 0)
            jo::self<jo::Ending>(this)->picture2 =
                joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kPictures))->load(joan::orig::arg::str(&img::global<const char>(d::s_ENDING)), 1, 0u));  // 0x006099c0 ENDING
        joan::orig::as_texture_manager(img::obj(kPictures))->set_mode(saved);  // 0x0060a740
        return 0;  // void callee; Ending::load_pictures's caller drops the eax
    }

    // 0x006d1d30 (ret)
    void end_reset() {
        c_end_reset.hit();
        jo::self<jo::Ending>(this)->is_open_ = 0;
        joan::orig::as_ending(this)->free_pictures();  // 0x006d2280
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->title)->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061f1b0
        const char* const good1 = &img::global<const char>(d::s_good1);
        // Argument evaluation order of the original: +0x36 first, then +0x34.
        const std::int16_t h = jo::view<std::int16_t>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(img::obj(d::g_bmp_ending))->find(joan::orig::arg::str(good1))))->height);  // 0x00620b40
        const std::int16_t w = jo::view<std::int16_t>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(img::obj(d::g_bmp_ending))->find(joan::orig::arg::str(good1))))->width);  // 0x00620b40
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->title)->resize_to(joan::orig::arg::u32(static_cast<std::int32_t>(w)), joan::orig::arg::u32(static_cast<std::int32_t>(h)), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), 0u, 1u, 0u);  // 0x0061e370 (reads both as words)
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->title)->move_to(-745, -53, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061e020
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->text)->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);                     // 0x0061f1b0
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->text)->move_to(0x28, 0x14, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);            // 0x0061e020
        joan::orig::as_label(&jo::self<jo::Ending>(this)->text)->set_layout(0x24eu, 0xdcu, 2u, 1u, 1u, 0u, 0xffffffffu);      // 0x006299b0
        joan::orig::as_widget(&jo::self<jo::Ending>(this)->frame)->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);                    // 0x0061f1b0
        jo::view<std::uint16_t>(jo::self<jo::Ending>(this)->score) = 0;
        jo::view<std::uint8_t>(jo::self<jo::Ending>(this)->kind) = 0;
        joan::orig::as_cursor(img::obj(d::g_mouse))->reset_pos();  // 0x00592c30
        if (global<std::uint8_t>(d::g_game_running) == 0) {
            global<std::uint8_t>(d::g_game_running) = 1;
            const float v = static_cast<float>(static_cast<std::int32_t>(global<std::int16_t>(d::g_saved_game_speed)));  // exact
            joan::orig::as_game(img::obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(v));  // 0x0042f930
        }
    }

    // 0x006d2280 (ret)
    void end_free() {
        if (jo::self<jo::Ending>(this)->picture != 0) {
            joan::orig::as_texture_manager(img::obj(kPictures))->release(joan::orig::arg::mptr(jo::self<jo::Ending>(this)->picture));  // 0x0060a550
            jo::self<jo::Ending>(this)->picture = 0;
        }
        if (jo::self<jo::Ending>(this)->picture2 != 0 && static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Ending>(this)->is_open_)) == 0) {
            joan::orig::as_texture_manager(img::obj(kPictures))->release(joan::orig::arg::mptr(jo::self<jo::Ending>(this)->picture2));  // 0x0060a550
            jo::self<jo::Ending>(this)->picture2 = 0;
        }
    }

    // 0x006d5080 (ret); returns 0x006d3cb0's eax.
    std::uint32_t dialog_init() {
        joan::orig::as_panel(this)->set_pos(joan::orig::arg::i16(0x1c8), joan::orig::arg::i16(-1));  // 0x006d50e0
        jo::self<jo::MessageBox>(this)->text_box = jo::object<jo::Font2d>(img::obj(d::g_font_goudy14_22_white));  // this = the message box 0x00a52df8 (all callers)
        jo::self<jo::Dialog>(this)->u8_4 = 0;
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_popup(this)->draw(1, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1)));  // 0x006d3cb0
    }
};

#define B12(name, addr, fn) JOAN_REPLACE("game/b82_12", name, addr, fn)
B12("Ending::load_pictures", 0x006d1f20, &H::end_load);
B12("Ending::reset", 0x006d1d30, &H::end_reset);
B12("Ending::free_pictures", 0x006d2280, &H::end_free);
B12("Dialog::init", 0x006d5080, &H::dialog_init);
#undef B12

}  // namespace game_b82_12
}  // namespace

// ---------------------------------------- from modules/game_b116_10.cpp (game/b116_10)
//
//    - TalkPrompt::draw 0x006d23b0 (thiscall, ret 0, caller 0x004d60b0 ignores eax): nothing while
//      0x004301c0 / 0x0042ced0 / byte 0xa48a38 / 0x006e0a90 / 0x006d37c0 report a blocking state or
//      byte +8 is clear. Game mode 4 (0xa5b9f8): the "units still to recruit" box of the unit +0x1d
//      (0x004de080 text). Otherwise the action prompt (first that applies: unit +0x19 flag +0x2e2,
//      unit +0x11 name, +0x25, item object +0xd (kind +0x4e 1 / 0xb / 0xa / other, item type +0x20),
//      +0x21, +0x29, quest object +0x2d with the ALENCON distance test) in a BG + BORDER box, then
//      the hire price of the unit +0x15 (type +0x149 >= 3: unit name, else type name; price
//      +0x135 * 0xa3cd10 / 0xa3cc54 * max(0.5, (+0x124 - 21) / 10 + 1), * 1.6 for MER_EX_ARCHER /
//      MER_EX_ARCHER_CAPTAIN; "not enough gold" text when 0xa596c4 < price) with 0x00626320.
//      Canvas: wrapped (hud_top) by canvas_hooks.cpp; the replacement needs nothing.
namespace {
namespace game_b116_10 {

using joan::hook::x87::ftol;

using joan::call::import_call;
using joan::orig::helpers::at;
u8* bp(void* p) { return static_cast<u8*>(p); }
template <class R = void>
R vcall(const void* self, u32 offset) {  // thiscall virtual without arguments (vtable byte offset)
    return thiscall<R>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(self)[joan::call::x86_vslot(offset)]), self);
}
u32 w16(i16 v) { return static_cast<u32>(static_cast<u16>(v)); }  // word argument

JOAN_COUNTER(c_talk, "TalkPrompt::draw");

void* const kStrings = obj(d::g_strings);
void* const kSprites = obj(d::g_sprites_a46128);
void* const kText = obj(d::g_text_a58ac0);
void* const kItems = obj(d::g_item_res);
void* const kObjects = obj(d::g_a50530);

struct Path {  // 0x005a1170 string: char[0x190] + NUL
    char s[0x191];
};
void path_ctor(Path& p) { joan::orig::as_string(&p)->construct(); }              // 0x005a1170
void path_assign(Path& p, const void* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
char* c_str(Path& p) { return joan::orig::as_string(&p)->c_str(); }  // 0x00417d90

u32 colour(u32 v) {  // 0x005e9640 (ret 4) into a local, copied
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(v)));
}
i32 unit_missing(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* unit_type(u8* u) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(w16(jo::as_unit(u)->type_id)))); }  // 0x0042d0a0
u8* item_type(u32 word) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(word))); }  // 0x00425ce0
const char* key_name() {                                                           // 0x004cb5b0
    return joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(global<u32>(d::g_control_mode)), 0x11));
}
template <class... A>
const char* format(A... args) {  // 0x005a6660 (cdecl, varargs)
    return cdecl_call<const char*>(fn::format_positional, args...);
}
// 0x00633500 (thunk 0x00401bbd, ret 0x44): the BG / BORDER box pieces.
void box_piece(u8* self, img::Addr name) {
    const u32 c = colour(0xffffffffu);
    joan::orig::as_sprite_queue(kSprites)->draw_named(0x180, 0x64, 6u, joan::orig::arg::str(str(name)), joan::orig::arg::mptr(jo::object<jo::TalkPrompt>(self)->sheet), joan::orig::arg::u32(c), 3, 0, 0, 1, -1, -1, -1, -1, 0, -1, -1);
}
// 0x00626e00 (thunk 0x0040a3ee, ret 0x3c): the box text.
void box_text(const void* text, u32 c) {
    joan::orig::as_text_renderer(kText)->draw_wrapped(joan::orig::arg::u32(i32{0x188}), joan::orig::arg::u32(i32{0x6a}), joan::orig::arg::u32(i32{0x28c}), joan::orig::arg::u32(i32{0xa2}), joan::orig::arg::u32(i32{6}), joan::orig::arg::str(text), joan::orig::arg::u32(c), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::u32(i32{2}));
}

// ---- 0x006d23b0 TalkPrompt::draw ---------------------------------------------------------------
struct HTalk {
    // [ebp-0x360]: hire price of the unit +0x15.
    static i32 price_of(u8* u, img::Addr factor) {
        const i32 lv = static_cast<i32>(jo::as_unit(u)->level) - 0x15;
        float s;  // [ebp-0x3bc] / [ebp-0x3cc]
        const double probe = Fpu{true}.add(Fpu{true}.div(static_cast<double>(lv), static_cast<double>(global<float>(d::flt_10))),
                                           static_cast<double>(global<float>(d::flt_1)));
        if (!(probe >= static_cast<double>(global<float>(d::flt_0_5)))) {  // fcomp, test ah,1 (< or NaN)
            s = 0.5f;                                                        // 0x3f000000
        } else {
            s = static_cast<float>(Fpu{true}.add(Fpu{true}.div(static_cast<double>(lv), static_cast<double>(global<float>(d::flt_10))),
                                                 static_cast<double>(global<float>(d::flt_1))));
        }
        const i32 base = static_cast<i32>(static_cast<u32>(jo::as_unit(u)->hit_points) * static_cast<u32>(global<i32>(factor)));  // imul
        return ftol(Fpu{true}.mul(static_cast<double>(base), static_cast<double>(s)));
    }

    void draw() {
        joan::canvas::ScreenScope canvas_scope(0x006d23b0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_talk.hit();
        u8* const self = bp(this);
        if (static_cast<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0 ||  // 0x004301c0
            static_cast<i8>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) != 0 ||  // 0x0042ced0
            global<i8>(d::g_fade_title) != 0)
            return;
        if (static_cast<i8>(joan::orig::as_letter(obj(d::g_letter))->get_8()) != 0) return;  // 0x006e0a90
        if (static_cast<i8>(joan::orig::as_player(obj(d::g_player_unit_recno))->flag_ac()) != 0) return;  // 0x006d37c0
        if (jo::self<jo::TalkPrompt>(this)->visible == 0) return;

        if (global<i32>(d::g_control_mode) == 4) {
            // Recruiting: the box when units of the leader's group are still missing.
            if (unit_missing(jo::self<jo::TalkPrompt>(this)->leader) != 0) return;
            u8* const leader = unit(jo::self<jo::TalkPrompt>(this)->leader);  // [ebp-0x8]
            i32 missing = 0;                        // [ebp-0x10]
            for (i32 i = 0; i < jo::as_unit_type(unit_type(leader))->slot_count; ++i) {
                if (unit_missing(jo::view<u32*>(jo::as_unit(leader)->mask_slots)[i]) == 0) ++missing;  // ([ebp-0xc] = 1, never read)
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->hostile_to(joan::orig::arg::i16(static_cast<u32>(global<u16>(d::g_player_unit_recno))))) != 0) return;  // 0x005094d0
            if (missing >= jo::as_unit_type(unit_type(leader))->slot_count) return;
            box_piece(self, d::s_BG);  // "BG"
            box_piece(self, d::s_BORDER);  // "BORDER"
            const u32 c = colour(0xffffffffu);
            const char* const name = jo::as_unit_type(unit_type(leader))->title_b4;
            const i32 n = jo::as_unit_type(unit_type(leader))->slot_count;
            box_text(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->rts_text_1(joan::orig::arg::u32(n), name)), c);  // 0x004de080
            return;
        }

        bool done = false;   // [ebp-0x18]
        bool priced = false;  // [ebp-0x1c]
        Path msg;            // [ebp-0x1b0]
        path_ctor(msg);
        if (!done && unit_missing(jo::self<jo::TalkPrompt>(this)->near_c) == 0) {
            u8* const u = unit(jo::self<jo::TalkPrompt>(this)->near_c);  // [ebp-0x1b4]
            if (jo::as_unit(u)->prisoner_pose != 0) {
                done = true;
                const char* const k = key_name();
                path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_menu(kStrings)->gmsg_04()), k));  // 0x004da930
            }
        }
        if (!done && jo::self<jo::TalkPrompt>(this)->near_a != 0) {
            done = true;
            const char* const k = key_name();
            const char* const name = jo::as_unit(unit(jo::self<jo::TalkPrompt>(this)->near_a))->name_;
            path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_text_2(name)), k));  // 0x004da8a0
        }
        if (!done && jo::self<jo::TalkPrompt>(this)->enemy != 0) {
            done = true;
            const char* const k = key_name();
            path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_text_12()), k));  // 0x004daac0
        }
        if (!done && joan::orig::arg::as<i32>(joan::orig::as_types(kObjects)->empty_at(joan::orig::arg::i32(jo::self<jo::TalkPrompt>(this)->object))) == 0) {  // 0x00485410
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_list(kObjects)->at_plus4(joan::orig::arg::u32(jo::self<jo::TalkPrompt>(this)->object)));     // 0x004853e0, [ebp-0x1b8]
            const i32 kind = jo::as_map_object(o)->class_id;
            if (kind == 1) {
                if (jo::as_item_def(item_type(w16(jo::as_map_object(o)->type_id)))->description != nullptr) {
                    done = true;
                    const i32 b = jo::object<jo::Gate>(vcall<u8*>(o, 0x4c))->state;  // the object as its Gate (Gate::state)
                    const char* const k = key_name();
                    const char* const s = jo::as_item_def(item_type(w16(jo::as_map_object(o)->type_id)))->description;
                    path_assign(msg, format(s, k, b));
                }
            } else if (kind == 0xb || kind == 0xa) {
                if (jo::as_item_def(item_type(w16(jo::as_map_object(o)->type_id)))->description != nullptr) {
                    done = true;
                    const char* const k = key_name();
                    const char* const s = jo::as_item_def(item_type(w16(jo::as_map_object(o)->type_id)))->description;
                    path_assign(msg, format(s, k));
                }
            } else {
                u8* const o2 = joan::orig::arg::as<u8*>(joan::orig::as_list(kObjects)->at_plus4(joan::orig::arg::u32(jo::self<jo::TalkPrompt>(this)->object)));  // 0x004853e0
                if (jo::as_item_def(item_type(w16(jo::as_map_object(o2)->type_id)))->description != nullptr) {
                    done = true;
                    u8* const o3 = joan::orig::arg::as<u8*>(joan::orig::as_list(kObjects)->at_plus4(joan::orig::arg::u32(jo::self<jo::TalkPrompt>(this)->object)));
                    path_assign(msg, jo::view<void*>(jo::as_item_def(item_type(w16(jo::as_map_object(o3)->type_id)))->description));
                }
            }
        }
        if (!done && jo::view<u32>(jo::self<jo::TalkPrompt>(this)->enemy_of) != 0) {
            done = true;
            const char* const k = key_name();
            path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_menu(kStrings)->gmsg_0b()), k));  // 0x004daa90
        }
        if (!done && jo::self<jo::TalkPrompt>(this)->zone != 0) {
            done = true;
            const char* const k = key_name();
            path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_text_6()), k));  // 0x004da9a0
        }
        if (!done && jo::self<jo::TalkPrompt>(this)->u32_2d != 0) {
            bool near = false;                                                     // [ebp-0x1bc]
            u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(jo::self<jo::TalkPrompt>(this)->u32_2d)));  // 0x0043c450
            u8* const r = vcall<u8*>(q, 0x1c);                                     // [ebp-0x1c0]; slot 0x1c returns this only in the Siege vtable 0x009d0980
            if (jo::as_siege(r)->i32_79 != 0) {  // the workshop's resource kind 0/1/2
                const u32 id = joan::orig::arg::as<u32>(joan::orig::as_unit_types(kTypes)->find(str(d::s_ALENCON_a23fb8)));  // 0x005270b0 "ALENCON"
                const u32 un = joan::orig::arg::as<u32>(joan::orig::as_units(kUnits)->find_by_3b(joan::orig::arg::i32(id)));               // 0x0051fd50
                u8* const w = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(un)));  // 0x0041b3f0, [ebp-0x1c4]
                if (w != nullptr && static_cast<i32>(jo::as_unit(w)->id) == global<i32>(d::g_player_unit_recno)) {
                    struct V3 {
                        float x, y, z;
                    };
                    V3 tmp;  // [ebp-0x390]
                    const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_unit(w)->position_), joan::orig::arg::ptr(jo::as_siege(r)->owner->position_)));  // 0x0060cc90
                    V3 dv = d;                                                                         // [ebp-0x384]
                    const double len = import_call<double>(d::iat_v3d_87_length, &dv);                           // length (st0)
                    if (!(len >= static_cast<double>(global<float>(d::flt_1024)))) near = true;  // < 1024 or NaN
                }
            }
            if (jo::as_siege(r)->i32_79 == 0) {
                const char* const k = key_name();
                path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_text_7()), k));  // 0x004da9d0
            } else if (jo::as_siege(r)->i32_79 == 1) {
                if (near) {
                    const char* const k = key_name();
                    path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_9()), k));  // 0x004daa30
                } else {
                    path_assign(msg, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_text_10()));  // 0x004daa60
                }
            } else if (jo::as_siege(r)->i32_79 == 2) {
                if (near) {
                    const char* const k = key_name();
                    path_assign(msg, format(joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_8()), k));  // 0x004daa00
                } else {
                    path_assign(msg, joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_text_10()));  // 0x004daa60
                }
            }
            done = true;
        }
        if (done) {
            box_piece(self, d::s_BG);  // "BG"
            box_piece(self, d::s_BORDER);  // "BORDER"
            const u32 c = colour(0xffffffffu);
            box_text(c_str(msg), c);
        }

        if (!priced && unit_missing(jo::self<jo::TalkPrompt>(this)->near_b) == 0) {
            u8* const u = unit(jo::self<jo::TalkPrompt>(this)->near_b);  // [ebp-0x1c8]
            Path name;                         // [ebp-0x35c]
            path_ctor(name);
            if (static_cast<i32>(jo::as_unit_type(unit_type(u))->rank) >= 3) {
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(c_str(name))), joan::orig::arg::str(jo::as_unit(u)->name_));  // strcpy
            } else {
                const char* const tn = jo::as_unit_type(unit_type(u))->title_b4;
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(c_str(name))), joan::orig::arg::str(tn));  // strcpy
            }
            i32 price = jo::as_unit(u)->rank < 3 ? price_of(u, d::g_a3cd10) : price_of(u, d::g_a3cc54);  // [ebp-0x360]
            if (joan::crt::stricmp(joan::orig::arg::str(jo::as_unit_type(unit_type(u))->name), str(d::s_MER_EX_ARCHER)) == 0 ||   // _stricmp "MER_EX_ARCHER"
                joan::crt::stricmp(joan::orig::arg::str(jo::as_unit_type(unit_type(u))->name), str(d::s_MER_EX_ARCHER_CAPTAIN)) == 0) {  // "MER_EX_ARCHER_CAPTAIN"
                price = ftol(Fpu{true}.mul(static_cast<double>(price), static_cast<double>(global<float>(d::flt_1_6))));
            }
            if (global<i32>(d::g_get_a596c4) < price) {
                const i32 short_by = price - global<i32>(d::g_get_a596c4);
                path_assign(msg, joan::orig::arg::as<const char*>(joan::orig::as_menu(kStrings)->gmsg_5(c_str(name), joan::orig::arg::u32(short_by))));  // 0x004da960
            } else {
                const char* const k = key_name();
                const i32 gold = global<i32>(d::g_get_a596c4);
                const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->gmsg_text_3(c_str(name), joan::orig::arg::u32(price), joan::orig::arg::u32(gold)));  // 0x004da8e0
                path_assign(msg, format(s, k));
            }
            priced = true;
        }
        if (priced) {
            // The hire price / "you cannot bribe" text sits under the minimap (820, 220 of 1024x768): anchored to the
            // top-right corner as the minimap is (hud_top_right, as Hud::draw_pause_caption at 823, 220), not
            // centred at the top with the talk box.
            const joan::display::LayoutSwap under_minimap(joan::display::Layout::hud_top_right);
            const u32 c = colour(0xffffffffu);
            joan::orig::as_text(kText)->draw(i32{0x334}, i32{0xdc}, i32{0x3e8}, i32{0x140}, joan::orig::arg::u32(i32{6}), c_str(msg), joan::orig::arg::u32(c), joan::orig::arg::i32(// 0x00626320
                          i32{2}), i32{1}, i32{1}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}), i32{0}, i32{-1});
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_10", name, addr, fn)
B("TalkPrompt::draw", 0x006d23b0, &HTalk::draw);
JOAN_CANVAS_SELF(0x006d23b0);  // ScreenScope in the body (no thunk wrapper)
#undef B

}  // namespace game_b116_10
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
//
//    - Screen::ctor_6d37f0 0x006d37f0 (ret 0): Widgets +0x458 / +0x79e / +0xae4, memset 0x458.
namespace {
namespace game_b102_15 {

using joan::orig::helpers::memset_;  // memset

using joan::orig::helpers::widget_ctor;  // 0x00620ee0

struct H {
    // ------------------------------------------------------------------ 0x006d37f0
    void* ctor_6d37f0() {
        widget_ctor(&jo::self<jo::MessageBox>(this)->button0);  // the popup 0x00a52df8 is the message box
        widget_ctor(&jo::self<jo::MessageBox>(this)->button1);
        widget_ctor(&jo::self<jo::MessageBox>(this)->button2);
        memset_(this, 0, static_cast<u32>(offsetof(jo::MessageBox, button0)));  // 0x458 on x86: everything before the buttons
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::ctor_6d37f0", 0x006d37f0, &H::ctor_6d37f0);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
//
//      - VideoPlayer::draw 0x006d5ae0 (unused arg): while playing (+8 == 1 / 3) steps the
//        player 0x00a5d2f0 (0x00685a80) on the timer 0x00a45fa8, draws its frame fitted to the
//        screen (0x006332b0) with black side bars (0x00632a70), stops it (0x00686160) at the
//        end; state 2 draws a black screen and fades in the picture +0 (0x006332b0). ret 4.
//        It only reads the screen size 0x00a4885c / 0x00a48860 like any 2D code; display.cpp /
//        canvas_hooks.cpp do not reference it (no size read patched inside, no return-address
//        test on its callees: the frame passed to canvas_sprite_from is only used for logging).
//
//      - Widget::stop_fade 0x0061f120, Widget::set_4e 0x0061ea50 / set_4a 0x0061ea20 (ret 4),
//        0x006d50e0 (two shorts +0x44c / +0x44e, ret 8), Dialog::bottom 0x0063e380 /
//        Dialog::right 0x0063e340.
namespace {
namespace game_b83_05 {

using joan::hook::x87::ftol;

constexpr img::Addr kSprites(d::g_sprites_a46128);
constexpr img::Addr kTimer(d::g_misc);
constexpr img::Addr kVideo(d::g_video_file);
constexpr fn::Code kQueueSprite = fn::Sprite2D_queue;  // 0x00632a70 (hooked by canvas_hooks.cpp)
constexpr fn::Code kQueueImage = fn::SpriteQueue_blit_image;   // 0x006332b0 (never replaced)

JOAN_COUNTER(c_video, "VideoPlayer::draw");

// 0x0042a260 Color(r, g, b, a) on a local; only the low bytes of the arguments are read.
std::uint32_t color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}

// 0x00632a70 with the trailing arguments used by the video player bars.
void black_bar(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2) {
    const std::uint32_t c = color(0, 0, 0, 0xff);
    thiscall<std::int32_t>(kQueueSprite, img::obj(kSprites), x1, y1, x2, y2, 1u, c, static_cast<void*>(nullptr) /* no texture */, 3u, 0u, 0u, 0u, 0xffffffffu, 0xffffffffu,
                           0xffffffffu, 0xffffffffu, 0u);
}

struct H {
    // 0x006d5ae0 (the argument is not used). ret 4.
    void video_draw(std::uint32_t) {
        c_video.hit();
        const std::int32_t state = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::VideoPlayer>(this)->stage));
        if ((state == 1 || state == 3) && global<std::uint32_t>(d::g_video_file) != 0) {
            const std::uint32_t now = joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(img::obj(kTimer))->get_time_591050());  // 0x00591050
            std::uint32_t elapsed = now - global<std::uint32_t>(d::g_movie_start_ms);
            if (global<std::uint32_t>(d::g_movie_start_ms) == 0) {
                elapsed = 0;
                global<std::uint32_t>(d::g_movie_start_ms) = joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(img::obj(kTimer))->get_time_591050());
            }
            const std::int32_t status = joan::orig::arg::as<std::int32_t>(joan::orig::as_video(img::obj(kVideo))->update(joan::orig::arg::u32(elapsed)));  // 0x00685a80
            if (status == 2 || status == 4) {
                const Fpu fpu{true};
                const double sw = fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_w)),
                                          static_cast<double>(global<std::int32_t>(d::g_video_width)));
                const double sh = fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_h)),
                                          static_cast<double>(global<std::int32_t>(d::g_video_height)));
                // fcompp (st0 = sh, st1 = sw) ; test ah,0x41 ; jne : sh <= sw or unordered -> sh.
                const float scale = static_cast<float>(sh > sw ? sw : sh);
                const std::int32_t w = ftol(fpu.mul(static_cast<double>(global<std::uint32_t>(d::g_video_width)), static_cast<double>(scale)));
                const std::int32_t h = ftol(fpu.mul(static_cast<double>(global<std::uint32_t>(d::g_video_height)), static_cast<double>(scale)));
                // 32-bit wrapping arithmetic as the original (sub ; cdq ; sub ; sar 1 = signed / 2).
                auto add = [](std::int32_t p, std::int32_t q) {
                    return static_cast<std::int32_t>(static_cast<std::uint32_t>(p) + static_cast<std::uint32_t>(q));
                };
                auto sub = [](std::int32_t p, std::int32_t q) {
                    return static_cast<std::int32_t>(static_cast<std::uint32_t>(p) - static_cast<std::uint32_t>(q));
                };
                const std::int32_t x = sub(global<std::int32_t>(d::g_screen_w), w) / 2;
                const std::int32_t y = sub(global<std::int32_t>(d::g_screen_h), h) / 2;
                const std::int32_t right = add(add(x, w), -1), bottom = add(add(y, h), -1);
                const std::int32_t vw1 = global<std::int32_t>(d::g_video_width) - 1, vh1 = global<std::int32_t>(d::g_video_height) - 1;
                const std::uint32_t c = color(0xff, 0xff, 0xff, 0xff);
                // x / y pushed as `mov ax/dx, word; push eax/edx` with the colour in the upper half.
                const std::uint32_t hi = c & 0xffff0000u;
                thiscall<std::int32_t>(kQueueImage, img::obj(kSprites), hi | static_cast<std::uint16_t>(x), hi | static_cast<std::uint16_t>(y),
                                       0xau, global<void*>(d::g_video_texture), c, 0u, 0u, 1u, 0u, 0u, vw1, vh1, 0u, right,
                                       bottom);  // 0x006332b0
                {
                    void* const texture = jo::as_texture2d(global<void*>(d::g_video_texture))->d3d_texture;
                    // COM call through vtable slot 0x24 with ecx = the object, as the original.
                    using Virtual = void (Obj::*)(void*);
                    JOAN_MEMBER_POINTER_CHECK(Virtual);
                    Virtual method{};
                    std::memcpy(&method, &joan::call::vtable_of(texture)[0x24 / 4], sizeof(void*));
                    (reinterpret_cast<Obj*>(texture)->*method)(texture);
                }
                if (x > 0) black_bar(0, 0, x - 1, global<std::int32_t>(d::g_screen_h) - 1);
                if (y > 0) black_bar(0, 0, global<std::int32_t>(d::g_screen_w) - 1, y - 1);
                if (add(x, w) < global<std::int32_t>(d::g_screen_w))
                    black_bar(add(x, w), 0, global<std::int32_t>(d::g_screen_w) - 1, global<std::int32_t>(d::g_screen_h) - 1);
                if (add(y, h) < global<std::int32_t>(d::g_screen_h))
                    black_bar(0, add(y, h), global<std::int32_t>(d::g_screen_w) - 1, global<std::int32_t>(d::g_screen_h) - 1);
            } else {
                black_bar(0, 0, global<std::int32_t>(d::g_screen_w) - 1, global<std::int32_t>(d::g_screen_h) - 1);
            }
            if (status == 4 || status == 3) {
                joan::orig::as_video(img::obj(kVideo))->close();  // 0x00686160
                global<std::uint32_t>(d::g_movie_start_ms) = 0;
                jo::view<std::uint8_t>(jo::self<jo::VideoPlayer>(this)->stage) = static_cast<std::uint8_t>(jo::view<std::uint8_t>(jo::self<jo::VideoPlayer>(this)->stage) + 1);
                jo::self<jo::VideoPlayer>(this)->start_time = global<std::uint32_t>(d::g_game_time_ms);
            }
            return;
        }
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::VideoPlayer>(this)->stage)) != 2) return;
        black_bar(0, 0, global<std::int32_t>(d::g_screen_w) - 1, global<std::int32_t>(d::g_screen_h) - 1);
        std::uint32_t alpha = ((global<std::uint32_t>(d::g_game_time_ms) - jo::self<jo::VideoPlayer>(this)->start_time) * 0xffu) / 0x7d0u;
        if (static_cast<std::int32_t>(alpha) > 0xff) alpha = 0xff;
        const std::uint32_t c = color(0xff, 0xff, 0xff, alpha & 0xffu);
        thiscall<std::int32_t>(kQueueImage, img::obj(kSprites), 0x100u, 0x140u, 1u, jo::self<jo::VideoPlayer>(this)->tex, c, 0u, 1u, 0u, 0xffffffffu,
                               0xffffffffu, 0xffffffffu, 0xffffffffu, 0u, 0xffffffffu, 0xffffffffu);  // 0x006332b0
    }

    // 0x006d50e0. ret 8. this = the MessageBox 0x00a52df8 (+0x44c x, +0x44e y).
    void set_pos(std::int16_t x, std::int16_t y) {
        jo::self<jo::MessageBox>(this)->x = x;
        jo::self<jo::MessageBox>(this)->y = y;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("VideoPlayer::draw", 0x006d5ae0, &H::video_draw);
B("Panel::set_pos", 0x006d50e0, &H::set_pos);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- from modules/game_b107_07.cpp (game/b107_07)
//
//    - Weather::start_any 0x006d5120 (ret 0) -> 1 when flag bit 1 / 2 / 4 of [this] is set and
namespace {
namespace game_b107_07 {

JOAN_COUNTER(c_start_any, "Weather::start_any_6d5120");

struct H {
    // 0x006d5120
    // this: the MessageBox object (b_a52df8); its button flags, result and buttons through jo::MessageBox (r8_x64)
    u32 start_any_6d5120() {
        c_start_any.hit();
        if ((jo::self<jo::MessageBox>(this)->state & 1) != 0 &&
            joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::MessageBox>(this)->button0)->mouse(0xd, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {  // 0x006218c0
            jo::self<jo::MessageBox>(this)->u32_444 = 1;
            return 1;
        }
        if ((jo::self<jo::MessageBox>(this)->state & 2) != 0 &&
            joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::MessageBox>(this)->button1)->mouse(0x1b, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {
            jo::self<jo::MessageBox>(this)->u32_444 = 2;
            return 1;
        }
        if ((jo::self<jo::MessageBox>(this)->state & 4) != 0 &&
            joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::MessageBox>(this)->button2)->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {
            jo::self<jo::MessageBox>(this)->u32_444 = 4;
            return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_07", name, addr, fn)
B("Weather::start_any_6d5120", 0x006d5120, &H::start_any_6d5120);
#undef B

}  // namespace game_b107_07
}  // namespace

// ---------------------------------------- from modules/game_b109_20.cpp (game/b109_20)
//
//    - Button::draw_6d53a0 0x006d53a0 (cdecl (button, a, flags), ret 0; no references at all ->
//      dead code): CBUT_DW / CBUT_OV / CBUT_OT + CBUT_CON sprites, optional scaled 0x00633e60,
//      label 0x00626130 with random jitter 0x00590a20. Returns 0 / 1.
namespace {
namespace game_b109_20 {


JOAN_COUNTER(c_button_draw, "Button::draw_6d53a0");

// 0x0042a260 colour ctor (r, g, b, a bytes) on a 4-byte local; the dword is passed on.
u32 color4(i32 a) {
    u8 c[4];
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(c)->construct(i32{0xff}, i32{0xff}, i32{0xff}, a));  // 0x0042a260
}
// 0x00633500 sprite draw on 0xa46128 (17 args, ret 0x44).
void sprite(i32 x, i32 y, u32 a3, const char* name, img::Addr font, u32 color, i32 a9) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), img::obj(font), joan::orig::arg::u32(color), 3, 0, joan::orig::arg::i16(a9), 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// 0x00593980 point-in-rect style test on 0xa516b0 (ret 0x10).
i32 hover(i32 x, i32 y, i32 w, i32 h) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h)));  // 0x00593980
}

// ------------------------------------------------------------------------------- 0x006d53a0
i32 JOAN_CDECL button_draw(u8* p, u32 a, u32 flags) {
    c_button_draw.hit();
    const u32 b = flags & 0xff;
    if (b != 7 && (b & 1) != 0) return 0;
    i16 x = jo::as_button(p)->x;
    i16 y = jo::as_button(p)->y;
    i16 w = jo::as_button(p)->x2;
    i16 h = jo::as_button(p)->y2;
    float sx = 1.0f;  // [ebp-0x14]
    float sy = 1.0f;  // [ebp-0x18]
    if (jo::as_button(p)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(p)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(p)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(p)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(p)->y2)), &sx, &sy, &x, &y, &w, &h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    const img::Addr font = d::g_set;
    if (jo::as_button(p)->pressed != 0) {
        sprite(x, y, a, str(d::s_CBUT_DW), font, color4(0xff), 0);          // "CBUT_DW"
        sprite(x - 4, y - 4, a, str(d::s_CBUT_CON), font, color4(0xff), 0);  // "CBUT_CON"
    } else if (hover(x, y, w, h) != 0) {
        sprite(x, y, a, str(d::s_CBUT_OV), font, color4(0xff), 0);          // "CBUT_OV"
        sprite(x - 4, y - 4, a, str(d::s_CBUT_CON), font, color4(0xff), 0);  // "CBUT_CON"
    } else {
        sprite(x, y, a, str(d::s_CBUT_OT), font, color4(0xff), 0);          // "CBUT_OT"
        sprite(x - 4, y - 4, a, str(d::s_CBUT_CON), font, color4(0xff), 0);  // "CBUT_CON"
    }
    if (jo::view<i8>(jo::as_button(p)->caption[0]) != 0) {
        // Both original branches (hovered / not) are the same code.
        if (jo::as_button(p)->pressed == 0) (void)hover(x, y, w, h);
        i32 dy = 0;
        if (jo::as_button(p)->pressed != 0) {
            const float v = global<float>(d::flt_1) * sy;  // 1.0f
            dy = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(v))));  // 0x00590a20
        }
        i32 dx = 0;
        if (jo::as_button(p)->pressed != 0) {
            const float v = global<float>(d::flt_1) * sx;
            dx = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(v))));  // 0x00590a20
        }
        u8 c[4];
        const u32 color = *joan::orig::arg::as<u32*>(joan::orig::as_color(c)->Color_dword(joan::orig::arg::u32(u32{0xff003366})));  // 0x005e9640
        joan::orig::as_text(jo::as_button(p)->caption_box)->draw_3(joan::orig::arg::i32(static_cast<i32>(x) + dx), joan::orig::arg::i32(static_cast<i32>(y) + dy), static_cast<i32>(w), static_cast<i32>(h), joan::orig::arg::u32(a), joan::orig::arg::str(jo::as_button(p)->caption), joan::orig::arg::u32(color), i32{0}, i32{0}, i32{2}, i32{0}, joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x3f800000}));  // 0x00626130
    }
    return 1;
}

#define B109(name, addr, fn) JOAN_REPLACE("game/b109_20", name, addr, fn)
B109("Button::draw_6d53a0", 0x006d53a0, &button_draw);
#undef B109

}  // namespace game_b109_20
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
//
//    - 0x0068c960 -> 0x0068c980, 0x0068c9c0 -> 0x0068c9e0, 0x0068ca10 -> 0x0068ca30,
//      0x0068ca60 -> 0x0068ca80, 0x00694a30 -> 0x00694a50, 0x00694a90 -> 0x00694ab0,
//      0x00694ae0 -> 0x00694b00, 0x00694b30 -> 0x00694b50, 0x006aaa60 -> 0x006aaa80,
//      0x006aaac0 -> 0x006aaae0, 0x006aab10 -> 0x006aab30, 0x006aab60 -> 0x006aab80,
//      0x006ac9f0 -> 0x006aca10, 0x006aca50 -> 0x006aca70, 0x006acaa0 -> 0x006acac0,
//      0x006acaf0 -> 0x006acb10, 0x006acd80 -> 0x006acda0, 0x006acde0 -> 0x006ace00,
//      0x006ace30 -> 0x006ace50, 0x006ace80 -> 0x006acea0, 0x006b0630 -> 0x006b0650,
//      0x006b0680 -> 0x006b06a0, 0x006b06d0 -> 0x006b06f0, 0x006b21a0 -> 0x006b21c0,
//      0x006b2200 -> 0x006b2220, 0x006b2250 -> 0x006b2270, 0x006b22a0 -> 0x006b22c0,
//      0x006c2010 -> 0x006c2030, 0x006d6550 -> 0x006d6570, 0x006def70 -> 0x006def90,
//      0x006defd0 -> 0x006deff0, 0x006df020 -> 0x006df040, 0x006df070 -> 0x006df090,
//      0x006ef230 -> 0x006ef250 (all cdecl, ret 0).
//
//    - empty thiscall (ret 0): 0x0068c820, 0x006b8240, 0x006bd490, 0x006ddef0, 0x006e1530,
//      0x006ea000; empty thiscall (arg), ret 4: 0x006dded0 - all unreferenced (dead code).
namespace {
namespace game_b105_19 {


// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

struct H {
    void nop0() {}
    void nop4(u32) {}

    // 0x006d6280
    std::uintptr_t byte_a_6d6280() { return (addr(this) & ~std::uintptr_t{0xff}) | jo::self<jo::VideoPlayer>(this)->playing; }  // eax = this, al = the flag
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6d6550", 0x006d6550, &static_init<fn::init_a6a550>);
B("Obj::byte_a_6d6280", 0x006d6280, &H::byte_a_6d6280);
B("Obj::nop_6dded0", 0x006dded0, &H::nop4);
B("Obj::nop_6ddef0", 0x006ddef0, &H::nop0);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
//
//    - Movie::tick 0x006d6370 (ret 0): intro-movie sequencer (eax unused by the caller).
namespace {
namespace game_b108_03 {


JOAN_COUNTER(c_movie_tick, "Movie::tick_6d6370");

void* const kRes = obj(d::g_misc);
void* const kSnd = obj(d::g_snd);

struct H {
    // ------------------------------------------------------------------ 0x006d6370
    void movie_play(i32 idx) {
        const char* const name = str(d::blob_9de0ec + static_cast<u32>(idx - 1) * 32u);
        joan::orig::as_video(obj(d::g_video_file))->open_684650(joan::orig::arg::str(name), joan::orig::as_sound3d(global<void*>(d::g_a5cf9c)));  // 0x00684650
        joan::orig::as_sound(kSnd)->set_volume_59b150(0x4d);                                      // 0x0059b150
    }
    void movie_tick() {
        c_movie_tick.hit();
        if (jo::view<i8>(jo::self<jo::VideoPlayer>(this)->playing) == 0) return;
        if (static_cast<i32>(jo::self<jo::VideoPlayer>(this)->stage) > 3) {
            joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(static_cast<u8>(1)));  // 0x004d7e70
            return;
        }
        for (; static_cast<i32>(jo::self<jo::VideoPlayer>(this)->stage) <= 3; jo::self<jo::VideoPlayer>(this)->stage = static_cast<i8>(jo::self<jo::VideoPlayer>(this)->stage + 1)) {
            const i32 s = jo::self<jo::VideoPlayer>(this)->stage;
            if (s == 1) {
                if (global<u32>(d::g_video_file) != 0) return;
                const char* const name = str(d::blob_9de0ec + static_cast<u32>(static_cast<i32>(jo::self<jo::VideoPlayer>(this)->stage) - 1) * 32u);
                if (joan::orig::as_misc(kRes)->is_file_exist(joan::orig::arg::str(name)) != 0) {  // 0x00590b80
                    movie_play(jo::self<jo::VideoPlayer>(this)->stage);
                    return;
                }
                jo::self<jo::VideoPlayer>(this)->start_time = global<u32>(d::g_game_time_ms);
            } else if (s == 2) {
                if (!(global<u32>(d::g_game_time_ms) - jo::self<jo::VideoPlayer>(this)->start_time > 0xfa0u)) return;
            } else if (s == 3) {
                if (global<u32>(d::g_video_file) != 0) return;
                const char* const name = str(d::blob_9de0ec + static_cast<u32>(static_cast<i32>(jo::self<jo::VideoPlayer>(this)->stage) - 1) * 32u);
                if (joan::orig::as_misc(kRes)->is_file_exist(joan::orig::arg::str(name)) != 0) {  // 0x00590b80
                    movie_play(jo::self<jo::VideoPlayer>(this)->stage);
                    return;
                }
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Movie::tick", 0x006d6370, &H::movie_tick);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
//
//      - 0x006d6680 Inventory::load_sprite (ret 0): 'INVENT' -> [+0] when null, same way.
namespace {
namespace game_b87_06 {


constexpr img::Addr kResources = d::g_texture_array;

std::int32_t res_mode() { return joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(obj(kResources))->mode()); }                       // 0x0060a720
void res_set_mode(std::int32_t m) { joan::orig::as_texture_manager(obj(kResources))->set_mode(joan::orig::arg::u32(m)); }                               // 0x0060a740
void* res_load(const char* name) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(kResources))->load(joan::orig::arg::str(name), 1, 0u)); }          // 0x006099c0

struct H {
    void load_one(const char* name) {
        if (jo::arg::as<void*>(jo::self<jo::InventoryScreen>(this)->tex) != nullptr) return;
        const std::int32_t mode = res_mode();
        res_set_mode(1);
        jo::self<jo::InventoryScreen>(this)->tex = res_load(name);
        res_set_mode(mode);
    }
    // ------------------------------------------------------------ 0x006d6680 (ret 0)
    void inventory_load_sprite() { load_one(&img::global<const char>(d::s_INVENT)); }  // 'INVENT'
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("Inventory::load_sprite", 0x006d6680, &H::inventory_load_sprite);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_b116_02.cpp (fix/uninit)
//
//    - Inventory::draw_items 0x006d6740 (thiscall(player id), ret 4, void: the only caller
//      0x006f7016 (Inventory::draw 0x006f6280, this = 0x00a488e0) ignores eax; thunk 0x00409d8b).
//      Returns at once when 0x00428b70(id) on 0x00a47c08 is non-zero. Else: player 0x0041b3c0(id),
//      "PANEL" / "INVENTBG" backgrounds (0x00633500), the equipment slots of the player's
//      equipment block (+0x1c1: +2 / +6 hands, +0xa, +0xe) and the three grids (+0x12 belt,
//      +0x16 / +0x1a packs; 0x00417ad0 count, 0x00457f60 empty, 0x00417200 get) drawn with
//      0x006ddf10 ("JOAN_SWORD_LV10[_<n>]" for the Saint-Catherine-de-Fierbois sword, a suffix
namespace {
namespace game_b116_02 {

// CRT (called directly by the original).
u32 strlen_(const void* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
void strcpy_(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void strcat_(void* d, const void* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void memset_(void* d, i32 v, u32 n) { std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }
i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

constexpr u32 kM1 = 0xffffffffu;
constexpr img::Addr kMark = d::s_M_9ddd9c;     // suffix of the item under the cursor
constexpr img::Addr kMouseX = d::g_mouse_x;
constexpr img::Addr kMouseY = d::g_mouse_y;
constexpr img::Addr kItems = d::g_item_res;
constexpr img::Addr kRenderer = d::g_sprites_a46128;
constexpr img::Addr kStrings = d::g_strings;
constexpr img::Addr kFont = d::g_font_a47d18;
constexpr img::Addr kTooltip = d::g_tooltip;
constexpr img::Addr kMouse = d::g_mouse;

i32 mx() { return global<i32>(kMouseX); }
i32 my() { return global<i32>(kMouseY); }
u8* item_rec(i16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(kItems))->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

JOAN_COUNTER(c_items, "Inventory::draw_items");

// The original frame: [ebp-0x79c] .. [ebp-1], then [ebp+0] .. padding (saved ebp, return
// address, the argument at [ebp+8]).
constexpr i32 kFrame = 0x79c;
constexpr i32 kPad = 0x1000;
struct Frame {
    u8 bytes[kFrame + kPad];
    u8* p(i32 off) { return bytes + (kFrame + off); }
    i32& d(i32 off) { return *reinterpret_cast<i32*>(p(off)); }
    // The pointer locals of the original frame ([ebp-4] this, [ebp-8] unit, the grid records, the item definitions ...):
    // a frame dword cannot hold an x64 pointer, so they live in a side table with one pointer per frame dword.
    u8* pslot[(kFrame + kPad) / 4] = {};
    u8*& ptr(i32 off) { return pslot[static_cast<std::size_t>(kFrame + off) / 4]; }
    void setp(i32 off, u8* p) { ptr(off) = p; }
    // The argument block of format_5a4c30 ([ebp-0x500], 15 dwords in the original): pointer-sized slots now; a 16th
    // value (it overran into the neighbouring locals) is dropped.
    std::uintptr_t args[15] = {};
    void arg_put(i32 n, i32 value) {
        if (n >= 0 && n < 15) args[n] = static_cast<std::uint32_t>(value);
    }
};

struct H {
    // Color(argb) ctor 0x005e9640 on [ebp+tmp]; the dword is copied to [ebp+copy] and returned.
    static u32 color(Frame& fr, i32 tmp, i32 copy, u32 argb) {
        const u8* const c = joan::orig::arg::as<u8*>(joan::orig::as_color(fr.p(tmp))->Color_dword(joan::orig::arg::u32(argb)));  // 0x005e9640
        fr.d(copy) = jo::view<i32>(*jo::as_colour(c));
        return static_cast<u32>(fr.d(copy));
    }
    // 0x00633500 background image (17 args, ret 0x44) with the common tail of both calls.
    static void image(u32 a, u32 b, img::Addr name, void* tex, u32 col) {
        joan::orig::as_sprite_queue(obj(kRenderer))->draw_named(joan::orig::arg::i16(a), joan::orig::arg::i16(b), 1u, joan::orig::arg::str(str(name)), joan::orig::arg::mptr(tex), joan::orig::arg::u32(col), 3, 0, 0, 1, joan::orig::arg::i16(kM1), joan::orig::arg::i16(kM1), joan::orig::arg::i16(kM1), joan::orig::arg::i16(kM1), 0, joan::orig::arg::i16(kM1), joan::orig::arg::i16(kM1));  // 0x00633500
    }
    // 0x00632a70 sprite queue (16 args, ret 0x40) with the tail all eight calls share.
    static void sprite(i32 x1, i32 y1, i32 x2, i32 y2, u32 col) {
        joan::orig::as_sprite2d(obj(kRenderer))->queue(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(u32{1}), joan::orig::arg::u32(col), joan::orig::arg::mptr(u32{0}), joan::orig::arg::i32(u32{3}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(kM1), joan::orig::arg::i32(u32{0}));  // 0x00632a70
    }
    // Text::draw_line 0x006255d0 (12 args, ret 0x30) with the common tail.
    static void text(i16 x, i16 y, const char* s, u32 col) {
        joan::orig::as_text(obj(kFont))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(u32{1}), s, joan::orig::arg::u32(col), joan::orig::arg::i32(kM1), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
    }
    // 0x006b8db0 tooltip (x, y, a, b, 1), ret 0x14.
    static void tooltip(const char* a, const char* b) {
        // pushes: 1, b, a, word [0xa516c8], word [0xa516c4]
        joan::orig::as_help_box(obj(kTooltip))->draw_box(joan::orig::arg::u32(static_cast<i16>(mx())), joan::orig::arg::u32(static_cast<i16>(my())), a, b, joan::orig::arg::u32(u32{1}));  // 0x006b8db0
    }

    // The item name into the 0x84-byte buffer [ebp+buf]: "JOAN_SWORD_LV10" (+ "_" + char('0' +
    // diff)) for the Saint-Catherine-de-Fierbois sword (+0x14), else the name (+0x10). `id`
    // re-reads the item id word, `diff` is the local of the length difference.
    template <class Id>
    static void sword_name(Frame& fr, i32 buf, i32 diff, Id id) {
        if (stricmp_(jo::as_item_def(item_rec(id()))->file_name, str(kSaint)) == 0) {
            memset_(fr.p(buf), 0, 0x82);
            strcpy_(fr.p(buf), str(kSword));
            const u32 n = strlen_(jo::as_item_def(item_rec(id()))->name);
            fr.d(diff) = static_cast<i32>(n - strlen_(str(kSword)));
            if (fr.d(diff) != 0) {
                fr.p(buf)[strlen_(fr.p(buf))] = 0x5f;
                const u8 c = static_cast<u8>(fr.d(diff) + 0x30);
                fr.p(buf)[strlen_(fr.p(buf))] = c;
            }
        } else {
            strcpy_(fr.p(buf), jo::as_item_def(item_rec(id()))->name);
        }
    }
    // [self+0x27] != 0 && [self+0x2d] == kind: appends the cursor mark.
    static void mark(Frame& fr, i32 buf, i32 kind) {
        if (jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_item) != 0 && jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_source) == kind) strcat_(fr.p(buf), str(kMark));
    }
    // Same with the grid index check [self+0x2f] == [ebp-0x238].
    static void mark_at(Frame& fr, i32 buf, i32 kind) {
        if (jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_item) != 0 && jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_source) == kind &&
            jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_grid_key) == fr.d(-0x238))
            strcat_(fr.p(buf), str(kMark));
    }
    static void draw_item(Frame& fr, i32 x, i32 y, i32 buf) {
        joan::orig::as_inventory(fr.ptr(-4))->draw_item_6ddf10(joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::str(fr.p(buf)));  // 0x006ddf10
    }
    // [[[ebp-8]+0x1c1]+off]: a member of the unit's modifiers (Modifiers : UnitData: hands / grids)
    template <class M>
    static M* block(Frame& fr, M* jo::UnitData::* m) { return jo::as_unit(fr.ptr(-8))->modifiers->*m; }

    // One grid of item pictures (0x006d6b2a / 0x006d6c4f / 0x006d6e41).
    static void grid_items(Frame& fr, jo::Grid* jo::UnitData::* grid, i32 rec, i32 xl, i32 yl, i32 x0, i32 y0, i32 buf, i32 diff,
                           bool sword, i32 kind) {
        for (fr.d(-0x238) = joan::orig::as_dynarray(block(fr, grid))->size(); fr.d(-0x238) > 0; fr.d(-0x238) -= 1) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(block(fr, grid))->is_empty_at(joan::orig::arg::i32(fr.d(-0x238)))) != 0) continue;  // 0x00457f60
            fr.setp(rec, joan::orig::arg::as<u8*>(joan::orig::as_dynarray(block(fr, grid))->get(joan::orig::arg::i32(fr.d(-0x238)))));  // 0x00417200
            fr.d(xl) = static_cast<i32>(jo::as_inv_item(fr.ptr(rec))->x) * 0x28 + x0;
            fr.d(yl) = static_cast<i32>(jo::as_inv_item(fr.ptr(rec))->y) * 0x28 + y0;
            auto id = [&]() { return jo::as_inv_item(fr.ptr(rec))->type; };
            if (sword) {
                sword_name(fr, buf, diff, id);
            } else {
                strcpy_(fr.p(buf), jo::as_item_def(item_rec(id()))->name);
            }
            mark_at(fr, buf, kind);
            draw_item(fr, fr.d(xl), fr.d(yl), buf);
        }
    }

    // Drop highlight over a pack grid (0x006d7246 / 0x006d73c7).
    static void pack_highlight(Frame& fr, jo::Grid* jo::UnitData::* grid, i32 cxl, i32 cyl, i32 y0, i32 flag, i32 tmp) {
        joan::orig::as_grid(fr.ptr(-4))->cell_at_mouse(static_cast<i32>(jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[0])), static_cast<i32>(jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[1])), joan::orig::arg::mptr(fr.p(cxl)), joan::orig::arg::mptr(fr.p(cyl)), joan::orig::arg::u32(flag));  // 0x006de200
        if (fr.d(cxl) < 0 || fr.d(cyl) < 0) return;
        const i32 fits = joan::orig::arg::as<i32>(joan::orig::as_grid(block(fr, grid))->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i8>(jo::as_item_def(fr.ptr(-0x3f4))->stats[0]))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i8>(jo::as_item_def(fr.ptr(-0x3f4))->stats[1]))), joan::orig::arg::i32(u32{0}), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i8>(fr.d(cxl)))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i8>(fr.d(cyl)))), joan::orig::arg::i32(u32{0})));  // 0x004574a0
        if (fits == 0) {
            const i32 swap = joan::orig::arg::as<i32>(joan::orig::as_unit(fr.ptr(-4))->item_under(joan::orig::arg::i32(fr.d(8)), static_cast<i32>(jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[0])), static_cast<i32>(jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[1])), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(fr.d(cxl)), joan::orig::arg::i32(fr.d(cyl)), static_cast<std::int8_t>(joan::orig::arg::i32(flag))));  // 0x006de060
            if (swap == 0) return;
        }
        fr.d(tmp) = global<i32>(d::g_a6a550);
        const i32 y2 = (fr.d(cyl) + jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[1])) * 0x28 + y0;
        const i32 x2 = (fr.d(cxl) + jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[0])) * 0x28 + 0x21a;
        sprite(fr.d(cxl) * 0x28 + 0x21a, fr.d(cyl) * 0x28 + y0, x2, y2, static_cast<u32>(fr.d(tmp)));
    }

    // Labels over a grid (0x006d77de / 0x006d79e7): level / name for weapons, level for kind 9.
    static void grid_labels(Frame& fr, jo::Grid* jo::UnitData::* grid, i32 rec, i32 xl, i32 yl, i32 kl, i32 x0, i32 y0, i32 c1, i32 c2) {
        for (fr.d(-0x238) = joan::orig::as_dynarray(block(fr, grid))->size(); fr.d(-0x238) > 0; fr.d(-0x238) -= 1) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(block(fr, grid))->is_empty_at(joan::orig::arg::i32(fr.d(-0x238)))) != 0) continue;  // 0x00457f60
            fr.setp(rec, joan::orig::arg::as<u8*>(joan::orig::as_dynarray(block(fr, grid))->get(joan::orig::arg::i32(fr.d(-0x238)))));  // 0x00417200
            fr.d(xl) = static_cast<i32>(jo::as_inv_item(fr.ptr(rec))->x) * 0x28 + x0;
            fr.d(yl) = static_cast<i32>(jo::as_inv_item(fr.ptr(rec))->y) * 0x28 + y0;
            fr.d(kl) = jo::view<i16>(jo::as_item_def(item_rec(jo::as_inv_item(fr.ptr(rec))->type))->type_id);
            const i32 k = fr.d(kl);
            if (jo::as_inv_item(fr.ptr(rec))->count > 1 || k == 0xd || k == 0xe || k == 0xf || k == 0x10 || k == 0x11 || k == 0x12) {
                const u32 col = color(fr, c1, c1 + 4, kM1);
                const char* const s = joan::orig::as_misc(obj(d::g_misc))->format_int_short(joan::orig::arg::i16(jo::as_inv_item(fr.ptr(rec))->count), joan::orig::arg::u32(u32{1}));  // 0x004e1360
                text(static_cast<i16>(fr.d(xl)), static_cast<i16>(fr.d(yl)), s, col);
            } else if (jo::view<i16>(jo::as_item_def(item_rec(jo::as_inv_item(fr.ptr(rec))->type))->class_id) == 9) {
                level_text(fr, rec, xl, yl, c2);
            }
        }
    }
    // Level label: color, (short)[rec+4] - [item+0x36] + 1 through 0x004dce00, Text::draw_line.
    static void level_text(Frame& fr, i32 rec, i32 xl, i32 yl, i32 c) {
        const u32 col = color(fr, c, c + 4, kM1);
        const i32 lv = jo::as_inv_item(fr.ptr(rec))->level;
        const i32 v = static_cast<i32>(static_cast<u32>(lv) - jo::view<u32>(jo::as_item_def(item_rec(jo::as_inv_item(fr.ptr(rec))->type))->min_level) + 1u);  // wrapping
        const char* const s = joan::orig::arg::str(joan::orig::as_txt_res(obj(kStrings))->invent_text_1(joan::orig::arg::u32(v)));  // 0x004dce00
        text(static_cast<i16>(fr.d(xl)), static_cast<i16>(fr.d(yl)), s, col);
    }

    // ------------------------------------------------------------------------------ 0x006d6740
    void draw_items(u32 player) {
        c_items.hit();
        Frame fr;
        std::memset(static_cast<void*>(&fr), 0, sizeof fr);  // fix/uninit: the original's untouched frame bytes are garbage
        fr.setp(-4, reinterpret_cast<u8*>(this));
        fr.d(8) = static_cast<i32>(player);
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(fr.d(8))) != 0) return;  // 0x00428b70
        fr.setp(-8, joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(fr.d(8)))));  // 0x0041b3c0

        {
            const u32 col = color(fr, -0x70c, -0x708, kM1);
            image(0x144, 8, d::s_PANEL, global<void*>(d::g_a563a0), col);  // "PANEL"
        }
        {
            const u32 col = color(fr, -0x714, -0x710, kM1);
            image(0x154, 0x20, d::s_INVENTBG, jo::view<void*>(jo::as_inventory_screen(fr.ptr(-4))->sheet), col);  // "INVENTBG"
        }

        // Hands (+2 / +6).
        {
            const u8* const eq = reinterpret_cast<const u8*>(jo::as_unit(fr.ptr(-8))->modifiers);
            bool any = jo::as_unit_data(eq)->hands[0] != nullptr;
            if (!any) any = jo::as_unit_data(jo::as_unit(fr.ptr(-8))->modifiers)->hands[1] != nullptr;
            if (any) {
                fr.setp(-0xc, jo::arg::as<u8*>(jo::as_unit_data(jo::as_unit(fr.ptr(-8))->modifiers)->hands[0]));
                fr.setp(-0x10, jo::arg::as<u8*>(jo::as_unit_data(jo::as_unit(fr.ptr(-8))->modifiers)->hands[1]));
                if (fr.ptr(-0xc) != nullptr) {
                    fr.d(-0x14) = 0x157;
                    fr.d(-0x18) = 0x34;
                    sword_name(fr, -0x9c, -0xa0, [&]() { return jo::view<i16>(jo::as_held_item(fr.ptr(-0xc))->item); });
                    mark(fr, -0x9c, 2);
                    draw_item(fr, fr.d(-0x14), fr.d(-0x18), -0x9c);
                }
                if (fr.ptr(-0x10) != nullptr) {
                    fr.d(-0xa4) = 0x1a7;
                    fr.d(-0xa8) = 0x34;
                    strcpy_(fr.p(-0x12c), jo::as_item_def(item_rec(jo::view<i16>(jo::as_held_item(fr.ptr(-0x10))->item)))->name);
                    mark(fr, -0x12c, 1);
                    draw_item(fr, fr.d(-0xa4), fr.d(-0xa8), -0x12c);
                }
            }
        }
        // Slots +0xa / +0xe.
        if (jo::as_unit_data(jo::as_unit(fr.ptr(-8))->modifiers)->equipped != nullptr) {
            strcpy_(fr.p(-0x1b0), jo::as_item_def(item_rec(jo::view<i16>(jo::as_held_item(block(fr, &jo::UnitData::equipped))->item)))->name);
            mark(fr, -0x1b0, 7);
            draw_item(fr, 0x157, 0x122, -0x1b0);
        }
        if (jo::as_unit_data(jo::as_unit(fr.ptr(-8))->modifiers)->extra_item != nullptr) {
            strcpy_(fr.p(-0x234), jo::as_item_def(item_rec(jo::view<i16>(jo::as_held_item(block(fr, &jo::UnitData::extra_item))->item)))->name);
            mark(fr, -0x234, 8);
            draw_item(fr, 0x1cc, 0x128, -0x234);
        }

        // Grids.
        grid_items(fr, &jo::UnitData::list_12, -0x23c, -0x240, -0x244, 0x157, 0x218, -0x2c8, 0, false, 9);
        grid_items(fr, &jo::UnitData::grid_16, -0x2cc, -0x2d0, -0x2d4, 0x212, 0x36, -0x358, -0x35c, true, 0xa);
        grid_items(fr, &jo::UnitData::grid_1a, -0x360, -0x364, -0x368, 0x212, 0x19e, -0x3ec, -0x3f0, true, 0xb);

        // The dragged item.
        if (jo::as_inventory_screen(fr.ptr(-4))->carried_item != 0) {
            fr.setp(-0x3f4, item_rec(jo::as_inventory_screen(fr.ptr(-4))->carried_item));
            if (mx() >= 0x15f && mx() <= 0x1ff && my() >= 0x3c && my() <= 0xdc) {
                fr.d(-0x718) = global<i32>(d::g_a6a550);
                sprite(0x15f, 0x3c, 0x1ff, 0xdc, static_cast<u32>(fr.d(-0x718)));
            }
            if (mx() > 0x15f && mx() <= 0x1af && my() > 0x12a && my() <= 0x1cc &&
                (jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0xb || jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0xc)) {
                fr.d(-0x71c) = global<i32>(d::g_a6a550);
                sprite(0x15f, 0x12a, 0x1af, 0x1cc, static_cast<u32>(fr.d(-0x71c)));
            }
            if (mx() > 0x1d4 && mx() <= 0x1fc && my() > 0x130 && my() <= 0x1d0 &&
                (jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0xd || jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0xe ||
                 jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0xf || jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0x10 ||
                 jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0x11 || jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->type_id) == 0x12)) {
                fr.d(-0x720) = global<i32>(d::g_a6a550);
                sprite(0x1d4, 0x130, 0x1fc, 0x1d0, static_cast<u32>(fr.d(-0x720)));
            }
            if (mx() > 0x21a && mx() <= 0x30a && my() > 0x3e && my() <= 0x17e)
                pack_highlight(fr, &jo::UnitData::grid_16, -0x3f8, -0x3fc, 0x3e, 0, -0x724);
            if (mx() > 0x21a && mx() <= 0x30a && my() > 0x1a6 && my() <= 0x246)
                pack_highlight(fr, &jo::UnitData::grid_1a, -0x400, -0x404, 0x1a6, 1, -0x728);
            if (mx() > 0x15f && mx() <= 0x1fe && my() > 0x220 && my() <= 0x247 && jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->class_id) == 9) {
                fr.d(-0x408) = (mx() - 0x15f) / 0x28;
                fr.d(-0x40c) = (my() - 0x220) / 0x28;
                fr.d(-0x72c) = global<i32>(d::g_a6a550);
                const i32 y2 = (fr.d(-0x40c) + jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[1])) * 0x28 + 0x220;
                const i32 x2 = (fr.d(-0x408) + jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[0])) * 0x28 + 0x15f;
                sprite(fr.d(-0x408) * 0x28 + 0x15f, fr.d(-0x40c) * 0x28 + 0x220, x2, y2, static_cast<u32>(fr.d(-0x72c)));
            }
            if (stricmp_(jo::as_item_def(fr.ptr(-0x3f4))->file_name, str(kSaint)) == 0) {
                memset_(fr.p(-0x490), 0, 0x82);
                strcpy_(fr.p(-0x490), str(kSword));
                const u32 n = strlen_(jo::as_item_def(fr.ptr(-0x3f4))->name);
                fr.d(-0x494) = static_cast<i32>(n - strlen_(str(kSword)));
                if (fr.d(-0x494) != 0) {
                    fr.p(-0x490)[strlen_(fr.p(-0x490))] = 0x5f;
                    const u8 c = static_cast<u8>(fr.d(-0x494) + 0x30);
                    fr.p(-0x490)[strlen_(fr.p(-0x490))] = c;
                }
            } else {
                strcpy_(fr.p(-0x490), jo::as_item_def(fr.ptr(-0x3f4))->name);
            }
            const i32 y = my() - static_cast<i32>(jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[1])) * 0x28 / 2;
            const i32 x = mx() - static_cast<i32>(jo::view<i16>(jo::as_item_def(fr.ptr(-0x3f4))->stats[0])) * 0x28 / 2;
            joan::canvas::draw_dragged_item(fr.ptr(-4), x, y, reinterpret_cast<const char*>(fr.p(-0x490)));  // call @0x006d7766: the item name built in [ebp-0x490]
        }

        // Shield slot label (+0xe).
        if (jo::as_unit_data(jo::as_unit(fr.ptr(-8))->modifiers)->extra_item != nullptr) {
            const u32 col = color(fr, -0x734, -0x730, kM1);
            const char* const s = joan::orig::as_misc(obj(d::g_misc))->format_int_short(joan::orig::arg::i16(jo::view<i16>(jo::as_held_item(block(fr, &jo::UnitData::extra_item))->count)), joan::orig::arg::u32(u32{1}));  // 0x004e1360
            text(0x1d4, 0x130, s, col);
        }
        grid_labels(fr, &jo::UnitData::grid_16, -0x498, -0x49c, -0x4a0, -0x4a4, 0x21a, 0x3e, -0x73c, -0x744);
        grid_labels(fr, &jo::UnitData::grid_1a, -0x4a8, -0x4ac, -0x4b0, -0x4b4, 0x21a, 0x1a6, -0x74c, -0x754);
        // Belt levels (+0x12).
        for (fr.d(-0x238) = joan::orig::as_dynarray(block(fr, &jo::UnitData::list_12))->size(); fr.d(-0x238) > 0; fr.d(-0x238) -= 1) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(block(fr, &jo::UnitData::list_12))->is_empty_at(joan::orig::arg::i32(fr.d(-0x238)))) != 0) continue;  // 0x00457f60
            fr.setp(-0x4b8, joan::orig::arg::as<u8*>(joan::orig::as_dynarray(block(fr, &jo::UnitData::list_12))->get(joan::orig::arg::i32(fr.d(-0x238)))));  // 0x00417200
            fr.d(-0x4bc) = static_cast<i32>(jo::as_inv_item(fr.ptr(-0x4b8))->x) * 0x28 + 0x15f;
            fr.d(-0x4c0) = static_cast<i32>(jo::as_inv_item(fr.ptr(-0x4b8))->y) * 0x28 + 0x220;
            level_text(fr, -0x4b8, -0x4bc, -0x4c0, -0x75c);
        }

        // Tooltip of the pointed item.
        if (jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_item) != 0 && jo::as_inventory_screen(fr.ptr(-4))->carried_item == 0) {
            fr.setp(-0x4c4, item_rec(jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_item)));
            if (jo::as_item_def(fr.ptr(-0x4c4))->description != nullptr) {
                fr.d(-0x504) = 0;
                {
                    const i32 v = joan::orig::arg::as<i32>(joan::orig::as_mod_table(fr.ptr(-0x4c4))->value_own(joan::orig::arg::i32(jo::as_inventory_screen(fr.ptr(-4))->u32_29)));  // 0x004a41c0
                    fr.arg_put(fr.d(-0x504), v);
                    fr.d(-0x504) += 1;
                }
                for (fr.d(-0x238) = 0; fr.d(-0x238) < jo::as_item_def(fr.ptr(-0x4c4))->effect_count; fr.d(-0x238) += 1) {
                    const u32 i = static_cast<u32>(fr.d(-0x238)) * 0xcu;
                    const u32 lvl = jo::as_inventory_screen(fr.ptr(-4))->u32_29 - jo::view<u32>(jo::as_item_def(fr.ptr(-0x4c4))->min_level);
                    const u8* const rows = reinterpret_cast<const u8*>(jo::as_item_def(fr.ptr(-0x4c4))->effects);
                    const u32 k = *reinterpret_cast<const u32*>(rows + i + 8u);
                    const u32 base = *reinterpret_cast<const u32*>(rows + i + 4u);
                    fr.arg_put(fr.d(-0x504), static_cast<i32>(base + k * lvl));
                    fr.d(-0x504) += 1;
                }
                for (fr.d(-0x508) = 0; fr.d(-0x508) < jo::as_item_def(fr.ptr(-0x4c4))->requirement_count; fr.d(-0x508) += 1) {
                    const u32 i = static_cast<u32>(fr.d(-0x508)) << 4;
                    const u32 lvl = jo::as_inventory_screen(fr.ptr(-4))->u32_29 - jo::view<u32>(jo::as_item_def(fr.ptr(-0x4c4))->min_level);
                    const u8* const rows = reinterpret_cast<const u8*>(jo::as_item_def(fr.ptr(-0x4c4))->requirements);
                    const u32 k = *reinterpret_cast<const u32*>(rows + i + 8u);
                    const u32 base = *reinterpret_cast<const u32*>(rows + i + 4u);
                    fr.arg_put(fr.d(-0x504), static_cast<i32>(base + k * lvl));
                    fr.d(-0x504) += 1;
                }
                const char* const t = joan::orig::arg::as<const char*>(joan::orig::text::format_5a4c30(jo::as_item_def(fr.ptr(-0x4c4))->description, static_cast<void*>(fr.args)));  // 0x005a4c30
                strcpy_(fr.p(-0x6fc), t);
                std::uintptr_t suffix = 0;
                if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->class_id) == 7) {
                    suffix = fn::code_value(fn::Menu_invent_0c);  // 0x004dd020
                } else if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->class_id) == 0x10) {
                    suffix = fn::code_value(fn::Invent_open_d);  // 0x004dd050
                } else if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->class_id) == 0x13) {
                    suffix = fn::code_value(fn::Invent_open_0e);  // 0x004dd080
                } else if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->type_id) == 0x23) {
                    suffix = fn::code_value(fn::Invent_open_0f);  // 0x004dd0b0
                }
                if (suffix != 0) {
                    strcat_(fr.p(-0x6fc), str(d::kStrNewlines));
                    const char* const s = thiscall<const char*>(suffix, obj(kStrings));
                    strcat_(fr.p(-0x6fc), s);
                }
                tooltip(jo::as_item_def(fr.ptr(-0x4c4))->file_name, joan::orig::arg::str(fr.p(-0x6fc)));
            } else {
                std::uintptr_t getter = 0;
                if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->class_id) == 7) {
                    getter = fn::code_value(fn::Menu_invent_0c);  // 0x004dd020
                } else if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->class_id) == 0x10) {
                    getter = fn::code_value(fn::Invent_open_d);  // 0x004dd050
                } else if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->class_id) == 0x13) {
                    getter = fn::code_value(fn::Invent_open_0e);  // 0x004dd080
                } else if (jo::view<i16>(jo::as_item_def(fr.ptr(-0x4c4))->type_id) == 0x23) {
                    getter = fn::code_value(fn::Invent_open_0f);  // 0x004dd0b0
                }
                const char* const s = getter != 0 ? thiscall<const char*>(getter, obj(kStrings)) : str(d::s_sp_9d05f0);  // " " when no getter
                tooltip(jo::as_item_def(fr.ptr(-0x4c4))->file_name, s);
            }
        }

        // Tooltips of the panel buttons.
        if (jo::view<i16>(jo::as_inventory_screen(fr.ptr(-4))->hover_item) != 0 || jo::as_inventory_screen(fr.ptr(-4))->carried_item != 0) return;
        if (global<u32>(d::g_game_time_ms) - jo::as_inventory_screen(fr.ptr(-4))->mouse_moved_ms <= 0xfa) return;
        struct Button {
            i32 x1, y1, x2, y2;
            std::uintptr_t title, body;
        };
        static const Button kButtons[5] = {
            {0x156, 0x1a, 0x208, 0x2e, fn::code_value(fn::Invent_open_02), fn::code_value(fn::Invent_open_3)},     // 0x004dce40 / 0x004dce70
            {0x156, 0x110, 0x208, 0x124, fn::code_value(fn::Menu_invent_04), fn::code_value(fn::Invent_open_5)},   // 0x004dcea0 / 0x004dced0
            {0x156, 0x1ff, 0x208, 0x213, fn::code_value(fn::Menu_invent_06), fn::code_value(fn::Invent_open_07)},   // 0x004dcf00 / 0x004dcf30
            {0x21b, 0x23, 0x31f, 0x37, fn::code_value(fn::Invent_open_08), fn::code_value(fn::Invent_open_9)},     // 0x004dcf60 / 0x004dcf90
            {0x21b, 0x18b, 0x31f, 0x19f, fn::code_value(fn::Invent_open_10), fn::code_value(fn::Invent_open_11)},   // 0x004dcfc0 / 0x004dcff0
        };
        for (const Button& b : kButtons) {
            if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(kMouse))->inside(joan::orig::arg::i32(b.x1), joan::orig::arg::i32(b.y1), joan::orig::arg::i32(b.x2), joan::orig::arg::i32(b.y2))) == 0) continue;  // 0x00593980
            const char* const title = thiscall<const char*>(b.title, obj(kStrings));  // [ebp-0x700]
            const char* const body = thiscall<const char*>(b.body, obj(kStrings));    // [ebp-0x704]
            tooltip(joan::orig::arg::str(title), joan::orig::arg::str(body));
            return;
        }
    }
};

JOAN_REPLACE("fix/uninit", "Inventory::draw_items_6d6740", 0x006d6740, &H::draw_items);

}  // namespace game_b116_02
}  // namespace

// ---------------------------------------- from modules/game_b110_03.cpp (game/b110_03)
//
//    - Inventory::update_6d8ac0 0x006d8ac0 (thiscall(u32 unit id) on the inventory 0xa488e0, ret 4,
//      al return 0 / 1; the only caller 0x006f7bd8 ignores eax). Body 0x006d8ac0..0x006dcc88, then
//      the two jump tables 0x6dcc8b / 0x6dccb7 (decoded from the binary, 11 entries each) and int3
//      padding (functions.json's end 0x6dded0 overshoots). Steps:
//        mouse position / press-time / double-click state (+8: 1 pressed, 2 / 4 double, 3 held
//        >= 500 ms); dead / hidden / out-of-range unit -> hover only;
//        Parser::value(1) (0x00594700): carried item -> drop on its source kind (switch on +0x11,
//        table 0x6dcc8b: 1, 2 -> 0x0050c4f0, 7 -> 0x00497270, 8 -> 0x004973a0, 9 / 10 / 11 -> grid
//        area test 0x004574a0 + 0x004975d0 / 0x004976e0), else use a potion / scroll / letter / town
//        item from the backpack grids (0x0050b4f0 / 0x0050ba00 / 0x0050bcd0 / 0x006e0b70 /
//        0x006de590);
//        carried item (+0x12 != 0) with state 3 / 4: hands (+2 / +6), slot +0xa, slot +0xe (stack
//        merge by record +0x2c), backpack grids +0x16 / +0x1a (0x006de200 cell, 0x006de060 item
//        under, merge or swap), belt grid +0x12, give to the clicked unit (0x006ffd90 /
//        0x00495f40), throw into the world (record byte +0x25c, 0x00512830), else put it back
//        (switch on +0x11, table 0x6dccb7);
//        empty hand: pick up from the slots / grids on press (0x005940f0), else hover tooltips
//        (0x00593980) into +0x27 / +0x29 / +0x2d / +0x2f.
namespace {
namespace game_b110_03 {

// 12-byte vector passed by value (copied as raw dwords like the original's mov copies).
struct V3 {
    u32 x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

using joan::orig::helpers::units;
void* input() { return obj(d::g_mouse); }
void* records() { return obj(d::g_item_res); }

// Item record of id (0x00425ce0 reads the word argument only).
void* record(i32 item) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(records())->get(joan::orig::arg::i16(item))); }  // 0x00425ce0
// The unit's equipment holder (Unit::modifiers) and its held-item slots (+2 / +6 hands, +a equipped, +e extra).
jo::Modifiers* holder(const void* unit) { return jo::as_unit(unit)->modifiers; }
enum class Slot { hand0, hand1, equipped, extra };
template <Slot S>
jo::HeldItem* slot(const void* unit) {
    if constexpr (S == Slot::hand0) return holder(unit)->hands[0];
    else if constexpr (S == Slot::hand1) return holder(unit)->hands[1];
    else if constexpr (S == Slot::equipped) return holder(unit)->equipped;
    else return holder(unit)->extra_item;
}
// A grid of the holder (UnitData::list_12 belt / grid_16 / grid_1a backpack).
using GridOf = jo::Grid* jo::UnitData::*;
jo::Grid* grid(const void* unit, GridOf g) { return holder(unit)->*g; }

JOAN_COUNTER(c_update, "Inventory::update_6d8ac0");

struct H {
    // ---- this (Inventory screen 0xa488e0) fields used below:
    //  +0x08 i8 mouse state, +0x09 hold time, +0x0d press time, +0x11 i8 carried-from kind,
    //  +0x12 i16 carried item id, +0x14 / +0x15 bytes, +0x16 i8, +0x17 dword (word arg),
    //  +0x1b / +0x1f / +0x23 dwords, +0x27 w, +0x29 d, +0x2d w, +0x2f w, +0x31 / +0x33 w mouse,
    //  +0x35 d time.
    i32 w12() { return jo::self<jo::InventoryScreen>(this)->carried_item; }
    // Word arguments: the original pushes registers whose upper half is stale; every callee
    // used with them reads the low word / byte only (checked: 0x0050c4f0, 0x00497270,
    // 0x004973a0, 0x004975d0, 0x004976e0, 0x004574a0, 0x00425ce0, 0x006de590, 0x006e0b70).
    i32 a12() { return jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item); }
    i32 a17() { return static_cast<u16>(jo::self<jo::InventoryScreen>(this)->i32_17); }
    i32 a16() { return static_cast<u16>(static_cast<i16>(jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_count))); }
    u32 a14() { return jo::self<jo::InventoryScreen>(this)->carried_grid_x; }
    u32 a15() { return jo::self<jo::InventoryScreen>(this)->carried_grid_y; }

    // 0x006de4d0 Inventory::item_sound(int item)
    void sound() { joan::orig::as_inventory(this)->item_sound(joan::orig::arg::u32(w12())); }  // 0x006de4d0
    // 0x0050c4f0 Obj::slot_event(a, b, item, w17, w16, d1b, d1f, d23) on the unit
    void slot_event(void* unit, i32 a, i32 b) {
        joan::orig::as_unit(unit)->slot_event(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::i16(a12()), joan::orig::arg::i16(a17()), joan::orig::arg::i16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)));  // 0x0050c4f0
    }
    // Owner check: record word +0x255 set and different from the unit's +0x3b.
    static bool foreign(const void* rec, const void* unit) {
        return jo::view<i16>(jo::as_item_def(rec)->usable_unit) != 0 && jo::as_unit(unit)->type_id != jo::view<i16>(jo::as_item_def(rec)->usable_unit);
    }
    // 0x006d91fa: drop the carried item.
    void clear_carried() {
        jo::self<jo::InventoryScreen>(this)->carried_source = 0xff;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_17) = 0;
        jo::self<jo::InventoryScreen>(this)->carried_count = 0;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = 0;
        jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = 0;
    }
    // Drop the carried item and the whole carry state (+0x1b / +0x1f too, state +8 = 0).
    void clear_carried_all() {
        jo::self<jo::InventoryScreen>(this)->carried_source = 0xff;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_17) = 0;
        jo::self<jo::InventoryScreen>(this)->carried_count = 0;
        jo::self<jo::InventoryScreen>(this)->carried_uses = 0;
        jo::self<jo::InventoryScreen>(this)->carried_max_uses = 0;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = 0;
        jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = 0;
        jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 0;
    }
    // Slot record fields saved before a swap (movsx words + dwords, in the original's order).
    struct Saved {
        i32 id, s2, s4;
        u32 d6, da, de;
    };
    template <Slot S>
    static Saved save_slot(const void* unit) {
        Saved s{};
        s.id = jo::view<i16>(jo::as_held_item(slot<S>(unit))->item);
        s.s2 = jo::view<i16>(jo::as_held_item(slot<S>(unit))->count);
        s.s4 = jo::view<i16>(jo::as_held_item(slot<S>(unit))->u16_4);
        s.d6 = jo::view<u32>(jo::as_held_item(slot<S>(unit))->uses);
        s.da = jo::view<u32>(jo::as_held_item(slot<S>(unit))->max_uses);
        s.de = jo::as_held_item(slot<S>(unit))->u32_e;
        return s;
    }
    // Take the saved slot item into the hand (+8 = 4).
    void take_saved(const Saved& s) {
        jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = static_cast<u16>(s.id);
        jo::self<jo::InventoryScreen>(this)->i32_17 = s.s4;
        jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(s.s2);
        jo::self<jo::InventoryScreen>(this)->carried_uses = s.d6;
        jo::self<jo::InventoryScreen>(this)->carried_max_uses = s.da;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = s.de;
        jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 4;
    }

    // ------------------------------------------------------------ 0x006d8c45 .. 0x006d922a
    // Carried item dropped on its source slot kind (switch on +0x11, jump table 0x6dcc8b).
    // Returns 0 / 1 for a return of al, -1 to continue at 0x006d9832.
    int drop_carried(void* unit) {
        void* const rec = record(a12());  // [ebp-0xc]
        switch (static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_source)) - 1)) {
        case 0:  // 1
            if (foreign(rec, unit)) return 0;
            slot_event(unit, 1, 1);
            sound();
            break;
        case 1:  // 2
            if (foreign(rec, unit)) return 0;
            slot_event(unit, 1, 0);
            sound();
            break;
        case 6:  // 7
            if (foreign(rec, unit)) return 0;
            if (jo::as_unit_data(holder(unit))->extra_item != nullptr) {
                const i32 k = jo::view<i16>(jo::as_item_def(record(jo::as_held_item(slot<Slot::extra>(unit))->item))->type_id);  // [ebp-0x10]
                if (jo::view<i16>(jo::as_item_def(rec)->type_id) == 0xb) {
                    if (k == 0x10 || k == 0x11 || k == 0x12) return 0;
                } else {
                    if (k == 0xd || k == 0xe || k == 0xf) return 0;
                }
            }
            joan::orig::as_names(holder(unit))->set_item(joan::orig::arg::u32(a12()), joan::orig::arg::u32(a17()), joan::orig::arg::u32(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)));  // 0x00497270
            sound();
            break;
        case 7:  // 8
            if (foreign(rec, unit)) return 0;
            if (jo::as_unit_data(holder(unit))->equipped != nullptr) {
                const i32 k = jo::view<i16>(jo::as_item_def(record(jo::as_held_item(slot<Slot::equipped>(unit))->item))->type_id);  // [ebp-0x14]
                if (k == 0xb) {
                    const i32 r = jo::view<i16>(jo::as_item_def(rec)->type_id);
                    if (r == 0x10 || r == 0x11 || r == 0x12) return 0;
                } else {
                    const i32 r = jo::view<i16>(jo::as_item_def(rec)->type_id);
                    if (r == 0xd || r == 0xe || r == 0xf) return 0;
                }
            }
            joan::orig::as_unit_data(holder(unit))->new_stack_e(joan::orig::arg::u32(a12()), joan::orig::arg::u32(a17()), joan::orig::arg::u32(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)));  // 0x004973a0
            sound();
            break;
        case 8:  // 9
            if (foreign(rec, unit)) return 0;
            if (joan::orig::arg::as<i32>(joan::orig::as_grid(grid(unit, &jo::UnitData::list_12))->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rec)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rec)->stats[1])))), 0, static_cast<std::int8_t>(joan::orig::arg::i32(a14())), static_cast<std::int8_t>(joan::orig::arg::i32(a15())), 0)) == 0)  // 0x004574a0
                return 0;
            joan::orig::as_unit_data(holder(unit))->add_to_grid12(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(a14())), static_cast<std::uint8_t>(joan::orig::arg::u32(a15())));  // 0x004975d0
            sound();
            break;
        case 9:  // 10
            if (foreign(rec, unit)) return 0;
            if (joan::orig::arg::as<i32>(joan::orig::as_grid(grid(unit, &jo::UnitData::grid_16))->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rec)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rec)->stats[1])))), 0, static_cast<std::int8_t>(joan::orig::arg::i32(a14())), static_cast<std::int8_t>(joan::orig::arg::i32(a15())), 0)) == 0)
                return 0;
            joan::orig::as_unit_data(holder(unit))->add_to_grid16_1a(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(a14())), static_cast<std::uint8_t>(joan::orig::arg::u32(a15())), static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x004976e0
            sound();
            break;
        case 10:  // 11
            if (foreign(rec, unit)) return 0;
            if (joan::orig::arg::as<i32>(joan::orig::as_grid(grid(unit, &jo::UnitData::grid_1a))->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rec)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rec)->stats[1])))), 0, static_cast<std::int8_t>(joan::orig::arg::i32(a14())), static_cast<std::int8_t>(joan::orig::arg::i32(a15())), 0)) == 0)
                return 0;
            joan::orig::as_unit_data(holder(unit))->add_to_grid16_1a(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(a14())), static_cast<std::uint8_t>(joan::orig::arg::u32(a15())), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x004976e0
            sound();
            break;
        default:
            break;
        }
        clear_carried();  // 0x006d91fa
        return -1;
    }

    // ------------------------------------------------------------ 0x006d922a / 0x006d9535
    // Click on a backpack grid (holder +0x16 at (0x21a, 0x3e), +0x1a at (0x21a, 0x1a6)): use a
    // potion / scroll / letter or a town item. The two blocks differ in details (kept apart).
    // Returns 0 / 1 for a return of al, -1 to continue at 0x006d9832.
    int use_grid_16(void* unit, u32 id) {
        const i32 cx = (mouse_x() - 0x21a) / 0x28;  // [ebp-0x18]
        const i32 cy = (mouse_y() - 0x3e) / 0x28;   // [ebp-0x1c]
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_grid(grid(unit, &jo::UnitData::grid_16))->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170 [ebp-0x20]
        if (k == 0) return -1;
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_ptr_list(grid(unit, &jo::UnitData::grid_16))->get(joan::orig::arg::i32(k)));  // 0x00457e90 [ebp-0x24]
        void* const r = record(jo::view<u16>(jo::as_inv_item(e)->type));                             // [ebp-0x28]
        if (jo::view<i16>(jo::as_item_def(r)->type_id) == 0x15) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->heal(joan::orig::arg::u32(static_cast<i32>(jo::as_inv_item(e)->type)), 0, 1)) != 0) {  // 0x0050b4f0
                joan::orig::as_hud(obj(d::g_player_hud))->update_health_state(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x00703750
                jo::view<u16>(jo::as_inv_item(e)->count) = static_cast<u16>(jo::view<u16>(jo::as_inv_item(e)->count) - 1);
                if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(grid(unit, &jo::UnitData::grid_16))->fill_from(joan::orig::arg::i32(k));  // 0x00457ec0
                return 1;
            }
            return 1;
        }
        if (jo::view<i16>(jo::as_item_def(r)->type_id) == 0x17) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->heal_group(joan::orig::arg::u32(static_cast<i32>(jo::as_inv_item(e)->type)), 0, 1)) != 0) {  // 0x0050ba00
                if (static_cast<i32>(jo::as_unit(unit)->id) == global<i32>(d::g_player_unit_recno))
                    global<u32>(d::g_a4d63c) = jo::view<u32>(jo::as_unit(unit)->mana);
                jo::view<u16>(jo::as_inv_item(e)->count) = static_cast<u16>(jo::view<u16>(jo::as_inv_item(e)->count) - 1);
                if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(grid(unit, &jo::UnitData::grid_16))->fill_from(joan::orig::arg::i32(k));
                return 1;
            }
            return 1;
        }
        if (jo::view<i16>(jo::as_item_def(r)->type_id) == 0x19) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->heal_and_mana(joan::orig::arg::u32(static_cast<i32>(jo::as_inv_item(e)->type)), 0, 1)) != 0) {  // 0x0050bcd0
                jo::view<u16>(jo::as_inv_item(e)->count) = static_cast<u16>(jo::view<u16>(jo::as_inv_item(e)->count) - 1);
                if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(grid(unit, &jo::UnitData::grid_16))->fill_from(joan::orig::arg::i32(k));
                return 1;
            }
            return 1;
        }
        grid_other(unit, id, r, e, k, &jo::UnitData::grid_16);
        return -1;
    }
    // Shared tail of both grid blocks (0x006d941f / 0x006d9721): identical code apart from the grid.
    void grid_other(void* unit, u32 id, void* r, void* e, u32 k, GridOf g) {
        if (jo::view<i16>(jo::as_item_def(r)->class_id) == 0x10) {
            if (jo::view<i16>(jo::as_item_def(r)->usable_unit) == 0 || jo::view<i16>(jo::as_item_def(r)->usable_unit) == jo::as_unit(unit)->type_id) {
                if (static_cast<i32>(jo::as_unit(unit)->stats[4]) < jo::as_item_def(r)->defense) {
                    jo::view<u16>(jo::as_unit(unit)->stats[4]) = static_cast<u16>(jo::as_item_def(r)->defense);
                    joan::orig::as_unit(unit)->set_anim_set(joan::orig::arg::i32(jo::view<u32>(jo::as_item_def(r)->damage)));  // 0x00511060
                    jo::view<u16>(jo::as_unit(unit)->stats[5]) = jo::as_item_def(r)->id;
                }
                joan::orig::as_grid(grid(unit, g))->fill_from(joan::orig::arg::i32(k));  // 0x00457ec0
            }
            return;
        }
        if (jo::view<i16>(jo::as_item_def(r)->class_id) == 0xf && jo::view<i16>(jo::as_item_def(r)->type_id) == 0x23) {
            joan::orig::as_letter(obj(d::g_letter))->load_6e0b70(joan::orig::arg::i16(static_cast<i32>(jo::view<u16>(jo::as_inv_item(e)->type))));  // 0x006e0b70
            joan::orig::as_letter(obj(d::g_letter))->open();                                     // 0x006e0900
            return;
        }
        if (jo::view<i16>(jo::as_item_def(r)->combine_item) == 0) return;
        if (jo::view<i16>(jo::as_item_def(r)->result_item) == 0) return;
        if (joan::orig::arg::as<u8>(joan::orig::as_inventory(this)->move_to_town(joan::orig::arg::i32(id), joan::orig::arg::u32(static_cast<i32>(jo::as_item_def(r)->id)))) != 0)  // 0x006de590
            joan::orig::as_grid(grid(unit, g))->fill_from(joan::orig::arg::i32(k));
    }
    int use_grid_1a(void* unit, u32 id) {
        const i32 cx = (mouse_x() - 0x21a) / 0x28;  // [ebp-0x2c]
        const i32 cy = (mouse_y() - 0x1a6) / 0x28;  // [ebp-0x30]
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_grid(grid(unit, &jo::UnitData::grid_1a))->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170 [ebp-0x34]
        if (k == 0) return -1;
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_ptr_list(grid(unit, &jo::UnitData::grid_1a))->get(joan::orig::arg::i32(k)));  // 0x00457e90 [ebp-0x38]
        void* const r = record(jo::view<u16>(jo::as_inv_item(e)->type));                             // [ebp-0x3c]
        if (jo::view<i16>(jo::as_item_def(r)->type_id) == 0x15) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->heal(joan::orig::arg::u32(static_cast<i32>(jo::as_inv_item(e)->type)), 0, 1)) != 0) {  // 0x0050b4f0
                joan::orig::as_hud(obj(d::g_player_hud))->update_health_state(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x00703750
                jo::view<u16>(jo::as_inv_item(e)->count) = static_cast<u16>(jo::view<u16>(jo::as_inv_item(e)->count) - 1);
                if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(grid(unit, &jo::UnitData::grid_1a))->fill_from(joan::orig::arg::i32(k));
                return 1;
            }
            return 1;
        }
        if (jo::view<i16>(jo::as_item_def(r)->type_id) == 0x17) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->heal_group(joan::orig::arg::u32(static_cast<i32>(jo::as_inv_item(e)->type)), 0, 0)) != 0) {  // 0x0050ba00
                global<u32>(d::g_a4d63c) = jo::view<u32>(jo::as_unit(unit)->mana);
                jo::view<u16>(jo::as_inv_item(e)->count) = static_cast<u16>(jo::view<u16>(jo::as_inv_item(e)->count) - 1);
                if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(grid(unit, &jo::UnitData::grid_1a))->fill_from(joan::orig::arg::i32(k));
                return 1;
            }
            return 1;
        }
        if (jo::view<i16>(jo::as_item_def(r)->type_id) == 0x19) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->heal_and_mana(joan::orig::arg::u32(static_cast<i32>(jo::as_inv_item(e)->type)), 0, 1)) != 0) {  // 0x0050bcd0
                jo::view<u16>(jo::as_inv_item(e)->count) = static_cast<u16>(jo::view<u16>(jo::as_inv_item(e)->count) - 1);
                if (jo::as_inv_item(e)->count == 0) joan::orig::as_grid(grid(unit, &jo::UnitData::grid_1a))->fill_from(joan::orig::arg::i32(k));
                return 1;
            }
            return 1;
        }
        grid_other(unit, id, r, e, k, &jo::UnitData::grid_1a);
        return -1;
    }
    bool state_34() { return jo::self<jo::InventoryScreen>(this)->open == 4 || jo::self<jo::InventoryScreen>(this)->open == 3; }
    // 0x00594900 Parser::rect_mode2(x0, y0, x1, y1, 0) on the input object
    static bool in_rect(i32 x0, i32 y0, i32 x1, i32 y1) {
        return joan::orig::arg::as<i32>(joan::orig::as_parser(input())->rect_mode2(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), 0)) != 0;  // 0x00594900
    }
    i32 rec2() { return jo::view<i16>(jo::as_item_def(record(a12()))->type_id); }  // fresh record of the carried item, word +2
    template <Slot S>
    static i32 slot_rec2(const void* unit) { return jo::view<i16>(jo::as_item_def(record(jo::as_held_item(slot<S>(unit))->item))->type_id); }
    void put_e(void* unit) {  // 0x004973a0 Owner::new_record (slot +0xe)
        joan::orig::as_unit_data(holder(unit))->new_stack_e(joan::orig::arg::u32(a12()), joan::orig::arg::u32(a17()), joan::orig::arg::u32(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)));
    }
    void put_a(void* unit) {  // 0x00497270 Names::set_item (slot +0xa)
        joan::orig::as_names(holder(unit))->set_item(joan::orig::arg::u32(a12()), joan::orig::arg::u32(a17()), joan::orig::arg::u32(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)));
    }

    // ------------------------------------------------------------ 0x006d9882 (rect 0x15f,0x3c-0x1ff,0xdc)
    // Carried item of kind 8 / 9 / 10 dropped on the hand slots (+2 / +6).
    u8 drop_hands(void* unit) {
        void* const r40 = record(a12());  // [ebp-0x40]
        {
            const i32 k = jo::view<i16>(jo::as_item_def(r40)->type_id);
            if (k != 8 && k != 9 && k != 0xa) return 1;
        }
        if (foreign(r40, unit)) return 1;
        const i32 mx = mouse_x();  // [ebp-0x44]
        const i32 my = mouse_y();  // [ebp-0x48]
        if (mx < 0x15f || mx > 0x1ff || my < 0x3c || my > 0xdc) return 1;
        if (rec2() == 0xa) {
            if (jo::as_unit_data(holder(unit))->hands[0] != nullptr && jo::as_unit_data(holder(unit))->hands[1] == nullptr) {
                const Saved s = save_slot<Slot::hand0>(unit);           // [ebp-0x4c..0x60]
                joan::orig::as_cell(unit)->release_slot(joan::orig::arg::i16(1), joan::orig::arg::i16(0));             // 0x0050c430
                slot_event(unit, 1, 0);
                sound();
                take_saved(s);
                return 1;
            }
            if (jo::as_unit_data(holder(unit))->hands[0] == nullptr && jo::as_unit_data(holder(unit))->hands[1] == nullptr) {
                slot_event(unit, 1, 0);
                sound();
                clear_carried_all();
                return 1;
            }
            return 1;
        }
        if (jo::view<i16>(jo::as_item_def(r40)->type_id) == 8) {
            if (jo::as_unit_data(holder(unit))->hands[1] != nullptr && jo::as_unit_data(holder(unit))->hands[0] == nullptr) {
                const Saved s = save_slot<Slot::hand1>(unit);  // [ebp-0x64..0x78]
                joan::orig::as_cell(unit)->release_slot(joan::orig::arg::i16(1), joan::orig::arg::i16(1));    // 0x0050c430
                slot_event(unit, 1, 1);
                sound();
                take_saved(s);
                return 1;
            }
            if (jo::as_unit_data(holder(unit))->hands[1] == nullptr && jo::as_unit_data(holder(unit))->hands[0] == nullptr) {
                slot_event(unit, 1, 1);
                sound();
                clear_carried_all();
                return 1;
            }
            // falls to 0x006d9d3e: +2 == 9 is false -> return 1
        }
        if (jo::view<i16>(jo::as_item_def(r40)->type_id) == 9) {
            if (jo::as_unit_data(holder(unit))->hands[0] != nullptr && jo::as_unit_data(holder(unit))->hands[1] == nullptr) {
                const Saved s = save_slot<Slot::hand0>(unit);  // [ebp-0x7c..0x90]
                joan::orig::as_cell(unit)->release_slot(joan::orig::arg::i16(1), joan::orig::arg::i16(0));    // 0x0050c430
                slot_event(unit, 1, 0);
                sound();
                take_saved(s);
                return 1;
            }
            if (jo::as_unit_data(holder(unit))->hands[0] == nullptr && jo::as_unit_data(holder(unit))->hands[1] == nullptr) {
                slot_event(unit, 1, 0);
                sound();
                clear_carried_all();
                return 1;
            }
            return 1;
        }
        return 1;
    }

    // ------------------------------------------------------------ 0x006d9f9e (rect 0x15f,0x12a-0x1af,0x1cc)
    // Carried item of kind 0xb / 0xc dropped on slot +0xa.
    u8 drop_slot_a(void* unit) {
        void* const r94 = record(a12());  // [ebp-0x94]
        if (jo::view<i16>(jo::as_item_def(r94)->type_id) != 0xb && jo::view<i16>(jo::as_item_def(r94)->type_id) != 0xc) return 1;
        if (foreign(r94, unit)) return 1;
        if (jo::view<i16>(jo::as_item_def(r94)->type_id) == 0xb && jo::as_unit_data(holder(unit))->extra_item != nullptr) {
            if (slot_rec2<Slot::extra>(unit) != 0xd && slot_rec2<Slot::extra>(unit) != 0xe && slot_rec2<Slot::extra>(unit) != 0xf) return 1;
        }
        if (jo::view<i16>(jo::as_item_def(r94)->type_id) == 0xc && jo::as_unit_data(holder(unit))->extra_item != nullptr) {
            if (slot_rec2<Slot::extra>(unit) != 0x10 && slot_rec2<Slot::extra>(unit) != 0x11 && slot_rec2<Slot::extra>(unit) != 0x12)
                return 1;
        }
        if (jo::as_unit_data(holder(unit))->equipped != nullptr) {
            const Saved s = save_slot<Slot::equipped>(unit);  // [ebp-0x98..0xac]
            joan::orig::as_unit_data(holder(unit))->free_slot_a();    // 0x00497350
            put_a(unit);
            sound();
            take_saved(s);
            return 1;
        }
        put_a(unit);
        sound();
        clear_carried_all();
        return 1;
    }

    // ------------------------------------------------------------ 0x006da349 (rect 0x1d4,0x130-0x1fc,0x1d0)
    // Carried item of kind 0xd .. 0x12 dropped on slot +0xe (stacks merge by id up to +0x2c).
    u8 drop_slot_e(void* unit) {
        if (rec2() != 0xd && rec2() != 0xe && rec2() != 0xf && rec2() != 0x10 && rec2() != 0x11 &&
            rec2() != 0x12)
            return 1;
        void* const rb0 = record(a12());  // [ebp-0xb0]
        {
            const i32 k = jo::view<i16>(jo::as_item_def(rb0)->type_id);
            if ((k == 0xd || k == 0xe || k == 0xf) && jo::as_unit_data(holder(unit))->equipped != nullptr) {
                if (slot_rec2<Slot::equipped>(unit) != 0xb) return 1;
            }
        }
        {
            const i32 k = jo::view<i16>(jo::as_item_def(rb0)->type_id);
            if ((k == 0x10 || k == 0x11 || k == 0x12) && jo::as_unit_data(holder(unit))->equipped != nullptr) {
                if (slot_rec2<Slot::equipped>(unit) != 0xc) return 1;
            }
        }
        if (jo::as_unit_data(holder(unit))->extra_item != nullptr) {
            const Saved s = save_slot<Slot::extra>(unit);  // [ebp-0xb4..0xc8]
            if (s.id == w12()) {
                const i32 lim = jo::view<i16>(jo::as_item_def(record(static_cast<u16>(s.id)))->stats[4]);
                if (lim >= static_cast<i32>(jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_count)) + s.s2) {
                    void* const p = slot<Slot::extra>(unit);
                    const u16 sum = static_cast<u16>(jo::as_held_item(p)->count + static_cast<u16>(static_cast<i16>(jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_count))));
                    jo::as_held_item(slot<Slot::extra>(unit))->count = sum;
                    jo::self<jo::InventoryScreen>(this)->carried_source = 0xff;
                    jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_17) = 0;
                    jo::self<jo::InventoryScreen>(this)->carried_count = 0;
                    jo::self<jo::InventoryScreen>(this)->carried_uses = 0;
                    jo::self<jo::InventoryScreen>(this)->carried_max_uses = 0;
                    jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = 0;
                    jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 0;
                    return 1;
                }
                const i32 room = static_cast<i32>(jo::view<i16>(jo::as_item_def(record(static_cast<u16>(s.id)))->stats[4])) - s.s2;
                jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(jo::self<jo::InventoryScreen>(this)->carried_count - static_cast<u8>(room));
                void* const r = record(static_cast<u16>(s.id));
                jo::as_held_item(slot<Slot::extra>(unit))->count = jo::as_item_def(r)->stats[4];
                jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 4;
                return 1;
            }
            joan::orig::as_unit_data(holder(unit))->free_slot_e();  // 0x00497440
            put_e(unit);
            sound();
            take_saved(s);
            return 1;
        }
        put_e(unit);
        sound();
        clear_carried_all();
        return 1;
    }
    static void* gget(const void* unit, GridOf g, u32 k) {  // 0x00457e90 PtrList::get on grid g
        return joan::orig::arg::as<void*>(joan::orig::as_ptr_list(grid(unit, g))->get(joan::orig::arg::i32(k)));
    }

    // ------------------------------------------------------------ 0x006da7ff / 0x006dacd4
    // Carried item dropped into a backpack grid (holder +0x16 rect (0x21a, 0x3e)-(0x30a, 0x17e), lower
    // flag 0 / holder +0x1a rect (0x21a, 0x1a6)-(0x30a, 0x246), lower flag 1): free area -> place;
    // else the item under it: same id and +4 -> merge the stacks (limit record +0x2c), else swap.
    u8 drop_grid(void* unit, u32 id, GridOf g, u32 lower) {
        void* const rcc = record(a12());  // [ebp-0xcc]
        i32 x = 0;                        // [ebp-0xd0] (always written by 0x006de200)
        i32 y = 0;                        // [ebp-0xd4]
        joan::orig::as_grid(this)->cell_at_mouse(static_cast<i32>(jo::view<i16>(jo::as_item_def(rcc)->stats[0])), static_cast<i32>(jo::view<i16>(jo::as_item_def(rcc)->stats[1])), &x, &y, joan::orig::arg::u32(lower));  // 0x006de200
        if (x < 0 || y < 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(grid(unit, g))->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rcc)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(rcc)->stats[1])))), 0, static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(x)))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(y)))), 0)) != 0) {  // 0x004574a0
            joan::orig::as_unit_data(holder(unit))->add_to_grid16_1a(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(static_cast<u8>(x)))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(static_cast<u8>(y)))), static_cast<std::uint8_t>(joan::orig::arg::u32(lower)));  // 0x004976e0
            sound();
            clear_carried_all();
            return 1;
        }
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->item_under(joan::orig::arg::i32(id), static_cast<i32>(jo::view<i16>(jo::as_item_def(rcc)->stats[0])), static_cast<i32>(jo::view<i16>(jo::as_item_def(rcc)->stats[1])), 0, joan::orig::arg::i32(x), joan::orig::arg::i32(y), static_cast<std::int8_t>(joan::orig::arg::i32(lower))));  // 0x006de060 [ebp-0xd8]
        if (k == 0) return 1;
        const i32 dc = jo::as_inv_item(gget(unit, g, k))->type;  // [ebp-0xdc] / [ebp-0x104]
        // The +0x1a block reads the entry's word +4 once before the sound ([ebp-0x108]); the +0x16
        // block re-fetches it after the id compare.
        const i32 e4_early = lower != 0 ? static_cast<i32>(jo::as_inv_item(gget(unit, g, k))->level) : 0;
        sound();
        if (dc == w12() &&
            (lower != 0 ? e4_early : static_cast<i32>(jo::as_inv_item(gget(unit, g, k))->level)) == jo::self<jo::InventoryScreen>(this)->i32_17) {
            const i32 lim = jo::view<i16>(jo::as_item_def(record(static_cast<u16>(dc)))->stats[4]);
            const i32 have = jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_count);
            if (lim >= have + static_cast<i32>(jo::as_inv_item(gget(unit, g, k))->count)) {
                u16& p = jo::view<u16>(jo::as_inv_item(gget(unit, g, k))->count);  // [ebp-0x1c0] / [ebp-0x1c4]
                p = static_cast<u16>(p + static_cast<u16>(static_cast<i16>(jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_count))));
                clear_carried_all();
                return 1;
            }
            i32 room = jo::view<i16>(jo::as_item_def(record(static_cast<u16>(dc)))->stats[4]);
            room -= jo::as_inv_item(gget(unit, g, k))->count;
            jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(jo::self<jo::InventoryScreen>(this)->carried_count - static_cast<u8>(room));
            void* const r = record(static_cast<u16>(dc));
            jo::view<u16>(jo::as_inv_item(gget(unit, g, k))->count) = jo::as_item_def(r)->stats[4];
            jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 4;
            return 1;
        }
        const i32 e0 = jo::as_inv_item(gget(unit, g, k))->count;   // [ebp-0xe0]
        const i32 e4 = jo::as_inv_item(gget(unit, g, k))->level;   // [ebp-0xe4]
        const u32 e8 = jo::as_inv_item(gget(unit, g, k))->u32_6;   // [ebp-0xe8]
        const u32 ec = jo::as_inv_item(gget(unit, g, k))->u32_a; // [ebp-0xec]
        const u32 f0 = jo::as_inv_item(gget(unit, g, k))->u32_e; // [ebp-0xf0]
        joan::orig::as_unit_data(holder(unit))->fill_grid16_1a(joan::orig::arg::u32(static_cast<i32>(static_cast<u16>(k))), joan::orig::arg::u32(lower));  // 0x004978a0
        joan::orig::as_unit_data(holder(unit))->add_to_grid16_1a(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(static_cast<u8>(x)))), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<u32>(static_cast<u8>(y)))), static_cast<std::uint8_t>(joan::orig::arg::u32(lower)));  // 0x004976e0
        jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = static_cast<u16>(dc);
        jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(e0);
        jo::self<jo::InventoryScreen>(this)->i32_17 = e4;
        jo::self<jo::InventoryScreen>(this)->carried_uses = e8;
        jo::self<jo::InventoryScreen>(this)->carried_max_uses = ec;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = f0;
        jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 4;
        return 1;
    }
    // ------------------------------------------------------------ 0x006db1f9 (rect 0x15f,0x220-0x1fe,0x247)
    // Carried item of class 9 dropped on the belt grid (holder +0x12): place or swap.
    u8 drop_belt(void* unit) {
        void* const r120 = record(a12());  // [ebp-0x120]
        if (jo::view<i16>(jo::as_item_def(r120)->class_id) != 9) return 1;
        if (foreign(r120, unit)) return 1;
        const i32 cx = (mouse_x() - 0x15f) / 0x28;  // [ebp-0x124]
        const i32 cy = (mouse_y() - 0x220) / 0x28;  // [ebp-0x128]
        void* const r12c = record(a12());           // [ebp-0x12c]
        const u32 bx = static_cast<u8>(cx);
        const u32 by = static_cast<u8>(cy);
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(grid(unit, &jo::UnitData::list_12))->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(r12c)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(r12c)->stats[1])))), 0, static_cast<std::int8_t>(joan::orig::arg::i32(bx)), static_cast<std::int8_t>(joan::orig::arg::i32(by)), 0)) != 0) {  // 0x004574a0
            joan::orig::as_unit_data(holder(unit))->add_to_grid12(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(bx)), static_cast<std::uint8_t>(joan::orig::arg::u32(by)));  // 0x004975d0
            sound();
            clear_carried_all();
            return 1;
        }
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_grid(grid(unit, &jo::UnitData::list_12))->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170 [ebp-0x130]
        if (k == 0) return 1;
        const i32 e0 = jo::as_inv_item(gget(unit, &jo::UnitData::list_12, k))->type;    // [ebp-0x134]
        const i32 e2 = jo::as_inv_item(gget(unit, &jo::UnitData::list_12, k))->count;    // [ebp-0x138]
        const i32 e4 = jo::as_inv_item(gget(unit, &jo::UnitData::list_12, k))->level;    // [ebp-0x13c]
        const u32 e6 = jo::as_inv_item(gget(unit, &jo::UnitData::list_12, k))->u32_6;    // [ebp-0x140]
        const u32 ea = jo::as_inv_item(gget(unit, &jo::UnitData::list_12, k))->u32_a;  // [ebp-0x144]
        const u32 ee = jo::as_inv_item(gget(unit, &jo::UnitData::list_12, k))->u32_e;  // [ebp-0x148]
        joan::orig::as_unit_data(holder(unit))->fill_grid12(joan::orig::arg::i16(static_cast<i32>(static_cast<u16>(k))));  // 0x004976b0
        joan::orig::as_unit_data(holder(unit))->add_to_grid12(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(bx)), static_cast<std::uint8_t>(joan::orig::arg::u32(by)));  // 0x004975d0
        sound();
        jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = static_cast<u16>(e0);
        jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(e2);
        jo::self<jo::InventoryScreen>(this)->i32_17 = e4;
        jo::self<jo::InventoryScreen>(this)->carried_uses = e6;
        jo::self<jo::InventoryScreen>(this)->carried_max_uses = ea;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = ee;
        jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 4;
        return 1;
    }
    // Drop the carried item without touching the mouse state +8.
    void clear_carried_keep_state() {
        jo::self<jo::InventoryScreen>(this)->carried_source = 0xff;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_17) = 0;
        jo::self<jo::InventoryScreen>(this)->carried_count = 0;
        jo::self<jo::InventoryScreen>(this)->carried_uses = 0;
        jo::self<jo::InventoryScreen>(this)->carried_max_uses = 0;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = 0;
        jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = 0;
    }
    static bool joy_pressed() { return joan::orig::arg::as<i32>(joan::orig::as_joystick(input())->pressed_kind(0)) != 0; }  // 0x00594a10
    // 0x00717a90 UnitMarker::in_range(V3 pos) on 0xa4f381 with the unit's position +0xe (al).
    static bool in_range(const void* u) {
        V3 v;
        std::memcpy(&v, jo::as_unit(u)->position_, sizeof(v));
        return joan::orig::as_mini_map(obj(d::g_mini_map))->near_tracked_unit(joan::orig::arg::f32(v.x), joan::orig::arg::f32(v.y), joan::orig::arg::u32(v.z));  // 0x00717a90
    }

    // ------------------------------------------------------------ 0x006d9832 .. 0x006dbd7e
    // A carried item (+0x12 != 0): drop it on a slot / grid, give it to a clicked unit, throw it
    // into the world, or put it back where it came from (switch on +0x11, jump table 0x6dccb7).
    u8 carried(void* unit, u32 id) {
        if (state_34() && in_rect(0x15f, 0x3c, 0x1ff, 0xdc)) return drop_hands(unit);
        if (state_34() && in_rect(0x15f, 0x12a, 0x1af, 0x1cc)) return drop_slot_a(unit);
        if (state_34() && in_rect(0x1d4, 0x130, 0x1fc, 0x1d0)) return drop_slot_e(unit);
        if (state_34() && in_rect(0x21a, 0x3e, 0x30a, 0x17e)) return drop_grid(unit, id, &jo::UnitData::grid_16, 0);
        if (state_34() && in_rect(0x21a, 0x1a6, 0x30a, 0x246)) return drop_grid(unit, id, &jo::UnitData::grid_1a, 1);
        if (state_34() && in_rect(0x15f, 0x220, 0x1fe, 0x247)) return drop_belt(unit);
        bool back = false;  // [ebp-0x14c]
        if ((jo::self<jo::InventoryScreen>(this)->open == 3 || jo::self<jo::InventoryScreen>(this)->open == 4) && joy_pressed()) {
            const u32 my = joan::orig::arg::as<u32>(joan::orig::as_table(input())->get_67e(0));  // 0x006def40
            const u32 mx = joan::orig::arg::as<u32>(joan::orig::as_table(input())->get_67a(0));  // 0x006def10
            const u32 t = joan::orig::arg::as<u32>(joan::orig::as_hud(obj(d::g_player_hud))->hit_portrait(joan::orig::arg::i32(mx), joan::orig::arg::i32(my)));  // 0x006ffd90 [ebp-0x150]
            if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(t)) == 0) {  // 0x00428b70
                void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(t));  // 0x0041b3c0 [ebp-0x154]
                if (jo::as_unit(u)->rank >= 4 && static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_game(u)->byte_2e1())) == 0 &&  // 0x00432b90
                    jo::as_unit(u)->speech_off == 0 && in_range(u) &&
                    joan::orig::arg::as<i32>(joan::orig::as_inventory(holder(u))->add_495f40(joan::orig::arg::u32(a12()), joan::orig::arg::u32(a17()), joan::orig::arg::u32(a16()), joan::orig::arg::u32(t), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), 0u)) != 0) {  // 0x00495f40
                    sound();
                    clear_carried_keep_state();
                    return 1;
                }
                if (jo::self<jo::InventoryScreen>(this)->open != 3) return 1;
                back = true;
            }
        }
        if ((jo::self<jo::InventoryScreen>(this)->open == 3 || jo::self<jo::InventoryScreen>(this)->open == 4) && joy_pressed() && !in_rect(0, 0, 0x320, 0x258) &&
            !back) {
            void* const r158 = record(a12());  // [ebp-0x158]
            if (jo::view<i8>(jo::as_item_def(r158)->can_drop) != 0 &&
                joan::orig::arg::as<i32>(joan::orig::as_spawner(unit)->fire_512830(joan::orig::arg::i16(a12()), joan::orig::arg::i16(a17()), joan::orig::arg::i16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)))) != 0) {  // 0x00512830
                clear_carried_keep_state();
                return 1;
            }
        }
        if (!((jo::self<jo::InventoryScreen>(this)->open == 3 && joy_pressed()) || back)) return 0;
        switch (static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_source)) - 1)) {
        case 0:
            slot_event(unit, 1, 1);
            sound();
            break;
        case 1:
            slot_event(unit, 1, 0);
            sound();
            break;
        case 2:
            slot_event(unit, 2, 1);
            sound();
            break;
        case 3:
            slot_event(unit, 2, 0);
            sound();
            break;
        case 4:
            slot_event(unit, 3, 1);
            sound();
            break;
        case 5:
            slot_event(unit, 3, 0);
            sound();
            break;
        case 6:
            put_a(unit);
            sound();
            break;
        case 7:
            put_e(unit);
            sound();
            break;
        case 8:
        case 9:
        case 10: {
            static const GridOf kGrid[3] = {&jo::UnitData::list_12, &jo::UnitData::grid_16, &jo::UnitData::grid_1a};
            const u32 sel = static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::self<jo::InventoryScreen>(this)->carried_source)) - 9);
            void* const r = record(a12());  // [ebp-0x15c] / [ebp-0x160] / [ebp-0x164]
            if (joan::orig::arg::as<i32>(joan::orig::as_grid(grid(unit, kGrid[sel]))->area_free(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(r)->stats[0])))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(static_cast<u8>(jo::as_item_def(r)->stats[1])))), 0, static_cast<std::int8_t>(joan::orig::arg::i32(a14())), static_cast<std::int8_t>(joan::orig::arg::i32(a15())), 0)) == 0)  // 0x004574a0
                return 0;
            if (sel == 0) {
                joan::orig::as_unit_data(holder(unit))->add_to_grid12(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(a14())), static_cast<std::uint8_t>(joan::orig::arg::u32(a15())));  // 0x004975d0
            } else {
                joan::orig::as_unit_data(holder(unit))->add_to_grid16_1a(joan::orig::arg::u16(a12()), joan::orig::arg::u16(a17()), joan::orig::arg::u16(a16()), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_uses), joan::orig::arg::u32(jo::self<jo::InventoryScreen>(this)->carried_max_uses), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(a14())), static_cast<std::uint8_t>(joan::orig::arg::u32(a15())), static_cast<std::uint8_t>(joan::orig::arg::u32(sel == 2 ? 1 : 0)));  // 0x004976e0
            }
            sound();
            break;
        }
        default:
            break;
        }
        clear_carried_keep_state();  // 0x006dbd31
        return 1;
    }
    // ------------------------------------------------------------ 0x006dbd7e
    // Empty hand: press on a slot / grid picks the item up (the mouse state +8 = 1, press time).
    static bool pressed(i32 x0, i32 y0, i32 x1, i32 y1) {
        return joan::orig::arg::as<i32>(joan::orig::as_parser(input())->rect_mode(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), 0)) != 0;  // 0x005940f0
    }
    void press() {
        jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 1;
        jo::self<jo::InventoryScreen>(this)->press_time_ms = global<u32>(d::g_game_time_ms);
        jo::self<jo::InventoryScreen>(this)->press_duration_ms = 0;
    }
    // Right edge of a slot's item picture: record +0x24 cells of 0x28 from x 0x15f.
    static i32 item_right(i32 item) { return static_cast<i32>(jo::view<i16>(jo::as_item_def(record(item))->stats[0])) * 0x28 + 0x15f; }
    // Mouse x not right of the item picture (record looked up first, then the mouse read; jg = signed).
    static bool below_right(i32 item) {
        const i32 right = item_right(item);
        return !(mouse_x() > right);
    }
    // Pick the item of grid g (0x00458170 cell, 0x00457e90 entry), kind = +0x11 value.
    void pick_grid(void* unit, GridOf g, i32 ox, i32 oy, u8 kind) {
        const i32 cx = (mouse_x() - ox) / 0x28;
        const i32 cy = (mouse_y() - oy) / 0x28;
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_grid(grid(unit, g))->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170
        if (k == 0) return;
        jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = jo::view<u16>(jo::as_inv_item(gget(unit, g, k))->type);
        jo::self<jo::InventoryScreen>(this)->carried_grid_x = jo::view<u8>(jo::as_inv_item(gget(unit, g, k))->x);
        jo::self<jo::InventoryScreen>(this)->carried_grid_y = jo::view<u8>(jo::as_inv_item(gget(unit, g, k))->y);
        jo::self<jo::InventoryScreen>(this)->carried_source = kind;
        jo::self<jo::InventoryScreen>(this)->i32_17 = jo::as_inv_item(gget(unit, g, k))->level;
        jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(jo::as_inv_item(gget(unit, g, k))->count);
        jo::self<jo::InventoryScreen>(this)->carried_uses = jo::as_inv_item(gget(unit, g, k))->u32_6;
        jo::self<jo::InventoryScreen>(this)->carried_max_uses = jo::as_inv_item(gget(unit, g, k))->u32_a;
        jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = jo::as_inv_item(gget(unit, g, k))->u32_e;
        joan::orig::as_grid(grid(unit, g))->fill_from(joan::orig::arg::i32(k));  // 0x00457ec0
        sound();
    }
    u8 empty_hand(void* unit) {
        if (pressed(0x15f, 0x3c, 0x1ff, 0xdc)) {
            press();
            if (jo::as_unit_data(holder(unit))->hands[1] != nullptr) {
                jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = jo::as_held_item(slot<Slot::hand1>(unit))->item;
                jo::self<jo::InventoryScreen>(this)->carried_source = 1;
                jo::self<jo::InventoryScreen>(this)->carried_count = 1;
                jo::self<jo::InventoryScreen>(this)->i32_17 = jo::view<i16>(jo::as_held_item(slot<Slot::hand1>(unit))->u16_4);
                jo::self<jo::InventoryScreen>(this)->carried_uses = jo::view<u32>(jo::as_held_item(slot<Slot::hand1>(unit))->uses);
                jo::self<jo::InventoryScreen>(this)->carried_max_uses = jo::view<u32>(jo::as_held_item(slot<Slot::hand1>(unit))->max_uses);
                jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = jo::as_held_item(slot<Slot::hand1>(unit))->u32_e;
                joan::orig::as_cell(unit)->release_slot(joan::orig::arg::i16(1), joan::orig::arg::i16(1));  // 0x0050c430
                sound();
            }
            if (jo::as_unit_data(holder(unit))->hands[0] != nullptr && below_right(jo::as_held_item(slot<Slot::hand0>(unit))->item)) {
                jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = jo::as_held_item(slot<Slot::hand0>(unit))->item;
                jo::self<jo::InventoryScreen>(this)->carried_source = 2;
                jo::self<jo::InventoryScreen>(this)->carried_count = 1;
                jo::self<jo::InventoryScreen>(this)->i32_17 = jo::view<i16>(jo::as_held_item(slot<Slot::hand0>(unit))->u16_4);
                jo::self<jo::InventoryScreen>(this)->carried_uses = jo::view<u32>(jo::as_held_item(slot<Slot::hand0>(unit))->uses);
                jo::self<jo::InventoryScreen>(this)->carried_max_uses = jo::view<u32>(jo::as_held_item(slot<Slot::hand0>(unit))->max_uses);
                jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = jo::as_held_item(slot<Slot::hand0>(unit))->u32_e;
                joan::orig::as_cell(unit)->release_slot(joan::orig::arg::i16(1), joan::orig::arg::i16(0));  // 0x0050c430
                sound();
            }
        }
        if (pressed(0x15f, 0x12a, 0x1af, 0x1cc)) {
            press();
            if (jo::as_unit_data(holder(unit))->equipped != nullptr && below_right(jo::as_held_item(slot<Slot::equipped>(unit))->item)) {
                jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = jo::as_held_item(slot<Slot::equipped>(unit))->item;
                jo::self<jo::InventoryScreen>(this)->carried_source = 7;
                jo::self<jo::InventoryScreen>(this)->i32_17 = jo::view<i16>(jo::as_held_item(slot<Slot::equipped>(unit))->u16_4);
                jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(slot<Slot::equipped>(unit)->count);
                jo::self<jo::InventoryScreen>(this)->carried_uses = jo::view<u32>(jo::as_held_item(slot<Slot::equipped>(unit))->uses);
                jo::self<jo::InventoryScreen>(this)->carried_max_uses = jo::view<u32>(jo::as_held_item(slot<Slot::equipped>(unit))->max_uses);
                jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = jo::as_held_item(slot<Slot::equipped>(unit))->u32_e;
                joan::orig::as_unit_data(holder(unit))->free_slot_a();  // 0x00497350
                sound();
            }
        }
        if (pressed(0x1d4, 0x130, 0x1fc, 0x1d0)) {
            press();
            if (jo::as_unit_data(holder(unit))->extra_item != nullptr) {
                jo::view<u16>(jo::self<jo::InventoryScreen>(this)->carried_item) = jo::as_held_item(slot<Slot::extra>(unit))->item;
                jo::self<jo::InventoryScreen>(this)->carried_source = 8;
                jo::self<jo::InventoryScreen>(this)->i32_17 = jo::view<i16>(jo::as_held_item(slot<Slot::extra>(unit))->u16_4);
                jo::self<jo::InventoryScreen>(this)->carried_count = static_cast<u8>(slot<Slot::extra>(unit)->count);
                jo::self<jo::InventoryScreen>(this)->carried_uses = jo::view<u32>(jo::as_held_item(slot<Slot::extra>(unit))->uses);
                jo::self<jo::InventoryScreen>(this)->carried_max_uses = jo::view<u32>(jo::as_held_item(slot<Slot::extra>(unit))->max_uses);
                jo::view<u32>(jo::self<jo::InventoryScreen>(this)->i32_23) = jo::as_held_item(slot<Slot::extra>(unit))->u32_e;
                joan::orig::as_unit_data(holder(unit))->free_slot_e();  // 0x00497440
                sound();
            }
        }
        if (pressed(0x21a, 0x3e, 0x30a, 0x17e)) {
            press();
            pick_grid(unit, &jo::UnitData::grid_16, 0x21a, 0x3e, 0xa);
            return 1;
        }
        if (pressed(0x21a, 0x1a6, 0x30a, 0x246)) {
            press();
            pick_grid(unit, &jo::UnitData::grid_1a, 0x21a, 0x1a6, 0xb);
            return 1;
        }
        if (pressed(0x15f, 0x220, 0x1fe, 0x247)) {
            press();
            pick_grid(unit, &jo::UnitData::list_12, 0x15f, 0x220, 9);
            return 1;
        }
        if (w12() == 0) jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 0;
        return hover(unit);
    }

    // ------------------------------------------------------------ 0x006dc730
    // Hover (nothing carried): tooltip item +0x27 / +0x29, source +0x2d, grid key +0x2f.
    static bool inside(i32 x0, i32 y0, i32 x1, i32 y1) {
        return joan::orig::arg::as<i32>(joan::orig::as_point(input())->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1))) != 0;  // 0x00593980
    }
    // Returns 1 when an item is found, 0 otherwise (+0x27 = 0).
    u8 hover_grid(void* unit, GridOf g, i32 ox, i32 oy, u16 src) {
        const i32 cx = (mouse_x() - ox) / 0x28;
        const i32 cy = (mouse_y() - oy) / 0x28;
        const u32 k = joan::orig::arg::as<u32>(joan::orig::as_grid(grid(unit, g))->find_at(joan::orig::arg::i32(cx), joan::orig::arg::i32(cy)));  // 0x00458170
        if (k == 0) {
            jo::self<jo::InventoryScreen>(this)->hover_item = 0;
            return 0;
        }
        jo::self<jo::InventoryScreen>(this)->hover_item = jo::view<u16>(jo::as_inv_item(gget(unit, g, k))->type);
        jo::view<i32>(jo::self<jo::InventoryScreen>(this)->u32_29) = jo::as_inv_item(gget(unit, g, k))->level;
        jo::self<jo::InventoryScreen>(this)->hover_source = src;
        jo::self<jo::InventoryScreen>(this)->hover_grid_key = static_cast<u16>(k);
        return 1;
    }
    u8 hover(void* unit) {
        if (w12() != 0) return 0;
        if (inside(0x15f, 0x3c, 0x1ff, 0xdc)) {
            if (jo::as_unit_data(holder(unit))->hands[1] != nullptr) {
                const u16 item = jo::as_held_item(slot<Slot::hand1>(unit))->item;  // [ebp-0x18c]
                if (inside(0x15f, 0x3c, item_right(item), 0xdc)) {
                    jo::self<jo::InventoryScreen>(this)->hover_item = item;
                    jo::view<i32>(jo::self<jo::InventoryScreen>(this)->u32_29) = jo::view<i16>(jo::as_held_item(slot<Slot::hand1>(unit))->u16_4);
                    jo::self<jo::InventoryScreen>(this)->hover_source = 1;
                    jo::self<jo::InventoryScreen>(this)->hover_grid_key = 0;
                }
            } else if (jo::as_unit_data(holder(unit))->hands[0] != nullptr) {
                const u16 item = jo::as_held_item(slot<Slot::hand0>(unit))->item;  // [ebp-0x190]
                if (inside(0x15f, 0x3c, item_right(item), 0xdc)) {
                    jo::self<jo::InventoryScreen>(this)->hover_item = jo::as_held_item(slot<Slot::hand0>(unit))->item;
                    jo::view<i32>(jo::self<jo::InventoryScreen>(this)->u32_29) = jo::view<i16>(jo::as_held_item(slot<Slot::hand0>(unit))->u16_4);
                    jo::self<jo::InventoryScreen>(this)->hover_source = 2;
                    jo::self<jo::InventoryScreen>(this)->hover_grid_key = 0;
                }
            }
        }
        if (inside(0x15f, 0x12a, 0x1af, 0x1cc) && jo::as_unit_data(holder(unit))->equipped != nullptr) {
            const u16 item = jo::as_held_item(slot<Slot::equipped>(unit))->item;  // [ebp-0x194]
            if (inside(0x15f, 0x12a, item_right(item), 0x1cc)) {
                jo::self<jo::InventoryScreen>(this)->hover_item = item;
                jo::view<i32>(jo::self<jo::InventoryScreen>(this)->u32_29) = jo::view<i16>(jo::as_held_item(slot<Slot::equipped>(unit))->u16_4);
                jo::self<jo::InventoryScreen>(this)->hover_source = 7;
            }
        }
        if (inside(0x1d4, 0x130, 0x1fc, 0x1d0) && jo::as_unit_data(holder(unit))->extra_item != nullptr) {
            jo::self<jo::InventoryScreen>(this)->hover_item = jo::as_held_item(slot<Slot::extra>(unit))->item;
            jo::view<i32>(jo::self<jo::InventoryScreen>(this)->u32_29) = jo::view<i16>(jo::as_held_item(slot<Slot::extra>(unit))->u16_4);
            jo::self<jo::InventoryScreen>(this)->hover_source = 8;
        }
        if (inside(0x21a, 0x3e, 0x30a, 0x17e)) {
            if (hover_grid(unit, &jo::UnitData::grid_16, 0x21a, 0x3e, 0xa) != 0) return 1;
        }
        if (inside(0x21a, 0x1a6, 0x30a, 0x246)) {
            if (hover_grid(unit, &jo::UnitData::grid_1a, 0x21a, 0x1a6, 0xb) != 0) return 1;
        }
        if (inside(0x15f, 0x220, 0x1fe, 0x247)) return hover_grid(unit, &jo::UnitData::list_12, 0x15f, 0x220, 9);
        return 0;
    }

    // ------------------------------------------------------------ 0x006d8ac0
    // Inventory::update(unit id): thiscall, ret 4, returns al (the only caller 0x006f7bd8 ignores
    // eax and sets al = 1 itself).
    u8 update(u32 id) {
        c_update.hit();
        void* const unit = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0 [ebp-0x8]
        if (global<i32>(d::g_mouse_x) != jo::self<jo::InventoryScreen>(this)->last_mouse_x || global<i32>(d::g_mouse_y) != jo::self<jo::InventoryScreen>(this)->last_mouse_y) {
            jo::view<u16>(jo::self<jo::InventoryScreen>(this)->last_mouse_x) = global<u16>(d::g_mouse_x);
            jo::view<u16>(jo::self<jo::InventoryScreen>(this)->last_mouse_y) = global<u16>(d::g_mouse_y);
            jo::self<jo::InventoryScreen>(this)->mouse_moved_ms = global<u32>(d::g_game_time_ms);
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_input(input())->button_down_state(0)) != 0 && jo::self<jo::InventoryScreen>(this)->open == 1)  // 0x00595aa0
            jo::self<jo::InventoryScreen>(this)->press_duration_ms = global<u32>(d::g_game_time_ms) - jo::self<jo::InventoryScreen>(this)->press_time_ms;
        else
            jo::self<jo::InventoryScreen>(this)->press_duration_ms = 0;
        if (jo::view<i32>(jo::self<jo::InventoryScreen>(this)->press_duration_ms) >= 0x1f4) jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 3;
        if (jo::self<jo::InventoryScreen>(this)->open == 1 && joan::orig::arg::as<i32>(joan::orig::as_input(input())->double_click(0)) != 0) {  // 0x00594d30
            jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 2;
        } else if (jo::self<jo::InventoryScreen>(this)->open == 2 && joan::orig::arg::as<i32>(joan::orig::as_input(input())->double_click(0)) != 0) {
            jo::view<u8>(jo::self<jo::InventoryScreen>(this)->open) = 4;
        }
        jo::self<jo::InventoryScreen>(this)->hover_item = 0;
        jo::self<jo::InventoryScreen>(this)->u32_29 = 0;
        jo::self<jo::InventoryScreen>(this)->hover_grid_key = 0;
        jo::self<jo::InventoryScreen>(this)->hover_source = 0;
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_game(unit)->byte_2e1())) != 0) return hover(unit);  // 0x00432b90
        if (jo::as_unit(unit)->speech_off != 0) return hover(unit);
        if (!in_range(unit)) return hover(unit);
        if (joan::orig::arg::as<i32>(joan::orig::as_parser(input())->value(1)) != 0) {  // 0x00594700
            if (w12() != 0) {
                const int r = drop_carried(unit);
                if (r >= 0) return static_cast<u8>(r);
            } else if (joan::orig::arg::as<i32>(joan::orig::as_parser(input())->rect_value(0x21a, 0x3e, 0x30a, 0x17e, 1)) != 0) {  // 0x005945d0
                const int r = use_grid_16(unit, id);
                if (r >= 0) return static_cast<u8>(r);
            } else if (joan::orig::arg::as<i32>(joan::orig::as_parser(input())->rect_value(0x21a, 0x1a6, 0x30a, 0x246, 1)) != 0) {
                const int r = use_grid_1a(unit, id);
                if (r >= 0) return static_cast<u8>(r);
            }
        }
        if (w12() != 0) return carried(unit, id);  // 0x006d9832
        return empty_hand(unit);                    // 0x006dbd7e
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b110_03", name, addr, fn)
B("Inventory::update_6d8ac0", 0x006d8ac0, &H::update);
#undef B

}  // namespace game_b110_03
}  // namespace

// ---------------------------------------- from modules/game_b111_20.cpp (game/b111_20)
//
//    - Inventory::draw_item 0x006ddf10 (thiscall(x, y, name), ret 0xc; thunk 0x004067e9):
//      "ITEM\" + name, image load 0x006099c0 on 0xa403a0 with flag 1 (saved / restored through
//      0x0060a720 / 0x0060a740), draw 0x006332b0 on 0xa46128 (15 args, colour 0x0042a260
//      (255,255,255,255)), release 0x0060a550; returns its eax. Skipped in b99 because
//      canvas_hooks.cpp names it: it is NOT a kScreens / kSizeReads entry; canvas_hooks only
//      retargets the call at 0x006d7766 to drag_item_stub, which jumps to the THUNK 0x004067e9,
//      so it reaches this replacement unchanged (enter/leave bracket kept). The x / y words are
//      pushed by the original as `mov cx/ax, word; push ecx/eax` with the upper halves holding
//      the colour dword loaded just before: reproduced exactly ((colour & 0xffff0000) | word).
//      The strrchr(name, '\\') result is dead (call kept). The 0x80-byte path buffer is followed
//      by the dead strrchr local, as in the original frame.
namespace {
namespace game_b111_20 {

JOAN_COUNTER(c_item, "Inventory::draw_item_6ddf10");

struct H {
    // ------------------------------------------------------------ 0x006ddf10
    std::uintptr_t draw_item(u32 x, u32 y, const char* name) {
        c_item.hit();
        struct {
            char buf[0x80];
            char* slash;  // [ebp-8], dead
        } fr;
        fr.slash = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(name), 0x5c));  // strrchr
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(fr.buf)), str(d::s_ITEM_9cf648));  // strcpy "ITEM\"
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(fr.buf)), joan::orig::arg::str(name));  // strcat
        void* const images = obj(d::g_texture_array);
        const u32 old = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(images)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(images)->set_mode(1u);                   // 0x0060a740
        void* const img = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(images)->load(joan::orig::arg::str(fr.buf), 1, 0u));  // 0x006099c0
        joan::orig::as_texture_manager(images)->set_mode(joan::orig::arg::u32(old));
        u32 tmp = 0;
        const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
        const u32 hi = colour & 0xffff0000u;
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->blit_image(joan::orig::arg::i16(hi | (x & 0xffffu)), joan::orig::arg::i16(hi | (y & 0xffffu)), 1u, joan::orig::arg::mptr(img), joan::orig::arg::u32(colour), joan::orig::arg::i16(0), joan::orig::arg::i16(0), joan::orig::arg::i16(0), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(0), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1));  // 0x006332b0
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_texture_manager(images)->release(joan::orig::arg::mptr(img)));  // 0x0060a550
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_20", name, addr, fn)
B("Inventory::draw_item_6ddf10", 0x006ddf10, &H::draw_item);
#undef B

}  // namespace game_b111_20
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
//
//    - Map::move_to_town_6de590 0x006de590 (thiscall unit, short town; ret 8): bool.
namespace {
namespace game_b108_09 {

JOAN_COUNTER(c_6de590, "Map::move_to_town_6de590");

struct H {
    // 0x006de590
    bool move_to_town(i32 unit_id, u32 town_raw) {
        c_6de590.hit();
        void* const units = obj(d::g_units);
        void* const towns = obj(d::g_item_res);
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(unit_id)));  // 0x0041b3c0
        const i16 town = static_cast<i16>(town_raw);
        u8* const t0 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(towns)->get(joan::orig::arg::i16(town)));  // 0x00425ce0
        i16 s14 = 0;                                           // always written by 0x006dea60
        const i16 s10 = joan::orig::arg::as<i16>(joan::orig::as_inventory(this)->sword_item(joan::orig::arg::u32(unit_id), joan::orig::arg::u32(town), &s14));  // 0x006dea60
        if (s14 == 0 || s10 == 0) return false;
        u8* const t18 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(towns)->get(joan::orig::arg::i16(s10)));
        bool done = false;
        auto table = [unit]() { return jo::view<u8*>(jo::as_unit(unit)->modifiers); };
        for (i32 i = 0; i < 1; ++i) {
            for (i32 j = 0; j < 2; ++j) {
                if (done) continue;
                jo::HeldItem* const e = jo::as_unit(unit)->modifiers->hands[i * 2 + j];  // [[unit+0x1c1] + i * 8 + 2 + j * 4]
                if (e == nullptr) continue;
                if (jo::view<i16>(e->item) != s14) continue;
                joan::orig::as_cell_object(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(unit_id)))->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
                joan::orig::as_slots(table())->free(joan::orig::arg::i32(i + 1), joan::orig::arg::i32(static_cast<i16>(j)));                      // 0x004971e0
                joan::orig::as_names(table())->set_4970c0(joan::orig::arg::u32(i + 1), joan::orig::arg::u32(static_cast<i16>(j)), joan::orig::arg::u32(s10), joan::orig::arg::u32(static_cast<i16>(jo::as_item_def(t18)->min_level)), 1u, 0u, 0u, 0u);  // 0x004970c0
                if (joan::orig::arg::as<i32>(joan::orig::as_world(unit)->flag_1fd()) == 0) {                                     // 0x0042d690
                    void* const mgr = obj(d::g_items);
                    const i32 id28 = joan::orig::arg::as<i32>(joan::orig::as_units(mgr)->create_49ea90(joan::orig::arg::u32(static_cast<i32>(s10))));      // 0x0049ea90
                    if (id28 != 0) {
                        virtual_call(joan::orig::as_units(mgr)->list.ptr_at(joan::orig::arg::i32(id28)), 0x18, 1);  // 0x00425d20
                        virtual_call(joan::orig::as_units(mgr)->list.ptr_at(joan::orig::arg::i32(id28)), 0x1c);
                        joan::orig::as_cell_object(unit)->attach_unit(joan::orig::arg::i16(static_cast<i16>(id28)), static_cast<i32>(jo::view<i16>(jo::as_item_def(t18)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));
                    }
                }
                done = true;
            }
        }
        jo::Grid* jo::UnitData::* const lists[2] = {&jo::UnitData::grid_16, &jo::UnitData::grid_1a};
        for (i32 n = 0; n < 2; ++n) {
            if (done) continue;
            jo::Grid* jo::UnitData::* const off = lists[n];
            auto list = [unit, off]() { return jo::as_unit(unit)->modifiers->*off; };
            if (list() == nullptr) continue;
            const i32 key = joan::orig::arg::as<i32>(joan::orig::as_inventory(list())->find(static_cast<i32>(s14)));  // 0x00458260
            if (key == 0) continue;
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(list())->get(joan::orig::arg::i32(key)));  // 0x00457e90
            const i32 b14 = jo::as_inv_item(rec)->u8_14, b15 = jo::as_inv_item(rec)->x, b16 = jo::as_inv_item(rec)->y;
            joan::orig::as_grid(list())->fill_from(joan::orig::arg::i32(key));  // 0x00457ec0
            joan::orig::as_inventory(list())->add_457ba0(joan::orig::arg::i32(s10), joan::orig::arg::i32(static_cast<i16>(jo::as_item_def(t18)->min_level)), 1, 0, 0, 0, joan::orig::arg::i32(static_cast<i8>(b14)), joan::orig::arg::i32(static_cast<i8>(b15)), joan::orig::arg::i32(static_cast<i8>(b16)));  // 0x00457ba0
            done = true;
        }
        if (done && joan::crt::stricmp(joan::orig::arg::str(jo::as_item_def(t18)->file_name), str(d::s_Saint_Catherine_de_Fierbois)) == 0) {  // 'Saint-Catherine-de-Fierbois'
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_txt_res(obj(d::g_strings))->message_text_4(jo::as_item_def(t0)->file_name));  // 0x004dd5e0
            joan::orig::as_message_list(obj(d::g_names))->post(static_cast<const char*>(r), nullptr, 0u);                                     // 0x004ad710
        }
        return done;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Map::move_to_town_6de590", 0x006de590, &H::move_to_town);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b111_07.cpp (fix/uninit)
//
//    - Obj::sword_item_6dea60 0x006dea60 (thiscall (unit, item, short* out), ret 0xc, returns
//      ax): Saint-Catherine-de-Fierbois sword lookup: the unit's two hand slots and its two
//      item lists are searched for an item named "Saint-Catherine-de-Fierbois" (_stricmp
//      0x007c07b0); found -> the letters A..E of the found sword's name suffix (from index 15)
//      plus the last letter of the item's own name are collected as flags and
//      "JOAN_SWORD_LV10" + the set letters is looked up (0x004a36c0). Old reason: the flags are
//      written at [ebp-0x71 + c] for EVERY suffix char c, so other chars write into the frame.
//      Now the frame is emulated byte by byte: every local the code reads after those writes
//      (the found record [ebp-0x10], the item record [ebp-0xc], the counter [ebp-0x34], the
//      flags [ebp-0x30..-0x2c]) lives in a 0x98-byte array laid out like the original frame, so
//      any char that lands inside the locals has the original's effect (e.g. 'a'..'h' corrupt
//      the record pointers, '='..'@' the counter); chars below the frame (c < -0x33) write
//      into free stack below esp that the following calls overwrite, and c in 0x79..0x7f hit
//      the dead args 1/2: both have no effect. Only c in -0x33..-0x28 (saved edi/esi/ebx) and
//      0x71..0x78 (saved ebp / return address) corrupt the CALLER's state in the original -
//      those writes are dropped here (not reproducible; item names are expected to be upper-case
//      identifiers, digits and '_', all of which land in dead locals or the flags).
//      Review: the saved edi/esi/ebx writes are harmless (the only caller 0x006de590 never reads
//      ebx/esi/edi between the call and its own pops), but 'q'..'x' (and the game data is not
//      available to rule them out) corrupt the caller's ebp / return address in the original.
//      Dropping that crash is a fix, not an exact replacement -> moved from game/ to fix/uninit
//      (stack-buffer-overflow class, registered there because it is the switchable fix module).
namespace {
namespace game_b111_07 {


using joan::orig::helpers::strlen_;  // strlen

JOAN_COUNTER(c_sword_item, "Obj::sword_item");

// ---------------------------------------------------------------------------------- 0x006dea60
constexpr img::Addr kItems = d::g_item_res;
u8* item_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(kItems))->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (word id)
i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

// Locals of 0x006dea60 from [ebp-0x98] to [ebp-1].
struct SwordFrame {
    u8 bytes[0x98];
    u8* found = nullptr;  // [ebp-0x10] (pointer locals are typed members: a frame dword cannot hold an x64 pointer)
    u8* rec = nullptr;    // [ebp-0xc]
    u8* at_ebp(i32 off) { return bytes + (0x98 + off); }  // off < 0
    u32& dw(i32 off) { return *reinterpret_cast<u32*>(at_ebp(off)); }
    // mov byte [ebp + c - 0x71], 1 for a sign-extended char c.
    void flag(i8 c) {
        const i32 off = static_cast<i32>(c) - 0x71;
        if (off >= -0x98 && off <= -1) *at_ebp(off) = 1;
        // off < -0xa4: free stack below the original esp (overwritten by the next calls);
        // off 8..0xe: args 1 / 2 (dead here). Both without effect. -0xa4..-0x99 (saved
        // edi / esi / ebx) and 0..7 (saved ebp / return address): not reproducible, dropped.
    }
};

struct H {
    // ------------------------------------------------------------------------------ 0x006dea60
    u16 sword_item(u32 unit_id, u32 item_id, i16* out) {
        c_sword_item.hit();
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(unit_id)));  // 0x0041b3c0
        u8* const rec_c = item_rec(item_id);
        u8* const own = item_rec(static_cast<u32>(jo::as_item_def(rec_c)->combine_item));
        if (stricmp_(jo::as_item_def(own)->file_name, str(kSaint)) != 0) {
            *out = jo::view<i16>(jo::as_item_def(rec_c)->combine_item);
            return jo::as_item_def(rec_c)->result_item;
        }
        u8* found = nullptr;  // [ebp-0x10]
        for (i32 i = 0; i < 1; ++i) {
            for (i32 j = 0; j < 2; ++j) {
                auto slot = [&]() {
                    return jo::as_unit(unit)->modifiers->hands[i * 2 + j];  // [[unit+0x1c1] + i * 8 + 2 + j * 4]
                };
                if (slot() == nullptr) continue;
                if (stricmp_(jo::as_item_def(item_rec(static_cast<u32>(slot()->item)))->file_name,
                             str(kSaint)) != 0)
                    continue;
                found = item_rec(static_cast<u32>(slot()->item));
            }
        }
        jo::Grid* jo::UnitData::* const kLists[2] = {&jo::UnitData::grid_16, &jo::UnitData::grid_1a};
        for (i32 n = 0; n < 2; ++n) {
            if (found != nullptr) break;
            auto list = [&]() { return jo::as_unit(unit)->modifiers->*kLists[n]; };
            if (list() == nullptr) continue;
            for (i32 k = joan::orig::as_dynarray(list())->size(); k > 0; --k) {  // 0x00417ad0
                if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(list())->is_empty_at(joan::orig::arg::i32(k))) != 0) continue;     // 0x00457f60
                const u16* const e = joan::orig::arg::as<const u16*>(joan::orig::as_ptr_list(list())->get(joan::orig::arg::i32(k)));  // 0x00457e90
                if (stricmp_(jo::as_item_def(item_rec(static_cast<u32>(*e)))->file_name, str(kSaint)) == 0) {
                    found = item_rec(static_cast<u32>(*e));
                    break;
                }
            }
        }
        if (found == nullptr) {
            *out = 0;
            return 0;
        }
        SwordFrame fr;
        std::memset(fr.bytes, 0, sizeof fr.bytes);
        fr.found = found;
        fr.rec = rec_c;
        for (i32 k = 0; k < 5; ++k) *fr.at_ebp(-0x30 + k) = 0;
        fr.dw(-0x34) = strlen_(str(kSword));
        for (;;) {
            const u32 len = strlen_(jo::as_item_def(fr.found)->name);
            if (fr.dw(-0x34) >= len) break;  // jae
            const char* const name = jo::as_item_def(fr.found)->name;
            fr.flag(*reinterpret_cast<const i8*>(name + fr.dw(-0x34)));
            fr.dw(-0x34) = fr.dw(-0x34) + 1u;
        }
        {
            const u32 len = strlen_(jo::as_item_def(fr.rec)->name);
            const char* const name = jo::as_item_def(fr.rec)->name;
            fr.flag(*reinterpret_cast<const i8*>(name + len - 1u));
        }
        char* const buf = reinterpret_cast<char*>(fr.at_ebp(-0x54));
        std::memset(static_cast<void*>(buf), i32{0}, u32{0x1e});  // memset
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(str(kSword)));  // strcpy
        for (fr.dw(-0x34) = 0; static_cast<i32>(fr.dw(-0x34)) < 5; fr.dw(-0x34) = fr.dw(-0x34) + 1u) {
            const i32 k = static_cast<i32>(fr.dw(-0x34));
            if (*reinterpret_cast<const i8*>(fr.at_ebp(-0x30 + k)) == 0) continue;
            const u8 letter = static_cast<u8>(k + 0x41);
            const u32 len = strlen_(buf);
            buf[len] = static_cast<char>(letter);  // at most 20 chars in the 30-byte buffer
        }
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(kItems))->find_4a36c0(joan::orig::arg::str(static_cast<void*>(buf))));  // 0x004a36c0
        *out = jo::view<i16>(jo::as_item_def(fr.found)->id);
        return static_cast<u16>(r);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

// fix (stack overflow, not an uninitialised read): 0x006dea60 writes flag bytes at [ebp + c - 0x71] for
// every suffix char c; chars 'q'..'x' hit the saved ebp / return address in the original (crash) -> those
// writes are dropped here, every other char is emulated on a copy of the original frame.
JOAN_REPLACE("fix/uninit", "Obj::sword_item_6dea60", 0x006dea60, &H::sword_item);

}  // namespace game_b111_07
}  // namespace

// ---------------------------------------- from modules/game_b116_13.cpp (game/b116_13)
//
//    - ItemScreen::setup_6b9870 0x006b9870 (ret 0, SEH frame: normal path only): TextDescs
//      "frame" / "frame_bg" / "title" / title caption / "l_arrow" / "r_arrow" / "bg1" / "bg2" /
//      "LONGBAR" (0x00629660 / 0x00629530), LONGBAR widget +0x697 (0x00621540 + caption
//      0x00623ce0), four text lines with 0x006299b0, "fancybar" + caption; new Model (0xd5,
//      0x005e9a00) at +0x1c, new Camera (0x4ca, 0x00613650) at +0x18 with set_viewport(0x200,
//      0xc0, 0x3df, 0x29f) (not in the 0x006c2120 range: plain thunk call), bounds box +-10000,
//      returns 0x005e9430's eax. size_read @0x006b9969, @0x006b9c4e, @0x006b9c68, @0x006b9d49,
//      @0x006b9dca.
//
//    - Shop::camera_setup_6c2120 0x006c2120 (ret 0, SEH frame: normal path only) -> this: two
//      buttons (o_* / c_*), two models (+0x5f / +0x148) and two cameras (+0x134 / +0x21d);
//      canvas::shop_viewport_args(args) before each of the two Camera::set_viewport thunk calls
//      (0x0040169f, originally at 0x006c2437 / 0x006c25f1). Its own screen-size reads are not
//      canvas-patched and read the real globals.
namespace {
namespace game_b116_13 {

// The two portrait cameras of the shop, as the setup placed them before UiScale: placed again when the scale changes
// while the game runs (Shop::camera_setup_6c2120 runs once, at load time).
struct ShopCameraPlace {
    jo::Camera* camera = nullptr;
    i32 x1 = 0, y1 = 0, x2 = 0, y2 = 0;
};
ShopCameraPlace g_shop_cameras[2];

void replace_shop_cameras() {
    for (int i = 0; i < shop_cameras::g_count; ++i) {
        const ShopCameraPlace& c = g_shop_cameras[i];
        i32 args[6] = {0, 0, c.x1, c.y1, c.x2, c.y2};
        joan::canvas::shop_viewport_args(args);
        joan::orig::as_camera(c.camera)->set_viewport(nullptr, nullptr, joan::orig::arg::u32(args[2]), joan::orig::arg::u32(args[3]),
                                                      joan::orig::arg::u32(args[4]), joan::orig::arg::u32(args[5]));
    }
}

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

constexpr u32 kOne = 0x3f800000u;   // 1.0f

using joan::orig::helpers::screen_w;
using joan::orig::helpers::screen_h;

using joan::orig::helpers::game_new;  // operator new

// Colour record 0x0042a260(r, g, b, a) on a 4-byte local; returns its dword.
u32 colour(i32 r, i32 g, i32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}

JOAN_COUNTER(c_shop, "Shop::camera_setup_6c2120");

struct H {
    // ------------------------------------------------------------------ 0x006c2120
    void shop_button(jo::Button*& button, i32 dx, img::Addr out, img::Addr down, img::Addr over) {
        void* const m = game_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
        button = m != nullptr ? joan::orig::arg::as<jo::Button*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;  // 0x00620ee0
        const i32 y = (screen_h() - 0x258) / 2 + 0x218;
        const i32 x = (screen_w() - 0x320) / 2 + dx;
        joan::orig::as_menu_button(button)->setup(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::mptr(jo::self<jo::BuyScreen>(this)->sheet), joan::orig::arg::str(str(out)), joan::orig::arg::str(str(down)), joan::orig::arg::str(str(over)), joan::orig::arg::str(str(out)), static_cast<std::uint8_t>(joan::orig::arg::u32(// 0x006211d0
                      1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));
        joan::orig::as_widget(button)->reset_child();  // 0x0062b660
    }
    void shop_model(jo::ShopStage& st, u32* box) {
        joan::orig::as_scene(&st)->reset();       // 0x005e9ac0
        joan::orig::as_scene(&st)->setup(joan::orig::arg::mptr(box));  // 0x005e9e50
        u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_scene(&st)->add_light());  // 0x005eb5d0
        joan::orig::as_camera(r)->init(0u);                             // 0x005c58a0
        jo::view<u32>(jo::as_light(r)->f32_30) = kOne;
        jo::view<u32>(jo::as_light(r)->colour) = colour(0xff, 0xff, 0xff, 0xff);
        joan::orig::as_light(r)->invalidate();  // 0x005c5c00
    }
    void shop_camera(jo::ShopStage& st, i32 y1_add, i32 y2_add) {
        {
            void* const m = game_new(static_cast<std::uint32_t>(sizeof(jo::Camera)));  // 0x4ca on x86
            jo::view<void*>(st.camera) = m != nullptr ? joan::orig::as_camera(m)->construct() : nullptr;  // 0x00613650
        }
        if (st.camera == nullptr) joan::orig::as_error_log(obj(d::g_err))->mem_error();  // 0x00586af0 (out of memory)
        joan::orig::as_sys_en(st.camera)->init_613730();                           // 0x00613730
        i32 args[6];
        args[5] = (screen_h() - 0x258) / 2 + y2_add;
        args[4] = (screen_w() - 0x320) / 2 + 0xd0;
        args[3] = (screen_h() - 0x258) / 2 + y1_add;
        args[2] = (screen_w() - 0x320) / 2 + 0x10;
        args[1] = static_cast<i32>(img::addr(d::g_a56ef0));
        args[0] = static_cast<i32>(img::addr(d::g_vga_back));
        if (shop_cameras::g_count < 2) g_shop_cameras[shop_cameras::g_count++] = {st.camera, args[2], args[3], args[4], args[5]};
        joan::display::set_ui_scale_listener(&replace_shop_cameras);
        joan::canvas::shop_viewport_args(args);
        joan::orig::as_camera(st.camera)->set_viewport(joan::orig::as_surface(obj(d::g_vga_back)), obj(d::g_a56ef0), joan::orig::arg::u32(args[2]), joan::orig::arg::u32(args[3]), joan::orig::arg::u32(args[4]), joan::orig::arg::u32(args[5]));  // 0x00613bf0
        joan::orig::as_camera(st.camera)->set_cf(0u, 1u, joan::orig::arg::u32(global<u32>(d::g_init_black)));  // 0x00613d30
        joan::orig::as_camera(st.camera)->set_ranges(joan::orig::arg::f32(0x41000000u), joan::orig::arg::f32(0x46801000u), joan::orig::arg::f32(0x46801000u));  // 0x00613ca0 (8.0f)
        joan::orig::as_camera(st.camera)->set_scene(joan::orig::as_scene(&st));  // 0x00613db0
    }
    H* shop_6c2120() {
        c_shop.hit();
        jo::self<jo::BuyScreen>(this)->sheet = img::obj(d::g_bmp_res_buy);  // the sprite sheet
        jo::self<jo::BuyScreen>(this)->tex = nullptr;
        shop_button(jo::self<jo::BuyScreen>(this)->panel_a, 0x4c, d::s_o_out, d::s_o_down, d::s_o_over);   // o_out / o_down / o_over
        shop_button(jo::self<jo::BuyScreen>(this)->panel_b, 0x67, d::s_c_out, d::s_c_down, d::s_c_over);  // c_out / c_down / c_over
        jo::self<jo::BuyScreen>(this)->items_loaded = 0;
        jo::self<jo::BuyScreen>(this)->prices = nullptr;
        jo::self<jo::BuyScreen>(this)->dragging = 0;
        // -10, -10, -1000, 50000, 50000, 3000
        u32 box[6] = {0xc1200000u, 0xc1200000u, 0xc47a0000u, 0x47435000u, 0x47435000u, 0x453b8000u};
        jo::self<jo::BuyScreen>(this)->stage_player.model = nullptr;
        shop_model(jo::self<jo::BuyScreen>(this)->stage_player, box);
        shop_camera(jo::self<jo::BuyScreen>(this)->stage_player, 0x10, 0xd0);
        jo::view<u32>(jo::self<jo::BuyScreen>(this)->stage_player.model_res) = 0;
        jo::self<jo::BuyScreen>(this)->stage_player.u32_e1 = 0;
        jo::self<jo::BuyScreen>(this)->stage_shop.model = nullptr;
        shop_model(jo::self<jo::BuyScreen>(this)->stage_shop, box);
        shop_camera(jo::self<jo::BuyScreen>(this)->stage_shop, 0xe8, 0x1a8);
        jo::view<u32>(jo::self<jo::BuyScreen>(this)->stage_shop.model_res) = 0;
        jo::self<jo::BuyScreen>(this)->stage_shop.u32_e1 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_13", name, addr, fn)
B("Shop::camera_setup_6c2120", 0x006c2120, &H::shop_6c2120);
#undef B

}  // namespace game_b116_13
}  // namespace

// ---------------------------------------- from modules/game_b96_08_2.cpp (game/b96_08)
namespace {
namespace game_b96_08_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::memset_;

JOAN_COUNTER(c_obj6c2070, "Obj6c2070::Obj");

struct H {
    u8* self() { return static_cast<u8*>(static_cast<void*>(this)); }

    // ------------------------------------------------------------------ 0x006c2070
    void* ctor_6c2070() {
        c_obj6c2070.hit();
        joan::orig::as_unit_data(&jo::as_buy_screen(self())->items_b)->construct();   // 0x00495950
        joan::orig::as_unit_data(&jo::as_buy_screen(self())->items)->construct();   // 0x00495950
        joan::orig::as_scene(&jo::as_buy_screen(self())->stage_player)->construct();   // 0x005e9a00
        joan::orig::as_scene(&jo::as_buy_screen(self())->stage_shop)->construct();  // 0x005e9a00
        memset_(this, 0, static_cast<u32>(sizeof(jo::BuyScreen)));  // 0x259 on x86
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6c2070::Obj", 0x006c2070, &H::ctor_6c2070);
#undef B96

}  // namespace game_b96_08_2
}  // namespace

// ---------------------------------------- from modules/game_b93_06_2.cpp (game/b93_06)
namespace {
namespace game_b93_06_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

// this = the input object 0x00a516b0 (Input): click i's release position (Input::click_0_ / click_1_).
struct H {
    jo::MouseClick& click(i32 i) { return jo::self<jo::Input>(this)->clicks[i]; }

    i32 get_67a(i32 i) { return click(i).release_x; }  // 0x006def10
    i32 get_67e(i32 i) { return click(i).release_y; }  // 0x006def40
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Table::get_67a", 0x006def10, &H::get_67a);
B93("Table::get_67e", 0x006def40, &H::get_67e);
#undef B93

}  // namespace game_b93_06_2
}  // namespace

// ---------------------------------------- from modules/game_b105_19_2.cpp (game/b105_19)
namespace {
namespace game_b105_19_2 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6def70", 0x006def70, &static_init<fn::set_pi_a6a554>);
B("init_6defd0", 0x006defd0, &static_init<fn::init_a6a558>);
B("init_6df020", 0x006df020, &static_init<fn::init_a6a55c>);
B("init_6df070", 0x006df070, &static_init<fn::init_a6a560>);
#undef B

}  // namespace game_b105_19_2
}  // namespace

// ---------------------------------------- from modules/game_b95_08_2.cpp (game/b95_08)
namespace {
namespace game_b95_08_2 {

using joan::call::cdecl_call;

using joan::hook::global;

using joan::hook::x87::Fpu;

// ---------------------------------------------------------------------------- 0x006def90 / 0x0070dd00
float pi_value() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(Fpu{true}.mul(a, global<double>(d::dbl_4)));  // * 4.0
}
static void JOAN_CDECL set_pi_a6a554() { global<float>(d::g_pi_a6a554) = pi_value(); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("set_pi_a6a554", 0x006def90, &set_pi_a6a554);
#undef B95

}  // namespace game_b95_08_2
}  // namespace

// ---------------------------------------- from modules/game_b97_08_2.cpp (game/b97_08)
namespace {
namespace game_b97_08_2 {

using joan::hook::global;

// 0x006deff0
void JOAN_CDECL init_a6a558() { global<float>(d::g_a6a558) = global<float>(d::g_pi_a6a554) / global<float>(d::flt_2); }

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("init_a6a558", 0x006deff0, &init_a6a558);
#undef B97

}  // namespace game_b97_08_2
}  // namespace

// ---------------------------------------- from modules/game_b99_10_2.cpp (game/b99_10)
namespace {
namespace game_b99_10_2 {

using joan::hook::global;

static void JOAN_CDECL init_a6a55c() { global<float>(d::g_a6a55c) = global<float>(d::g_pi_a6a554) / global<float>(d::flt_3); }  // 0x006df040

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("init_a6a55c", 0x006df040, &init_a6a55c);
#undef B

}  // namespace game_b99_10_2
}  // namespace

// ---------------------------------------- from modules/game_b96_08_3.cpp (game/b96_08)
namespace {
namespace game_b96_08_3 {

using joan::hook::global;

// 0x006df090
void JOAN_CDECL init_a6a560() { global<float>(d::g_a6a560) = global<float>(d::g_pi_a6a554) / global<float>(d::flt_4); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("init_a6a560", 0x006df090, &init_a6a560);
#undef B96

}  // namespace game_b96_08_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Shop::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_08_2::H::ctor_6c2070>(this)); }  // 0x6c2070
auto joan::orig::Shop::draw_item_icon(std::int32_t a0, std::int32_t a1, const char* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_06::H::draw_item_icon>(this, a0, a1, a2)); }  // 0x6c2ba0
auto joan::orig::Shop::free_slot_for(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_10::H::free_slot_for>(this, a0)); }  // 0x6c2ce0
auto joan::orig::Shop::setup_cameras() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_13::H::setup_cameras>(this)); }  // 0x6c2dd0
auto joan::orig::Shop::init() -> void { joan::bind::member<&::game_b108_12::H::init_6c3230>(this); }  // 0x6c3230
auto joan::orig::Shop::apply() -> void { joan::bind::member<&::game_b95_08::H::buy_apply>(this); }  // 0x6c3540
auto joan::orig::Shop::buy_6c5a00() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_17::H::buy>(this)); }  // 0x6c5a00
auto joan::orig::Shop::drop_item_6c5f10() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_16::H::drop_item>(this)); }  // 0x6c5f10
auto joan::orig::Shop::set_open(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::set_open>(this, a0)); }  // 0x6c8d00
auto joan::orig::Shop::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b94_08::H::byte_10>(this)); }  // 0x6c9040
auto joan::orig::Shop::load_sprites() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::buy_load>(this)); }  // 0x6c9060
auto joan::orig::Shop::free_sprites() -> void { joan::bind::member<&::game_b93_06::H::free1_b>(this); }  // 0x6c90e0
auto joan::orig::Shop::anim_time_left(std::int32_t a0) -> double { return joan::bind::conv<double>(joan::bind::member<&::game_b97_08::H::time_left>(this, a0)); }  // 0x6c9130
auto joan::orig::Shop::refresh_6c92e0(std::int32_t a0) -> void { joan::bind::member<&::game_b103_15::H::refresh_6c92e0>(this, a0); }  // 0x6c92e0
auto joan::orig::Shop::cell_at_cursor(std::int32_t a0, std::int32_t a1, void* a2, void* a3) -> void { joan::bind::member<&::game_b92_06::H::place_at_cursor>(this, a0, a1, a2, a3); }  // 0x6c99e0
auto joan::orig::Shop::item_under(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_06::H::item_under>(this, a0, a1, a2, a3, a4)); }  // 0x6c9c40
auto joan::orig::Shop::find_free(std::int32_t a0, std::int32_t a1, void* a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_06::H::find_free>(this, a0, a1, a2, a3)); }  // 0x6c9d20
auto joan::orig::Shop::has_enough(std::int32_t a0, std::int16_t a1, std::int16_t a2, std::int16_t a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b92_06::H::has_enough>(this, a0, a1, a2, a3)); }  // 0x6c9f10
auto joan::orig::WidgetChild::enable() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::flag3_set>(this)); }  // 0x6ca000
auto joan::orig::WidgetChild::disable() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::flag3_clear>(this)); }  // 0x6ca020
auto joan::orig::Cutscene::reset() -> void { joan::bind::member<&::game_b84_04::H::reset_6ca090>(this); }  // 0x6ca090
auto joan::orig::Cutscene::release() -> void { joan::bind::member<&::game_b84_04::H::release_6ca0d0>(this); }  // 0x6ca0d0
auto joan::orig::Cutscene::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::reset_6ca170>(this)); }  // 0x6ca170
auto joan::orig::Cutscene::draw_bars() -> void { joan::bind::member<&::game_ui7::H::draw_bars>(this); }  // 0x6ca230
auto joan::orig::Cutscene::begin(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::set_6cab90>(this, a0, a1)); }  // 0x6cab90
auto joan::orig::Cutscene::load_camera() -> void { joan::bind::member<&::game_b85_05::H::cutscene_load>(this); }  // 0x6cabd0
auto joan::orig::Cutscene::start(std::uint32_t a0, std::uint32_t a1, std::int8_t a2, std::int8_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::uint32_t a8, std::uint32_t a9, std::int8_t a10) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b106_06::H::cut_start>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x6cad50
auto joan::orig::Cutscene::start_6cb9e0(std::uint32_t a0, std::uint32_t a1, std::int8_t a2, std::int8_t a3, std::int32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::int8_t a8) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b106_08::H::cut_start>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x6cb9e0
auto joan::orig::Cutscene::start_6cc6a0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_15::H::cut_start>(this, a0, a1, a2, a3)); }  // 0x6cc6a0
auto joan::orig::Cutscene::move_camera(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b106_09::H::cut_scene_start>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x6ccf00
auto joan::orig::Cutscene::focus_unit(char a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b106_11::H::cut_start_camera>(this, a0, a1, a2, a3, a4)); }  // 0x6cd720
auto joan::orig::Cutscene::lerp(float a0, float a1, float a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b90_06::H::lerp>(this, a0, a1, a2)); }  // 0x6cdf70
auto joan::orig::Shop::leave() -> void { joan::bind::member<&::game_b94_08::H::shop_leave>(this); }  // 0x6cdfa0
auto joan::orig::Ending::release() -> void { joan::bind::member<&::game_b93_06::H::reset_6ce390>(this); }  // 0x6ce390
auto joan::orig::Ending::draw() -> void { joan::bind::member<&::game_ui3::H::draw_ending>(this); }  // 0x6ce450
auto joan::orig::Ending::open(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_15::H::end_open>(this, a0)); }  // 0x6d19e0
auto joan::orig::Ending::reset() -> void { joan::bind::member<&::game_b82_12::H::end_reset>(this); }  // 0x6d1d30
auto joan::orig::Ending::is_open() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b99_10::H::byte_234>(this)); }  // 0x6d1ef0
auto joan::orig::Ending::load_pictures() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_12::H::end_load>(this)); }  // 0x6d1f20
auto joan::orig::Ending::free_pictures() -> void { joan::bind::member<&::game_b82_12::H::end_free>(this); }  // 0x6d2280
auto joan::orig::TalkPrompt::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_08::H::clear31>(this)); }  // 0x6d2300
auto joan::orig::TalkPrompt::release() -> void { joan::bind::member<&::game_b93_06::H::reset_6d2380>(this); }  // 0x6d2380
auto joan::orig::TalkPrompt::show() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_15::H::set_flag_8>(this)); }  // 0x6d3450
auto joan::orig::TalkPrompt::hide() -> void { joan::bind::member<&::game_b84_04::H::reset_6d3480>(this); }  // 0x6d3480
auto joan::orig::TalkPrompt::load_sprite() -> void { joan::bind::member<&::game_b97_08::H::load_gmsg>(this); }  // 0x6d34d0
auto joan::orig::TalkPrompt::free_sprite() -> void { joan::bind::member<&::game_b85_05::H::release_handle>(this); }  // 0x6d3540
auto joan::orig::Player::flag_ac() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b99_10::H::byte_ac>(this)); }  // 0x6d37c0
auto joan::orig::Popup::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_08::H::dtor_6d38a0>(this)); }  // 0x6d38a0
auto joan::orig::Popup::release_6d3b80() -> void { joan::bind::member<&::game_b93_06::H::reset_6d3b80>(this); }  // 0x6d3b80
auto joan::orig::Popup::load_gpopup() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::load_gpopup>(this)); }  // 0x6d3be0
auto joan::orig::Popup::free_gpopup() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::free_gpopup>(this)); }  // 0x6d3c60
auto joan::orig::Panel::set_pos(std::int16_t a0, std::int16_t a1) -> void { joan::bind::member<&::game_b83_05::H::set_pos>(this, a0, a1); }  // 0x6d50e0
auto joan::orig::Popup::poll_buttons() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_07::H::start_any_6d5120>(this)); }  // 0x6d5120
auto joan::orig::Popup::set_text_6d5230(const char* a0, Font2d* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::set_text>(this, a0, a1)); }  // 0x6d5230
auto joan::orig::Popup::set_title(const char* a0, Font2d* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::set_title>(this, a0, a1)); }  // 0x6d52c0
auto joan::orig::Popup::set_buttons(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06::H::set_first>(this, a0)); }  // 0x6d5340
auto joan::orig::Popup::result() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06::H::get_444>(this)); }  // 0x6d5370
auto joan::orig::VideoPlayer::shutdown() -> void { joan::bind::member<&::game_b93_06::H::wrap_6d5ab0>(this); }  // 0x6d5ab0
auto joan::orig::VideoPlayer::draw(std::uint32_t a0) -> void { joan::bind::member<&::game_b83_05::H::video_draw>(this, a0); }  // 0x6d5ae0
auto joan::orig::VideoPlayer::skip() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_08::H::start>(this)); }  // 0x6d6130
auto joan::orig::VideoPlayer::start() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_08::H::reset_6d61d0>(this)); }  // 0x6d61d0
auto joan::orig::VideoPlayer::stop() -> void { joan::bind::member<&::game_b84_04::H::reset_6d6230>(this); }  // 0x6d6230
auto joan::orig::Talk::load_image() -> void { joan::bind::member<&::game_b94_08::H::load_trevor>(this); }  // 0x6d62a0
auto joan::orig::VideoPlayer::release_image() -> void { joan::bind::member<&::game_b85_05::H::release_and_reset>(this); }  // 0x6d6310
auto joan::orig::Inventory::release() -> void { joan::bind::member<&::game_b94_08::H::reset_6d6650>(this); }  // 0x6d6650
auto joan::orig::Inventory::load_sprite() -> void { joan::bind::member<&::game_b87_06::H::inventory_load_sprite>(this); }  // 0x6d6680
auto joan::orig::Inventory::free_sprite() -> void { joan::bind::member<&::game_b93_06::H::free1>(this); }  // 0x6d66f0
auto joan::orig::Inventory::update_6d8ac0(std::uint32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b110_03::H::update>(this, a0)); }  // 0x6d8ac0
auto joan::orig::Unit::item_under(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int8_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_06::H::unit_item_under>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x6de060
auto joan::orig::Inventory::is_closed() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_08::H::not_byte8>(this)); }  // 0x6de1d0
auto joan::orig::Grid::cell_at_mouse(std::int32_t a0, std::int32_t a1, void* a2, void* a3, std::uint32_t a4) -> void { joan::bind::member<&::game_b94_08::H::cell_at_mouse>(this, a0, a1, a2, a3, a4); }  // 0x6de200
auto joan::orig::Inventory::item_sound(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_15::H::item_sound>(this, a0)); }  // 0x6de4d0
auto joan::orig::Inventory::move_to_town(std::int32_t a0, std::uint32_t a1) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b108_09::H::move_to_town>(this, a0, a1)); }  // 0x6de590
auto joan::orig::Inventory::sword_item(std::uint32_t a0, std::uint32_t a1, void* a2) -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b111_07::H::sword_item>(this, a0, a1, a2)); }  // 0x6dea60
auto joan::orig::Table::get_67a(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_06_2::H::get_67a>(this, a0)); }  // 0x6def10
auto joan::orig::Table::get_67e(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_06_2::H::get_67e>(this, a0)); }  // 0x6def40
