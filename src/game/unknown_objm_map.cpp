// game/unknown_objm_map.cpp: functions whose original source file is unknown (formerly game/_unplaced/objm_map.cpp;
// re/attribution.csv: between ww/interfac/objmenub.cpp and ww/interfac/objmenuw.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::Fpu;


using joan::orig::helpers::fd;
u32 f_bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

constexpr Fpu kFpu{true};
constexpr fn::Code kStrcpy = fn::crt_strcpy;   // CRT strcpy
constexpr fn::Code kStricmp = fn::crt_stricmp;  // CRT _stricmp

using joan::orig::helpers::stricmp_;  // _stricmp

}  // namespace

// ---------------------------------------- from modules/game_b116_04.cpp (game/b116_04)
namespace {
namespace game_b116_04 {

constexpr fn::Code kNew = fn::op_new;      // operator new (cdecl)

using joan::hook::x87::ftol;

using joan::orig::helpers::fild;
using joan::orig::helpers::wrap_add;
using joan::orig::helpers::wrap_sub;
i32 size_read(std::uintptr_t instruction) { return joan::canvas::size_read(instruction); }

void* const kTex = obj(d::g_texture_array);
void* const kCanvas = obj(d::g_sprites_a46128);
void* const kUnits = obj(d::g_units);
void* const kHeroIcons = obj(d::g_heroIcons);

constexpr img::Addr kHalf = d::flt_0_5;  // 0.5f
constexpr img::Addr kOne = d::flt_1;   // 1.0f
constexpr img::Addr kZero = d::k_9cf5c4;  // 0.0f
constexpr img::Addr kPi = d::flt_3_1415927;    // 3.14159274f
constexpr img::Addr k90 = d::flt_90;    // 90.0f
constexpr img::Addr kDeg = d::flt_0_017453292;   // 0.0174532924f
constexpr img::Addr k3Pi2 = d::flt_4_712389;  // 4.71238899f

// 0x0042a260 colour ctor (thunk 0x004098b8, ret 0x10) on a stack dword, then its value.
u32 white() {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xffu, 0xffu, 0xffu, 0xffu));
}
// 0x00633500 sprite queue (thunk 0x00401bbd, 17 args, ret 0x44) on 0xa46128.
void sprite(u32 x, u32 y, const char* name, void* sheet, u32 colour, u32 a8, u32 a9, u32 a10, u32 a11, u32 a12, u32 a13,
            u32 a14, u32 a15, u32 a16, u32 a17) {
    joan::orig::as_sprite_queue(kCanvas)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 7u, joan::orig::arg::str(name), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(colour), 3, joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));
}

JOAN_COUNTER(c_camera, "ObjectiveMap::create_camera_72d380");
JOAN_COUNTER(c_draw, "ObjectiveMap::draw_72e480");

