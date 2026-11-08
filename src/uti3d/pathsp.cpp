// uti3d/pathsp.cpp: functions placed by joan.exe's ww/uti3d/pathsp.cpp anchor and its address range
// (re/attribution.csv). Most also match the Restaurant Empire 2 engine's enpath/pathsp.cpp (see enpath/pathsp.cpp);
// the two files are not merged yet.
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/gen/height_map.hpp"
#include <cstdarg>
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::call::cdecl_call;

struct V3 {
    float x, y, z;
};

using joan::img::obj;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_path1.cpp (game/path1)
//
//      0x00670980 PathSpace::load(path) - reads a .pfd file (called by game_world5.cpp): the
//                 point list (this+0), the zones (this+0x14, 0x74-byte records with their
//                 neighbour lists), the zone links, the zone grid (this+0x30, cells of
//                 this+0x24 x this+0x28) with its per-cell sub grids (this+0x34), then marks
//                 the zones whose perimeter is shorter than 170 per open edge (+0x20 = -1).
//
//      0x00673d00 PathSpace::find_zone(out, zone, pos, a1c, depth, max_depth) - depth-limited
//                 walk over the zone links (+0x34[0..+0x28)) for a zone containing pos
//                 (called by game_units5.cpp Players::find_slot); recursion goes through its
//                 own thunk.
//
// 0x00670980 has an SEH frame that only covers the operator new + constructor pairs and the
namespace {
namespace game_path1 {


static_assert(sizeof(V3) == 12, "by-value size");

struct alignas(alignof(void*)) Reader {  // a FileMem: 0x11c bytes on x86, 0x12c on x64
    std::uint8_t bytes[((sizeof(joan::orig::FileMem) > 0x11c ? sizeof(joan::orig::FileMem) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

constexpr img::Addr kFiles(d::g_misc);
constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kNames(d::g_item_res);
constexpr img::Addr kPathspCpp(d::s_d_ww_uti3d_pathsp_cpp);  // "d:\ww\uti3d\pathsp.cpp"
std::int32_t line_base() { return static_cast<std::int32_t>(global<std::int16_t>(d::g_a21678)); }

std::int32_t read_int(Reader* r) { return joan::orig::as_file_mem(r)->file_get_long(); }  // 0x005893c0
using joan::orig::helpers::game_new;      // operator new
void* elem(void* array, std::int32_t i) { return joan::orig::arg::as<void*>(joan::orig::as_billboard(array)->item(joan::orig::arg::i32(i))); }  // 0x00637da0
// operator new(0x30) + 0x00637aa0 Array::Array(size, grow); null when new fails.
void* new_array(std::int32_t size, std::int32_t grow) {
    void* const p = game_new(static_cast<std::uint32_t>(sizeof(jo::BlkArr)));  // 0x30 on x86
    return p != nullptr ? joan::orig::as_blk_arr(p)->construct(joan::orig::arg::u32(size), joan::orig::arg::u32(grow)) : nullptr;
}
// operator new(0x14) + 0x004c56b0 Grid::Grid().
void* new_grid() {
    void* const p = game_new(static_cast<std::uint32_t>(sizeof(joan::orig::HeightMap)));  // 0x14 on x86 (HeightGrid + roughness/scale/depth)
    return p != nullptr ? joan::orig::as_height_grid(p)->construct() : nullptr;
}
// 0x004c58a0 init(w - 1, h - 1, 0) and the (w + 1) x (h + 1) cells through 0x004c5a20.
void fill_grid(void* grid, std::int32_t w, std::int32_t h, Reader* r) {
    joan::orig::as_height_grid(grid)->create(joan::orig::arg::i16(w - 1), joan::orig::arg::i16(h - 1), joan::orig::arg::i16(0));  // 0x004c58a0
    for (std::int32_t y = 0; y <= static_cast<std::int32_t>(jo::as_height_grid(grid)->height); ++y) {
        for (std::int32_t x = 0; x <= static_cast<std::int32_t>(jo::as_height_grid(grid)->width); ++x) {
            const std::int32_t v = read_int(r);
            joan::orig::as_height_grid(grid)->set(joan::orig::arg::u32(static_cast<std::int16_t>(x)), joan::orig::arg::u32(static_cast<std::int16_t>(y)), joan::orig::arg::u16(v));  // 0x004c5a20
        }
    }
}

JOAN_COUNTER(c_load, "PathSpace::load");
JOAN_COUNTER(c_find_zone, "PathSpace::find_zone");

struct H {
    // 0x00670980 (ret 4)
    std::int32_t load(const char* path) {
        c_load.hit();
        if (joan::orig::as_misc(img::obj(kFiles))->is_file_exist(joan::orig::arg::str(path)) == 0) return 0;  // 0x00590b80 exists
        Reader reader;
        joan::orig::as_file_mem(&reader)->construct();                      // 0x00587f80 Reader::Reader
        joan::orig::as_file_mem(&reader)->open_588000(joan::orig::arg::str(path), 1, static_cast<char>(joan::orig::arg::i32(1)));  // 0x00588000 open

        // Names, 0x80 bytes each.
        alignas(4) std::uint8_t local_names[0x1000];  // ebp-0x1130
        std::uint8_t* names = local_names;
        const std::int32_t name_count = read_int(&reader);
        if (name_count > 0x20) {
            const std::uint32_t size = static_cast<std::uint32_t>(name_count) << 7;
            names = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(size), joan::orig::arg::str(img::str(kPathspCpp)), joan::orig::arg::i32(line_base() + 0x15)));  // 0x0058e7b0 alloc
        }
        for (std::int32_t i = 0; i < name_count; ++i)
            joan::orig::as_file_mem(&reader)->file_read(joan::orig::arg::mptr(names + (static_cast<std::uint32_t>(i) << 7)), 0x80u);  // 0x00588a80 read bytes

        // Points (12-byte records) in this+0.
        const std::int32_t point_count = read_int(&reader);
        if (point_count > 0) {
            jo::view<void*>(jo::self<jo::PathSpace>(this)->points) = new_array(0xc, 0x800);
            const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_blk_arr(jo::view<void*>(jo::self<jo::PathSpace>(this)->points))->add(joan::orig::arg::i32(point_count)));  // 0x00637cb0 add
            for (std::int32_t i = 0; i < point_count; ++i) {
                std::int32_t* const p = static_cast<std::int32_t*>(elem(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), base + i));
                p[0] = read_int(&reader);
                p[1] = read_int(&reader);
                p[2] = read_int(&reader);
            }
        }

        // Zones (0x74-byte records) in this+0x14.
        const std::int32_t zone_count = read_int(&reader);
        if (zone_count > 0) {
            if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr) jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) = new_array(static_cast<std::int32_t>(sizeof(jo::PfdZone)), 0x800);
            joan::orig::as_blk_arr(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->add(joan::orig::arg::i32(zone_count));
            std::int32_t capacity = 0;  // ebp-0x1154
            void** list = nullptr;      // ebp-0x1158
            for (std::int32_t i = 1; i <= zone_count; ++i) {
                jo::PfdZone* const zone = jo::as_pfd_zone(elem(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), i));
                zone->id = i;
                jo::view<void*>(zone->path_space) = this;
                const std::int32_t kind = read_int(&reader);
                if (kind == 0) {
                    zone->kind = 0;
                } else if (kind == 1) {
                    zone->kind = 1;
                }
                zone->bbox_min[0] = read_int(&reader);
                zone->bbox_min[1] = read_int(&reader);
                zone->bbox_min[2] = read_int(&reader);
                zone->bbox_max[0] = read_int(&reader);
                zone->bbox_max[1] = read_int(&reader);
                zone->bbox_max[2] = read_int(&reader);
                const std::int32_t name = read_int(&reader);
                const std::uintptr_t name_ptr =
                    reinterpret_cast<std::uintptr_t>(names) + static_cast<std::uintptr_t>(static_cast<std::intptr_t>(name - 1) * 128);  // name 0 -> names - 128 (wraps on x86)
                zone->name_id =
                    joan::orig::arg::as<std::int32_t>(joan::orig::as_name_table(img::obj(kNames))->find_4a36c0(joan::orig::arg::str(reinterpret_cast<const char*>(name_ptr))));  // 0x004a36c0
                const std::int32_t n = read_int(&reader);
                if (n > capacity) {
                    capacity = n + 4;
                    list = joan::orig::arg::as<void**>(joan::orig::as_mem(img::obj(kMemory))->resize(static_cast<void*>(list), joan::orig::arg::u32(static_cast<std::uint32_t>(capacity) * sizeof(void*)), joan::orig::arg::str(img::str(kPathspCpp)), joan::orig::arg::i32(line_base() + 0x5d)));                         // 0x0058eb30 realloc
                }
                for (std::int32_t j = 0; j < n; ++j) {
                    const std::int32_t index = read_int(&reader);
                    list[j] = elem(jo::view<void*>(jo::self<jo::PathSpace>(this)->points), index);
                }
                joan::orig::as_zone(zone)->set_points(joan::orig::arg::mptr(list), joan::orig::arg::i32(n), joan::orig::arg::u16(1));  // 0x00663cb0 set points
            }
            joan::orig::as_mem(img::obj(kMemory))->del(static_cast<void*>(list), joan::orig::arg::str(img::str(kPathspCpp)), joan::orig::arg::i32(line_base() + 0x6b));                         // 0x0058ed70 free
        }

        // Zone links.
        const std::int32_t link_count = read_int(&reader);
        if (link_count > 0) {
            for (std::int32_t i = 0; i < link_count; ++i) {
                const std::int32_t a = read_int(&reader);
                void* const za = elem(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), a);
                const std::int32_t b = read_int(&reader);
                const std::int32_t c = read_int(&reader);
                void* const zc = elem(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), c);
                const std::int32_t d = read_int(&reader);
                joan::orig::as_zone(za)->link(joan::orig::arg::i32(b), joan::orig::arg::mptr(zc), joan::orig::arg::i32(d));  // 0x00664130 link
            }
        }

        // Zone grid.
        if (read_int(&reader) != 0) {
            (void)read_int(&reader);
            (void)read_int(&reader);
            std::int32_t w = read_int(&reader);
            std::int32_t h = read_int(&reader);
            const std::int32_t cell_w = read_int(&reader);
            const std::int32_t cell_h = read_int(&reader);
            jo::self<jo::PathSpace>(this)->cell_w = cell_w;
            jo::self<jo::PathSpace>(this)->cell_h = cell_h;
            jo::view<void*>(jo::self<jo::PathSpace>(this)->grid) = new_grid();
            fill_grid(jo::view<void*>(jo::self<jo::PathSpace>(this)->grid), w, h, &reader);
            const std::int32_t sub_count = read_int(&reader);
            if (sub_count != 0) {
                const std::uint32_t gw = static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::PathSpace>(this)->grid->width));
                const std::uint32_t gh = static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::PathSpace>(this)->grid->height));
                jo::view<void*>(jo::self<jo::PathSpace>(this)->sub_grids) = game_new(gw * gh * static_cast<std::uint32_t>(sizeof(void*)));
                std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PathSpace>(this)->sub_grids)), 0, joan::orig::arg::u32(gw * static_cast<std::uint32_t>(sizeof(void*)) * gh));  // memset
                for (std::int32_t k = 0; k < sub_count; ++k) {
                    const std::int32_t px = read_int(&reader);
                    const std::int32_t py = read_int(&reader);
                    w = read_int(&reader);
                    h = read_int(&reader);
                    (void)read_int(&reader);
                    (void)read_int(&reader);
                    const std::int32_t ix = px / jo::self<jo::PathSpace>(this)->cell_w;
                    const std::int32_t iy = py / jo::self<jo::PathSpace>(this)->cell_h;
                    void* const sub = new_grid();
                    fill_grid(sub, w, h, &reader);
                    const std::uint32_t stride =
                        static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::PathSpace>(this)->grid->width));
                    const std::uint32_t cell = static_cast<std::uint32_t>(iy) * stride + static_cast<std::uint32_t>(ix);
                    jo::view<void**>(jo::self<jo::PathSpace>(this)->sub_grids)[static_cast<std::int32_t>(cell)] = sub;
                }
            }
        }

        // Zones with a short perimeter over their open edges.
        const std::int32_t n = joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(this)->zone_count());  // 0x0066b9f0 zone count
        const Fpu fpu{true};
        for (std::int32_t i = 1; i <= n; ++i) {
            jo::PfdZone* const zone = jo::as_pfd_zone(elem(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), i));
            if (zone->blocked == 1) continue;
            float sum = 0.0f;           // ebp-0x11d4
            std::int32_t open = 0;      // ebp-0x11d8
            for (std::int32_t j = 1; j <= zone->point_count; ++j) {
                const std::int32_t* const p0 = zone->points[j - 1].item->xyz;
                const std::int32_t x0 = p0[0], y0 = p0[1];
                const std::int32_t next = j != zone->point_count ? j : 0;
                const std::int32_t* const p1 = zone->points[next].item->xyz;
                const std::int32_t x1 = p1[0], y1 = p1[1];
                if (zone->links[j - 1].item == nullptr) continue;
                ++open;
                const double prev = static_cast<double>(sum);
                const std::int32_t dx = static_cast<std::int32_t>(static_cast<std::uint32_t>(x0) - static_cast<std::uint32_t>(x1));
                const std::int32_t dy = static_cast<std::int32_t>(static_cast<std::uint32_t>(y0) - static_cast<std::uint32_t>(y1));
                const std::int32_t dy2 = static_cast<std::int32_t>(static_cast<std::uint32_t>(dy) * static_cast<std::uint32_t>(dy));
                double t = fpu.mul(static_cast<double>(dx), static_cast<double>(dx));  // fild / fimul
                t = fpu.add(t, static_cast<double>(dy2));                               // fiadd
                const double root = joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(t)));                  // CRT sqrt
                sum = static_cast<float>(fpu.add(root, prev));
            }
            const std::int32_t limit = static_cast<std::int32_t>(static_cast<std::uint32_t>(open) * 0xaau);
            if (static_cast<double>(limit) > static_cast<double>(sum)) zone->blocked = -1;
        }
        joan::orig::as_path_space(this)->group_zones();   // 0x00675660
        joan::orig::as_file_mem(&reader)->destruct();  // 0x00587fd0 Reader::~Reader
        return 1;
    }

    // 0x00673d00 (ret 0x20)
    std::int32_t find_zone(std::int32_t* out, std::int32_t zone_index, V3 pos, std::uint32_t a1c, std::int32_t depth,
                           std::int32_t max_depth) {
        c_find_zone.hit();
        if (zone_index == 0) return 0;
        jo::PfdZone* const zone = jo::as_pfd_zone(elem(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), zone_index));
        if (depth == max_depth) {
            std::uint32_t xy[2];
            std::memcpy(xy, &pos, 8);
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_pfd_zone(zone)->contains(joan::orig::arg::f32(xy[0]), joan::orig::arg::f32(xy[1]), joan::orig::arg::f32(a1c), static_cast<char>(joan::orig::arg::i32(0)))) != 0) {  // 0x006718c0 contains
                *out = zone_index;
                return 1;
            }
            return 0;
        }
        for (std::int32_t i = 1; i <= zone->point_count; ++i) {
            std::int32_t* const link = jo::view<std::int32_t*>(zone->links[i - 1].item);
            if (link == nullptr) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(this)->find_zone(joan::orig::arg::mptr(out), joan::orig::arg::i32(*link), pos, joan::orig::arg::u32(a1c), joan::orig::arg::i32(depth + 1), joan::orig::arg::i32(max_depth))) != 0) return 1;  // 0x00673d00
        }
        return 0;
    }
};

#define P1(name, addr, fn) JOAN_REPLACE("game/path1", name, addr, fn)
P1("PathSpace::load_path_polygon", 0x00670980, &H::load);
P1("PathSpace::find_polygon_from_cur", 0x00673d00, &H::find_zone);
#undef P1

}  // namespace game_path1
}  // namespace

// ---------------------------------------- from modules/game_b82_13.cpp (game/b82_13)
//
//      0x006718c0 PfdZone::contains(x, y, r, any) (ret 0x10): returns 0 when !any and the
//                 zone's +0x20 == 1, or when (x, y) lies outside the int bounding box
//                 (+0x8, +0xc)..(+0x14, +0x18); then for each edge i = 1..+0x28 of the
//                 polygon (+0x30 point pointers, edge i-1 -> i mod n) returns 0 when the point
//                 is on the outer side (cross product > 0). For edges without a link
//                 (+0x34[i-1] null or link +0x20 == 1) and r > 0, when the point is closer than
//                 r to the edge (0x00664d70), the edge must be axis aligned and lie on the
//                 border of the map rectangle returned by 0x004a2b20(0xa57636, [0xa57bb2],
//                 +0x6c), else 0. For linked edges with r > 0 and the point within r, the
//                 global stamp 0xa57632 is incremented into +0x2c and the link's
//                 0x00671cb0(x, y, stamp, r) must succeed. Returns 1.
//
//      0x00671890 PfdZone::point(i) (ret 4): +0x30[i - 1].
//
// 0x006718c0 notes: the original's ebp-0x8 local is set to 0 and never written, so the
namespace {
namespace game_b82_13 {


JOAN_COUNTER(c_contains, "PfdZone::contains");

// x87 "fcom/fcompp; test ah,0x40; jnz" : equal or unordered.
bool eq_or_unordered(double a, double b) { return !(a < b || a > b); }

struct H {
    // 0x006718c0 (ret 0x10)
    std::uint8_t contains(float x, float y, float r, char any) {
        c_contains.hit();
        if (static_cast<std::int32_t>(any) == 0 && jo::self<jo::PfdZone>(this)->blocked == 1) return 0;
        const double xd = x, yd = y;
        if (static_cast<double>(jo::self<jo::PfdZone>(this)->bbox_min[0]) > xd) return 0;       // test ah,0x41 je
        if (!(static_cast<double>(jo::self<jo::PfdZone>(this)->bbox_max[0]) >= xd)) return 0;  // test ah,1 jne
        if (static_cast<double>(jo::self<jo::PfdZone>(this)->bbox_min[1]) > yd) return 0;
        if (!(static_cast<double>(jo::self<jo::PfdZone>(this)->bbox_max[1]) >= yd)) return 0;
        const Fpu fpu{true};
        for (std::int32_t i = 1; i <= jo::self<jo::PfdZone>(this)->point_count; ++i) {
            const std::int32_t* const p0 = jo::self<jo::PfdZone>(this)->points[i - 1].item->xyz;
            const std::int32_t x0 = p0[0], y0 = p0[1];
            const std::int32_t j = (i != jo::self<jo::PfdZone>(this)->point_count) ? i : 0;
            const std::int32_t* const p1 = jo::self<jo::PfdZone>(this)->points[j].item->xyz;
            const std::int32_t x1 = p1[0], y1 = p1[1];
            const std::int32_t dy = static_cast<std::int32_t>(static_cast<std::uint32_t>(y1) - static_cast<std::uint32_t>(y0));
            const std::int32_t dx = static_cast<std::int32_t>(static_cast<std::uint32_t>(x1) - static_cast<std::uint32_t>(x0));
            const double a = fpu.mul(fpu.sub(xd, static_cast<double>(x0)), static_cast<double>(dy));
            const double b = fpu.mul(fpu.sub(yd, static_cast<double>(y0)), static_cast<double>(dx));
            const double cross = fpu.sub(a, b);
            if (cross > 0.0) return 0;  // test ah,0x41 jne -> continue

            const jo::PfdZone* const link = jo::self<jo::PfdZone>(this)->links[i - 1].item;
            if (link == nullptr || link->blocked == 1) {
                if (r > 0.0f) {
                    const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(static_cast<float>(x0)), joan::orig::arg::f32(static_cast<float>(y0)), joan::orig::arg::f32(static_cast<float>(x1)), joan::orig::arg::f32(static_cast<float>(y1)), joan::orig::arg::f32(x), joan::orig::arg::f32(y)));  // 0x00664d70
                    if (fpu.mul(r, r) > static_cast<double>(d)) {
                        const jo::RWorldCell* const rect = jo::as_rworld_cell(joan::orig::placements::find(joan::orig::arg::mptr(img::obj(d::g_placements)), joan::orig::arg::i32(global<std::int32_t>(d::g_a57bb2)), jo::self<jo::PfdZone>(this)->path_space));  // 0x004a2b20
                        if (rect == nullptr) return 0;
                        if (x0 == x1) {
                            const double base = static_cast<double>(rect->x);
                            const double t1 = fpu.sub(static_cast<double>(rect->box[0]), base);
                            if (!eq_or_unordered(static_cast<double>(x0), t1)) {
                                const double t2 = fpu.sub(static_cast<double>(rect->box[3]), base);
                                if (!eq_or_unordered(static_cast<double>(x0), t2)) return 0;
                            }
                        } else if (y0 == y1) {
                            const double base = static_cast<double>(rect->y);
                            const double t1 = fpu.sub(static_cast<double>(rect->box[1]), base);
                            if (!eq_or_unordered(static_cast<double>(y0), t1)) {
                                const double t2 = fpu.sub(static_cast<double>(rect->box[4]), base);
                                if (!eq_or_unordered(static_cast<double>(y0), t2)) return 0;
                            }
                        } else {
                            return 0;
                        }
                    }
                    continue;
                }
                // r not > 0: the original falls into the linked-edge block, whose r > 0 test
                // fails the same way.
                continue;
            }
            if (!(r > 0.0f)) continue;
            const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(static_cast<float>(x0)), joan::orig::arg::f32(static_cast<float>(y0)), joan::orig::arg::f32(static_cast<float>(x1)), joan::orig::arg::f32(static_cast<float>(y1)), joan::orig::arg::f32(x), joan::orig::arg::f32(y)));  // 0x00664d70
            if (!(fpu.mul(r, r) > static_cast<double>(d))) continue;
            // ebp-0x8 is always 0 here.
            global<std::int32_t>(d::g_path_poly_visit_counter) = global<std::int32_t>(d::g_path_poly_visit_counter) + 1;
            jo::self<jo::PfdZone>(this)->visit_stamp = global<std::int32_t>(d::g_path_poly_visit_counter);
            void* const link2 = jo::self<jo::PfdZone>(this)->links[i - 1].item;
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_path_space(link2)->flood(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::i32(jo::self<jo::PfdZone>(this)->visit_stamp), joan::orig::arg::f32(r))) == 0) return 0;  // 0x00671cb0
        }
        return 1;
    }

    // 0x00671890 (ret 4)
    void* point(std::int32_t i) { return jo::self<jo::PfdZone>(this)->points[i - 1].item; }
};

