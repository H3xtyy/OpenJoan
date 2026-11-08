// game/dyngrass.cpp: functions of the original d:\ww\dyngrass.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../interfac/view_range.hpp"
#include "../render/grass_batch.hpp"
#include "../render/v3d.hpp"
#include "../en3d/scene_lists.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


using joan::call::thiscall;

using joan::hook::x87::Fpu;
using i8 = std::int8_t;

using joan::call::cdecl_call;
using joan::call::import_call;

using u16 = std::uint16_t;

u32 addr(const void* p) { return joan::call::pointer_low_dword(p); }  // the low 32 bits: the x86 frame slot (high_half() keeps the rest, vp() rejoins them)

// DynGrass::build_mesh (0x00466560) ends every blade with Plane::invalidate(-1) on the patch's plane (dirty flags,
// two stream cache notices, the per-material lists freed, load state 1) and the grass batching's forget. While
// DynGrass::populate (0x00465a20) fills one patch, nothing between two of its blades reads any of that - the next
// blade only adds vertices / triangles / materials - so populate has it done once, for the last blade, when the
// patch is complete: the same plane state as after the original's last call (the flags are or-ed, the frees find
// nothing the second time, the notices name the arrays as they end up). ~217,000 calls in one frame at a cutscene's
// camera jump.
struct GrassInvalidate {
    bool deferred = false;  // inside populate's blade loop
    void* plane = nullptr;  // a plane build_mesh left to invalidate
};
GrassInvalidate g_grass_invalidate;

void invalidate_grass_plane(void* plane) {
    joan::orig::as_plane(plane)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
    joan::render::grass_forget(plane);  // [Graphics] GrassBatching: the mesh changed
}

// build_mesh's end: now, or when populate's blade loop is done.
void grass_plane_changed(void* plane) {
    GrassInvalidate& g = g_grass_invalidate;
    if (!g.deferred) {
        invalidate_grass_plane(plane);
        return;
    }
    if (g.plane != nullptr && g.plane != plane) invalidate_grass_plane(g.plane);  // (one patch, one plane: not seen)
    g.plane = plane;
}

void begin_grass_blades() { g_grass_invalidate.deferred = true; }
void end_grass_blades() {
    GrassInvalidate& g = g_grass_invalidate;
    g.deferred = false;
    if (g.plane != nullptr) invalidate_grass_plane(g.plane);
    g.plane = nullptr;
}

}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
//
//      0x004641a0 DynGrass::init(n, b)            (thiscall, ret 8)
//
//  - 0x004641a0: fild -> fstp float = (float)int; the rest are float-operand x87 ops (PC24)
//    done as separate C++ float statements, then _ftol.
namespace {
namespace game_b100_02 {

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_grass_init, "DynGrass::init");

struct H {

    // ------------------------------------------------------------------ 0x004641a0
    void* grass_init(i32 n, i32 b) {
        c_grass_init.hit();
        const i32 m = n > 0x64 ? n : 0x64;
        jo::self<jo::DynGrass>(this)->density = static_cast<float>(m);
        jo::self<jo::DynGrass>(this)->enabled = static_cast<u8>(b);
        const float k = global<float>(d::flt_64);  // 64.0
        const float c = global<float>(d::flt_2);  // 2.0
        float s0 = jo::self<jo::DynGrass>(this)->density / k;
        s0 = s0 + s0;
        s0 = s0 + c;
        float s1 = jo::self<jo::DynGrass>(this)->density / k;
        s1 = s1 + s1;
        s1 = s1 + c;
        const float p = s1 * s0;
        const i32 cnt = joan::hook::x87::ftol(static_cast<double>(p));
        jo::view<void*>(jo::self<jo::DynGrass>(this)->patches) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(cnt) * static_cast<u32>(sizeof(jo::GrassPatch)))), str(d::s_d_ww_dyngrass_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fd894)) + 8));  // 0x0058e7b0
        std::memset(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches), 0, static_cast<u32>(cnt) * static_cast<u32>(sizeof(jo::GrassPatch)));
        jo::view<void*>(jo::self<jo::DynGrass>(this)->patches2) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(cnt) * static_cast<u32>(sizeof(jo::GrassPatch)))), str(d::s_d_ww_dyngrass_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fd894)) + 0xb));
        std::memset(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches2), 0, static_cast<u32>(cnt) * static_cast<u32>(sizeof(jo::GrassPatch)));
        jo::self<jo::DynGrass>(this)->res_count = 0;
        float tmp[3];
        const float* const v = joan::orig::arg::as<const float*>(joan::orig::vec_scale(static_cast<void*>(tmp), joan::orig::arg::ptr(obj(d::g_a18a40)), 10.0f));  // 0x0060cd00
        u32 w[3];
        std::memcpy(w, v, sizeof(w));
        std::memcpy(jo::self<jo::DynGrass>(this)->wind, w, sizeof(w));
        std::memcpy(w, obj(d::g_pos), sizeof(w));
        std::memcpy(jo::self<jo::DynGrass>(this)->pos, w, sizeof(w));
        std::memcpy(w, obj(d::g_pos), sizeof(w));
        std::memcpy(jo::self<jo::DynGrass>(this)->last_pos, w, sizeof(w));
        jo::view<i32>(jo::self<jo::DynGrass>(this)->f32_1cd) = 0;
        return this;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("DynGrass::init", 0x004641a0, &H::grass_init);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
//
//    - DynGrass::release 0x00464390 (dyngrass.cpp).
namespace {
namespace game_b84_05 {

void* const kRes = obj(d::g_item_res);
void* const kMem = obj(d::g_mem);

void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

JOAN_COUNTER(c_grass_release, "DynGrass::release");

struct H {
    // ---- 0x00464390 DynGrass::release (dyngrass.cpp) ------------------------------------------
    void grass_release() {
        c_grass_release.hit();
        joan::orig::as_grass(this)->set_enabled(static_cast<char>(joan::orig::arg::i32(0)));  // 0x00464600
        if (jo::view<void*>(jo::self<jo::DynGrass>(this)->patches) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches), str(d::s_d_ww_dyngrass_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_9fd898)) + 5);
            jo::view<void*>(jo::self<jo::DynGrass>(this)->patches) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::DynGrass>(this)->patches2) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches2), str(d::s_d_ww_dyngrass_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_9fd898)) + 0xb);
            jo::view<void*>(jo::self<jo::DynGrass>(this)->patches2) = nullptr;
        }
        for (std::int32_t i = 0; i < jo::self<jo::DynGrass>(this)->res_count; ++i) {
            const std::int32_t id = jo::self<jo::DynGrass>(this)->res_ids[i];
            if (id == 0) continue;
            void* const res = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kRes)->get(joan::orig::arg::i16(static_cast<std::int16_t>(id))));  // 0x00425ce0
            joan::orig::as_res(res)->release_slot(0, 0);                                                   // 0x004a3e30
        }
        jo::view<std::uint32_t>(jo::self<jo::DynGrass>(this)->patch_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::DynGrass>(this)->patch2_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::DynGrass>(this)->res_count) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("DynGrass::release", 0x00464390, &H::grass_release);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