struct H {
    // ------------------------------------------------------------------ 0x0072d380
    u32 create_camera() {
        c_camera.hit();
        void* const m1 = cdecl_call<void*>(kNew, static_cast<u32>(sizeof(jo::Scene)));  // 0xd5 on x86
        jo::self<jo::CampScreen>(this)->map_scene = m1 != nullptr ? joan::orig::arg::as<jo::Scene*>(joan::orig::as_scene(m1)->construct()) : nullptr;  // 0x005e9a00
        joan::orig::as_scene(jo::self<jo::CampScreen>(this)->map_scene)->reset();                       // 0x005e9ac0
        void* const m2 = cdecl_call<void*>(kNew, static_cast<u32>(sizeof(jo::Camera)));  // 0x4ca on x86
        jo::self<jo::CampScreen>(this)->map_camera = m2 != nullptr ? joan::orig::arg::as<jo::Camera*>(joan::orig::as_camera(m2)->construct()) : nullptr;  // 0x00613650
        joan::orig::as_sys_en(jo::self<jo::CampScreen>(this)->map_camera)->init_613730();                       // 0x00613730
        const i32 h = wrap_sub(size_read(0x0072d451), 1);
        const i32 w = wrap_sub(size_read(0x0072d45b), 1);
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->map_camera)->set_viewport(joan::orig::as_surface(img::obj(d::g_vga_back)), img::obj(d::g_a56ef0), 0u, 0u, joan::orig::arg::u32(static_cast<u32>(w)), joan::orig::arg::u32(static_cast<u32>(h)));  // 0x00613bf0
        const u32 g = global<u32>(d::g_init_black);
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->map_camera)->set_cf(0u, 1u, joan::orig::arg::u32(g));                                // 0x00613d30
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->map_camera)->set_ranges(joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x46000400u), joan::orig::arg::f32(0x46000400u));  // 0x00613ca0
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->map_camera)->set_scene(jo::self<jo::CampScreen>(this)->map_scene);             // 0x00613db0
        jo::self<jo::CampScreen>(this)->camera_view = 1;
        return 0;  // 0x00613db0 returns nothing; the caller (objmenu.cpp) ignores the result
    }

    // ------------------------------------------------------------------ 0x0072e480 (fix/uninit)
    void draw(u32 /*unused*/) {
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::CampScreen>(this)->visible)) != 5) return;
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::CampScreen>(this)->camera_view)) != 0) return;
        c_draw.hit();
        const u32 saved_state = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kTex)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kTex)->set_mode(1u);                      // 0x0060a740
        const i32 rot = static_cast<i32>(global<i16>(d::g_rotation));
        const i32 sel = rot & 3;  // fix/uninit (a): layout of rotations outside 0..3

        float scale;
        if (jo::self<jo::CampScreen>(this)->zoom_step == 1) {
            scale = 0.25f;
        } else {
            scale = static_cast<float>(kFpu.mul(fild(wrap_sub(jo::self<jo::CampScreen>(this)->zoom_step, 1)), fd(kHalf)));
        }
        u8 fit = 1;
        u8* const info = static_cast<u8*>(global<void*>(d::g_a4f385));
        u8* const sheet = reinterpret_cast<u8*>(&jo::as_world(global<void*>(d::g_world))->bitmaps);
        {
            const double a = kFpu.div(fild(jo::as_mini_res(info)->i32_29), scale);
            const double b = kFpu.mul(fild(wrap_add(jo::as_mini_res(info)->i32_29, jo::as_mini_res(info)->i32_21)), fd(kHalf));
            if (!(b >= a)) {  // fcompp: b < a or unordered
                fit = 0;
                scale = static_cast<float>(
                    kFpu.div(kFpu.mul(fild(jo::as_mini_res(info)->i32_21), scale), fild(jo::as_mini_res(info)->i32_29)));
            }
        }
        const char* const tile = joan::orig::arg::as<const char*>(joan::orig::as_mini_res(info)->find_frame(joan::orig::arg::i16(static_cast<u32>(fit)), joan::orig::arg::i16(0u), joan::orig::arg::i16(0u)));  // 0x0071b3b0
        void* const tile_tex = joan::orig::as_texture_manager(kTex)->load(joan::orig::arg::str(// 0x006099c0
                                           jo::as_bmp_entry(joan::orig::as_bmp_res(sheet)->find(joan::orig::arg::str(tile)))->file), 1, 0u);  // 0x00620b40
        const i32 rows = fit != 0 ? static_cast<i32>(jo::as_mini_res(info)->rect[2]) : static_cast<i32>(jo::as_mini_res(info)->rect[0]);       // [ebp-0x30]
        const i32 cols = fit != 0 ? static_cast<i32>(jo::as_mini_res(info)->rect[3]) : static_cast<i32>(jo::as_mini_res(info)->rect[1]);       // [ebp-0x34]
        const i32 size_x = (fit != 0 ? jo::as_mini_res(info)->i32_29 : jo::as_mini_res(info)->i32_21);          // [ebp-0x38]
        const i32 size_y = (fit != 0 ? jo::as_mini_res(info)->i32_2d : jo::as_mini_res(info)->i32_25);          // [ebp-0x3c]
        const float tw = static_cast<float>(kFpu.mul(fild((fit != 0 ? jo::self<jo::CampScreen>(this)->u32_201 : jo::self<jo::CampScreen>(this)->u32_1f9)), scale));  // [ebp-0x40]
        const float th = static_cast<float>(kFpu.mul(fild((fit != 0 ? jo::self<jo::CampScreen>(this)->u32_205 : jo::self<jo::CampScreen>(this)->u32_1fd)), scale));  // [ebp-0x44]
        float cx = static_cast<float>(size_read(0x0072e6ec) / 2);  // [ebp-0x18]
        float cy = static_cast<float>(size_read(0x0072e705) / 2);  // [ebp-0x1c]
        const double one = fd(kOne);

        // Map rectangle [ebp-0x48] left, [ebp-0x4c] top, [ebp-0x50] right, [ebp-0x54] bottom.
        i32 left = 0;
        i32 top = 0;
        i32 right = 0;
        i32 bottom = 0;
        {
            const i32 d1 = wrap_sub(jo::view<i32>(jo::self<jo::CampScreen>(this)->view_x), jo::view<i32>(jo::self<jo::CampScreen>(this)->sector_min_x));
            const i32 d2 = wrap_sub(jo::view<i32>(jo::self<jo::CampScreen>(this)->view_y), jo::view<i32>(jo::self<jo::CampScreen>(this)->sector_min_y));
            switch (sel) {
            case 0:
                cx = static_cast<float>(kFpu.sub(cx, kFpu.div(kFpu.mul(fild(d1), tw), fild(size_x))));
                cy = static_cast<float>(kFpu.add(kFpu.div(kFpu.mul(fild(d2), th), fild(size_y)), cy));
                left = ftol(cx);
                right = ftol(kFpu.sub(kFpu.add(kFpu.mul(fild(cols), tw), cx), one));
                top = ftol(kFpu.add(kFpu.sub(cy, kFpu.mul(fild(rows), th)), one));
                bottom = ftol(cy);
                break;
            case 1:
                cx = static_cast<float>(kFpu.add(kFpu.div(kFpu.mul(fild(d2), th), fild(size_y)), cx));
                cy = static_cast<float>(kFpu.add(kFpu.div(kFpu.mul(fild(d1), tw), fild(size_x)), cy));
                left = ftol(kFpu.add(kFpu.sub(cx, kFpu.mul(fild(rows), th)), one));
                right = ftol(cx);
                top = ftol(kFpu.add(kFpu.sub(cy, kFpu.mul(fild(cols), tw)), one));
                bottom = ftol(cy);
                break;
            case 2:
                cx = static_cast<float>(kFpu.add(kFpu.div(kFpu.mul(fild(d1), tw), fild(size_x)), cx));
                cy = static_cast<float>(kFpu.sub(cy, kFpu.div(kFpu.mul(fild(d2), th), fild(size_y))));
                left = ftol(kFpu.add(kFpu.sub(cx, kFpu.mul(fild(cols), tw)), one));
                right = ftol(cx);
                top = ftol(cy);
                bottom = ftol(kFpu.sub(kFpu.add(kFpu.mul(fild(rows), th), cy), one));
                break;
            default:
                // the original multiplies d2 by [ebp-0x40] / [ebp-0x3c] and d1 by [ebp-0x44] / [ebp-0x38] here
                cx = static_cast<float>(kFpu.sub(cx, kFpu.div(kFpu.mul(fild(d2), tw), fild(size_y))));
                cy = static_cast<float>(kFpu.sub(cy, kFpu.div(kFpu.mul(fild(d1), th), fild(size_x))));
                left = ftol(cx);
                right = ftol(kFpu.sub(kFpu.add(kFpu.mul(fild(rows), th), cx), one));
                top = ftol(cy);
                bottom = ftol(kFpu.sub(kFpu.add(kFpu.mul(fild(cols), tw), cy), one));
                break;
            }
        }

        // The four bars around the map (clip box 1, 1, size - 2).
        const u32 r = static_cast<u32>(rot);
        const u32 clip_w = static_cast<u32>(wrap_sub((fit != 0 ? jo::self<jo::CampScreen>(this)->u32_201 : jo::self<jo::CampScreen>(this)->u32_1f9), 2));  // [ebp-0x60]
        const u32 clip_h = static_cast<u32>(wrap_sub((fit != 0 ? jo::self<jo::CampScreen>(this)->u32_205 : jo::self<jo::CampScreen>(this)->u32_1fd), 2));  // [ebp-0x64]
        if (top > 0) {
            const i32 w1 = wrap_sub(size_read(0x0072ea3d), 1);
            const u32 c = white();
            sprite(0u, 0u, tile, sheet, c, 0u, 0u, 0u, 1u, 1u, clip_w, clip_h, r, static_cast<u32>(w1),
                   static_cast<u32>(wrap_sub(top, 1)));
        }
        if (wrap_add(bottom, 1) < wrap_sub(size_read(0x0072eab4), 1)) {
            const i32 h1 = wrap_sub(size_read(0x0072eac1), 1);
            const i32 w1 = wrap_sub(size_read(0x0072eacb), 1);
            const u32 c = white();
            sprite(0u, static_cast<u32>(wrap_add(bottom, 1)), tile, sheet, c, 0u, 0u, 0u, 1u, 1u, clip_w, clip_h, r,
                   static_cast<u32>(w1), static_cast<u32>(h1));
        }
        if (left > 0) {
            const u32 c = white();
            sprite(0u, static_cast<u32>(top), tile, sheet, c, 0u, 0u, 0u, 1u, 1u, clip_w, clip_h, r,
                   static_cast<u32>(wrap_sub(left, 1)), static_cast<u32>(bottom));
        }
        if (wrap_add(right, 1) < wrap_sub(size_read(0x0072ebc2), 1)) {
            const i32 w1 = wrap_sub(size_read(0x0072ebd3), 1);
            const u32 c = white();
            sprite(static_cast<u32>(wrap_add(right, 1)), static_cast<u32>(top), tile, sheet, c, 0u, 0u, 0u, 1u, 1u,
                   clip_w, clip_h, r, static_cast<u32>(w1), static_cast<u32>(bottom));
        }

        // Tile grid: rows [ebp-0x78] = rows .. 1, columns [ebp-0x8c] = 1 .. cols.
        {
            float px = cx;  // [ebp-0x70]
            float py = cy;  // [ebp-0x74]
            i32 x1 = 0;     // [ebp-0x7c]
            i32 y1 = 0;     // [ebp-0x80]
            i32 x2 = 0;     // [ebp-0x84]
            i32 y2 = 0;     // [ebp-0x88]
            for (i32 row = rows; row >= 1; --row) {
                switch (sel) {
                case 0:
                    y1 = ftol(kFpu.add(kFpu.sub(py, th), one));
                    y2 = ftol(py);
                    break;
                case 1:
                    x1 = ftol(kFpu.add(kFpu.sub(px, th), one));
                    x2 = ftol(px);
                    break;
                case 2:
                    y1 = ftol(py);
                    y2 = ftol(kFpu.sub(kFpu.add(py, th), one));
                    break;
                default:
                    x1 = ftol(px);
                    x2 = ftol(kFpu.sub(kFpu.add(px, th), one));
                    break;
                }
                for (i32 col = 1; col <= cols; ++col) {
                    switch (sel) {
                    case 0:
                        x1 = ftol(px);
                        x2 = ftol(kFpu.sub(kFpu.add(px, tw), one));
                        break;
                    case 1:
                        y1 = ftol(kFpu.add(kFpu.sub(py, tw), one));
                        y2 = ftol(py);
                        break;
                    case 2:
                        x1 = ftol(kFpu.add(kFpu.sub(px, tw), one));
                        x2 = ftol(px);
                        break;
                    default:
                        y1 = ftol(py);
                        y2 = ftol(kFpu.sub(kFpu.add(py, tw), one));
                        break;
                    }
                    if (x1 < size_read(0x0072ee26) && y1 < size_read(0x0072ee35) && x2 >= 0 && y2 >= 0) {
                        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_mini_res(info)->find_frame(joan::orig::arg::i16(static_cast<u32>(fit)), joan::orig::arg::i16(static_cast<u32>(row)), joan::orig::arg::i16(static_cast<u32>(col))));  // 0x0071b3b0
                        void* const tex = joan::orig::as_texture_manager(kTex)->load(joan::orig::arg::str(jo::as_bmp_entry(joan::orig::as_bmp_res(sheet)->find(joan::orig::arg::str(name)))->file), 1, 0u);
                        const u32 c = white();
                        sprite(static_cast<u32>(x1), static_cast<u32>(y1), name, sheet, c, 0u, 0u, 0u, 0xffffffffu,
                               0xffffffffu, 0xffffffffu, 0xffffffffu, r, static_cast<u32>(x2), static_cast<u32>(y2));
                        joan::orig::as_texture_manager(kTex)->release(joan::orig::arg::mptr(tex));  // 0x0060a550
                    }
                    switch (sel) {
                    case 0: px = px + tw; break;
                    case 1: py = py - tw; break;
                    case 2: px = px - tw; break;
                    default: py = py + tw; break;
                    }
                }
                switch (sel) {
                case 0:
                    px = cx;
                    py = py - th;
                    break;
                case 1:
                    px = px - th;
                    py = cy;
                    break;
                case 2:
                    px = cx;
                    py = py + th;
                    break;
                default:
                    px = px + th;
                    py = cy;
                    break;
                }
            }
        }
        joan::orig::as_texture_manager(kTex)->release(joan::orig::arg::mptr(tile_tex));     // 0x0060a550
        joan::orig::as_texture_manager(kTex)->set_mode(joan::orig::arg::u32(saved_state));  // 0x0060a740

        // Hero markers.
        {
            u8* const me = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
            const i32 side = static_cast<i32>(jo::as_unit(me)->side);
            char icon[12];          // [ebp-0xc4]
            bool icon_set = false;  // fix/uninit (b)
            float pos[3];           // [ebp-0xb8]
            for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
                u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
                if (u == nullptr || static_cast<i32>(jo::as_unit(u)->side) != side || jo::as_unit(u)->rank < 4 || u == me) continue;
                u8* const desc = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
                joan::orig::as_unit(u)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
                static const img::Addr kHeroes[8][3] = {
                    {d::s_JOAN_OF_ARC, d::s_joan_H, d::s_joan_M},  // JOAN_OF_ARC          !joan_H / !joan_M
                    {d::s_Jean_de_Metz_9df40c, d::s_metz_H, d::s_metz_M},  // Jean_de_Metz         !metz_
                    {d::s_Alencon, d::s_alen_H, d::s_alen_M},  // Alencon              !alen_
                    {d::s_LaHire, d::s_lahi_H, d::s_lahi_M},  // LaHire               !lahi_
                    {d::s_ORLEAN, d::s_bast_H, d::s_bast_M},  // ORLEAN               !bast_
                    {d::s_FLAVY, d::s_flav_H, d::s_flav_M},  // FLAVY                !flav_
                    {d::s_CHARLIE7, d::s_char_H, d::s_char_M},  // CHARLIE7             !char_
                    {d::s_arthur_de_richemont, d::s_rich_H, d::s_rich_M},  // arthur_de_richemont  !rich_
                };
                for (int k = 0; k < 8; ++k) {
                    if (cdecl_call<int>(kStricmp, jo::as_unit_type(desc)->name, str(kHeroes[k][0])) != 0) continue;
                    const bool h = jo::view<jo::TaskSlot>(jo::as_unit(u)->task).kind == 0 && jo::view<jo::TaskSlot>(jo::as_unit(u)->task).target != 0;
                    cdecl_call<char*>(kStrcpy, icon, str(h ? kHeroes[k][1] : kHeroes[k][2]));
                    icon_set = true;
                    break;
                }
                const i32 ux = ftol(kFpu.mul(kFpu.div(pos[0], fild(size_x)), tw));  // [ebp-0xc8]
                const i32 uy = ftol(kFpu.mul(kFpu.div(pos[1], fild(size_y)), th));  // [ebp-0xcc]
                if (!icon_set) continue;  // fix/uninit (b): no known name yet
                const i32 iw = static_cast<i32>(static_cast<i16>(jo::as_bmp_entry(joan::orig::as_bmp_res(kHeroIcons)->find(joan::orig::arg::str(icon)))->width));  // 0x00620b40
                const i32 ih = static_cast<i32>(static_cast<i16>(jo::as_bmp_entry(joan::orig::as_bmp_res(kHeroIcons)->find(joan::orig::arg::str(icon)))->height));
                i32 ix = 0;  // [ebp-0xd8]
                i32 iy = 0;  // [ebp-0xdc]
                switch (sel) {
                case 0:
                    ix = ftol(kFpu.sub(kFpu.add(fild(ux), cx), fild(iw / 2)));
                    iy = ftol(kFpu.sub(kFpu.sub(cy, fild(uy)), fild(ih / 2)));
                    break;
                case 1:
                    ix = ftol(kFpu.sub(kFpu.sub(cx, fild(uy)), fild(iw / 2)));
                    iy = ftol(kFpu.sub(kFpu.sub(cy, fild(ux)), fild(ih / 2)));
                    break;
                case 2:
                    ix = ftol(kFpu.sub(kFpu.sub(cx, fild(ux)), fild(iw / 2)));
                    iy = ftol(kFpu.sub(kFpu.add(fild(uy), cy), fild(ih / 2)));
                    break;
                default:
                    ix = ftol(kFpu.sub(kFpu.add(fild(uy), cx), fild(iw / 2)));
                    iy = ftol(kFpu.sub(kFpu.add(fild(ux), cy), fild(ih / 2)));
                    break;
                }
                if (ix >= size_read(0x0072f5e7)) continue;
                if (iy >= size_read(0x0072f5f5)) continue;
                if (wrap_add(ix, iw) < 0) continue;
                if (wrap_add(iy, ih) < 0) continue;
                void* const tex = joan::orig::as_texture_manager(kTex)->load(joan::orig::arg::str(jo::as_bmp_entry(joan::orig::as_bmp_res(kHeroIcons)->find(joan::orig::arg::str(icon)))->file), 1, 0u);
                const u32 c1 = white();
                sprite(static_cast<u32>(ix), static_cast<u32>(iy), str(d::s_outbox) /* "!outbox" */,
                       kHeroIcons, c1, 0u, 0u, 0u, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0u,
                       0xffffffffu, 0xffffffffu);
                const u32 c2 = white();
                sprite(static_cast<u32>(ix), static_cast<u32>(iy), icon, kHeroIcons, c2, 0u, 0u, 0u,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, 0u, 0xffffffffu, 0xffffffffu);
                joan::orig::as_texture_manager(kTex)->release(joan::orig::arg::mptr(tex));  // 0x0060a550
            }
        }

        // Objective markers (2D-layer models).
        float mpos[3] = {0.0f, 0.0f, 0.0f};  // [ebp-0xf0], fix/uninit (c)
        float angle = 0.0f;                  // [ebp-0xf4], fix/uninit (c)
        for (i32 i = 0; i < jo::as_mini_res(info)->map_count; ++i) {
            u8* const e = reinterpret_cast<u8*>(jo::as_mini_res(info)->maps + i);
            if (jo::view<i8>(jo::as_mini_res_map(e)->enabled) == 0 || jo::view<i8>(jo::as_mini_res_map(e)->no_mesh) != 0 || jo::as_mini_res_map(e)->object == nullptr || jo::as_mini_res_map(e)->overlay == nullptr) continue;
            switch (static_cast<i32>(jo::as_mini_res_map(e)->kind)) {
            case 0:
                std::memcpy(mpos, jo::as_mini_res_map(e)->pos, 12);
                angle = 0.0f;
                break;
            case 1: {
                const u32 h = joan::orig::arg::as<u32>(joan::orig::as_list(kUnits)->find_name_51fdd0(joan::orig::arg::str(jo::as_mini_res_map(e)->unit_name)));  // 0x0051fdd0
                if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(h)) != 0) continue;  // 0x00428b70
                joan::orig::as_unit(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(h)))->position(joan::orig::arg::mptr(mpos));  // 0x004fedb0
                angle = joan::orig::arg::as<float>(joan::orig::as_ground_object(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(h)))->heading());  // 0x004ff850
                break;
            }
            case 2: {
                void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3f0
                if (u == nullptr) continue;
                joan::orig::as_unit(u)->position(joan::orig::arg::mptr(mpos));
                angle = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());
                break;
            }
            default:
                break;  // fix/uninit (c): previous values
            }
            const i32 mx = ftol(kFpu.mul(kFpu.div(mpos[0], fild(size_x)), tw));  // [ebp-0x100]
            const i32 my = ftol(kFpu.mul(kFpu.div(mpos[1], fild(size_y)), th));  // [ebp-0x104]
            i32 sx_ = 0;  // [ebp-0x108]
            i32 sy_ = 0;  // [ebp-0x10c]
            switch (sel) {
            case 0:
                sx_ = ftol(kFpu.add(fild(mx), cx));
                sy_ = ftol(kFpu.sub(cy, fild(my)));
                break;
            case 1:
                sx_ = ftol(kFpu.sub(cx, fild(my)));
                sy_ = ftol(kFpu.sub(cy, fild(mx)));
                break;
            case 2:
                sx_ = ftol(kFpu.sub(cx, fild(mx)));
                sy_ = ftol(kFpu.add(fild(my), cy));
                break;
            default:
                sx_ = ftol(kFpu.add(fild(my), cx));
                sy_ = ftol(kFpu.add(fild(mx), cy));
                break;
            }
            const float rad = jo::as_mini_res_map(e)->radius;
            // fcompp / test ah,0x41: proceed only when size > position - radius (not equal, not NaN)
            if (!(fild(size_read(0x0072f9dc)) > kFpu.sub(fild(sx_), rad))) continue;
            if (!(fild(size_read(0x0072f9fd)) > kFpu.sub(fild(sy_), rad))) continue;
            // fcomp 0.0 / test ah,1: skip when position + radius < 0 or unordered
            if (!(kFpu.add(fild(sx_), rad) >= fd(kZero))) continue;
            if (!(kFpu.add(fild(sy_), rad) >= fd(kZero))) continue;
            if (static_cast<i32>(jo::as_mini_res_map(e)->kind) == 2) {
                const double a = kFpu.sub(-static_cast<double>(angle), fd(kPi));
                const double b = kFpu.mul(kFpu.mul(fild(rot), fd(k90)), fd(kDeg));
                angle = static_cast<float>(kFpu.sub(a, b));
            } else {
                angle = static_cast<float>(kFpu.sub(-static_cast<double>(angle), fd(k3Pi2)));
            }
            joan::orig::as_sprite_queue(kCanvas)->place_model(joan::orig::arg::i32(static_cast<u32>(sx_)), joan::orig::arg::i32(static_cast<u32>(sy_)), 7u, joan::orig::arg::mptr(jo::as_mini_res_map(e)->overlay), joan::orig::arg::u32(f_bits(angle)), 0u, 0u, 0x3f800000u);  // 0x006349b0
        }
    }
};