#define B13(name, addr, fn) JOAN_REPLACE("game/b82_13", name, addr, fn)
B13("PathPolygon::is_inside_polygon", 0x006718c0, &H::contains);
B13("PfdZone::point", 0x00671890, &H::point);
#undef B13

}  // namespace game_b82_13
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
//
//      0x00671cb0 PathSpace::flood(x, y, id, radius) (uti3d/pathsp.cpp, thiscall, ret 0x10, al)
namespace {
namespace game_b83_04 {


JOAN_COUNTER(c_flood, "PathSpace::flood");

bool equal_or_nan(double a, double b) { return !(a < b || a > b); }

// x87 fmul at precision control 24: 24-bit significand, extended exponent range.
double mul24(float a, float b) {
    double p = static_cast<double>(a) * static_cast<double>(b);  // exact
    if (p == 0.0 || !std::isfinite(p)) return p;
    double scale = 1.0;  // powers of two: exact (|p| >= 2^-298)
    while (std::fabs(p) > 1e30) {
        p *= 0x1p-64;
        scale *= 0x1p64;
    }
    while (std::fabs(p) < 1e-30) {
        p *= 0x1p64;
        scale *= 0x1p-64;
    }
    return static_cast<double>(static_cast<float>(p)) * scale;
}

struct H {
    // 0x00671cb0 (ret 0x10, al)
    std::uint8_t flood(float x, float y, std::int32_t id, float radius) {
        c_flood.hit();
        if (jo::self<jo::PfdZone>(this)->visit_stamp == id) return 1;
        for (std::int32_t i = 1; i <= jo::self<jo::PfdZone>(this)->point_count; ++i) {
            const std::int32_t* const p0 = jo::view<std::int32_t**>(jo::self<jo::PfdZone>(this)->points)[i - 1];
            const std::int32_t x0 = p0[0], y0 = p0[1];
            const std::int32_t j = i != jo::self<jo::PfdZone>(this)->point_count ? i : 0;
            const std::int32_t* const p1 = jo::view<std::int32_t**>(jo::self<jo::PfdZone>(this)->points)[j];
            const std::int32_t x1 = p1[0], y1 = p1[1];
            const jo::PfdZone* const child = jo::self<jo::PfdZone>(this)->links[i - 1].item;
            const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(static_cast<float>(static_cast<double>(x0))), joan::orig::arg::f32(static_cast<float>(static_cast<double>(y0))), joan::orig::arg::f32(static_cast<float>(static_cast<double>(x1))), joan::orig::arg::f32(static_cast<float>(static_cast<double>(y1))), joan::orig::arg::f32(x), joan::orig::arg::f32(y)));  // 0x00664d70
            const bool within = mul24(radius, radius) > static_cast<double>(d);
            if (child == nullptr || child->blocked == 1) {
                if (!within) continue;
                const jo::RWorldCell* const e = jo::as_rworld_cell(cdecl_call<std::uint8_t*>(
                    fn::Placements_find, obj(d::g_placements), global<std::uint32_t>(d::g_a57bb2), jo::self<jo::PfdZone>(this)->path_space));  // 0x004a2b20
                if (e == nullptr) {
                    jo::self<jo::PfdZone>(this)->visit_stamp = 0;
                    return 0;
                }
                const joan::hook::x87::Fpu fpu{true};
                if (x0 == x1) {
                    const double ex = static_cast<double>(x0);
                    if (equal_or_nan(ex, fpu.sub(e->box[0], static_cast<double>(e->x))))
                        continue;
                    if (equal_or_nan(ex, fpu.sub(e->box[3], static_cast<double>(e->x))))
                        continue;
                    jo::self<jo::PfdZone>(this)->visit_stamp = 0;
                    return 0;
                }
                if (y0 == y1) {
                    const double ey = static_cast<double>(y0);
                    if (equal_or_nan(ey, fpu.sub(e->box[1], static_cast<double>(e->y))))
                        continue;
                    if (equal_or_nan(ey, fpu.sub(e->box[4], static_cast<double>(e->y))))
                        continue;
                    jo::self<jo::PfdZone>(this)->visit_stamp = 0;
                    return 0;
                }
                jo::self<jo::PfdZone>(this)->visit_stamp = 0;
                return 0;
            }
            if (!within) continue;
            jo::self<jo::PfdZone>(this)->visit_stamp = id;
            if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_path_space(jo::view<void**>(jo::self<jo::PfdZone>(this)->links)[i - 1])->flood(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::i32(id), joan::orig::arg::f32(radius))) == 0) {  // 0x00671cb0
                jo::self<jo::PfdZone>(this)->visit_stamp = 0;
                return 0;
            }
        }
        jo::self<jo::PfdZone>(this)->visit_stamp = id;
        return 1;
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("PathPolygon::is_inside_polygon2", 0x00671cb0, &H::flood);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
//
//    - Path::check_671fb0 0x00671fb0 (ret 0x20, uti3d/pathsp.cpp): polygon edge / radius test,
//      recursion into children through 0x00672500, fallback 0x00672e40. Returns al.
namespace {
namespace game_b108_12 {

using i8 = std::int8_t;


JOAN_COUNTER(c_671fb0, "Path::check_671fb0");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x00671fb0
    void edge_fallback(u32 a1, u32 a2, u32 a3, u32 a4, float x1, float y1, float x2, float y2, void* a5,
                       void* a6) {
        joan::orig::as_pfd_zone(this)->offset_points(joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4), joan::orig::arg::f32(x1), joan::orig::arg::f32(y1), joan::orig::arg::f32(x2), joan::orig::arg::f32(y2), a5, a6);  // 0x00672e40
    }
    // x87: fild [e+i]; fsubr float [e+f]; fild v; fcompp; test ah,0x40 -> "equal or unordered".
    static bool edge_equal(float f, i32 base, i32 v) {
        const double diff = Fpu{true}.sub(static_cast<double>(f), static_cast<double>(base));
        const double dv = static_cast<double>(v);
        return !(dv < diff) && !(dv > diff);
    }
    u8 check_671fb0(u32 a1, u32 a2, u32 a3, u32 a4, void* a5, void* a6, float r, u32 flag) {
        c_671fb0.hit();
        if (static_cast<i8>(flag) == 0 && jo::view<u32>(jo::self<jo::PfdZone>(this)->blocked) == 1) return 0;
        // n / point / child arrays re-read every iteration as in the original (callees in between).
        for (i32 i = 1; i <= jo::self<jo::PfdZone>(this)->point_count; ++i) {
            const jo::PfdPoint* const p = jo::self<jo::PfdZone>(this)->points[i - 1].item;
            const i32 x1 = p->xyz[0], y1 = p->xyz[1];
            const i32 j = i == jo::self<jo::PfdZone>(this)->point_count ? 0 : i;
            const jo::PfdPoint* const q = jo::self<jo::PfdZone>(this)->points[j].item;
            const i32 x2 = q->xyz[0], y2 = q->xyz[1];
            const jo::PfdZone* const kid = jo::self<jo::PfdZone>(this)->links[i - 1].item;
            if (kid != nullptr && jo::view<u32>(kid->blocked) != 1) continue;
            if (!(r > 0.0f)) continue;
            const float fx1 = static_cast<float>(x1), fy1 = static_cast<float>(y1);
            const float fx2 = static_cast<float>(x2), fy2 = static_cast<float>(y2);
            const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(fx1), joan::orig::arg::f32(fy1), joan::orig::arg::f32(fx2), joan::orig::arg::f32(fy2), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4)));  // 0x00664d70
            if (!(Fpu{true}.mul(r, r) > static_cast<double>(d))) continue;
            const jo::RWorldCell* const e = jo::as_rworld_cell(joan::orig::placements::find(img::obj(d::g_placements), joan::orig::arg::i32(global<u32>(d::g_a57bb2)), jo::self<jo::PfdZone>(this)->path_space));  // 0x004a2b20
            if (e == nullptr) {
                edge_fallback(a1, a2, a3, a4, fx1, fy1, fx2, fy2, a5, a6);
                return 0;
            }
            if (x1 == x2) {
                if (edge_equal(e->box[0], e->x, x1) || edge_equal(e->box[3], e->x, x1)) continue;
                edge_fallback(a1, a2, a3, a4, fx1, fy1, fx2, fy2, a5, a6);
                return 0;
            }
            if (y1 == y2) {
                if (edge_equal(e->box[1], e->y, y1) || edge_equal(e->box[4], e->y, y1)) continue;
                edge_fallback(a1, a2, a3, a4, fx1, fy1, fx2, fy2, a5, a6);
                return 0;
            }
            edge_fallback(a1, a2, a3, a4, fx1, fy1, fx2, fy2, a5, a6);
            return 0;
        }
        for (i32 i = 1; i <= jo::self<jo::PfdZone>(this)->point_count; ++i) {
            const jo::PfdPoint* const p = jo::self<jo::PfdZone>(this)->points[i - 1].item;
            const i32 x1 = p->xyz[0], y1 = p->xyz[1];
            const i32 j = i == jo::self<jo::PfdZone>(this)->point_count ? 0 : i;
            const jo::PfdPoint* const q = jo::self<jo::PfdZone>(this)->points[j].item;
            const i32 x2 = q->xyz[0], y2 = q->xyz[1];
            jo::PfdZone* const kid = jo::self<jo::PfdZone>(this)->links[i - 1].item;
            if (kid == nullptr || jo::view<u32>(kid->blocked) == 1) continue;  // both r branches skip
            if (!(r > 0.0f)) continue;
            const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(static_cast<float>(x1)), joan::orig::arg::f32(static_cast<float>(y1)), joan::orig::arg::f32(static_cast<float>(x2)), joan::orig::arg::f32(static_cast<float>(y2)), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4)));
            if (!(Fpu{true}.mul(r, r) > static_cast<double>(d))) continue;
            // [ebp-8] is never written after its 0 init: the counter always advances.
            global<u32>(d::g_path_poly_visit_counter) = global<u32>(d::g_path_poly_visit_counter) + 1u;
            jo::view<u32>(jo::self<jo::PfdZone>(this)->visit_stamp) = global<u32>(d::g_path_poly_visit_counter);
            const u32 res = joan::orig::arg::as<u32>(joan::orig::as_pfd_zone(kid)->find_crossing(a1, a2, a3, a4, a5, a6, joan::orig::arg::i32(jo::view<u32>(jo::self<jo::PfdZone>(this)->visit_stamp)), r));  // 0x00672500
            if (static_cast<u8>(res) == 0) return 0;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("PathPolygon::is_inside_polygon3", 0x00671fb0, &H::check_671fb0);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b109_20.cpp (game/b109_20)
//
//    - PathSpace::find_crossing 0x00672500 (thiscall (x, y, tx, ty, V3* out1, V3* out2, id, radius),
//      ret 0x20, uti3d/pathsp.cpp; recursive through 0x004069ba; callers use al only): if +0x2c is
//      already id returns 1. Walks the polygon edges ([+0x30] int points, +0x28 count, children
//      [+0x34]); for every edge the point (x, y) lies left of (side < 0 or NaN), either tests
//      a direct crossing (no child, or child +0x20 == 1): distance 0x00664d70 < radius^2, record
//      lookup 0x004a2b20(0xa57636, [0xa57bb2], +0x6c) with the vertical / horizontal edge
//      exclusions, segment intersection 0x00664c60 and on a hit 0x00672e40, +0x2c = 0, returns 0;
//      or recurses into the child (+0x2c = id first; child failure -> +0x2c = 0, returns 0).
//      End of the loop: +0x2c = id, returns 1.
namespace {
namespace game_b109_20 {


// Raw 12-byte vector passed by value (bit copies, no float conversion).
struct V3r {
    u32 x, y, z;
};
static_assert(sizeof(V3r) == 12, "V3r");
static_assert(sizeof(V3) == 12, "V3");

float bits_f(u32 v) {
    float f;
    std::memcpy(&f, &v, sizeof f);
    return f;
}
i32 isub(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); }
float zero_c() { return global<float>(d::k_9cf5c4); }  // 0.0f

JOAN_COUNTER(c_find_crossing, "PathSpace::find_crossing");

struct H {
    // ---------------------------------------------------------------------------- 0x00672500
    bool find_crossing(u32 a1, u32 a2, u32 a3, u32 a4, V3* out1, V3* out2, i32 id, float radius) {
        c_find_crossing.hit();
        if (jo::self<jo::PfdZone>(this)->visit_stamp == id) return true;
        const V3r va{a1, a2, 0};
        const V3r vb{a3, a4, 0};
        u8 l24[8];
        joan::orig::as_point(l24)->construct();  // 0x004eea60 (empty ctor)
        const Fpu fpu{true};
        for (i32 i = 1; i <= jo::self<jo::PfdZone>(this)->point_count; ++i) {
            const i32* const p = jo::view<i32**>(jo::self<jo::PfdZone>(this)->points)[i - 1];
            const i32 x1 = p[0], y1 = p[1];
            const i32 j = (i != jo::self<jo::PfdZone>(this)->point_count) ? i : 0;
            const i32* const q = jo::view<i32**>(jo::self<jo::PfdZone>(this)->points)[j];
            const i32 x2 = q[0], y2 = q[1];
            const double t1 = fpu.mul(fpu.sub(bits_f(a1), static_cast<double>(x1)), static_cast<double>(isub(y2, y1)));
            const double t2 = fpu.mul(fpu.sub(bits_f(a2), static_cast<double>(y1)), static_cast<double>(isub(x2, x1)));
            const double side = fpu.sub(t1, t2);
            if (side >= static_cast<double>(zero_c())) continue;  // jump back unless < 0 or NaN

            const jo::PfdZone* const child = jo::self<jo::PfdZone>(this)->links[i - 1].item;
            if (child == nullptr || child->blocked == 1) {
                const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(static_cast<float>(x1)), joan::orig::arg::f32(static_cast<float>(y1)), joan::orig::arg::f32(static_cast<float>(x2)), joan::orig::arg::f32(static_cast<float>(y2)), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4)));  // 0x00664d70
                const float r2 = radius * radius;
                if (!(r2 > d)) continue;
                const jo::RWorldCell* const r = jo::as_rworld_cell(joan::orig::placements::find(img::obj(d::g_placements), joan::orig::arg::i32(global<u32>(d::g_a57bb2)), jo::self<jo::PfdZone>(this)->path_space));  // 0x004a2b20
                bool go = true;
                if (r != nullptr) {
                    if (x1 == x2) {
                        const double xd = static_cast<double>(x1);
                        const double e1 = fpu.sub(r->box[0], static_cast<double>(r->x));
                        go = xd < e1 || xd > e1;
                        if (go) {
                            const double e2 = fpu.sub(r->box[3], static_cast<double>(r->x));
                            go = xd < e2 || xd > e2;
                        }
                    } else if (y1 == y2) {
                        const double yd = static_cast<double>(y1);
                        const double e1 = fpu.sub(r->box[1], static_cast<double>(r->y));
                        go = yd < e1 || yd > e1;
                        if (go) {
                            const double e2 = fpu.sub(r->box[4], static_cast<double>(r->y));
                            go = yd < e2 || yd > e2;
                        }
                    }
                }
                if (!go) continue;
                out1->x = static_cast<float>(x1);
                out1->y = static_cast<float>(y1);
                out1->z = static_cast<float>(jo::view<i32**>(jo::self<jo::PfdZone>(this)->points)[i - 1][2]);
                out2->x = static_cast<float>(x2);
                out2->y = static_cast<float>(y2);
                const i32 j2 = (i != jo::self<jo::PfdZone>(this)->point_count) ? i : 0;
                out2->z = static_cast<float>(jo::view<i32**>(jo::self<jo::PfdZone>(this)->points)[j2][2]);
                float l30 = 0.0f, l28 = 0.0f, l2c = 0.0f;  // all written by 0x00664c60
                V3r o1, o2;
                std::memcpy(&o1, out1, sizeof o1);
                std::memcpy(&o2, out2, sizeof o2);
                const i32 hit = joan::orig::tri_normal_2d(joan::orig::arg::f32(va.x), joan::orig::arg::f32(va.y), joan::orig::arg::f32(va.z), joan::orig::arg::f32(vb.x), joan::orig::arg::f32(vb.y), joan::orig::arg::f32(vb.z), joan::orig::arg::f32(o1.x), joan::orig::arg::f32(o1.y), joan::orig::arg::f32(o1.z), joan::orig::arg::f32(o2.x), joan::orig::arg::f32(o2.y), joan::orig::arg::f32(o2.z), &l30, &l28, &l2c);  // 0x00664c60
                if (hit == 0) continue;
                if (!(l28 >= zero_c())) continue;
                joan::orig::as_pfd_zone(this)->offset_points(joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4), joan::orig::arg::f32(static_cast<float>(x1)), joan::orig::arg::f32(static_cast<float>(y1)), joan::orig::arg::f32(static_cast<float>(x2)), joan::orig::arg::f32(static_cast<float>(y2)), joan::orig::arg::mptr(out1), joan::orig::arg::mptr(out2));  // 0x00672e40
                jo::self<jo::PfdZone>(this)->visit_stamp = 0;
                return false;
            } else {
                const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(static_cast<float>(x1)), joan::orig::arg::f32(static_cast<float>(y1)), joan::orig::arg::f32(static_cast<float>(x2)), joan::orig::arg::f32(static_cast<float>(y2)), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4)));  // 0x00664d70
                const float r2 = radius * radius;
                if (!(r2 > d)) continue;
                jo::self<jo::PfdZone>(this)->visit_stamp = id;
                const u8 ok = joan::orig::arg::as<u8>(joan::orig::as_pfd_zone(jo::view<void**>(jo::self<jo::PfdZone>(this)->links)[i - 1])->find_crossing(joan::orig::arg::u32(a1), joan::orig::arg::u32(a2), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), joan::orig::arg::mptr(out1), joan::orig::arg::mptr(out2), joan::orig::arg::i32(id), joan::orig::arg::f32(radius)));  // 0x00672500
                if (ok == 0) {
                    jo::self<jo::PfdZone>(this)->visit_stamp = 0;
                    return false;
                }
            }
        }
        jo::self<jo::PfdZone>(this)->visit_stamp = id;
        return true;
    }
};