//
//      0x004644d0 DynGrass::reserve(int n)                 (thiscall, ret 4)
//
//  - 0x004644d0: fild n vs float compare done in double (exact); (f/64*2+2)^2 in float ops.
namespace {
namespace game_b97_01 {

JOAN_COUNTER(c_grass_reserve, "DynGrass::reserve");

struct H {
    // ------------------------------------------------------------------ 0x004644d0
    void reserve(i32 n) {
        c_grass_reserve.hit();
        const i32 v = n > 0x64 ? n : 0x64;
        if (!(static_cast<double>(n) >= static_cast<double>(jo::self<jo::DynGrass>(this)->density))) {  // n < f or unordered
            jo::self<jo::DynGrass>(this)->density = static_cast<float>(v);
            return;
        }
        jo::self<jo::DynGrass>(this)->density = static_cast<float>(v);
        const float k = global<float>(d::flt_64);  // 64.0f
        const float two = global<float>(d::flt_2);  // 2.0f
        float a = jo::self<jo::DynGrass>(this)->density / k;
        a = a + a;
        a = a + two;
        float b = jo::self<jo::DynGrass>(this)->density / k;
        b = b + b;
        b = b + two;
        const i32 cnt = joan::hook::x87::ftol(static_cast<double>(b * a));
        void* const mem = obj(d::g_mem);
        const char* const file = str(d::s_d_ww_dyngrass_cpp);  // "d:\\ww\\dyngrass.cpp"
        jo::view<void*>(jo::self<jo::DynGrass>(this)->patches) = joan::orig::as_mem(mem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches)), joan::orig::arg::u32(static_cast<u32>(cnt) * static_cast<u32>(sizeof(jo::GrassPatch))), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fd89c)) + 0xd));  // 0x0058eb30
        jo::view<void*>(jo::self<jo::DynGrass>(this)->patches2) = joan::orig::as_mem(mem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches2)), joan::orig::arg::u32(static_cast<u32>(cnt) * static_cast<u32>(sizeof(jo::GrassPatch))), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fd89c)) + 0xe));  // 0x0058eb30
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("DynGrass::reserve", 0x004644d0, &H::reserve);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
//
//    - Grass::set_enabled 0x00464600 (0xa45d30; off: removes every grass patch from the
//      scene and deletes it) - "gate input" in the camera manager notes.
namespace {
namespace game_world4 {

JOAN_COUNTER(c_grass, "Grass::set_enabled");

struct H {
    // 0x00464600 Grass::set_enabled(on): +4 flag; off removes the patches (+5 array of 0x66
    // bytes, count +9: +0x24 scene, +0x28 object) and empties the list.
    void grass_set_enabled(char on) {
        if (static_cast<std::int32_t>(on) != 0) {
            jo::view<char>(jo::self<jo::DynGrass>(this)->enabled) = on;
            return;
        }
        c_grass.hit();
        jo::view<char>(jo::self<jo::DynGrass>(this)->enabled) = on;
        for (std::int32_t i = 0; i < jo::self<jo::DynGrass>(this)->patch_count; ++i) {
            if (jo::self<jo::DynGrass>(this)->patches[i].object == nullptr) continue;
            if (jo::view<void*>(jo::self<jo::DynGrass>(this)->patches[i].scene) != nullptr)
                joan::orig::as_scene(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches[i].scene))->detach(joan::orig::arg::mptr(jo::self<jo::DynGrass>(this)->patches[i].object));  // 0x005efdb0
            void* const obj = jo::self<jo::DynGrass>(this)->patches[i].object;
            if (obj != nullptr) joan::render::grass_forget(jo::view<void*>(jo::as_scene_object(obj)->plane));  // [Graphics] GrassBatching
            if (obj != nullptr) joan::orig::as_scene_object(obj)->destroy(1u);  // 0x00420840 delete
            jo::self<jo::DynGrass>(this)->patches[i].object = nullptr;
        }
        jo::self<jo::DynGrass>(this)->patch_count = 0;
    }
};

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("Grass::set_enabled", 0x00464600, &H::grass_set_enabled);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- from modules/game_b109_16.cpp (game/b109_16)
//
//    - DynGrass::update_464740 0x00464740 (ret 0, dyngrass.cpp; called from 0x004e5fc0 only, eax
//      discarded): camera-centred 3x3-cell patch grid rebuild (0x00465930 lookup, record copy /
//      memmove, new records with 0x00429090 height and 0x005ef610 region search, 0x00465a20),
//      release of dropped records (0x005efdb0 / 0x00420840), buffer swap, wind vector update
//      (+0x1a9 / +0x1b5 / +0x1c1 / +0x1cd gust timer, random 0x005906f0 / 0x00590540, CRT
//      cos / sin) and per-record distance fade (0x0042cf20, 0x005d6e80, colour 0x005d5e80).
//
//  - 0x00464740: int -> float (fild) and the u32 wind clock (fild qword) go through x87::Fpu; the
//    two CRT floor / ceil calls (0x007b32bb / 0x007b31ec) are called directly as in the original
//    (results are integers); cos / sin results are multiplied by the float amplitude under PC24
//    by crt_trig_fmul (24-bit significand, x87 exponent range); sqrt result stored straight to
//    float. Every x87 compare keeps the original NaN direction. The rep-movs record copy (0x66 bytes) is a memmove between two
//    0x66-aligned slots of different buffers.
namespace {
namespace game_b109_16 {

using joan::orig::helpers::at;
using joan::orig::helpers::imul;
i32 shl6(i32 a) { return static_cast<i32>(static_cast<u32>(a) << 6); }
using joan::orig::helpers::fbits;
using joan::orig::helpers::gf;  // float constant in .rdata
using joan::orig::helpers::gd;  // double constant in .rdata

const Fpu fpu{true};

// x87 arithmetic at the game's precision control (PC24) with the x87 exponent range: src/fp/x87.hpp.
using joan::fp::pc24_mul_m32;

// CRT cos / sin (0x007b24b4 / 0x007b2404, double arg): the original multiplies their st0 by a float under the
// game's PC24 (`fmul dword`) and stores it with fstp dword: the CRT result as the double its slot call gives,
// times the float rounded to 24 bits with the x87 exponent range (pc24_mul_m32), stored as a float (the cast).
float crt_trig_fmul(double st0, const float* m) { return static_cast<float>(pc24_mul_m32(st0, m)); }

// cdecl vector helpers (thunks); each returns its out pointer.
u8* vec_scale(void* out, const void* v, float s) { return joan::orig::arg::as<u8*>(joan::orig::vec_scale(joan::orig::arg::mptr(out), joan::orig::arg::ptr(v), joan::orig::arg::f32(s))); }  // 0x0060cd00
void vec_add_to(void* a, const void* b) { joan::orig::vec_add_in_place(joan::orig::arg::mptr(a), joan::orig::arg::ptr(b)); }                    // 0x0060cdc0

JOAN_COUNTER(c_grass, "DynGrass::update_464740");

struct H {
    // ---- 0x00464740 ---------------------------------------------------------------------------
    // One new grass record (the 0x004649ee branch): height, region and material flags.
    void grass_new(u8* rec, i32 x, i32 y, float zlo, float zhi) {
        std::memset(static_cast<void*>(rec), i32{0}, sizeof(jo::GrassPatch));  // memset
        jo::as_grass_patch(rec)->grid_x = static_cast<i16>(x);
        jo::as_grass_patch(rec)->grid_y = static_cast<i16>(y);
        jo::as_grass_patch(rec)->box[0] = static_cast<float>(shl6(x));
        jo::as_grass_patch(rec)->box[1] = static_cast<float>(shl6(y));
        jo::as_grass_patch(rec)->box[3] = static_cast<float>(shl6(x));
        jo::as_grass_patch(rec)->box[4] = static_cast<float>(shl6(y));
        jo::as_grass_patch(rec)->box[2] = zlo;
        jo::as_grass_patch(rec)->box[5] = zhi;
        i32 hits = 0;
        u8* h = joan::orig::arg::as<u8*>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(joan::orig::arg::mptr(jo::as_grass_patch(rec)->box), &hits));  // 0x00429090
        if (hits != 0) jo::as_grass_patch(rec)->owner = reinterpret_cast<void* const*>(h)[0];  // the first cell of the list
        jo::as_grass_patch(rec)->box[3] = static_cast<float>(static_cast<i32>(static_cast<u32>(shl6(x)) + 0xc0u));
        jo::as_grass_patch(rec)->box[4] = static_cast<float>(static_cast<i32>(static_cast<u32>(shl6(y)) + 0xc0u));
        i32 count = 0;
        float box[6];
        box[0] = (jo::as_grass_patch(rec)->box[0] + jo::as_grass_patch(rec)->box[3]) / gf(d::flt_2);
        box[3] = box[0];
        box[1] = (jo::as_grass_patch(rec)->box[1] + jo::as_grass_patch(rec)->box[4]) / gf(d::flt_2);
        box[4] = box[1];
        box[2] = jo::as_grass_patch(rec)->box[2];
        box[5] = jo::as_grass_patch(rec)->box[5];
        u8** list = joan::orig::arg::as<u8**>(joan::orig::as_scene(jo::view<u8*>(jo::as_world(global<u8*>(d::g_world))->scene))->nodes_in_box(joan::orig::arg::mptr(box), &count));  // 0x005ef610
        if (count != 0) {
            jo::view<u8*>(jo::as_grass_patch(rec)->scene) = list[0];
            for (i32 j = 0; j < count; ++j) {
                u8* e = list[j];
                if (jo::view<float>(jo::as_scene_s(e)->desc[0]) > jo::as_grass_patch(rec)->box[0]) continue;
                if (jo::as_grass_patch(rec)->box[0] > jo::view<float>(jo::as_scene_s(e)->desc[3])) continue;
                if (jo::view<float>(jo::as_scene_s(e)->desc[1]) > jo::as_grass_patch(rec)->box[1]) continue;
                if (jo::as_grass_patch(rec)->box[1] > jo::view<float>(jo::as_scene_s(e)->desc[4])) continue;
                if (jo::view<float>(jo::as_scene_s(e)->desc[0]) > jo::as_grass_patch(rec)->box[3]) continue;
                if (jo::as_grass_patch(rec)->box[3] > jo::view<float>(jo::as_scene_s(e)->desc[3])) continue;
                if (jo::view<float>(jo::as_scene_s(e)->desc[1]) > jo::as_grass_patch(rec)->box[4]) continue;
                if (jo::as_grass_patch(rec)->box[4] > jo::view<float>(jo::as_scene_s(e)->desc[4])) continue;
                jo::view<u8*>(jo::as_grass_patch(rec)->scene) = e;
                break;
            }
        }
        joan::orig::as_grass(this)->populate(joan::orig::arg::mptr(rec));  // 0x00465a20
        if (jo::as_grass_patch(rec)->object != nullptr) {
            u8* const mesh = jo::view<u8*>(jo::as_scene_object(jo::as_grass_patch(rec)->object)->plane);
            for (i32 k = 0; k < jo::as_plane(mesh)->material_count; ++k) {
                u8* const face = jo::view<u8*>(jo::as_plane(mesh)->materials) + imul(k, 0x54);
                jo::as_material(face)->raster &= 0xfffff0ffu;
            }
        }
    }
    // floor / ceil steps of the cell range: f2(f1(v / 64) / 3.0) * 3 (0x007b32bb / 0x007b31ec).
    static i32 grass_cell(float v, std::uintptr_t f1, std::uintptr_t f2) {
        // fld v; fdiv dword 64.0 (exact: power of two, x87 exponent range); fstp qword
        const double a = cdecl_call<double>(f1, static_cast<double>(v) / static_cast<double>(gf(d::flt_64)));
        const double b = cdecl_call<double>(f2, fpu.div(a, gd(d::dbl_3)));
        return imul(joan::hook::x87::ftol(b), 3);
    }
    void grass_update() {  // 0x00464740
        joan::hitch::Span hitch_span("grass_update");  // [Debug] HitchLog
        if (jo::view<i8>(jo::self<jo::DynGrass>(this)->enabled) == 0) return;
        if (global<void*>(d::g_camera) == nullptr) return;
        c_grass.hit();
        jo::self<jo::DynGrass>(this)->patch2_count = 0;
        jo::CamPose cam;
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(&cam)));  // 0x006140a0
        const float cx = cam.eye[0], cy = cam.eye[1], cz = cam.eye[2];
        const float r = jo::self<jo::DynGrass>(this)->density;
        const float x0 = cx - r, y0 = cy - r, zlo = cz - gf(d::flt_5000);
        const float x1 = cx + r, y1 = cy + r, zhi = cz + gf(d::flt_5000);
        const i32 gx0 = grass_cell(x0, fn::code_value(fn::crt_floor), fn::code_value(fn::crt_ceil));
        const i32 gy0 = grass_cell(y0, fn::code_value(fn::crt_floor), fn::code_value(fn::crt_ceil));
        const i32 gx1 = grass_cell(x1, fn::code_value(fn::crt_floor), fn::code_value(fn::crt_floor));
        const i32 gy1 = grass_cell(y1, fn::code_value(fn::crt_floor), fn::code_value(fn::crt_floor));
        // New patches per frame while others exist: the original's 1 refilled the ~500 cells of its 2000 grass distance
        // in seconds, but the ~7000 of the widened 8000 (interfac/view_range.hpp) in minutes - after the level-start
        // camera cut, the grass around the player stayed missing. Scaled with the area (1 at 2000, 16 at 8000), and the
        // nearest missing cells come first (below).
        const float area = jo::self<jo::DynGrass>(this)->density / static_cast<float>(joan::optmenu::kGrassMin);
        i32 budget = area * area > 1.0f ? static_cast<i32>(area * area) : 1;
        if (jo::self<jo::DynGrass>(this)->patch_count == 0 || jo::self<jo::DynGrass>(this)->patch_count <= budget) {
            double t = fpu.div(jo::self<jo::DynGrass>(this)->density, gf(d::flt_192));
            t = fpu.add(t, t);
            t = fpu.add(t, gf(d::flt_2));
            budget = joan::hook::x87::ftol(fpu.mul(t, t));
        }
        // Cell -> 1-based record index of the patches not yet carried over. The original looks every cell up with the
        // linear find_record scan (0x00465930), quadratic in the patch count: harmless at the original 2000 grass
        // distance (~500 patches), ~25M compares a frame at the widened 8000 (interfac/view_range.hpp). Same result:
        // a cell holds at most one record, and the index follows the swap-with-last removal below. The scanned cells
        // are a grid of every 3rd cell in the window, so the index is a flat array over that grid (a record outside
        // it is never looked up).
        const i32 cols = gx1 >= gx0 ? (gx1 - gx0) / 3 + 1 : 0, rows = gy1 >= gy0 ? (gy1 - gy0) / 3 + 1 : 0;
        static std::vector<i32> by_cell;
        by_cell.assign(static_cast<std::size_t>(cols) * static_cast<std::size_t>(rows), 0);
        auto slot = [&](i32 x, i32 y) -> i32* {
            const i32 dx = x - gx0, dy = y - gy0;
            if (dx < 0 || dy < 0 || dx % 3 != 0 || dy % 3 != 0 || dx / 3 >= cols || dy / 3 >= rows) return nullptr;
            return &by_cell[static_cast<std::size_t>(dx / 3) * static_cast<std::size_t>(rows) + static_cast<std::size_t>(dy / 3)];
        };
        struct Cell {
            i32 x, y;
        };
        static std::vector<Cell> missing;  // cells without a record, in scan order
        missing.clear();
        for (i32 i = 0; i < jo::self<jo::DynGrass>(this)->patch_count; ++i)
            if (i32* s = slot(jo::self<jo::DynGrass>(this)->patches[i].grid_x, jo::self<jo::DynGrass>(this)->patches[i].grid_y); s && *s == 0)
                *s = i + 1;  // the first record of a cell, as the linear scan finds
        for (i32 x = gx0; x <= gx1; x += 3) {
            for (i32 y = gy0; y <= gy1; y += 3) {
                i32* const s = slot(x, y);
                const i32 idx = *s;
                if (idx != 0) {
                    *s = 0;
                    if (idx != jo::self<jo::DynGrass>(this)->patch_count) {
                        const jo::GrassPatch& last = jo::self<jo::DynGrass>(this)->patches[jo::self<jo::DynGrass>(this)->patch_count - 1];
                        if (i32* ls = slot(last.grid_x, last.grid_y); ls && *ls == jo::self<jo::DynGrass>(this)->patch_count) *ls = idx;
                    }
                    u8* const src = reinterpret_cast<u8*>(jo::self<jo::DynGrass>(this)->patches + (idx - 1));
                    u8* const dst = reinterpret_cast<u8*>(jo::self<jo::DynGrass>(this)->patches2 + (jo::self<jo::DynGrass>(this)->patch2_count));
                    std::memmove(dst, src, sizeof(jo::GrassPatch));
                    jo::self<jo::DynGrass>(this)->patch2_count = jo::self<jo::DynGrass>(this)->patch2_count + 1;
                    if (idx != jo::self<jo::DynGrass>(this)->patch_count) {
                        std::memcpy(static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::DynGrass>(this)->patches + (idx - 1))), static_cast<void*>(reinterpret_cast<u8*>(jo::self<jo::DynGrass>(this)->patches + (jo::self<jo::DynGrass>(this)->patch_count - 1))), u32{sizeof(jo::GrassPatch)});
                    }
                    jo::self<jo::DynGrass>(this)->patch_count = jo::self<jo::DynGrass>(this)->patch_count - 1;
                } else {
                    missing.push_back({x, y});
                }
            }
        }
        // The original made the first `budget` missing cells of the scan new records; here the nearest ones (the same
        // cells, in scan order, whenever the budget covers them all).
        if (static_cast<i32>(missing.size()) > budget) {
            auto dist2 = [cx, cy](const Cell& c) {
                const float dx = static_cast<float>(shl6(c.x) + 0x60) - cx, dy = static_cast<float>(shl6(c.y) + 0x60) - cy;
                return dx * dx + dy * dy;
            };
            std::partial_sort(missing.begin(), missing.begin() + budget, missing.end(),
                              [&](const Cell& a, const Cell& b) { return dist2(a) < dist2(b); });
            missing.resize(static_cast<std::size_t>(budget));
        }
        for (const Cell& c : missing) {
            u8* const rec = reinterpret_cast<u8*>(jo::self<jo::DynGrass>(this)->patches2 + (jo::self<jo::DynGrass>(this)->patch2_count));
            jo::self<jo::DynGrass>(this)->patch2_count = jo::self<jo::DynGrass>(this)->patch2_count + 1;
            grass_new(rec, c.x, c.y, zlo, zhi);
        }
        for (i32 i = 0; i < jo::self<jo::DynGrass>(this)->patch_count; ++i) {
            u8* const rec = reinterpret_cast<u8*>(jo::self<jo::DynGrass>(this)->patches + (i));
            if (jo::as_grass_patch(rec)->object == nullptr) continue;
            if (jo::as_grass_patch(rec)->scene != nullptr) joan::orig::as_scene(jo::view<u8*>(jo::as_grass_patch(rec)->scene))->detach(joan::orig::arg::mptr(jo::as_grass_patch(rec)->object));  // 0x005efdb0
            u8* const g = jo::view<u8*>(jo::self<jo::DynGrass>(this)->patches[i].object);
            if (g != nullptr) joan::render::grass_forget(jo::view<void*>(jo::as_scene_object(g)->plane));  // [Graphics] GrassBatching
            if (g != nullptr) joan::orig::as_scene_object(g)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00420840
            jo::self<jo::DynGrass>(this)->patches[i].object = nullptr;
        }
        jo::GrassPatch* const swap = jo::self<jo::DynGrass>(this)->patches;
        jo::self<jo::DynGrass>(this)->patches = jo::self<jo::DynGrass>(this)->patches2;
        jo::self<jo::DynGrass>(this)->patches2 = swap;
        jo::self<jo::DynGrass>(this)->patch_count = jo::self<jo::DynGrass>(this)->patch2_count;
        jo::self<jo::DynGrass>(this)->patch2_count = 0;

        // wind
        float dt = static_cast<float>(fpu.mul(static_cast<double>(global<u32>(d::g_frame_step_ms)), gf(d::flt_0_001)));
        if (dt > gf(d::flt_0_05)) dt = fbits(0x3d4ccccd);
        u8 tmp[12];
        u32 v[3];
        std::memcpy(v, vec_scale(tmp, jo::self<jo::DynGrass>(this)->pos, dt), 12);
        vec_add_to(jo::self<jo::DynGrass>(this)->wind, v);
        if (import_call<double>(d::iat_v3d_87_length_sq, static_cast<void*>(jo::self<jo::DynGrass>(this)->wind)) > gf(d::flt_100)) {  // length_sq
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, static_cast<void*>(jo::self<jo::DynGrass>(this)->wind));                    // normalize
            joan::orig::vec_scale_in_place(static_cast<void*>(jo::self<jo::DynGrass>(this)->wind), joan::orig::arg::f32(fbits(0x41200000)));   // 0x0060ce80
        }
        u32 wind[3];
        std::memcpy(wind, jo::self<jo::DynGrass>(this)->wind, 12);
        u32 a138[3], a150[3], push[3];
        u8 t144[12], t15c[12], t168[12];
        std::memcpy(a138, vec_scale(t144, jo::self<jo::DynGrass>(this)->pos, fbits(0xbe19999a)), 12);
        std::memcpy(a150, vec_scale(t15c, jo::self<jo::DynGrass>(this)->wind, fbits(0xc08c28f6)), 12);
        std::memcpy(push, joan::orig::arg::as<u8*>(joan::orig::vec_add(static_cast<void*>(t168), static_cast<void*>(a150), static_cast<void*>(a138))),
                    12);  // 0x0060cc20
        float& gust = jo::self<jo::DynGrass>(this)->f32_1cd;
        if (gust > gf(d::k_9cf5c4)) {
            float part;
            if (gust > dt) {
                part = dt;
                gust = gust - dt;
            } else {
                part = gust;
                gust = 0.0f;
            }
            u8 t180[12];
            u32 a174[3];
            std::memcpy(a174, vec_scale(t180, jo::self<jo::DynGrass>(this)->last_pos, part / dt), 12);
            vec_add_to(push, a174);
        } else if (!(gust >= gf(d::flt_m10))) {
            void* const rnd = obj(d::g_misc);
            const bool strong = joan::orig::as_misc(rnd)->random(i32{10}) < 3;  // 0x00590540
            float amp;
            if (strong)
                amp = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_misc(rnd)->random_float(joan::orig::arg::f32(fbits(0x41a00000)), joan::orig::arg::f32(fbits(0x41f00000)))));  // 0x005906f0
            else
                amp = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_misc(rnd)->random_float(joan::orig::arg::f32(fbits(0x41700000)), joan::orig::arg::f32(fbits(0x41c80000)))));
            const float ang = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_misc(rnd)->random_float(joan::orig::arg::f32(fbits(0)), joan::orig::arg::f32(fbits(0x40c90fdb)))));
            jo::self<jo::DynGrass>(this)->last_pos[0] = crt_trig_fmul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(ang)))), &amp);  // cos(ang) * amp
            jo::self<jo::DynGrass>(this)->last_pos[1] = crt_trig_fmul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(ang)))), &amp);  // sin(ang) * amp
            jo::view<u32>(jo::self<jo::DynGrass>(this)->last_pos[2]) = 0;
            if (strong)
                jo::self<jo::DynGrass>(this)->f32_1cd = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_misc(rnd)->random_float(joan::orig::arg::f32(fbits(0x40a00000)), joan::orig::arg::f32(fbits(0x41200000)))));
            else
                jo::self<jo::DynGrass>(this)->f32_1cd = static_cast<float>(joan::orig::arg::as<double>(joan::orig::as_misc(rnd)->random_float(joan::orig::arg::f32(fbits(0x41200000)), joan::orig::arg::f32(fbits(0x41700000)))));
        } else {
            const double rr = joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->random_float(joan::orig::arg::f32(fbits(0)), joan::orig::arg::f32(dt + gf(d::flt_0_001))));  // 0x005906f0
            jo::self<jo::DynGrass>(this)->f32_1cd = static_cast<float>(fpu.sub(jo::self<jo::DynGrass>(this)->f32_1cd, fpu.mul(rr, gf(d::flt_5))));
        }
        {
            u8 t198[12];
            u32 a18c[3];
            std::memcpy(a18c, vec_scale(t198, push, dt), 12);
            vec_add_to(jo::self<jo::DynGrass>(this)->pos, a18c);
        }

        // distance fade. Run for every patch (thousands at the widened grass distance), so the original's helpers are
        // done in place, with the same results: the CRT sqrt at PC24 stored to a float is the float square root
        // (a correctly rounded double root rounded to 24 bits is the correctly rounded 24-bit root: 53 >= 2 * 24 + 2);
        // Actor::set_10a 0x0042cf20 = visible, Plane::set_104 0x005d6e80 = flags_104, Plane::bone 0x005cf8f0 = bones + i;
        // Colour(0xff, 0xff, 0xff, alpha) 0x0042a260 -> from_rgba 0x005e95b0 (r <-> b) -> set_fade 0x005d5e80 = colour_.
        const float density = jo::self<jo::DynGrass>(this)->density;
        const float half = gf(d::flt_2), hundred = gf(d::flt_100), m128 = gf(d::flt_m128), c255 = gf(d::flt_255);
        const double fade_range = fpu.sub(density, hundred);
        for (i32 i = 0; i < jo::self<jo::DynGrass>(this)->patch_count; ++i) {
            jo::GrassPatch& rec = jo::self<jo::DynGrass>(this)->patches[i];
            if (rec.object == nullptr) continue;
            const float mx = (rec.box[0] + rec.box[3]) / half;
            const float my = (rec.box[1] + rec.box[4]) / half;
            const double ddx = fpu.sub(cx, mx), ddy = fpu.sub(cy, my);
            const double sum = fpu.add(fpu.mul(ddx, ddx), fpu.mul(ddy, ddy));
            const float dist = static_cast<float>(std::sqrt(sum));  // = the CRT sqrt at PC24 stored to a float
            u8* const mesh = jo::view<u8*>(jo::as_scene_object(rec.object)->plane);
            jo::Actor* const actor = jo::as_actor(jo::view<u8*>(rec.object));
            u8 alpha;
            if (dist > density) {
                // Hidden (the window's corners beyond the grass distance, ~1/5 of the patches): the original moves
                // its wind bone too, which nothing reads while it is hidden - the first frame it is in range again
                // sets it before it is drawn. (Visual only: grass patches take no part in the game.)
                actor->visible = 0;
                jo::as_plane(mesh)->flags_104 = 0;
                continue;
            }
            jo::Bone* const node = jo::as_plane(mesh)->bones + 1;
            std::memcpy(&node->local[12], wind, 12);  // the translation row
            joan::orig::as_bone(node)->invalidate();  // 0x0060ee10 (dirty, the owner's callback)
            if (!(dist > hundred)) {
                actor->visible = 1;
                jo::as_plane(mesh)->flags_104 = 0;
                alpha = 0xff;
            } else {
                actor->visible = 1;
                jo::as_plane(mesh)->flags_104 = 1;
                const double q = fpu.div(fpu.sub(dist, hundred), fade_range);
                alpha = static_cast<u8>(joan::hook::x87::ftol(fpu.add(fpu.mul(q, m128), c255)));
            }
            jo::view<u32>(jo::as_plane(mesh)->colour_) = static_cast<u32>(alpha) << 24 | 0x00ffffffu;
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_16", name, addr, fn)
B("DynGrass::update_464740", 0x00464740, &H::grass_update);
#undef B

}  // namespace game_b109_16
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
//
//    - Unit::recruit_nearby_4655a0 0x004655a0 (ret 0): converts idle peasants (word +0x31 ==
//      9999) of the same side within 300 units to this unit's group. Caller (0x0052cfee) ignores
//      eax.
namespace {
namespace game_b104_02 {

void* const kKinds = obj(d::g_types);

i32 word_arg(u16 w) { return static_cast<i32>(static_cast<i16>(w)); }

u8* kind_of(u16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(word_arg(w)))); }  // 0x0042d0a0
u32 unit_busy(void* u) { return joan::orig::arg::as<u32>(joan::orig::as_unit(u)->dead()); }               // 0x0042d0e0