JOAN_REPLACE("game/b116_04", "ObjectiveMap::create_camera_72d380", 0x0072d380, &H::create_camera);
// fix/uninit: rotation [0xa4f3ac] outside 0..3 uses the layout of rotation & 3 (original: garbage map
// rectangle / tile / icon / marker positions); a unit that is not one of the 8 heroes before any hero
// name was copied is skipped (original: garbage icon name); objective marker types other than 0..2
// use the last written position / angle, 0 before the first (original: stack garbage).
JOAN_REPLACE("fix/uninit", "ObjectiveMap::draw_72e480", 0x0072e480, &H::draw);

}  // namespace game_b116_04
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

JOAN_COUNTER(c_free_lists, "Obj::free_lists");

struct H {
    // ------------------------------------------------------------------ 0x0072d550
    void free_lists() {
        c_free_lists.hit();
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene) != nullptr) {
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->count_5ea3f0()); i > 0; --i) {  // 0x005ea3f0
                void** const e = joan::orig::arg::as<void**>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->actor_at(joan::orig::arg::i32(i)));    // 0x005ea410
                joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->remove_5ea2f0(joan::orig::arg::mptr(e));                              // 0x005ea2f0
                void* const p = *e;
                if (p != nullptr) joan::orig::as_actor_def(p)->destroy(1u);  // 0x004b0c20 (delete)
                if (e != nullptr) joan::orig::as_scene_object(e)->destroy(1u);  // 0x00420840 (delete)
            }
            void* const list = jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene);
            if (list != nullptr) joan::orig::as_scene(list)->destroy(1u);  // 0x004201f0 (delete)
            jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->map_camera) != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::CampScreen>(this)->map_camera);
            if (o != nullptr) joan::orig::as_camera(o)->destroy(1u);  // 0x00562910 (delete)
            jo::view<void*>(jo::self<jo::CampScreen>(this)->map_camera) = nullptr;
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj::free_lists", 0x0072d550, &H::free_lists);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b116_06.cpp (fix/uninit)
namespace {
namespace game_b116_06 {


using joan::hook::x87::ftol;

using joan::orig::helpers::as_float;
u32 as_bits(float f) {
    u32 bits;
    std::memcpy(&bits, &f, sizeof bits);
    return bits;
}

JOAN_COUNTER(c_map, "ObjectiveMenu::init_map_72d6f0");

void* const kFiles = obj(d::g_misc);

// ------------------------------------------------------------------------- 0x0072d6f0 helpers
// [ebp-0x368] camera block (0x006140a0 copies 10 dwords), [ebp-0x340] file object (0x00589550,
// fields up to +0x19a), [ebp-0x1a4] path string (0x005a1440 / 0x005a1550 cap it at 0x190 + NUL).
struct Frame72 {
    u32 cam[10];
    alignas(alignof(void*)) u8 file[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger FileTxt
    char path[0x194];
};
static_assert(offsetof(Frame72, path) == 0x28 + sizeof(Frame72::file) && sizeof(Frame72) >= 0x368 - 0x10, "Frame72");  // 0x368 - 0x10 bytes on x86

i16 rotation() { return global<i16>(d::g_rotation); }

// Fit distance along one axis: (|p - size / 2| * k * 2 / span + dist) - k.
float fit(i32 p, i32 size, float k, img::Addr span, float dist) {
    const i32 d = joan::orig::arg::as<i32>(joan::orig::libc::abs(static_cast<i32>(static_cast<u32>(p) - static_cast<u32>(size / 2))));  // abs
    const Fpu fpu{true};
    double t = fpu.mul(static_cast<double>(d), k);
    t = fpu.add(t, t);
    t = fpu.div(t, global<float>(span));
    t = fpu.add(t, dist);
    t = fpu.sub(t, k);
    return static_cast<float>(t);
}

void map_camera(u8* self) {
    Frame72 f;
    joan::orig::as_string(f.path)->construct();              // 0x005a1170
    joan::orig::as_string(f.path)->assign(str(d::s_SCRIPT_SCENARIO));              // 0x005a1440 "SCRIPT\\SCENARIO\\"
    {
        const char* const entry = global<jo::ScenarioEntry*>(d::g_stages)[global<i32>(d::g_stage_index) - 1].texts + 0x145;
        joan::orig::as_string(f.path)->append(joan::orig::arg::str(entry));  // 0x005a1550
    }
    joan::orig::as_string(f.path)->append(str(d::s_BM));                  // ".BM"
    {
        const char* const p = joan::orig::arg::as<const char*>(joan::orig::as_string(f.path)->c_str());  // 0x00417d90
        if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(p)) == 0) return;  // 0x00590b80
    }
    {
        const char* const p = joan::orig::arg::as<const char*>(joan::orig::as_string(f.path)->c_str());
        joan::orig::as_file_txt(f.file)->construct(joan::orig::arg::str(p));  // 0x00589550
    }
    jo::view<u32>(jo::as_camp_screen(self)->map_min_x) = 0;
    jo::view<u32>(jo::as_camp_screen(self)->map_min_y) = 0;
    jo::view<u32>(jo::as_camp_screen(self)->map_max_x) = 0;
    jo::view<u32>(jo::as_camp_screen(self)->map_max_y) = 0;
    joan::orig::as_obj_menu(self)->parse_map_script(joan::orig::arg::mptr(f.file));                  // 0x00731a60
    joan::orig::as_camera(jo::as_camp_screen(self)->map_camera)->frame(joan::orig::arg::mptr(f.cam));  // 0x006140a0
    {
        const Fpu fpu{true};
        const double two = global<float>(d::flt_2);
        f.cam[1] = as_bits(static_cast<float>(
            fpu.div(fpu.add(jo::as_camp_screen(self)->map_min_x, jo::as_camp_screen(self)->map_max_x), two)));
        f.cam[2] = as_bits(static_cast<float>(
            fpu.div(fpu.add(jo::as_camp_screen(self)->map_min_y, jo::as_camp_screen(self)->map_max_y), two)));
        f.cam[3] = 0;
        const double k90 = global<float>(d::flt_90);
        double a = fpu.mul(static_cast<double>(rotation()), k90);
        a = fpu.sub(k90, a);  // fsubr
        a = fpu.mul(a, global<float>(d::flt_0_017453292));
        f.cam[4] = as_bits(static_cast<float>(a));
        f.cam[5] = 0x3f32b8c2u;
        f.cam[7] = 0x447a0000u;  // 1000.0f
    }
    joan::orig::as_camera(jo::as_camp_screen(self)->map_camera)->set_frame(joan::orig::arg::mptr(f.cam));  // 0x00614040