#define B109(name, addr, fn) JOAN_REPLACE("game/b109_20", name, addr, fn)
B109("PathPolygon::is_inside_polygon4", 0x00672500, &H::find_crossing);
#undef B109

}  // namespace game_b109_20
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00672e40 segment_offset_points(...)                    thiscall, ret 0x28
//
//  - 0x00672e40: all x87 ops have float operands -> C++ float; NaN goes the way of the
//    original compares; point copies are dword copies.
namespace {
namespace game_b92_05 {

JOAN_COUNTER(c_offset_points, "segment_offset_points");

struct H {
    // 0x00672e40
    void offset_points(float px, float py, float ax, float ay, float bx, float by, float cx, float cy, float* o1, float* o2) {
        c_offset_points.hit();
        std::uint32_t ub[2], uc[2], ua[2];
        std::memcpy(&ua[0], &ax, 4); std::memcpy(&ua[1], &ay, 4);
        std::memcpy(&ub[0], &bx, 4); std::memcpy(&ub[1], &by, 4);
        std::memcpy(&uc[0], &cx, 4); std::memcpy(&uc[1], &cy, 4);
        std::uint32_t* const q1 = reinterpret_cast<std::uint32_t*>(o1);
        std::uint32_t* const q2 = reinterpret_cast<std::uint32_t*>(o2);
        const float s8 = (cx - bx) * (cx - bx) + (cy - by) * (cy - by);
        const float sc = (ax - bx) * (cx - bx) + (ay - by) * (cy - by);
        if (sc >= 0.0f && !(sc > s8)) {
            q1[0] = ub[0]; q1[1] = ub[1];
            q2[0] = uc[0]; q2[1] = uc[1];
            return;
        }
        const float s10 = (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
        const float s14 = (ax - cx) * (ax - cx) + (ay - cy) * (ay - cy);
        q1[0] = ua[0]; q1[1] = ua[1];
        float ex, ey;
        if (!(s10 >= s14)) {  // s10 < s14 or unordered
            ex = bx - o1[0];
            ey = by - o1[1];
        } else {
            ex = cx - o1[0];
            ey = cy - o1[1];
        }
        const float cross = (ax - px) * ey - (ay - py) * ex;
        if (cross >= 0.0f) {
            o2[0] = ey + o1[0];
            o2[1] = -ex + o1[1];
        } else {  // < 0 or unordered
            o2[0] = -ey + o1[0];
            o2[1] = ex + o1[1];
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("PathPolygon::calculate_along_edge_direction", 0x00672e40, &H::offset_points);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
//
//    - Vec2i::cross 0x0066a400, stub_zero 0x0066a5e0, PathBox::set 0x0066e640,
//      PathBox::set_end 0x0066e720, List::count 0x0066e8a0, List::from_end 0x0066e8d0,
//      Polygon::min_distance 0x006730d0 (uti3d/pathsp.cpp), Pair::ctor 0x0067a4a0,
//      Holder::inner 0x0067ac80, Obj9dcaa0::dtor 0x0067d970, Flag8::get 0x0067ef70;
namespace {
namespace game_b90_06 {


JOAN_COUNTER(c_polymin, "Polygon::min_distance");

struct H {
    // 0x006730d0 Polygon::min_distance(a, b): smallest edge distance (0x00664d70) over the
    // closed polygon (+0x28 count, +0x30 int point pointers), starting at 1e10.
    float poly_min_distance(std::uint32_t a, std::uint32_t b) {
        c_polymin.hit();
        float best;
        const std::uint32_t bits = 0x501502f9u;  // 1e10f
        std::memcpy(&best, &bits, 4);
        for (std::int32_t i = 1; i <= jo::self<jo::PfdZone>(this)->point_count; ++i) {
            std::int32_t** const pts = jo::view<std::int32_t**>(jo::self<jo::PfdZone>(this)->points);
            const std::int32_t* const p = pts[i - 1];
            const std::int32_t px = p[0], py = p[1];
            const std::int32_t j = (i - jo::self<jo::PfdZone>(this)->point_count) != 0 ? i : 0;
            const std::int32_t* const q = jo::view<std::int32_t**>(jo::self<jo::PfdZone>(this)->points)[j];
            const std::int32_t qx = q[0], qy = q[1];
            const float d = joan::orig::arg::as<float>(joan::orig::point_segment_dist2(joan::orig::arg::f32(static_cast<float>(px)), joan::orig::arg::f32(static_cast<float>(py)), joan::orig::arg::f32(static_cast<float>(qx)), joan::orig::arg::f32(static_cast<float>(qy)), joan::orig::arg::f32(a), joan::orig::arg::f32(b)));  // 0x00664d70
            if (best > d) best = d;
        }
        return best;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("PathPolygon::find_min_dist2", 0x006730d0, &H::poly_min_distance);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
//
//    - Path::find_6731d0 0x006731d0 (ret 0x24): the block 0x006733ed..0x006735c1 is
//      unreachable in the original (only reached from itself) and is omitted. SEH frame for
//      new+ctor: normal path only.
namespace {
namespace game_b108_11 {

using S = jo::PathSpace;  // `this` (0x006745e0 build_bvh on it; +0x14 zones, +0x18 tree, +0x1c items)

JOAN_COUNTER(c_path_find, "Path::find_6731d0");

struct H {
    // ------------------------------------------------------------------ 0x006731d0
    void path_find(float x, float y, float lim, u32 a14, u32* best_lo, float* d_lo, u32* best_hi, float* d_hi,
                   u32 flag) {
        c_path_find.hit();
        *best_lo = 0;
        *best_hi = 0;
        if (jo::self<S>(this)->zones == nullptr) return;
        joan::orig::as_path_space(this)->build_bvh();  // 0x006745e0
        if (jo::self<S>(this)->tree == nullptr) return;
        if (!(joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::self<S>(this)->tree)->get_2c()) > 0)) return;  // 0x0066b7a0
        if (jo::self<S>(this)->items == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
            void* const l = mem != nullptr ? joan::orig::dynarray(mem)->construct(static_cast<std::int32_t>(sizeof(void*)), i32{0x1e}, i32{0}) : nullptr;  // 0x005843d0 (pointers: 4 on x86)
            jo::self<S>(this)->items = static_cast<jo::DynArray*>(l);
        }
        joan::orig::dynarray(jo::self<S>(this)->items)->zap(i32{0});  // 0x00584580
        alignas(4) u8 p18[8];
        alignas(4) u8 p20[8];
        {
            const i32 py = joan::hook::x87::ftol(static_cast<double>(y - 1.0f));
            const i32 px = joan::hook::x87::ftol(static_cast<double>(x - 1.0f));
            joan::orig::as_point(p18)->set(joan::orig::arg::u32(px), joan::orig::arg::u32(py));  // 0x004eeb80
        }
        {
            const i32 py = joan::hook::x87::ftol(static_cast<double>(y + 1.0f));
            const i32 px = joan::hook::x87::ftol(static_cast<double>(x + 1.0f));
            joan::orig::as_point(p20)->set(joan::orig::arg::u32(px), joan::orig::arg::u32(py));
        }
        void* const zone = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::self<S>(this)->tree)->item(i32{1}));  // 0x00637da0
        joan::orig::as_bvh_node(zone)->query_xy(joan::orig::arg::mptr(static_cast<void*>(p18)), joan::orig::arg::mptr(static_cast<void*>(p20)), joan::orig::arg::mptr(// 0x006751a0
                      jo::self<S>(this)->items));
        for (i32 i = joan::orig::as_dynarray(jo::self<S>(this)->items)->size(); i > 0; --i) {  // 0x00417ad0
            void* const q = joan::orig::as_dynarray(jo::self<S>(this)->items)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (joan::orig::arg::as<u8>(joan::orig::as_pfd_zone(q)->contains(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(a14), static_cast<char>(joan::orig::arg::i32(flag)))) == 0) continue;     // 0x006718c0
            const float d = joan::orig::arg::as<float>(joan::orig::as_terrain_block(q)->height(joan::orig::arg::f32(x), joan::orig::arg::f32(y)));                // 0x00664180
            if (!(*d_hi >= d) || *best_hi == 0) {
                if (!(d > lim)) {
                    *best_hi = jo::view<u32>(jo::as_pfd_zone(q)->id);
                    *d_hi = d;
                    continue;
                }
            }
            if (!(*d_lo > d) && *best_lo != 0) continue;
            if (lim >= d) continue;  // jump when neither "lim < d" nor unordered
            *best_lo = jo::view<u32>(jo::as_pfd_zone(q)->id);
            *d_lo = d;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("PathSpace::generate_offsetted_path", 0x006731d0, &H::path_find);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
//
//      0x006736e0 Polygon::segment_cut(...)     (thiscall, ret 0x1c) - returns 0 / 1 / 2.
//
//  - 0x006736e0: fild / fidiv -> Fpu{true}.div (x/0 gives the same inf / default NaN). The
namespace {
namespace game_b94_07 {


// Two dwords passed by value (P2::sub 0x006639e0 takes its points as structs).
struct XY {
    i32 x, y;
};
static_assert(sizeof(XY) == 8, "XY");

JOAN_COUNTER(c_segment_cut, "Polygon::segment_cut");

struct H {
    // ------------------------------------------------------------------ 0x006736e0
    i32 segment_cut(i32* out, void* pt, i32 key, i32 x, i32 y, i32 x2, i32 y2) {
        c_segment_cut.hit();
        jo::PfdZone* const poly = jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(key)));  // 0x00637da0
        *out = 0;
        float lo = 1e10f;   // [ebp-0x10] 0x501502f9
        float hi = -1e10f;  // [ebp-0x14] 0xd01502f9
        for (i32 j = 1; j <= poly->point_count; ++j) {
            const i32 i1 = j - 1;
            const i32 i2 = j != poly->point_count ? j : 0;
            i32 tmp1[2];
            const i32* const q1 = joan::orig::arg::as<const i32*>(joan::orig::as_vec2i(poly->points[i1].item)->copy_to(joan::orig::arg::mptr(static_cast<void*>(tmp1))));  // 0x0066a4f0
            const i32 p1x = q1[0];
            const i32 p1y = q1[1];
            i32 tmp2[2];
            const i32* const q2 = joan::orig::arg::as<const i32*>(joan::orig::as_vec2i(poly->points[i2].item)->copy_to(joan::orig::arg::mptr(static_cast<void*>(tmp2))));  // 0x0066a4f0
            const i32 p2x = q2[0];
            const i32 p2y = q2[1];
            i32 rel[2];  // [ebp-0x44], never read
            joan::orig::as_point(static_cast<void*>(rel))->set(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(x) - static_cast<u32>(p1x))), joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(y) - static_cast<u32>(p1y))));  // 0x004eeb80
            i32 den = 0;  // [ebp-0x24]
            i32 num = 0;  // [ebp-0x1c]
            i32 par = 0;  // [ebp-0x20]
            const i32* const b = poly->points[i2].item->xyz;
            const i32 b0 = b[0];
            const i32 b1 = b[1];
            const i32* const a = poly->points[i1].item->xyz;
            const i32 a0 = a[0];
            const i32 a1 = a[1];
            const u32 hit = joan::orig::arg::as<u32>(joan::orig::line_intersect_terms(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::i32(a0), joan::orig::arg::i32(a1), joan::orig::arg::i32(b0), joan::orig::arg::i32(b1), &den, &num, &par));  // 0x00664b70
            if (hit == 0 || par < 0 || par > den) continue;
            const float t = static_cast<float>(Fpu{true}.div(static_cast<double>(num), static_cast<double>(den)));
            const i32 c1 = static_cast<i32>((static_cast<u32>(x2) - static_cast<u32>(x)) *
                                            (static_cast<u32>(p2y) - static_cast<u32>(p1y)));
            const i32 c2 = static_cast<i32>((static_cast<u32>(y2) - static_cast<u32>(y)) *
                                            (static_cast<u32>(p2x) - static_cast<u32>(p1x)));
            const i32 cross = static_cast<i32>(static_cast<u32>(c1) - static_cast<u32>(c2));
            if (cross < 0) {
                lo = t;
            } else {
                hi = t;
                const i32* const entry = jo::view<const i32*>(poly->links[j - 1].item);
                if (entry != nullptr) *out = *entry;
            }
        }
        if (!(hi > lo)) return 0;                                // test ah,0x41 jne
        if (!(hi >= global<float>(d::k_9cf5c4))) return 2;        // < 0 or NaN
        if (lo > global<float>(d::flt_1)) return 2;            // lo > 1
        if (hi > global<float>(d::flt_1)) return 0;            // hi > 1
        i32 l64[2];
        const i32* const d = joan::orig::arg::as<const i32*>(joan::orig::p2::sub(joan::orig::arg::mptr(l64), XY{x2, y2}, XY{x, y}));  // 0x006639e0
        const i32 d0 = d[0];
        const i32 d1 = d[1];
        i32 l6c[2];
        const i32* const s = joan::orig::arg::as<const i32*>(joan::orig::point::scale(joan::orig::arg::mptr(static_cast<void*>(l6c)), joan::orig::arg::i32(d0), joan::orig::arg::i32(d1), joan::orig::arg::f32(hi)));  // 0x00663a30
        const i32 s0 = s[0];
        const i32 s1 = s[1];
        i32 l74[2];
        void* const r = joan::orig::arg::as<void*>(joan::orig::point::add(joan::orig::arg::mptr(static_cast<void*>(l74)), joan::orig::arg::i32(s0), joan::orig::arg::i32(s1), joan::orig::arg::i32(x), joan::orig::arg::i32(y)));  // 0x00663990
        joan::orig::as_vec2i(pt)->assign(joan::orig::arg::mptr(r));                                                    // 0x0066a380
        return 1;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("PathSpace::get_polygon_exit", 0x006736e0, &H::segment_cut);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
//
//    - PathSp::cross_segment 0x006739c0 (u32* out_id, vec3* out, int path, vec3 a, vec3 b)
//      -> int, ret 0x24 (no references: dead): intersects a..b with every edge of path
//      0x00637da0(path) (2D, 0x00664c60), keeps the min / max hit ratio and the id of the
//      edge with the max; 0 / 2 / 0 / 1 + out = a + (b - a) * max.
//
//  - 0x006739c0 passes the two edge points by value with an uninitialised z ([ebp-0x3c],
//    [ebp-0x30]); 0x00664c60 never reads the z components (args +0x10 / +0x1c / +0x28 / +0x34),
//    so 0 is passed.
namespace {
namespace game_b101_14 {


struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_cross, "PathSp::cross_segment");

struct H {
    // ------------------------------------------------------------------ 0x006739c0
    i32 cross_segment(u32* out_id, Vec3* out, i32 key, Vec3 a, Vec3 b) {
        c_cross.hit();
        const jo::PfdZone* const path = jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(key)));  // 0x00637da0
        *out_id = 0;
        float hi = 1e10f;   // 0x501502f9
        float lo = -1e10f;  // 0xd01502f9
        for (i32 i = 1; i <= path->point_count; ++i) {
            const i32 prev = i - 1;
            const i32 next = (i - path->point_count) != 0 ? i : 0;
            Vec3 p0{static_cast<float>(path->points[prev].item->xyz[0]), 0.0f, 0.0f};
            p0.y = static_cast<float>(path->points[prev].item->xyz[1]);
            Vec3 p1{static_cast<float>(path->points[next].item->xyz[0]), 0.0f, 0.0f};
            p1.y = static_cast<float>(path->points[next].item->xyz[1]);
            float t = 0.0f, u = 0.0f, v = 0.0f;  // [ebp-0x24], [ebp-0x1c], [ebp-0x20]
            const i32 hit = joan::orig::tri_normal_2d(a.x, a.y, a.z, b.x, b.y, b.z, p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, &t, &u, &v);  // 0x00664c60
            if (hit == 0) continue;
            if (0.0f > v) continue;  // test ah,0x41 je
            if (v > t) continue;
            const float r = u / t;
            if (hi > r) hi = r;
            if (lo >= r) continue;  // test ah,1 je: taken unless lo < r or unordered
            lo = r;
            const u32* const id = jo::view<const u32*>(path->links[i - 1].item);
            if (id != nullptr) *out_id = *id;
        }
        if (!(lo > hi)) return 0;   // test ah,0x41 jne
        if (!(lo >= 0.0f)) return 2;  // test ah,1 jne: lo < 0 or unordered
        if (hi > 1.0f) return 2;
        if (lo > 1.0f) return 0;
        Vec3 t1, t2, t3;
        const Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&t1, &b, &a));  // 0x0060cc90 (b - a)
        const Vec3 s = *joan::orig::arg::as<Vec3*>(joan::orig::vec_scale(&t2, &d, joan::orig::arg::f32(lo)));  // 0x0060cd00 (d * lo)
        const Vec3 r = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&t3, &s, &a));  // 0x0060cc20 (s + a)
        std::memcpy(out, &r, sizeof r);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("PathSpace::get_polygon_exit2", 0x006739c0, &H::cross_segment);
#undef B

}  // namespace game_b101_14
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
//
//    - PathSpace::find_best_673e20 0x00673e20 (ret 0x20, recursive; no outside caller -> dead code).
namespace {
namespace game_b108_20 {


JOAN_COUNTER(c_best_673e20, "PathSpace::find_best_673e20");

struct Vec12 {
    u32 a, b, c;
};
static_assert(sizeof(Vec12) == 12, "by-value vector");

struct H {
    // -------------------------------------------------------------- 0x00673e20
    void find_best(i32* out, i32 id, Vec12 p, i32 depth, i32 max_depth, float* best) {
        c_best_673e20.hit();
        jo::PfdZone* const node = jo::as_pfd_zone(joan::orig::as_billboard(jo::self<jo::PathSpace>(this)->zones)->item(joan::orig::arg::i32(id)));  // 0x00637da0
        const float d = joan::orig::arg::as<float>(joan::orig::as_pfd_zone(node)->min_distance(joan::orig::arg::u32(p.a), joan::orig::arg::u32(p.b)));           // 0x006730d0
        if (*best > d) {
            *best = d;
            *out = id;
        }
        if (depth == max_depth) return;
        for (i32 i = 1; i <= node->point_count; ++i) {
            void* const child = node->links[i - 1].item;
            if (child != nullptr) joan::orig::as_path_space(this)->find_best(joan::orig::arg::mptr(out), joan::orig::arg::i32(jo::as_pfd_zone(child)->id), p, joan::orig::arg::i32(depth + 1), joan::orig::arg::i32(max_depth), joan::orig::arg::mptr(best));
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("PathSpace::find_nearest_polygon_from_cur", 0x00673e20, &H::find_best);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
//
//    - Path::smooth_673f30 0x00673f30 (thiscall, 7 args, ret 0x1c) -> eax. Dead code (no
//      references at all).
//
//  - 0x00673f30: the out point [ebp-0x4c] is compared before 0x006736e0 ever wrote it (it writes
//    it only when it returns 1) -> an unwritten point never equals the current point. Once
//    written it keeps its value across iterations like the original stack slot.
namespace {
namespace game_b115_10 {

using joan::hook::x87::ftol;

using joan::orig::helpers::addr;
using joan::orig::helpers::fild;

constexpr Fpu kFpu{true};

// Two dwords passed by value (P2::sub 0x006639e0 takes its points as structs).
struct XY {
    i32 x, y;
};
static_assert(sizeof(XY) == 8, "XY");

JOAN_COUNTER(c_smooth, "Path::smooth_673f30");

struct H {
    // ===================================================================== 0x00673f30
    std::uintptr_t smooth(void* out, void* in, i32 dx, i32 dy, u32 /*a18*/, i32 r1c, u32 /*a20*/) {
        c_smooth.hit();
        auto pt = [](void* path, i32 i) { return joan::orig::arg::as<float*>(joan::orig::as_list(path)->from_end(joan::orig::arg::i32(i))); };  // 0x0066e8d0
        auto cnt = [](void* path) { return joan::orig::arg::as<i32>(joan::orig::as_list(path)->count_66e8a0()); };            // 0x0066e8a0
        auto set2 = [](i32* dst, const i32* src) { joan::orig::as_vec2i(dst)->assign(joan::orig::arg::mptr(src)); };  // 0x0066a380
        auto ctor2 = [](i32* p, i32 x, i32 y) { joan::orig::as_point(p)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y)); };     // 0x004eeb80
        i32 p0[2] = {0, 0};
        i32 p1[2] = {0, 0};
        i32 pc[2] = {0, 0};
        {
            const float* const q0 = pt(in, 0);
            const i32 y = ftol(kFpu.add(fild(dy), q0[1]));
            const i32 x = ftol(kFpu.add(fild(dx), q0[0]));
            ctor2(p0, x, y);
            const float* const q1 = pt(in, 1);
            const i32 y1 = ftol(kFpu.add(fild(dy), q1[1]));
            const i32 x1 = ftol(kFpu.add(fild(dx), q1[0]));
            ctor2(p1, x1, y1);
            ctor2(pc, 0, 0);
            joan::orig::as_path(out)->clear();  // 0x004eeca0
            const i32 z1 = ftol(q1[2]);
            const i32 z0 = ftol(q0[2]);
            joan::orig::as_path_box(out)->set(joan::orig::arg::i32(p0[0]), joan::orig::arg::i32(p0[1]), joan::orig::arg::i32(z0), joan::orig::arg::i32(p1[0]), joan::orig::arg::i32(p1[1]), joan::orig::arg::i32(z1));  // 0x0066e640
        }
        bool written = false;  // [ebp-0x2c]
        i32 p3c[2] = {0, 0};   // persists like the stack slot (always written before use)
        i32 p4c[2] = {0, 0};   // fix: compared before 0x006736e0 ever wrote it
        bool p4c_set = false;
        i32 v40 = 0;
        for (i32 i = 1; i <= cnt(in) + 1; ++i) {
            joan::orig::as_point(p3c)->construct();  // 0x004eea60
            if (i <= cnt(in)) {
                const float* const q = pt(in, i);
                p3c[0] = ftol(kFpu.add(fild(dx), q[0]));
                p3c[1] = ftol(kFpu.add(fild(dy), q[1]));
            }
            bool to_tail = false;
            {
                i32 d[2] = {0, 0};
                const i32* const dv = joan::orig::arg::as<i32*>(joan::orig::p2::sub(joan::orig::arg::mptr(d), XY{p3c[0], p3c[1]}, XY{p1[0], p1[1]}));  // 0x006639e0
                if (joan::orig::arg::as<i32>(joan::orig::as_vec2i(pc)->cross(joan::orig::arg::i32(dv[0]), joan::orig::arg::i32(dv[1]))) == 0 && i != cnt(in)) to_tail = true;  // 0x0066a400
            }
            if (!to_tail) {
                joan::orig::as_point(p4c)->construct();  // 0x004eea60
                bool cont = true;  // [ebp-0x50]
                for (;;) {
                    if (p0[0] == p1[0] && p0[1] == p1[1]) break;
                    if (r1c == 0) break;
                    const jo::PathPoint* const m28 = jo::as_path_point(joan::orig::as_path(out)->first());  // 0x0066e940
                    const i32 res = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->segment_cut(&v40, joan::orig::arg::mptr(p4c), joan::orig::arg::i32(r1c), joan::orig::arg::i32(p0[0]), joan::orig::arg::i32(p0[1]), joan::orig::arg::i32(p1[0]), joan::orig::arg::i32(p1[1])));  // 0x006736e0
                    if (res == 1) p4c_set = true;
                    if (p4c_set && p0[0] == p4c[0] && p0[1] == p4c[1]) {
                        cont = false;
                        break;
                    }
                    if (res == 0 || res == 1) {
                        if (written)
                            joan::orig::as_path(out)->add_segment(joan::orig::arg::i32(jo::view<u32>(m28->a[0])), joan::orig::arg::i32(jo::view<u32>(m28->a[1])), joan::orig::arg::i32(jo::view<u32>(m28->a[2])), joan::orig::arg::i32(jo::view<u32>(m28->b[0])), joan::orig::arg::i32(jo::view<u32>(m28->b[1])), joan::orig::arg::i32(jo::view<u32>(m28->b[2])));  // 0x0066e790
                        void* const ter = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::self<jo::PathSpace>(this)->zones)->item(joan::orig::arg::i32(r1c)));  // 0x00637da0
                        const i32* const q = res == 0 ? p1 : p4c;
                        const float fx = static_cast<float>(q[0]);
                        const float fy = static_cast<float>(q[1]);
                        const i32 z = ftol(joan::orig::arg::as<double>(joan::orig::as_terrain_block(ter)->height(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy))));  // 0x00664180
                        joan::orig::as_path_box(out)->set_end(joan::orig::arg::i32(q[0]), joan::orig::arg::i32(q[1]), joan::orig::arg::i32(z));                   // 0x0066e720
                        if (res == 1) r1c = v40;
                        written = true;
                        set2(p0, q);
                    } else if (res == 2) {
                        r1c = 0;
                    }
                }
                if (!cont || r1c == 0) break;
            }
            if (i <= cnt(in)) {
                set2(p1, p3c);
                i32 d[2] = {0, 0};
                const i32* const dv = joan::orig::arg::as<i32*>(joan::orig::p2::sub(joan::orig::arg::mptr(d), XY{p1[0], p1[1]}, XY{p0[0], p0[1]}));  // 0x006639e0
                set2(pc, dv);
            }
        }
        for (i32 i = 1; i <= (cnt(out) - 1) / 2; ++i) {
            float* const a = pt(out, i);
            float* const b = pt(out, cnt(out) - i);
            a[0] = static_cast<float>(jo::as_path_point(a)->a[0]);
            a[1] = static_cast<float>(jo::as_path_point(a)->a[1]);
            a[2] = static_cast<float>(jo::as_path_point(a)->a[2]);
            b[0] = static_cast<float>(jo::as_path_point(b)->a[0]);
            b[1] = static_cast<float>(jo::as_path_point(b)->a[1]);
            b[2] = static_cast<float>(jo::as_path_point(b)->a[2]);
            u8 tmp[0x24];
            std::memcpy(tmp, a, 0x24);
            std::memcpy(a, b, 0x24);
            std::memcpy(b, tmp, 0x24);
        }
        const i32 rem = cnt(out) % 2;
        if (rem != 0) return static_cast<u32>(rem);
        float* const mid = pt(out, cnt(out) / 2);
        mid[0] = static_cast<float>(jo::as_path_point(mid)->a[0]);
        mid[1] = static_cast<float>(jo::as_path_point(mid)->a[1]);
        mid[2] = static_cast<float>(jo::as_path_point(mid)->a[2]);
        return addr(mid);
    }
};

// 0x00673f30: out point compared before ever written -> treated as different (dead code).
JOAN_REPLACE("fix/uninit", "PathSpace::find_polygon_673f30", 0x00673f30, &H::smooth);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
//
//      0x00674590 PathSp::flush()                        - uti3d/pathsp.cpp
namespace {
namespace game_b84_04 {


struct H {
    void pathsp_flush() {  // 0x00674590
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->tree) == nullptr) return;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->tree))->get_2c()) <= 0) return;  // 0x0066b7a0
        joan::orig::as_blk_arr(jo::view<void*>(jo::self<jo::PathSpace>(this)->tree))->reset();                                  // 0x00637bb0
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("PathSpace::invalidate_polygon_tree_struct", 0x00674590, &H::pathsp_flush);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
//
//    - PathSp::build_6745e0 0x006745e0 (ret 0): SEH frame (normal path): new 0x30 + ctor
//      0x00637aa0(0x38, 0x800) when +0x18 is null, table [0xa6a278] alloc / fill / free around
//      0x006747a0(0, 0, n). Returns the last eax.
namespace {
namespace game_b108_04 {

using i16 = std::int16_t;

using joan::img::str;

JOAN_COUNTER(c_pathsp, "PathSp::build_6745e0");

struct H {
    // ------------------------------------------------------------------ 0x006745e0
    u32 build_6745e0() {
        c_pathsp.hit();
        if (jo::self<jo::PathSp>(this)->tree != nullptr) {
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::self<jo::PathSp>(this)->tree)->get_2c());  // 0x0066b7a0
            if (r > 0) return static_cast<u32>(r);
        }
        {
            const i32 r = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->zone_count());  // 0x0066b9f0
            if (!(r > 0)) return static_cast<u32>(r);
        }
        const i32 count = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->zone_count());
        if (jo::self<jo::PathSp>(this)->tree == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::BlkArr)));  // 0x30 on x86; operator new
            jo::BlkArr* made = nullptr;
            if (mem != nullptr) made = joan::orig::arg::as<jo::BlkArr*>(joan::orig::as_blk_arr(mem)->construct(joan::orig::arg::u32(static_cast<i32>(sizeof(jo::BoxTree))), joan::orig::arg::u32(i32{0x800})));  // 0x00637aa0 (the BoxTree nodes Bvh::build_6747a0 writes: 0x38 bytes on x86)
            jo::self<jo::PathSp>(this)->tree = made;
        }
        joan::orig::as_blk_arr(jo::self<jo::PathSp>(this)->tree)->reset();  // 0x00637bb0
        // [0xa6a278]: the zone pointers handed to Bvh::build_6747a0 (game_b109_02), one per zone.
        global<void*>(d::g_a6a278) = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<u32>(count) * static_cast<u32>(sizeof(jo::PfdZone*))), img::str(d::s_d_ww_uti3d_pathsp_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2167c)) + 0xc));  // 0x0058e7b0
        for (i32 i = 1; i <= count; ++i) {
            jo::PfdZone* const v = jo::as_pfd_zone(joan::orig::as_billboard(jo::self<jo::PathSp>(this)->zones)->item(joan::orig::arg::i32(i)));  // 0x00637da0
            static_cast<jo::PfdZone**>(global<void*>(d::g_a6a278))[i - 1] = v;
        }
        joan::orig::as_path_space(this)->build_bvh_range(nullptr, i32{0}, joan::orig::arg::i32(count));  // 0x006747a0
        joan::orig::as_mem(obj(d::g_mem))->del_58ed70(joan::orig::arg::mptr(global<void*>(d::g_a6a278)), img::str(d::s_d_ww_uti3d_pathsp_cpp),
                                                       static_cast<i32>(global<i16>(d::g_a2167c)) + 0x16);  // 0x0058ed70
        global<void*>(d::g_a6a278) = nullptr;
        return 0;  // void callee; all 5 callers of PathSp::build (0x006745e0) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("PathSpace::build_polygon_tree_struct", 0x006745e0, &H::build_6745e0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