// 0x004bb660 + 0x00584580 on the unit's +0x2a1 helper
void detach_helper(u8* u) {
    joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(u)));  // 0x004bb660
    joan::orig::dynarray(jo::as_unit(u)->orders)->zap(1);                   // 0x00584580
}

JOAN_COUNTER(c_recruit, "Unit::recruit_nearby_4655a0");

struct H {
    // ------------------------------------------------------------------ 0x004655a0
    bool below_limit() {
        const i32 have = static_cast<i8>(jo::view<u8>(jo::as_group(jo::self<jo::Unit>(this)->group)->count));
        return have < joan::orig::arg::as<i32>(joan::orig::as_unit(this)->morale_level());  // 0x00448080
    }
    void recruit_nearby_4655a0() {
        c_recruit.hit();
        if (static_cast<i8>(unit_busy(this)) != 0) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->leads()) == 0) return;  // 0x0044a660
        if (jo::view<u32>(jo::self<jo::Unit>(this)->markers) != 0) return;
        if (!below_limit()) return;
        const float rad = global<float>(d::flt_300);
        float box[6];  // [ebp-0x1c]
        box[0] = reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[0] - rad;
        box[1] = reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[1] - rad;
        box[2] = reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[2] - rad;
        box[3] = reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[0] + rad;
        box[4] = reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[1] + rad;
        box[5] = reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[2] + rad;
        i32 count = 0;  // [ebp-0x20] (0x005ef610 writes it first)
        u8** const list = joan::orig::arg::as<u8**>(joan::orig::as_scene(jo::view<void*>(jo::as_world(jo::self<jo::Unit>(this)->world)->scene))->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        // One actor of a sector's list; false: the group is full (the original returns).
        auto visit = [&](u8* node) -> bool {
                if (static_cast<void*>(node) != static_cast<void*>(jo::self<jo::Unit>(this)->actor) && jo::as_actor(node)->kind == 0xb) {
                    u8* const u = static_cast<u8*>(jo::as_actor(node)->owner);
                    bool take = true;
                    // The plain fields first (most units are in a group or on another side), then the calls - all
                    // pure tests, any one failing skips the unit, as in the original's order.
                    if (jo::view<u32>(jo::as_unit(u)->markers) != 0 || jo::as_unit(u)->side != jo::self<jo::Unit>(this)->side ||
                        jo::view<i16>(jo::as_unit(u)->team_flag) != 0 || jo::as_unit(u)->group_id != 0x270f) {
                        take = false;
                    } else if (static_cast<i8>(unit_busy(u)) != 0 ||
                               joan::orig::arg::as<i32>(joan::orig::as_unit(u)->leader()) != 0) {  // 0x004216d0
                        take = false;
                    } else {
                        const u8* const kind = kind_of(jo::view<u16>(jo::as_unit(u)->type_id));
                        const i32 cls = jo::as_unit_type(kind)->kind;
                        const i32 type = jo::as_unit_type(kind)->unit_class;
                        if (cls != 0 && cls != 1) {
                            take = false;
                        } else if (type == 0 || type == 8 || type == 9) {
                            take = false;
                        } else {
                            const float dx = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[0] - reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[0];
                            const float dy = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[1] - reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[1];
                            const float dz = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[2] - reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[2];
                            const float d = dx * dx + dy * dy + dz * dz;
                            if (d > global<float>(d::flt_9d3968)) take = false;
                        }
                    }
                    if (take) {
                        if (!below_limit()) return false;
                        joan::orig::as_unit(u)->link_to(joan::orig::arg::i32(word_arg(jo::view<u16>(jo::self<jo::Unit>(this)->id))));  // 0x00438f80
                        if (jo::as_unit(u)->orders != nullptr) detach_helper(u);
                        joan::orig::as_unit(u)->play_idle(0, 1);  // 0x00508b90
                        joan::orig::as_unit_task(&jo::as_unit(u)->task)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u, 0u, 0u, 0u);  // 0x004e4520
                    }
                }
                return true;
        };
        for (i32 i = 0; i < count; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_scene_s(list[i])->first_actor());  // 0x005effa0
            u8* node = first;
            if (node == nullptr) continue;
            // [Debug] UnitIndex (en3d/scene_lists.hpp): the sector's units in walk order - the other actors of the list
            // are skipped by the kind test anyway. When something edits a list while a unit is taken, the walk goes on
            // from that unit's neighbour as the original.
            if (const std::vector<u8*>* const units = joan::en3d::sector_units(list[i])) {
                const std::uint32_t edits = joan::en3d::g_unit_list_edits;
                bool walk_on = false;
                for (u8* const n : *units) {
                    if (!visit(n)) return;
                    if (joan::en3d::g_unit_list_edits != edits) {
                        node = jo::view<u8*>(jo::as_actor(n)->scene_prev);
                        walk_on = true;
                        break;
                    }
                }
                if (!walk_on) continue;
                while (node != nullptr && node != first) {
                    if (!visit(node)) return;
                    node = jo::view<u8*>(jo::as_actor(node)->scene_prev);
                }
                continue;
            }
            do {
                if (!visit(node)) return;
                node = jo::view<u8*>(jo::as_actor(node)->scene_prev);
            } while (node != nullptr && node != first);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Unit::recruit_nearby_4655a0", 0x004655a0, &H::recruit_nearby_4655a0);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