    u32 corner[3];  // [ebp-0x374] x, [ebp-0x370] y, [ebp-0x36c] z
    corner[2] = 0;
    switch (static_cast<u32>(static_cast<i32>(rotation()))) {
    case 0:
        corner[0] = jo::view<u32>(jo::as_camp_screen(self)->map_min_x);
        corner[1] = jo::view<u32>(jo::as_camp_screen(self)->map_min_y);
        break;
    case 1:
        corner[0] = jo::view<u32>(jo::as_camp_screen(self)->map_min_x);
        corner[1] = jo::view<u32>(jo::as_camp_screen(self)->map_max_y);
        break;
    case 2:
        corner[0] = jo::view<u32>(jo::as_camp_screen(self)->map_max_x);
        corner[1] = jo::view<u32>(jo::as_camp_screen(self)->map_max_y);
        break;
    case 3:
        corner[0] = jo::view<u32>(jo::as_camp_screen(self)->map_max_x);
        corner[1] = jo::view<u32>(jo::as_camp_screen(self)->map_min_y);
        break;
    default:
        // fix/uninit: the original projects uninitialised stack words here; (0, 0) instead.
        corner[0] = 0;
        corner[1] = 0;
        break;
    }
    i32 sx = 0, sy = 0;  // always written by 0x006153c0
    joan::orig::as_camera(jo::as_camp_screen(self)->map_camera)->project(joan::orig::arg::mptr(corner), &sx, &sy, joan::orig::arg::mptr(i32{0}));  // 0x006153c0
    const float k = jo::as_matrix44(joan::orig::arg::as<u8*>(joan::orig::as_camera(jo::as_camp_screen(self)->map_camera)->matrix_ee()))->m[14];  // 0x00616ec0
    const float dist = as_float(f.cam[7]);
    const float dx = fit(sx, joan::canvas::size_read(0x0072d9c4), k, d::flt_680, dist);  // W, 680
    const float dy = fit(sy, joan::canvas::size_read(0x0072da0b), k, d::flt_424, dist);  // H, 424
    f.cam[7] = as_bits(dx > dy ? dx : dy);
    {
        const double v = static_cast<double>(as_float(f.cam[7]));
        const double r = joan::orig::arg::as<double>(joan::orig::as_misc(kFiles)->limit_double(joan::orig::arg::f64(v), joan::orig::arg::f64(250.0), joan::orig::arg::f64(1000.0)));  // 0x00590b00 clamp
        f.cam[7] = as_bits(static_cast<float>(r));
    }
    joan::orig::as_camera(jo::as_camp_screen(self)->map_camera)->set_frame(joan::orig::arg::mptr(f.cam));  // 0x00614040
    joan::orig::as_file_txt(f.file)->destruct();             // 0x00589890
}

i32 ftolf(float v) { return ftol(static_cast<double>(v)); }

// First grid cell (row r = 1.., column c = 1..) of grid `kind` whose entry exists with byte
// +0x1f == 0: its image size (+0x34 / +0x36) into self+off / +off+4.
void first_tile(i32& out_w, i32& out_h, u8* grid, u8* images, i32 kind, i32 rows_i, i32 cols_i) {
    for (i32 r = 1; r <= static_cast<i32>(jo::as_mini_res(grid)->rect[rows_i]); ++r) {
        u8* found = nullptr;
        for (i32 c = 1; c <= static_cast<i32>(jo::as_mini_res(grid)->rect[cols_i]); ++c) {
            found = joan::orig::arg::as<u8*>(joan::orig::as_mini_res(grid)->find_frame(joan::orig::arg::i16(kind), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(r))), joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(c)))));  // 0x0071b3b0
            if (found != nullptr && jo::view<i8>(jo::as_mini_res_frame(found)->is_default) == 0) {
                u8* const img = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(images)->find(joan::orig::arg::str(found)));  // 0x00620b40
                out_w = static_cast<i16>(jo::as_bmp_entry(img)->width);
                out_h = static_cast<i16>(jo::as_bmp_entry(img)->height);
                break;
            }
        }
        if (found != nullptr && jo::view<i8>(jo::as_mini_res_frame(found)->is_default) == 0) break;
    }
}

struct H {