//
//  * module game/b82_14:
//      Video::read_sample 0x00685dd0 (ret 0xc): IWMSyncReader::GetNextSample on +0x24; a video
//        sample (stream +0xc) is copied bottom-up into the +4 texture (+0x58, LockRect /
//        UnlockRect), an audio sample (stream +0xe) is wrapped in a sound buffer (0x0048d120)
//        and queued on the +8 stream; *time = sample time / 10000. Returns 0 (no sample),
//        1 video, 2 audio, -1 other stream.
//      Video::has_audio_stream 0x00685990, Video::has_video_stream 0x00685940,
//      Video::has_audio 0x00685a30, Video::has_video 0x006859e0.
//      Zone::set_points 0x00663cb0 (ret 0xc; copies the point pointer list, bounding box),
//      Zone::link 0x00664130 (ret 0xc), PathZones::group 0x00675660 (connected components of
//      the zone links into +0x70), PathSpace::zone_count 0x0066b9f0,
//      PfdData::clear_grid 0x00665290, PfdData::free_10 0x00666d80,
//      PfdItem::scalar_deleting_destructor 0x006651e0 (ret 4),
//      Zone::vector_deleting_destructor 0x00675d70 (ret 4), Zones::~Zones 0x00675930,
//      Container::clear 0x00677e00.
//      Collisions::add 0x0068dbf0 (ret 0x14), Collisions::remove 0x0068dd80 (ret 8).
//      LensFlares::acquire_texture 0x00662d20, LensFlares::release_texture 0x00662db0,
//      FlareEntry::FlareEntry 0x00662800, FlareEntry::operator= 0x00662830 (ret 4).
//      List::~List 0x006893a0, Entry::~Entry 0x0068c800 (empty), Entry::flag 0x0068c940.
namespace {
namespace game_b82_14 {

using joan::call::thiscall;
using joan::call::cdecl_call;

JOAN_COUNTER(c_set_points, "Zone::set_points");
JOAN_COUNTER(c_group, "PathZones::group");

struct H {

    // 0x00663cb0 Zone::set_points(points, count, flag) (ret 0xc).
    void set_points(const void* points, std::int32_t count, std::uint16_t flag) {
        c_set_points.hit();
        jo::self<jo::PfdZone>(this)->point_count = count;
        jo::view<void*>(jo::self<jo::PfdZone>(this)->points) = joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::PfdZone>(this)->point_count) * sizeof(void*)));  // new
        jo::view<void*>(jo::self<jo::PfdZone>(this)->links) = joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::PfdZone>(this)->point_count) * sizeof(void*)));
        std::memcpy(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PfdZone>(this)->points)), joan::orig::arg::ptr(points), joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::PfdZone>(this)->point_count) * sizeof(void*)));  // memcpy
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PfdZone>(this)->links)), 0, joan::orig::arg::u32(static_cast<std::uint32_t>(jo::self<jo::PfdZone>(this)->point_count) * sizeof(void*)));       // memset
        jo::self<jo::PfdZone>(this)->flags = flag;
        if (count <= 0) return;
        joan::orig::as_triple(jo::self<jo::PfdZone>(this)->bbox_min)->assign(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PfdZone>(this)->points->item)));     // 0x00663f30
        joan::orig::as_triple(jo::self<jo::PfdZone>(this)->bbox_max)->assign(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PfdZone>(this)->points->item)));  // 0x00663f30
        for (std::int32_t i = 1; i < count; ++i) {
            if (jo::self<jo::PfdZone>(this)->points[i].item->xyz[0] < jo::self<jo::PfdZone>(this)->bbox_min[0])
                jo::self<jo::PfdZone>(this)->bbox_min[0] = jo::self<jo::PfdZone>(this)->points[i].item->xyz[0];
            if (jo::self<jo::PfdZone>(this)->points[i].item->xyz[0] > jo::self<jo::PfdZone>(this)->bbox_max[0])
                jo::self<jo::PfdZone>(this)->bbox_max[0] = jo::self<jo::PfdZone>(this)->points[i].item->xyz[0];
            if (jo::self<jo::PfdZone>(this)->points[i].item->xyz[1] < jo::self<jo::PfdZone>(this)->bbox_min[1])
                jo::self<jo::PfdZone>(this)->bbox_min[1] = jo::self<jo::PfdZone>(this)->points[i].item->xyz[1];
            if (jo::self<jo::PfdZone>(this)->points[i].item->xyz[1] > jo::self<jo::PfdZone>(this)->bbox_max[1])
                jo::self<jo::PfdZone>(this)->bbox_max[1] = jo::self<jo::PfdZone>(this)->points[i].item->xyz[1];
            if (jo::self<jo::PfdZone>(this)->points[i].item->xyz[2] < jo::self<jo::PfdZone>(this)->bbox_min[2])
                jo::self<jo::PfdZone>(this)->bbox_min[2] = jo::self<jo::PfdZone>(this)->points[i].item->xyz[2];
            if (jo::self<jo::PfdZone>(this)->points[i].item->xyz[2] > jo::self<jo::PfdZone>(this)->bbox_max[2])
                jo::self<jo::PfdZone>(this)->bbox_max[2] = jo::self<jo::PfdZone>(this)->points[i].item->xyz[2];
        }
    }

    // 0x00664130 Zone::link(index, other, other_index) (ret 0xc) -> 1.
    std::int32_t link(std::int32_t index, void* other, std::int32_t other_index) {
        jo::self<jo::PfdZone>(this)->links[index - 1].item = jo::as_pfd_zone(other);
        if (other_index != 0) jo::as_pfd_zone(other)->links[other_index - 1].item = jo::self<jo::PfdZone>(this);
        return 1;
    }

    // 0x00675660 PathZones::group() (ret 0): numbers the connected zone groups into +0x70.
    std::uint32_t group() {
        c_group.hit();
        std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(this)->zone_count());  // 0x0066b9f0
        for (; i > 0; --i) {
            jo::PfdZone* const n = jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(i)));  // 0x00637da0
            n->group = -1;
            n->open_next = nullptr;
        }
        std::int32_t k = joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(this)->zone_count());  // ebp-0x1c
        std::int32_t id = 0;                                       // ebp-0x20
        alignas(8) std::uint8_t sentinel[sizeof(jo::PfdZone) + 4];  // ebp-0x94 (a PfdZone, 0x74 bytes)
        joan::orig::as_zone(sentinel)->construct();                     // 0x00663c50
        for (; k > 0; --k) {
            jo::PfdZone* const n = jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(k)));  // ebp-0x98
            if (n->blocked == 1 || n->group != -1) continue;
            n->open_next = jo::as_pfd_zone(sentinel);
            jo::PfdZone* p = n;  // ebp-0x9c
            while (p != jo::as_pfd_zone(sentinel)) {
                p->group = id;
                for (std::int32_t j = 0; j < p->point_count; ++j) {
                    jo::PfdZone* const q = p->links[j].item;
                    if (q == nullptr) continue;
                    if (q->blocked == 1) continue;
                    if (q->open_next != nullptr) continue;
                    q->open_next = p->open_next;
                    p->open_next = q;
                }
                p = p->open_next;
            }
            ++id;
        }
        joan::orig::as_pfd_zone(sentinel)->destruct();  // 0x00674f50
        return 0;  // void callee; all 3 callers of PathZones::group discard the result
    }

    // 0x0066b9f0 PathSpace::zone_count() (ret 0).
    std::int32_t zone_count() {
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr) return 0;
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->get_2c());  // 0x0066b7a0
    }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("PathPolygon::init", 0x00663cb0, &H::set_points);
B82_14("PathPolygon::connect_polygon", 0x00664130, &H::link);
B82_14("PathSpace::calc_connectivity", 0x00675660, &H::group);
B82_14("PathSpace::get_polygon_count", 0x0066b9f0, &H::zone_count);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
//
//      0x0066b7a0 getter +0x2c, 0x0067a3e0 empty method, 0x006b7d30 sets byte +4 = 1.
namespace {
namespace game_b83_04_2 {


struct H {
    // 0x0066b7a0
    std::int32_t get_2c() { return jo::self<jo::BlkArr>(this)->id_count; }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("PathSpace::get_2c", 0x0066b7a0, &H::get_2c);
#undef B83_04

}  // namespace game_b83_04_2
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
//
//    - Nav::find_region_66bb40 0x0066bb40 (x, y, r), ret 0xc.
//
//  - 0x0066bb40: the code after the first loop (0x0066bcd6..0x0066beff) is unreachable in the
//    original (the loop always returns) and is not reproduced. Verified: every jump into it comes
//    from inside the block, 0x0066bcd1 is an unconditional jmp, and the EH FuncInfo 0x009f0348
//    has no try blocks (no catch continuation); it also reads the never-written local [ebp-0x2c].
//
//  - new (0x007b1d35) + ctor SEH frames (0x0063cdc0, 0x0066bb40, 0x00742470): normal path only.
namespace {
namespace game_b108_08 {

using joan::hook::global;
using joan::hook::x87::ftol;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_find_region, "Nav::find_region_66bb40");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x0066bb40
    i32 find_region_66bb40(float x, float y, float r) {
        c_find_region.hit();
        if (jo::self<jo::Nav>(this)->zones == nullptr) return 0;
        joan::orig::as_path_space(this)->build_bvh();  // 0x006745e0
        if (jo::view<void*>(jo::self<jo::Nav>(this)->tree) == nullptr || !(joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::Nav>(this)->tree))->get_2c()) > 0)) return 0;  // 0x0066b7a0
        if (jo::self<jo::Nav>(this)->items == nullptr) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
            void* const l = mem != nullptr ? joan::orig::dynarray(mem)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x1e, 0) : nullptr;  // 0x005843d0 (pointers: 4 on x86)
            jo::view<void*>(jo::self<jo::Nav>(this)->items) = l;
        }
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::Nav>(this)->items))->zap(0);  // 0x00584580
        const float one = global<float>(d::flt_1);
        u32 lo[2] = {0, 0}, hi[2] = {0, 0};
        joan::orig::as_point(lo)->set(joan::orig::arg::u32(ftol(x - one)), joan::orig::arg::u32(ftol(y - one)));  // 0x004eeb80
        joan::orig::as_point(hi)->set(joan::orig::arg::u32(ftol(x + one)), joan::orig::arg::u32(ftol(y + one)));
        void* const root = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::Nav>(this)->tree))->item(1));  // 0x00637da0
        joan::orig::as_bvh_node(root)->query_xy(joan::orig::arg::mptr(lo), joan::orig::arg::mptr(hi), joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Nav>(this)->items)));                // 0x006751a0
        for (i32 i = joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Nav>(this)->items))->size(); i > 0; --i) {  // 0x00417ad0
            u32* const e = joan::orig::arg::as<u32*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Nav>(this)->items))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if ((joan::orig::arg::as<u32>(joan::orig::as_pfd_zone(e)->contains(x, y, r, 0)) & 0xffu) != 0) return static_cast<i32>(*e);  // 0x006718c0
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("PathSpace::find_polygon_66bb40", 0x0066bb40, &H::find_region_66bb40);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b109_19.cpp (game/b109_19)
//
//    - Region::pick_66c020 0x0066c020 (thiscall, ret 0x10: float x, y, z, int): returns the id
//      (dword +0) of the first candidate cell whose height brackets z within +-16, or 0.
//      SEH frame: normal path only (operator new 0x1d + ctor 0x005843d0).
//
//  - 0x0066c020: 0x004eeb80 (ret 8) takes only two of the three pushed ints; the third belongs
//    to 0x0066a4b0 (ret 0xc). Same for 0x00637da0 (ret 4) / 0x006752c0 (ret 0xc). The block
//    0x0066c271..0x0066c591 is unreachable (no jump into it; its counter [ebp-0x3c] is never
//    initialised) and is omitted; the dead store [ebp-0x14] = 0x10 too. The height of 0x00664180
//    goes straight from st0 to _ftol (one of its paths returns an unrounded fild): read as double.
namespace {
namespace game_b109_19 {

using joan::hook::global;
using joan::hook::x87::ftol;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
i32 ftolf(float f) { return ftol(static_cast<double>(f)); }

i32 list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
u8* list_get(const void* list, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i))); }  // 0x0041b3f0

JOAN_COUNTER(c_pick, "Region::pick_66c020");

struct H {
    // -------------------------------------------------------------- 0x0066c020
    struct P3 {
        u32 a, b, c;
    };
    u32 region_pick(float x, float y, float z, u32 extra) {
        c_pick.hit();
        if (jo::self<jo::PathSpace>(this)->zones == nullptr) return 0;
        joan::orig::as_path_space(this)->build_bvh();  // 0x006745e0
        if (jo::self<jo::PathSpace>(this)->tree == nullptr || joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->tree))->get_2c()) <= 0) return 0;  // 0x0066b7a0
        if (jo::self<jo::PathSpace>(this)->items == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // operator new
            void* list = nullptr;
            if (mem != nullptr) list = joan::orig::dynarray(mem)->construct(static_cast<std::int32_t>(sizeof(void*)), i32{0x1e}, i32{0});  // 0x005843d0 (pointers: 4 on x86)
            jo::view<void*>(jo::self<jo::PathSpace>(this)->items) = list;
        }
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::PathSpace>(this)->items))->zap(i32{0});  // 0x00584580
        const float k16 = global<float>(d::flt_16);
        const float k1 = global<float>(d::flt_1);
        P3 lo{0, 0, 0};
        P3 hi{0, 0, 0};
        {
            const i32 zi = ftolf((z - k16) - k1);
            const i32 yi = ftolf(y - k1);
            const i32 xi = ftolf(x - k1);
            u32 pt[2] = {0, 0};
            const u32* q = joan::orig::arg::as<u32*>(joan::orig::as_point(static_cast<void*>(pt))->set(joan::orig::arg::u32(xi), joan::orig::arg::u32(yi)));  // 0x004eeb80
            const u32 q0 = q[0];
            const u32 q1 = q[1];
            joan::orig::as_triple(&lo)->construct(joan::orig::arg::u32(q0), joan::orig::arg::u32(q1), joan::orig::arg::u32(zi));  // 0x0066a4b0
        }
        {
            const i32 zi = ftolf((z + k16) + k1);
            const i32 yi = ftolf(y + k1);
            const i32 xi = ftolf(x + k1);
            u32 pt[2] = {0, 0};
            const u32* q = joan::orig::arg::as<u32*>(joan::orig::as_point(static_cast<void*>(pt))->set(joan::orig::arg::u32(xi), joan::orig::arg::u32(yi)));  // 0x004eeb80
            const u32 q0 = q[0];
            const u32 q1 = q[1];
            joan::orig::as_triple(&hi)->construct(joan::orig::arg::u32(q0), joan::orig::arg::u32(q1), joan::orig::arg::u32(zi));  // 0x0066a4b0
        }
        void* const grid = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->tree))->item(i32{1}));  // 0x00637da0
        joan::orig::as_bvh_node(grid)->query(&lo, &hi, joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PathSpace>(this)->items)));                    // 0x006752c0
        for (i32 i = list_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->items)); i > 0; --i) {
            u8* const cell = list_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->items), i);
            if ((joan::orig::arg::as<u32>(joan::orig::as_pfd_zone(cell)->contains(x, y, joan::orig::arg::f32(extra), 0)) & 0xff) == 0) continue;  // 0x006718c0
            // st0 is taken unrounded: one path of 0x00664180 returns a raw fild int.
            const double height = joan::orig::arg::as<double>(joan::orig::as_terrain_block(cell)->height(joan::orig::arg::f32(x), joan::orig::arg::f32(y)));                 // 0x00664180
            const i32 h = ftol(height);
            const i32 below = static_cast<i32>(static_cast<u32>(h) - 0x10u);
            if (static_cast<double>(below) > static_cast<double>(z)) continue;
            const i32 above = static_cast<i32>(static_cast<u32>(h) + 0x10u);
            if (!(static_cast<double>(above) >= static_cast<double>(z))) continue;
            return jo::view<u32>(jo::as_pfd_zone(cell)->id);
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_19", name, addr, fn)
B("PathSpace::find_polygon_66c020", 0x0066c020, &H::region_pick);
#undef B

}  // namespace game_b109_19
}  // namespace

// ---------------------------------------- from modules/game_b111_07.cpp (fix/uninit)
//
//    - PathTree::nearest_66c710 0x0066c710 (thiscall (x, y, a3, a4), ret 0x10, returns a dword
//      or 0): point (0x004eeb80), best distance 0x7fffffff, recursive search 0x00675430 from
//      the tree root (0x00637da0(1)) -> *best node's object or 0. The code after 0x0066c79c is
//      unreachable (a3/a4 are only used there). Original: the out pointer [ebp-0x14] is never
//      initialised; 0x00675430 tests it (`*out == 0` = none found yet) before its first write,
//      and when no node has an object (+0x34) it is never written and the garbage is
//      dereferenced. Fix: 0 (the value both readers treat as "nothing found"); whenever a node
//      is found the result is the original's (its first candidate is written either way: an
//      _ftol distance can never equal 0x7fffffff).
namespace {
namespace game_b111_07 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_nearest, "PathTree::nearest");

struct H {
    // ------------------------------------------------------------------------------ 0x0066c710
    u32 nearest(i32 x, i32 y, u32 /*a3: dead code only*/, u32 /*a4: dead code only*/) {
        c_nearest.hit();
        if (jo::self<jo::PathSpace>(this)->zones == nullptr) return 0;
        joan::orig::as_path_space(this)->build_bvh();  // 0x006745e0
        if (jo::self<jo::PathSpace>(this)->tree == nullptr || joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::self<jo::PathSpace>(this)->tree)->get_2c()) <= 0) return 0;  // 0x0066b7a0
        alignas(4) i32 point[2] = {0, 0};
        joan::orig::as_point(static_cast<void*>(point))->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y));  // 0x004eeb80
        i32 best = 0x7fffffff;
        jo::PfdZone* found = nullptr;  // fix/uninit: [ebp-0x14] is never initialised in the original (the leaf's zone)
        void* const root = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::self<jo::PathSpace>(this)->tree)->item(i32{1}));  // 0x00637da0
        joan::orig::as_bvh_node(root)->nearest(joan::orig::arg::mptr(static_cast<void*>(point)), &best, &found);    // 0x00675430
        if (found == nullptr) return 0;
        return static_cast<u32>(found->id);  // the zone's first dword
    }
};

// fix/uninit: 0x0066c710's out pointer [ebp-0x14] is uninitialised (tested by 0x00675430, dereferenced
// when nothing is found) -> 0.
JOAN_REPLACE("fix/uninit", "PathSpace::find_nearest_polygon_66c710", 0x0066c710, &H::nearest);

}  // namespace game_b111_07
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
//
//    - 0x0066c900 Map::query_66c900      (thiscall, 5 args, ret 0x14). The loop after 0x0066c98c
//      is unreachable in the original (no jump into it) and is not reproduced.
namespace {
namespace game_b108_16 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_query_66c900, "Map::query_66c900");

struct H {
    // --------------------------------------------------------------------- 0x0066c900
    u32 query_66c900(i32 x, i32 y, u32 a10, u32 a14, u32 a18) {
        (void)a10;
        (void)a14;
        c_query_66c900.hit();
        if (jo::self<jo::PathSpace>(this)->zones == nullptr) return 0;
        joan::orig::as_path_space(this)->build_bvh();  // 0x006745e0
        if (jo::self<jo::PathSpace>(this)->tree == nullptr) return 0;
        if (!(joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->tree))->get_2c()) > 0)) return 0;  // 0x0066b7a0
        alignas(4) u8 pt[8];
        joan::orig::as_point(pt)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y));  // 0x004eeb80
        u32* found = nullptr;
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->tree))->item(i32{1}));  // 0x00637da0
        joan::orig::as_bvh_node(o)->nearest(joan::orig::arg::mptr(pt), &a18, &found);                     // 0x00675430
        if (found == nullptr) return 0;
        return *found;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("PathSpace::find_nearest_polygon_66c900", 0x0066c900, &H::query_66c900);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b109_02.cpp (game/b109_02)
//
//    - Bvh::build_6747a0 0x006747a0 (ret 0xc, (parent, lo, hi), recursive through thunk 0x00403d19):
//      bounds / centroid bounds of items [0xa6a278][lo..hi), split axis, node from block array
//      0x00637be0 / 0x00637da0, partition 0x00674fc0 (cdecl, comparators 0x6750b0 / 0x675100 /
//      0x675150). SEH frame (local 0x00663c50 / 0x00674f50 object): normal path only.
namespace {
namespace game_b109_02 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
// [0xa6a278]: the zone pointers of PathSp::build_6745e0 (game_b108_04).
jo::PfdZone** zones() { return static_cast<jo::PfdZone**>(global<void*>(d::g_a6a278)); }
using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

// 3 x int vector (0x0066a480 default ctor, 0x00663f30 copy).
struct I3 {
    i32 x, y, z;
};
static_assert(sizeof(I3) == 12, "i3");

JOAN_COUNTER(c_bvh, "Bvh::build_6747a0");

struct H {
    // ---- 0x006747a0 helpers
    static jo::PfdZone* item(i32 i) { return zones()[i]; }
    static i32 half(i32 a, i32 b) { return wadd(a, b) / 2; }
    static void vctor(I3* v) { joan::orig::as_triple(v)->construct_default(); }                  // 0x0066a480
    static void vcopy(void* d, const void* s) { joan::orig::as_triple(d)->assign(joan::orig::arg::mptr(s)); }  // 0x00663f30