//
//      0x00465930 find_record(arr, n, a, b)     (thiscall, ret 0x10) - 1-based index or 0.
namespace {
namespace game_b96_01 {

struct H {
    // ------------------------------------------------------------------ 0x00465930
    i32 find_record(u8* arr, i32 n, i32 a, i32 b) {
        for (i32 i = 0; i < n; ++i) {
            if (static_cast<i32>(jo::as_grass_patch(arr)[i].grid_x) != a) continue;
            if (static_cast<i32>(jo::as_grass_patch(arr)[i].grid_y) != b) continue;
            return i + 1;
        }
        return 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("find_record", 0x00465930, &H::find_record);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x004659b0 Obj::find_slot(v)             (thiscall, ret 4) - index+1 in +0x15[] (count +0x1a5), 0.
namespace {
namespace game_b99_02 {

struct H {
    // ------------------------------------------------------------------ 0x004659b0
    i32 find_slot(i32 v) {
        for (i32 i = 0; i < jo::self<jo::DynGrass>(this)->res_count; ++i)
            if (jo::self<jo::DynGrass>(this)->res_ids[i] == v) return i + 1;
        return 0;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::find_slot_4659b0", 0x004659b0, &H::find_slot);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
//
//    - DynGrass::populate_465a20 0x00465a20 (Cell* p, ret 4): the code formerly kept under #if 0 in
//      game_b107_15.cpp, re-verified, with the 256-int histogram emulated inside a copy of the
//      original frame (see below).
//
//  - 0x00465a20: the histogram index is a map cell value (any short) and the max search runs to
//    map+0x80; the original writes / reads outside the 256 ints. The frame [ebp-0x544, ebp-4] and
//    the argument slot [ebp+8] are emulated byte for byte (an index of -1 increments the loop
//    counter, 256 the cell count, 258 the map pointer, ... exactly as in the original). Writes
//    to the saved registers, saved ebp / return address and the caller's frame are dropped;
//    reads of those (max search with map+0x80 > 263) return 0.
namespace {
namespace game_b115_10 {

using joan::hook::x87::ftol;
using i64 = std::int64_t;

// x64: the emulated x86 frame has 4-byte slots; a pointer kept in one keeps its upper half outside the frame
// (0 on x86, so the slot alone is the pointer there).
std::uintptr_t high_half(const void* p) { return reinterpret_cast<std::uintptr_t>(p) & ~static_cast<std::uintptr_t>(0xffffffffu); }
void* vp(u32 a, std::uintptr_t hi) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(a) | hi); }
// One GrassType::entries record (0x00465a20 / its placement loop): item id, count %, chance %, yaw, tilt.
#pragma pack(push, 1)
struct GrassEntry {
    i16 id;       // +0
    i16 density;  // +2
    i16 chance;   // +4
    float yaw;    // +6
    float tilt;   // +0xa
};
#pragma pack(pop)
static_assert(sizeof(GrassEntry) == 0xe, "GrassEntry");
const GrassEntry* GE(const void* e) { return static_cast<const GrassEntry*>(e); }
using joan::orig::helpers::fild;
using joan::orig::helpers::fd;
float bits_f(u32 b) {
    float f = 0.0f;
    std::memcpy(&f, &b, 4);
    return f;
}

constexpr Fpu kFpu{true};
constexpr fn::Code kMemset = fn::crt_memset;  // CRT memset

JOAN_COUNTER(c_grass, "DynGrass::populate_465a20");

// =========================================================================================
// 0x00465a20 helpers: the original frame [ebp-0x544, ebp+0xc) as bytes.
// =========================================================================================
struct GrassFrame {
    static constexpr i32 kLow = -0x544;  // lowest local
    static constexpr i32 kHigh = 0xc;    // end of the argument slot [ebp+8]
    u8 b[kHigh - kLow];

    static bool writable(i64 off) { return (off >= kLow && off <= -4) || off == 8; }
    u32 rd(i32 off) const {
        u32 v = 0;
        std::memcpy(&v, &b[off - kLow], 4);
        return v;
    }
    void wr(i32 off, u32 v) { std::memcpy(&b[off - kLow], &v, 4); }
    i32 ri(i32 off) const { return static_cast<i32>(rd(off)); }
    void wi(i32 off, i32 v) { wr(off, static_cast<u32>(v)); }
    // hist[v] = [ebp - 0x41c + 4 v]
    static i64 hoff(i32 v) { return -0x41c + 4 * static_cast<i64>(v); }
    u32 hist_rd(i32 v) const {
        const i64 off = hoff(v);
        return writable(off) ? rd(static_cast<i32>(off)) : 0u;  // fix: outside the frame -> 0
    }
    void hist_wr(i32 v, u32 val) {
        const i64 off = hoff(v);
        if (writable(off)) wr(static_cast<i32>(off), val);  // fix: saved regs / caller frame: dropped
    }
};

i32 rnd(i32 n) { return joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(n)); }  // 0x00590540
// 0x0060bf40 matrix_rotation_ypr, remembered by its exact arguments (and the x87 precision it ran at). The angles
// of grass come from small random ranges (deg(rnd(360)), deg(rnd(5) - 10) + tilt, deg(rnd(10) - 20) + yaw), so a
// patch grid asks for the same few hundred rotations again and again, each an exact sin / cos emulation - about half
// of a populate when a cutscene's camera jumps to new ground. Same arguments, same function: the same 16 floats.
struct RotKey {
    u32 a, b, c;
    joan::fp::Precision precision;
    bool operator==(const RotKey& o) const { return a == o.a && b == o.b && c == o.c && precision == o.precision; }
};
struct RotKeyHash {
    std::size_t operator()(const RotKey& k) const {
        std::uint64_t h = 0xcbf29ce484222325ull;
        for (const u32 v : {k.a, k.b, k.c, static_cast<u32>(k.precision)}) h = (h ^ v) * 0x100000001b3ull;
        return static_cast<std::size_t>(h);
    }
};
struct RotValue {
    float m[16];
};
void rot_y(float* m, float a, float b, float c) {
    static std::unordered_map<RotKey, RotValue, RotKeyHash> memo;
    const RotKey key{joan::fp::fbits(a), joan::fp::fbits(b), joan::fp::fbits(c), joan::fp::current_precision()};
    if (const auto it = memo.find(key); it != memo.end()) {
        std::memcpy(m, it->second.m, sizeof it->second.m);
        return;
    }
    joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(m), joan::orig::arg::bits(a), joan::orig::arg::bits(b), joan::orig::arg::bits(c));  // 0x0060bf40
    if (memo.size() >= 0x4000) memo.clear();  // (bounded; a few hundred in practice)
    RotValue v;
    std::memcpy(v.m, m, sizeof v.m);
    memo.emplace(key, v);
}
double crt_fabs(float v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(v)))); }
// fild int * float at PC24
float deg(i32 v) { return static_cast<float>(kFpu.mul(fild(v), fd(d::flt_0_017453292))); }

struct H {
    // ===================================================================== 0x00465a20
    void populate(void* p_arg) {
        joan::hitch::Span hitch_span("grass_populate");  // [Debug] HitchLog
        c_grass.hit();
        GrassFrame F;
        std::memset(F.b, 0, sizeof F.b);
        F.wr(-4, addr(this));
        F.wr(8, addr(p_arg));
        F.wr(-8, 0x3f800000u);
        const std::uintptr_t hi_this = high_half(this);
        const std::uintptr_t hi_p = high_half(p_arg);
        std::uintptr_t hi_map = 0;
        auto P = [&]() { return vp(F.rd(8), hi_p); };
        auto MAP = [&]() { return vp(F.rd(-0x14), hi_map); };
        if (jo::as_grass_patch(P())->owner == nullptr) return;  // GrassFrame keeps P / MAP / this in u32 slots (the emulated x86 frame) + their upper halves
        {
            const u32 a = (static_cast<u32>(jo::as_grass_patch(P())->grid_x) << 6) - jo::view<u32>(jo::as_cell(jo::as_grass_patch(P())->owner)->origin_x);
            F.wi(-0xc, static_cast<i32>(a) / 64);
            const u32 b = (static_cast<u32>(jo::as_grass_patch(P())->grid_y) << 6) - jo::view<u32>(jo::as_cell(jo::as_grass_patch(P())->owner)->origin_y);
            F.wi(-0x10, static_cast<i32>(b) / 64);
        }
        F.wr(-0x14, addr(jo::as_grass_patch(P())->owner));
        hi_map = high_half(jo::as_grass_patch(P())->owner);
        if (jo::as_cell(MAP())->p_84 == nullptr || jo::as_cell(MAP())->rows == nullptr) return;
        {
            const i16* const row = reinterpret_cast<const i16*>(jo::as_cell(MAP())->rows[F.ri(-0xc)].buffer);
            F.wi(-0x18, static_cast<i32>(row[F.ri(-0x10)]));  // dead (overwritten)
        }
        F.wr(-0x1c, 0);
        cdecl_call<void*>(kMemset, &F.b[-0x41c - GrassFrame::kLow], 0, 0x400u);
        auto cell = [&]() -> i32 {
            const i16* const row = reinterpret_cast<const i16*>(jo::as_cell(MAP())->rows[F.ri(-0x424)].buffer);
            return static_cast<i32>(row[F.ri(-0x42c)]);
        };
        for (F.wr(-0x420, 0); F.ri(-0x420) < 3; F.wr(-0x420, F.rd(-0x420) + 1)) {
            F.wr(-0x424, F.rd(-0xc) + F.rd(-0x420));
            if (!(F.ri(-0x424) < jo::as_cell(MAP())->sector_rows)) continue;
            for (F.wr(-0x428, 0); F.ri(-0x428) < 3; F.wr(-0x428, F.rd(-0x428) + 1)) {
                F.wr(-0x42c, F.rd(-0x10) + F.rd(-0x428));
                if (!(F.ri(-0x42c) < jo::as_cell(MAP())->sector_cols)) continue;
                const u32 val = F.hist_rd(cell()) + 1u;
                F.hist_wr(cell(), val);
                F.wr(-0x1c, F.rd(-0x1c) + 1u);
            }
        }
        i32 best = 0;
        i32 best_count = 0;
        for (F.wr(-0x420, 0); F.ri(-0x420) < jo::as_cell(MAP())->p84_count; F.wr(-0x420, F.rd(-0x420) + 1)) {
            if (static_cast<i32>(F.hist_rd(F.ri(-0x420))) > best_count) {
                best = F.ri(-0x420);
                best_count = static_cast<i32>(F.hist_rd(F.ri(-0x420)));
            }
        }
        if (best_count < F.ri(-0x1c) / 2) return;
        void* const grass_set = obj(d::g_a50680);
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_table(grass_set)->find_m(joan::orig::arg::str(// 0x00488210
                                    static_cast<u8*>(jo::as_cell(MAP())->p_84) + static_cast<u32>(best) * 32u), joan::orig::arg::i32(global<u32>(d::g_mission))));
        if (r == 0) return;
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_table(grass_set)->at_4671c0(joan::orig::arg::i16(r)));  // 0x004671c0 (full eax pushed)
        if (jo::as_grass_type(o)->entries == nullptr) return;
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_world_cell(jo::as_grass_patch(P())->owner)->grid(joan::orig::arg::i16(0)));  // 0x005600c0
        if (t == nullptr) return;
        if (global<i32>(d::g_a38fa0) == 0) {
            for (i32 i = 0; i < 0x20; ++i) {
                const float a = static_cast<float>(kFpu.div(kFpu.mul(fild(i), fd(d::flt_6_2831855)), fd(d::flt_32)));
                rot_y(&img::global<float>(d::g_a38790 + static_cast<u32>(i) * 64u), a, 0.0f, 0.0f);
            }
            global<i32>(d::g_a38fa0) = 1;
        }
        const double one = static_cast<double>(bits_f(F.rd(-8)));
        const float lim_x = static_cast<float>(kFpu.div(kFpu.mul(fild(jo::as_path_space(t)->cell_w), fd(d::flt_0_4)), one));
        const float lim_y = static_cast<float>(kFpu.div(kFpu.mul(fild(jo::as_path_space(t)->cell_h), fd(d::flt_0_4)), one));
        const float steep_x = static_cast<float>(kFpu.div(kFpu.mul(fild(jo::as_path_space(t)->cell_w), fd(d::flt_0_3)), one));
        // [ebp-0x458] (+0x28 * 0.3 / one) is computed but never read by the original.
        void* const self = vp(F.rd(-4), hi_this);
        void* const p = P();
        {
            struct Blades {  // build_mesh's invalidate once, when this patch's blades are all in (g_grass_invalidate)
                Blades() { begin_grass_blades(); }
                ~Blades() { end_grass_blades(); }
            } blades;
            for (i32 i = 1; i <= joan::orig::as_dynarray(jo::as_grass_type(o)->entries)->size(); ++i) {  // 0x00417ad0
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(jo::as_grass_type(o)->entries)->get(joan::orig::arg::i32(i)));  // 0x00417200
                if (static_cast<i32>(GE(e)->id) == 0) continue;
                if (!(rnd(100) < static_cast<i32>(GE(e)->chance))) continue;
                const u32 id_arg = (addr(e) & 0xffff0000u) | static_cast<u16>(GE(e)->id);
                void* const kind = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(id_arg)));  // 0x00425ce0
                if (joan::orig::arg::as<i32>(joan::orig::as_grass(self)->find_kind(joan::orig::arg::i32(static_cast<i32>(GE(e)->id)))) == 0) {                     // 0x004659b0
                    joan::orig::as_item_def(kind)->acquire_anim(0, 0);                                 // 0x004a39f0
                    jo::as_dyn_grass(self)->res_ids[jo::as_dyn_grass(self)->res_count] = static_cast<i32>(GE(e)->id);
                    jo::as_dyn_grass(self)->res_count = jo::as_dyn_grass(self)->res_count + 1;
                }
                i32 n = static_cast<i32>(static_cast<u32>(static_cast<i32>(GE(e)->density)) * global<u32>(d::g_a5805c)) / 100;
                if (n < 1) n = 1;
                for (i32 j = 1; j <= n; ++j) place(self, p, e, kind, t, lim_x, lim_y, steep_x);
            }
        }
        if (jo::as_grass_patch(p)->object != nullptr) {
            if (jo::as_scene_object(jo::as_grass_patch(p)->object)->plane != nullptr) {
                joan::orig::as_plane(jo::as_scene_object(jo::as_grass_patch(p)->object)->plane)->sort_by_material(1);  // 0x005cf1d0
                joan::render::grass_forget(jo::view<void*>(jo::as_scene_object(jo::as_grass_patch(p)->object)->plane));  // [Graphics] GrassBatching
            }
            joan::orig::as_scene(jo::as_grass_patch(p)->scene)->attach(joan::orig::arg::mptr(jo::as_grass_patch(p)->object));      // 0x005efc50
        }
    }

    // one grass instance of 0x465a20's inner loop
    static void place(void* self, void* p, u8* e, void* kind, void* t, float lim_x, float lim_y, float steep_x) {
        float m[16] = {};
        const float zero = global<float>(d::k_9cf5c4);
        if (!(GE(e)->yaw >= zero)) {                                    // +6 < 0 or NaN
            if (!(GE(e)->tilt < zero || GE(e)->tilt > zero)) {  // +0xa == 0 or NaN
                const i32 k = rnd(0x20);
                std::memcpy(m, img::obj(d::g_a38790 + static_cast<u32>(k) * 64u), 64);
            } else {
                const float b = static_cast<float>(kFpu.add(deg(rnd(5) - 10), GE(e)->tilt));
                const float a = deg(rnd(0x168));
                rot_y(m, a, b, 0.0f);
            }
        } else if (!(GE(e)->tilt < zero || GE(e)->tilt > zero)) {  // == 0 or NaN
            const float a = static_cast<float>(kFpu.add(deg(rnd(0xa) - 0x14), GE(e)->yaw));
            rot_y(m, a, 0.0f, 0.0f);
        } else {
            const float b = static_cast<float>(kFpu.add(deg(rnd(5) - 10), GE(e)->tilt));
            const float a = static_cast<float>(kFpu.add(deg(rnd(0xa) - 0x14), GE(e)->yaw));
            rot_y(m, a, b, 0.0f);
        }
        m[12] = static_cast<float>(kFpu.add(fild(rnd(0xc0)), jo::as_grass_patch(p)->box[0]));
        m[13] = static_cast<float>(kFpu.add(fild(rnd(0xc0)), jo::as_grass_patch(p)->box[1]));
        const float u = static_cast<float>(kFpu.div(kFpu.sub(m[12], fild(jo::as_cell(jo::as_grass_patch(p)->owner)->origin_x)), fild(jo::as_path_space(t)->cell_w)));
        const float v = static_cast<float>(kFpu.div(kFpu.sub(m[13], fild(jo::as_cell(jo::as_grass_patch(p)->owner)->origin_y)), fild(jo::as_path_space(t)->cell_h)));
        const i32 iu = ftol(u);
        const i32 iv = ftol(v);
        if (iu > jo::as_cell(jo::as_grass_patch(p)->owner)->sector_rows || iv > jo::as_cell(jo::as_grass_patch(p)->owner)->sector_cols) return;
        const i32 even = ((iu + iv) % 2 == 0) ? 1 : 0;
        void* const hf = jo::as_path_space(t)->grid;
        m[14] = joan::orig::arg::as<float>(joan::orig::as_terrain(hf)->height_4c7b70(joan::orig::arg::f32(u), joan::orig::arg::f32(v), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70
        float sa = 0.0f;  // always written by 0x004c7100 (every return path stores both)
        float sb = 0.0f;
        joan::orig::as_height_grid(hf)->sample(joan::orig::arg::f32(u), joan::orig::arg::f32(v), &sa, &sb);  // 0x004c7100 (result stored, unused)
        if (!(crt_fabs(sa) >= lim_x)) {
            if (!(crt_fabs(sb) >= lim_y)) {
                bool steep = true;
                if (!(crt_fabs(sa) >= steep_x)) {
                    if (!(crt_fabs(sb) >= lim_y)) steep = false;  // original compares with [ebp-0x450]
                }
                if (steep) m[14] = static_cast<float>(kFpu.sub(m[14], fd(d::flt_10)));
                void* const model = joan::orig::arg::as<void*>(joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->entry(joan::orig::arg::u32(jo::as_item_def(kind)->icon_sprite)));  // 0x0041a180
                joan::orig::as_grass(self)->build_mesh(joan::orig::arg::mptr(model), joan::orig::arg::mptr(p), joan::orig::arg::mptr(m));                               // 0x00466560
            }
        }
    }
};

// 0x00465a20: histogram index out of the 256 ints -> frame emulated, writes outside it dropped,
// reads outside it 0.
JOAN_REPLACE("fix/uninit", "DynGrass::populate_465a20", 0x00465a20, &H::populate);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x00466320 DynGrass::copy_to(dst)            (thiscall, ret 4) - field copy (dead code).
namespace {
namespace game_b101_02 {

JOAN_COUNTER(c_grass_copy, "DynGrass::copy_to");

char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

struct H {

    // ------------------------------------------------------------------ 0x00466320
    u32 grass_copy_to(u8* d) {
        c_grass_copy.hit();
        jo::view<u16>(jo::as_unit(d)->type_id) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->type_id);
        jo::view<u16>(jo::as_unit(d)->layer) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->layer);
        // rep movsd x 0xe, movsw, movsb: 0x3b bytes forward.
        {
            u8* dst = reinterpret_cast<u8*>(&jo::as_unit(d)->stats_block);
            const u8* src = reinterpret_cast<const u8*>(&jo::self<jo::UnitTemplate>(this)->stats);
            for (i32 i = 0; i < 0xe; ++i) {
                u32 w;
                std::memcpy(&w, src, 4);
                std::memcpy(dst, &w, 4);
                src += 4;
                dst += 4;
            }
            u16 h;
            std::memcpy(&h, src, 2);
            std::memcpy(dst, &h, 2);
            dst[2] = src[2];
        }
        crt_strcpy(jo::as_unit(d)->name_, jo::self<jo::UnitTemplate>(this)->name);
        crt_strcpy(jo::as_unit(d)->name_c, jo::self<jo::UnitTemplate>(this)->text);
        jo::view<u32>(jo::as_unit(d)->rank) = jo::self<jo::UnitTemplate>(this)->rank;
        jo::view<u16>(jo::as_unit(d)->kingdom_id) = jo::view<u16>(jo::as_unit(d)->kingdom_id);  // self-copy in the original
        jo::view<u16>(jo::as_unit(d)->race_id) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->race_id);
        jo::view<u16>(jo::as_unit(d)->level) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->level);
        jo::view<u16>(jo::as_unit(d)->i16_12e) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->i16_10e);
        jo::view<u32>(jo::as_unit(d)->hit_points) = jo::view<u32>(jo::self<jo::UnitTemplate>(this)->hit_points);
        jo::view<u32>(jo::as_unit(d)->hit_points_base) = jo::view<u32>(jo::self<jo::UnitTemplate>(this)->hit_points_base);
        jo::view<u32>(jo::as_unit(d)->regen_hp) = jo::view<u32>(jo::self<jo::UnitTemplate>(this)->regen_hp);
        jo::view<u32>(jo::as_unit(d)->regen_ep) = jo::view<u32>(jo::self<jo::UnitTemplate>(this)->regen_ep);
        jo::view<u16>(jo::as_unit(d)->stats[0]) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->stat_0);
        jo::view<u16>(jo::as_unit(d)->stats[1]) = jo::view<u16>(jo::as_unit(d)->stats[1]);  // self-copy in the original
        jo::view<u32>(jo::as_unit(d)->experience) = jo::view<u32>(jo::self<jo::UnitTemplate>(this)->experience);
        jo::view<u16>(jo::as_unit(d)->stats[6]) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->i16_128);
        jo::view<u16>(jo::as_unit(d)->stats[7]) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->i16_12a);
        jo::view<u16>(jo::as_unit(d)->stats[8]) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->i16_12c);
        const u32 hi = jo::self<jo::UnitTemplate>(this)->idle_time;
        jo::view<u32>(jo::as_unit(d)->idle_time) = hi;
        jo::view<u16>(jo::as_unit(d)->morale) = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->morale);
        const u16 last = jo::view<u16>(jo::self<jo::UnitTemplate>(this)->i16_13c);
        jo::as_unit(d)->u16_1d5 = last;
        return (hi & 0xffff0000u) | static_cast<u32>(last);  // eax
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("DynGrass::copy_to", 0x00466320, &H::grass_copy_to);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b106_05.cpp (game/b106_05)
//
//      0x00466560 DynGrass::build_mesh(a, patch, matrix)   (thiscall, ret 0xc) - creates the patch
//                 mesh object on first use (new 0x11c / 0x10c / 0x2d8 + ctors), merges the source
//                 materials, transforms the vertices, sets the grass bend weight, remaps indices.
//
//  - 0x00466560: the SEH frame only guards the operator new + constructor pairs.
namespace {
namespace game_b106_05 {

using joan::orig::helpers::at;
float fconst(img::Addr a) { return global<float>(a); }

using joan::orig::helpers::game_new;  // operator new

// Fixed 0x191-byte string class (0x005a1170 ctor, 0x005a1440 assign, 0x005a1550 append,
// 0x00417d90 c_str); no destructor.
struct alignas(4) Str {
    char b[0x194];
};

JOAN_COUNTER(c_grass, "DynGrass::build_mesh");

// Room in the patch for this blade and those after it. Plane::add_vertices / add_triangles grow an array to what is
// asked plus 31 / 7 spare, so a patch filled blade by blade (up to 0xccc vertices) was reallocated - copied, the old
// block wiped with 0xff, freed - for nearly every blade: most of a populate's time when a cutscene's camera jumps
// to new ground. Here a full array grows once to double its count, through the game's own add (the same file /
// line tags), and the count is put back; the blade's own add then fits. The arrays' contents and counts are as
// before - only the spare room past the count differs, which nothing reads (vertex_capacity sizes the arrays).
void reserve(u8* plane, i32 n, bool vertices) {
    jo::Plane& pl = *jo::as_plane(plane);
    std::int32_t& count = vertices ? pl.vertex_count : pl.triangle_count;
    const std::int32_t capacity = vertices ? pl.vertex_capacity : pl.triangle_capacity;
    if (count + n <= capacity) return;
    const std::int32_t saved = count;
    const std::int32_t grow = std::max(n, count);  // to count * 2 (at least this blade)
    if (vertices)
        joan::orig::as_plane(plane)->add_vertices(joan::orig::arg::i32(grow));   // 0x005cd7f0
    else
        joan::orig::as_plane(plane)->add_triangles(joan::orig::arg::i32(grow));  // 0x005ce6f0
    count = saved;
}

struct H {
    // ------------------------------------------------------------------------ 0x00466560
    void build_mesh(u8* a, u8* p, void* m) {
        joan::hitch::Span hitch_span("grass_build_mesh");  // [Debug] HitchLog
        if (jo::as_grass_patch(p)->scene == nullptr) return;
        c_grass.hit();
        i32 count = 0;
        u32* buf = nullptr;
        u8* r20 = nullptr;
        u8* const mesh = jo::view<u8*>(jo::as_model_entry(a)->actor_def->model->mesh);
        if (jo::as_grass_patch(p)->object != nullptr) {
            r20 = jo::view<u8*>(jo::as_grass_patch(p)->object);
            if (jo::view<i8>(jo::as_plane(jo::as_scene_object(r20)->plane)->buffer_mode) != 0 && jo::as_plane(jo::as_scene_object(r20)->plane)->vertex_count >= 0xccc) return;
            if (jo::view<i8>(jo::as_plane(jo::as_scene_object(r20)->plane)->buffer_mode) != 0 &&
                static_cast<i32>(static_cast<u32>(jo::as_plane(jo::as_scene_object(r20)->plane)->triangle_count) * 3u) >= 0x3333)
                return;
        } else {
            void* const m1 = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const o1 = m1 != nullptr ? joan::orig::as_plane(m1)->construct(8u, 1u) : nullptr;  // 0x005cbaf0
            joan::orig::as_plane(o1)->init(0, 0, 0, 2u);                                           // 0x005cc7a0
            const float half = fconst(d::flt_0_5);
            float v[4];
            v[0] = (jo::as_grass_patch(p)->box[0] + jo::as_grass_patch(p)->box[3]) * half;
            v[1] = (jo::as_grass_patch(p)->box[1] + jo::as_grass_patch(p)->box[4]) * half;
            v[2] = (jo::as_grass_patch(p)->box[2] + jo::as_grass_patch(p)->box[5]) * half;
            const u32 big = 0x483b8000u;
            std::memcpy(&v[3], &big, 4);
            void* const m2 = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
            r20 = m2 != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_scene_object(m2)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
            void* const m3 = game_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
            void* const o3 = m3 != nullptr ? joan::orig::as_actor_def(m3)->construct() : nullptr;  // 0x005af390
            jo::view<void*>(jo::as_scene_object(r20)->def) = o3;
            joan::orig::as_actor_def(jo::view<void*>(jo::as_scene_object(r20)->def))->create(joan::orig::as_plane(o1), static_cast<void*>(v));  // 0x005b02f0
            joan::orig::as_scene_object(r20)->init();                                          // 0x005aab70
            joan::orig::as_actor(r20)->set_position_5ac4b0(0u, 0u, 0u);                                 // 0x005ac4b0
            jo::view<u32>(jo::as_scene_object(r20)->holder_kind) = 3;
            Str s;
            joan::orig::as_string(&s)->construct(joan::orig::arg::str(jo::as_model_entry(a)->dir));  // 0x005a11a0
            joan::orig::as_string(&s)->append(str(d::s_ANIM));                     // 0x005a1550 "ANIM\\"
            jo::view<u8*>(jo::as_grass_patch(p)->object) = r20;
        }
        u8* const r24 = jo::view<u8*>(jo::as_scene_object(r20)->plane);
        if (jo::as_plane(mesh)->material_count > count) {
            count = jo::as_plane(mesh)->material_count + 8;
            const i32 line = static_cast<i32>(global<i16>(d::g_9fd8a0)) + 0x6d;
            buf = joan::orig::arg::as<u32*>(joan::orig::as_mem(obj(d::g_mem))->resize(static_cast<void*>(buf), joan::orig::arg::u32(static_cast<u32>(count) << 2), str(d::s_d_ww_dyngrass_cpp), joan::orig::arg::i32(line)));                         // 0x0058eb30
        }
        // Material remap.
        for (i32 i = 0; i < jo::as_plane(mesh)->material_count; ++i) {
            jo::as_plane(mesh)->materials[i].diffuse = global<u32>(d::g_init_white);
            i32 j = 0;
            for (; j < jo::as_plane(r24)->material_count; ++j) {
                const u32 same = joan::orig::arg::as<u32>(joan::orig::as_material(&jo::as_plane(mesh)->materials[i])->equals(joan::orig::arg::mptr(&jo::as_plane(r24)->materials[j])));  // 0x005d5500
                if ((same & 0xff) != 0) break;
            }
            if (j >= jo::as_plane(r24)->material_count) {
                j = joan::orig::arg::as<i32>(joan::orig::as_plane(r24)->add_materials(1));                                              // 0x005cd470
                joan::orig::as_material(&jo::as_plane(r24)->materials[j])->assign(joan::orig::arg::mptr(&jo::as_plane(mesh)->materials[i]));  // 0x005d53e0
            }
            buf[i] = static_cast<u32>(j);
        }
        // Vertices.
        const i32 nv = jo::as_plane(mesh)->vertex_count;
        reserve(r24, nv, true);
        const i32 base = joan::orig::arg::as<i32>(joan::orig::as_plane(r24)->add_vertices(joan::orig::arg::i32(nv)));  // 0x005cd7f0
        jo::VertexF8* const verts = &jo::as_plane(r24)->vertices_f8[base];  // 0x28-byte vertices
        float top = 1.0f;
        for (i32 i = 0; i < nv; ++i) {
            if (jo::as_plane(mesh)->vertices_f1[i].pos[2] > top) top = jo::as_plane(mesh)->vertices_f1[i].pos[2];
        }
        const float c80 = fconst(d::flt_9cfc38), c40 = fconst(d::flt_40), c0 = fconst(d::k_9cf5c4),
                    c1 = fconst(d::flt_1);
        float w = 1.0f;
        if (top >= c80) {
            w = 0.0f;
        } else if (top >= c40) {
            w = (c80 - top) / c40;
        } else if (top >= c0) {
            w = top / c40 * c1;
        } else {
            w = 0.0f;
        }
        for (i32 i = 0; i < nv; ++i) {
            jo::VertexF8* const vert = &verts[i];
            joan::orig::as_vertex_format(vert)->from_28(joan::orig::arg::mptr(&jo::as_plane(mesh)->vertices_f1[i]));  // 0x0060f4b0
            // v3d_87 point_transform / vector_transform called directly: the functions its import slots lead to
            // (lib/v3d/v3d87.cpp only counts the call), without the slot read and wrapper for every vertex.
            joan::render::v3d::point_transform(reinterpret_cast<joan::render::v3d::Vec3*>(vert), static_cast<const joan::render::v3d::Matrix*>(m));
            joan::render::v3d::vector_transform(reinterpret_cast<joan::render::v3d::Vec3*>(&vert->attr[1]), static_cast<const joan::render::v3d::Matrix*>(m));  // the normal (+0x10)
            const float t = jo::as_plane(mesh)->vertices_f1[i].pos[2] / top;
            vert->attr[0] = c1 - w * t * t;  // +0xc
        }
        jo::as_plane(r24)->transformed = 1;
        void* const dst = joan::orig::arg::as<void*>(joan::orig::as_plane(r24)->bone_5cf8f0(1));  // 0x005cf8f0
        std::memcpy(dst, obj(d::g_identity_matrix), 0x40);
        // Indices.
        const i32 nf = jo::as_plane(mesh)->triangle_count;
        reserve(r24, nf, false);
        const i32 base2 = joan::orig::arg::as<i32>(joan::orig::as_plane(r24)->add_triangles(joan::orig::arg::i32(nf)));  // 0x005ce6f0
        u16* s2 = reinterpret_cast<u16*>(jo::view<u8*>(jo::as_plane(mesh)->triangle_materials));
        u16* d2 = reinterpret_cast<u16*>(jo::view<u8*>(jo::as_plane(r24)->triangle_materials) + static_cast<u32>(base2) * 2u);
        u16* s1 = reinterpret_cast<u16*>(jo::view<u8*>(jo::as_plane(mesh)->triangles));
        u16* d1 = reinterpret_cast<u16*>(jo::view<u8*>(jo::as_plane(r24)->triangles) + static_cast<u32>(base2) * 3u * 2u);
        for (i32 i = 0; i < nf; ++i, ++s2, ++d2) {
            for (i32 k = 0; k < 3; ++k, ++s1, ++d1) {
                if (static_cast<i32>(*s1) >= jo::as_plane(mesh)->vertex_count) {
                    *d1 = *s1;
                } else {
                    *d1 = static_cast<u16>(static_cast<u32>(*s1) + static_cast<u32>(base));
                }
            }
            *d2 = static_cast<u16>(buf[*s2]);
        }
        if (r24 != nullptr) {
            grass_plane_changed(r24);  // Plane::invalidate(-1) 0x005cf920 + grass batching forget (once per patch in populate)
            jo::view<u8*>(jo::as_grass_patch(p)->object) = r20;
        }
        if (buf != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_9fd8a0)) + 0xdd;
            joan::orig::as_mem(obj(d::g_mem))->del(static_cast<void*>(buf), str(d::s_d_ww_dyngrass_cpp), joan::orig::arg::i32(line));                         // 0x0058ed70
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_05", name, addr, fn)
B("DynGrass::build_mesh", 0x00466560, &H::build_mesh);
#undef B

}  // namespace game_b106_05
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using joan::call::thiscall;

struct H {
    void forward_464390() { joan::orig::as_grass(this)->release(); }  // 0x00464170
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::forward_464390", 0x00464170, &H::forward_464390);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

void* memset_(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // ------------------------------------------------------------------ 0x00464130
    void* clear_1d1() {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::DynGrass)));  // 0x1d1 on x86
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::clear_1d1", 0x00464130, &H::clear_1d1);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

using joan::hook::global;

using joan::call::thiscall;

using i32 = std::int32_t;

using joan::img::obj;

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }

JOAN_COUNTER(c_hit_effect, "Unit::hit_effect_466d50");

struct H {
    // 0x00466d50
    void hit_effect() {
        c_hit_effect.hit();
        void* const units = obj(d::g_units);
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->mount_id))) != 0) return;  // 0x00428b70
        void* const other = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->mount_id)));      // 0x0041b3c0
        const std::int32_t s = jo::self<jo::Unit>(this)->anim_kind;
        if (s == 0x2b || s == 0x2c || s == 0x2d || s == 0x4f || s == 0x50 || s == 0x51 || s == 0x52) return;
        if (jo::self<jo::Unit>(this)->action == 1) return;
        if (jo::as_unit(other)->action == 1) return;
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(other)->can_act()) == 0) return;  // 0x00500ba0
        void* const fx =
            joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_actor(jo::as_unit(other)->actor)->record)));  // 0x006af310
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(this)->can_act()) != 0 && jo::self<jo::Unit>(this)->anim_kind != 0x49)  // 0x00500ba0
            joan::orig::as_unit(this)->set_action(0x44, 0, 1);                                            // 0x00501190
        if (jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->paired_anim_start_ms) != 0 && jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->paired_anim_start_ms) + 0x3e8u < now()) {
            joan::orig::as_body(fx)->set_2a2(joan::orig::arg::i16(1), 0x41200000, 0x425c0000);  // 0x006b1d80 (1, 10.0f, 55.0f)
            joan::orig::as_unit(other)->set_action(0xa, 0, 1);                 // 0x00501190
        } else {
            joan::orig::as_body(fx)->set_2a2(joan::orig::arg::i16(1), 0x40a00000, 0x42200000);  // 0x006b1d80 (1, 5.0f, 40.0f)
        }
        if (jo::as_unit(other)->anim_kind != 0xa) joan::orig::as_unit(other)->set_action(0xa, 0, 1);  // 0x00501190
        if (jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->paired_anim_start_ms) == 0) jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->paired_anim_start_ms) = now();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::hit_effect_466d50", 0x00466d50, &H::hit_effect);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