    // ----------------------------------------------------------------------- 0x0072d6f0
    void init_map() {
        c_map.hit();
        u8* const s = reinterpret_cast<u8*>(this);
        if (jo::as_camp_screen(s)->map_scene != nullptr) {
            bool busy = false;
            if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::as_camp_screen(s)->map_scene)->count_5ea3f0()) != 0) {          // 0x005ea3f0
                busy = joan::orig::arg::as<i32>(joan::orig::as_scene(jo::as_camp_screen(s)->map_scene)->get_4c()) != 0;     // 0x005eeba0
            }
            if (!busy && global<i32>(d::g_stage_index) > 0) map_camera(s);
        }

        // Bounding box of the sector list.
        for (i32 i = 0; i < jo::as_world(global<u8*>(d::g_world))->cell_count; ++i) {
            const auto entry = [i]() { return jo::as_world(global<u8*>(d::g_world))->cells[i].item; };
            if (i == 0) {
                jo::view<i32>(jo::as_camp_screen(s)->sector_min_x) = ftolf(entry()->box[0]);
                jo::view<i32>(jo::as_camp_screen(s)->sector_min_y) = ftolf(entry()->box[1]);
                jo::view<i32>(jo::as_camp_screen(s)->sector_max_x) = ftolf(entry()->box[3]);
                jo::view<i32>(jo::as_camp_screen(s)->sector_max_y) = ftolf(entry()->box[4]);
                continue;
            }
            // fild + fcomp: test ah,0x41 / jne = set when strictly greater; test ah,1 / je = set
            // when less or unordered.
            if (static_cast<double>(jo::view<i32>(jo::as_camp_screen(s)->sector_min_x)) > static_cast<double>(entry()->box[0])) {
                jo::view<i32>(jo::as_camp_screen(s)->sector_min_x) = ftolf(entry()->box[0]);
            }
            if (!(static_cast<double>(jo::view<i32>(jo::as_camp_screen(s)->sector_max_x)) >= static_cast<double>(entry()->box[3]))) {
                jo::view<i32>(jo::as_camp_screen(s)->sector_max_x) = ftolf(entry()->box[3]);
            }
            if (static_cast<double>(jo::view<i32>(jo::as_camp_screen(s)->sector_min_y)) > static_cast<double>(entry()->box[1])) {
                jo::view<i32>(jo::as_camp_screen(s)->sector_min_y) = ftolf(entry()->box[1]);
            }
            if (!(static_cast<double>(jo::view<i32>(jo::as_camp_screen(s)->sector_max_y)) >= static_cast<double>(entry()->box[4]))) {
                jo::view<i32>(jo::as_camp_screen(s)->sector_max_y) = ftolf(entry()->box[4]);
            }
        }

        u8* const grid = global<u8*>(d::g_a4f385);
        u8* const images = reinterpret_cast<u8*>(&jo::as_world(global<u8*>(d::g_world))->bitmaps);
        first_tile(jo::as_camp_screen(s)->u32_1f9, jo::as_camp_screen(s)->u32_1fd, grid, images, 0, 0, 1);  // rect[0] / rect[1] -> +0x1f9 / +0x1fd
        first_tile(jo::as_camp_screen(s)->u32_201, jo::as_camp_screen(s)->u32_205, grid, images, 1, 2, 3);  // rect[2] / rect[3] -> +0x201 / +0x205

        const Fpu fpu{true};
        const double half = global<float>(d::flt_0_5);
        {
            const i32 sum = static_cast<i32>(jo::as_camp_screen(s)->sector_min_x + jo::as_camp_screen(s)->sector_max_x);
            jo::view<i32>(jo::as_camp_screen(s)->view_x) = ftol(fpu.mul(static_cast<double>(sum), half));
        }
        {
            const i32 sum = static_cast<i32>(jo::as_camp_screen(s)->sector_min_y + jo::as_camp_screen(s)->sector_max_y);
            jo::view<i32>(jo::as_camp_screen(s)->view_y) = ftol(fpu.mul(static_cast<double>(sum), half));
        }
        float wx, wy;  // [ebp-0x3b4], [ebp-0x3b8]
        {
            const i32 q = static_cast<i32>(static_cast<u32>(jo::view<i32>(jo::as_camp_screen(s)->sector_max_x)) - static_cast<u32>(jo::view<i32>(jo::as_camp_screen(s)->sector_min_x))) /
                          jo::as_mini_res(grid)->i32_29;
            wx = static_cast<float>(static_cast<i32>(static_cast<u32>(q) * static_cast<u32>(jo::as_camp_screen(s)->u32_201)));
        }
        {
            const i32 q = static_cast<i32>(static_cast<u32>(jo::view<i32>(jo::as_camp_screen(s)->sector_max_y)) - static_cast<u32>(jo::view<i32>(jo::as_camp_screen(s)->sector_min_y))) /
                          jo::as_mini_res(grid)->i32_2d;
            wy = static_cast<float>(static_cast<i32>(static_cast<u32>(q) * static_cast<u32>(jo::as_camp_screen(s)->u32_205)));
        }
        const float k680 = global<float>(d::flt_680), k424 = global<float>(d::flt_424);
        float zoom;  // [ebp-0x3bc]
        if (rotation() == 1 || rotation() == 3) {
            // fcompp: st0 = 424 / wx, st1 = 680 / wy; fallthrough (st0 > st1) takes 680 / wy.
            zoom = (k424 / wx > k680 / wy) ? k680 / wy : k424 / wx;
        } else {
            zoom = (k424 / wy > k680 / wx) ? k680 / wx : k424 / wy;
        }
        if (zoom > global<float>(d::flt_0_25)) {  // test ah,0x41 / je: strictly greater than 0.25
            double z = fpu.add(zoom, zoom);
            z = fpu.add(z, global<float>(d::flt_1));
            jo::as_camp_screen(s)->zoom_step = ftol(z);
        } else {
            jo::as_camp_screen(s)->zoom_step = 1;
        }
    }
};

// fix/uninit: a map rotation 0xa4f3ac outside 0..3 leaves the projected corner [ebp-0x374] /
// [ebp-0x370] unwritten (stack garbage); (0, 0) is used. Exact for rotations 0..3.
JOAN_REPLACE("fix/uninit", "ObjectiveMenu::init_map_72d6f0", 0x0072d6f0, &H::init_map);

}  // namespace game_b116_06
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

JOAN_COUNTER(c_clear_list, "X::clear_list");

struct H {
    void clear_list() {  // 0x0072e370
        c_clear_list.hit();
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene) == nullptr) return;
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->count_5ea3f0()); i > 0; --i) {  // 0x005ea3f0
            void** const e = joan::orig::arg::as<void**>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->actor_at(joan::orig::arg::i32(i)));  // 0x005ea410
            joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->remove_5ea2f0(joan::orig::arg::mptr(e));                           // 0x005ea2f0
            void* const o = *e;
            if (o != nullptr) joan::orig::as_actor_def(o)->destroy(1u);  // 0x004b0c20
            if (e != nullptr) joan::orig::as_scene_object(e)->destroy(1u);  // 0x00420840
        }
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->delete_lights();  // 0x005eb820
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::clear_list", 0x0072e370, &H::clear_list);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b111_16.cpp (game/b111_16)
namespace {
namespace game_b111_16 {

i32 ftol(double v) { return joan::hook::x87::ftol(v); }
float as_float(u32 bits) {
    float f = 0.0f;
    std::memcpy(&f, &bits, 4);
    return f;
}
u32 as_bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}
double as_double(std::uint64_t bits) {
    double d = 0.0;
    std::memcpy(&d, &bits, 8);
    return d;
}

struct Vec {
    float x, y, z;
};
static_assert(sizeof(Vec) == 12, "vec3 by value");

void* const kRandom = obj(d::g_misc);
void* const kKeys = obj(d::g_keys_a48820);
void* const kTexts = obj(d::g_strings);

JOAN_COUNTER(c_pan, "MapView::pan_zoom_730150");

struct H {
    // -------------------------------------------------------------- 0x00730150
    double clampd(double x, double lo, double hi) {
        return joan::orig::arg::as<double>(joan::orig::as_misc(kRandom)->limit_double(joan::orig::arg::f64(x), joan::orig::arg::f64(lo), joan::orig::arg::f64(hi)));  // 0x00590b00
    }
    i32 clampi(i32 x, i32 lo, i32 hi) { return joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->limit_int(joan::orig::arg::i32(x), joan::orig::arg::i32(lo), joan::orig::arg::i32(hi))); }  // 0x00590aa0