    // 0x006747a0
    jo::BoxTree* bvh_build(jo::BoxTree* parent, i32 lo, i32 hi) {
        (void)parent;
        if (!(hi > lo)) return nullptr;
        c_bvh.hit();
        I3 mn{}, mx{}, cmn{}, cmx{}, mid{}, c{}, ext{};
        vctor(&mn);
        vctor(&mx);
        vctor(&cmn);
        vctor(&cmx);
        vctor(&mid);
        vctor(&c);
        vcopy(&mn, item(lo)->bbox_min);
        vcopy(&mx, item(lo)->bbox_max);
        c.x = half(item(lo)->bbox_min[0], item(lo)->bbox_max[0]);
        c.y = half(item(lo)->bbox_min[1], item(lo)->bbox_max[1]);
        c.z = half(item(lo)->bbox_min[2], item(lo)->bbox_max[2]);
        vcopy(&cmn, &c);
        vcopy(&cmx, &c);
        for (i32 i = lo + 1; i < hi; ++i) {
            c.x = half(item(i)->bbox_min[0], item(i)->bbox_max[0]);
            c.y = half(item(i)->bbox_min[1], item(i)->bbox_max[1]);
            c.z = half(item(i)->bbox_min[2], item(i)->bbox_max[2]);
            if (item(i)->bbox_min[0] < mn.x) mn.x = item(i)->bbox_min[0];
            if (item(i)->bbox_max[0] > mx.x) mx.x = item(i)->bbox_max[0];
            if (item(i)->bbox_min[1] < mn.y) mn.y = item(i)->bbox_min[1];
            if (item(i)->bbox_max[1] > mx.y) mx.y = item(i)->bbox_max[1];
            if (item(i)->bbox_min[2] < mn.z) mn.z = item(i)->bbox_min[2];
            if (item(i)->bbox_max[2] > mx.z) mx.z = item(i)->bbox_max[2];
            if (c.x < cmn.x) cmn.x = c.x;
            if (c.x > cmx.x) cmx.x = c.x;
            if (c.y < cmn.y) cmn.y = c.y;
            if (c.y > cmx.y) cmx.y = c.y;
            if (c.z < cmn.z) cmn.z = c.z;
            if (c.z > cmx.z) cmx.z = c.z;
        }
        mid.x = half(cmn.x, cmx.x);
        mid.y = half(cmn.y, cmx.y);
        mid.z = half(cmn.z, cmx.z);
        // 0x00663c50 object: memset 0x40 bytes, plus its +0x38 member (0x00663b90) memsets 0x34 bytes
        // -> 0x6c bytes written; the original frame has 0x74 bytes (ebp-0xd4 .. ebp-0x60).
        alignas(4) u8 part[sizeof(jo::PfdZone)];  // 0x74
        std::memset(part, 0, sizeof(part));
        joan::orig::as_zone(part)->construct();  // 0x00663c50
        vcopy(jo::as_pfd_zone(part)->bbox_min, &cmn);
        vcopy(jo::as_pfd_zone(part)->bbox_max, &cmx);
        vctor(&ext);
        ext.x = wsub(cmx.x, cmn.x);
        ext.y = wsub(cmx.y, cmn.y);
        ext.z = wsub(cmx.z, cmn.z);
        i32 axis = 0;
        if (ext.x >= ext.y && ext.x >= ext.z) {
            axis = 0;
        } else if (ext.y >= ext.z) {
            axis = 1;
        } else {
            axis = 2;
        }
        const u32 slot = joan::orig::arg::as<u32>(joan::orig::as_block_array(jo::self<jo::PathSpace>(this)->tree)->add(nullptr));    // 0x00637be0
        jo::BoxTree* const node = jo::as_box_tree(joan::orig::as_billboard(jo::self<jo::PathSpace>(this)->tree)->item(joan::orig::arg::i32(slot)));  // 0x00637da0
        jo::view<i32>(node->u32_c) = axis;
        node->left = nullptr;
        node->right = nullptr;
        vcopy(node->box, &mn);
        vcopy(&node->box[3], &mx);
        vcopy(node->mid, &mid);
        if (wsub(hi, lo) == 1) {
            node->item = zones()[lo];
        } else {
            i32 split = 0;
            if (ext.x == 0 && ext.y == 0 && ext.z == 0) {
                split = half(lo, hi);
            } else {
                static const std::uintptr_t kCmp[3] = {fn::code_value(fn::cmp_mid_x), fn::code_value(fn::cmp_mid_y), fn::code_value(fn::cmp_mid_z)};
                u32 scratch = 0;
                const i32 r = joan::orig::arg::as<i32>(joan::orig::array_remove_if(joan::orig::arg::mptr(&zones()[lo]), joan::orig::arg::i32(wsub(hi, lo)), static_cast<i32>(sizeof(jo::PfdZone*)), static_cast<void*>(part), kCmp[axis], &scratch));
                split = wadd(lo, r);
            }
            node->left = joan::orig::as_path_space(this)->build_bvh_range(node, joan::orig::arg::i32(lo), joan::orig::arg::i32(split));  // 0x006747a0
            if (node->left != nullptr) node->left->u32_0 = node;
            node->right = joan::orig::as_path_space(this)->build_bvh_range(node, joan::orig::arg::i32(split), joan::orig::arg::i32(hi));
            if (node->right != nullptr) node->right->u32_0 = node;
        }
        joan::orig::as_pfd_zone(part)->destruct();  // 0x00674f50
        return node;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_02", name, addr, fn)
B("PathSpace::build_polygon_tree_struct_recur", 0x006747a0, &H::bvh_build);
#undef B

}  // namespace game_b109_02
}  // namespace

// ---------------------------------------- from modules/game_b98_07.cpp (game/b98_07)
namespace {
namespace game_b98_07 {

using joan::hook::global;
using u8 = std::uint8_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;


using u32 = std::uint32_t;

JOAN_COUNTER(c_link_edges, "PolySet::link_edges");
JOAN_COUNTER(c_separate, "separate_circles");

struct H {
    // ------------------------------------------------------------------ 0x0066b7c0
    void link_edges(i32 n) {
        c_link_edges.hit();
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr || n <= 0) return;
        if (!(joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->get_2c()) > n)) return;  // 0x0066b7a0
        for (i32 i = n + 1; i <= joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->get_2c()); ++i) {  // 0x0066b7a0
            jo::PfdZone* const a = jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(i)));  // 0x00637da0
            for (i32 j = 1; j <= a->point_count; ++j) {
                if (a->links[j - 1].item != nullptr) continue;
                const jo::PfdPoint* const a0 = a->points[j - 1].item;
                const jo::PfdPoint* a1;
                if (j >= a->point_count) {
                    a1 = a->points[0].item;
                } else {
                    a1 = a->points[j].item;
                }
                for (i32 k = 1; k <= n; ++k) {
                    jo::PfdZone* const b = jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(k)));  // 0x00637da0
                    for (i32 m = 1; m <= b->point_count; ++m) {
                        if (b->links[m - 1].item != nullptr) continue;
                        const jo::PfdPoint* const b0 = b->points[m - 1].item;
                        const jo::PfdPoint* b1;
                        if (m >= b->point_count) {
                            b1 = b->points[0].item;
                        } else {
                            b1 = b->points[m].item;
                        }
                        if (a0 == b1 && a1 == b0) {
                            joan::orig::as_zone(a)->link(joan::orig::arg::i32(j), joan::orig::arg::mptr(static_cast<void*>(b)), joan::orig::arg::i32(m));  // 0x00664130
                        }
                    }
                }
            }
        }
    }
};

// ---------------------------------------------------------------------------- 0x0066f820
static i32 JOAN_CDECL separate_circles(float* x0, float* y0, float* x1, float* y1, float r) {
    c_separate.hit();
    const float dx = *x1 - *x0;
    const float dy = *y1 - *y0;
    const float d2 = dx * dx + dy * dy;
    const float lim = r * r * global<float>(d::flt_4);  // 4.0f
    if (lim >= d2) return 0;                               // continue when < or unordered
    const float d = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(d2)))));  // sqrt
    const float ox = r * dx / d;
    const float oy = r * dy / d;
    *x0 = ox + *x0;
    *x1 = *x1 - ox;
    *y0 = oy + *y0;
    *y1 = *y1 - oy;
    return 1;
}

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_07", name, addr, fn)
B98("PathSpace::link_gen_path_polygon", 0x0066b7c0, &H::link_edges);
B98("separate_circles", 0x0066f820, &separate_circles);
#undef B98

}  // namespace game_b98_07
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;


void* game_memcpy(void* d, const void* s, u32 n) { return std::memcpy(joan::orig::arg::mptr(d), joan::orig::arg::ptr(s), joan::orig::arg::u32(n)); }

// Block array (0x00637aa0 ctor): +0x2c count.
u8* arr_get(void* arr, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(arr)->item(joan::orig::arg::i32(i))); }        // 0x00637da0
i32 arr_count(void* arr) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(arr)->get_2c()); }                // 0x0066b7a0

JOAN_COUNTER(c_copy_ctor, "List::copy_ctor");
JOAN_COUNTER(c_serialize, "List::serialize");

struct H {
    // ------------------------------------------------------------------ 0x0066ba30
    i32 row_word(i32 i) {
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr || i <= 0 || i > arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))) return 0;
        const jo::PfdZone* const e = jo::as_pfd_zone(arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), i));
        return static_cast<i32>(jo::view<i16>(e->flags));
    }
    // ------------------------------------------------------------------ 0x0066baa0
    i32 cell_word(i32 i, i32 j, i32* out) {
        if (jo::view<void*>(jo::self<jo::PathSpace>(this)->zones) == nullptr || i <= 0 || i > arr_count(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))) return 0;
        const jo::PfdZone* const e = jo::as_pfd_zone(arr_get(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones), i));
        const jo::PfdZone* const cell = e->links[j - 1].item;
        if (cell == nullptr) return 0;
        *out = static_cast<i32>(jo::view<i16>(cell->flags));
        return 1;
    }
    // ------------------------------------------------------------------ 0x0066e4e0
    void* copy_ctor(void* src) {
        c_copy_ctor.hit();
        joan::orig::dynarray(this)->construct(static_cast<std::int32_t>(sizeof(jo::PathPoint)), 0x10, 0);  // 0x005843d0
        std::memcpy(&jo::self<jo::PointList>(this)->start, &jo::self<jo::PointList>(src)->start, sizeof(jo::PathPoint));  // rep movsd x9
        for (i32 i = 1; i <= joan::orig::as_dynarray(src)->size(); ++i) {  // 0x00417ad0
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(src)->get(joan::orig::arg::i32(i)));      // 0x00417200
            joan::orig::dynarray(this)->linkin(joan::orig::arg::ptr(e));     // 0x00584620
        }
        return this;
    }
    // ------------------------------------------------------------------ 0x0066eb90
    i32 serialize(u8* buf) {
        c_serialize.hit();
        u8* p = buf;
        if (buf != nullptr) *reinterpret_cast<u16*>(p) = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_list(this)->count_66e8a0()));  // 0x0066e8a0
        p += 2;
        if (buf != nullptr) *reinterpret_cast<u16*>(p) = 0x24;
        p += 2;
        if (buf != nullptr) game_memcpy(p, &jo::self<jo::PointList>(this)->start, sizeof(jo::PathPoint));
        p += 0x24;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->empty()) == 0) {  // 0x0043c420
            void* const extra = joan::orig::arg::as<void*>(joan::orig::as_path(this)->first());  // 0x0066e940
            if (extra != nullptr) {
                if (buf != nullptr) game_memcpy(p, extra, 0x24);
                p += 0x24;
            }
            for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_list(this)->count_66e8a0()) - 1; i > 0; --i) {  // 0x0066e8a0
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->from_end(joan::orig::arg::i32(i)));           // 0x0066e8d0
                if (buf != nullptr) game_memcpy(p, e, 0x24);
                p += 0x24;
            }
        }
        return static_cast<i32>(reinterpret_cast<std::uintptr_t>(p) - reinterpret_cast<std::uintptr_t>(buf));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("PathSpace::get_polygon_priority", 0x0066ba30, &H::row_word);
B("PathSpace::get_adjacent_polygon", 0x0066baa0, &H::cell_word);
B("List::copy_ctor", 0x0066e4e0, &H::copy_ctor);
B("List::serialize", 0x0066eb90, &H::serialize);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_objects1.cpp (game/objects1)
namespace {
namespace game_objects1 {

using joan::hook::global;
using i32 = std::int32_t;

using joan::call::thiscall;


JOAN_COUNTER(c_height, "Terrain::height");

struct H {
    // 0x0066cae0 Terrain::height(cell, x, y): the cell block's height (0x00664180) or the
    // default 0x009cf5c4. The st0 of 0x00664180 can be an unrounded int (fild): kept as double.
    double terrain_height(std::int32_t cell, float x, float y) {
        c_height.hit();
        void* const block = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(cell)));  // 0x00637da0
        if (block == nullptr) return static_cast<double>(global<float>(d::k_9cf5c4));
        return joan::orig::arg::as<double>(joan::orig::as_terrain_block(block)->height(joan::orig::arg::f32(x), joan::orig::arg::f32(y)));  // 0x00664180
    }
};

#define OB1(name, addr, fn) JOAN_REPLACE("game/objects1", name, addr, fn)
OB1("PathSpace::calc_polygon_z", 0x0066cae0, &H::terrain_height);
#undef OB1

}  // namespace game_objects1
}  // namespace

// ---------------------------------------- from modules/game_b108_08_2.cpp (game/b108_08)
namespace {
namespace game_b108_08_2 {

using u8 = std::uint8_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;

using joan::orig::helpers::add32;
// x87 fcomp + test ah,0x40: equal or unordered.
bool eq_or_nan(double a, double b) { return !(a < b) && !(a > b); }

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_find_path, "Nav::find_path_66cb40");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x0066cb40
    i32 find_path_66cb40(float x, float y, float z, float tx, float ty, float tz, void* path, i32 a, i32 node,
                         i32 b, u32 c, u32 d, u32 e) {
        c_find_path.hit();
        joan::orig::as_path(path)->clear();  // 0x004eeca0
        if (jo::self<jo::PathSpace>(this)->zones == nullptr) return 0;
        if (node == 0) node = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->pick(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(z), joan::orig::arg::u32(a)));  // 0x0066c020
        if (node == 0) {
            node = joan::orig::arg::as<i32>(joan::orig::as_path_space(this)->nearest_zone(joan::orig::arg::i32(ftol(x)), joan::orig::arg::i32(ftol(y)), joan::orig::arg::u32(ftol(z)), joan::orig::arg::u32(a)));  // 0x0066c710
            if (node == 0) return 0;
        }
        void* const start = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(node)));  // 0x00637da0
        const i32 itx = ftol(tx), ity = ftol(ty), itz = ftol(tz);
        if (jo::self<jo::PathSpace>(this)->search_stamp == 0) {
            for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->get_2c()); ++i) {  // 0x0066b7a0
                jo::PfdZone* const n = jo::as_pfd_zone(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->zones))->item(joan::orig::arg::i32(i)));
                jo::view<u32>(n->search_stamp) = 0;
            }
            jo::self<jo::PathSpace>(this)->search_stamp = 1;
        }
        const i32 stamp = jo::self<jo::PathSpace>(this)->search_stamp;
        jo::self<jo::PathSpace>(this)->search_stamp = add32(jo::self<jo::PathSpace>(this)->search_stamp, 1);
        joan::orig::as_path_box(path)->set(joan::orig::arg::i32(ftol(x)), joan::orig::arg::i32(ftol(y)), joan::orig::arg::i32(ftol(z)), joan::orig::arg::i32(itx), joan::orig::arg::i32(ity), joan::orig::arg::i32(itz));  // 0x0066e640
        const Vec3 to{tx, ty, tz};
        const Vec3 from{x, y, z};
        const i32 found = joan::orig::path::astar_66ce30(start, 0u, joan::orig::arg::bits(from.x), joan::orig::arg::bits(from.y), joan::orig::arg::bits(from.z), joan::orig::arg::bits(to.x), joan::orig::arg::bits(to.y), joan::orig::arg::bits(to.z), stamp, path, joan::orig::arg::u32(a), b, -1, joan::orig::arg::i32(jo::self<jo::PathSpace>(this)->map_id));
        if (found != 0) {
            joan::orig::as_path_box(path)->set_end(joan::orig::arg::i32(itx), joan::orig::arg::i32(ity), joan::orig::arg::i32(itz));  // 0x0066e720
            joan::orig::as_path(path)->smooth(joan::orig::arg::f32(a), joan::orig::arg::f32(c), joan::orig::arg::f32(d), joan::orig::arg::i32(e));     // 0x0066ece0
            if (eq_or_nan(static_cast<double>(itx), tx) && eq_or_nan(static_cast<double>(ity), ty)) return 1;
            return 0;
        }
        joan::orig::as_path(path)->smooth(joan::orig::arg::f32(a), joan::orig::arg::f32(c), joan::orig::arg::f32(d), joan::orig::arg::i32(e));
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("PathSpace::find_path", 0x0066cb40, &H::find_path_66cb40);
#undef B

}  // namespace game_b108_08_2
}  // namespace