using joan::call::thiscall;

JOAN_COUNTER(c_grass_remove, "Grass::remove_patch");

using joan::orig::helpers::node_remove;  // 0x005efdb0

struct H {
    // 0x00466f40 Grass::remove_patch(owner): patches (+5 array of 0x66, count +9) with
    // +0 == owner and a node +0x28: detached from +0x24 and deleted.
    void grass_remove(void* owner) {
        c_grass_remove.hit();
        for (std::int32_t i = 0; i < jo::self<jo::DynGrass>(this)->patch_count; ++i) {
            if (jo::self<jo::DynGrass>(this)->patches[i].object == nullptr) continue;
            if (jo::self<jo::DynGrass>(this)->patches[i].owner != owner) continue;
            if (jo::view<void*>(jo::self<jo::DynGrass>(this)->patches[i].scene) != nullptr)
                node_remove(jo::view<void*>(jo::self<jo::DynGrass>(this)->patches[i].scene), jo::self<jo::DynGrass>(this)->patches[i].object);
            void* const p = jo::self<jo::DynGrass>(this)->patches[i].object;
            if (p != nullptr) joan::render::grass_forget(jo::view<void*>(jo::as_scene_object(p)->plane));  // [Graphics] GrassBatching
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840 delete
            jo::self<jo::DynGrass>(this)->patches[i].object = nullptr;
        }
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("Grass::remove_patch", 0x00466f40, &H::grass_remove);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

using u32 = std::uint32_t;

u32 JOAN_CDECL init_467070() {
    joan::orig::init_pi_a38f90();
    return 0;  // void callee; _initterm ignores the eax
}
u32 JOAN_CDECL init_4670d0() {
    joan::orig::init_a38f94();
    return 0;  // void callee; _initterm ignores the eax
}
u32 JOAN_CDECL init_467120() {
    joan::orig::init_a38f98();
    return 0;  // void callee; _initterm ignores the eax
}
u32 JOAN_CDECL init_467170() {
    joan::orig::init_a38f9c();
    return 0;  // void callee; _initterm ignores the eax
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("init_467070", 0x00467070, &init_467070);
B("init_4670d0", 0x004670d0, &init_4670d0);
B("init_467120", 0x00467120, &init_467120);
B("init_467170", 0x00467170, &init_467170);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::hook::global;

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}
static void JOAN_CDECL init_pi_a38f90() { init_pi(d::g_a38f90); }  // 0x00467090

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_pi_a38f90", 0x00467090, &init_pi_a38f90);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02_2.cpp (game/b100_02)
namespace {
namespace game_b100_02_2 {

using joan::hook::global;

// ---------------------------------------------------------------------- static initialisers
static void JOAN_CDECL init_a38f94() { global<float>(d::g_a38f94) = global<float>(d::g_a38f90) / global<float>(d::flt_2); }

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("init_a38f94", 0x004670f0, &init_a38f94);
#undef B100

}  // namespace game_b100_02_2
}  // namespace

// ---------------------------------------- from modules/game_b95_01_2.cpp (game/b95_01)
namespace {
namespace game_b95_01_2 {

using joan::hook::global;

void JOAN_CDECL init_a38f98() { global<float>(d::g_a38f98) = global<float>(d::g_a38f90) / global<float>(d::flt_3); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("init_a38f98", 0x00467140, &init_a38f98);
#undef B95

}  // namespace game_b95_01_2
}  // namespace

// ---------------------------------------- from modules/game_b97_01_2.cpp (game/b97_01)
namespace {
namespace game_b97_01_2 {

using joan::hook::global;

using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a38f9c() { div_global(d::g_a38f9c, d::g_a38f90, d::flt_4); }  // 0x00467190 (/4)

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_a38f9c", 0x00467190, &init_a38f9c);
#undef B97

}  // namespace game_b97_01_2
}  // namespace

// ---------------------------------------- from modules/game_b94_01_2.cpp (game/b94_01)
namespace {
namespace game_b94_01_2 {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using i16 = std::int16_t;

struct H {
    void* table_at(i16 i) {  // 0x004671c0
        return jo::self<jo::GrassRes>(this)->rows + (static_cast<i32>(i) - 1);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Table::at", 0x004671c0, &H::table_at);
#undef B94

}  // namespace game_b94_01_2
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;

using joan::orig::helpers::memset_;

struct H {
    // ------------------------------------------------------------------ 0x004671f0
    void* init_4671f0() {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::EndCon)));  // 0x1a on x86
        jo::view<u16>(jo::self<jo::EndCon>(this)->used) = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::init_4671f0", 0x004671f0, &H::init_4671f0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Grass::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_01::H::clear_1d1>(this)); }  // 0x464130
auto joan::orig::Grass::init(std::int32_t a0, std::int32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_02::H::grass_init>(this, a0, a1)); }  // 0x4641a0
auto joan::orig::Grass::release() -> void { joan::bind::member<&::game_b84_05::H::grass_release>(this); }  // 0x464390
auto joan::orig::Grass::reserve(std::int32_t a0) -> void { joan::bind::member<&::game_b97_01::H::reserve>(this, a0); }  // 0x4644d0
auto joan::orig::Grass::set_enabled(char a0) -> void { joan::bind::member<&::game_world4::H::grass_set_enabled>(this, a0); }  // 0x464600
auto joan::orig::Unit::recruit_nearby_4655a0() -> void { joan::bind::member<&::game_b104_02::H::recruit_nearby_4655a0>(this); }  // 0x4655a0
auto joan::orig::Grass::find_patch(void* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_01::H::find_record>(this, a0, a1, a2, a3)); }  // 0x465930
auto joan::orig::Grass::find_kind(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_02::H::find_slot>(this, a0)); }  // 0x4659b0
auto joan::orig::Grass::populate(void* a0) -> void { joan::bind::member<&::game_b115_10::H::populate>(this, a0); }  // 0x465a20
auto joan::orig::Grass::build_mesh(void* a0, void* a1, void* a2) -> void { joan::bind::member<&::game_b106_05::H::build_mesh>(this, a0, a1, a2); }  // 0x466560
auto joan::orig::Unit::hit_effect_466d50() -> void { joan::bind::member<&::game_b92_02::H::hit_effect>(this); }  // 0x466d50
auto joan::orig::Grass::remove_patch(void* a0) -> void { joan::bind::member<&::game_leaves81::H::grass_remove>(this, a0); }  // 0x466f40
auto joan::orig::init_pi_a38f90() -> void { joan::bind::free<&::game_b98_01::init_pi_a38f90>(); }  // 0x467090
auto joan::orig::init_a38f94() -> void { joan::bind::free<&::game_b100_02_2::init_a38f94>(); }  // 0x4670f0
auto joan::orig::init_a38f98() -> void { joan::bind::free<&::game_b95_01_2::init_a38f98>(); }  // 0x467140
auto joan::orig::init_a38f9c() -> void { joan::bind::free<&::game_b97_01_2::init_a38f9c>(); }  // 0x467190
auto joan::orig::Table::at_4671c0(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_01_2::H::table_at>(this, a0)); }  // 0x4671c0