    u8 pan_zoom() {
        // Orientation outside 0..3: the original adds uninitialised deltas; joan.exe's body no longer
        // exists (stage S5), so the fix/uninit deltas are used.
        c_pan.hit();
        i32 mode = global<i32>(d::g_control_mode);
        if (mode == 4) mode = global<i32>(d::g_control_base_mode);
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]))->mouse(0x1b, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {  // 0x006218c0
            joan::orig::as_camp_screen(this)->set_state_721a60(0u);                                       // 0x00721a60
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->view_toggle_button))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0) {
            jo::view<i8>(jo::self<jo::CampScreen>(this)->camera_view) = jo::view<i8>(jo::self<jo::CampScreen>(this)->camera_view) == 0 ? 1 : 0;
            u32 c = 0;
            const u32 white = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
            void* const text = jo::view<i8>(jo::self<jo::CampScreen>(this)->camera_view) != 0 ? joan::orig::arg::as<void*>(joan::orig::as_obj_menu(kTexts)->open_1a())    // 0x004dbf40
                                                 : joan::orig::arg::as<void*>(joan::orig::as_menu(kTexts)->objmenu_1b());   // 0x004dbf70
            joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->view_toggle_button))->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(white), 1, 2, 0, 2, 2);  // 0x00623ce0
            return 1;
        }
        void* const cam = jo::view<void*>(jo::self<jo::CampScreen>(this)->map_camera);
        const Fpu fpu{true};
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->camera_view) != 0) {
            u32 s[10] = {};
            joan::orig::as_camera(cam)->frame(joan::orig::arg::mptr(static_cast<void*>(s)));  // 0x006140a0
            const i32 d34 = global<i32>(d::g_mouse_delta_x), d38 = global<i32>(d::g_mouse_delta_y), d3c = global<i32>(d::g_mouse_wheel);
            i32 dx = 0, dy = 0;
            if (global<u32>(d::g_mouse_left_down) != 0) {
                dx = jo::view<i16>(jo::self<jo::CampScreen>(this)->map_drag_x) - global<i32>(d::g_mouse_x);
                dy = jo::view<i16>(jo::self<jo::CampScreen>(this)->map_drag_y) - global<i32>(d::g_mouse_y);
            }
            if (dx != 0 || dy != 0) {
                const float s7 = as_float(s[7]);
                const float f48 = static_cast<float>(
                    fpu.div(fpu.mul(fpu.mul(dx, global<float>(d::flt_0_25)), s7), global<float>(d::flt_250)));
                const float f4c = static_cast<float>(
                    fpu.div(fpu.mul(fpu.mul(dy, global<float>(d::flt_0_25)), s7), global<float>(d::flt_250)));
                Vec m58{}, m64{};
                joan::orig::matrix_row1(&m58, joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->matrix_ee())));  // 0x0060c1d0(0x00616ec0)
                joan::orig::matrix_axis_x(&m64, joan::orig::arg::mptr(joan::orig::arg::as<void*>(joan::orig::as_camera(cam)->matrix_ee())));  // 0x0060c180
                const float x1 = (m58.x * f4c + as_float(s[1])) + m64.x * f48;
                s[1] = as_bits(static_cast<float>(clampd(x1, jo::self<jo::CampScreen>(this)->map_min_x, jo::self<jo::CampScreen>(this)->map_max_x)));
                const float x2 = (m58.y * f4c + as_float(s[2])) + m64.y * f48;
                s[2] = as_bits(static_cast<float>(clampd(x2, jo::self<jo::CampScreen>(this)->map_min_y, jo::self<jo::CampScreen>(this)->map_max_y)));
            }
            if (global<u32>(d::g_a516d8) != 0) {
                s[4] = as_bits(static_cast<float>(fpu.add(fpu.mul(d34, global<float>(d::flt_0_01)), as_float(s[4]))));
                const double y = fpu.sub(as_float(s[5]), fpu.mul(d38, global<float>(d::flt_0_01)));
                s[5] = as_bits(static_cast<float>(
                    clampd(y, as_double(0x3fd657184ae74487ull), as_double(0x3ff921fb54442d18ull))));
            } else if (joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(joan::orig::arg::i32(mode), 0x32u)) != 0) {  // 0x004cb6d0
                s[4] = as_bits(as_float(s[4]) + global<float>(d::flt_0_02));
            } else if (joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(joan::orig::arg::i32(mode), 0x33u)) != 0) {
                s[4] = as_bits(as_float(s[4]) - global<float>(d::flt_0_02));
            }
            const double lo = as_double(0x406f400000000000ull), hi = as_double(0x408f400000000000ull);
            if (d3c != 0) {
                const double z = fpu.sub(as_float(s[7]), fpu.mul(d3c, global<float>(d::flt_0_4)));
                s[7] = as_bits(static_cast<float>(clampd(z, lo, hi)));
            } else if (joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(joan::orig::arg::i32(mode), 0x30)) != 0) {  // 0x004cb820
                s[7] = as_bits(static_cast<float>(clampd(as_float(s[7]) - global<float>(d::flt_40), lo, hi)));
            } else if (joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(joan::orig::arg::i32(mode), 0x31)) != 0) {
                s[7] = as_bits(static_cast<float>(clampd(as_float(s[7]) + global<float>(d::flt_40), lo, hi)));
            }
            joan::orig::as_camera(cam)->set_frame(joan::orig::arg::mptr(static_cast<void*>(s)));  // 0x00614040
            jo::self<jo::CampScreen>(this)->map_drag_x = global<u16>(d::g_mouse_x);
            jo::self<jo::CampScreen>(this)->map_drag_y = global<u16>(d::g_mouse_y);
            return 1;
        }
        u32 s2[10] = {};
        joan::orig::as_camera(cam)->frame(joan::orig::arg::mptr(static_cast<void*>(s2)));  // 0x006140a0 (copy not used further)
        u8* const map = static_cast<u8*>(global<void*>(d::g_a4f385));
        const i32 wheel = global<i32>(d::g_mouse_wheel);
        i32 dx = 0, dy = 0;
        if (global<u32>(d::g_mouse_left_down) != 0) {
            dx = global<i32>(d::g_mouse_x) - jo::view<i16>(jo::self<jo::CampScreen>(this)->map_drag_x);
            dy = global<i32>(d::g_mouse_y) - jo::view<i16>(jo::self<jo::CampScreen>(this)->map_drag_y);
        }
        if (dx != 0 || dy != 0) {
            float zoom = 0.25f;  // 0x3e800000
            if (jo::self<jo::CampScreen>(this)->zoom_step != 1) zoom = static_cast<float>(fpu.mul(jo::self<jo::CampScreen>(this)->zoom_step - 1, global<float>(d::flt_0_5)));
            auto delta = [&](i32 d, const i32& m, const i32& div) {
                const u32 prod = static_cast<u32>(d) * static_cast<u32>(m);
                return ftol(fpu.div(fpu.div(static_cast<i32>(prod), zoom), div));
            };
            i32 a8 = 0, ac = 0;  // other orientations (fix/uninit / stage S5): never written, 0
            switch (global<i16>(d::g_rotation)) {
                case 0:
                    a8 = delta(static_cast<i32>(0u - static_cast<u32>(dx)), jo::as_mini_res(map)->i32_29, jo::self<jo::CampScreen>(this)->u32_201);
                    ac = delta(dy, jo::as_mini_res(map)->i32_2d, jo::self<jo::CampScreen>(this)->u32_205);
                    break;
                case 1:
                    a8 = delta(dy, jo::as_mini_res(map)->i32_29, jo::self<jo::CampScreen>(this)->u32_201);
                    ac = delta(dx, jo::as_mini_res(map)->i32_2d, jo::self<jo::CampScreen>(this)->u32_205);
                    break;
                case 2:
                    a8 = delta(dx, jo::as_mini_res(map)->i32_29, jo::self<jo::CampScreen>(this)->u32_201);
                    ac = delta(dy, jo::as_mini_res(map)->i32_2d, jo::self<jo::CampScreen>(this)->u32_205);
                    break;
                case 3:
                    a8 = delta(static_cast<i32>(0u - static_cast<u32>(dy)), jo::as_mini_res(map)->i32_29, jo::self<jo::CampScreen>(this)->u32_201);
                    ac = delta(static_cast<i32>(0u - static_cast<u32>(dx)), jo::as_mini_res(map)->i32_2d, jo::self<jo::CampScreen>(this)->u32_205);
                    break;
                default:
                    break;
            }
            jo::view<i32>(jo::self<jo::CampScreen>(this)->view_x) = clampi(static_cast<i32>(jo::self<jo::CampScreen>(this)->view_x + static_cast<u32>(a8)), jo::view<i32>(jo::self<jo::CampScreen>(this)->sector_min_x), jo::view<i32>(jo::self<jo::CampScreen>(this)->sector_max_x));
            jo::view<i32>(jo::self<jo::CampScreen>(this)->view_y) = clampi(static_cast<i32>(jo::self<jo::CampScreen>(this)->view_y + static_cast<u32>(ac)), jo::view<i32>(jo::self<jo::CampScreen>(this)->sector_min_y), jo::view<i32>(jo::self<jo::CampScreen>(this)->sector_max_y));
        }
        if (wheel > 100 || joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(joan::orig::arg::i32(mode), 0x30)) != 0) {
            jo::self<jo::CampScreen>(this)->zoom_step = clampi(jo::self<jo::CampScreen>(this)->zoom_step + 1, 1, 5);
        } else if (wheel < -100 || joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(joan::orig::arg::i32(mode), 0x31)) != 0) {
            jo::self<jo::CampScreen>(this)->zoom_step = clampi(jo::self<jo::CampScreen>(this)->zoom_step - 1, 1, 5);
        }
        jo::self<jo::CampScreen>(this)->map_drag_x = global<u16>(d::g_mouse_x);
        jo::self<jo::CampScreen>(this)->map_drag_y = global<u16>(d::g_mouse_y);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_16", name, addr, fn)