// ---------------------------------------- from modules/game_b109_18.cpp (game/b109_18)
namespace {
namespace game_b109_18 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::hook::x87::ftol;

using i8 = std::int8_t;
using u32 = std::uint32_t;

float bits_f(u32 b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

// x87 compare outcomes (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool lt_or_nan(double a, double b) { return !(a >= b); }
bool gt(double a, double b) { return a > b; }

struct P2 {
    i32 x, y;
};
static_assert(sizeof(P2) == 8, "P2");

JOAN_COUNTER(c_astar, "Path::astar_66ce30");

// The CRT's abs, inline (INT_MIN stays INT_MIN, as the original's neg).
i32 crt_abs(i32 v) { return static_cast<i32>(v < 0 ? 0u - static_cast<u32>(v) : static_cast<u32>(v)); }

// ------------------------------------------------------------------ 0x0066ce30
P2* point_of(void* n, P2* out) { return joan::orig::arg::as<P2*>(joan::orig::as_vec2i(n)->copy_to(joan::orig::arg::mptr(out))); }  // 0x0066a4f0

// Estimated total cost: g + h - (|dx| + |dy|) * (n0 % 3 == p38 ? 6 : 4) * p3c.
u32 path_score(void* n, u32 sum, i32 p38, i32 p3c) {
    u32 dist = static_cast<u32>(crt_abs(static_cast<i32>(jo::view<u32>(jo::as_pfd_zone(n)->bbox_max[0]) - jo::view<u32>(jo::as_pfd_zone(n)->bbox_min[0]))));
    dist += static_cast<u32>(crt_abs(static_cast<i32>(jo::view<u32>(jo::as_pfd_zone(n)->bbox_max[1]) - jo::view<u32>(jo::as_pfd_zone(n)->bbox_min[1]))));
    const u32 mult = (jo::as_pfd_zone(n)->id % 3 != p38) ? 4u : 6u;
    return sum - dist * mult * static_cast<u32>(p3c);
}

// Zone::not_after (0x00664b00, game/unknown_pathmesh.cpp) with its key_compare (0x0066fca0), inline - the open
// list's order, evaluated for every entry an insertion passes: the max flags (+0x66, int16) first, then the cost
// (+0x5c), each as the wrapping 32-bit difference; "not after" = difference <= 0.
bool not_after(const void* n, const void* other) {
    const i32 c = jo::as_pfd_zone(n)->max_flags, f = jo::as_pfd_zone(other)->max_flags;
    const i32 r = c != f ? static_cast<i32>(static_cast<u32>(c) - static_cast<u32>(f))
                         : static_cast<i32>(jo::view<u32>(jo::as_pfd_zone(n)->cost_f) - jo::view<u32>(jo::as_pfd_zone(other)->cost_f));
    return r <= 0;
}

// Inserts n into the open list after the first entry for which 0x00664b00(n, next) holds.
void open_insert_after(void* n, void* head) {
    void* p = head;
    while (jo::as_pfd_zone(p)->open_next != nullptr) {
        if (not_after(n, jo::as_pfd_zone(p)->open_next)) break;  // 0x00664b00
        p = jo::as_pfd_zone(p)->open_next;
    }
    jo::as_pfd_zone(n)->open_next = jo::as_pfd_zone(p)->open_next;
    jo::as_pfd_zone(p)->open_next = jo::as_pfd_zone(n);
}

static i32 JOAN_CDECL astar(void* start, u32 a0c, u32 f10, u32 f14, u32 a18, u32 f1c, u32 f20, u32 a24, i32 key,
                         void* out, u32 f30_bits, i32 budget, i32 p38, i32 p3c) {
    c_astar.hit();
    (void)a0c, (void)a18, (void)a24;
    float f30 = bits_f(f30_bits);
    if (lt_or_nan(f30, 200.0)) f30 = 0.0f;
    if ((joan::orig::arg::as<u32>(joan::orig::as_pfd_zone(start)->contains(joan::orig::arg::f32(f1c), joan::orig::arg::f32(f20), joan::orig::arg::f32(u32{0}), 0)) & 0xff) != 0) return 1;  // 0x006718c0
    P2 goal;
    joan::orig::as_point(&goal)->set(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x004eeb80 ctor
    goal.x = joan::hook::x87::ftol(bits_f(f1c));
    goal.y = joan::hook::x87::ftol(bits_f(f20));
    P2 from;
    joan::orig::as_point(&from)->set(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}));  // 0x004eeb80 ctor
    from.x = joan::hook::x87::ftol(bits_f(f10));
    from.y = joan::hook::x87::ftol(bits_f(f14));
    joan::orig::as_group(start)->recentre(joan::orig::arg::i32(key), nullptr);  // 0x00664920
    jo::view<i32>(jo::as_pfd_zone(start)->cost_g) = 0;
    jo::view<i32>(jo::as_pfd_zone(start)->cost_h) = joan::orig::arg::as<i32>(joan::orig::grid_590410(joan::orig::arg::i32(goal.x), joan::orig::arg::i32(goal.y), joan::orig::arg::i32(from.x), joan::orig::arg::i32(from.y)));
    jo::view<u32>(jo::as_pfd_zone(start)->cost_f) = path_score(start, jo::as_pfd_zone(start)->cost_g + jo::as_pfd_zone(start)->cost_h, p38, p3c);
    jo::view<i32>(jo::as_pfd_zone(start)->flags_sum) = jo::view<i16>(jo::as_pfd_zone(start)->flags);
    jo::as_pfd_zone(start)->max_flags = jo::view<i16>(jo::as_pfd_zone(start)->flags) < 0 ? i16{0} : jo::view<i16>(jo::as_pfd_zone(start)->flags);

    void* open = start;
    void* closed = nullptr;
    for (; open != nullptr && budget > 0; --budget) {
        void* const cur = open;
        open = jo::as_pfd_zone(cur)->open_next;
        jo::as_pfd_zone(cur)->open_next = nullptr;
        if ((joan::orig::arg::as<u32>(joan::orig::as_pfd_zone(cur)->contains(joan::orig::arg::f32(f1c), joan::orig::arg::f32(f20), joan::orig::arg::f32(u32{0}), 0)) & 0xff) != 0) {  // 0x006718c0
            for (void* n = cur; jo::as_pfd_zone(n)->came_from != nullptr; n = jo::as_pfd_zone(n)->came_from) {
                for (i32 i = 0; i < jo::as_pfd_zone(n)->point_count; ++i) {
                    if (jo::as_pfd_zone(n)->links[i].item != jo::as_pfd_zone(n)->came_from) continue;
                    const i32 j = (i == jo::as_pfd_zone(n)->point_count - 1) ? 0 : i + 1;
                    void* const nb = jo::as_pfd_zone(n)->points[j].item;
                    const i32 b8 = jo::as_pfd_point(nb)->xyz[2];
                    P2 t0, t1, t2, t3;
                    const i32 by = point_of(jo::as_pfd_zone(n)->points[j].item, &t0)->y;
                    const i32 bx = point_of(jo::as_pfd_zone(n)->points[j].item, &t1)->x;
                    const i32 a8 = jo::as_pfd_zone(n)->points[i].item->xyz[2];
                    const i32 ay = point_of(jo::as_pfd_zone(n)->points[i].item, &t2)->y;
                    const i32 ax = point_of(jo::as_pfd_zone(n)->points[i].item, &t3)->x;
                    joan::orig::as_path(out)->add_segment(joan::orig::arg::i32(ax), joan::orig::arg::i32(ay), joan::orig::arg::i32(a8), joan::orig::arg::i32(bx), joan::orig::arg::i32(by), joan::orig::arg::i32(b8));  // 0x0066e790
                }
            }
            return 1;
        }
        for (i32 k = 1; k <= jo::as_pfd_zone(cur)->point_count; ++k) {
            void* const nb = joan::orig::arg::as<void*>(joan::orig::as_zone(cur)->link_at(joan::orig::arg::i32(k)));  // 0x00664080
            if (nb == nullptr) continue;
            if (nb == jo::as_pfd_zone(cur)->came_from) continue;
            if (jo::as_pfd_zone(nb)->blocked == 1) continue;
            if (jo::as_pfd_zone(nb)->blocked == -1 && gt(f30, 0.0)) continue;
            const bool known = joan::orig::arg::as<i32>(joan::orig::as_pfd_zone(nb)->is_stamp(joan::orig::arg::i32(key))) != 0;  // 0x0066dd00
            if (!known) {
                joan::orig::as_group(nb)->recentre(joan::orig::arg::i32(key), cur);  // 0x00664920
                if (cur == start) {
                    const i32 g = joan::orig::arg::as<i32>(joan::orig::script_random_range(joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_y), joan::orig::arg::i32(from.x), joan::orig::arg::i32(from.y)));
                    jo::as_pfd_zone(nb)->cost_g = jo::as_pfd_zone(cur)->cost_g + static_cast<u32>(g);
                } else {
                    const i32 g = joan::orig::arg::as<i32>(joan::orig::script_random_range(joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_y), joan::orig::arg::i32(jo::as_pfd_zone(cur)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(cur)->centre_y)));
                    jo::as_pfd_zone(nb)->cost_g = jo::as_pfd_zone(cur)->cost_g + static_cast<u32>(g);
                }
                jo::view<i32>(jo::as_pfd_zone(nb)->cost_h) = joan::orig::arg::as<i32>(joan::orig::grid_590410(joan::orig::arg::i32(goal.x), joan::orig::arg::i32(goal.y), joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_y)));
                jo::view<u32>(jo::as_pfd_zone(nb)->cost_f) = path_score(nb, jo::as_pfd_zone(nb)->cost_g + jo::as_pfd_zone(nb)->cost_h, p38, p3c);
                jo::as_pfd_zone(nb)->flags_sum = static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_pfd_zone(nb)->flags))) + jo::as_pfd_zone(cur)->flags_sum;
                jo::as_pfd_zone(nb)->max_flags = jo::as_pfd_zone(cur)->max_flags > jo::view<i16>(jo::as_pfd_zone(nb)->flags) ? jo::as_pfd_zone(cur)->max_flags : jo::view<i16>(jo::as_pfd_zone(nb)->flags);
                if (open == nullptr || not_after(nb, open)) {  // 0x00664b00
                    jo::as_pfd_zone(nb)->open_next = jo::as_pfd_zone(open);
                    open = nb;
                } else {
                    open_insert_after(nb, open);
                }
                continue;
            }
            u32 g40;
            if (cur == start) {
                g40 = jo::as_pfd_zone(cur)->cost_g +
                      static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::script_random_range(joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_y), joan::orig::arg::i32(from.x), joan::orig::arg::i32(from.y))));
            } else {
                g40 = jo::as_pfd_zone(cur)->cost_g + static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::script_random_range(joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(nb)->centre_y), joan::orig::arg::i32(jo::as_pfd_zone(cur)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(cur)->centre_y))));
            }
            const u32 h44 = jo::as_pfd_zone(nb)->cost_h;
            const u32 s48 = path_score(nb, g40 + h44, p38, p3c);
            const u32 v4c = jo::as_pfd_zone(cur)->flags_sum + jo::as_pfd_zone(nb)->flags_sum;
            const i16 m = jo::as_pfd_zone(cur)->max_flags > jo::view<i16>(jo::as_pfd_zone(nb)->flags) ? jo::as_pfd_zone(cur)->max_flags : jo::view<i16>(jo::as_pfd_zone(nb)->flags);
            const i32 v50 = m;
            const i32 cmp = joan::orig::arg::as<i32>(joan::orig::key_compare(static_cast<i32>(s48), static_cast<i32>(v4c), joan::orig::arg::i32(v50), joan::orig::arg::i32(jo::as_pfd_zone(nb)->cost_f), joan::orig::arg::i32(jo::view<i32>(jo::as_pfd_zone(nb)->flags_sum)), static_cast<i32>(jo::as_pfd_zone(nb)->max_flags)));
            if (cmp >= 0) continue;
            joan::orig::as_member(nb)->set_group_664eb0(joan::orig::arg::mptr(cur));  // 0x00664eb0
            jo::as_pfd_zone(nb)->cost_g = g40;
            jo::as_pfd_zone(nb)->cost_h = h44;
            jo::view<u32>(jo::as_pfd_zone(nb)->cost_f) = s48;
            jo::as_pfd_zone(nb)->flags_sum = v4c;
            jo::as_pfd_zone(nb)->max_flags = static_cast<i16>(v50);
            if (jo::view<i8>(jo::as_pfd_zone(nb)->closed) == 0) {
                if (open == nb) {
                    open = jo::as_pfd_zone(nb)->open_next;
                    jo::as_pfd_zone(nb)->open_next = nullptr;
                } else {
                    void* p = open;
                    while (jo::as_pfd_zone(p)->open_next != nullptr && jo::as_pfd_zone(p)->open_next != nb) p = jo::as_pfd_zone(p)->open_next;
                    jo::as_pfd_zone(p)->open_next = jo::as_pfd_zone(nb)->open_next;
                    jo::as_pfd_zone(nb)->open_next = nullptr;
                }
                if (open == nullptr || !(jo::as_pfd_zone(nb)->cost_f > jo::as_pfd_zone(open)->cost_f)) {
                    jo::as_pfd_zone(nb)->open_next = jo::as_pfd_zone(open);
                    open = nb;
                } else {
                    open_insert_after(nb, open);
                }
            }
            if (jo::as_pfd_zone(nb)->child_count > 0) joan::orig::path_tree_accumulate(joan::orig::arg::mptr(nb), joan::orig::arg::i32(p38), joan::orig::arg::i32(p3c));
        }
        jo::as_pfd_zone(cur)->open_next = jo::as_pfd_zone(closed);
        closed = cur;
        jo::as_pfd_zone(cur)->closed = 1;
    }

    void* best = nullptr;
    i32 best_score = 0;
    for (void* p = closed; p != nullptr; p = jo::as_pfd_zone(p)->open_next) {
        const i32 s = static_cast<i32>(jo::as_pfd_zone(p)->cost_h + static_cast<u32>(jo::view<i32>(jo::as_pfd_zone(p)->cost_g) / 16));
        if (best == nullptr || s < best_score) {
            best = p;
            best_score = s;
        }
    }
    for (void* p = open; p != nullptr; p = jo::as_pfd_zone(p)->open_next) {
        const i32 s = static_cast<i32>(jo::as_pfd_zone(p)->cost_h + static_cast<u32>(jo::view<i32>(jo::as_pfd_zone(p)->cost_g) / 16));
        if (best == nullptr || s < best_score) {
            best = p;
            best_score = s;
        }
    }
    if (best == nullptr) return 0;
    joan::orig::as_path_box(out)->set_end(joan::orig::arg::i32(jo::as_pfd_zone(best)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(best)->centre_y), static_cast<i32>(jo::as_pfd_zone(best)->centre_z));  // 0x0066e720
    i32 v70 = 0;
    i32 v74 = 1;
    for (i32 i = 0; i < jo::as_pfd_zone(best)->point_count; ++i) {
        void* const n88 = jo::as_pfd_zone(best)->points[i].item;
        const i32 j = (i == jo::as_pfd_zone(best)->point_count - 1) ? 0 : i + 1;
        void* const n8c = jo::as_pfd_zone(best)->points[j].item;
        i32 v78 = 0, v7c = 0, v80 = 0;
        P2 tc, td;
        const P2 b = *point_of(n8c, &tc);
        const P2 a = *point_of(n88, &td);
        if (joan::orig::arg::as<i32>(joan::orig::cross3(joan::orig::arg::i32(jo::as_pfd_zone(best)->centre_x), joan::orig::arg::i32(jo::as_pfd_zone(best)->centre_y), joan::orig::arg::i32(goal.x), joan::orig::arg::i32(goal.y), joan::orig::arg::i32(a.x), joan::orig::arg::i32(a.y), joan::orig::arg::i32(b.x), joan::orig::arg::i32(b.y), &v78, &v7c, &v80)) == 0)
            continue;
        if (v7c < 0) continue;
        if (v7c > v80) continue;
        if (joan::orig::arg::as<i32>(joan::orig::rational_compare(joan::orig::arg::i32(v78), joan::orig::arg::i32(v80), joan::orig::arg::i32(v70), joan::orig::arg::i32(v74))) <= 0) continue;
        P2 te, tf, tg, th, ti;
        const P2 a2 = *point_of(n88, &te);
        const P2 b2 = *point_of(n8c, &tf);
        const P2* const dp = joan::orig::arg::as<P2*>(joan::orig::p2::sub(joan::orig::arg::mptr(&tg), b2, a2));  // 0x006639e0
        const i32 d94 = dp->x;
        const i32 d90 = dp->y;
        const i32 rz = joan::orig::arg::as<i32>(joan::orig::muldiv_signed(static_cast<i32>(jo::view<u32>(jo::as_pfd_point(n8c)->xyz[2]) - jo::view<u32>(jo::as_pfd_point(n88)->xyz[2])), joan::orig::arg::i32(v7c), joan::orig::arg::i32(v80)));  // 0x005921a0
        const i32 z = static_cast<i32>(jo::view<u32>(jo::as_pfd_point(n88)->xyz[2]) + static_cast<u32>(rz));
        const P2* const py = point_of(n88, &th);
        const i32 ry = joan::orig::arg::as<i32>(joan::orig::muldiv_signed(joan::orig::arg::i32(d90), joan::orig::arg::i32(v7c), joan::orig::arg::i32(v80)));
        const i32 y = static_cast<i32>(static_cast<u32>(py->y) + static_cast<u32>(ry));
        const P2* const px = point_of(n88, &ti);
        const i32 rx = joan::orig::arg::as<i32>(joan::orig::muldiv_signed(joan::orig::arg::i32(d94), joan::orig::arg::i32(v7c), joan::orig::arg::i32(v80)));
        const i32 x = static_cast<i32>(static_cast<u32>(px->x) + static_cast<u32>(rx));
        joan::orig::as_path_box(out)->set_end(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(z));  // 0x0066e720
        v70 = v78;
        v74 = v80;
    }
    for (void* p = best; jo::as_pfd_zone(p)->came_from != nullptr; p = jo::as_pfd_zone(p)->came_from) {
        for (i32 i = 0; i < jo::as_pfd_zone(p)->point_count; ++i) {
            if (jo::as_pfd_zone(p)->links[i].item != jo::as_pfd_zone(p)->came_from) continue;
            void* const a0 = jo::as_pfd_zone(p)->points[i].item;
            const i32 j = (i == jo::as_pfd_zone(p)->point_count - 1) ? 0 : i + 1;
            void* const a4 = jo::as_pfd_zone(p)->points[j].item;
            joan::orig::as_path(out)->add_segment(joan::orig::arg::i32(jo::as_pfd_point(a0)->xyz[0]), joan::orig::arg::i32(jo::as_pfd_point(a0)->xyz[1]), joan::orig::arg::i32(jo::as_pfd_point(a0)->xyz[2]), joan::orig::arg::i32(jo::as_pfd_point(a4)->xyz[0]), joan::orig::arg::i32(jo::as_pfd_point(a4)->xyz[1]), joan::orig::arg::i32(jo::as_pfd_point(a4)->xyz[2]));  // 0x0066e790
            break;
        }
    }
    return 0;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_18", name, addr, fn)
B("find_path2", 0x0066ce30, &astar);
#undef B

}  // namespace game_b109_18
}  // namespace

// ---------------------------------------- from modules/game_b96_07.cpp (game/b96_07)
namespace {
namespace game_b96_07 {

using i32 = std::int32_t;

using joan::call::thiscall;


using u32 = std::uint32_t;

JOAN_COUNTER(c_scale, "PointList::scale");

struct H {
    // ------------------------------------------------------------------ 0x0066dd00
    u32 is_38(i32 v) { return jo::self<jo::PfdZone>(this)->search_stamp == v ? 1u : 0u; }

    // ------------------------------------------------------------------ 0x0066faa0
    void scale(float sxy, float sz) {
        c_scale.hit();
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_list(this)->count_66e8a0());  // 0x0066e8a0
        for (i32 i = 0; i <= n; ++i) {
            float* const p = joan::orig::arg::as<float*>(joan::orig::as_list(this)->from_end(joan::orig::arg::i32(i)));  // 0x0066e8d0
            p[0] = sxy * p[0];
            p[1] = sxy * p[1];
            p[2] = sz * p[2];
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_07", name, addr, fn)
B96("Obj::is_38", 0x0066dd00, &H::is_38);
B96("PointList::scale", 0x0066faa0, &H::scale);
#undef B96

}  // namespace game_b96_07
}  // namespace

// ---------------------------------------- from modules/game_b93_04.cpp (game/b93_04)
namespace {
namespace game_b93_04 {

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::obj;

using i64 = std::int64_t;
using u32 = std::uint32_t;

void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_translate, "Points::translate");
JOAN_COUNTER(c_rational_cmp, "rational_compare");

struct H {
    // ------------------------------------------------------------------ 0x0066e870
    i32 call_5848d0(void* x) {
        joan::orig::as_dynarray(this)->insert_at_5848d0(1, x);  // 0x005848d0
        return 0;  // void callee; both callers (0x004f7bf0, 0x006ab4e0) discard the result
    }

    // ------------------------------------------------------------------ 0x0066fa00
    i32 translate(float x, float y, float z) {
        c_translate.hit();
        const i32 n = joan::orig::arg::as<i32>(joan::orig::as_list(this)->count_66e8a0());  // 0x0066e8a0
        i32 eax = n;
        for (i32 i = 0; i <= n; ++i) {
            float* const p = joan::orig::arg::as<float*>(joan::orig::as_list(this)->from_end(joan::orig::arg::i32(i)));  // 0x0066e8d0
            p[0] = x + p[0];
            p[1] = y + p[1];
            p[2] = z + p[2];
            eax = i + 1;
        }
        return eax;
    }
};

// ---------------------------------------------------------------------------- 0x0066dd30
static i32 JOAN_CDECL script_random_range(i32 a, i32 b, i32 c, i32 d) {
    return joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->points_distance_590410(joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(a), joan::orig::arg::i32(b)));  // 0x00590410
}

// ---------------------------------------------------------------------------- 0x0066fc50
static i32 JOAN_CDECL rational_compare(i32 a, i32 b, i32 c, i32 d) {
    c_rational_cmp.hit();
    const std::uint64_t p1 = static_cast<std::uint64_t>(static_cast<i64>(a) * static_cast<i64>(d));
    const std::uint64_t p2 = static_cast<std::uint64_t>(static_cast<i64>(c) * static_cast<i64>(b));
    // high dwords subtracted (wrapping); only when equal the low dwords decide (unsigned).
    const u32 diff = static_cast<u32>(p1 >> 32) - static_cast<u32>(p2 >> 32);
    if (diff != 0) return static_cast<i32>(diff);
    const u32 lo1 = static_cast<u32>(p1);
    const u32 lo2 = static_cast<u32>(p2);
    if (lo1 > lo2) return 1;
    if (lo1 == lo2) return 0;
    return -1;
}

// ---------------------------------------------------------------------------- 0x0066fca0
static i32 JOAN_CDECL key_compare(i32 a, i32, i32 c, i32 d, i32, i32 f) {
    if (c != f) return static_cast<i32>(static_cast<u32>(c) - static_cast<u32>(f));
    return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(d));
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_04", name, addr, fn)
B93("pfind_f_dist", 0x0066dd30, &script_random_range);
B93("PathResult2::append_path_point", 0x0066e870, &H::call_5848d0);
B93("Points::translate", 0x0066fa00, &H::translate);
B93("compare_rational", 0x0066fc50, &rational_compare);
B93("key_compare", 0x0066fca0, &key_compare);
#undef B93

}  // namespace game_b93_04
}  // namespace

// ---------------------------------------- from modules/game_b97_07.cpp (game/b97_07)
namespace {
namespace game_b97_07 {

using joan::hook::global;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::obj;

using u32 = std::uint32_t;

JOAN_COUNTER(c_cross_terms, "cross_terms");

// ---------------------------------------------------------------------------- 0x0066dd70
static u32 JOAN_CDECL grid_590410(i32 a, i32 b, i32 c, i32 d) {
    return joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->points_distance_590410(joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(a), joan::orig::arg::i32(b)));  // 0x00590410
}

// ---------------------------------------------------------------------------- 0x0066f910
static float JOAN_CDECL cross_terms(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7,
                                 float* p28, float* p2c, float* p30) {
    c_cross_terms.hit();
    *p30 = (a3 - a1) * (a6 - a4) - (a2 - a0) * (a7 - a5);
    *p28 = (a5 - a1) * (a6 - a4) - (a4 - a0) * (a7 - a5);
    *p2c = (a2 - a0) * (a5 - a1) - (a3 - a1) * (a4 - a0);
    if (!(*p30 >= global<float>(d::k_9cf5c4))) {  // < 0 or unordered
        *p30 = -*p30;
        *p28 = -*p28;
        *p2c = -*p2c;
    }
    return *p30;
}

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_07", name, addr, fn)
B97("grid_590410", 0x0066dd70, &grid_590410);
B97("cross_terms", 0x0066f910, &cross_terms);
#undef B97

}  // namespace game_b97_07
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;

using u32 = std::uint32_t;


JOAN_COUNTER(c_tree_acc, "path_tree_accumulate");

// ------------------------------------------------------------------ 0x0066ddb0 (cdecl)
void JOAN_CDECL path_tree_accumulate(void* node, i32 a, i32 b) {
    c_tree_acc.hit();
    for (i32 i = 0; i < static_cast<i32>(jo::as_pfd_zone(node)->child_count); ++i) {
        void* const child = jo::as_pfd_zone(node)->children[i].item;
        const i32 d = joan::orig::arg::as<i32>(joan::orig::script_random_range(joan::orig::arg::i32(jo::view<u32>(jo::as_pfd_zone(child)->centre_x)), joan::orig::arg::i32(jo::view<u32>(jo::as_pfd_zone(child)->centre_y)), joan::orig::arg::i32(jo::view<u32>(jo::as_pfd_zone(node)->centre_x)), joan::orig::arg::i32(jo::view<u32>(jo::as_pfd_zone(node)->centre_y))));
        jo::as_pfd_zone(child)->cost_g = jo::as_pfd_zone(node)->cost_g + static_cast<u32>(d);
        const u32 base = jo::as_pfd_zone(child)->cost_g + jo::as_pfd_zone(child)->cost_h;
        u32 dist = static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::libc::abs(static_cast<i32>(jo::view<u32>(jo::as_pfd_zone(child)->bbox_max[0]) - jo::view<u32>(jo::as_pfd_zone(child)->bbox_min[0])))));
        dist += static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::libc::abs(static_cast<i32>(jo::view<u32>(jo::as_pfd_zone(child)->bbox_max[1]) - jo::view<u32>(jo::as_pfd_zone(child)->bbox_min[1])))));
        const u32 k = (jo::as_pfd_zone(child)->id % 3 != a) ? 4u : 6u;
        dist = dist * k * static_cast<u32>(b);
        jo::view<u32>(jo::as_pfd_zone(child)->cost_f) = base - dist;
        jo::as_pfd_zone(child)->flags_sum = jo::as_pfd_zone(node)->flags_sum + static_cast<u32>(static_cast<i32>(jo::view<i16>(jo::as_pfd_zone(child)->flags)));
        jo::as_pfd_zone(child)->max_flags = (static_cast<i32>(jo::as_pfd_zone(node)->max_flags) > static_cast<i32>(jo::view<i16>(jo::as_pfd_zone(child)->flags))) ? jo::as_pfd_zone(node)->max_flags : jo::view<i16>(jo::as_pfd_zone(child)->flags);
        joan::orig::path_tree_accumulate(joan::orig::arg::mptr(child), joan::orig::arg::i32(a), joan::orig::arg::i32(b));
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("propagate_update", 0x0066ddb0, &path_tree_accumulate);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b100_14.cpp (game/b100_14)
namespace {
namespace game_b100_14 {

using u8 = std::uint8_t;
using i32 = std::int32_t;


using u32 = std::uint32_t;

JOAN_COUNTER(c_cross3, "cross3");

struct H {
    // ------------------------------------------------------------------ 0x0066fc20
    i32 pair_set(u8 b, i32 d) {  // void in the original; eax is left = d
        jo::self<jo::Key>(this)->kind = b;
        jo::view<i32>(jo::self<jo::Key>(this)->code) = d;
        return d;
    }
};

// ---------------------------------------------------------------------- 0x0066df30
static i32 JOAN_CDECL cross3(i32 a, i32 b, i32 c, i32 d, i32 e, i32 g, i32 h, i32 k, i32* p, i32* q, i32* r) {
    c_cross3.hit();
    // args: a=+8 b=+c c=+10 d=+14 e=+18 g=+1c h=+20 k=+24 p=+28 q=+2c r=+30
    const u32 ua = static_cast<u32>(a), ub = static_cast<u32>(b), uc = static_cast<u32>(c), ud = static_cast<u32>(d);
    const u32 ue = static_cast<u32>(e), ug = static_cast<u32>(g), uh = static_cast<u32>(h), uk = static_cast<u32>(k);
    *r = static_cast<i32>((ud - ub) * (uh - ue) - (uc - ua) * (uk - ug));
    *p = static_cast<i32>((ug - ub) * (uh - ue) - (ue - ua) * (uk - ug));
    *q = static_cast<i32>((uc - ua) * (ug - ub) - (ud - ub) * (ue - ua));
    if (*r < 0) {
        *r = static_cast<i32>(0u - static_cast<u32>(*r));
        *p = static_cast<i32>(0u - static_cast<u32>(*p));
        *q = static_cast<i32>(0u - static_cast<u32>(*q));
    }
    return *r;
}

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_14", name, addr, fn)
B100("cross3", 0x0066df30, &cross3);
B100("Pair::set", 0x0066fc20, &H::pair_set);
#undef B100

}  // namespace game_b100_14
}  // namespace

// ---------------------------------------- from modules/game_b101_13.cpp (game/b101_13)
namespace {
namespace game_b101_13 {

using i32 = std::int32_t;

using joan::call::thiscall;

using u32 = std::uint32_t;


JOAN_COUNTER(c_find_point, "Regions::find_point");

struct H {
    // ------------------------------------------------------------------ 0x0066e010
    u32 find_point(u32 r, u32 x, u32 y, i32* out, u32 flags) {
        c_find_point.hit();
        struct L {
            jo::BlkArr* jo::PathSpace::*arr;
            u32 bit;
            std::uintptr_t test;
        };
        const L lists[4] = {{&jo::PathSpace::points, 1, fn::code_value(fn::Point_within)},    // 0x0066a530
                            {&jo::PathSpace::pairs4, 2, fn::code_value(fn::PathLine_detect_select)},    // 0x0066e2a0
                            {&jo::PathSpace::pairs8, 4, fn::code_value(fn::PathLine_detect_select)},    // 0x0066e2a0
                            {&jo::PathSpace::links, 8, fn::code_value(fn::PathLine_detect_select)}};   // 0x0066e2a0
        for (int k = 0; k < 4; ++k) {
            if (jo::view<void*>(jo::self<jo::PathSpace>(this)->*lists[k].arr) == nullptr || (flags & lists[k].bit) == 0) continue;
            for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_path_space(jo::view<void*>(jo::self<jo::PathSpace>(this)->*lists[k].arr))->get_2c()); ++i) {  // 0x0066b7a0
                u32 pt[2];
                const u32* const p = joan::orig::arg::as<const u32*>(joan::orig::as_point(pt)->set(joan::orig::arg::u32(x), joan::orig::arg::u32(y)));  // 0x004eeb80
                const u32 px = p[0], py = p[1];
                void* const o = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::PathSpace>(this)->*lists[k].arr))->item(joan::orig::arg::i32(i)));  // 0x00637da0
                if (thiscall<i32>(lists[k].test, o, px, py, r) != 0) {
                    *out = i;
                    return lists[k].bit;
                }
            }
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_13", name, addr, fn)
B("PathSpace::test_detect", 0x0066e010, &H::find_point);
#undef B

}  // namespace game_b101_13
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
namespace {
namespace game_b99_09 {

using joan::hook::x87::Fpu;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;


using u32 = std::uint32_t;


struct V2 {
    i32 x, y;
};
static_assert(sizeof(V2) == 8, "V2");

JOAN_COUNTER(c_near_point, "Segment::near_point");

V2* v_sub(V2* out, V2 a, V2 b) { return joan::orig::arg::as<V2*>(joan::orig::p2::sub(joan::orig::arg::mptr(out), a, b)); }  // 0x006639e0
V2* v_add(V2* out, V2 a, V2 b) { return joan::orig::arg::as<V2*>(joan::orig::point::add(joan::orig::arg::mptr(out), a.x, a.y, b.x, b.y)); }  // 0x00663990
V2* v_scale(V2* out, V2 a, float f) { return joan::orig::arg::as<V2*>(joan::orig::point::scale(joan::orig::arg::mptr(out), a.x, a.y, f)); }  // 0x00663a30
i32 v_len2(const V2* v) { return joan::orig::arg::as<i32>(joan::orig::as_vec2i(v)->length_sq()); }                  // 0x0066a440
i32 v_dot(const V2* v, V2 w) { return joan::orig::arg::as<i32>(joan::orig::as_vec2i(v)->dot(w.x, w.y)); }  // 0x0066a3c0 (w by value = 2 dwords)
V2 v_get(const void* p) {
    V2 t;
    return *joan::orig::arg::as<V2*>(joan::orig::as_vec2i(p)->copy_to(&t));  // 0x0066a4f0
}

struct H {
    // 0x0066e2a0: is point (x, y) within r of the segment this[0] -> this[4]?
    i32 near_point(i32 x, i32 y, i32 r) {
        c_near_point.hit();
        const V2 A = v_get(jo::self<jo::Segment>(this)->a);
        const V2 B = v_get(jo::self<jo::Segment>(this)->b);
        const V2 P{x, y};
        V2 t0, t1, t2;
        const i32 L = v_len2(v_sub(&t0, B, A));
        const V2 ba = *v_sub(&t1, B, A);
        const i32 d = v_dot(v_sub(&t2, P, A), ba);
        const i32 rr = static_cast<i32>(static_cast<u32>(r) * static_cast<u32>(r));
        if (L < 1) {
            V2 t3;
            return v_len2(v_sub(&t3, P, B)) > rr ? 0 : 1;
        }
        if (d < 0) return 0;
        if (d > L) return 0;
        const Fpu fpu{true};
        const float q = static_cast<float>(fpu.div(static_cast<double>(d), static_cast<double>(L)));
        V2 t4, t5, t6, t7;
        const V2 v = *v_sub(&t4, B, A);
        const V2 s = *v_scale(&t5, v, q);
        const V2 u = *v_add(&t6, s, A);
        return v_len2(v_sub(&t7, u, P)) > rr ? 0 : 1;
    }
};

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("PathLine::detect_select", 0x0066e2a0, &H::near_point);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

struct H {
    void* self() { return static_cast<void*>(this); }