B("MapView::pan_zoom_730150", 0x00730150, &H::pan_zoom);
#undef B

}  // namespace game_b111_16
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Screen::nop_730ba0", 0x00730ba0, &H::nop);
B("Screen::nop_730bc0", 0x00730bc0, &H::nop);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
namespace {
namespace game_b115_10 {

constexpr fn::Code kNew = fn::op_new;      // operator new (cdecl)


void* new_obj(u32 size, std::uintptr_t ctor) {
    void* const mem = cdecl_call<void*>(kNew, size);
    return mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
}

JOAN_COUNTER(c_heroes, "HeroModels::spawn_730be0");

struct H {
    // ===================================================================== 0x00730be0
    u32 spawn_heroes() {
        c_heroes.hit();
        // fix: [ebp-0x8c] / [ebp-0x90] start at 0 and keep their last value (see header)
        float pos8c[3] = {0.0f, 0.0f, 0.0f};
        u32 scale90 = 0;
        if (jo::self<jo::CampScreen>(this)->map_scene != nullptr) {
            u8* const map = static_cast<u8*>(global<void*>(d::g_a4f385));
            void* const hero = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0
            const i32 kind = static_cast<i32>(jo::as_unit(hero)->side);
            char name[12] = {};
            bool name_set = false;  // fix: no known name copied yet -> skip the spawn
            for (i32 k = joan::orig::as_dynarray(obj(d::g_units))->size(); k > 0; --k) {  // 0x00417ad0
                u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(k)));  // 0x0041b3f0
                if (u == nullptr || static_cast<i32>(jo::as_unit(u)->side) != kind || jo::as_unit(u)->rank < 4 || static_cast<i32>(jo::as_unit(u)->excluded) != 0 || u == hero) continue;
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
                {
                    float v34[3] = {0.0f, 0.0f, 0.0f};
                    joan::orig::as_unit(u)->position(joan::orig::arg::mptr(v34));  // 0x004fedb0 (result unused)
                }
                static const img::Addr kNames[8][3] = {
                    {d::s_Joan_of_Arc, d::s_joan_H, d::s_joan_M},  // Joan_of_Arc
                    {d::s_Jean_de_Metz_9df40c, d::s_metz_H, d::s_metz_M},  // Jean_de_Metz
                    {d::s_Alencon, d::s_alen_H, d::s_alen_M},  // Alencon
                    {d::s_LaHire, d::s_lahi_H, d::s_lahi_M},  // LaHire
                    {d::s_Orlean, d::s_bast_H, d::s_bast_M},  // Orlean
                    {d::s_FLAVY, d::s_flav_H, d::s_flav_M},  // FLAVY
                    {d::s_CHARLIE7, d::s_char_H, d::s_char_M},  // CHARLIE7
                    {d::s_arthur_de_richemont, d::s_rich_H, d::s_rich_M},  // arthur_de_richemont
                };
                for (i32 n = 0; n < 8; ++n) {
                    if (cdecl_call<i32>(kStricmp, jo::as_unit_type(rec)->name, str(kNames[n][0])) == 0) {
                        const bool h = jo::view<jo::TaskSlot>(jo::as_unit(u)->task).kind == 0 && jo::view<jo::TaskSlot>(jo::as_unit(u)->task).target != 0;
                        cdecl_call<char*>(kStrcpy, name, str(h ? kNames[n][1] : kNames[n][2]));
                        name_set = true;
                        break;
                    }
                }
                if (!name_set) continue;
                float p[3] = {0.0f, 0.0f, 0.0f};  // [ebp-0x4c] (always written by 0x004fedb0)
                joan::orig::as_unit(u)->position(joan::orig::arg::mptr(p));
                const u32 scale = f_bits(joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading()));  // 0x004ff850
                place_model(p, scale, name);
            }
            for (i32 i = 0; i < jo::as_mini_res(map)->map_count; ++i) {
                u8* const e = reinterpret_cast<u8*>(jo::as_mini_res(map)->maps + i);
                if (jo::view<i8>(jo::as_mini_res_map(e)->enabled) == 0 || jo::view<i8>(jo::as_mini_res_map(e)->no_mesh) != 0) continue;
                switch (jo::as_mini_res_map(e)->kind) {
                case 0:
                    std::memcpy(pos8c, jo::as_mini_res_map(e)->pos, 12);
                    scale90 = 0;
                    break;
                case 1: {
                    const u32 k = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_units))->find_name_51fdd0(joan::orig::arg::str(jo::as_mini_res_map(e)->unit_name)));  // 0x0051fdd0
                    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(k)) != 0) continue;  // 0x00428b70
                    joan::orig::as_unit(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(k)))->position(joan::orig::arg::mptr(pos8c));
                    scale90 = f_bits(joan::orig::arg::as<float>(joan::orig::as_ground_object(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(k)))->heading()));
                    break;
                }
                case 2: {
                    const u32 h0 = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());            // 0x004203c0
                    void* const o = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(h0));  // 0x0041b3f0
                    if (o != nullptr) {
                        joan::orig::as_unit(o)->position(joan::orig::arg::mptr(pos8c));
                        scale90 = f_bits(joan::orig::arg::as<float>(joan::orig::as_ground_object(o)->heading()));
                    }
                    break;
                }
                default:
                    break;
                }
                place_model(pos8c, scale90, jo::as_mini_res_map(e)->icon);
            }
        }
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->menu_buttons[7])->scale_to_61f1b0(1, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0xc8u);  // 0x0061f1b0
        return 0;  // 0x0061f1b0 returns nothing; the result reaches only callers that ignore it
    }

    // shared tail of both 0x730be0 loops: scale the position, drop it onto the scenery below,
    // create the model (new 0x10c + 0x005aab20, new 0x2d8 + 0x005af390, load MODEL\<name>).
    void place_model(float* p, u32 scale, const char* name) {
        const float k = global<float>(d::flt_0_012207031);
        p[0] = static_cast<float>(kFpu.mul(p[0], k));
        p[1] = static_cast<float>(kFpu.mul(p[1], k));
        p[2] = static_cast<float>(kFpu.mul(p[2], k));
        float bot[3];  // [ebp-0x5c] / [ebp-0xa4]
        std::memcpy(bot, p, 12);
        float ray[6];  // top [ebp-0x68] / [ebp-0xb0], then bottom
        std::memcpy(ray, bot, 12);
        ray[2] = static_cast<float>(kFpu.add(ray[2], fd(d::flt_1000)));
        bot[2] = static_cast<float>(kFpu.sub(bot[2], fd(d::flt_1000)));
        std::memcpy(&ray[3], bot, 12);
        for (i32 n = joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::CampScreen>(this)->map_scene)->count_5ea3f0()); n > 0; --n) {  // 0x005ea3f0
            u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::self<jo::CampScreen>(this)->map_scene)->actor_at(joan::orig::arg::i32(n)));       // 0x005ea410
            if (jo::as_actor(m)->kind == -1) continue;
            float t = 0.0f;
            if (joan::orig::arg::as<i32>(joan::orig::as_plane(jo::as_actor(m)->plane)->segment_hit(&t, joan::orig::arg::mptr(ray))) != 0) {  // 0x005d4a80
                p[2] = static_cast<float>(kFpu.add(kFpu.mul(kFpu.sub(ray[5], ray[2]), t), ray[2]));
                break;
            }
        }
        u8* const model = static_cast<u8*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::SceneObject)), fn::code_value(fn::Actor_Actor_5aab20)));  // 0x005aab20
        void* const inner = new_obj(static_cast<std::uint32_t>(sizeof(jo::ActorDef)), fn::code_value(fn::ActorDef_ActorDef));                    // 0x005af390
        jo::view<void*>(jo::as_actor(model)->def) = inner;
        joan::orig::as_actor_def(jo::as_actor(model)->def)->load2(str(d::s_MODEL_9d3750), joan::orig::arg::str(name), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), nullptr, nullptr);  // 0x005af3d0
        jo::as_actor(model)->anim = nullptr;
        joan::orig::as_scene_object(model)->init();     // 0x005aab70
        joan::orig::as_actor(model)->set_position_5ac310(joan::orig::arg::mptr(p));  // 0x005ac310
        jo::view<u32>(jo::as_actor(model)->scale) = 0x3f800000u;
        joan::orig::as_scene_object(model)->set_yaw(joan::orig::arg::u32(scale));  // 0x005ac6a0
        jo::as_actor(model)->kind = -1;
        for (i32 q = 0; q < jo::as_actor(model)->plane->material_count; ++q) {
            jo::as_actor(model)->plane->materials[q].texture_mode = 0x20022u;
            void* const mesh = jo::as_actor(model)->def->model->mesh;
            jo::as_plane(mesh)->materials[q].texture_mode = 0x20022u;
        }
        joan::orig::as_scene(jo::self<jo::CampScreen>(this)->map_scene)->add_actor(joan::orig::arg::mptr(model));  // 0x005ea1f0
    }
};

// 0x00730be0: model name unset for an unknown hero (before any known one) -> spawn skipped;
// marker position / scale unset -> start at 0 (then last value).
JOAN_REPLACE("fix/uninit", "HeroModels::spawn_730be0", 0x00730be0, &H::spawn_heroes);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

// 0x0061f1b0 (ret 0x14), raw dwords as pushed by the original.
using joan::orig::helpers::anim_set;
using joan::orig::helpers::anim_set6;  // 6.0f

JOAN_COUNTER(c_purge, "Screen::purge_items");

struct H {
    // 0x00731930
    void purge_items() {
        c_purge.hit();
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene) != nullptr) {
            for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->count_5ea3f0()); i > 0; --i) {  // 0x005ea3f0
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->actor_at(joan::orig::arg::i32(i)));     // 0x005ea410
                if (jo::as_actor(e)->kind != -1) continue;
                joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_scene))->remove_5ea2f0(joan::orig::arg::mptr(static_cast<void*>(e)));  // 0x005ea2f0
                void* const p = jo::as_actor(e)->def;
                if (p != nullptr) joan::orig::as_actor_def(p)->destroy(1u);  // 0x004b0c20
                if (e != nullptr) joan::orig::as_scene_object(e)->destroy(1u);  // 0x00420840
            }
        }
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Screen::purge_items", 0x00731930, &H::purge_items);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

JOAN_COUNTER(c_parse_map, "Loader::parse_map_731a60");