    void* init_66e4b0() {  // 0x0066e4b0
        joan::orig::dynarray(self())->construct(static_cast<std::int32_t>(sizeof(jo::PathPoint)), 0x10, 0);  // 0x005843d0
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Obj::init_66e4b0", 0x0066e4b0, &H::init_66e4b0);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b90_06_2.cpp (game/b90_06)
namespace {
namespace game_b90_06_2 {

using joan::call::thiscall;


// 36-byte box record: float position, int position, int position (copied 9 dwords).
struct Box9 {
    float fx, fy, fz;
    std::int32_t a[3];
    std::int32_t b[3];
};
static_assert(sizeof(Box9) == 36, "Box9");

JOAN_COUNTER(c_pathbox, "PathBox::set");

struct H {
    // 0x0066e640 PathBox::set(start, end).
    std::uint32_t pathbox_set(std::int32_t x0, std::int32_t y0, std::int32_t z0, std::int32_t x1, std::int32_t y1,
                              std::int32_t z1) {
        c_pathbox.hit();
        joan::orig::dynarray(this)->zap(1);  // 0x00584580
        Box9 a;
        a.fx = static_cast<float>(x0);
        a.fy = static_cast<float>(y0);
        a.fz = static_cast<float>(z0);
        a.a[0] = x0;
        a.a[1] = y0;
        a.a[2] = z0;
        a.b[0] = x0;
        a.b[1] = y0;
        a.b[2] = z0;
        std::memcpy(&jo::self<jo::PointList>(this)->start, &a, sizeof(a));
        Box9 b;
        b.fx = static_cast<float>(x1);
        b.fy = static_cast<float>(y1);
        b.fz = static_cast<float>(z1);
        b.a[0] = x1;
        b.a[1] = y1;
        b.a[2] = z1;
        b.b[0] = x1;
        b.b[1] = y1;
        b.b[2] = z1;
        joan::orig::as_dynarray(this)->linkin_584620(&b);  // 0x00584620
        return 0;  // void callee; all 6 callers of PathBox::set discard the result
    }
    // 0x0066e720 PathBox::set_end(x, y, z).
    std::uint32_t pathbox_set_end(std::int32_t x, std::int32_t y, std::int32_t z) {
        Box9 b;
        b.fx = static_cast<float>(x);
        b.fy = static_cast<float>(y);
        b.fz = static_cast<float>(z);
        b.a[0] = x;
        b.a[1] = y;
        b.a[2] = z;
        b.b[0] = x;
        b.b[1] = y;
        b.b[2] = z;
        joan::orig::as_dynarray(this)->update_584b30(&b, 1);  // 0x00584b30
        return 0;  // void callee; all 5 callers of PathBox::set_end discard the result
    }
    // 0x0066e8a0
    std::int32_t list_count() { return joan::orig::as_dynarray(this)->size(); }  // 0x00417ad0
    // 0x0066e8d0: 0 -> this + 0x1d, n in 1..count -> n-th element from the end, else 0.
    void* list_from_end(std::int32_t n) {
        if (n == 0) return &jo::self<jo::PointList>(this)->start;
        if (n > 0 && n <= joan::orig::as_dynarray(this)->size()) {  // 0x00417ad0
            const std::int32_t c = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
            return joan::orig::arg::as<void*>(joan::orig::as_dynarray(this)->get(static_cast<std::int32_t>(static_cast<std::uint32_t>(c) + 1u -
                                                                                 static_cast<std::uint32_t>(n))));  // 0x00417200
        }
        return nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("PathResult2::set_start", 0x0066e640, &H::pathbox_set);
B("PathResult2::set_actual_dest", 0x0066e720, &H::pathbox_set_end);
B("List::count", 0x0066e8a0, &H::list_count);
B("List::from_end", 0x0066e8d0, &H::list_from_end);
#undef B

}  // namespace game_b90_06_2
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

using joan::hook::global;
using joan::hook::x87::Fpu;

using joan::call::thiscall;

struct H {
    void* self() { return this; }

    // 0x0066e790 - element of 0x24 bytes (array created with size 0x24 at 0x0066e4c0)
    std::uint32_t line_add(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t b0, std::int32_t b1,
                           std::int32_t b2) {
        struct alignas(4) Line {
            float mid[3];
            std::int32_t a[3];
            std::int32_t b[3];
        } line;
        static_assert(sizeof(Line) == 0x24);
        const joan::hook::x87::Fpu fpu{true};
        const double two = global<float>(d::flt_2);
        const std::int32_t s0 = static_cast<std::int32_t>(static_cast<std::uint32_t>(a0) + static_cast<std::uint32_t>(b0));
        const std::int32_t s1 = static_cast<std::int32_t>(static_cast<std::uint32_t>(a1) + static_cast<std::uint32_t>(b1));
        const std::int32_t s2 = static_cast<std::int32_t>(static_cast<std::uint32_t>(a2) + static_cast<std::uint32_t>(b2));
        line.mid[0] = static_cast<float>(fpu.div(static_cast<double>(s0), two));
        line.mid[1] = static_cast<float>(fpu.div(static_cast<double>(s1), two));
        line.mid[2] = static_cast<float>(fpu.div(static_cast<double>(s2), two));
        line.a[0] = a0;
        line.a[1] = a1;
        line.a[2] = a2;
        line.b[0] = b0;
        line.b[1] = b1;
        line.b[2] = b2;
        joan::orig::as_dynarray(self())->linkin_584620(static_cast<void*>(&line));  // 0x00584620
        return 0;  // void callee; all callers of LineList::add discard the result
    }
    // 0x0066e940
    void* clear_66e940() { return joan::orig::as_dynarray(self())->get(1); }  // 0x00417200
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("LineList::add", 0x0066e790, &H::line_add);
B91("Obj66e::clear", 0x0066e940, &H::clear_66e940);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b95_07.cpp (game/b95_07)
namespace {
namespace game_b95_07 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0066e840
    u32 push(void* p) {
        joan::orig::as_dynarray(this)->linkin_584620(p);  // 0x00584620
        return 0;  // void callee; the one caller (unit_core.cpp push()) discards the result
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_07", name, addr, fn)
B95("PointList::push", 0x0066e840, &H::push);
#undef B95

}  // namespace game_b95_07
}  // namespace

// ---------------------------------------- from modules/game_b101_14.cpp (game/b101_14)
namespace {
namespace game_b101_14_2 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;


using joan::orig::helpers::crt_memcpy;

struct Key8 {
    u32 d;
    u8 b;
    u8 pad[3];
};
static_assert(sizeof(Key8) == 8, "Key8");

JOAN_COUNTER(c_parse, "Mesh::parse_block");
JOAN_COUNTER(c_add_rect, "SpIndex::add_rect");

// ------------------------------------------------------------------ 0x0066fb40
i32 JOAN_CDECL add_rect(u32 key, i32 x0, i32 y0, i32 x1, i32 y1, void* index) {
    c_add_rect.hit();
    Key8 k{};
    joan::orig::as_key(&k)->set_66fc20(static_cast<std::uint8_t>(joan::orig::arg::u32(0)), joan::orig::arg::i32(key));  // 0x0066fc20 (byte +4 = 0, dword = key)
    const i32 min_x = x0 < x1 ? x0 : x1;
    const i32 min_y = y0 < y1 ? y0 : y1;
    const i32 max_x = x0 > x1 ? x0 : x1;
    const i32 max_y = y0 > y1 ? y0 : y1;
    return joan::orig::arg::as<i32>(joan::orig::as_sp_index(index)->insert(&k, joan::orig::arg::u32(min_x), joan::orig::arg::u32(min_y), joan::orig::arg::u32(max_x), joan::orig::arg::u32(max_y)));  // 0x006821d0
}

struct H {
    // ------------------------------------------------------------------ 0x0066e970
    i32 parse_block(const u8* data) {
        c_parse.hit();
        const u8* p = data;
        const i16 count = *reinterpret_cast<const i16*>(p);
        p += 2;
        const i16 stride = *reinterpret_cast<const i16*>(p);
        p += 2;
        if (static_cast<i32>(count) == 0) {
            const float* const q = reinterpret_cast<const float*>(p);
            p += static_cast<i32>(stride);
            const i32 z = ftol(q[2]), y = ftol(q[1]), x = ftol(q[0]);
            const i32 z2 = ftol(q[2]), y2 = ftol(q[1]), x2 = ftol(q[0]);
            joan::orig::as_path_box(this)->set(joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::i32(z2), joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(z));  // 0x0066e640
            joan::orig::as_path(this)->clear();                        // 0x004eeca0
            return static_cast<i32>(p - data);
        }
        const float* const a = reinterpret_cast<const float*>(p);
        p += static_cast<i32>(stride);
        const float* const b = reinterpret_cast<const float*>(p);
        p += static_cast<i32>(stride);
        const i32 bz = ftol(b[2]), by = ftol(b[1]), bx = ftol(b[0]);
        const i32 az = ftol(a[2]), ay = ftol(a[1]), ax = ftol(a[0]);
        joan::orig::as_path_box(this)->set(joan::orig::arg::i32(ax), joan::orig::arg::i32(ay), joan::orig::arg::i32(az), joan::orig::arg::i32(bx), joan::orig::arg::i32(by), joan::orig::arg::i32(bz));  // 0x0066e640
        for (i32 i = 1; i < static_cast<i32>(count); ++i) {
            const jo::PathPoint* const r = jo::as_path_point(p);
            p += static_cast<i32>(stride);
            joan::orig::as_path(this)->add_segment(joan::orig::arg::i32(jo::view<u32>(r->a[0])), joan::orig::arg::i32(jo::view<u32>(r->a[1])), joan::orig::arg::i32(jo::view<u32>(r->a[2])), joan::orig::arg::i32(jo::view<u32>(r->b[0])), joan::orig::arg::i32(jo::view<u32>(r->b[1])), joan::orig::arg::i32(jo::view<u32>(r->b[2])));  // 0x0066e790
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->from_end(1));                   // 0x0066e8d0
            crt_memcpy(e, r, 0x24);
        }
        return static_cast<i32>(p - data);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_14", name, addr, fn)
B("Mesh::parse_block", 0x0066e970, &H::parse_block);
B("add_line_to_spatial_index", 0x0066fb40, &add_rect);
#undef B

}  // namespace game_b101_14_2
}  // namespace

// ---------------------------------------- from modules/game_b106_15.cpp (game/b106_15)
namespace {
namespace game_b106_15 {

using joan::hook::x87::Fpu;
using u8 = std::uint8_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::img::obj;

using u32 = std::uint32_t;

bool equal_or_nan(float a, float b) { return !(a < b || a > b); }
using joan::orig::helpers::as_float;

JOAN_COUNTER(c_smooth, "Path::smooth");

// ---- shared helpers -----------------------------------------------------------------------
void* const kRandom = obj(d::g_misc);
float game_sqrt(float x) { return joan::orig::arg::as<float>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(x)))); }
double game_fabs(float x) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(static_cast<double>(x)))); }

// 0x00590b00 clamp(x, lo, hi) on 0x00a45fa8 (thiscall, doubles).
double clamp(double x, double lo, double hi) { return joan::orig::arg::as<double>(joan::orig::as_misc(kRandom)->limit_double(joan::orig::arg::f64(x), joan::orig::arg::f64(lo), joan::orig::arg::f64(hi))); }

// 0x0066f820 / 0x0066f910 (direct calls in the original).
i32 shrink_segment(float* ax, float* ay, float* bx, float* by, float r) { return joan::orig::arg::as<i32>(joan::orig::separate_circles(joan::orig::arg::mptr(ax), joan::orig::arg::mptr(ay), joan::orig::arg::mptr(bx), joan::orig::arg::mptr(by), joan::orig::arg::f32(r))); }
float cross_terms(float a, float b, float c, float d, float e, float f, float g, float h, float* o0, float* o1, float* o2) {
    return joan::orig::arg::as<float>(joan::orig::cross_terms(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), joan::orig::arg::f32(e), joan::orig::arg::f32(f), joan::orig::arg::f32(g), joan::orig::arg::f32(h), joan::orig::arg::mptr(o0), joan::orig::arg::mptr(o1), joan::orig::arg::mptr(o2)));
}

struct H {
    // ---- 0x0066ece0 -------------------------------------------------------------------
    i32 points() { return joan::orig::arg::as<i32>(joan::orig::as_list(this)->count_66e8a0()); }                // 0x0066e8a0
    jo::PathPoint* point(i32 i) { return jo::as_path_point(joan::orig::as_list(this)->from_end(joan::orig::arg::i32(i))); }         // 0x0066e8d0

    void smooth(float r, float a, float b, i32) {
        c_smooth.hit();
        const Fpu fpu{true};
        a = a * 2.5f;
        b = b * 2.5f;
        for (i32 i = 1; i < points(); ++i) {
            u32 prev[9];
            std::memcpy(prev, point(i - 1), 36);
            jo::PathPoint* const curp = point(i);
            u32 cur[9];
            std::memcpy(cur, curp, 36);
            const float px = as_float(prev[0]), py = as_float(prev[1]);
            auto isub = [&](u32 v, float p) { return static_cast<float>(fpu.sub(static_cast<i32>(v), p)); };
            const float l1x = isub(cur[3], px), l1y = isub(cur[4], py);
            const float l2x = isub(cur[6], px), l2y = isub(cur[7], py);
            float ax = l1x, ay = l1y, bx = l2x, by = l2y;  // [ebp-0x6c], -0x70, -0x74, -0x78
            if (shrink_segment(&ax, &ay, &bx, &by, r) == 0) continue;
            float t = 0.5f;  // [ebp-0x7c]
            const i32 end = points() + 1;
            for (i32 j = i + 1; j <= i + 8 && j < end; ++j) {
                u32 e[9];
                std::memcpy(e, point(j), 36);
                float ex = isub(e[3], px), ey = isub(e[4], py);
                float fx = isub(e[6], px), fy = isub(e[7], py);
                if (shrink_segment(&ex, &ey, &fx, &fy, r) == 0) {
                    ex = (ex + fx) / 2.0f;
                    ey = (ey + fy) / 2.0f;
                    fx = ex;
                    fy = ey;
                }
                if (!(fx * ay - fy * ax >= 0.0f)) {
                    t = 0.0f;
                    break;
                }
                if (ex * by - ey * bx > 0.0f) {
                    t = 1.0f;
                    break;
                }
                if (ex * ay - ey * ax > 0.0f) {
                    if (j != points()) {
                        const float d = game_sqrt((ex - bx) * (ex - bx) + (ey - by) * (ey - by));
                        if (!equal_or_nan(b, 0.0f) && b / 2.0f > d) break;
                    }
                    ax = ex;
                    ay = ey;
                } else {
                    float o0, o1, o2;
                    if (!equal_or_nan(cross_terms(0.0f, 0.0f, ax, ay, ex, ey, fx, fy, &o0, &o1, &o2), 0.0f)) {
                        ax = (fx - ex) * o1 / o2 + ex;
                        ay = (fy - ey) * o1 / o2 + ey;
                    }
                }
                if (!(fx * by - fy * bx >= 0.0f)) {
                    if (j != points()) {
                        const float d = game_sqrt((ax - fx) * (ax - fx) + (ay - fy) * (ay - fy));
                        if (!equal_or_nan(b, 0.0f) && b / 2.0f > d) break;
                    }
                    bx = fx;
                    by = fy;
                } else {
                    float o0, o1, o2;
                    if (!equal_or_nan(cross_terms(0.0f, 0.0f, bx, by, ex, ey, fx, fy, &o0, &o1, &o2), 0.0f)) {
                        bx = (fx - ex) * o1 / o2 + ex;
                        by = (fy - ey) * o1 / o2 + ey;
                    }
                }
            }
            const float d = game_sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));  // [ebp-0xdc]
            if (!equal_or_nan(b, 0.0f) && !equal_or_nan(d, 0.0f)) {
                if (!(d >= b)) {
                    t = static_cast<float>(clamp(static_cast<double>(-a / b + 0.5f), 0.0, 1.0));
                } else if (equal_or_nan(t, 0.0f)) {
                    const float c = static_cast<float>(clamp(static_cast<double>(-a / b + 0.5f), 0.0, 1.0));
                    t = c * b / d;
                } else if (equal_or_nan(t, 0.5f)) {
                    const float c = static_cast<float>(clamp(static_cast<double>(-a / b), -0.5, 0.5));
                    t = c * b / d + 0.5f;
                } else if (equal_or_nan(t, 1.0f)) {
                    const float c = static_cast<float>(clamp(static_cast<double>(-a / b + 0.5f), 0.0, 1.0));
                    t = 1.0f - (1.0f - c) * b / d;
                }
            }
            const float nx = (bx - ax) * t + ax;  // [ebp-0xec]
            const float ny = (by - ay) * t + ay;  // [ebp-0xf0]
            float q0, q1, q2;
            if (!equal_or_nan(cross_terms(0.0f, 0.0f, nx, ny, l1x, l1y, l2x, l2y, &q0, &q1, &q2), 0.0f) && !(q1 >= q2)) {
                for (i32 k = 0; k < 3; ++k) {
                    const i32 lo = static_cast<i32>(cur[3 + k]);
                    const i32 span = static_cast<i32>(cur[6 + k] - cur[3 + k]);
                    curp->pos[k] = static_cast<float>(fpu.add(lo, fpu.div(fpu.mul(span, q1), q2)));
                }
            }
        }
        for (i32 k = points() - 1; k > 0; --k) {
            const jo::PathPoint* const p = point(k - 1);
            const jo::PathPoint* const q = point(k);
            bool drop = false;
            if (!(game_fabs(p->pos[0] - q->pos[0]) >= 0.5) && !(game_fabs(p->pos[1] - q->pos[1]) >= 0.5)) {
                drop = true;
            } else if (k == points() - 1) {
                const jo::PathPoint* const r2 = point(k + 1);
                if (r2 != nullptr && !(game_fabs(r2->pos[0] - q->pos[0]) >= 0.5) &&
                    !(game_fabs(r2->pos[1] - q->pos[1]) >= 0.5)) {
                    drop = true;
                }
            }
            if (drop) joan::orig::dynarray(this)->linkout(joan::orig::arg::i32(points() + 1 - k));  // 0x00584a10
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_15", name, addr, fn)
B("PathResult2::smooth_path", 0x0066ece0, &H::smooth);
#undef B

}  // namespace game_b106_15
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (fix/uninit)
namespace {
namespace game_b115_11 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::str;

using u32 = std::uint32_t;


// File object 0x110 bytes on x86 (0x004179c0 ctor / 0x005872b0 dtor); sizeof(File) = 0x112 on x64.
struct alignas(alignof(void*)) File {
    u8 b[((sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

JOAN_COUNTER(c_saver, "ScenarioSaver::write_66fcd0 (fix/uninit)");

struct H {
    // ======================================================== 0x0066fcd0
    u32 saver_write(const char* file_name) {
        c_saver.hit();
        File file;
        joan::orig::as_file(&file)->construct();              // 0x004179c0
        joan::orig::as_file(&file)->file_create(joan::orig::arg::str(file_name), 1, static_cast<char>(1));  // 0x00586f00
        auto put = [&file](i32 v) { joan::orig::as_file(&file)->file_put_long(joan::orig::arg::i32(v)); };  // 0x00587da0
        auto count = [](const void* list) { return joan::orig::arg::as<i32>(joan::orig::as_path_space(list)->get_2c()); };  // 0x0066b7a0
        auto item = [](const void* list, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_billboard(list)->item(joan::orig::arg::i32(i))); };  // 0x00637da0
        auto in_list = [](void* e, void* group) { return joan::orig::arg::as<i32>(joan::orig::as_zone(e)->link_index(group)); };  // 0x006640d0
        auto cell = [](const void* grid, i32 x, i32 y) {
            return static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_height_grid(grid)->at(joan::orig::arg::i32(x), joan::orig::arg::i32(y))));  // 0x004c59d0 (words)
        };
        const i32 n0 = count(jo::self<jo::PathSpace>(this)->points);
        put(n0);
        if (n0 > 0) {
            for (i32 i = 1; i <= n0; ++i) {
                const jo::PfdPoint* const e = jo::as_pfd_point(item(jo::self<jo::PathSpace>(this)->points, i));
                put(e->xyz[0]);
                put(e->xyz[1]);
                put(e->xyz[2]);
            }
        }
        const i32 n1 = count(jo::self<jo::PathSpace>(this)->zones);
        put(n1);
        i32 kind = 0;  // [ebp-0x13c] (fix/uninit: 0 instead of garbage before the first 0 / 1 entry)
        if (n1 > 0) {
            for (i32 i = 1; i <= n1; ++i) {
                const jo::PfdZone* const e = jo::as_pfd_zone(item(jo::self<jo::PathSpace>(this)->zones, i));
                const i32 k = e->kind;
                if (k == 0) {
                    kind = 0;
                } else if (k == 1) {
                    kind = 1;
                }
                put(kind);
                put(e->bbox_min[0]);
                put(e->bbox_min[1]);
                put(e->bbox_min[2]);
                put(e->bbox_max[0]);
                put(e->bbox_max[1]);
                put(e->bbox_max[2]);
                const i32 m = e->point_count;
                put(m);
                for (i32 j = 0; j < m; ++j) {
                    i32 idx = 1;
                    for (;; ++idx) {
                        if (idx > count(jo::self<jo::PathSpace>(this)->points)) break;
                        u8* const p = item(jo::self<jo::PathSpace>(this)->points, idx);
                        if (p == jo::view<u8**>(e->points)[j]) break;  // the zone's point pointers
                    }
                    put(idx);
                }
            }
        }
        i32 a = 0, bcount = 0;  // [ebp-0x14c] / [ebp-0x150]
        if (n1 > 0) {
            for (i32 i = 1; i <= n1; ++i) {
                jo::PfdZone* const g = jo::as_pfd_zone(item(jo::self<jo::PathSpace>(this)->zones, i));
                for (i32 j = 0; j < g->point_count; ++j) {
                    jo::PfdZone* const member = g->links[j].item;
                    if (member == nullptr) continue;
                    if (in_list(member, g) != 0) {
                        ++a;
                    } else {
                        ++bcount;
                    }
                }
            }
            a = a / 2 + bcount;
        }
        put(a);
        if (a > 0) {
            for (i32 i = 1; i <= n1; ++i) {
                jo::PfdZone* const g = jo::as_pfd_zone(item(jo::self<jo::PathSpace>(this)->zones, i));
                for (i32 j = 1; j <= g->point_count; ++j) {
                    jo::PfdZone* const member = g->links[j - 1].item;
                    if (member == nullptr) continue;
                    const i32 back = in_list(member, g);
                    if (back != 0 && member->id < i) continue;
                    put(i);
                    put(j);
                    put(member->id);
                    put(back);
                }
            }
        }
        const i32 has_grid = jo::self<jo::PathSpace>(this)->grid != nullptr ? 1 : 0;
        put(has_grid);
        if (has_grid != 0) {
            put(0);
            put(0);
            const jo::HeightGrid* const grid = jo::self<jo::PathSpace>(this)->grid;
            put(static_cast<i32>(grid->width) + 1);
            put(static_cast<i32>(grid->height) + 1);
            put(jo::self<jo::PathSpace>(this)->cell_w);
            put(jo::self<jo::PathSpace>(this)->cell_h);
            for (i32 y = 0; y <= static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->height); ++y)
                for (i32 x = 0; x <= static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->width); ++x)
                    put(cell(jo::self<jo::PathSpace>(this)->grid, x, y));
            i32 used = 0;  // [ebp-0x19c]
            if (jo::self<jo::PathSpace>(this)->sub_grids != nullptr) {
                for (i32 y = 0; y < static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->height); ++y)
                    for (i32 x = 0; x < static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->width); ++x)
                        if (jo::self<jo::PathSpace>(this)->sub_grids[y * static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->width) + x].item != nullptr) ++used;
            }
            put(used);
            if (jo::self<jo::PathSpace>(this)->sub_grids != nullptr && used > 0) {
                for (i32 y = 0; y < static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->height); ++y) {
                    for (i32 x = 0; x < static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->width); ++x) {
                        const i32 w = static_cast<i32>(jo::self<jo::PathSpace>(this)->grid->width);
                        if (jo::self<jo::PathSpace>(this)->sub_grids[y * w + x].item == nullptr) continue;
                        const jo::HeightGrid* const sub = jo::self<jo::PathSpace>(this)->sub_grids[y * w + x].item;
                        put(x * jo::self<jo::PathSpace>(this)->cell_w);
                        put(y * jo::self<jo::PathSpace>(this)->cell_h);
                        put(static_cast<i32>(sub->width) + 1);
                        put(static_cast<i32>(sub->height) + 1);
                        put(jo::self<jo::PathSpace>(this)->cell_w / static_cast<i32>(sub->width));
                        put(jo::self<jo::PathSpace>(this)->cell_h / static_cast<i32>(sub->height));
                        for (i32 sy = 0; sy <= static_cast<i32>(sub->height); ++sy)
                            for (i32 sx = 0; sx <= static_cast<i32>(sub->width); ++sx)
                                put(cell(sub, sx, sy));
                    }
                }
            }
        }
        joan::orig::as_file(&file)->destruct();  // 0x005872b0
        return 1;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: entry kind [ebp-0x13c] starts at 0 (dead code).