struct H {
    // ------------------------------------------------------------------ 0x00731a60
    u32 parse_map_731a60(void* reader) {
        c_parse_map.hit();
        for (;;) {
            const char* tok = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(reader)->get_token(1));  // 0x00589c70
            if (tok == nullptr) return 0;
            if (stricmp_(tok, str(d::s_CreateLight)) == 0) {          // "CreateLight"
                joan::orig::as_scene(this)->parse_light(joan::orig::arg::mptr(reader));        // 0x00731b00
            } else if (stricmp_(tok, str(d::s_CreateMapActor)) == 0) {   // "CreateMapActor"
                joan::orig::as_scene(this)->parse_model(joan::orig::arg::mptr(reader));        // 0x00731ff0
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Loader::parse_map_731a60", 0x00731a60, &H::parse_map_731a60);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
namespace {
namespace game_b104_15 {

JOAN_COUNTER(c_parse_light, "Scene::parse_light");
JOAN_COUNTER(c_parse_model, "Scene::parse_model");

// Script tokenizer of the scene file (0x00589c70, arg 1).
const char* tok(void* file) { return joan::orig::arg::as<const char*>(joan::orig::as_file_txt(file)->get_token(1)); }  // 0x00589c70
using joan::orig::helpers::strlen_;  // strlen
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }                 // atof (st0)
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }                        // atoi

struct H {
    // ------------------------------------------------------------------------ 0x00731b00
    std::uintptr_t parse_light(void* file) {  // EAX: the last token (pointer-wide on x64)
        c_parse_light.hit();
        tok(file);
        std::uintptr_t light = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_scene(jo::self<jo::CampScreen>(this)->map_scene)->add_light());  // 0x005eb5d0
        tok(file);
        char buf[0x100];
        joan::orig::as_file_txt(file)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0xff);  // 0x00589f10 read_line
        const u32 len = strlen_(buf);
        if (len == 0)
            light &= ~(std::uintptr_t{0xff} << 24);  // buf[-1] is the top byte of the (32-bit) light pointer local
        else
            buf[len - 1] = 0;
        if (stricmp_(buf, str(d::s_ambient)) == 0)         // "ambient"
            joan::orig::as_camera(reinterpret_cast<void*>(light))->init(0u);       // 0x005c58a0
        else if (stricmp_(buf, str(d::s_directional)) == 0)    // "directional"
            joan::orig::as_camera(reinterpret_cast<void*>(light))->init(1u);
        else if (stricmp_(buf, str(d::s_spot)) == 0)    // "spot"
            joan::orig::as_camera(reinterpret_cast<void*>(light))->init(2u);
        else if (stricmp_(buf, str(d::s_point)) == 0)    // "point"
            joan::orig::as_camera(reinterpret_cast<void*>(light))->init(3u);
        for (u32 k = 0; k < 4; ++k) {
            tok(file);
            const char* t = tok(file);
            const i32 v = atoi_(t);
            jo::view<u8[4]>(jo::as_light(reinterpret_cast<void*>(light))->colour)[k] = static_cast<u8>(v);
        }
        {
            u32 factor = 0x3f800000u;
            const char* t = tok(file);
            if (stricmp_(t, str(d::s_HD)) == 0) factor = 0x3c8efa35u;  // "HD": degrees
            t = tok(file);
            const double d = atof_(t);
            float ff;
            std::memcpy(&ff, &factor, 4);
            jo::as_light(reinterpret_cast<void*>(light))->orbit_a = static_cast<float>(Fpu{true}.mul(d, ff));
        }
        {
            u32 factor = 0x3f800000u;
            const char* t = tok(file);
            if (stricmp_(t, str(d::s_VD)) == 0) factor = 0x3c8efa35u;  // "VD"
            t = tok(file);
            const double d = atof_(t);
            float ff;
            std::memcpy(&ff, &factor, 4);
            jo::as_light(reinterpret_cast<void*>(light))->orbit_b = static_cast<float>(Fpu{true}.mul(d, ff));
        }
        jo::Light* const l = jo::as_light(reinterpret_cast<void*>(light));
        float* const fields[7] = {&l->position[0], &l->position[1], &l->position[2], &l->attenuation0, &l->attenuation1, &l->attenuation2, &l->range};  // +0xc +0x10 +0x14 +0x48 +0x4c +0x50 +0x2c
        for (u32 k = 0; k < 7; ++k) {
            tok(file);
            const char* t = tok(file);
            const double d = atof_(t);
            *fields[k] = static_cast<float>(d);
        }
        joan::orig::as_light(reinterpret_cast<void*>(light))->invalidate();  // 0x005c5c00
        return reinterpret_cast<std::uintptr_t>(tok(file));
    }

    // ------------------------------------------------------------------------ 0x00731ff0
    u32 parse_model(void* file) {
        c_parse_model.hit();
        tok(file);
        tok(file);
        char buf[0x100];
        joan::orig::as_file_txt(file)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0xff);  // 0x00589f10 read_line
        const u32 len = strlen_(buf);
        if (len != 0) buf[len - 1] = 0;  // len 0: dead write below the buffer (see header)
        float v[3];  // [ebp-0x120], [ebp-0x11c], [ebp-0x118] (passed to 0x005ac310)
        for (u32 k = 0; k < 3; ++k) {
            tok(file);
            v[k] = static_cast<float>(atof_(tok(file)));
        }
        const Fpu fpu{true};
        tok(file);
        const float a = static_cast<float>(fpu.add(atof_(tok(file)), v[0]));  // [ebp-0x124]
        tok(file);
        const float b = static_cast<float>(fpu.add(atof_(tok(file)), v[1]));  // [ebp-0x128]
        tok(file);
        const float c = static_cast<float>(fpu.add(atof_(tok(file)), v[0]));  // [ebp-0x12c]
        tok(file);
        const float e = static_cast<float>(fpu.add(atof_(tok(file)), v[1]));  // [ebp-0x130]
        tok(file);
        const float angle = static_cast<float>(atof_(tok(file)));  // [ebp-0x134]
        tok(file);
        const float scale = static_cast<float>(atof_(tok(file)));  // [ebp-0x138]
        if (joan::orig::arg::as<i32>(joan::orig::as_list(jo::self<jo::CampScreen>(this)->map_scene)->count_5ea3f0()) != 0) {  // 0x005ea3f0
            if (!(a >= jo::self<jo::CampScreen>(this)->map_min_x)) jo::self<jo::CampScreen>(this)->map_min_x = a;
            if (!(b >= jo::self<jo::CampScreen>(this)->map_min_y)) jo::self<jo::CampScreen>(this)->map_min_y = b;
            if (c > jo::self<jo::CampScreen>(this)->map_max_x) jo::self<jo::CampScreen>(this)->map_max_x = c;
            if (e > jo::self<jo::CampScreen>(this)->map_max_y) jo::self<jo::CampScreen>(this)->map_max_y = e;
        } else {
            jo::self<jo::CampScreen>(this)->map_min_x = a;
            jo::self<jo::CampScreen>(this)->map_min_y = b;
            jo::self<jo::CampScreen>(this)->map_max_x = c;
            jo::self<jo::CampScreen>(this)->map_max_y = e;
        }
        tok(file);
        void* const m1 = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86; operator new
        jo::Actor* const model = m1 != nullptr ? joan::orig::arg::as<jo::Actor*>(joan::orig::as_scene_object(m1)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        void* const m2 = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86; operator new
        jo::ActorDef* const mesh = m2 != nullptr ? joan::orig::arg::as<jo::ActorDef*>(joan::orig::as_actor_def(m2)->construct()) : nullptr;  // 0x005af390
        model->def = mesh;
        joan::orig::as_actor_def(model->def)->load2(str(d::s_MODEL_9d3750), joan::orig::arg::str(static_cast<char*>(buf)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), nullptr, nullptr);  // 0x005af3d0 "MODEL\"
        model->anim = nullptr;
        joan::orig::as_scene_object(model)->init();     // 0x005aab70
        joan::orig::as_actor(model)->set_position_5ac310(joan::orig::arg::mptr(v));  // 0x005ac310
        float sc = scale;
        u32 sbits;
        std::memcpy(&sbits, &sc, 4);
        jo::view<u32>(model->scale) = sbits;
        const float k = global<float>(d::flt_0_017453292);
        joan::orig::as_scene_object(model)->set_yaw(joan::orig::arg::bits(angle * k));  // 0x005ac6a0
        joan::orig::as_scene(jo::self<jo::CampScreen>(this)->map_scene)->add_actor(joan::orig::arg::mptr(model));  // 0x005ea1f0
        return 0;  // 0x005ea1f0 returns nothing; the caller (parse_map_731a60) ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("Scene::parse_light", 0x00731b00, &H::parse_light);
B("Scene::parse_model", 0x00731ff0, &H::parse_model);
#undef B

}  // namespace game_b104_15
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Screen::nop_730ba0() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x730ba0
auto joan::orig::Screen::nop_730bc0() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x730bc0
auto joan::orig::ObjMenu::spawn_hero_models() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b115_10::H::spawn_heroes>(this)); }  // 0x730be0
auto joan::orig::Screen::purge_items() -> void { joan::bind::member<&::game_b92_06::H::purge_items>(this); }  // 0x731930
auto joan::orig::ObjMenu::parse_map_script(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_20::H::parse_map_731a60>(this, a0)); }  // 0x731a60
auto joan::orig::Scene::parse_light(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15::H::parse_light>(this, a0)); }  // 0x731b00
auto joan::orig::Scene::parse_model(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15::H::parse_model>(this, a0)); }  // 0x731ff0