F("PathSpace::save_path_polygon", 0x0066fcd0, &H::saver_write);
#undef F

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b83_04_3.cpp (game/b83_04)
namespace {
namespace game_b83_04_3 {

using joan::call::thiscall;

struct H {
    // 0x00674f50
    void dtor_674f50() {
        joan::orig::as_zone(this)->free_points();              // 0x00663f70
        joan::orig::as_path_node(&jo::self<jo::PfdZone>(this)->search_stamp)->destruct();     // 0x00663bd0 on the +0x38 search node
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("PathSearchNode::~PathSearchNode", 0x00674f50, &H::dtor_674f50);
#undef B83_04

}  // namespace game_b83_04_3
}  // namespace

// ---------------------------------------- from modules/game_b100_14_2.cpp (game/b100_14)
namespace {
namespace game_b100_14_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_remove_if, "array_remove_if");

// ---------------------------------------------------------------------- 0x00674fc0
// key: the comparison's second operand, a pointer (the BVH's partition box, 0x006747a0), a dword on x86 only
using RemoveCb = i32(JOAN_CDECL*)(void*, const void*);
static i32 JOAN_CDECL array_remove_if(u8* base, i32 n, i32 size, const void* key, RemoveCb cb, void* tmp) {
    c_remove_if.hit();
    i32 i = 0;
    while (i < n) {
        u8* const e = base + static_cast<i32>(static_cast<u32>(i) * static_cast<u32>(size));
        if (cb(e, key) < 0) {
            ++i;
            continue;
        }
        u8* const last = base + static_cast<i32>(static_cast<u32>(n - 1) * static_cast<u32>(size));
        if (last != e) {
            std::memcpy(joan::orig::arg::mptr(tmp), static_cast<void*>(e), joan::orig::arg::u32(size));  // memcpy
            std::memcpy(static_cast<void*>(e), static_cast<void*>(last), joan::orig::arg::u32(size));
            std::memcpy(static_cast<void*>(last), joan::orig::arg::ptr(tmp), joan::orig::arg::u32(size));
        }
        --n;
    }
    return n;
}

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_14", name, addr, fn)
B100("array_remove_if", 0x00674fc0, &array_remove_if);
#undef B100

}  // namespace game_b100_14_2
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

using i32 = std::int32_t;
using u32 = std::uint32_t;


// k = axis: the zone's bounding box centre (+0x8 + 4k, +0x14 + 4k) against the split zone's.
i32 mid_cmp(const void* const* a, const void* b, i32 k) {
    const jo::PfdZone* const za = jo::as_pfd_zone(*a);
    const jo::PfdZone* const zb = jo::as_pfd_zone(b);
    const i32 s = static_cast<i32>(static_cast<u32>(za->bbox_min[k]) + static_cast<u32>(za->bbox_max[k]));
    const i32 h = s / 2;  // cdq; sub; sar 1
    u32 r = static_cast<u32>(h) * 2u;
    r -= static_cast<u32>(zb->bbox_min[k]);
    r -= static_cast<u32>(zb->bbox_max[k]);
    return static_cast<i32>(r);
}
i32 JOAN_CDECL cmp_mid_x(const void* const* a, const void* b) { return mid_cmp(a, b, 0); }
i32 JOAN_CDECL cmp_mid_y(const void* const* a, const void* b) { return mid_cmp(a, b, 1); }
i32 JOAN_CDECL cmp_mid_z(const void* const* a, const void* b) { return mid_cmp(a, b, 2); }

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("cmp_mid_x", 0x006750b0, &cmp_mid_x);
B103("cmp_mid_y", 0x00675100, &cmp_mid_y);
B103("cmp_mid_z", 0x00675150, &cmp_mid_z);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b108_12_2.cpp (game/b108_12)
namespace {
namespace game_b108_12_2 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


JOAN_COUNTER(c_6751a0, "QuadNode::query_6751a0");

// ------------------------------------------------------------------ thiscall members
struct H {
    // 0x006751a0
    void query_6751a0(const i32* p, const i32* q, void* out) {
        c_6751a0.hit();
        const i32 x0 = jo::self<jo::BoxTree>(this)->box[0], x1 = jo::self<jo::BoxTree>(this)->box[3];
        const i32 y0 = jo::self<jo::BoxTree>(this)->box[1], y1 = jo::self<jo::BoxTree>(this)->box[4];
        if (!(x0 <= p[0] && p[0] <= x1)) {
            if (p[0] > x0) return;
            if (x0 > q[0]) return;
        }
        if (!(y0 <= p[1] && p[1] <= y1)) {
            if (p[1] > y0) return;
            if (y0 > q[1]) return;
        }
        if (jo::self<jo::BoxTree>(this)->item != nullptr) joan::orig::dynarray(out)->linkin(joan::orig::arg::ptr(&jo::self<jo::BoxTree>(this)->item));  // 0x00584620
        if (jo::self<jo::BoxTree>(this)->left != nullptr) joan::orig::as_bvh_node(jo::self<jo::BoxTree>(this)->left)->query_xy(joan::orig::arg::mptr(p), joan::orig::arg::mptr(q), joan::orig::arg::mptr(out));
        if (jo::self<jo::BoxTree>(this)->right != nullptr) joan::orig::as_bvh_node(jo::self<jo::BoxTree>(this)->right)->query_xy(joan::orig::arg::mptr(p), joan::orig::arg::mptr(q), joan::orig::arg::mptr(out));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("PathResult2::insert_check_point", 0x006751a0, &H::query_6751a0);
#undef B

}  // namespace game_b108_12_2
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


struct H {
    // ------------------------------------------------------------------ 0x006752c0
    void box_query(const i32* p, const i32* q, void* list) {
        for (i32 k = 0; k < 3; ++k) {
            const i32 lo = jo::self<jo::BoxTree>(this)->box[k], hi = jo::self<jo::BoxTree>(this)->box[3 + k];
            if (lo > p[k] || p[k] > hi) {
                if (p[k] > lo) return;
                if (lo > q[k]) return;
            }
        }
        if (jo::self<jo::BoxTree>(this)->item != nullptr) joan::orig::dynarray(list)->linkin(static_cast<void*>(&jo::self<jo::BoxTree>(this)->item));      // 0x00584620
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->left) != nullptr) joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->left))->query(joan::orig::arg::mptr(p), joan::orig::arg::mptr(q), joan::orig::arg::mptr(list));  // 0x006752c0
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->right) != nullptr) joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->right))->query(joan::orig::arg::mptr(p), joan::orig::arg::mptr(q), joan::orig::arg::mptr(list));  // 0x006752c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("BoxTree::query", 0x006752c0, &H::box_query);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::x87::ftol;


JOAN_COUNTER(c_kd, "KdTree::nearest");

struct H {
    // ------------------------------------------------------------------ 0x00675430
    void kd_nearest(const i32* point, i32* best, jo::PfdZone** best_leaf) {
        c_kd.hit();
        const i32 d = joan::orig::arg::as<i32>(joan::orig::point_rect_dist2(joan::orig::arg::i32(point[0]), joan::orig::arg::i32(point[1]), joan::orig::arg::mptr(jo::self<jo::BoxTree>(this)->box), joan::orig::arg::mptr(&jo::self<jo::BoxTree>(this)->box[3])));  // 0x00663ad0
        if (d > *best) return;
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->item) != nullptr) {
            const float y = static_cast<float>(point[1]);
            const float x = static_cast<float>(point[0]);
            const i32 v = ftol(joan::orig::arg::as<float>(joan::orig::as_pfd_zone(jo::view<void*>(jo::self<jo::BoxTree>(this)->item))->min_distance(joan::orig::arg::bits(x), joan::orig::arg::bits(y))));  // 0x006730d0
            if (*best_leaf == nullptr || v < *best) {
                *best = v;
                *best_leaf = static_cast<jo::PfdZone*>(jo::self<jo::BoxTree>(this)->item);
            }
        }
        if (jo::view<void*>(jo::self<jo::BoxTree>(this)->left) != nullptr && jo::view<void*>(jo::self<jo::BoxTree>(this)->right) != nullptr) {
            const i32 d1 = joan::orig::arg::as<i32>(joan::orig::point_rect_dist2(joan::orig::arg::i32(point[0]), joan::orig::arg::i32(point[1]), joan::orig::arg::mptr(jo::self<jo::BoxTree>(this)->left->box), joan::orig::arg::mptr(&jo::self<jo::BoxTree>(this)->left->box[3])));
            const i32 d2 = joan::orig::arg::as<i32>(joan::orig::point_rect_dist2(joan::orig::arg::i32(point[0]), joan::orig::arg::i32(point[1]), joan::orig::arg::mptr(jo::self<jo::BoxTree>(this)->right->box), joan::orig::arg::mptr(&jo::self<jo::BoxTree>(this)->right->box[3])));
            if (d1 <= d2) {
                joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->left))->nearest(joan::orig::arg::mptr(point), joan::orig::arg::mptr(best), joan::orig::arg::mptr(best_leaf));
                joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->right))->nearest(joan::orig::arg::mptr(point), joan::orig::arg::mptr(best), joan::orig::arg::mptr(best_leaf));
            } else {
                joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->right))->nearest(joan::orig::arg::mptr(point), joan::orig::arg::mptr(best), joan::orig::arg::mptr(best_leaf));
                joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->left))->nearest(joan::orig::arg::mptr(point), joan::orig::arg::mptr(best), joan::orig::arg::mptr(best_leaf));
            }
        } else if (jo::view<void*>(jo::self<jo::BoxTree>(this)->left) != nullptr) {
            joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->left))->nearest(joan::orig::arg::mptr(point), joan::orig::arg::mptr(best), joan::orig::arg::mptr(best_leaf));
        } else if (jo::view<void*>(jo::self<jo::BoxTree>(this)->right) != nullptr) {
            joan::orig::as_bvh_node(jo::view<void*>(jo::self<jo::BoxTree>(this)->right))->nearest(joan::orig::arg::mptr(point), joan::orig::arg::mptr(best), joan::orig::arg::mptr(best_leaf));
        }
    }
};

JOAN_REPLACE("game/b108_07", "PathPolygonTree::search_nearest_polygon_xy", 0x00675430, &H::kd_nearest);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07_2 {

using u8 = std::uint8_t;
using i32 = std::int32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_assign, "PointList::assign");

struct H {
    // ------------------------------------------------------------------ 0x0066e5b0
    void* assign(void* src) {
        c_assign.hit();
        joan::orig::as_path(this)->clear();  // 0x004eeca0
        std::memcpy(&jo::self<jo::PointList>(this)->start, &jo::as_point_list(src)->start, sizeof(jo::PathPoint));
        for (i32 i = 1; i <= joan::orig::as_dynarray(src)->size(); ++i) {  // 0x00417ad0
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(src)->get(joan::orig::arg::i32(i)));  // 0x00417200
            joan::orig::dynarray(this)->linkin(joan::orig::arg::ptr(e));    // 0x00584620
        }
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("PointList::assign", 0x0066e5b0, &H::assign);
#undef B94

}  // namespace game_b94_07_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Zone::set_points(void* a0, std::int32_t a1, std::uint16_t a2) -> void { joan::bind::member<&::game_b82_14::H::set_points>(this, a0, a1, a2); }  // 0x663cb0
auto joan::orig::Zone::link(std::int32_t a0, void* a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_14::H::link>(this, a0, a1, a2)); }  // 0x664130
auto joan::orig::PathSpace::get_2c() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_04_2::H::get_2c>(this)); }  // 0x66b7a0
auto joan::orig::PathSpace::link_edges(std::int32_t a0) -> void { joan::bind::member<&::game_b98_07::H::link_edges>(this, a0); }  // 0x66b7c0
auto joan::orig::PathSpace::zone_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_14::H::zone_count>(this)); }  // 0x66b9f0
auto joan::orig::PathSpace::find_region(float a0, float a1, float a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_08::H::find_region_66bb40>(this, a0, a1, a2)); }  // 0x66bb40
auto joan::orig::PathSpace::pick(float a0, float a1, float a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_19::H::region_pick>(this, a0, a1, a2, a3)); }  // 0x66c020
auto joan::orig::PathSpace::nearest_zone(std::int32_t a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_07::H::nearest>(this, a0, a1, a2, a3)); }  // 0x66c710
auto joan::orig::Map::query_66c900(std::int32_t a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::query_66c900>(this, a0, a1, a2, a3, a4)); }  // 0x66c900
auto joan::orig::Terrain::height_66cae0(std::int32_t a0, float a1, float a2) -> double { return joan::bind::conv<double>(joan::bind::member<&::game_objects1::H::terrain_height>(this, a0, a1, a2)); }  // 0x66cae0
auto joan::orig::PathSpace::find_path(float a0, float a1, float a2, float a3, float a4, float a5, void* a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::uint32_t a10, std::uint32_t a11, std::uint32_t a12) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_08_2::H::find_path_66cb40>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)); }  // 0x66cb40
auto joan::orig::path::astar_66ce30(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, std::uint32_t a7, std::int32_t a8, void* a9, std::uint32_t a10, std::int32_t a11, std::int32_t a12, std::int32_t a13) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b109_18::astar>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)); }  // 0x66ce30
auto joan::orig::PfdZone::is_stamp(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b96_07::H::is_38>(this, a0)); }  // 0x66dd00
auto joan::orig::script_random_range(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::script_random_range>(a0, a1, a2, a3)); }  // 0x66dd30
auto joan::orig::grid_590410(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b97_07::grid_590410>(a0, a1, a2, a3)); }  // 0x66dd70
auto joan::orig::path_tree_accumulate(void* a0, std::int32_t a1, std::int32_t a2) -> void { joan::bind::free<&::game_b108_13::path_tree_accumulate>(a0, a1, a2); }  // 0x66ddb0
auto joan::orig::cross3(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, void* a8, void* a9, void* a10) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b100_14::cross3>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x66df30
auto joan::orig::Path::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::init_66e4b0>(this)); }  // 0x66e4b0
auto joan::orig::Path::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_07_2::H::assign>(this, a0)); }  // 0x66e5b0
auto joan::orig::PathBox::set(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06_2::H::pathbox_set>(this, a0, a1, a2, a3, a4, a5)); }  // 0x66e640
auto joan::orig::PathBox::set_end(std::int32_t a0, std::int32_t a1, std::int32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_06_2::H::pathbox_set_end>(this, a0, a1, a2)); }  // 0x66e720
auto joan::orig::Path::add_segment(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_04::H::line_add>(this, a0, a1, a2, a3, a4, a5)); }  // 0x66e790
auto joan::orig::Path::push(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_07::H::push>(this, a0)); }  // 0x66e840
auto joan::orig::Path::insert_front(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::call_5848d0>(this, a0)); }  // 0x66e870
auto joan::orig::List::count_66e8a0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_06_2::H::list_count>(this)); }  // 0x66e8a0
auto joan::orig::List::from_end(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_06_2::H::list_from_end>(this, a0)); }  // 0x66e8d0
auto joan::orig::Path::first() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::clear_66e940>(this)); }  // 0x66e940
auto joan::orig::Path::smooth(float a0, float a1, float a2, std::int32_t a3) -> void { joan::bind::member<&::game_b106_15::H::smooth>(this, a0, a1, a2, a3); }  // 0x66ece0
auto joan::orig::separate_circles(void* a0, void* a1, void* a2, void* a3, float a4) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b98_07::separate_circles>(a0, a1, a2, a3, a4)); }  // 0x66f820
auto joan::orig::cross_terms(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, void* a8, void* a9, void* a10) -> float { return joan::bind::conv<float>(joan::bind::free<&::game_b97_07::cross_terms>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)); }  // 0x66f910
auto joan::orig::Path::translate(float a0, float a1, float a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_04::H::translate>(this, a0, a1, a2)); }  // 0x66fa00
auto joan::orig::Path::scale(float a0, float a1) -> void { joan::bind::member<&::game_b96_07::H::scale>(this, a0, a1); }  // 0x66faa0
auto joan::orig::sp_index::add_rect(std::uint32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, void* a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b101_14_2::add_rect>(a0, a1, a2, a3, a4, a5)); }  // 0x66fb40
auto joan::orig::Key::set_66fc20(std::uint8_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_14::H::pair_set>(this, a0, a1)); }  // 0x66fc20
auto joan::orig::rational_compare(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::rational_compare>(a0, a1, a2, a3)); }  // 0x66fc50
auto joan::orig::key_compare(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_04::key_compare>(a0, a1, a2, a3, a4, a5)); }  // 0x66fca0
auto joan::orig::PathSpace::load(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_path1::H::load>(this, a0)); }  // 0x670980
auto joan::orig::PfdZone::point(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_13::H::point>(this, a0)); }  // 0x671890
auto joan::orig::PfdZone::contains(float a0, float a1, float a2, char a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b82_13::H::contains>(this, a0, a1, a2, a3)); }  // 0x6718c0
auto joan::orig::PathSpace::flood(float a0, float a1, std::int32_t a2, float a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b83_04::H::flood>(this, a0, a1, a2, a3)); }  // 0x671cb0
auto joan::orig::Path::check_671fb0(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5, float a6, std::uint32_t a7) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_12::H::check_671fb0>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x671fb0
auto joan::orig::PfdZone::find_crossing(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5, std::int32_t a6, float a7) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b109_20::H::find_crossing>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x672500
auto joan::orig::PfdZone::offset_points(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, void* a8, void* a9) -> void { joan::bind::member<&::game_b92_05::H::offset_points>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }  // 0x672e40
auto joan::orig::PfdZone::min_distance(std::uint32_t a0, std::uint32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b90_06::H::poly_min_distance>(this, a0, a1)); }  // 0x6730d0
auto joan::orig::Path::find_6731d0(float a0, float a1, float a2, std::uint32_t a3, void* a4, void* a5, void* a6, void* a7, std::uint32_t a8) -> void { joan::bind::member<&::game_b108_11::H::path_find>(this, a0, a1, a2, a3, a4, a5, a6, a7, a8); }  // 0x6731d0
auto joan::orig::PathSpace::segment_cut(void* a0, void* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_07::H::segment_cut>(this, a0, a1, a2, a3, a4, a5, a6)); }  // 0x6736e0
auto joan::orig::PathSpace::flush_bvh() -> void { joan::bind::member<&::game_b84_04::H::pathsp_flush>(this); }  // 0x674590
auto joan::orig::PathSpace::build_bvh() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_04::H::build_6745e0>(this)); }  // 0x6745e0
auto joan::orig::PathSpace::build_bvh_range(BoxTree* a0, std::int32_t a1, std::int32_t a2) -> BoxTree* { return joan::bind::conv<BoxTree*>(joan::bind::member<&::game_b109_02::H::bvh_build>(this, a0, a1, a2)); }  // 0x6747a0
auto joan::orig::PfdZone::destruct() -> void { joan::bind::member<&::game_b83_04_3::H::dtor_674f50>(this); }  // 0x674f50
auto joan::orig::BvhNode::query_xy(void* a0, void* a1, void* a2) -> void { joan::bind::member<&::game_b108_12_2::H::query_6751a0>(this, a0, a1, a2); }  // 0x6751a0
auto joan::orig::BvhNode::query(void* a0, void* a1, void* a2) -> void { joan::bind::member<&::game_b108_03::H::box_query>(this, a0, a1, a2); }  // 0x6752c0
auto joan::orig::BvhNode::nearest(void* a0, void* a1, void* a2) -> void { joan::bind::member<&::game_b108_07::H::kd_nearest>(this, a0, a1, a2); }  // 0x675430
auto joan::orig::PathSpace::group_zones() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_14::H::group>(this)); }  // 0x675660
